#include "StatusConPool.h"
#include "grpcpp/grpcpp.h"
#include <iostream>
#include <mutex>
#include "message.grpc.pb.h"

StatusConPool::StatusConPool(size_t poolSize, const std::string& host, const std::string port)
	:pool_size_(poolSize), host_(host), port_(port), b_stop_(false)
{
	for(size_t i = 0; i < pool_size_; i++)
	{
		std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(host + ":" + port,
			grpc::InsecureChannelCredentials());
		connections_.push(message::StatusService::NewStub(channel));
	}
}

StatusConPool::~StatusConPool()
{
	std::unique_lock lock(mutex_);
	Close();
	while (!connections_.empty())
	{
		connections_.pop();
	}
}

std::unique_ptr<message::StatusService::Stub> StatusConPool::getConnection()
{
	std::unique_lock lock(mutex_);
	condition_.wait(lock, [this]()
		{
			if (b_stop_)
			{
				return true;
			}
			return !connections_.empty();
		});

	if(b_stop_)
	{
		return nullptr;
	}

	auto connection = std::move(connections_.front());
	connections_.pop();
	return connection;
}

void StatusConPool::Close()
{
	b_stop_ = true;
	condition_.notify_all();
}

void StatusConPool::returnConnection(std::unique_ptr<message::StatusService::Stub> connection)
{
	std::lock_guard lock(mutex_);
	if(b_stop_)
	{
		return;
	}
	connections_.push(std::move(connection));
	condition_.notify_one();
}
