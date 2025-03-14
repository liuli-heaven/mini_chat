#include "StatusServiceImpl.h"

#include <boost/filesystem/directory.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "ConfigMgr.h"
#include "const.h"
#include "RedisMgr.h"

std::string generate_unique_string() {
	// 创建UUID对象
	
	boost::uuids::uuid uuid = boost::uuids::random_generator()();
	// 将UUID转换为字符串
	std::string unique_string = boost::uuids::to_string(uuid);
	return unique_string;
}

StatusServiceImpl::StatusServiceImpl()
{
	ConfigMgr& config_mgr = ConfigMgr::Inst();
	auto server_list = config_mgr["ChatServers"]["name"];

	std::vector<std::string> words;
	std::stringstream ss(server_list);
	std::string word;

	while(std::getline(ss, word, ','))
	{
		words.push_back(word);
	}
	ChatServer server;
	for(auto& word_ : words)
	{
		if(config_mgr[word]["name"].empty())
		{
			continue;
		}
		server.host = config_mgr[word]["host"];
		server.port = config_mgr[word]["port"];
		server.name = config_mgr[word]["name"];
		servers_[server.name] = server;
	}

}

grpc::Status StatusServiceImpl::GetChatServer(grpc::ServerContext* context, const message::GetChatServerReq* request, message::GetChatServerRsp* response)
{
	std::string prefix("status server has received:");
	const auto& server = getChatServer();
	response->set_host(server.host);
	response->set_port(server.port);
	response->set_error(ErrorCodes::Success);
	response->set_token(generate_unique_string());
	insertToken(request->uid(), response->token());
	return grpc::Status::OK;
}

grpc::Status StatusServiceImpl::Login(grpc::ServerContext* context, const message::LoginReq* request, message::LoginRsp* reply)
{
	auto uid = request->uid();
	auto token = request->token();
	std::string uid_str = std::to_string(uid);
	std::string token_get{};
	bool success = RedisMgr::GetInstance()->Get(user_token_prefix + uid_str, token_get);
	if(!success)
	{
		reply->set_error(ErrorCodes::UidInvalid);
		return grpc::Status::OK;
	}
	//成功后，获取的token放入token_get中
	if(token != token_get)
	{
		reply->set_error(ErrorCodes::TokenInvalid);
		return grpc::Status::OK;
	}
	reply->set_uid(uid);
	reply->set_token(token);
	reply->set_error(ErrorCodes::Success);
	return grpc::Status::OK;
}

void StatusServiceImpl::insertToken(int uid, std::string token)
{
	std::string uid_str = std::to_string(uid);
	std::string token_uid = user_token_prefix + uid_str;
	RedisMgr::GetInstance()->Set(token_uid, token);
}

ChatServer StatusServiceImpl::getChatServer()
{
	std::lock_guard lock(mutex_);
	auto minServer = servers_.begin()->second;
	auto count_str = RedisMgr::GetInstance()->HGet(login_count, minServer.name);
	if(count_str.empty())
	{
		minServer.con_count = INT_MAX;
	}
	else
	{
		minServer.con_count = std::stoi(count_str);
	}
	//遍历所有的server，找到连接数最少的一个并取出。
	for(auto& server : servers_)
	{
		if(server.second.name == minServer.name)
		{
			continue;
		}
		std::string count = RedisMgr::GetInstance()->HGet(login_count, server.first);

		if(count.empty())
		{
			server.second.con_count = INT_MAX;
		}
		else
		{
			server.second.con_count = std::stoi(count);
		}

		if (server.second.con_count < minServer.con_count)
			minServer = server.second;
	}
	return minServer;
}
