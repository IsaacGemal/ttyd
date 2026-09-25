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
extern char str_8041CD68[];
extern char str_8041CD6C[];
extern char str_8041CD70[];
extern char str_8041CD74[];
extern const u32 dat_8041cce8;
extern const u32 dat_8041ccec;
extern const u32 dat_8041ccf0;
extern const u32 dat_8041ccf4;
extern char str_802F3CC4[];
extern char str_802F3CDC[];
extern char str_802F3CF4[];
extern EffectEntry* effKemuri1N64Entry(s32 type, f32 a2, f32 a3, f32 a4, f32 a5);
extern void psndSFXOn_3D(const char* name, Vec* position);
void _mobj_shake_main(BattleStageObject* obj);
void _disp_escape_wall_message(CameraId cameraId, void* param);

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

extern const f32 float_neg45_8041cd4c;
extern const f32 float_neg135_8041cd50;
extern const f32 float_neg90_8041cd54;
const f32 float_neg45_8041cd4c = -45.0f;
const f32 float_neg135_8041cd50 = -135.0f;
const f32 float_neg90_8041cd54 = -90.0f;

f32 _get_nozzle_dir(s32 index) {
	u8* w = (u8*)_battleWorkPointer + 0x20000;
	u8* p = w + index;
	s32 dir = p[-0x7D7C];

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
				if (flag != 0) {
					*(s32*)(w - 0x7D68) = evtEntry(_nozzle_work_event_1, 10, 0)->eventId;
				} else {
					*(s32*)(w - 0x7D68) = evtEntry(_nozzle_work_event_0, 10, 0)->eventId;
				}
			}
		}
	}
}

USER_FUNC(_nozzle_work_get_weapon_address) {
	u8* w = (u8*)_battleWorkPointer + 0x20000 - 0x7F40;
	s32 idx;

	idx = evtGetValue(event, *event->args);
	evtSetValue(event, event->args[1], *(s32*)(w + idx * 4 + 0x3C));
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

USER_FUNC(_get_bgset_attack_data) {
	BattleWork* wp = _battleWorkPointer;
	s32 kind = evtGetValue(event, event->args[0]);
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
	evtSetValue(event, event->args[1], (s32)r5);
	return EVT_RETURN_DONE;
}

void BattleStage_DestroyA2(void) {
	BattleWork* wp = _battleWorkPointer;
	u8* w = (u8*)wp + 0x20000;
	void* evt = *(void**)(*(u8**)((u8*)wp->fbatInfo->setup) + 0x198);

	evtDeleteID(*(s32*)(w - 0x7F18));
	*(s32*)(w - 0x7F18) = 0;
	if (evt == NULL) {
		evt = _object_turn_event_A2;
	}
	evtEntry(evt, 10, 0);
}

void BattleStage_DestroyB(void) {
	BattleWork* wp = _battleWorkPointer;
	u8* w = (u8*)wp + 0x20000;
	void* evt = *(void**)(*(u8**)((u8*)wp->fbatInfo->setup) + 0x19C);

	evtDeleteID(*(s32*)(w - 0x7F14));
	*(s32*)(w - 0x7F14) = 0;
	if (*(s32*)(w - 0x7F20) != 0 && evtCheckID(*(s32*)(w - 0x7F20))) {
		evtDeleteID(*(s32*)(w - 0x7F20));
	}
	if (evt == NULL) {
		evt = _object_turn_event_B;
	}
	{
		EventEntry* e = evtEntry(evt, 10, 0);
		if (e != NULL) {
			*(s32*)(w - 0x7F20) = e->eventId;
		}
	}
}

void BattleStage_DestroyA1(void) {
	BattleWork* wp = _battleWorkPointer;
	u8* w = (u8*)wp + 0x20000;
	void* evt = *(void**)(*(u8**)((u8*)wp->fbatInfo->setup) + 0x194);

	evtDeleteID(*(s32*)(w - 0x7F1C));
	*(s32*)(w - 0x7F1C) = 0;
	if (*(s32*)(w - 0x7F20) != 0 && evtCheckID(*(s32*)(w - 0x7F20))) {
		evtDeleteID(*(s32*)(w - 0x7F20));
	}
	if (evt == NULL) {
		evt = _object_turn_event_A1;
	}
	{
		EventEntry* e = evtEntry(evt, 10, 0);
		if (e != NULL) {
			*(s32*)(w - 0x7F20) = e->eventId;
		}
	}
}

USER_FUNC(_fall_frame_delete) {
	u8* w = (u8*)_battleWorkPointer + 0x20000;
	GXColor color;

	if (isFirstCall) {
		event->userdata[2] = 0xFF;
	}
	event->userdata[2] -= 5;
	color.r = 0xFF;
	color.g = 0xFF;
	color.b = 0xFF;
	color.a = event->userdata[2];
	mapGrpSetColor(str_802F3C7C, &color);
	if (event->userdata[2] > 0) {
		return EVT_RETURN_BLOCK;
	}
	*(s32*)(w - 0x7D58) = 0;
	return EVT_RETURN_DONE;
}

void _mobj_shake_main(BattleStageObject* obj) {
	obj->mShakePeriodPosition++;
	if (obj->mShakePeriodPosition >= obj->mShakePeriodLength) {
		obj->mShakePeriodPosition = 0;
	}
	obj->mShakeAngle = sinfd(obj->mShakePeriodPosition * 360 / obj->mShakePeriodLength) * 5.0f;
	obj->mRotation.x = obj->mShakeAngle;
}

USER_FUNC(_set_mobj_shake) {
	BattleWork* wp = _battleWorkPointer;
	u8* w = (u8*)wp + 0x20000;
	s32 layer = evtGetValue(event, *event->args);
	BattleStageObject* obj;
	s32 i;

	if (!BattleWaitAllActiveEvtEnd_NoBgSetEndWait()
	    || (*(s32*)(w - 0x7D68) != 0 && evtCheckID(*(s32*)(w - 0x7D68)))
	    || (*(s32*)(w - 0x7D58) != 0 && evtCheckID(*(s32*)(w - 0x7D58)))) {
		if (layer >= 0 && layer < 2) {
			for (i = 0; i < 32; i++) {
				obj = &wp->stageObjects[i];
				if (obj->id > 0 && obj->data->mLayer == layer) {
					_mobj_shake_main(obj);
				}
			}
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
			if (unit != NULL && (*(u32*)((u8*)unit + 0x1C) & 0x80000000) == 0
			    && !BtlUnit_CheckStatus(unit, 0x1B)) {
				if (*(void**)((u8*)unit + 0x2BC) != NULL) {
					EventEntry* e = evtEntry(*(void**)((u8*)unit + 0x2BC), 10, 0);
					if (e != NULL) {
						e->unitId = *(s32*)((u8*)unit + 0x0);
					}
					*(s32*)((u8*)unit + 0x2C0) = e->eventId;
				}
			}
		}
	}
	for (i = 0; i < 64; i++) {
		unit = BattleGetUnitPtr(wp, i);
		if (unit != NULL && (*(u32*)((u8*)unit + 0x1C) & 0x80000000) == 0) {
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
s32 _get_nozzle_attack_index(u8 index) {
	u8* base = (u8*)_battleWorkPointer + 0x180C0;
	u8 type = base[index + 0x1C4];

	switch (index) {
		case 0:
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
			if (*slot == 0) {
				*slot = (s32)(base + 0x44 + idx * 0xC0);
			}
			_combine_nozzle_weapon(0, (void*)*slot,
			                       *(u8**)(base + 0x34) + (s8)*(base + 0x1CA) * 0xC0 + 0x10);
		}
	}
	return 2;
}

#pragma dont_inline on
void _Nozzle_Change_Check(u8 index, void* nozzle) {
	u8* base = (u8*)_battleWorkPointer + 0x180C0;
	u8* data = nozzle;
	s32 sum;
	u8 i;

	sum = 0;
	for (i = 0; i < 4; i++) {
		sum += data[i];
	}
	sum = irand(sum);
	for (i = 0; i < 4; i++) {
		sum -= data[i];
		if (sum < 0) {
			break;
		}
	}
	if (i != *(u8*)(base + index + 0x1C4)) {
		s32* slot = (s32*)(base + index * 4 + 0x1CC);
		if (*slot != 0 && !evtCheckID(*slot)) {
			*slot = 0;
		}
		if (*slot == 0) {
			*(u8*)(base + index + 0x1C7) = i;
			{
				EventEntry* e = evtEntry(_nozzle_turn_event, 10, 0);
				*slot = e->eventId;
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
				u8* p = *(u8**)((u8*)unit + 0x14);
				while (p != NULL) {
					*(u32*)(p + 0x1AC) |= 0x10000;
					p = *(u8**)p;
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
USER_FUNC(_bgset_kemuri_effect) {
	BattleWork* wp = _battleWorkPointer;
	s32 layer = evtGetValue(event, *event->args);
	BattleStageObject* obj;
	f32 x, y, z;
	s32 i;

	for (i = 0; i < 32; i++) {
		obj = &wp->stageObjects[i];
		if (obj->id > 0 && obj->data->mLayer == layer) {
			x = obj->mPosition.x;
			y = obj->mPosition.y;
			z = obj->mPosition.z;
			switch (layer) {
				case 0:
					effKemuri1N64Entry(3, x, y, z + 30.0f, 0.0f);
					psndSFXOn_3D(str_802F3CC4, &obj->mPosition);
					break;
				case 1:
					effKemuri1N64Entry(0, x, y, z + 15.0f, 0.0f);
					psndSFXOn_3D(str_802F3CDC, &obj->mPosition);
					break;
				case 2:
					effKemuri1N64Entry(3, x, y, z + 30.0f, 0.0f);
					effKemuri1N64Entry(4, x, y, z + 30.0f, 0.0f);
					psndSFXOn_3D(str_802F3CF4, &obj->mPosition);
					break;
			}
		}
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(_mobj_set_alpha) {
	s32 which = evtGetValue(event, event->args[0]);
	s32 alpha = evtGetValue(event, event->args[1]);
	GXColor color;

	switch (which) {
		case 0:
			if (mapGetMapObj(str_8041CD68) != NULL) {
				*(u32*)&color = dat_8041cce8;
				color.a = alpha;
				mapGrpSetColor(str_8041CD68, &color);
			}
			break;
		case 1:
			if (mapGetMapObj(str_8041CD6C) != NULL) {
				*(u32*)&color = dat_8041ccec;
				color.a = alpha;
				mapGrpSetColor(str_8041CD6C, &color);
			}
			break;
		case 2:
			if (mapGetMapObj(str_8041CD70) != NULL) {
				*(u32*)&color = dat_8041ccf0;
				color.a = alpha;
				mapGrpSetColor(str_8041CD70, &color);
			}
			break;
		case 3:
			if (mapGetMapObj(str_8041CD74) != NULL) {
				*(u32*)&color = dat_8041ccf4;
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
		if (wp->stageObjects[i].id == id) {
			return &wp->stageObjects[i];
		}
	}
	return NULL; //if none of the IDs match
}

BattleStageObject* BattleSearchObjectPtr(const char* name) {
	BattleWork* wp = _battleWorkPointer;
	BattleStageObject* object;
	int i;

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
	BattleStageObject* obj;
	int i, layer;
	u8 position;

	//get the layer this object is on
	layer = evtGetValue(event, *event->args);

	for (i = 0; i < 32; i++) {
		obj = &_battleWorkPointer->stageObjects[i];
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
			obj->mShakeAngle = 0.0f;
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
