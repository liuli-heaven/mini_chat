#ifndef APPLYFRIENDDLG_H
#define APPLYFRIENDDLG_H

#include <QDialog>
#include "clickedlabel.h"
#include "userdata.h"
#include "friendlabel.h"
namespace Ui {
class ApplyFriendDlg;
}

class ApplyFriendDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ApplyFriendDlg(QWidget *parent = nullptr);
    ~ApplyFriendDlg();
    void initTipLabels();
    void addTipLabel(ClickedLabel* label, QPoint cur_point, QPoint& next_point, int text_width, int text_height);
    void setSearchInfo(std::shared_ptr<SearchInfo> info);
private:
    void resetLabels();
    void addLabel(QString name);
protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
public slots:
    void showMoreLabel();//显示更多label标签
    void slot_label_enter();//输入label按下回车将其放入展示栏。
    void slot_remove_friendLabel(QString text);//点击关闭，移除展示栏和好友标签。
    void slot_change_friendLabel_by_tip(QString, LabelState);//通过点击tip改变好友标签
    void slot_label_text_change(const QString& text);//输入文本框的文本变化显示不同提示
    void slot_label_edit_finished();//输入框输入完成
    void slot_add_friendLabel_by_tip(QString text);//点击提示框内容后添加好友标签。
    void slot_apply_sure();//确认添加
    void slot_apply_cancel();//取消添加
private:
    Ui::ApplyFriendDlg *ui;
    QMap<QString, ClickedLabel*> add_labels_;//已经创建好的标签
    std::vector<QString> add_label_keys_;
    QPoint label_point_;
    QMap<QString, FriendLabel*> friend_labels_;//在输入框显示添加新好友的标签。
    std::vector<QString> friend_label_keys_;
    std::vector<QString> tip_data_;
    QPoint tip_cur_point_;
    std::shared_ptr<SearchInfo> info_;
};

#endif // APPLYFRIENDDLG_H
