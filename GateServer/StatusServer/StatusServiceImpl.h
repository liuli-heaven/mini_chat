#pragma once
#include "message.grpc.pb.h"
#include "message.pb.h"

struct ChatServer
{
	ChatServer() : host(""), port(""), name(""), con_count(0){}
	ChatServer(const ChatServer& cs): host(cs.host), port(cs.port), name(cs.name), con_count(cs.con_count){}
	ChatServer& operator=(const ChatServer& cs)
	{
		if (&cs == this)
		{
			return *this;
		}
		host = cs.host;
		port = cs.port;
		name = cs.name;
		con_count = cs.con_count;
		return *this;
	}

	std::string host;
	std::string port;
	std::string name;
	int con_count;
};

class StatusServiceImpl final : public message::StatusService::Service
{
public:
	StatusServiceImpl();
	grpc::Status GetChatServer(grpc::ServerContext* context, const message::GetChatServerReq* request, message::GetChatServerRsp* response) override;
	grpc::Status Login(grpc::ServerContext* context, const message::LoginReq* request, message::LoginRsp* reply) override;
private:
	void insertToken(int uid, std::string token);
	ChatServer getChatServer();
	std::unordered_map<std::string, ChatServer> servers_;
	std::mutex mutex_;
};

