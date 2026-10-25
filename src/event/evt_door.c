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
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_cmd.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"
#include "party/party_motion.h"

s32 DoorFLAG;
s32 door_mapflag;

typedef struct DoorWork {
    DoorParam* param; // 0x00
    Vec in;           // 0x04
    Vec out;          // 0x10
} DoorWork;

extern s32 N_evt_mario_party_door_halve_hitbox[];
s32 door_up_se[2];
s32 house_close_se[4];

// clang-format off
USER_FUNC(snd_door_in) {
	psndSetFlag(0x100);
	return EVT_RETURN_DONE;
}


USER_FUNC(snd_door_out) {
	psndClearFlag(0x100);
	return EVT_RETURN_DONE;
}

EVT_BEGIN(door_open_se)
    CALL(evt_door_param, LW(0), 8, LW(1))
    CALL(evt_door_param, LW(0), 1, LW(2), LW(3), LW(4))
    CALL(evt_hit_get_position, LW(3), LW(10), LW(11), LW(12))
    SWITCH(LW(1))
        CASE_EQUAL(0)
        CASE_EQUAL(1)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN1"), LW(10), LW(11), LW(12), 0)
        CASE_EQUAL(2)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_WOOD1"), LW(10), LW(11), LW(12), 0)
        CASE_EQUAL(3)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_WOOD2"), LW(10), LW(11), LW(12), 0)
        CASE_EQUAL(4)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_WOOD3"), LW(10), LW(11), LW(12), 0)
        CASE_EQUAL(5)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_IRON1"), LW(10), LW(11), LW(12), 0)
        CASE_EQUAL(6)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_IRON2"), LW(10), LW(11), LW(12), 0)
        CASE_EQUAL(7)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_GRILLE1"), LW(10), LW(11), LW(12), 0)
        CASE_EQUAL(8)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_SLIDE1"), LW(10), LW(11), LW(12), 0)
        CASE_EQUAL(9)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_TURN1"), LW(10), LW(11), LW(12), 0)
        CASE_EQUAL(10)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_ELEC1"), LW(10), LW(11), LW(12), 0)
        CASE_EQUAL(11)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_STONE1"), LW(10), LW(11), LW(12), 0)
        CASE_EQUAL(12)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_CASTLE1"), LW(10), LW(11), LW(12), 0)
        CASE_EQUAL(13)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_CASTLE2"), LW(10), LW(11), LW(12), 0)
        CASE_EQUAL(14)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_METAL1"), LW(10), LW(11), LW(12), 0)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(door_close_se)
    CALL(evt_door_param, LW(0), 8, LW(1))
    CALL(evt_door_param, LW(0), 1, LW(2), LW(3), LW(4))
    CALL(evt_hit_get_position, LW(3), LW(10), LW(11), LW(12))
    SWITCH(LW(1))
        CASE_EQUAL(0)
        CASE_EQUAL(1)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT1"), LW(10), LW(11), LW(12), 0)
        CASE_EQUAL(2)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_WOOD1"), LW(10), LW(11), LW(12), 0)
        CASE_EQUAL(3)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_WOOD2"), LW(10), LW(11), LW(12), 0)
        CASE_EQUAL(4)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_WOOD2"), LW(10), LW(11), LW(12), 0)
        CASE_EQUAL(5)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_IRON1"), LW(10), LW(11), LW(12), 0)
        CASE_EQUAL(6)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_IRON2"), LW(10), LW(11), LW(12), 0)
        CASE_EQUAL(7)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_GRILLE1"), LW(10), LW(11), LW(12), 0)
        CASE_EQUAL(8)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_SLIDE1"), LW(10), LW(11), LW(12), 0)
        CASE_EQUAL(9)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_TURN1"), LW(10), LW(11), LW(12), 0)
        CASE_EQUAL(10)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_ELEC1"), LW(10), LW(11), LW(12), 0)
        CASE_EQUAL(11)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_STONE1"), LW(10), LW(11), LW(12), 0)
        CASE_EQUAL(12)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_CASTLE1"), LW(10), LW(11), LW(12), 0)
        CASE_EQUAL(13)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_CASTLE2"), LW(10), LW(11), LW(12), 0)
        CASE_EQUAL(14)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_METAL1"), LW(10), LW(11), LW(12), 0)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(door_down_se)
    CALL(evt_door_param, LW(0), 8, LW(1))
    CALL(evt_door_param, LW(0), 1, LW(2), LW(3), LW(4))
    CALL(evt_hit_get_position, LW(3), LW(10), LW(11), LW(12))
    SWITCH(LW(1))
        CASE_EQUAL(0)
        CASE_DEFAULT
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_FALL1"), LW(10), LW(11), LW(12), 0)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(house_open_se)
    CALL(evt_door_param, LW(0), 9, LW(1))
    IF_BITS_SET(LW(1), 2)
        RETURN
    END_IF
    CALL(evt_door_param, LW(0), 1, LW(2), LW(3), LW(4))
    CALL(evt_hit_get_position, LW(3), LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING("SFX_HOUSE_OPEN1"), LW(10), LW(11), LW(12), 0)
    RETURN
EVT_END()


USER_FUNC(door_entry) {
	s32* args = event->args;
	DoorParam* param = (DoorParam*)evtGetValue(event, args[0]);
	Vec a = { 0 };
	Vec b = { 0 };
	DoorWork* work = _mapAlloc(sizeof(DoorWork));

	work->param = param;
	switch (param->type) {
	case 0:
	case 1:
		hitObjGetPos(param->outsideHit, &a);
		hitObjGetNormal(param->outsideHit, &b);
		work->out.x = a.x + b.x * 40.0f - b.z * 10.0f;
		work->out.z = a.z + b.z * 40.0f + b.x * 10.0f;
		hitObjGetPos(param->insideHit, &a);
		hitObjGetNormal(param->insideHit, &b);
		work->in.x = a.x + b.x * 30.0f;
		work->in.z = a.z + b.z * 30.0f;
		break;
	case 2:
		hitObjGetPos(param->outsideHit, &a);
		hitObjGetNormal(param->outsideHit, &b);
		work->out.x = a.x + b.x * 40.0f + b.z * 10.0f;
		work->out.z = a.z + b.z * 40.0f - b.x * 10.0f;
		hitObjGetPos(param->insideHit, &a);
		hitObjGetNormal(param->insideHit, &b);
		work->in.x = a.x + b.x * 30.0f;
		work->in.z = a.z + b.z * 30.0f;
		break;
	case 3:
		hitObjGetPos(param->outsideHit, &a);
		hitObjGetNormal(param->outsideHit, &b);
		work->out.x = a.x + b.x * 40.0f;
		work->out.z = a.z + b.z * 40.0f;
		hitObjGetPos(param->insideHit, &a);
		hitObjGetNormal(param->insideHit, &b);
		work->in.x = a.x + b.x * 40.0f;
		work->in.z = a.z + b.z * 40.0f;
		break;
	}
	evtSetValue(event, args[1], (s32)work);
	return EVT_RETURN_DONE;
}


USER_FUNC(inout_hit) {
	s32* args = event->args;
	DoorWork* work = (DoorWork*)evtGetValue(event, args[0]);

	evtSetValue(event, args[1], work->param->outsideConditionId);
	evtSetValue(event, args[2], (s32)work->param->outsideHit);
	evtSetValue(event, args[3], work->param->insideConditionId);
	evtSetValue(event, args[4], (s32)work->param->insideHit);
	return EVT_RETURN_DONE;
}


USER_FUNC(out_pos) {
	s32* args = event->args;
	DoorWork* work = (DoorWork*)evtGetValue(event, args[0]);

	evtSetValue(event, args[1], (s32)work->out.x);
	evtSetValue(event, args[2], (s32)work->out.z);
	return EVT_RETURN_DONE;
}


USER_FUNC(in_pos) {
	s32* args = event->args;
	DoorWork* work = (DoorWork*)evtGetValue(event, args[0]);

	evtSetValue(event, args[1], (s32)work->in.x);
	evtSetValue(event, args[2], (s32)work->in.z);
	return EVT_RETURN_DONE;
}


USER_FUNC(outside_group) {
	s32* args = event->args;
	DoorWork* work = (DoorWork*)evtGetValue(event, args[0]);

	evtSetValue(event, args[1], (s32)work->param->outsideGroup);
	return EVT_RETURN_DONE;
}


USER_FUNC(door_group) {
	s32* args = event->args;
	DoorWork* work = (DoorWork*)evtGetValue(event, args[0]);

	evtSetValue(event, args[1], (s32)work->param->doorGroup);
	return EVT_RETURN_DONE;
}


USER_FUNC(inside_group) {
	s32* args = event->args;
	DoorWork* work = (DoorWork*)evtGetValue(event, args[0]);

	evtSetValue(event, args[1], (s32)work->param->insideGroup);
	evtSetValue(event, args[2], (s32)work->param->insideHitGroup);
	return EVT_RETURN_DONE;
}


USER_FUNC(init_inside_group) {
	s32* args = event->args;
	DoorWork* work = (DoorWork*)evtGetValue(event, args[0]);

	evtSetValue(event, args[1], (s32)work->param->initInsideGroup);
	evtSetValue(event, args[2], (s32)work->param->initInsideHitGroup);
	return EVT_RETURN_DONE;
}


USER_FUNC(animation) {
	s32* args = event->args;
	DoorWork* work = (DoorWork*)evtGetValue(event, args[0]);

	evtSetValue(event, args[1], (s32)work->param->openAnim);
	evtSetValue(event, args[2], (s32)work->param->field_0x18);
	evtSetValue(event, args[3], (s32)work->param->closeAnim);
	evtSetValue(event, args[4], (s32)work->param->field_0x20);
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
	const char** list;
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
	if (work->param->npcNames != NULL) {
		list = work->param->npcNames;
		for (i = 0; list[i] != NULL; i++) {
			npc = npcNameToPtr_NoAssert(list[i]);
			if (npc != NULL) {
				animPoseSetMaterialFlagOn(npc->poseId, 0x1800);
			}
		}
	}
	if (work->param->mobjNames != NULL) {
		list = work->param->mobjNames;
		for (i = 0; list[i] != NULL; i++) {
			mobj = mobjNameToPtrNoAssert(list[i]);
			if (mobj != NULL) {
				animPoseSetMaterialFlagOn(mobj->poseId, 0x1800);
			}
		}
	}
	if (work->param->itemNames != NULL) {
		list = work->param->itemNames;
		for (i = 0; list[i] != NULL; i++) {
			item = itemNameToPtr(list[i]);
			if (item != NULL) {
				iconFlagOn(item->name, 0x40);
			}
		}
	}
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
	const char** list;
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
	if (work->param->npcNames != NULL) {
		list = work->param->npcNames;
		for (i = 0; list[i] != NULL; i++) {
			npc = npcNameToPtr_NoAssert(list[i]);
			if (npc != NULL) {
				animPoseSetMaterialFlagOff(npc->poseId, 0x1800);
			}
		}
	}
	if (work->param->mobjNames != NULL) {
		list = work->param->mobjNames;
		for (i = 0; list[i] != NULL; i++) {
			mobj = mobjNameToPtrNoAssert(list[i]);
			if (mobj != NULL) {
				animPoseSetMaterialFlagOff(mobj->poseId, 0x1800);
			}
		}
	}
	if (work->param->itemNames != NULL) {
		list = work->param->itemNames;
		for (i = 0; list[i] != NULL; i++) {
			item = itemNameToPtr(list[i]);
			if (item != NULL) {
				iconFlagOff(item->name, 0x40);
			}
		}
	}
	return EVT_RETURN_DONE;
}


USER_FUNC(get_noclip_map) {
	s32* args = event->args;
	DoorWork* work = (DoorWork*)evtGetValue(event, args[0]);
	s32 index = evtGetValue(event, args[1]);
	const char** list = work->param->noclipMaps;

	if (list != NULL) {
		const char* value = list[index];
		if (value != NULL) {
			evtSetValue(event, args[2], (s32)value);
			return EVT_RETURN_DONE;
		}
	}
	evtSetValue(event, args[2], -1);
	return EVT_RETURN_DONE;
}


void door_position_sub(s32 mode, const char* mapName, const char* hitIn, const char* hitOut, Vec* in, Vec* out, s32 flag) {
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
		PSMTXInverse(mapObj->userMtx, m1);
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


void door_dark_flag_sub(s32 flag, const char** npcs, const char** mobjs, const char** items) {
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


USER_FUNC(door_dark_flag) {
	s32* args = event->args;
	DoorEntry* param = (DoorEntry*)evtGetValue(event, args[0]);
	s32 flag = evtGetValue(event, args[1]);

	door_dark_flag_sub(flag, param->npcNames, param->mobjNames, param->itemNames);
	return EVT_RETURN_DONE;
}

EVT_BEGIN(evt_dark)
    CALL(evt_sub_intpl_msec_init, 11, 255, 0, 300)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_map_set_blend, 0, LW(0), LW(0), LW(0), 255)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_map_set_blend, 0, LW(0), LW(0), LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_dark2)
    CALL(evt_sub_intpl_msec_init, 11, 255, 0, 300)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_map_set_blend, 1, LW(0), LW(0), LW(0), 255)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_map_set_blend, 1, LW(0), LW(0), LW(0), 0)
    RETURN
EVT_END()

static EVT_BEGIN(evt_light)
    CALL(evt_sub_intpl_msec_init, 11, 0, 255, 300)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_map_set_blend, 0, LW(0), LW(0), LW(0), 255)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_map_blend_off, 0)
    RETURN
EVT_END()

EVT_BEGIN(evt_light2)
    CALL(evt_sub_intpl_msec_init, 11, 0, 255, 300)
    LOOP(0)
        CALL(evt_sub_intpl_msec_get_value)
        CALL(evt_map_set_blend, 1, LW(0), LW(0), LW(0), 255)
        IF_INT_EQUAL(LW(1), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_map_blend_off, 1)
    RETURN
EVT_END()

EVT_BEGIN(in_houseanim)
    WAIT_MS(500)
    CALL(animation, LW(10), LW(0), LW(1), LW(2), LW(3))
    IF_INT_NOT_EQUAL(LW(0), 0)
        BEGIN_INLINE_EVENT
            IF_STR_EQUAL(LW(0), STRING("S_mise_open"))
                CALL(door_group, LW(10), LW(0))
                CALL(evt_mapobj_get_position, LW(0), LW(2), LW(3), LW(4))
                CALL(evt_snd_sfxon_3d, STRING("SFX_HOUSE_OPEN1"), LW(2), LW(3), LW(4), 0)
                LOOP(4)
                    WAIT_MS(166)
                    CALL(evt_snd_sfxon_3d, STRING("SFX_HOUSE_DOWN1"), LW(2), LW(3), LW(4), 0)
                END_LOOP
            ELSE
                CALL(door_group, LW(10), LW(0))
                CALL(evt_mapobj_get_position, LW(0), LW(2), LW(3), LW(4))
                CALL(evt_snd_sfxon_3d, STRING("SFX_HOUSE_OPEN1"), LW(2), LW(3), LW(4), 0)
            END_IF
        END_INLINE_EVENT
        CALL(evt_map_playanim, LW(0), 0, 0)
        CALL(evt_map_checkanim, LW(0), LW(4), LW(5))
        WAIT_MS(LW(5))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(out_houseanim)
    CALL(animation, LW(10), LW(0), LW(1), LW(2), LW(3))
    IF_INT_NOT_EQUAL(LW(2), 0)
        BEGIN_INLINE_EVENT
            IF_STR_EQUAL(LW(2), STRING("S_mise_close"))
                CALL(door_group, LW(10), LW(0))
                CALL(evt_mapobj_get_position, LW(0), LW(2), LW(3), LW(4))
                LOOP(4)
                    CALL(evt_snd_sfxon_3d, STRING("SFX_HOUSE_DOWN1"), LW(2), LW(3), LW(4), 0)
                    WAIT_MS(166)
                END_LOOP
            END_IF
        END_INLINE_EVENT
        CALL(evt_map_playanim, LW(2), 0, 0)
        CALL(evt_map_checkanim, LW(2), LW(4), LW(5))
        WAIT_MS(LW(5))
        BEGIN_INLINE_EVENT
            CALL(door_group, LW(10), LW(0))
            CALL(evt_mapobj_get_position, LW(0), LW(2), LW(3), LW(4))
            CALL(evt_snd_sfxon_3d, STRING("SFX_HOUSE_SHUT1"), LW(2), LW(3), LW(4), 0)
        END_INLINE_EVENT
    END_IF
    RETURN
EVT_END()


USER_FUNC(evt_door_param) {
	s32* args = event->args;
	DoorEntry* param = (DoorEntry*)evtGetValue(event, args[0]);
	s32 mode = evtGetValue(event, args[1]);
	Vec in = { 0 };
	Vec out = { 0 };

	switch (mode) {
	case 0:
		evtSetValue(event, args[2], param->type);
		break;
	case 1:
		evtSetValue(event, args[2], (s32)param->doorMapObj);
		evtSetValue(event, args[3], (s32)param->closeHit);
		evtSetValue(event, args[4], (s32)param->openHit);
		break;
	case 2:
		evtSetValue(event, args[2], (s32)param->outsideMapObj);
		evtSetValue(event, args[3], (s32)param->outsideHit);
		break;
	case 3:
		evtSetValue(event, args[2], (s32)param->insideMapObj);
		evtSetValue(event, args[3], (s32)param->insideHit);
		break;
	case 4:
		door_position_sub(param->type, param->doorMapObj, param->closeHit, param->openHit, &in, &out, 0);
		evtSetValue(event, args[2], (s32)in.x);
		evtSetValue(event, args[3], (s32)in.y);
		evtSetValue(event, args[4], (s32)in.z);
		break;
	case 5:
		door_position_sub(param->type, param->doorMapObj, param->closeHit, param->openHit, &in, &out, 0);
		evtSetValue(event, args[2], (s32)out.x);
		evtSetValue(event, args[3], (s32)out.y);
		evtSetValue(event, args[4], (s32)out.z);
		break;
	case 6:
		evtSetValue(event, args[2], (s32)param->doorEvt);
		evtSetValue(event, args[3], (s32)param->downEvt);
		evtSetValue(event, args[4], (s32)param->houseEvt);
		break;
	case 7:
		evtSetValue(event, args[2], (s32)param->openEvt);
		evtSetValue(event, args[3], (s32)param->closeEvt);
		break;
	case 8:
		evtSetValue(event, args[2], param->sfxType);
		break;
	case 9:
		evtSetValue(event, args[2], param->flags);
		break;
	case 10:
		door_position_sub(param->type, param->doorMapObj, param->closeHit, param->openHit, &in, &out, 1);
		evtSetValue(event, args[2], (s32)in.x);
		evtSetValue(event, args[3], (s32)in.y);
		evtSetValue(event, args[4], (s32)in.z);
		break;
	case 11:
		door_position_sub(param->type, param->doorMapObj, param->closeHit, param->openHit, &in, &out, 1);
		evtSetValue(event, args[2], (s32)out.x);
		evtSetValue(event, args[3], (s32)out.y);
		evtSetValue(event, args[4], (s32)out.z);
		break;
	}
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
	door_mapflag = obj->flags;
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_door_load_mapflag) {
	s32* args = event->args;
	char* name = (char*)evtGetValue(event, args[0]);
	MapObject* obj = mapGetMapObj(name);

	if (obj == NULL) {
		mapErrorEntry(0, name);
		return EVT_RETURN_DONE;
	}
	obj->flags = door_mapflag;
	return EVT_RETURN_DONE;
}


USER_FUNC(npc_hide_onoff) {
	s32* args = event->args;
	DoorEntry* param = (DoorEntry*)evtGetValue(event, args[0]);
	s32 flag = evtGetValue(event, args[1]);
	const char** npcs = param->npcNames;
	NpcEntry* npc;
	s32 i;

	for (i = 0; npcs[i] != NULL; i++) {
		npc = npcNameToPtr_NoAssert(npcs[i]);
		if (npc != NULL) {
			if (flag != 0) {
				npc->flags |= 0x800;
			} else {
				npc->flags &= ~0x800;
			}
		}
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_door_data_copy) {
	s32* args = event->args;
	DoorEntry* src = (DoorEntry*)evtGetValue(event, args[0]);
	DoorEntry* dst = _mapAlloc(0x48);

	*dst = *src;
	evtSetValue(event, args[1], (s32)dst);
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_door_set_param) {
	s32* args = event->args;
	DoorEntry* param = (DoorEntry*)evtGetValue(event, args[0]);
	s32 which = evtGetValue(event, args[1]);
	s32 value = evtGetValue(event, args[2]);

	switch (which) {
	case 0:
		param->npcNames = (const char**)value;
		break;
	default:
		break;
	}
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_door_end_wait) {
	return DoorFLAG ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

EVT_BEGIN(in_dooranim)
    CALL(door_group, LW(10), LW(9))
    BEGIN_INLINE_EVENT
        CALL(evt_mapobj_get_position, LW(9), LW(2), LW(3), LW(4))
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN1"), LW(2), LW(3), LW(4), 0)
        WAIT_MS(1500)
        CALL(evt_mapobj_get_position, LW(9), LW(2), LW(3), LW(4))
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT1"), LW(2), LW(3), LW(4), 0)
        WAIT_MS(500)
        CALL(evt_mapobj_get_position, LW(9), LW(2), LW(3), LW(4))
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_FALL1"), LW(2), LW(3), LW(4), 0)
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 5, 0, 80, 500)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
    CALL(evt_mapobj_rotate, 1, LW(9), 0, LW(0), 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    WAIT_MS(500)
    CALL(evt_sub_intpl_msec_init, 2, 80, 0, 500)
    LABEL(1)
    CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
    CALL(evt_mapobj_rotate, 1, LW(9), 0, LW(0), 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(1)
    END_IF
    CALL(evt_sub_intpl_msec_init, 2, 0, 90, 500)
    LABEL(2)
    CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
    CALL(evt_mapobj_rotate, 1, LW(9), LW(0), 0, 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(2)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(out_dooranim)
    CALL(door_group, LW(10), LW(9))
    BEGIN_INLINE_EVENT
        WAIT_MS(500)
        CALL(evt_mapobj_get_position, LW(9), LW(2), LW(3), LW(4))
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN1"), LW(2), LW(3), LW(4), 0)
        WAIT_MS(1000)
        CALL(evt_mapobj_get_position, LW(9), LW(2), LW(3), LW(4))
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT1"), LW(2), LW(3), LW(4), 0)
    END_INLINE_EVENT
    CALL(evt_sub_intpl_msec_init, 5, 90, 0, 500)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
    CALL(evt_mapobj_rotate, 1, LW(9), LW(0), 0, 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_sub_intpl_msec_init, 5, 0, 80, 500)
    LABEL(1)
    CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
    CALL(evt_mapobj_rotate, 1, LW(9), 0, LW(0), 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(1)
    END_IF
    WAIT_MS(500)
    CALL(evt_sub_intpl_msec_init, 5, 80, 0, 500)
    LABEL(2)
    CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
    CALL(evt_mapobj_rotate, 1, LW(9), 0, LW(0), 0)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(2)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(house_in)
    PEEK_INT(LW(0), PTR(&DoorFLAG))
    IF_INT_EQUAL(LW(0), 1)
        RETURN
    END_IF
    POKE_INT(1, PTR(&DoorFLAG))
    CALL(evt_snd_bgmoff, 18432)
    CALL(evt_snd_envoff, 18432)
    CALL(evt_snd_env_lpf, 0, 800)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam_letter_box_disable, 1)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(in_pos, LW(10), LW(0), LW(2))
        ADD(LW(1), 1)
        CALL(evt_cam3d_evt_set_xyz, LW(0), LW(1), LW(2), 0, 150, 11)
    END_INLINE_EVENT
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(get_noclip_map, LW(10), LW(0), LW(1))
        IF_INT_EQUAL(LW(1), -1)
            BREAK_LOOP
        END_IF
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 2)
        ADD(LW(0), 1)
    END_LOOP
    CALL(outside_group, LW(10), LW(0))
    CALL(evt_mapobj_flag_onoff, 1, 1, LW(0), 2048)
    CALL(door_group, LW(10), LW(0))
    CALL(evt_mapobj_flag_onoff, 1, 1, LW(0), 2048)
    CALL(evt_mapobj_flag_onoff, 1, 1, LW(0), 4096)
    CALL(inside_group, LW(10), LW(0), LW(1))
    CALL(evt_mapobj_flag_onoff, 1, 0, LW(0), 1)
    CALL(evt_hitobj_onoff, LW(1), 1, 1)
    CALL(evt_mapobj_flag_onoff, 1, 1, LW(0), 2048)
    CALL(evt_mapobj_flag_onoff, 1, 1, LW(0), 4096)
    CALL(N_evt_mario_party_door_halve_hitbox)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(out_pos, LW(10), LW(0), LW(2))
    CALL(evt_mario_mov_pos2, LW(0), LW(2), FLOAT(135.0f))
    CALL(evt_party_move_beside_mario, 0)
    CALL(evt_party_move_beside_mario, 1)
    SPAWN_EVENT_GET_ID(PTR(in_dooranim), LW(15))
    BEGIN_INLINE_EVENT
        CALL(npc_dark, LW(10))
        CALL_CHILD(in_houseanim)
        SPAWN_EVENT(evt_dark2)
    END_INLINE_EVENT
    WAIT_MS(300)
    SPAWN_EVENT(evt_dark)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(in_pos, LW(10), LW(0), LW(2))
    CALL(evt_mario_party_door_move, LW(0), LW(2), FLOAT(135.0f))
    CALL(evt_cam3d_evt_xyz_off, 0, 11)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_disable, 0)
    LOOP(0)
        IS_EVENT_RUNNING(LW(15), LW(0))
        IF_INT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(snd_door_in)
    POKE_INT(0, PTR(&DoorFLAG))
    RETURN
EVT_END()

EVT_BEGIN(house_out)
    PEEK_INT(LW(0), PTR(&DoorFLAG))
    IF_INT_EQUAL(LW(0), 1)
        RETURN
    END_IF
    POKE_INT(1, PTR(&DoorFLAG))
    CALL(evt_snd_bgmon, 160, 0)
    CALL(evt_snd_envon, 160, 0)
    CALL(evt_snd_env_lpf, 0, 0)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_cam_letter_box_disable, 1)
    BEGIN_INLINE_EVENT
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(out_pos, LW(10), LW(0), LW(2))
        ADD(LW(1), 1)
        CALL(evt_cam3d_evt_set_xyz, LW(0), LW(1), LW(2), 0, 150, 11)
    END_INLINE_EVENT
    BEGIN_INLINE_EVENT
        SPAWN_EVENT(evt_light2)
        CALL_CHILD(out_houseanim)
        SPAWN_EVENT(evt_light)
    END_INLINE_EVENT
    SPAWN_EVENT_GET_ID(PTR(out_dooranim), LW(15))
    WAIT_MS(1000)
    CALL(N_evt_mario_party_door_halve_hitbox)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(out_pos, LW(10), LW(0), LW(2))
    CALL(evt_mario_party_door_move, LW(0), LW(2), FLOAT(135.0f))
    SET_VALUE(LW(0), 0)
    LOOP(0)
        CALL(get_noclip_map, LW(10), LW(0), LW(1))
        IF_INT_EQUAL(LW(1), -1)
            BREAK_LOOP
        END_IF
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 2)
        ADD(LW(0), 1)
    END_LOOP
    CALL(evt_cam3d_evt_xyz_off, 100, 11)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_disable, 0)
    LOOP(0)
        IS_EVENT_RUNNING(LW(15), LW(0))
        IF_INT_EQUAL(LW(0), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    POKE_INT(0, PTR(&DoorFLAG))
    CALL(inside_group, LW(10), LW(0), LW(1))
    CALL(evt_mapobj_flag_onoff, 1, 1, LW(0), 1)
    CALL(evt_hitobj_onoff, LW(1), 1, 0)
    CALL(evt_mapobj_flag_onoff, 1, 0, LW(0), 2048)
    CALL(evt_mapobj_flag_onoff, 1, 0, LW(0), 4096)
    CALL(outside_group, LW(10), LW(0))
    CALL(evt_mapobj_flag_onoff, 1, 0, LW(0), 2048)
    CALL(door_group, LW(10), LW(0))
    CALL(evt_mapobj_flag_onoff, 1, 0, LW(0), 2048)
    CALL(evt_mapobj_flag_onoff, 1, 0, LW(0), 4096)
    CALL(npc_light, LW(10))
    CALL(snd_door_out)
    RETURN
EVT_END()

EVT_BEGIN(evt_door_setup)
    CALL(door_entry, LW(0), LW(10))
    POKE_INT(0, PTR(&DoorFLAG))
    CALL(init_inside_group, LW(10), LW(0), LW(1))
    IF_INT_NOT_EQUAL(LW(0), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(0), 1)
    END_IF
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_hitobj_onoff, LW(1), 1, 0)
    END_IF
    CALL(inout_hit, LW(10), LW(0), LW(1), LW(2), LW(3))
    CALL(evt_run_case_evt, LW(0), 1, LW(1), 0, PTR(house_in), 0)
    CALL(evt_run_case_evt, LW(2), 1, LW(3), 0, PTR(house_out), 0)
    RETURN
EVT_END()

EVT_BEGIN(door_dark)
    CALL(evt_sub_intpl_msec_init, 11, 255, 0, 300)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_map_set_blend, 0, LW(0), LW(0), LW(0), 255)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_map_set_blend, 0, LW(0), LW(0), LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(door_dark2)
    CALL(evt_sub_intpl_msec_init, 11, 255, 0, 300)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_map_set_blend, 1, LW(0), LW(0), LW(0), 255)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_map_set_blend, 1, LW(0), LW(0), LW(0), 0)
    RETURN
EVT_END()

EVT_BEGIN(door_light)
    CALL(evt_sub_intpl_msec_init, 11, 0, 255, 300)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_map_set_blend, 0, LW(0), LW(0), LW(0), 255)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_map_blend_off, 0)
    RETURN
EVT_END()

EVT_BEGIN(door_light2)
    CALL(evt_sub_intpl_msec_init, 11, 0, 255, 300)
    LABEL(0)
    CALL(evt_sub_intpl_msec_get_value)
    CALL(evt_map_set_blend, 1, LW(0), LW(0), LW(0), 255)
    WAIT_FRAMES(1)
    IF_INT_EQUAL(LW(1), 1)
        GOTO(0)
    END_IF
    CALL(evt_map_blend_off, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_door_open)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_mario_key_onoff, 0)
    POKE_INT(1, PTR(&DoorFLAG))
    CALL(evt_door_param, LW(0), 7, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        SET_VALUE(LF(0), 0)
        SPAWN_EVENT(LW(1))
    END_IF
    CALL(evt_door_param, LW(0), 2, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_door_save_mapflag, LW(1))
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 2)
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_door_param, LW(0), 4, LW(1), LW(2), LW(3))
        CALL(evt_mario_get_pos, 0, LW(4), LW(5), LW(6))
        ADD(LW(5), 2)
        CALL(evt_cam3d_evt_set_xyz, LW(1), LW(5), LW(3), 0, 150, 11)
    END_INLINE_EVENT
    CALL(evt_door_param, LW(0), 2, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 2048)
    END_IF
    CALL(evt_door_param, LW(0), 1, LW(1), LW(2), LW(3))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 2048)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 4096)
    END_IF
    CALL(evt_door_param, LW(0), 3, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 2048)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 4096)
    END_IF
    IF_INT_NOT_EQUAL(LW(2), 0)
        CALL(evt_hitobj_onoff, LW(2), 1, 1)
    END_IF
    CALL(evt_door_param, LW(0), 9, LW(1))
    IF_BITS_SET(LW(1), 1)
        CALL(npc_hide_onoff, LW(0), 0)
    END_IF
    CALL(door_dark_flag, LW(0), 1)
    IF_BITS_SET(LW(15), 4)
        CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
        IF_INT_NOT_EQUAL(LW(9), 0)
            BEGIN_INLINE_EVENT
                WAIT_MS(500)
                SPAWN_EVENT(house_open_se)
                CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
                SET_VALUE(LW(6), 0)
                CALL(evt_sub_intpl_msec_init, 2, 0, 180, 500)
                LABEL(2)
                CALL(evt_sub_intpl_msec_get_value)
                SET_VALUE(LW(5), LW(1))
                SET_VALUE(LW(1), 0)
                SET_VALUE(LF(0), 0)
                SPAWN_EVENT(LW(9))
                SET_VALUE(LW(6), 1)
                WAIT_FRAMES(1)
                IF_INT_EQUAL(LW(5), 1)
                    GOTO(2)
                END_IF
            END_INLINE_EVENT
        END_IF
    END_IF
    CALL(N_evt_mario_party_door_halve_hitbox)
    CALL(evt_door_param, LW(0), 5, LW(1), LW(2), LW(3))
    CALL(evt_mario_mov_pos2, LW(1), LW(3), FLOAT(135.0f))
    CALL(evt_party_move_beside_mario, 0)
    CALL(evt_party_move_beside_mario, 1)
    IF_BITS_CLEAR(LW(15), 2)
        SPAWN_EVENT(door_dark)
    END_IF
    SPAWN_EVENT(door_open_se)
    CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
    IF_INT_NOT_EQUAL(LW(7), 0)
        BEGIN_INLINE_EVENT
            CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
            CALL(evt_sub_intpl_msec_init, 5, 0, 180, 500)
            LABEL(0)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(LW(5), LW(1))
            SET_VALUE(LW(1), 0)
            SET_VALUE(LF(0), 0)
            SPAWN_EVENT(LW(7))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(5), 1)
                GOTO(0)
            END_IF
        END_INLINE_EVENT
    END_IF
    WAIT_MS(500)
    CALL(evt_door_param, LW(0), 4, LW(1), LW(2), LW(3))
    CALL(evt_mario_party_door_move, LW(1), LW(3), FLOAT(135.0f))
    CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
    IF_INT_NOT_EQUAL(LW(7), 0)
        BEGIN_INLINE_EVENT
            SPAWN_EVENT(door_close_se)
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
            CALL(evt_sub_intpl_msec_init, 2, 180, 0, 500)
            LABEL(1)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(LW(5), LW(1))
            SET_VALUE(LW(1), 1)
            SET_VALUE(LF(0), 0)
            SPAWN_EVENT(LW(7))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(5), 1)
                GOTO(1)
            END_IF
        END_INLINE_EVENT
    END_IF
    IF_BITS_CLEAR(LW(15), 4)
        CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
        IF_INT_NOT_EQUAL(LW(9), 0)
            BEGIN_INLINE_EVENT
                SPAWN_EVENT(house_open_se)
                CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
                SET_VALUE(LW(6), 0)
                CALL(evt_sub_intpl_msec_init, 2, 0, 180, 500)
                LABEL(2)
                CALL(evt_sub_intpl_msec_get_value)
                SET_VALUE(LW(5), LW(1))
                SET_VALUE(LW(1), 0)
                SET_VALUE(LF(0), 0)
                SPAWN_EVENT(LW(9))
                SET_VALUE(LW(6), 1)
                WAIT_FRAMES(1)
                IF_INT_EQUAL(LW(5), 1)
                    GOTO(2)
                END_IF
            END_INLINE_EVENT
        END_IF
    END_IF
    WAIT_MS(500)
    IF_BITS_CLEAR(LW(15), 2)
        SPAWN_EVENT(door_dark2)
    END_IF
    CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
    IF_INT_NOT_EQUAL(LW(8), 0)
        BEGIN_INLINE_EVENT
            WAIT_MS(500)
            SPAWN_EVENT(door_down_se)
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
            CALL(evt_sub_intpl_msec_init, 2, 0, 180, 500)
            LABEL(3)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(LF(0), 0)
            SPAWN_EVENT(LW(8))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(3)
            END_IF
        END_INLINE_EVENT
    END_IF
    WAIT_MS(500)
    CALL(evt_cam3d_evt_xyz_off, 0, 11)
    POKE_INT(0, PTR(&DoorFLAG))
    WAIT_FRAMES(2)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_disable, 0)
    CALL(snd_door_in)
    RETURN
EVT_END()

EVT_BEGIN(evt_door_open_bom_01)
    CALL(evt_door_param, LW(0), 7, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        SPAWN_EVENT(LW(1))
    END_IF
    CALL(evt_door_param, LW(0), 2, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_door_save_mapflag, LW(1))
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 2)
    END_IF
    CALL(evt_door_param, LW(0), 2, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 2048)
    END_IF
    CALL(evt_door_param, LW(0), 1, LW(1), LW(2), LW(3))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 2048)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 4096)
    END_IF
    CALL(evt_door_param, LW(0), 3, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 2048)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 4096)
    END_IF
    IF_INT_NOT_EQUAL(LW(2), 0)
        CALL(evt_hitobj_onoff, LW(2), 1, 1)
    END_IF
    CALL(door_dark_flag, LW(0), 1)
    SPAWN_EVENT(door_open_se)
    CALL(N_evt_mario_party_door_halve_hitbox)
    CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
    IF_INT_NOT_EQUAL(LW(7), 0)
        BEGIN_INLINE_EVENT
            CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
            CALL(evt_sub_intpl_msec_init, 5, 0, 180, 500)
            LABEL(0)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(LW(5), LW(1))
            SET_VALUE(LW(1), 0)
            SPAWN_EVENT(LW(7))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(5), 1)
                GOTO(0)
            END_IF
        END_INLINE_EVENT
    END_IF
    WAIT_MS(500)
    SET_VALUE(GF(0), 1)
    LOOP(0)
        IF_INT_EQUAL(GF(0), 0)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    SPAWN_EVENT(door_dark)
    CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
    IF_INT_NOT_EQUAL(LW(7), 0)
        BEGIN_INLINE_EVENT
            SPAWN_EVENT(door_close_se)
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
            CALL(evt_sub_intpl_msec_init, 2, 180, 0, 500)
            LABEL(1)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(LW(5), LW(1))
            SET_VALUE(LW(1), 1)
            SPAWN_EVENT(LW(7))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(5), 1)
                GOTO(1)
            END_IF
        END_INLINE_EVENT
    END_IF
    CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
    IF_INT_NOT_EQUAL(LW(9), 0)
        BEGIN_INLINE_EVENT
            SPAWN_EVENT(house_open_se)
            CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
            SET_VALUE(LW(6), 0)
            CALL(evt_sub_intpl_msec_init, 2, 0, 180, 500)
            LABEL(2)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(LW(5), LW(1))
            SET_VALUE(LW(1), 0)
            SPAWN_EVENT(LW(9))
            SET_VALUE(LW(6), 1)
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(5), 1)
                GOTO(2)
            END_IF
        END_INLINE_EVENT
    END_IF
    WAIT_MS(500)
    SPAWN_EVENT(door_dark2)
    CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
    IF_INT_NOT_EQUAL(LW(8), 0)
        BEGIN_INLINE_EVENT
            WAIT_MS(500)
            SPAWN_EVENT(door_down_se)
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
            CALL(evt_sub_intpl_msec_init, 2, 0, 180, 500)
            LABEL(3)
            CALL(evt_sub_intpl_msec_get_value)
            SPAWN_EVENT(LW(8))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(3)
            END_IF
        END_INLINE_EVENT
    END_IF
    WAIT_MS(500)
    CALL(snd_door_in)
    RETURN
EVT_END()

EVT_BEGIN(evt_door_open_roll)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_door_param, LW(0), 7, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        SPAWN_EVENT(LW(1))
    END_IF
    CALL(evt_door_param, LW(0), 2, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_door_save_mapflag, LW(1))
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 2)
    END_IF
    CALL(evt_door_param, LW(0), 2, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 2048)
    END_IF
    CALL(evt_door_param, LW(0), 1, LW(1), LW(2), LW(3))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 2048)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 4096)
    END_IF
    CALL(evt_door_param, LW(0), 3, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 2048)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 4096)
    END_IF
    IF_INT_NOT_EQUAL(LW(2), 0)
        CALL(evt_hitobj_onoff, LW(2), 1, 1)
    END_IF
    CALL(door_dark_flag, LW(0), 1)
    SPAWN_EVENT(door_dark)
    CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
    IF_INT_NOT_EQUAL(LW(9), 0)
        BEGIN_INLINE_EVENT
            CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
            SET_VALUE(LW(6), 0)
            CALL(evt_sub_intpl_msec_init, 2, 0, 180, 500)
            LABEL(2)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(LW(5), LW(1))
            SET_VALUE(LW(1), 0)
            SPAWN_EVENT(LW(9))
            SET_VALUE(LW(6), 1)
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(5), 1)
                GOTO(2)
            END_IF
        END_INLINE_EVENT
    END_IF
    WAIT_MS(500)
    SPAWN_EVENT(door_dark2)
    CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
    IF_INT_NOT_EQUAL(LW(8), 0)
        BEGIN_INLINE_EVENT
            CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
            CALL(evt_sub_intpl_msec_init, 2, 0, 180, 500)
            LABEL(3)
            CALL(evt_sub_intpl_msec_get_value)
            SPAWN_EVENT(LW(8))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(3)
            END_IF
        END_INLINE_EVENT
    END_IF
    WAIT_MS(500)
    CALL(evt_cam3d_evt_xyz_off, 0, 11)
    CALL(evt_mario_key_onoff, 1)
    CALL(snd_door_in)
    RETURN
EVT_END()

EVT_BEGIN(evt_door_close)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_mario_key_onoff, 0)
    POKE_INT(1, PTR(&DoorFLAG))
    CALL(evt_door_param, LW(0), 7, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(2), 0)
        SET_VALUE(LF(0), 1)
        SPAWN_EVENT(LW(2))
    END_IF
    BEGIN_INLINE_EVENT
        IF_BITS_CLEAR(LW(15), 16)
            CALL(evt_door_param, LW(0), 5, LW(1), LW(2), LW(3))
        ELSE
            CALL(evt_door_param, LW(0), 11, LW(1), LW(2), LW(3))
        END_IF
        CALL(evt_mario_get_pos, 0, LW(4), LW(5), LW(6))
        ADD(LW(5), 2)
        CALL(evt_cam3d_evt_set_xyz, LW(1), LW(5), LW(3), 0, 150, 11)
    END_INLINE_EVENT
    IF_BITS_CLEAR(LW(15), 2)
        SPAWN_EVENT(door_light2)
    END_IF
    SPAWN_EVENT(door_up_se)
    CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
    IF_INT_NOT_EQUAL(LW(8), 0)
        BEGIN_INLINE_EVENT
            CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
            CALL(evt_sub_intpl_msec_init, 5, 180, 0, 500)
            LABEL(3)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(LF(0), 1)
            SPAWN_EVENT(LW(8))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(3)
            END_IF
        END_INLINE_EVENT
    END_IF
    CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
    IF_INT_NOT_EQUAL(LW(9), 0)
        BEGIN_INLINE_EVENT
            WAIT_MS(400)
            SPAWN_EVENT(house_close_se)
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
            SET_VALUE(LW(6), 0)
            CALL(evt_sub_intpl_msec_init, 5, 180, 0, 500)
            LABEL(2)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(LW(5), LW(1))
            SET_VALUE(LW(1), 3)
            SET_VALUE(LF(0), 1)
            SPAWN_EVENT(LW(9))
            SET_VALUE(LW(6), 1)
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(5), 1)
                GOTO(2)
            END_IF
        END_INLINE_EVENT
    END_IF
    WAIT_MS(500)
    IF_BITS_CLEAR(LW(15), 2)
        SPAWN_EVENT(door_light)
    END_IF
    SPAWN_EVENT(door_open_se)
    CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
    IF_INT_NOT_EQUAL(LW(7), 0)
        BEGIN_INLINE_EVENT
            CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
            CALL(evt_sub_intpl_msec_init, 5, 0, 180, 500)
            LABEL(1)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(LW(5), LW(1))
            SET_VALUE(LW(1), 2)
            SET_VALUE(LF(0), 1)
            SPAWN_EVENT(LW(7))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(5), 1)
                GOTO(1)
            END_IF
        END_INLINE_EVENT
    END_IF
    WAIT_MS(500)
    IF_BITS_CLEAR(LW(15), 16)
        CALL(evt_door_param, LW(0), 5, LW(1), LW(2), LW(3))
    ELSE
        CALL(evt_door_param, LW(0), 11, LW(1), LW(2), LW(3))
    END_IF
    IF_BITS_CLEAR(LW(15), 8)
        CALL(evt_mario_party_door_move, LW(1), LW(3), FLOAT(135.0f))
    END_IF
    CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
    IF_INT_NOT_EQUAL(LW(7), 0)
        BEGIN_INLINE_EVENT
            SPAWN_EVENT(door_close_se)
        END_INLINE_EVENT
        BEGIN_INLINE_EVENT
            CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
            CALL(evt_sub_intpl_msec_init, 5, 180, 0, 500)
            LABEL(0)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(LW(5), LW(1))
            SET_VALUE(LW(1), 3)
            SET_VALUE(LF(0), 1)
            SPAWN_EVENT(LW(7))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(5), 1)
                GOTO(0)
            END_IF
        END_INLINE_EVENT
        WAIT_MS(500)
    END_IF
    CALL(evt_cam3d_evt_xyz_off, 100, 11)
    CALL(door_dark_flag, LW(0), 0)
    CALL(evt_door_param, LW(0), 9, LW(1))
    IF_BITS_SET(LW(1), 1)
        CALL(npc_hide_onoff, LW(0), 1)
    END_IF
    CALL(evt_door_param, LW(0), 3, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        IF_BITS_CLEAR(LW(15), 1)
            CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 1)
        END_IF
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 2048)
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 4096)
    END_IF
    IF_INT_NOT_EQUAL(LW(2), 0)
        CALL(evt_hitobj_onoff, LW(2), 1, 0)
    END_IF
    CALL(evt_door_param, LW(0), 2, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 2048)
    END_IF
    CALL(evt_door_param, LW(0), 1, LW(1), LW(2), LW(3))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 2048)
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 4096)
    END_IF
    CALL(evt_door_param, LW(0), 2, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_door_load_mapflag, LW(1))
    END_IF
    POKE_INT(0, PTR(&DoorFLAG))
    WAIT_FRAMES(2)
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_disable, 0)
    CALL(snd_door_out)
    RETURN
EVT_END()

EVT_BEGIN(evt_door_close_roll)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_door_param, LW(0), 7, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(2), 0)
        SPAWN_EVENT(LW(2))
    END_IF
    BEGIN_INLINE_EVENT
        CALL(evt_door_param, LW(0), 5, LW(1), LW(2), LW(3))
        CALL(evt_mario_get_pos, 0, LW(4), LW(5), LW(6))
        ADD(LW(5), 2)
        CALL(evt_cam3d_evt_set_xyz, LW(1), LW(5), LW(3), 0, 150, 11)
    END_INLINE_EVENT
    SPAWN_EVENT(door_light2)
    CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
    IF_INT_NOT_EQUAL(LW(8), 0)
        BEGIN_INLINE_EVENT
            CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
            CALL(evt_sub_intpl_msec_init, 5, 180, 0, 500)
            LABEL(3)
            CALL(evt_sub_intpl_msec_get_value)
            SPAWN_EVENT(LW(8))
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(1), 1)
                GOTO(3)
            END_IF
        END_INLINE_EVENT
    END_IF
    CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
    IF_INT_NOT_EQUAL(LW(9), 0)
        BEGIN_INLINE_EVENT
            CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
            SET_VALUE(LW(6), 0)
            CALL(evt_sub_intpl_msec_init, 5, 180, 0, 500)
            LABEL(2)
            CALL(evt_sub_intpl_msec_get_value)
            SET_VALUE(LW(5), LW(1))
            SET_VALUE(LW(1), 3)
            SPAWN_EVENT(LW(9))
            SET_VALUE(LW(6), 1)
            WAIT_FRAMES(1)
            IF_INT_EQUAL(LW(5), 1)
                GOTO(2)
            END_IF
        END_INLINE_EVENT
    END_IF
    WAIT_MS(500)
    SPAWN_EVENT(door_light)
    CALL(evt_cam3d_evt_xyz_off, 100, 11)
    CALL(door_dark_flag, LW(0), 0)
    CALL(evt_door_param, LW(0), 3, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        IF_BITS_CLEAR(LW(15), 1)
            CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 1)
        END_IF
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 2048)
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 4096)
    END_IF
    IF_INT_NOT_EQUAL(LW(2), 0)
        CALL(evt_hitobj_onoff, LW(2), 1, 0)
    END_IF
    CALL(evt_door_param, LW(0), 2, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 2048)
    END_IF
    CALL(evt_door_param, LW(0), 1, LW(1), LW(2), LW(3))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 2048)
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 4096)
    END_IF
    CALL(evt_door_param, LW(0), 2, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_door_load_mapflag, LW(1))
    END_IF
    CALL(evt_mario_key_onoff, 1)
    CALL(snd_door_out)
    RETURN
EVT_END()

EVT_BEGIN(evt_door_open_quick)
    CALL(evt_door_param, LW(0), 7, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        SPAWN_EVENT(LW(1))
    END_IF
    CALL(evt_door_param, LW(0), 2, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_door_save_mapflag, LW(1))
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 2)
    END_IF
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(10), LW(0))
        CALL(evt_door_param, LW(10), 4, LW(0), LW(1), LW(2))
        CALL(evt_mario_get_pos, 0, LW(4), LW(5), LW(6))
        SET_VALUE(LW(1), LW(5))
        CALL(evt_cam3d_get_shift, LW(0), LW(1), LW(2), LW(3))
        CALL(evt_cam3d_event_from_road, LW(0), LW(1), LW(2), LW(3), LW(4), LW(5), LW(6), LW(7), LW(8), LW(9))
        CALL(evt_cam3d_evt_set, LW(4), LW(5), LW(6), LW(7), LW(8), LW(9), 0, 11)
        WAIT_FRAMES(1)
        CALL(evt_cam3d_evt_off, 0, 11)
    END_INLINE_EVENT
    CALL(evt_door_param, LW(0), 2, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 2048)
    END_IF
    CALL(evt_door_param, LW(0), 1, LW(1), LW(2), LW(3))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 2048)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 4096)
    END_IF
    CALL(evt_door_param, LW(0), 3, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_mapobj_flag_onoff, 1, 0, LW(1), 1)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 2048)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 4096)
    END_IF
    IF_INT_NOT_EQUAL(LW(2), 0)
        CALL(evt_hitobj_onoff, LW(2), 1, 1)
    END_IF
    CALL(door_dark_flag, LW(0), 1)
    CALL(evt_map_set_blend, 0, 0, 0, 0, 0)
    CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
    IF_INT_NOT_EQUAL(LW(9), 0)
        BEGIN_INLINE_EVENT
            SET_VALUE(LW(0), 180)
            SPAWN_EVENT(LW(9))
        END_INLINE_EVENT
    END_IF
    CALL(evt_map_set_blend, 1, 0, 0, 0, 0)
    CALL(evt_door_param, LW(0), 6, LW(7), LW(8), LW(9))
    IF_INT_NOT_EQUAL(LW(8), 0)
        BEGIN_INLINE_EVENT
            SET_VALUE(LW(0), 180)
            SPAWN_EVENT(LW(8))
        END_INLINE_EVENT
    END_IF
    CALL(snd_door_in)
    RETURN
EVT_END()

EVT_BEGIN(evt_door_entry)
    CALL(evt_door_param, LW(0), 3, LW(1), LW(2))
    IF_INT_NOT_EQUAL(LW(1), 0)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(1), 1)
    END_IF
    IF_INT_NOT_EQUAL(LW(2), 0)
        CALL(evt_hitobj_onoff, LW(2), 1, 0)
    END_IF
    CALL(evt_door_param, LW(0), 1, LW(1), LW(2), LW(3))
    IF_INT_NOT_EQUAL(LW(2), 0)
        SET_VALUE(LW(15), 0)
        CALL(evt_run_case_evt, 9, 1, LW(2), 0, PTR(evt_door_close), 0)
    END_IF
    IF_INT_NOT_EQUAL(LW(3), 0)
        SET_VALUE(LW(15), 0)
        CALL(evt_run_case_evt, 9, 1, LW(3), 0, PTR(evt_door_open), 0)
    END_IF
    CALL(evt_door_param, LW(0), 9, LW(1))
    IF_BITS_SET(LW(1), 1)
        CALL(npc_hide_onoff, LW(0), 1)
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(door_up_se)
    RETURN
EVT_END()

EVT_BEGIN(house_close_se)
    CALL_CHILD(house_open_se)
    RETURN
EVT_END()
// clang-format on
