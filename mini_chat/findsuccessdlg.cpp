#include "findsuccessdlg.h"
#include "ui_findsuccessdlg.h"
#include "QDir"
#include "applyfrienddlg.h"
FindSuccessDlg::FindSuccessDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FindSuccessDlg), parent_(parent)
{
    ui->setupUi(this);

    //设置对话框标题
    setWindowTitle("添加");
    //隐藏对话框标题栏
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    //获取当前应用程序的路径
    QString app_path = QCoreApplication::applicationDirPath();
    QString pix_path = QDir::toNativeSeparators(app_path + QDir::separator() +
                                                "static" + QDir::separator() + "河城荷取.png");
    QPixmap head_pix(pix_path);
    head_pix = head_pix.scaled(ui->head_label->size(),
                               Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->head_label->setPixmap(head_pix);
    ui->add_friend_btn->setState("normal", "hover", "press");
}

FindSuccessDlg::~FindSuccessDlg()
{
    delete ui;
}

void FindSuccessDlg::setSearchInfo(std::shared_ptr<SearchInfo> info)
{
    ui->name_label->setText(info->name_);
    info_ = info;
}

void FindSuccessDlg::on_add_friend_btn_clicked()
{
    this->hide();
    auto applyFriend = new ApplyFriendDlg(parent_);
    applyFriend->setSearchInfo(info_);
    applyFriend->setModal(true);
    applyFriend->show();
}

