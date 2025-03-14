#pragma once
#include "Singleton.h"
#include "MysqlDAO.h"
class MysqlMgr : public  Singleton<MysqlMgr>
{
	friend class Singleton<MysqlMgr>;
public:
	~MysqlMgr();
	int RegUser(const std::string& name, const std::string& email, const std::string& password);
	bool CheckEmail(const std::string& name, const std::string& email);
	bool UpdatePassword(const std::string& name, const std::string& password);
	bool CheckPassword(const std::string& name, const std::string& password, UserInfo& userInfo);
	std::shared_ptr<UserInfo> GetUser(short uid);
private:
	MysqlMgr();
	MysqlDAO mysql_dao_;
};

