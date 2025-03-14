#ifndef CLICKEDLABEL_H
#define CLICKEDLABEL_H
#include"QLabel"
#include"global.h"
class ClickedLabel : public QLabel
{
    Q_OBJECT
public:
    ClickedLabel(QWidget* parent);
    virtual void mousePressEvent(QMouseEvent *ev) override;
    virtual void enterEvent(QEnterEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent* ev)override;

    void setState(QString normal, QString normal_hover, QString normal_press,
                  QString selected, QString selected_hover, QString selected_press);

    LabelState getCurState();
    bool setCurState(LabelState state);
    void resetNormalState();
private:
    QString normal_;
    QString normal_hover_;
    QString normal_press_;
    QString selected_;
    QString selected_hover_;
    QString selected_press_;

    LabelState cur_state_;
signals:
    void clicked(QString, LabelState);
};

#endif // CLICKEDLABEL_H
