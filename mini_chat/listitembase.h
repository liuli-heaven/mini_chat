#ifndef LISTITEMBASE_H
#define LISTITEMBASE_H
#include <QWidget>
#include <QDialog>
#include "global.h"
class ListItemBase : public QDialog
{
    Q_OBJECT
public:
    explicit ListItemBase(QWidget* parent = nullptr);
    void SetItemType(ListItemType itemType);
    ListItemType GetItemType();

private:
    ListItemType itemType_;

public slots:

signals:

};

#endif // LISTITEMBASE_H
