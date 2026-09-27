#include "evt/evt_door.h"
#include "pmario_sound.h"
#include "memory.h"
#include "driver/mapdrv.h"
#include "driver/hitdrv.h"
#include "driver/npcdrv.h"
#include "driver/mobjdrv.h"
#include "driver/itemdrv.h"
#include "driver/icondrv.h"
#include "driver/animdrv.h"
#include "mario/mario.h"
#include "party.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include <dolphin/mtx.h>
#include <math.h>
#include "math_trig.h"

s32 DoorFLAG;
s32 door_mapflag;

typedef struct DoorParam {
    s32 field_00;    // 0x00
    s32 field_04;    // 0x04
    s32 field_08;    // 0x08
    s32 field_0C;    // 0x0C
    s32 field_10;    // 0x10
    s32 field_14;    // 0x14
    s32 field_18;    // 0x18
    s32 field_1C;    // 0x1C
    s32 field_20;    // 0x20
    s32 field_24;    // 0x24
    s32 field_28;    // 0x28
    s32 field_2C;    // 0x2C
    s32 field_30;    // 0x30
    char** field_34; // 0x34
    s32 field_38;    // 0x38
    char** field_3C; // 0x3C
    s32 field_40;    // 0x40
    char** field_44; // 0x44
    char** field_48; // 0x48
} DoorParam;

typedef struct DoorWork {
    DoorParam* param; // 0x00
    Vec in;           // 0x04
    Vec out;          // 0x10
} DoorWork;

typedef struct DoorData {
    u32 data[0x48 / 4];
} DoorData;


USER_FUNC(snd_door_in) {
	psndSetFlag(0x100);
	return EVT_RETURN_DONE;
}

USER_FUNC(snd_door_out) {
	psndClearFlag(0x100);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_door_end_wait) {
	return DoorFLAG ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

USER_FUNC(evt_door_set_param) {
	s32* args = event->args;
	DoorParam* param = (DoorParam*)evtGetValue(event, args[0]);
	s32 which = evtGetValue(event, args[1]);
	s32 value = evtGetValue(event, args[2]);

	switch (which) {
	case 0:
		param->field_34 = (char**)value;
		break;
	default:
		break;
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_door_data_copy) {
	s32* args = event->args;
	DoorData* src = (DoorData*)evtGetValue(event, args[0]);
	DoorData* dst = (DoorData*)_mapAlloc(0x48);

	*dst = *src;
	evtSetValue(event, args[1], (s32)dst);
	return EVT_RETURN_DONE;
}

// NONMATCHING (95.29%): 1 instruction differs, 1 extra, plus register differences.
USER_FUNC(npc_hide_onoff) {
	s32* args = event->args;
	DoorParam* param = (DoorParam*)evtGetValue(event, args[0]);
	s32 flag = evtGetValue(event, args[1]);
	char** npcs = param->field_34;
	NpcEntry* npc;

	while (*npcs != NULL) {
		npc = npcNameToPtr_NoAssert(*npcs);
		if (npc != NULL) {
			if (flag != 0) {
				npc->flags |= 0x800;
			} else {
				npc->flags &= ~0x800;
			}
		}
		npcs++;
	}
	return event ? EVT_RETURN_DONE : EVT_RETURN_DONE;
}

USER_FUNC(evt_door_load_mapflag) {
	s32* args = event->args;
	char* name = (char*)evtGetValue(event, args[0]);
	MapObject* obj = mapGetMapObj(name);

	if (obj == NULL) {
		mapErrorEntry(0, name);
		return EVT_RETURN_DONE;
	}
	*(s32*)obj = door_mapflag;
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_door_save_mapflag) {
	s32* args = event->args;
	char* name = (char*)evtGetValue(event, args[0]);
	MapObject* obj = mapGetMapObj(name);

	if (obj == NULL) {
		mapErrorEntry(0, name);
		return EVT_RETURN_DONE;
	}
	door_mapflag = *(s32*)obj;
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_door_param) {
	s32* args = event->args;
	DoorParam* param = (DoorParam*)evtGetValue(event, args[0]);
	s32 mode = evtGetValue(event, args[1]);
	Vec in = { 0 };
	Vec out = { 0 };

	switch (mode) {
	case 0:
		evtSetValue(event, args[2], param->field_04);
		break;
	case 1:
		evtSetValue(event, args[2], param->field_0C);
		evtSetValue(event, args[3], param->field_10);
		evtSetValue(event, args[4], param->field_14);
		break;
	case 2:
		evtSetValue(event, args[2], param->field_18);
		evtSetValue(event, args[3], param->field_1C);
		break;
	case 3:
		evtSetValue(event, args[2], param->field_20);
		evtSetValue(event, args[3], param->field_24);
		break;
	case 4:
		door_position_sub(param->field_04, (char*)param->field_0C, (char*)param->field_10, (char*)param->field_14, &in, &out, 0);
		evtSetValue(event, args[2], (s32)in.x);
		evtSetValue(event, args[3], (s32)in.y);
		evtSetValue(event, args[4], (s32)in.z);
		break;
	case 5:
		door_position_sub(param->field_04, (char*)param->field_0C, (char*)param->field_10, (char*)param->field_14, &in, &out, 0);
		evtSetValue(event, args[2], (s32)out.x);
		evtSetValue(event, args[3], (s32)out.y);
		evtSetValue(event, args[4], (s32)out.z);
		break;
	case 6:
		evtSetValue(event, args[2], param->field_28);
		evtSetValue(event, args[3], param->field_2C);
		evtSetValue(event, args[4], param->field_30);
		break;
	case 7:
		evtSetValue(event, args[2], param->field_40);
		evtSetValue(event, args[3], (s32)param->field_44);
		break;
	case 8:
		evtSetValue(event, args[2], param->field_08);
		break;
	case 9:
		evtSetValue(event, args[2], param->field_00);
		break;
	case 10:
		door_position_sub(param->field_04, (char*)param->field_0C, (char*)param->field_10, (char*)param->field_14, &in, &out, 1);
		evtSetValue(event, args[2], (s32)in.x);
		evtSetValue(event, args[3], (s32)in.y);
		evtSetValue(event, args[4], (s32)in.z);
		break;
	case 11:
		door_position_sub(param->field_04, (char*)param->field_0C, (char*)param->field_10, (char*)param->field_14, &in, &out, 1);
		evtSetValue(event, args[2], (s32)out.x);
		evtSetValue(event, args[3], (s32)out.y);
		evtSetValue(event, args[4], (s32)out.z);
		break;
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(door_dark_flag) {
	s32* args = event->args;
	DoorParam* param = (DoorParam*)evtGetValue(event, args[0]);
	s32 flag = evtGetValue(event, args[1]);

	door_dark_flag_sub(flag, (char**)param->field_34, (char**)param->field_38, (char**)param->field_3C);
	return EVT_RETURN_DONE;
}

void door_dark_flag_sub(s32 flag, char** npcs, char** mobjs, char** items) {
	MarioWork* mario = marioGetPtr();
	PartyEntry* party0 = partyGetPtr(0);
	PartyEntry* party1 = partyGetPtr(1);
	NpcEntry* npc;
	MapObjectEntry* mobj;
	ItemEntry* item;
	s32 i;

	if (flag != 0) {
		if (mario != NULL) {
			animPoseSetMaterialFlagOn(mario->paperAnimGroupId[0], 0x1800);
			animPoseSetMaterialFlagOn(mario->paperAnimGroupId[1], 0x1800);
			animPoseSetMaterialFlagOn(mario->paperAnimGroupId[2], 0x1800);
		}
		if (party0 != NULL) {
			animPoseSetMaterialFlagOn(party0->paperAnimGroup, 0x1800);
		}
		if (party1 != NULL) {
			animPoseSetMaterialFlagOn(party1->paperAnimGroup, 0x1800);
		}
		if (npcs != NULL) {
			for (i = 0; npcs[i] != NULL; i++) {
				npc = npcNameToPtr_NoAssert(npcs[i]);
				if (npc != NULL) {
					animPoseSetMaterialFlagOn(npc->poseId, 0x1800);
				}
			}
		}
		if (mobjs != NULL) {
			for (i = 0; mobjs[i] != NULL; i++) {
				mobj = mobjNameToPtrNoAssert(mobjs[i]);
				if (mobj != NULL) {
					animPoseSetMaterialFlagOn(mobj->poseId, 0x1800);
				}
			}
		}
		if (items != NULL) {
			for (i = 0; items[i] != NULL; i++) {
				item = itemNameToPtr(items[i]);
				if (item != NULL) {
					iconFlagOn(item->name, 0x40);
				}
			}
		}
	} else {
		if (mario != NULL) {
			animPoseSetMaterialFlagOff(mario->paperAnimGroupId[0], 0x1800);
			animPoseSetMaterialFlagOff(mario->paperAnimGroupId[1], 0x1800);
			animPoseSetMaterialFlagOff(mario->paperAnimGroupId[2], 0x1800);
		}
		if (party0 != NULL) {
			animPoseSetMaterialFlagOff(party0->paperAnimGroup, 0x1800);
		}
		if (party1 != NULL) {
			animPoseSetMaterialFlagOff(party1->paperAnimGroup, 0x1800);
		}
		if (npcs != NULL) {
			for (i = 0; npcs[i] != NULL; i++) {
				npc = npcNameToPtr_NoAssert(npcs[i]);
				if (npc != NULL) {
					animPoseSetMaterialFlagOff(npc->poseId, 0x1800);
				}
			}
		}
		if (mobjs != NULL) {
			for (i = 0; mobjs[i] != NULL; i++) {
				mobj = mobjNameToPtrNoAssert(mobjs[i]);
				if (mobj != NULL) {
					animPoseSetMaterialFlagOff(mobj->poseId, 0x1800);
				}
			}
		}
		if (items != NULL) {
			for (i = 0; items[i] != NULL; i++) {
				item = itemNameToPtr(items[i]);
				if (item != NULL) {
					iconFlagOff(item->name, 0x40);
				}
			}
		}
	}
}

void door_position_sub(s32 mode, char* mapName, char* hitIn, char* hitOut, Vec* in, Vec* out, s32 flag) {
	MapObject* mapObj = mapGetMapObj(mapName);
	Vec pos;
	Vec normal;
	Vec v20;
	Vec v14;
	Vec v8;
	Mtx m1;
	Mtx m2;
	f32 f31 = 10.0f;
	f32 f30;
	f32 f29;

	if (flag == 0) {
		f30 = 40.0f;
		f29 = 30.0f;
	} else {
		f30 = 50.0f;
		f29 = f30;
	}

	if (mapObj != NULL) {
		PSMTXInverse(mapObj->unkAC, m1);
		PSMTXConcat(mapObj->modelWorldMtx, m1, m2);
		PSMTXMultVec(m2, &mapObj->joints->bboxMin, &v14);
		PSMTXMultVec(m2, &mapObj->joints->bboxMax, &v20);
		PSVECSubtract(&v20, &v14, &v8);
		{
			f32 len;
			f32 half = 0.5f;
			if (v8.x) {
				f32 angle = atan(v8.z / v8.x);
				f32 ratio = v8.x / (f32)cos(angle);
				len = ratio * half;
			} else {
				len = v8.z * half;
			}
			f31 = len;
		}
	}
	switch (mode) {
	case 0:
	case 1:
		if (hitOut != NULL) {
			if (flag == 0) {
				hitObjGetPos(hitOut, &pos);
			} else {
				pos = marioGetPtr()->position;
			}
			hitObjGetNormal(hitOut, &normal);
			out->x = pos.x + normal.x * f30 - normal.z * f31;
			out->z = pos.z + normal.z * f30 + normal.x * f31;
		} else {
			*out = marioGetPtr()->position;
		}
		if (hitIn != NULL) {
			if (flag == 0) {
				hitObjGetPos(hitIn, &pos);
			} else {
				pos = marioGetPtr()->position;
			}
			hitObjGetNormal(hitIn, &normal);
			in->x = pos.x + normal.x * f29;
			in->z = pos.z + normal.z * f29;
		} else {
			*in = marioGetPtr()->position;
		}
		break;
	case 2:
		if (hitOut != NULL) {
			if (flag == 0) {
				hitObjGetPos(hitOut, &pos);
			} else {
				pos = marioGetPtr()->position;
			}
			hitObjGetNormal(hitOut, &normal);
			out->x = pos.x + normal.x * f30 + normal.z * f31;
			out->z = pos.z + normal.z * f30 - normal.x * f31;
		} else {
			*out = marioGetPtr()->position;
		}
		if (hitIn != NULL) {
			if (flag == 0) {
				hitObjGetPos(hitIn, &pos);
			} else {
				pos = marioGetPtr()->position;
			}
			hitObjGetNormal(hitIn, &normal);
			in->x = pos.x + normal.x * f29;
			in->z = pos.z + normal.z * f29;
		} else {
			*in = marioGetPtr()->position;
		}
		break;
	case 3:
		if (hitOut != NULL) {
			if (flag == 0) {
				hitObjGetPos(hitOut, &pos);
			} else {
				pos = marioGetPtr()->position;
			}
			hitObjGetNormal(hitOut, &normal);
			out->x = pos.x + normal.x * f30;
			out->z = pos.z + normal.z * f30;
		} else {
			*out = marioGetPtr()->position;
		}
		if (hitIn != NULL) {
			if (flag == 0) {
				hitObjGetPos(hitIn, &pos);
			} else {
				pos = marioGetPtr()->position;
			}
			hitObjGetNormal(hitIn, &normal);
			in->x = pos.x + normal.x * f30;
			in->z = pos.z + normal.z * f30;
		} else {
			*in = marioGetPtr()->position;
		}
		break;
	}
}

USER_FUNC(get_noclip_map) {
	s32* args = event->args;
	DoorWork* work = (DoorWork*)evtGetValue(event, args[0]);
	s32 index = evtGetValue(event, args[1]);
	void** list = (void**)work->param->field_40;

	if (list != NULL) {
		void* value = list[index];
		if (value != NULL) {
			evtSetValue(event, args[2], (s32)value);
			return EVT_RETURN_DONE;
		}
	}
	evtSetValue(event, args[2], -1);
	return EVT_RETURN_DONE;
}

USER_FUNC(npc_light) {
	s32* args = event->args;
	DoorWork* work = (DoorWork*)evtGetValue(event, args[0]);
	MarioWork* mario;
	PartyEntry* party0;
	PartyEntry* party1;
	NpcEntry* npc;
	MapObjectEntry* mobj;
	ItemEntry* item;
	char** list;
	s32 i;

	mario = marioGetPtr();
	party0 = partyGetPtr(0);
	party1 = partyGetPtr(1);
	if (mario != NULL) {
		animPoseSetMaterialFlagOff(marioGetPtr()->paperAnimGroupId[0], 0x1800);
		animPoseSetMaterialFlagOff(marioGetPtr()->paperAnimGroupId[1], 0x1800);
		animPoseSetMaterialFlagOff(marioGetPtr()->paperAnimGroupId[2], 0x1800);
	}
	if (party0 != NULL) {
		animPoseSetMaterialFlagOff(party0->paperAnimGroup, 0x1800);
	}
	if (party1 != NULL) {
		animPoseSetMaterialFlagOff(party1->paperAnimGroup, 0x1800);
	}
	if (work->param->field_3C != NULL) {
		list = work->param->field_3C;
		for (i = 0; list[i] != NULL; i++) {
			npc = npcNameToPtr_NoAssert(list[i]);
			if (npc != NULL) {
				animPoseSetMaterialFlagOff(npc->poseId, 0x1800);
			}
		}
	}
	if (work->param->field_44 != NULL) {
		list = work->param->field_44;
		for (i = 0; list[i] != NULL; i++) {
			mobj = mobjNameToPtrNoAssert(list[i]);
			if (mobj != NULL) {
				animPoseSetMaterialFlagOff(mobj->poseId, 0x1800);
			}
		}
	}
	if (work->param->field_48 != NULL) {
		list = work->param->field_48;
		for (i = 0; list[i] != NULL; i++) {
			item = itemNameToPtr(list[i]);
			if (item != NULL) {
				iconFlagOff(item->name, 0x40);
			}
		}
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(npc_dark) {
	s32* args = event->args;
	DoorWork* work = (DoorWork*)evtGetValue(event, args[0]);
	MarioWork* mario;
	PartyEntry* party0;
	PartyEntry* party1;
	NpcEntry* npc;
	MapObjectEntry* mobj;
	ItemEntry* item;
	char** list;
	s32 i;

	mario = marioGetPtr();
	party0 = partyGetPtr(0);
	party1 = partyGetPtr(1);
	if (mario != NULL) {
		animPoseSetMaterialFlagOn(marioGetPtr()->paperAnimGroupId[0], 0x1800);
		animPoseSetMaterialFlagOn(marioGetPtr()->paperAnimGroupId[1], 0x1800);
		animPoseSetMaterialFlagOn(marioGetPtr()->paperAnimGroupId[2], 0x1800);
	}
	if (party0 != NULL) {
		animPoseSetMaterialFlagOn(party0->paperAnimGroup, 0x1800);
	}
	if (party1 != NULL) {
		animPoseSetMaterialFlagOn(party1->paperAnimGroup, 0x1800);
	}
	if (work->param->field_3C != NULL) {
		list = work->param->field_3C;
		for (i = 0; list[i] != NULL; i++) {
			npc = npcNameToPtr_NoAssert(list[i]);
			if (npc != NULL) {
				animPoseSetMaterialFlagOn(npc->poseId, 0x1800);
			}
		}
	}
	if (work->param->field_44 != NULL) {
		list = work->param->field_44;
		for (i = 0; list[i] != NULL; i++) {
			mobj = mobjNameToPtrNoAssert(list[i]);
			if (mobj != NULL) {
				animPoseSetMaterialFlagOn(mobj->poseId, 0x1800);
			}
		}
	}
	if (work->param->field_48 != NULL) {
		list = work->param->field_48;
		for (i = 0; list[i] != NULL; i++) {
			item = itemNameToPtr(list[i]);
			if (item != NULL) {
				iconFlagOn(item->name, 0x40);
			}
		}
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(animation) {
	s32* args = event->args;
	DoorWork* work = (DoorWork*)evtGetValue(event, args[0]);

	evtSetValue(event, args[1], work->param->field_14);
	evtSetValue(event, args[2], work->param->field_18);
	evtSetValue(event, args[3], work->param->field_1C);
	evtSetValue(event, args[4], work->param->field_20);
	return EVT_RETURN_DONE;
}

USER_FUNC(init_inside_group) {
	s32* args = event->args;
	DoorWork* work = (DoorWork*)evtGetValue(event, args[0]);

	evtSetValue(event, args[1], (s32)work->param->field_34);
	evtSetValue(event, args[2], work->param->field_38);
	return EVT_RETURN_DONE;
}

USER_FUNC(inside_group) {
	s32* args = event->args;
	DoorWork* work = (DoorWork*)evtGetValue(event, args[0]);

	evtSetValue(event, args[1], work->param->field_2C);
	evtSetValue(event, args[2], work->param->field_30);
	return EVT_RETURN_DONE;
}

USER_FUNC(door_group) {
	s32* args = event->args;
	DoorWork* work = (DoorWork*)evtGetValue(event, args[0]);

	evtSetValue(event, args[1], work->param->field_28);
	return EVT_RETURN_DONE;
}

USER_FUNC(outside_group) {
	s32* args = event->args;
	DoorWork* work = (DoorWork*)evtGetValue(event, args[0]);

	evtSetValue(event, args[1], work->param->field_24);
	return EVT_RETURN_DONE;
}

USER_FUNC(in_pos) {
	s32* args = event->args;
	DoorWork* work = (DoorWork*)evtGetValue(event, args[0]);

	evtSetValue(event, args[1], (s32)work->in.x);
	evtSetValue(event, args[2], (s32)work->in.z);
	return EVT_RETURN_DONE;
}

USER_FUNC(out_pos) {
	s32* args = event->args;
	DoorWork* work = (DoorWork*)evtGetValue(event, args[0]);

	evtSetValue(event, args[1], (s32)work->out.x);
	evtSetValue(event, args[2], (s32)work->out.z);
	return EVT_RETURN_DONE;
}

USER_FUNC(inout_hit) {
	s32* args = event->args;
	DoorWork* work = (DoorWork*)evtGetValue(event, args[0]);

	evtSetValue(event, args[1], work->param->field_04);
	evtSetValue(event, args[2], work->param->field_08);
	evtSetValue(event, args[3], work->param->field_0C);
	evtSetValue(event, args[4], work->param->field_10);
	return EVT_RETURN_DONE;
}

USER_FUNC(door_entry) {
	s32* args = event->args;
	DoorParam* param = (DoorParam*)evtGetValue(event, args[0]);
	Vec a = { 0 };
	Vec b = { 0 };
	DoorWork* work = (DoorWork*)_mapAlloc(sizeof(DoorWork));

	work->param = param;
	switch (param->field_00) {
	case 0:
	case 1:
		hitObjGetPos((char*)param->field_08, &a);
		hitObjGetNormal((char*)param->field_08, &b);
		work->out.x = a.x + b.x * 40.0f - b.z * 10.0f;
		work->out.z = a.z + b.z * 40.0f + b.x * 10.0f;
		hitObjGetPos((char*)param->field_10, &a);
		hitObjGetNormal((char*)param->field_10, &b);
		work->in.x = a.x + b.x * 30.0f;
		work->in.z = a.z + b.z * 30.0f;
		break;
	case 2:
		hitObjGetPos((char*)param->field_08, &a);
		hitObjGetNormal((char*)param->field_08, &b);
		work->out.x = a.x + b.x * 40.0f + b.z * 10.0f;
		work->out.z = a.z + b.z * 40.0f - b.x * 10.0f;
		hitObjGetPos((char*)param->field_10, &a);
		hitObjGetNormal((char*)param->field_10, &b);
		work->in.x = a.x + b.x * 30.0f;
		work->in.z = a.z + b.z * 30.0f;
		break;
	case 3:
		hitObjGetPos((char*)param->field_08, &a);
		hitObjGetNormal((char*)param->field_08, &b);
		work->out.x = a.x + b.x * 40.0f;
		work->out.z = a.z + b.z * 40.0f;
		hitObjGetPos((char*)param->field_10, &a);
		hitObjGetNormal((char*)param->field_10, &b);
		work->in.x = a.x + b.x * 40.0f;
		work->in.z = a.z + b.z * 40.0f;
		break;
	}
	evtSetValue(event, args[1], (s32)work);
	return EVT_RETURN_DONE;
}
