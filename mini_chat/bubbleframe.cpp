#include "bubbleframe.h"
#include "QPainter"
constexpr int width_triangle = 8;
BubbleFrame::BubbleFrame(ChatRole role, QWidget *parent): QFrame(parent), m_role_(role), m_margin(3)
{
    m_pHLayout = new QHBoxLayout();
    if(m_role_ == ChatRole::self)
    {
        m_pHLayout->setContentsMargins(m_margin, m_margin, width_triangle + m_margin, m_margin);
    }
    else
    {
       m_pHLayout->setContentsMargins(m_margin + width_triangle, m_margin, m_margin, m_margin);
    }
    this->setLayout(m_pHLayout);
}

void BubbleFrame::setWidget(QWidget *w)
{
    if(m_pHLayout->count() > 0)
    {
        return;
    }
    else
    {
        m_pHLayout->addWidget(w);
    }
}

void BubbleFrame::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(Qt::PenStyle::NoPen);
    if(m_role_ == ChatRole::other)
    {
        //画气泡
        QColor block_color(Qt::white);
        painter.setBrush(QBrush(block_color));
        QRect block_rect = QRect(width_triangle, 0, this->width() - width_triangle, this->height());
        painter.drawRoundedRect(block_rect, 5, 5);
        //画小三角
        QPointF points[3] = {
            QPointF(block_rect.x(), 12),
            QPointF(block_rect.x(), 10 + width_triangle + 2),
            QPointF(block_rect.x() - width_triangle, 10 + width_triangle / 2)
        };
        painter.drawPolygon(points, 3);
    }
    else
    {
        QColor block_color(158, 234, 106);
        painter.setBrush(QBrush(block_color));
        //画气泡
        QRect block_rect = QRect(0, 0, this->width() - width_triangle, this->height());
        painter.drawRoundedRect(block_rect, 5, 5);
        //画三角
        QPointF points[3] = {
            QPointF(block_rect.x() + block_rect.width(), 12),
            QPointF(block_rect.x() + block_rect.width(), 12 + width_triangle + 2),
            QPointF(block_rect.x() + block_rect.width() + width_triangle, 10 + width_triangle / 2)
        };
        painter.drawPolygon(points, 3);
    }
    QFrame::paintEvent(e);
}
