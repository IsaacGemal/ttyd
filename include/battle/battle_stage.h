#pragma once

#include "evt/evt_cmd.h"

#include <dolphin/types.h>
#include "battle/battle_stage_object.h"

typedef struct BattleSpotEntry {
	u16 flags; // 0x0
	u8 field_0x2[0x4 - 0x2]; // 0x2
	Vec field_4; // 0x4
	Vec field_10; // 0x10
	Vec field_1C; // 0x1C
	s32 field_28; // 0x28, unknown, unused and set by an sbss2 var, TODO?
} BattleSpotEntry;

typedef struct BattleSpotWork {
	s32 count; //0x0
	BattleSpotEntry* entries; //0x4
	BOOL skipRender; //0x8
} BattleSpotWork;

typedef struct BattleStageData {
	char* mGlobalStageDataDir; //0x0
	char* mSpecificStageDataDir; //0x4
	u32 mNumObjects; //0x8
	BattleStageObjectData* mObjects; //0xC, TODO extern?
} BattleStageData;

typedef struct BattleWorkStage {
	u8 field_0x0[0xB3C - 0x0]; //0x0
} BattleStage;

BattleStage* BattleStageGetPtr(void);



void BattleStageInit(void);
void BattleStageMain(void);
void BattleStageEnd(void);

void BattleStageSetStgDark(u8 a, s32 frames, s32 ease);
void BattleStageSetStgDarkBase(u8 a, s32 frames, s32 ease);
void BattleStageSetAudDark(u8 a, s32 frames, s32 ease);
void BattleStageSetAudDarkBase(u8 a, s32 frames, s32 ease);
void BattleStageReturnStgDarkBase(s32 frames, s32 ease);
void BattleStageReturnAudDarkBase(s32 frames, s32 ease);
void BattleStageOffLightInCommand(s32 arg0, s32 arg1);
void batSpotInit(void);
s32 screen_evt_func(void);
s32 BattleStageGetLightNumberByName(const char* name);
s32 BattleStageSysLightEntry(const char* name);
void BattleStageLightRelease(s32 idx, s32 frames, u8 ease);
void BattleStageLightSetLightColor(s32 idx, u8 r, u8 g, u8 b, s32 frames, u8 ease);
void BattleStageLightSetWidth(s32 idx, s32 width, s32 frames, u8 ease);
void BattleStageLightSetPosition(s32 idx, s32 frames, f32 x, f32 y, f32 z, u8 ease);
void BattleStageLightSetUpVec(s32 idx, s32 frames, f32 x, f32 y, f32 z, u8 ease);
void BattleStageLightSetTarget(s32 idx, s32 frames, f32 x, f32 y, f32 z, u8 ease);
void BattleStageLightSetTargetByUnit(s32 idx, s32 unitId, f32 x, f32 y, f32 z, s32 frames, u8 ease);
BattleSpotWork* BatSpotGetPtr(void);
void batSpotDisp(CameraId camId, void* param);
void drawCone(CameraId camId, BattleSpotEntry* entry);
BattleSpotEntry* batSpotEntry(void);
void BattleStageDispDark(CameraId camId, void* param);
void BattleStageDispLight(CameraId camId, void* param);
void tou_gamen_screen_tev(void* param);
void BattleStageOnLightFriendInCommand(void);
void BattleStageOnLightEnemyInCommand(void);

void BattleStageOnLightInCommand(void);
void BattleStageOffLightFriendInCommand(void);
void BattleStageOffLightEnemyInCommand(void);
