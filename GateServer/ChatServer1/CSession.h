#pragma once
#include <queue>
#include <boost/asio.hpp>
#include "Message.h"
#include "const.h"
class CServer;
class CSession : public std::enable_shared_from_this<CSession>
{
	using socket = boost::asio::ip::tcp::socket;
public:
	CSession(boost::asio::io_context& ioc, CServer* server);
	void start();
	void close();
	socket& getSocket();
	std::string getSessionId();
	void asyncWrite(char* data, short length, short msg_id);
	void asyncWrite(const std::string& data, int msg_id);
	void asyncReadHead();
	void asyncReadBody(unsigned short);
private:
	void asyncReadFull(std::size_t maxLength, std::function<void(const boost::system::error_code&, std::size_t)> handler);
	void asyncReadLen(std::size_t read_len, std::size_t total_len,
		std::function<void(const boost::system::error_code&, std::size_t)> handler);
	void writeHandler(const boost::system::error_code& error_code, size_t bytes_transferred, std::shared_ptr<CSession> self);
private:
	CServer* server_;
	std::string token_;
	socket socket_;
	std::string session_id_;
	std::atomic<bool> b_close_;
	std::shared_ptr<Message> recv_head_node_;
	std::shared_ptr<Message> recv_msg_node_;;
	char* data_[max_length]{};
	int uuid_;
	int que_max_;
	std::queue<std::shared_ptr<Message>> write_que_;
	std::mutex write_mutex_;
};

