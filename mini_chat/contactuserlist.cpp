#include "contactuserlist.h"
#include "tcpmgr.h"
#include "contactuseritem.h"
#include "userdata.h"
#include "grouptipitem.h"
#include "QWheelEvent"
#include "QScrollBar"
#include "QTimer"
ContactUserList::ContactUserList(QWidget *parent) : QListWidget(parent)
{
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //安装时间过滤器
    this->viewport()->installEventFilter(this);
    //模拟传来的数据，进行列表加载
    addContactUserList();
    //连接点击的信号和槽
    connect(this, &ContactUserList::itemClicked, this, &ContactUserList::slot_item_clicked);
    //连接对端同意认证后的通知
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_add_auth_friend,
            this, &ContactUserList::slot_add_auth_friend);
    //连接自己点击同意认证后界面刷新
    connect(TcpMgr::GetInstance().get(), &TcpMgr::sig_auth_rsp,
            this, &ContactUserList::slot_auth_rsp);
    connect(this->verticalScrollBar(), &QScrollBar::valueChanged, this, &ContactUserList::slot_test);
    this->verticalScrollBar()->setValue(0);
}

void ContactUserList::showRedPoint(bool b_show)
{
    add_friend_item_->showRedPoint(true);
}

bool ContactUserList::eventFilter(QObject *watched, QEvent *event)
{
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
        //处理滚轮事件
        if(event->type() == QEvent::Wheel)
        {
            QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
            int numDegrees = wheelEvent->angleDelta().y() / 8;
            int numSteps = numDegrees / 15;
            //设置滚动幅度
            this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);

            //检查是否滚动到底部
            QScrollBar* scrollBar = this->verticalScrollBar();
            int maxValue = scrollBar->maximum();
            int currentValue = scrollBar->value();
            if(maxValue <= currentValue)
            {
                qDebug() << "load more contact user";
                emit sig_loading_contact_user();
            }
            return true;//停止事件传递
        }

    }
    return QListWidget::eventFilter(watched, event);
}

void ContactUserList::addContactUserList()
{
    auto* groupTip = new GroupTipItem;
    auto* item = new QListWidgetItem;
    item->setSizeHint(groupTip->sizeHint());
    this->addItem(item);
    this->setItemWidget(item, groupTip);
    item->setFlags(Qt::ItemFlags() & ~Qt::ItemIsSelectable);
    //添加新的好友条目
    add_friend_item_ = new ContactUserItem;
    add_friend_item_->setObjectName("new_friend_item");
    add_friend_item_->setInfo(0, "新的朋友", ":/icons/icon/add_friend.png");
    add_friend_item_->SetItemType(ListItemType::APPLY_FRIEND_ITEM);
    auto* add_item = new QListWidgetItem;
    add_item->setSizeHint(add_friend_item_->sizeHint());
    this->addItem(add_item);
    this->setItemWidget(add_item, add_friend_item_);
    //设置新的朋友条目默认为选中状态。
    this->setCurrentItem(add_item);
    //添加联系人组
    auto groupConTip = new GroupTipItem;
    groupConTip->setGroupTip("联系人");
    group_item_ = new QListWidgetItem;
    group_item_->setSizeHint(groupConTip->sizeHint());
    this->addItem(group_item_);
    this->setItemWidget(group_item_, groupConTip);
    group_item_->setFlags(Qt::ItemFlags() & ~Qt::ItemIsSelectable);

    //模拟添加联系人
    for(int i = 0; i < 14; i++)
    {
        QString name_i = names[i];
        QString head_i = heads[i];
        QString str_i = strs[i % strs.size()];

        auto* user_item = new ContactUserItem;
        user_item->setInfo(i, name_i, head_i);
        auto* new_item = new QListWidgetItem;
        new_item->setSizeHint(user_item->sizeHint());
        this->addItem(new_item);
        this->setItemWidget(new_item, user_item);
    }

    // 延迟设置滚动条值
    QTimer::singleShot(0, this, [this]() {
        this->verticalScrollBar()->setValue(0);
    });
}

void ContactUserList::slot_item_clicked(QListWidgetItem *item)
{
    QWidget* widget = this->itemWidget(item);
    if(!widget)
    {
        qDebug() << "slot item clicked widget is nullptr";
        return;
    }
    auto* customItem = qobject_cast<ListItemBase*>(widget);
    if(!customItem)
    {
        qDebug() << "slot item change to ListItemBase error";
        return;
    }
    ListItemType itemType = customItem->GetItemType();
    if(itemType == ListItemType::INVALID_ITEM ||
        itemType == ListItemType::GROUP_TIP_ITEM)
    {
        qDebug() << "slot invalid item clicked";
        return;
    }

    if(itemType == ListItemType::APPLY_FRIEND_ITEM)
    {
        qDebug() << "apply friend item clicked";
        emit sig_switch_apply_friend_page();
        return;
    }
    if(itemType == ListItemType::CONTACT_USER_ITEM)
    {
        qDebug() << "contact user item clicked";
        emit sig_switch_friend_info_page();
        return;
    }

}

void ContactUserList::slot_add_auth_friend(std::shared_ptr<AuthInfo>)
{

}

void ContactUserList::slot_auth_rsp(std::shared_ptr<AuthRsp>)
{

}

void ContactUserList::slot_test(int value)
{
    qDebug() << "now value is:" << value;
}
