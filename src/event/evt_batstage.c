#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "battle/battle.h"
#include "battle/battle_stage.h"
#include <dolphin/types.h>
#include <string.h>

// Locally declared prototypes (move to battle/battle_stage.h later).

// .rodata
const char dat_802f32f8[3][0x20] = {
	"sys_activeUnit_1",
	"sys_activeUnit_2",
	"sys_activeUnit_3",
};
const Vec dat_802f3358[3] = {
	{100.0f, 250.0f, 70.0f},
	{-300.0f, 250.0f, 70.0f},
	{-120.0f, 250.0f, -70.0f},
};
const Vec dat_802f337c[3] = {
	{100.0f, 0.0f, -70.0f},
	{-300.0f, 0.0f, -70.0f},
	{-120.0f, 0.0f, -70.0f},
};
const Vec dat_802f33a0[3] = {
	{15.0f, 0.0f, 15.0f},
	{-15.0f, 0.0f, 15.0f},
	{0.0f, 0.0f, -15.0f},
};
const Vec dat_802f33c4[3] = {
	{0.0f, 0.0f, -1.0f},
	{0.0f, 0.0f, -1.0f},
	{0.0f, 0.0f, -1.0f},
};
const char dat_802f33e8[12] = "xxxxxxxxx";

typedef struct Blob60 {
	char b[0x60];
} Blob60;

typedef struct Vec3 {
	Vec v[3];
} Vec3;

// .sbss2
u8 unk_80423030[4];
u8 gap_10_80423034_sbss2[4];

s32 evt_batstage_set_stg_dark(EventEntry* event, BOOL isFirstCall) {
	s32* args = event->args;
	u8 a;
	s32 frames;
	s32 ease;

	a = evtGetValue(event, args[0]);
	frames = evtGetValue(event, args[1]);
	ease = evtGetValue(event, args[2]);
	BattleStageSetStgDark(a, frames, ease);
	return EVT_RETURN_DONE;
}

s32 evt_batstage_set_stg_dark_base(EventEntry* event, BOOL isFirstCall) {
	s32* args = event->args;
	u8 a;
	s32 frames;
	s32 ease;

	a = evtGetValue(event, args[0]);
	frames = evtGetValue(event, args[1]);
	ease = evtGetValue(event, args[2]);
	BattleStageSetStgDarkBase(a, frames, ease);
	return EVT_RETURN_DONE;
}

s32 evt_batstage_return_stg_dark_base(EventEntry* event, BOOL isFirstCall) {
	s32* args = event->args;
	s32 frames;
	s32 ease;

	frames = evtGetValue(event, args[0]);
	ease = evtGetValue(event, args[1]);
	BattleStageReturnStgDarkBase(frames, ease);
	return EVT_RETURN_DONE;
}

s32 evt_batstage_set_aud_dark(EventEntry* event, BOOL isFirstCall) {
	s32* args = event->args;
	u8 a;
	s32 frames;
	s32 ease;

	a = evtGetValue(event, args[0]);
	frames = evtGetValue(event, args[1]);
	ease = evtGetValue(event, args[2]);
	BattleStageSetAudDark(a, frames, ease);
	return EVT_RETURN_DONE;
}

s32 evt_batstage_set_aud_dark_base(EventEntry* event, BOOL isFirstCall) {
	s32* args = event->args;
	u8 a;
	s32 frames;
	s32 ease;

	a = evtGetValue(event, args[0]);
	frames = evtGetValue(event, args[1]);
	ease = evtGetValue(event, args[2]);
	BattleStageSetAudDarkBase(a, frames, ease);
	return EVT_RETURN_DONE;
}

s32 evt_batstage_return_aud_dark_base(EventEntry* event, BOOL isFirstCall) {
	s32* args = event->args;
	s32 frames;
	s32 ease;

	frames = evtGetValue(event, args[0]);
	ease = evtGetValue(event, args[1]);
	BattleStageReturnAudDarkBase(frames, ease);
	return EVT_RETURN_DONE;
}

#pragma opt_dead_assignments off // keep the original's r16/r17 assignment in the setup copy block
// NONMATCHING (91.85%): the four Vec3 copies, the mode switch and the whole light loop match; only the
// load/register schedule of the copy block differs (original spills pos.x, unitOff1.z/2.y/2.z, all of
// up and color word0 to 0x108+, ours spills a different subset and starts r18.. at 0x6c not 0x64).
s32 evt_batstage_light_set_player_on(EventEntry* event, BOOL isFirstCall) {
	BattleWork* bw = _battleWorkPointer;
	s32* args = event->args;
	s32 mode;
	u8 enabled[3];
	u8 color[3][3];
	Vec pos[3];
	Vec target[3];
	Vec unitOff[3];
	Vec up[3];
	char name[3][0x20];
	s32 i;
	s32 lightNo;

	mode = evtGetValue(event, args[0]);

	*(Blob60*)name = *(const Blob60*)dat_802f32f8;

	*(Vec3*)pos = *(const Vec3*)dat_802f3358;
	*(Vec3*)target = *(const Vec3*)dat_802f337c;
	*(Vec3*)unitOff = *(const Vec3*)dat_802f33a0;
	*(Vec3*)up = *(const Vec3*)dat_802f33c4;
	*(u16*)enabled = *(const u16*)unk_80423030;
	enabled[2] = unk_80423030[2];
	memcpy(color, dat_802f33e8, sizeof(color));

	switch (mode) {
	case 1:
		enabled[2] = 1;
		color[2][0] = 0xff;
		color[2][1] = 0xff;
		color[2][2] = 0xff;
		break;
	default:
		enabled[0] = 1;
		enabled[1] = 1;
		enabled[2] = 1;
		break;
	}

	for (i = 0; i < 3; i++) {
		if (enabled[i] != 0) {
			lightNo = BattleStageGetLightNumberByName(name[i]);
			if (lightNo == -1) {
				lightNo = BattleStageSysLightEntry(name[i]);
			}
			BattleStageLightSetLightColor(lightNo, color[i][0], color[i][1], color[i][2], 0x1e, 0);
			BattleStageLightSetWidth(lightNo, 0x78, 1, 0);
			BattleStageLightSetPosition(lightNo, 0, pos[i].x, pos[i].y, pos[i].z, 0);
			BattleStageLightSetUpVec(lightNo, 0, up[i].x, up[i].y, up[i].z, 0);
			BattleStageLightSetTarget(lightNo, 0, target[i].x, target[i].y, target[i].z, 0);
			BattleStageLightSetTargetByUnit(lightNo, bw->activeUnitId, unitOff[i].x, unitOff[i].y, unitOff[i].z, 0x1e, 5);
		}
	}

	BattleStageSetStgDark(0x8c, 0x1e, 0);
	BattleStageSetAudDark(0x8c, 0, 0);
	return EVT_RETURN_DONE;
}

#pragma opt_dead_assignments reset
s32 evt_batstage_light_set_player_off(EventEntry* event, BOOL isFirstCall) {
	s32 lightNo;

	BattleStageReturnAudDarkBase(0, 0);
	BattleStageReturnStgDarkBase(0x1e, 0);

	lightNo = BattleStageGetLightNumberByName("sys_activeUnit_1");
	if (lightNo != -1) {
		BattleStageLightRelease(lightNo, 0x1e, 0);
	}

	lightNo = BattleStageGetLightNumberByName("sys_activeUnit_2");
	if (lightNo != -1) {
		BattleStageLightRelease(lightNo, 0x1e, 0);
	}

	lightNo = BattleStageGetLightNumberByName("sys_activeUnit_3");
	if (lightNo != -1) {
		BattleStageLightRelease(lightNo, 0x1e, 0);
	}

	return EVT_RETURN_DONE;
}
