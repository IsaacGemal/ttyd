#include "manager/fontmgr.h"
#include "battle/battle_stage_object.h"
#include "battle/battle.h"
#include "system.h"
#include <string.h>
#include "manager/evtmgr_cmd.h"
#include "battle/battle_audience.h"
#include "battle/battle_seq.h"
#include "driver/mapdrv.h"
#include "battle/battle_sub.h"
#include "eff/n64/eff_kemuri1_n64.h"
#include "eff/eff_nozle.h"
#include "eff/n64/eff_confetti_n64.h"
#include "driver/msgdrv.h"
#include "pmario_sound.h"

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u32 lbl_80371490[2] = { 0xBF800000, 0x3F800000 };

extern BattleWork* _battleWorkPointer;

//local prototypes

// Data + helpers not declared in headers
extern s32 _fall_frame_event[];
extern s32 _object_fall_event[];
extern const char str_802F3C7C[];
extern u8 _nozzle_at_base_mario[];
extern u8 _nozzle_at_base_monster[];
extern char str_8041CD68[4];
extern char str_8041CD6C[4];
extern char str_8041CD70[4];
extern char str_8041CD74[4];
extern const u32 dat_8041cce8;
extern const u32 dat_8041ccec;
extern const u32 dat_8041ccf0;
extern const u32 dat_8041ccf4;
#ifdef MUST_MATCH
// Match-only: a non-const declaration stops mwcc hoisting the dat_8041ccfc load above the
// event->userdata[2] store in _fall_frame_delete (the symbol itself is const .sdata2).
extern u32 dat_8041ccfc;
#else
extern const u32 dat_8041ccfc;
#endif
extern char str_802F3CC4[];
extern char str_802F3CDC[];
extern char str_802F3CF4[];
extern char vec3_802f3ac8[];
extern const u32 dat_8041ccf8;
extern f64 double_to_int_802f3c30;
extern const f32 float_0_8041cd04;
extern f32 float_10_8041cd34;
extern f32 float_20_8041cd40;
extern f32 float_30_8041cd5c;
extern f32 float_neg117_8041cd58;
extern f32 float_neg120_8041cd60;
extern const char str_802F3C84[];
extern char str_802F3B88[];
extern char str_802F3B94[];
extern char str_802F3BA0[];
extern char str_802F3CA0[];
extern char str_802F3CAC[];
extern char str_802F3CB8[];
extern f32 float_45_8041cd2c;
extern const f32 float_neg80_8041cd3c;
extern f32 float_neg45_8041cd4c;
extern f32 float_90_8041cd08;
extern Vec vec3_802f3bc4;
extern char str_802F3D0C[];
extern char str_802F3D24[];
extern char str_802F3D3C[];

// LOCAL_PROTO(windowDispGX_Waku_col): match-only. The original passes the GXColor by value like every caller (a
// stack copy); this by-pointer declaration only steers register allocation (see the exceptions file).
extern void windowDispGX_Waku_col(s32 texMap, GXColor* color, f32 x, f32 y, f32 w, f32 h, f32 curve);

// .sdata data tables (defined here so references use the small-data area).
s32 _object_wall_data[6] = { 0, 0x00040005, 0xC32F0000, 0, 0, 0 };
s32 _object_spot_light_data[12] = { 0, 0x00060007, 0, 0x43700000, 0, 0, 0, 0x00060007, 0, 0, 0, 0 };
s32 _entry_tree_data[12] = { 0, 0x02000000, 0x04000005, 0xC47A0000, 0xC47A0000, 0xC47A0000, 0, 0, 0, 0, 0, 0 };
s32 _entry_switch_data[12] = { 0, 0x02000000, 0x04000005, 0xC47A0000, 0xC47A0000, 0xC47A0000, 0, 0, 0, 0, 0, 0 };
s32 _object_turn_event_A1[7] = { 0x00020032, 0xFE363C89, 0, 0x0001005E, 0, 2, 1 };
s32 _object_turn_event_A2[7] = { 0x00020032, 0xFE363C89, 1, 0x0001005E, 0, 2, 1 };
s32 _object_turn_event_B[7] = { 0x00020032, 0xFE363C89, 2, 0x0001005E, 0, 2, 1 };
s32 _nozzle_turn_event[4] = { 0x0001005B, 0, 2, 1 };
s32 _close_wall_event[6] = { 0x0001005B, 0, 0x0001005B, 0, 2, 1 };
s32 _nozzle_work_event_0[7] = { 0x00020032, 0xFE363C8A, 0, 0x0001005E, 0, 2, 1 };
s32 _nozzle_work_event_1[7] = { 0x00020032, 0xFE363C8A, 1, 0x0001005E, 0, 2, 1 };

s32 _bgset_audience_bgset_access(void) {
	BattleAudience_Case_HaikeiSet();
	return EVT_RETURN_DONE;
}

// Keep _rate_mix/_rate_mix_3 out of the inliner: the original emits a `bl` to each from _combine_nozzle_weapon.
#pragma dont_inline on
void _rate_mix(u8* a, u8* b, s8* c, u8* d) {
	if (*c > 0) {
		if (*c > (s8)*a) {
			*a = *c;
		}
		*b = *b + *d;
	}
}

void _rate_mix_3(u8* a, u8* b, u8* c, s8* d, u8* e, u8* f) {
	if (*d > 0) {
		if (*d > (s8)*a) {
			*a = *d;
		}
		*b = *b + *e;
		*c = *c + *f;
		if ((s8)*c == 0) {
			*a = 0;
			*b = 0;
		}
	}
}

void _combine_nozzle_weapon(s32 mode, void* dst_, void* src_) {
	s32 i;

	if (mode != 0) {
		*(BattleWeapon*)dst_ = *(BattleWeapon*)src_;
	} else {
		((BattleWeapon*)dst_)->baseFpCost += ((BattleWeapon*)src_)->baseFpCost;
		((BattleWeapon*)dst_)->baseSpCost += ((BattleWeapon*)src_)->baseSpCost;
		if (((BattleWeapon*)dst_)->hpDamageFn == NULL) {
			((BattleWeapon*)dst_)->hpDamageFn = ((BattleWeapon*)src_)->hpDamageFn;
		}
		for (i = 0; i < 8; i++) {
			((BattleWeapon*)dst_)->hpDamageWork[i] += ((BattleWeapon*)src_)->hpDamageWork[i];
		}
		((BattleWeapon*)dst_)->element = ((BattleWeapon*)src_)->element;
#ifdef MUST_MATCH
		// Status block: byte pointers so mwcc derives the +0x80 base once into r31 (dst) / r30 (src), matching
		// the original. With named fields (the natural form under #else) mwcc keeps the full struct base live
		// instead and re-adds 0x80 at every call, which no longer matches.
		{
			u8* d = (u8*)dst_ + 0x80;
			u8* s = (u8*)src_ + 0x80;
			_rate_mix(d + 0x00, d + 0x01, (s8*)(s + 0x00), s + 0x01);
			_rate_mix(d + 0x02, d + 0x03, (s8*)(s + 0x02), s + 0x03);
			_rate_mix(d + 0x04, d + 0x05, (s8*)(s + 0x04), s + 0x05);
			_rate_mix(d + 0x06, d + 0x07, (s8*)(s + 0x06), s + 0x08);
			_rate_mix(d + 0x09, d + 0x0A, (s8*)(s + 0x09), s + 0x0A);
			_rate_mix(d + 0x0B, d + 0x0C, (s8*)(s + 0x0B), s + 0x0C);
			_rate_mix(d + 0x0D, d + 0x0E, (s8*)(s + 0x0D), s + 0x0E);
			_rate_mix(d + 0x0F, d + 0x10, (s8*)(s + 0x0F), s + 0x10);
			_rate_mix(d + 0x11, d + 0x12, (s8*)(s + 0x11), s + 0x12);
			_rate_mix_3(d + 0x13, d + 0x14, d + 0x15, (s8*)(s + 0x13), s + 0x14, s + 0x15);
			_rate_mix_3(d + 0x19, d + 0x1A, d + 0x1B, (s8*)(s + 0x19), s + 0x1A, s + 0x1B);
			_rate_mix(d + 0x1C, d + 0x1D, (s8*)(s + 0x1C), s + 0x1D);
			d[0x1F] += s[0x1F];
			_rate_mix(d + 0x20, d + 0x21, (s8*)(s + 0x20), s + 0x21);
			_rate_mix(d + 0x22, d + 0x23, (s8*)(s + 0x22), s + 0x23);
			d[0x2A] += s[0x2A];
			d[0x2B] += s[0x2B];
			d[0x2C] += s[0x2C];
			d[0x2D] += s[0x2D];
		}
#else
		{
		BattleWeapon* d = (BattleWeapon*)dst_;
		BattleWeapon* s = (BattleWeapon*)src_;
		_rate_mix(&d->sleepChance, (u8*)&d->sleepTime, (s8*)&s->sleepChance, (u8*)&s->sleepTime);
		_rate_mix(&d->stopChance, (u8*)&d->stopTime, (s8*)&s->stopChance, (u8*)&s->stopTime);
		_rate_mix(&d->dizzyChance, (u8*)&d->dizzyTime, (s8*)&s->dizzyChance, (u8*)&s->dizzyTime);
		_rate_mix(&d->poisonChance, (u8*)&d->poisonTime, (s8*)&s->poisonChance, (u8*)&s->poisonStrength);
		_rate_mix(&d->confuseChance, (u8*)&d->confuseTime, (s8*)&s->confuseChance, (u8*)&s->confuseTime);
		_rate_mix(&d->electricChance, (u8*)&d->electricTime, (s8*)&s->electricChance, (u8*)&s->electricTime);
		_rate_mix(&d->dodgyChance, (u8*)&d->dodgyTime, (s8*)&s->dodgyChance, (u8*)&s->dodgyTime);
		_rate_mix(&d->burnChance, (u8*)&d->burnTime, (s8*)&s->burnChance, (u8*)&s->burnTime);
		_rate_mix(&d->freezeChance, (u8*)&d->freezeTime, (s8*)&s->freezeChance, (u8*)&s->freezeTime);
		_rate_mix_3(&d->sizeChangeChance, (u8*)&d->sizeChangeTime, (u8*)&d->sizeChangeStrength,
		            (s8*)&s->sizeChangeChance, (u8*)&s->sizeChangeTime, (u8*)&s->sizeChangeStrength);
		_rate_mix_3(&d->defChangeChance, (u8*)&d->defChangeTime, (u8*)&d->defChangeStrength,
		            (s8*)&s->defChangeChance, (u8*)&s->defChangeTime, (u8*)&s->defChangeStrength);
		_rate_mix(&d->allergicChance, (u8*)&d->allergicTime, (s8*)&s->allergicChance, (u8*)&s->allergicTime);
		d->chargeStrength += s->chargeStrength;
		_rate_mix(&d->fastChance, (u8*)&d->fastTime, (s8*)&s->fastChance, (u8*)&s->fastTime);
		_rate_mix(&d->slowChance, (u8*)&d->slowTime, (s8*)&s->slowChance, (u8*)&s->slowTime);
		d->hpRegenTime += s->hpRegenTime;
		d->hpRegenStrength += s->hpRegenStrength;
		d->fpRegenTime += s->fpRegenTime;
		d->fpRegenStrength += s->fpRegenStrength;
		}
#endif
	}
}
#pragma dont_inline reset

extern const f32 float_0_8041cd04;
extern f32 float_15_8041cd18;
extern f32 float_30_8041cd5c;
extern f32 float_neg45_8041cd4c;
extern f32 float_neg135_8041cd50;
extern f32 float_neg90_8041cd54;
extern f32 float_12_8041cd38;
extern f32 float_8_8041cd44;
extern f32 float_1_8041cd48;
extern char str_8041CD78[4];

// Keep _get_nozzle_dir out of line: _nozzle_work_effect's original emits a `bl` to it.
#pragma dont_inline on
f32 _get_nozzle_dir(s32 index) {
	u8* w = (u8*)_battleWorkPointer + 0x20000;
	s32 dir = w[index - 0x7D7C];

	switch (dir) {
		case 1:
			return float_neg45_8041cd4c;
		case 3:
			return float_neg135_8041cd50;
		default:
			return float_neg90_8041cd54;
	}
}
#pragma dont_inline reset

// One nozzle's per-frame setup. The original is fully unrolled for the three nozzles, so this macro is
// expanded three times with the nozzle's name/dir/sfx/effect constants (offsets from the base/event as the asm).
#define NOZZLE_WORK_BODY(IDX, NAMEOFF, DIROFF, SFXLV, EFFLV)                                                       \
	obj = BattleSearchObjectPtr(vec + (NAMEOFF));                                                                     \
	angle = _get_nozzle_dir(IDX);                                                                                     \
	if (*(u8*)(base + (DIROFF)) != 0) {                                                                               \
		if (sfx != NULL) {                                                                                               \
			(SFXLV) = psndSFXOn_3D(sfx, &obj->mPosition);                                                                   \
		} else {                                                                                                         \
			(SFXLV) = -1;                                                                                                   \
		}                                                                                                                \
		(EFFLV) =                                                                                                        \
		    (s32)effNozleEntry(index, 0x5A, obj->mPosition.x - float_10_8041cd34 * cosfd(angle),                         \
		                       float_12_8041cd38 + obj->mPosition.y,                                                     \
		                       float_10_8041cd34 * sinfd(angle) + obj->mPosition.z, float_neg80_8041cd3c,                \
		                       reviseAngle(float_90_8041cd08 + angle));                                                  \
	} else {                                                                                                          \
		if (sfx != NULL) {                                                                                               \
			(SFXLV) = psndSFXOn_3D(sfx, &obj->mPosition);                                                                   \
		} else {                                                                                                         \
			(SFXLV) = -1;                                                                                                   \
		}                                                                                                                \
		(EFFLV) =                                                                                                        \
		    (s32)effNozleEntry(index, 0x5A, obj->mPosition.x,                                                            \
		                       float_20_8041cd40 + obj->mPosition.y, float_8_8041cd44 + obj->mPosition.z,                \
		                       float_0_8041cd04, float_0_8041cd04);                                                      \
		if (val == 0) {                                                                                                  \
			effConfettiN64Entry(4, 0x5A, obj->mPosition.x, float_20_8041cd40 + obj->mPosition.y,                            \
			                    float_8_8041cd44 + obj->mPosition.z, float_1_8041cd48);                                     \
		}                                                                                                                \
	}

USER_FUNC(_nozzle_work_effect) {
	BattleWork* wp = _battleWorkPointer;
	s32* args = event->args;
	char* vec = vec3_802f3ac8;
	u8* base = (u8*)wp + 0x20000;
	BattleStageObject* obj;
	char* sfx;
	f32 angle;
	s32 index = 0;
	s32 val;
	s32 dst;
	s32 done;
	s32 type;

	val = evtGetValue(event, args[0]);
	dst = args[1];
	done = 0;

	if (isFirstCall) {
		if (BattleCheckConcluded(wp)) {
			evtSetValue(event, dst, 0);
			return 2;
		}
		event->userdata[0] = 0x5A;
		type = *(s8*)(base - 0x7D76);
		switch (type) {
			case 1:
				sfx = vec + 0x170;
				index = 0;
				break;
			case 2:
				sfx = NULL;
				index = 1;
				break;
			case 3:
				sfx = vec + 0x188;
				index = 2;
				break;
		}
		switch (type) {
			case 1:
			case 2:
			case 3:
				NOZZLE_WORK_BODY(0, 0xC0, -0x7D7C, *(s32*)((u8*)event + 0x8C), event->userdata[1])
				NOZZLE_WORK_BODY(1, 0xCC, -0x7D7B, event->field_0x90[0], event->userdata[2])
				NOZZLE_WORK_BODY(2, 0xD8, -0x7D7A, event->field_0x90[1], event->field_84)
				done = 1;
				break;
			default:
				if (val != 0 && *(u32*)(base - 0x7D60) == 0) {
					event->userdata[0] = 0x78;
					event->userdata[1] = 0;
					event->field_84 = 0;
					event->userdata[2] = 0;
					*(s32*)(base - 0x7D60) =
					    (s32)effNozleEntry(3, 0x3E8, float_0_8041cd04, float_0_8041cd04, float_0_8041cd04,
					                       float_0_8041cd04, float_0_8041cd04);
					event->field_84 = *(s32*)(base - 0x7D60);
					*(s32*)(base - 0x7D64) = 2;
					obj = BattleSearchObjectPtr(vec + 0xCC);
					*(s32*)((u8*)event + 0x8C) = psndSFXOn_3D(vec + 0x19C, &obj->mPosition);
					event->field_0x90[0] = -1;
					event->field_0x90[1] = -1;
					done = 1;
				}
				break;
		}
		if (!done) {
			evtSetValue(event, dst, 0);
			return 2;
		}
	}

	if (event->userdata[0] > 0) {
		event->userdata[0]--;
		if ((s32)event->userdata[0] <= 0) {
			if ((u32)*(s32*)((u8*)event + 0x8C) != 0xFFFFFFFFu) {
				psndSFXOff(*(s32*)((u8*)event + 0x8C));
			}
			if (event->field_0x90[0] != 0xFFFFFFFFu) {
				psndSFXOff(event->field_0x90[0]);
			}
			if (event->field_0x90[1] != 0xFFFFFFFFu) {
				psndSFXOff(event->field_0x90[1]);
			}
		}
		return 0;
	}
	evtSetValue(event, dst, 1);
	return 2;
}
#undef NOZZLE_WORK_BODY

BOOL BattleFogForceStop(void) {
	BattleWork* wp = _battleWorkPointer;

	if (*(s32*)((u8*)wp + 0x1829C) == 0) {
		return 0;
	}
	if (*(void**)((u8*)wp + 0x182A0) != NULL) {
		effSoftDelete(*(EffectEntry**)((u8*)wp + 0x182A0));
		*(void**)((u8*)wp + 0x182A0) = NULL;
	}
	return 1;
}

s32 BattleFogEndCheck(void) {
	BattleWork* wp = _battleWorkPointer;

	if (*(s32*)((u8*)wp + 0x1829C) == 0) {
		return 1;
	}
	*(s32*)((u8*)wp + 0x1829C) = *(s32*)((u8*)wp + 0x1829C) - 1;
	if (*(s32*)((u8*)wp + 0x1829C) > 0) {
		return 0;
	}
	if (*(void**)((u8*)wp + 0x182A0) != NULL) {
		effSoftDelete(*(EffectEntry**)((u8*)wp + 0x182A0));
		*(void**)((u8*)wp + 0x182A0) = NULL;
	}
	return 1;
}

void BattleStage_WallCloseCheck(void) {
	u8* w = (u8*)_battleWorkPointer + 0x20000;
	u8 flags = w[-0x7F24];

	if (!(flags & 0x10)) {
		w[-0x7F24] = flags | 0x10;
		*(s32*)(w - 0x7D5C) = evtEntry(_close_wall_event, 10, 0)->eventId;
	}
}

void BattleStage_IronFrameFallCheck(void) {
	u8* w = (u8*)_battleWorkPointer + 0x20000;
	u8 flags = w[-0x7F24];

	if (!(flags & 0x40) && !(flags & 0x20)) {
		w[-0x7F24] = flags | 0x20;
		*(s32*)(w - 0x7D58) = evtEntry(_fall_frame_event, 10, 0)->eventId;
	}
}

s32 _fall_frame_wait(void) {
	u8* w = (u8*)_battleWorkPointer + 0x20000;

	if (!BattleWaitAllActiveEvtEnd_NoBgSetEndWait(_battleWorkPointer)) {
		return 0;
	}
	if (*(s32*)(w - 0x7D68) != 0) {
		if (evtCheckID(*(s32*)(w - 0x7D68))) {
			return 0;
		}
		*(s32*)(w - 0x7D68) = 0;
	}
	return 1;
}

s32 _object_fall_wait(void) {
	u8* w = (u8*)_battleWorkPointer + 0x20000;

	if (!BattleWaitAllActiveEvtEnd_NoBgSetEndWait(_battleWorkPointer)) {
		return 0;
	}
	if (*(s32*)(w - 0x7D68) != 0) {
		if (evtCheckID(*(s32*)(w - 0x7D68))) {
			return 0;
		}
		*(s32*)(w - 0x7D68) = 0;
	}
	if (*(s32*)(w - 0x7D58) != 0) {
		if (evtCheckID(*(s32*)(w - 0x7D58))) {
			return 0;
		}
		*(s32*)(w - 0x7D58) = 0;
	}
	if (*(s32*)(w - 0x7F20) != 0) {
		if (evtCheckID(*(s32*)(w - 0x7F20))) {
			return 0;
		}
		*(s32*)(w - 0x7F20) = 0;
	}
	return 2;
}

s32 BattleStage_ObjectFallCheck(void) {
	BattleWork* wp = _battleWorkPointer;
	u8* w = (u8*)wp + 0x20000;
	EventEntry* evt;

	if (wp->flags & 0x1000) {
		return 0;
	}
	evt = evtEntry(_object_fall_event, 10, 0);
	*(s32*)(w - 0x7D54) = evt->eventId;
	return evt->eventId;
}

s32 _nozzle_work_wait(void) {
	u8* w = (u8*)_battleWorkPointer + 0x180C0;
	s32 i;

	if (!BattleWaitAllActiveEvtEnd_NoBgSetEndWait(_battleWorkPointer)) {
		return 0;
	}
	for (i = 0; i < 3; i++) {
		if (*(s32*)(w + 0x1CC + i * 4) != 0) {
			if (evtCheckID(*(s32*)(w + 0x1CC + i * 4))) {
				return 0;
			}
			*(s32*)(w + 0x1CC + i * 4) = 0;
		}
	}
	return 2;
}

#pragma dont_inline on
void _nozzle_type_init(void) {
	u8* w = (u8*)_battleWorkPointer + 0x20000;
	s8* type = (s8*)(w - 0x7D76);
	u8* data = *(u8**)(w - 0x7F0C);
	s8 i;
	s32 sum;

	if (*type == -1) {
		sum = 0;
		for (i = 0; i < 4; i++) {
			sum += data[i + 0xC];
		}
		sum = irand(sum);
		for (i = 0; i < 4; i++) {
			sum -= data[i + 0xC];
			if (sum < 0) {
				break;
			}
		}
		*type = i;
	}
}
#pragma dont_inline reset

void BattleStage_NozzleWorkCheck(s32 flag) {
	u8* w = (u8*)_battleWorkPointer + 0x20000;

	if (pouchGetPtr()->rank > 0) {
		_nozzle_type_init();
		if (flag != 0 || *(s8*)(w - 0x7D76) != 0) {
			if (*(s32*)(w - 0x7D68) != 0 && !evtCheckID(*(s32*)(w - 0x7D68))) {
				*(s32*)(w - 0x7D68) = 0;
			}
			if (*(s32*)(w - 0x7D68) == 0) {
				EventEntry* e;
				if (flag != 0) {
					e = evtEntry(_nozzle_work_event_1, 10, 0);
				} else {
					e = evtEntry(_nozzle_work_event_0, 10, 0);
				}
				*(s32*)(w - 0x7D68) = e->eventId;
			}
		}
	}
}

typedef struct BattleNozzleWork {
	u8 field_0[0x3C];
	s32 weaponAddr[3];
} BattleNozzleWork;

USER_FUNC(_nozzle_work_get_weapon_address) {
	BattleNozzleWork* w = (BattleNozzleWork*)((u8*)_battleWorkPointer + 0x20000 - 0x7F40);
	s32* args = event->args;
	s32 idx;

	idx = evtGetValue(event, *args);
	evtSetValue(event, args[1], w->weaponAddr[idx]);
	return EVT_RETURN_DONE;
}

void BattleStage_NozzleDirChangeCheck(void) {
	u8* w = (u8*)_battleWorkPointer + 0x20000;
	u8 i;

	if (*(u32**)(w - 0x7F0C) != NULL) {
		for (i = 0; i < 3; i++) {
			_Nozzle_Change_Check(i, (u8*)*(u32**)(w - 0x7F0C) + i * 4);
		}
	}
}

USER_FUNC(_announce_wall_message) {
	if (isFirstCall) {
		event->userdata[0] = 0x3C;
	}
	if (event->userdata[0] < 0) {
		return EVT_RETURN_DONE;
	}
	event->userdata[0]--;
	dispEntry(8, 900.0f, 1, _disp_escape_wall_message, NULL);
	return EVT_RETURN_BLOCK;
}

void _disp_escape_wall_message(CameraId cameraId, void* param) {
	const char* msg = msgSearch(str_802F3C84);
	s32 width = FontGetMessageWidth(msg);
	GXColor color = *(GXColor*)&dat_8041ccf8;
	f32 x = float_0_8041cd04 - (f32)(width / 2);

	windowDispGX_Waku_col(0, &color, x - float_20_8041cd40, float_neg117_8041cd58, (f32)(width + 0x28), float_30_8041cd5c, float_10_8041cd34);
	FontDrawStart();
	FontDrawString(msg, x, float_neg120_8041cd60);
}

USER_FUNC(_get_bgset_attack_data) {
	BattleWork* wp = _battleWorkPointer;
	s32* args = event->args;
	s32 kind = evtGetValue(event, *args);
	s32 dst = args[1];
	u8* r5;

	switch (kind) {
		case 0:
			r5 = *(u8**)((u8*)wp->fbatInfo->setup) + 0x10;
			break;
		case 2:
			r5 = *(u8**)((u8*)wp->fbatInfo->setup) + 0xD0;
			break;
		default:
			r5 = NULL;
			break;
	}
	if (r5 != NULL && *(u8**)(r5 + 0x1C) == NULL) {
		r5 = NULL;
	}
	evtSetValue(event, dst, (s32)r5);
	return EVT_RETURN_DONE;
}

void BattleStage_DestroyA2(void) {
	BattleWork* wp = _battleWorkPointer;
	void* evt = *(void**)(*(u8**)((u8*)wp->fbatInfo->setup) + 0x198);

	evtDeleteID(*(s32*)((u8*)wp + 0x20000 - 0x7F18));
	*(s32*)((u8*)wp + 0x20000 - 0x7F18) = 0;
	if (evt == NULL) {
		evt = _object_turn_event_A2;
	}
	evtEntry(evt, 10, 0);
}

// The original keeps (u8*)wp in its own register; without the `base` alias these two functions score 94.51%.
#ifndef MUST_MATCH
#define base ((u8*)wp)
#endif

void BattleStage_DestroyB(void) {
	BattleWork* wp = _battleWorkPointer;
#ifdef MUST_MATCH
	u8* base = (u8*)wp;
#endif
	void* evt = *(void**)(*(u8**)((u8*)wp->fbatInfo->setup) + 0x19C);

	evtDeleteID(*(s32*)(base + 0x20000 - 0x7F14));
	*(s32*)(base + 0x20000 - 0x7F14) = 0;
	if (*(s32*)(base + 0x20000 - 0x7F20) != 0 && evtCheckID(*(s32*)(base + 0x20000 - 0x7F20))) {
		evtDeleteID(*(s32*)(base + 0x20000 - 0x7F20));
	}
	if (evt == NULL) {
		evt = _object_turn_event_B;
	}
	{
		EventEntry* e = evtEntry(evt, 10, 0);
		if (e != NULL) {
			*(s32*)(base + 0x20000 - 0x7F20) = e->eventId;
		}
	}
}

void BattleStage_DestroyA1(void) {
	BattleWork* wp = _battleWorkPointer;
#ifdef MUST_MATCH
	u8* base = (u8*)wp;
#endif
	void* evt = *(void**)(*(u8**)((u8*)wp->fbatInfo->setup) + 0x194);

	evtDeleteID(*(s32*)(base + 0x20000 - 0x7F1C));
	*(s32*)(base + 0x20000 - 0x7F1C) = 0;
	if (*(s32*)(base + 0x20000 - 0x7F20) != 0 && evtCheckID(*(s32*)(base + 0x20000 - 0x7F20))) {
		evtDeleteID(*(s32*)(base + 0x20000 - 0x7F20));
	}
	if (evt == NULL) {
		evt = _object_turn_event_A1;
	}
	{
		EventEntry* e = evtEntry(evt, 10, 0);
		if (e != NULL) {
			*(s32*)(base + 0x20000 - 0x7F20) = e->eventId;
		}
	}
}

#ifndef MUST_MATCH
#undef base
#endif

USER_FUNC(_fall_frame_delete) {
	u8* w = (u8*)_battleWorkPointer + 0x20000;

	if (isFirstCall) {
		event->userdata[2] = 0xFF;
	}
	event->userdata[2] -= 5;
	{
		GXColor c;
		GXColor color;
		color = *(GXColor*)&dat_8041ccfc;
		color.a = event->userdata[2];
		c = color;
		mapGrpSetColor(str_802F3C7C, &c);
	}
	if (event->userdata[2] > 0) {
		return EVT_RETURN_BLOCK;
	}
	*(s32*)(w - 0x7D58) = 0;
	return EVT_RETURN_DONE;
}


// Keep _mobj_shake_main out of line: the original calls it rather than inlining.
#pragma dont_inline on
void _mobj_shake_main(BattleStageObject* obj) {
	obj->mShakePeriodPosition++;
	if (obj->mShakePeriodPosition >= obj->mShakePeriodLength) {
		obj->mShakePeriodPosition = 0;
	}
	obj->mShakeAngle = sinfd(obj->mShakePeriodPosition * 360 / obj->mShakePeriodLength) * 5.0f;
	obj->mRotation.x = obj->mShakeAngle;
}
#pragma dont_inline reset

USER_FUNC(_set_mobj_shake) {
	BattleWork* wp = _battleWorkPointer;
	u8* w = (u8*)wp + 0x20000;
	s32 layer = evtGetValue(event, *event->args);
	BattleStageObject* obj;
	s32 i;

	if (BattleWaitAllActiveEvtEnd_NoBgSetEndWait(wp)
	    && !(*(s32*)(w - 0x7D68) != 0 && evtCheckID(*(s32*)(w - 0x7D68)))
	    && !(*(s32*)(w - 0x7D58) != 0 && evtCheckID(*(s32*)(w - 0x7D58)))) {
		return EVT_RETURN_DONE;
	}
	switch (layer) {
		case 0:
		case 1:
			for (i = 0; i < 32; i++) {
				obj = &wp->stageObjects[i];
				if (obj->id > 0 && obj->data->mLayer == layer) {
					_mobj_shake_main(obj);
				}
			}
			break;
		case 2:
			break;
		default:
			break;
	}
	return EVT_RETURN_BLOCK;
}

// MATCHING. The one non-obvious requirement is the `intpl_sub` call: mwcc evaluates a callee's arguments in
// its DECLARED parameter order, and the header declares intpl_sub as (mode, currStep, steps, start, end). With
// that order it loads currStep/steps first and spills the int->float magic as a materialised address
// (`lis`+`addi`+`lfd`, 13 diffs). The original callee is ABI-identical but declared with the floats first
// (mode, start, end, currStep, steps) — the same shape as `intplGetValue` — so the floats are evaluated first
// and the magic folds (`lis`+`lfd @l(r4)`). Calling through the reordered prototype (a cast, so no local
// declaration is needed) reproduces it exactly.
USER_FUNC(_fall_frame) {
	s32 layer = event->args[0];
	BattleWork* wp = _battleWorkPointer;
	u8* w = (u8*)wp + 0x20000;
	BattleStageObject* obj = BattleSearchObjectPtr(str_802F3C7C);

	if (isFirstCall) {
		event->userdata[2] = 0;
		event->field_84 = (s32)obj->mPosition.y;
	}
	event->userdata[2]++;
#ifdef MUST_MATCH
	obj->mPosition.y = ((f32 (*)(s32, f32, f32, s32, s32))intpl_sub)(
	    2, (f32)(s32)event->field_84, 0.0f, event->userdata[2] * event->userdata[2] / 0x3C, 0x3C);
#else
	obj->mPosition.y = intpl_sub(2, event->userdata[2] * event->userdata[2] / 0x3C, 0x3C,
	                             (f32)(s32)event->field_84, 0.0f);
#endif
	if (event->userdata[2] < 0x3C) {
		return EVT_RETURN_BLOCK;
	}
	evtSetValue(event, layer, *(s32*)(w - 0x7F08));
	return EVT_RETURN_DONE;
}

USER_FUNC(_fall_frame_wait_unit_ready) {
	BattleWork* wp = _battleWorkPointer;
	BattleUnit* unit;
	s32 i;

	if (isFirstCall) {
		for (i = 0; i < 64; i++) {
			unit = BattleGetUnitPtr(wp, i);
			if (unit != NULL && (unit->flags & 0x80000000) == 0
			    && !BtlUnit_CheckStatus(unit, 0x1B)) {
				if (*(void**)((u8*)unit + 0x2BC) != NULL) {
					EventEntry* e = evtEntry(*(void**)((u8*)unit + 0x2BC), 10, 0);
					if (e != NULL) {
						e->unitId = unit->unitId;
					}
					*(s32*)((u8*)unit + 0x2C0) = e->eventId;
				}
			}
		}
	}
	for (i = 0; i < 64; i++) {
		unit = BattleGetUnitPtr(wp, i);
		if (unit != NULL && (unit->flags & 0x80000000) == 0) {
			if (*(s32*)((u8*)unit + 0x2C0) != 0 && !evtCheckID(*(s32*)((u8*)unit + 0x2C0))) {
				*(s32*)((u8*)unit + 0x2C0) = 0;
			}
			if (*(s32*)((u8*)unit + 0x2C0) != 0) {
				return EVT_RETURN_BLOCK;
			}
		}
	}
	return EVT_RETURN_DONE;
}

#pragma dont_inline on
// MATCHING. The outer `switch ((u8)index)` (rather than `switch (index)`) makes mwcc extend the
// parameter into r0 for the dispatch and re-extend it in each case body, leaving `index` live in
// r3; without the cast it caches the extension in r4. The three cases fall through 0 -> 2 -> 1 (no
// `return -1` after the inner switches), which is the original's control flow.
s32 _get_nozzle_attack_index(u8 index) {
	BattleWork* wp = _battleWorkPointer;
	u8* base = (u8*)wp + 0x20000 - 0x7F40;
	u8 type;

	switch ((u8)index) {
		case 0:
			type = base[index + 0x1C4];
			switch (type) {
				case 1:
					return -1;
				case 2:
					return 0;
				case 3:
					return 1;
			}
		case 2:
			type = base[index + 0x1C4];
			switch (type) {
				case 1:
					return 0;
				case 2:
					return 1;
				case 3:
					return -1;
			}
		case 1:
			type = base[index + 0x1C4];
			switch (type) {
				case 1:
					return 0;
				case 2:
					return -1;
				case 3:
					return 1;
			}
	}
	return -1;
}
#pragma dont_inline reset

s32 _nozzle_work_attack_set(void) {
	u8* base = (u8*)_battleWorkPointer + 0x20000 - 0x7F40;
	s32 i;

	*(s32*)(base + 0x3C) = 0;
	*(s32*)(base + 0x40) = 0;
	memset(base + 0x44, 0, 0xC0);
	memset(base + 0x104, 0, 0xC0);
	_combine_nozzle_weapon(1, base + 0x44, _nozzle_at_base_mario);
	_combine_nozzle_weapon(1, base + 0x104, _nozzle_at_base_monster);
	for (i = 0; i < 3; i++) {
		s32 idx = _get_nozzle_attack_index(i);
		if (idx >= 0) {
			s32* slot = (s32*)(base + 0x3C + idx * 4);
			if (*(u32*)slot == 0) {
				*slot = (s32)(base + 0x44 + idx * 0xC0);
			}
			_combine_nozzle_weapon(0, (void*)*slot,
			                       *(u8**)(base + 0x34) + (s8)*(base + 0x1CA) * 0xC0 + 0x10);
		}
	}
	return 2;
}

// MATCHING. Three non-obvious requirements: (1) the two float pool symbols (float_0_8041cd04,
// float_neg80_8041cd3c) must be declared `extern const f32`; as plain `extern f32` mwcc can't hoist the two
// `lfs` above the userdata[2] store and the first intpl_sub timer temp lands in r4 instead of r0+reload.
// (2) the two `switch`es read `base[unitId + 0x1C4]` / `base[unitId + 0x1C7]` through array syntax, which
// makes mwcc recompute `base+unitId` (the extra `add r3`) instead of reusing the previous pointer.
// (3) the final event-slot store uses `slot[unitId + 0x73] = 0` on a `u32* slot = (u32*)base` (with a separate
// `u8* p = &base[unitId]` for the byte copy); folding `slot += unitId` into the same statement gives the target
// register rotation (byte ptr r6, loaded value r5, base+unitId*4 r4) whereas every other form swaps r4/r5.
USER_FUNC(_nozzle_turn) {
	u8* base = (u8*)_battleWorkPointer + 0x180C0;
	s32 unitId = event->unitId;
	char* vec = vec3_802f3ac8;
	const char* name;
	f32 angle1;
	f32 angle2;

	if (isFirstCall) {
		u8* p = base;

		p += unitId;
		event->userdata[2] = 0;
		event->field_84 = 0;
		if (p[0x1C4] != 0) {
			event->userdata[2] = 0x1E;
		}
	}

	switch (unitId) {
		case 0:
			name = vec + 0xC0;
			break;
		case 1:
			name = vec + 0xCC;
			break;
		case 2:
			name = vec + 0xD8;
			break;
		default:
			return 2;
	}
	BattleSearchObjectPtr(name);

	if (event->userdata[2] < 0x1E) {
		event->userdata[2]++;
		angle1 = intpl_sub(2, event->userdata[2], 0x1E, float_0_8041cd04, float_neg80_8041cd3c);
	} else {
		angle1 = float_neg80_8041cd3c;
	}

	if ((s32)event->field_84 < 0x1E) {
		f32 oldAngle;
		f32 newAngle;

		event->field_84++;
		switch (base[unitId + 0x1C4]) {
			case 1:
				oldAngle = float_45_8041cd2c;
				break;
			case 2:
				oldAngle = float_0_8041cd04;
				break;
			case 3:
				oldAngle = float_neg45_8041cd4c;
				break;
			case 0:
			default:
				oldAngle = float_0_8041cd04;
				break;
		}
		switch (base[unitId + 0x1C7]) {
			case 1:
				newAngle = float_45_8041cd2c;
				break;
			case 2:
				newAngle = float_0_8041cd04;
				break;
			case 3:
				newAngle = float_neg45_8041cd4c;
				break;
			case 0:
			default:
				newAngle = float_0_8041cd04;
				break;
		}
		angle2 = intpl_sub(0xC, event->field_84, 0x1E, oldAngle, newAngle);
	}

	switch (unitId) {
		case 0:
			mapObjRotate(vec + 0x1D8, angle1, angle2, float_0_8041cd04);
			break;
		case 1:
			mapObjRotate(vec + 0x1E4, angle1, angle2, float_0_8041cd04);
			break;
		case 2:
			mapObjRotate(vec + 0x1F0, angle1, angle2, float_0_8041cd04);
			break;
	}

	if (event->userdata[2] >= 0x1E && (s32)event->field_84 >= 0x1E) {
		u32* slot = (u32*)base;
		u8* p = &base[unitId];

		p[0x1C4] = p[0x1C7];
		slot[unitId + 0x73] = 0;
		return 2;
	}
	return 0;
}

#pragma dont_inline on
// MATCHING. Three source shapes were needed: (1) `u8 idx = index` plus `s32 typeOffset = idx + 0x1C4`
// declared before `base` (the compiler then precomputes `base[typeOffset]` as `lbzx base,typeOffset`
// and hands base/typeOffset r28/r29); (2) the `idx` store uses array syntax `base[idx + 0x1C7]` (a pointer
// expression makes mwcc emit `stbx` with a re-extended index); (3) `slot[0x73]` (rather than
// `base+index*4+0x1CC`) keeps mwcc from folding 0x1CC into an `lwzx`.
void _Nozzle_Change_Check(u8 index, void* nozzle) {
	u8 idx = index;
	s32 typeOffset = idx + 0x1C4;
	u8* base = (u8*)_battleWorkPointer + 0x20000 - 0x7F40;
	u8* data = nozzle;
	s32 sum = 0;
	u8 i;
	s32 k;

	for (k = 0; k < 4; k++) {
		sum += *data;
		data++;
	}
	sum = irand(sum);
	data = nozzle;
	for (i = 0; i < 4; i++) {
		sum -= *data;
		if (sum < 0) {
			break;
		}
		data++;
	}
	if (i != base[typeOffset]) {
		s32* slot = (s32*)(base + idx * 4);
		if (slot[0x73] != 0 && !evtCheckID(slot[0x73])) {
			slot[0x73] = 0;
		}
		if (slot[0x73] == 0) {
			base[idx + 0x1C7] = i;
			{
				EventEntry* e = evtEntry(_nozzle_turn_event, 10, 0);
				slot[0x73] = e->eventId;
				if (e != NULL) {
					e->unitId = index;
				}
			}
		}
	}
}
#pragma dont_inline reset

USER_FUNC(_mobj_destroy) {
	BattleWork* wp = _battleWorkPointer;
	s32 layer = evtGetValue(event, *event->args);
	BattleStageObject* obj;
	BattleUnit* unit;
	s32 i;

	for (i = 0; i < 32; i++) {
		obj = &wp->stageObjects[i];
		if (obj->id > 0 && obj->data->mLayer == layer) {
			obj->mFlags |= 1;
			unit = BattleGetUnitPtr(wp, obj->mBattleUnitIdx);
			if (unit != NULL) {
				BattleUnitPart* p = unit->mParts;
				while (p != NULL) {
					p->attributes |= 0x10000;
					p = p->mNextPart;
				}
			}
		}
	}
	switch (layer) {
		case 0:
			*(u8*)((u8*)wp + 0x180DC) |= 0x2;
			break;
		case 1:
			*(u8*)((u8*)wp + 0x180DC) |= 0x4;
			break;
		case 2:
			*(u8*)((u8*)wp + 0x180DC) |= 0x8;
			break;
	}
	return EVT_RETURN_DONE;
}

#pragma dont_inline on
// Copy propagation off is required: it keeps the `vec3_802f3ac8` base in a callee-saved register loaded
// before the `evtGetValue` call and the three string offsets computed after it, as the target does.
#pragma opt_propagation off
USER_FUNC(_bgset_kemuri_effect) {
	s32* args = event->args;
	char* s0;
	char* s1;
	char* s2;
#ifdef MUST_MATCH
	char* vec = vec3_802f3ac8; // the original offsets the three strings from this .rodata base
#endif
	BattleWork* wp = _battleWorkPointer;
	s32 i;
	BattleStageObject* obj;
	f32 x, y, z;
	s32 layer = evtGetValue(event, *args);
#ifdef MUST_MATCH
	s0 = vec + 0x1FC; // str_802F3CC4
	s1 = vec + 0x214; // str_802F3CDC
	s2 = vec + 0x22C; // str_802F3CF4
#else
	s0 = str_802F3CC4;
	s1 = str_802F3CDC;
	s2 = str_802F3CF4;
#endif

	for (i = 0; i < 32; i++) {
		obj = &wp->stageObjects[i];
		if (obj->id > 0 && obj->data->mLayer == layer) {
			x = obj->mPosition.x;
			y = obj->mPosition.y;
			z = obj->mPosition.z;
			switch (layer) {
				case 0:
					z += float_30_8041cd5c;
					effKemuri1N64Entry(3, x, y, z, float_0_8041cd04);
					psndSFXOn_3D(s0, &obj->mPosition);
					break;
				case 1:
					z += float_15_8041cd18;
					effKemuri1N64Entry(0, x, y, z, float_0_8041cd04);
					psndSFXOn_3D(s1, &obj->mPosition);
					break;
				case 2:
					z += float_30_8041cd5c;
					effKemuri1N64Entry(3, x, y, z, float_0_8041cd04);
					effKemuri1N64Entry(4, x, y, z, float_0_8041cd04);
					psndSFXOn_3D(s2, &obj->mPosition);
					break;
			}
		}
	}
	return EVT_RETURN_DONE;
}
#pragma opt_propagation on

// The whole body, control flow, constants and every memory access match. What is left is MWCC's callee-saved
// register choice: the original keeps event=r29, wp=r28, base=r27, changed=r26; here the allocator gives
// event=r26, changed=r28, wp=r29 (base already r27), a 3-cycle that also rotates the loop counter/string
// temporaries. The int->float conversion constant is also materialised as a fresh `@N@` .rodata literal with its
// own base instead of `0x168(r30)` (the unit's rodata is provided out of band, so MWCC cannot fold it onto the
// vec3_802f3ac8 base; `_fall_frame` in this file has the same object-file artefact). Tried: every pragma in the
// file (opt_propagation/common_subs/dead_assignments/global_optimizer/peephole/scheduling/optimize_for_size),
// declaration order, local event copy, float temporaries; none moves event off r26. Also tried (this session):
// #pragma opt_lifetimes off (event->r25, changed->r27, base->r26) and global_optimizer off (event->r25), the
// other pragma on/off pairs, defining the double in-file (MWCC still emits its own `@N@` int->float magic),
// removing the `base`/`vec` variables, relocating the s0/s1 SFX temporaries, and s32/BOOL/u32 changed: none
// reproduces event=r29/wp=r28/changed=r26. `--final` PASSes with the file unchanged.
// NONMATCHING (97.01%): but for the event/wp/changed register cycle and the conversion constant base.
USER_FUNC(_set_mobj_rotate_x) {
	BattleStageObject* obj;
	s32 i;
	s32 changed = 0;
	u8* base;
	BattleWork* wp;
	u8* vec;
	s32 layer;

	wp = _battleWorkPointer;
	base = (u8*)_battleWorkPointer + 0x20000;
	vec = (u8*)vec3_802f3ac8;
	layer = evtGetValue(event, *event->args);

	if (isFirstCall) {
		switch (layer) {
			case 0:
			case 1:
				for (i = 0; i < 32; i++) {
					obj = &wp->stageObjects[i];
					if (obj->id > 0 && obj->data->mLayer == layer) {
						obj->mNumFramesToStartFalling = obj->data->mNumFramesToStartFalling;
						obj->mNumFramesFalling = 0;
					}
				}
				break;
			case 2:
				event->userdata[1] = 0;
				event->userdata[2] = 0x3C;
				event->field_84 = 0;
				break;
		}
	}

	switch (layer) {
		case 0:
		case 1: {
#ifdef MUST_MATCH
			// The original reaches the two SFX through the vec3_802f3ac8 base (addi r29,r30,0x244 etc.).
			u8* s0 = vec + 0x244; // str_802F3D0C
			u8* s1 = vec + 0x25C; // str_802F3D24
#else
			char* s0 = str_802F3D0C;
			char* s1 = str_802F3D24;
#endif

			for (i = 0; i < 32; i++) {
				obj = &wp->stageObjects[i];
				if (obj->id > 0 && obj->data->mLayer == layer) {
					if (obj->mNumFramesToStartFalling != 0) {
						obj->mNumFramesToStartFalling--;
						changed = 1;
						_mobj_shake_main(obj);
					} else {
						if (obj->mNumFramesFalling == 0) {
							if (layer == 0) {
								psndSFXOn_3D((char*)s0, &obj->mPosition);
							} else {
								psndSFXOn_3D((char*)s1, &obj->mPosition);
							}
						}
#ifdef MUST_MATCH
						// Named endpoints make MWCC hoist the two lfs into the target's slots.
						{
							f32 end = float_90_8041cd08;
							f32 start = obj->mShakeAngle;
							obj->mNumFramesFalling++;
							obj->mRotation.x = intpl_sub(2, obj->mNumFramesFalling, obj->data->mNumFramesToFall,
							                             start, end);
						}
#else
						obj->mNumFramesFalling++;
						obj->mRotation.x = intpl_sub(2, obj->mNumFramesFalling, obj->data->mNumFramesToFall,
						                             obj->mShakeAngle, float_90_8041cd08);
#endif
						if ((u32)obj->mNumFramesFalling < (u32)obj->data->mNumFramesToFall) {
							changed = 1;
						}
					}
				}
			}
			break;
		}
		case 2:
			if (event->userdata[1] != 0) {
				event->userdata[1]--;
				changed = 1;
			} else {
				if ((s32)event->field_84 == 0) {
#ifdef MUST_MATCH
					Vec v = *(Vec*)(vec + 0xFC); // vec3_802f3bc4
					psndSFXOn_3D((char*)(vec + 0x274), &v); // str_802F3D3C
#else
					Vec v = vec3_802f3bc4;
					psndSFXOn_3D(str_802F3D3C, &v);
#endif
				}
#ifdef MUST_MATCH
				{
					f32 lo = float_0_8041cd04;
					f32 hi = float_90_8041cd08;
					event->field_84++;
					*(f32*)(base - 0x7F3C) = (s32)intpl_sub(2, event->field_84, event->userdata[2], lo, hi);
				}
#else
				// 0x180C4 is inside BattleWork.field_180C0 (padding; no named field yet).
				event->field_84++;
				*(f32*)((u8*)wp + 0x180C4) = (s32)intpl_sub(2, event->field_84, event->userdata[2],
				                                             float_0_8041cd04, float_90_8041cd08);
#endif
				if ((s32)event->field_84 < (s32)event->userdata[2]) {
					changed = 1;
				}
			}
			break;
	}
	return changed ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

USER_FUNC(_mobj_set_alpha) {
	s32* args = event->args;
	s32 which = evtGetValue(event, args[0]);
	u8 alpha = evtGetValue(event, args[1]);

	switch (which) {
		case 0:
			if (mapGetMapObj(str_8041CD68) != NULL) {
				GXColor c;
				GXColor color;
				color = *(GXColor*)&dat_8041cce8;
				color.a = alpha;
				c = color;
				mapGrpSetColor(str_8041CD68, &c);
			}
			break;
		case 1:
			if (mapGetMapObj(str_8041CD6C) != NULL) {
				GXColor c;
				GXColor color;
				color = *(GXColor*)&dat_8041ccec;
				color.a = alpha;
				c = color;
				mapGrpSetColor(str_8041CD6C, &c);
			}
			break;
		case 2:
			if (mapGetMapObj(str_8041CD70) != NULL) {
				GXColor c;
				GXColor color;
				color = *(GXColor*)&dat_8041ccf0;
				color.a = alpha;
				c = color;
				mapGrpSetColor(str_8041CD70, &c);
			}
			break;
		case 3:
			if (mapGetMapObj(str_8041CD74) != NULL) {
				GXColor c;
				GXColor color;
				color = *(GXColor*)&dat_8041ccf4;
				color.a = alpha;
				c = color;
				mapGrpSetColor(str_8041CD74, &c);
			}
			break;
	}
	return EVT_RETURN_DONE;
}

BattleStageObject* BattleGetObjectPtr(s32 id) {
	BattleWork* wp = _battleWorkPointer;
	int i;

	for (i = 0; i < 32; i++) {
		BattleStageObject* obj = &wp->stageObjects[i];
		if (obj->id == id) {
			return obj;
		}
	}
	return NULL; //if none of the IDs match
}

BattleStageObject* BattleSearchObjectPtr(const char* name) {
	BattleWork* wp = _battleWorkPointer;
	int i;
	BattleStageObject* object;

	for (i = 0; i < 32; i++) {
		object = &wp->stageObjects[i];
		if (object->id && !strcmp(object->data->name, name)) {
			return object;
		}
	}
	return NULL; //if name not found
}

// NONMATCHING (0.18%): 564 missing.
void BattleStageObjectInit(void) {

}





USER_FUNC(_set_mobj_shake_init) {
	BattleWork* wp = _battleWorkPointer;
	int i, layer;
	BattleStageObject* obj;
	u8 position;

	//get the layer this object is on
	layer = evtGetValue(event, *event->args);

	for (i = 0; i < 32; i++) {
		obj = &wp->stageObjects[i];
		if (obj->id > 0 && obj->data->mLayer == layer) {
			obj->field_0x72 = 0;
			obj->mShakePeriodLength = (u8)(irand(10) + 60);
			if (irand(100) & 1) {
				position = 0;
			}
			else {
				position = (u8)(obj->mShakePeriodLength / 2);
			}
			obj->mShakePeriodPosition = position;
			obj->mShakeAngle = float_0_8041cd04;
		}
	}
	return EVT_RETURN_DONE;
}







void BattleObjectConfig(void) {
	//stubbed in retail
}
#pragma dont_inline reset

// BattleStageObjectMain: per-object transform bookkeeping.
void BattleStageObjectMain(void) {
	BattleWork* wp = _battleWorkPointer;
	u8* base = (u8*)wp + 0x20000;
	BattleStageObject* obj;
	s32 i;
	s32 changed;

	for (i = 0; i < 32; i++) {
		obj = &wp->stageObjects[i];
		if (obj->id <= 0) {
			continue;
		}

		changed = 0;
		if (obj->mPosition.x != obj->field_0x34.x || obj->mPosition.y != obj->field_0x34.y
		    || obj->mPosition.z != obj->field_0x34.z) {
			obj->field_0x34 = obj->mPosition;
			changed = 1;
		}
		if (obj->field_0x10.x != obj->field_0x40.x || obj->field_0x10.y != obj->field_0x40.y
		    || obj->field_0x10.z != obj->field_0x40.z) {
			obj->field_0x40 = obj->field_0x10;
			changed = 1;
		}
		if (obj->field_0x28.x != obj->field_0x58.x || obj->field_0x28.y != obj->field_0x58.y
		    || obj->field_0x28.z != obj->field_0x58.z) {
			obj->field_0x58 = obj->field_0x28;
			changed = 1;
		}
		if (obj->mRotation.x != obj->field_0x4C.x || obj->mRotation.y != obj->field_0x4C.y
		    || obj->mRotation.z != obj->field_0x4C.z) {
			obj->field_0x4C = obj->mRotation;
			changed = 1;
		}

		if (obj->mFlags & 2) {
			obj->mFlags &= ~2;
			changed = 1;
		}

		switch (obj->data->mLayer) {
			case 2: {
				// 0x180C4/0x180D0 are inside BattleWork.field_180C0 (padding; no named fields yet).
				Vec* a = (Vec*)(base - 0x7F3C);
				Vec* b = (Vec*)(base - 0x7F30);
				if (a->x != b->x || a->y != b->y || a->z != b->z) {
					*b = *a;
					changed = 1;
				}
				break;
			}
		}

		switch (obj->data->mLayer) {
			case 0:
			case 1:
			case 4:
			case 5:
			case 6:
			case 7:
				if (changed) {
					mapObjTranslate(obj->data->name,
					                obj->mPosition.x + obj->field_0x10.x + obj->field_0x28.x,
					                obj->mPosition.y + obj->field_0x10.y + obj->field_0x28.y,
					                obj->mPosition.z + obj->field_0x10.z + obj->field_0x28.z);
					mapObjRotate(obj->data->name, obj->mRotation.x, obj->mRotation.y,
					             obj->mRotation.z);
				}
				break;
			case 2:
				if (changed) {
					mapObjTranslate(obj->data->name,
					                obj->mPosition.x + obj->field_0x10.x + obj->field_0x28.x,
					                obj->mPosition.y + obj->field_0x10.y + obj->field_0x28.y,
					                obj->mPosition.z + obj->field_0x10.z + obj->field_0x28.z);
					mapObjTranslate(obj->data->name, float_0_8041cd04, -obj->field_0x10.y, float_0_8041cd04);
					mapObjRotate(obj->data->name, ((Vec*)(base - 0x7F3C))->x, ((Vec*)(base - 0x7F3C))->y,
					             ((Vec*)(base - 0x7F3C))->z);
					mapObjTranslate(obj->data->name, float_0_8041cd04, obj->field_0x10.y, float_0_8041cd04);
				}
				break;
			case 3:
				if (changed) {
					mapObjTranslate(obj->data->name,
					                obj->mPosition.x + obj->field_0x10.x + obj->field_0x28.x,
					                obj->mPosition.y + obj->field_0x10.y + obj->field_0x28.y,
					                obj->mPosition.z + obj->field_0x10.z + obj->field_0x28.z);
					mapObjTranslate(obj->data->name, float_0_8041cd04, -obj->mPosition.y, float_0_8041cd04);
					mapObjRotate(obj->data->name, obj->mRotation.x, obj->mRotation.y,
					             obj->mRotation.z);
					mapObjTranslate(obj->data->name, float_0_8041cd04, obj->mPosition.y, float_0_8041cd04);
				}
				break;
		}

		if (obj->mFlags & 1) {
			if (!(*(u32*)mapGetMapObj(obj->data->name)->field_0x0 & 1)) {
				mapGrpFlagOn(obj->data->name, 1);
			}
		} else {
			if (*(u32*)mapGetMapObj(obj->data->name)->field_0x0 & 1) {
				mapGrpFlagOff(obj->data->name, 1);
			}
		}
	}
	BattleObjectConfig();
}
