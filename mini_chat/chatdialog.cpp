#include "chatdialog.h"
#include "ui_chatdialog.h"
#include "ChatUserWid.h"
#include<QRandomGenerator>
#include "loadingdialog.h"
#include "customizeedit.h"


ChatDialog::ChatDialog(QWidget *parent): QDialog(parent), ui(new Ui::ChatDialog),
    mode_(ChatUIMode::ChatMode), state_(ChatUIMode::ChatMode), b_loading_(false)
{
    ui->setupUi(this);
    ui->add_btn->setState("normal", "hover", "press");
    ui->search_edit->setClearButtonEnabled(true);
    connect(ui->search_edit, &CustomizeEdit::textChanged,[this](const QString& text){
        if(text.isEmpty())
        {
            showSearch(false);
        }
    });

    ui->search_edit->SetMaxLength(15);
    showSearch(false);
    connect(ui->chat_user_list_wid, &ChatUserList::sig_loading_chat_user, this, &ChatDialog::slot_loading_chat_user);
    addChatUserList();

    //设置侧边框
    QPixmap pixmap(":/icons/icon/爱丽丝_玛格特罗依德.png");
    if (pixmap.isNull()) {
        qDebug() << "Failed to load pixmap from" << ":/icons/icon/爱丽丝_玛格特罗依德.png";
    }
    ui->side_head_widget->setPixmap(pixmap); // 将图片设置到QLabel上
    ui->side_head_widget->setScaledContents(true); // 设置QLabel自动缩放图片内容以适应大小
    ui->side_chat_widget->setProperty("state","normal");
    ui->side_chat_widget->setState("normal","hover","press","select_normal","select_hover","select_pressed");
    ui->side_contact_widget->setState("normal","hover","press","select_normal","select_hover","select_pressed");

    addLBGroup(ui->side_contact_widget);
    addLBGroup(ui->side_chat_widget);
    connect(ui->side_contact_widget, &StateWidget::clicked, this, &ChatDialog::slot_side_contact);
    connect(ui->side_chat_widget, &StateWidget::clicked, this, &ChatDialog::slot_side_chat);

    //链接搜索框内容变化
    connect(ui->search_edit, &CustomizeEdit::textChanged, this, &ChatDialog::slot_search_text_changed);

    //安装事件过滤器，检测鼠标点击位置来判断是否需要清空搜索框
    this->installEventFilter(this);

    ui->side_chat_widget->setSelected(true);
}

ChatDialog::~ChatDialog()
{
    delete ui;
}

void ChatDialog::showSearch(bool bsearch)
{
    if(bsearch)
    {
        ui->con_user_list_wid->hide();
        ui->chat_user_list_wid->hide();
        ui->search_list_wid->show();
        mode_ = ChatUIMode::SearchMode;
    }
    else if(state_ == ChatUIMode::ChatMode)
    {
        ui->con_user_list_wid->hide();
        ui->chat_user_list_wid->show();
        ui->search_list_wid->hide();
        mode_ = ChatUIMode::ChatMode;
    }
    else if(state_ == ChatUIMode::ContactMode)
    {
        ui->con_user_list_wid->show();
        ui->chat_user_list_wid->hide();
        ui->search_list_wid->hide();
        mode_ = ChatUIMode::ContactMode;
    }
}

void ChatDialog::addChatUserList()
{
    // 创建QListWidgetItem，并设置自定义的widget
    for(int i = 0; i < 14; i++){
        //int randomValue = QRandomGenerator::global()->bounded(100); // 生成0到99之间的随机整数
        int str_i =  i % strs.size();
        int head_i =  i % heads.size();
        int name_i =  i % names.size();
        auto *chat_user_wid = new ChatUserWid();
        chat_user_wid->setInfo(names[name_i], heads[head_i], strs[str_i]);
        chat_user_wid->setTime(QTime::currentTime());
        QListWidgetItem *item = new QListWidgetItem;
        //qDebug()<<"chat_user_wid sizeHint is " << chat_user_wid->sizeHint();
        item->setSizeHint(chat_user_wid->sizeHint());
        ui->chat_user_list_wid->addItem(item);
        ui->chat_user_list_wid->setItemWidget(item, chat_user_wid);
    }
}

void ChatDialog::addLBGroup(StateWidget *lb)
{
    lb_list_.push_back(lb);
}

void ChatDialog::clearState(StateWidget *lb)
{
    for(auto& lb_ : lb_list_)
    {
        if(lb_ == lb)
            continue;
        lb_->clearState();
    }
}

void ChatDialog::handleGlobalMouseEvent(QMouseEvent *event)
{
    //这里对搜索模式下的鼠标点击进行处理
    if(mode_ == ChatUIMode::SearchMode)
    {
        //将鼠标点击位置转换为搜索列表坐标系中的位置
        QPoint posInSearchList = ui->search_list_wid->mapFromGlobal(event->globalPosition()).toPoint();
        //判断点击位置是否在搜索列表的范围内
        if(!ui->search_list_wid->rect().contains(posInSearchList))
        {
            //不在搜索列表范围内，则清空搜索框
            ui->search_edit->clear();
            showSearch(false);
        }
    }

}

bool ChatDialog::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        handleGlobalMouseEvent(mouseEvent);
    }
    return QDialog::eventFilter(watched, event);
}

void ChatDialog::slot_loading_chat_user()
{
    if(b_loading_)
    {
        return;
    }
    b_loading_ = true;

    LoadingDialog* loadingDialog = new LoadingDialog(this);
    loadingDialog->setModal(true);
    loadingDialog->show();
    qDebug() << "add new data to list...";
    addChatUserList();
    //加载完后关闭窗口
    loadingDialog->deleteLater();
    b_loading_ = false;
}

void ChatDialog::slot_side_chat()
{
    qDebug() << "receive chat clicked";
    clearState(ui->side_chat_widget);
    ui->stackedWidget->setCurrentWidget(ui->chat_page);
    state_ = ChatUIMode::ChatMode;
    showSearch(false);
}

void ChatDialog::slot_side_contact()
{
    qDebug() << "receive chat clicked";
    clearState(ui->side_contact_widget);
    ui->stackedWidget->setCurrentWidget(ui->friend_apply_page);
    state_ = ChatUIMode::ContactMode;
    showSearch(false);
}

void ChatDialog::slot_search_text_changed(const QString &str)
{
    if(!str.isEmpty())
    {
        showSearch(true);
    }
}

