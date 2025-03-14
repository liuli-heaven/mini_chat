#pragma once
#include <boost/beast/http.hpp>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include <hiredis.h>

namespace beast = boost::beast;
namespace http = beast::http;
namespace asio = boost::asio;
using tcp = asio::ip::tcp;

enum ErrorCodes
{
	Success = 0,
	Error_Json = 1,				// Json解析错误
	NetworkError = 2,			// 网络错误
	VerifyExpired = 3,			// 验证码获取失败
	VerifyCodeErr = 4,			// 验证码不一致
	UserExist = 5,				// 用户已经注册
	EmailNotMatch = 6,			// 邮箱和用户不匹配
	PasswordUpdateFailed = 7,	// 密码更新失败
	PasswordInvalid = 8,		// 密码错误
	RPCGetFailed = 9,			// RPC请求错误
	TokenInvalid = 10,			// token验证错误
	UidInvalid = 11,			// uid验证错误
};

class ConfigMgr;
extern ConfigMgr gCfgMgr;

class Defer {
public:
	// 接受一个lambda表达式或者函数指针
	Defer(std::function<void()> func) : func_(func) {}

	// 析构函数中执行传入的函数
	~Defer() {
		func_();
	}

private:
	std::function<void()> func_;
};

constexpr const char* code_prefix = "code_";