#include "chatpage.h"
#include "ui_chatpage.h"
#include "QStyleOption"
#include "QPainter"
#include "global.h"
#include "chatitembase.h"
#include "textbubble.h"
#include "picturebubble.h"
ChatPage::ChatPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatPage)
{
    ui->setupUi(this);

    //设置按钮样式
    ui->receive_btn->setState("normal", "hover", "press");
    ui->send_btn->setState("normal", "hover", "press");
    //设置图标样式
    ui->emo_label->setState("normal", "hover", "press","normal", "hover", "press");
    ui->file_label->setState("normal", "hover", "press","normal", "hover", "press");
    connect(ui->chat_edit, &MessageTextEdit::send, this, &ChatPage::on_send_btn_clicked);
}

ChatPage::~ChatPage()
{
    delete ui;
}

void ChatPage::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ChatPage::on_send_btn_clicked()
{
    auto pTextEdit = ui->chat_edit;
    ChatRole role = ChatRole::self;
    QString userName = QStringLiteral("");
    QString userIcon = ":/icons/icon/爱丽丝_玛格特罗依德.png";

    const QVector<MsgInfo>& msgList = pTextEdit->getMsgList();
    for(int i = 0; i < msgList.size(); i++)
    {
        QString type = msgList[i].msgFlag;
        ChatItemBase* pChatItem = new ChatItemBase(role);
        pChatItem->setUserName(userName);
        pChatItem->setUserIcon(QPixmap(userIcon));
        QWidget* pBubble = nullptr;
        if(type == "text")
        {
            pBubble = new TextBubble(role, msgList[i].msgcontent);
        }
        else if(type == "image")
        {
            pBubble = new PictureBubble(QPixmap(msgList[i].msgcontent), role);
        }
        else if(type == "file")
        {

        }

        if(pBubble != nullptr)
        {
            pChatItem->setWidget(pBubble);
            ui->chat_data_list->appendChatItem(pChatItem);
        }
    }
}


void ChatPage::on_receive_btn_clicked()
{
    qDebug() << "clicked";
}

