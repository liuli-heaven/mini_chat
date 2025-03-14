#include "grouptipitem.h"
#include "ui_grouptipitem.h"

GroupTipItem::GroupTipItem(QWidget *parent)
    : ListItemBase(parent)
    , ui(new Ui::GroupTipItem), tip_("")
{
    ui->setupUi(this);
    SetItemType(ListItemType::GROUP_TIP_ITEM);
}

GroupTipItem::~GroupTipItem()
{
    delete ui;
}

void GroupTipItem::setGroupTip(QString str)
{
    tip_ = str;
    ui->label->setText(str);
}

QSize GroupTipItem::sizeHint() const
{
    return QSize(250, 30);
}
