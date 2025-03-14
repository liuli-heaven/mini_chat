#include "userdata.h"

SearchInfo::SearchInfo(int uid, QString name, QString nick, QString desc, int sex)
    :uid_(uid), name_(name), nick_(nick), desc_(desc), sex_(sex)
{

}

AddFriendApply::AddFriendApply(int from_uid, QString name, QString desc)
    :from_uid_(from_uid), name_(name), desc_(desc)
{

}

ApplyInfo::ApplyInfo(int uid, QString name, QString desc, QString icon, QString nick, int sex, int status)
    :uid_(uid), name_(name), desc_(desc), icon_(icon), nick_(nick), sex_(sex), status_(status)
{

}

void ApplyInfo::setIcon(QString head)
{
    icon_ = head;
}

AuthInfo::AuthInfo(int uid, QString name, QString nick, QString icon, int sex)
    :uid_(uid), name_(name), nick_(nick), icon_(icon), sex_(sex)
{

}

AuthRsp::AuthRsp(int peer_uid, QString peer_name, QString peer_nick, QString peer_icon, int peer_sex)
    :uid_(peer_uid), name_(peer_name), nick_(peer_nick), icon_(peer_icon), sex_(peer_sex)
{

}

UserInfo::UserInfo(int uid, QString name, QString nick, QString icon, int sex)
    :uid_(uid), name_(name), nick_(nick), icon_(icon), sex_(sex)
{

}

UserInfo::UserInfo(std::shared_ptr<AuthInfo> auth):uid_(auth->uid_),
    name_(auth->name_), nick_(auth->nick_), icon_(auth->icon_), sex_(auth->sex_)
{

}

UserInfo::UserInfo(int uid, QString name, QString icon):UserInfo(uid, name, name, icon, 0)
{

}

UserInfo::UserInfo(std::shared_ptr<AuthRsp> auth):uid_(auth->uid_),
    name_(auth->name_), nick_(auth->nick_), icon_(auth->icon_), sex_(auth->sex_)
{

}
