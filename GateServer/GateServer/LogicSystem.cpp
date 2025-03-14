#include "LogicSystem.h"
#include "LogicSystem.h"
#include "HttpConnection.h"
#include "message.pb.h"
#include "RedisMgr.h"
#include "VerifyGrpcClient.h"
#include "const.h"
#include "MysqlMgr.h"
#include "StatusGrpcClient.h"

LogicSystem::LogicSystem()
{
	RegGet("/get_test", [](std::shared_ptr<HttpConnection> connection){
		beast::ostream(connection->_response.body()) << "receive get_test req\n";
		int i = 0;
		for (auto& elem : connection->_get_params)
		{
			i++;
			beast::ostream(connection->_response.body())
				<< "param " << i << " key is " << elem.first << " value is " << elem.second << "\n";
		}
	});

	RegPost("/get_varifycode", [](std::shared_ptr<HttpConnection> connection)
		{
			auto body_str = beast::buffers_to_string(connection->_request.body().data());
			std::cout << "receive body is " << body_str << "\n";
			connection->_response.set(http::field::content_type, "text/json");
			Json::Value root;
			Json::Reader reader;
			Json::Value src_root;

			bool parse_success = reader.parse(body_str, src_root);
			if (!parse_success)
			{
				std::cout <<  "Failed to parse Json data!\n";
				root["error"] = ErrorCodes::Error_Json;
				std::string jsonstr = root.toStyledString();
				beast::ostream(connection->_response.body()) << jsonstr;
				return true;
			}
			if (!src_root.isMember("email"))
			{
				std::cout << "Failed to parse Json data!\n";
				root["error"] = ErrorCodes::Error_Json;
				std::string jsonstr = root.toStyledString();
				beast::ostream(connection->_response.body()) << jsonstr;
				return true;
			}
			auto email = src_root["email"].asString();
			message::GetVarifyRsp rsp = VerifyGrpcClient::GetInstance()->GetVarifyCode(email);
			std::cout << "email is " << email << "\n";
			root["error"] = rsp.error();
			root["email"] = src_root["email"];
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->_response.body()) << jsonstr;
			return true;
		});

	RegPost("/user_register", [](std::shared_ptr<HttpConnection> connection)
		{
			auto body_str = beast::buffers_to_string(connection->_request.body().data());
			std::cout << "receive body is " << body_str << "\n";
			connection->_response.set(http::field::content_type, "text/json");
			Json::Value root;
			Json::Reader reader;
			Json::Value src_root;

			bool parse_success = reader.parse(body_str, src_root);
			if (!parse_success)
			{
				std::cout << "Failed to parse Json data!\n";
				root["error"] = ErrorCodes::Error_Json;
				std::string jsonstr = root.toStyledString();
				beast::ostream(connection->_response.body()) << jsonstr;
				return true;
			}

			std::string varify_code;
			bool b_get_varify = RedisMgr::GetInstance()->Get(
				code_prefix + src_root["email"].asString(), varify_code);
			//���ʧ�ܣ������redis��û�и��������֤����Ϣ����δ�ɹ���ȡ��Ӧ��֤��
			if(!b_get_varify)
			{
				std::cout << "get verify_code expired\n";
				root["error"] = ErrorCodes::VerifyExpired;
				std::string jsonstr = root.toStyledString();
				beast::ostream(connection->_response.body()) << jsonstr;
				return true;
			}

			auto email = src_root["email"].asString();
			auto name = src_root["user"].asString();
			auto password = src_root["password"].asString();
			auto code = src_root["verify_code"].asString();

			//��ʱ˵����ȡ������֤�룬���ж��Ƿ���ͬ
			if (varify_code != code)
			{
				std::cout << "verify code error\n";
				root["error"] = ErrorCodes::VerifyCodeErr;
				std::string jsonstr = root.toStyledString();
				beast::ostream(connection->_response.body()) << jsonstr;
				return true;
			}
			//��mysql�в鿴�û��Ƿ��Ѿ�����
			int result = MysqlMgr::GetInstance()->RegUser(name, email, password);
			if(result == 0 | result == 1)
			{
				std::cout << " user or email exist" << std::endl;
				root["error"] = ErrorCodes::UserExist;
				std::string jsonstr = root.toStyledString();
				beast::ostream(connection->_response.body()) << jsonstr;
				return true;
			}

			root["error"] = ErrorCodes::Success;
			root["email"] = email;
			root["user"] = name;
			root["password"] = password;
			root["verify_code"] = code;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->_response.body()) << jsonstr;
			return true;
		});
	RegPost("/reset_password", [](std::shared_ptr<HttpConnection> connection)
		{
			auto body_str = beast::buffers_to_string(connection->_request.body().data());
			std::cout << "receive body is " << body_str << "\n";
			connection->_response.set(http::field::content_type, "text/json");
			Json::Value root;
			Json::Reader reader;
			Json::Value src_root;

			bool parse_success = reader.parse(body_str, src_root);
			if (!parse_success)
			{
				std::cout << "Failed to parse Json data!\n";
				root["error"] = ErrorCodes::Error_Json;
				std::string jsonstr = root.toStyledString();
				beast::ostream(connection->_response.body()) << jsonstr;
				return true;
			}

			auto email = src_root["email"].asString();
			auto name = src_root["user"].asString();
			auto password = src_root["password"].asString();
			auto code = src_root["verify_code"].asString();

			std::string verify_code;
			bool b_get_verify = RedisMgr::GetInstance()->Get(
				code_prefix + email, verify_code);
			//���ʧ�ܣ������redis��û�и��������֤����Ϣ����δ�ɹ���ȡ��Ӧ��֤��
			if (!b_get_verify)
			{
				std::cout << "get verify_code expired\n";
				root["error"] = ErrorCodes::VerifyExpired;
				std::string jsonstr = root.toStyledString();
				beast::ostream(connection->_response.body()) << jsonstr;
				return true;
			}
			//��ʱ˵����ȡ������֤�룬���ж��Ƿ���ͬ
			if (verify_code != code)
			{
				std::cout << "verify code error\n";
				root["error"] = ErrorCodes::VerifyCodeErr;
				std::string jsonstr = root.toStyledString();
				beast::ostream(connection->_response.body()) << jsonstr;
				return true;
			}
			//����û����������Ƿ��ܹ���Ӧ
			bool is_ok = MysqlMgr::GetInstance()->CheckEmail(name, email);
			if(!is_ok)
			{
				std::cout << "user email not match\n";
				root["error"] = ErrorCodes::EmailNotMatch;
				std::string jsonstr = root.toStyledString();
				beast::ostream(connection->_response.body()) << jsonstr;
				return true;
			}
			//��������Ϊ��������
			bool b_up = MysqlMgr::GetInstance()->UpdatePassword(name, password);
			if (!b_up) {
				std::cout << " update pwd failed" << std::endl;
				root["error"] = ErrorCodes::PasswordUpdateFailed;
				std::string jsonstr = root.toStyledString();
				beast::ostream(connection->_response.body()) << jsonstr;
				return true;
			}

			std::cout << std::format("{}'s password update success.\n", name);
			root["error"] = ErrorCodes::Success;
			root["email"] = email;
			root["user"] = name;
			root["password"] = password;
			root["verify_code"] = code;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->_response.body()) << jsonstr;
			return true;
		});
		RegPost("/user_login", [](std::shared_ptr<HttpConnection> connection)
			{
				auto body_str = beast::buffers_to_string(connection->_request.body().data());
				std::cout << "request body is: " << body_str << "\n";
				connection->_response.set(http::field::content_type, "text/json");

				Json::Value src_root;
				Json::Value root;
				Json::Reader reader;
				bool parse_success = reader.parse(body_str, src_root);
				//�������ɹ��򷵻ش�����Ϣ
				if (!parse_success)
				{
					std::cout << "json��������\n";
					root["error"] = ErrorCodes::Error_Json;
					auto jsonstr = root.toStyledString();
					beast::ostream(connection->_response.body()) << jsonstr;
					return true;
				}
				//��ȡ������Ϣ
				auto name = src_root["user"].asString();
				auto password = src_root["password"].asString();

				//��mysql�в�ѯ�û�������
				UserInfo user_info;
				bool check_success = MysqlMgr::GetInstance()->CheckPassword(name, password, user_info);
				if (!check_success)
				{
					std::cout << "user password not match\n";
					root["error"] = ErrorCodes::PasswordInvalid;
					auto jsonstr = root.toStyledString();
					beast::ostream(connection->_response.body()) << jsonstr;
					return true;
				}
				//��ѯStatusServerѰ������
				auto reply = StatusGrpcClient::GetInstance()->GetChatServer(user_info.uid);
				if (reply.error())
				{
					std::cout << "grpc get chat server failed, error is: " << reply.error() << "\n";
					root["error"] = ErrorCodes::RPCGetFailed;
					std::string jsonstr = root.toStyledString();
					beast::ostream(connection->_response.body()) << jsonstr;
					return true;
				}
				std::cout << "succeed to load user_info, uid is:" << user_info.uid << "\n";
				root["user"] = user_info.name;
				root["password"] = user_info.password;
				root["token"] = reply.token();
				root["host"] = reply.host();
				root["port"] = reply.port();
				root["error"] = ErrorCodes::Success;

				auto jsonstr = root.toStyledString();
				beast::ostream(connection->_response.body()) << jsonstr;
				return true;
			});
}

void LogicSystem::RegGet(std::string url, HttpHandler handler)
{
	_get_handlers.insert(std::make_pair(url, handler));
}

bool LogicSystem::HandlePost(std::string path, std::shared_ptr<HttpConnection> con)
{
	if (_post_handlers.find(path) == _post_handlers.end())
	{
		return false;
	}
	_post_handlers[path](con);
	return true;
}

void LogicSystem::RegPost(std::string url, HttpHandler handler)
{
	_post_handlers.insert(std::make_pair(url, handler));
}

bool LogicSystem::HandleGet(std::string path, std::shared_ptr<HttpConnection> con)
{
	//���Ѿ��ж�Ӧ��Ϣ�Ĵ����߼�������ã����򷵻ش���
	if (_get_handlers.find(path) == _get_handlers.end())
	{
		return false;
	}
	_get_handlers[path](con);
	return true;
}
