#include "evt/evt_light.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"
#include "driver/lightdrv.h"
#include "driver/mapdrv.h"
#include "driver/npcdrv.h"
#include "driver/animdrv.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "party.h"
#include <dolphin/mtx.h>

USER_FUNC(evt_light_charalight_all_onoff) {
	s32* args = event->args;
	LightEntry* entry;
	s32 i;

	if (evtGetValue(event, args[0]) != 0) {
		for (i = 0; i < lightGetEntryNum(); i++) {
			entry = lightNumberToPtr(i);
			entry->flags |= LIGHT_CHARA;
		}
	} else {
		for (i = 0; i < lightGetEntryNum(); i++) {
			entry = lightNumberToPtr(i);
			entry->flags &= ~LIGHT_CHARA;
		}
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_light_mario_lighting_onoff) {
	s32* args = event->args;
	MarioWork* mario;
	s32 onoff;

	onoff = evtGetValue(event, args[0]);
	mario = marioGetPtr();
	if (onoff != 0) {
		animPoseSetMaterialLightFlagOn(mario->paperAnimGroupId[0], 2);
		animPoseSetMaterialLightFlagOn(mario->paperAnimGroupId[1], 2);
		animPoseSetMaterialLightFlagOn(mario->paperAnimGroupId[2], 2);
	} else {
		animPoseSetMaterialLightFlagOff(mario->paperAnimGroupId[0], 2);
		animPoseSetMaterialLightFlagOff(mario->paperAnimGroupId[1], 2);
		animPoseSetMaterialLightFlagOff(mario->paperAnimGroupId[2], 2);
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_light_party_lighting_onoff) {
	s32* args = event->args;
	PartyEntry* party;
	s32 onoff;

	onoff = evtGetValue(event, args[0]);
	party = partyGetPtr(marioGetPartyId());
	if (onoff != 0) {
		animPoseSetMaterialLightFlagOn(party->paperAnimGroup, 2);
	} else {
		animPoseSetMaterialLightFlagOff(party->paperAnimGroup, 2);
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_light_npc_lighting_onoff) {
	s32* args = event->args;
	NpcEntry* npc;
	s32 onoff;
	s32 name;

	onoff = evtGetValue(event, args[0]);
	name = evtGetValue(event, args[1]);
	npc = npcNameToPtr((const char*)name);
	if (onoff != 0) {
		animPoseSetMaterialLightFlagOn(npc->poseId, 2);
	} else {
		animPoseSetMaterialLightFlagOff(npc->poseId, 2);
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_light_bind_mapobj) {
	s32* args = event->args;
	LightEntry* light;
	MapObject* mapObj;
	Vec pos;
	Vec v;
	Mtx mtx;
	s32 mode;
	s32 name;
	s32 mapName;

	name = evtGetValue(event, args[0]);
	mapName = evtGetValue(event, args[1]);
	mode = evtGetValue(event, args[2]);
	light = lightNameToPtr((const char*)name);
	mapObj = mapGetMapObj((const char*)mapName);
	if (isFirstCall) {
		pos = (Vec){0.0f, 0.0f, 0.0f};
		PSMTXMultVec(mapObj->modelWorldMtx, &pos, &pos);
		PSVECSubtract(&light->field_0x3C, &pos, &pos);
		event->userdataf[0] = pos.x;
		event->userdataf[1] = pos.y;
		event->userdataf[2] = pos.z;
	}
	pos.x = event->userdataf[0];
	pos.y = event->userdataf[1];
	pos.z = event->userdataf[2];
	switch (mode) {
	case 0:
		PSMTXMultVec(mapObj->userMtx, &pos, &pos);
		break;
	case 1:
		PSMTXTrans(mtx, mapObj->userMtx[0][3], mapObj->userMtx[1][3], mapObj->userMtx[2][3]);
		PSMTXMultVec(mtx, &pos, &pos);
		break;
	}
	v = (Vec){0.0f, 0.0f, 0.0f};
	PSMTXMultVec(mapObj->modelWorldMtx, &v, &v);
	PSVECAdd(&v, &pos, &light->position);
	return EVT_RETURN_BLOCK;
}
