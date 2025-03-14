#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "const.h"
#include "Singleton.h"
#include <queue>
#include "ConfigMgr.h"


class RPConPool
{
public:
	RPConPool(size_t poolSize, std::string host, std::string port);
	RPConPool(const RPConPool&) = delete;
	RPConPool& operator=(const RPConPool&) = delete;
	~RPConPool();

	void Close();
	std::unique_ptr<message::VarifyService::Stub> getConnection();
	void returnConnection(std::unique_ptr<message::VarifyService::Stub> context);

private:
	std::size_t poolSize_;
	std::string host_;
	std::string port_;
	std::atomic<bool> b_stop_;
	std::queue<std::unique_ptr<message::VarifyService::Stub>> connections_;
	std::mutex mutex_;
	std::condition_variable condition_;
};

class VerifyGrpcClient : public Singleton<VerifyGrpcClient>
{
	friend Singleton<VerifyGrpcClient>;

public:
	message::GetVarifyRsp GetVarifyCode(std::string email);

private:
	VerifyGrpcClient();

	std::unique_ptr<RPConPool> pool_;
};

