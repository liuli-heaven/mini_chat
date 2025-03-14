#ifndef SEARCHLIST_H
#define SEARCHLIST_H
#include "QListWidget"
#include "loadingdialog.h"
#include "userdata.h"
class SearchList: public QListWidget
{
    Q_OBJECT
public:
    SearchList(QWidget* parent = nullptr);
    void closeFindDialog();
    void setSearchEdit(QWidget* edit);
protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void slot_item_clicked(QListWidgetItem* item);
    void slot_user_search(std::shared_ptr<SearchInfo> info);
private:
    void waitPending(bool pending = true);
    void addTipItem();

    bool send_pending_;
    std::shared_ptr<QDialog> find_dlg_;
    QWidget* search_edit_;
    LoadingDialog* loadingDlg_;
};

#endif // SEARCHLIST_H
