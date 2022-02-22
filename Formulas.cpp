#include "MapleCalc.h"

#define SUM_PROPERTY_EXCEPT_ARC_AUTH_INNER_HYPER_LEAGUETEAM(x, y) (x.combine.y + \
x.ring0.y + \
x.ring1.y + \
x.ring2.y + \
x.ring3.y + \
x.ring4.y + \
x.ring5.y + \
x.pendant0.y + \
x.pendant1.y + \
x.belt.y + \
x.faceAccessory.y + \
x.eyeDecoraion.y + \
x.pocketItem.y + \
x.earring.y + \
x.shoulderAccessory.y + \
x.badge.y + \
x.medal.y + \
x.totem.y + \
x.hat.y + \
x.top.y + \
x.bottom.y + \
x.glove.y + \
x.shoes.y + \
x.cape.y + \
x.MainWeapon.y + \
x.SubWeapon.y + \
x.Emblem.y + \
x.robotHeart.y + \
x.leagueCapture.y + \
x.link.y + \
x.petEquipment.y + \
x.petSkill.y + \
x.cash.y + \
x.family.y + \
x.monster.y + \
x.title.y + \
x.skill.y + \
x.other.y)

#define SUM_ARC_AUTH_INNER_HYPER_LEAGUETEAM(x, y) (x.arc.y + \
x.auth.y + \
x.inner.y + \
x.hyper.y + \
x.leagueTeam.y)

#define SUM_PROPERTY_EQUIPMENT(x, y) (x.combine.y + \
x.ring0.y + \
x.ring1.y + \
x.ring2.y + \
x.ring3.y + \
x.ring4.y + \
x.ring5.y + \
x.pendant0.y + \
x.pendant1.y + \
x.belt.y + \
x.faceAccessory.y + \
x.eyeDecoraion.y + \
x.pocketItem.y + \
x.earring.y + \
x.shoulderAccessory.y + \
x.badge.y + \
x.medal.y + \
x.totem.y + \
x.hat.y + \
x.top.y + \
x.bottom.y + \
x.glove.y + \
x.shoes.y + \
x.cape.y + \
x.MainWeapon.y + \
x.SubWeapon.y + \
x.Emblem.y + \
x.petEquipment.y + \
x.robotHeart.y)

#define SUM_PROPERTY_EXCEPT_EQUIPMENT(x, y) (x.leagueCapture.y + \
x.link.y + \
x.petSkill.y + \
x.cash.y + \
x.family.y + \
x.monster.y + \
x.title.y + \
x.skill.y + \
x.other.y)

qreal MapleCalc::calcPercent(qint64 newValue, qint64 oldValue)
{
	qreal percent = 100 * ((qreal)newValue / (qreal)oldValue) - 100;
	return percent;
}

qreal MapleCalc::getWeaponRatio(Profession p, Weapon w)
{
	qreal ratio = 0;
	switch (w)
	{
	case DualBowGun:
		ratio = 1.3;
		break;
	case Bow:
		ratio = 1.3;
		break;
	case CrossBow:
		ratio = 1.35;
		break;
	case AncientBow:
		ratio = 1.3;
		break;
	case BreathShooter:
		ratio = 1.3;
		break;
	case TwoHanded_Sword:
		ratio = 1.34;
		if(p == Hero) ratio = 1.44;
		break;
	case TwoHanded_Axe:
		ratio = 1.34;
		if (p == Hero) ratio = 1.44;
		break;
	case TwoHanded_Blunt:
		ratio = 1.34;
		break;
	case Spear:
		ratio = 1.49;
		break;
	case PoleArm:
		ratio = 1.49;
		break;
	case Claw:
		ratio = 1.75;
		break;
	case Knuckle:
		ratio = 1.7;
		break;
	case Gun:
		ratio = 1.5;
		break;
	case HandCannon:
		ratio = 1.5;
		break;
	case Katana:
		ratio = 1.25;
		break;
	case Fan:
		ratio = 1.35;
		break;
	case Lapis:
		ratio = 1.49;
		break;
	case Lazuli:
		ratio = 1.34;
		break;
	case ArmCannon:
		ratio = 1.7;
		break;
	case OneHanded_Sword:
		ratio = 1.2;
		if (p == Hero) ratio = 1.3;
		break;
	case OneHanded_Axe:
		ratio = 1.2;
		if (p == Hero) ratio = 1.3;
		break;
	case OneHanded_Blunt:
		ratio = 1.2;
		break;
	case ShiningRod:
		ratio = 1.2;
		break;
	case SoulShooter:
		ratio = 1.7;
		break;
	case Desperado:
		ratio = 1.3;
		break;
	case WhipBlade:
		ratio = 1.75; // 此时尖兵属性系数采用3
		break;
	case Scepter:
		ratio = 1.34;
		break;
	case Psy_Limiter:
		ratio = 1.2;
		break;
	case Chain:
		ratio = 1.3;
		break;
	case Gauntlet:
		ratio = 1.2;
		break;
	case RitualFan:
		ratio = 1.3;
		break;
	case Dagger:
		ratio = 1.3;
		break;
	case Cane:
		ratio = 1.3;
		break;
	case Wand:
		ratio = 1.0;
		if(p == Adventurer_WithoutHero || p == Knight) ratio = 1.2;
		break;
	case Staff:
		ratio = 1.0;
		if (p == Adventurer_WithoutHero || p == Knight) ratio = 1.2;
		break;
	case Tuner:
		ratio = 1.3;
		break;
	case Quanfeng:
		ratio = 1.75;
		break;
	default:
		break;
	}

	return ratio;
}

qint64 MapleCalc::calculateMain()
{
	if (tempCharacter.profession == DemonAvenger) {
		qint64 pureHp = tempCharacter.pureMain;
		qint64 addHp = SUM_PROPERTY_EQUIPMENT(tempCharacter, MainProperty) / 2 + SUM_PROPERTY_EXCEPT_EQUIPMENT(tempCharacter, MainProperty);
		qint64 addHpFinal = SUM_ARC_AUTH_INNER_HYPER_LEAGUETEAM(tempCharacter, MainProperty);
		qreal rate = 1 + (qreal)SUM_PROPERTY_EXCEPT_ARC_AUTH_INNER_HYPER_LEAGUETEAM(tempCharacter, MainPercent) / 100;
		qint64 demonAvengerMain = (qint64)(((qreal)pureHp * rate) / 3.5) + (qint64)((qreal)(((qreal)addHp * rate + addHpFinal) / 3.5) * 0.8);
		return demonAvengerMain;
	}
	// 暂时忽略内在按AP某属性增加另一属性%的情况
	qreal percent = (qreal)(SUM_PROPERTY_EXCEPT_ARC_AUTH_INNER_HYPER_LEAGUETEAM(tempCharacter, MainPercent)) / 100 + 1;
	qint64 main = (tempCharacter.pureMain // 裸属性（裸属性各个职业略有不同，不做区分）
		+ SUM_PROPERTY_EXCEPT_ARC_AUTH_INNER_HYPER_LEAGUETEAM(tempCharacter, MainProperty)) // 附加属性
		* percent // 附加属性百分比
		+ SUM_ARC_AUTH_INNER_HYPER_LEAGUETEAM(tempCharacter, MainProperty); // 最终属性
	return main;
}

qint64 MapleCalc::calculateSub()
{
	// 暂时忽略内在按AP某属性增加另一属性%的情况
	qreal percent = (qreal)(SUM_PROPERTY_EXCEPT_ARC_AUTH_INNER_HYPER_LEAGUETEAM(tempCharacter, SubPercent)) / 100 + 1;
	qint64 sub = (SUM_PROPERTY_EXCEPT_ARC_AUTH_INNER_HYPER_LEAGUETEAM(tempCharacter, SubProperty)) // 附加属性
		* percent // 附加属性百分比
		+ SUM_ARC_AUTH_INNER_HYPER_LEAGUETEAM(tempCharacter, SubProperty); // 最终属性
	return sub;
}

qint64 MapleCalc::calculateAtk()
{
	// 暂时忽略内在能力：按等级增加攻击力这一情况（属于最终攻击力）
	qreal percent = (qreal)(SUM_ARC_AUTH_INNER_HYPER_LEAGUETEAM(tempCharacter, AttackPercent) + SUM_PROPERTY_EXCEPT_ARC_AUTH_INNER_HYPER_LEAGUETEAM(tempCharacter, AttackPercent)) / 100 + 1;
	qint64 atk = 0;
	if (tempCharacter.profession == Kanna) {
		atk = (SUM_ARC_AUTH_INNER_HYPER_LEAGUETEAM(tempCharacter, Attack) + SUM_PROPERTY_EXCEPT_ARC_AUTH_INNER_HYPER_LEAGUETEAM(tempCharacter, Attack) - 5)
			* percent + 5;
	}
	else {
		atk = (SUM_ARC_AUTH_INNER_HYPER_LEAGUETEAM(tempCharacter, Attack) + SUM_PROPERTY_EXCEPT_ARC_AUTH_INNER_HYPER_LEAGUETEAM(tempCharacter, Attack))
			* percent; // + 最终攻击力;
	}
	return atk;
}

qint64 MapleCalc::calcRealAtk()
{
	qint64 realAtk = 0;
	if (tempCharacter.profession == DemonAvenger) realAtk = getWeaponRatio(tempCharacter.profession, tempCharacter.weapon) * (calculateMain() + calculateSub()) * calculateAtk() / 100;
	else realAtk = getWeaponRatio(tempCharacter.profession, tempCharacter.weapon) * (calculateMain() * 4 + calculateSub()) * calculateAtk() / 100;
	return realAtk;
}

qint64 MapleCalc::calcPropertyAtk(qint64 realAtk)
{
	qreal damageRate = (qreal)tempCharacter.panel.DamagePercent / 100 + 1;
	qreal finalRate = (qreal)tempCharacter.panel.FinalDamage / 100 + 1;
	qint64 propertyAtk = realAtk * damageRate * finalRate;
	return propertyAtk;
}

qint64 MapleCalc::calcBossAtk(qint64 realAtk)
{
	qreal damageRate = (qreal)(tempCharacter.panel.DamagePercent + tempCharacter.panel.BossDamage) / 100 + 1;
	qreal finalRate = (qreal)tempCharacter.panel.FinalDamage / 100 + 1;
	//暴击期望，默认暴击伤害取20~50的中间数35%
	qreal expectation = ((qreal)tempCharacter.panel.CriticalHit / 100) * (qreal)((qreal)tempCharacter.panel.CriticalDamage / 100 + 1.35)
		+ (qreal)(100 - tempCharacter.panel.CriticalHit) / 100;

	qint64 bossAtk = realAtk * damageRate * finalRate * expectation;
	return bossAtk;
}

qint64 MapleCalc::calcNormalMAtk(qint64 realAtk)
{
	qreal damageRate = (qreal)(tempCharacter.panel.DamagePercent + tempCharacter.panel.NormalMonsterDamage) / 100 + 1;
	qreal finalRate = (qreal)tempCharacter.panel.FinalDamage / 100 + 1;
	//暴击期望，默认暴击伤害取20~50的中间数35%
	qreal expectation = ((qreal)tempCharacter.panel.CriticalHit / 100) * (qreal)((qreal)tempCharacter.panel.CriticalDamage / 100 + 1.35)
		+ (qreal)(100 - tempCharacter.panel.CriticalHit) / 100;

	qint64 normalMAtk = realAtk * damageRate * finalRate * expectation;
	return normalMAtk;
}

qint64 MapleCalc::calc300DefAtk(qint64 realAtk)
{
	qreal realDef = (qreal)(100 - tempCharacter.panel.IgnoreDefence) * 0.03;
	qint64 boss300DefAtk = calcBossAtk(realAtk) * (qreal)(1 - realDef);
	return boss300DefAtk >= 0 ? boss300DefAtk : -1;
}