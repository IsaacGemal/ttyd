#include "battle/battle_audience.h"
#include "battle/battle.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "mario/mario_pouch.h"
#include "system.h"
#include <string.h>
#include "pmario_sound.h"
#include "battle/battle_weapon_power.h"

extern s32 psndSFX_get_vol(s32 id);
extern void psndSFX_vol(s32 id, s32 vol);
extern s32 psndSFX_pit(s32 id, s32 pit);

#pragma optimize_for_size on

const char str_802F9578[] = "btl_msg_audience_puni_escape";
const char str_802F9598[] = "btl_msg_audience_puni_meeting";
const char str_802F95B8[] = "btl_msg_audience_pansy_sleep";
const char str_802F95D8[] = "btl_msg_audience_heavybomb_dynamite";

const f32 R_audience_delta_multiplier[] = {
	8.0f, 11.0f, 14.0f, 17.0f, 20.0f, 23.0f, 26.0f, 29.0f, 32.0f, 35.0f, 38.0f, 41.0f, 44.0f,
	47.0f, 50.0f, 53.0f, 56.0f, 59.0f, 62.0f, 65.0f, 68.0f, 71.0f, 74.0f, 77.0f, 80.0f, 83.0f,
	86.0f, 89.0f, 92.0f, 95.0f, 98.0f, 101.0f, 104.0f, 107.0f, 110.0f, 113.0f, 116.0f, 119.0f,
	122.0f, 125.0f, 128.0f, 131.0f, 134.0f, 137.0f, 140.0f, 143.0f, 146.0f, 149.0f, 152.0f, 155.0f,
	158.0f, 161.0f, 164.0f, 167.0f, 170.0f, 173.0f, 176.0f, 179.0f, 182.0f, 185.0f, 188.0f, 191.0f,
	194.0f, 197.0f, 200.0f,
};

const s32 R_num_audience_type_weights[] = { 20, 40, 20, 5, 3, 2, 1, 1, 1, 1, 1 };

const Vec vec3_802f972c = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f9738 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f9744 = { 0.9f, 0.9f, 0.9f };
const Vec vec3_802f9750 = { 0.75f, 0.75f, 0.75f };
const Vec vec3_802f975c = { -96.0f, 106.0f, 0.0f };
const Vec vec3_802f9768 = { 0.0f, 0.0f, -1.0f };
const f32 dat_802f9774[] = {
	0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	0.0f, 1.075f, 0.0f, 0.075f, 1.0f, 0.075f, 1.0f, 1.075f,
	0.0f, 0.075f, 0.0f, 1.075f, 1.0f, 1.075f, 0.0f, 1.075f,
};
const Vec vec3_802f97f4 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f9800 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f980c = { 0.0f, 0.0f, -1.0f };
const Vec vec3_802f9818 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f9824 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f9830 = { 1.0f, 1.0f, 1.0f };
const Vec vec3_802f983c = { 1.0f, 1.0f, 1.0f };
const Vec vec3_802f9848 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f9854 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f9860 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f986c = { 1.0f, 1.0f, 1.0f };
const Vec vec3_802f9878 = { 0.0f, 0.0f, 0.0f };
const s32 dat_802f9884[] = { 10000, 1000, 100, 10, 5, 2, 1, 1, 1, 1 };
const s32 dat_802f98ac[] = { 1, 1, 2, 2, 4, 4, 8, 8, 16, 16, 16, 16, 8, 8, 4, 4, 2, 2, 1, 1 };
const s32 dat_802f98fc[] = { -1, 1, 0, 1, 1, 1, -1, 0, 1, 0, -1, -1, 0, -1, 1, -1 };
const s32 dat_802f993c[] = {
	0, 3, -1, 2, 0, 2, 1, 2, -2, 1, -1, 1, 0, 1, 1, 1, 2, 1, -3, 0, -2, 0, -1, 0, 1, 0, 2, 0, 3, 0, -2, -1,
	-1, -1, 0, -1, 1, -1, 2, -1, -1, -2, 0, -2, 1, -2, 0, -3,
};
const f64 double_to_int_802f9a00 = 4503601774854144.0;
const f64 double_to_int_mask_802f9a08 = 4503599627370496.0;

const char str_802F9A10[] = "SFX_AUDIENCE_PAKU_EAT1";
const char str_802F9A28[] = "SFX_AUDIENCE_BOMB_BURST1";
const char str_802F9A44[] = "SFX_AUDIENCE_BOMB_READY1";
const char str_802F9A60[] = "SFX_AUDIENCE_SHELL_TRANSFORM1";
const char str_802F9A80[] = "SFX_AUDIENCE_PANSY_SINGING1";
const char str_802F9A9C[] = "SFX_AUDIENCE_ZZZ1";
const char str_802F9AB0[] = "SFX_AUDIENCE_SLEEP1";
const char str_802F9AC4[] = "SFX_AUDIENCE_SLEEP2";
const char str_802F9AD8[] = "SFX_AUDIENCE_SLEEP3";
const char str_802F9AEC[] = "SFX_AUDIENCE_RUN1";
const char str_802F9B00[] = "SFX_AUDIENCE_RUN2";
const char str_802F9B14[] = "SFX_AUDIENCE_THROW_OBJECT1";
const char str_802F9B30[] = "SFX_AUDIENCE_THROW_OBJECT2";
const char str_802F9B4C[] = "SFX_AUDIENCE_HANDBEAT1";
const char str_802F9B64[] = "SFX_AUDIENCE_CALL_ALLY1";
const char str_802F9B7C[] = "SFX_AUDIENCE_CALL_ALLY2";
const char str_802F9B94[] = "SFX_AUDIENCE_WHISTLE1";
const char str_802F9BAC[] = "SFX_AUDIENCE_WHISTLE2";
const char str_802F9BC4[] = "SFX_AUDIENCE_WHISTLE3";
const char str_802F9BDC[] = "SFX_AUDIENCE_CHEER_ALLY1";
const char str_802F9BF8[] = "SFX_AUDIENCE_CHEER_ALLY2";
const char str_802F9C14[] = "SFX_AUDIENCE_CHEER_ALLY3";
const char str_802F9C30[] = "SFX_AUDIENCE_CHEER_ALLY4";
const char str_802F9C4C[] = "SFX_AUDIENCE_NOISY1";
const char str_802F9C60[] = "SFX_AUDIENCE_NOISY2";
const char str_802F9C74[] = "SFX_AUDIENCE_NOISY3";
const char str_802F9C88[] = "SFX_AUDIENCE_NOISY4";
const char str_802F9C9C[] = "SFX_AUDIENCE_CLAPPING1";
const char str_802F9CB4[] = "SFX_AUDIENCE_CLAPPING2";
const char str_802F9CCC[] = "SFX_AUDIENCE_CLAPPING3";
const char str_802F9CE4[] = "SFX_AUDIENCE_CLAPPING4";
const char str_802F9CFC[] = "SFX_AUDIENCE_BOO_ALLY1";
const char str_802F9D14[] = "SFX_AUDIENCE_BOO_ALLY2";
const char str_802F9D2C[] = "SFX_AUDIENCE_BOO_ALLY3";
const char str_802F9D44[] = "SFX_AUDIENCE_BOO_ALLY4";
const char str_802F9D5C[] = "SFX_AUDIENCE_BOO_ENEMY1";
const char str_802F9D74[] = "SFX_AUDIENCE_BOO_ENEMY2";
const char str_802F9D8C[] = "SFX_AUDIENCE_BOO_ENEMY3";
const char str_802F9DA4[] = "SFX_AUDIENCE_BOO_ENEMY4";
const char str_802F9DBC[] = "SFX_BTL_SYS_AP_STAR1";
const char str_802F9DD4[] = "battle/audience/%s";
const char str_802F9DE8[] = "battle/audience/audience_normal.tpl";
const char str_802F9E0C[] = "audience_luigi.tpl";

f32 float_1_8041e620 = 1.0f;
f32 float_neg1_8041e664 = -1.0f;
f32 float_0p25_8041e63c = 0.25f;
f32 float_200_8041e648 = 200.0f;
f32 float_26_8041e650 = 26.0f;
f32 float_75_8041e658 = 75.0f;
f32 float_63_8041e644 = 63.0f;
f32 float_250_8041e654 = 250.0f;
f32 float_100_8041e64c = 100.0f;
f32 float_3_8041e65c = 3.0f;
f32 float_2_8041e660 = 2.0f;
f32 float_0_8041e618 = 0.0f;
f32 float_0p05_8041e624 = 0.05f;
f32 float_0p15_8041e630 = 0.15f;
f32 float_neg0p25_8041e640 = -0.25f;
f32 float_neg0p05_8041e614 = -0.05f;
f32 float_neg0p1_8041e634 = -0.1f;
f32 float_neg0p2_8041e638 = -0.2f;
f32 float_0p02_8041e61c = 0.02f;
f32 float_5_8041e62c = 5.0f;
f32 float_0p01_8041e628 = 0.01f;

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

extern BattleWork* _battleWorkPointer;
extern int sprintf(char* str, const char* format, ...);

//local prototypes
FileEntry* tplRead(const char* path);
BOOL check_exe_phase_evt_status(s32 index, s32 type);
void BattleAudienceSettingAudience(void);
void BattleAudienceGuestTPLRead(s32 index, AudienceMemberType member, const char* tplName);
void BattleAudienceCtrlProcess(void);
BOOL BattleAudienceItemOn(s32 index, u16 itemId, s32 numItems);
void BattleAudienceSetThrowItemMax(void);
BattleUnit* BattleAudienceDetectTargetAll(void);
BattleUnit* BattleAudienceDetectTargetPlayer(void);
void BattleAudienceItemCtrlProcess(void);
void BattleAudienceApSrcCtrlProcess(void);
BOOL BattleAudienceApSrcEntry(void);
void BattleAudienceWinCtrlProcess(void);
void BattleAudienceGXInit(void);
void BattleAudienceAnimProcess(void);
void BattleAudienceDispAudience(CameraId camId, void* param);
void BattleAudienceDispItem(CameraId camId, void* param);
void BattleAudienceDispApSrc(CameraId camId, void* param);
void BattleAudienceDispWin(CameraId camId, void* param);
void BattleAudience_Entry_Sub(s32 index, u8 type, u8 status);
BOOL BattleAudience_GetAnimEnd(s32 index);
BOOL BattleAudience_GetFront(s32 index);
BOOL BattleAudience_GetEscapeChangeOK(s32 index);
s32 BattleAudience_GetAudienceNoFromOffset(s32 index, s32 column, s32 row);
s32 BattleAudience_NoUsedHaitiRand(void);
s32 BattleAudience_NoUsedFCHaitiRand(void);
s32 BattleAudience_GetPPAudienceNum(void);
s32 BattleAudience_GetPPAudienceNumKind(s32 type);
s32 BattleAudience_GetPPAudienceNum_R(void);
s32 BattleAudience_GetPPAudienceNum_L(void);
s32 BattleAudience_GetPPAudienceNum_RL_Sub(s32 offset);
BOOL BattleAudience_GetPPAudienceNum_Sub(s32 id);










void BattleAudienceAddTargetNumSub(f32 value);
void BattleAudienceAddTargetNum(f32 value, f32 delta);
void BattleAudienceAddAudienceNum(s32 num);
void BattleAudienceAddPhaseEvtList(s32 evtId);
void BattleAudienceJoy(s32 a);
void BattleAudienceJoy_Sub(s32 a, s32 b, s32 c, s32 d);
void BattleAudienceSoundCheer(s32 a, s32 b);
void BattleAudienceSoundCheerKind(s32 kind, s32 a, s32 b);
void BattleAudienceSoundBooingKind(s32 kind);
void BattleAudienceSoundBooing(void);
void BattleAudienceSoundWhistleKind(s32 kind);
void BattleAudienceSoundGetInfo1(s32 a, s32 b, u8* c, Vec* d);
void BattleAudienceSoundGetInfo2(s32 index, Vec* pos);
s32 BattleAudienceSound1(const char* name, s32 a, s32 b);
s32 BattleAudienceSound2(const char* name, s32 index);
void BattleAudienceSoundStop(s32 index);
void BattleAudienceSoundWhistle(void);
void BattleAudiencePuniAllEscape(void);
void BattleAudienceAddPuni(s32 num);
void BattleAudienceCheer(s32 a);
void BattleAudienceNoiseMain(void);
extern s32 psndSFXOn(const char* name);
extern s32 psndSFXOn_3D(const char* name, Vec* pos);
extern void BattleStatusWindowAPRecoveryOn(void);
extern void BattleAudienceSoundClap(s32 a, s32 b);
s32 BattleAudienceDetectPakkunEatTargetSub2(s32 a, s32 b);
s32 BattleAudienceDetectPakkunEatTargetSub(s32 a, s32 b, s32 c);
s32 BattleAudienceNumToTargetSub(void);


void BattleAudienceSoundNoiseAlways(void);
void BattleAudienceSoundSetVol(s32 index, u8 a2, s32 a3);
void BattleAudienceNumToTarget(void);
void BattleAudience_WinSetActive(u8 active);

void BattleAudienceSoundMain(void);


#pragma dont_inline on
FileEntry* tplRead(const char* path) {
	return fileAllocf(4, "%s/%s", getMarioStDvdRoot(), path);
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
#pragma dont_inline reset

// NONMATCHING (77.73%): blocked by the empty BattleAudienceSettingAudience stub (its bl is
// dropped); the resulting liveness difference also shifts the loop registers.
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
	audience->normalTex = tplRead(str_802F9DE8);
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

// NONMATCHING (94.29%): 2 missing, plus register differences.
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

// NONMATCHING (3.28%): 1 instruction differs, 263 missing, plus register differences.
void BattleAudience_PerAct(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	pouchGetPtr();
	//TODO: finish
}

BOOL BattleAudience_CheckReaction(void) {
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

BOOL check_exe_phase_evt_status(s32 index, s32 type) {
	BattleAudienceMember* member;

	member = BattleAudienceGetPtr(index);
	if ((u8)BattleAudience_GetSysCtrl(index) == 0) {
		return FALSE;
	}
	if ((s32)member->type != (s32)type) {
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

//TODO: add evt support and then uncomment message scripts
// NONMATCHING (13.15%): 21 instructions differ, 852 missing, 1 extra, plus register differences.
BOOL BattleAudience_CheckReactionPerPhase(void) {
	BattleAudience* audience;
	BattleAudienceMember* member;
	s32 counter, i;

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
				return TRUE;
			}
			//no break
		case 5:
			for (counter = 0, i = 0; i < 200; i++) {
				member = BattleAudienceGetPtr(i);
				if (BattleAudience_GetSysCtrl(i) && member->type == AUDIENCE_BULKY_BOBOMB &&
						member->status == 18 && --member->field_0x12C <= 0) {
					BattleAudience_ChangeStatus(i, 19);
					counter++;
				}
			}
			if (counter < 1) {
				audience->mCheckPhaseReactionState = 7;
			}
			else {
				audience->mCheckPhaseReactionState = 6;
			}
			//no break
		case 6:
		case 7:
			for (i = 0; i < 200; i++) {
				member = BattleAudienceGetPtr(i);
				if (BattleAudience_GetSysCtrl(i) && member->type == AUDIENCE_BULKY_BOBOMB &&
						member->status == 19) {
					break;
				}
			}
			if (i == 200) { //didn't meet that condition
				if (audience->mCheckPhaseReactionState == 6) {
					audience->mCheckPhaseReactionState = 8;
				}
				else {
					audience->mCheckPhaseReactionState = 13;
				}
			}
			return TRUE;
		case 8:
			//audience->evt = evtEntry(&msg_heavy_bomb_fire, 0, 0x20);
			audience->mCheckPhaseReactionState = 9;
			return TRUE;
		case 9:
			if (!evtCheckID(audience->event->eventId)) {
				audience->event = NULL;
				audience->mCheckPhaseReactionState = 10;
			}
			return TRUE;
		case 10:
			if (audience->flags & 0x8000) {
				//audience->evt = evtEntry(&msg_puni_all_escape, 0, 0x20);
				audience->flags &= ~0x8000;
				audience->mCheckPhaseReactionState = 11;
			}
			else {
				audience->mCheckPhaseReactionState = 15;
			}
			return TRUE;
		case 11:
			if (!evtCheckID(audience->event->eventId)) {
				audience->event = NULL;
				audience->mCheckPhaseReactionState = 15;
			}
			return TRUE;












	}




	return FALSE; //TODO: finish, a lot
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

// NONMATCHING (0.15%): 658 missing.
void BattleAudienceSettingAudience(void) {
	//TODO: very long and terrible
}

// needs dont_inline: BattleAudience_Init must emit a call to this function, not inline it
#pragma dont_inline on
void BattleAudienceGuestTPLRead(s32 id, AudienceMemberType member, const char* path) {
	char full_path[256];

	BattleAudience* audience = BattleAudienceBaseGetPtr();
	if (id < 2) {
		sprintf(full_path, str_802F9DD4, path);
		audience->guestTex[id] = tplRead(full_path);
		audience->guestType[id] = member;
	}
}
#pragma dont_inline reset

// NONMATCHING (0.43%): 233 missing.
void BattleAudienceCtrlProcess(void) {
	//TODO: very long and terrible
}

#pragma dont_inline on
// NONMATCHING (1.10%): 1 instruction differs, 125 missing.
BOOL BattleAudienceItemOn(s32 index, u16 itemId, s32 numItems) {
	return FALSE;
}

void BattleAudienceSetThrowItemMax(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 r = irand(100);
	if (r < 0x50) {
		*(s32*)((u8*)audience + 0x137F4) = 0;
	}
	else if (r < 0x5a) {
		*(s32*)((u8*)audience + 0x137F4) = 1;
	}
	else if (r < 0x5f) {
		*(s32*)((u8*)audience + 0x137F4) = 2;
	}
	else if (r < 0x64) {
		*(s32*)((u8*)audience + 0x137F4) = 3;
	}
	*(s32*)((u8*)audience + 0x137F0) = 0;
	*(s32*)((u8*)audience + 0x137F8) = 0;
}

// NONMATCHING (2.59%): 1 instruction differs, 52 missing.
BattleUnit* BattleAudienceDetectTargetAll(void) {
	return NULL;
}

// NONMATCHING (2.69%): 1 instruction differs, 50 missing.
BattleUnit* BattleAudienceDetectTargetPlayer(void) {
	return NULL;
}

// NONMATCHING (0.17%): 597 missing.
void BattleAudienceItemCtrlProcess(void) {

}

// NONMATCHING (0.29%): 348 missing.
void BattleAudienceApSrcCtrlProcess(void) {

}

// NONMATCHING (0.97%): 1 instruction differs, 143 missing.
BOOL BattleAudienceApSrcEntry(void) {
	return FALSE;
}

// NONMATCHING (0.83%): 120 missing.
void BattleAudienceWinCtrlProcess(void) {

}

// NONMATCHING (1.11%): 89 missing.
void BattleAudienceGXInit(void) {

}

// NONMATCHING (0.83%): 120 missing.
void BattleAudienceAnimProcess(void) {

}

// NONMATCHING (0.17%): 586 missing.
void BattleAudienceDispAudience(CameraId camId, void* param) {

}

// NONMATCHING (1.52%): 65 missing.
void BattleAudienceDispItem(CameraId camId, void* param) {

}

// NONMATCHING (1.35%): 73 missing.
void BattleAudienceDispApSrc(CameraId camId, void* param) {

}

// NONMATCHING (1.61%): 61 missing.
void BattleAudienceDispWin(CameraId camId, void* param) {

}

// NONMATCHING (1.21%): 1 instruction differs, 114 missing.
BOOL BattleAudience_Entry(s32 index, AudienceMemberType type, u8 status) {
	return FALSE;
}

// NONMATCHING (0.67%): 149 missing.
void BattleAudience_Entry_Sub(s32 index, u8 type, u8 status) {

}
#pragma dont_inline reset

void BattleAudience_Delete(s32 index) {
	BattleAudienceMember* member;
	BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(index);
	if (member->flags & 0x80) {
		*(s32*)((u8*)member + 0x134) = 0;
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
	if (*(s32**)((u8*)member + 0x24) != NULL && *(s32**)((u8*)member + 0x24) == anim && !force) {
		return;
	}
	*(s32**)((u8*)member + 0x24) = anim;
	*(s32**)((u8*)member + 0x28) = anim;
	*(s32*)((u8*)member + 0x2c) = 0;
	for (i = 0; i < 3; i++) {
		*(s32*)((u8*)member + 0x34 + i * 4) = 0;
	}
	*(u8*)((u8*)member + 0x40) = 0;
}

BOOL BattleAudience_GetAnimEnd(s32 index) {
	BattleAudienceMember* member = BattleAudienceGetPtr(index);
	if (**(s32**)((u8*)member + 0x28) != -1) {
		return FALSE;
	}
	return *(s32*)((u8*)member + 0x2c) <= 0;
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
	return !(dx * *(f32*)((u8*)member + 0x90) +
	         dy * *(f32*)((u8*)member + 0x94) +
	         dz * *(f32*)((u8*)member + 0x98) >=
	         float_0_8041e618);
}

#pragma dont_inline on
// NONMATCHING (1.54%): 64 missing.
void BattleAudience_ChangeStatus(s32 index, s32 status) {

}
#pragma dont_inline reset
//breaks -inline auto,deferred
/*
BOOL BattleAudience_GetExist(s32 id) {
	return FALSE;
}
*/

#pragma dont_inline on
BOOL BattleAudience_GetExist(s32 id) {
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

BOOL BattleAudience_GetWaiting(s32 index) {
	BattleAudienceMember* member;
	BattleAudienceBaseGetPtr();
	member = BattleAudienceGetPtr(index);
	if ((u8)BattleAudience_GetSysCtrl(index) == 0) {
		return FALSE;
	}
	if (member->itemId != -1) {
		return FALSE;
	}
	return (u8)(member->status <= 3);
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
#pragma dont_inline reset

#pragma dont_inline on
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
#pragma dont_inline reset

#pragma dont_inline on
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

// NONMATCHING (96.97%): same instructions, different callee-saved register numbers (r22-r31).
s32 BattleAudience_NoUsedHaitiRand(void) {
	extern s32 haiti_data[][10];
	s32 count, i, j, start, memberId;
	s32 stack[200];
	s32* haiti;

	BattleAudienceBaseGetPtr();
	haiti = haiti_data[pouchGetPtr()->rank];
	count = 0;
	for (i = 0; i < 10; i++) {
		start = (20 - haiti[i]) / 2;
		for (j = 0; j < haiti[i]; j++) {
			memberId = start + i * 20;
			if (!(BattleAudienceGetPtr(memberId)->flags & 1) &&
			    !(BattleAudienceGetPtr(memberId)->flags & 0x100)) {
				stack[count] = memberId;
				count++;
			}
			start++;
		}
	}
	if (count >= 1) {
		return stack[irand(count)];
	}
	return -1;
}
// NONMATCHING (1.09%): 1 instruction differs, 126 missing.
s32 BattleAudience_NoUsedFCHaitiRand(void) {
	return 0;
}
#pragma dont_inline reset

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

#pragma dont_inline on
s32 BattleAudience_GetPPAudienceNum_R(void) {
	return BattleAudience_GetPPAudienceNum_RL_Sub(10);
}

s32 BattleAudience_GetPPAudienceNum_L(void) {
	return BattleAudience_GetPPAudienceNum_RL_Sub(0);
}

// NONMATCHING (99.48%): register allocation and add-operand order of the index differ. MWCC emits
// `add r29,r31` (j+akk) then `add r27,r3` (offset+...) where the original emits `add r31,r27`
// (akk+offset) then `add r29,r3` (j+...). Everything else (saved regs, loop bodies, compare) matches.
s32 BattleAudience_GetPPAudienceNum_RL_Sub(s32 offset) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 akk;
	s32 i, j;
	s32 count = 0;

	count = 0;
	for (i = 0, akk = 0; i < 10; i++) {
		for (j = 0; j < 10; j++) {
			if (BattleAudience_GetPPAudienceNum_Sub(akk + (offset + j)) == 1) {
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
#pragma dont_inline reset































































//1:1
// keep AddTargetNumSub out of AddTargetNum (original has a `bl`)
#pragma dont_inline on
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
#pragma dont_inline reset














// NONMATCHING (0.82%): 121 missing.
void BattleAudienceSoundMain(void) {

}


// NONMATCHING (1.12%): 88 missing.
void BattleAudienceSoundNoiseAlways(void) {

}

#pragma dont_inline on
void BattleAudienceSoundSetVol(s32 index, u8 a2, s32 a3) {
	BattleAudienceSound* sound;

	sound = BattleAudienceSoundGetPtr(index);
	sound->mFadeStartVolumeMultiplier = sound->mVolumeMultiplier;
	sound->mFadeEndVolumeMultiplier = a2;
	sound->mForceFadeTimer = 0;
	sound->mForceFadeLength = a3;
	sound->mFadeoutTime = 0;
}
#pragma dont_inline reset

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

// Keep BattleAudience_WinSetActive out of line: the original calls it rather than inlining.
#pragma dont_inline on
void BattleAudience_WinSetActive(u8 active) {
	BattleAudienceWinGetPtr()->mActive = active;
}
#pragma dont_inline reset

void BattleAudience_Case_JumpNewRecord(void) {
}

void BattleAudience_Case_FinalAttack(void) {
}

void BattleAudience_Case_Escape(void) {
}

s32 BattleAudienceSound0(const char* name) {
	return psndSFXOn(name);
}

void BattleAudience_Case_AcrobatNoTry(void) {
	BattleAudienceBaseGetPtr();
}

void BattleAudience_Case_MarioBigDamage(void) {
	BattleAudienceBaseGetPtr();
}

void BattleAudienceJoyEnding(void) {
	BattleAudienceJoy(3);
}

BOOL BattleAudienceSoundCheck(s32 index) {
	return *(s32*)BattleAudienceSoundGetPtr(index) & 1;
}

s32 BattleAudience_GetPresentItemNo(void) {
	return *(s32*)((u8*)BattleAudienceBaseGetPtr() + 0x137E4);
}

s32 BattleAudience_GetPresentTargetUnitId(void) {
	return *(s32*)((u8*)BattleAudienceBaseGetPtr() + 0x137EC);
}

s32 BattleAudience_GetPresentItemType(void) {
	return *(s32*)((u8*)BattleAudienceBaseGetPtr() + 0x137E8);
}

void BattleAudience_SetPresentItemNo(s32 value) {
	*(s32*)((u8*)BattleAudienceBaseGetPtr() + 0x137E4) = value;
}

void BattleAudience_SetPresentTargetUnitId(s32 value) {
	*(s32*)((u8*)BattleAudienceBaseGetPtr() + 0x137EC) = value;
}

void BattleAudience_SetPresentItemType(s32 value) {
	*(s32*)((u8*)BattleAudienceBaseGetPtr() + 0x137E8) = value;
}

void BattleAudienceSoundWhistle(void) {
	BattleAudienceSoundWhistleKind(irand(3) + 1);
}

void BattleAudience_Case_KillEnemy(void) {
	BattleAudienceBaseGetPtr()->flags |= 0x1000;
}

void BattleAudience_Case_GuardGood(void) {
	BattleAudienceBaseGetPtr()->flags |= 0x40;
}

void BattleAudience_Case_GuardBad(void) {
	BattleAudienceBaseGetPtr()->flags |= 0x80;
}

void BattleAudience_Case_EnemyDamage(void) {
	BattleAudienceBaseGetPtr()->flags |= 0x400;
}

void BattleAudience_Case_EnemyNoDamage(void) {
	BattleAudienceBaseGetPtr()->flags |= 0x800;
}

void BattleAudienceJoySACLecture(void) {
	BattleAudienceSoundCheer(0x78, 0x3c);
	BattleAudienceJoy(2);
}

void BattleAudience_Case_MarioPinch(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = float_0p05_8041e624 * delta;
	BattleAudienceAddTargetNum(delta, float_0_8041e618);
}

void BattleAudience_Case_MarioDanger(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = float_0p15_8041e630 * delta;
	BattleAudienceAddTargetNum(delta, float_0_8041e618);
}

void BattleAudience_Case_PartyDown(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = float_neg0p25_8041e640 * delta;
	BattleAudienceAddTargetNum(delta, float_0_8041e618);
}

void BattleAudience_Case_HaikeiSet(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = float_0p05_8041e624 * delta;
	BattleAudienceAddTargetNum(delta, float_0_8041e618);
}

void BattleAudience_Case_GreatVictory(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = float_0p15_8041e630 * delta;
	BattleAudienceAddTargetNum(delta, float_0_8041e618);
}

void BattleAudience_Case_FallObject_Stage(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = float_0p05_8041e624 * delta;
	BattleAudienceAddTargetNum(delta, float_0_8041e618);
}

void BattleAudience_Case_FirstAttack_Good(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = float_0p02_8041e61c * delta;
	BattleAudienceAddTargetNum(delta, float_0_8041e618);
}

void BattleAudience_Case_FirstAttack_Bad(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = float_neg0p05_8041e614 * delta;
	BattleAudienceAddTargetNum(delta, float_0_8041e618);
}

void BattleAudience_Case_Escape_Bad(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = float_neg0p2_8041e638 * delta;
	BattleAudienceAddTargetNum(delta, float_0_8041e618);
	BattleAudienceSoundBooing();
}

void BattleAudience_Case_Countered(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = float_neg0p1_8041e634 * delta;
	BattleAudienceAddTargetNum(delta, float_0_8041e618);
	BattleAudienceAddPhaseEvtList(5);
}

void BattleAudience_Case_Escape_Good(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 delta = audience->mAudienceDeltaMultiplier;
	delta = float_neg0p1_8041e634 * delta;
	BattleAudienceAddTargetNum(delta, float_0_8041e618);
	BattleAudienceSoundSetVol(2, 100, 30);
	BattleAudienceSoundBooing();
}

void BattleAudience_Case_FastVictory(f32 value) {
	BattleAudienceAddTargetNum(
	    float_0p01_8041e628 * (float_5_8041e62c * value) * BattleAudienceBaseGetPtr()->mAudienceDeltaMultiplier,
	    float_0_8041e618);
}

#pragma dont_inline on
void BattleAudienceSoundGetInfo2(s32 index, Vec* pos) {
	BattleAudienceMember* member = BattleAudienceGetPtr(index);
	pos->x = member->position.x;
	pos->y = member->position.y;
	pos->z = member->position.z;
}

void BattleAudienceSoundGetInfo1(s32 a, s32 b, u8* c, Vec* d) {
	s32 sum = a + b;

	if (sum == 0) {
		*c = 0;
	}
	else {
		*c = (u8)(s32)(float_100_8041e64c + float_26_8041e650 * (f32)sum / float_200_8041e648);
	}
	if (b == 0 || sum == 0) {
		d->x = float_250_8041e654;
	}
	else {
		d->x = (f32)(u8)(s32)((f32)(a - b) * float_250_8041e654 / (f32)sum);
	}
	d->y = 0.0f;
	d->z = 0.0f;
}

void BattleAudienceSoundSetVolAll(u8 a2, s32 a3) {
	s32 i;
	for (i = 0; i < 24; i++) {
		BattleAudienceSoundSetVol(i, a2, a3);
	}
}

void BattleAudienceSoundStop(s32 index) {
	BattleAudienceSound* sound = BattleAudienceSoundGetPtr(index);
	if (*(s32*)sound & 1) {
		psndSFXOff(*(s32*)((u8*)sound + 4));
		*(s32*)sound &= ~1;
	}
}

s32 BattleAudienceSound2(const char* name, s32 index) {
	Vec pos;
	BattleAudienceSoundGetInfo2(index, &pos);
	return psndSFXOn_3D(name, &pos);
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


#define AUD_SOUND_HEAD(sound) \
	*(s32*)((u8*)(sound)) |= 1; \
	*(s32*)((u8*)(sound) + 0xC) = -1; \
	(sound)->mFadeoutTime = 0

#define AUD_SOUND_TAIL(sound, vol) \
	(sound)->mCurrentBaseSfxVolume = (vol); \
	(sound)->mBaseSfxVolume = (vol); \
	(sound)->mForceFadeLength = 0; \
	(sound)->mForceFadeTimer = 0; \
	(sound)->mVolumeMultiplier = 100

#define AUD_SOUND_ID(sound) (*(s32*)((u8*)(sound) + 4))

void BattleAudienceSoundZZZ(s32 index) {
	BattleAudienceSound* sound = BattleAudienceSoundGetPtr(0x10);
	s32 vol;
	BattleAudienceSoundStop(0x10);
	BattleAudienceSound2(str_802F9A9C, index);
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
	AUD_SOUND_ID(sound) = BattleAudienceSound2(str_802F9A80, index);
	if (AUD_SOUND_ID(sound) != -1) {
		AUD_SOUND_HEAD(sound);
		vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
		AUD_SOUND_TAIL(sound, vol);
	}
}

void BattleAudienceSoundPakkunEat(s32 index) {
	BattleAudienceSound* sound = BattleAudienceSoundGetPtr(0x17);
	s32 vol;
	BattleAudienceSoundStop(0x17);
	AUD_SOUND_ID(sound) = BattleAudienceSound2(str_802F9A10, index);
	if (AUD_SOUND_ID(sound) != -1) {
		AUD_SOUND_HEAD(sound);
		vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
		AUD_SOUND_TAIL(sound, vol);
	}
}

void BattleAudienceSoundRun(s32 kind) {
	BattleAudienceSound* sound = BattleAudienceSoundGetPtr(0xd);
	s32 vol;
	BattleAudienceSoundStop(0xd);
	switch (kind) {
		case 0:
			AUD_SOUND_ID(sound) = BattleAudienceSound0(str_802F9AEC);
			break;
		case 1:
			AUD_SOUND_ID(sound) = BattleAudienceSound0(str_802F9B00);
			break;
	}
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
	if (pos.x <= float_0_8041e618) {
		idx = 0x12;
	}
	else {
		idx = 0x13;
	}
	sound = BattleAudienceSoundGetPtr(idx);
	{
		s32 vol;
		BattleAudienceSoundStop(idx);
		AUD_SOUND_ID(sound) = BattleAudienceSound2(str_802F9A60, index);
		if (AUD_SOUND_ID(sound) != -1) {
			AUD_SOUND_HEAD(sound);
			vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
			AUD_SOUND_TAIL(sound, vol);
		}
	}
}

void BattleAudienceSoundBombFire(s32 index) {
	Vec pos;
	BattleAudienceSound* sound;
	s32 idx;
	BattleAudienceSoundGetInfo2(index, &pos);
	if (pos.x <= float_0_8041e618) {
		idx = 0x15;
	}
	else {
		idx = 0x16;
	}
	sound = BattleAudienceSoundGetPtr(idx);
	{
		s32 vol;
		BattleAudienceSoundStop(idx);
		AUD_SOUND_ID(sound) = BattleAudienceSound2(str_802F9A28, index);
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
	AUD_SOUND_ID(sound) = BattleAudienceSound2(str_802F9A44, index);
	if (AUD_SOUND_ID(sound) != -1) {
		*(s32*)((u8*)sound) |= 1;
		*(s32*)((u8*)sound + 0xC) = a + b;
		sound->mFadeoutTime = b;
		vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
		AUD_SOUND_TAIL(sound, vol);
	}
}

void BattleAudienceSoundItemThrow(s32 kind, s32 index) {
	Vec pos;
	BattleAudienceSound* sound;
	s32 idx;
	BattleAudienceSoundGetInfo2(index, &pos);
	if (pos.x <= float_0_8041e618) {
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
				AUD_SOUND_ID(sound) = BattleAudienceSound2(str_802F9B14, index);
				break;
			default:
				AUD_SOUND_ID(sound) = BattleAudienceSound2(str_802F9B30, index);
				break;
		}
		if (AUD_SOUND_ID(sound) != -1) {
			AUD_SOUND_HEAD(sound);
			vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
			AUD_SOUND_TAIL(sound, vol);
		}
	}
}

void BattleAudienceSoundSleep(s32 index) {
	Vec pos;
	BattleAudienceSound* sound;
	s32 idx;
	BattleAudienceSoundGetInfo2(index, &pos);
	if (pos.x <= float_0_8041e618) {
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
				AUD_SOUND_ID(sound) = BattleAudienceSound2(str_802F9AB0, index);
				break;
			case 1:
				AUD_SOUND_ID(sound) = BattleAudienceSound2(str_802F9AC4, index);
				break;
			case 2:
				AUD_SOUND_ID(sound) = BattleAudienceSound2(str_802F9AD8, index);
				break;
		}
		if (AUD_SOUND_ID(sound) != -1) {
			AUD_SOUND_HEAD(sound);
			vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
			AUD_SOUND_TAIL(sound, vol);
		}
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

void BattleAudience_Case_AcrobatBad(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 count = 0;
	s32 i;
	for (i = 0; i < 200; i++) {
		if ((u8)BattleAudience_GetWaiting(i) == 1) {
			count++;
		}
	}
	if (count >= 1) {
		BattleAudienceSoundBooing();
		BattleAudienceJoy(0);
	}
	audience->flags |= 0x200;
}

void BattleAudience_Case_AcrobatGood(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 count = 0;
	s32 i;
	for (i = 0; i < 200; i++) {
		if ((u8)BattleAudience_GetWaiting(i) == 1) {
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

void BattleAudience_Case_ActionCommandGood(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 i;
	s32 count = 0;
	for (i = 0; i < 200; i++) {
		if ((u8)BattleAudience_GetWaiting(i) == 1) {
			count++;
		}
	}
	if (count != 0) {
		BattleAudienceSoundCheer(0xb4, 0x78);
		BattleAudienceJoy(0);
	}
	audience->flags |= 0x10;
}

void BattleAudience_Case_ActionCommandBad(void* arg) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 count = 0;
	s32 i;
	for (i = 0; i < 200; i++) {
		if ((u8)BattleAudience_GetWaiting(i) == 1) {
			count++;
		}
	}
	if (count != 0) {
		BattleAudienceSoundBooing();
		BattleAudienceJoy(0);
		if (*(u32*)((u8*)arg + 0x1c) == (u32)weaponGetPowerFromMarioHammerLv) {
			BattleAudienceAddPhaseEvtList(7);
		}
	}
	audience->flags |= 0x20;
}

// NONMATCHING (91.10%): MWCC caches audience + 0x10000 in a third callee-saved register
// (frame 0x20); the original recomputes addis r3,r31,1 at each access and only saves r30/r31.
void BattleAudienceCheer(s32 a) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	if (*(s32*)((u8*)audience + 0x137C4) > 0x320) {
		*(s32*)((u8*)audience + 0x137C4) = 0x320;
	}
	if (*(s32*)((u8*)audience + 0x137C4) > 0) {
		if (pouchGetPtr()->maximumSP == 0) {
			*(s32*)((u8*)audience + 0x137C4) = 0;
		}
		else {
			audience->flags |= 2;
		}
		BattleAudienceJoy(a);
		if (*(s32*)((u8*)audience + 0x137C4) != 0) {
			BattleStatusWindowAPRecoveryOn();
		}
	}
}

void BattleAudiencePuniAllEscape(void) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	f32 one = float_1_8041e620;
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
		AUD_SOUND_ID(sound) = BattleAudienceSound1(str_802F9B64, r, l);
		break;
	case 2:
		AUD_SOUND_ID(sound) = BattleAudienceSound1(str_802F9B7C, r, l);
		break;
	}
	if (AUD_SOUND_ID(sound) != -1) {
		AUD_SOUND_HEAD(sound);
		vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
		AUD_SOUND_TAIL(sound, vol);
	}
}

// NONMATCHING (98.51%): logic matches; the vol=0/if(total!=0) region is laid out
// beq-tail + fall-through compute instead of the original bne-compute; b-tail; compute.
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
	AUD_SOUND_ID(sound) = BattleAudienceSound1(str_802F9B4C, r, l);
	if (AUD_SOUND_ID(sound) != -1) {
		*(s32*)((u8*)sound) |= 1;
		*(s32*)((u8*)sound + 0xC) = -1;
		sound->mFadeoutTime = 0;
		sound->mForceFadeLength = 0;
		sound->mForceFadeTimer = 0;
		vol = 0;
		if (total != 0) {
			vol = (s32)(float_75_8041e658 + ((f32)total * (float_26_8041e650 + (f32)irand(26))) / float_200_8041e648);
		}
		else {
			vol = 0;
		}		psndSFX_vol(AUD_SOUND_ID(sound), vol);
		sound->mCurrentBaseSfxVolume = vol;
		sound->mBaseSfxVolume = vol;
		sound->mVolumeMultiplier = 100;
	}
}

// NONMATCHING (69.52%): loop body matches (mtctr/bdnz); the prologue computes the biased
// base/count/i in a different order and the tail folds the final address differently.
void BattleAudienceAddPhaseEvtList(s32 evtId) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 i = 0;
	s32 count = *(s32*)((u8*)audience + 0x13904);
	for (; i < count; i++) {
		if (*(s32*)((u8*)audience + 0x138CC + i * 4) == evtId) {
			return;
		}
	}
	*(s32*)((u8*)audience + 0x138CC + count * 4) = evtId;
	*(s32*)((u8*)audience + 0x13904) = count + 1;
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

void BattleAudienceAddPuni(s32 num) {
	BattleWork* wp = _battleWorkPointer;
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 i;
	if (pouchGetPtr()->mStarPowersObtained != 0 && *(s32*)((u8*)audience + 0x137B0) > 0 &&
			num > 0 && !(wp->flags & 0x100)) {
		f32 one = float_1_8041e620;
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
		strcpy(name, str_802F9C9C);
		*(u8*)((u8*)sound + 8) = 1;
	}
	if (total >= 0x32 && total <= 0x63) {
		strcpy(name, str_802F9CB4);
		*(u8*)((u8*)sound + 8) = 2;
	}
	if (total >= 0x64 && total <= 0x95) {
		strcpy(name, str_802F9CCC);
		*(u8*)((u8*)sound + 8) = 3;
	}
	if (total >= 0x96) {
		strcpy(name, str_802F9CE4);
		*(u8*)((u8*)sound + 8) = 4;
	}
	AUD_SOUND_ID(sound) = BattleAudienceSound1(name, r, l);
	if (AUD_SOUND_ID(sound) != -1) {
		*(s32*)((u8*)sound) |= 1;
		*(s32*)((u8*)sound + 0xC) = a + b;
		sound->mFadeoutTime = b;
		vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
		AUD_SOUND_TAIL(sound, vol);
	}
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
		strcpy(name, str_802F9CFC);
		*(u8*)((u8*)sound + 8) = 1;
	}
	if (kind == 2) {
		strcpy(name, str_802F9D14);
		*(u8*)((u8*)sound + 8) = 2;
	}
	if (kind == 3) {
		strcpy(name, str_802F9D2C);
		*(u8*)((u8*)sound + 8) = 3;
	}
	if (kind == 4) {
		strcpy(name, str_802F9D44);
		*(u8*)((u8*)sound + 8) = 4;
	}
	if (kind == 5) {
		strcpy(name, str_802F9D5C);
		*(u8*)((u8*)sound + 8) = 5;
	}
	if (kind == 6) {
		strcpy(name, str_802F9D74);
		*(u8*)((u8*)sound + 8) = 6;
	}
	if (kind == 7) {
		strcpy(name, str_802F9D8C);
		*(u8*)((u8*)sound + 8) = 7;
	}
	if (kind == 8) {
		strcpy(name, str_802F9DA4);
		*(u8*)((u8*)sound + 8) = 8;
	}
	AUD_SOUND_ID(sound) = BattleAudienceSound1(name, r, l);
	if (AUD_SOUND_ID(sound) != -1) {
		AUD_SOUND_HEAD(sound);
		vol = psndSFX_get_vol(AUD_SOUND_ID(sound));
		AUD_SOUND_TAIL(sound, vol);
	}
}

// NONMATCHING (94.63%): uses r26-r31 (frame 0x20) where the original uses r25-r31 (frame 0x30);
// the compiler keeps one fewer value live across the BattleAudience_ChangeStatus calls.
void BattleAudienceJoy_Sub(s32 a, s32 b, s32 c, s32 d) {
	BattleAudience* audience = BattleAudienceBaseGetPtr();
	s32 sum = a + b + c;
	s32 i = 0;
	BattleAudienceMember* member = BattleAudienceGetPtr(0);
	s32 r;
	for (; i < 200; i++, member++) {
		if ((u8)BattleAudience_GetWaiting(i)) {
			if (member->type == AUDIENCE_PIRANHA_PLANT) {
				BattleAudience_ChangeStatus(i, 4);
				if (d == 3) {
					*(f32*)((u8*)member + 0xc) = float_3_8041e65c;
				}
				else {
					*(f32*)((u8*)member + 0xc) = float_2_8041e660;
				}
			}
			else {
				r = irand(sum);
				if (r < a) {
					BattleAudience_ChangeStatus(i, 4);
					if (d == 3) {
						*(f32*)((u8*)member + 0xc) = float_3_8041e65c;
					}
					else {
						*(f32*)((u8*)member + 0xc) = float_2_8041e660;
					}
				}
				else if (r < a + b) {
					BattleAudience_ChangeStatus(i, 5);
				}
				else if (r < a + b + c) {
					BattleAudience_ChangeStatus(i, 6);
				}
			}
		}
	}
}

// NONMATCHING (97.58%): 2 missing, plus register differences.
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
		switch (*(s32*)((u8*)audience + 0x1390c)) {
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
		(*(s32*)((u8*)audience + 0x1390c))++;
		if (*(s32*)((u8*)audience + 0x1390c) > 4) {
			*(s32*)((u8*)audience + 0x1390c) = 4;
		}
			break;
	}
}

#pragma dont_inline reset
