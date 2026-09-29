#pragma once

typedef struct EffectEntry EffectEntry;

#include <dolphin/types.h>
BOOL effStageClearEndChk(EffectEntry* effect);
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include "manager/evtmgr.h"
typedef struct effStageClearWork {
	s32 type;         // 0x00
	f32 x;            // 0x04
	f32 y;            // 0x08
	f32 z;            // 0x0C
	f32 field_0x10;   // 0x10
	f32 scale;        // 0x14
	s32 timer;        // 0x18
	s32 field_0x1C;   // 0x1C
	EventEntry* evt;  // 0x20
	s32 count;        // 0x24
	s32 done;         // 0x28
} effStageClearWork;
void effStageClearDisp(CameraId camId, void* param);
void effStageClearMain(EffectEntry* effect);
EffectEntry* effStageClearEntry(s32 type, f32 x, f32 y, f32 z);
