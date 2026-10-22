#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "battle/battle.h"
#include "battle/battle_stage.h"
#include <dolphin/types.h>

// Locally declared prototypes (move to battle/battle_stage.h later).

// .sbss2
// .sbss2: the default .sbss2 is only used for compiler-generated pool
// templates, so pair it with a small-data section and place it by hand.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") u8 gap_10_80423034_sbss2[4];

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

s32 evt_batstage_light_set_player_on(EventEntry* event, BOOL isFirstCall) {
	BattleWork* bw = _battleWorkPointer;
	s32* args = event->args;
	s32 mode = evtGetValue(event, *args++);
	char name[3][0x20] = { "sys_activeUnit_1", "sys_activeUnit_2", "sys_activeUnit_3" };
	Vec pos[3] = { { 100.0f, 250.0f, 70.0f }, { -300.0f, 250.0f, 70.0f }, { -120.0f, 250.0f, -70.0f } };
	Vec target[3] = { { 100.0f, 0.0f, -70.0f }, { -300.0f, 0.0f, -70.0f }, { -120.0f, 0.0f, -70.0f } };
	Vec unitOff[3] = { { 15.0f, 0.0f, 15.0f }, { -15.0f, 0.0f, 15.0f }, { 0.0f, 0.0f, -15.0f } };
	Vec up[3] = { { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, -1.0f } };
	u8 color[3][3] = { { 0x78, 0x78, 0x78 }, { 0x78, 0x78, 0x78 }, { 0x78, 0x78, 0x78 } };
	u8 enabled[3] = { 0, 0, 0 };
	s32 i;
	s32 lightNo;

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
