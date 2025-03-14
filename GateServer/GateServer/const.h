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
	Error_Json = 1,				// Json��������
	NetworkError = 2,			// �������
	VerifyExpired = 3,			// ��֤���ȡʧ��
	VerifyCodeErr = 4,			// ��֤�벻һ��
	UserExist = 5,				// �û��Ѿ�ע��
	EmailNotMatch = 6,			// ������û���ƥ��
	PasswordUpdateFailed = 7,	// �������ʧ��
	PasswordInvalid = 8,		// �������
	RPCGetFailed = 9,			// RPC�������
	TokenInvalid = 10,			// token��֤����
	UidInvalid = 11,			// uid��֤����
};

class ConfigMgr;
extern ConfigMgr gCfgMgr;

class Defer {
public:
	// ����һ��lambda���ʽ���ߺ���ָ��
	Defer(std::function<void()> func) : func_(func) {}

	// ����������ִ�д���ĺ���
	~Defer() {
		func_();
	}

private:
	std::function<void()> func_;
};

constexpr const char* code_prefix = "code_";