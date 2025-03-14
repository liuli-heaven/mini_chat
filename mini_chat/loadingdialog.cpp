#include "loadingdialog.h"
#include "ui_loadingdialog.h"
#include "QMovie"

LoadingDialog::LoadingDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoadingDialog)
{
    ui->setupUi(this);
    //设置为没有菜单，没有边框，始终在最上层的对话框
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);//将背景设置为透明
    //设置为全屏
    setFixedSize(parent->size());
    //加载动画文件
    QMovie* movie = new QMovie(":/icons/icon/loading.gif");
    this->ui->loading_label->setMovie(movie);
    movie->start();
}

LoadingDialog::~LoadingDialog()
{
    delete ui;
}
