#include "eff/n64/eff_butterfly_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .rodata
const f64 double_to_int_802fa3a8 = 4503601774854144.0;
const char str_802FA3C8[] = "ButterflyN64";

// .sdata2
extern const u32 dat_8041eb68;
extern const f32 float_0p3_8041eb6c;
extern const f32 float_deg2rad_8041eb70;
extern const f32 float_0p02_8041eb74;
extern const f32 float_0p03125_8041eb78;
extern const f32 float_0p015625_8041eb7c;
extern const f32 float_0_8041eb80;
extern const f32 float_50_8041eb90;
extern const f32 float_30_8041eb94;

// .data / .sdata
extern u8 but_02_v[];

// .sdata
static s8 y_data[32] = {
    -1, -2, -3, -4, -5, -6, -7, -8,
    -9, -10, -11, -12, -13, -14, -15, -16,
    -17, -18, -19, -17, -15, -13, -11, -9,
    -7, -5, -3, -1, 0, 0, 0, 0,
};

typedef struct effButterflyData {
    s32 type;       // 0x0
    s32 field_0x4;  // 0x4
    s32 field_0x8;  // 0x8
    Vec position;   // 0xC
    Vec field_0x18; // 0x18
    s32 field_0x24; // 0x24
    f32 field_0x28; // 0x28
    s32 field_0x2C; // 0x2C
    s32 field_0x30; // 0x30
    f32 field_0x34; // 0x34
    f32 field_0x38; // 0x38
} effButterflyData;

EffectEntry* effButterflyN64Entry(s32 a, f32 x, f32 y, f32 z) {
    EffectEntry* effect;
    effButterflyData* data;

    effect = effEntry();
    effect->type = str_802FA3C8;
    effect->count = 1;
    data = __memAlloc(HEAP_EFFECT, 0x48);
    effect->userdata = data;
    effect->callback = effButterflyMain;
    effect->flags |= 2;
    data->type = a;
    data->field_0x4 = 0x3E8;
    data->field_0x24 = 0xFF;
    data->position.x = x;
    data->field_0x18.x = x;
    data->position.y = y;
    data->field_0x18.y = y;
    data->position.z = z;
    data->field_0x18.z = z;
    data->field_0x8 = 0;
    data->field_0x2C = 0;
    data->field_0x30 = 0;
    data->field_0x34 = float_50_8041eb90;
    data->field_0x38 = float_30_8041eb94;
    return effect;
}

#ifdef MUST_MATCH
static inline GXColor butterflyColor(u8 v) {
    GXColor c = *(GXColor*)&dat_8041eb68;
    c.r = v;
    c.g = v;
    c.b = v;
    return c;
}
#endif

// NONMATCHING (99.96%): the GXColor local and by-value argument stack slots
// are swapped (original builds at r1+0x8 and copies to r1+0xC), and the
// int->float magic is an anonymous @166, not double_to_int_802fa3a8.
void effButterflyDisp(CameraId camId, void* param) {
    EffectEntry* effect = param;
    CameraEntry* cam;
    effButterflyData* data;
    s32 alpha;
    s32 type;
    GXColor color;
    GXTexObj texObj;
    Mtx trans;
    Mtx scale;
    Mtx rot;

    cam = camGetPtr(camId);
    data = effect->userdata;
    alpha = data->field_0x24;
    type = data->type;

    PSMTXTrans(trans, data->position.x,
               float_0p3_8041eb6c * (f32)y_data[data->field_0x2C] + data->position.y, data->position.z);
    PSMTXRotRad(rot, 'y', float_deg2rad_8041eb70 * data->field_0x28);
    PSMTXConcat(trans, rot, trans);
    PSMTXScale(scale, float_0p02_8041eb74, float_0p02_8041eb74, float_0p02_8041eb74);
    PSMTXConcat(trans, scale, trans);
    PSMTXConcat(cam->view, trans, trans);
    GXLoadPosMtxImm(trans, 0);
    GXSetCurrentMtx(0);

#ifdef MUST_MATCH
    color = butterflyColor(alpha);
#else
    color = *(GXColor*)&dat_8041eb68;
    color.r = alpha;
    color.g = alpha;
    color.b = alpha;
#endif
    GXSetTevColor(1, color);
    GXSetNumChans(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 2, 8, 0xF);
    GXSetTevAlphaIn(0, 7, 7, 7, 4);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
    PSMTXScale(scale, float_0p03125_8041eb78, float_0p015625_8041eb7c, float_0_8041eb80);
    GXLoadTexMtxImm(scale, 0x1E, 1);
    effGetTexObjN64(type + 0x78, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetCullMode(0);
    effSetVtxDescN64((effVtxArray*)(but_02_v + data->field_0x2C * 0x54));
    GXBegin(0x90, 0, 0xC);
    tri2(0, 1, 2, 0, 0, 2, 3, 0);
    tri2(3, 4, 0, 0, 3, 5, 4, 0);
}
