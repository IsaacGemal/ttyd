#include "eff/n64/eff_stardust_n64.h"
#include "driver/effdrv.h"

// Work data allocated by effStardustN64Entry and stored in EffectEntry->userdata.
// Only the fields written by this file are named; the rest is padding.
typedef struct EffStardustN64Data {
    u8 pad0[0x1C];
    f32 scale;   // 0x1C
    u8 pad1[0x1C];
    s32 drawCam; // 0x3C
} EffStardustN64Data;

void effStardustN64SetDrawCam(EffectEntry* entry, s32 cam) {
    ((EffStardustN64Data*)entry->userdata)->drawCam = cam;
}

void effStardustN64SetScale(EffectEntry* entry, f32 scale) {
    ((EffStardustN64Data*)entry->userdata)->scale = scale;
}
