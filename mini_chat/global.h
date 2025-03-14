#ifndef GLOBAL_H
#define GLOBAL_H

#include<QDir>
#include <QWidget>
#include<functional>
#include "QStyle"

extern std::function<void(QWidget*)> repolish;
extern std::function<QString(QString)> xorString;//简单加密
extern std::function<QString(const QString&)> md5Encrypt; //md5加密


enum class ReqId
{
    ID_GET_VARIFY_CODE = 1001,  //获取验证码
    ID_REG_USER = 1002,         //注册用户
    ID_RESET_PASSWORD = 1003,   //重置密码
    ID_LOGIN_USER = 1004,       //请求登录
    ID_CHAT_LOGIN = 1005,       //向ChatServer请求连接
    ID_CHAT_LOGIN_RSP = 1006,   //处理返回的登录信息
};

enum class ErrorCodes
{
    SUCCESS = 0,
    ERR_JSON = 1,               //Json解析失败
    ERR_NETWORK = 2,            //网络请求错误
    ERR_VERIFY = 3,             //验证码获取失败
    ERR_VERIFY_CODE = 4,        //验证码不一致
    ERR_USR_EXIST = 5,          //用户已注册
    ERR_EMAIL_MATCH = 6,		//邮箱和用户不匹配
    ERR_PASSWORD_UPDATE = 7,	//密码更新失败
    ERR_PASSWORD = 8,           //密码错误
    ERR_RPCFAILED = 9,          //RPC请求错误
};

inline bool checkError(ErrorCodes err)
{
    switch(err)
    {
    case ErrorCodes::SUCCESS: return true;
    case ErrorCodes::ERR_JSON: qDebug() << "Json解析失败"; return false;
    case ErrorCodes::ERR_NETWORK: qDebug() << "网络请求失败"; return false;
    case ErrorCodes::ERR_VERIFY: qDebug() << "验证码获取失败"; return false;
    case ErrorCodes::ERR_VERIFY_CODE: qDebug() << "验证码不一致"; return false;
    case ErrorCodes::ERR_USR_EXIST: qDebug() << "用户已注册"; return false;
    case ErrorCodes::ERR_EMAIL_MATCH: qDebug() << "邮箱和用户不匹配"; return false;
    case ErrorCodes::ERR_PASSWORD_UPDATE: qDebug() << "密码更新失败"; return false;
    case ErrorCodes::ERR_PASSWORD: qDebug() << "密码错误"; return false;
    case ErrorCodes::ERR_RPCFAILED: qDebug() << "RPC请求错误"; return false;
    default:
        qDebug() << "return errorCode is not in ErrorCodes";return false;
    }
}
enum class Modules
{
    REGISTERMOD = 0,       //用户注册模块
    RESETMOD = 1,          //修改密码模块
    LOGINMOD = 2,          //用户登录模块
};

enum class TipErr{
    TIP_SUCCESS = 0,
    TIP_EMAIL_ERR = 1,
    TIP_PWD_ERR = 2,
    TIP_CONFIRM_ERR = 3,
    TIP_PWD_CONFIRM = 4,
    TIP_VERIFY_ERR = 5,
    TIP_USER_ERR = 6
};

enum class LabelState
{
    Normal = 1,
    Selected = 2
};

struct ServerInfo
{
    int uid;
    QString host;
    QString port;
    QString token;
};

enum class ChatUIMode
{
    ChatMode,
    SearchMode,
    ContactMode
};

enum class ListItemType
{
    CHAT_USER_ITEM, //聊天用户
    CONTACT_USER_ITEM, //联系人用户
    SEARCH_USER_ITEM, //搜索到的用户
    ADD_USER_TIP_ITEM, //提示添加用户
    INVALID_ITEM,  //不可点击条目
    GROUP_TIP_ITEM, //分组提示条目
    LINE_ITEM,  //分割线
    APPLY_FRIEND_ITEM, //好友申请
};

enum class ChatRole
{
    self,
    other
};

struct MsgInfo
{
    QString msgFlag;//表示消息类型
    QString msgcontent;//表示文件和图像的url,文本信息
    QPixmap pixmap;//文件和图片的缩略图
};

extern std::vector<QString> strs;
extern std::vector<QString> heads;
extern std::vector<QString> names;
extern QString gate_url_prefix;
constexpr int min_apply_label_edit_len = 40;
const QString add_prefix = "添加标签 ";
constexpr int tip_offset = 5;
#endif // GLOBAL_H
