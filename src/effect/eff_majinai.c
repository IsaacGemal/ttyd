#include "eff/eff_majinai.h"
#include "driver/effdrv.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/mapdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>

extern const f32 float_1_80422594;
extern const u32 dat_80422568;
extern const char str_80301940[];
extern const f32 float_deg2rad_80422578;
extern const u32 dat_80422574;
extern const f32 float_0_8042257c;
extern const f32 float_10_80422580;

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

static inline void majinaiSetColor(MajinaiWork* work) {
    GXColor color = *(GXColor*)&dat_80422574;
    color.a = work->alpha;
    animPoseSetMaterialEvtColor(work->animPose, color);
}

void effMajinaiDisp(CameraId cameraId, void* param) {
    EffectEntry* effect = param;
    MajinaiWork* work = effect->userdata;
    Mtx trans;
    Mtx scale;
    Mtx rot;

    if (work->animPose != -1) {
        PSMTXTrans(trans, work->x, work->y, work->z);
        PSMTXScale(scale, work->field_0x10, work->field_0x10, work->field_0x10);
        PSMTXRotRad(rot, 'y', float_deg2rad_80422578 * -camGetPtr(cameraId)->viewYaw);
        PSMTXConcat(trans, rot, trans);
        PSMTXConcat(trans, scale, trans);
        animPoseSetMaterialFlagOn(work->animPose, 0x840);
        majinaiSetColor(work);
        animPoseMain(work->animPose);
        animPoseDrawMtx(work->animPose, trans, 1, float_0_8042257c, float_10_80422580);
        animPoseDrawMtx(work->animPose, trans, 2, float_0_8042257c, float_10_80422580);
        animPoseDrawMtx(work->animPose, trans, 3, float_0_8042257c, float_10_80422580);
    }
}
