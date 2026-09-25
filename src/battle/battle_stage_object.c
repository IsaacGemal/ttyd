#include "manager/fontmgr.h"
#include "battle/battle_stage_object.h"
#include "battle/battle.h"
#include "system.h"
#include <string.h>

extern BattleWork* _battleWorkPointer;

//local prototypes
void BattleObjectConfig(void);

// Data + helpers not declared in headers
extern s32 _fall_frame_event[];
extern s32 _object_fall_event[];
extern void BattleAudience_Case_HaikeiSet(void);
extern s32 BattleWaitAllActiveEvtEnd_NoBgSetEndWait(void);
extern void _Nozzle_Change_Check(u8 index, void* nozzle);
extern const char str_802F3C7C[];
extern void mapGrpSetColor(const char* name, GXColor* color);
extern s32 _get_nozzle_attack_index(u8 index);
extern void _combine_nozzle_weapon(s32 mode, void* dst, void* src);
extern u8 _nozzle_at_base_mario[];
extern u8 _nozzle_at_base_monster[];
extern f32 intpl_sub(s32 mode, s32 currStep, s32 steps, f32 start, f32 end);
extern f32 sinfd(f32 deg);
extern char str_8041CD68[4];
extern char str_8041CD6C[4];
extern char str_8041CD70[4];
extern char str_8041CD74[4];
extern const u32 dat_8041cce8;
extern const u32 dat_8041ccec;
extern const u32 dat_8041ccf0;
extern const u32 dat_8041ccf4;
extern const u32 dat_8041ccfc;
extern char str_802F3CC4[];
extern char str_802F3CDC[];
extern char str_802F3CF4[];
extern char vec3_802f3ac8[];
extern EffectEntry* effKemuri1N64Entry(s32 type, f32 a2, f32 a3, f32 a4, f32 a5);
extern void psndSFXOn_3D(const char* name, Vec* position);
void _mobj_shake_main(BattleStageObject* obj);
void _disp_escape_wall_message(CameraId cameraId, void* param);
extern const u32 dat_8041ccf8;
extern f64 double_to_int_802f3c30;
extern f32 float_0_8041cd04;
extern f32 float_10_8041cd34;
extern f32 float_20_8041cd40;
extern f32 float_30_8041cd5c;
extern f32 float_neg117_8041cd58;
extern f32 float_neg120_8041cd60;
extern const char str_802F3C84[];
extern const char* msgSearch(const char* tag);
extern void FontDrawStart(void);
extern void FontDrawString(const char* str, f32 x, f32 y);
// Differs from windowdrv.h (s16, GXColor by value): with the header's signature the caller copies the color
// and _disp_escape_wall_message stops matching (99.06%). The ABI is the same; the original passes it uncopied.
extern void windowDispGX_Waku_col(s32 texMap, GXColor* color, f32 x, f32 y, f32 h, f32 w, f32 curve);

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

extern f32 float_0_8041cd04;
extern f32 float_15_8041cd18;
extern f32 float_30_8041cd5c;
extern f32 float_neg45_8041cd4c;
extern f32 float_neg135_8041cd50;
extern f32 float_neg90_8041cd54;

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

	if (!BattleWaitAllActiveEvtEnd_NoBgSetEndWait()) {
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

	if (!BattleWaitAllActiveEvtEnd_NoBgSetEndWait()) {
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

	if (!BattleWaitAllActiveEvtEnd_NoBgSetEndWait()) {
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

// NONMATCHING (94.57%): only the address form differs. Target emits `slwi r0,r3,2; add r5,r31,r0;
// lwz r5,0x3c(r5)`; mwcc folds the +0x3C into the index and emits `addi r0,r5,0x3c; lwzx r5,r31,r0`.
USER_FUNC(_nozzle_work_get_weapon_address) {
	u8* w = (u8*)_battleWorkPointer + 0x20000 - 0x7F40;
	s32* args = event->args;
	s32 idx;

	idx = evtGetValue(event, *args);
	evtSetValue(event, args[1], *(s32*)(w + idx * 4 + 0x3C));
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

// NONMATCHING (94.51%): same as DestroyA1; target keeps `wp` in r31 and recomputes the `+0x20000` base,
// mwcc merges it and mutates r31 after the first `evtDeleteID`.
void BattleStage_DestroyB(void) {
	BattleWork* wp = _battleWorkPointer;
	void* evt = *(void**)(*(u8**)((u8*)wp->fbatInfo->setup) + 0x19C);

	evtDeleteID(*(s32*)((u8*)wp + 0x20000 - 0x7F14));
	*(s32*)((u8*)wp + 0x20000 - 0x7F14) = 0;
	if (*(s32*)((u8*)wp + 0x20000 - 0x7F20) != 0 && evtCheckID(*(s32*)((u8*)wp + 0x20000 - 0x7F20))) {
		evtDeleteID(*(s32*)((u8*)wp + 0x20000 - 0x7F20));
	}
	if (evt == NULL) {
		evt = _object_turn_event_B;
	}
	{
		EventEntry* e = evtEntry(evt, 10, 0);
		if (e != NULL) {
			*(s32*)((u8*)wp + 0x20000 - 0x7F20) = e->eventId;
		}
	}
}

// NONMATCHING (94.51%): target keeps `wp` in r31 and recomputes `wp + 0x20000` into r3 at each use; mwcc
// merges `wp` into the byte-offset pointer after the first `evtDeleteID` call (`addis r31,r31,2`) and uses r31.
void BattleStage_DestroyA1(void) {
	BattleWork* wp = _battleWorkPointer;
	void* evt = *(void**)(*(u8**)((u8*)wp->fbatInfo->setup) + 0x194);

	evtDeleteID(*(s32*)((u8*)wp + 0x20000 - 0x7F1C));
	*(s32*)((u8*)wp + 0x20000 - 0x7F1C) = 0;
	if (*(s32*)((u8*)wp + 0x20000 - 0x7F20) != 0 && evtCheckID(*(s32*)((u8*)wp + 0x20000 - 0x7F20))) {
		evtDeleteID(*(s32*)((u8*)wp + 0x20000 - 0x7F20));
	}
	if (evt == NULL) {
		evt = _object_turn_event_A1;
	}
	{
		EventEntry* e = evtEntry(evt, 10, 0);
		if (e != NULL) {
			*(s32*)((u8*)wp + 0x20000 - 0x7F20) = e->eventId;
		}
	}
}

// NONMATCHING (89.03%): target materializes the GXColor in a temp at 0x8 then copies it to `color` at 0xc and
// references the named `dat_8041ccfc` (=0xFFFFFF00); mwcc builds it in place at 0x8 and emits an anonymous
// constant (`@345`). Functionally identical. Discovered the data value 0xFFFFFF00 (alpha 0) while trying forms.
USER_FUNC(_fall_frame_delete) {
	u8* w = (u8*)_battleWorkPointer + 0x20000;

	if (isFirstCall) {
		event->userdata[2] = 0xFF;
	}
	event->userdata[2] -= 5;
	{
		GXColor color = { 0xFF, 0xFF, 0xFF, 0 };
		color.a = event->userdata[2];
		mapGrpSetColor(str_802F3C7C, &color);
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

// NONMATCHING (87.33%): target loads `_battleWorkPointer` into r5, computes `w` from r5 and then `mr r29,r5`
// before the call; mwcc loads straight into r29. All other instructions match. The `layer >= 0 && layer < 2`
// guard compiled correctly as a `switch (layer) { case 0: case 1: ... }`; `_mobj_shake_main` must be wrapped in
// `#pragma dont_inline on/reset` or mwcc inlines the call away (target keeps the `bl`).
USER_FUNC(_set_mobj_shake) {
	BattleWork* wp = _battleWorkPointer;
	u8* w = (u8*)wp + 0x20000;
	s32 layer = evtGetValue(event, *event->args);
	BattleStageObject* obj;
	s32 i;

	if (!BattleWaitAllActiveEvtEnd_NoBgSetEndWait()
	    || (*(s32*)(w - 0x7D68) != 0 && evtCheckID(*(s32*)(w - 0x7D68)))
	    || (*(s32*)(w - 0x7D58) != 0 && evtCheckID(*(s32*)(w - 0x7D58)))) {
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
		}
		return EVT_RETURN_BLOCK;
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(_fall_frame) {
	BattleWork* wp = _battleWorkPointer;
	u8* w = (u8*)wp + 0x20000;
	s32 layer = event->args[0];
	BattleStageObject* obj = BattleSearchObjectPtr(str_802F3C7C);

	if (isFirstCall) {
		event->userdata[0] = 0;
		event->userdata[1] = (s32)obj->mPosition.y;
	}
	event->userdata[0]++;
	obj->mPosition.y = intpl_sub(2, event->userdata[0] * event->userdata[0] / 0x3C, 0x3C,
	                             (f32)event->userdata[1], 0.0f);
	if (event->userdata[0] >= 0x3C) {
		evtSetValue(event, layer, *(s32*)(w - 0x7F08));
		return EVT_RETURN_DONE;
	}
	return EVT_RETURN_BLOCK;
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
// NONMATCHING (91.12%): target keeps the base in r5 and the zero-extended index in r3, reloading
// `clrlwi r0,r3,24` per case; mwcc keeps the base in r3 and skips the reloads. All case bodies/labels match.
s32 _get_nozzle_attack_index(u8 index) {
	u8* base = (u8*)_battleWorkPointer + 0x20000 - 0x7F40;
	u8 type;

	switch (index) {
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
			return -1;
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
			return -1;
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
			return -1;
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

#pragma dont_inline on
// NONMATCHING (82.61%): target precomputes `index+0x1C4` into r29 and uses `lbzx base,r29`; mwcc adds
// `base+index` then uses the 0x1C4 displacement, and keeps one fewer callee-saved register (r28 vs r27 start).
// All sequence/opcodes match. The `slot[0x73]` (rather than `base+index*4+0x1CC`) form was needed to stop mwcc
// folding the 0x1CC into an `lwzx`.
void _Nozzle_Change_Check(u8 index, void* nozzle) {
	u8* base = (u8*)_battleWorkPointer + 0x20000 - 0x7F40;
	u8* data = nozzle;
	s32 sum;
	u8 i;
	s32 k;

	sum = 0;
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
	if (i != *(u8*)(base + index + 0x1C4)) {
		s32* slot = (s32*)(base + index * 4);
		if (slot[0x73] != 0 && !evtCheckID(slot[0x73])) {
			slot[0x73] = 0;
		}
		if (slot[0x73] == 0) {
			*(u8*)(base + index + 0x1C7) = i;
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
// NONMATCHING (94.50%): target hoists `lis/addi vec3_802f3ac8@ha/@l` before the `evtGetValue` call and keeps
// the args pointer in r5; mwcc does the vec3 base load after the call in r3 and uses r4 for `event->args`, which
// also shifts the `li r25/r31,0` ordering. Functionally identical. Uses named `float_15/30/0_8041cd*` and
// `vec3_802f3ac8` (the strings live at +0x1FC/+0x214/+0x22C) so the constants relocate to the target symbols.
USER_FUNC(_bgset_kemuri_effect) {
	BattleWork* wp = _battleWorkPointer;
	s32* args = event->args;
	s32 layer = evtGetValue(event, *args);
	s32 i;
	BattleStageObject* obj;
	f32 x, y, z;

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
					psndSFXOn_3D(vec3_802f3ac8 + 0x1FC, &obj->mPosition);
					break;
				case 1:
					z += float_15_8041cd18;
					effKemuri1N64Entry(0, x, y, z, float_0_8041cd04);
					psndSFXOn_3D(vec3_802f3ac8 + 0x214, &obj->mPosition);
					break;
				case 2:
					z += float_30_8041cd5c;
					effKemuri1N64Entry(3, x, y, z, float_0_8041cd04);
					effKemuri1N64Entry(4, x, y, z, float_0_8041cd04);
					psndSFXOn_3D(vec3_802f3ac8 + 0x22C, &obj->mPosition);
					break;
			}
		}
	}
	return EVT_RETURN_DONE;
}

// NONMATCHING (89.80%): target materialises each GXColor in a 4-byte temp then copies it to the real slot
// (temp at color-4), giving a 0x30 frame; mwcc builds it in place (0x20 frame). Every other instruction matches.
// Fixed by declaring `str_8041CD6{8,C,0,4}` as `[4]` (size <= sdata2 threshold -> `li r3,sym@sda21`),
// `u8 alpha` (target `clrlwi r30,r3,24`), `s32* args = event->args` (target keeps it in r31), and assigning
// `color = *(GXColor*)&dat_8041cce8; color.a = alpha;` so the constant relocates to the named symbol.
USER_FUNC(_mobj_set_alpha) {
	s32* args = event->args;
	s32 which = evtGetValue(event, args[0]);
	u8 alpha = evtGetValue(event, args[1]);
	GXColor color;

	switch (which) {
		case 0:
			if (mapGetMapObj(str_8041CD68) != NULL) {
				color = *(GXColor*)&dat_8041cce8;
				color.a = alpha;
				mapGrpSetColor(str_8041CD68, &color);
			}
			break;
		case 1:
			if (mapGetMapObj(str_8041CD6C) != NULL) {
				color = *(GXColor*)&dat_8041ccec;
				color.a = alpha;
				mapGrpSetColor(str_8041CD6C, &color);
			}
			break;
		case 2:
			if (mapGetMapObj(str_8041CD70) != NULL) {
				color = *(GXColor*)&dat_8041ccf0;
				color.a = alpha;
				mapGrpSetColor(str_8041CD70, &color);
			}
			break;
		case 3:
			if (mapGetMapObj(str_8041CD74) != NULL) {
				color = *(GXColor*)&dat_8041ccf4;
				color.a = alpha;
				mapGrpSetColor(str_8041CD74, &color);
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

void BattleStageObjectMain(void) {

}
