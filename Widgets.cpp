#include "Widgets.h"
#include <qvalidator.h>

LineEdit::LineEdit(QWidget* parent)
	: QLineEdit(parent)
{
	setValidator(new QIntValidator(0, INT_MAX, this));
	//setFixedWidth(40);
}

ComboBox::ComboBox(QWidget* parent)
	: QComboBox(parent)
{
	QStringList list;
	list
		<< QString::fromUtf8("双弩精灵")
		<< QString::fromUtf8("冒险家英雄")
		<< QString::fromUtf8("其他冒险家")
		<< QString::fromUtf8("骑士团")
		<< QString::fromUtf8("尖兵")
		<< QString::fromUtf8("其他")
		<< QString::fromUtf8("恶魔复仇者")
		<< QString::fromUtf8("阴阳师");
	addItems(list);
}

WComboBox::WComboBox(QWidget* parent)
{
	QStringList list;
	list
		<< QString::fromUtf8("双弩枪")
		<< QString::fromUtf8("弓")
		<< QString::fromUtf8("弩")
		<< QString::fromUtf8("远古弓")
		<< QString::fromUtf8("龙息臂箭")
		<< QString::fromUtf8("双手剑")
		<< QString::fromUtf8("双手斧")
		<< QString::fromUtf8("双手钝器")
		<< QString::fromUtf8("枪")
		<< QString::fromUtf8("矛")
		<< QString::fromUtf8("拳套")
		<< QString::fromUtf8("指节")
		<< QString::fromUtf8("短枪")
		<< QString::fromUtf8("手持火炮")
		<< QString::fromUtf8("武士刀")
		<< QString::fromUtf8("折扇")
		<< QString::fromUtf8("大剑")
		<< QString::fromUtf8("太刀")
		<< QString::fromUtf8("机甲手枪")
		<< QString::fromUtf8("单手剑")
		<< QString::fromUtf8("单手斧")
		<< QString::fromUtf8("单手钝器")
		<< QString::fromUtf8("双头杖")
		<< QString::fromUtf8("灵魂手铳")
		<< QString::fromUtf8("亡命剑")
		<< QString::fromUtf8("能量剑")
		<< QString::fromUtf8("驯兽魔法棒")
		<< QString::fromUtf8("ESP限制器")
		<< QString::fromUtf8("锁链")
		<< QString::fromUtf8("魔力手套")
		<< QString::fromUtf8("扇子")
		<< QString::fromUtf8("短剑/短刀")
		<< QString::fromUtf8("手杖")
		<< QString::fromUtf8("短杖")
		<< QString::fromUtf8("长杖")
		<< QString::fromUtf8("调谐器")
		<< QString::fromUtf8("拳封");
	addItems(list);
}
