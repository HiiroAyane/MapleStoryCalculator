#pragma once
#include "Defs.h"
#include "ui_Input.h"
#include "ui_MapleCalc.h"
using namespace Ui;

class EquipInput : public QDialog {
    Q_OBJECT;
public:
    EquipInput(QWidget* parent);
public:
    InputUi uii;
public slots:
    void onOkBtnClicked();
signals:
    void editEndClicked();
};

class MapleCalc : public QMainWindow {
    Q_OBJECT;
public:
    MapleCalc(QWidget* parent = Q_NULLPTR);
public slots:
    void onOpenTriggered();
    void onCreateTriggered();
    void onChangeTriggered();
    void onSaveTriggered();
    void onUpdateTriggered();
    void onLogTriggered();
    void onAboutTriggered();
    void onEditEndClicked();
private:
    void initWidget();
    void initDialog();
    void makeOutPut();
    void makeCompareOutput();
	QString makeNumber(qint64);
    void makeCharacter(EquipInput*);
    void remakeCharacter(EquipInput*);
    void openChFile(const QString&);
    QByteArray makeChFile();

	// 计算各种攻击力
	qreal  calcPercent(qint64 newValue, qint64 oldValue);
	qreal  getWeaponRatio(Profession p, Weapon w);
    qint64 calculateMain();  // 总主属性
    qint64 calculateSub();   // 总副属性
    qint64 calculateAtk();   // 总攻击力
	qint64 calcRealAtk();    // 实际攻击力
	qint64 calcPropertyAtk(qint64 realAtk);// 面板攻击力（包含了伤害、最终伤害）
	qint64 calcBossAtk(qint64 realAtk);    // 对BOSS等效攻击力,无防御
	qint64 calcNormalMAtk(qint64 realAtk); // 对普通怪物等效攻击力
	qint64 calc300DefAtk(qint64 realAtk);  // 对300%防御BOSS等效攻击力

private:
    // Ui组件;
    MapleCalcUi ui;
    EquipInput *eDlg = Q_NULLPTR;
    // 临时角色;
    Character tempCharacter;
    // 角色名
    QString m_fileName;
	// 属性和攻击力
	int allProperty = 0;
	int allAttack = 0;
    // 基准
    qint64 basePropertyAtk = 0;
    qint64 baseRealAtk     = 0;
    qint64 baseBossAtk     = 0;
    qint64 baseNormalMAtk  = 0;
    qint64 base300Atk      = 0;
	// 是否新建过或打开过
	bool m_CanBeChange = false;
	// 是否是更改角色
	bool m_Changed = false;
};
