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
			// ת��_socket�е���Դ�Դ���HttpConnection������
			new_con->Start();
			// ���������µ���������
			self->Start();
		}
		catch (std::exception& exp)
		{
			//����������⣬���ӡ������Ϣ�������¼�����������
			std::cout << "exception is:" << exp.what() << "\n";
		}
		});
}
