#ifndef TEXTBUBBLE_H
#define TEXTBUBBLE_H
#include "bubbleframe.h"
#include "QTextEdit"
class TextBubble : public BubbleFrame
{
    Q_OBJECT
public:
    TextBubble(ChatRole role, const QString& text, QWidget* parent = 0);
protected:
    bool eventFilter(QObject* o, QEvent* e) override;
private:
    void adjustTextHeight();
    void setPlainText(const QString& text);
    void initStyleSheet();
private:
    QTextEdit* m_pTextEdit;
};

#endif // TEXTBUBBLE_H
