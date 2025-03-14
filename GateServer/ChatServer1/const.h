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

constexpr const char* code_prefix = "code_";

constexpr int head_total_len = 4;
constexpr int head_id_len = 2;
constexpr int max_length = 1024 * 2;

enum class MSG_IDS
{
	MSG_CHAT_LOGIN = 1005, //�û���½
	MSG_CHAT_LOGIN_RSP = 1006, //�û���½�ذ�
	ID_SEARCH_USER_REQ = 1007, //�û���������
	ID_SEARCH_USER_RSP = 1008, //�����û��ذ�
	ID_ADD_FRIEND_REQ = 1009, //������Ӻ�������
	ID_ADD_FRIEND_RSP = 1010, //������Ӻ��ѻظ�
	ID_NOTIFY_ADD_FRIEND_REQ = 1011,  //֪ͨ�û���Ӻ�������
	ID_AUTH_FRIEND_REQ = 1013,  //��֤��������
	ID_AUTH_FRIEND_RSP = 1014,  //��֤���ѻظ�
	ID_NOTIFY_AUTH_FRIEND_REQ = 1015, //֪ͨ�û���֤��������
	ID_TEXT_CHAT_MSG_REQ = 1017, //�ı�������Ϣ����
	ID_TEXT_CHAT_MSG_RSP = 1018, //�ı�������Ϣ�ظ�
	ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019, //֪ͨ�û��ı�������Ϣ
};

//mysql��ѯ��Ҫ�����ݽṹ
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