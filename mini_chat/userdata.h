#ifndef USERDATA_H
#define USERDATA_H

#include "QString"

class SearchInfo
{
public:
    SearchInfo(int uid, QString name, QString nick, QString desc, int sex);

    int uid_;
    QString name_;
    QString nick_;
    QString desc_;
    int sex_;
};

class AddFriendApply
{
public:
    AddFriendApply(int from_uid, QString name, QString desc);
    int from_uid_;
    QString name_;
    QString desc_;
};

struct ApplyInfo
{
    ApplyInfo(int uid, QString name, QString desc, QString icon, QString nick, int sex, int status);
    void setIcon(QString head);
    int uid_;
    QString name_;
    QString desc_;
    QString icon_;
    QString nick_;
    int sex_;
    int status_;
};

struct AuthInfo
{
    AuthInfo(int uid, QString name, QString nick, QString icon, int sex);
    int uid_;
    QString name_;
    QString nick_;
    QString icon_;
    int sex_;
};

struct AuthRsp
{
    AuthRsp(int peer_uid, QString peer_name, QString peer_nick, QString peer_icon, int peer_sex);
    int uid_;
    QString name_;
    QString nick_;
    QString icon_;
    int sex_;
};

struct UserInfo{
    UserInfo(int uid, QString name, QString nick, QString icon, int sex);
    UserInfo(std::shared_ptr<AuthInfo> auth);
    UserInfo(int uid, QString name, QString icon);
    UserInfo(std::shared_ptr<AuthRsp> auth);
    int uid_;
    QString name_;
    QString nick_;
    QString icon_;
    int sex_;
};

#endif // USERDATA_H
