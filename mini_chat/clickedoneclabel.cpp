#include "clickedoneclabel.h"

ClickedOnecLabel::ClickedOnecLabel(QWidget *parent) : QLabel(parent)
{
    setCursor(Qt::PointingHandCursor);
}

void ClickedOnecLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::LeftButton)
    {
        emit clicked(this->text());
        return;
    }
    //调用基类事件
    return mouseReleaseEvent(ev);
}
