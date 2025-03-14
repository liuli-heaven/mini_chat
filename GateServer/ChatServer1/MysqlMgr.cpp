#include "MysqlMgr.h"

MysqlMgr::MysqlMgr()
{
	
}

MysqlMgr::~MysqlMgr()
{

}

int MysqlMgr::RegUser(const std::string& name, const std::string& email, const std::string& password)
{
	return mysql_dao_.RegUser(name, email, password);
}

bool MysqlMgr::CheckEmail(const std::string& name, const std::string& email)
{
	return mysql_dao_.CheckEmail(name, email);
}

bool MysqlMgr::UpdatePassword(const std::string& name, const std::string& password)
{
	return mysql_dao_.UpdatePassword(name, password);
}

bool MysqlMgr::CheckPassword(const std::string& name, const std::string& password, UserInfo& userInfo)
{
	return mysql_dao_.CheckPassword(name, password, userInfo);
}

std::shared_ptr<UserInfo> MysqlMgr::GetUser(short uid)
{
	return mysql_dao_.GetUser(uid);
}


