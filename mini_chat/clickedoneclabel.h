#ifndef CLICKEDONECLABEL_H
#define CLICKEDONECLABEL_H

#include <QLabel>
#include <QMouseEvent>

class ClickedOnecLabel : public QLabel
{
    Q_OBJECT
public:
    ClickedOnecLabel(QWidget* parent = nullptr);
protected:
    virtual void mouseReleaseEvent(QMouseEvent* ev)override;

signals:
    void clicked(QString);

};

#endif // CLICKEDONECLABEL_H
