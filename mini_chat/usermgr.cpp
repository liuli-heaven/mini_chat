#include "usermgr.h"

UserMgr::~UserMgr()
{

}

void UserMgr::setName(QString name)
{
    name_ = name;
}

void UserMgr::setUid(int uid)
{
    uid_ = uid;
}

void UserMgr::setToken(QString token)
{
    token_ = token;
}

QString UserMgr::getName()
{
    return name_;
}

UserMgr::UserMgr() {}
