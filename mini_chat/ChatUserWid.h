#ifndef CHATUSERWID_H
#define CHATUSERWID_H

#include <QDialog>
#include "listitembase.h"

namespace Ui {
class ChatUserWid;
}

class ChatUserWid : public ListItemBase
{
    Q_OBJECT

public:
    explicit ChatUserWid(QWidget *parent = nullptr);
    ~ChatUserWid();

    QSize sizeHint() const override;
    void setInfo(QString name, QString head, QString msg);
    void setTime(QTime time);
private:
    Ui::ChatUserWid *ui;
    QString name_;
    QString head_;
    QString msg_;
};

#endif // CHATUSERWID_H
