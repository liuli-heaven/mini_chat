#ifndef HTTPMGR_H
#define HTTPMGR_H
#include"singleton.h"
#include<QString>
#include<QUrl>
#include<QObject>
#include<QtNetwork/QNetworkAccessManager>
#include"global.h"
#include<memory>
#include<QJsonDocument>
#include<QJsonObject>

class HttpMgr :public QObject, public Singleton<HttpMgr>,
            public std::enable_shared_from_this<HttpMgr>
{
    Q_OBJECT

public:
    ~HttpMgr();
    //参数分别为：使用http发送请求所需要的url,请求的数据,请求的id, 哪个模块发出的请求。
    void PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod);

private slots:
    void slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);
private:
    HttpMgr();
    friend class Singleton<HttpMgr>;
    QNetworkAccessManager _manager;

signals:
    void sig_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);
    void sig_reg_mod_finish(ReqId, QString, ErrorCodes);
    void sig_reset_mod_finish(ReqId, QString, ErrorCodes);
    void sig_login_mod_finish(ReqId, QString, ErrorCodes);
};


#endif // HTTPMGR_H
