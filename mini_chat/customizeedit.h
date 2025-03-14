#ifndef CUSTOMIZEEDIT_H
#define CUSTOMIZEEDIT_H
#include <QLineEdit>
class CustomizeEdit : public QLineEdit
{
    Q_OBJECT
public:
    CustomizeEdit(QWidget* parent = nullptr);
    void SetMaxLength(int max_len);
    void limitMaxLength(QString len);
protected:
    void focusOutEvent(QFocusEvent * fe) override;
private:
    int max_len_;
signals:
    void sig_focus_out();
};

#endif // CUSTOMIZEEDIT_H
