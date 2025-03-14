#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "const.h"
#include "ConfigMgr.h"
#include "hiredis.h"
#include "RedisMgr.h"
#include "MysqlMgr.h"
#include "IOServicePool.h"
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <boost/asio.hpp>
#include <grpcpp/server_builder.h>

#include "StatusServiceImpl.h"

void RunServer()
{
	auto& config_mgr = ConfigMgr::Inst();

	std::string server_address = config_mgr["StatusServer"]["host"] +
								":" + config_mgr["StatusServer"]["port"];
	StatusServiceImpl service;

	grpc::ServerBuilder builder;
	//监听端口和添加服务
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	builder.RegisterService(&service);

	//构建并启动grpc服务器
	std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << "\n";

	//创建上下文
	boost::asio::io_context ioc;
	boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);

	//异步等待信号
	signals.async_wait([&server](const boost::system::error_code& error, int signal_number)
		{
			if (!error)
			{
				std::cout << "Shutting down server...\n";
				server->Shutdown();//服务器关闭
			}
		});

	//让io_context异步运行
	std::thread([&ioc]()
	{
			ioc.run();
		}).detach();

	//等待服务器关闭
	server->Wait();
	ioc.stop();//停止io_context
}

int main(int argc, char** argv)
{
	try
	{
		RunServer();
	}
	catch (std::exception const& e)
	{
		std::cerr << "error: " << e.what() << "\n";
		return EXIT_FAILURE;
	}

	return 0;
}
