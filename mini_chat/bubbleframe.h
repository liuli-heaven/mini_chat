#ifndef BUBBLEFRAME_H
#define BUBBLEFRAME_H
#include "QFrame"
#include "global.h"
#include "QHBoxLayout"

class BubbleFrame : public QFrame
{
    Q_OBJECT
public:
    BubbleFrame(ChatRole role, QWidget* parent = nullptr);
    void setWidget(QWidget* w);
protected:
    void paintEvent(QPaintEvent* e) override;
protected:
    QHBoxLayout* m_pHLayout;
    ChatRole m_role_;
    int m_margin;
};

#endif // BUBBLEFRAME_H
