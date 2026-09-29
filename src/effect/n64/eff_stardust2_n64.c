#include "eff/n64/eff_stardust2_n64.h"

void effStardust2N64SetDrawCam(EffectEntry* entry, s32 cam) {
	*(s32*)((u8*)entry->userdata + 0x2C) = cam;
}
