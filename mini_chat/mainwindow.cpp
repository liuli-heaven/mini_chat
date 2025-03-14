#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "logindialog.h"
#include "registerdialog.h"
#include "resetdialog.h"
#include "chatdialog.h"
#include "tcpmgr.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initLogin();
    login_dlg_->show();
    emit TcpMgr::GetInstance()->sig_swich_chat();

}

MainWindow::~MainWindow()
{
    delete ui;
    if(login_dlg_)
    {
        delete login_dlg_;
        login_dlg_ = nullptr;
    }

    if(reg_dlg_)
    {
        delete reg_dlg_;
        reg_dlg_ = nullptr;
    }
}

void MainWindow::initLogin()
{
    login_dlg_ = new LoginDialog(this);
    login_dlg_->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);

    connect(login_dlg_, &LoginDialog::switchReg, this, &MainWindow::SlotSwitchReg);
    connect(login_dlg_, &LoginDialog::switchReset, this, &MainWindow::SlotSwitchReset);
    setCentralWidget(login_dlg_);

    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_swich_chat, this, &MainWindow::SlotSwitchChat);
}

void MainWindow::initRegister()
{
    reg_dlg_ = new RegisterDialog(this);
    reg_dlg_->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);

    connect(reg_dlg_, &RegisterDialog::switchLogin, this, &MainWindow::SlotSwitchLogin);
    setCentralWidget(reg_dlg_);
}

void MainWindow::initReset()
{
    reset_dlg_ = new ResetDialog(this);
    reset_dlg_->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);

    connect(reset_dlg_, &ResetDialog::switchLogin, this, &MainWindow::SlotSwitchLogin2);
    setCentralWidget(reset_dlg_);
}

void MainWindow::initChat()
{
    chat_dlg_ = new ChatDialog(this);
    chat_dlg_->setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);

    setCentralWidget(chat_dlg_);
}

void MainWindow::SlotSwitchReg()
{
    initRegister();
    login_dlg_->hide();
    reg_dlg_->show();
}

void MainWindow::SlotSwitchLogin()
{
    initLogin();
    reg_dlg_->hide();
    login_dlg_->show();
}

void MainWindow::SlotSwitchReset()
{
    initReset();
    login_dlg_->hide();
    reset_dlg_->show();
}

void MainWindow::SlotSwitchLogin2()
{
    initLogin();
    reset_dlg_->hide();
    login_dlg_->show();
}

void MainWindow::SlotSwitchChat()
{
    initChat();
    login_dlg_->hide();
    chat_dlg_->show();
    this->setMinimumSize(QSize(1050, 900));
    this->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
}
