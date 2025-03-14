#include "textbubble.h"
#include "QEvent"
#include "QTextBlock"
#include <QPainter>

constexpr int width_triangle = 8;
TextBubble::TextBubble(ChatRole role, const QString &text, QWidget *parent): BubbleFrame(role, parent)
{
    m_pTextEdit = new QTextEdit();
    m_pTextEdit->setReadOnly(true);
    m_pTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    m_pTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    m_pTextEdit->installEventFilter(this);
    QFont font("Microsoft YaHei");
    m_pTextEdit->setFont(font);
    setPlainText(text);
    setWidget(m_pTextEdit);
    initStyleSheet();

}

bool TextBubble::eventFilter(QObject *o, QEvent *e)
{
    if(m_pTextEdit == o && e->type() == QEvent::Type::Paint)
    {
        adjustTextHeight();
    }
    return BubbleFrame::eventFilter(o, e);
}



void TextBubble::adjustTextHeight()
{
    qreal doc_margin = m_pTextEdit->document()->documentMargin(); // 字体到边框的距离
    QTextDocument* doc = m_pTextEdit->document();
    qreal text_height = 0;
    //每一段的高度相加得到文本高
    for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next())
    {
        QTextLayout* pLayout = it.layout();
        QRectF text_rect = pLayout->boundingRect();
        text_height += text_rect.height();
    }
    int vMargin = this->layout()->contentsMargins().top();
    setFixedHeight(text_height + doc_margin * 2 + vMargin * 2);
}

void TextBubble::setPlainText(const QString &text)
{
    m_pTextEdit->setPlainText(text);
    qreal doc_margin = m_pTextEdit->document()->documentMargin();
    int margin_left = this->layout()->contentsMargins().left();
    int margin_right = this->layout()->contentsMargins().right();
    QFontMetricsF fm(m_pTextEdit->font());
    QTextDocument* doc = m_pTextEdit->document();
    int max_width = 0;
    for(QTextBlock it = doc->begin(); it != doc->end(); it = it.next())
    {
        int text_width = int(fm.horizontalAdvance(it.text()));
        max_width = max_width < text_width? text_width : max_width;
    }
    setMaximumWidth(max_width + doc_margin * 2 + margin_left + margin_right + 1);
}

void TextBubble::initStyleSheet()
{
    m_pTextEdit->setStyleSheet("QTextEdit{background:transparent; border:none}");
}
