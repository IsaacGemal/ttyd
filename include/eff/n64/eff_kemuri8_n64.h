#pragma once

#include <dolphin/types.h>
#include "driver/effdrv.h"
// One element of this effect's userdata array is 0x5C bytes; the camera id lives at 0x58.
typedef struct effKemuri8Data {
	u8 pad_0x00[0x58]; // 0x0
	s32 camId;         // 0x58
} effKemuri8Data;
void effKemuri8N64SetCamId(EffectEntry* entry, s32 camId);
