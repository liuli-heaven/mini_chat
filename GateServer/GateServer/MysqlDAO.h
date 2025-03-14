#pragma once
#include <memory>
#include <string>
#include "MysqlPool.h"

//mysql查询需要的数据结构
struct UserInfo
{
	int uid;
	std::string name;
	std::string password;
	std::string email;
};

class MysqlDAO
{
public:
	MysqlDAO();
	~MysqlDAO();
	int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
	bool CheckEmail(const std::string& name, const std::string& email);
	bool UpdatePassword(const std::string& name, const std::string& password);
	bool CheckPassword(const std::string& name, const std::string& password, UserInfo& userInfo);
private:
	std::unique_ptr<MysqlPool> pool_;
};

