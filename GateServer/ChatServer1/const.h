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

constexpr const char* code_prefix = "code_";

constexpr int head_total_len = 4;
constexpr int head_id_len = 2;
constexpr int max_length = 1024 * 2;

enum class MSG_IDS
{
	MSG_CHAT_LOGIN = 1005, //用户登陆
	MSG_CHAT_LOGIN_RSP = 1006, //用户登陆回包
	ID_SEARCH_USER_REQ = 1007, //用户搜索请求
	ID_SEARCH_USER_RSP = 1008, //搜索用户回包
	ID_ADD_FRIEND_REQ = 1009, //申请添加好友请求
	ID_ADD_FRIEND_RSP = 1010, //申请添加好友回复
	ID_NOTIFY_ADD_FRIEND_REQ = 1011,  //通知用户添加好友申请
	ID_AUTH_FRIEND_REQ = 1013,  //认证好友请求
	ID_AUTH_FRIEND_RSP = 1014,  //认证好友回复
	ID_NOTIFY_AUTH_FRIEND_REQ = 1015, //通知用户认证好友申请
	ID_TEXT_CHAT_MSG_REQ = 1017, //文本聊天信息请求
	ID_TEXT_CHAT_MSG_RSP = 1018, //文本聊天信息回复
	ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019, //通知用户文本聊天信息
};

//mysql查询需要的数据结构
struct UserInfo
{
	UserInfo() :name(""), password(""), uid(0), email(""), nick(""), desc(""), sex(0), icon(""), back("") {}
	int uid;
	std::string name;
	std::string password;
	std::string email;
	std::string nick;
	std::string desc;
	int sex;
	std::string icon;
	std::string back;
};

struct ApplyInfo {
	ApplyInfo(int uid, std::string name, std::string desc,
		std::string icon, std::string nick, int sex, int status)
		:_uid(uid), _name(name), _desc(desc),
		_icon(icon), _nick(nick), _sex(sex), _status(status) {}

	int _uid;
	std::string _name;
	std::string _desc;
	std::string _icon;
	std::string _nick;
	int _sex;
	int _status;
};