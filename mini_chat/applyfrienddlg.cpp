#include "applyfrienddlg.h"
#include "ui_applyfrienddlg.h"
#include "customizeedit.h"
#include "QScrollBar"
#include "global.h"
#include "usermgr.h"
ApplyFriendDlg::ApplyFriendDlg(QWidget *parent)
    : QDialog(parent), label_point_(2, 6)
    , ui(new Ui::ApplyFriendDlg)
{
    ui->setupUi(this);
    //隐藏对话框标题栏

    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    this->setObjectName("ApplyFriend");
    this->setModal(true);
    ui->desc_edit->setPlaceholderText(tr("博丽灵梦"));
    ui->remark_name_edit->setPlaceholderText(tr("红色杀人魔"));
    ui->label_edit->setPlaceholderText(tr("搜索、添加标签"));

    ui->label_edit->SetMaxLength(21);
    ui->label_edit->move(2, 2);
    ui->label_edit->setFixedHeight(20);
    ui->label_edit->setMaxLength(10);
    ui->tip_widget->hide();

    tip_cur_point_ = QPoint(5, 5);
    tip_data_ = {"幻想乡", "博丽神社", "红魔馆", "永远亭", "地灵殿", "魔法森林", "雾之湖",
                 "迷途竹林", "妖怪之山", "命莲寺", "神灵庙", "冥界"};
    connect(ui->more_label, &ClickedOnecLabel::clicked, this, &ApplyFriendDlg::showMoreLabel);
    initTipLabels();

    connect(ui->label_edit, &CustomizeEdit::returnPressed, this, &ApplyFriendDlg::slot_label_enter);
    connect(ui->label_edit, &CustomizeEdit::textChanged, this, &ApplyFriendDlg::slot_label_text_change);
    connect(ui->label_edit, &CustomizeEdit::editingFinished, this, &ApplyFriendDlg::slot_label_edit_finished);
    connect(ui->tip_label, &ClickedOnecLabel::clicked, this, &ApplyFriendDlg::slot_add_friendLabel_by_tip);

    ui->scrollArea->horizontalScrollBar()->setHidden(true);
    ui->scrollArea->verticalScrollBar()->setHidden(true);
    ui->scrollArea->installEventFilter(this);
    ui->sure_btn->setState("normal", "hover", "press");
    ui->cancel_btn->setState("normal", "hover", "press");

    connect(ui->sure_btn, &ClickedBtn::clicked, this, &ApplyFriendDlg::slot_apply_sure);
    connect(ui->cancel_btn, &ClickedBtn::clicked, this, &ApplyFriendDlg::slot_apply_cancel);
}

ApplyFriendDlg::~ApplyFriendDlg()
{
    delete ui;
}

void ApplyFriendDlg::initTipLabels()
{
    int lines = 1;
    for(int i = 0; i < tip_data_.size(); i++)
    {
        auto* label = new ClickedLabel(ui->label_list_widget);
        label->setState("normal", "hover", "press", "select_normal", "select_hover", "select_press");
        label->setObjectName("TipsLabel");
        label->setText(tip_data_[i]);
        connect(label, &ClickedLabel::clicked, this, &ApplyFriendDlg::slot_change_friendLabel_by_tip);

        QFontMetrics fontMetrics(label->font());
        int textWidth = fontMetrics.horizontalAdvance(label->text());
        int textHeight = fontMetrics.height();

        if(tip_cur_point_.x() + textWidth + tip_offset > ui->label_list_widget->width())
        {
            lines++;
            if(lines > 2)
            {
                delete label;
                return;
            }
            tip_cur_point_.setX(tip_offset);
            tip_cur_point_.setY(tip_cur_point_.y() + textHeight + 15);
        }

        auto next_point = tip_cur_point_;
        addTipLabel(label, tip_cur_point_, next_point, textWidth, textHeight);
        tip_cur_point_ = next_point;

    }
}

void ApplyFriendDlg::addTipLabel(ClickedLabel *label, QPoint cur_point, QPoint &next_point, int text_width, int text_height)
{
    label->move(cur_point);
    label->show();
    add_labels_.insert(label->text(), label);
    add_label_keys_.push_back(label->text());
    next_point.setX(label->pos().x() + text_width + 15);
    next_point.setY(label->pos().y());
}

void ApplyFriendDlg::setSearchInfo(std::shared_ptr<SearchInfo> info)
{
    info_ = info;
    auto applyName = UserMgr::GetInstance()->getName();
    auto remark_name = info->name_;
    ui->desc_edit->setText(applyName);
    ui->remark_name_edit->setText(remark_name);

}

void ApplyFriendDlg::resetLabels()
{
    auto max_width = ui->grid_widget->width();
    auto label_height = 0;
    for(auto iter = friend_labels_.begin(); iter != friend_labels_.end(); iter++)
    {
        if(label_point_.x() + iter.value()->width() > max_width)
        {
            label_point_.setY(label_point_.y() + iter.value()->height() + 6);
            label_point_.setX(2);
        }
        iter.value()->move(label_point_);
        iter.value()->show();

        label_point_.setX(label_point_.x() + iter.value()->width() + 2);
        label_point_.setY(label_point_.y());
        label_height = iter.value()->height();
    }

    if(friend_labels_.isEmpty())
    {
        ui->label_edit->move(label_point_);
        return;
    }
    if(label_point_.x() + min_apply_label_edit_len > ui->grid_widget->width())
    {
        ui->label_edit->move(2, label_point_.y() + label_height + 6);
    }
    else
    {
        ui->label_edit->move(label_point_);
    }
}

void ApplyFriendDlg::addLabel(QString name)
{
    //如果存在标签，则不再添加
    if(friend_labels_.find(name) != friend_labels_.end())
    {
        return;
    }
    auto tmpLabel = new FriendLabel(ui->grid_widget);
    tmpLabel->setText(name);
    tmpLabel->setObjectName("FriendLabel");

    auto max_width = ui->grid_widget->width();

    // 如果宽度超过最大宽度，则需要换行
    if(label_point_.x() + tmpLabel->width() > max_width)
    {
        label_point_.setY(label_point_.y() + tmpLabel->height() + 6);
        label_point_.setX(2);
    }

    tmpLabel->move(label_point_);
    tmpLabel->show();
    friend_labels_[tmpLabel->Text()] = tmpLabel;
    friend_label_keys_.push_back(tmpLabel->Text());

    connect(tmpLabel, &FriendLabel::sig_close, this, &ApplyFriendDlg::slot_remove_friendLabel);

    qDebug() << "tmpLabel-> width() is: " << tmpLabel->width();
    label_point_.setX(label_point_.x() + tmpLabel->width() + 2);
    qDebug() << "label_point_'s x is:" << label_point_.x();
    if(label_point_.x() + min_apply_label_edit_len > ui->grid_widget->width())
    {
        ui->label_edit->move(2, label_point_.y() + tmpLabel->height() + 2);
    }
    else
    {
        ui->label_edit->move(label_point_);
    }
    ui->label_edit->updateGeometry();
    ui->label_edit->clear();
    if(ui->grid_widget->height() < label_point_.y() + tmpLabel->height() * 2 + 2)
    {
        ui->grid_widget->setFixedHeight(label_point_.y() + tmpLabel->height() * 2 + 2);
    }

}

bool ApplyFriendDlg::eventFilter(QObject *o, QEvent *event)
{
    if(o == ui->scrollArea)
    {
        if(event->type() == QEvent::Enter)
        {
            ui->scrollArea->verticalScrollBar()->setHidden(false);
        }
        else if(event->type() == QEvent::Leave)
        {
            ui->scrollArea->verticalScrollBar()->setHidden(true);
        }
    }
    return QDialog::eventFilter(o, event);
}

void ApplyFriendDlg::showMoreLabel()
{
    qDebug()<< "receive more label clicked";
    ui->more_label_widget->hide();

    ui->label_list_widget->setFixedWidth(325);
    tip_cur_point_ = QPoint(5, 5);
    auto next_point = tip_cur_point_;
    int textWidth;
    int textHeight;
    //重排现有的label
    for(auto & added_key : add_label_keys_){
        auto added_lb = add_labels_[added_key];

        QFontMetrics fontMetrics(added_lb->font()); // 获取QLabel控件的字体信息
        textWidth = fontMetrics.horizontalAdvance(added_lb->text()); // 获取文本的宽度
        textHeight = fontMetrics.height(); // 获取文本的高度

        if(tip_cur_point_.x() +textWidth + tip_offset > ui->label_list_widget->width()){
            tip_cur_point_.setX(tip_offset);
            tip_cur_point_.setY(tip_cur_point_.y()+textHeight+15);
        }
        added_lb->move(tip_cur_point_);

        next_point.setX(added_lb->pos().x() + textWidth + 15);
        next_point.setY(tip_cur_point_.y());

        tip_cur_point_ = next_point;

    }

    //添加未添加的
    for(int i = 0; i < tip_data_.size(); i++){
        auto iter = add_labels_.find(tip_data_[i]);
        if(iter != add_labels_.end()){
            continue;
        }

        auto* lb = new ClickedLabel(ui->label_list_widget);
        lb->setState("normal", "hover", "press", "select_normal",
                     "select_hover", "select_press");
        lb->setObjectName("TipsLabel");
        lb->setText(tip_data_[i]);
        connect(lb, &ClickedLabel::clicked, this, &ApplyFriendDlg::slot_change_friendLabel_by_tip);

        QFontMetrics fontMetrics(lb->font()); // 获取QLabel控件的字体信息
        int textWidth = fontMetrics.horizontalAdvance(lb->text()); // 获取文本的宽度
        int textHeight = fontMetrics.height(); // 获取文本的高度

        if (tip_cur_point_.x() + textWidth + tip_offset > ui->label_list_widget->width()) {

            tip_cur_point_.setX(tip_offset);
            tip_cur_point_.setY(tip_cur_point_.y() + textHeight + 15);

        }

        next_point = tip_cur_point_;

        addTipLabel(lb, tip_cur_point_, next_point, textWidth, textHeight);

        tip_cur_point_ = next_point;

    }

    int diff_height = next_point.y() + textHeight + tip_offset - ui->label_list_widget->height();
    ui->label_list_widget->setFixedHeight(next_point.y() + textHeight + tip_offset);

    //qDebug()<<"after resize ui->label_list_widget size is " <<  ui->label_list_widget->size();
    ui->scrollAreaWidgetContents->setFixedHeight(ui->scrollAreaWidgetContents->height()+diff_height);
}

void ApplyFriendDlg::slot_label_enter()
{
    if(ui->label_edit->text().isEmpty())
    {
        return;
    }
    this->addLabel(ui->label_edit->text());
    ui->tip_widget->hide();

    auto find_iter = std::find(tip_data_.begin(), tip_data_.end(), ui->label_edit->text());
    if(find_iter == tip_data_.end())
    {
        tip_data_.push_back(ui->label_edit->text());
    }
    //查找标签展示栏是否有该标签
    auto iter = add_labels_.find(ui->label_edit->text());
    if(iter != add_labels_.end())
    {
        iter.value()->setCurState(LabelState::Selected);
        return;
    }
    auto* label = new ClickedLabel(ui->label_list_widget);
    label->setState("normal", "hover", "press", "select_normal", "select_hover", "select_press");
    label->setObjectName("TipsLabel");
    label->setText(ui->label_edit->text());
    connect(label, &ClickedLabel::clicked, this, &ApplyFriendDlg::slot_change_friendLabel_by_tip);
    qDebug() << "ui->label_list_widget->width() is: " << ui->label_list_widget->width();
    qDebug() << "tip_cur_point_.x() is: " << tip_cur_point_.x();

    QFontMetrics fontMetrics(label->font());
    int textWidth = fontMetrics.horizontalAdvance(label->text());
    int textHeight = fontMetrics.height();
    qDebug() << "textWidth is: " << textWidth;

    if(tip_cur_point_.x() + textWidth + 3 > ui->label_list_widget->width())
    {
        tip_cur_point_.setX(5);
        tip_cur_point_.setY(tip_cur_point_.y() + textHeight + 15);
    }

    auto next_point = tip_cur_point_;
    addTipLabel(label, tip_cur_point_, next_point, textWidth, textHeight);
    tip_cur_point_ = next_point;

    int diff_height = next_point.y() + textHeight + tip_offset - ui->label_list_widget->height();
    ui->label_list_widget->setFixedHeight(next_point.y() + textHeight + tip_offset);

    label->setCurState(LabelState::Selected);
    ui->scrollAreaWidgetContents->setFixedHeight(ui->scrollAreaWidgetContents->height() + diff_height);
}

void ApplyFriendDlg::slot_remove_friendLabel(QString text)
{
    qDebug() << "receive close signal";

    label_point_.setX(2);
    label_point_.setY(6);

    auto iter = friend_labels_.find(text);

    if(iter == friend_labels_.end())
    {
        return;
    }
    //清除friend_label_keys_中对应的元素
    auto key = friend_label_keys_.end();
    for(auto it = friend_label_keys_.begin(); it != friend_label_keys_.end(); it++)
    {
        if(*it == text)
        {
            key = it;
            break;
        }
    }
    if(key != friend_label_keys_.end())
    {
        friend_label_keys_.erase(key);
    }
    //清除对应已添加的标签
    delete iter.value();
    friend_labels_.erase(iter);

    resetLabels();
    //将add_labels_中的标签设置为初始状态
    auto add_iter = add_labels_.find(text);
    if(add_iter != add_labels_.end())
    {
        add_iter.value()->resetNormalState();
    }
}

void ApplyFriendDlg::slot_change_friendLabel_by_tip(QString label_text, LabelState state)
{
    auto iter = add_labels_.find(label_text);
    if(iter == add_labels_.end())
    {
        return;
    }
    if(state == LabelState::Selected)
    {
        addLabel(label_text);
        return;
    }
    if(state == LabelState::Normal)
    {
        slot_remove_friendLabel(label_text);
        return;
    }
}

void ApplyFriendDlg::slot_label_text_change(const QString &text)
{
    if(text.isEmpty())
    {
        ui->tip_label->setText("");
        ui->tip_widget->hide();
        return;
    }
    auto iter = std::find(tip_data_.begin(), tip_data_.end(), text);
    if(iter == tip_data_.end())
    {
        auto new_text = add_prefix + text;
        ui->tip_label->setText(new_text);
        ui->tip_widget->show();
        return;
    }
    ui->tip_label->setText(text);
    ui->tip_widget->show();
}

void ApplyFriendDlg::slot_label_edit_finished()
{
    ui->tip_widget->hide();
}

void ApplyFriendDlg::slot_add_friendLabel_by_tip(QString text)
{
    int index = text.indexOf(add_prefix);
    if(index != -1)
    {
        text = text.mid(index + add_prefix.length());
    }
    addLabel(text);
    //标签展示栏也添加一个标签，并设置选中
    auto iter = std::find(tip_data_.begin(), tip_data_.end(), text);
    if(iter == tip_data_.end())
    {
        tip_data_.push_back(text);
    }
    //判断展示栏是否有该标签
    auto add_iter = add_labels_.find(text);
    if(add_iter != add_labels_.end())
    {
        add_iter.value()->setCurState(LabelState::Selected);
        return;
    }

    auto* label = new ClickedLabel(ui->label_list_widget);
    label->setState("normal", "hover", "press", "select_normal", "select_hover", "select_press");
    label->setObjectName("TipsLabel");
    label->setText(text);
    connect(label, &ClickedLabel::clicked, this, &ApplyFriendDlg::slot_change_friendLabel_by_tip);
    qDebug() << "ui->label_list_widget->width() is: " << ui->label_list_widget->width();
    qDebug() << "tip_cur_point_.x() is: " << tip_cur_point_.x();

    QFontMetrics fontMetrics(label->font());
    int textWidth = fontMetrics.horizontalAdvance(label->text());
    int textHeight = fontMetrics.height();
    qDebug() << "textWidth is: " << textWidth;

    if(tip_cur_point_.x() + textWidth + 3 > ui->label_list_widget->width())
    {
        tip_cur_point_.setX(5);
        tip_cur_point_.setY(tip_cur_point_.y() + textHeight + 15);
    }

    auto next_point = tip_cur_point_;
    addTipLabel(label, tip_cur_point_, next_point, textWidth, textHeight);
    tip_cur_point_ = next_point;

    int diff_height = next_point.y() + textHeight + tip_offset - ui->label_list_widget->height();
    ui->label_list_widget->setFixedHeight(next_point.y() + textHeight + tip_offset);

    label->setCurState(LabelState::Selected);
    ui->scrollAreaWidgetContents->setFixedHeight(ui->scrollAreaWidgetContents->height() + diff_height);
}

void ApplyFriendDlg::slot_apply_sure()
{

}

void ApplyFriendDlg::slot_apply_cancel()
{
    qDebug() << "Slot Apply Cancel";
    this->hide();
    deleteLater();
}
