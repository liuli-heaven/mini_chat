#include "LogicSystem.h"

#include <ranges>

#include "MysqlMgr.h"
#include "StatusGrpcClient.h"

LogicSystem::LogicSystem(): b_stop_(false), que_max_(100)
{
	initCallBacks();
	worker_thread_ = std::thread(&LogicSystem::dealMsg);
}

LogicSystem::~LogicSystem()
{
	b_stop_ = true;
	condition_.notify_all();
	worker_thread_.join();
}

void LogicSystem::postMsgToQue(std::shared_ptr<LogicNode> msg)
{
	std::unique_lock lock(mutex_);
	//如果队列已满，则进入阻塞状态，等待消费之后被唤醒。
	condition_.wait(lock, [this]()
		{
			return this->msg_que_.size() < que_max_;
		});
	msg_que_.push(msg);
	if(msg_que_.size() == 1)
	{
		lock.unlock();
		condition_.notify_one();
	}
}

void LogicSystem::LoginHandler(std::shared_ptr<CSession> session, short msg_id, const std::string& data)
{
	Json::Value src_root;
	Json::Reader reader;
	Json::Value root;
	bool success = reader.parse(data, src_root);
	if(!success)
	{
		std::cout << "LoginHandler parse json error.\n";
		root["error"] = ErrorCodes::Error_Json;
		std::string jsonstr = root.toStyledString();
		session->asyncWrite(jsonstr, msg_id);
		return;
	}
	auto uid = src_root["uid"].asInt();
	auto token = src_root["token"].asString();
	std::cout << "login user uid is: " << uid << "\n";
	std::cout << "token is: " << token << "\n";
	//确认token是否匹配
	message::LoginRsp rsp = StatusGrpcClient::GetInstance()->Login(uid, token);
	root["error"] = rsp.error();
	if(rsp.error() != ErrorCodes::Success)
	{
		std::string jsonstr = root.toStyledString();
		session->asyncWrite(jsonstr, msg_id);
		return;
	}
	//查询用户信息
	auto iter = users_.find(uid);
	std::shared_ptr<UserInfo> user_info = nullptr;
	if(iter == users_.end())
	{
		//如果内存中没有存储该用户信息，则在数据库中查找
		user_info = MysqlMgr::GetInstance()->GetUser(uid);
		if(user_info == nullptr)
		{
			root["error"] = ErrorCodes::UidInvalid;
			std::string jsonstr = root.toStyledString();
			session->asyncWrite(jsonstr, msg_id);
			return;
		}
		users_[uid] = user_info;
	}
	else
	{
		user_info = iter->second;
	}
	root["uid"] = user_info->uid;
	root["name"] = user_info->name;
	root["token"] = token;
	std::string jsonstr = root.toStyledString();
	session->asyncWrite(jsonstr, msg_id);
}
void LogicSystem::initCallBacks()
 {
	 callback_functions_[MSG_IDS::MSG_CHAT_LOGIN] = std::bind(
		 &LogicSystem::LoginHandler, this, std::placeholders::_1,
		 std::placeholders::_2, std::placeholders::_3);

 }

void LogicSystem::dealMsg()
 {
	 while (true)
	 {
	 	std::unique_lock lock(mutex_);
		condition_.wait(lock, [this]()
			{
				return !this->b_stop_ && !this->msg_que_.empty();
			});
		//如果已经停止，则将所有消息处理完毕后停止。
		if(b_stop_)
		{
			while(!msg_que_.empty())
			{
				auto msg = msg_que_.front();
				short id = msg->recv_node_->id_;
				MSG_IDS msg_id = static_cast<MSG_IDS>(id);
				auto func_iter = callback_functions_.find(msg_id);
				if(func_iter == callback_functions_.end())
				{
					func_iter->second(msg->session_, id, msg->recv_node_->data_);
					
				}
				else
				{
					std::cout << "msg [" << id << "] handler not found\n";
				}
				msg_que_.pop();
				if(msg_que_.size() + 1 == que_max_)
				{
					lock.unlock();
					condition_.notify_one();
				}
			}
			break;
		}
		//此时说明逻辑系统没有停止，且队列中存在数据
		auto msg = msg_que_.front();
		short id = msg->recv_node_->id_;
		MSG_IDS msg_id = static_cast<MSG_IDS>(id);
		auto func_iter = callback_functions_.find(msg_id);
		if (func_iter == callback_functions_.end())
		{
			func_iter->second(msg->session_, id, msg->recv_node_->data_);
		}
		else
		{
			std::cout << "msg [" << id << "] handler not found\n";
		}
		msg_que_.pop();
		if (msg_que_.size() + 1 == que_max_)
		{
			lock.unlock();
			condition_.notify_one();
		}
	 }
 }

LogicNode::LogicNode(std::shared_ptr<CSession> session, std::shared_ptr<Message> message)
	: session_(session), recv_node_(message)
{
	
}
