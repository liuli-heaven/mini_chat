#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class LoginDialog;
class RegisterDialog;
class ResetDialog;
class ChatDialog;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private slots:
    void SlotSwitchReg();
    void SlotSwitchLogin();
    void SlotSwitchReset();
    void SlotSwitchLogin2();
    void SlotSwitchChat();
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    LoginDialog* login_dlg_;
    RegisterDialog* reg_dlg_;
    ResetDialog* reset_dlg_;
    ChatDialog* chat_dlg_;
private:
    void initLogin();
    void initRegister();
    void initReset();
    void initChat();

};
#endif // MAINWINDOW_H
