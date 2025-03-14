#pragma once
#include <map>
#include <boost/asio.hpp>
class CSession;
class CServer
{
public:
	CServer(boost::asio::io_context& ioc, unsigned short port);
	~CServer();
	void startAccept();
	void handleAccept(std::shared_ptr<CSession> session, const boost::system::error_code& ec);
	void clearSession(std::string uuid);
private:
	boost::asio::ip::tcp::acceptor acceptor_;
	boost::asio::io_context& io_context_;
	unsigned short port_;
	std::map<std::string, std::shared_ptr<CSession>> sessions_;
	std::mutex mutex_;
	std::condition_variable condition_;
};

