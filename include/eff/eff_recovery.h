#pragma once

#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include "driver/effdrv.h"

typedef struct RecoveryMayaAnim {
	u16 flags;       // 0x00
	u8 pad_02[2];    // 0x02
	f32 frame;       // 0x04
	u32 timeLo;      // 0x08
	u32 timeHi;      // 0x0C
	void* animData;  // 0x10
	Mtx mtx;         // 0x14
} RecoveryMayaAnim;

typedef struct RecoveryData {
	s8 type;         // 0x00
	s8 field_1;      // 0x01
	s8 state;        // 0x02
	u8 pad_03;       // 0x03
	s32 color;       // 0x04
	f32 x;           // 0x08
	f32 y;           // 0x0C
	f32 z;           // 0x10
	f32 velX;        // 0x14
	f32 velY;        // 0x18
	f32 velZ;        // 0x1C
	f32 scaleX;      // 0x20
	f32 scaleY;      // 0x24
	f32 scaleZ;      // 0x28
	s16 timer;       // 0x2C
	u8 pad_2E[2];    // 0x2E
	f32 angle;       // 0x30
	u32 timeBase;    // 0x34
	RecoveryMayaAnim* anim; // 0x38
	s32 delay;       // 0x3C
	f32 field_40;    // 0x40
} RecoveryData;

struct EffectEntry;
#include <dolphin/mtx.h>
typedef struct RecoveryMayaAnim {
	u16 flags;       // 0x00
	u8 pad_02[2];    // 0x02
	f32 frame;       // 0x04
	u32 timeLo;      // 0x08
	u32 timeHi;      // 0x0C
	void* animData;  // 0x10
	Mtx mtx;         // 0x14
} RecoveryMayaAnim;

typedef struct RecoveryData {
	s8 type;         // 0x00
	s8 field_1;      // 0x01
	s8 state;        // 0x02
	u8 pad_03;       // 0x03
	s32 color;       // 0x04
	f32 x;           // 0x08
	f32 y;           // 0x0C
	f32 z;           // 0x10
	f32 velX;        // 0x14
	f32 velY;        // 0x18
	f32 velZ;        // 0x1C
	f32 scaleX;      // 0x20
	f32 scaleY;      // 0x24
	f32 scaleZ;      // 0x28
	s16 timer;       // 0x2C
	u8 pad_2E[2];    // 0x2E
	f32 angle;       // 0x30
	u32 timeBase;    // 0x34
	RecoveryMayaAnim* anim; // 0x38
	s32 delay;       // 0x3C
	f32 field_40;    // 0x40
} RecoveryData;

EffectEntry* effRecoveryEntry(s32 type, s32 count, f32 x, f32 y, f32 z);
#include "driver/camdrv.h"
void effRecoveryDisp4(CameraId camId, void* param);
void effRecoveryDisp3(CameraId camId, void* param);
void effRecoveryDisp2(CameraId camId, void* param);
void effRecoveryDisp(CameraId camId, void* param);
void effRecoveryMain2(EffectEntry* effect);
void effRecoveryMain(EffectEntry* effect);
extern u32 recovery_effAnimDt[3];
