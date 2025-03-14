#pragma once
#include "const.h"

class HttpConnection : public std::enable_shared_from_this<HttpConnection>
{
	friend class LogicSystem;
public:
	HttpConnection(boost::asio::io_context& ioc);
	void Start();
	tcp::socket& GetSocket()
	{
		return _socket;
	}
private:
	void CheckDeadline();
	void WriteResponse();
	void HandleReq();
	void PreParseGetParam();
	tcp::socket _socket;
	//序列化缓冲区
	beast::flat_buffer _buffer{ 8192 };
	//接收到的消息(动态类型)
	http::request<http::dynamic_body> _request;
	//要回复的消息(动态类型)
	http::response<http::dynamic_body> _response;
	//连接过程消耗的最长时间
	asio::steady_timer _deadline{
		_socket.get_executor(), std::chrono::seconds(60)
	};
	std::string _get_url;
	std::unordered_map<std::string, std::string> _get_params;
};

