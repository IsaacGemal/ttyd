#include "driver/effdrv.h"
#include "eff/eff_n64.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sdata2 constants (named after their original addresses).
const f32 float_255_8041eab8 = 255.0f;
const f32 float_deg2rad_8041eabc = 0.017453292f;

// .rodata constant (named after its original address).
const f64 double_to_int_802fa340 = 4503601774854144.0;

// .sbss2 zero GXColor the original copies before filling its fields.
extern GXColor unk_804230bc;

// .data vertex strip; the original is a raw 0x1C0-byte object and main_dl
// passes 0x38-byte slices of it to effSetVtxDescN64.
static u32 size8x8_tex22x22_vtx[0x70] = {
    0xFFF8FFF8, 0x00000000, 0x00000000, 0x00FF0008, 0xFFF80000, 0x02C00000,
    0x000000FF, 0x00080008, 0x000002C0, 0x02C00000, 0x00FFFFF8, 0x00080000,
    0x000002C0, 0x000000FF, 0xFFF8FFF8, 0x000002C0, 0x00000000, 0x00FF0008,
    0xFFF80000, 0x05800000, 0x000000FF, 0x00080008, 0x00000580, 0x02C00000,
    0x00FFFFF8, 0x00080000, 0x02C002C0, 0x000000FF, 0xFFF8FFF8, 0x00000580,
    0x00000000, 0x00FF0008, 0xFFF80000, 0x08400000, 0x000000FF, 0x00080008,
    0x00000840, 0x02C00000, 0x00FFFFF8, 0x00080000, 0x058002C0, 0x000000FF,
    0xFFF8FFF8, 0x00000840, 0x00000000, 0x00FF0008, 0xFFF80000, 0x0B000000,
    0x000000FF, 0x00080008, 0x00000B00, 0x02C00000, 0x00FFFFF8, 0x00080000,
    0x084002C0, 0x000000FF, 0xFFF8FFF8, 0x00000B00, 0x00000000, 0x00FF0008,
    0xFFF80000, 0x0DC00000, 0x000000FF, 0x00080008, 0x00000DC0, 0x02C00000,
    0x00FFFFF8, 0x00080000, 0x0B0002C0, 0x000000FF, 0xFFF8FFF8, 0x00000DC0,
    0x00000000, 0x00FF0008, 0xFFF80000, 0x10800000, 0x000000FF, 0x00080008,
    0x00001080, 0x02C00000, 0x00FFFFF8, 0x00080000, 0x0DC002C0, 0x000000FF,
    0xFFF8FFF8, 0x00001080, 0x00000000, 0x00FF0008, 0xFFF80000, 0x13400000,
    0x000000FF, 0x00080008, 0x00001340, 0x02C00000, 0x00FFFFF8, 0x00080000,
    0x108002C0, 0x000000FF, 0xFFF8FFF8, 0x00001340, 0x00000000, 0x00FF0008,
    0xFFF80000, 0x16000000, 0x000000FF, 0x00080008, 0x00001600, 0x02C00000,
    0x00FFFFF8, 0x00080000, 0x134002C0, 0x000000FF,
};

// Layout of one BrokenBarrierN64 particle; the base element (index 0) also
// carries the shared colour whose bytes main_dl caches before the loop.
typedef struct effBrokenBarrierData {
    u8 pad_00[0x4];   // 0x0
    f32 x;            // 0x4
    f32 y;            // 0x8
    f32 z;            // 0xC
    u8 pad_10[0x20];  // 0x10
    s32 field_0x30;   // 0x30
    f32 field_0x34;   // 0x34
    u8 pad_38[0x14];  // 0x38
    s32 field_0x4C;   // 0x4C
    u32 field_0x50;   // 0x50
    u32 field_0x54;   // 0x54
    u32 field_0x58;   // 0x58
    u32 field_0x5C;   // 0x5C
    f32 field_0x60;   // 0x60
    u8 pad_64[0x8];   // 0x64
} effBrokenBarrierData;

// NONMATCHING (99.95%): all instructions match except that mwcc builds the by-value GXSetTevColor
// GXColor temporary at r1+0xC and its outgoing copy at r1+0x8, while the original has them at
// r1+0x8 and r1+0xC (the known mwcc stack-temp quirk of effBoobooDisp / shadowCharShadowDisp_Polygon).
static void main_dl(EffectEntry* effect, Mtx matrix) {
    effBrokenBarrierData* data;
    Mtx mtx, scale, rot;
    s32 c0, c1, c2;
    s32 i;
    effVtxArray* vtx;
    GXColor color;
    f32 alphaScale;
    f32 v;

    data = effect->userdata;
    c0 = data->field_0x50;
    c1 = data->field_0x54;
    c2 = data->field_0x58;
    alphaScale = (f32)data->field_0x4C / float_255_8041eab8;
    data++;
    for (i = 1; i < effect->count; i++, data++) {
        if (data->field_0x30 < 0) {
            continue;
        }
        PSMTXTrans(mtx, data->x, data->y, data->z);
        PSMTXRotRad(rot, 'z', float_deg2rad_8041eabc * data->field_0x34);
        v = data->field_0x60 * alphaScale;
        PSMTXScale(scale, v, v, v);
        PSMTXConcat(mtx, rot, mtx);
        PSMTXConcat(mtx, scale, mtx);
        PSMTXConcat(matrix, mtx, mtx);
        GXLoadPosMtxImm(mtx, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        color = unk_804230bc;
        color.r = c0;
        color.g = c1;
        color.b = c2;
        color.a = (u8)((f32)data->field_0x4C * alphaScale);
        GXSetTevColor(GX_TEVREG0, color);
        switch (i & 7) {
            case 0:
                vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0xA8);
                break;
            case 1:
                vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0xE0);
                break;
            case 2:
                vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0x118);
                break;
            case 3:
                vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0x150);
                break;
            case 4:
                vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0x188);
                break;
            case 5:
                vtx = (effVtxArray*)size8x8_tex22x22_vtx;
                break;
            case 6:
                vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0x38);
                break;
            case 7:
                vtx = (effVtxArray*)((u8*)size8x8_tex22x22_vtx + 0x70);
                break;
        }
        effSetVtxDescN64(vtx);
        GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
        tri2(0, 1, 2, 0, 0, 2, 3, 0);
    }
}
