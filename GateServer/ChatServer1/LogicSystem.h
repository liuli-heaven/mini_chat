#pragma once
#include <map>
#include <iostream>
#include <functional>
#include <queue>

#include "CSession.h"
#include "Singleton.h"
#include <string>
#include "const.h"

class LogicNode {
	friend class LogicSystem;
public:
	LogicNode(std::shared_ptr<CSession>, std::shared_ptr<Message>);
private:
	std::shared_ptr<CSession> session_;
	std::shared_ptr<Message> recv_node_;
};

class LogicSystem : public Singleton<LogicSystem>
{
	friend class Singleton<LogicSystem>;
	using CallBackFunc = std::function<void(std::shared_ptr<CSession>, const short&, const std::string&)>;
public:
	~LogicSystem();
	void postMsgToQue(std::shared_ptr<LogicNode> msg);
private:
	void initCallBacks();
	void LoginHandler(std::shared_ptr<CSession> session, short msg_id, const std::string& data);
	LogicSystem();
	void dealMsg();
private:
	std::thread worker_thread_;
	std::queue<std::shared_ptr<LogicNode>> msg_que_;
	short que_max_;
	std::mutex mutex_;
	std::condition_variable condition_;
	bool b_stop_;
	std::map<MSG_IDS, CallBackFunc> callback_functions_;
	std::map<short, std::shared_ptr<UserInfo>> users_;
};

