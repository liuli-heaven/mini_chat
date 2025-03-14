#ifndef RESETDIALOG_H
#define RESETDIALOG_H
#include "global.h"
#include <QDialog>

namespace Ui {
class ResetDialog;
}

class ResetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResetDialog(QWidget *parent = nullptr);
    ~ResetDialog();

signals:
    void switchLogin();

private slots:
    void on_cancel_btn_clicked();
    void on_verifyCode_btn_clicked();
    void slot_reset_mod_finish(ReqId, QString, ErrorCodes);
    void on_confirm_btn_clicked();

private:
    Ui::ResetDialog *ui;
    void showTip(QString, bool is_err);
    QMap<ReqId, std::function<void(const QJsonObject&)>> handlers_;
    QMap<TipErr, QString> tip_errs_;

private:
    void initHttpHandler();

    //错误提示函数
    void addErr(TipErr tip_err, QString err_str);
    void delErr(TipErr tip_err);
    bool checkUserValid();
    bool checkEmailValid();
    bool checkPasswordValid();
    bool checkVerifyValid();

};

#endif // RESETDIALOG_H
