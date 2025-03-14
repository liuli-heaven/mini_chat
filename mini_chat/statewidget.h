#ifndef STATEWIDGET_H
#define STATEWIDGET_H
#include "QWidget"
#include "global.h"
#include <QLabel>
#include <QEvent>
class StateWidget : public QLabel
{
    Q_OBJECT
public:
    explicit StateWidget(QWidget* parent = nullptr);

    void setState(QString normal = "", QString hover = "", QString press = "",
                  QString select = "", QString select_hover = "", QString select_press = "");

    LabelState getCurState();
    void clearState();

    void setSelected(bool bSelected);
    void addRedPoint();
    void showRedPoint(bool show = true);
protected:
    virtual void paintEvent(QPaintEvent* event);
    virtual void mousePressEvent(QMouseEvent *ev) override;
    virtual void mouseReleaseEvent(QMouseEvent *ev) override;
    virtual void enterEvent(QEnterEvent* event) override;
    virtual void leaveEvent(QEvent* event) override;
protected:

    QString normal_;
    QString normal_hover_;
    QString normal_press_;
    QString select_;
    QString select_hover_;
    QString select_press_;

    LabelState cur_state_;
    QLabel* red_point_;

signals:
    void clicked();

public slots:

};

#endif // STATEWIDGET_H
