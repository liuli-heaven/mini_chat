#include "httpmgr.h"
#include<QtNetwork/QNetworkReply>
void HttpMgr::PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod)
{
    /*用于创建一个HTTP POST请求*/

    //将要请求的数据序列化
    QByteArray data = QJsonDocument(json).toJson();
    //通过url构造请求
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));
    //发送请求， 并处理相应，获取自己的智能指针，构造伪闭包并增加智能指针引用计数。
    auto self = shared_from_this();
    QNetworkReply* reply = _manager.post(request, data);
    QObject::connect(reply, &QNetworkReply::finished, [reply, self, req_id, mod](){
        //处理错误的情况
        if(reply->error() != QNetworkReply::NoError)
        {
            //输出错误信息
            qDebug() << reply->errorString();
            //发送信号通知异常
            emit self->sig_http_finish(req_id, "", ErrorCodes::ERR_NETWORK, mod);
            reply->deleteLater();
            return;
        }
        //没有错误则读回请求
        QString res = reply->readAll();

        //发送信号通知完成
        emit self->sig_http_finish(req_id, res, ErrorCodes::SUCCESS, mod);
        reply->deleteLater();
        return;
    });
}

void HttpMgr::slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod)
{
    //发送信号到对应模块，通知http响应结束。
    if(mod == Modules::REGISTERMOD)
    {
        emit sig_reg_mod_finish(id, res, err);
        return;
    }
    if(mod == Modules::RESETMOD)
    {
        emit sig_reset_mod_finish(id, res, err);
        return;
    }
    if(mod == Modules::LOGINMOD)
    {
        emit sig_login_mod_finish(id, res, err);
        return;
    }

}

HttpMgr::HttpMgr()
{
    connect(this, &HttpMgr::sig_http_finish, this, &HttpMgr::slot_http_finish);
}

HttpMgr::~HttpMgr()
{

}

