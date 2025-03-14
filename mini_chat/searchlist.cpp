#include "searchlist.h"
#include "QEvent"
#include "QWheelEvent"
#include "QScrollBar"
#include "tcpmgr.h"
#include "adduseritem.h"
#include "findsuccessdlg.h"
#include "memory"
SearchList::SearchList(QWidget *parent): QListWidget(parent),
    find_dlg_(nullptr), search_edit_(nullptr), send_pending_(false)
{
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //安装事件过滤器
    this->viewport()->installEventFilter(this);
    //连接点击的信号和槽
    connect(this, &QListWidget::itemClicked, this, &SearchList::slot_item_clicked);
    //添加条目
    addTipItem();
    //连接搜索条目
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_user_search, this, &SearchList::slot_user_search);
}

void SearchList::closeFindDialog()
{
    if(find_dlg_)
    {
        find_dlg_->hide();
        find_dlg_ = nullptr;
    }
}

void SearchList::setSearchEdit(QWidget *edit)
{

}

bool SearchList::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == this->viewport())
    {
        //检查事件是否为鼠标悬浮进入或离开。
        if(event->type() == QEvent::Enter)
        {
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }
        else if(event->type() == QEvent::Leave)
        {
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }

        //检测是否为鼠标滚轮事件
        if(event->type() == QEvent::Wheel)
        {
            QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
            int numDegrees = wheelEvent->angleDelta().y() / 8;
            int numSteps = numDegrees / 15;//计算滚动步数

            //设置滚动幅度
            this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);
            return true;//停止事件传递。
        }
    }
    return QListWidget::eventFilter(watched, event);
}

void SearchList::slot_item_clicked(QListWidgetItem *item)
{
    QWidget* widget = this->itemWidget(item);
    if(!widget)
    {
        qDebug() << "slot item clicked widget is nullptr!";
        return;
    }
    //获取自定义的widget
    ListItemBase* customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem)
    {
        qDebug() << "slot item clicked widget is nullptr!";
        return;
    }

    auto itemType = customItem->GetItemType();
    if(itemType == ListItemType::INVALID_ITEM)
    {
        qDebug() << "slot invalid item clicked";
        return;
    }

    if(itemType == ListItemType::ADD_USER_TIP_ITEM)
    {
        /***
            处理过程
        */
        find_dlg_ =  std::make_shared<FindSuccessDlg>(this);

        std::shared_ptr<SearchInfo> info = std::make_shared<SearchInfo>(0, "河城荷取", "河城荷取", "hello, world!", 0);
        std::dynamic_pointer_cast<FindSuccessDlg>(find_dlg_)->setSearchInfo(info);
        find_dlg_->show();
        return;
    }

    closeFindDialog();
}

void SearchList::slot_user_search(std::shared_ptr<SearchInfo> info)
{

}

void SearchList::waitPending(bool pending)
{

}

void SearchList::addTipItem()
{
    QWidget* invalid_item = new QWidget();
    QListWidgetItem* item = new QListWidgetItem();
    item->setSizeHint(QSize(250, 10));
    this->addItem(item);
    invalid_item->setObjectName("invalid_item");
    this->setItemWidget(item, invalid_item);
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);

    AddUserItem* add_user_item = new AddUserItem();
    QListWidgetItem* item_2 = new QListWidgetItem();
    item_2->setSizeHint(add_user_item->sizeHint());
    this->addItem(item_2);
    this->setItemWidget(item_2, add_user_item);
}

