#include "MapleCalc.h"
#include <qscrollbar.h>
#include <qmessagebox.h>
#include <qfiledialog.h>

EquipInput::EquipInput(QWidget* parent)
    : QDialog(parent)
{
    uii.setupUi(this);
	uii.okBtn->setDefault(true);
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setWindowTitle(QString::fromUtf8("输入装备信息"));
    move(parentWidget()->mapToGlobal(parentWidget()->rect().center()) - QPoint(width() / 2, height() / 2));
    connect(uii.okBtn, SIGNAL(clicked()), this, SLOT(onOkBtnClicked()));
}
void EquipInput::onOkBtnClicked()
{
    if (uii.nameEdit->text().isEmpty()) {
        uii.nameEdit->setStyleSheet("border: 1px solid red");
        QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("角色名为必填!"), QMessageBox::Yes);
    }
    else {
        emit editEndClicked();
        hide();
    }
}

MapleCalc::MapleCalc(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
	ui.dispLabel->hide();
    connect(ui.action_create, SIGNAL(triggered()), this, SLOT(onCreateTriggered()));
    connect(ui.action_open, SIGNAL(triggered()), this, SLOT(onOpenTriggered()));
    connect(ui.action_change, SIGNAL(triggered()), this, SLOT(onChangeTriggered()));
    connect(ui.action_save, SIGNAL(triggered()), this, SLOT(onSaveTriggered()));
	connect(ui.action_update, SIGNAL(triggered()), this, SLOT(onUpdateTriggered()));
	connect(ui.action_updatelog, SIGNAL(triggered()), this, SLOT(onLogTriggered()));
	connect(ui.action_about, SIGNAL(triggered()), this, SLOT(onAboutTriggered()));
}

void MapleCalc::onCreateTriggered()
{
    initDialog();
    eDlg->exec();
	m_Changed = false;
    return;
}

void MapleCalc::onOpenTriggered()
{
    QString strCh = QFileDialog::getOpenFileName(this, QString::fromUtf8("选择要打开的ch文件"), QDir::currentPath(), "Ch Files(*.ch)");
    if (!strCh.isEmpty()) {
        openChFile(strCh);
    }
	return;
}

void MapleCalc::onChangeTriggered()
{
	if (!m_CanBeChange) {
		QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("请先打开角色或者新建角色!"), QMessageBox::Yes);
		return;
	}
    initDialog();
    remakeCharacter(eDlg);
	m_Changed = true;
    eDlg->exec();
}

void MapleCalc::onSaveTriggered()
{
    QFile file(m_fileName + ".ch");
    if (file.exists()) {
        QMessageBox::StandardButton btn = QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("角色已存在，是否覆盖？"), QMessageBox::Yes | QMessageBox::No);
        if (btn == QMessageBox::Yes) {
            if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) return;
        }
        else return;
    }
    else if (!file.open(QIODevice::WriteOnly)) return;
    file.write(makeChFile());
    file.close();
	QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("保存成功!"), QMessageBox::Yes);
	openChFile(m_fileName + ".ch");
}

void MapleCalc::onUpdateTriggered()
{
	// 没合适服务器，搁置
}

void MapleCalc::onLogTriggered()
{
	if (ui.dispLabel->isHidden()) ui.dispLabel->show();
	else ui.dispLabel->hide();

	QString strLog = QString::fromUtf8("v0.0.4_1 作者: _绯色幻音\n");
	strLog += QString::fromUtf8("①之前版本有保存角色的，把裸主属性加上（原等级的位置）。\n");
	strLog += QString::fromUtf8("②ver0.0.4 更改了显示，更加易用\n③添加了恶魔复仇者（测试版，主属性HP，副属性力量）。\n");
	strLog += QString::fromUtf8("④刀飞副属性有力量和敏捷，故可能面板有误差，\n");
	strLog += QString::fromUtf8("但不涉及副属性的时候，提升百分比是准确的。\n");
	strLog += QString::fromUtf8("⑤提升值仅以新建、打开或者保存了的角色为基准。\n");
	strLog += QString::fromUtf8("（不以上一次更改后的角色为基准计算提升）\n");
	strLog += QString::fromUtf8("⑥暂时忽略内在能力：按AP某属性增加另一属性%的情况\n");
	
	ui.dispLabel->setText(strLog);
}

void MapleCalc::onAboutTriggered()
{
	QString strAbout = "v0.0.4_1 作者: _绯色幻音\n感谢冒险岛BWiki提供数据\n";
	QMessageBox::information(this, QString::fromUtf8("关于"), strAbout, QMessageBox::Yes);
}

void MapleCalc::onEditEndClicked()
{
    makeCharacter(eDlg);
	m_CanBeChange = true;
	if (m_Changed) makeCompareOutput();
	else makeOutPut();
	return;
}

void MapleCalc::initWidget()
{
    resize(400, 300);
}

void MapleCalc::initDialog()
{
    if (eDlg) delete eDlg;
    eDlg = new EquipInput(this);
    connect(eDlg, SIGNAL(editEndClicked()), this, SLOT(onEditEndClicked()));
}

void MapleCalc::makeOutPut()
{
	QString strName = "版本：0.0.4 目前角色： " + m_fileName;
	ui.characterLabel->setText(strName);
	ui.dispLabel->hide();

	baseRealAtk = calcRealAtk();
	basePropertyAtk = calcPropertyAtk(baseRealAtk);
	baseNormalMAtk = calcNormalMAtk(baseRealAtk);
	baseBossAtk = calcBossAtk(baseRealAtk);
	base300Atk = calc300DefAtk(baseRealAtk);
	ui.propertyAtkLabel->setText("白字面板：" + makeNumber(basePropertyAtk));
	ui.realAtkLabel->setText("基础面板：" + makeNumber(baseRealAtk));
	ui.normalMAtkLabel->setText("对普通怪物等效面板：" + makeNumber(baseNormalMAtk));
	ui.bossAtkLabel->setText("对BOSS等效面板：" + makeNumber(baseBossAtk));
	ui.boss300AtkLabel->setText("对300%防御BOSS等效面板：" + makeNumber(base300Atk));
}

void MapleCalc::makeCompareOutput()
{
	QString strName = "版本：0.0.4 目前角色： " + m_fileName;
	ui.characterLabel->setText(strName);
	ui.dispLabel->hide();

	QString strOut;
	qint64 realAtk = calcRealAtk();
	qint64 propertyAtk = calcPropertyAtk(realAtk);
	qint64 normalMAtk = calcNormalMAtk(realAtk);
	qint64 bossAtk = calcBossAtk(realAtk);
	qint64 boss300Atk = calc300DefAtk(realAtk);

	strOut = "  ↑：" + makeNumber(propertyAtk - basePropertyAtk);
	strOut += " ↑：" + QString::number(calcPercent(propertyAtk, basePropertyAtk)) + "%";
	ui.propertyAtkLabel->setText("白字面板：" + makeNumber(propertyAtk) + strOut);
	strOut = "  ↑：" + QString::number(calcPercent(realAtk, baseRealAtk)) + "%";
	ui.realAtkLabel->setText("基础面板：" + makeNumber(realAtk) + strOut);
	strOut = "  ↑：" + QString::number(calcPercent(normalMAtk, baseNormalMAtk)) + "%";
	ui.normalMAtkLabel->setText("对普通怪物等效面板：" + makeNumber(normalMAtk) + strOut);
	strOut = "  ↑：" + QString::number(calcPercent(bossAtk, baseBossAtk)) + "%";
	ui.bossAtkLabel->setText("对BOSS等效面板：" + makeNumber(bossAtk) + strOut);
	strOut = "  ↑：" + QString::number(calcPercent(boss300Atk, base300Atk)) + "%";
	ui.boss300AtkLabel->setText("对300%防御BOSS等效面板：" + makeNumber(boss300Atk) + strOut);
}

QString MapleCalc::makeNumber(qint64 num)
{
	QString strTemp = QString::number(num);
	int nNum = strTemp.size();
	if (nNum >= 5) {
		strTemp.insert(strTemp.size() - 4, "w");
		if (nNum >= 10) strTemp.insert(strTemp.size() - 9, "e");
	}
	return strTemp;
}

void MapleCalc::makeCharacter(EquipInput* e)
{
    m_fileName = eDlg->uii.nameEdit->text();

    tempCharacter.profession = (Profession)e->uii.proComboBox->currentIndex();
	tempCharacter.weapon = (Weapon)e->uii.weaponComboBox->currentIndex();
	tempCharacter.pureMain = e->uii.pureMainEdit->text().toInt();
    tempCharacter.panel.DamagePercent = e->uii.damageEdit->text().toInt();
    tempCharacter.panel.BossDamage = e->uii.BossDamageEdit->text().toInt();
    tempCharacter.panel.FinalDamage = e->uii.finalDamageEdit->text().toInt();
    tempCharacter.panel.IgnoreDefence = e->uii.ignoreDefenseEdit->text().toInt();
    tempCharacter.panel.CriticalHit = e->uii.criticalEdit->text().toInt();
    tempCharacter.panel.CriticalDamage = e->uii.criticalDamageEdit->text().toInt();
	tempCharacter.panel.NormalMonsterDamage = e->uii.normalMonDamageEdit->text().toInt();

    tempCharacter.combine.Attack = e->uii.lineEdit_321->text().toInt();
    tempCharacter.combine.AttackPercent = e->uii.lineEdit_322->text().toInt();
    tempCharacter.combine.MainProperty = e->uii.lineEdit_323->text().toInt();
    tempCharacter.combine.MainPercent = e->uii.lineEdit_324->text().toInt();
    tempCharacter.combine.SubProperty = e->uii.lineEdit_325->text().toInt();
    tempCharacter.combine.SubPercent = e->uii.lineEdit_326->text().toInt();

    tempCharacter.MainWeapon.Attack = e->uii.lineEdit_11->text().toInt();
    tempCharacter.MainWeapon.AttackPercent = e->uii.lineEdit_12->text().toInt();
    tempCharacter.MainWeapon.MainProperty = e->uii.lineEdit_13->text().toInt();
    tempCharacter.MainWeapon.MainPercent = e->uii.lineEdit_14->text().toInt();
    tempCharacter.MainWeapon.SubProperty = e->uii.lineEdit_15->text().toInt();
    tempCharacter.MainWeapon.SubPercent = e->uii.lineEdit_16->text().toInt();

    tempCharacter.SubWeapon.Attack = e->uii.lineEdit_21->text().toInt();
    tempCharacter.SubWeapon.AttackPercent = e->uii.lineEdit_22->text().toInt();
    tempCharacter.SubWeapon.MainProperty = e->uii.lineEdit_23->text().toInt();
    tempCharacter.SubWeapon.MainPercent = e->uii.lineEdit_24->text().toInt();
    tempCharacter.SubWeapon.SubProperty = e->uii.lineEdit_25->text().toInt();
    tempCharacter.SubWeapon.SubPercent = e->uii.lineEdit_26->text().toInt();

    tempCharacter.Emblem.Attack = e->uii.lineEdit_31->text().toInt();
    tempCharacter.Emblem.AttackPercent = e->uii.lineEdit_32->text().toInt();
    tempCharacter.Emblem.MainProperty = e->uii.lineEdit_33->text().toInt();
    tempCharacter.Emblem.MainPercent = e->uii.lineEdit_34->text().toInt();
    tempCharacter.Emblem.SubProperty = e->uii.lineEdit_35->text().toInt();
    tempCharacter.Emblem.SubPercent = e->uii.lineEdit_36->text().toInt();

    tempCharacter.hat.Attack = e->uii.lineEdit_41->text().toInt();
    tempCharacter.hat.AttackPercent = e->uii.lineEdit_42->text().toInt();
    tempCharacter.hat.MainProperty = e->uii.lineEdit_43->text().toInt();
    tempCharacter.hat.MainPercent = e->uii.lineEdit_44->text().toInt();
    tempCharacter.hat.SubProperty = e->uii.lineEdit_45->text().toInt();
    tempCharacter.hat.SubPercent = e->uii.lineEdit_46->text().toInt();

    tempCharacter.top.Attack = e->uii.lineEdit_51->text().toInt();
    tempCharacter.top.AttackPercent = e->uii.lineEdit_52->text().toInt();
    tempCharacter.top.MainProperty = e->uii.lineEdit_53->text().toInt();
    tempCharacter.top.MainPercent = e->uii.lineEdit_54->text().toInt();
    tempCharacter.top.SubProperty = e->uii.lineEdit_55->text().toInt();
    tempCharacter.top.SubPercent = e->uii.lineEdit_56->text().toInt();

    tempCharacter.bottom.Attack = e->uii.lineEdit_61->text().toInt();
    tempCharacter.bottom.AttackPercent = e->uii.lineEdit_62->text().toInt();
    tempCharacter.bottom.MainProperty = e->uii.lineEdit_63->text().toInt();
    tempCharacter.bottom.MainPercent = e->uii.lineEdit_64->text().toInt();
    tempCharacter.bottom.SubProperty = e->uii.lineEdit_65->text().toInt();
    tempCharacter.bottom.SubPercent = e->uii.lineEdit_66->text().toInt();

    tempCharacter.shoes.Attack = e->uii.lineEdit_71->text().toInt();
    tempCharacter.shoes.AttackPercent = e->uii.lineEdit_72->text().toInt();
    tempCharacter.shoes.MainProperty = e->uii.lineEdit_73->text().toInt();
    tempCharacter.shoes.MainPercent = e->uii.lineEdit_74->text().toInt();
    tempCharacter.shoes.SubProperty = e->uii.lineEdit_75->text().toInt();
    tempCharacter.shoes.SubPercent = e->uii.lineEdit_76->text().toInt();

    tempCharacter.glove.Attack = e->uii.lineEdit_81->text().toInt();
    tempCharacter.glove.AttackPercent = e->uii.lineEdit_82->text().toInt();
    tempCharacter.glove.MainProperty = e->uii.lineEdit_83->text().toInt();
    tempCharacter.glove.MainPercent = e->uii.lineEdit_84->text().toInt();
    tempCharacter.glove.SubProperty = e->uii.lineEdit_85->text().toInt();
    tempCharacter.glove.SubPercent = e->uii.lineEdit_86->text().toInt();

    tempCharacter.cape.Attack = e->uii.lineEdit_91->text().toInt();
    tempCharacter.cape.AttackPercent = e->uii.lineEdit_92->text().toInt();
    tempCharacter.cape.MainProperty = e->uii.lineEdit_93->text().toInt();
    tempCharacter.cape.MainPercent = e->uii.lineEdit_94->text().toInt();
    tempCharacter.cape.SubProperty = e->uii.lineEdit_95->text().toInt();
    tempCharacter.cape.SubPercent = e->uii.lineEdit_96->text().toInt();

    tempCharacter.ring0.Attack = e->uii.lineEdit_101->text().toInt();
    tempCharacter.ring0.AttackPercent = e->uii.lineEdit_102->text().toInt();
    tempCharacter.ring0.MainProperty = e->uii.lineEdit_103->text().toInt();
    tempCharacter.ring0.MainPercent = e->uii.lineEdit_104->text().toInt();
    tempCharacter.ring0.SubProperty = e->uii.lineEdit_105->text().toInt();
    tempCharacter.ring0.SubPercent = e->uii.lineEdit_106->text().toInt();

    tempCharacter.ring1.Attack = e->uii.lineEdit_111->text().toInt();
    tempCharacter.ring1.AttackPercent = e->uii.lineEdit_112->text().toInt();
    tempCharacter.ring1.MainProperty = e->uii.lineEdit_113->text().toInt();
    tempCharacter.ring1.MainPercent = e->uii.lineEdit_114->text().toInt();
    tempCharacter.ring1.SubProperty = e->uii.lineEdit_115->text().toInt();
    tempCharacter.ring1.SubPercent = e->uii.lineEdit_116->text().toInt();

    tempCharacter.ring2.Attack = e->uii.lineEdit_121->text().toInt();
    tempCharacter.ring2.AttackPercent = e->uii.lineEdit_122->text().toInt();
    tempCharacter.ring2.MainProperty = e->uii.lineEdit_123->text().toInt();
    tempCharacter.ring2.MainPercent = e->uii.lineEdit_124->text().toInt();
    tempCharacter.ring2.SubProperty = e->uii.lineEdit_125->text().toInt();
    tempCharacter.ring2.SubPercent = e->uii.lineEdit_126->text().toInt();

    tempCharacter.ring3.Attack = e->uii.lineEdit_131->text().toInt();
    tempCharacter.ring3.AttackPercent = e->uii.lineEdit_132->text().toInt();
    tempCharacter.ring3.MainProperty = e->uii.lineEdit_133->text().toInt();
    tempCharacter.ring3.MainPercent = e->uii.lineEdit_134->text().toInt();
    tempCharacter.ring3.SubProperty = e->uii.lineEdit_135->text().toInt();
    tempCharacter.ring3.SubPercent = e->uii.lineEdit_136->text().toInt();

    tempCharacter.ring4.Attack = e->uii.lineEdit_141->text().toInt();
    tempCharacter.ring4.AttackPercent = e->uii.lineEdit_142->text().toInt();
    tempCharacter.ring4.MainProperty = e->uii.lineEdit_143->text().toInt();
    tempCharacter.ring4.MainPercent = e->uii.lineEdit_144->text().toInt();
    tempCharacter.ring4.SubProperty = e->uii.lineEdit_145->text().toInt();
    tempCharacter.ring4.SubPercent = e->uii.lineEdit_146->text().toInt();

    tempCharacter.ring5.Attack = e->uii.lineEdit_151->text().toInt();
    tempCharacter.ring5.AttackPercent = e->uii.lineEdit_152->text().toInt();
    tempCharacter.ring5.MainProperty = e->uii.lineEdit_153->text().toInt();
    tempCharacter.ring5.MainPercent = e->uii.lineEdit_154->text().toInt();
    tempCharacter.ring5.SubProperty = e->uii.lineEdit_155->text().toInt();
    tempCharacter.ring5.SubPercent = e->uii.lineEdit_156->text().toInt();

    tempCharacter.pendant0.Attack = e->uii.lineEdit_161->text().toInt();
    tempCharacter.pendant0.AttackPercent = e->uii.lineEdit_162->text().toInt();
    tempCharacter.pendant0.MainProperty = e->uii.lineEdit_163->text().toInt();
    tempCharacter.pendant0.MainPercent = e->uii.lineEdit_164->text().toInt();
    tempCharacter.pendant0.SubProperty = e->uii.lineEdit_165->text().toInt();
    tempCharacter.pendant0.SubPercent = e->uii.lineEdit_166->text().toInt();

    tempCharacter.pendant1.Attack = e->uii.lineEdit_171->text().toInt();
    tempCharacter.pendant1.AttackPercent = e->uii.lineEdit_172->text().toInt();
    tempCharacter.pendant1.MainProperty = e->uii.lineEdit_173->text().toInt();
    tempCharacter.pendant1.MainPercent = e->uii.lineEdit_174->text().toInt();
    tempCharacter.pendant1.SubProperty = e->uii.lineEdit_175->text().toInt();
    tempCharacter.pendant1.SubPercent = e->uii.lineEdit_176->text().toInt();

    tempCharacter.belt.Attack = e->uii.lineEdit_181->text().toInt();
    tempCharacter.belt.AttackPercent = e->uii.lineEdit_182->text().toInt();
    tempCharacter.belt.MainProperty = e->uii.lineEdit_183->text().toInt();
    tempCharacter.belt.MainPercent = e->uii.lineEdit_184->text().toInt();
    tempCharacter.belt.SubProperty = e->uii.lineEdit_185->text().toInt();
    tempCharacter.belt.SubPercent = e->uii.lineEdit_186->text().toInt();

    tempCharacter.faceAccessory.Attack = e->uii.lineEdit_191->text().toInt();
    tempCharacter.faceAccessory.AttackPercent = e->uii.lineEdit_192->text().toInt();
    tempCharacter.faceAccessory.MainProperty = e->uii.lineEdit_193->text().toInt();
    tempCharacter.faceAccessory.MainPercent = e->uii.lineEdit_194->text().toInt();
    tempCharacter.faceAccessory.SubProperty = e->uii.lineEdit_195->text().toInt();
    tempCharacter.faceAccessory.SubPercent = e->uii.lineEdit_196->text().toInt();

    tempCharacter.eyeDecoraion.Attack = e->uii.lineEdit_201->text().toInt();
    tempCharacter.eyeDecoraion.AttackPercent = e->uii.lineEdit_202->text().toInt();
    tempCharacter.eyeDecoraion.MainProperty = e->uii.lineEdit_203->text().toInt();
    tempCharacter.eyeDecoraion.MainPercent = e->uii.lineEdit_204->text().toInt();
    tempCharacter.eyeDecoraion.SubProperty = e->uii.lineEdit_205->text().toInt();
    tempCharacter.eyeDecoraion.SubPercent = e->uii.lineEdit_206->text().toInt();

    tempCharacter.earring.Attack = e->uii.lineEdit_211->text().toInt();
    tempCharacter.earring.AttackPercent = e->uii.lineEdit_212->text().toInt();
    tempCharacter.earring.MainProperty = e->uii.lineEdit_213->text().toInt();
    tempCharacter.earring.MainPercent = e->uii.lineEdit_214->text().toInt();
    tempCharacter.earring.SubProperty = e->uii.lineEdit_215->text().toInt();
    tempCharacter.earring.SubPercent = e->uii.lineEdit_216->text().toInt();

    tempCharacter.shoulderAccessory.Attack = e->uii.lineEdit_221->text().toInt();
    tempCharacter.shoulderAccessory.AttackPercent = e->uii.lineEdit_222->text().toInt();
    tempCharacter.shoulderAccessory.MainProperty = e->uii.lineEdit_223->text().toInt();
    tempCharacter.shoulderAccessory.MainPercent = e->uii.lineEdit_224->text().toInt();
    tempCharacter.shoulderAccessory.SubProperty = e->uii.lineEdit_225->text().toInt();
    tempCharacter.shoulderAccessory.SubPercent = e->uii.lineEdit_226->text().toInt();

    tempCharacter.pocketItem.Attack = e->uii.lineEdit_231->text().toInt();
    tempCharacter.pocketItem.AttackPercent = e->uii.lineEdit_232->text().toInt();
    tempCharacter.pocketItem.MainProperty = e->uii.lineEdit_233->text().toInt();
    tempCharacter.pocketItem.MainPercent = e->uii.lineEdit_234->text().toInt();
    tempCharacter.pocketItem.SubProperty = e->uii.lineEdit_235->text().toInt();
    tempCharacter.pocketItem.SubPercent = e->uii.lineEdit_236->text().toInt();

    tempCharacter.badge.Attack = e->uii.lineEdit_241->text().toInt();
    tempCharacter.badge.AttackPercent = e->uii.lineEdit_242->text().toInt();
    tempCharacter.badge.MainProperty = e->uii.lineEdit_243->text().toInt();
    tempCharacter.badge.MainPercent = e->uii.lineEdit_244->text().toInt();
    tempCharacter.badge.SubProperty = e->uii.lineEdit_245->text().toInt();
    tempCharacter.badge.SubPercent = e->uii.lineEdit_246->text().toInt();

    tempCharacter.medal.Attack = e->uii.lineEdit_251->text().toInt();
    tempCharacter.medal.AttackPercent = e->uii.lineEdit_252->text().toInt();
    tempCharacter.medal.MainProperty = e->uii.lineEdit_253->text().toInt();
    tempCharacter.medal.MainPercent = e->uii.lineEdit_254->text().toInt();
    tempCharacter.medal.SubProperty = e->uii.lineEdit_255->text().toInt();
    tempCharacter.medal.SubPercent = e->uii.lineEdit_256->text().toInt();

    tempCharacter.totem.Attack = e->uii.lineEdit_261->text().toInt();
    tempCharacter.totem.AttackPercent = e->uii.lineEdit_262->text().toInt();
    tempCharacter.totem.MainProperty = e->uii.lineEdit_263->text().toInt();
    tempCharacter.totem.MainPercent = e->uii.lineEdit_264->text().toInt();
    tempCharacter.totem.SubProperty = e->uii.lineEdit_265->text().toInt();
    tempCharacter.totem.SubPercent = e->uii.lineEdit_266->text().toInt();

    tempCharacter.robotHeart.Attack = e->uii.lineEdit_271->text().toInt();
    tempCharacter.robotHeart.AttackPercent = e->uii.lineEdit_272->text().toInt();
    tempCharacter.robotHeart.MainProperty = e->uii.lineEdit_273->text().toInt();
    tempCharacter.robotHeart.MainPercent = e->uii.lineEdit_274->text().toInt();
    tempCharacter.robotHeart.SubProperty = e->uii.lineEdit_275->text().toInt();
    tempCharacter.robotHeart.SubPercent = e->uii.lineEdit_276->text().toInt();

    tempCharacter.petEquipment.Attack = e->uii.lineEdit_281->text().toInt();
    tempCharacter.petEquipment.AttackPercent = e->uii.lineEdit_282->text().toInt();
    tempCharacter.petEquipment.MainProperty = e->uii.lineEdit_283->text().toInt();
    tempCharacter.petEquipment.MainPercent = e->uii.lineEdit_284->text().toInt();
    tempCharacter.petEquipment.SubProperty = e->uii.lineEdit_285->text().toInt();
    tempCharacter.petEquipment.SubPercent = e->uii.lineEdit_286->text().toInt();

    tempCharacter.petSkill.Attack = e->uii.lineEdit_291->text().toInt();
    tempCharacter.petSkill.AttackPercent = e->uii.lineEdit_292->text().toInt();
    tempCharacter.petSkill.MainProperty = e->uii.lineEdit_293->text().toInt();
    tempCharacter.petSkill.MainPercent = e->uii.lineEdit_294->text().toInt();
    tempCharacter.petSkill.SubProperty = e->uii.lineEdit_295->text().toInt();
    tempCharacter.petSkill.SubPercent = e->uii.lineEdit_296->text().toInt();

    tempCharacter.link.Attack = e->uii.lineEdit_301->text().toInt();
    tempCharacter.link.AttackPercent = e->uii.lineEdit_302->text().toInt();
    tempCharacter.link.MainProperty = e->uii.lineEdit_303->text().toInt();
    tempCharacter.link.MainPercent = e->uii.lineEdit_304->text().toInt();
    tempCharacter.link.SubProperty = e->uii.lineEdit_305->text().toInt();
    tempCharacter.link.SubPercent = e->uii.lineEdit_306->text().toInt();

    tempCharacter.cash.Attack = e->uii.lineEdit_311->text().toInt();
    tempCharacter.cash.AttackPercent = e->uii.lineEdit_312->text().toInt();
    tempCharacter.cash.MainProperty = e->uii.lineEdit_313->text().toInt();
    tempCharacter.cash.MainPercent = e->uii.lineEdit_314->text().toInt();
    tempCharacter.cash.SubProperty = e->uii.lineEdit_315->text().toInt();
    tempCharacter.cash.SubPercent = e->uii.lineEdit_316->text().toInt();

    tempCharacter.leagueCapture.Attack = e->uii.lineEdit_331->text().toInt();
    tempCharacter.leagueCapture.MainProperty = e->uii.lineEdit_333->text().toInt();
    tempCharacter.leagueCapture.SubProperty = e->uii.lineEdit_335->text().toInt();

    tempCharacter.leagueTeam.Attack = e->uii.lineEdit_341->text().toInt();
    tempCharacter.leagueTeam.AttackPercent = e->uii.lineEdit_342->text().toInt();
    tempCharacter.leagueTeam.MainProperty = e->uii.lineEdit_343->text().toInt();
    tempCharacter.leagueTeam.MainPercent = e->uii.lineEdit_344->text().toInt();
    tempCharacter.leagueTeam.SubProperty = e->uii.lineEdit_345->text().toInt();
    tempCharacter.leagueTeam.SubPercent = e->uii.lineEdit_346->text().toInt();

    tempCharacter.arc.MainProperty = e->uii.lineEdit_353->text().toInt();
    tempCharacter.auth.MainProperty = e->uii.lineEdit_363->text().toInt();

    tempCharacter.inner.Attack = e->uii.lineEdit_371->text().toInt();
    tempCharacter.inner.MainProperty = e->uii.lineEdit_373->text().toInt();
    tempCharacter.inner.SubProperty = e->uii.lineEdit_375->text().toInt();

    tempCharacter.hyper.Attack = e->uii.lineEdit_381->text().toInt();
    tempCharacter.hyper.MainProperty = e->uii.lineEdit_383->text().toInt();
    tempCharacter.hyper.SubProperty = e->uii.lineEdit_385->text().toInt();

    tempCharacter.monster.Attack = e->uii.lineEdit_391->text().toInt();
    tempCharacter.monster.AttackPercent = e->uii.lineEdit_392->text().toInt();
    tempCharacter.monster.MainProperty = e->uii.lineEdit_393->text().toInt();
    tempCharacter.monster.MainPercent = e->uii.lineEdit_394->text().toInt();
    tempCharacter.monster.SubProperty = e->uii.lineEdit_395->text().toInt();
    tempCharacter.monster.SubPercent = e->uii.lineEdit_396->text().toInt();

	tempCharacter.family.Attack = e->uii.lineEdit_401->text().toInt();
	tempCharacter.family.MainProperty = e->uii.lineEdit_403->text().toInt();
	tempCharacter.family.SubProperty = e->uii.lineEdit_405->text().toInt();

	tempCharacter.skill.Attack = e->uii.lineEdit_411->text().toInt();
	tempCharacter.skill.AttackPercent = e->uii.lineEdit_412->text().toInt();
	tempCharacter.skill.MainProperty = e->uii.lineEdit_413->text().toInt();
	tempCharacter.skill.MainPercent = e->uii.lineEdit_414->text().toInt();
	tempCharacter.skill.SubProperty = e->uii.lineEdit_415->text().toInt();
	tempCharacter.skill.SubPercent = e->uii.lineEdit_416->text().toInt();

	tempCharacter.title.Attack = e->uii.lineEdit_421->text().toInt();
	tempCharacter.title.AttackPercent = e->uii.lineEdit_422->text().toInt();
	tempCharacter.title.MainProperty = e->uii.lineEdit_423->text().toInt();
	tempCharacter.title.MainPercent = e->uii.lineEdit_424->text().toInt();
	tempCharacter.title.SubProperty = e->uii.lineEdit_425->text().toInt();
	tempCharacter.title.SubPercent = e->uii.lineEdit_426->text().toInt();

	tempCharacter.other.Attack = e->uii.lineEdit_431->text().toInt();
	tempCharacter.other.AttackPercent = e->uii.lineEdit_432->text().toInt();
	tempCharacter.other.MainProperty = e->uii.lineEdit_433->text().toInt();
	tempCharacter.other.MainPercent = e->uii.lineEdit_434->text().toInt();
	tempCharacter.other.SubProperty = e->uii.lineEdit_435->text().toInt();
	tempCharacter.other.SubPercent = e->uii.lineEdit_436->text().toInt();

    return;
}

void MapleCalc::remakeCharacter(EquipInput* e)
{
    eDlg->uii.nameEdit->setText(m_fileName);

    e->uii.proComboBox->setCurrentIndex((int)tempCharacter.profession);
	e->uii.weaponComboBox->setCurrentIndex((int)tempCharacter.weapon);
	e->uii.pureMainEdit->setText(QString::number(tempCharacter.pureMain));
    e->uii.damageEdit->setText(QString::number(tempCharacter.panel.DamagePercent));
    e->uii.BossDamageEdit->setText(QString::number(tempCharacter.panel.BossDamage));
    e->uii.finalDamageEdit->setText(QString::number(tempCharacter.panel.FinalDamage));
    e->uii.ignoreDefenseEdit->setText(QString::number(tempCharacter.panel.IgnoreDefence));
    e->uii.criticalEdit->setText(QString::number(tempCharacter.panel.CriticalHit));
    e->uii.criticalDamageEdit->setText(QString::number(tempCharacter.panel.CriticalDamage));
	e->uii.normalMonDamageEdit->setText(QString::number(tempCharacter.panel.NormalMonsterDamage));

    e->uii.lineEdit_321->setText(QString::number(tempCharacter.combine.Attack));
    e->uii.lineEdit_322->setText(QString::number(tempCharacter.combine.AttackPercent));
    e->uii.lineEdit_323->setText(QString::number(tempCharacter.combine.MainProperty));
    e->uii.lineEdit_324->setText(QString::number(tempCharacter.combine.MainPercent));
    e->uii.lineEdit_325->setText(QString::number(tempCharacter.combine.SubProperty));
    e->uii.lineEdit_326->setText(QString::number(tempCharacter.combine.SubPercent));

    e->uii.lineEdit_11->setText(QString::number(tempCharacter.MainWeapon.Attack));
    e->uii.lineEdit_12->setText(QString::number(tempCharacter.MainWeapon.AttackPercent));
    e->uii.lineEdit_13->setText(QString::number(tempCharacter.MainWeapon.MainProperty));
    e->uii.lineEdit_14->setText(QString::number(tempCharacter.MainWeapon.MainPercent));
    e->uii.lineEdit_15->setText(QString::number(tempCharacter.MainWeapon.SubProperty));
    e->uii.lineEdit_16->setText(QString::number(tempCharacter.MainWeapon.SubPercent));

    e->uii.lineEdit_21->setText(QString::number(tempCharacter.SubWeapon.Attack));
    e->uii.lineEdit_22->setText(QString::number(tempCharacter.SubWeapon.AttackPercent));
    e->uii.lineEdit_23->setText(QString::number(tempCharacter.SubWeapon.MainProperty));
    e->uii.lineEdit_24->setText(QString::number(tempCharacter.SubWeapon.MainPercent));
    e->uii.lineEdit_25->setText(QString::number(tempCharacter.SubWeapon.SubProperty));
    e->uii.lineEdit_26->setText(QString::number(tempCharacter.SubWeapon.SubPercent));

    e->uii.lineEdit_31->setText(QString::number(tempCharacter.Emblem.Attack));
    e->uii.lineEdit_32->setText(QString::number(tempCharacter.Emblem.AttackPercent));
    e->uii.lineEdit_33->setText(QString::number(tempCharacter.Emblem.MainProperty));
    e->uii.lineEdit_34->setText(QString::number(tempCharacter.Emblem.MainPercent));
    e->uii.lineEdit_35->setText(QString::number(tempCharacter.Emblem.SubProperty));
    e->uii.lineEdit_36->setText(QString::number(tempCharacter.Emblem.SubPercent));

    e->uii.lineEdit_41->setText(QString::number(tempCharacter.hat.Attack));
    e->uii.lineEdit_42->setText(QString::number(tempCharacter.hat.AttackPercent));
    e->uii.lineEdit_43->setText(QString::number(tempCharacter.hat.MainProperty));
    e->uii.lineEdit_44->setText(QString::number(tempCharacter.hat.MainPercent));
    e->uii.lineEdit_45->setText(QString::number(tempCharacter.hat.SubProperty));
    e->uii.lineEdit_46->setText(QString::number(tempCharacter.hat.SubPercent));

    e->uii.lineEdit_51->setText(QString::number(tempCharacter.top.Attack));
    e->uii.lineEdit_52->setText(QString::number(tempCharacter.top.AttackPercent));
    e->uii.lineEdit_53->setText(QString::number(tempCharacter.top.MainProperty));
    e->uii.lineEdit_54->setText(QString::number(tempCharacter.top.MainPercent));
    e->uii.lineEdit_55->setText(QString::number(tempCharacter.top.SubProperty));
    e->uii.lineEdit_56->setText(QString::number(tempCharacter.top.SubPercent));

    e->uii.lineEdit_61->setText(QString::number(tempCharacter.bottom.Attack));
    e->uii.lineEdit_62->setText(QString::number(tempCharacter.bottom.AttackPercent));
    e->uii.lineEdit_63->setText(QString::number(tempCharacter.bottom.MainProperty));
    e->uii.lineEdit_64->setText(QString::number(tempCharacter.bottom.MainPercent));
    e->uii.lineEdit_65->setText(QString::number(tempCharacter.bottom.SubProperty));
    e->uii.lineEdit_66->setText(QString::number(tempCharacter.bottom.SubPercent));

    e->uii.lineEdit_71->setText(QString::number(tempCharacter.shoes.Attack));
    e->uii.lineEdit_72->setText(QString::number(tempCharacter.shoes.AttackPercent));
    e->uii.lineEdit_73->setText(QString::number(tempCharacter.shoes.MainProperty));
    e->uii.lineEdit_74->setText(QString::number(tempCharacter.shoes.MainPercent));
    e->uii.lineEdit_75->setText(QString::number(tempCharacter.shoes.SubProperty));
    e->uii.lineEdit_76->setText(QString::number(tempCharacter.shoes.SubPercent));

    e->uii.lineEdit_81->setText(QString::number(tempCharacter.glove.Attack));
    e->uii.lineEdit_82->setText(QString::number(tempCharacter.glove.AttackPercent));
    e->uii.lineEdit_83->setText(QString::number(tempCharacter.glove.MainProperty));
    e->uii.lineEdit_84->setText(QString::number(tempCharacter.glove.MainPercent));
    e->uii.lineEdit_85->setText(QString::number(tempCharacter.glove.SubProperty));
    e->uii.lineEdit_86->setText(QString::number(tempCharacter.glove.SubPercent));

    e->uii.lineEdit_91->setText(QString::number(tempCharacter.cape.Attack));
    e->uii.lineEdit_92->setText(QString::number(tempCharacter.cape.AttackPercent));
    e->uii.lineEdit_93->setText(QString::number(tempCharacter.cape.MainProperty));
    e->uii.lineEdit_94->setText(QString::number(tempCharacter.cape.MainPercent));
    e->uii.lineEdit_95->setText(QString::number(tempCharacter.cape.SubProperty));
    e->uii.lineEdit_96->setText(QString::number(tempCharacter.cape.SubPercent));

    e->uii.lineEdit_101->setText(QString::number(tempCharacter.ring0.Attack));
    e->uii.lineEdit_102->setText(QString::number(tempCharacter.ring0.AttackPercent));
    e->uii.lineEdit_103->setText(QString::number(tempCharacter.ring0.MainProperty));
    e->uii.lineEdit_104->setText(QString::number(tempCharacter.ring0.MainPercent));
    e->uii.lineEdit_105->setText(QString::number(tempCharacter.ring0.SubProperty));
    e->uii.lineEdit_106->setText(QString::number(tempCharacter.ring0.SubPercent));

    e->uii.lineEdit_111->setText(QString::number(tempCharacter.ring1.Attack));
    e->uii.lineEdit_112->setText(QString::number(tempCharacter.ring1.AttackPercent));
    e->uii.lineEdit_113->setText(QString::number(tempCharacter.ring1.MainProperty));
    e->uii.lineEdit_114->setText(QString::number(tempCharacter.ring1.MainPercent));
    e->uii.lineEdit_115->setText(QString::number(tempCharacter.ring1.SubProperty));
    e->uii.lineEdit_116->setText(QString::number(tempCharacter.ring1.SubPercent));

    e->uii.lineEdit_121->setText(QString::number(tempCharacter.ring2.Attack));
    e->uii.lineEdit_122->setText(QString::number(tempCharacter.ring2.AttackPercent));
    e->uii.lineEdit_123->setText(QString::number(tempCharacter.ring2.MainProperty));
    e->uii.lineEdit_124->setText(QString::number(tempCharacter.ring2.MainPercent));
    e->uii.lineEdit_125->setText(QString::number(tempCharacter.ring2.SubProperty));
    e->uii.lineEdit_126->setText(QString::number(tempCharacter.ring2.SubPercent));

    e->uii.lineEdit_131->setText(QString::number(tempCharacter.ring3.Attack));
    e->uii.lineEdit_132->setText(QString::number(tempCharacter.ring3.AttackPercent));
    e->uii.lineEdit_133->setText(QString::number(tempCharacter.ring3.MainProperty));
    e->uii.lineEdit_134->setText(QString::number(tempCharacter.ring3.MainPercent));
    e->uii.lineEdit_135->setText(QString::number(tempCharacter.ring3.SubProperty));
    e->uii.lineEdit_136->setText(QString::number(tempCharacter.ring3.SubPercent));

    e->uii.lineEdit_141->setText(QString::number(tempCharacter.ring4.Attack));
    e->uii.lineEdit_142->setText(QString::number(tempCharacter.ring4.AttackPercent));
    e->uii.lineEdit_143->setText(QString::number(tempCharacter.ring4.MainProperty));
    e->uii.lineEdit_144->setText(QString::number(tempCharacter.ring4.MainPercent));
    e->uii.lineEdit_145->setText(QString::number(tempCharacter.ring4.SubProperty));
    e->uii.lineEdit_146->setText(QString::number(tempCharacter.ring4.SubPercent));

    e->uii.lineEdit_151->setText(QString::number(tempCharacter.ring5.Attack));
    e->uii.lineEdit_152->setText(QString::number(tempCharacter.ring5.AttackPercent));
    e->uii.lineEdit_153->setText(QString::number(tempCharacter.ring5.MainProperty));
    e->uii.lineEdit_154->setText(QString::number(tempCharacter.ring5.MainPercent));
    e->uii.lineEdit_155->setText(QString::number(tempCharacter.ring5.SubProperty));
    e->uii.lineEdit_156->setText(QString::number(tempCharacter.ring5.SubPercent));

    e->uii.lineEdit_161->setText(QString::number(tempCharacter.pendant0.Attack));
    e->uii.lineEdit_162->setText(QString::number(tempCharacter.pendant0.AttackPercent));
    e->uii.lineEdit_163->setText(QString::number(tempCharacter.pendant0.MainProperty));
    e->uii.lineEdit_164->setText(QString::number(tempCharacter.pendant0.MainPercent));
    e->uii.lineEdit_165->setText(QString::number(tempCharacter.pendant0.SubProperty));
    e->uii.lineEdit_166->setText(QString::number(tempCharacter.pendant0.SubPercent));

    e->uii.lineEdit_171->setText(QString::number(tempCharacter.pendant1.Attack));
    e->uii.lineEdit_172->setText(QString::number(tempCharacter.pendant1.AttackPercent));
    e->uii.lineEdit_173->setText(QString::number(tempCharacter.pendant1.MainProperty));
    e->uii.lineEdit_174->setText(QString::number(tempCharacter.pendant1.MainPercent));
    e->uii.lineEdit_175->setText(QString::number(tempCharacter.pendant1.SubProperty));
    e->uii.lineEdit_176->setText(QString::number(tempCharacter.pendant1.SubPercent));

    e->uii.lineEdit_181->setText(QString::number(tempCharacter.belt.Attack));
    e->uii.lineEdit_182->setText(QString::number(tempCharacter.belt.AttackPercent));
    e->uii.lineEdit_183->setText(QString::number(tempCharacter.belt.MainProperty));
    e->uii.lineEdit_184->setText(QString::number(tempCharacter.belt.MainPercent));
    e->uii.lineEdit_185->setText(QString::number(tempCharacter.belt.SubProperty));
    e->uii.lineEdit_186->setText(QString::number(tempCharacter.belt.SubPercent));

    e->uii.lineEdit_191->setText(QString::number(tempCharacter.faceAccessory.Attack));
    e->uii.lineEdit_192->setText(QString::number(tempCharacter.faceAccessory.AttackPercent));
    e->uii.lineEdit_193->setText(QString::number(tempCharacter.faceAccessory.MainProperty));
    e->uii.lineEdit_194->setText(QString::number(tempCharacter.faceAccessory.MainPercent));
    e->uii.lineEdit_195->setText(QString::number(tempCharacter.faceAccessory.SubProperty));
    e->uii.lineEdit_196->setText(QString::number(tempCharacter.faceAccessory.SubPercent));

    e->uii.lineEdit_201->setText(QString::number(tempCharacter.eyeDecoraion.Attack));
    e->uii.lineEdit_202->setText(QString::number(tempCharacter.eyeDecoraion.AttackPercent));
    e->uii.lineEdit_203->setText(QString::number(tempCharacter.eyeDecoraion.MainProperty));
    e->uii.lineEdit_204->setText(QString::number(tempCharacter.eyeDecoraion.MainPercent));
    e->uii.lineEdit_205->setText(QString::number(tempCharacter.eyeDecoraion.SubProperty));
    e->uii.lineEdit_206->setText(QString::number(tempCharacter.eyeDecoraion.SubPercent));

    e->uii.lineEdit_211->setText(QString::number(tempCharacter.earring.Attack));
    e->uii.lineEdit_212->setText(QString::number(tempCharacter.earring.AttackPercent));
    e->uii.lineEdit_213->setText(QString::number(tempCharacter.earring.MainProperty));
    e->uii.lineEdit_214->setText(QString::number(tempCharacter.earring.MainPercent));
    e->uii.lineEdit_215->setText(QString::number(tempCharacter.earring.SubProperty));
    e->uii.lineEdit_216->setText(QString::number(tempCharacter.earring.SubPercent));

    e->uii.lineEdit_221->setText(QString::number(tempCharacter.shoulderAccessory.Attack));
    e->uii.lineEdit_222->setText(QString::number(tempCharacter.shoulderAccessory.AttackPercent));
    e->uii.lineEdit_223->setText(QString::number(tempCharacter.shoulderAccessory.MainProperty));
    e->uii.lineEdit_224->setText(QString::number(tempCharacter.shoulderAccessory.MainPercent));
    e->uii.lineEdit_225->setText(QString::number(tempCharacter.shoulderAccessory.SubProperty));
    e->uii.lineEdit_226->setText(QString::number(tempCharacter.shoulderAccessory.SubPercent));

    e->uii.lineEdit_231->setText(QString::number(tempCharacter.pocketItem.Attack));
    e->uii.lineEdit_232->setText(QString::number(tempCharacter.pocketItem.AttackPercent));
    e->uii.lineEdit_233->setText(QString::number(tempCharacter.pocketItem.MainProperty));
    e->uii.lineEdit_234->setText(QString::number(tempCharacter.pocketItem.MainPercent));
    e->uii.lineEdit_235->setText(QString::number(tempCharacter.pocketItem.SubProperty));
    e->uii.lineEdit_236->setText(QString::number(tempCharacter.pocketItem.SubPercent));

    e->uii.lineEdit_241->setText(QString::number(tempCharacter.badge.Attack));
    e->uii.lineEdit_242->setText(QString::number(tempCharacter.badge.AttackPercent));
    e->uii.lineEdit_243->setText(QString::number(tempCharacter.badge.MainProperty));
    e->uii.lineEdit_244->setText(QString::number(tempCharacter.badge.MainPercent));
    e->uii.lineEdit_245->setText(QString::number(tempCharacter.badge.SubProperty));
    e->uii.lineEdit_246->setText(QString::number(tempCharacter.badge.SubPercent));

    e->uii.lineEdit_251->setText(QString::number(tempCharacter.medal.Attack));
    e->uii.lineEdit_252->setText(QString::number(tempCharacter.medal.AttackPercent));
    e->uii.lineEdit_253->setText(QString::number(tempCharacter.medal.MainProperty));
    e->uii.lineEdit_254->setText(QString::number(tempCharacter.medal.MainPercent));
    e->uii.lineEdit_255->setText(QString::number(tempCharacter.medal.SubProperty));
    e->uii.lineEdit_256->setText(QString::number(tempCharacter.medal.SubPercent));

    e->uii.lineEdit_261->setText(QString::number(tempCharacter.totem.Attack));
    e->uii.lineEdit_262->setText(QString::number(tempCharacter.totem.AttackPercent));
    e->uii.lineEdit_263->setText(QString::number(tempCharacter.totem.MainProperty));
    e->uii.lineEdit_264->setText(QString::number(tempCharacter.totem.MainPercent));
    e->uii.lineEdit_265->setText(QString::number(tempCharacter.totem.SubProperty));
    e->uii.lineEdit_266->setText(QString::number(tempCharacter.totem.SubPercent));

    e->uii.lineEdit_271->setText(QString::number(tempCharacter.robotHeart.Attack));
    e->uii.lineEdit_272->setText(QString::number(tempCharacter.robotHeart.AttackPercent));
    e->uii.lineEdit_273->setText(QString::number(tempCharacter.robotHeart.MainProperty));
    e->uii.lineEdit_274->setText(QString::number(tempCharacter.robotHeart.MainPercent));
    e->uii.lineEdit_275->setText(QString::number(tempCharacter.robotHeart.SubProperty));
    e->uii.lineEdit_276->setText(QString::number(tempCharacter.robotHeart.SubPercent));

    e->uii.lineEdit_281->setText(QString::number(tempCharacter.petEquipment.Attack));
    e->uii.lineEdit_282->setText(QString::number(tempCharacter.petEquipment.AttackPercent));
    e->uii.lineEdit_283->setText(QString::number(tempCharacter.petEquipment.MainProperty));
    e->uii.lineEdit_284->setText(QString::number(tempCharacter.petEquipment.MainPercent));
    e->uii.lineEdit_285->setText(QString::number(tempCharacter.petEquipment.SubProperty));
    e->uii.lineEdit_286->setText(QString::number(tempCharacter.petEquipment.SubPercent));

    e->uii.lineEdit_291->setText(QString::number(tempCharacter.petSkill.Attack));
    e->uii.lineEdit_292->setText(QString::number(tempCharacter.petSkill.AttackPercent));
    e->uii.lineEdit_293->setText(QString::number(tempCharacter.petSkill.MainProperty));
    e->uii.lineEdit_294->setText(QString::number(tempCharacter.petSkill.MainPercent));
    e->uii.lineEdit_295->setText(QString::number(tempCharacter.petSkill.SubProperty));
    e->uii.lineEdit_296->setText(QString::number(tempCharacter.petSkill.SubPercent));
    
    e->uii.lineEdit_301->setText(QString::number(tempCharacter.link.Attack));
    e->uii.lineEdit_302->setText(QString::number(tempCharacter.link.AttackPercent));
    e->uii.lineEdit_303->setText(QString::number(tempCharacter.link.MainProperty));
    e->uii.lineEdit_304->setText(QString::number(tempCharacter.link.MainPercent));
    e->uii.lineEdit_305->setText(QString::number(tempCharacter.link.SubProperty));
    e->uii.lineEdit_306->setText(QString::number(tempCharacter.link.SubPercent));

    e->uii.lineEdit_311->setText(QString::number(tempCharacter.cash.Attack));
    e->uii.lineEdit_312->setText(QString::number(tempCharacter.cash.AttackPercent));
    e->uii.lineEdit_313->setText(QString::number(tempCharacter.cash.MainProperty));
    e->uii.lineEdit_314->setText(QString::number(tempCharacter.cash.MainPercent));
    e->uii.lineEdit_315->setText(QString::number(tempCharacter.cash.SubProperty));
    e->uii.lineEdit_316->setText(QString::number(tempCharacter.cash.SubPercent));

    e->uii.lineEdit_331->setText(QString::number(tempCharacter.leagueCapture.Attack));
    e->uii.lineEdit_333->setText(QString::number(tempCharacter.leagueCapture.MainProperty));
    e->uii.lineEdit_335->setText(QString::number(tempCharacter.leagueCapture.SubProperty));

    e->uii.lineEdit_341->setText(QString::number(tempCharacter.leagueTeam.Attack));
    e->uii.lineEdit_342->setText(QString::number(tempCharacter.leagueTeam.AttackPercent));
    e->uii.lineEdit_343->setText(QString::number(tempCharacter.leagueTeam.MainProperty));
    e->uii.lineEdit_344->setText(QString::number(tempCharacter.leagueTeam.MainPercent));
    e->uii.lineEdit_345->setText(QString::number(tempCharacter.leagueTeam.SubProperty));
    e->uii.lineEdit_346->setText(QString::number(tempCharacter.leagueTeam.SubPercent));

    e->uii.lineEdit_353->setText(QString::number(tempCharacter.arc.MainProperty));
    e->uii.lineEdit_363->setText(QString::number(tempCharacter.auth.MainProperty));

    e->uii.lineEdit_371->setText(QString::number(tempCharacter.inner.Attack));
    e->uii.lineEdit_373->setText(QString::number(tempCharacter.inner.MainProperty));
    e->uii.lineEdit_375->setText(QString::number(tempCharacter.inner.SubProperty));

    e->uii.lineEdit_381->setText(QString::number(tempCharacter.hyper.Attack));
    e->uii.lineEdit_383->setText(QString::number(tempCharacter.hyper.MainProperty));
    e->uii.lineEdit_385->setText(QString::number(tempCharacter.hyper.SubProperty));

    e->uii.lineEdit_391->setText(QString::number(tempCharacter.monster.Attack));
    e->uii.lineEdit_392->setText(QString::number(tempCharacter.monster.AttackPercent));
    e->uii.lineEdit_393->setText(QString::number(tempCharacter.monster.MainProperty));
    e->uii.lineEdit_394->setText(QString::number(tempCharacter.monster.MainPercent));
    e->uii.lineEdit_395->setText(QString::number(tempCharacter.monster.SubProperty));
    e->uii.lineEdit_396->setText(QString::number(tempCharacter.monster.SubPercent));

	e->uii.lineEdit_401->setText(QString::number(tempCharacter.family.Attack));
	e->uii.lineEdit_403->setText(QString::number(tempCharacter.family.MainProperty));
	e->uii.lineEdit_405->setText(QString::number(tempCharacter.family.SubProperty));

	e->uii.lineEdit_411->setText(QString::number(tempCharacter.skill.Attack));
	e->uii.lineEdit_412->setText(QString::number(tempCharacter.skill.AttackPercent));
	e->uii.lineEdit_413->setText(QString::number(tempCharacter.skill.MainProperty));
	e->uii.lineEdit_414->setText(QString::number(tempCharacter.skill.MainPercent));
	e->uii.lineEdit_415->setText(QString::number(tempCharacter.skill.SubProperty));
	e->uii.lineEdit_416->setText(QString::number(tempCharacter.skill.SubPercent));

	e->uii.lineEdit_421->setText(QString::number(tempCharacter.title.Attack));
	e->uii.lineEdit_422->setText(QString::number(tempCharacter.title.AttackPercent));
	e->uii.lineEdit_423->setText(QString::number(tempCharacter.title.MainProperty));
	e->uii.lineEdit_424->setText(QString::number(tempCharacter.title.MainPercent));
	e->uii.lineEdit_425->setText(QString::number(tempCharacter.title.SubProperty));
	e->uii.lineEdit_426->setText(QString::number(tempCharacter.title.SubPercent));

	e->uii.lineEdit_431->setText(QString::number(tempCharacter.other.Attack));
	e->uii.lineEdit_432->setText(QString::number(tempCharacter.other.AttackPercent));
	e->uii.lineEdit_433->setText(QString::number(tempCharacter.other.MainProperty));
	e->uii.lineEdit_434->setText(QString::number(tempCharacter.other.MainPercent));
	e->uii.lineEdit_435->setText(QString::number(tempCharacter.other.SubProperty));
	e->uii.lineEdit_436->setText(QString::number(tempCharacter.other.SubPercent));

    return;
}

void MapleCalc::openChFile(const QString& strCh)
{
	m_CanBeChange = true;
	m_Changed = false;

	QFile file(strCh);
	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8("读取文件失败!"), QMessageBox::Yes);
		return;
	}

	m_fileName = strCh.mid(0, strCh.lastIndexOf("."));
	m_fileName = m_fileName.mid(strCh.lastIndexOf("/") + 1);

	for (int i = 0; !file.atEnd(); i++) {
		int value = QString(file.readLine()).toInt();
		switch (i)
		{
		case 0:
			tempCharacter.profession = (Profession)value;
			break;
		case 1:
			tempCharacter.pureMain = value;
			break;
		case 2:
			tempCharacter.panel.DamagePercent = value;
			break;
		case 3:
			tempCharacter.panel.BossDamage = value;
			break;
		case 4:
			tempCharacter.panel.FinalDamage = value;
			break;
		case 5:
			tempCharacter.panel.IgnoreDefence = value;
			break;
		case 6:
			tempCharacter.panel.CriticalHit = value;
			break;
		case 7:
			tempCharacter.panel.CriticalDamage = value;
			break;
		case 8:
			tempCharacter.panel.NormalMonsterDamage = value;
			break;
		case 9:
			tempCharacter.combine.Attack = value;
			break;
		case 10:
			tempCharacter.combine.AttackPercent = value;
			break;
		case 11:
			tempCharacter.combine.MainProperty = value;
			break;
		case 12:
			tempCharacter.combine.MainPercent = value;
			break;
		case 13:
			tempCharacter.combine.SubProperty = value;
			break;
		case 14:
			tempCharacter.combine.SubPercent = value;
			break;
		case 15:
			tempCharacter.MainWeapon.Attack = value;
			break;
		case 16:
			tempCharacter.MainWeapon.AttackPercent = value;
			break;
		case 17:
			tempCharacter.MainWeapon.MainProperty = value;
			break;
		case 18:
			tempCharacter.MainWeapon.MainPercent = value;
			break;
		case 19:
			tempCharacter.MainWeapon.SubProperty = value;
			break;
		case 20:
			tempCharacter.MainWeapon.SubPercent = value;
			break;
		case 21:
			tempCharacter.SubWeapon.Attack = value;
			break;
		case 22:
			tempCharacter.SubWeapon.AttackPercent = value;
			break;
		case 23:
			tempCharacter.SubWeapon.MainProperty = value;
			break;
		case 24:
			tempCharacter.SubWeapon.MainPercent = value;
			break;
		case 25:
			tempCharacter.SubWeapon.SubProperty = value;
			break;
		case 26:
			tempCharacter.SubWeapon.SubPercent = value;
			break;
		case 27:
			tempCharacter.Emblem.Attack = value;
			break;
		case 28:
			tempCharacter.Emblem.AttackPercent = value;
			break;
		case 29:
			tempCharacter.Emblem.MainProperty = value;
			break;
		case 30:
			tempCharacter.Emblem.MainPercent = value;
			break;
		case 31:
			tempCharacter.Emblem.SubProperty = value;
			break;
		case 32:
			tempCharacter.Emblem.SubPercent = value;
			break;
		case 33:
			tempCharacter.hat.Attack = value;
			break;
		case 34:
			tempCharacter.hat.AttackPercent = value;
			break;
		case 35:
			tempCharacter.hat.MainProperty = value;
			break;
		case 36:
			tempCharacter.hat.MainPercent = value;
			break;
		case 37:
			tempCharacter.hat.SubProperty = value;
			break;
		case 38:
			tempCharacter.hat.SubPercent = value;
			break;
		case 39:
			tempCharacter.top.Attack = value;
			break;
		case 40:
			tempCharacter.top.AttackPercent = value;
			break;
		case 41:
			tempCharacter.top.MainProperty = value;
			break;
		case 42:
			tempCharacter.top.MainPercent = value;
			break;
		case 43:
			tempCharacter.top.SubProperty = value;
			break;
		case 44:
			tempCharacter.top.SubPercent = value;
			break;
		case 45:
			tempCharacter.bottom.Attack = value;
			break;
		case 46:
			tempCharacter.bottom.AttackPercent = value;
			break;
		case 47:
			tempCharacter.bottom.MainProperty = value;
			break;
		case 48:
			tempCharacter.bottom.MainPercent = value;
			break;
		case 49:
			tempCharacter.bottom.SubProperty = value;
			break;
		case 50:
			tempCharacter.bottom.SubPercent = value;
			break;
		case 51:
			tempCharacter.shoes.Attack = value;
			break;
		case 52:
			tempCharacter.shoes.AttackPercent = value;
			break;
		case 53:
			tempCharacter.shoes.MainProperty = value;
			break;
		case 54:
			tempCharacter.shoes.MainPercent = value;
			break;
		case 55:
			tempCharacter.shoes.SubProperty = value;
			break;
		case 56:
			tempCharacter.shoes.SubPercent = value;
			break;
		case 57:
			tempCharacter.glove.Attack = value;
			break;
		case 58:
			tempCharacter.glove.AttackPercent = value;
			break;
		case 59:
			tempCharacter.glove.MainProperty = value;
			break;
		case 60:
			tempCharacter.glove.MainPercent = value;
			break;
		case 61:
			tempCharacter.glove.SubProperty = value;
			break;
		case 62:
			tempCharacter.glove.SubPercent = value;
			break;
		case 63:
			tempCharacter.cape.Attack = value;
			break;
		case 64:
			tempCharacter.cape.AttackPercent = value;
			break;
		case 65:
			tempCharacter.cape.MainProperty = value;
			break;
		case 66:
			tempCharacter.cape.MainPercent = value;
			break;
		case 67:
			tempCharacter.cape.SubProperty = value;
			break;
		case 68:
			tempCharacter.cape.SubPercent = value;
			break;
		case 69:
			tempCharacter.ring0.Attack = value;
			break;
		case 70:
			tempCharacter.ring0.AttackPercent = value;
			break;
		case 71:
			tempCharacter.ring0.MainProperty = value;
			break;
		case 72:
			tempCharacter.ring0.MainPercent = value;
			break;
		case 73:
			tempCharacter.ring0.SubProperty = value;
			break;
		case 74:
			tempCharacter.ring0.SubPercent = value;
			break;
		case 75:
			tempCharacter.ring1.Attack = value;
			break;
		case 76:
			tempCharacter.ring1.AttackPercent = value;
			break;
		case 77:
			tempCharacter.ring1.MainProperty = value;
			break;
		case 78:
			tempCharacter.ring1.MainPercent = value;
			break;
		case 79:
			tempCharacter.ring1.SubProperty = value;
			break;
		case 80:
			tempCharacter.ring1.SubPercent = value;
			break;
		case 81:
			tempCharacter.ring2.Attack = value;
			break;
		case 82:
			tempCharacter.ring2.AttackPercent = value;
			break;
		case 83:
			tempCharacter.ring2.MainProperty = value;
			break;
		case 84:
			tempCharacter.ring2.MainPercent = value;
			break;
		case 85:
			tempCharacter.ring2.SubProperty = value;
			break;
		case 86:
			tempCharacter.ring2.SubPercent = value;
			break;
		case 87:
			tempCharacter.ring3.Attack = value;
			break;
		case 88:
			tempCharacter.ring3.AttackPercent = value;
			break;
		case 89:
			tempCharacter.ring3.MainProperty = value;
			break;
		case 90:
			tempCharacter.ring3.MainPercent = value;
			break;
		case 91:
			tempCharacter.ring3.SubProperty = value;
			break;
		case 92:
			tempCharacter.ring3.SubPercent = value;
			break;
		case 93:
			tempCharacter.ring4.Attack = value;
			break;
		case 94:
			tempCharacter.ring4.AttackPercent = value;
			break;
		case 95:
			tempCharacter.ring4.MainProperty = value;
			break;
		case 96:
			tempCharacter.ring4.MainPercent = value;
			break;
		case 97:
			tempCharacter.ring4.SubProperty = value;
			break;
		case 98:
			tempCharacter.ring4.SubPercent = value;
			break;
		case 99:
			tempCharacter.ring5.Attack = value;
			break;
		case 100:
			tempCharacter.ring5.AttackPercent = value;
			break;
		case 101:
			tempCharacter.ring5.MainProperty = value;
			break;
		case 102:
			tempCharacter.ring5.MainPercent = value;
			break;
		case 103:
			tempCharacter.ring5.SubProperty = value;
			break;
		case 104:
			tempCharacter.ring5.SubPercent = value;
			break;
		case 105:
			tempCharacter.pendant0.Attack = value;
			break;
		case 106:
			tempCharacter.pendant0.AttackPercent = value;
			break;
		case 107:
			tempCharacter.pendant0.MainProperty = value;
			break;
		case 108:
			tempCharacter.pendant0.MainPercent = value;
			break;
		case 109:
			tempCharacter.pendant0.SubProperty = value;
			break;
		case 110:
			tempCharacter.pendant0.SubPercent = value;
			break;
		case 111:
			tempCharacter.pendant1.Attack = value;
			break;
		case 112:
			tempCharacter.pendant1.AttackPercent = value;
			break;
		case 113:
			tempCharacter.pendant1.MainProperty = value;
			break;
		case 114:
			tempCharacter.pendant1.MainPercent = value;
			break;
		case 115:
			tempCharacter.pendant1.SubProperty = value;
			break;
		case 116:
			tempCharacter.pendant1.SubPercent = value;
			break;
		case 117:
			tempCharacter.belt.Attack = value;
			break;
		case 118:
			tempCharacter.belt.AttackPercent = value;
			break;
		case 119:
			tempCharacter.belt.MainProperty = value;
			break;
		case 120:
			tempCharacter.belt.MainPercent = value;
			break;
		case 121:
			tempCharacter.belt.SubProperty = value;
			break;
		case 122:
			tempCharacter.belt.SubPercent = value;
			break;
		case 123:
			tempCharacter.faceAccessory.Attack = value;
			break;
		case 124:
			tempCharacter.faceAccessory.AttackPercent = value;
			break;
		case 125:
			tempCharacter.faceAccessory.MainProperty = value;
			break;
		case 126:
			tempCharacter.faceAccessory.MainPercent = value;
			break;
		case 127:
			tempCharacter.faceAccessory.SubProperty = value;
			break;
		case 128:
			tempCharacter.faceAccessory.SubPercent = value;
			break;
		case 129:
			tempCharacter.eyeDecoraion.Attack = value;
			break;
		case 130:
			tempCharacter.eyeDecoraion.AttackPercent = value;
			break;
		case 131:
			tempCharacter.eyeDecoraion.MainProperty = value;
			break;
		case 132:
			tempCharacter.eyeDecoraion.MainPercent = value;
			break;
		case 133:
			tempCharacter.eyeDecoraion.SubProperty = value;
			break;
		case 134:
			tempCharacter.eyeDecoraion.SubPercent = value;
			break;
		case 135:
			tempCharacter.pocketItem.Attack = value;
			break;
		case 136:
			tempCharacter.pocketItem.AttackPercent = value;
			break;
		case 137:
			tempCharacter.pocketItem.MainProperty = value;
			break;
		case 138:
			tempCharacter.pocketItem.MainPercent = value;
			break;
		case 139:
			tempCharacter.pocketItem.SubProperty = value;
			break;
		case 140:
			tempCharacter.pocketItem.SubPercent = value;
			break;
		case 141:
			tempCharacter.earring.Attack = value;
			break;
		case 142:
			tempCharacter.earring.AttackPercent = value;
			break;
		case 143:
			tempCharacter.earring.MainProperty = value;
			break;
		case 144:
			tempCharacter.earring.MainPercent = value;
			break;
		case 145:
			tempCharacter.earring.SubProperty = value;
			break;
		case 146:
			tempCharacter.earring.SubPercent = value;
			break;
		case 147:
			tempCharacter.shoulderAccessory.Attack = value;
			break;
		case 148:
			tempCharacter.shoulderAccessory.AttackPercent = value;
			break;
		case 149:
			tempCharacter.shoulderAccessory.MainProperty = value;
			break;
		case 150:
			tempCharacter.shoulderAccessory.MainPercent = value;
			break;
		case 151:
			tempCharacter.shoulderAccessory.SubProperty = value;
			break;
		case 152:
			tempCharacter.badge.SubPercent = value;
			break;
		case 153:
			tempCharacter.badge.Attack = value;
			break;
		case 154:
			tempCharacter.badge.AttackPercent = value;
			break;
		case 155:
			tempCharacter.badge.MainProperty = value;
			break;
		case 156:
			tempCharacter.badge.MainPercent = value;
			break;
		case 157:
			tempCharacter.badge.SubProperty = value;
			break;
		case 158:
			tempCharacter.badge.SubPercent = value;
			break;
		case 159:
			tempCharacter.medal.Attack = value;
			break;
		case 160:
			tempCharacter.medal.AttackPercent = value;
			break;
		case 161:
			tempCharacter.medal.MainProperty = value;
			break;
		case 162:
			tempCharacter.medal.MainPercent = value;
			break;
		case 163:
			tempCharacter.medal.SubProperty = value;
			break;
		case 164:
			tempCharacter.medal.SubPercent = value;
			break;
		case 165:
			tempCharacter.totem.Attack = value;
			break;
		case 166:
			tempCharacter.totem.AttackPercent = value;
			break;
		case 167:
			tempCharacter.totem.MainProperty = value;
			break;
		case 168:
			tempCharacter.totem.MainPercent = value;
			break;
		case 169:
			tempCharacter.totem.SubProperty = value;
			break;
		case 170:
			tempCharacter.totem.SubPercent = value;
			break;
		case 171:
			tempCharacter.robotHeart.Attack = value;
			break;
		case 172:
			tempCharacter.robotHeart.AttackPercent = value;
			break;
		case 173:
			tempCharacter.robotHeart.MainProperty = value;
			break;
		case 174:
			tempCharacter.robotHeart.MainPercent = value;
			break;
		case 175:
			tempCharacter.robotHeart.SubProperty = value;
			break;
		case 176:
			tempCharacter.robotHeart.SubPercent = value;
			break;
		case 177:
			tempCharacter.petEquipment.Attack = value;
			break;
		case 178:
			tempCharacter.petEquipment.AttackPercent = value;
			break;
		case 179:
			tempCharacter.petEquipment.MainProperty = value;
			break;
		case 180:
			tempCharacter.petEquipment.MainPercent = value;
			break;
		case 181:
			tempCharacter.petEquipment.SubProperty = value;
			break;
		case 182:
			tempCharacter.petEquipment.SubPercent = value;
			break;
		case 183:
			tempCharacter.petSkill.Attack = value;
			break;
		case 184:
			tempCharacter.petSkill.AttackPercent = value;
			break;
		case 185:
			tempCharacter.petSkill.MainProperty = value;
			break;
		case 186:
			tempCharacter.petSkill.MainPercent = value;
			break;
		case 187:
			tempCharacter.petSkill.SubProperty = value;
			break;
		case 188:
			tempCharacter.petSkill.SubPercent = value;
			break;
		case 189:
			tempCharacter.link.Attack = value;
			break;
		case 190:
			tempCharacter.link.AttackPercent = value;
			break;
		case 191:
			tempCharacter.link.MainProperty = value;
			break;
		case 192:
			tempCharacter.link.MainPercent = value;
			break;
		case 193:
			tempCharacter.link.SubProperty = value;
			break;
		case 194:
			tempCharacter.link.SubPercent = value;
			break;
		case 195:
			tempCharacter.cash.Attack = value;
			break;
		case 196:
			tempCharacter.cash.AttackPercent = value;
			break;
		case 197:
			tempCharacter.cash.MainProperty = value;
			break;
		case 198:
			tempCharacter.cash.MainPercent = value;
			break;
		case 199:
			tempCharacter.cash.SubProperty = value;
			break;
		case 200:
			tempCharacter.cash.SubPercent = value;
			break;
		case 201:
			tempCharacter.leagueCapture.Attack = value;
			break;
		case 202:
			tempCharacter.leagueCapture.MainProperty = value;
			break;
		case 203:
			tempCharacter.leagueCapture.SubProperty = value;
			break;
		case 204:
			tempCharacter.leagueTeam.Attack = value;
			break;
		case 205:
			tempCharacter.leagueTeam.AttackPercent = value;
			break;
		case 206:
			tempCharacter.leagueTeam.MainProperty = value;
			break;
		case 207:
			tempCharacter.leagueTeam.MainPercent = value;
			break;
		case 208:
			tempCharacter.leagueTeam.SubProperty = value;
			break;
		case 209:
			tempCharacter.leagueTeam.SubPercent = value;
			break;
		case 210:
			tempCharacter.arc.MainProperty = value;
			break;
		case 211:
			tempCharacter.auth.MainProperty = value;
			break;
		case 212:
			tempCharacter.inner.Attack = value;
			break;
		case 213:
			tempCharacter.inner.MainProperty = value;
			break;
		case 214:
			tempCharacter.inner.SubProperty = value;
			break;
		case 215:
			tempCharacter.hyper.Attack = value;
			break;
		case 216:
			tempCharacter.hyper.MainProperty = value;
			break;
		case 217:
			tempCharacter.hyper.SubProperty = value;
			break;
		case 218:
			tempCharacter.monster.Attack = value;
			break;
		case 219:
			tempCharacter.monster.AttackPercent = value;
			break;
		case 220:
			tempCharacter.monster.MainProperty = value;
			break;
		case 221:
			tempCharacter.monster.MainPercent = value;
			break;
		case 222:
			tempCharacter.monster.SubProperty = value;
			break;
		case 223:
			tempCharacter.monster.SubPercent = value;
			break;
		case 224:
			tempCharacter.family.Attack = value;
			break;
		case 225:
			tempCharacter.family.MainProperty = value;
			break;
		case 226:
			tempCharacter.family.SubProperty = value;
			break;
		case 227:
			tempCharacter.skill.Attack = value;
			break;
		case 228:
			tempCharacter.skill.AttackPercent = value;
			break;
		case 229:
			tempCharacter.skill.MainProperty = value;
			break;
		case 230:
			tempCharacter.skill.MainPercent = value;
			break;
		case 231:
			tempCharacter.skill.SubProperty = value;
			break;
		case 232:
			tempCharacter.skill.SubPercent = value;
			break;
		case 233:
			tempCharacter.title.Attack = value;
			break;
		case 234:
			tempCharacter.title.AttackPercent = value;
			break;
		case 235:
			tempCharacter.title.MainProperty = value;
			break;
		case 236:
			tempCharacter.title.MainPercent = value;
			break;
		case 237:
			tempCharacter.title.SubProperty = value;
			break;
		case 238:
			tempCharacter.title.SubPercent = value;
			break;
		case 239:
			tempCharacter.other.Attack = value;
			break;
		case 240:
			tempCharacter.other.AttackPercent = value;
			break;
		case 241:
			tempCharacter.other.MainProperty = value;
			break;
		case 242:
			tempCharacter.other.MainPercent = value;
			break;
		case 243:
			tempCharacter.other.SubProperty = value;
			break;
		case 244:
			tempCharacter.other.SubPercent = value;
			break;
		case 245:
			tempCharacter.weapon = (Weapon)value;
			break;
		default:
			break;
		}
	}
	file.close();

	makeOutPut();
	return;
}

QByteArray MapleCalc::makeChFile()
{
	QString strCh = 
		QString::number(tempCharacter.profession) + "\n" +
		QString::number(tempCharacter.pureMain) + "\n" +
		QString::number(tempCharacter.panel.DamagePercent) + "\n" +
		QString::number(tempCharacter.panel.BossDamage) + "\n" +
		QString::number(tempCharacter.panel.FinalDamage) + "\n" +
		QString::number(tempCharacter.panel.IgnoreDefence) + "\n" +
		QString::number(tempCharacter.panel.CriticalHit) + "\n" +
		QString::number(tempCharacter.panel.CriticalDamage) + "\n" +
		QString::number(tempCharacter.panel.NormalMonsterDamage) + "\n" +
		QString::number(tempCharacter.combine.Attack) + "\n" +
		QString::number(tempCharacter.combine.AttackPercent) + "\n" +
		QString::number(tempCharacter.combine.MainProperty) + "\n" +
		QString::number(tempCharacter.combine.MainPercent) + "\n" +
		QString::number(tempCharacter.combine.SubProperty) + "\n" +
		QString::number(tempCharacter.combine.SubPercent) + "\n" +
		QString::number(tempCharacter.MainWeapon.Attack) + "\n" +
		QString::number(tempCharacter.MainWeapon.AttackPercent) + "\n" +
		QString::number(tempCharacter.MainWeapon.MainProperty) + "\n" +
		QString::number(tempCharacter.MainWeapon.MainPercent) + "\n" +
		QString::number(tempCharacter.MainWeapon.SubProperty) + "\n" +
		QString::number(tempCharacter.MainWeapon.SubPercent) + "\n" +
		QString::number(tempCharacter.SubWeapon.Attack) + "\n" +
		QString::number(tempCharacter.SubWeapon.AttackPercent) + "\n" +
		QString::number(tempCharacter.SubWeapon.MainProperty) + "\n" +
		QString::number(tempCharacter.SubWeapon.MainPercent) + "\n" +
		QString::number(tempCharacter.SubWeapon.SubProperty) + "\n" +
		QString::number(tempCharacter.SubWeapon.SubPercent) + "\n" +
		QString::number(tempCharacter.Emblem.Attack) + "\n" +
		QString::number(tempCharacter.Emblem.AttackPercent) + "\n" +
		QString::number(tempCharacter.Emblem.MainProperty) + "\n" +
		QString::number(tempCharacter.Emblem.MainPercent) + "\n" +
		QString::number(tempCharacter.Emblem.SubProperty) + "\n" +
		QString::number(tempCharacter.Emblem.SubPercent) + "\n" +
		QString::number(tempCharacter.hat.Attack) + "\n" +
		QString::number(tempCharacter.hat.AttackPercent) + "\n" +
		QString::number(tempCharacter.hat.MainProperty) + "\n" +
		QString::number(tempCharacter.hat.MainPercent) + "\n" +
		QString::number(tempCharacter.hat.SubProperty) + "\n" +
		QString::number(tempCharacter.hat.SubPercent) + "\n" +
		QString::number(tempCharacter.top.Attack) + "\n" +
		QString::number(tempCharacter.top.AttackPercent) + "\n" +
		QString::number(tempCharacter.top.MainProperty) + "\n" +
		QString::number(tempCharacter.top.MainPercent) + "\n" +
		QString::number(tempCharacter.top.SubProperty) + "\n" +
		QString::number(tempCharacter.top.SubPercent) + "\n" +
		QString::number(tempCharacter.bottom.Attack) + "\n" +
		QString::number(tempCharacter.bottom.AttackPercent) + "\n" +
		QString::number(tempCharacter.bottom.MainProperty) + "\n" +
		QString::number(tempCharacter.bottom.MainPercent) + "\n" +
		QString::number(tempCharacter.bottom.SubProperty) + "\n" +
		QString::number(tempCharacter.bottom.SubPercent) + "\n" +
		QString::number(tempCharacter.shoes.Attack) + "\n" +
		QString::number(tempCharacter.shoes.AttackPercent) + "\n" +
		QString::number(tempCharacter.shoes.MainProperty) + "\n" +
		QString::number(tempCharacter.shoes.MainPercent) + "\n" +
		QString::number(tempCharacter.shoes.SubProperty) + "\n" +
		QString::number(tempCharacter.shoes.SubPercent) + "\n" +
		QString::number(tempCharacter.glove.Attack) + "\n" +
		QString::number(tempCharacter.glove.AttackPercent) + "\n" +
		QString::number(tempCharacter.glove.MainProperty) + "\n" +
		QString::number(tempCharacter.glove.MainPercent) + "\n" +
		QString::number(tempCharacter.glove.SubProperty) + "\n" +
		QString::number(tempCharacter.glove.SubPercent) + "\n" +
		QString::number(tempCharacter.cape.Attack) + "\n" +
		QString::number(tempCharacter.cape.AttackPercent) + "\n" +
		QString::number(tempCharacter.cape.MainProperty) + "\n" +
		QString::number(tempCharacter.cape.MainPercent) + "\n" +
		QString::number(tempCharacter.cape.SubProperty) + "\n" +
		QString::number(tempCharacter.cape.SubPercent) + "\n" +
		QString::number(tempCharacter.ring0.Attack) + "\n" +
		QString::number(tempCharacter.ring0.AttackPercent) + "\n" +
		QString::number(tempCharacter.ring0.MainProperty) + "\n" +
		QString::number(tempCharacter.ring0.MainPercent) + "\n" +
		QString::number(tempCharacter.ring0.SubProperty) + "\n" +
		QString::number(tempCharacter.ring0.SubPercent) + "\n" +
		QString::number(tempCharacter.ring1.Attack) + "\n" +
		QString::number(tempCharacter.ring1.AttackPercent) + "\n" +
		QString::number(tempCharacter.ring1.MainProperty) + "\n" +
		QString::number(tempCharacter.ring1.MainPercent) + "\n" +
		QString::number(tempCharacter.ring1.SubProperty) + "\n" +
		QString::number(tempCharacter.ring1.SubPercent) + "\n" +
		QString::number(tempCharacter.ring2.Attack) + "\n" +
		QString::number(tempCharacter.ring2.AttackPercent) + "\n" +
		QString::number(tempCharacter.ring2.MainProperty) + "\n" +
		QString::number(tempCharacter.ring2.MainPercent) + "\n" +
		QString::number(tempCharacter.ring2.SubProperty) + "\n" +
		QString::number(tempCharacter.ring2.SubPercent) + "\n" +
		QString::number(tempCharacter.ring3.Attack) + "\n" +
		QString::number(tempCharacter.ring3.AttackPercent) + "\n" +
		QString::number(tempCharacter.ring3.MainProperty) + "\n" +
		QString::number(tempCharacter.ring3.MainPercent) + "\n" +
		QString::number(tempCharacter.ring3.SubProperty) + "\n" +
		QString::number(tempCharacter.ring3.SubPercent) + "\n" +
		QString::number(tempCharacter.ring4.Attack) + "\n" +
		QString::number(tempCharacter.ring4.AttackPercent) + "\n" +
		QString::number(tempCharacter.ring4.MainProperty) + "\n" +
		QString::number(tempCharacter.ring4.MainPercent) + "\n" +
		QString::number(tempCharacter.ring4.SubProperty) + "\n" +
		QString::number(tempCharacter.ring4.SubPercent) + "\n" +
		QString::number(tempCharacter.ring5.Attack) + "\n" +
		QString::number(tempCharacter.ring5.AttackPercent) + "\n" +
		QString::number(tempCharacter.ring5.MainProperty) + "\n" +
		QString::number(tempCharacter.ring5.MainPercent) + "\n" +
		QString::number(tempCharacter.ring5.SubProperty) + "\n" +
		QString::number(tempCharacter.ring5.SubPercent) + "\n" +
		QString::number(tempCharacter.pendant0.Attack) + "\n" +
		QString::number(tempCharacter.pendant0.AttackPercent) + "\n" +
		QString::number(tempCharacter.pendant0.MainProperty) + "\n" +
		QString::number(tempCharacter.pendant0.MainPercent) + "\n" +
		QString::number(tempCharacter.pendant0.SubProperty) + "\n" +
		QString::number(tempCharacter.pendant0.SubPercent) + "\n" +
		QString::number(tempCharacter.pendant1.Attack) + "\n" +
		QString::number(tempCharacter.pendant1.AttackPercent) + "\n" +
		QString::number(tempCharacter.pendant1.MainProperty) + "\n" +
		QString::number(tempCharacter.pendant1.MainPercent) + "\n" +
		QString::number(tempCharacter.pendant1.SubProperty) + "\n" +
		QString::number(tempCharacter.pendant1.SubPercent) + "\n" +
		QString::number(tempCharacter.belt.Attack) + "\n" +
		QString::number(tempCharacter.belt.AttackPercent) + "\n" +
		QString::number(tempCharacter.belt.MainProperty) + "\n" +
		QString::number(tempCharacter.belt.MainPercent) + "\n" +
		QString::number(tempCharacter.belt.SubProperty) + "\n" +
		QString::number(tempCharacter.belt.SubPercent) + "\n" +
		QString::number(tempCharacter.faceAccessory.Attack) + "\n" +
		QString::number(tempCharacter.faceAccessory.AttackPercent) + "\n" +
		QString::number(tempCharacter.faceAccessory.MainProperty) + "\n" +
		QString::number(tempCharacter.faceAccessory.MainPercent) + "\n" +
		QString::number(tempCharacter.faceAccessory.SubProperty) + "\n" +
		QString::number(tempCharacter.faceAccessory.SubPercent) + "\n" +
		QString::number(tempCharacter.eyeDecoraion.Attack) + "\n" +
		QString::number(tempCharacter.eyeDecoraion.AttackPercent) + "\n" +
		QString::number(tempCharacter.eyeDecoraion.MainProperty) + "\n" +
		QString::number(tempCharacter.eyeDecoraion.MainPercent) + "\n" +
		QString::number(tempCharacter.eyeDecoraion.SubProperty) + "\n" +
		QString::number(tempCharacter.eyeDecoraion.SubPercent) + "\n" +
		QString::number(tempCharacter.pocketItem.Attack) + "\n" +
		QString::number(tempCharacter.pocketItem.AttackPercent) + "\n" +
		QString::number(tempCharacter.pocketItem.MainProperty) + "\n" +
		QString::number(tempCharacter.pocketItem.MainPercent) + "\n" +
		QString::number(tempCharacter.pocketItem.SubProperty) + "\n" +
		QString::number(tempCharacter.pocketItem.SubPercent) + "\n" +
		QString::number(tempCharacter.earring.Attack) + "\n" +
		QString::number(tempCharacter.earring.AttackPercent) + "\n" +
		QString::number(tempCharacter.earring.MainProperty) + "\n" +
		QString::number(tempCharacter.earring.MainPercent) + "\n" +
		QString::number(tempCharacter.earring.SubProperty) + "\n" +
		QString::number(tempCharacter.earring.SubPercent) + "\n" +
		QString::number(tempCharacter.shoulderAccessory.Attack) + "\n" +
		QString::number(tempCharacter.shoulderAccessory.AttackPercent) + "\n" +
		QString::number(tempCharacter.shoulderAccessory.MainProperty) + "\n" +
		QString::number(tempCharacter.shoulderAccessory.MainPercent) + "\n" +
		QString::number(tempCharacter.shoulderAccessory.SubProperty) + "\n" +
		QString::number(tempCharacter.badge.SubPercent) + "\n" +
		QString::number(tempCharacter.badge.Attack) + "\n" +
		QString::number(tempCharacter.badge.AttackPercent) + "\n" +
		QString::number(tempCharacter.badge.MainProperty) + "\n" +
		QString::number(tempCharacter.badge.MainPercent) + "\n" +
		QString::number(tempCharacter.badge.SubProperty) + "\n" +
		QString::number(tempCharacter.badge.SubPercent) + "\n" +
		QString::number(tempCharacter.medal.Attack) + "\n" +
		QString::number(tempCharacter.medal.AttackPercent) + "\n" +
		QString::number(tempCharacter.medal.MainProperty) + "\n" +
		QString::number(tempCharacter.medal.MainPercent) + "\n" +
		QString::number(tempCharacter.medal.SubProperty) + "\n" +
		QString::number(tempCharacter.medal.SubPercent) + "\n" +
		QString::number(tempCharacter.totem.Attack) + "\n" +
		QString::number(tempCharacter.totem.AttackPercent) + "\n" +
		QString::number(tempCharacter.totem.MainProperty) + "\n" +
		QString::number(tempCharacter.totem.MainPercent) + "\n" +
		QString::number(tempCharacter.totem.SubProperty) + "\n" +
		QString::number(tempCharacter.totem.SubPercent) + "\n" +
		QString::number(tempCharacter.robotHeart.Attack) + "\n" +
		QString::number(tempCharacter.robotHeart.AttackPercent) + "\n" +
		QString::number(tempCharacter.robotHeart.MainProperty) + "\n" +
		QString::number(tempCharacter.robotHeart.MainPercent) + "\n" +
		QString::number(tempCharacter.robotHeart.SubProperty) + "\n" +
		QString::number(tempCharacter.robotHeart.SubPercent) + "\n" +
		QString::number(tempCharacter.petEquipment.Attack) + "\n" +
		QString::number(tempCharacter.petEquipment.AttackPercent) + "\n" +
		QString::number(tempCharacter.petEquipment.MainProperty) + "\n" +
		QString::number(tempCharacter.petEquipment.MainPercent) + "\n" +
		QString::number(tempCharacter.petEquipment.SubProperty) + "\n" +
		QString::number(tempCharacter.petEquipment.SubPercent) + "\n" +
		QString::number(tempCharacter.petSkill.Attack) + "\n" +
		QString::number(tempCharacter.petSkill.AttackPercent) + "\n" +
		QString::number(tempCharacter.petSkill.MainProperty) + "\n" +
		QString::number(tempCharacter.petSkill.MainPercent) + "\n" +
		QString::number(tempCharacter.petSkill.SubProperty) + "\n" +
		QString::number(tempCharacter.petSkill.SubPercent) + "\n" +
		QString::number(tempCharacter.link.Attack) + "\n" +
		QString::number(tempCharacter.link.AttackPercent) + "\n" +
		QString::number(tempCharacter.link.MainProperty) + "\n" +
		QString::number(tempCharacter.link.MainPercent) + "\n" +
		QString::number(tempCharacter.link.SubProperty) + "\n" +
		QString::number(tempCharacter.link.SubPercent) + "\n" +
		QString::number(tempCharacter.cash.Attack) + "\n" +
		QString::number(tempCharacter.cash.AttackPercent) + "\n" +
		QString::number(tempCharacter.cash.MainProperty) + "\n" +
		QString::number(tempCharacter.cash.MainPercent) + "\n" +
		QString::number(tempCharacter.cash.SubProperty) + "\n" +
		QString::number(tempCharacter.cash.SubPercent) + "\n" +
		QString::number(tempCharacter.leagueCapture.Attack) + "\n" +
		QString::number(tempCharacter.leagueCapture.MainProperty) + "\n" +
		QString::number(tempCharacter.leagueCapture.SubProperty) + "\n" +
		QString::number(tempCharacter.leagueTeam.Attack) + "\n" +
		QString::number(tempCharacter.leagueTeam.AttackPercent) + "\n" +
		QString::number(tempCharacter.leagueTeam.MainProperty) + "\n" +
		QString::number(tempCharacter.leagueTeam.MainPercent) + "\n" +
		QString::number(tempCharacter.leagueTeam.SubProperty) + "\n" +
		QString::number(tempCharacter.leagueTeam.SubPercent) + "\n" +
		QString::number(tempCharacter.arc.MainProperty) + "\n" +
		QString::number(tempCharacter.auth.MainProperty) + "\n" +
		QString::number(tempCharacter.inner.Attack) + "\n" +
		QString::number(tempCharacter.inner.MainProperty) + "\n" +
		QString::number(tempCharacter.inner.SubProperty) + "\n" +
		QString::number(tempCharacter.hyper.Attack) + "\n" +
		QString::number(tempCharacter.hyper.MainProperty) + "\n" +
		QString::number(tempCharacter.hyper.SubProperty) + "\n" +

		QString::number(tempCharacter.monster.Attack) + "\n" +
		QString::number(tempCharacter.monster.AttackPercent) + "\n" +
		QString::number(tempCharacter.monster.MainProperty) + "\n" +
		QString::number(tempCharacter.monster.MainPercent) + "\n" +
		QString::number(tempCharacter.monster.SubProperty) + "\n" +
		QString::number(tempCharacter.monster.SubPercent) + "\n" +
		QString::number(tempCharacter.family.Attack) + "\n" +
		QString::number(tempCharacter.family.MainProperty) + "\n" +
		QString::number(tempCharacter.family.SubProperty) + "\n" +
		QString::number(tempCharacter.skill.Attack) + "\n" +
		QString::number(tempCharacter.skill.AttackPercent) + "\n" +
		QString::number(tempCharacter.skill.MainProperty) + "\n" +
		QString::number(tempCharacter.skill.MainPercent) + "\n" +
		QString::number(tempCharacter.skill.SubProperty) + "\n" +
		QString::number(tempCharacter.skill.SubPercent) + "\n" +
		QString::number(tempCharacter.title.Attack) + "\n" +
		QString::number(tempCharacter.title.AttackPercent) + "\n" +
		QString::number(tempCharacter.title.MainProperty) + "\n" +
		QString::number(tempCharacter.title.MainPercent) + "\n" +
		QString::number(tempCharacter.title.SubProperty) + "\n" +
		QString::number(tempCharacter.title.SubPercent) + "\n" +
		QString::number(tempCharacter.other.Attack) + "\n" +
		QString::number(tempCharacter.other.AttackPercent) + "\n" +
		QString::number(tempCharacter.other.MainProperty) + "\n" +
		QString::number(tempCharacter.other.MainPercent) + "\n" +
		QString::number(tempCharacter.other.SubProperty) + "\n" +
		QString::number(tempCharacter.other.SubPercent) + "\n" +
		QString::number(tempCharacter.weapon);
    return strCh.toLocal8Bit();
}



