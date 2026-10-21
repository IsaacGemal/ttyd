#pragma once

#include <dolphin/mtx.h>
#include "evt/evt_cmd.h"

typedef struct BattleWeapon BattleWeapon;

typedef struct BattleStageObjectData {
	char* name; //0x0
	u16 mSpawnSpecialUnitType; //0x4
	u16 mLayer; //0x6
	Vec mPosition; //0x8
	u8 mNumFramesToStartFalling; //0x14
	u8 mNumFramesToFall; //0x15
	u8 pad_16[2]; //0x16, TODO remove cuz compiler auto-pads
} BattleStageObjectData;

enum BattleStageObjectSpawnType {
	BSO_SPAWN_TREE = 0, //spawns the tree unit (_entry_tree_data)
	BSO_SPAWN_SWITCH = 1, //spawns the switch unit (_entry_switch_data)
	BSO_SPAWN_NONE = 2, //no unit
};

enum BattleStageObjectFlags {
	BSO_FLAG_HIDDEN = 0x1, //keeps map group flag 1 (hidden: the map draw loop skips it) on for the map object while set
	BSO_FLAG_DIRTY = 0x2, //transform changed: BattleStageObjectMain re-applies it to the map object, then clears this
};

typedef struct BattleStageObject {
	s32 id; //0x0
	Vec mPosition; //0x4
	Vec mPositionOffset1; //0x10, added to mPosition on every layer (layer 2 pivots about its y)
	Vec mRotation; //0x1C
	Vec mPositionOffset2; //0x28, added to mPosition on every layer
	Vec mLastPosition; //0x34, mPosition as last applied to the map object
	Vec mLastPositionOffset1; //0x40, mPositionOffset1 as last applied
	Vec mLastRotation; //0x4C, mRotation as last applied
	Vec mLastPositionOffset2; //0x58, mPositionOffset2 as last applied
	BattleStageObjectData* data; //0x64
	u32 mFlags; //0x68
	u32 mBattleUnitIdx; //0x6C
	u8 mNumFramesToStartFalling; //0x70
	u8 mNumFramesFalling; //0x71
	u8 field_0x72; //0x72
	u8 mShakePeriodLength; //0x73
	u8 mShakePeriodPosition; //0x74
	u8 pad_75[3]; //0x75
	f32 mShakeAngle; //0x78
} BattleStageObject;

BattleStageObject* BattleGetObjectPtr(s32 id);
BattleStageObject* BattleSearchObjectPtr(const char* name);

void BattleStageObjectMain(void);





USER_FUNC(_set_mobj_shake_init);


void BattleStageObjectInit(void);
BOOL BattleFogForceStop(void);
s32 BattleStage_ObjectFallCheck(void);
void BattleStage_NozzleWorkCheck(s32 flag);
void BattleStage_WallCloseCheck(void);
void BattleObjectConfig(void);
void _Nozzle_Change_Check(u8 index, void* nozzle);
s32 _get_nozzle_attack_index(u8 index);
void _combine_nozzle_weapon(s32 mode, BattleWeapon* dst, BattleWeapon* src);
void _mobj_shake_main(BattleStageObject* obj);
void _disp_escape_wall_message(CameraId cameraId, void* param);
s32 BattleFogEndCheck(void);
void BattleStage_IronFrameFallCheck(void);
void BattleStage_NozzleDirChangeCheck(void);
void BattleStage_DestroyA1(void);
void BattleStage_DestroyA2(void);
void BattleStage_DestroyB(void);

s32 _bgset_audience_bgset_access(void);
s32 _fall_frame_wait(void);
void _nozzle_type_init(void);
s32 _nozzle_work_wait(void);
f32 _get_nozzle_dir(s32 index);
s32 _nozzle_work_attack_set(void);
void _rate_mix(u8* a, u8* b, s8* c, u8* d);
void _rate_mix_3(u8* a, u8* b, u8* c, s8* d, u8* e, u8* f);
s32 _object_fall_wait(void);
USER_FUNC(_set_mobj_rotate_x);
USER_FUNC(_mobj_set_alpha);
USER_FUNC(_mobj_destroy);
USER_FUNC(_get_bgset_attack_data);
USER_FUNC(_bgset_kemuri_effect);
USER_FUNC(_set_mobj_shake);
USER_FUNC(_nozzle_turn);
USER_FUNC(_announce_wall_message);
USER_FUNC(_fall_frame_wait_unit_ready);
USER_FUNC(_fall_frame);
USER_FUNC(_fall_frame_delete);
USER_FUNC(_nozzle_work_effect);
USER_FUNC(_nozzle_work_get_weapon_address);
USER_FUNC(_object_fall_attack);
