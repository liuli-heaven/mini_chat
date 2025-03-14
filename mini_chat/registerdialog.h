#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H
#include"httpmgr.h"
#include "global.h"
#include <QDialog>
#include <QTimer>
namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();
    void InitHttpHandlers();

    void setHandlers(const QMap<ReqId, std::function<void (const QJsonObject &)> > &newHandlers);

private slots:
    void on_verification_get_btn_clicked();
    void slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
    void on_confirm_btn_clicked();

    void on_page2_return_btn_clicked();

    void on_cancel_btn_clicked();

private:
    Ui::RegisterDialog *ui;
    void showTip(QString, bool is_err);
    QMap<ReqId, std::function<void(const QJsonObject&)>> handlers_;
    QMap<TipErr, QString> tip_errs_;
    QTimer* page2_timer;
    int page2_wait_count = 5;
private:
    //错误提示函数
    void addErr(TipErr tip_err, QString err_str);
    void delErr(TipErr tip_err);
    bool checkUserValid();
    bool checkEmailValid();
    bool checkPasswordValid();
    bool checkConfirmValid();
    bool checkVerifyValid();

    void changeTipPage();

signals:
    void switchLogin();
};

#endif // REGISTERDIALOG_H
