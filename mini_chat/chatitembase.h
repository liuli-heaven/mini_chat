#ifndef CHATITEMBASE_H
#define CHATITEMBASE_H
#include "global.h"
#include "QWidget"
#include "QLabel"
class BubbleFrame;

class ChatItemBase : public QWidget
{
    Q_OBJECT
public:
    explicit ChatItemBase(ChatRole role, QWidget* parent = nullptr);
    void setUserName(const QString& name);
    void setUserIcon(const QPixmap& icon);
    void setWidget(QWidget* w);

private:
    ChatRole m_role_;
    QLabel* m_pNameLabel;
    QLabel* m_pIconLabel;
    QWidget* m_pBubble;
};

#endif // CHATITEMBASE_H
