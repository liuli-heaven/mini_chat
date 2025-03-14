#include "clickedlabel.h"
#include"QMouseEvent"
ClickedLabel::ClickedLabel(QWidget *parent) : QLabel(parent), cur_state_(LabelState::Normal)
{

}

void ClickedLabel::mousePressEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::LeftButton)
    {
        if(cur_state_ == LabelState::Normal)
        {
            qDebug() << "clicked, changed to selected hover";
            cur_state_ = LabelState::Selected;
            setProperty("state", selected_press_);
            repolish(this);
            update();
        }
        else
        {
            qDebug() << "clicked, changed to normal hover";
            cur_state_ = LabelState::Normal;
            setProperty("state", normal_press_);
            repolish(this);
            update();
        }
        return;
    }
    QLabel::mousePressEvent(ev);
}

void ClickedLabel::enterEvent(QEnterEvent *event)
{
    if(cur_state_ == LabelState::Normal)
    {
        qDebug() << "enter, changed to normal hover";
        setProperty("state", normal_hover_);
        repolish(this);
        update();
    }
    else
    {
        qDebug() << "enter, changed to selected hover";
        setProperty("state", selected_hover_);
        repolish(this);
        update();
    }
    QLabel::enterEvent(event);
}

void ClickedLabel::leaveEvent(QEvent *event)
{
    if(cur_state_ == LabelState::Normal)
    {
        qDebug() << "leave, changed to normal";
        setProperty("state", normal_);
        repolish(this);
        update();
    }
    else
    {
        qDebug() << "leave, changed to selected";
        setProperty("state", selected_);
        repolish(this);
        update();
    }
    QLabel::leaveEvent(event);
}

void ClickedLabel::mouseReleaseEvent(QMouseEvent *ev)
{
    if(ev->button() == Qt::LeftButton)
    {

        if(cur_state_ == LabelState::Normal)
        {
            //qDebug() << "released, changed to selected hover";
            setProperty("state", selected_hover_);
            repolish(this);
            update();
        }
        else
        {
            //qDebug() << "released, changed to normal hover";
            setProperty("state", normal_hover_);
            repolish(this);
            update();
        }
        emit clicked(this->text(), cur_state_);
        return;
    }
    QLabel::mouseReleaseEvent(ev);
}

void ClickedLabel::setState(QString normal, QString normal_hover, QString normal_press,
                            QString selected, QString selected_hover, QString selected_press)
{
    normal_ = normal;
    normal_hover_ = normal_hover;
    normal_press_ = normal_press;
    selected_ = selected;
    selected_hover_ = selected_hover;
    selected_press_ = selected_press;

    setProperty("state", normal_);
    repolish(this);
}

LabelState ClickedLabel::getCurState()
{
    return cur_state_;
}

bool ClickedLabel::setCurState(LabelState state)
{
    cur_state_ = state;
    if(cur_state_ == LabelState::Normal)
    {
        setProperty("state", normal_);
        repolish(this);
    }
    else if(cur_state_ == LabelState::Selected)
    {
        setProperty("state", selected_);
        repolish(this);
    }
    return true;
}

void ClickedLabel::resetNormalState()
{
    setProperty("state", normal_);
    cur_state_ = LabelState::Normal;
    repolish(this);
}
