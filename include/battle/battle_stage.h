#pragma once

#include "evt/evt_cmd.h"

#include <dolphin/gx.h>
#include <dolphin/types.h>
#include "battle/battle_stage_object.h"
#include "battle/battle_database_common.h"

typedef struct BattleSpotEntry {
	u16 flags; // 0x0
	u8 pad_2[0x4 - 0x2]; // 0x2
	Vec pos; // 0x4, translation in drawCone
	Vec rot; // 0x10, rotation in degrees per axis (drawCone)
	Vec scale; // 0x1C
	GXColor color; // 0x28, drawn by batSpotDisp; the alpha byte at 0x2B is read by drawCone
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
	BattleWeapon bgsetAttackA1; //0x10, _get_bgset_attack_data kind 0 (_object_turn_event_A1); none unless hpDamageFn is set
	BattleWeapon bgsetAttackB; //0xD0, _get_bgset_attack_data kind 2 (_object_turn_event_B)
	void* initEvent; //0x190, battle_stage_object starts it at stage setup when set (las and mri point it at their stg_* init events)
	void* objectTurnEventA1; //0x194, BattleStage_DestroyA1 runs it in place of _object_turn_event_A1 when set
	void* objectTurnEventA2; //0x198, BattleStage_DestroyA2 runs it in place of _object_turn_event_A2 when set
	void* objectTurnEventB; //0x19C, BattleStage_DestroyB runs it in place of _object_turn_event_B when set
	void* layerEvents[4]; //0x1A0, battle_stage_object starts layer N's event at setup when the layer has objects
	u8 field_0x1B0; //0x1B0
	s8 layerDestroyed[3]; //0x1B1, layers 0-2: set means stageFlags 0x2/0x4/0x8 at setup, as _mobj_destroy sets them
} BattleStageData;

typedef struct BattleStageColor {
	u8 r; //0x0
	u8 g; //0x1
	u8 b; //0x2
	u8 a; //0x3
} BattleStageColor;

typedef struct BattleStageDark {
	u32 field_0x0;        //0x0, set to 1 in BattleStageInit, never read
	u8 alpha;             //0x4, current darkness drawn by BattleStageDispDark
	u8 baseAlpha;         //0x5
	u8 alphaFrom;         //0x6
	u8 baseFrom;          //0x7
	u8 alphaTo;           //0x8
	u8 baseTo;            //0x9
	u8 pad_A[2];          //0xA
	s32 alphaFrames;      //0xC
	s32 baseFrames;       //0x10
	s32 alphaFramesTotal; //0x14
	s32 baseFramesTotal;  //0x18
	s8 alphaEase;         //0x1C
	s8 baseEase;          //0x1D
	u8 pad_1E[2];         //0x1E
} BattleStageDark;

// BattleStageLight.spline: the target path BattleStageMain follows while flag 4 is set (getSpline's arguments).
typedef struct BattleStageLightSpline {
	s32 knotCount;             //0x0, getSpline's count
	s32 knots[8];              //0x4, knot frames (getSpline's knots); the path ends at the last one
	f32 coef[3][4][8];         //0x24, per axis (x, y, z), getSpline's four coefficient tables
} BattleStageLightSpline;

typedef struct BattleStageLight {
	u32 flags;                 //0x0
	char name[0x20];           //0x4
	BattleStageColor colorCur; //0x24
	BattleStageColor colorFrom; //0x28
	BattleStageColor colorTo;   //0x2C
	s32 colorFrames;           //0x30
	s32 colorFramesTotal;      //0x34
	s8 colorEase;              //0x38
	u8 pad_39[3];              //0x39
	f32 widthCur;              //0x3C
	f32 widthFrom;             //0x40
	f32 widthTo;               //0x44
	s32 widthFrames;           //0x48
	s32 widthFramesTotal;      //0x4C
	s8 widthEase;              //0x50
	u8 pad_51[3];              //0x51
	Vec posCur;                //0x54
	Vec posFrom;               //0x60
	Vec posTo;                 //0x6C
	s32 posFrames;             //0x78
	s32 posFramesTotal;        //0x7C
	s8 posEase;                //0x80
	u8 pad_81[3];              //0x81
	Vec targetCur;             //0x84
	Vec targetFrom;            //0x90
	Vec targetTo;              //0x9C
	s32 targetFrames;          //0xA8
	s32 targetFramesTotal;     //0xAC
	s8 targetEase;             //0xB0
	u8 pad_B1[3];              //0xB1
	Vec upCur;                 //0xB4
	Vec upFrom;                //0xC0
	Vec upTo;                  //0xCC
	s32 upFrames;              //0xD8
	s32 upFramesTotal;         //0xDC
	s8 upEase;                 //0xE0
	u8 pad_E1[3];              //0xE1
	BattleStageLightSpline* spline; //0xE4
} BattleStageLight;

typedef struct BattleStage {
	u32 field_0x0;               //0x0
	BattleStageLight lights[12]; //0x4
	BattleStageDark stgDark;     //0xAE4
	BattleStageDark audDark;     //0xB04
	Vec stageSize;               //0xB24, btlevtcmd_GetStageSize; y is the stage object spawn height, z the stage depth (floor is 0 up to it)
	Vec audienceAreaSize;        //0xB30, btlevtcmd_GetStageAudienceAreaSize; y is the audience floor height (BattleGetFloorHeight past stageSize.z)
} BattleStage;

BattleStage* BattleStageGetPtr(void);

extern const f32 float_290_8041c854;
extern const f32 float_200_8041c858;
extern const f32 float_90_8041c85c;
extern const f32 float_neg50_8041c860;
extern const f32 float_neg100_8041c830;
extern const f32 float_50_8041c834;



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
void batSpotMain(void);
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
void BattleStageDisp(void);
void BattleStageDispDark(CameraId camId, void* param);
void BattleStageDispLight(CameraId camId, void* param);
void tou_gamen_screen_tev(void* param);
void BattleStageOnLightFriendInCommand(void);
void BattleStageOnLightEnemyInCommand(void);
f32 getSpline(f32* a, f32* b, f32* c, f32* d, s32* knots, s32 count, s32 value);

void BattleStageOnLightInCommand(void);
void BattleStageOffLightFriendInCommand(void);
void BattleStageOffLightEnemyInCommand(void);
