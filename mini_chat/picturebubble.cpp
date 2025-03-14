#include "picturebubble.h"
#include "QLabel"
constexpr int pic_max_width = 160;
constexpr int pic_max_height = 96;

PictureBubble::PictureBubble(const QPixmap &picture, ChatRole role, QWidget *parent)
    : BubbleFrame(role, parent)
{
    QLabel* lb = new QLabel();
    lb->setScaledContents(true);
    lb->setScaledContents(true);
    QPixmap pix = picture.scaled(pic_max_width, pic_max_height, Qt::AspectRatioMode::KeepAspectRatio);
    lb->setPixmap(pix);
    this->setWidget(lb);
    int margin_left = this->layout()->contentsMargins().left();
    int margin_right = this->layout()->contentsMargins().right();
    int margin_v = this->layout()->contentsMargins().bottom();
    setFixedSize(pix.width() + margin_right + margin_left, pix.height() + margin_v * 2);
}
