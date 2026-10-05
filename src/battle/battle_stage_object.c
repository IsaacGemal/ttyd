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
#include "eff/eff_fall.h"
#include "eff/eff_nice.h"
#include "battle/battle_unit_event.h"
#include "battle/battle_detect.h"
#include "eff/n64/eff_confetti_n64.h"
#include "driver/msgdrv.h"
#include "pmario_sound.h"
#include "battle/battle_weapon_power.h"
#include "battle/battle_event_cmd.h"
#include "unit/unit_object_tree.h"
#include "unit/unit_object_switch.h"
#include "battle/battle_camera.h"


extern BattleWork* _battleWorkPointer;

// Data objects referenced by the functions but defined at the end of the file.
extern s32 _close_wall_event[6];
extern s32 _nozzle_turn_event[4];
extern s32 _object_turn_event_A1[7];
extern s32 _object_turn_event_A2[7];
extern s32 _object_turn_event_B[7];
extern s32 _nozzle_work_event_0[7];
extern s32 _nozzle_work_event_1[7];
extern f32 float_5_8041cd10;
extern f32 float_900_8041cd64;
extern char str_802F3B88[];
extern char str_802F3B94[];
extern char str_802F3BA0[];
extern char str_802F3BAC[];
extern char str_802F3BB8[];
extern s32 battle_stage_nozzle_data[];
extern s32 battle_stage_fall_object_data[];
extern s32 _object_nozzle_data[];
extern s32 _object_wall_data[];
extern s32 _object_spot_light_data[];
extern s32 _entry_tree_data[];
extern s32 _entry_switch_data[];
extern BattleUnitKind unit_object_tree;
extern BattleUnitKind unit_object_switch;
#pragma pool_data on // the original addresses its .rodata through one base register

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
	obj->mShakeAngle = sinfd(obj->mShakePeriodPosition * 360 / obj->mShakePeriodLength) * float_5_8041cd10;
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
	    2, (f32)(s32)event->field_84, float_0_8041cd04, event->userdata[2] * event->userdata[2] / 0x3C, 0x3C);
#else
	obj->mPosition.y = intpl_sub(2, event->userdata[2] * event->userdata[2] / 0x3C, 0x3C,
	                             (f32)(s32)event->field_84, float_0_8041cd04);
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

// NONMATCHING (99.30%): only the int->float pool and two `lfs` slots remain. The body, control flow,
// constants and every memory access match; all callee-saved registers now match retail (event=r29,
// changed=r26, wp=r28, base=r27, vec=r30, layer=r31, i/obj in their retail homes) and the volatile
// r5/r6 pair matches.
// The four remaining instruction diffs are:
//  - the int->float bias: the compiler pools its own anonymous `@N@` .rodata literal and forms its
//    address with its own `lis` (`lis r3,@N@ha; lfd f1,@N@l(r3)`) instead of retail's fold onto the
//    vec3_802f3ac8 base (`lfd f1, 0x168(r30)`). MWCC 2.6 only folds that pooled literal when it lays
//    out the unit's .rodata itself (the original .o has no relocation for this load); with the data
//    supplied out of band it cannot, and a hand-rolled i2f loses the fused `fsubs`. Same limitation as
//    win_badge.c / win_log.c; the only fix is porting the unit's .rodata.
//  - the two float arguments of the layer-2 `intpl_sub` are scheduled in the opposite order
//    (`lfs f1,float_0; lfs f2,float_90`, ours reversed); every declaration/statement order tried keeps
//    the scheduler's choice.
// Tried and rejected (all measured with mwcc/objdiff): ~2,000 declaration-order permutations, all
// prologue assignment orders, local event copy, several intpl_sub prototype casts (the
// (mode,start,end,currStep,steps) cast in the layer-0 arm is the one that fixed that arm's lfs
// scheduling), manual i2f helpers, removing base/vec/wp, s0/s1 as two scalars, s32/BOOL/u32 changed,
// and i as s16/u16/u8/u64; 60+ pragmas (only opt_propagation off helps).
// opt_propagation off is required: with it on the vec `lis` temp and the `event->args` temp take the
// opposite volatile registers (r6/r5 instead of retail's r5/r6) for four extra diffs.
#pragma opt_propagation off
// NONMATCHING (99.30%): see the full note above.
USER_FUNC(_set_mobj_rotate_x) {
	u8* vec;
	s32 layer;
	u8* base;
	s32 i;
	BattleStageObject* obj;
	BattleWork* wp;
	s32* args;
	s32 changed = 0;

	vec = (u8*)vec3_802f3ac8;
	wp = _battleWorkPointer;
	base = (u8*)_battleWorkPointer + 0x20000;
	args = event->args;
	layer = evtGetValue(event, args[0]);

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
			// The original reaches the two SFX through the vec3_802f3ac8 base (addi r29,r30,0x244,
			// addi r30,r30,0x25c). Two separate pointers compile to a 36-diff colouring; this array is
			// a match-only shape that reaches retail's event/changed/i registers.
			u8* sfx[2];
			sfx[0] = vec + 0x244;
			sfx[1] = vec + 0x25C;
#else
			char* sfx[2];
			sfx[0] = (char*)str_802F3D0C;
			sfx[1] = (char*)str_802F3D24;
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
								psndSFXOn_3D((char*)sfx[0], &obj->mPosition);
							} else {
								psndSFXOn_3D((char*)sfx[1], &obj->mPosition);
							}
						}
#ifdef MUST_MATCH
						// Named endpoints make MWCC hoist the two lfs into the target's slots; the cast
						// (ABI-identical) declares the floats before the int steps so they are evaluated
						// first, as retail does.
						{
							f32 end = float_90_8041cd08;
							f32 start;
							obj->mNumFramesFalling++;
							start = obj->mShakeAngle;
							obj->mRotation.x = ((f32 (*)(s32, f32, f32, s32, s32))intpl_sub)(2, start, end,
							                                                                 obj->mNumFramesFalling,
							                                                                 obj->data->mNumFramesToFall);
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
#pragma opt_propagation reset

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

// BattleWork fields past stageObjects (inside field_180C0, no names yet) and the stage data's tail.
#define BSO_WP_U8(wp, off) (*(u8*)((u8*)(wp) + (off)))
#define BSO_WP_S8(wp, off) (*(s8*)((u8*)(wp) + (off)))
#define BSO_WP_S32(wp, off) (*(s32*)((u8*)(wp) + (off)))
#define BSO_WP_PTR(wp, off) (*(void**)((u8*)(wp) + (off)))
#define BSO_STAGE_EVT(stage, off) (*(void**)((u8*)(stage) + (off)))
#define BSO_STAGE_S8(stage, off) (*(s8*)((u8*)(stage) + (off)))

#ifdef MUST_MATCH
// The unit's data still defines named copies of objects the original pools from literals: the 16
// (Vec){999, 999, 999} "unset" templates from vec3_802f3ac8 on, and these strings. Read those until the
// unit's pool is ported, so the literals don't add second copies.
#define BSO_UNSET(n) (*(Vec*)(rodata + (n) * 0xC))
#define BSO_STR_DEN str_8041CD78
#define BSO_STR_A1 str_8041CD68
#define BSO_STR_A2 str_8041CD6C
#define BSO_STR_B str_8041CD70
#define BSO_STR_C str_8041CD74
#define BSO_STR_ESCAPE_WALL (rodata + 0xE4) // str_802F3BAC
#define BSO_STR_IF_BODY (rodata + 0x1B4) // str_802F3C7C
#define BSO_STR_IF_WIRE (rodata + 0x28C) // str_802F3D54
#else
#define BSO_UNSET(n) ((Vec){999.0f, 999.0f, 999.0f})
#define BSO_STR_DEN "den"
#define BSO_STR_A1 "A1"
#define BSO_STR_A2 "A2"
#define BSO_STR_B "B"
#define BSO_STR_C "C"
#define BSO_STR_ESCAPE_WALL "escape_wall"
#define BSO_STR_IF_BODY "if_body"
#define BSO_STR_IF_WIRE "if_wire"
#endif

// NONMATCHING (98.29%): calls, branches and immediates match; the original computes wp + 0x20000 once into
// r20 for the BattleWork stores at 0x180F4-0x182A0, where this recomputes it per store.
void BattleStageObjectInit(void) {
	BattleUnit* unit;
	BattleStageData* stage;
	BattleStageObjectData* nozzle;
	s32 count;
	BattleStageObjectData* data;
	s32 id;
#ifdef MUST_MATCH
	char* rodata = vec3_802f3ac8;
#endif
	s32 layer0 = 0;
	s32 i;
	s32 layer1 = 0;
	BattleWork* wp;
	s32 layer2 = 0;
	s32 layer3 = 0;
	s32 k;
	BattleStageObject* obj;
	BattleStageObjectData* light;
	BOOL hasLayer6 = FALSE;

	wp = _battleWorkPointer;
	stage = *(BattleStageData**)wp->fbatInfo->setup;
	count = stage->mNumObjects;
	data = stage->mObjects;
	memset(wp->stageObjects, 0, sizeof(wp->stageObjects));

	switch (pouchGetPtr()->rank) {
	case 1:
	case 2:
	case 3:
		mapPlayAnimationLv(BSO_STR_DEN, 1, 0);
		break;
	}
	switch (pouchGetPtr()->rank) {
	case 1:
	case 2:
	case 3:
		BSO_WP_PTR(wp, 0x180F4) = (u8*)battle_stage_nozzle_data + pouchGetPtr()->rank * 0x310;
		break;
	default:
		BSO_WP_PTR(wp, 0x180F4) = NULL;
		break;
	}
	BSO_WP_PTR(wp, 0x180F8) = (u8*)battle_stage_fall_object_data + pouchGetPtr()->rank * 0x78C;
	BSO_WP_S8(wp, 0x1828A) = -1;
	BSO_WP_S32(wp, 0x1829C) = 0;
	BSO_WP_S32(wp, 0x182A0) = 0;

	mapGrpFlagOff(mapGetWork()->entries[0].rootMapObj->joints->name, 2);
	if (mapGetMapObj(BSO_STR_A1) != NULL) {
		mapGrpFlagOn(BSO_STR_A1, 1);
		mapGrpFlagOff(BSO_STR_A1, 2);
	}
	if (mapGetMapObj(BSO_STR_A2) != NULL) {
		mapGrpFlagOn(BSO_STR_A2, 1);
		mapGrpFlagOff(BSO_STR_A2, 2);
	}
	if (mapGetMapObj(BSO_STR_B) != NULL) {
		mapGrpFlagOn(BSO_STR_B, 1);
		mapGrpFlagOff(BSO_STR_B, 2);
	}
	if (mapGetMapObj(BSO_STR_C) != NULL) {
		mapGrpFlagOn(BSO_STR_C, 1);
		mapGrpFlagOff(BSO_STR_C, 2);
	}
	mapGrpFlagOn(BSO_STR_ESCAPE_WALL, 1);

	// the stage's own objects
	id = 1;
	if (data != NULL) {
		for (; count != 0; id++, count--, data++) {
			for (i = 0; i < 32; i++) {
				obj = &wp->stageObjects[i];
				if (obj->id <= 0) {
					break;
				}
			}
			if (i >= 32) {
				break;
			}
			obj->data = data;
			obj->id = id;
			obj->mBattleUnitIdx = -1;
			switch (obj->data->mLayer) {
			case 0: layer0++; break;
			case 1: layer1++; break;
			case 2: layer2++; break;
			case 3: layer3++; break;
			case 6: hasLayer6 = TRUE; break;
			}
			switch (obj->data->mLayer) {
			case 0:
			case 1:
			case 6:
			case 7:
				obj->mPosition = data->mPosition;
				break;
			case 2:
			case 3:
				obj->mPosition = data->mPosition;
				break;
			}
			obj->mRotation.x = obj->mRotation.y = obj->mRotation.z = 0.0f;
			obj->field_0x34 = BSO_UNSET(0);
			obj->field_0x40 = BSO_UNSET(1);
			obj->field_0x4C = BSO_UNSET(2);
			obj->field_0x58 = BSO_UNSET(3);
			mapGrpFlagOff(obj->data->name, 1);
			switch (data->mSpawnSpecialUnitType) {
			case 0:
				unit = BtlUnit_Entry((BattleUnitSetup*)_entry_tree_data);
				*(s32*)((u8*)unit + 0xB2C) = obj->id;
				obj->mBattleUnitIdx = unit->unitId;
				BtlUnit_SetPos(unit, obj->mPosition.x, obj->mPosition.y, obj->mPosition.z);
				BtlUnit_SetHomePos(unit, obj->mPosition.x, obj->mPosition.y, obj->mPosition.z);
				break;
			case 1:
				unit = BtlUnit_Entry((BattleUnitSetup*)_entry_switch_data);
				*(s32*)((u8*)unit + 0xB2C) = obj->id;
				obj->mBattleUnitIdx = unit->unitId;
				BtlUnit_SetPos(unit, obj->mPosition.x, obj->mPosition.y, obj->mPosition.z);
				BtlUnit_SetHomePos(unit, obj->mPosition.x, obj->mPosition.y, obj->mPosition.z);
				break;
			case 2:
				break;
			}
		}
	}

	// the rank's three nozzles
	if (pouchGetPtr()->rank > 0) {
		nozzle = (BattleStageObjectData*)_object_nozzle_data;
		for (k = 0; k < 3; k++, id++, nozzle++) {
			for (i = 0; i < 32; i++) {
				obj = &wp->stageObjects[i];
				if (obj->id <= 0) {
					break;
				}
			}
			if (i >= 32) {
				break;
			}
			obj->data = nozzle;
			obj->id = id;
			obj->mBattleUnitIdx = -1;
			obj->mPosition = nozzle->mPosition;
			obj->mRotation.x = obj->mRotation.y = obj->mRotation.z = 0.0f;
			obj->field_0x34 = BSO_UNSET(4);
			obj->field_0x40 = BSO_UNSET(5);
			obj->field_0x4C = BSO_UNSET(6);
			obj->field_0x58 = BSO_UNSET(7);
			mapGrpFlagOff(obj->data->name, 1);
		}
	}

	// the escape wall, at the stage's wall height
	data = (BattleStageObjectData*)_object_wall_data;
	for (i = 0; i < 32; i++) {
		obj = &wp->stageObjects[i];
		if (obj->id <= 0) {
			break;
		}
	}
	obj->data = data;
	obj->id = id;
	obj->mBattleUnitIdx = -1;
	obj->mPosition = data->mPosition;
	obj->mPosition.y = *(f32*)((u8*)BattleStageGetPtr() + 0xB28);
	obj->mRotation.x = obj->mRotation.y = obj->mRotation.z = 0.0f;
	obj->field_0x34 = BSO_UNSET(8);
	obj->field_0x40 = BSO_UNSET(9);
	obj->field_0x4C = BSO_UNSET(10);
	obj->field_0x58 = BSO_UNSET(11);
	mapGrpFlagOn(obj->data->name, 1);
	obj->mFlags |= 1;
	id++;

	// the spotlights, up to the entry with no name
	for (k = 0;; id++, k++) {
		light = &((BattleStageObjectData*)_object_spot_light_data)[k];
		if (light->name == NULL) {
			break;
		}
		for (i = 0; i < 32; i++) {
			obj = &wp->stageObjects[i];
			if (obj->id <= 0) {
				break;
			}
		}
		obj->data = light;
		obj->id = id;
		obj->mBattleUnitIdx = -1;
		obj->mPosition = light->mPosition;
		obj->mRotation.x = obj->mRotation.y = obj->mRotation.z = 0.0f;
		obj->field_0x34 = BSO_UNSET(12);
		obj->field_0x40 = BSO_UNSET(13);
		obj->field_0x4C = BSO_UNSET(14);
		obj->field_0x58 = BSO_UNSET(15);
		mapGrpFlagOn(obj->data->name, 1);
		obj->mFlags |= 1;
	}

	// the stage's events; each layer with objects gets its own
	if (BSO_STAGE_EVT(stage, 0x190) != NULL) {
		evtEntry(BSO_STAGE_EVT(stage, 0x190), 10, 0);
	}
	if (layer0 != 0 && BSO_STAGE_EVT(stage, 0x1A0) != NULL) {
		BSO_WP_S32(wp, 0x180E4) = evtEntry(BSO_STAGE_EVT(stage, 0x1A0), 10, 0)->eventId;
	}
	if (layer1 != 0 && BSO_STAGE_EVT(stage, 0x1A4) != NULL) {
		BSO_WP_S32(wp, 0x180E8) = evtEntry(BSO_STAGE_EVT(stage, 0x1A4), 10, 0)->eventId;
	}
	if (layer2 != 0 && BSO_STAGE_EVT(stage, 0x1A8) != NULL) {
		BSO_WP_S32(wp, 0x180EC) = evtEntry(BSO_STAGE_EVT(stage, 0x1A8), 10, 0)->eventId;
	}
	if (layer3 != 0 && BSO_STAGE_EVT(stage, 0x1AC) != NULL) {
		BSO_WP_S32(wp, 0x180F0) = evtEntry(BSO_STAGE_EVT(stage, 0x1AC), 10, 0)->eventId;
	}
	if (layer0 == 0) {
		BSO_WP_U8(wp, 0x180DC) |= 2;
	}
	if (layer1 == 0) {
		BSO_WP_U8(wp, 0x180DC) |= 4;
	}
	if (layer2 == 0) {
		BSO_WP_U8(wp, 0x180DC) |= 8;
	}
	if (BSO_STAGE_S8(stage, 0x1B1)) {
		BSO_WP_U8(wp, 0x180DC) |= 2;
	}
	if (BSO_STAGE_S8(stage, 0x1B2)) {
		BSO_WP_U8(wp, 0x180DC) |= 4;
	}
	if (BSO_STAGE_S8(stage, 0x1B3)) {
		BSO_WP_U8(wp, 0x180DC) |= 8;
	}
	if (!hasLayer6) {
		BSO_WP_U8(wp, 0x180DC) |= 0x20;
		mapGrpFlagOn(BSO_STR_IF_BODY, 1);
		mapGrpFlagOn(BSO_STR_IF_WIRE, 1);
	}
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

// ---------------------------------------------------------------------------
// _object_fall_attack: spawns and updates the "falling object" hazard. Each
// spawned object gets a BattleWeapon copied from the battle's fall-object data
// table and either targets a unit (BattlePreCheckDamage/BattleCheckDamage) or is
// thrown by the audience. Data offsets used because the structs don't declare
// the fields are marked inline.
// ---------------------------------------------------------------------------

typedef struct FallObjectEntry {
	s8 flag;              // 0x00, 0 = active, -1 = done
	s8 type;              // 0x01, fall-object kind (indexes the data table)
	u8 pad_02[2];         // 0x02
	s32 mode;             // 0x04, 0 = stage object, 1 = audience thrown
	s32 unitId;           // 0x08
	s32 partId;           // 0x0C
	s16 timer;            // 0x10
	s16 counter;          // 0x12
	u8 flag14;            // 0x14, this object targeted a unit
	u8 flag15;            // 0x15
	u8 pad_16[6];         // 0x16
	s32 index;            // 0x1C, BattlePreCheckDamage result
	s32 sfxId;            // 0x20
	EffectEntry* effect;  // 0x24
	BattleWeapon weapon;  // 0x28
} FallObjectEntry;        // size 0xE8

extern f32 float_0p1_8041cd00;
extern f32 float_270_8041cd0c;
extern f32 float_5_8041cd10;
extern f32 float_neg300_8041cd14;
extern f32 float_300_8041cd1c;
extern f32 float_85_8041cd20;
extern f32 float_50_8041cd24;
extern f32 float_neg50_8041cd28;
extern f32 float_0p75_8041cd30;
extern Vec vec3_802f3bd0;

// Register allocation only. Two things got the int->float magic double onto the .rodata pool base
// (`lfd f1, 0x168(rN)`, matching the original): the three SFX names are string literals pooled with
// the double (a large enough literal pool makes MWCC open a base register), and `vec` was inlined to
// `(u8*)vec3_802f3ac8 + 0x108` at its single use so the compiler no longer materialises a second
// base pointer. What remains is register colouring: the original keeps one `.rodata` base in r30 and
// `event` in r15; MWCC loads the literal-pool base into r29 and `vec3_802f3ac8` into r30 (two bases
// for the same address), then colours the other callee-saved locals differently, so the frame is 16 B
// short (0x1670 vs 0x1660) and many operands are offset. The scalar declaration order below is a
// permutation-search artefact (the three SFX pointers are part of it) that maximises the match.
//
// Tried and rejected: renaming `gap_802f3ac8_rodata` to `vec3_802f3ac8` (or pointing the spawnPos
// access at the defined alias) makes the compiler CSE the two bases into r30, so the four `lfd` and
// the base instructions match, but the surrounding colouring re-schedules and the score falls to
// 96.37; the rename also drops `_set_mobj_rotate_x` to 97.96. Making the .rodata objects `const`,
// giving `vec3_802f3ac8` `Vec` type, and `PAD_STACK(16)` all hang MWCC. A `#pragma` sweep and larger
// declaration permutations were neutral. Merging the timer-loop `rand = (s16)(irand(0x16)+rand);
// rand += 0x14;` pair scored 95.36%.
// NONMATCHING (96.82%): see above.
USER_FUNC(_object_fall_attack) {
	// Scalar declarations: the order is not semantic. MWCC colours callee-saved registers roughly in
	// declaration order, so this sequence only decides which register each local lands in.
	s32 off;
	s32 outPartId;
	s32 idx;
	s32 flag;
	BattleWork* wp;
	f32 f31;
	s32 check;
	s32 dst;
	FallObjectEntry* entry;
	s32 rand;
	char* strTub;
	s32 outId;
	BattleUnit* unit;
	BattleUnitPart* part;
	char* strNormal;
	s32 sum;
	char* strRest;
	s32 m;
	s32 active;
	f32 f30;
	s32 flags;
	BattleWeapon* weapon;
	s8* alloc;
	s32 i;
	u8* typeTable;
	// Stack objects are declared so MWCC lays them out at the original offsets.
	u8 samplingA[0xACC];
	u8 samplingB[0xACC];
	Vec floorPos;
	Vec spawnPos;
	Vec hitPos;
	Vec pos;

	// `wp` is the BattleWork base; typeTable is its fall-object data table (weights at +0xC0, stats at +0xCC).
	wp = _battleWorkPointer;
	typeTable = *(u8**)((u8*)wp + 0x180F8);
	dst = *event->args;
	flag = 0;
	if (isFirstCall) {
		alloc = (s8*)BattleAlloc(0xEC);
		event->userdata[0] = (s32)alloc;
		event->userdata[1] = 0;
		memset(alloc, 0, 0xEC);
		alloc[4] = -1;
		for (idx = 0; idx < 3; idx++) {
			alloc[idx + 1] = -1;
		}
		if (BattleCheckConcluded(wp)) {
			evtSetValue(event, dst, event->userdata[1]);
			return EVT_RETURN_DONE;
		}
		alloc[0] = 1;
		i = 0;
		off = 0;
		while (i < (u8)alloc[0]) {
			entry = (FallObjectEntry*)(alloc + 4 + off);
			entry->flag = 0;
			for (idx = 0; idx < 3; idx++) {
				if ((s8)alloc[idx + 1] == -1) {
					break;
				}
			}
			if (idx >= 3) {
				entry->type = (s8)alloc[irand(3) + 1];
			} else {
				sum = 0;
				for (m = 0; m < 9; m++) {
					sum += typeTable[0xC0 + m];
				}
				sum = irand(sum);
				entry->type = 0;
				while (entry->type < 8) {
					sum -= typeTable[0xC0 + (s8)entry->type];
					if (sum < 0) {
						break;
					}
					entry->type++;
				}
				alloc[idx + 1] = entry->type;
			}
			{
				s32 t = entry->type;
				entry->weapon = *(BattleWeapon*)(typeTable + 0xCC + t * 0xC0);
			}
			if (entry->type == 6) {
				if (irand(100) < 0x32) {
					entry->weapon.targetClassFlags |= 0x100;
					entry->weapon.targetClassFlags &= ~0x200;
				} else {
					entry->weapon.targetClassFlags &= ~0x100;
					entry->weapon.targetClassFlags |= 0x200;
				}
			}
			weapon = &entry->weapon;
			entry->mode = 0;
			if (pouchGetPtr()->mStarPowersObtained != 0
			    && (BattleAudienceBaseGetPtr()->flags & 0x4000) == 0
			    && entry->type != 6 && entry->type != 7
			    && ((*(u32*)((u8*)wp + 0x19068) & 2) != 0 || irand(100) < 10)) {
				// BattleWork.field_19064 padding: a BattleWork flag word.
				entry->mode = 1;
				entry->partId = -1;
				entry->unitId = BattleAudience_HaitiRandForFallObject();
				flag = 1;
				event->userdata[1] = 1;
			}
			if (entry->mode == 0) {
				// BattleWork.alliances[1].attackDirection (0x12).
				BattleSamplingEnemy(samplingA, weapon, BattleGetSystemPtr(wp)->unitId, 1,
				                    weapon->targetClassFlags, weapon->targetPropertyFlags,
				                    (s8)-wp->alliances[1].attackDirection);
				if ((s8)samplingA[0xA6C] <= 0) {
					evtSetValue(event, dst, event->userdata[1]);
					return EVT_RETURN_DONE;
				}
				flag = 1;
				BattleChoiceSamplingEnemy(samplingA, weapon->targetWeightingFlags, &outId,
				                          &outPartId);
				entry->unitId = outId;
				entry->partId = outPartId;
				entry->flag14 = 1;
				for (m = i; m >= 0; m--) {
					FallObjectEntry* prev = (FallObjectEntry*)(alloc + 4 + m * 0xE8);
					if (prev != entry && prev->mode == 0 && prev->unitId == entry->unitId) {
						prev->flag14 = 0;
					}
				}
			}
			i++;
			off += 0xE8;
		}
		if (flag == 0) {
			evtSetValue(event, dst, event->userdata[1]);
			return EVT_RETURN_DONE;
		}
		for (i = 0; i < (u8)alloc[0]; i++) {
			entry = (FallObjectEntry*)(alloc + 4 + i * 0xE8);
			weapon = &entry->weapon;
			if (entry->mode != 0) {
				continue;
			}
			flags = 0;
			if (entry->flag14 != 0) {
				flags |= 0x100;
			}
			if (entry->flag15 != 0) {
				flags |= 0x100000;
			}
			part = BattleGetUnitPartsPtr(entry->unitId, entry->partId);
			unit = BattleGetUnitPtr(wp, entry->unitId);
			entry->index = BattlePreCheckDamage(BattleGetSystemPtr(wp), unit, part, weapon, flags);
			if (entry->index == 1 && entry->flag14 == 0) {
				rand = (s16)irand(0);
				for (m = 0; m < i; m++) {
					FallObjectEntry* prev = (FallObjectEntry*)(alloc + 4 + m * 0xE8);
					if (prev->unitId == entry->unitId) {
						prev->timer = (s16)rand;
						rand = (s16)(irand(0x16) + rand);
						rand += 0x14;
						if (prev->flag14 != 0) {
							prev->flag15 = 1;
						}
					}
				}
			}
		}
	}

	// The three SFX names are string literals; MWCC pools them with the other
	// rodata constants, which makes the int->float magic double ride the pool
	// base register as the original does.
	alloc = (s8*)event->userdata[0];
	strTub = "SFX_BTL_STAGE_DAMAGE_TUB1";
	strNormal = "SFX_BTL_STAGE_DAMAGE_NORMAL1";
	strRest = "SFX_REST_CONDITION1";
	active = 1;
	for (i = 0; i < (u8)alloc[0]; i++) {
		entry = (FallObjectEntry*)(alloc + 4 + i * 0xE8);
		if (entry->flag == -1) {
			continue;
		}
		check = 0;
		active = 0;
		if (entry->mode == 0) {
			unit = BattleGetUnitPtr(wp, entry->unitId);
			part = BattleGetUnitPartsPtr(entry->unitId, entry->partId);
		}
		if (entry->effect != NULL && entry->timer <= 0) {
			goto effect_active;
		}
		entry->timer--;
		if (entry->timer > 0) {
			continue;
		}
		switch (entry->mode) {
		case 0:
			BtlUnit_GetHitPos(unit, part, &spawnPos.x, &spawnPos.y, &spawnPos.z);
			f30 = (f32)BtlUnit_GetWidth(unit);
			break;
		case 1:
			spawnPos.x = (f32)((entry->unitId % 20) * 25 - 230);
			spawnPos.y = *(f32*)((u8*)BattleStageGetPtr() + 0xB34);
			spawnPos.z = float_0p1_8041cd00 * (f32)(entry->unitId % 20)
			             + (f32)((entry->unitId / 20) * 30 + 115);
			btl_camera_set_mode(0, 6);
			btl_camera_set_moveSpeedLv(0, 2);
			f30 = float_0_8041cd04;
			break;
		default:
#ifdef MUST_MATCH
			spawnPos = *(Vec*)(vec3_802f3ac8 + 0x108); // vec3_802f3bd0, reached through the rodata base
#else
			spawnPos = vec3_802f3bd0;
#endif
			f30 = float_0_8041cd04;
			break;
		}
		floorPos = spawnPos;
		floorPos.y = BattleGetFloorHeight(wp, floorPos.x, floorPos.y, floorPos.z);
		if (spawnPos.x >= float_0_8041cd04) {
			f31 = float_90_8041cd08;
		} else {
			f31 = float_270_8041cd0c;
		}
		switch (entry->index) {
		case 2:
		case 3:
		case 4:
		case 6:
			spawnPos.y = floorPos.y;
			break;
		}
		entry->sfxId = -1;
		entry->counter = 0;
		switch (entry->type) {
		case 0:
			entry->effect = effFallEntry(4, floorPos.x, floorPos.y, 1, floorPos.z, 0x5A, f30,
			                              spawnPos.y - floorPos.y, f31);
			break;
		case 1:
			entry->effect = effFallEntry(5, floorPos.x, floorPos.y, 1, floorPos.z, 0x5A, f30,
			                              spawnPos.y - floorPos.y, f31);
			break;
		case 2:
			entry->effect = effFallEntry(0, floorPos.x, floorPos.y, 0x64, floorPos.z, 0x5A, f30,
			                              spawnPos.y - floorPos.y, f31);
			break;
		case 3:
			entry->effect = effFallEntry(1, floorPos.x, floorPos.y, 1, floorPos.z, 0x5A, f30,
			                              spawnPos.y - floorPos.y, f31);
			break;
		case 4:
			entry->effect = effFallEntry(2, floorPos.x, floorPos.y, 1, float_5_8041cd10 + floorPos.z,
			                              0x5A, f30, spawnPos.y - floorPos.y, f31);
			break;
		case 8:
			entry->effect = effFallEntry(8, floorPos.x, floorPos.y, 1, floorPos.z, 0x5A, f30,
			                              spawnPos.y - floorPos.y, f31);
			break;
		case 5:
			entry->effect = effFallEntry(3, floorPos.x, floorPos.y, 1, floorPos.z, 0x5A, f30,
			                              spawnPos.y - floorPos.y, f31);
			break;
		case 6:
			if (BtlUnit_GetBelong(unit) == 0) {
				entry->effect = effFallEntry(6, float_neg300_8041cd14, floorPos.y, 1,
				                              float_15_8041cd18 + floorPos.z, 0xB4,
				                              float_0_8041cd04, float_0_8041cd04, f31);
			} else {
				entry->effect = effFallEntry(6, float_300_8041cd1c, floorPos.y, 1,
				                              float_15_8041cd18 + floorPos.z, 0xB4,
				                              float_0_8041cd04, float_0_8041cd04, f31);
			}
			break;
		case 7:
			entry->effect = effFallEntry(7, floorPos.x, floorPos.y, 1, float_5_8041cd10 + floorPos.z,
			                              0xB4, f30, spawnPos.y - floorPos.y, f31);
			break;
		}
		continue;

	effect_active:
		if (entry->effect != NULL && !effFallCheckHit(entry->effect)) {
			continue;
		}
		weapon = &entry->weapon;
		if (entry->mode != 1) {
			goto stage_hit;
		}
		BattleAudience_Case_FallObject_Aud(entry->unitId, entry->type);
		psndSFXOff(entry->sfxId);
		BattleAudience_GetHomePosition(entry->unitId, &hitPos.x, &hitPos.y, &hitPos.z);
		switch (entry->type) {
		case 0:
		case 1:
			psndSFXOn_3D(strTub, &hitPos);
			break;
		case 5:
			break;
		default:
			psndSFXOn_3D(strNormal, &hitPos);
			break;
		}
		goto done;

	stage_hit:
		BattleAudience_Case_FallObject_Stage();
		if (weapon->targetClassFlags & 0x01000000) {
			BtlUnit_GetHitPos(unit, part, &hitPos.x, &hitPos.y, &hitPos.z);
			m = 0;
			if (entry->flag14 != 0) {
				m |= 0x100;
			}
			psndSFXOff(entry->sfxId);
			switch (entry->index) {
			case 2:
			case 3:
			case 4:
			case 6:
				switch (entry->type) {
				case 0:
				case 1:
					psndSFXOn_3D(strTub, &hitPos);
					break;
				case 5:
					break;
				}
				break;
			case 1:
				switch (entry->type) {
				case 0:
				case 1:
					psndSFXOn_3D(strTub, &hitPos);
					break;
				case 5:
					psndSFXOn_3D(strRest, &hitPos);
					break;
				default:
					psndSFXOn_3D(strNormal, &hitPos);
					break;
				}
				if (BtlUnit_GetACPossibility(unit) != 0) {
					BattleActionCommandCheckDefence(unit, weapon);
				}
				BattleCheckDamage(BattleGetSystemPtr(wp), unit, part, weapon, m);
				break;
			}
		} else {
			psndSFXOff(entry->sfxId);
			BattleSamplingEnemy(samplingB, weapon, BattleGetSystemPtr(wp)->unitId, 1,
			                    weapon->targetClassFlags, weapon->targetPropertyFlags,
			                    (s8)-wp->alliances[1].attackDirection);
			if ((s8)samplingB[0xA6C] > 0)
			for (;;) {
				s32 si;
				u8* se;
				s32 res;
				if ((s8)samplingB[0xAB7] >= (s8)samplingB[0xA6C]) {
					break;
				}
				si = (s8)samplingB[0xA6D + (s8)samplingB[0xAB7]];
				se = samplingB + 4 + si * 0x24;
				unit = BattleGetUnitPtr(wp, *(s16*)(se + 0));
				part = BattleGetUnitPartsPtr(*(s16*)(se + 0), *(s16*)(se + 2));
				res = BattlePreCheckDamage(BattleGetSystemPtr(wp), unit, part, weapon, 0x100);
				switch (res) {
				case 2:
				case 4:
					break;
				case 3:
					BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
					if (pos.y < float_85_8041cd20) {
						pos.y += float_50_8041cd24;
					} else {
						pos.y += float_neg50_8041cd28;
					}
					pos.x -= float_45_8041cd2c;
					{
						EffectEntry* e = effNiceEntry(6, pos.x, pos.y, pos.z);
						*(f32*)((u8*)e->userdata + 0x1C) = float_0p75_8041cd30;
					}
					unit->damagePattern = 0;
					BattleRunHitEvent(unit, 0x126);
					break;
				case 6:
					unit->damagePattern = 0;
					BattleRunHitEvent(unit, 0x127);
					break;
				case 1:
					if (BtlUnit_GetACPossibility(unit) != 0 && check <= 0) {
						BattleActionCommandCheckDefence(unit, weapon);
						check++;
					}
					BattleCheckDamage(BattleGetSystemPtr(wp), unit,
					                  BattleGetUnitPartsPtr(*(s16*)(se + 0), *(s16*)(se + 2)),
					                  weapon, 0x100);
					break;
				}
				samplingB[0xAB7]++;
			}
		}
	done:
		entry->flag = -1;
	}

	if (!active) {
		return 0;
	}
	BattleFree((void*)event->userdata[0]);
	evtSetValue(event, dst, event->userdata[1]);
	return EVT_RETURN_DONE;
}


// .data: tables, event scripts and compiler constants, in the original order.
#pragma force_active on
__declspec(section ".data") s32 lbl_80371490[2] = {
    0xBF800000, 0x3F800000,
};
__declspec(section ".data") s32 negone_one_374[2] = {
    0xBF800000, 0x3F800000,
};
__declspec(section ".data") s32 lbl_803714A0[2] = {
    0x3F800000, 0xBF800000,
};
__declspec(section ".data") s32 one_negone_373[2] = {
    0x3F800000, 0xBF800000,
};
__declspec(section ".data") s32 lbl_803714B0[2] = {
    0xBF800000, 0x3F800000,
};
__declspec(section ".data") s32 negone_one_360[2] = {
    0xBF800000, 0x3F800000,
};
__declspec(section ".data") s32 lbl_803714C0[2] = {
    0x3F800000, 0xBF800000,
};
__declspec(section ".data") s32 one_negone_359[2] = {
    0x3F800000, 0xBF800000,
};
__declspec(section ".data") u8 _nozzle_at_base_mario[192] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x70, 0x20, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x03, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x07, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
__declspec(section ".data") u8 _nozzle_at_base_monster[192] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x6F, 0x20, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x03, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x07, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
__declspec(section ".data") s32 battle_stage_nozzle_data[784] = {
    0x00003232, 0x00320032, 0x00323200, 0x64000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, (s32)&weaponGetPowerDefault,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02100060, 0x20000000, 0x00000302,
    0x00000000, 0x00000000, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, (s32)&weaponGetPowerDefault,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02100060, 0x20000000, 0x00000302,
    0x00000000, 0x00000000, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, (s32)&weaponGetPowerDefault,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02100060, 0x20000000, 0x00000302,
    0x00000000, 0x00000000, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, (s32)&weaponGetPowerDefault,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02100060, 0x20000000, 0x00000302,
    0x00000000, 0x00000000, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
    0x00003232, 0x00320032, 0x00323200, 0x3C280000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, (s32)&weaponGetPowerDefault,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02100060, 0x20000000, 0x00000302,
    0x00000000, 0x00000000, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02100060, 0x20000000, 0x02000302,
    0x00000000, 0x00000000, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00640100, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, (s32)&weaponGetPowerDefault,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02100060, 0x20000000, 0x00000302,
    0x00000000, 0x00000000, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, (s32)&weaponGetPowerDefault,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02100060, 0x20000000, 0x00000302,
    0x00000000, 0x00000000, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
    0x00003232, 0x00320032, 0x00323200, 0x281E1E00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, (s32)&weaponGetPowerDefault,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02100060, 0x20000000, 0x00000302,
    0x00000000, 0x00000000, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02100060, 0x20000000, 0x02000302,
    0x00000000, 0x00000000, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00640100, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, (s32)&weaponGetPowerDefault,
    0x00000002, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02100060, 0x20000000, 0x03000302,
    0x00000000, 0x00000000, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, (s32)&weaponGetPowerDefault,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02100060, 0x20000000, 0x00000302,
    0x00000000, 0x00000000, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
    0x00003232, 0x00320032, 0x00323200, 0x141E1E14,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, (s32)&weaponGetPowerDefault,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02100060, 0x20000000, 0x00000302,
    0x00000000, 0x00000000, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02100060, 0x20000000, 0x02000302,
    0x00000000, 0x00000000, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00640100, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, (s32)&weaponGetPowerDefault,
    0x00000002, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02100060, 0x20000000, 0x03000302,
    0x00000000, 0x00000000, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, (s32)&weaponGetPowerDefault,
    0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02100060, 0x20000000, 0x01000302,
    0x00000000, 0x00000000, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000064,
    0x03000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};
__declspec(section ".data") s32 battle_stage_fall_object_data[1932] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, (s32)&weaponGetPowerDefault,
    0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02100060, 0x20000000, 0x000A0302,
    0x00000000, 0x00000000, 0x000007FF, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
    0x000A0000, 0x00000000, 0x0A000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x64000002,
    0x00000000, 0x00000000, (s32)&weaponGetPowerDefault, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x01100060, 0x20000000, 0x00000302, 0x00000000,
    0x00000000, 0x000007FF, 0x80000000, 0x00000000,
    0x32030000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000064, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x64000002,
    0x00000000, 0x00000000, (s32)&weaponGetPowerDefault, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x01100060, 0x20000000, 0x00000302, 0x00000000,
    0x00000000, 0x000007FF, 0x80000000, 0x00000000,
    0x32030000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000064, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x64000002,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x01100060, 0x20000000, 0x00010302, 0x00000000,
    0x00000000, 0x000007FF, 0x80000000, 0x00000000,
    0x00000000, 0x00320300, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000064, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x64000002,
    0x00000000, 0x00000000, (s32)&weaponGetPowerDefault, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x01100060, 0x20000000, 0x00000302, 0x00000000,
    0x00000000, 0x000007FF, 0x80000000, 0x00000000,
    0x00000000, 0x00320300, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000064, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x64000002,
    0x00000000, 0x00000000, (s32)&weaponGetPowerDefault, 0x00000002,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x01100060, 0x20000000, 0x00000302, 0x00000000,
    0x00000000, 0x000007FF, 0x80000000, 0x64000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000064, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x64000002,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x01100060, 0x20000000, 0x00000302, 0x00000000,
    0x00000100, 0x000007FF, 0x80000000, 0x64006400,
    0x64006400, 0x00640064, 0x00000064, 0x00640000,
    0x00000000, 0x00000000, 0x0000009D, 0x00000000,
    0x00009D00, 0x00000000, 0x00000000, 0x00000000,
    0x00000064, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x64000002,
    0x00000000, 0x00000000, (s32)&weaponGetPowerDefault, 0x00000005,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x02100060, 0x20000000, 0x000A0302, 0x00000000,
    0x00000000, 0x000007FF, 0x80000000, 0x00000000,
    0x00000000, 0x00640300, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000064, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x64000002,
    0x00000000, 0x00000000, (s32)&weaponGetPowerDefault, 0x00000003,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x02100060, 0x20000000, 0x000A0302, 0x00000000,
    0x00000000, 0x000007FF, 0x80000000, 0x00000000,
    0x32030000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000064, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x64000002,
    0x00000000, 0x00000000, (s32)&weaponGetPowerDefault, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x01100060, 0x20000000, 0x000A0302, 0x00000000,
    0x00000000, 0x000007FF, 0x80000000, 0x00000000,
    0x00000000, 0x00000019, 0x03000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000064, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x64000002,
    0x00000000, 0x00000000, (s32)&weaponGetPowerDefault, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x02100060, 0x20000000, 0x000A0302, 0x00000000,
    0x00000000, 0x000007FF, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000064, 0x00000000, 0x00000000, 0x0A0A0000,
    0x000A0000, 0x0A000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x64000002, 0x00000000,
    0x00000000, (s32)&weaponGetPowerDefault, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x01100060,
    0x20000000, 0x00000302, 0x00000000, 0x00000000,
    0x000007FF, 0x80000000, 0x00000000, 0x32030000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000064,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x64000002, 0x00000000,
    0x00000000, (s32)&weaponGetPowerDefault, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x01100060,
    0x20000000, 0x00000302, 0x00000000, 0x00000000,
    0x000007FF, 0x80000000, 0x00000000, 0x32030000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000064,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x64000002, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x01100060,
    0x20000000, 0x00010302, 0x00000000, 0x00000000,
    0x000007FF, 0x80000000, 0x00000000, 0x00000000,
    0x00320300, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000064,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x64000002, 0x00000000,
    0x00000000, (s32)&weaponGetPowerDefault, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x01100060,
    0x20000000, 0x00000302, 0x00000000, 0x00000000,
    0x000007FF, 0x80000000, 0x00000000, 0x00000000,
    0x00320300, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000064,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x64000002, 0x00000000,
    0x00000000, (s32)&weaponGetPowerDefault, 0x00000002, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x01100060,
    0x20000000, 0x00000302, 0x00000000, 0x00000000,
    0x000007FF, 0x80000000, 0x64000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000064,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x64000002, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x01100060,
    0x20000000, 0x00000302, 0x00000000, 0x00000100,
    0x000007FF, 0x80000000, 0x64006400, 0x64006400,
    0x00640064, 0x00000064, 0x00640000, 0x00000000,
    0x00000000, 0x0000009D, 0x00000000, 0x00009D00,
    0x00000000, 0x00000000, 0x00000000, 0x00000064,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x64000002, 0x00000000,
    0x00000000, (s32)&weaponGetPowerDefault, 0x00000005, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x02100060,
    0x20000000, 0x000A0302, 0x00000000, 0x00000000,
    0x000007FF, 0x80000000, 0x00000000, 0x00000000,
    0x00640300, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000064,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x64000002, 0x00000000,
    0x00000000, (s32)&weaponGetPowerDefault, 0x00000003, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x02100060,
    0x20000000, 0x000A0302, 0x00000000, 0x00000000,
    0x000007FF, 0x80000000, 0x00000000, 0x32030000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000064,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x64000002, 0x00000000,
    0x00000000, (s32)&weaponGetPowerDefault, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x01100060,
    0x20000000, 0x000A0302, 0x00000000, 0x00000000,
    0x000007FF, 0x80000000, 0x00000000, 0x00000000,
    0x00000019, 0x03000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000064,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x64000002, 0x00000000,
    0x00000000, (s32)&weaponGetPowerDefault, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x02100060,
    0x20000000, 0x000A0302, 0x00000000, 0x00000000,
    0x000007FF, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000064,
    0x00000000, 0x00000000, 0x0A0A0A0A, 0x0A0A0000,
    0x0A000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x64000002, 0x00000000, 0x00000000,
    (s32)&weaponGetPowerDefault, 0x00000001, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x01100060, 0x20000000,
    0x00000302, 0x00000000, 0x00000000, 0x000007FF,
    0x80000000, 0x00000000, 0x32030000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000064, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x64000002, 0x00000000, 0x00000000,
    (s32)&weaponGetPowerDefault, 0x00000001, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x01100060, 0x20000000,
    0x00000302, 0x00000000, 0x00000000, 0x000007FF,
    0x80000000, 0x00000000, 0x32030000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000064, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x64000002, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x01100060, 0x20000000,
    0x00010302, 0x00000000, 0x00000000, 0x000007FF,
    0x80000000, 0x00000000, 0x00000000, 0x00320300,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000064, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x64000002, 0x00000000, 0x00000000,
    (s32)&weaponGetPowerDefault, 0x00000001, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x01100060, 0x20000000,
    0x00000302, 0x00000000, 0x00000000, 0x000007FF,
    0x80000000, 0x00000000, 0x00000000, 0x00320300,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000064, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x64000002, 0x00000000, 0x00000000,
    (s32)&weaponGetPowerDefault, 0x00000002, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x01100060, 0x20000000,
    0x00000302, 0x00000000, 0x00000000, 0x000007FF,
    0x80000000, 0x64000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000064, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x64000002, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x01100060, 0x20000000,
    0x00000302, 0x00000000, 0x00000100, 0x000007FF,
    0x80000000, 0x64006400, 0x64006400, 0x00640064,
    0x00000064, 0x00640000, 0x00000000, 0x00000000,
    0x0000009D, 0x00000000, 0x00009D00, 0x00000000,
    0x00000000, 0x00000000, 0x00000064, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x64000002, 0x00000000, 0x00000000,
    (s32)&weaponGetPowerDefault, 0x00000005, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x02100060, 0x20000000,
    0x000A0302, 0x00000000, 0x00000000, 0x000007FF,
    0x80000000, 0x00000000, 0x00000000, 0x00640300,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000064, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x64000002, 0x00000000, 0x00000000,
    (s32)&weaponGetPowerDefault, 0x00000003, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x02100060, 0x20000000,
    0x000A0302, 0x00000000, 0x00000000, 0x000007FF,
    0x80000000, 0x00000000, 0x32030000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000064, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x64000002, 0x00000000, 0x00000000,
    (s32)&weaponGetPowerDefault, 0x00000001, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x01100060, 0x20000000,
    0x000A0302, 0x00000000, 0x00000000, 0x000007FF,
    0x80000000, 0x00000000, 0x00000000, 0x00000019,
    0x03000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000064, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x64000002, 0x00000000, 0x00000000,
    (s32)&weaponGetPowerDefault, 0x00000001, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x02100060, 0x20000000,
    0x000A0302, 0x00000000, 0x00000000, 0x000007FF,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000064, 0x00000000,
    0x00000000, 0x0A0A0A0A, 0x0A0A0A0A, 0x0A000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, (s32)&weaponGetPowerDefault,
    0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100060, 0x20000000, 0x00000302,
    0x00000000, 0x00000000, 0x000007FF, 0x80000000,
    0x00000000, 0x32030000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, (s32)&weaponGetPowerDefault,
    0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100060, 0x20000000, 0x00000302,
    0x00000000, 0x00000000, 0x000007FF, 0x80000000,
    0x00000000, 0x32030000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100060, 0x20000000, 0x00010302,
    0x00000000, 0x00000000, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00320300, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, (s32)&weaponGetPowerDefault,
    0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100060, 0x20000000, 0x00000302,
    0x00000000, 0x00000000, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00320300, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, (s32)&weaponGetPowerDefault,
    0x00000002, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100060, 0x20000000, 0x00000302,
    0x00000000, 0x00000000, 0x000007FF, 0x80000000,
    0x64000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100060, 0x20000000, 0x00000302,
    0x00000000, 0x00000100, 0x000007FF, 0x80000000,
    0x64006400, 0x64006400, 0x00640064, 0x00000064,
    0x00640000, 0x00000000, 0x00000000, 0x0000009D,
    0x00000000, 0x00009D00, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, (s32)&weaponGetPowerDefault,
    0x00000005, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02100060, 0x20000000, 0x000A0302,
    0x00000000, 0x00000000, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00640300, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, (s32)&weaponGetPowerDefault,
    0x00000003, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x02100060, 0x20000000, 0x000A0302,
    0x00000000, 0x00000000, 0x000007FF, 0x80000000,
    0x00000000, 0x32030000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x64000002, 0x00000000, 0x00000000, (s32)&weaponGetPowerDefault,
    0x00000001, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x01100060, 0x20000000, 0x000A0302,
    0x00000000, 0x00000000, 0x000007FF, 0x80000000,
    0x00000000, 0x00000000, 0x00000019, 0x03000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000064, 0x00000000, 0x00000000,
};
__declspec(section ".data") s32 _object_nozzle_data[18] = {
    (s32)&str_802F3B88, 0x00030004, 0xC2F00000, 0x00000000,
    0x42B40000, 0x00000000, (s32)&str_802F3B94, 0x00030004,
    0x00000000, 0x00000000, 0x42B40000, 0x00000000,
    (s32)&str_802F3BA0, 0x00030004, 0x42F00000, 0x00000000,
    0x42B40000, 0x00000000,
};
__declspec(section ".data") s32 _object_turn_event_core[175] = {
    0x0002005B, (s32)&_set_mobj_shake_init, 0xFE363C89, 0x0002005B,
    (s32)&_set_mobj_shake, 0xFE363C89, 0x0002005B, (s32)&_set_mobj_rotate_x,
    0xFE363C89, 0x00020019, 0xFE363C89, 0x00000001,
    0x0006005B, (s32)&evt_btl_camera_shake_h, 0x00000000, 0x00000004,
    0x00000000, 0x0000001E, 0x0000000D, 0x00000021,
    0x0002005B, (s32)&_bgset_kemuri_effect, 0xFE363C89, 0x00020019,
    0xFE363C89, 0x00000001, 0x0001005B, (s32)&_bgset_audience_bgset_access,
    0x00000021, 0x0003005B, (s32)&btlevtcmd_check_battleflag, 0xFE363C80,
    0x00000100, 0x00020019, 0xFE363C80, 0x00000000,
    0x00010004, 0x00000014, 0x00000021, 0x0003005B,
    (s32)&_get_bgset_attack_data, 0xFE363C89, 0xFE363C8C, 0x00020018,
    0xFE363C8C, 0x00000000, 0x00010004, 0x00000014,
    0x00000021, 0x0003005B, (s32)&btlevtcmd_GetEnemyBelong, 0xFFFFFFFB,
    0xFE363C80, 0x0004005B, (s32)&btlevtcmd_SamplingEnemy, 0xFFFFFFFB,
    0xFE363C80, 0xFE363C8C, 0x0003005B, (s32)&btlevtcmd_GetSelectEnemy,
    0xFE363C8A, 0xFE363C8B, 0x00020032, 0xFE363C8F,
    0x00000000, 0x00010003, 0x0000000A, 0x00020018,
    0xFE363C8A, 0xFFFFFFFF, 0x00010004, 0x00000014,
    0x00000021, 0x00020018, 0xFE363C8F, 0x00000000,
    0x0003005B, (s32)&btlevtcmd_GetACPossibility, 0xFE363C8A, 0xFE363C80,
    0x00020019, 0xFE363C80, 0x00000000, 0x00020035,
    0xFE363C8F, 0x00000001, 0x0003005B, (s32)&btlevtcmd_ResultACDefence,
    0xFE363C8A, 0xFE363C8C, 0x00000021, 0x00000021,
    0x0007005B, (s32)&btlevtcmd_PreCheckDamage, 0xFFFFFFFB, 0xFE363C8A,
    0xFE363C8B, 0xFE363C8C, 0x00000100, 0xFE363C85,
    0x00010022, 0xFE363C85, 0x00010024, 0x00000004,
    0x00010024, 0x00000002, 0x0003005B, (s32)&btlevtcmd_StartAvoid,
    0xFE363C8A, 0x00000028, 0x00010024, 0x00000003,
    0x0003005B, (s32)&btlevtcmd_StartAvoid, 0xFE363C8A, 0x00000026,
    0x00010024, 0x00000006, 0x0003005B, (s32)&btlevtcmd_StartAvoid,
    0xFE363C8A, 0x00000027, 0x00010024, 0x00000001,
    0x0007005B, (s32)&btlevtcmd_CheckDamage, 0xFFFFFFFB, 0xFE363C8A,
    0xFE363C8B, 0xFE363C8C, 0x00000100, 0xFE363C85,
    0x0000002A, 0x00000031, 0x0003005B, (s32)&btlevtcmd_GetSelectNextEnemy,
    0xFE363C8A, 0xFE363C8B, 0x00010004, 0x0000000A,
    0x00010003, 0x00000014, 0x00010009, 0x00000032,
    0x00020032, 0xFE363C80, 0x000000FF, 0x00020032,
    0xFE363C81, 0x00000033, 0x00010003, 0x00000032,
    0x00020036, 0xFE363C80, 0x00000005, 0x0003005B,
    (s32)&_mobj_set_alpha, 0xFE363C89, 0xFE363C80, 0x00020036,
    0xFE363C81, 0x00000001, 0x0002001B, 0xFE363C81,
    0x00000000, 0x00010009, 0x00000001, 0x00010004,
    0x00000032, 0x00000021, 0x0002005B, (s32)&_mobj_destroy,
    0xFE363C89, 0x00000002, 0x00000001,
};
__declspec(section ".data") s32 _fall_frame_event[77] = {
    0x0001005B, (s32)&_fall_frame_wait, 0x0001005B, (s32)&_fall_frame_wait_unit_ready,
    0x0002005B, (s32)&_fall_frame, 0xFE363C8C, 0x0003005B,
    (s32)&btlevtcmd_GetEnemyBelong, 0xFFFFFFFB, 0xFE363C80, 0x0004005B,
    (s32)&btlevtcmd_SamplingEnemy, 0xFFFFFFFB, 0xFE363C80, 0xFE363C8C,
    0x0003005B, (s32)&btlevtcmd_GetSelectEnemy, 0xFE363C8A, 0xFE363C8B,
    0x00020032, 0xFE363C8F, 0x00000000, 0x00010003,
    0x0000000A, 0x00020018, 0xFE363C8A, 0xFFFFFFFF,
    0x00010004, 0x00000014, 0x00000021, 0x00020018,
    0xFE363C8F, 0x00000000, 0x0003005B, (s32)&btlevtcmd_GetACPossibility,
    0xFE363C8A, 0xFE363C80, 0x00020019, 0xFE363C80,
    0x00000000, 0x00020035, 0xFE363C8F, 0x00000001,
    0x0003005B, (s32)&btlevtcmd_ResultACDefence, 0xFE363C8A, 0xFE363C8C,
    0x00000021, 0x00000021, 0x0007005B, (s32)&btlevtcmd_CheckDamage,
    0xFFFFFFFB, 0xFE363C8A, 0xFE363C8B, 0xFE363C8C,
    0x00000100, 0xFE363C85, 0x0003005B, (s32)&btlevtcmd_GetSelectNextEnemy,
    0xFE363C8A, 0xFE363C8B, 0x00010004, 0x0000000A,
    0x00010003, 0x00000014, 0x0006005B, (s32)&evt_btl_camera_shake_h,
    0x00000000, 0x0000000A, 0x00000000, 0x00000014,
    0x0000000D, 0x0001005B, (s32)&_fall_frame_delete, 0x00000002,
    0x00000001,
};
__declspec(section ".data") s32 _nozzle_work_event_core[199] = {
    0x0001005B, (s32)&_nozzle_work_wait, 0x0003005B, (s32)&_nozzle_work_effect,
    0xFE363C8A, 0xFE363C80, 0x00020018, 0xFE363C80,
    0x00000000, 0x00000002, 0x00000021, 0x0001005B,
    (s32)&_nozzle_work_attack_set, 0x0003005B, (s32)&_nozzle_work_get_weapon_address, 0x00000000,
    0xFE363C8C, 0x00020018, 0xFE363C8C, 0x00000000,
    0x00010004, 0x00000013, 0x00000021, 0x0003005B,
    (s32)&btlevtcmd_GetEnemyBelong, 0xFFFFFFFB, 0xFE363C80, 0x0004005B,
    (s32)&btlevtcmd_SamplingEnemy, 0xFFFFFFFB, 0xFE363C80, 0xFE363C8C,
    0x0003005B, (s32)&btlevtcmd_GetSelectEnemy, 0xFE363C8A, 0xFE363C8B,
    0x00020032, 0xFE363C8F, 0x00000000, 0x00010003,
    0x0000000A, 0x00020018, 0xFE363C8A, 0xFFFFFFFF,
    0x00010004, 0x00000013, 0x00000021, 0x00020018,
    0xFE363C8F, 0x00000000, 0x0003005B, (s32)&btlevtcmd_GetACPossibility,
    0xFE363C8A, 0xFE363C80, 0x00020019, 0xFE363C80,
    0x00000000, 0x00020035, 0xFE363C8F, 0x00000001,
    0x0003005B, (s32)&btlevtcmd_ResultACDefence, 0xFE363C8A, 0xFE363C8C,
    0x00000021, 0x00000021, 0x0007005B, (s32)&btlevtcmd_PreCheckDamage,
    0xFFFFFFFB, 0xFE363C8A, 0xFE363C8B, 0xFE363C8C,
    0x00000100, 0xFE363C85, 0x00010022, 0xFE363C85,
    0x00010024, 0x00000004, 0x00010024, 0x00000002,
    0x0003005B, (s32)&btlevtcmd_StartAvoid, 0xFE363C8A, 0x00000028,
    0x00010024, 0x00000003, 0x0003005B, (s32)&btlevtcmd_StartAvoid,
    0xFE363C8A, 0x00000026, 0x00010024, 0x00000006,
    0x0003005B, (s32)&btlevtcmd_StartAvoid, 0xFE363C8A, 0x00000027,
    0x00010024, 0x00000001, 0x0007005B, (s32)&btlevtcmd_CheckDamage,
    0xFFFFFFFB, 0xFE363C8A, 0xFE363C8B, 0xFE363C8C,
    0x00000100, 0xFE363C85, 0x0000002A, 0x00000031,
    0x0003005B, (s32)&btlevtcmd_GetSelectNextEnemy, 0xFE363C8A, 0xFE363C8B,
    0x00010004, 0x0000000A, 0x00010003, 0x00000013,
    0x0003005B, (s32)&_nozzle_work_get_weapon_address, 0x00000001, 0xFE363C8C,
    0x00020018, 0xFE363C8C, 0x00000000, 0x00010004,
    0x0000001D, 0x00000021, 0x0003005B, (s32)&btlevtcmd_GetEnemyBelong,
    0xFFFFFFFB, 0xFE363C80, 0x0004005B, (s32)&btlevtcmd_SamplingEnemy,
    0xFFFFFFFB, 0xFE363C80, 0xFE363C8C, 0x0003005B,
    (s32)&btlevtcmd_GetSelectEnemy, 0xFE363C8A, 0xFE363C8B, 0x00010003,
    0x00000014, 0x00020018, 0xFE363C8A, 0xFFFFFFFF,
    0x00010004, 0x0000001D, 0x00000021, 0x0007005B,
    (s32)&btlevtcmd_PreCheckDamage, 0xFFFFFFFB, 0xFE363C8A, 0xFE363C8B,
    0xFE363C8C, 0x00000100, 0xFE363C85, 0x00010022,
    0xFE363C85, 0x00010024, 0x00000004, 0x00010024,
    0x00000002, 0x0003005B, (s32)&btlevtcmd_StartAvoid, 0xFE363C8A,
    0x00000028, 0x00010024, 0x00000003, 0x0003005B,
    (s32)&btlevtcmd_StartAvoid, 0xFE363C8A, 0x00000026, 0x00010024,
    0x00000006, 0x0003005B, (s32)&btlevtcmd_StartAvoid, 0xFE363C8A,
    0x00000027, 0x00010024, 0x00000001, 0x0007005B,
    (s32)&btlevtcmd_CheckDamage, 0xFFFFFFFB, 0xFE363C8A, 0xFE363C8B,
    0xFE363C8C, 0x00000100, 0xFE363C85, 0x0000002A,
    0x00000031, 0x0003005B, (s32)&btlevtcmd_GetSelectNextEnemy, 0xFE363C8A,
    0xFE363C8B, 0x00010004, 0x00000014, 0x00010003,
    0x0000001D, 0x00000002, 0x00000001,
};
__declspec(section ".data") s32 _object_fall_event[13] = {
    0x0001005B, (s32)&_object_fall_wait, 0x0002005B, (s32)&_object_fall_attack,
    0xFE363C80, 0x00020018, 0xFE363C80, 0x00000001,
    0x0001000A, 0x000003E8, 0x00000021, 0x00000002,
    0x00000001,
};
__declspec(section ".data") s32 jumptable_80374848[9] = {
    (s32)&_object_fall_attack, (s32)&_object_fall_attack, (s32)&_object_fall_attack, (s32)&_object_fall_attack,
    (s32)&_object_fall_attack, (s32)&_object_fall_attack, (s32)&_object_fall_attack, (s32)&_object_fall_attack,
    (s32)&_object_fall_attack,
};
__declspec(section ".data") static u32 gap_05_8037486C_data[1] = { 0x00000000 };
#pragma force_active off

// .sdata: small-data tables and event scripts, in the original order.
#pragma force_active on
__declspec(section ".sdata") s32 _object_wall_data[6] = {
    (s32)&str_802F3BAC, 0x00040005, 0xC32F0000, 0x00000000,
    0x00000000, 0x00000000,
};
__declspec(section ".sdata") s32 _object_spot_light_data[12] = {
    (s32)&str_802F3BB8, 0x00060007, 0x00000000, 0x43700000,
    0x00000000, 0x00000000, 0x00000000, 0x00060007,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
__declspec(section ".sdata") s32 _entry_tree_data[12] = {
    (s32)&unit_object_tree, 0x02000000, 0x04000005, 0xC47A0000,
    0xC47A0000, 0xC47A0000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
__declspec(section ".sdata") s32 _entry_switch_data[12] = {
    (s32)&unit_object_switch, 0x02000000, 0x04000005, 0xC47A0000,
    0xC47A0000, 0xC47A0000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
__declspec(section ".sdata") s32 _object_turn_event_A1[7] = {
    0x00020032, 0xFE363C89, 0x00000000, 0x0001005E,
    (s32)&_object_turn_event_core, 0x00000002, 0x00000001,
};
__declspec(section ".sdata") s32 _object_turn_event_A2[7] = {
    0x00020032, 0xFE363C89, 0x00000001, 0x0001005E,
    (s32)&_object_turn_event_core, 0x00000002, 0x00000001,
};
__declspec(section ".sdata") s32 _object_turn_event_B[7] = {
    0x00020032, 0xFE363C89, 0x00000002, 0x0001005E,
    (s32)&_object_turn_event_core, 0x00000002, 0x00000001,
};
__declspec(section ".sdata") s32 _nozzle_turn_event[4] = {
    0x0001005B, (s32)&_nozzle_turn, 0x00000002, 0x00000001,
};
__declspec(section ".sdata") s32 _close_wall_event[6] = {
    0x0001005B, (s32)&btlevtcmd_WaitAttackEnd, 0x0001005B, (s32)&_announce_wall_message,
    0x00000002, 0x00000001,
};
__declspec(section ".sdata") s32 _nozzle_work_event_0[7] = {
    0x00020032, 0xFE363C8A, 0x00000000, 0x0001005E,
    (s32)&_nozzle_work_event_core, 0x00000002, 0x00000001,
};
__declspec(section ".sdata") s32 _nozzle_work_event_1[7] = {
    0x00020032, 0xFE363C8A, 0x00000001, 0x0001005E,
    (s32)&_nozzle_work_event_core, 0x00000002, 0x00000001,
};
__declspec(section ".sdata") static u32 gap_07_80411F3C_sdata[1] = { 0x00000000 };
#pragma force_active off

// .rodata: vectors and strings, in the original order.
#pragma force_active on
__declspec(section ".rodata") char gap_802f3ac8_rodata[12] = { 0x44, 0x79, 0xC0, 0x00, 0x44, 0x79, 0xC0, 0x00, 0x44, 0x79, 0xC0, 0x00 };
__declspec(section ".rodata") Vec vec3_802f3ad4 = { 999, 999, 999 };
__declspec(section ".rodata") Vec vec3_802f3ae0 = { 999, 999, 999 };
__declspec(section ".rodata") Vec vec3_802f3aec = { 999, 999, 999 };
__declspec(section ".rodata") Vec vec3_802f3af8 = { 999, 999, 999 };
__declspec(section ".rodata") Vec vec3_802f3b04 = { 999, 999, 999 };
__declspec(section ".rodata") Vec vec3_802f3b10 = { 999, 999, 999 };
__declspec(section ".rodata") Vec vec3_802f3b1c = { 999, 999, 999 };
__declspec(section ".rodata") Vec vec3_802f3b28 = { 999, 999, 999 };
__declspec(section ".rodata") Vec vec3_802f3b34 = { 999, 999, 999 };
__declspec(section ".rodata") Vec vec3_802f3b40 = { 999, 999, 999 };
__declspec(section ".rodata") Vec vec3_802f3b4c = { 999, 999, 999 };
__declspec(section ".rodata") Vec vec3_802f3b58 = { 999, 999, 999 };
__declspec(section ".rodata") Vec vec3_802f3b64 = { 999, 999, 999 };
__declspec(section ".rodata") Vec vec3_802f3b70 = { 999, 999, 999 };
__declspec(section ".rodata") Vec vec3_802f3b7c = { 999, 999, 999 };
__declspec(section ".rodata") char str_802F3B88[12] = "nozzle_l";
__declspec(section ".rodata") char str_802F3B94[12] = "nozzle_c";
__declspec(section ".rodata") char str_802F3BA0[12] = "nozzle_r";
__declspec(section ".rodata") char str_802F3BAC[12] = "escape_wall";
__declspec(section ".rodata") char str_802F3BB8[12] = "spotlight";
__declspec(section ".rodata") Vec vec3_802f3bc4 = { 0, 0, -100 };
__declspec(section ".rodata") Vec vec3_802f3bd0 = { 0, 0, 0 };
__declspec(section ".rodata") static u32 gap_04_802F3D5C_rodata[1] = { 0x00000000 };
#pragma force_active off

// .sdata2: colour words, float constants and short strings, in the original order.
#pragma force_active on
__declspec(section ".sdata2") const u32 dat_8041cce8 = 0xFFFFFF00;
__declspec(section ".sdata2") const u32 dat_8041ccec = 0xFFFFFF00;
__declspec(section ".sdata2") const u32 dat_8041ccf0 = 0xFFFFFF00;
__declspec(section ".sdata2") const u32 dat_8041ccf4 = 0xFFFFFF00;
__declspec(section ".sdata2") const u32 dat_8041ccf8 = 0xFFFFFFFF;
#ifdef MUST_MATCH
__declspec(section ".sdata2") u32 dat_8041ccfc = 0xFFFFFF00;
#else
const u32 dat_8041ccfc = 0xFFFFFF00;
#endif
__declspec(section ".sdata2") f32 float_0p1_8041cd00 = 0.100000001f;
__declspec(section ".sdata2") const f32 float_0_8041cd04 = 0.0f;
__declspec(section ".sdata2") f32 float_90_8041cd08 = 90.0f;
__declspec(section ".sdata2") f32 float_270_8041cd0c = 270.0f;
__declspec(section ".sdata2") f32 float_5_8041cd10 = 5.0f;
__declspec(section ".sdata2") f32 float_neg300_8041cd14 = -300.0f;
__declspec(section ".sdata2") f32 float_15_8041cd18 = 15.0f;
__declspec(section ".sdata2") f32 float_300_8041cd1c = 300.0f;
__declspec(section ".sdata2") f32 float_85_8041cd20 = 85.0f;
__declspec(section ".sdata2") f32 float_50_8041cd24 = 50.0f;
__declspec(section ".sdata2") f32 float_neg50_8041cd28 = -50.0f;
__declspec(section ".sdata2") f32 float_45_8041cd2c = 45.0f;
__declspec(section ".sdata2") f32 float_0p75_8041cd30 = 0.75f;
__declspec(section ".sdata2") f32 float_10_8041cd34 = 10.0f;
__declspec(section ".sdata2") f32 float_12_8041cd38 = 12.0f;
__declspec(section ".sdata2") const f32 float_neg80_8041cd3c = -80.0f;
__declspec(section ".sdata2") f32 float_20_8041cd40 = 20.0f;
__declspec(section ".sdata2") f32 float_8_8041cd44 = 8.0f;
__declspec(section ".sdata2") f32 float_1_8041cd48 = 1.0f;
__declspec(section ".sdata2") f32 float_neg45_8041cd4c = -45.0f;
__declspec(section ".sdata2") f32 float_neg135_8041cd50 = -135.0f;
__declspec(section ".sdata2") f32 float_neg90_8041cd54 = -90.0f;
__declspec(section ".sdata2") f32 float_neg117_8041cd58 = -117.0f;
__declspec(section ".sdata2") f32 float_30_8041cd5c = 30.0f;
__declspec(section ".sdata2") f32 float_neg120_8041cd60 = -120.0f;
__declspec(section ".sdata2") f32 float_900_8041cd64 = 900.0f;
__declspec(section ".sdata2") char str_8041CD68[4] = "A1";
__declspec(section ".sdata2") char str_8041CD6C[4] = "A2";
__declspec(section ".sdata2") char str_8041CD70[4] = "B";
__declspec(section ".sdata2") char str_8041CD74[4] = "C";
__declspec(section ".sdata2") char str_8041CD78[4] = "den";
__declspec(section ".sdata2") static u32 gap_09_8041CD7C_sdata2[1] = { 0x00000000 };
#pragma force_active off
