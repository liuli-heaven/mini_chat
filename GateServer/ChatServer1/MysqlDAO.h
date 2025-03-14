#pragma once
#include <memory>
#include <string>
#include "MysqlPool.h"
#include "const.h"


class MysqlDAO
{
public:
	MysqlDAO();
	~MysqlDAO();
	int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
	bool CheckEmail(const std::string& name, const std::string& email);
	bool UpdatePassword(const std::string& name, const std::string& password);
	bool CheckPassword(const std::string& name, const std::string& password, UserInfo& userInfo);
	std::shared_ptr<UserInfo> GetUser(short uid);
private:
	std::unique_ptr<MysqlPool> pool_;
};

