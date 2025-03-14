#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H
#include "global.h"
#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();
    void showTip(const QString& err, bool is_err);

signals:
    void switchReg();
    void switchReset();
    void sig_tcp_connect(ServerInfo server);

private slots:
    void on_forget_password_btn_clicked();
    void on_login_btn_clicked();
    void slot_login_mod_finish(ReqId, QString, ErrorCodes);
    void slot_tcp_connect_success(bool);
    void slot_login_failed(ErrorCodes);
private:
    Ui::LoginDialog *ui;
    QMap<TipErr, QString> tip_errs_;
    QMap<ReqId, std::function<void(const QJsonObject&)>> handlers_;
    int uid_;
    QString token_;
private:
    void addErr(TipErr tip_err, const QString &err_str);
    void delErr(TipErr tip_err);
    bool checkUserValid();
    bool checkPasswordValid();

    void initHandlers();
    void enableBtn(bool);
};


#endif // LOGINDIALOG_H
