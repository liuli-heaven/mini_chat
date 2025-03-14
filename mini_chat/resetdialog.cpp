#include "resetdialog.h"
#include "ui_resetdialog.h"
#include "QRegularExpression"
#include "QJsonObject"
#include "httpmgr.h"
ResetDialog::ResetDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ResetDialog)
{
    ui->setupUi(this);

    ui->err_tip->setProperty("state","normal");
    repolish(ui->err_tip);
    ui->verifyCode_btn->setEmailLine(ui->email_edit);
    //负责http请求结束后的处理和通知
    initHttpHandler();
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reset_mod_finish, this, &ResetDialog::slot_reset_mod_finish);
    //处理错误的逻辑
    ui->err_tip->clear();
    connect(ui->user_edit, &QLineEdit::editingFinished, this, &ResetDialog::checkUserValid);
    connect(ui->email_edit, &QLineEdit::editingFinished, this, &ResetDialog::checkEmailValid);
    connect(ui->newPassword_edit, &QLineEdit::editingFinished, this, &ResetDialog::checkPasswordValid);
    connect(ui->verifyCode_edit, &QLineEdit::editingFinished, this, &ResetDialog::checkVerifyValid);
}

ResetDialog::~ResetDialog()
{
    delete ui;
}

void ResetDialog::showTip(QString str, bool is_err)
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

void ResetDialog::initHttpHandler()
{
    //添加对应的回调函数。
    handlers_.insert(ReqId::ID_GET_VARIFY_CODE, [this](QJsonObject jsonObj){
        auto error = jsonObj["error"].toInt();
        bool is_ok = checkError(static_cast<ErrorCodes>(error));
        if(!is_ok)
        {
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("验证码已发送到邮箱, 请注意查收"), true);
        qDebug() << "email is " << email;
    });

    //注册用户注册回包逻辑
    handlers_.insert(ReqId::ID_RESET_PASSWORD, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        bool is_ok = checkError(static_cast<ErrorCodes>(error));
        if(!is_ok)
        {
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("密码修改成功，请返回登录。"), false);
        qDebug() << "email is " << email;
    });
}

void ResetDialog::addErr(TipErr tip_err, QString err_str)
{
    tip_errs_[tip_err] = err_str;
    showTip(err_str, true);
}

void ResetDialog::delErr(TipErr tip_err)
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

bool ResetDialog::checkUserValid()
{
    if(ui->user_edit->text() == "")
    {
        addErr(TipErr::TIP_USER_ERR, "用户名不能为空");
        return false;
    }
    delErr(TipErr::TIP_USER_ERR);
    return true;
}

bool ResetDialog::checkEmailValid()
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

bool ResetDialog::checkPasswordValid()
{
    auto password = ui->newPassword_edit->text();

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

bool ResetDialog::checkVerifyValid()
{
    auto verify = ui->verifyCode_edit->text();
    if(verify == "")
    {
        addErr(TipErr::TIP_VERIFY_ERR, "验证码不能为空");
        return false;
    }
    delErr(TipErr::TIP_VERIFY_ERR);
    return true;
}

void ResetDialog::on_cancel_btn_clicked()
{
    emit switchLogin();
}


void ResetDialog::on_verifyCode_btn_clicked()
{
    //获取填写邮箱地址
    auto email = ui->email_edit->text();
    //验证邮箱是否正确
    bool match = checkEmailValid();

    if(match)
    {
        //发送http请求获取验证码
        QJsonObject json_obj;
        json_obj["email"] = email;
        HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix + "/get_varifycode"),
                                            json_obj, ReqId::ID_GET_VARIFY_CODE, Modules::RESETMOD);
    }
    else
    {
        showTip(tr("邮箱地址不正确"),true);
    }
}

void ResetDialog::slot_reset_mod_finish(ReqId id,QString res, ErrorCodes err)
{
    if(err != ErrorCodes::SUCCESS)
    {
        showTip(tr("网络请求错误"), true);
        return;
    }
    //开始解析json格式
    QJsonDocument json = QJsonDocument::fromJson(res.toUtf8());
    if(json.isNull())
    {
        showTip(tr("Json解析错误"), true);
        return;
    }
    if(!json.isObject())
    {
        showTip(tr("数据不是json对象"), true);
        return;
    }

    QJsonObject jsonObj = json.object();
    handlers_[id](jsonObj);
    return;
}


void ResetDialog::on_confirm_btn_clicked()
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
    if(!checkVerifyValid())
    {
        return;
    }
    //做完基本校验之后，发送数据包进行验证
    QJsonObject json_obj;
    json_obj["user"] = ui->user_edit->text();
    json_obj["email"] = ui->email_edit->text();
    json_obj["password"] = md5Encrypt(ui->newPassword_edit->text());
    json_obj["verify_code"] = ui->verifyCode_edit->text();
    HttpMgr::GetInstance()->PostHttpReq(gate_url_prefix + "/reset_password",
                                        json_obj, ReqId::ID_RESET_PASSWORD, Modules::RESETMOD);
}

