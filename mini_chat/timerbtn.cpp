#include "timerbtn.h"
#include"QMouseEvent"

#include<QDebug>

TimerBtn::TimerBtn(QWidget* parent) : QPushButton(parent), counter_(30)
{
    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, [this](){
        counter_--;
        if(counter_ <= 0)
        {
            timer_->stop();
            counter_ = 30;
            this->setText("获取");
            this->setEnabled(true);
            return;
        }
        this->setText(QString::number(counter_));
    });

}

TimerBtn::~TimerBtn()
{
    timer_->stop();
}

void TimerBtn::setEmailLine(QLineEdit* edit)
{
    email_edit = edit;
}

void TimerBtn::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        //如果是由鼠标左键触发，则有效
        qDebug() << "MyButton was released!";
        if(email_edit->text() == "")
        {
            qDebug() << "email不能为空";
            return;
        }
        this->setEnabled(false);
        this->setText(QString::number(counter_));
        timer_->start(1000);
        emit clicked();
    }
    //调用基类的事件处理函数。
    QPushButton::mouseReleaseEvent(e);
}
