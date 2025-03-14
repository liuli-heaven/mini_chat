#include "chatuserlist.h"
#include "QScrollBar"
ChatUserList::ChatUserList(QWidget *parent): QListWidget(parent)
{
    Q_UNUSED(parent);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //安装事件过滤器
    this->viewport()->installEventFilter(this);
}

bool ChatUserList::eventFilter(QObject *watched, QEvent *event)
{
    //自定义进入离开事件
    if(watched == this->viewport())
    {
        if(event->type() == QEvent::Enter)
        {
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }
        else if(event->type() == QEvent::Leave)
        {
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }
    //自定义滚动事件
    if(watched == this->viewport() && event->type() == QEvent::Wheel)
    {
        QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
        int numDegrees = wheelEvent->angleDelta().y() / 8;
        int numSteps = numDegrees / 15;

        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);

        QScrollBar* scrollBar = this->verticalScrollBar();
        int maxValue = scrollBar->maximum();
        int currentValue = scrollBar->value();

        if(maxValue - currentValue <= 0)
        {
            qDebug() << "load more users";
            emit sig_loading_chat_user();
        }
        return true;
    }
    //调用基类的其余处理事件
    return QListWidget::eventFilter(watched, event);
}
