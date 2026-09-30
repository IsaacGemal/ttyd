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

// NONMATCHING (81.55%): 18 instructions differ, 16 missing, 17 extra, plus register differences.
s32 evt_batstage_light_set_player_on(EventEntry* event, BOOL isFirstCall) {
	BattleWork* bw = _battleWorkPointer;
	s32* args = event->args;
	s32 mode;
	u8 enabled[3];
	u8 color[3][3];
	Vec up[3];
	Vec unitOff[3];
	Vec target[3];
	Vec pos[3];
	char name[3][0x20];
	s32 i;
	s32 lightNo;

	mode = evtGetValue(event, args[0]);

	*(Blob60*)name = *(const Blob60*)dat_802f32f8;

	pos[0] = dat_802f3358[0];
	pos[1] = dat_802f3358[1];
	pos[2] = dat_802f3358[2];
	target[0] = dat_802f337c[0];
	target[1] = dat_802f337c[1];
	target[2] = dat_802f337c[2];
	unitOff[0] = dat_802f33a0[0];
	unitOff[1] = dat_802f33a0[1];
	unitOff[2] = dat_802f33a0[2];
	up[0] = dat_802f33c4[0];
	up[1] = dat_802f33c4[1];
	up[2] = dat_802f33c4[2];

	memcpy(enabled, unk_80423030, sizeof(enabled));
	memcpy(color, dat_802f33e8, sizeof(color));

	if (mode == 1) {
		enabled[2] = 1;
		color[2][0] = 0xff;
		color[2][1] = 0xff;
		color[2][2] = 0xff;
	} else {
		enabled[0] = 1;
		enabled[1] = 1;
		enabled[2] = 1;
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
