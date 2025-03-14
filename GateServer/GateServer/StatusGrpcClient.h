#pragma once
#include "Singleton.h"
#include "ConfigMgr.h"
#include "StatusConPool.h"
#include "const.h"
class StatusGrpcClient : public Singleton<StatusGrpcClient>
{
	friend class Singleton<StatusGrpcClient>;
public:
	~StatusGrpcClient();
	message::GetChatServerRsp GetChatServer(int uid);
	message::LoginRsp Login(int uid, std::string token);
private:
	StatusGrpcClient();
	std::unique_ptr<StatusConPool> pool_;
};

