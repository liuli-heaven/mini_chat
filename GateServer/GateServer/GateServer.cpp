#include <iostream>
#include <json/reader.h>
#include <json/json.h>
#include <json/value.h>
#include <boost/asio/io_context.hpp>
#include "const.h"
#include "CServer.h"
#include "ConfigMgr.h"
#include "RedisMgr.h"

void TestRedis()
{
	//redis默认监听端口为6379,可在redis.windows.conf配置文件中修改
	redisContext* c = redisConnect("127.0.0.1", 6380);
	if (c->err)
	{
		std::cout << "Connect to redisServer fail: " << std::string(c->errstr) << "\n";
		redisFree(c);
		return;
	}
	std::cout << "Connect to redisServer Success\n";

	const char* redis_password =  "518518";
	redisReply* r = (redisReply*)redisCommand(c, "AUTH %s", redis_password);
	if(r->type == REDIS_REPLY_ERROR)
	{
		std::cout << "Redis认证失败!\n";
	}
	else
	{
		std::cout << "Redis认证成功!\n";
	}

	//为redis设置key
	const char* command1 = "set test1 value1";

	//执行redis命令行
	r = (redisReply*)redisCommand(c, command1);

	//如果返回null则说明执行失败
	if(r == NULL)
	{
		std::cout << "Execute command1 failure\n";
		redisFree(c);
		return;
	}

	//如果执行失败则释放连接
	if (!(r->type == REDIS_REPLY_STATUS && 
		(strcmp(r->str, "OK") == 0 || strcmp(r->str, "ok") == 0))
		)
	{
		std::cout << std::format("Failed to execute command[{}]\n", command1);
		freeReplyObject(r);
		redisFree(c);
		return;
	}

	//执行成功，释放redisCommand执行后返回的redisReply所占用的内存
	freeReplyObject(r);
	std::cout << std::format("Succeed to execute command[{}]\n", command1);

	const char* command2 = "STRLEN test1";
	r = (redisReply*)redisCommand(c, command2);

	//如果返回类型不是整形，则释放连接
	if(r->type != REDIS_REPLY_INTEGER)
	{
		std::cout << std::format("Failed to execute command[{}]\n", command2);
		freeReplyObject(r);
		redisFree(c);
		return;
	}

	//获取字符串长度
	int length = r->integer;
	freeReplyObject(r);
	std::cout << std::format("The length of 'test1' is {}.\n", length);
	std::cout << std::format("Succeed to execute command[{}]\n", command2);

	//获取redis键值对信息
	const char* command3 = "get test1";
	r = (redisReply*)redisCommand(c, command3);
	if(r->type != REDIS_REPLY_STRING)
	{
		std::cout << std::format("Failed to execute command[{}]\n", command3);
		freeReplyObject(r);
		redisFree(c);
		return;
	}
	std::cout << std::format("The value of 'test1' is: {}.\n", r->str);
	freeReplyObject(r);
	std::cout << std::format("Succeed to execute command[{}]\n", command3);

	const char* command4 = "get test2";
	r = (redisReply*)redisCommand(c, command4);
	if (r->type != REDIS_REPLY_NIL)
	{
		std::cout << std::format("Failed to execute command[{}]\n", command4);
		freeReplyObject(r);
		redisFree(c);
		return;
	}
	std::cout << std::format("Succeed to execute command[{}].\n", command4);

	//释放连接资源
	redisFree(c);
}

void TestRedisMgr() {
	assert(RedisMgr::GetInstance()->Set("blogwebsite", "llfc.club"));
	std::string value = "";
	assert(RedisMgr::GetInstance()->Get("blogwebsite", value));
	assert(RedisMgr::GetInstance()->Get("nonekey", value) == false);
	assert(RedisMgr::GetInstance()->Delete("bloginfo"));
	assert(RedisMgr::GetInstance()->HSet("bloginfo", "blogwebsite", "llfc.club"));
	assert(RedisMgr::GetInstance()->HGet("bloginfo", "blogwebsite") != "");
	assert(RedisMgr::GetInstance()->ExistKey("bloginfo"));
	assert(RedisMgr::GetInstance()->Delete("bloginfo"));
	assert(RedisMgr::GetInstance()->Delete("bloginfo"));
	assert(RedisMgr::GetInstance()->ExistKey("bloginfo") == false);
	assert(RedisMgr::GetInstance()->LPush("lpushkey1", "lpushvalue1"));
	assert(RedisMgr::GetInstance()->LPush("lpushkey1", "lpushvalue2"));
	assert(RedisMgr::GetInstance()->LPush("lpushkey1", "lpushvalue3"));
	assert(RedisMgr::GetInstance()->RPop("lpushkey1", value));
	assert(RedisMgr::GetInstance()->RPop("lpushkey1", value));
	assert(RedisMgr::GetInstance()->LPop("lpushkey1", value));
	assert(RedisMgr::GetInstance()->LPop("lpushkey2", value) == false);
	RedisMgr::GetInstance()->Close();
}

int main()
{
	try
	{
		//TestRedis();
		//TestRedisMgr();
		ConfigMgr& gCfgMgr = ConfigMgr::Inst();
		std::string gate_port_str = gCfgMgr["GateServer"]["port"];
		unsigned short gate_port = atoi(gate_port_str.c_str());
		asio::io_context ioc{ 1 };
		asio::signal_set signals(ioc, SIGINT, SIGTERM);
		signals.async_wait([&ioc](boost::system::error_code ec, int signal_number)
			{
				if (ec)
				{
					return;
				}
				ioc.stop();
			});
		std::make_shared<CServer>(ioc, gate_port)->Start();
		std::cout <<  "Gate Server listen on port: " <<  gate_port << "\n";
		ioc.run();
	}
	catch (const std::exception& ex)
	{
		std::cout << "exception is: " << ex.what() << "\n";
		return EXIT_FAILURE;
	}
	return 0;
}
