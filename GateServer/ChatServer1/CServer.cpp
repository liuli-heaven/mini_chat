#include "CServer.h"

#include <iostream>

#include "CSession.h"
#include "IOServicePool.h"

CServer::CServer(boost::asio::io_context& ioc, unsigned short port):port_(port), io_context_(ioc),
                                                                    acceptor_(ioc, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{
	std::cout << "start accept connection.\n";
	startAccept();
}

CServer::~CServer()
{
	std::lock_guard lock(mutex_);
	if(!sessions_.empty())
	{
		sessions_.erase(sessions_.begin(), sessions_.end());
	}
}

void CServer::startAccept()
{
	auto& ioc = IOServicePool::GetInstance()->GetIOService();
	auto session = std::make_shared<CSession>(CSession(ioc, this));
	acceptor_.async_accept(session->getSocket(), std::bind(&CServer::handleAccept, this, session, std::placeholders::_1));
}

void CServer::handleAccept(std::shared_ptr<CSession> session, const boost::system::error_code& ec)
{
	if(!ec)
	{
		session->start();
		std::lock_guard lock(mutex_);
		sessions_.insert({ session->getSessionId(), session });
	}
	else
	{
		std::cout << "handleAccept error: " << ec.what() << "\n";
	}
	startAccept();
}

void CServer::clearSession(std::string uuid)
{
		erase_if(sessions_, uuid);
}
