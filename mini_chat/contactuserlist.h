#ifndef CONTACTUSERLIST_H
#define CONTACTUSERLIST_H

#include<QEvent>
#include <QListWidget>

class ContactUserItem;
class AuthInfo;
class AuthRsp;

class ContactUserList : public QListWidget
{
    Q_OBJECT
public:
    ContactUserList(QWidget* parent = nullptr);
    void showRedPoint(bool b_show = true);
protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
private:
    void addContactUserList();
signals:
    void sig_loading_contact_user();
    void sig_switch_apply_friend_page();
    void sig_switch_friend_info_page();
public slots:
    void slot_item_clicked(QListWidgetItem* item);
    void slot_add_auth_friend(std::shared_ptr<AuthInfo>);
    void slot_auth_rsp(std::shared_ptr<AuthRsp>);
    void slot_test(int value);

private:
    ContactUserItem* add_friend_item_;
    QListWidgetItem* group_item_;
};

#endif // CONTACTUSERLIST_H
