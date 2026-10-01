#include "eff/eff_charge.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "math_ext.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

const Vec vec3_802f8e60 = { 0.0f, 0.0f, 0.0f };
const char str_802F8E78[] = "Charge";

extern const f64 double_to_int_802f8e70;
extern GXColor unk_80423068;
extern GXColor unk_8042306c;


// NONMATCHING (80.49%): the s32->f32 conversion pool symbol plus the schedule/register allocation of the
// particle-init loop (frame 0x80 vs 0x70, extra conversion spills, A/B interleave order).
EffectEntry* effChargeEntry(s32 type, s32 a2, f32 x, f32 y, f32 z, f32 scale) {
    EffectEntry* entry;
    ChargeEffectData* data;
    ChargeEffectData* elem;
    s32 i;

    entry = effEntry();
    entry->type = str_802F8E78;
    entry->count = 0x1f;
    data = __memAlloc(HEAP_EFFECT, 0x8b8);
    entry->userdata = data;
    entry->callback = effChargeMain;
    data->type = type;
    data->counter = 0;
    data->timer = a2;
    data->alpha = 0;
    data->x = x;
    data->y = y;
    data->z = z;
    data->scale = scale;

    switch (type) {
    case 0:
        data->colorR = 0xff;
        data->colorG = 0;
        data->colorB = 0xff;
        data->field_0x3C = 0xff;
        data->field_0x40 = 0xff;
        data->field_0x44 = 0xff;
        break;
    case 1:
        data->colorR = 0xff;
        data->colorG = 0x14;
        data->colorB = 0x21;
        data->field_0x3C = 0x32;
        data->field_0x40 = 0x14;
        data->field_0x44 = 0x0a;
        break;
    default:
        data->colorR = 0;
        data->colorG = 0;
        data->colorB = 0;
        data->field_0x3C = 0xff;
        data->field_0x40 = 0xff;
        data->field_0x44 = 0xff;
        break;
    }

    for (i = 1; i < 0x1f; i += 2) {
        elem = &data[i];
        elem[0].angle1 = 9.0f * (360.f * (f32)(i - 1)) / 30.f;
        elem[0].angle2 = 0.0f;
        elem[0].radius = (f32)((i - 1) * 100 / 30);
        elem[0].field_0x00 = 0;
        elem[1].angle1 = 9.0f * (360.f * (f32)i) / 30.f;
        elem[1].angle2 = 0.0f;
        elem[1].radius = (f32)(i * 100 / 30);
        elem[1].field_0x00 = 0;
    }
    return entry;
}

// `s32 odd` declared first plus the local `counter`/`angle2` reproduce the original's register allocation. The clamp reads the 0.0f literal the original pools (not the named extern).
void effChargeMain(EffectEntry* effect) {
    s32 odd;
    ChargeEffectData* data = effect->userdata;
    Vec position;
    Vec disp;
    f32 s1;
    f32 c1;
    f32 s2;
    f32 c2;
    f32 a1;
    f32 a2;
    f32 radius;
    s32 i;

    disp = vec3_802f8e60;
    disp.x = data->x;
    disp.y = data->y;
    disp.z = data->z;
    position = disp;

    if (data->timer < 1000) {
        data->timer--;
    }
    data->counter++;
    if (data->timer < 0) {
        effDelete(effect);
        return;
    }
    {
        s32 counter = data->counter;
        if (counter <= 0x14) {
            data->alpha = counter * 12;
        } else {
            data->alpha = 0xff;
        }
        if (counter & 1) {
            data->alpha /= 2;
        }
    }
    odd = data->counter & 1;
    data++;
    for (i = 1; i < effect->count; i++, data++) {
        f32 angle2;
        radius = data->radius;
        angle2 = data->angle2;
        a1 = 6.2831855f * data->angle1 / 360.f;
        s1 = (f32)sin(a1);
        c1 = (f32)cos(a1);
        a2 = 6.2831855f * angle2 / 360.f;
        s2 = (f32)sin(a2);
        c2 = (f32)cos(a2);
        data->x = radius * s1 * c2;
        data->y = radius * c1 * c2;
        data->z = radius * s2;
        data->alpha = (s32)(100.f - data->radius) * 510 / 100;
        if (data->alpha > 0xff) {
            data->alpha = 0xff;
        }
        data->scale = 0.2f + data->radius / 80.f;
        data->radius -= (f32)((i & 3) * 2 + 2);
        if (data->radius < 0.0f) {
            data->radius = 0.0f;
            data->scale = 0.0f;
        }
        data->angle1 += (f32)(odd ? 5 : 2);
        data->angle2 += (f32)(odd ? 2 : 5);
    }
    dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effChargeDisp, effect);
}

#ifdef MUST_MATCH
// The original builds each TEV colour in a helper that returns it by value; passing the base colour into the
// first helper as a parameter (rather than reading the global inside it) is what makes mwcc place the inline
// local and the outgoing argument copy in retail's stack slots.
static inline GXColor effChargeColorBase(GXColor c, u8 r, u8 g, u8 b) {
    c.r = r;
    c.g = g;
    c.b = b;
    return c;
}
static inline GXColor effChargeColorElem(u8 r, u8 g, u8 b, u8 a) {
    GXColor c = unk_8042306c;
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = a;
    return c;
}
#endif
// Matches. Two shapes were needed beyond the straightforward code: the -yaw angle is built from
// `camGetPtr(cameraId)->viewYaw` inline (a cached `camera` local gives mwcc the wrong fpr and `fmuls` order),
// and the zero-scale test is written `if (!scale) continue;` (a `scale == 0.0f` literal emits the reversed
// `fcmpu` operand order; `!scale` keeps the literal in the pool and compares f30 first).
void effChargeDisp(CameraId cameraId, void* param) {
    EffectEntry* effect = param;
    ChargeEffectData* data = effect->userdata;
    GXColor color2;
    GXColor color;
    GXTexObj tex;
    Mtx mtxF8;
    Mtx mtxC8;
    Mtx mtx98;
    Mtx mtx68;
    Mtx mtx38;
    s32 colR;
    s32 colG;
    s32 colB;
    f32 scale;
    s32 i;

    colR = data->colorR;
    colG = data->colorG;
    colB = data->colorB;
    PSMTXScale(mtx38, data->scale, data->scale, 1.0f);
    PSMTXTransApply(mtx38, mtx38, data->x, data->y, data->z);
    PSMTXRotRad(mtxC8, 'y', -camGetPtr(cameraId)->viewYaw * 0.017453292f);
    PSMTXConcat(mtx38, mtxC8, mtx38);

    effGetTexObj(0x1e, &tex);
    GXLoadTexObj(&tex, GX_TEXMAP0);

#ifdef MUST_MATCH
    GXSetTevColor(GX_TEVREG1, effChargeColorBase(unk_80423068, data->field_0x3C, data->field_0x40, data->field_0x44));
#else
    color = unk_80423068;
    color.r = data->field_0x3C;
    color.g = data->field_0x40;
    color.b = data->field_0x44;
    GXSetTevColor(GX_TEVREG1, color);
#endif

    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
    GXSetNumTevStages(2);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_C0, GX_CC_C1, GX_CC_CPREV, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_A0, GX_CA_APREV, GX_CA_ZERO);
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

    data++;
    for (i = 1; i < effect->count; i++, data++) {
        scale = data->scale;
        if (!scale) {
            continue;
        }

#ifdef MUST_MATCH
        color2 = effChargeColorElem(colR, colG, colB, data->alpha);
#else
        color2 = unk_8042306c;
        color2.r = colR;
        color2.g = colG;
        color2.b = colB;
        color2.a = data->alpha;
#endif
        GXSetTevColor(GX_TEVREG0, color2);

        PSMTXTrans(mtxF8, data->x, data->y, data->z);
        PSMTXScale(mtx98, scale, scale, scale);
        PSMTXConcat(mtxF8, mtx98, mtxF8);
        PSMTXConcat(mtx38, mtxF8, mtxF8);
        PSMTXConcat(camGetPtr(cameraId)->view, mtxF8, mtx68);
        GXLoadPosMtxImm(mtx68, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);

        if ((i & 1) == 0) {
                                    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
            GXPosition3f32(-8.0f, -8.0f, 0.0f);
            GXTexCoord2f32(0.0f, 0.0f);
            GXPosition3f32(8.0f, -8.0f, 0.0f);
            GXTexCoord2f32(1.0f, 0.0f);
            GXPosition3f32(8.0f, -24.0f, 0.0f);
            GXTexCoord2f32(1.0f, 1.0f);
            GXPosition3f32(-8.0f, -24.0f, 0.0f);
            GXTexCoord2f32(0.0f, 1.0f);
        } else {
                                    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
            GXPosition3f32(-12.0f, -12.0f, 0.0f);
            GXTexCoord2f32(0.0f, 0.0f);
            GXPosition3f32(12.0f, -12.0f, 0.0f);
            GXTexCoord2f32(1.0f, 0.0f);
            GXPosition3f32(12.0f, -36.0f, 0.0f);
            GXTexCoord2f32(1.0f, 1.0f);
            GXPosition3f32(-12.0f, -36.0f, 0.0f);
            GXTexCoord2f32(0.0f, 1.0f);
        }
    }
}
