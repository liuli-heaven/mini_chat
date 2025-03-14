#ifndef CLICKEDBTN_H
#define CLICKEDBTN_H
#include <QPushButton>

class ClickedBtn : public QPushButton
{
    Q_OBJECT
public:
    ClickedBtn(QWidget* parent);
    virtual void mousePressEvent(QMouseEvent *e) override;
    virtual void enterEvent(QEnterEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *e) override;
    void setState(QString normal, QString hover, QString press);

private:
    QString normal_;
    QString hover_;
    QString press_;
};

#endif // CLICKEDBTN_H
