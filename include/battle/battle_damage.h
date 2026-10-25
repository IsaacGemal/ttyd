#pragma once

typedef struct BattleUnit BattleUnit;
typedef struct BattleWorkUnitPart BattleUnitPart;
typedef struct BattleWeapon BattleWeapon;

typedef struct BattleWork BattleWork;

#include <dolphin/types.h>

// Scratch space for BattleCheckCounter, zeroed by BattleInitCounterPreCheckWork with hitLands set to 1.
typedef struct CounterPreCheckWork {
	s32 hitLands;        // 0x0, cleared by counter kinds 0x2B..0x30 (spiky, fiery, icy, electric part); BattleCheckDamage skips the hit when 0
	s32 halfDamage;      // 0x4, payback status or poison counter: adds half the hit damage to counterDamage
	s32 inflictBurn;     // 0x8, fiery status counter: the attacker gets STATUS_BURN
	s32 inflictPoison;   // 0xC, poison status counter: the attacker gets STATUS_POISON
	s32 inflictElectric; // 0x10, electric status counter: the attacker gets STATUS_ELECTRIC
	s32 inflictFreeze;   // 0x14, icy status counter: the attacker gets STATUS_FREEZE
	s32 holdFast;        // 0x18, hold fast status: adds half the hit damage, plays effBombEntry and the thunder sfx
	s32 returnPostage;   // 0x1C, Return Postage badge: adds half the hit damage
	s32 counterType;     // 0x20, counter kind (0x2B..0x36); BattleCheckCounter's return value is picked by it
	s32 damageCode;      // 0x24, damage code (0x12B..0x136) for BattleDamageDirect on the attacker; nonzero once a counter matched
	s32 counterDamage;   // 0x28, damage dealt back to the attacker (at least 1)
	s32 hitEventCode;    // 0x2C, damage code (0x1F..0x24) BattleRunHitEvent runs on the target; 0 for the non-contact counters
} CounterPreCheckWork;

typedef struct BattleVulnerableStatus {
	u8 sleepRate; //0x0
	u8 stopRate; //0x1
	u8 dizzyRate; //0x2
	u8 poisonRate; //0x3
	u8 confuseRate; //0x4
	u8 electricRate; //0x5
	u8 burnRate; //0x6
	u8 freezeRate; //0x7
	u8 hugeRate; //0x8
	u8 tinyRate; //0x9
	u8 attackUpRate; //0xA
	u8 attackDownRate; //0xB
	u8 defenseUpRate; //0xC
	u8 defenseDownRate; //0xD
	u8 allergicRate; //0xE
	u8 frightRate; //0xF
	u8 galeForceRate; //0x10
	u8 fastRate; //0x11
	u8 slowRate; //0x12
	u8 dodgyRate; //0x13
	u8 invisibleRate; //0x14
	u8 instaKillRate; //0x15
} BattleVulnerableStatus;

s32 BattleCheckDamage(struct BattleUnit* unit1, struct BattleUnit* unit2, struct BattleWorkUnitPart* part, struct BattleWeapon* weapon, s32 flags);
void BattleAttackDeclareAll(BattleWork* wp);
s32 BattlePreCheckDamage(BattleUnit* attacker, BattleUnit* target, BattleUnitPart* part, BattleWeapon* weapon, s32 a);

void BattleInitCounterPreCheckWork(CounterPreCheckWork* work);
void BattleDamageDirect(s32 damageCode, BattleUnit* unit, BattleUnitPart* part, s32 arg3, s32 arg4,
                        s32 arg5, s32 damagePattern, s32 arg7);
void BattleAttackDeclare(s32* out1, s32* out2);
s32 BattleCheckPikkyoro(BattleWeapon* weapon, s32* out);
s32 _checkDamageCode_EmergencyRevival(BattleUnit* unit, s32* flags);
s32 BattleCalculateFpDamage(BattleUnit* attacker, BattleUnit* target, BattleUnitPart* part, BattleWeapon* weapon, s32* damageCode, s32 flags);
s32 BattleCalculateDamage(BattleUnit* unit1, BattleUnit* unit2, BattleUnitPart* part, BattleWeapon* weapon, s32* damageCode, s32 flags);
void __declare(s32 unitId, s32 partId, s32* out1, s32* out2);
// .sdata2 constants for BattleDamageDirect, named as in the original unit.
extern const f32 float_1_8041bed8;
extern const f32 float_8_8041bef0;
extern const f32 float_5_8041bef4;
extern const f32 float_10_8041bed0;
extern const f32 float_30_8041bee4;
extern const f32 float_60_8041bec8;
extern const f32 float_neg30_8041bef8;
extern const f32 float_neg60_8041befc;
extern const f32 float_0_8041bf00;
