#pragma once
#include <atomic>
#include <queue>
#include "hiredis.h"
#include <mutex>
#include <condition_variable>

class RedisConPool
{
public:
	RedisConPool(size_t poolSize, const char* host, int port, const char* pwd);
	~RedisConPool();
	redisContext* GetConnection();
	void ReturnConnection(redisContext* context);
	void Close();

private:
	std::atomic<bool> b_stop_;
	size_t poolSize_;
	const char* host_;
	int port_;
	std::queue<redisContext*> connections_;
	std::mutex mutex_;
	std::condition_variable condition_;
};

