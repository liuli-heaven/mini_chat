#ifndef TIMERBTN_H
#define TIMERBTN_H
#include<QPushButton>
#include<QTimer>
#include<QLineEdit>

class TimerBtn : public QPushButton
{
    Q_OBJECT
public:
    TimerBtn(QWidget* parent);
    ~TimerBtn();
    void setEmailLine(QLineEdit* edit);
    virtual void mouseReleaseEvent(QMouseEvent* e)override;

private:
    QTimer* timer_;
    int counter_;
    QLineEdit* email_edit;


};

#endif // TIMERBTN_H
