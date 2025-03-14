#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "global.h"
#include <QRegularExpression>
#include "httpmgr.h"
RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);

    ui->confirm_edit->setEchoMode(QLineEdit::Password);
    ui->password_edit->setEchoMode(QLineEdit::Password);

    ui->err_tip->setProperty("state","normal");
    repolish(ui->err_tip);
    ui->verification_get_btn->setEmailLine(ui->email_edit);
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reg_mod_finish, this, &RegisterDialog::slot_reg_mod_finish);
    InitHttpHandlers();

    //设置错误提示逻辑
    ui->err_tip->clear();
    connect(ui->user_edit, &QLineEdit::editingFinished, this, &RegisterDialog::checkUserValid);
    connect(ui->email_edit, &QLineEdit::editingFinished, this, &RegisterDialog::checkEmailValid);
    connect(ui->password_edit, &QLineEdit::editingFinished, this, &RegisterDialog::checkPasswordValid);
    connect(ui->confirm_edit, &QLineEdit::editingFinished, this, &RegisterDialog::checkConfirmValid);
    connect(ui->verification_edit, &QLineEdit::editingFinished, this, &RegisterDialog::checkVerifyValid);

    //设置浮动显示手形状
    ui->password_visible->setCursor(Qt::PointingHandCursor);
    ui->confirm_visible->setCursor(Qt::PointingHandCursor);

    ui->password_visible->setState("unvisible", "unvisible_hover", "",
                                  "visible", "visible_hover", "");
    ui->confirm_visible->setState("unvisible", "unvisible_hover", "",
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
    connect(ui->confirm_visible, &ClickedLabel::clicked, this, [this](){
        auto state = ui->confirm_visible->getCurState();
        if(state == LabelState::Normal)
        {
            ui->confirm_edit->setEchoMode(QLineEdit::Password);
            qDebug() << "password can be seen";
        }
        else
        {
            ui->confirm_edit->setEchoMode(QLineEdit::Normal);
            qDebug() << "password can not be seen";
        }

    });

    page2_timer = new QTimer(this);

    connect(page2_timer, &QTimer::timeout, [this](){
        if(page2_wait_count <= 0)
        {
            page2_timer->stop();
            emit switchLogin();
            return;
        }
        auto str = QString("注册成功，%1 s后返回登录").arg(page2_wait_count);
        page2_wait_count--;
        ui->page2_tip_edit->setText(str);
    });
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::InitHttpHandlers()
{
    //注册获取验证码回包逻辑
    handlers_.insert(ReqId::ID_GET_VARIFY_CODE, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if (static_cast<ErrorCodes>(error) != ErrorCodes::SUCCESS)
        {
            showTip(tr("参数错误"), true);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("验证码已发送到邮箱, 请注意查收"), false);
        qDebug() << "email is " << email;
    });
    //注册用户注册回包逻辑
    handlers_.insert(ReqId::ID_REG_USER, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if (static_cast<ErrorCodes>(error) != ErrorCodes::SUCCESS)
        {
            showTip(tr("注册失败"), true);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("注册成功"), false);
        qDebug() << "email is " << email;
        changeTipPage();
    });
}

void RegisterDialog::on_verification_get_btn_clicked()
{
    //获取填写邮箱地址
    auto email = ui->email_edit->text();
    //邮箱地址的正则表达式
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch();

    if(match)
    {
        //发送http请求获取验证码
        QJsonObject json_obj;
        json_obj["email"] = email;
        HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/get_varifycode"),
                                            json_obj, ReqId::ID_GET_VARIFY_CODE, Modules::REGISTERMOD);
    }
    else
    {
        showTip(tr("邮箱地址不正确"),true);
    }
}

void RegisterDialog::showTip(QString str, bool is_err)
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

void RegisterDialog::addErr(TipErr tip_err, QString err_str)
{
    tip_errs_[tip_err] = err_str;
    showTip(err_str, true);
}

void RegisterDialog::delErr(TipErr tip_err)
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

void RegisterDialog::setHandlers(const QMap<ReqId, std::function<void (const QJsonObject &)> > &newHandlers)
{
    handlers_ = newHandlers;
}

bool RegisterDialog::checkUserValid()
{
    if(ui->user_edit->text() == "")
    {
        addErr(TipErr::TIP_USER_ERR, "用户名不能为空");
        return false;
    }
    delErr(TipErr::TIP_USER_ERR);
    return true;
}

bool RegisterDialog::checkEmailValid()
{
    auto email = ui->email_edit->text();

    QRegularExpression regex(R"((\w+)(.\_)?(\w*)@(\w*)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch();
    if(match)
    {
        delErr(TipErr::TIP_EMAIL_ERR);
        return true;
    }
    else
    {
        addErr(TipErr::TIP_EMAIL_ERR, "邮箱地址不正确");
        return false;
    }
}

bool RegisterDialog::checkPasswordValid()
{
    auto password = ui->password_edit->text();

    if(password.length() > 15 || password.length() < 6)
    {
        addErr(TipErr::TIP_PWD_ERR, "密码长度需为6-15位");
        return false;
    }
    QRegularExpression regex(R"(^[a-zA-Z0-9_]{6,15}$)");
    bool match = regex.match(password).hasMatch();
    if(!match)
    {
        addErr(TipErr::TIP_PWD_ERR, "密码必须为数字，字母，_的组合");
        return false;
    }
    delErr(TipErr::TIP_PWD_ERR);
    return true;
}

bool RegisterDialog::checkConfirmValid()
{
    auto password = ui->password_edit->text();
    auto confirm = ui->confirm_edit->text();
    if(confirm != password)
    {
        addErr(TipErr::TIP_CONFIRM_ERR, "两次输入密码不匹配");
        return false;
    }
    delErr(TipErr::TIP_CONFIRM_ERR);
    return true;
}

bool RegisterDialog::checkVerifyValid()
{
    auto verify = ui->verification_edit->text();
    if(verify == "")
    {
        addErr(TipErr::TIP_VERIFY_ERR, "验证码不能为空");
        return false;
    }
    delErr(TipErr::TIP_VERIFY_ERR);
    return true;
}

void RegisterDialog::changeTipPage()
{
    page2_timer->stop();
    ui->stackedWidget->setCurrentWidget(ui->page_2);
    this->page2_wait_count = 5;
    page2_timer->start(1000);

}

void RegisterDialog::slot_reg_mod_finish(ReqId id,QString res, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS)
    {
        showTip(tr("网络请求错误"), true);
    }

    // 解析json字符串，res需转化为QByteArray
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
    //根据id调用注册的对应逻辑
    handlers_[id](jsonObj);
    return;
}

void RegisterDialog::on_confirm_btn_clicked()
{
    if(!checkUserValid())
    {
        return;
    }
    if(!checkEmailValid())
    {
        return;
    }
    if(!checkPasswordValid())
    {
        return;
    }
    if(!checkConfirmValid())
    {
        return;
    }
    if(!checkVerifyValid())
    {
        return;
    }

    //做完基本校验之后，发送数据包进行验证
    QJsonObject json_obj;
    json_obj["user"] = ui->user_edit->text();
    json_obj["email"] = ui->email_edit->text();
    json_obj["password"] = md5Encrypt(ui->password_edit->text());
    json_obj["verify_code"] = ui->verification_edit->text();
    HttpMgr::GetInstance()->PostHttpReq(gate_url_prefix + "/user_register",
                                        json_obj, ReqId::ID_REG_USER, Modules::REGISTERMOD);
}


void RegisterDialog::on_page2_return_btn_clicked()
{
    page2_timer->stop();
    emit switchLogin();
}


void RegisterDialog::on_cancel_btn_clicked()
{
    if(page2_timer->isActive())
    {
        page2_timer->stop();
    }
    emit switchLogin();
}

