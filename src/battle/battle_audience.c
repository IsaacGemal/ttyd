#include "battle/battle_audience.h"
#include "battle/battle.h"
#include "battle/battle_audience_kind_data.h"
#include "battle/battle_break_slot.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "mario/mario_pouch.h"
#include "math_ext.h"
#include "system.h"
#include <stddef.h>
#include <string.h>
#include "pmario_sound.h"
#include "statuswindow.h"
#include "battle/battle_weapon_power.h"
#include "battle/battle_message.h"
#include <stdio.h>
#include <stdlib.h>
#include "battle/battle_audience_kinopio.h"
#include "data/item_icon_view.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "driver/windowdrv.h"
#include "driver/swdrv.h"
#include "battle/battle_database_common.h"

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u32 lbl_80399940[2] = { 0xBF800000, 0x3F800000 };

// The original .data opens with four {-1,1} / {1,-1} pairs the throwable-item code walks through; the
// local ($) halves are declared static and force_active keeps every pair emitted.
#pragma force_active on
__declspec(section ".data") static u32 negone_one_374[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") u32 lbl_80399950[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") static u32 one_negone_373[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") u32 lbl_80399960[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") static u32 negone_one_360[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") u32 lbl_80399970[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") static u32 one_negone_359[2] = { 0x3F800000, 0xBF800000 };
#pragma force_active reset

// Throwable-item weapon descriptors copied into the audience work area on release.
#pragma force_active on
BattleWeapon weapon_data_akikan = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_AUDIENCE_CAN,                        // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    2,                                        // superguardType
    0.0f,                                     // field_0x14
    0,                                        // mStylishCommandMultiplier
    0,                                        // field_0x19
    0,                                        // mBingoSlotChance
    0,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 1, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01100060,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000048,                               // special
    0x000007FF,                               // counterResistanceFlags
    0,                                        // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // stageEffect
};
BattleWeapon weapon_data_brick = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_AUDIENCE_ROCK,                       // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    2,                                        // superguardType
    0.0f,                                     // field_0x14
    0,                                        // mStylishCommandMultiplier
    0,                                        // field_0x19
    0,                                        // mBingoSlotChance
    0,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 1, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01100060,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000048,                               // special
    0x000007FF,                               // counterResistanceFlags
    0,                                        // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // stageEffect
};
BattleWeapon weapon_data_bone = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_AUDIENCE_BONE,                       // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    2,                                        // superguardType
    0.0f,                                     // field_0x14
    0,                                        // mStylishCommandMultiplier
    0,                                        // field_0x19
    0,                                        // mBingoSlotChance
    0,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 1, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01100060,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000048,                               // special
    0x000007FF,                               // counterResistanceFlags
    0,                                        // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // stageEffect
};
BattleWeapon weapon_data_hammer = {
    NULL,                                     // name
    0,                                        // icon
    ITEM_AUDIENCE_HAMMER,                     // item
    NULL,                                     // description
    100,                                      // baseAccuracy
    0,                                        // baseFpCost
    0,                                        // baseSpCost
    2,                                        // superguardType
    0.0f,                                     // field_0x14
    0,                                        // mStylishCommandMultiplier
    0,                                        // field_0x19
    0,                                        // mBingoSlotChance
    0,                                        // field_0x1B
    weaponGetPowerDefault,                    // hpDamageFn
    { 2, 0, 0, 0, 0, 0, 0, 0 },               // hpDamageWork
    NULL,                                     // fpDamageFn
    { 0, 0, 0, 0, 0, 0, 0, 0 },               // fpDamageWork
    0x01100060,                               // targetClassFlags
    0x20000000,                               // targetPropertyFlags
    0,                                        // element
    0,                                        // damagePattern
    3,                                        // actionLevel
    2,                                        // field_0x6F
    NULL,                                     // acHelpMsg
    0x00000048,                               // special
    0x000007FF,                               // counterResistanceFlags
    0,                                        // targetWeightingFlags
    0,                                        // sleepChance
    0,                                        // sleepTime
    0,                                        // stopChance
    0,                                        // stopTime
    0,                                        // dizzyChance
    0,                                        // dizzyTime
    0,                                        // poisonChance
    0,                                        // poisonTime
    0,                                        // poisonStrength
    0,                                        // confuseChance
    0,                                        // confuseTime
    0,                                        // electricChance
    0,                                        // electricTime
    0,                                        // dodgyChance
    0,                                        // dodgyTime
    0,                                        // burnChance
    0,                                        // burnTime
    0,                                        // freezeChance
    0,                                        // freezeTime
    0,                                        // sizeChangeChance
    0,                                        // sizeChangeTime
    0,                                        // sizeChangeStrength
    0,                                        // atkChangeChance
    0,                                        // atkChangeTime
    0,                                        // atkChangeStrength
    0,                                        // defChangeChance
    0,                                        // defChangeTime
    0,                                        // defChangeStrength
    0,                                        // allergicChance
    0,                                        // allergicTime
    0,                                        // ohkoChance
    0,                                        // chargeStrength
    0,                                        // fastChance
    0,                                        // fastTime
    0,                                        // slowChance
    0,                                        // slowTime
    0,                                        // frightChance
    0,                                        // galeForceChance
    0,                                        // paybackTime
    0,                                        // holdFastTime
    0,                                        // invisibleChance
    0,                                        // invisibleTime
    0,                                        // hpRegenTime
    0,                                        // hpRegenStrength
    0,                                        // fpRegenTime
    0,                                        // fpRegenStrength
    NULL,                                     // mAttackEvtScript
    { 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0 }, // stageEffect
};

// Per-pouch-rank audience counts (indexed by pouch rank; 10 seats per side).
s32 haiti_data[4][10] = {
	0x00000012, 0x00000010, 0x00000010, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000012, 0x00000012, 0x00000012, 0x00000010, 0x00000010, 0x0000000E, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000014, 0x00000014, 0x00000014, 0x00000014, 0x00000012, 0x00000012, 0x00000012, 0x00000010, 0x00000000, 0x00000000,
	0x00000014, 0x00000014, 0x00000014, 0x00000014, 0x00000014, 0x00000014, 0x00000014, 0x00000014, 0x00000014, 0x00000014,
};
#pragma force_active off

// For the struct-copy that BattleAudience_Case_FallObject_Aud uses to load its two const offset
// tables; MWCC only emits the paired lwzu/stwu copy loop for a struct assignment, not for memcpy.
typedef struct BattleAudienceFallOffsetTable1 {
    s32 v[16];
} BattleAudienceFallOffsetTable1;

typedef struct BattleAudienceFallOffsetTable2 {
    s32 v[48];
} BattleAudienceFallOffsetTable2;


// LOCAL_PROTO(psndSFX_get_vol): retail battle callers consume a full-width result; sharing the u8 implementation signature adds a mask and regresses BtlUnit_snd_se_pos.
extern s32 psndSFX_get_vol(s32 id);

// LOCAL_PROTO(psndSFX_pit): retail callers disagree on pitch width and signedness; the shared u16 signature regresses matched sound commands.
extern s32 psndSFX_pit(s32 id, s32 pit);

// LOCAL_PROTO(psndSFX_vol): retail units disagree on volume width; a shared u8 prototype adds narrowing instructions in battle callers.
extern void psndSFX_vol(s32 id, s32 vol);


// Small event scripts that announce an audience reaction; the retail objects sit in .sdata (too large for
// automatic small-data placement, so the section is requested explicitly).
__declspec(section ".sdata") EVT_BEGIN(msg_puni_all_escape)
    CALL(btlevtcmd_AnnounceMessage, 0, 0, -225, STRING("btl_msg_audience_puni_escape"), 120)
    RETURN
EVT_END()

__declspec(section ".sdata") EVT_BEGIN(msg_puni_all_enter)
    CALL(btlevtcmd_AnnounceMessage, 0, 0, -225, STRING("btl_msg_audience_puni_meeting"), 120)
    RETURN
EVT_END()

__declspec(section ".sdata") EVT_BEGIN(msg_pansy_sing)
    CALL(btlevtcmd_AnnounceMessage, 0, 0, -225, STRING("btl_msg_audience_pansy_sleep"), 120)
    RETURN
EVT_END()

__declspec(section ".sdata") EVT_BEGIN(msg_heavy_bomb_fire)
    CALL(btlevtcmd_AnnounceMessage, 0, 0, -225, STRING("btl_msg_audience_heavybomb_dynamite"), 120)
    RETURN
EVT_END()

const f32 R_audience_delta_multiplier[] = {
	8.0f, 11.0f, 14.0f, 17.0f, 20.0f, 23.0f, 26.0f, 29.0f, 32.0f, 35.0f, 38.0f, 41.0f, 44.0f,
	47.0f, 50.0f, 53.0f, 56.0f, 59.0f, 62.0f, 65.0f, 68.0f, 71.0f, 74.0f, 77.0f, 80.0f, 83.0f,
	86.0f, 89.0f, 92.0f, 95.0f, 98.0f, 101.0f, 104.0f, 107.0f, 110.0f, 113.0f, 116.0f, 119.0f,
	122.0f, 125.0f, 128.0f, 131.0f, 134.0f, 137.0f, 140.0f, 143.0f, 146.0f, 149.0f, 152.0f, 155.0f,
	158.0f, 161.0f, 164.0f, 167.0f, 170.0f, 173.0f, 176.0f, 179.0f, 182.0f, 185.0f, 188.0f, 191.0f,
	194.0f, 197.0f, 200.0f,
};

const s32 R_num_audience_type_weights[] = { 20, 40, 20, 5, 3, 2, 1, 1, 1, 1, 1 };

__declspec(section ".sdata2") u32 dat_8041e608 = 0xFFFFFF00;

// The section's trailing 4-byte .sbss2 gap, after the audience colour literal's template.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") u8 gap_10_80423084_sbss2[4];
#pragma section ".sdata" ".sbss"


#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

extern BattleWork* _battleWorkPointer;

typedef struct BattleAudienceTexCoords {
	f32 v[32];
} BattleAudienceTexCoords;
#define AUD_SOUND_HEAD(sound) \
	(sound)->flags |= 1; \
	(sound)->timeLeft = -1; \
	(sound)->mFadeoutTime = 0
#define AUD_SOUND_TAIL(sound, vol) \
	(sound)->mCurrentBaseSfxVolume = (vol); \
	(sound)->mBaseSfxVolume = (vol); \
	(sound)->mForceFadeLength = 0; \
	(sound)->mForceFadeTimer = 0; \
	(sound)->mVolumeMultiplier = 100
#define AUD_SOUND_ID(sound) ((sound)->sfxId)

//local prototypes


FileEntry* tplRead(const char* path) {
	return fileAllocf(FILE_TYPE_TPL, "%s/%s", getMarioStDvdRoot(), path);
}

BattleAudience* BattleAudienceBaseGetPtr(void) {
	return &_battleWorkPointer->audience;
}

BattleAudienceMember* BattleAudienceGetPtr(s32 id) {
	return &BattleAudienceBaseGetPtr()->members[id];
}

BattleAudienceItem* BattleAudienceItemGetPtr(s32 id) {
	return &BattleAudienceBaseGetPtr()->items[id];
}

BattleAudienceWindow* BattleAudienceWinGetPtr(void) {
	return &BattleAudienceBaseGetPtr()->window;
}

BattleAudienceSound* BattleAudienceSoundGetPtr(s32 id) {
	return &BattleAudienceBaseGetPtr()->sounds[id];
}

void BattleAudience_Init(void) {
	BattleAudience* audience;
	BattleAudienceWindow* window;
	int i, j, index;
	u8 status;

	audience = BattleAudienceBaseGetPtr();
	window = BattleAudienceWinGetPtr();
	pouchGetPtr(); //useless call
	memset(audience, 0, sizeof(BattleAudience));
	BattleAudienceSettingAudience();
	audience->normalTex = tplRead("battle/audience/audience_normal.tpl");
	BattleAudienceGuestTPLRead(0, AUDIENCE_LUIGI, "audience_luigi.tpl");
	BattleAudienceSetThrowItemMax();

	audience->mTurnEndPhaseEventChance = 0;

	audience->mCurrentAudienceIntCountLeft = 0;
	audience->mCurrentAudienceIntCountRight = 0;
	audience->mCurrentAudienceIntCount = 0;

	for (i = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {
			index = (i * 20) + j + 10;
			if ((u8)BattleAudience_GetExist(index) == 1) {
				status = BattleAudienceGetPtr(index)->status;
				if (status != 12 && status != 11) { //TODO: add status enum
					audience->mCurrentAudienceIntCountRight++;
				}
			}
		}
	}

	for (i = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {
			index = (i * 20) + j;
			if ((u8)BattleAudience_GetExist(index) == 1) {
				status = BattleAudienceGetPtr(index)->status;
				if (status != 12 && status != 11) { //TODO: add status enum
					audience->mCurrentAudienceIntCountLeft++;
				}
			}
		}
	}

	audience->mCurrentAudienceIntCount = audience->mCurrentAudienceIntCountRight + audience->mCurrentAudienceIntCountLeft;
	window->count = (f32)audience->mCurrentAudienceIntCount;
	BattleAudienceSoundNoiseAlways();
	BattleAudienceSoundSetVol(2, 60, 180);
	audience->excited = FALSE;
	BattleAudienceNumToTarget();
}

void BattleAudience_Main(void) {
	BattleWork* wp = _battleWorkPointer;
	BattleAudience* audience = BattleAudienceBaseGetPtr();

	if (!(audience->flags & 0x10000) && wp->flags & 0x80) {
		BattleAudience_WinSetActive(1);
	}
	BattleAudienceCtrlProcess();
	BattleAudienceItemCtrlProcess();
	BattleAudienceApSrcCtrlProcess();
	BattleAudienceWinCtrlProcess();
	BattleAudienceSoundMain();
	if (wp->flags & 0x80) {
		audience->flags |= 0x10000;
	}
	else {
		audience->flags &= ~0x10000;
	}
}

void BattleAudience_ActInit(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	audience->mNumStylishCommandsThisAttack = 0;
	audience->flags &= ~0x10;
	audience->flags &= ~0x20;
	audience->flags &= ~0x40;
	audience->flags &= ~0x80;
	audience->flags &= ~0x100;
	audience->flags &= ~0x200;
	audience->flags &= ~0x400;
	audience->flags &= ~0x800;
	audience->flags &= ~0x1000;
}

void BattleAudience_PerAct(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	StarPowerInfo* info;
	u32 flags;
	s32 flag0, flag1, flag2;
	f32 delta;
	f32 value;
	BOOL positive;

	pouchGetPtr();
	delta = 0.0f;
	value = delta;
	if (audience->flags & 0x100) {
		if (!(audience->flags & 0x200)) {
			BattleAudienceCheer(3);
		}
	}
	else {
		BattleAudienceCheer(2);
	}
	if (audience->flags & 0x40000) {
		return;
	}
	if (!(audience->flags & 0x400) && (audience->flags & 0x800)) {
		delta -= 5.0f;
		BattleAudienceAddPhaseEvtList(5);
	}
	if (audience->impendingBonuses != NULL && ((flag0 = (flags = audience->flags) & 0x10) || (flags & 0x20))) {
		if (flag0) {
			positive = TRUE;
			audience->crowdPleasedStreak = audience->crowdPleasedStreak + 1;
			if (audience->crowdPleasedStreak > 2) {
				audience->crowdPleasedStreak = 2;
			}
			value = (f32)audience->crowdPleasedStreak * (audience->impendingBonuses)->mSpAcSuccessMultiplier;
			audience->crowdDispleasedStreak = 0;
			delta += value;
		}
		else {
			if (!(flags & 0x1000)) {
				audience->crowdPleasedStreak = 0;
				positive = FALSE;
				audience->crowdDispleasedStreak = audience->crowdDispleasedStreak + 1;
				if (audience->crowdDispleasedStreak > 5) {
					audience->crowdDispleasedStreak = 5;
				}
				value = -5.0f;
				delta += value;
			}
		}
	}
	if ((flag1 = audience->flags & 0x40) || (audience->flags & 0x80)) {
		if (flag1) {
			positive = TRUE;
			audience->crowdPleasedStreak = audience->crowdPleasedStreak + 1;
			if (audience->crowdPleasedStreak > 2) {
				audience->crowdPleasedStreak = 2;
			}
			value = (f32)audience->crowdPleasedStreak;
			audience->crowdDispleasedStreak = 0;
			delta += value;
		}
		else {
			audience->crowdPleasedStreak = 0;
			positive = FALSE;
		}
	}
	if (positive == TRUE) {
		if (value >= 25.0f) {
			BattleAudienceAddPhaseEvtList(0);
		}
		if (value >= 40.0f) {
			BattleAudienceAddPhaseEvtList(3);
		}
		if (value >= 20.0f) {
			BattleAudienceAddPhaseEvtList(8);
		}
		if (value >= 15.0f) {
			BattleAudienceAddPhaseEvtList(13);
		}
	}
	else {
		if (value <= -15.0f) {
			BattleAudienceAddPhaseEvtList(1);
		}
		if (value <= -20.0f) {
			BattleAudienceAddPhaseEvtList(2);
		}
		if (value <= -20.0f) {
			BattleAudienceAddPhaseEvtList(5);
		}
		if (value <= -15.0f) {
			BattleAudienceAddPhaseEvtList(9);
		}
		if (value <= -20.0f) {
			BattleAudienceAddPhaseEvtList(11);
		}
		if (value <= -10.0f) {
			BattleAudienceAddPhaseEvtList(13);
		}
	}
	info = audience->impendingBonuses;
	if (info != NULL && ((flag2 = audience->flags & 0x100) || (audience->flags & 0x200))) {
		if (flag2) {
			value = (f32)(info->mStylishCommandMultiplier * (audience->mNumStylishCommandsThisAttack * 2));
			delta += value;
			if (value >= 30.0f) {
				BattleAudienceAddPhaseEvtList(0);
			}
			if (value >= 40.0f) {
				BattleAudienceAddPhaseEvtList(3);
			}
			if (value >= 25.0f) {
				BattleAudienceAddPhaseEvtList(8);
			}
			if (value >= 20.0f) {
				BattleAudienceAddPhaseEvtList(13);
			}
			BattleAudienceAddPuni(0xA);
		}
		if (audience->flags & 0x200) {
			delta -= 5.0f;
		}
	}
	if (audience->flags & 0x8000) {
		audience->event = evtEntry(msg_puni_all_escape, 0, 0x20);
		audience->flags &= ~0x8000;
	}
	BattleAudienceAddTargetNum(audience->mAudienceDeltaMultiplier * (0.01f * delta), 0.0f);
	BattleAudience_ActInit();
}

u8 BattleAudience_CheckReaction(void) {
	BattleAudience* audience;
	EventEntry* evt;

	audience = BattleAudienceBaseGetPtr();
	BattleAudienceGetPtr(0); //unused
	evt = audience->event;
	if (evt && evtCheckID(evt->eventId)) {
		return TRUE;
	}
	audience->event = NULL;
	return FALSE;
}

u8 check_exe_phase_evt_status(s32 index, s32 type) {
	BattleAudienceMember* member;

	member = BattleAudienceGetPtr(index);
	if ((u8)BattleAudience_GetSysCtrl(index) == 0) {
		return FALSE;
	}
	if ((s32)member->type != type) {
		return FALSE;
	}
	if (member->status == 13) {
		return FALSE;
	}
	if (member->status == 12) {
		return FALSE;
	}
	if (member->status == 11) {
		return FALSE;
	}
	if (member->status == 21) {
		return FALSE;
	}
	if (member->status == 22) {
		return FALSE;
	}
	return (member->flags >> 3) & 1;
}

//TODO: re-type "value" for btlseq "states"
void BattleAudience_PerPhase(u32 value) {
	BattleAudience* audience;
	BattleAudienceMember* member;
	s32 i, itemId;

	audience = BattleAudienceBaseGetPtr();
	if (value == 0x4000002 && !(audience->flags & 0x40000)) {
		for (i = 0; i < 200; i++) {
			member = BattleAudienceGetPtr(i);
			if ((u8)BattleAudience_GetExist(i) == 1 && member->status == 7) {
				itemId = member->itemId;
				if (itemId != -1) {
					BattleAudienceItemGetPtr(itemId)->flags = 0;
				}
				member->itemId = -1;
				BattleAudience_ChangeStatus(i, 0);
			}
		}
		for (i = 0; i < 100; i++) {
			if (BattleAudienceItemGetPtr(i)->state <= 1) {
				BattleAudienceItemGetPtr(i)->flags = 0;
			}
		}
	}
	if (value == 0x4000004) {
		BattleAudienceSetThrowItemMax();
		audience->flags |= 8;
		audience->mCheckPhaseReactionState = 0;
	}
}

u8 BattleAudience_CheckReactionPerPhase(void) {
	BattleAudience* audience;
	BattleAudienceMember* member;
	s32 evtList[200];
	s32 phaseList[14];
	s32 i, j, k, n, count, idx;

	audience = BattleAudienceBaseGetPtr();
	pouchGetPtr();
	if (!(audience->flags & 8)) {
		return FALSE;
	}
	switch (audience->mCheckPhaseReactionState) {
	case 0:
		audience->mCheckPhaseReactionState = 5;
		if (audience->flags & 0x40000) {
			audience->mCheckPhaseReactionState = 15;
			break;
		}
		// fall through
	case 5:
		count = 0;
		for (i = 0; i < 200; i++) {
			member = BattleAudienceGetPtr(i);
			if ((u8)BattleAudience_GetSysCtrl(i) == 1 && member->type == AUDIENCE_BULKY_BOBOMB &&
					member->status == 18) {
				member->timer--;
				if (member->timer <= 0) {
					BattleAudience_ChangeStatus(i, 19);
					count++;
				}
			}
		}
		if (count >= 1) {
			audience->mCheckPhaseReactionState = 6;
		}
		else {
			audience->mCheckPhaseReactionState = 7;
		}
		// fall through
	case 6:
	case 7:
		for (j = 0; j < 200; j++) {
			member = BattleAudienceGetPtr(j);
			if ((u8)BattleAudience_GetSysCtrl(j) == 1 && member->type == AUDIENCE_BULKY_BOBOMB &&
					member->status == 19) {
				break;
			}
		}
		if (j == 200) {
			if (audience->mCheckPhaseReactionState == 6) {
				audience->mCheckPhaseReactionState = 8;
			}
			else {
				audience->mCheckPhaseReactionState = 13;
			}
		}
		break;
	case 8:
		audience->event = evtEntry(msg_heavy_bomb_fire, 0, 0x20);
		audience->mCheckPhaseReactionState = 9;
		break;
	case 9:
		if (!evtCheckID(audience->event->eventId)) {
			audience->event = NULL;
			audience->mCheckPhaseReactionState = 10;
		}
		break;
	case 10:
		if (audience->flags & 0x8000) {
			audience->event = evtEntry(msg_puni_all_escape, 0, 0x20);
			audience->flags &= ~0x8000;
			audience->mCheckPhaseReactionState = 11;
		}
		else {
			audience->mCheckPhaseReactionState = 15;
		}
		break;
	case 11:
		if (!evtCheckID(audience->event->eventId)) {
			audience->event = NULL;
			audience->mCheckPhaseReactionState = 15;
		}
		break;
	case 13:
		memcpy(phaseList, audience->phaseEvtList, audience->phaseEvtCount * 4);
		n = audience->phaseEvtCount;
		audience->phaseEvtCount = 0;
		for (i = 0; i < n; i++) {
			switch (phaseList[i]) {
			case 0:
				for (j = 0; j <= 59; j++) {
					if (check_exe_phase_evt_status(j, 0)) {
						BattleAudienceAddPhaseEvtList(0);
						break;
					}
				}
				break;
			case 1:
				for (j = 0; j <= 59; j++) {
					if (check_exe_phase_evt_status(j, 8)) {
						if (BattleAudienceGetPtr(j)->status != 0x11) {
							BattleAudienceAddPhaseEvtList(1);
							break;
						}
					}
				}
				break;
			case 2:
				for (j = 0; j <= 59; j++) {
					if (check_exe_phase_evt_status(j, 4)) {
						BattleAudienceAddPhaseEvtList(2);
						break;
					}
				}
				break;
			case 3:
				if (audience->typeWeights[7] != 0) {
					for (j = 0; j < audience->mMaxAudience; ) {
						if (!(u8)BattleAudience_GetExist(j)) {
							BattleAudienceAddPhaseEvtList(3);
							break;
						}
						member = BattleAudienceGetPtr(j);
						if (member->flags & 0x80) {
							j += 2;
						}
						else {
							j += 1;
						}
					}
				}
				break;
			case 4:
				for (j = 0; j <= 59; j++) {
					if (check_exe_phase_evt_status(j, 2)) {
						BattleAudienceAddPhaseEvtList(4);
						break;
					}
				}
				break;
			case 5:
				for (j = 0; j <= 59; j++) {
					if (check_exe_phase_evt_status(j, 1)) {
						BattleAudienceAddPhaseEvtList(5);
						break;
					}
				}
				break;
			case 6:
				for (j = 0; j <= 59; j++) {
					if (check_exe_phase_evt_status(j, 1)) {
						BattleAudienceAddPhaseEvtList(6);
						break;
					}
				}
				break;
			case 7:
				for (j = 0; j <= 59; j++) {
					if (check_exe_phase_evt_status(j, 3)) {
						BattleAudienceAddPhaseEvtList(7);
						break;
					}
				}
				break;
			case 8:
				for (j = 0; j <= 59; j++) {
					if (check_exe_phase_evt_status(j, 5)) {
						BattleAudienceAddPhaseEvtList(8);
						break;
					}
				}
				break;
			case 9:
				if (!(audience->flags & 0x4000)) {
					for (j = 0; j <= 59; j++) {
						if (check_exe_phase_evt_status(j, 5)) {
							BattleAudienceAddPhaseEvtList(9);
							break;
						}
					}
				}
				break;
			case 10:
				for (j = 0; j <= 59; j++) {
					if (check_exe_phase_evt_status(j, 6)) {
						BattleAudienceAddPhaseEvtList(0xA);
						break;
					}
				}
				break;
			case 11:
				if (!(audience->flags & 0x4000)) {
					for (j = 0; j <= 59; j++) {
						if (check_exe_phase_evt_status(j, 9) &&
								BattleAudienceGetPtr(j)->status != 0x12) {
							BattleAudienceAddPhaseEvtList(0xB);
							break;
						}
					}
				}
				break;
			case 12:
				if (!(audience->flags & 0x4000)) {
					for (j = 0; j <= 59; j++) {
						if (check_exe_phase_evt_status(j, 0xB) &&
								BattleAudienceDetectPakkunEatTarget(j) != -1) {
							BattleAudienceAddPhaseEvtList(0xC);
							break;
						}
					}
				}
				break;
			case 13:
				for (j = 0; j <= 59; j++) {
					if (check_exe_phase_evt_status(j, 0xC)) {
						BattleAudienceAddPhaseEvtList(0xD);
						break;
					}
				}
				break;
			}
		}
		if (audience->phaseEvtCount >= 1) {
			audience->phaseEvtList[0] = audience->phaseEvtList[irand(audience->phaseEvtCount)];
			audience->phaseEvtCount = 1;
		}
		else {
			audience->phaseEvtCount = 0;
		}
		audience->mCheckPhaseReactionState = 12;
		audience->phaseStep = 0;
		// fall through
	case 12:
		if (audience->phaseEvtCount == 1) {
		switch (audience->phaseEvtList[0]) {
		case 0:
			k = 0;
			for (i = 0; i <= 59; i++) {
				if (check_exe_phase_evt_status(i, 0)) {
					evtList[k++] = i;
				}
			}
			BattleAudienceItemOn(evtList[irand(k)], 0, 1);
			audience->phaseEvtCount = 0;
			break;
		case 1:
			for (i = 0; i < 200; i++) {
				if (check_exe_phase_evt_status(i, 8)) {
					if (BattleAudienceGetPtr(i)->status != 0x11) {
						BattleAudience_ChangeStatus(i, 0x11);
						BattleAudienceGetPtr(i)->timer = 5;
					}
				}
			}
			audience->phaseEvtCount = 0;
			break;
		case 2:
			for (i = 0; i <= 59; i++) {
				if (check_exe_phase_evt_status(i, 4)) {
					BattleAudienceItemOn(i, 0xEE, 0x64);
				}
			}
			audience->phaseEvtCount = 0;
			break;
		case 3:
			switch (audience->phaseStep) {
			case 0:
				BattleAudienceAddPuni(0xC8);
				audience->phaseStep = 5;
				break;
			case 5:
				for (i = 0; i < 200; i++) {
					if ((u8)BattleAudience_GetSysCtrl(i) && BattleAudienceGetPtr(i)->status == 0xB) {
						break;
					}
				}
				if (i == 200) {
					audience->phaseStep = 0xA;
				}
				break;
			case 10:
				audience->phaseStep = 0xF;
				audience->event = evtEntry(msg_puni_all_enter, 0, 0x20);
				break;
			case 15:
				if (!evtCheckID(audience->event->eventId)) {
					audience->event = NULL;
					audience->phaseEvtCount = 0;
				}
				break;
			}
			break;
		case 4:
			switch (audience->phaseStep) {
			case 0:
				count = 0;
				for (i = 0; i <= 59; i++) {
					if (check_exe_phase_evt_status(i, 2)) {
						evtList[count++] = i;
					}
				}
				BattleAudience_ChangeStatus(evtList[irand(count)], 0xE);
				audience->phaseStep = 5;
				// fall through
			case 5:
				for (i = 0; i <= 59; i++) {
					if ((u8)BattleAudience_GetSysCtrl(i) && BattleAudienceGetPtr(i)->status == 0xE) {
						break;
					}
				}
				if (i == 0x3C) {
					audience->phaseEvtCount = 0;
				}
				break;
			default:
				if (audience->phaseStep >= 5) {
					break;
				}
				break;
			}
			break;
		case 5:
			for (i = 0; i <= 59; i++) {
				if (check_exe_phase_evt_status(i, 1)) {
					BattleAudienceItemOn(i, 0xED, 0x64);
				}
			}
			audience->phaseEvtCount = 0;
			break;
		case 6:
			count = 0;
			for (i = 0; i <= 59; i++) {
				if (check_exe_phase_evt_status(i, 1)) {
					evtList[count++] = i;
				}
			}
			BattleAudienceItemOn(evtList[irand(count)], 0, 1);
			audience->phaseEvtCount = 0;
			break;
		case 7:
			count = 0;
			for (i = 0; i <= 59; i++) {
				if (check_exe_phase_evt_status(i, 3)) {
					evtList[count++] = i;
				}
			}
			BattleAudienceItemOn(evtList[irand(count)], 0xEF, 1);
			audience->phaseEvtCount = 0;
			break;
		case 8:
			count = 0;
			for (i = 0; i <= 59; i++) {
				if (check_exe_phase_evt_status(i, 5)) {
					evtList[count++] = i;
				}
			}
			BattleAudienceItemOn(evtList[irand(count)], 0, 1);
			audience->phaseEvtCount = 0;
			break;
		case 9:
			switch (audience->phaseStep) {
			case 0:
				count = 0;
				for (i = 0; i <= 59; i++) {
					if (check_exe_phase_evt_status(i, 5)) {
						evtList[count++] = i;
					}
				}
				BattleAudience_ChangeStatus(evtList[irand(count)], 0xF);
				audience->phaseStep = 5;
				// fall through
			case 5:
				for (i = 0; i <= 59; i++) {
					if ((u8)BattleAudience_GetSysCtrl(i) && BattleAudienceGetPtr(i)->status == 0xF) {
						break;
					}
				}
				if (i == 0x3C) {
					audience->phaseEvtCount = 0;
				}
				break;
			default:
				if (audience->phaseStep >= 5) {
					break;
				}
				break;
			}
			break;
		case 10:
			switch (audience->phaseStep) {
			case 0:
				count = 0;
				for (i = 0; i <= 59; i++) {
					if (check_exe_phase_evt_status(i, 6)) {
						evtList[count++] = i;
					}
				}
				BattleAudience_ChangeStatus(evtList[irand(count)], 0x10);
				audience->phaseStep = 5;
				// fall through
			case 5:
				for (i = 0; i <= 59; i++) {
					if ((u8)BattleAudience_GetSysCtrl(i) && BattleAudienceGetPtr(i)->status == 0x10) {
						break;
					}
				}
				if (i == 0x3C) {
					audience->phaseStep = 0xA;
				}
				break;
			case 10:
				audience->phaseStep = 0xF;
				audience->event = evtEntry(msg_pansy_sing, 0, 0x20);
				break;
			case 15:
				if (!evtCheckID(audience->event->eventId)) {
					audience->event = NULL;
					audience->phaseEvtCount = 0;
				}
				break;
			}
			break;
		case 11:
			count = 0;
			for (i = 0; i <= 59; i++) {
				if (check_exe_phase_evt_status(i, 9) &&
						BattleAudienceGetPtr(i)->status != 0x12) {
					evtList[count++] = i;
				}
			}
			idx = evtList[irand(count)];
			member = BattleAudienceGetPtr(idx);
			BattleAudience_ChangeStatus(idx, 0x12);
			member->timer = 5;
			audience->phaseEvtCount = 0;
			break;
		case 12:
			switch (audience->phaseStep) {
			case 0:
				count = 0;
				for (i = 0; i <= 59; i++) {
					if (check_exe_phase_evt_status(i, 0xB) &&
							BattleAudienceDetectPakkunEatTarget(i) != -1) {
						evtList[count++] = i;
					}
				}
				BattleAudience_ChangeStatus(evtList[irand(count)], 0x14);
				audience->phaseStep = 5;
				// fall through
			case 5:
				for (i = 0; i <= 59; i++) {
					if ((u8)BattleAudience_GetSysCtrl(i) && BattleAudienceGetPtr(i)->status == 0x14) {
						break;
					}
				}
				if (i == 0x3C) {
					audience->phaseEvtCount = 0;
				}
				break;
			default:
				if (audience->phaseStep >= 5) {
					break;
				}
				break;
			}
			break;
		case 13:
			for (i = 0; i <= 59; i++) {
				if (check_exe_phase_evt_status(i, 0xC)) {
					BattleAudienceItemOn(i, 0, 1);
					break;
				}
			}
			audience->phaseEvtCount = 0;
			break;
		}
		}
		else {
			audience->mCheckPhaseReactionState = 15;
		}
		break;
	case 15:
		for (i = 0; i < 200; i++) {
			member = BattleAudienceGetPtr(i);
			if ((u8)BattleAudience_GetSysCtrl(i) == 1 && member->status == 0xD) {
				member->singCount -= 1;
				if (member->singCount <= 0) {
					BattleAudience_ChangeStatus(i, 3);
					member->singCount = 0;
				}
			}
		}
		for (i = 0; i < 200; i++) {
			member = BattleAudienceGetPtr(i);
			if ((u8)BattleAudience_GetSysCtrl(i) == 1 && member->type == AUDIENCE_KOOPA &&
					member->status == 0x11) {
				member->timer--;
				if (member->timer <= 0) {
					member->step = 4;
				}
			}
		}
		BattleBreakSlot_DecBreakTurn();
		// fall through
	case 20:
		audience->flags &= ~8;
		audience->phaseEvtCount = 0;
		return FALSE;
	}
	return TRUE;
}

void BattleAudience_Disp(void) {
	BattleAudienceBaseGetPtr();
	BattleAudienceAnimProcess();
	if (!(BattleAudienceBaseGetPtr()->flags & 0x20000)) {
		dispEntry(CAMERA_3D, 0.0f, 1, BattleAudienceDispAudience, NULL);
		dispEntry(CAMERA_3D, 0.0f, 2, BattleAudienceDispAudience, NULL);
		dispEntry(CAMERA_3D, 0.0f, 1, BattleAudienceDispItem, NULL);
		dispEntry(CAMERA_2D, 499.0f, 0, BattleAudienceDispApSrc, NULL);
	}
	if (pouchGetPtr()->mStarPowersObtained) {
		dispEntry(CAMERA_2D, 499.0f, 1, BattleAudienceDispWin, NULL);
	}
}

void BattleAudience_End(void) {
	BattleAudience* audience;
	FileEntry* obj;
	int i;

	audience = BattleAudienceBaseGetPtr();
	for (i = 0; i < 2; i++) {
		obj = audience->guestTex[i];
		if (obj) {
			fileFree(obj);
		}
	}

	obj = audience->normalTex;
	if (obj) {
		fileFree(obj);
	}

	if (pouchGetPtr()->mStarPowersObtained) {
		if (audience->mTargetAudienceCount < 0.0f) {
			audience->mTargetAudienceCount = 0.0f;
		}
		if (audience->mTargetAudienceCount > (f32)audience->mMaxAudience) {
			audience->mTargetAudienceCount = (f32)audience->mMaxAudience;
		}
		pouchSetAudienceNum(audience->mTargetAudienceCount);
	}
}

// The final dispatch is a `switch`; the clamp index is a separate `deltaIdx` ternary, `i = 0`
// is set before `target = 0`, `listTotal` is declared after `kind`, the random-multiplier cast
// is inlined into the FMA. The weight sort walks `kind` from `k + 1`, tests
// `weights[k] == weights[kind]` and swaps `order[k]` first; `diff_shape.py` shows the same calls
// and branches as the original.
void BattleAudienceSettingAudience(void) {
	s32 kind;
	s32 deltaIdx;
	s32 i, j, k;
	s32 target;
	s32 targetNum;
	s32 order[12];
	s32 weights[12];
	s32 counts[13];
	BattleAudienceTypeWeights typeWeights;
	BattleAudienceDeltaMultiplierTable deltaMultiplier;
	s32 list[200];
	f32 oldAudience;
	s32 mode;
	s32 maxKinds;
	s32 sum;
	s32 total;
	BattleWorkField19068* force;
	PouchData* pouch;
	s32* setupBase;
	s32 listTotal;
	BattleAudience* audience;

	force = &_battleWorkPointer->force;
	audience = BattleAudienceBaseGetPtr();
	setupBase = &_battleWorkPointer->fbatInfo->setup->audienceMode;
	pouch = pouchGetPtr();

	deltaMultiplier =
	    *(const BattleAudienceDeltaMultiplierTable*)R_audience_delta_multiplier;

	switch (pouch->rank) {
	case 0:
		audience->mMaxAudience = 50;
		break;
	case 1:
		audience->mMaxAudience = 100;
		break;
	case 2:
		audience->mMaxAudience = 150;
		break;
	case 3:
		audience->mMaxAudience = 200;
		break;
	}

	if (pouchGetPtr()->mStarPowersObtained == 0) {
		audience->mMaxAudience = 0;
	}

	deltaIdx = (pouch->level - 1 < 0x41) ? pouch->level - 1 : 0x40;
	audience->mAudienceDeltaMultiplier = deltaMultiplier.v[deltaIdx];

	if (force->forceFlags & 0x10000) {
		audience->mTargetAudienceCount = (f32)force->forceAudienceCount;
		if (audience->mTargetAudienceCount > (f32)audience->mMaxAudience) {
			audience->mTargetAudienceCount = (f32)audience->mMaxAudience;
		}
	}
	else {
		oldAudience = pouchGetAudienceNum();
		audience->mTargetAudienceCount =
		    0.01f * ((pouchGetAudienceNum() - audience->mAudienceDeltaMultiplier) * (f32)(irand(10) - 13 + irand(10))) + oldAudience;
	}

	switch (*setupBase) {
	case 0:
	case 1:
		audience->mAddedAudienceCount = 0.0f;
		break;
	case 4:
		audience->mAddedAudienceCount = 0.5f * audience->mTargetAudienceCount;
		break;
	case 2:
	case 3:
	case 5:
	case 6:
		audience->mAddedAudienceCount = (f32)audience->mMaxAudience - audience->mTargetAudienceCount;
		break;
	}

	switch (*setupBase) {
	case 2:
		for (i = 0; i < 12; i++) {
			if (i == 1 || i == 7) {
				weights[i] = 1;
			}
			else {
				weights[i] = 0;
			}
		}
		mode = 0;
		break;
	case 5:
		for (i = 0; i < 12; i++) {
			if (i == 1) {
				weights[i] = 1;
			}
			else {
				weights[i] = 0;
			}
		}
		mode = 0;
		break;
	case 6:
		for (i = 0; i < 12; i++) {
			if (audience_kind[i].flags & AUDIENCE_KIND_TWO_SEATS) {
				weights[i] = 2;
			}
			else {
				weights[i] = 1;
			}
		}
		mode = 1;
		break;
	default: {
		typeWeights = *(const BattleAudienceTypeWeights*)R_num_audience_type_weights;
		sum = 0;
		j = irand(100);
		maxKinds = 12;
		for (i = 1; i <= 11; i++) {
			sum += typeWeights.v[i - 1];
			if (j < sum) {
				maxKinds = i;
				break;
			}
		}

		for (i = 0; i < 12; i++) {
			order[i] = i;
		}
		for (k = 0; k < 24; k++) {
			s32 a = irand(12);
			s32 b = irand(12);
			s32 t = order[a];
			order[a] = order[b];
			order[b] = t;
		}

		i = 0;
		target = 0;
		for (; i <= 11; i++) {
			kind = order[i];
			if (target >= maxKinds) {
				weights[kind] = 0;
			}
			else {
				if (((s8*)setupBase)[4 + kind] != 0) {
					weights[kind] = ((s8*)setupBase)[4 + kind] * (irand(10) + 1);
					if (audience_kind[kind].flags & AUDIENCE_KIND_TWO_SEATS) {
						weights[kind] *= 2;
					}
					target++;
				}
				else {
					weights[kind] = 0;
				}
			}
		}

		if (force->forceAudienceKind != 0) {
			switch (force->forceAudienceKind) {
			case 1:
				kind = 0;
				break;
			case 2:
				kind = 10;
				break;
			case 3:
				kind = 8;
				break;
			case 4:
				kind = 4;
				break;
			case 5:
				kind = 7;
				break;
			case 6:
				kind = 2;
				break;
			case 7:
				kind = 1;
				break;
			case 8:
				kind = 3;
				break;
			case 9:
				kind = 5;
				break;
			case 10:
				kind = 6;
				break;
			case 11:
				kind = 9;
				break;
			case 12:
				kind = 11;
				break;
			}
			for (i = 0; i < 12; i++) {
				if (i == kind) {
					weights[i] = 1;
				}
				else {
					weights[i] = 0;
				}
			}
		}
		mode = 1;
		break;
	}
	}

	memset(counts, 0, sizeof(counts));

	targetNum = (s32)(audience->mTargetAudienceCount + audience->mAddedAudienceCount);
	if (targetNum < 0) {
		targetNum = 0;
	}
	if (targetNum > audience->mMaxAudience) {
		targetNum = audience->mMaxAudience;
	}

	total = 0;
	sum = 0;
	for (i = 0; i < 12; i++) {
		sum += weights[i];
	}

	for (i = 0; i < 12; i++) {
		counts[i] = targetNum * weights[i] / sum;
		if (audience_kind[i].flags & AUDIENCE_KIND_TWO_SEATS) {
			counts[i] /= 2;
			total += counts[i] * 2;
		}
		else {
			total += counts[i];
		}
	}

	for (i = 0; i < 12; i++) {
		order[i] = i;
	}

	for (k = 0; k <= 10; k++) {
		for (kind = k + 1; kind <= 11; kind++) {
			if ((weights[k] == weights[kind] && irand(2) == 0) || weights[k] < weights[kind]) {
				s32 t = order[k];
				order[k] = order[kind];
				order[kind] = t;
			}
		}
	}

	for (i = 0, j = 0; total < targetNum; i++, i %= 12) {
		kind = order[i];
		if (weights[kind] <= 0) {
			continue;
		}
		if (audience_kind[kind].flags & AUDIENCE_KIND_TWO_SEATS) {
			if (total <= targetNum - 2) {
				counts[kind]++;
				total += 2;
				j = 0;
			}
			else if (++j >= 12) {
				break;
			}
		}
		else {
			counts[kind]++;
			total++;
		}
	}

	if (1 == mode && total != 0 && (irand(20) == 0 || (force->forceFlags & 0x40000))) {
		for (i = 0; i < 12; i++) {
			order[i] = i;
		}
		for (i = 0; i < 24; i++) {
			s32 a = irand(12);
			s32 b = irand(12);
			s32 t = order[a];
			order[a] = order[b];
			order[b] = t;
		}
		for (i = 0; i < 12; i++) {
			kind = order[i];
			if (counts[kind] > 0) {
				counts[kind]--;
				counts[12]++;
				break;
			}
		}
	}

	switch (*setupBase) {
	case 2:
		while ((kind = BattleAudience_NoUsedHaitiRand()) != -1) {
			if (kind % 20 <= 9) {
				BattleAudience_Entry(kind, AUDIENCE_PUNI, 0);
			}
			else {
				BattleAudience_Entry(kind, AUDIENCE_XNAUT, 0);
			}
		}
		break;
	default: {
		kind = 0;
		listTotal = 0;
		for (; kind <= 12; kind++) {
			for (j = 0; j < counts[kind]; j++) {
				list[listTotal++] = kind;
			}
		}
		for (i = 0; i < 150; i++) {
			s32 a = irand(listTotal);
			s32 b = irand(listTotal);
			s32 t = list[a];
			list[a] = list[b];
			list[b] = t;
		}
		for (i = 0; i < listTotal; i++) {
			kind = BattleAudience_NoUsedFCHaitiRand();
			if (kind == -1) {
				break;
			}
			BattleAudience_Entry(kind, (u8)list[i], 0);
		}
		break;
	}
	}

	memcpy(audience->typeWeights, weights, 0x30);
}

void BattleAudienceGuestTPLRead(s32 id, AudienceMemberType member, const char* path) {
	char full_path[256];

	BattleAudience* audience = BattleAudienceBaseGetPtr();
	if (id < 2) {
		sprintf(full_path, "battle/audience/%s", path);
		audience->guestTex[id] = tplRead(full_path);
		audience->guestType[id] = member;
	}
}

// 100%: writing the first loop's index as `j + i * 20 + 10` (instead of `i * 20 + 10 + j`) and
// declaring `s32 r;` immediately after `wp` (before every other local) put `force` (&wp->force)
// in r27 and `r` in r28 as the original does.
void BattleAudienceCtrlProcess(void) {
	BattleWork* wp = _battleWorkPointer;
	s32 r;
	BattleWorkField19068* force = &wp->force;
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	BattleAudienceMember* member;
	s32 order[200];
	s32 i;
	s32 j;
	s32 index;
	s32 temp;
	s32 a;
	s32 b;

	audience->mCurrentAudienceIntCountLeft = 0;
	audience->mCurrentAudienceIntCountRight = 0;
	audience->mCurrentAudienceIntCount = 0;

	for (i = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {
			index = j + i * 20 + 10;
			if ((u8)BattleAudience_GetExist(index) == 1) {
				member = BattleAudienceGetPtr(index);
				if (member->status != 0xC && member->status != 0xB) {
					audience->mCurrentAudienceIntCountRight++;
				}
			}
		}
	}

	for (i = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {
			index = i * 20 + j;
			if ((u8)BattleAudience_GetExist(index) == 1) {
				member = BattleAudienceGetPtr(index);
				if (member->status != 0xC && member->status != 0xB) {
					audience->mCurrentAudienceIntCountLeft++;
				}
			}
		}
	}

	i = 0;
	audience->mCurrentAudienceIntCount =
	    audience->mCurrentAudienceIntCountRight + audience->mCurrentAudienceIntCountLeft;

	member = BattleAudienceGetPtr(0);
	for (; i < 200; i++, member++) {
		if (member->flags & 0x10) {
			f32 zero = 0.0f;

			member->jumpOffset.y += member->jumpVelY;
			member->jumpVelY += member->jumpAccelY;
			if (member->jumpOffset.y <= zero) {
				member->jumpOffset.y = zero;
				member->jumpVelY = zero;
				member->jumpAccelY = zero;
			}
		}
		else if ((u8)BattleAudience_GetExist(i) != 0) {
			switch (member->type) {
			case AUDIENCE_TOAD:
				BattleAudienceCtrlProcessKinopio(i);
				break;
			case AUDIENCE_XNAUT:
				BattleAudienceCtrlProcessKinopio(i);
				break;
			case AUDIENCE_BOO:
				BattleAudienceCtrlProcessKinopio(i);
				break;
			case AUDIENCE_HAMMER_BRO:
				BattleAudienceCtrlProcessKinopio(i);
				break;
			case AUDIENCE_DULL_BONES:
				BattleAudienceCtrlProcessKinopio(i);
				break;
			case AUDIENCE_SHY_GUY:
				BattleAudienceCtrlProcessKinopio(i);
				break;
			case AUDIENCE_CRAZY_DAYZEE:
				BattleAudienceCtrlProcessKinopio(i);
				break;
			case AUDIENCE_PUNI:
				BattleAudienceCtrlProcessKinopio(i);
				break;
			case AUDIENCE_KOOPA:
				BattleAudienceCtrlProcessKinopio(i);
				break;
			case AUDIENCE_BULKY_BOBOMB:
				BattleAudienceCtrlProcessKinopio(i);
				break;
			case AUDIENCE_GOOMBA:
				BattleAudienceCtrlProcessKinopio(i);
				break;
			case AUDIENCE_PIRANHA_PLANT:
				BattleAudienceCtrlProcessKinopio(i);
				break;
			case AUDIENCE_LUIGI:
				BattleAudienceCtrlProcessKinopio(i);
				break;
			}
		}
	}

	if (force->forceFlags & 0x20000) {
		audience->itemThrowMax = 0x2710;
	}

	if (wp->flags & 0x80) {
		if (audience->itemThrowCount < audience->itemThrowMax) {
			switch (audience->itemThrowCount) {
			case 0:
				r = 0x708;
				break;
			case 1:
				r = 0x1770;
				break;
			case 2:
				r = 0x7530;
				break;
			}
			if (force->forceFlags & 0x20000) {
				r = 0x3C;
			}
			if (irand(r) == 0) {
				for (i = 0; i < 200; i++) {
					order[i] = i;
				}
				for (i = 0; i < 1000; i++) {
					a = irand(200);
					b = irand(200);
					temp = order[a];
					order[a] = order[b];
					order[b] = temp;
				}
				for (i = 0; i < 200; i++) {
					if (BattleAudienceItemOn(order[i], 0, 1) == 1) {
						(audience->itemThrowCount)++;
						break;
					}
				}
			}
		}
	}
}

BOOL BattleAudienceItemOn(s32 index, u16 itemId, s32 numItems) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	BattleAudienceMember* member = BattleAudienceGetPtr(index);
	BattleAudienceItem* item;
	s32 i;
	if (audience->flags & 0x40000) {
		return FALSE;
	}
	if (audience_kind[member->type].itemTable->type == 0) {
		return FALSE;
	}
	if (itemId >= 0x79 && itemId < 0xec && swGet(0xeb) == 0) {
		return FALSE;
	}
	if (index % 20 < 4 || index % 20 > 0xf) {
		return FALSE;
	}
	if ((u8)BattleAudience_GetExist(index) == 0) {
		return FALSE;
	}
	if (member->status == 7) {
		return FALSE;
	}
	if (member->status == 8) {
		return FALSE;
	}
	if (member->status == 0xd) {
		return FALSE;
	}
	if (member->status == 0xc) {
		return FALSE;
	}
	if (member->status == 0xb) {
		return FALSE;
	}
	if (member->status == 0x12) {
		return FALSE;
	}
	if (member->status == 0x11) {
		return FALSE;
	}
	if (member->status == 0x15) {
		return FALSE;
	}
	if (member->status == 0xa) {
		return FALSE;
	}
	if (member->flags & 8) {
		i = 0;
		item = BattleAudienceItemGetPtr(0);
		for (; i < numItems; i++, item++) {
			if (!(item->flags & 1)) {
				item->flags |= 1;
				item->state = 0;
				item->memberId = index;
				item->mItemType = itemId;
				BattleAudience_ChangeStatus(index, 7);
				member->itemId = i;
				return TRUE;
			}
		}
	}
	return FALSE;
}

void BattleAudienceSetThrowItemMax(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 r = irand(100);
	if (r < 0x50) {
		audience->itemThrowMax = 0;
	}
	else if (r < 0x5a) {
		audience->itemThrowMax = 1;
	}
	else if (r < 0x5f) {
		audience->itemThrowMax = 2;
	}
	else if (r < 0x64) {
		audience->itemThrowMax = 3;
	}
	audience->itemThrowCount = 0;
	audience->itemCombo = 0;
}

BattleUnit* BattleAudienceDetectTargetAll(void) {
	s32 ids[64];
	s32 offset;
	s32 i;
	BattleWork* wp = _battleWorkPointer;
	BattleUnit* unit = NULL;
	s32 count;
	s32 sel;
	BattleAudienceBaseGetPtr();
	count = 0;
	offset = 0;
	i = 0;
	for (; i < 0x40; i++) {
		unit = wp->units[i];
		if (unit != NULL) {
			BattleUnitPart* part = BattleGetUnitPartsPtr(i, BtlUnit_GetBodyPartsId(unit));
			if (!(part->attributes & kNotTargetable) && unit->currentHp > 0 && (unit->currentKind < 0xD8 || unit->currentKind >= 0xDA)) {
				ids[count] = i;
				count++;
				offset += 4;
			}
		}
	}
	if (count == 0) {
		return NULL;
	}
	sel = irand(count);
	return BattleGetUnitPtr(wp, ids[sel]);
}

BattleUnit* BattleAudienceDetectTargetPlayer(void) {
	s32 ids[64];
	s32 offset;
	s32 i;
	BattleWork* wp = _battleWorkPointer;
	BattleUnit* unit = NULL;
	s32 count;
	s32 sel;
	BattleAudienceBaseGetPtr();
	count = 0;
	offset = 0;
	i = 0;
	for (; i < 0x40; i++) {
		unit = wp->units[i];
		if (unit != NULL) {
			BattleUnitPart* part = BattleGetUnitPartsPtr(i, BtlUnit_GetBodyPartsId(unit));
			if (unit->mAlliance == 0 && unit->currentHp > 0 && !(part->attributes & kNotTargetable)) {
				ids[count] = i;
				count++;
				offset += 4;
			}
		}
	}
	if (count == 0) {
		return NULL;
	}
	sel = irand(count);
	return BattleGetUnitPtr(wp, ids[sel]);
}

// The weight-count loop is written as `for (;;) { if (type == 0) break; ... }` so MWCC does not rotate it (a
// `while`/`for` rotated the condition to the bottom). What got it to 98.46%:
//  - every `float_*_8041e6xx` external was replaced by its literal (0.0f, 1.0f, 10.0f, …), which is what the
//    original source used: with literals MWCC colours the FP registers as the target does;
//  - the outer `if (item+0x44)` was inverted so the else body is the fall-through branch;
//  - the case-0 position is a compound literal stored after the scale/angle stores, not a copy of a
//    named Vec: MWCC then schedules the arithmetic `y, z, x` like the original instead of `z, x, y`;
//  - the case-3 inner `if (0x44 == 2)` jumps straight to the case-4 body like the original (a
//    `goto`; without it MWCC falls through the `state != 4` check, 1 instruction);
//  - the deactivate path reuses `member` for the second `BattleAudienceGetPtr` call, which is what the
//    original did (it materialises -1 in r0, not r22).
// 100%: every call, immediate, value, branch and the case-0 scheduling match.
void BattleAudienceItemCtrlProcess(void) {
	s32 i;
	BattleWork* wp = _battleWorkPointer;
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	BattleAudienceItem* item;
	BattleAudienceMember* member;
	BattleAudienceKind* kind;
	BattleUnit* unit;
	BattleUnitPart* part;
	BattleAudienceItemKind* kindItems;
	Vec hitPos;
	f32 dist;
	f32 dx;
	s8 turns;
	s8 strength;
	s32 j;
	s32 count;
	s32 offset;
	s32 total;
	s32 random;
	s32 acc;

	i = 0;
	item = BattleAudienceItemGetPtr(0);
	for (; i < 100; i++, item++) {
		if (!(item->flags & 1)) {
			continue;
		}
		member = BattleAudienceGetPtr(item->memberId);
		kind = &audience_kind[(s32)member->type];
		// goto: the remove and deactivate tails inside the switch are shared by these exits and case 7
		if (member->status == 0xd || member->status == 0x13 || member->status == 0x15 || member->status == 0xc) {
			goto deactivate;
		}
		if ((u8)BattleAudience_GetExist(item->memberId) == 0) {
			goto deactivate;
		}
		switch (item->state) {
		case 0:
			item->flags |= 2;
			if (item->mItemType == 0) {
				kindItems = kind->itemTable;
				count = 0;
				offset = 0;
				total = 0;
				for (;;) {
					if (kindItems[offset].type == 0) {
						break;
					}
					total += kindItems[offset].chance;
					count++;
					offset++;
				}
				if (count == 0) {
					goto remove;
				}
				random = irand(total);
				offset = 0;
				acc = 0;
				for (;;) {
					if (random >= acc && random < acc + kindItems[offset].chance) {
						break;
					}
					acc += kindItems[offset].chance;
					offset++;
				}
				item->mItemType = kindItems[offset].type;
			}
			item->timer = i + 0xb4;
			item->scale = 1.0f;
			item->angleZ = 0.0f;
			item->pos = (Vec){ member->position.x + kind->offsetX, member->position.y + kind->offsetY - 10.0f,
			                   member->position.z + 1.0f };
			item->state++;
			/* fallthrough */
		case 1:
			if (item->timer <= 0) {
				for (j = 0; j < 100; j++) {
					if (j == i) {
						continue;
					}
					if (!(BattleAudienceItemGetPtr(j)->flags & 1)) {
						continue;
					}
					if (BattleAudienceItemGetPtr(j)->state != 1) {
						continue;
					}
					item->hitResult = 2;
					break;
				}
				if (j == 100) {
					item->hitResult = 1;
				}
				unit = BattleAudienceDetectTargetPlayer();
				if (unit == NULL) {
					goto remove;
				}
				item->target = unit;
				part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
				BtlUnit_GetHitPos(unit, part, &hitPos.x, &hitPos.y, &hitPos.z);
				dx = hitPos.x - item->pos.x;
				dist = 30.0f + (f32)__fabs(dx) / 15.0f;
				item->vel.x = dx / dist;
				item->accel.x = 0.0f;
				item->vel.y = 7.0f;
				item->accel.y = 2.0f * ((hitPos.y - item->pos.y) - item->vel.y * dist) / (dist * dist);
				item->vel.z = (hitPos.z - item->pos.z) / dist;
				item->accel.z = 0.0f;
				item->timer = (s32)dist;
				item->state++;
			}
			else {
				offset = 0xb4 - item->timer;
				if (offset >= 5) {
					offset = 5;
				}
				item->pos = (Vec){ member->dispPos.x + kind->offsetX,
				                   member->dispPos.y + kind->offsetY + intplGetValue(0, offset, 5, -10.0f, 0.0f),
				                   member->dispPos.z + 1.0f };
			}
			break;
		case 2:
			if (!(wp->flags & 0x80)) {
				item->hitResult = 0;
			}
			if (item->timer == 0x14) {
				BattleAudience_ChangeStatus(item->memberId, 0);
			}
			if (item->timer == 0) {
				item->state++;
			}
			if (item->timer > 0) {
				item->pos.x += item->vel.x;
				item->pos.y += item->vel.y;
				item->pos.z += item->vel.z;
				item->vel.x += item->accel.x;
				item->vel.y += item->accel.y;
				item->vel.z += item->accel.z;
				if (item->mItemType == 0xef) {
					item->angleZ += 18.0f;
				}
				item->timer -= 1;
			}
			break;
		case 3:
			if (!(wp->flags & 0x80)) {
				item->hitResult = 0;
			}
			unit = item->target;
			BtlUnit_GetStatus(unit, STATUS_INVISIBLE, &turns, &strength);
			if (turns >= 1 || BtlUnit_CheckShadowGuard(unit)) {
				item->hitResult = 0;
			}
			if (item->flags & 4) {
				item->hitResult = 0;
			}
			if (item->mItemType >= 0x80 && item->mItemType < 0xec && pouchGetEmptyHaveItemCnt() <= 0) {
				item->hitResult = 0;
			}
			if (item->hitResult != 0) {
				audience->itemCombo += 1;
				if (audience->itemCombo > 3) {
					audience->itemCombo = 3;
				}
				if (item->hitResult == 2) {
					item->state = 4;
#ifdef MUST_MATCH
					goto case4_body;
#endif
				}
				else {
					if (item->mItemType >= 0x79 && item->mItemType < 0xec) {
						BattleAudience_SetPresentItemType(0);
					}
					else if (item->mItemType >= 0xec && item->mItemType < 0xf0) {
						BattleAudience_SetPresentItemType(1);
					}
					BattleAudience_SetPresentTargetUnitId(BtlUnit_GetUnitId(unit));
					switch (item->mItemType) {
					case 0xec:
						memcpy(&audience->itemWeapon, &weapon_data_akikan, 0xc0);
						audience->itemWeapon.hpDamageWork[0] += audience->itemCombo - 1;
						break;
					case 0xed:
						memcpy(&audience->itemWeapon, &weapon_data_brick, 0xc0);
						audience->itemWeapon.hpDamageWork[0] += audience->itemCombo - 1;
						break;
					case 0xee:
						memcpy(&audience->itemWeapon, &weapon_data_bone, 0xc0);
						audience->itemWeapon.hpDamageWork[0] += audience->itemCombo - 1;
						break;
					case 0xef:
						memcpy(&audience->itemWeapon, &weapon_data_hammer, 0xc0);
						audience->itemWeapon.hpDamageWork[0] += audience->itemCombo - 1;
						break;
					}
					BattleAudience_SetPresentItemNo(item->mItemType);
					switch (BattleAudience_GetPresentItemType()) {
					case 0:
						item->flags &= ~2;
						item->timer = 5;
						item->state = 7;
						break;
					case 1:
						BattleAudienceSoundStop(10);
						BattleAudienceSoundStop(11);
						if (item->vel.x < 0.0f) {
							item->vel.x *= -0.2f;
						}
						item->vel.y = 1.5f;
						item->vel.z = 0.0f;
						item->state = 4;
						break;
					}
					audience->itemCombo = 0;
				}
			}
			else {
				item->state = 4;
			}
			if (item->state != 4) {
				break;
			}
			/* fall through */
#ifdef MUST_MATCH
		case4_body:
#endif
		case 4:
			item->pos.x += item->vel.x;
			item->pos.y += item->vel.y;
			item->pos.z += item->vel.z;
			item->vel.x += item->accel.x;
			item->vel.y += item->accel.y;
			item->vel.z += item->accel.z;
			if (item->pos.y <= 0.0f) {
				BattleAudienceSoundStop(10);
				BattleAudienceSoundStop(11);
				item->state = 5;
				item->pos.y = 0.1f;
				item->vel.y = 1.0f;
				item->vel.z = 0.0f;
			}
			break;
		case 5:
			item->pos.x += item->vel.x;
			item->pos.y += item->vel.y;
			item->pos.z += item->vel.z;
			item->vel.x += item->accel.x;
			item->vel.y += item->accel.y;
			item->vel.z += item->accel.z;
			if (item->pos.y <= 0.0f) {
				item->timer = 0x3c;
				item->state = 6;
				item->pos.y = 0.0f;
			}
			break;
		case 6:
			if (item->timer <= 0) {
				goto remove;
			}
			if (item->timer < 0x1e) {
				if ((item->timer & 3) <= 1) {
					item->flags |= 2;
				}
				else {
					item->flags &= ~2;
				}
			}
			item->timer -= 1;
			break;
		case 7:
			item->timer -= 1;
			if (item->timer > 0) {
				break;
			}
			/* fall through */
		remove:
			BattleAudienceSoundStop(10);
			BattleAudienceSoundStop(11);
			if (item->memberId != -1) {
				BattleAudienceGetPtr(item->memberId);
				BattleAudience_ChangeStatus(item->memberId, 0);
			}
		deactivate:
			item->flags = 0;
			item->timer = 0;
			item->state = 0;
			if (item->memberId != -1) {
				member = BattleAudienceGetPtr(item->memberId);
				member->itemId = -1;
			}
			break;
		}
	}
}

// Computing the case-10 `dx` before `radius` loads star+0x20 first, matching retail's register order.
void BattleAudienceApSrcCtrlProcess(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	BattleAudienceStar* star;
	Vec pos;
	Vec pos2;
	s32 active;
	s32 inactive;
	s32 i;
	f32 radius;
	f32 dx;
	f32 start;
	f32 end;
	f32 zero;

	if (!(audience->flags & 2)) {
		return;
	}
	active = 0;
	for (i = 0; i < 100; i++) {
		if (audience->starpower[i].flags & 1) {
			active++;
		}
	}
	if (psndSFXChk(audience->apStarSfxId) != -1) {
		psndSFX_vol(audience->apStarSfxId,
		            (s32)(64.0f + 63.0f * (f32)active / 100.0f));
	}
	if (audience->apGauge > 0) {
		if (BattleAudienceApSrcEntry() == 1) {
			audience->apGauge = audience->apGauge - 1;
		}
		if (active == 0) {
			if (psndSFXChk(audience->apStarSfxId) == -1) {
				audience->apStarSfxId = psndSFXOn("SFX_BTL_SYS_AP_STAR1");
			}
		}
	}
	else {
		inactive = 0;
		for (i = 0; i < 100; i++) {
			if (audience->starpower[i].flags & 1) {
				break;
			}
			inactive++;
		}
		if (inactive == 100) {
			audience->flags &= ~2;
			psndSFXOff(audience->apStarSfxId);
			audience->apStarSfxId = 0;
			audience->apGauge = 0;
			BattleStatusWindowAPRecoveryOff();
			return;
		}
	}
	for (i = 0; i < 100; i++) {
		star = &audience->starpower[i];
		pouchGetPtr();
		if (!(star->flags & 1)) {
			continue;
		}
		switch (star->state) {
		case 0:
			star->state = 5;
			star->endPos = star->pos;
			star->startPos = star->endPos;
			star->endPos.x += 50.0f * (f32)rand() / 32767.0f - 25.0f;
			star->endPos.y += 25.0f;
			star->frame = 0;
			star->duration = 0x1E;
			// fall through
		case 5:
			star->frame = star->frame + 1;
			start = star->startPos.x;
			end = star->endPos.x;
			star->pos.x = intplGetValue(5, star->frame, star->duration, start, end);
			start = star->startPos.y;
			end = star->endPos.y;
			star->pos.y = intplGetValue(5, star->frame, star->duration, start, end);
			if (star->frame >= star->duration - 0x14) {
				star->scale.y = intplGetValue(4, star->frame - (star->duration - 0x14), 0xA,
				                                     0.9f, 0.8f);
			}
			if (star->frame < star->duration) {
				continue;
			}
			star->state = 0xA;
			star->frame = 0;
			star->duration = irand(0xA) + 0xF;
			star->scale = (Vec){ 0.9f, 0.9f, 0.9f };
			break;
		case 10:
			star->frame = star->frame + 1;
			if (star->frame < star->duration) {
				continue;
			}
			star->state = 0xF;
			star->frame = 0;
			star->duration = irand(0x14) + 0x14;
			star->startPos = star->pos;
			pos = statusGetApPos();
			star->endPos = pos;
			dx = star->startPos.x - star->endPos.x;
			radius = star->startPos.y - star->endPos.y;
			star->radius = (star->radiusStart = sqrt(dx * dx + radius * radius));
			star->angle = asin(radius / star->radius);
			star->angleStep = 0.05f +
			                        0.2f * (f32)rand() / 32767.0f;
			break;
		case 15:
			end = 0.0f;
			star->frame = star->frame + 1;
			start = star->radiusStart;
			star->radius = intplGetValue(4, star->frame, star->duration, start, end);
			zero = 0.0f;
			start = star->angle + star->angleStep;
			star->angle = start;
			if (star->angle < zero) {
				star->angle += 6.283184f;
			}
			star->pos.x = star->radius * (f32)cos(star->angle) + star->endPos.x;
			star->pos.y = star->radius * (f32)sin(star->angle) + star->endPos.y;
			if (star->frame >= star->duration) {
				star->state = 0x14;
				star->frame = 0;
				star->duration = 0xF;
				pos2 = statusGetApPos();
				star->pos = pos2;
			}
			break;
		case 20:
			start = 128.0f;
			end = 0.0f;
			star->frame = star->frame + 1;
			star->alpha = (u8)intplGetValue(4, star->frame, star->duration, start, end);
			if (star->frame >= star->duration) {
				star->flags = 0;
				pouchAddAP(1);
			}
			break;
		default:
			break;
		}
	}
}

// The second loop reuses `i` (its second definition becomes a compiler object, so the backend copies the
// shared zero into it: retail's `li r30,0 ... mr r28,r30`). Making the `star+0x20` store `volatile`
// keeps the `star+0x2c` copy's loads out of the two rand blocks.
BOOL BattleAudienceApSrcEntry(void) {
	BattleAudience* audience;
	BattleAudienceMember* member;
	CameraEntry* cam;
	BattleAudienceStar* star;
	s32 ids[200];
	s32 count;
	s32 i;

	audience = BattleAudienceBaseGetPtr();
	i = 0;
	star = audience->starpower;
	for (; i < 100; i++, star++) {
		if (!(star->flags & 1)) {
			break;
		}
	}
	if (i == 100) {
		return FALSE;
	}
	count = 0;
	i = 0;
	member = BattleAudienceGetPtr(0);
	for (; i < 200; i++) {
		if ((u8)BattleAudience_GetExist(i) == 1) {
			ids[count++] = i;
		}
	}
	if (count == 0) {
		return TRUE;
	}
	star->flags = 1;
	star->state = 0;
	star->frame = 0;
	star->scale = (Vec){ 0.75f, 0.75f, 0.75f };
	star->alpha = 0x80;
	member = BattleAudienceGetPtr(ids[irand(count)]);
	star->pos.x = member->position.x;
	star->pos.y = 20.0f + member->position.y;
	star->pos.z = 1.0f + member->position.z;
	cam = camGetPtr(CAMERA_3D);
	PSMTXMultVec(cam->view, &star->pos, &star->pos);
	PSMTX44MultVec(cam->projection, &star->pos, &star->pos);
	star->pos.x *= 304.0f;
	star->pos.y *= 240.0f;
	star->pos.z = 0.0f;
	star->randOffset.x = 40.0f * (f32)rand() / 32767.0f - 20.0f;
	star->randOffset.y = 10.0f * (f32)rand() / 32767.0f;
	star->randOffset.z = 0.0f;
	star->startPos = star->pos;
	star->endPos = (Vec){ -96.0f, 106.0f, 0.0f };
	return TRUE;
}

// The target colours the first cast's 0.0f constant f0 (window->count f1, cast temp f2, diff f3); MWCC colours
// ours f3 instead, shifting the first block and both step clamps up a register, which also turns the positive
// clamp's bge+b diamond into a plain bge. Loading the subtrahend first (`diff = window->count; diff =
// (f32)audience->mCurrentAudienceIntCount - diff;`) gets one more instruction to match than the plain one-liner.
// Writing the zero test constant-first (`0.0f != diff`) makes objdiff's first `fcmpu` operand order match
// (the constant is FRA) and is worth 95.87 -> 95.91.
#define BA_ZERO 0.0f
// The zero test is constant-first for the target's `(f0, diff)` operand order, the branches use
// literal 0.0f so the target's four separate `lfs` reloads reappear, and `step` is a function-scope
// local so the diff result takes f3.
void BattleAudienceWinCtrlProcess(void) {
	BattleAudience* audience;
	BattleAudienceWindow* window;
	f32 diff;
	f32 step;

	audience = BattleAudienceBaseGetPtr();
	window = BattleAudienceWinGetPtr();
	diff = (f32)audience->mCurrentAudienceIntCount - window->count;
	if (0.0f != diff) {
		if (diff > 0.0f) {
			step = 0.1f * diff;
			step = step < 0.1f ? 0.1f : step;
			window->count += step;
			if ((f32)audience->mCurrentAudienceIntCount - window->count < 0.0f) {
				window->count = (f32)audience->mCurrentAudienceIntCount;
			}
		}
		else {
			step = 0.1f * diff;
			if (step > -0.1f) {
				step = 0.1f;
			}
			else {
				step = -0.1f * diff;
			}
			window->count -= step;
			if ((f32)audience->mCurrentAudienceIntCount - window->count > 0.0f) {
				window->count = (f32)audience->mCurrentAudienceIntCount;
			}
		}
	}

	if (window->mActive) {
		window->slide += 1;
		if (window->slide > 0xF) {
			window->slide = 0xF;
		}
	}
	else {
		window->slide -= 1;
		if (window->slide < 0) {
			window->slide = 0;
		}
	}

	window->posX =
	    intplGetValue(4, window->slide, 0xF, -500.0f, -280.0f);
	{
		f32 a = -160.0f;
		f32 b = 0.0f;
		window->posY = a;
		window->field_0x10 = b;
	}
}
#undef BA_ZERO

void BattleAudienceGXInit(void) {
	BattleAudienceBaseGetPtr();
	GXSetCullMode(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xB, 1);
	GXSetVtxDesc(0xD, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
	GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 1, 1, 0, 2, 2);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
	GXSetNumTevStages(1);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xF, 0xA, 8, 0xF);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaIn(0, 7, 5, 4, 7);
	GXSetTevOrder(0, 0, 0, 4);
}

// Runs each active member's animation command script. Opcodes are negative words; the script
// base is 0x24, the current command 0x28, the wait timer 0x2C, and 0x34/0x40 are a 3-deep
// loop-counter stack and its depth (none of these have named struct fields yet).
void BattleAudienceAnimProcess(void) {
	BattleAudienceMember* member;
	s32 cmd;
	s32 target;
	s32 depth;
	s32 i;
	s32* anim;

	BattleAudienceBaseGetPtr();
	for (i = 0, member = BattleAudienceGetPtr(0); i < 200; i++, member++) {
		if (!(member->flags & 1)) {
			continue;
		}
		if (!(member->flags & 4)) {
			continue;
		}
		anim = member->animPos;
		if (member->animWait > 0) {
			member->animWait -= 1;
			continue;
		}
		switch (*anim) {
		case -2:
			member->texIndex = anim[1];
			member->animWait = anim[2];
			member->animPos += 3;
			break;
		case -3:
			member->animPos = member->animCur;
			break;
		case -4:
			member->animPos += 2;
			break;
		case -5:
			target = anim[1];
			member->animPos = member->animCur;
			while (!(member->animPos[0] == -4 && member->animPos[1] == target)) {
				member->animPos += 1;
			}
			break;
		case -6:
			member->animLoop[member->animDepth] = anim[1];
			member->animDepth += 1;
			member->animPos += 2;
			break;
		case -7:
			member->animLoop[member->animDepth - 1] -= 1;
			if (member->animLoop[member->animDepth - 1] <= 0) {
				member->animDepth -= 1;
				member->animPos += 1;
			} else {
				depth = 1;
				do {
					member->animPos -= 1;
					cmd = *member->animPos;
					if (cmd == -6) {
						depth--;
					}
					if (cmd == -7) {
						depth++;
					}
				} while (depth > 0);
				member->animPos += 2;
			}
			break;
		case -8:
			member->animWait = anim[1];
			member->animPos += 2;
			break;
		case -1:
			break;
		}
	}
}

// Same idea as BattleAudienceDispApSrc_color below: routing the colour through an inline puts the
// named local at r1+0x8 and the hidden by-value copy at r1+0xc, matching the original; a plain
// assignment puts them the other way round.
static inline GXColor BattleAudienceDispAudience_color(u8 r, u8 g, u8 b, u8 a) {
	GXColor color = (GXColor){ r, g, b, a };
	return color;
}

void BattleAudienceDispAudience(CameraId camId, void* param) {
	BattleAudience* audience;
	CameraEntry* cam;
	s32 i;
	BattleAudienceMember* member;
	s32 flip;
	s32 texBase;
	s32 front;
	s32 j;
	GXTexObj texObj;
	Mtx mtxRot;
	Mtx mtxScale;
	Mtx mtxDraw;
	BattleAudienceTexCoords texCoords;
	f32 scaleX;
	f32 cx;
	f32 scaleZ;
	f32 sy;
	f32 cy;
	f32 cz;
	f32 sz;
	f32 neg20;
	f32 rx;
	f32 rz;
	f32 scaleY;
	f32 offsetX;
	f32 offsetY;
	f32 angle;
	f32 ry;
	f32 offsetZ;
	f32 sx;
	f32 w;
	u8 cr;
	u8 cg;
	u8 cb;
	u8 ca;
	GXColor color;

	audience = BattleAudienceBaseGetPtr();
	cam = camGetPtr(CAMERA_3D);

	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_VTX, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
	GXSetNumTevStages(1);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ONE, GX_CC_TEXC, GX_CC_RASC);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);

	i = 0;
	member = BattleAudienceGetPtr(0);
	w = 20.0f;
	neg20 = -w;

	for (; i < 0xC8; i++, member++) {
		if (dispGetCurWork()->renderMode != 2) {
			if (member->colorA != 0xFF) {
				continue;
			}
		}
		else if (member->colorA == 0xFF) {
			continue;
		}
		if (!(member->flags & 1)) {
			continue;
		}
		if (!(member->flags & 4)) {
			continue;
		}

		if (member->type <= AUDIENCE_PIRANHA_PLANT) {
			TEXGetGXTexObjFromPalette(audience->normalTex->data[0], &texObj, member->texIndex);
			GXLoadTexObj(&texObj, GX_TEXMAP0);
			{
				f32 w = (f32)GXGetTexObjWidth(&texObj);
				f32 k = audience_kind[member->type].scaleX;
				w = k * w / 64.0f;
				member->scale.x = member->baseSize.x * w;
			}
			{
				f32 h = (f32)GXGetTexObjHeight(&texObj);
				f32 k = audience_kind[member->type].scaleY;
				h = k * h / 64.0f;
				member->scale.y = member->baseSize.y * h;
			}
		}
		else {
			for (j = 0; j < 2; j++) {
				if (member->type == audience->guestType[j]) {
					TEXGetGXTexObjFromPalette(audience->guestTex[j]->data[0], &texObj,
					                          member->texIndex);
					GXLoadTexObj(&texObj, GX_TEXMAP0);
					{
						f32 w = (f32)GXGetTexObjWidth(&texObj);
						f32 k = audience_kind[member->type].scaleX;
						w = k * w / 64.0f;
						member->scale.x = member->baseSize.x * w;
					}
					{
						f32 h = (f32)GXGetTexObjHeight(&texObj);
						f32 k = audience_kind[member->type].scaleY;
						h = k * h / 64.0f;
						member->scale.y = member->baseSize.y * h;
					}
					break;
				}
			}
		}

		scaleX = member->scale.x;
		offsetX = member->mRotationOffset.x * scaleX;
		scaleY = member->scale.y;
		offsetY = member->mRotationOffset.y * scaleY;
		scaleZ = member->scale.z;
		offsetZ = member->mRotationOffset.z * scaleZ;
		PSMTXScale(mtxScale, scaleX, scaleY, scaleZ);

		{
			f32 d2r = 0.017453289f;
			rx = member->mRotation.x * d2r;
			ry = member->mRotation.y * d2r;
			rz = member->mRotation.z * d2r;
		}
		sx = (f32)sin(rx);
		cx = (f32)cos(rx);
		sy = (f32)sin(ry);
		cy = (f32)cos(ry);
		sz = (f32)sin(rz);
		cz = (f32)cos(rz);

		{
			f32 sxsy = sx * sy;
			f32 sxcy = sx * cy;
			mtxRot[0][0] = cy * cz - sxsy * sz;
			mtxRot[0][1] = -cy * sz - sxsy * cz;
			mtxRot[0][2] = -cx * sy;
			mtxRot[0][3] = 0.0f;
			mtxRot[1][0] = cx * sz;
			mtxRot[1][1] = cx * cz;
			mtxRot[1][2] = -sx;
			mtxRot[1][3] = 0.0f;
			mtxRot[2][0] = sy * cz + sxcy * sz;
			mtxRot[2][1] = -sy * sz + sxcy * cz;
			mtxRot[2][2] = cx * cy;
			mtxRot[2][3] = 0.0f;
		}

		member->normal = (Vec){ 0.0f, 0.0f, -1.0f };
		PSMTXMultVec(mtxRot, &member->normal, &member->normal);

		mtxScale[0][3] += offsetX;
		mtxScale[1][3] += offsetY;
		mtxScale[2][3] += offsetZ;
		PSMTXConcat(mtxRot, mtxScale, mtxScale);

		mtxScale[0][3] = mtxScale[0][3] + (member->jumpOffset.x + (-offsetX + member->position.x));
		mtxScale[2][3] = mtxScale[2][3] + (member->jumpOffset.z + (-offsetZ + member->position.z));
		if (member->type != AUDIENCE_BOO) {
			mtxScale[1][3] = mtxScale[1][3] + (member->jumpOffset.y + (-offsetY + member->position.y));
		}
		else {
			mtxScale[1][3] = mtxScale[1][3] + (member->bobOffsetY +
			                                      (member->jumpOffset.y + (-offsetY + member->position.y)));
		}

		PSMTXConcat(cam->view, mtxScale, mtxDraw);
		GXLoadPosMtxImm(mtxDraw, 0);

		texCoords = (BattleAudienceTexCoords){ {
			0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
			0.0f, 1.075f, 0.0f, 0.075f, 1.0f, 0.075f, 1.0f, 1.075f,
			0.0f, 0.075f, 0.0f, 1.075f, 1.0f, 1.075f, 0.0f, 1.075f,
		} };

		texBase = 0;
		front = BattleAudience_GetFront(i);
		cr = member->colorR;
		cg = member->colorG;
		cb = member->colorB;
		ca = member->colorA;
		if (member->animCur == member->animTable[9]) {
			texBase = 8;
		}

		if (member->yaw >= 360.0f) {
			member->yaw = member->yaw -
			                     360.0f * (f32)(s32)(member->yaw / 360.0f);
		}
		if (member->yaw < 0.0f) {
			angle = (f32)(1 - (s32)(member->yaw / 360.0f));
			member->yaw = member->yaw + 360.0f * angle;
		}
		angle = member->yaw;
		if (angle > 0.0f && angle <= 180.0f) {
			flip = 1;
		}
		if (angle == 0.0f || (angle > 180.0f && angle < 360.0f)) {
			flip = 0;
		}
		if ((u8)front == 1) {
			if (member->animCur != member->animTable[9] &&
			    member->animCur != member->animTable[8] &&
			    member->animCur != member->animTable[7]) {
				flip ^= 1;
			}
		}
		if (flip == 1) {
			texBase += 4;
		}

		color = BattleAudienceDispAudience_color(cr, cg, cb, ca);
		GXSetChanMatColor(GX_COLOR0A0, color);

		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(neg20 + member->drawOffset.x, 40.0f + member->drawOffset.y,
		               member->drawOffset.z);
		GXTexCoord2f32(texCoords.v[texBase * 2], texCoords.v[texBase * 2 + 1]);
		GXPosition3f32(w + member->drawOffset.x, 40.0f + member->drawOffset.y,
		               member->drawOffset.z);
		GXTexCoord2f32(texCoords.v[(texBase + 1) * 2], texCoords.v[(texBase + 1) * 2 + 1]);
		GXPosition3f32(w, 0.0f, 0.0f);
		GXTexCoord2f32(texCoords.v[(texBase + 2) * 2], texCoords.v[(texBase + 2) * 2 + 1]);
		GXPosition3f32(neg20, 0.0f, 0.0f);
		GXTexCoord2f32(texCoords.v[(texBase + 3) * 2], texCoords.v[(texBase + 3) * 2 + 1]);
	}
}

void BattleAudienceDispItem(CameraId camId, void* param) {
	BattleAudienceItem* item;
	Mtx mtx;
	Mtx rotMtx;
	s32 i;
	u16 iconId;

	BattleAudienceGXInit();
	i = 0;
	item = BattleAudienceItemGetPtr(0);
	for (; i < 100; i++, item++) {
		if (!(item->flags & 1)) {
			continue;
		}
		if (!(item->flags & 2)) {
			continue;
		}
		iconId = itemDataTable[item->mItemType].icon_id;
		PSMTXTrans(mtx, 0.0f, -20.0f, 0.0f);
		PSMTXRotRad(rotMtx, 'z', 0.017453292f * item->angleZ);
		PSMTXConcat(rotMtx, mtx, mtx);
		PSMTXTransApply(mtx, mtx, 0.0f, 20.0f, 0.0f);
		PSMTXScaleApply(mtx, mtx, item->scale, item->scale, 1.0f);
		PSMTXTransApply(mtx, mtx, item->pos.x, item->pos.y,
		                item->pos.z);
		iconDispGx2(mtx, 0, iconId);
	}
}

// The inline's returned colour lands where the original keeps it: the named local at r1+0x8,
// the hidden by-value copy at r1+0xc. A plain assignment puts them the other way round.
static inline GXColor BattleAudienceDispApSrc_color(u8 alpha) {
	GXColor color = *(GXColor*)&dat_8041e608;
	color.a = alpha;
	return color;
}

void BattleAudienceDispApSrc(CameraId camId, void* param) {
	BattleAudience* audience;
	BattleAudienceStar* star;
	Mtx rot;
	Mtx scale;
	Mtx trans;
	Mtx mtx;
	GXColor color;
	s32 i;

	audience = BattleAudienceBaseGetPtr();
	camGetPtr(CAMERA_2D);
	BattleAudienceGXInit();
	GXSetBlendMode(1, 4, 5, 0);
	GXSetZCompLoc(1);
	GXSetAlphaCompare(7, 0, 0, 7, 0);
	GXSetZMode(1, 3, 0);
	star = audience->starpower;
	for (i = 0; i < 100; i++) {
		if (star->flags & 1) {
			PSMTXScale(scale, star->scale.x, star->scale.y, star->scale.z);
			PSMTXRotRad(rot, 'z', 0.0f);
			PSMTXTrans(trans, star->pos.x, star->pos.y, 0.0f);
			PSMTXConcat(scale, rot, mtx);
			PSMTXConcat(trans, mtx, mtx);
			color = BattleAudienceDispApSrc_color(star->alpha);
			iconDispGxCol(mtx, 0x10, 0x1B2, color);
		}
		star++;
	}
}

void BattleAudienceDispWin(CameraId camId, void* param) {
	BattleAudienceWindow* win;
	GXColor color;
	GXColor numberColor;
	Vec pos;
	Mtx mtx;

	BattleAudienceBaseGetPtr();
	win = BattleAudienceWinGetPtr();
	color = (GXColor){ 0xFF, 0xFF, 0xFF, 0xCF };
	windowDispGX_Waku_col(win->posX, win->posY, 90.0f, 40.0f, 10.0f, 0, color);

	pos = (Vec){ 0.0f, 0.0f, 0.0f };
	pos.x = 17.0f + win->posX;
	pos.y = win->posY - 40.0f;
	iconDispGx(0.8f, &pos, 0x10, 0x209);

	PSMTXTrans(mtx, 75.0f + win->posX,
	           win->posY - 40.0f, 0.0f);
	numberColor = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
	iconNumberDispGx(mtx, (s32)win->count, FALSE, numberColor);
}

// `audience_kind[type]` (no `(u8)` cast) is required so MWCC uses the parameter directly, and
// `m * 0x19 - 0xeb + r` (not `m * 0x19 + r - 0xeb`) so `r` lands in r6. In the second branch the
// draw of `rand()` must be written inline in the expression so the fadds keeps the original order.
BOOL BattleAudience_Entry(s32 index, AudienceMemberType type, u8 status) {
	BattleAudienceMember* member;
	f32 f;
	s32 r;
	s32 m;
	BattleAudienceBaseGetPtr();
	BattleStageGetPtr();
	member = BattleAudienceGetPtr(index);
	if ((audience_kind[type].flags & AUDIENCE_KIND_TWO_SEATS) == 0) {
		if (member->flags & 1) {
			return FALSE;
		}
		if (member->flags & 0x100) {
			return FALSE;
		}
		member->flags = 5;
		r = rand() * 10 / 0x7fff;
		m = index % 20;
		f = (f32)(m * 0x19 - 0xeb + r);
		member->position.x = f;
		member->mHomePosition.x = f;
		BattleAudience_Entry_Sub(index, type, status);
	}
	else {
		if (index % 2 == 1) {
			return FALSE;
		}
		if (member->flags & 1) {
			return FALSE;
		}
		if (member->flags & 0x100) {
			return FALSE;
		}
		member->flags = 0x85;
		m = index % 20;
		f = 12.5f + (f32)(m * 0x19 - 0xe6) + (f32)(rand() * 10 / 0x7fff) - 5.0f;
		member->position.x = f;
		member->mHomePosition.x = f;
		BattleAudience_Entry_Sub(index, type, status);
		(&member[1])->flags = 0x101;
	}
	return TRUE;
}

void BattleAudience_Entry_Sub(s32 index, s32 type, u8 status) {
	BattleAudienceMember* member;
	BattleStage* stage;
	s32 col;
	s32 row;
	f32 y;
	f32 z;

	BattleAudienceBaseGetPtr();
	stage = BattleStageGetPtr();
	member = BattleAudienceGetPtr(index);

	member->step = 0;
	member->counter = 0;
	member->status = status;
	member->type = type;
	member->itemId = -1;
	member->animTable = audience_kind[type].animTable;
	member->motionTimer = 0;
	BattleAudience_SetAnim(index, 1, 1);

	col = index / 20;
	row = index % 20;
	member->animDepth = 0;
	member->colorB = 0;
	member->colorG = 0;
	member->colorR = 0;
	member->colorA = 0xff;

	y = stage->audienceAreaSize.y;
	z = 0.1f * (f32)row + (f32)(col * 0x1e + 0x73);
	member->position.y = y;
	member->mHomePosition.y = y;
	member->position.z = z;
	member->mHomePosition.z = z;

	if (member->type == AUDIENCE_PIRANHA_PLANT) {
		f32 off = 0.2f;
		member->mHomePosition.z += off;
		member->position.z += off;
	}
	if (member->status == 0xb) {
		member->position.y = -2000.0f;
	}

	member->accel = (Vec){ 0.0f, 0.0f, 0.0f };
	member->velocity = member->accel;
	member->normal = (Vec){ 0.0f, 0.0f, -1.0f };
	member->mRotation = (Vec){ 0.0f, 0.0f, 0.0f };
	member->mRotationOffset = (Vec){ 0.0f, 0.0f, 0.0f };
	member->scale = (Vec){ 1.0f, 1.0f, 1.0f };
	member->baseSize = (Vec){ 1.0f, 1.0f, 1.0f };
	member->drawOffset = (Vec){ 0.0f, 0.0f, 0.0f };
	member->yaw = 270.0f;

	if ((index >= 4 && index <= 0xf) ||
	    (index >= 0x18 && index <= 0x23) ||
	    (index >= 0x2c && index <= 0x37)) {
		member->flags |= 8;
	}
}

void BattleAudience_Delete(s32 index) {
	BattleAudienceMember* member;
	BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(index);
	if (member->flags & 0x80) {
		member[1].flags = 0;
	}
	member->flags = 0;
}

void BattleAudience_SetAnim(s32 id, s32 animId, BOOL force) {
	BattleAudienceMember* member;
	s32* anim;
	s32 i;
	BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(id);
	if (!(member->flags & 1)) {
		return;
	}
	if (member->flags & 0x100) {
		return;
	}
	anim = member->animTable[animId];
	if (member->animCur != NULL && member->animCur == anim && !force) {
		return;
	}
	member->animCur = anim;
	member->animPos = anim;
	member->animWait = 0;
	for (i = 0; i < 3; i++) {
		member->animLoop[i] = 0;
	}
	member->animDepth = 0;
}

BOOL BattleAudience_GetAnimEnd(s32 index) {
	BattleAudienceMember* member = BattleAudienceGetPtr(index);
	if (*member->animPos != -1) {
		return FALSE;
	}
	return member->animWait <= 0;
}

void BattleAudience_GetPosition(s32 index, f32* x, f32* y, f32* z) {
	BattleAudienceMember* member;

	member = BattleAudienceGetPtr(index);
	*x = member->position.x;
	*y = member->position.y;
	*z = member->position.z;
}

void BattleAudience_GetHomePosition(s32 index, f32* x, f32* y, f32* z) {
	BattleAudienceMember* member;

	member = BattleAudienceGetPtr(index);
	*x = member->mHomePosition.x;
	*y = member->mHomePosition.y;
	*z = member->mHomePosition.z;
}

void BattleAudience_GetRotate(s32 index, f32* x, f32* y, f32* z) {
	BattleAudienceMember* member;

	member = BattleAudienceGetPtr(index);
	*x = member->mRotation.x;
	*y = member->mRotation.y;
	*z = member->mRotation.z;
}

void BattleAudience_SetPosition(s32 index, f32 x, f32 y, f32 z) {
	BattleAudienceMember* member;

	member = BattleAudienceGetPtr(index);
	member->position.x = x;
	member->position.y = y;
	member->position.z = z;
}

void BattleAudience_SetRotate(s32 index, f32 x, f32 y, f32 z) {
	BattleAudienceMember* member;

	member = BattleAudienceGetPtr(index);
	member->mRotation.x = x;
	member->mRotation.y = y;
	member->mRotation.z = z;
}

void BattleAudience_SetRotateOffset(s32 index, f32 x, f32 y, f32 z) {
	BattleAudienceMember* member;

	member = BattleAudienceGetPtr(index);
	member->mRotationOffset.x = x;
	member->mRotationOffset.y = y;
	member->mRotationOffset.z = z;
}

void BattleAudience_GetItemOn(s32* memberId, f32* x, f32* y, f32* z, ItemType* itemType) {
	BattleAudience* audience;
	BattleAudienceMember* member;
	BattleAudienceItem* item;
	int i;

	audience = BattleAudienceBaseGetPtr();
	i = 0;
	member = BattleAudienceGetPtr(0);
	for (; i < 200; i++, member++) {
		//TODO: invert this to multiple break; checks?
		if ((member->flags & 1) && !(member->flags & 0x100) && !(member->flags & 2)) {
			if (member->itemId != -1) {
				item = BattleAudienceItemGetPtr(member->itemId);
				if (item->state <= 4 || item->state == 7) {
					if (memberId) {
						audience->mItemOnMemberId = i;
						*memberId = i;
					}
					audience->mItemOnMemberPos.x = member->position.x;
					audience->mItemOnMemberPos.y = member->position.y;
					audience->mItemOnMemberPos.z = member->position.z + 3.0f;

					if (x) {
						*x = audience->mItemOnMemberPos.x;
					}
					if (y) {
						*y = audience->mItemOnMemberPos.y;
					}
					if (z) {
						*z = audience->mItemOnMemberPos.z;
					}

					if (itemType) {
						if (member->itemId < 0) {
							*itemType = ITEM_NULL;
						}
						else {
							*itemType = audience->items[member->itemId].mItemType;
						}
					}
					//has to pass the 4 checks up top, otherwise we keep looking at all 200 members
					return;
				}
			}
		}
	}
	*memberId = -1;
}

//might be inlined, 1 above has additional assignments
void BattleAudience_GetItemOn2(s32* memberId, f32* x, f32* y, f32* z, ItemType* itemType) {
	BattleAudience* audience;
	BattleAudienceMember* member;

	audience = BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(audience->mItemOnMemberId);
	if (memberId) {
		*memberId = audience->mItemOnMemberId;
	}

	if (x) {
		*x = audience->mItemOnMemberPos.x;
	}
	if (y) {
		*y = audience->mItemOnMemberPos.y;
	}
	if (z) {
		*z = audience->mItemOnMemberPos.z;
	}

	if (itemType) {
		if (member->itemId < 0) {
			*itemType = ITEM_NULL;
		}
		else {
			*itemType = audience->items[member->itemId].mItemType;
		}
	}
}

void BattleAudience_SetTarget(s32 index) {
	BattleAudienceMember* member = BattleAudienceGetPtr(index);
	s32 i;
	member->flags |= 0x20;
	for (i = 0; i < 200; i++) {
		if (i == index) {
			continue;
		}
		if ((u8)BattleAudience_GetExist(i)) {
			BattleAudienceMember* m = BattleAudienceGetPtr(i);
			if (m->status == 7) {
				BattleAudienceItemGetPtr(m->itemId)->flags = 0;
				m->itemId = -1;
				BattleAudience_ChangeStatus(i, 0);
			}
		}
	}
}

void BattleAudience_Attack(s32 index) {
	BattleAudienceGetPtr(index)->flags |= 0x40;
}

BOOL BattleAudience_GetFront(s32 index) {
	BattleAudienceMember* member;
	CameraEntry* cam;
	f32 dx, dy, dz;

	BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(index);
	cam = camGetPtr(CAMERA_3D);
	dx = cam->cameraAt.x - cam->cameraPos.x;
	dy = cam->cameraAt.y - cam->cameraPos.y;
	dz = cam->cameraAt.z - cam->cameraPos.z;
	return !(dx * member->normal.x +
	         dy * member->normal.y +
	         dz * member->normal.z >=
	         0.0f);
}

void BattleAudience_ChangeStatus(s32 index, s32 status) {
	BattleAudienceMember* member;
	s32 zero = 0;
	f32 zeroF;

	BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(index);
	if (member->flags & 1) {
		if (member->flags & 0x100) {
			if (index >= 1) {
				BattleAudience_ChangeStatus(index - 1, status);
			}
		} else {
			member->colorB = zero;
			// Match-only statement order: the original loads the zero float between these two stores.
			zeroF = 0.0f;
			member->colorG = zero;
			member->colorR = zero;
			member->colorA = 0xFF;
			member->mRotation = (Vec){ 0.0f, 0.0f, 0.0f };
			member->drawOffset = (Vec){ 0.0f, 0.0f, 0.0f };
			member->baseSize = (Vec){ 1.0f, 1.0f, 1.0f };
			member->velocity = (Vec){ 0.0f, 0.0f, 0.0f };
			member->motionTimer = zero;
			member->step = zero;
			member->counter = zero;
			member->enterLength = zeroF;
			member->enterSpeed = zeroF;
			member->repeatCount = zeroF; // clears the whole 0xC union (enterDist, intrudeDir)
			member->status = status;
		}
	}
}

BOOL BattleAudience_GetExist(u32 id) {
	BattleAudienceMember* member;
	BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(id);
	if (!(member->flags & 1)) {
		return FALSE;
	}
	if (member->flags & 0x100) {
		return FALSE;
	}
	return !(member->flags & 2);
}

BOOL BattleAudience_GetSysCtrl(s32 index) {
	BattleAudienceMember* member;
	BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(index);
	if ((u8)BattleAudience_GetExist(index) == 0) {
		return FALSE;
	}
	return !(member->flags & 0x10);
}

u8 BattleAudience_GetWaiting(s32 index) {
	BattleAudienceMember* member;
	BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(index);
	if ((u8)BattleAudience_GetSysCtrl(index) == 0) {
		return FALSE;
	}
	if (member->itemId != -1) {
		return FALSE;
	}
	return member->status <= 3;
}

BOOL BattleAudience_GetEscapeChangeOK(s32 index) {
	BattleAudience* audience;
	BattleAudienceMember* member;

	audience = BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(index);
	if (audience->flags & 0x4000) {
		return FALSE;
	}
	if (!(u8)BattleAudience_GetSysCtrl(index)) {
		return FALSE;
	}
	if (member->flags & 0x200) {
		return FALSE;
	}
	if (member->status == 13) {
		return FALSE;
	}
	if (member->status == 17) {
		return FALSE;
	}
	if (member->status == 18) {
		return FALSE;
	}
	if (member->status == 14) {
		return FALSE;
	}
	if (member->status == 15) {
		return FALSE;
	}
	if (member->status == 21) {
		return FALSE;
	}
	if (member->status == 22) {
		return FALSE;
	}
	return member->status != 12;
}

s32 BattleAudience_GetAudienceNoFromOffset(s32 index, s32 column, s32 row) {
	s32 audienceNo;

	BattleAudience* audience = BattleAudienceBaseGetPtr();
	BattleAudienceMember* member = BattleAudienceGetPtr(index);
	if (index % 20 + column < 0 || index % 20 + column >= 20) {
		return -1;
	}

	audienceNo = column + (index - (row * 20));
	if (audienceNo < 0 || audienceNo >= 200) {
		return -1;
	}
	return audienceNo;
}

s32 BattleAudience_HaitiRandForFallObject(void) {
	s32 r = irand(3);
	s32 ret = 10;
	u32 unused = irand(12);

	switch (r) {
	case 0:
		ret = irand(12) + 4;
		break;
	case 1:
		ret = irand(10) + 5;
		break;
	case 2:
		ret = irand(8) + 6;
		break;
	}
	return ret + r * 20;
}

s32 BattleAudience_NoUsedHaitiRand(void) {
	s32 stack[200];
	s32* haiti;
	s32 memberId;
	s32 count;
	s32 i;
	s32 j;
	s32 start;

	BattleAudienceBaseGetPtr();
	haiti = haiti_data[pouchGetPtr()->rank];
	count = 0;
	for (i = 0; i < 10; i++) {
		start = (20 - haiti[i]) / 2;
		j = 0;
		while (j < haiti[i]) {
			memberId = start + i * 20;
			if (!(BattleAudienceGetPtr(memberId)->flags & 1) &&
			    !(BattleAudienceGetPtr(memberId)->flags & 0x100)) {
				stack[count] = memberId;
				count++;
			}
			j++;
			start++;
		}
	}
	if (count >= 1) {
		return stack[irand(count)];
	}
	return -1;
}

const s32 dat_802f9884[] = { 10000, 1000, 100, 10, 5, 2, 1, 1, 1, 1 };
const s32 dat_802f98ac[] = { 1, 1, 2, 2, 4, 4, 8, 8, 16, 16, 16, 16, 8, 8, 4, 4, 2, 2, 1, 1 };

s32 BattleAudience_NoUsedFCHaitiRand(void) {
	s32 rate[10];
	s32 offset[20];
	s32 ids[200];
	s32 sums[200];
	PouchData* pouch;
	s32* haiti;
	s32 count;
	s32 total;
	s32 i, j;
	s32 start;
	s32 memberId;
	s32 pouchRank;

	BattleAudienceBaseGetPtr();
	pouch = pouchGetPtr();
	// A named rank (read before the const-table copies) reproduces the original's
	// `lha` placement and frees r3 for the copy loops; `pouch->rank` inline sinks.
	pouchRank = pouch->rank;
	count = 0;
	total = 0;
	{
		*(BattleAudienceHaitiRateTable*)rate = *(const BattleAudienceHaitiRateTable*)dat_802f9884;
	}
	{
		*(BattleAudienceHaitiOffsetTable*)offset = *(const BattleAudienceHaitiOffsetTable*)dat_802f98ac;
	}
	haiti = haiti_data[pouchRank];
	for (i = 0; i < 10; i++) {
		start = (20 - haiti[i]) / 2;
		j = 0;
		while (j < haiti[i]) {
			memberId = start + i * 20;
			if (!(BattleAudienceGetPtr(memberId)->flags & 1) &&
			    !(BattleAudienceGetPtr(memberId)->flags & 0x100)) {
				s32 w;
				w = rate[i] * offset[start] + 1;
				ids[count] = memberId;
				if (count != 0) {
					sums[count] = sums[count - 1] + w;
				} else {
					sums[count] = w;
				}
				total += w;
				count++;
			}
			j++;
			start++;
		}
	}
	{
		s32 pick = (s32)((f32)total * (f32)rand() / 32767.0f);

		for (i = 0; i < count; i++) {
			if (pick < sums[i]) {
				return ids[i];
			}
		}
	}
	return -1;
}

s32 BattleAudience_GetAudienceNum(void) {
	return BattleAudienceBaseGetPtr()->mCurrentAudienceIntCount;
}

s32 BattleAudience_GetPPAudienceNum(void) {
	s32 left = BattleAudience_GetPPAudienceNum_L();
	s32 right = BattleAudience_GetPPAudienceNum_R();
	return right + left;
}

s32 BattleAudience_GetPPAudienceNumKind(s32 type) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 i, count = 0;

	for (i = 0; i < 200; i++) {
		if (BattleAudience_GetPPAudienceNum_Sub(i)) {
			if (BattleAudienceGetPtr(i)->type == type) {
				count++;
			}
		}
	}
	return count;
}

s32 BattleAudience_GetPPAudienceNum_R(void) {
	return BattleAudience_GetPPAudienceNum_RL_Sub(10);
}

s32 BattleAudience_GetPPAudienceNum_L(void) {
	return BattleAudience_GetPPAudienceNum_RL_Sub(0);
}

s32 BattleAudience_GetPPAudienceNum_RL_Sub(s32 offset) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 akk;
	s32 i, j;
	s32 count = 0;

	count = 0;
	for (i = 0, akk = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {
// This operand order makes MWCC emit `akk+offset` then `j+...`, as the original does.
			if (BattleAudience_GetPPAudienceNum_Sub(j + akk + offset) == 1) {
				count++;
			}
		}
		akk += 20;
	}
	return count;
}

BOOL BattleAudience_GetPPAudienceNum_Sub(s32 id) {
	BattleAudienceMember* member = BattleAudienceGetPtr(id);

	if (!(u8)BattleAudience_GetExist(id)) {
		return FALSE;
	}

	if (13 <= member->status && member->status <= 13) {
		return FALSE;
	}

	if (member->status == 17) {
		return FALSE;
	}

	return member->status != 12;
}

#pragma opt_lifetimes off // needed: keep the loop accumulator in f31 and the i2f double in f30 (see below)
// Levers that matched this function: (a) writing the two `> 0.0f`
// comparisons and the `crowdKindSum = 0.0f` reset as float literals keeps each
// a separate load instead of being CSE'd; (b) the declaration order below (crowdKindSum first, then
// partyMultiplier) puts the multiplier count in f31 to match the original. (c) `opt_lifetimes off`
// resolves the loop's FPR tie: with lifetimes on MWCC kept the pooled i2f double in f31 and the
// accumulator in f30; with it off the accumulator keeps f31 and the double takes f30 like the
// original, which also fixes the FMUL and `add` operand orders that followed from that tie. (d) the slot
// factor read straight off the `BattleBreakSlotGetPtr()` call inside the product (no `slot` local) gives
// the original's final `fmuls f0, f0, f2` operand order.
void BattleAudience_ApRecoveryBuild(StarPowerInfo* info) {
	BattleWork* wp = _battleWorkPointer;
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	BattleUnit* mario;
	BattleUnit* party;
	s32 i;
	f32 crowdKindSum;
	f32 partyMultiplier;
	f32 sqrtWeight;
	f32 spAcMultiplier;
	f32 stylishMultiplier;
	f32 marioMultiplier;
	f32 rate = 1.0f;

	if (audience->impendingBonuses == NULL) {
		audience->impendingBonuses = BattleAlloc(0xC);
	}
	memcpy(audience->impendingBonuses, info, 0xC);
	mario = BattleGetMarioPtr(wp);
	party = BattleGetPartyPtr(wp);
	crowdKindSum = 0.0f;
	for (i = 0; i < 13; i++) {
		crowdKindSum += audience_kind[i].apWeight * (f32)BattleAudience_GetPPAudienceNumKind(i);
	}
	sqrtWeight = (f32)sqrt(crowdKindSum);
	spAcMultiplier = info->mSpAcSuccessMultiplier;
	stylishMultiplier = (f32)info->mStylishCommandMultiplier;
	crowdKindSum = 0.0f;
	if (spAcMultiplier > 0.0f) {
		crowdKindSum += 1.0f;
	}
	if (stylishMultiplier > 0.0f) {
		crowdKindSum += 1.0f;
	}
	if (mario->mStatusFlags & 0x20000000) {
		marioMultiplier = 3.0f;
	}
	else if (mario->mStatusFlags & 0x10000000) {
		marioMultiplier = 2.0f;
	}
	else {
		marioMultiplier = 1.0f;
	}
	if (party != NULL) {
		if (party->mStatusFlags & 0x20000000) {
			partyMultiplier = 2.0f;
		}
		else if (party->mStatusFlags & 0x10000000) {
			partyMultiplier = 1.5f;
		}
		else {
			partyMultiplier = 1.0f;
		}
	}
	else {
		partyMultiplier = 1.0f;
	}
	if (BattleBreakSlot_GetBreakTurn() >= 1) {
		audience->apGauge += (s32)(rate * sqrtWeight * (spAcMultiplier + stylishMultiplier) *
		                           crowdKindSum * marioMultiplier * partyMultiplier *
		                           BattleBreakSlotGetPtr()->apMultiplier);
	}
	else {
		audience->apGauge += (s32)(rate * sqrtWeight * (spAcMultiplier + stylishMultiplier) *
		                                            crowdKindSum * marioMultiplier * partyMultiplier);
	}
	if (irand(100) < (u8)info->mBingoSlotChance) {
		BattleBreakSlot_PointInc();
	}
}
#pragma opt_lifetimes reset

void BattleAudience_WinSetActive(u8 active) {
	BattleAudienceWinGetPtr()->mActive = active;
}

void BattleAudienceNumToTarget(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 target;
	if (!(audience->flags & 0x40000)) {
		target = (s32)(audience->mTargetAudienceCount + audience->mAddedAudienceCount);
		if (target < 0) {
			target = 0;
		}
		if (target > audience->mMaxAudience) {
			target = audience->mMaxAudience;
		}
		BattleAudienceAddAudienceNum(target - BattleAudienceNumToTargetSub());
	}
}

s32 BattleAudienceNumToTargetSub(void) {
	s32 i;
	s32 count;

	BattleAudienceBaseGetPtr();
	count = 0;
	for (i = 0; i < 200; i++) {
		if ((u8)BattleAudience_GetExist(i) == 1) {
			if (BattleAudienceGetPtr(i)->status != 12) {
				count++;
			}
		}
	}
	return count;
}

void BattleAudienceAddTargetNum(f32 value, f32 delta) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();

	audience->mAddedAudienceCount += delta;
	if (audience->mAddedAudienceCount > 0.0f && value < 0.0f) {
		audience->mAddedAudienceCount += value;
		if (audience->mAddedAudienceCount < 0.0f) {
			value = audience->mAddedAudienceCount;
			audience->mAddedAudienceCount = 0.0f;
		}
		else {
			value = 0.0f;
		}
	}
	else if (audience->mAddedAudienceCount < 0.0f && value > 0.0f) {
		audience->mAddedAudienceCount += value;
		if (audience->mAddedAudienceCount > 0.0f) {
			value = audience->mAddedAudienceCount;
			audience->mAddedAudienceCount = 0.0f;
		}
		else {
			value = 0.0f;
		}
	}
	BattleAudienceAddTargetNumSub(value);
	if (audience->mTargetAudienceCount < 0.0f) {
		audience->mTargetAudienceCount = 0.0f;
	}
	if (audience->mTargetAudienceCount > (f32)audience->mMaxAudience) {
		audience->mTargetAudienceCount = (f32)audience->mMaxAudience;
	}
	if (audience->mAddedAudienceCount < -1000.0f) {
		audience->mAddedAudienceCount = -1000.0f;
	}
	if (audience->mAddedAudienceCount > 1000.0f) {
		audience->mAddedAudienceCount = 1000.0f;
	}
}

//1:1
// keep AddTargetNumSub out of AddTargetNum (original has a `bl`)
void BattleAudienceAddTargetNumSub(f32 value) {
	f32 subtract;
	s32 skew;

	BattleAudience* audience = BattleAudienceBaseGetPtr();
	while (1) {
		if (value == 0.0f) return;
		if (value > 0.0f) { //positive
			subtract = min(value, 1.0f);
			value -= subtract;
			if (value < 0.0f) {
				value = 0.0f;
			}
		}
		if (value < 0.0f) { //negative
			subtract = max(value, -1.0f);
			value -= subtract;
			if (value > 0.0f) {
				value = 0.0f;
			}
		}
		if (subtract > 0.0f) {
			skew = (s32)(((audience->mTargetAudienceCount / audience->mAudienceDeltaMultiplier) - 1.0f) / 0.25f);
			if (skew >= 1) {
				audience->mTargetAudienceCount += subtract / (skew + 1);
				continue;
			}
		}
		if (subtract < 0.0f) {
			skew = (s32)(((audience->mTargetAudienceCount / audience->mAudienceDeltaMultiplier) - 1.0f) / 0.25f);
			if (skew <= -1) {
				audience->mTargetAudienceCount -= subtract / (skew - 1);
				continue;
			}
		}
		audience->mTargetAudienceCount += subtract;
	}
}

s32 BattleAudienceAddAudienceNum(s32 num) {
	BattleWork* wp = _battleWorkPointer;
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 pick;
	s32 total = 0;
	u8 type;
	s32 result = 0;
	s32 count;
	s32 id;
	s32 j;
	s32 i;
	s32 sum;
	s32 waitlist[200];

	if (pouchGetPtr()->mStarPowersObtained == 0) {
		return 0;
	}
	if (num == 0) {
		return 0;
	}
	if (wp->flags & 0x100) {
		return 0;
	}
	if (num > 0) {
		for (i = 0; i < 12; i++) {
			total += audience->typeWeights[i];
		}
		for (i = 0; i < num; i++) {
			sum = 0;
			pick = irand(total);
			for (j = 0; j < 12; j++) {
				sum += audience->typeWeights[j];
				if (pick < sum) {
					type = (u8)j;
					break;
				}
			}
			id = BattleAudience_NoUsedFCHaitiRand();
			if (id == -1) {
				break;
			}
			if (BattleAudience_Entry(id, type, 0xb) == 1) {
				result++;
				BattleAudienceGetPtr(id)->counter = result;
			}
		}
	}
	else {
		for (count = 0; count > num; count--) {
			s32 n = 0;
			for (j = 0; j < 200; j++) {
				if ((u8)BattleAudience_GetEscapeChangeOK(j)) {
					waitlist[n++] = j;
				}
			}
			if (n >= 1) {
				id = waitlist[irand(n)];
				BattleAudience_ChangeStatus(id, 0xc);
				result--;
				BattleAudienceGetPtr(id)->counter = -result;
			}
		}
	}
	return result;
}

void BattleAudiencePuniAllEscape(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 one = 1.0f;
	s32 i;
	s32 count = 0;
	for (i = 0; i < 200; i++) {
		BattleAudienceMember* member = BattleAudienceGetPtr(i);
		if ((u8)BattleAudience_GetEscapeChangeOK(i)) {
			if (member->type == AUDIENCE_PUNI) {
				BattleAudience_ChangeStatus(i, 12);
				count++;
				audience->mAddedAudienceCount -= one;
			}
		}
	}
	if (count >= 1) {
		audience->flags |= 0x8000;
	}
}

void BattleAudienceAddPuni(s32 num) {
	BattleWork* wp = _battleWorkPointer;
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 i;
	if (pouchGetPtr()->mStarPowersObtained != 0 && audience->typeWeights[7] > 0 &&
			num > 0 && !(wp->flags & 0x100)) {
		f32 one = 1.0f;
		for (i = 0; i < num; i++) {
			s32 idx = BattleAudience_NoUsedFCHaitiRand();
			if (idx == -1) {
				return;
			}
			if (BattleAudience_Entry(idx, AUDIENCE_PUNI, 0xb) == 1) {
				audience->mAddedAudienceCount += one;
			}
		}
	}
}

void BattleAudienceAddPhaseEvtList(s32 evtId) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 count = audience->phaseEvtCount;
	s32 i = 0;
	for (; i < count; i++) {
		if (audience->phaseEvtList[i] == evtId) {
			return;
		}
	}
	audience->phaseEvtList[count] = evtId;
	audience->phaseEvtCount++;
}

void BattleAudienceCheer(s32 a) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	if (audience->apGauge > 0x320) {
		audience->apGauge = 0x320;
	}
	if (audience->apGauge > 0) {
		if (pouchGetPtr()->maximumSP == 0) {
			audience->apGauge = 0;
		}
		else {
			audience->flags |= 2;
		}
		BattleAudienceJoy(a);
		if (audience->apGauge != 0) {
			BattleStatusWindowAPRecoveryOn();
		}
	}
}

void BattleAudienceJoy(s32 a) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	switch (a) {
		case 0:
			BattleAudienceJoy_Sub(0x64, 0, 0, a);
			break;
		case 1:
			BattleAudienceJoy_Sub(0, 0x64, 0, a);
			break;
		case 2:
			BattleAudienceJoy_Sub(0x46, 0x1e, 0, a);
			break;
		case 3:
		switch (audience->joyLevel) {
			case 0:
				BattleAudienceJoy_Sub(0x32, 0x28, 0xa, a);
				break;
			case 1:
				BattleAudienceJoy_Sub(0x28, 0x28, 0x14, a);
				break;
			case 2:
				BattleAudienceJoy_Sub(0x1e, 0x28, 0x1e, a);
				break;
			case 3:
				BattleAudienceJoy_Sub(0x14, 0x28, 0x28, a);
				break;
			case 4:
				BattleAudienceJoy_Sub(0xa, 0x28, 0x32, a);
				break;
			default:
				BattleAudienceJoy_Sub(0x32, 0x28, 0xa, a);
				break;
		}
		audience->joyLevel++;
		if (audience->joyLevel > 4) {
			audience->joyLevel = 4;
		}
			break;
	}
}

void BattleAudienceJoySACLecture(void) {
	BattleAudienceSoundCheer(0x78, 0x3c);
	BattleAudienceJoy(2);
}

void BattleAudienceJoyEnding(void) {
	BattleAudienceJoy(3);
}

void BattleAudienceJoy_Sub(s32 a, s32 b, s32 c, s32 d) {
	BattleAudienceMember* member;
	s32 i;
	s32 sum;
	BattleAudience* audience;
	s32 r;
	s32 limit;
	audience = BattleAudienceBaseGetPtr();
	sum = a + b + c;
	i = 0;
	member = BattleAudienceGetPtr(0);
	for (; i < 200; i++, member++) {
		if (BattleAudience_GetWaiting(i)) {
			if (member->type == AUDIENCE_PIRANHA_PLANT) {
				BattleAudience_ChangeStatus(i, 4);
				if (d == 3) {
					member->repeatCount = 3.0f;
				}
				else {
					member->repeatCount = 2.0f;
				}
			}
			else {
				r = irand(sum);
				limit = a;
				if (r < limit) {
					BattleAudience_ChangeStatus(i, 4);
					if (d == 3) {
						member->repeatCount = 3.0f;
					}
					else {
						member->repeatCount = 2.0f;
					}
				}
				else {
					limit += b;
					if (r < limit) {
						BattleAudience_ChangeStatus(i, 5);
					}
					else {
						limit += c;
						if (r < limit) {
							BattleAudience_ChangeStatus(i, 6);
						}
					}
				}
			}
		}
	}
}

s32 BattleAudienceDetectPakkunEatTargetSub2(s32 a, s32 b) {
	s32 no;
	BattleAudienceBaseGetPtr();
	no = BattleAudience_GetAudienceNoFromOffset(a, b, 0);
	if (no == -1) {
		return -1;
	}
	BattleAudienceGetPtr(no);
	if ((u8)BattleAudience_GetSysCtrl(no) != 1) {
		return -1;
	}
	return no;
}

s32 BattleAudienceDetectPakkunEatTargetSub(s32 a, s32 b, s32 c) {
	s32 x = BattleAudienceDetectPakkunEatTargetSub2(a, b);
	s32 y = BattleAudienceDetectPakkunEatTargetSub2(a, c);
	if (x != -1) {
		return x;
	}
	if (y == -1) {
		return -1;
	}
	return y;
}

s32 BattleAudienceDetectPakkunEatTarget(s32 a) {
	if (irand(2) == 0) {
		return BattleAudienceDetectPakkunEatTargetSub(a, -1, 1);
	}
	return BattleAudienceDetectPakkunEatTargetSub(a, 1, -1);
}

void BattleAudienceSoundBooing(void) {
	s32 kind;
	s32 l;
	s32 r;
	s32 count;
	BattleAudienceBaseGetPtr();
	kind = 0;
	l = BattleAudience_GetPPAudienceNum_L();
	r = BattleAudience_GetPPAudienceNum_R();
	count = r + l;
	if (count >= 0 && count <= 0x31) {
		kind = 1;
	}
	if (count >= 0x32 && count <= 0x63) {
		kind = 2;
	}
	if (count >= 0x64 && count <= 0x95) {
		kind = 3;
	}
	if (count >= 0x96) {
		kind = 4;
	}
	BattleAudienceSoundBooingKind(kind);
}

void BattleAudienceSoundBooingKind(s32 kind) {
	BattleAudienceSound* sound;
	s32 r;
	s32 l;
	s32 vol;
	char name[0x34];
	BattleAudienceBaseGetPtr();
	sound = BattleAudienceSoundGetPtr(0);
	r = BattleAudience_GetPPAudienceNum_R();
	l = BattleAudience_GetPPAudienceNum_L();
	BattleAudienceSoundStop(0);
	if (kind == 1) {
		strcpy(name, "SFX_AUDIENCE_BOO_ALLY1");
		sound->variant = 1;
	}
	if (kind == 2) {
		strcpy(name, "SFX_AUDIENCE_BOO_ALLY2");
		sound->variant = 2;
	}
	if (kind == 3) {
		strcpy(name, "SFX_AUDIENCE_BOO_ALLY3");
		sound->variant = 3;
	}
	if (kind == 4) {
		strcpy(name, "SFX_AUDIENCE_BOO_ALLY4");
		sound->variant = 4;
	}
	if (kind == 5) {
		strcpy(name, "SFX_AUDIENCE_BOO_ENEMY1");
		sound->variant = 5;
	}
	if (kind == 6) {
		strcpy(name, "SFX_AUDIENCE_BOO_ENEMY2");
		sound->variant = 6;
	}
	if (kind == 7) {
		strcpy(name, "SFX_AUDIENCE_BOO_ENEMY3");
		sound->variant = 7;
	}
	if (kind == 8) {
		strcpy(name, "SFX_AUDIENCE_BOO_ENEMY4");
		sound->variant = 8;
	}
	AUD_SOUND_ID(sound) = BattleAudienceSound1(name, r, l);
	if (AUD_SOUND_ID(sound) != -1) {
		AUD_SOUND_HEAD(sound);
		vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
		AUD_SOUND_TAIL(sound, vol);
	}
}

void BattleAudienceSoundClap(s32 a, s32 b) {
	BattleAudienceSound* sound;
	s32 r;
	s32 l;
	s32 total;
	s32 vol;
	char name[0x3c];
	BattleAudienceBaseGetPtr();
	sound = BattleAudienceSoundGetPtr(1);
	r = BattleAudience_GetPPAudienceNum_R();
	l = BattleAudience_GetPPAudienceNum_L();
	total = r + l;
	BattleAudienceSoundStop(1);
	if (total >= 1 && total <= 0x31) {
		strcpy(name, "SFX_AUDIENCE_CLAPPING1");
		sound->variant = 1;
	}
	if (total >= 0x32 && total <= 0x63) {
		strcpy(name, "SFX_AUDIENCE_CLAPPING2");
		sound->variant = 2;
	}
	if (total >= 0x64 && total <= 0x95) {
		strcpy(name, "SFX_AUDIENCE_CLAPPING3");
		sound->variant = 3;
	}
	if (total >= 0x96) {
		strcpy(name, "SFX_AUDIENCE_CLAPPING4");
		sound->variant = 4;
	}
	AUD_SOUND_ID(sound) = BattleAudienceSound1(name, r, l);
	if (AUD_SOUND_ID(sound) != -1) {
		sound->flags |= 1;
		sound->timeLeft = a + b;
		sound->mFadeoutTime = b;
		vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
		AUD_SOUND_TAIL(sound, vol);
	}
}

void BattleAudienceSoundNoiseAlways(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	BattleAudienceSound* sound = BattleAudienceSoundGetPtr(2);
	s32 r;
	s32 l;
	s32 total;
	s32 vol;
	char name[0x40];

	r = BattleAudience_GetPPAudienceNum_R();
	l = BattleAudience_GetPPAudienceNum_L();
	total = r + l;
	if (!(sound->flags & 1)) {
		if (total == 0) {
			sound->variant = 0;
		}
		if (total >= 1 && total <= 0x31) {
			strcpy(name, "SFX_AUDIENCE_NOISY1");
			sound->variant = 1;
		}
		if (total >= 0x32 && total <= 0x63) {
			strcpy(name, "SFX_AUDIENCE_NOISY2");
			sound->variant = 2;
		}
		if (total >= 0x64 && total <= 0x95) {
			strcpy(name, "SFX_AUDIENCE_NOISY3");
			sound->variant = 3;
		}
		if (total >= 0x96) {
			strcpy(name, "SFX_AUDIENCE_NOISY4");
			sound->variant = 4;
		}
		sound->sfxId = BattleAudienceSound1(name, r, l);
		if (sound->sfxId != -1) {
			sound->flags |= 1;
			sound->timeLeft = -1;
			sound->mFadeoutTime = 0;
			vol = psndSFX_get_vol(sound->sfxId);
			sound->mCurrentBaseSfxVolume = vol;
			sound->mBaseSfxVolume = vol;
			sound->mForceFadeLength = 0;
			sound->mForceFadeTimer = 0;
			sound->mVolumeMultiplier = 100;
			audience->flags |= 0x2000;
		}
	}
}

void BattleAudienceSoundCheer(s32 a, s32 b) {
	s32 kind;
	s32 l;
	s32 r;
	s32 count;
	BattleAudienceBaseGetPtr();
	kind = 0;
	l = BattleAudience_GetPPAudienceNum_L();
	r = BattleAudience_GetPPAudienceNum_R();
	count = r + l;
	if (count >= 1 && count <= 0x31) {
		kind = 1;
	}
	if (count >= 0x32 && count <= 0x63) {
		kind = 2;
	}
	if (count >= 0x64 && count <= 0x95) {
		kind = 3;
	}
	if (count >= 0x96) {
		kind = 4;
	}
	BattleAudienceSoundCheerKind(kind, a, b);
}

void BattleAudienceSoundCheerKind(s32 kind, s32 a, s32 b) {
	BattleAudienceSound* sound;
	BattleAudienceSound* snd4;
	BattleAudienceSound* snd5;
	s32 r;
	s32 l;
	s32 flag;
	s32 vol;
	char name[0x40];

	BattleAudienceBaseGetPtr();
	flag = 0;
	r = BattleAudience_GetPPAudienceNum_R();
	l = BattleAudience_GetPPAudienceNum_L();
	snd4 = BattleAudienceSoundGetPtr(4);
	snd5 = BattleAudienceSoundGetPtr(5);
	if ((snd4->flags & 1) && (snd5->flags & 1)) {
		if (snd4->timeLeft <= snd5->timeLeft) {
			BattleAudienceSoundStop(4);
			sound = BattleAudienceSoundGetPtr(4);
		}
		else {
			BattleAudienceSoundStop(5);
			sound = BattleAudienceSoundGetPtr(5);
		}
	}
	else if (snd4->flags & 1) {
		sound = BattleAudienceSoundGetPtr(5);
		flag = 1;
	}
	else if (snd5->flags & 1) {
		sound = BattleAudienceSoundGetPtr(4);
		flag = 1;
	}
	else {
		sound = BattleAudienceSoundGetPtr(4);
	}
	if (kind == 1) {
		strcpy(name, "SFX_AUDIENCE_CHEER_ALLY1");
		sound->variant = 1;
	}
	if (kind == 2) {
		strcpy(name, "SFX_AUDIENCE_CHEER_ALLY2");
		sound->variant = 2;
	}
	if (kind == 3) {
		strcpy(name, "SFX_AUDIENCE_CHEER_ALLY3");
		sound->variant = 3;
	}
	if (kind == 4) {
		strcpy(name, "SFX_AUDIENCE_CHEER_ALLY4");
		sound->variant = 4;
	}
	if (flag == 1) {
		sound->sfxId = BattleAudienceSound1(name, r, l);
		psndSFX_pit(sound->sfxId, 0x7F);
	}
	else {
		sound->sfxId = BattleAudienceSound1(name, r, l);
	}
	if (sound->sfxId != -1) {
		sound->flags |= 1;
		sound->timeLeft = a + b;
		sound->mFadeoutTime = b;
		vol = psndSFX_get_vol(sound->sfxId);
		sound->mCurrentBaseSfxVolume = vol;
		sound->mBaseSfxVolume = vol;
		sound->mForceFadeLength = 0;
		sound->mForceFadeTimer = 0;
		sound->mVolumeMultiplier = 100;
	}
}

void BattleAudienceSoundWhistle(void) {
	BattleAudienceSoundWhistleKind(irand(3) + 1);
}

void BattleAudienceSoundWhistleKind(s32 kind) {
	s32 waiting[200];
	s32 i;
	s32 vol;
	s32 member;
	s32 kindIdx;
	s32 count = 0;
	Vec pos;
	BattleAudienceSound* sound;

	for (i = 0; i < 200; i++) {
		if (BattleAudience_GetWaiting(i) == 1) {
			waiting[count++] = i;
		}
	}
	if (count == 0) {
		return;
	}
	member = waiting[irand(count)];
	BattleAudienceSoundGetInfo2(member, &pos);
	if (pos.x <= 0.0f) {
		kindIdx = 6;
	}
	else {
		kindIdx = 7;
	}
	sound = BattleAudienceSoundGetPtr(kindIdx);
	BattleAudienceSoundStop(kindIdx);
	switch (kind) {
	case 1:
		sound->sfxId = BattleAudienceSound2("SFX_AUDIENCE_WHISTLE1", member);
		break;
	case 2:
		sound->sfxId = BattleAudienceSound2("SFX_AUDIENCE_WHISTLE2", member);
		break;
	case 3:
		sound->sfxId = BattleAudienceSound2("SFX_AUDIENCE_WHISTLE3", member);
		break;
	}
	if (sound->sfxId != -1) {
		sound->flags |= 1;
		sound->timeLeft = -1;
		sound->mFadeoutTime = 0;
		vol = psndSFX_get_vol(sound->sfxId);
		sound->mCurrentBaseSfxVolume = vol;
		sound->mBaseSfxVolume = vol;
		sound->mForceFadeLength = 0;
		sound->mForceFadeTimer = 0;
		sound->mVolumeMultiplier = 100;
	}
}

void BattleAudienceSoundCallKind(s32 kind) {
	BattleAudienceSound* sound;
	s32 r;
	s32 l;
	s32 vol;
	BattleAudienceBaseGetPtr();
	sound = BattleAudienceSoundGetPtr(8);
	r = BattleAudience_GetPPAudienceNum_R();
	l = BattleAudience_GetPPAudienceNum_L();
	BattleAudienceSoundStop(8);
	switch (kind) {
	case 1:
		AUD_SOUND_ID(sound) = BattleAudienceSound1("SFX_AUDIENCE_CALL_ALLY1", r, l);
		break;
	case 2:
		AUD_SOUND_ID(sound) = BattleAudienceSound1("SFX_AUDIENCE_CALL_ALLY2", r, l);
		break;
	}
	if (AUD_SOUND_ID(sound) != -1) {
		AUD_SOUND_HEAD(sound);
		vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
		AUD_SOUND_TAIL(sound, vol);
	}
}

void BattleAudienceSoundHandBeat(void) {
	BattleAudienceSound* sound;
	s32 r;
	s32 l;
	s32 total;
	s32 vol;
	BattleAudienceBaseGetPtr();
	sound = BattleAudienceSoundGetPtr(9);
	r = BattleAudience_GetPPAudienceNum_R();
	l = BattleAudience_GetPPAudienceNum_L();
	total = r + l;
	BattleAudienceSoundStop(9);
	AUD_SOUND_ID(sound) = BattleAudienceSound1("SFX_AUDIENCE_HANDBEAT1", r, l);
	if (AUD_SOUND_ID(sound) != -1) {
		sound->flags |= 1;
		sound->timeLeft = -1;
		sound->mFadeoutTime = 0;
		sound->mForceFadeLength = 0;
		sound->mForceFadeTimer = 0;
		vol = 0;
// Writing the guard as total==0 (else compute) is what makes MWCC lay the
// compute block out-of-line (bne+b); the equivalent total!=0 form picks the beq layout.
		if (total == 0) {
			vol = 0;
		}
		else {
			vol = (s32)(75.0f + ((f32)total * (26.0f + (f32)irand(26))) / 200.0f);
		}
		psndSFX_vol(AUD_SOUND_ID(sound), vol);
		sound->mCurrentBaseSfxVolume = vol;
		sound->mBaseSfxVolume = vol;
		sound->mVolumeMultiplier = 100;
	}
}

void BattleAudienceSoundItemThrow(s32 kind, s32 index) {
	Vec pos;
	BattleAudienceSound* sound;
	s32 idx;
	BattleAudienceSoundGetInfo2(index, &pos);
	if (pos.x <= 0.0f) {
		idx = 0xa;
	}
	else {
		idx = 0xb;
	}
	sound = BattleAudienceSoundGetPtr(idx);
	{
		s32 vol;
		BattleAudienceSoundStop(idx);
		switch (kind) {
			case 0:
				AUD_SOUND_ID(sound) = BattleAudienceSound2("SFX_AUDIENCE_THROW_OBJECT1", index);
				break;
			default:
				AUD_SOUND_ID(sound) = BattleAudienceSound2("SFX_AUDIENCE_THROW_OBJECT2", index);
				break;
		}
		if (AUD_SOUND_ID(sound) != -1) {
			AUD_SOUND_HEAD(sound);
			vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
			AUD_SOUND_TAIL(sound, vol);
		}
	}
}

void BattleAudienceSoundRun(s32 kind) {
	BattleAudienceSound* sound = BattleAudienceSoundGetPtr(0xd);
	s32 vol;
	BattleAudienceSoundStop(0xd);
	switch (kind) {
		case 0:
			AUD_SOUND_ID(sound) = BattleAudienceSound0("SFX_AUDIENCE_RUN1");
			break;
		case 1:
			AUD_SOUND_ID(sound) = BattleAudienceSound0("SFX_AUDIENCE_RUN2");
			break;
	}
	if (AUD_SOUND_ID(sound) != -1) {
		AUD_SOUND_HEAD(sound);
		vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
		AUD_SOUND_TAIL(sound, vol);
	}
}

void BattleAudienceSoundSleep(s32 index) {
	Vec pos;
	BattleAudienceSound* sound;
	s32 idx;
	BattleAudienceSoundGetInfo2(index, &pos);
	if (pos.x <= 0.0f) {
		idx = 0xe;
	}
	else {
		idx = 0xf;
	}
	sound = BattleAudienceSoundGetPtr(idx);
	{
		s32 vol;
		BattleAudienceSoundStop(idx);
		switch (irand(3)) {
			case 0:
				AUD_SOUND_ID(sound) = BattleAudienceSound2("SFX_AUDIENCE_SLEEP1", index);
				break;
			case 1:
				AUD_SOUND_ID(sound) = BattleAudienceSound2("SFX_AUDIENCE_SLEEP2", index);
				break;
			case 2:
				AUD_SOUND_ID(sound) = BattleAudienceSound2("SFX_AUDIENCE_SLEEP3", index);
				break;
		}
		if (AUD_SOUND_ID(sound) != -1) {
			AUD_SOUND_HEAD(sound);
			vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
			AUD_SOUND_TAIL(sound, vol);
		}
	}
}

void BattleAudienceSoundZZZ(s32 index) {
	BattleAudienceSound* sound = BattleAudienceSoundGetPtr(0x10);
	s32 vol;
	BattleAudienceSoundStop(0x10);
	BattleAudienceSound2("SFX_AUDIENCE_ZZZ1", index);
	if (AUD_SOUND_ID(sound) != -1) {
		AUD_SOUND_HEAD(sound);
		vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
		AUD_SOUND_TAIL(sound, vol);
	}
}

void BattleAudienceSoundSing(s32 index) {
	BattleAudienceSound* sound = BattleAudienceSoundGetPtr(0x11);
	s32 vol;
	BattleAudienceSoundStop(0x11);
	AUD_SOUND_ID(sound) = BattleAudienceSound2("SFX_AUDIENCE_PANSY_SINGING1", index);
	if (AUD_SOUND_ID(sound) != -1) {
		AUD_SOUND_HEAD(sound);
		vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
		AUD_SOUND_TAIL(sound, vol);
	}
}

void BattleAudienceSoundShell(s32 index) {
	Vec pos;
	BattleAudienceSound* sound;
	s32 idx;
	BattleAudienceSoundGetInfo2(index, &pos);
	if (pos.x <= 0.0f) {
		idx = 0x12;
	}
	else {
		idx = 0x13;
	}
	sound = BattleAudienceSoundGetPtr(idx);
	{
		s32 vol;
		BattleAudienceSoundStop(idx);
		AUD_SOUND_ID(sound) = BattleAudienceSound2("SFX_AUDIENCE_SHELL_TRANSFORM1", index);
		if (AUD_SOUND_ID(sound) != -1) {
			AUD_SOUND_HEAD(sound);
			vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
			AUD_SOUND_TAIL(sound, vol);
		}
	}
}

void BattleAudienceSoundBombIgnite(s32 index, s32 a, s32 b) {
	BattleAudienceSound* sound = BattleAudienceSoundGetPtr(0x14);
	s32 vol;
	BattleAudienceSoundStop(0x14);
	AUD_SOUND_ID(sound) = BattleAudienceSound2("SFX_AUDIENCE_BOMB_READY1", index);
	if (AUD_SOUND_ID(sound) != -1) {
		sound->flags |= 1;
		sound->timeLeft = a + b;
		sound->mFadeoutTime = b;
		vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
		AUD_SOUND_TAIL(sound, vol);
	}
}

void BattleAudienceSoundBombFire(s32 index) {
	Vec pos;
	BattleAudienceSound* sound;
	s32 idx;
	BattleAudienceSoundGetInfo2(index, &pos);
	if (pos.x <= 0.0f) {
		idx = 0x15;
	}
	else {
		idx = 0x16;
	}
	sound = BattleAudienceSoundGetPtr(idx);
	{
		s32 vol;
		BattleAudienceSoundStop(idx);
		AUD_SOUND_ID(sound) = BattleAudienceSound2("SFX_AUDIENCE_BOMB_BURST1", index);
		if (AUD_SOUND_ID(sound) != -1) {
			AUD_SOUND_HEAD(sound);
			vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
			AUD_SOUND_TAIL(sound, vol);
		}
	}
}

void BattleAudienceSoundPakkunEat(s32 index) {
	BattleAudienceSound* sound = BattleAudienceSoundGetPtr(0x17);
	s32 vol;
	BattleAudienceSoundStop(0x17);
	AUD_SOUND_ID(sound) = BattleAudienceSound2("SFX_AUDIENCE_PAKU_EAT1", index);
	if (AUD_SOUND_ID(sound) != -1) {
		AUD_SOUND_HEAD(sound);
		vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
		AUD_SOUND_TAIL(sound, vol);
	}
}

void BattleAudienceSoundSetVol(s32 index, u8 a2, s32 a3) {
	BattleAudienceSound* sound;

	sound = BattleAudienceSoundGetPtr(index);
	sound->mFadeStartVolumeMultiplier = sound->mVolumeMultiplier;
	sound->mFadeEndVolumeMultiplier = a2;
	sound->mForceFadeTimer = 0;
	sound->mForceFadeLength = a3;
	sound->mFadeoutTime = 0;
}

void BattleAudienceSoundSetVolAll(u8 a2, s32 a3) {
	s32 i;
	for (i = 0; i < 24; i++) {
		BattleAudienceSoundSetVol(i, a2, a3);
	}
}

BOOL BattleAudienceSoundCheck(s32 index) {
	return BattleAudienceSoundGetPtr(index)->flags & 1;
}

void BattleAudienceSoundStop(s32 index) {
	BattleAudienceSound* sound = BattleAudienceSoundGetPtr(index);
	if (sound->flags & 1) {
		psndSFXOff(sound->sfxId);
		sound->flags &= ~1;
	}
}

s32 BattleAudienceSound0(const char* name) {
	return psndSFXOn(name);
}

s32 BattleAudienceSound1(const char* name, s32 a, s32 b) {
	u8 vol;
	Vec pos;
	s32 handle;
	BattleAudienceSoundGetInfo1(a, b, &vol, &pos);
	handle = psndSFXOn_3D(name, &pos);
	psndSFX_vol(handle, vol);
	return handle;
}

void BattleAudienceSoundGetInfo1(s32 a, s32 b, u8* c, Vec* d) {
	s32 sum = a + b;

	if (sum == 0) {
		*c = 0;
	}
	else {
		*c = (u8)(s32)(100.0f + 26.0f * (f32)sum / 200.0f);
	}
	if (b == 0 || sum == 0) {
		d->x = 250.0f;
	}
	else {
		d->x = (f32)(u8)(s32)((f32)(a - b) * 250.0f / (f32)sum);
	}
	d->y = 0.0f;
	d->z = 0.0f;
}

s32 BattleAudienceSound2(const char* name, s32 index) {
	Vec pos;
	BattleAudienceSoundGetInfo2(index, &pos);
	return psndSFXOn_3D(name, &pos);
}

void BattleAudienceSoundGetInfo2(s32 index, Vec* pos) {
	BattleAudienceMember* member = BattleAudienceGetPtr(index);
	pos->x = member->position.x;
	pos->y = member->position.y;
	pos->z = member->position.z;
}

void BattleAudienceSoundMain(void) {
	BattleAudienceSound* sound;
	s32 i;

	BattleAudienceBaseGetPtr();
	BattleAudienceNoiseMain();
	i = 0;
	sound = BattleAudienceSoundGetPtr(0);
	for (; i < 0x18; i++, sound++) {
		if (psndSFXChk(sound->sfxId) == -1) {
			sound->flags &= ~1;
		}
		else if (sound->timeLeft == 0) {
			psndSFXOff(sound->sfxId);
			sound->flags &= ~1;
		}
		else {
			if (sound->mForceFadeLength > 0) {
				sound->mForceFadeTimer++;
				sound->mVolumeMultiplier = (u8)intplGetValue(0, sound->mForceFadeTimer, sound->mForceFadeLength, (f32)sound->mFadeStartVolumeMultiplier, (f32)sound->mFadeEndVolumeMultiplier);
				psndSFX_vol(sound->sfxId, (u8)((sound->mCurrentBaseSfxVolume * sound->mVolumeMultiplier) / 100));
				if (sound->mForceFadeTimer >= sound->mForceFadeLength) {
					sound->mForceFadeLength = 0;
				}
			}
			if (sound->timeLeft > 0) {
				if (sound->timeLeft == sound->mFadeoutTime) {
					sound->mFadeStartVolumeMultiplier = sound->mVolumeMultiplier;
					sound->mFadeEndVolumeMultiplier = 0;
				}
				if (sound->timeLeft <= sound->mFadeoutTime) {
					sound->mVolumeMultiplier = (u8)intplGetValue(0, sound->mFadeoutTime - sound->timeLeft, sound->mFadeoutTime, (f32)sound->mFadeStartVolumeMultiplier, (f32)sound->mFadeEndVolumeMultiplier);
					psndSFX_vol(sound->sfxId, (u8)((sound->mCurrentBaseSfxVolume * sound->mVolumeMultiplier) / 100));
				}
				sound->timeLeft -= 1;
			}
		}
	}
}

void BattleAudienceNoiseMain(void) {
	BattleWork* wp = _battleWorkPointer;
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	BattleAudienceSound* sound = BattleAudienceSoundGetPtr(2);
	s32 pp = BattleAudience_GetPPAudienceNum();

	if (!(audience->flags & 0x10000) && (wp->flags & 0x80)) {
		BattleAudienceSoundSetVol(2, 0x3C, 0x50);
	}
	if ((audience->flags & 0x10000) && !(wp->flags & 0x80)) {
		BattleAudienceSoundSetVol(2, 0, 0x50);
	}
	if (pp == 0 && sound->variant != 0) {
		psndSFXOff(sound->sfxId);
		sound->variant = 0;
	}
	if (pp >= 1 && pp <= 0x31 && sound->variant != 1) {
		BattleAudienceSoundNoiseAlways();
		sound->variant = 1;
	}
	if (pp >= 0x32 && pp <= 0x63 && sound->variant != 2) {
		BattleAudienceSoundNoiseAlways();
		sound->variant = 2;
	}
	if (pp >= 0x64 && pp <= 0x95 && sound->variant != 3) {
		BattleAudienceSoundNoiseAlways();
		sound->variant = 3;
	}
	if (pp >= 0x96 && pp <= 0xC8 && sound->variant != 4) {
		BattleAudienceSoundNoiseAlways();
		sound->variant = 4;
	}
	if (!(audience->flags & 0x2000)) {
		sound->mCurrentBaseSfxVolume = 0;
	}
	else if (pp == 0) {
		sound->mCurrentBaseSfxVolume = 0;
	}
	else {
		sound->mCurrentBaseSfxVolume = (s32)(63.0f + 63.0f * (f32)pp / 200.0f);
	}
}

void BattleAudience_Case_KillEnemy(void) {
	BattleAudienceBaseGetPtr()->flags |= 0x1000;
}

void BattleAudience_Case_ActionCommandGood(BattleWeapon* weapon) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 i;
	s32 count = 0;
	for (i = 0; i < 200; i++) {
		if (BattleAudience_GetWaiting(i) == 1) {
			count++;
		}
	}
	if (count != 0) {
		BattleAudienceSoundCheer(0xb4, 0x78);
		BattleAudienceJoy(0);
	}
	audience->flags |= 0x10;
}

void BattleAudience_Case_ActionCommandBad(BattleWeapon* weapon) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 count = 0;
	s32 i;
	for (i = 0; i < 200; i++) {
		if (BattleAudience_GetWaiting(i) == 1) {
			count++;
		}
	}
	if (count != 0) {
		BattleAudienceSoundBooing();
		BattleAudienceJoy(0);
		if (weapon->hpDamageFn == weaponGetPowerFromMarioHammerLv) {
			BattleAudienceAddPhaseEvtList(7);
		}
	}
	audience->flags |= 0x20;
}

void BattleAudience_Case_GuardGood(void) {
	BattleAudienceBaseGetPtr()->flags |= 0x40;
}

void BattleAudience_Case_GuardBad(void) {
	BattleAudienceBaseGetPtr()->flags |= 0x80;
}

void BattleAudience_Case_AcrobatGood(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 count = 0;
	s32 i;
	for (i = 0; i < 200; i++) {
		if (BattleAudience_GetWaiting(i) == 1) {
			count++;
		}
	}
	if (count >= 1) {
		BattleAudienceSoundCheer(0xb4, 0x78);
		BattleAudienceSoundClap(0xb4, 0x78);
		BattleAudienceJoy(1);
	}
	audience->mNumStylishCommandsThisAttack++;
	if (audience->mNumStylishCommandsThisAttack > 5) {
		audience->mNumStylishCommandsThisAttack = 5;
	}
	audience->flags |= 0x100;
}

void BattleAudience_Case_AcrobatBad(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 count = 0;
	s32 i;
	for (i = 0; i < 200; i++) {
		if (BattleAudience_GetWaiting(i) == 1) {
			count++;
		}
	}
	if (count >= 1) {
		BattleAudienceSoundBooing();
		BattleAudienceJoy(0);
	}
	audience->flags |= 0x200;
}

void BattleAudience_Case_AcrobatNoTry(void) {
	BattleAudienceBaseGetPtr();
}

void BattleAudience_Case_MarioBigDamage(void) {
	BattleAudienceBaseGetPtr();
}

void BattleAudience_Case_MarioPinch(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = 0.049999997f * delta;
	BattleAudienceAddTargetNum(delta, 0.0f);
}

void BattleAudience_Case_MarioDanger(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = 0.14999999f * delta;
	BattleAudienceAddTargetNum(delta, 0.0f);
}

void BattleAudience_Case_PartyDown(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = -0.25f * delta;
	BattleAudienceAddTargetNum(delta, 0.0f);
}

void BattleAudience_Case_JumpNewRecord(void) {
}

void BattleAudience_Case_FinalAttack(void) {
}

void BattleAudience_Case_TurnEnd(void) {
	BattleWork* wp = _battleWorkPointer;
	BattleUnit* mario = BattleGetMarioPtr(wp);
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 expected;
	BattleGroupSetup* setup;
	BattleUnitSetup* enemy;
	s32 i;

	pouchGetPtr();
	expected = 0;
	setup = wp->fbatInfo->setup->groupSetup;
	enemy = setup->enemyData;
	if (!(audience->flags & 0x40000)) {
		audience->mTurnEndPhaseEventChance += 3;
		if (audience->mTurnEndPhaseEventChance > 100) {
			audience->mTurnEndPhaseEventChance = 100;
		}
		for (i = 0; i < setup->numEnemies; i++) {
			expected += enemy->mUnitKindParams->mLevel;
			enemy++;
		}
		expected = (expected * 2) / mario->mLevel;
		if (expected < 5) {
			expected = 5;
		}
		if (wp->turnCount >= expected) {
			BattleAudienceAddTargetNum(-0.049999997f * BattleAudienceBaseGetPtr()->mAudienceDeltaMultiplier,
			                           0.0f);
		}
		if (wp->turnCount == expected) {
			BattleAudienceAddPhaseEvtList(6);
		}
		if (irand(100) < audience->mTurnEndPhaseEventChance) {
			if (wp->turnCount >= expected) {
				BattleAudienceAddPhaseEvtList(1);
			}
			BattleAudienceAddPhaseEvtList(4);
			BattleAudienceAddPhaseEvtList(9);
			BattleAudienceAddPhaseEvtList(10);
			BattleAudienceAddPhaseEvtList(11);
			BattleAudienceAddPhaseEvtList(12);
			BattleAudienceAddPhaseEvtList(13);
			audience->mTurnEndPhaseEventChance = 0;
		}
		BattleAudienceNumToTarget();
	}
}

// The assignment's operand order is load-bearing: MWCC emits the target's
// `fmuls f0, f1, f0` (converted value first) only when the int->float conversion is
// held in a named temp, and only keeps the audience field in r0 and writes the
// result back through r0 when the sum is written as `(v + 0x19) + field`. The
// natural compound assignment is the #else arm.
void BattleAudience_Case_Appeal(BattleUnit* unit) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();

	if ((f32)BattleAudience_GetPPAudienceNum() > 0.0f) {
		audience->apGauge += (s32)((f32)BattleAudience_GetPPAudienceNum() / 4.0f) + 0x19;
		if (unit->badgesEquipped.SuperAppeal != 0) {
			audience->apGauge += unit->badgesEquipped.SuperAppeal * 0x19;
		}
		BattleAudienceSoundCheer(0xB4, 0x3C);
		BattleAudienceSoundClap(0xB4, 0x3C);
		BattleAudienceJoy(0);
	}
	BattleAudienceAddTargetNum(0.049999997f * BattleAudienceBaseGetPtr()->mAudienceDeltaMultiplier,
	                           0.0f);
}

void BattleAudience_Case_Escape(void) {
}

void BattleAudience_Case_Escape_Good(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = -0.099999994f * delta;
	BattleAudienceAddTargetNum(delta, 0.0f);
	BattleAudienceSoundSetVol(2, 100, 30);
	BattleAudienceSoundBooing();
}

void BattleAudience_Case_Escape_Bad(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = -0.19999999f * delta;
	BattleAudienceAddTargetNum(delta, 0.0f);
	BattleAudienceSoundBooing();
}

void BattleAudience_Case_EnemyDamage(s32 damage) {
	BattleAudienceBaseGetPtr()->flags |= 0x400;
}

void BattleAudience_Case_EnemyNoDamage(void) {
	BattleAudienceBaseGetPtr()->flags |= 0x800;
}

void BattleAudience_Case_Countered(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = -0.099999994f * delta;
	BattleAudienceAddTargetNum(delta, 0.0f);
	BattleAudienceAddPhaseEvtList(5);
}

void BattleAudience_Case_HaikeiSet(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = 0.049999997f * delta;
	BattleAudienceAddTargetNum(delta, 0.0f);
}

void BattleAudience_Case_GreatVictory(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = 0.14999999f * delta;
	BattleAudienceAddTargetNum(delta, 0.0f);
}

void BattleAudience_Case_FastVictory(f32 value) {
	BattleAudienceAddTargetNum(
	    0.01f * (5.0f * value) * BattleAudienceBaseGetPtr()->mAudienceDeltaMultiplier,
	    0.0f);
}

void BattleAudience_Case_FallObject_Stage(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = 0.049999997f * delta;
	BattleAudienceAddTargetNum(delta, 0.0f);
}

const s32 dat_802f98fc[] = { -1, 1, 0, 1, 1, 1, -1, 0, 1, 0, -1, -1, 0, -1, 1, -1 };
const s32 dat_802f993c[] = {
	0, 3, -1, 2, 0, 2, 1, 2, -2, 1, -1, 1, 0, 1, 1, 1, 2, 1, -3, 0, -2, 0, -1, 0, 1, 0, 2, 0, 3, 0, -2, -1,
	-1, -1, 0, -1, 1, -1, 2, -1, -1, -2, 0, -2, 1, -2, 0, -3,
};

void BattleAudience_Case_FallObject_Aud(s32 index, u8 type) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	BattleAudienceFallOffsetTable1 tbl1;
	BattleAudienceFallOffsetTable2 tbl2;
	s32 i;
	s32 id;
	f32 one = 1.0f;

	// Struct assignment gives the original's lwzu/stwu copy loop; memcpy doesn't.
	{
		tbl1 = *(const BattleAudienceFallOffsetTable1*)dat_802f98fc;
	}
	{
		tbl2 = *(const BattleAudienceFallOffsetTable2*)dat_802f993c;
	}

	switch (type) {
	case 0:
	case 1:
	case 3:
	case 4:
	case 5:
	case 8:
		if ((u8)BattleAudience_GetSysCtrl(index) == 1) {
			if (BattleAudienceGetPtr(index)->status != 0xF) {
				BattleAudience_ChangeStatus(index, 0xA);
			}
		}
		for (i = 0; i < 8; i++) {
			id = BattleAudience_GetAudienceNoFromOffset(index, tbl1.v[i * 2], tbl1.v[i * 2 + 1]);
			if (id != -1 && (u8)BattleAudience_GetEscapeChangeOK(id)) {
				BattleAudience_ChangeStatus(id, 0xC);
				audience->mTargetAudienceCount -= 1.0f;
			}
		}
		break;
	case 7:
		if ((u8)BattleAudience_GetSysCtrl(index) == 1) {
			if (BattleAudienceGetPtr(index)->status != 0xF) {
				BattleAudience_ChangeStatus(index, 0xA);
			}
		}
		for (i = 0; i < 0x18; i++) {
			id = BattleAudience_GetAudienceNoFromOffset(index, tbl2.v[i * 2], tbl2.v[i * 2 + 1]);
			if (id != -1 && (u8)BattleAudience_GetEscapeChangeOK(id)) {
				BattleAudience_ChangeStatus(id, 0xC);
				audience->mTargetAudienceCount -= 1.0f;
			}
		}
		break;
	case 6:
		if ((u8)BattleAudience_GetSysCtrl(index) == 1) {
			if (BattleAudienceGetPtr(index)->status != 0xF) {
				BattleAudience_ChangeStatus(index, 0xA);
			}
		}
		one = 1.0f;
		for (i = 0; i < 200; i++) {
			if ((u8)BattleAudience_GetEscapeChangeOK(i)) {
				BattleAudience_ChangeStatus(i, 0xC);
				audience->mTargetAudienceCount -= one;
			}
		}
		/* falls through */
	default:
		if ((u8)BattleAudience_GetSysCtrl(index) == 1) {
			if (BattleAudienceGetPtr(index)->status != 0xF) {
				BattleAudience_ChangeStatus(index, 0xA);
			}
		}
		audience->mTargetAudienceCount -= 1.0f;
		break;
	}
	BattleAudiencePuniAllEscape();
}

void BattleAudience_Case_FirstAttack_Good(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = 0.02f * delta;
	BattleAudienceAddTargetNum(delta, 0.0f);
}

void BattleAudience_Case_FirstAttack_Bad(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = -0.049999997f * delta;
	BattleAudienceAddTargetNum(delta, 0.0f);
}

void BattleAudience_SetPresentItemNo(s32 value) {
	BattleAudienceBaseGetPtr()->presentItemNo = value;
}

s32 BattleAudience_GetPresentItemNo(void) {
	return BattleAudienceBaseGetPtr()->presentItemNo;
}

void BattleAudience_SetPresentTargetUnitId(s32 value) {
	BattleAudienceBaseGetPtr()->presentTargetUnitId = value;
}

s32 BattleAudience_GetPresentTargetUnitId(void) {
	return BattleAudienceBaseGetPtr()->presentTargetUnitId;
}

void BattleAudience_SetPresentItemType(s32 value) {
	BattleAudienceBaseGetPtr()->presentItemType = value;
}

s32 BattleAudience_GetPresentItemType(void) {
	return BattleAudienceBaseGetPtr()->presentItemType;
}
