#pragma once
#include <iostream>
#include <queue>

#include "message.grpc.pb.h"

class StatusConPool
{
public:
	StatusConPool(size_t poolSize, const std::string& host, const std::string port);
	StatusConPool(const StatusConPool&) = delete;
	StatusConPool& operator=(const StatusConPool&) = delete;
	~StatusConPool();

	std::unique_ptr<message::StatusService::Stub> getConnection();
	void Close();
	void returnConnection(std::unique_ptr<message::StatusService::Stub>);

private:
	size_t pool_size_;
	std::string host_;
	std::string port_;
	std::queue<std::unique_ptr<message::StatusService::Stub>> connections_;
	std::mutex mutex_;
	std::condition_variable condition_;
	std::atomic<bool> b_stop_;

};

