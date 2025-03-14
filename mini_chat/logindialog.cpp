#include "logindialog.h"
#include "ui_logindialog.h"
#include "QRegularExpression"
#include "QJsonObject"
#include "httpmgr.h"
#include "tcpmgr.h"
LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    ui->password_edit->setEchoMode(QLineEdit::Password);

    //设置浮动显示手形状
    ui->password_visible->setCursor(Qt::PointingHandCursor);
    //设置匹配图案
    ui->password_visible->setState("unvisible", "unvisible_hover", "",
                                   "visible", "visible_hover", "");
    //设置密码可见与不可见
    connect(ui->password_visible, &ClickedLabel::clicked, this, [this](){
        auto state = ui->password_visible->getCurState();
        if(state == LabelState::Normal)
        {
            ui->password_edit->setEchoMode(QLineEdit::Password);
            qDebug() << "password can be seen";
        }
        else
        {
            ui->password_edit->setEchoMode(QLineEdit::Normal);
            qDebug() << "password can not be seen";
        }

    });
    initHandlers();

    connect(ui->register_btn, &QPushButton::clicked, this, &LoginDialog::switchReg);
    connect(ui->user_edit, &QLineEdit::editingFinished, this, &LoginDialog::checkUserValid);
    connect(ui->password_edit, &QLineEdit::editingFinished, this, &LoginDialog::checkPasswordValid);
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_login_mod_finish, this, &LoginDialog::slot_login_mod_finish);
    connect(this, &LoginDialog::sig_tcp_connect, TcpMgr::GetInstance().get(), &TcpMgr::slot_tcp_connect);
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_tcp_connect_success, this, &LoginDialog::slot_tcp_connect_success);
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_login_failed, this, &LoginDialog::slot_login_failed);


}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::showTip(const QString& str, bool is_err)
{
    if(is_err)
    {
        ui->err_tip->setProperty("state", "err");
    }
    else
    {
        ui->err_tip->setProperty("state", "normal");
    }

    ui->err_tip->setText(str);
    repolish(ui->err_tip);
}



void LoginDialog::on_forget_password_btn_clicked()
{
    emit switchReset();
}


void LoginDialog::on_login_btn_clicked()
{
    if(!checkUserValid())
    {
        return;
    }
    if(!checkPasswordValid())
    {
        return;
    }
    //获取用户名和密码
    auto user = ui->user_edit->text();
    auto password = ui->password_edit->text();
    //构造http请求。
    QJsonObject json;
    json["user"] = user;
    json["password"] = md5Encrypt(password);
    HttpMgr::GetInstance()->PostHttpReq(gate_url_prefix + "/user_login", json,
                                        ReqId::ID_LOGIN_USER, Modules::LOGINMOD);
}

void LoginDialog::slot_login_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    //验证错误信息
    if(err != ErrorCodes::SUCCESS)
    {
        showTip(tr("网络请求错误"), true);
        return;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    // json解析错误
    if(jsonDoc.isNull())
    {
        showTip(tr("json解析错误"), true);
        return;
    }
    if(!jsonDoc.isObject())
    {
        showTip(tr("json解析错误"), true);
        return;
    }
    QJsonObject jsonObj = jsonDoc.object();
    handlers_[id](jsonObj);
}

void LoginDialog::slot_tcp_connect_success(bool ok)
{
    if(ok)
    {
        showTip("聊天服务器连接成功，正在登录...", false);
        QJsonObject jsonObj;
        jsonObj["uid"] = uid_;
        jsonObj["token"] = token_;

        QJsonDocument jsonDoc(jsonObj);
        QString jsonStr = jsonDoc.toJson(QJsonDocument::Indented);
        //发送tcp请求
        TcpMgr::GetInstance()->sig_send_data(ReqId::ID_CHAT_LOGIN, jsonStr);
    }
    else
    {
        showTip(tr("网络异常"), true);

    }
}

void LoginDialog::slot_login_failed(ErrorCodes err)
{
    QString result = QString("登录失败, err is %1").arg(static_cast<int>(err));
    showTip(result,true);
    enableBtn(true);
}

void LoginDialog::addErr(TipErr tip_err, const QString &err_str)
{
    tip_errs_[tip_err] = err_str;
    showTip(err_str, true);
}

void LoginDialog::delErr(TipErr tip_err)
{
    tip_errs_.remove(tip_err);
    if(tip_errs_.empty())
    {
        ui->err_tip->clear();
    }
    else
    {
        showTip(tip_errs_.first(), true);
    }
}

bool LoginDialog::checkUserValid()
{
    if(ui->user_edit->text() == "")
    {
        addErr(TipErr::TIP_USER_ERR, "用户名不能为空");
        return false;
    }
    delErr(TipErr::TIP_USER_ERR);
    return true;
}

bool LoginDialog::checkPasswordValid()
{
    auto password = ui->password_edit->text();

    if(password.length() > 15 || password.length() < 6)
    {
        addErr(TipErr::TIP_PWD_ERR, "密码长度需为6-15位");
        return false;
    }

    delErr(TipErr::TIP_PWD_ERR);
    return true;
}

void LoginDialog::initHandlers()
{
    handlers_.insert(ReqId::ID_LOGIN_USER, [this](const QJsonObject& jsonObj){
        auto error = jsonObj["error"].toInt();
        bool is_ok = checkError(static_cast<ErrorCodes>(error));
        if(!is_ok)
        {
            showTip("登录失败", true);
            return;
        }
        showTip("登录成功", false);
        auto user = jsonObj["user"].toString();
        //已确认登录成功，则开始建立tcp连接
        ServerInfo server;

        server.uid = jsonObj["uid"].toInt();
        server.host = jsonObj["host"].toString();
        server.port = jsonObj["port"].toString();
        server.token = jsonObj["token"].toString();

        qDebug() << "uid is:" << server.uid << "\nhost is:" << server.host <<
            "\nport is:" << server.port << "\ntoken is:" << server.token;
        uid_ = server.uid;
        token_ = server.token;

        emit sig_tcp_connect(server);

    });
}

void LoginDialog::enableBtn(bool)
{

}

