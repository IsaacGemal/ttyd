#pragma once

#include "driver/camdrv.h"

#include <dolphin/mtx.h>

typedef struct BattleWorkStatusIconEntry {
	s32 type; //0x0, StatusEffectType
	s32 turnCount; //0x4, |turns|; drawn as a number when non-zero
	u32 texId; //0x8, texture for btlDispTexPlane2; 0 skips the texture
	Vec scale; //0xC, the texture's x/y size scale and z (times 0.8 on x/y)
	Vec pivot; //0x18, the texture is rotated about this point
	Vec rotation; //0x24, degrees about x, y and z
	Vec offset; //0x30, added to the icon's screen position
	GXColor color; //0x3C
	s32 flags; //0x40, 0x1 hides the flat plane; 0x2 hides the texture
} BattleWorkStatusIconEntry;

typedef struct BattleWorkStatusIcon {
	Vec mPosition; //0x0
	Vec mScreenSpaceOffset; //0xC
	f32 mDistanceBetweenIcons; //0x18
	s32 mFlags; //0x1C
	BattleWorkStatusIconEntry mIcons[28]; //0x20
	f32 mGuruguruAngle; //0x790, spin angle of the guruguru icon, advanced 4 per frame
	s16 mNumStatusIcons; //0x794
	s16 mAllergyCounter; //0x796
	s16 mRegenHpCounter; //0x798
	s16 mRegenFpCounter; //0x79A
	s16 mBiribiriState; //0x79C
	s16 mBiribiriDuration; //0x79E
} BattleWorkStatusIcon;

void BattleStatusIconInit(struct BattleUnit* unit);
void BattleStatusIconMain(struct BattleUnit* unit);
void BattleStatusIconDelete(struct BattleUnit* unit);
void BattleStatusIconDisp(CameraId camId, void* param);

struct IconDispTableEntry;
BOOL _bsi_default_turn(struct BattleUnit* unit, struct IconDispTableEntry* disp, BattleWorkStatusIconEntry* entry);
BOOL _bsi_guruguru(struct BattleUnit* unit, struct IconDispTableEntry* disp, BattleWorkStatusIconEntry* entry);
BOOL _bsi_biribiri(struct BattleUnit* unit, struct IconDispTableEntry* disp, BattleWorkStatusIconEntry* entry);
BOOL _bsi_allergy(struct BattleUnit* unit, struct IconDispTableEntry* disp, BattleWorkStatusIconEntry* entry);
BOOL _bsi_regeneration(struct BattleUnit* unit, struct IconDispTableEntry* disp, BattleWorkStatusIconEntry* entry);
