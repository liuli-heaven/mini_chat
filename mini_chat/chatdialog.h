#ifndef CHATDIALOG_H
#define CHATDIALOG_H
#include "global.h"
#include <QDialog>
#include "statewidget.h"
namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();

private:
    void showSearch(bool bsearch = false);
    void addChatUserList();
    void addLBGroup(StateWidget* lb);
    void clearState(StateWidget* lb);
    void handleGlobalMouseEvent(QMouseEvent* event);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void slot_loading_chat_user();
    void slot_side_chat();
    void slot_side_contact();
    void slot_search_text_changed(const QString& str);

private:
    Ui::ChatDialog *ui;
    ChatUIMode mode_;
    ChatUIMode state_;
    bool b_loading_;
    QList<StateWidget*> lb_list_;
};

#endif // CHATDIALOG_H
