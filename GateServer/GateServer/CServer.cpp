#include "CServer.h"

#include <iostream>
#include "HttpConnection.h"
#include "IOServicePool.h"

CServer::CServer(boost::asio::io_context& ioc, unsigned short port)
	:_ioc(ioc), _acceptor(ioc, tcp::endpoint(tcp::v4(), port))
{

}



void CServer::Start()
{
	auto self = shared_from_this();
	auto& ioc = IOServicePool::GetInstance()->GetIOService();
	std::shared_ptr<HttpConnection> new_con = std::make_shared<HttpConnection>(ioc);
	_acceptor.async_accept(new_con->GetSocket(), [self, new_con](beast::error_code ec) {
		try
		{
			if (ec)
			{
				self->Start();
				return;
			}
			// 转移_socket中的资源以创建HttpConnection来管理。
			new_con->Start();
			// 继续监听新的连接请求。
			self->Start();
		}
		catch (std::exception& exp)
		{
			//如果出现问题，则打印错误信息，并重新监听连接请求。
			std::cout << "exception is:" << exp.what() << "\n";
		}
		});
}
