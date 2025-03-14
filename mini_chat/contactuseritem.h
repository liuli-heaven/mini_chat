#ifndef CONTACTUSERITEM_H
#define CONTACTUSERITEM_H

#include "listitembase.h"
#include "userdata.h"
namespace Ui {
class ContactUserItem;
}

class ContactUserItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit ContactUserItem(QWidget *parent = nullptr);
    ~ContactUserItem();
    void setInfo(std::shared_ptr<AuthInfo> auth);
    void setInfo(std::shared_ptr<AuthRsp> auth);
    void setInfo(int uid, QString name, QString icon);
    void showRedPoint(bool bShow = false);
    QSize sizeHint() const override;
private:
    Ui::ContactUserItem *ui;
    std::shared_ptr<UserInfo> info_;
};

#endif // CONTACTUSERITEM_H
