#ifndef CHATPAGE_H
#define CHATPAGE_H
#include "clickedbtn.h"
#include <QWidget>

namespace Ui {
class ChatPage;
}

class ChatPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChatPage(QWidget *parent = nullptr);
    ~ChatPage();

protected:
    void paintEvent(QPaintEvent* event)override;


public slots:
    void on_send_btn_clicked();

private slots:
    void on_receive_btn_clicked();

private:
    Ui::ChatPage *ui;
};

#endif // CHATPAGE_H
