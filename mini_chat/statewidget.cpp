#include "statewidget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <qboxlayout.h>

StateWidget::StateWidget(QWidget *parent) : QLabel(parent), cur_state_(LabelState::Normal)
{
    setCursor(Qt::CursorShape::PointingHandCursor);
    //添加红点
    addRedPoint();
}

void StateWidget::setState(QString normal, QString hover, QString press, QString select, QString select_hover, QString select_press)
{
    normal_ = normal;
    normal_hover_ = hover;
    normal_press_ = press;
    select_ = select;
    select_hover_ = select_hover;
    select_press_ = select_press;
    setProperty("state",normal);
    repolish(this);
}

LabelState StateWidget::getCurState()
{
    return cur_state_;
}

void StateWidget::clearState()
{
    cur_state_ = LabelState::Normal;
    setProperty("state", normal_);
    repolish(this);
    update();
}

void StateWidget::setSelected(bool bSelected)
{
    if(bSelected)
    {
        cur_state_ = LabelState::Selected;
        setProperty("state", select_);
        repolish(this);
        update();
        return;
    }
    cur_state_ = LabelState::Normal;
    setProperty("state", normal_);
    repolish(this);
    update();
}

void StateWidget::addRedPoint()
{
    //添加红点示意图
    red_point_ = new QLabel();
    red_point_->setObjectName("red_point");
    QVBoxLayout* layout2 = new QVBoxLayout();
    red_point_->setAlignment(Qt::AlignmentFlag::AlignCenter);
    layout2->addWidget(red_point_);
    layout2->setContentsMargins(0, 0, 0, 0);
    this->setLayout(layout2);
    red_point_->setVisible(false);
}

void StateWidget::showRedPoint(bool show)
{
    if(show)
    {
        red_point_->setVisible(true);
        return;
    }
    red_point_->setVisible(false);
}

void StateWidget::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter painter(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
    QLabel::paintEvent(event);
    return;
}

void StateWidget::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) {
        if(cur_state_ == LabelState::Selected){
            qDebug()<<"PressEvent , already to selected press: "<< select_press_;
            //emit clicked();
            // 调用基类的mousePressEvent以保证正常的事件处理
            QLabel::mousePressEvent(ev);
            return;
        }
        if(cur_state_ == LabelState::Normal){
            qDebug()<<"PressEvent , change to selected press: "<< select_press_;
            cur_state_ = LabelState::Selected;
            setProperty("state",select_press_);
            repolish(this);
            update();
        }
        return;
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QLabel::mousePressEvent(ev);
}

void StateWidget::mouseReleaseEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton) {
        if(cur_state_ == LabelState::Normal){
            //qDebug()<<"ReleaseEvent , change to normal hover: "<< _normal_hover;
            setProperty("state",normal_hover_);
            repolish(this);
            update();
        }else{
            //qDebug()<<"ReleaseEvent , change to select hover: "<< _selected_hover;
            setProperty("state",select_hover_);
            repolish(this);
            update();
        }
        emit clicked();
        return;
    }
    // 调用基类的mousePressEvent以保证正常的事件处理
    QLabel::mousePressEvent(ev);
}

void StateWidget::enterEvent(QEnterEvent *event)
{
    // 在这里处理鼠标悬停进入的逻辑
    if(cur_state_ == LabelState::Normal){
        //qDebug()<<"enter , change to normal hover: "<< _normal_hover;
        setProperty("state",normal_hover_);
        repolish(this);
        update();
    }else{
        //qDebug()<<"enter , change to selected hover: "<< _selected_hover;
        setProperty("state",select_hover_);
        repolish(this);
        update();
    }
    QLabel::enterEvent(event);
}

void StateWidget::leaveEvent(QEvent *event)
{
    // 在这里处理鼠标悬停离开的逻辑
    if(cur_state_ == LabelState::Normal){
        // qDebug()<<"leave , change to normal : "<< _normal;
        setProperty("state",normal_);
        repolish(this);
        update();
    }else{
        // qDebug()<<"leave , change to select normal : "<< _selected;
        setProperty("state",select_);
        repolish(this);
        update();
    }
    QLabel::leaveEvent(event);
}
