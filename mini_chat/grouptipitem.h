#ifndef GROUPTIPITEM_H
#define GROUPTIPITEM_H

#include <QDialog>
#include "listitembase.h"
namespace Ui {
class GroupTipItem;
}

class GroupTipItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit GroupTipItem(QWidget *parent = nullptr);
    ~GroupTipItem();
    void setGroupTip(QString str);
    QSize sizeHint() const override;
private:
    Ui::GroupTipItem *ui;
    QString tip_;
};

#endif // GROUPTIPITEM_H
