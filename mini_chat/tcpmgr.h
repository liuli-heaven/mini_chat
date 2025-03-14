#ifndef TCPMGR_H
#define TCPMGR_H
#include "QTcpSocket"
#include "singleton.h"
#include "global.h"
#include "userdata.h"
class TcpMgr : public QObject, public Singleton<TcpMgr>, public std::enable_shared_from_this<TcpMgr>
{
    friend class Singleton<TcpMgr>;
    Q_OBJECT
public:
    ~TcpMgr();
    TcpMgr(const TcpMgr&) = delete;
    TcpMgr& operator=(const TcpMgr&) = delete;
protected:
    TcpMgr();
    void initHandler();
    void dealMsg(ReqId id, int len, QByteArray data);
private:
    QTcpSocket socket_;
    QString host_;
    uint16_t port_;
    QByteArray buffer_;
    bool b_recv_pending_;
    quint16 message_id_;
    quint16 message_len_;
    QMap<ReqId, std::function<void(ReqId, int, QByteArray)>> handlers_;
signals:
    void sig_tcp_connect_success(bool);
    void sig_send_data(ReqId id, QString data);
    void sig_login_failed(ErrorCodes);
    void sig_swich_chat();
    void sig_user_search(std::shared_ptr<SearchInfo> info);
    void sig_add_auth_friend(std::shared_ptr<AuthInfo>);
    void sig_auth_rsp(std::shared_ptr<AuthRsp>);
public slots:
    void slot_tcp_connect(ServerInfo);
    void slot_send_data(ReqId id, QString data);

};

#endif // TCPMGR_H
