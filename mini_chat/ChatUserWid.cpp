#include "ChatUserWid.h"
#include "ui_ChatUserWid.h"

ChatUserWid::ChatUserWid(QWidget *parent)
    : ListItemBase(parent)
    , ui(new Ui::ChatUserWid)
{
    ui->setupUi(this);
    SetItemType(ListItemType::CHAT_USER_ITEM);
}

ChatUserWid::~ChatUserWid()
{
    delete ui;
}

QSize ChatUserWid::sizeHint() const
{
    return QSize(250, 70);//返回自定义的高宽。
}

void ChatUserWid::setInfo(QString name, QString head, QString msg)
{
    name_ = name;
    head_ = head;
    msg_ = msg;
    //加载图片
    QPixmap pixmap(head_);
    //设置图片自动播放
    ui->icon_label->setPixmap(pixmap.scaled(ui->icon_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_label->setScaledContents(true);

    ui->user_name_label->setText(name_); //设置名称
    ui->user_data_label->setText(msg_); //设置最新记录
}

void ChatUserWid::setTime(QTime time)
{
    QString time_str = time.toString("HH:mm");
    ui->time_label->setText(time_str);
}
