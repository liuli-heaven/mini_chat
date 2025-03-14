#include "customizeedit.h"

CustomizeEdit::CustomizeEdit(QWidget *parent): max_len_(INT_MAX), QLineEdit(parent)
{
    connect(this, &QLineEdit::textChanged, this, &CustomizeEdit::limitMaxLength);
}

void CustomizeEdit::SetMaxLength(int max_len)
{
    max_len_ = max_len;
}

void CustomizeEdit::focusOutEvent(QFocusEvent *fe)
{
    qDebug() << "customizeEdit focusOut";
    QLineEdit::focusOutEvent(fe);

    emit sig_focus_out();
}

void CustomizeEdit::limitMaxLength(QString text)
{
    if(max_len_ <= 0)
    {
        return;
    }

    QByteArray byte = text.toUtf8();
    if(byte.size() > max_len_)
    {
        qDebug() << "text length out of ranges";
        byte = byte.left(max_len_);
        this->setText(QString::fromUtf8(byte));
    }
}
