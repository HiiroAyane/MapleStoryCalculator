#pragma once
#pragma warning(disable:26812)
#pragma warning(disable:26451)
#pragma warning(disable:26498)
#pragma warning(disable:26495)

/*
实际最大属性攻击力 ＝ 武器系数 × 属性值 × 总攻击力/100
实际最小属性攻击力 ＝ 实际最大属性攻击力 × 熟练度%/100

显示最大攻击力 =     实际最大属性攻击力 X （1 + 百分比伤害%/100）X （1 + 百分比最终伤害%/100）
显示最小攻击力 = 1 + 实际最小属性攻击力 X （1 + 百分比伤害%/100）X （1 + 百分比最终伤害%/100）
*/

enum Profession {
    Mercedes = 0,
    Hero,
	Adventurer_WithoutHero,
	Knight,
	Xenon,
	Other,
	DemonAvenger,
	Kanna,
};

enum Weapon {
    DualBowGun = 0,
	Bow,
	CrossBow,
	AncientBow,
	BreathShooter,
	TwoHanded_Sword,
	TwoHanded_Axe,
	TwoHanded_Blunt,
	Spear,
	PoleArm,
	Claw,
	Knuckle,
	Gun,
	HandCannon,
	Katana,
	Fan, // 折扇 阴阳师
	Lapis,// 拉比斯
	Lazuli,// 拉兹丽
	ArmCannon,
	// 单手武器
	OneHanded_Sword,
	OneHanded_Axe,
	OneHanded_Blunt,
	ShiningRod,
	SoulShooter,
	Desperado,// 亡命剑
	WhipBlade,// 能量剑
	Scepter, // 魔法棒
	Psy_Limiter,
	Chain,
	Gauntlet, // 魔力手套
	RitualFan, // 扇子
	Dagger, // 短剑
	Cane, // 手杖
	Wand, // 短杖
	Staff, // 长杖
	Tuner,
	Quanfeng,
};

struct EquipMent {
    /*
    攻击力
    主属性
    副属性
    攻击力百分比
    主属性百分比
    副属性百分比
    */
    int Attack        = 0;
    int MainProperty  = 0;
    int SubProperty   = 0;
    int AttackPercent = 0;
    int MainPercent   = 0;
    int SubPercent    = 0;
};

struct Panel {
    /*
    伤害
    BOSS伤害
    最终伤害
    暴击率
    暴击伤害
    无视防御
	对普通怪物伤害
    */
    int DamagePercent  = 0;
    int BossDamage     = 0;
    int FinalDamage    = 0;
    int CriticalHit    = 0;
    int CriticalDamage = 0;
    int IgnoreDefence  = 0;
	int NormalMonsterDamage = 0;
};

struct Character {
    Profession profession;
    Weapon weapon;
    Panel panel;
	int pureMain = 0;

    /*
    套装属性
    */
    EquipMent combine;

    /*
    饰品：
    耳环x6
    吊坠x2
    腰带
    脸饰
    眼饰
    口袋
    耳环
    肩饰
    徽章
	勋章
    图腾
    */
    EquipMent ring0;
    EquipMent ring1;
    EquipMent ring2;
    EquipMent ring3;
    EquipMent ring4;
    EquipMent ring5;
    EquipMent pendant0;
    EquipMent pendant1;
    EquipMent belt;
    EquipMent faceAccessory;
    EquipMent eyeDecoraion;
    EquipMent pocketItem;
    EquipMent earring;
    EquipMent shoulderAccessory;
    EquipMent badge;
    EquipMent medal;
    EquipMent totem;

    /*
    防具：
    帽子
    套服
    上衣
    裙裤
    手套
    鞋子
    披风
    */
    EquipMent hat;
    EquipMent top;
    EquipMent bottom;
    EquipMent glove;
    EquipMent shoes;
    EquipMent cape;

    /*
    主武器
    服务器
    纹  章
    */
    EquipMent MainWeapon;
    EquipMent SubWeapon;
    EquipMent Emblem;

    /*
    机器人
    */
    EquipMent robotHeart;

    /*
    神秘
    原初
    */
    EquipMent arc;  // cannot effect by %
    EquipMent auth; // cannot effect by %

    /*
    超级属性
    */
    EquipMent hyper; // cannot effect by %

    /*
    内在属性
    */
    EquipMent inner;// cannot effect by %

    /*
    联盟队员效果
    联盟占领效果
    */
    EquipMent leagueTeam; // cannot effect by %
    EquipMent leagueCapture;

    /*
    Link
    */
    EquipMent link;

    /*
    宠物装备
    宠物技能
    */
    EquipMent petEquipment;
    EquipMent petSkill;

    /*
    现金道具
    */
    EquipMent cash;

    /*
    怪怪
    */
    EquipMent monster;

    /*
家族
*/
    EquipMent family;

    /*
    被动技能
    */
    EquipMent skill;

    /*
    称号
    */
    EquipMent title;

    /*
    拼图及其他
    */
    EquipMent other;
};
