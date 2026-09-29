#include "eff/eff_majinai.h"
#include "driver/effdrv.h"
#include "driver/mapdrv.h"
#include "memory.h"

extern const f32 float_1_80422594;
extern const u32 dat_80422568;
extern const char str_80301940[];

EffectEntry* effMajinaiEntry(s32 type, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    MajinaiWork* work;
    MapWork* map;

    effect = effEntry();
    effect->type = str_80301940;
    effect->count = 1;
    work = __memAlloc(HEAP_EFFECT, effect->count * sizeof(MajinaiWork));
    effect->userdata = work;
    effect->callback = effMajinaiMain;
    effect->flags |= 2;

    work->type = type;
    work->x = x;
    work->y = y;
    work->z = z;
    work->field_0x10 = float_1_80422594;
    work->alpha = 0xff;
    work->state = 0;
    work->animPose = -1;

    // The map's third GXColor (0x1E) is still unnamed inside MapEntry.field_0x16.
    map = mapGetWork();
    *(u32*)&map->entries[0].field_0x16[8] = dat_80422568;

    return effect;
}
