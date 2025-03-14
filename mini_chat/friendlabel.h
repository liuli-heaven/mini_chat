#ifndef FRIENDLABEL_H
#define FRIENDLABEL_H

#include <QFrame>

namespace Ui {
class FriendLabel;
}

class FriendLabel : public QFrame
{
    Q_OBJECT

public:
    explicit FriendLabel(QWidget *parent = nullptr);
    ~FriendLabel();
    void setText(QString text);
    int Width();
    int Height();
    QString Text();
signals:
    void sig_close(QString);
public slots:
    void slot_close();
private:
    Ui::FriendLabel *ui;
    QString text_;
    int width_;
    int height_;
};

#endif // FRIENDLABEL_H
