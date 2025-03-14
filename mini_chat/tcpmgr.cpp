#include "tcpmgr.h"
#include"QJsonDocument"
#include"QJsonObject"
#include"usermgr.h"
#include"logindialog.h"
TcpMgr::~TcpMgr()
{

}

TcpMgr::TcpMgr():host_(""), port_(0), b_recv_pending_(false), message_id_(0), message_len_(0)
{
    QObject::connect(&socket_, &QTcpSocket::connected, [&](){
        qDebug() << "Connected to server";
        //发送连接建立成功的消息
        emit sig_tcp_connect_success(true);
    });
    QObject::connect(&socket_, &QTcpSocket::readyRead, [&](){
        //socket内所有数据可读时，将数据移至缓冲区
        buffer_.append(socket_.readAll());
        QDataStream stream(&buffer_, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_6_6);
        forever{
            //如果已经停止接收，则进行处理
            if(!b_recv_pending_)
            {
                //如果长度不足以解析出消息id和消息长度，则继续接收。
                if(buffer_.size() < static_cast<int>(sizeof(qint16) * 2))
                {
                    b_recv_pending_ = true;
                    return;
                }
                //读取消息id和长度
                stream >> message_id_ >> message_len_;
                //buffer中除去已读取数据
                buffer_ = buffer_.mid(sizeof(qint16) * 2);

                qDebug() << "received message_id is:" << message_id_ << " message_len is:" << message_len_;
            }

            //如果buffer的大小比message_len小，代表消息不完整，继续读取
            if(buffer_.size() < message_len_)
            {
                b_recv_pending_ = true;
                return;
            }
            b_recv_pending_ = false;

            QByteArray body = buffer_.mid(0, message_len_);
            buffer_ = buffer_.mid(message_len_);
            qDebug() << "recevie message is:" << body;
        }
    });
    //错误处理
    QObject::connect(&socket_, &QTcpSocket::errorOccurred, [&]() {
        qDebug() << "Error:" << socket_.errorString();
    });

    QObject::connect(&socket_, &QTcpSocket::disconnected, [&](){
        qDebug() << "disconnect from server.";
    });

    QObject::connect(this, &TcpMgr::sig_send_data, this, &TcpMgr::slot_send_data);

}

void TcpMgr::initHandler()
{
    handlers_.insert(ReqId::ID_CHAT_LOGIN_RSP, [this](ReqId id, int len, QByteArray data){
        qDebug() << "handle id is: " << static_cast<int>(id) << " data is: " << data;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if(jsonDoc.isNull())
        {
            qDebug() << "fail to create jsonDocument";
            return;
        }
        if(!jsonDoc.isObject())
        {
            qDebug() << "daat is not a json object";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        ErrorCodes err;
        int err_int;
        if(!jsonObj.contains("error"))
        {
            err = ErrorCodes::ERR_JSON;
            qDebug() << "login failed, error is json prase error";
            emit sig_login_failed(err);
            return;
        }
        err_int = jsonObj["error"].toInt();
        err = static_cast<ErrorCodes>(err_int);
        if(err != ErrorCodes::SUCCESS)
        {
            qDebug() << "login failed, error code is: " << err_int;
            emit sig_login_failed(err);
            return;
        }
        UserMgr::GetInstance()->setUid(jsonObj["uid"].toInt());
        UserMgr::GetInstance()->setName(jsonObj["name"].toString());
        UserMgr::GetInstance()->setToken(jsonObj["token"].toString());
        emit sig_swich_chat();
    });
}

void TcpMgr::dealMsg(ReqId id, int len, QByteArray data)
{
    auto find_iter = handlers_.find(id);
    if(find_iter == handlers_.end())
    {
        qDebug() << "not found id[" << static_cast<int>(id) << "] to handle";
        return;
    }
    find_iter.value()(id, len, data);
}

void TcpMgr::slot_tcp_connect(ServerInfo server)
{
    qDebug()<< "receive tcp connect signal";
    // 尝试连接到服务器
    qDebug() << "Connecting to server...";
    host_ = server.host;
    port_ = static_cast<uint16_t>(server.port.toUInt());
    socket_.connectToHost(host_, port_);
}

void TcpMgr::slot_send_data(ReqId id, QString data)
{
    quint16 data_id = static_cast<quint16>(id);
    QByteArray data_body = data.toUtf8();
    quint16 len = static_cast<quint16>(data_body.size());
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out.setByteOrder(QDataStream::BigEndian);
    out << id << len;
    block.append(data_body);

    socket_.write(block);
}
