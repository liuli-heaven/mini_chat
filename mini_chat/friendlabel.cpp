#include "friendlabel.h"
#include "ui_friendlabel.h"

FriendLabel::FriendLabel(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::FriendLabel)
{
    ui->setupUi(this);
    ui->close_label->setState("normal", "hover", "press",
                              "select_normal", "select_hover", "select_press");
    connect(ui->close_label, &ClickedLabel::clicked, this, &FriendLabel::slot_close);
}

FriendLabel::~FriendLabel()
{
    delete ui;
}

void FriendLabel::setText(QString text)
{
    text_ = text;
    ui->tip_label->setText(text);
    ui->tip_label->adjustSize();

    QFontMetrics fontMetrics(ui->tip_label->font());//获取字体信息
    auto textWidth = fontMetrics.horizontalAdvance(ui->tip_label->text());//获取文本的宽度
    auto textHeight = fontMetrics.height();//获取文本的高度

    qDebug() << "ui->tip_label.width() is: " << ui->tip_label->width();
    qDebug() << "ui->close_label.width() is: " << ui->close_label->width();
    qDebug() << "textWidth is: " << textWidth;

    this->setFixedWidth(ui->tip_label->width() + ui->close_label->width() + 5);
    this->setFixedHeight(textHeight + 2);
    qDebug() << "this->setFixedHeight " << this->height();
    width_ = this->width();
    height_ = this->height();
}

int FriendLabel::Width()
{
    return width_;
}

int FriendLabel::Height()
{
    return height_;
}

QString FriendLabel::Text()
{
    return text_;
}

void FriendLabel::slot_close()
{
    emit sig_close(text_);
}
