#include "eff/n64/eff_stardust_n64.h"
#include "driver/effdrv.h"

// Work data allocated by effStardustN64Entry and stored in EffectEntry->userdata.
// Only the field written by this file is named; the rest is padding.
typedef struct EffStardustN64Data {
    u8 pad[0x3C];
    s32 drawCam; // 0x3C
} EffStardustN64Data;

void effStardustN64SetDrawCam(EffectEntry* entry, s32 cam) {
    ((EffStardustN64Data*)entry->userdata)->drawCam = cam;
}
