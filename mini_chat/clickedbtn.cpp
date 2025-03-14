#include "clickedbtn.h"
#include <QMouseEvent>'
#include "global.h"
ClickedBtn::ClickedBtn(QWidget *parent) : QPushButton(parent)
{
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::NoFocus);
}

void ClickedBtn::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        setProperty("state", press_);
        repolish(this);
        update();
    }
    QPushButton::mousePressEvent(e);
}

void ClickedBtn::enterEvent(QEnterEvent *event)
{
    setProperty("state", hover_);
    repolish(this);
    update();
    QPushButton::enterEvent(event);
}

void ClickedBtn::leaveEvent(QEvent *event)
{
    setProperty("state", normal_);
    repolish(this);
    update();
    QPushButton::leaveEvent(event);
}

void ClickedBtn::mouseReleaseEvent(QMouseEvent *e)
{
    setProperty("state", hover_);
    repolish(this);
    update();
    QPushButton::mouseReleaseEvent(e);
}

void ClickedBtn::setState(QString normal, QString hover, QString press)
{
    normal_ = normal;
    hover_ = hover;
    press_ = press;

    setProperty("state", normal_);
}
