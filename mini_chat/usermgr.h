#ifndef USERMGR_H
#define USERMGR_H
#include"QObject"
#include"singleton.h"
class UserMgr:public QObject, public Singleton<UserMgr>
{
    Q_OBJECT
    friend class Singleton<UserMgr>;
public:
    ~UserMgr();
    void setName(QString name);
    void setUid(int uid);
    void setToken(QString token);
    QString getName();
private:
    UserMgr();
private:
    QString name_;
    QString token_;
    int uid_;
};

#endif // USERMGR_H
