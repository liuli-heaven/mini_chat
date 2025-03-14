#include "RedisMgr.h"
#include <format>
#include "ConfigMgr.h"

RedisMgr::~RedisMgr()
{
    Close();
}

RedisMgr::RedisMgr()
{
    auto& gCfMgr = ConfigMgr::Inst();
    auto host = gCfMgr["Redis"]["host"];
    auto port = gCfMgr["Redis"]["port"];
    auto pwd = gCfMgr["Redis"]["password"];
    _conPool.reset(new RedisConPool(5, host.c_str(), atoi(port.c_str()), pwd.c_str()));

}

//bool RedisMgr::Connect(const std::string& host, int port)
//{
//    auto* connect = _conPool->GetConnection();
//    connect = redisConnect(host.c_str(), port);
//    if(connect != NULL && connect->err)
//    {
//        std::cout << "connect error" << connect->errstr << "\n";
//        return false;
//    }
//    return true;
//}

bool RedisMgr::Get(const std::string& key, std::string& value)
{
    auto* connect = _conPool->GetConnection();
    if (connect == nullptr)
    {
        return false;
    }
    redisReply*  reply = (redisReply*)redisCommand(connect, "GET %s", key.c_str());

    if(reply == NULL)
    {
        std::cout << std::format("[GET {}] failed\n", key);
        freeReplyObject(reply);
        _conPool->ReturnConnection(connect);
        return false;
    }

    if(reply->type != REDIS_REPLY_STRING)
    {
        std::cout << std::format("[GET {}] failed\n", key);
        freeReplyObject(reply);
        _conPool->ReturnConnection(connect);
        return false;
    }

    value = reply->str;
    freeReplyObject(reply);
    std::cout << std::format("Succeed to execute command [GET {}]\n", key);
    _conPool->ReturnConnection(connect);
    return true;
}

//设置键值对
bool RedisMgr::Set(const std::string& key, const std::string& value)
{
    auto* connect = _conPool->GetConnection();
    if (connect == nullptr)
    {
        return false;
    }
    redisReply* reply = (redisReply*)redisCommand(connect,
        "SET %s %s", key.c_str(), value.c_str());

    if (reply == NULL)
    {
        std::cout << std::format("Execute command [ SET {0} {1}] failed\n", key, value);
        freeReplyObject(reply);
        _conPool->ReturnConnection(connect);
        return false;
    }

    if(!(reply->type == REDIS_REPLY_STATUS && 
        (strcmp(reply->str, "ok") == 0 
            || strcmp(reply->str, "OK") == 0)))
    {
        std::cout << std::format("Execute command [ SET {} {} ] failed\n", key, value);
        freeReplyObject(reply);
        _conPool->ReturnConnection(connect);
        return false;
    }

    freeReplyObject(reply);
    std::cout << std::format("Execute command [ SET {} {} ] success\n", key, value);
    _conPool->ReturnConnection(connect);
    return true;
}

//bool RedisMgr::Auth(const std::string& password)
//{
//    auto* connect = _conPool->GetConnection();
//    if (connect == nullptr)
//    {
//        return false;
//    }
//    redisReply* reply = (redisReply*)redisCommand(connect, "AUTH %s", password.c_str());
//    if(reply->type == REDIS_REPLY_ERROR)
//    {
//        std::cout << "认证失败\n";
//        freeReplyObject(reply);
//        return false;
//    }
//    else
//    {
//        freeReplyObject(reply);
//        std::cout << "认证成功\n";
//        return true;
//    }
//}

bool RedisMgr::LPush(const std::string& key, const std::string& value)
{
    auto* connect = _conPool->GetConnection();
    if (connect == nullptr)
    {
        return false;
    }
    redisReply* reply = (redisReply*)redisCommand(connect, "LPUSH %s %s", key.c_str(), value.c_str());
    if(reply == NULL)
    {
        std::cout << std::format("Execute command [ LPUSH {} {} ] failed\n", key, value);
        freeReplyObject(reply);
        _conPool->ReturnConnection(connect);
        return false;
    }
    if(reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0)
    {
        std::cout << std::format("Execute command [ LPUSH {} {} ] failed\n", key, value);
        freeReplyObject(reply);
        _conPool->ReturnConnection(connect);
        return false;
    }

    std::cout << std::format("Execute command [ LPUSH {} {} ] success\n", key, value);
    freeReplyObject(reply);
    _conPool->ReturnConnection(connect);
    return true;
}

bool RedisMgr::LPop(const std::string& key, std::string& value)
{
    auto* connect = _conPool->GetConnection();
    if (connect == nullptr)
    {
        return false;
    }
    redisReply* reply = (redisReply*)redisCommand(connect, "LPOP %s", key.c_str());
    if(reply == NULL || reply->type == REDIS_REPLY_NIL)
    {
        std::cout << std::format("Execute command [ LPOP {} ] failed\n", key);
        freeReplyObject(reply);
        _conPool->ReturnConnection(connect);
        return false;
    }
    value = reply->str;
    std::cout << std::format("Execute command [ LPOP {} ] success\n", key);
    freeReplyObject(reply);
    _conPool->ReturnConnection(connect);
    return true;
}

bool RedisMgr::RPush(const std::string& key, const std::string& value)
{
    auto* connect = _conPool->GetConnection();
    if (connect == nullptr)
    {
        return false;
    }
    redisReply* reply = (redisReply*)redisCommand(connect, "RPUSH %s %s", key.c_str(), value.c_str());
    if (reply == NULL)
    {
        std::cout << std::format("Execute command [ RPUSH {} {} ] failed\n", key, value);
        freeReplyObject(reply);
        _conPool->ReturnConnection(connect);
        return false;
    }
    if (reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0)
    {
        std::cout << std::format("Execute command [ RPUSH {} {} ] failed\n", key, value);
        freeReplyObject(reply);
        _conPool->ReturnConnection(connect);
        return false;
    }

    std::cout << std::format("Execute command [ RPUSH {} {} ] success\n", key, value);
    freeReplyObject(reply);
    _conPool->ReturnConnection(connect);
    return true;
}

bool RedisMgr::RPop(const std::string& key, std::string& value)
{
    auto* connect = _conPool->GetConnection();
    if (connect == nullptr)
    {
        return false;
    }
    redisReply* reply = (redisReply*)redisCommand(connect, "RPOP %s", key.c_str());
    if (reply == NULL || reply->type == REDIS_REPLY_NIL)
    {
        std::cout << std::format("Execute command [ RPOP {} ] failed\n", key);
        freeReplyObject(reply);
        _conPool->ReturnConnection(connect);
        return false;
    }
    value = reply->str;
    std::cout << std::format("Execute command [ RPOP {} ] success\n", key);
    freeReplyObject(reply);
    _conPool->ReturnConnection(connect);
    return true;
}

bool RedisMgr::HSet(const std::string& key, const std::string& hkey, const std::string& value)
{
    auto* connect = _conPool->GetConnection();
    if (connect == nullptr)
    {
        return false;
    }
    redisReply* reply = (redisReply*)redisCommand(connect,
        "HSET %s %s %s", key.c_str(), hkey.c_str(), value.c_str());
    if (reply == NULL || reply->type != REDIS_REPLY_INTEGER)
    {
        std::cout << std::format("Execute command [ HSET {0} {1} {2} ] failed\n",
            key, hkey, value);
        freeReplyObject(reply);
        _conPool->ReturnConnection(connect);
        return false;
    }
    std::cout << std::format("Execute command [ HSET {0} {1} {2} ] success\n",
        key, hkey, value);
    freeReplyObject(reply);
    _conPool->ReturnConnection(connect);
    return true;
}

bool RedisMgr::HSet(const char* key, const char* hkey, const char* hvalue, size_t hvaluelen)
{
    const char* argv[4];
    size_t argvlen[4];
    argv[0] = "HSET";
    argvlen[0] = 4;
    argv[1] = key;
    argvlen[1] = strlen(key);
    argv[2] = hkey;
    argvlen[2] = strlen(hkey);
    argv[3] = hvalue;
    argvlen[3] = hvaluelen;
    auto* connect = _conPool->GetConnection();
    if (connect == nullptr)
    {
        return false;
    }
    redisReply* reply = (redisReply*)redisCommandArgv(connect, 4, argv, argvlen);
    if (reply == NULL || reply->type != REDIS_REPLY_INTEGER)
    {
        std::cout << std::format("Execute command [ HSET {0} {1} {2} ] failed\n",
            key, hkey, hvalue);
        freeReplyObject(reply);
        _conPool->ReturnConnection(connect);
        return false;
    }
    std::cout << std::format("Execute command [ HSET {0} {1} {2} ] failed\n",
        key, hkey, hvalue);
    freeReplyObject(reply);
    _conPool->ReturnConnection(connect);
    return true;
}

std::string RedisMgr::HGet(const std::string& key, const std::string& hkey)
{
    const char* argv[3];
    size_t argvlen[3];
    argv[0] = "HGET";
    argvlen[0] = 4;
    argv[1] = key.c_str();
    argvlen[1] = key.size();
    argv[2] = hkey.c_str();
    argvlen[2] = hkey.size();
    auto* connect = _conPool->GetConnection();
    if (connect == nullptr)
    {
        return "";
    }
    redisReply* reply = (redisReply*)redisCommandArgv(connect, 3, argv, argvlen);
    if (reply == NULL || reply->type == REDIS_REPLY_NIL)
    {
        std::cout << std::format("Execute command [ HGET {0} {1}  ] failed\n",
            key, hkey);
        freeReplyObject(reply);
        _conPool->ReturnConnection(connect);
        return "";
    }

    std::string value = reply->str;
    std::cout << std::format("Execute command [ HGET {0} {1}  ] success\n",
        key, hkey);
    freeReplyObject(reply);
    _conPool->ReturnConnection(connect);
    return value;
}

bool RedisMgr::Delete(const std::string& key)
{
    auto* connect = _conPool->GetConnection();
    if (connect == nullptr)
    {
        return false;
    }
    redisReply* reply = (redisReply*)redisCommand(connect, "DEL %s", key.c_str());
    if (reply == NULL || reply->type != REDIS_REPLY_INTEGER)
    {
        std::cout << std::format("Execute command [ DEL {} ] failed\n", key);
        freeReplyObject(reply);
        _conPool->ReturnConnection(connect);
        return false;
    }
    std::cout << std::format("Execute command [ DEL {} ] success\n", key);
    freeReplyObject(reply);
    _conPool->ReturnConnection(connect);
    return true;
}

bool RedisMgr::ExistKey(const std::string& key)
{
    auto* connect = _conPool->GetConnection();
    if (connect == nullptr)
    {
        return false;
    }
    redisReply* reply = (redisReply*)redisCommand(connect, "EXISTS %s", key.c_str());
    if (reply == NULL || reply->type != REDIS_REPLY_INTEGER || reply->integer == 0)
    {
        std::cout << std::format("Not Found [ KEY {} ]\n", key);
        freeReplyObject(reply);
        _conPool->ReturnConnection(connect);
        return false;
    }

    std::cout << std::format("Found [ KEY {} ] exists\n", key);
    freeReplyObject(reply);
    _conPool->ReturnConnection(connect);
    return true;
}

void RedisMgr::Close()
{
    _conPool->Close();
}
