#include "contactuseritem.h"
#include "ui_contactuseritem.h"


ContactUserItem::ContactUserItem(QWidget *parent):
    ListItemBase(parent),
    ui(new Ui::ContactUserItem)
{
    ui->setupUi(this);
    this->SetItemType(ListItemType::CONTACT_USER_ITEM);
    ui->red_point->raise();
    showRedPoint(false);
}

ContactUserItem::~ContactUserItem()
{
    delete ui;
}

void ContactUserItem::setInfo(std::shared_ptr<AuthInfo> auth)
{
    info_ = std::make_shared<UserInfo>(auth);
    //加载图片
    QPixmap pixmap(auth->icon_);

    //设置图片自动缩放
    ui->icon_label->setPixmap(pixmap.scaled(ui->icon_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_label->setScaledContents(true);

    ui->user_name_label->setText(auth->name_);
}

void ContactUserItem::setInfo(std::shared_ptr<AuthRsp> auth)
{
    info_ = std::make_shared<UserInfo>(auth);
    //加载图片
    QPixmap pixmap(auth->icon_);

    //设置图片自动缩放
    ui->icon_label->setPixmap(pixmap.scaled(ui->icon_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_label->setScaledContents(true);

    ui->user_name_label->setText(auth->name_);
}

void ContactUserItem::setInfo(int uid, QString name, QString icon)
{
    info_ = std::make_shared<UserInfo>(uid, name, icon);
    //加载图片
    QPixmap pixmap(icon);

    //设置图片自动缩放
    ui->icon_label->setPixmap(pixmap.scaled(ui->icon_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_label->setScaledContents(true);

    ui->user_name_label->setText(name);
}

void ContactUserItem::showRedPoint(bool bShow)
{
    if(bShow)
    {
        ui->red_point->show();
    }
    else
    {
        ui->red_point->hide();
    }
}

QSize ContactUserItem::sizeHint() const
{
    return QSize(250, 70);
}
