#include "RedisConPool.h"
#include <iostream>
RedisConPool::RedisConPool(size_t poolSize, const char* host, int port, const char* pwd)
	:poolSize_(poolSize), host_(host), port_(port), b_stop_(false)
{
	for (size_t i = 0; i < poolSize_; i++)
	{
		auto* context = redisConnect(host, port);
		if (context == NULL || context->err != 0)
		{
			if (context != NULL)
			{
				redisFree(context);
			}
			continue;
		}

		auto reply = (redisReply*)redisCommand(context, "AUTH %s", pwd);
		if(reply->type == REDIS_REPLY_ERROR)
		{
			std::cout << "redis认证失败" << "\n";
			freeReplyObject(reply);
			continue;
		}

		freeReplyObject(reply);
		//std::cout << "redis认证成功\n";
		connections_.push(context);
	}
}

RedisConPool::~RedisConPool()
{
	std::lock_guard lock(mutex_);
	while (!connections_.empty())
	{
		connections_.pop();
	}
}

redisContext* RedisConPool::GetConnection()
{
	std::unique_lock<std::mutex> lock(mutex_);
	condition_.wait(lock, [this]()
		{
			if (b_stop_)
			{
				return true;
			}
			return !connections_.empty();
		});
	//如果停止则返回空指针
	if (b_stop_)
	{
		return nullptr;
	}
	auto* context = connections_.front();
	connections_.pop();
	return context;
}

void RedisConPool::ReturnConnection(redisContext* context)
{
	std::lock_guard<std::mutex> lock(mutex_);
	if (b_stop_)
	{
		return;
	}
	connections_.push(context);
	condition_.notify_one();
}

void RedisConPool::Close()
{
	b_stop_ = true;
	condition_.notify_all();
}
