#include "battle/battle.h"
#include "battle/battle_ac.h"
#include "battle/battle_audience.h"
#include "battle/battle_database_common.h"
#include "battle/battle_event_cmd.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/icondrv.h"
#include "eff/eff_starstone.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "memory.h"
#include "pmario_sound.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdlib.h>
#include <string.h>
#include "battle/sac/sac_zubastar.h"

extern BattleWork* _battleWorkPointer;

#pragma pool_data on // the original addresses its .rodata through one base register

#pragma force_active on // the original keeps these .rodata objects; nothing in the code references
                       // them yet, so the linker would otherwise drop them
const char str_802FF668[] = "btl_wn_sac_zubastar";
const char str_802FF67C[] = "msg_zubastar";
const char str_802FF68C[] = "msg_sac_8";
const Vec vec3_802ff6a4 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802ff6b0 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802ff6bc = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802ff6c8 = { 0.0f, 0.0f, 0.0f };
#pragma force_active reset

// .sdata2 constants referenced by this file's original.
extern f32 float_200_804217c8;
extern f32 float_60_804217cc;
extern f32 float_1_804217d0;
extern f32 float_0p01_804217d4;
extern f32 float_128_804217d8;
extern f32 float_255_804217dc;
extern f32 float_0_804217e0;
extern f32 float_20_804217e4;
extern f32 float_deg2rad_804217e8;
extern f32 float_96_804217ec;
extern f32 float_400_804217f0;
extern f32 float_240_804217f4;
extern f32 float_480_804217f8;
extern f32 float_600_804217fc;
extern f32 float_104_80421800;
extern f32 float_70_80421804;
extern f32 float_32767_80421808;
extern f32 float_35_8042180c;
extern f32 float_2_80421810;
extern f32 float_neg1p25_80421814;
extern f32 float_100_80421818;
extern f32 float_2p8_8042181c;
extern f32 float_neg96_80421820;
extern f32 float_344_80421824;
extern f32 float_264_80421828;
extern f32 float_neg240_8042182c;
extern f32 float_5_80421830;
extern f32 float_300_80421834;
extern f32 float_8_80421838;
extern f32 float_neg1_8042183c;
extern f32 float_neg1000_80421840;
extern f32 float_50_80421844;
extern f32 float_0p25_80421848;
extern f32 float_683_8042184c;
extern f32 float_4096_80421850;
extern f32 float_neg0p9_80421854;
extern f32 float_0p1_80421858;
extern f32 float_900_8042185c;
extern f32 float_1Eneg05_80421860;
extern f32 float_37_80421864;
extern f32 float_2160_80421868;
extern f32 float_1p5_8042186c;
extern u32 dat_804217b8;
extern u32 dat_804217bc;
extern const Vec vec3_802ff6b0;
extern const Vec vec3_802ff698;
extern const Vec vec3_802ff6a4;
extern const Vec vec3_802ff6bc;
extern const Vec vec3_802ff6c8;
extern const Vec vec3_802ff7ac;
extern const Vec vec3_802ff7b8;
extern const Vec vec3_802ff7c4;
extern const char str_802FF80C[];

// LOCAL_PROTO(psndSFX_pit): the retail callers disagree on the pitch width and
// signedness; this file passes a full 32-bit signed pitch.
s32 psndSFX_pit(s32 id, s32 pit);

// The original emits this as a local .sdata table (nice_event, local).
static u32 nice_event[8] = {
    0x0005005B, (u32)btlevtcmd_ACSuccessEffect, 0xFE363C80, 0xFFFFFF88,
    0x0000008C, 0,                              2,          1,
};

typedef struct ZubaStarBomb {
    s32 state;       // 0x00
    s32 count;       // 0x04
    Vec* points;     // 0x08
    Vec field_0x0C;  // 0x0C
    Vec field_0x18;  // 0x18
    Vec field_0x24;  // 0x24
    Vec field_0x30;  // 0x30
    f32 field_0x3C;  // 0x3C
    f32 field_0x40;  // 0x40
    f32 field_0x44;  // 0x44
    s32 timer;       // 0x48
    s32 maxTimer;    // 0x4C
} ZubaStarBomb;

typedef struct ZubaStarStar {
    s32 state;     // 0x00
    s32 field_0x04; // 0x04
    s32 timer;     // 0x08
    f32 x0;        // 0x0C
    f32 y0;        // 0x10
    f32 x1;        // 0x14
    f32 y1;        // 0x18
} ZubaStarStar;

struct ZubaStarWork {
    u8 field_0x000[0x04];          // 0x000
    s32 field_0x004;               // 0x004
    s32 field_0x008;               // 0x008
    s32 field_0x00C;               // 0x00C
    s32 field_0x010;               // 0x010
    s32 bombCount;                 // 0x014
    s32 field_0x018;               // 0x018
    s32 field_0x01C;               // 0x01C
    Vec field_0x020;               // 0x020
    Vec field_0x02C[4];            // 0x02C
    Vec field_0x05C;               // 0x05C
    Vec field_0x068;               // 0x068
    s32 field_0x074;               // 0x074
    s16 field_0x078;               // 0x078
    u8 field_0x07A[0x02];          // 0x07A
    s32 field_0x07C;               // 0x07C
    s32 field_0x080;               // 0x080
    f32 field_0x084;               // 0x084
    f32 field_0x088;               // 0x088
    f32 field_0x08C;               // 0x08C
    f32 field_0x090;               // 0x090
    Vec field_0x094;               // 0x094
    u8 field_0x0A0;                // 0x0A0
    u8 field_0x0A1;                // 0x0A1
    u8 field_0x0A2[0x02];          // 0x0A2
    s32 field_0x0A4;               // 0x0A4
    ZubaStarBomb* bombs[0xC8];     // 0x0A8
    ZubaStarStar stars[0x10];      // 0x3C8
    smartEntry* tex;               // 0x588
    s32 starState;                 // 0x58C
    s32 starCounter;               // 0x590
    Vec starCur;                   // 0x594
    Vec starStart;                 // 0x5A0
    Vec starEnd;                   // 0x5AC
    Vec field_0x5B8;               // 0x5B8
    Vec scale1;                    // 0x5C4
    Vec scale2;                    // 0x5D0
    EffectEntry* effect;           // 0x5DC
};

// .text:0x0 | 0x8022E590 | size: 0x368
// The original holds work+0x18 in r30 and reaches field_0x018, field_0x020 and field_0x02C
// through it (the compiler CSEs the base). MWCC here does not, so under MUST_MATCH a local
// u8* b18 = (u8*)work + 0x18 with Z18/Z20/Z2C(i) forces the same base; the natural struct-field
// accesses are under #else. All of disp2D's named float constants are now literals (200.0f, 60.0f, ...),
// which gives MWCC their known values and recovers the original's scheduling; the original TU defined
// them, so its loads use their names and ours use anonymous pooled entries (arg_mismatch only). Same for
// the `pos`/vertex zeros.
void zubastar_disp2D(CameraId cameraId, void* param) {
    ZubaStarWork* work = GetZubaStarPtr();
#ifdef MUST_MATCH
    u8* b18 = (u8*)work + 0x18;
#define Z18 (*(s32*)b18)
#define Z20 ((Vec*)(b18 + 8))
#define Z2C(i) (((Vec*)(b18 + 0x14)) + (i))
#else
#define Z18 (work->field_0x018)
#define Z20 (&work->field_0x020)
#define Z2C(i) (&work->field_0x02C[i])
#endif
    ZubaStarStar* star;
    s32 i;
    s32 alpha;
    Vec pos;

    if (work->field_0x07C >= 5) {
        s32 icon;

        icon = (work->field_0x0A1 <= 3) ? 0x64 : 0x65;
        pos = (Vec){ 0.0f, 0.0f, 0.0f };
        pos.x = work->field_0x094.x - 200.0f;
        pos.y = 60.0f + work->field_0x094.y;
#ifdef MUST_MATCH
        // Cast only to drop the s32->s16 `extsh` the original does not emit: the id is 0x64/0x65,
        // so the upper bits are already clear. The natural declared call is under #else.
        ((void (*)(Vec*, s16, s32, f32))iconDispGx)(&pos, 0x10, icon, 1.0f);
#else
        iconDispGx(&pos, 0x10, icon, 1.0f);
#endif
        BattleAcDrawGauge((s32)work->field_0x094.x, (s32)work->field_0x094.y, 0xB1, 4, 0x1A, 0x34, 0x4E,
                          0, 0.01f * work->field_0x084);
    }
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GXSetCurrentMtx(0);
    GXLoadPosMtxImm(camGetPtr(cameraId)->view, 0);
    GXSetLineWidth(0x18, 0);
    for (i = 0; i < 0x10; i++) {
        star = &work->stars[i];
        if (star->timer <= 0x1E) {
            alpha = (s32)intplGetValue(0, star->timer, 0x1E, 128.0f,
                                       255.0f);
        } else {
            alpha = (s32)intplGetValue(0, star->timer - 0x1E, 0x1E, 255.0f,
                                       128.0f);
        }
        if (star->state == 5) {
            {
#ifdef MUST_MATCH
                GXSetChanMatColor(GX_COLOR0A0, (GXColor){ alpha, alpha, alpha, 0xFF });
#else
                GXColor c = (GXColor){ 0x00, 0x00, 0x00, 0xFF };

                c.r = alpha;
                c.g = alpha;
                c.b = alpha;
                GXSetChanMatColor(GX_COLOR0A0, c);
#endif
            }
            GXBegin(0xA8, 0, 2);
#ifdef MUST_MATCH
            // Two separate 0.0f literals: the original reloads float_0_804217e0 for each vertex
            // (its compiler did not CSE across the GX fifo stores); naming the symbol makes ours
            // fold the two reads into one.
            GXPosition3f32(star->x0, star->y0, 0.0f);
            GXPosition3f32(star->x1, star->y1, 0.0f);
#else
            GXPosition3f32(star->x0, star->y0, float_0_804217e0);
            GXPosition3f32(star->x1, star->y1, float_0_804217e0);
#endif
        }
    }
    iconDispGx(Z20, 0x10, 0x19D, 1.0f);
    if (Z18 >= 0x19 && Z18 < 0x1E) {
        f32 yoffset = 20.0f;

        for (i = 0; i < 4; i++) {
            Mtx mtx;

            PSMTXTrans(mtx, Z2C(i)->x, Z2C(i)->y - yoffset, 0.0f);
#ifdef MUST_MATCH
            iconDispGxCol(mtx, 0x10, 0x19D, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF - (i + 1) * 0x32 });
#else
            {
                GXColor color = { 0xFF, 0xFF, 0xFF, 0x00 };

                color.a = 0xFF - (i + 1) * 0x32;
                iconDispGxCol(mtx, 0x10, 0x19D, color);
            }
#endif
        }
    }
}
#undef Z18
#undef Z20
#undef Z2C

// .text:0x368 | 0x8022E8F8 | size: 0x35C
// Constant-register mapping, Mtx slots and both loops match; loading p->x/p->y into locals fixes the inner
// vertex reloads, and indexing bomb->points[j] directly (no local Vec*) gives the original's r5 inner
// counter. The inner loop computes both full texcoords before the position call (that is what gives the
// target's f0/f3 colouring and the interleaved stores).
#pragma opt_propagation off // needed: without it MWCC hoists the t numerator and colours the s/t chain f1/f0 (98.88%)
void zubastar_disp_bomb(CameraId cameraId, void* param) {
    ZubaStarWork* work = GetZubaStarPtr();
    CameraEntry* cam = camGetPtr(8);
    GXTexObj texObj;
    Mtx m1;
    Mtx m2;
    Mtx m4;
    Mtx m3;
    Mtx mtx;
    ZubaStarBomb* bomb;
    s32 j;
    s32 i;
    f32 c480;
    f32 c240;
    f32 c400;
    f32 c96;
    f32 cdeg;

    GXSetCullMode(GX_CULL_NONE);
    GXSetZCompLoc(GX_FALSE);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_NEVER, 0);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXSetNumChans(0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ONE);
    GXInitTexObj(&texObj, work->tex->address, 0x190, 0x1E0, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, 0);
    GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, float_0_804217e0, float_0_804217e0,
                    float_0_804217e0, 0, 0, 0);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    c96 = float_96_804217ec;
    c400 = float_400_804217f0;
    c240 = float_240_804217f4;
    c480 = float_480_804217f8;
    cdeg = float_deg2rad_804217e8;
    for (i = 0; i < work->bombCount; i++) {
        bomb = work->bombs[i];

        PSMTXTrans(m1, bomb->field_0x18.x, bomb->field_0x18.y, bomb->field_0x18.z);
        PSMTXTrans(m2, -bomb->field_0x18.x, -bomb->field_0x18.y, -bomb->field_0x18.z);
        PSMTXTrans(m3, bomb->field_0x0C.x, bomb->field_0x0C.y, bomb->field_0x0C.z);
        PSMTXRotRad(m4, 'z', cdeg * bomb->field_0x3C);
        PSMTXConcat(m4, m2, mtx);
        PSMTXConcat(m1, mtx, mtx);
        PSMTXConcat(m3, mtx, mtx);
        PSMTXConcat(cam->view, mtx, mtx);
        GXLoadPosMtxImm(mtx, 0);
        GXBegin(GX_TRIANGLEFAN, GX_VTXFMT0, bomb->count);
        for (j = 0; j < bomb->count; j++) {
            f32 y = bomb->points[j].y;
            f32 x = bomb->points[j].x;
            f32 s = (c96 + x) / c400;
            f32 t = (c240 - y) / c480;

            GXPosition3f32(x, y, 0.0f);
            GXTexCoord2f32(s, t);
        }
    }
}
#pragma opt_propagation reset

// .text:0x6C4 | 0x8022EC54 | size: 0x68
void zubastar_capture(CameraId cameraId, void* param) {
    ZubaStarWork* work = GetZubaStarPtr();

    GXSetTexCopySrc(0xD0, 0, 0x190, 0x1E0);
    GXSetTexCopyDst(0x190, 0x1E0, 4, 0);
    GXCopyTex(work->tex->address, 0);
    GXPixModeSync();
}

// .text:0x72C | 0x8022ECBC | size: 0xA4
s32 zubastar_end(EventEntry* event, BOOL isFirstCall) {
    ZubaStarWork* work = GetZubaStarPtr();
    s32 i;

    effDelete(*(EffectEntry**)((u8*)work->effect->userdata + 0x34));
    effDelete(work->effect);
    for (i = 0; i < 0xC8; i++) {
        if (work->bombs[i] != NULL) {
            if (work->bombs[i]->points != NULL) {
                BattleFree(work->bombs[i]->points);
                work->bombs[i]->points = NULL;
            }
            BattleFree(work->bombs[i]);
            work->bombs[i] = NULL;
        }
    }
    smartFree(work->tex);
    return 2;
}

// .text:0x7D0 | 0x8022ED60 | size: 0x30
s32 zubastar_init(EventEntry* event, BOOL isFirstCall) {
    memset(GetZubaStarPtr(), 0, 0x5E0);
    return 2;
}

// MUST_MATCH: MWCC emits the original's inline 6-iteration copy loop only when `points` is copied
// from a named 48-byte template. A `ZubaStarPoints4` local initializer (or an unused file-scope one)
// makes this compiler loop forever, and copying from the extern dat_802ff6d4 makes it form that
// symbol's address with lis/addi instead of the original's single `addi r4, r29, 0x68`. A local
// `static const Vec[4]` (not `ZubaStarPoints4`, which hangs) gives the r29-relative copy. The
// natural form is under #else.
#ifdef MUST_MATCH
static const Vec zubastar_points_init[4] = {
    { 0.0f, 240.0f, 0.0f },
    { 304.0f, 240.0f, 0.0f },
    { 304.0f, -240.0f, 0.0f },
    { 0.0f, -240.0f, 0.0f },
};
#endif

// .text:0x85C | 0x8022F128 | size: 0xDF8
// Reconstruction of the SAC mini-game's main event callback. There are three state machines, each a
// switch on work->field_0x004, work->field_0x018 and work->field_0x07C. Case 0x19 of the first
// gathers the star endpoints, sorts them, finds the widest gap on each edge and randomly places a
// split point in it before calling zubastar_bunkatu.
// The instruction stream is complete; every remaining diff is the unit's
// `.rodata` pool being short. `zubastar_points_init` sits at r29+0xc where the original's
// dat_802ff6d4 is at r29+0x6c: the 0x60 missing bytes are the unit's not-yet-ported data strings
// (str_802FF668/67C/68C from weapon_zubastar / marioAttackEvent_ZubaStar) and the five zero vec3_* 
// objects. The double/string reads are a further 0x30 off because the original pools zubastar_bomb's
// str_802FF80C and the STAR1/2/3 event strings before the i2f double while this build pools them
// after. Both are unit pool layout, not this function.
// NONMATCHING (99.95%): remaining diffs are unit .rodata pool offsets (details above).
s32 zubastar_main(EventEntry* event, BOOL isFirstCall) {
    ZubaStarWork* work = GetZubaStarPtr();
#ifdef MUST_MATCH
    ZubaStarPoints4 points = *(const ZubaStarPoints4*)zubastar_points_init;
#else
    Vec points[4] = {
        { 0.0f, 240.0f, 0.0f },
        { 304.0f, 240.0f, 0.0f },
        { 304.0f, -240.0f, 0.0f },
        { 0.0f, -240.0f, 0.0f },
    };
#endif
    s32 audienceNum = BattleAudience_GetAudienceNum();
    s32 i;
    s32 j;

    (void)event;
    if (isFirstCall) {
        work->tex = smartAlloc(GXGetTexBufferSize(0x190, 0x1E0, 4, 0, 0), 1);
#ifdef MUST_MATCH
        points.v[0] = (Vec){ -96.0f, 240.0f, 0.0f };
        points.v[1] = (Vec){ 304.0f, 240.0f, 0.0f };
        points.v[2] = (Vec){ 304.0f, -240.0f, 0.0f };
        points.v[3] = (Vec){ -96.0f, -240.0f, 0.0f };
        zubastar_create_takaku(4, points.v);
#else
        points[0] = vec3_802ff704;
        points[1] = vec3_802ff710;
        points[2] = vec3_802ff71c;
        points[3] = vec3_802ff728;
        zubastar_create_takaku(4, points);
#endif
        work->field_0x020 = (Vec){ 0.0f, -1000.0f, 0.0f };
    }

    switch (work->field_0x004) {
    case 0:
        work->field_0x004 = 5;
        work->field_0x018 = 5;
        break;
    case 5:
        work->field_0x004 = 6;
        work->field_0x008 = 0;
        if (work->field_0x07C == 0) {
            work->field_0x07C = 5;
        }
        if (work->field_0x07C == 0xF) {
            work->field_0x07C = 0x10;
        }
        // fallthrough
    case 6:
        if (work->field_0x018 == 0xA && work->field_0x07C == 0xA) {
            work->field_0x004 = 0xA;
        }
        break;
    case 0xA:
        work->field_0x004 = 0xB;
        work->field_0x00C = 0x12C;
        // fallthrough
    case 0xB:
        if (work->field_0x084 >= 100.0f) {
            EventEntry* entry;

            work->field_0x004 = 0x14;
            work->field_0x010++;
            entry = evtEntry(nice_event, 0, 0x20);
            {
                s32 n = work->field_0x010 + 1;

                entry->lwData[0] = n < 6 ? n : 6;
            }
            psndSFXOff(work->field_0x0A4);
        } else {
            work->field_0x00C--;
            if (work->field_0x00C <= 0) {
                work->field_0x004 = 0xF;
                psndSFXOff(work->field_0x0A4);
            } else if (keyGetButtonTrg(0) & 0x100) {
                work->field_0x088 = 2.8f;
            }
        }
        break;
    case 0xF:
        work->field_0x07C = 0x14;
        psndSFXOff(work->field_0x0A4);
        work->field_0x004 = 0x1E;
        break;
    case 0x14:
        work->field_0x07C = 0xF;
        work->field_0x080 = 0;
        psndSFXOff(work->field_0x0A4);
        if (audienceNum >= 1 && audienceNum <= 0x31) {
            BattleAudienceSoundCallKind(1);
        }
        if (audienceNum >= 0x32 && audienceNum <= 0x63) {
            BattleAudienceSoundCallKind(1);
            BattleAudienceSoundWhistleKind(1);
        }
        if (audienceNum >= 0x64 && audienceNum <= 0x95) {
            BattleAudienceSoundCallKind(2);
            BattleAudienceSoundWhistleKind(2);
        }
        if (audienceNum >= 0x96) {
            BattleAudienceSoundCallKind(2);
            BattleAudienceSoundWhistleKind(3);
        }
        work->field_0x004 = 0x15;
        work->field_0x018 = 0xF;
        // fallthrough
    case 0x15:
        if (work->field_0x018 == 0x14) {
            work->field_0x004 = 0x19;
            work->field_0x008 = 0;
        }
        break;
    case 0x19: {
        f32 x[18];
        f32 y[18];
        f32 dx[17];
        f32 dy[17];
        s32 n;

        work->field_0x004 = 0x1A;
        work->field_0x018 = 0x19;
        if (irand(2) == 0) {
            work->field_0x05C = (Vec){ 0.0f, 240.0f, 0.0f };
            work->field_0x068 = (Vec){ 0.0f, -240.0f, 0.0f };
        } else {
            work->field_0x05C = (Vec){ 0.0f, -240.0f, 0.0f };
            work->field_0x068 = (Vec){ 0.0f, 240.0f, 0.0f };
        }
        y[0] = -96.0f;
        x[0] = -96.0f;
        i = 0;
        n = 1;
        for (; i < 0x11; i++) {
            ZubaStarStar* star = &work->stars[i];

            if (star->state == 5) {
                if (star->y0 == 240.0f) {
                    x[n] = star->x0;
                    y[n] = star->x1;
                } else {
                    x[n] = star->x1;
                    y[n] = star->x0;
                }
                n++;
            }
        }
        x[n] = 344.0f;
        y[n] = 264.0f;
        for (i = 0; i < n; i++) {
            for (j = i + 1; j < n + 1; j++) {
                if (x[i] > x[j]) {
                    f32 t = x[i];
                    x[i] = x[j];
                    x[j] = t;
                }
                if (y[i] > y[j]) {
                    f32 t = y[i];
                    y[i] = y[j];
                    y[j] = t;
                }
            }
        }
        for (i = 0; i < n; i++) {
            dx[i] = x[i + 1] - x[i];
            dy[i] = y[i + 1] - y[i];
        }
        {
            s32 ix = 0;
            s32 iy = 0;
            f32 mx;
            f32 my;

            my = 0.0f;
            mx = my;
            for (i = 0, iy = 0, ix = 0; i < n; i++) {
                if (dx[i] > mx) {
                    mx = dx[i];
                    ix = i;
                }
                if (dy[i] > my) {
                    my = dy[i];
                    iy = i;
                }
            }
            {
                f32 v = x[ix] + dx[ix] * (f32)rand() / 32767.0f;

                if (work->field_0x05C.y == 240.0f) {
                    work->field_0x05C.x = v;
                } else {
                    work->field_0x068.x = v;
                }
            }
            {
                f32 v = x[iy] + dx[iy] * (f32)rand() / 32767.0f;

                if (work->field_0x05C.y == -240.0f) {
                    work->field_0x05C.x = v;
                } else {
                    work->field_0x068.x = v;
                }
            }
        }
        {
            Vec a;
            Vec b;

            b = work->field_0x068;
            a = work->field_0x05C;
            zubastar_bunkatu(&a, &b);
        }
        // fallthrough
    }
    case 0x1A:
        if (work->field_0x018 == 0x1E) {
            work->field_0x008++;
            if (work->field_0x008 >= 3) {
                work->field_0x004 = 0x21;
            } else {
                work->field_0x004 = 0x19;
            }
        }
        break;
    case 0x1E:
        work->field_0x004 = 0x1F;
        work->field_0x018 = 0xF;
        // fallthrough
    case 0x1F:
        if (work->field_0x018 == 0x14) {
            work->field_0x004 = 0x23;
        }
        break;
    case 0x21:
        if (work->field_0x010 >= 5) {
            work->field_0x004 = 0x23;
        } else {
            work->field_0x004 = 0;
        }
        break;
    case 0x23:
        work->field_0x004 = 0x24;
        work->field_0x018 = 0x19;
        work->field_0x05C = (Vec){ -96.0f, 240.0f, 0.0f };
        work->field_0x068 = (Vec){ -96.0f, -240.0f, 0.0f };
        break;
    case 0x24:
        if (work->field_0x018 == 0x1E) {
            work->field_0x004 = 0x26;
            work->field_0x008 = 0x3C;
            btl_camera_shake_h(0, 0x3C, 0, 5.0f, 5.0f);
        }
        break;
    case 0x26:
        work->field_0x008--;
        if (work->field_0x008 <= 0) {
            work->field_0x004 = 0x28;
        }
        break;
    case 0x28: {
        s32 j;

        for (j = 0; j < 0x10; j++) {
            work->stars[j].state = 0;
        }
        return 2;
    }
    default:
        break;
    }

    switch (work->field_0x018) {
    case 0:
        break;
    case 5:
        work->field_0x018 = 6;
        work->field_0x01C = 0;
        // fallthrough
    case 6:
        work->field_0x01C++;
        work->field_0x020.x = 0.0f;
        work->field_0x020.y = intplGetValue(4, work->field_0x01C, 0xF, 300.0f, 300.0f);
        work->field_0x020.z = 0.0f;
        if (work->field_0x01C >= 0xF) {
            work->field_0x018 = 0xA;
        }
        break;
    case 0xA:
        work->field_0x020 = (Vec){ 0.0f, 300.0f, 0.0f };
        break;
    case 0xF:
        work->field_0x018 = 0x10;
        work->field_0x01C = 0;
        // fallthrough
    case 0x10:
        work->field_0x01C++;
        work->field_0x020.x = 0.0f;
        work->field_0x020.y = intplGetValue(4, work->field_0x01C, 0xF, 300.0f, 300.0f);
        work->field_0x020.z = 0.0f;
        if (work->field_0x01C >= 0xF) {
            work->field_0x018 = 0x14;
        }
        break;
    case 0x19:
        work->field_0x018 = 0x1A;
        work->field_0x074 = 0;
        for (i = 0; i < 0x10; i++) {
            ZubaStarStar* star = &work->stars[i];

            if (star->state == 0) {
                work->field_0x078 = i;
                star->state = 5;
                star->field_0x04 = 0;
                {
                    f32 px = work->field_0x05C.x;

                    star->x1 = px;
                    star->x0 = px;
                }
                {
                    f32 py = work->field_0x05C.y;

                    star->y1 = py;
                    star->y0 = py;
                }
                star->timer = 0;
                break;
            }
        }
        psndSFXOn("SFX_BTL_SAC_ZUBA_STAR2");
        // fallthrough
    case 0x1A:
        work->field_0x074++;
        work->field_0x020.x =
            intplGetValue(0, work->field_0x074, 0xF, work->field_0x05C.x, work->field_0x068.x);
        work->field_0x020.y =
            intplGetValue(0, work->field_0x074, 0xF, work->field_0x05C.y, work->field_0x068.y);
        {
            ZubaStarStar* star = &work->stars[work->field_0x078];

            star->x1 = work->field_0x020.x;
            star->y1 = work->field_0x020.y;
        }
        if (work->field_0x074 >= 0xF) {
            work->field_0x018 = 0x1B;
            work->field_0x01C = 0;
            btl_camera_shake_h(0, 0x1E, 0, 8.0f, 0.0f);
        }
        break;
    case 0x1B:
        work->field_0x01C++;
        if (work->field_0x01C >= 5) {
            work->field_0x018 = 0x1C;
        }
        break;
    case 0x1C:
        work->field_0x018 = 0x1E;
        // fallthrough
    case 0x1E:
        work->field_0x020 = (Vec){ 0.0f, -1000.0f, 0.0f };
        break;
    default:
        break;
    }

    work->field_0x02C[3] = work->field_0x02C[2];
    work->field_0x02C[2] = work->field_0x02C[1];
    work->field_0x02C[1] = work->field_0x02C[0];
    work->field_0x02C[0] = work->field_0x020;

    switch (work->field_0x07C) {
    case 0:
        break;
    case 5:
        work->field_0x07C = 6;
        work->field_0x080 = 0;
        work->field_0x094 = (Vec){ -1000.0f, 0.0f, 0.0f };
        work->field_0x090 = 200.0f;
        work->field_0x084 = 0.0f;
        work->field_0x088 = 0.0f;
        work->field_0x08C = -1.0f;
        work->field_0x0A0 = 0xFF;
        work->field_0x0A1 = 0;
        break;
    case 6:
        work->field_0x080++;
        work->field_0x094.x = intplGetValue(4, work->field_0x080, 0x1E, -1000.0f, 50.0f);
        if (work->field_0x080 >= 0x1E) {
            work->field_0x07C = 0xA;
        }
        break;
    case 0xA:
        work->field_0x084 += work->field_0x088;
        work->field_0x084 += work->field_0x08C;
        if (work->field_0x084 > 100.0f) {
            work->field_0x084 = 100.0f;
        }
        if (work->field_0x084 < 0.0f) {
            work->field_0x084 = 0.0f;
        }
        work->field_0x088 -= 0.25f;
        if (work->field_0x088 < 0.0f) {
            work->field_0x088 = 0.0f;
        }
        work->field_0x0A1++;
        if (work->field_0x0A1 >= 8) {
            work->field_0x0A1 = 0;
        }
        if (work->field_0x084 <= 0.0f) {
            work->field_0x084 = 0.0f;
            if (psndSFXChk(work->field_0x0A4) == 0) {
                psndSFXOff(work->field_0x0A4);
            }
        }
        if (work->field_0x084 > 0.0f) {
            if (psndSFXChk(work->field_0x0A4) == -1) {
                work->field_0x0A4 = psndSFXOn("SFX_BTL_SAC_ZUBA_STAR1");
            }
        }
        if (psndSFXChk(work->field_0x0A4) == 0) {
            psndSFX_pit(work->field_0x0A4,
                        (s32)(683.0f * (f32)work->field_0x010 +
                              4096.0f * work->field_0x084 / 100.0f));
        }
        break;
    case 0xF:
        work->field_0x084 = 100.0f;
        work->field_0x088 = 0.0f;
        work->field_0x08C = 0.0f;
        work->field_0x0A1 = 0;
        work->field_0x080++;
        work->field_0x094.x = intplGetValue(4, work->field_0x080, 0x1E, 50.0f, -1000.0f);
        if (work->field_0x080 >= 0x1E) {
            work->field_0x080 = 0x1E;
        }
        break;
    case 0x10:
        work->field_0x07C = 6;
        work->field_0x080 = 0;
        work->field_0x084 = 0.0f;
        work->field_0x088 = 0.0f;
        work->field_0x08C = -0.9f - 0.1f * (f32)work->field_0x010;
        break;
    case 0x14:
        work->field_0x07C = 0x15;
        work->field_0x080 = 0;
        break;
    case 0x15:
        work->field_0x080++;
        work->field_0x094.x = intplGetValue(4, work->field_0x080, 0x1E, 50.0f, -1000.0f);
        if (work->field_0x080 >= 0x1E) {
            work->field_0x080 = 0x1E;
        }
        break;
    default:
        break;
    }

    {
        s32 k;

        for (k = 0; k < 0x10; k++) {
            ZubaStarStar* star = &work->stars[k];

            switch (star->state) {
            case 0:
                break;
            case 5:
                star->timer++;
                if (star->timer > 0x3C) {
                    star->timer = 0;
                }
                break;
            default:
                break;
            }
        }
    }
    dispEntry(8, 900.0f, 1, zubastar_disp2D, NULL);
    return 0;
}
#undef ZS_ROV
#undef ZS_RD

// .text:0x800 | 0x8022ED90 | size: 0x398
// The state machine matches. Every named float constant is a literal (0.0f, 104.0f, 600.0f, 200.0f,
// 70.0f, 32767.0f, 35.0f, 2.0f, -1.25f) and the two initial vectors are the compound literals
// (Vec){0,0,0} and (Vec){0,-1000,200} (the original's vec3_802ff7ac/b8 hold exactly those), so MWCC has
// the known values and recovers the schedule. Two structural details matter (both under MUST_MATCH below):
// a single-case `switch` instead of `if` for the early-out guard, which makes MWCC emit `beq body;
// b main_loop` like the original, and hoisting the SFX string into a loop-lived local to fix the
// callee-saved colouring. The only remaining diffs are 9 rodata reads: the pool base symbol is
// `...rodata.0` here vs `str_802FF668` in the original, so the two vectors and the i2f double read at
// offsets 0xb4/0xc0/0xe8 instead of 0x144/0x158/0x1c0. The missing bytes are the .rodata prefix of
// the not-yet-written zubastar_main (the three `str_802FF6*` strings, five zero `vec3_*`s and the
// `dat_802ff6d4` point table, plus `str_802FF7D0/E4/F8/80C`), so this is not local to this function.
// NONMATCHING (99.96%): 9 rodata offsets (unit pool layout; depends on zubastar_main).
s32 zubastar_bomb(EventEntry* event, BOOL isFirstCall) {
    ZubaStarWork* work = GetZubaStarPtr();
    s32 i;

    if (isFirstCall) {
        dispEntry(8, 0.0f, 0, zubastar_capture, NULL);
        work->field_0x004 = 0;
        return 0;
    }
    // A single-case `switch` (rather than `if`) is what makes MWCC lay the guard out as
    // `beq body; b main_loop` like the original; an `if` collapses it to `bne skip`. Semantically
    // identical, so the natural form is the #else arm.
#ifdef MUST_MATCH
    switch (work->field_0x004) {
    case 0: {
#else
    if (work->field_0x004 == 0) {
#endif
        s32 count = 0;

        for (i = 0; i < work->bombCount; i++) {
            if (work->bombs[i]->state != 0xA) {
                break;
            }
            count++;
        }
        if (count == work->bombCount) {
            return 2;
        }
#ifdef MUST_MATCH
        break;
    }
    }
#else
    }
#endif
#ifdef MUST_MATCH
    // The SFX string address must stay live across the main loop (hoisted into a callee-saved
    // register) and the main-loop counter be declared after it; otherwise MWCC colours the whole
    // callee-saved set one register higher (a/b r30/r31, bombCount r23) than the original
    // (a/b r29/r30, bombCount r31). The natural form is the #else arm.
    {
        const char* sfx = "SFX_BTL_SAC_ZUBA_STAR3";
        s32 i;
#endif
    for (i = 0; i < work->bombCount; i++) {
        ZubaStarBomb* bomb = work->bombs[i];

        switch (bomb->state) {
        case 0: {
            s32 maxTimer = irand(0x1E) + 0x1E;
            f32 sx = 0.0f;
            f32 sy = 0.0f;
            f32 sz = 0.0f;
            s32 j;

            bomb->maxTimer = maxTimer;
            bomb->field_0x24 = (Vec){ 0.0f, 0.0f, 0.0f };
            bomb->field_0x30 = (Vec){ 0.0f, -1000.0f, 200.0f };
            for (j = 0; j < bomb->count; j++) {
                sx += bomb->points[j].x;
                sy += bomb->points[j].y;
                sz += bomb->points[j].z;
            }
            bomb->field_0x18.x = sx / (f32)bomb->count;
            bomb->field_0x18.y = sy / (f32)bomb->count;
            bomb->field_0x18.z = sz / (f32)bomb->count;
            bomb->field_0x30.x +=
                (bomb->field_0x18.x - 104.0f) * 600.0f / 200.0f;
            bomb->field_0x3C = 0.0f;
            bomb->field_0x40 = 70.0f * (f32)rand() / 32767.0f - 35.0f;
            // (f32)maxTimer is cast at each use: the original converts it three times, so a temp
            // here would fold them into one and change the schedule.
            bomb->field_0x44 = (2.0f * (bomb->field_0x30.y - bomb->field_0x24.y) -
                                ((f32)maxTimer * (-1.25f * (f32)maxTimer))) /
                               (2.0f * (f32)maxTimer);
            bomb->timer = 0;
            bomb->state = 5;
#ifdef MUST_MATCH
            psndSFXOn(sfx);
#else
            psndSFXOn("SFX_BTL_SAC_ZUBA_STAR3");
#endif
            break;
        }
        case 5:
            bomb->field_0x0C.x =
                intplGetValue(0, bomb->timer, bomb->maxTimer, bomb->field_0x24.x, bomb->field_0x30.x);
            bomb->field_0x0C.z =
                intplGetValue(0, bomb->timer, bomb->maxTimer, bomb->field_0x24.z, bomb->field_0x30.z);
            bomb->field_0x0C.y += bomb->field_0x44;
            bomb->field_0x3C += bomb->field_0x40;
            bomb->field_0x44 += -1.25f;
            bomb->timer++;
            if (bomb->timer > bomb->maxTimer) {
                bomb->state = 0xA;
            }
            break;
        case 0xA:
            break;
        }
    }
#ifdef MUST_MATCH
    }
#endif
    dispEntry(8, 0.0f, 1, zubastar_disp_bomb, NULL);
    return 0;
}

// .text:0x1A90 | 0x80230020 | size: 0x4C8
// The instruction stream is complete and in order. What still differs is the callee-saved register
// colouring throughout (this compiler gives a/b r30/r31 and bombCount r23; the original has a/b
// r29/r30 and bombCount r31, so the whole colouring is permuted) and the schedule of the two Vec
// copies that initialise bomb1/bomb2. Recovered from the original: the first bomb1->points copy loop
// uses a separate source/dest index pair (two strength-reduced offsets plus a counter), the
// found-point stores use post-increment so MWCC reuses the copy-loop counter for index[0]+1/+2 and k,
// and bomb1->count is written in the original's +2-then-+j order.
// The original's callee-saved order is bombCount,b,a,i*4,n,j,used,work,i*4+0xa8,bomb,i,j*0xc; this
// compiler produces b,a,i*4,j,n,work,offset,bombCount,i,bomb,used,j*0xc. Nudging `bombCount` to the
// front of that order is the whole remaining difference and no declaration order, statement order, alias,
// scope or pragma tried moves it. Declaring the per-iteration scratch (`used`/`index`/`point`/`n`/out1/
// out2) at function scope before i/j/k is the one source shape that moves `n` into the original's r27
// (94.51 -> 94.64); everything else stays permuted. Kept from the recovered shapes: `used` base held in a
// pointer just for the zeroing loop (the original keeps it in a callee-saved register and recomputes
// r1+0x18 for the dedupe loop), the dedupe comparison inlines the (s32)out1/out2 casts (the original CSEs
// them into r6/r7), the two point stores go through a `j1` temp (the original computes j+1 and adds 2 to
// j), and the bomb1/bomb2 field order below is the one whose schedule matches. The natural forms are
// under #else.
// NONMATCHING (94.64%): register colouring (callee-saved + caller-saved argument mismatches);
// the insert/delete/replace lines are the bomb1/bomb2 setup and the points[0]/[1] schedule.
void zubastar_bunkatu(Vec* a, Vec* b) {
    ZubaStarWork* work = GetZubaStarPtr();
    s32 bombCount = work->bombCount;
    s32 used[2];
    s32 index[2];
    Vec point[2];
    s32 n;
    f32 out1;
    f32 out2;
    s32 i;
    s32 j;
    s32 k;

    for (i = 0; i < bombCount; i++) {
        ZubaStarBomb* bomb = work->bombs[i];

        n = 0;
        {
            s32* usedp = used;

            for (k = 0; k < 2; k++) {
                usedp[k] = 0;
            }
        }
        for (j = 0; j < bomb->count; j++) {
#ifdef MUST_MATCH
            // The original compares the dedupe points against the CSE'd casts, not against stores of
            // nx/ny; inlining them lets MWCC reuse r6/r7 exactly as the target does.
#else
            s32 nx;
            s32 ny;
#endif

            if (n == 2) {
                break;
            }
            if (zubastar_get_kouten(a->x, a->y, b->x, b->y, bomb->points[j].x, bomb->points[j].y,
                                    bomb->points[(j + 1) % bomb->count].x,
                                    bomb->points[(j + 1) % bomb->count].y, &out1, &out2)) {
#ifndef MUST_MATCH
                nx = (s32)out1;
                ny = (s32)out2;
#endif
                for (k = 0; k < n; k++) {
#ifdef MUST_MATCH
                    if (used[k] == 1 && (s32)point[k].x == (s32)out1 && (s32)point[k].y == (s32)out2) {
#else
                    if (used[k] == 1 && (s32)point[k].x == nx && (s32)point[k].y == ny) {
#endif
                        break;
                    }
                }
                if (k == n) {
                    point[k].x = out1;
                    point[k].y = out2;
                    point[k].z = float_0_804217e0;
                    used[k] = 1;
                    index[k] = j;
                    n++;
                }
            }
        }
        if (n == 2) {
            ZubaStarBomb* bomb1;
            ZubaStarBomb* bomb2;

            bomb1 = BattleAlloc(0x50);
            bomb2 = BattleAlloc(0x50);
            for (j = 0; j < bomb->count; j++) {
                if (j == index[0]) {
                    bomb1->count = bomb->count - index[1] + 2 + j;
                    bomb2->count = index[1] - index[0] + 2;
                }
            }
            bomb1->points = BattleAlloc(bomb1->count * 0xC);
            bomb2->points = BattleAlloc(bomb2->count * 0xC);
            {
                s32 k;

                for (j = 0, k = 0; j <= index[0]; j++, k++) {
                    bomb1->points[k] = bomb->points[j];
                }
            }
#ifdef MUST_MATCH
            // Storing through j++ twice makes MWCC emit two `addi j,1`; the target computes j+1 into a
            // second register and adds 2 to j, so the stores use j and j+1 and j advances by two.
            {
                s32 j1 = j + 1;

                bomb1->points[j] = point[0];
                bomb1->points[j1] = point[1];
                j = j1 + 1;
            }
#else
            bomb1->points[j++] = point[0];
            bomb1->points[j++] = point[1];
#endif
            {
                s32 k = j;

                for (j = index[1] + 1; j < bomb->count; j++, k++) {
                    bomb1->points[k] = bomb->points[j];
                }
            }
            for (j = 0; j < bomb->count; j++) {
                if (j == index[0]) {
                    s32 m;

                    bomb2->points[0] = point[0];
                    for (m = 1; m <= bomb2->count - 2; m++) {
                        bomb2->points[m] = bomb->points[j + m];
                    }
                    bomb2->points[bomb2->count - 1] = point[1];
                }
            }
#ifdef MUST_MATCH
            // Statement order only: it is what makes the target's schedule of the two vec3 loads, the state
            // stores and the two irand calls come out (the state store of bomb1 lands before the vec3 load).
            bomb1->timer = irand(0x80);
            bomb1->field_0x0C = vec3_802ff6bc;
            bomb1->maxTimer = irand(5) + 1;
            bomb1->state = 0;
            bomb2->field_0x0C = vec3_802ff6c8;
            bomb2->state = 0;
            bomb2->timer = irand(0x80);
            bomb2->maxTimer = irand(5) + 1;
#else
            bomb1->state = 0;
            bomb1->field_0x0C = vec3_802ff6bc;
            bomb1->timer = irand(0x80);
            bomb1->maxTimer = irand(5) + 1;
            bomb2->state = 0;
            bomb2->field_0x0C = vec3_802ff6c8;
            bomb2->timer = irand(0x80);
            bomb2->maxTimer = irand(5) + 1;
#endif
            BattleFree(work->bombs[i]->points);
            BattleFree(work->bombs[i]);
            work->bombs[i] = bomb1;
            work->bombs[work->bombCount] = bomb2;
            work->bombCount++;
        }
    }
}

// .text:0x1990 | 0x8022FF20 | size: 0x100
// Kept out of line: the original's zubastar_bunkatu calls it (bl) instead of inlining it.
// `opt_lifetimes off` + `opt_propagation off` (reset after the function) give the original's float
// colouring: the first value (`a`) in f9, the first select result (`b`) in f10, the second select
// result (`c`) in f11, and the `ct` temp in f3. The zero compares must stay `0.0f` literals, not the
// named `float_0_804217e0` object: with the named object MWCC CSEs the zero reloads away, while the
// literal (whose pooled word the file already has) forces the original's reloads. The 1e-05 fallback
// is the named `float_1Eneg05_80421860` because a literal would add a new `.sdata2` word and shift the
// section. The reused `ct`/`dt` temps and the separate `e`/`f` locals land in the original's registers.
#pragma dont_inline on
#pragma opt_lifetimes off // keep the select results and the ct temp in f9/f10/f11/f3
#pragma opt_propagation off // ditto for the rest of the float web
s32 zubastar_get_kouten(f32 f1, f32 f2, f32 f3, f32 f4, f32 f5, f32 f6, f32 f7, f32 f8, f32* out1,
                        f32* out2) {
    f32 a = f3 - f1;
    f32 b;
    f32 c;
    f32 d;
    f32 e;
    f32 f;

    if (a == 0.0f && f7 - f5 == 0.0f) {
        return 0;
    }
    b = (a != 0.0f) ? a : float_1Eneg05_80421860;
    {
        // Named temps: MWCC keeps the differences in f3 and the selected results in their own
        // registers with a copy, like the original (see the comment above the function).
        f32 ct = f7 - f5;
        f32 c2 = (ct != 0.0f) ? ct : float_1Eneg05_80421860;

        c = c2;
    }
    {
        f32 dt = f4 - f2;

        d = dt;
    }
    e = f8 - f6;
    f = d / b;
    b = e / c;
    d = f - b;
    if (d == 0.0f) {
        return 0;
    }
    *out1 = ((f6 - f2) + (f * f1 - b * f5)) / d;
    if (a != 0.0f) {
        *out2 = f * (*out1 - f1) + f2;
    } else {
        *out2 = b * (*out1 - f5) + f6;
    }
    {
        f32 sl = (*out1 - f5) / c;

        if (sl >= 0.0f && sl <= 1.0f) {
            return 1;
        }
    }
    return 0;
}
#pragma opt_lifetimes reset
#pragma opt_propagation reset
#pragma dont_inline reset


// .text:0x1F58 | 0x802304E8 | size: 0xE4
// A volatile struct copy reproduces the original's load/store interleave for vec3_802ff6b0. A volatile
// store to bomb->count is needed too: it stops the scheduler hoisting the copy's first load above the
// count store (the natural form is under #else).
// dont_inline: the original calls zubastar_create_takaku from zubastar_main with `bl`.
#pragma dont_inline on
void zubastar_create_takaku(s32 count, Vec* points) {
    ZubaStarWork* work = GetZubaStarPtr();
    ZubaStarBomb* bomb;
    s32 i;

    work->bombs[work->bombCount] = BattleAlloc(0x50);
    bomb = work->bombs[work->bombCount];
    bomb->points = BattleAlloc(count * 0xC);
    bomb->state = 0;
#ifdef MUST_MATCH
    // Volatile count store: a volatile access cannot move across the volatile struct copy below,
    // which keeps the count store before the copy's first load as the original schedules it.
    *(volatile s32*)&bomb->count = count;
    *(volatile Vec*)&bomb->field_0x0C = *(const volatile Vec*)&vec3_802ff6b0;
#else
    bomb->count = count;
    bomb->field_0x0C = vec3_802ff6b0;
#endif
    bomb->timer = 0;
    bomb->maxTimer = 1;
    for (i = 0; i < count; i++) {
        bomb->points[i] = points[i];
    }
    work->bombCount++;
}
#pragma dont_inline reset

// .text:0x203C | 0x802305CC | size: 0x3C
s32 weaponGetPower_ZubaStar(BattleUnit* unit1, BattleWeapon* weapon, BattleUnit* unit2,
                            BattleUnitPart* part) {
    ZubaStarWork* work = GetZubaStarPtr();

    return weapon->hpDamageWork[work->field_0x010];
}

// .text:0x2078 | 0x80230608 | size: 0xC
// Keep GetZubaStarPtr out of line: the original issues a call (bl) to it.
#pragma dont_inline on
ZubaStarWork* GetZubaStarPtr(void) {
    return (ZubaStarWork*)((u8*)_battleWorkPointer + 0x1F4C);
}
#pragma dont_inline reset

// .text:0x2084 | 0x80230614 | size: 0x4
// The original body is a no-op; this exists only as a dispEntry callback.
void zubaStarDispStar(CameraId cameraId, void* param) {
    (void)cameraId;
    (void)param;
}

// Star-stone effect userdata fields touched by main_star (no header declares this struct).
typedef struct ZubaStarEffectData {
    u8 field_0x00[0x8];  // 0x00
    f32 positionX;       // 0x08
    f32 positionY;       // 0x0C
    f32 positionZ;       // 0x10
    f32 scale;           // 0x14
    f32 scaleX;          // 0x18
    f32 scaleY;          // 0x1C
    f32 scaleZ;          // 0x20
} ZubaStarEffectData;

static s32 main_star(EventEntry* event, BOOL isFirstCall) {
    ZubaStarWork* work = GetZubaStarPtr();
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    f32 scale;

    switch (work->starState) {
    case 0:
        break;
    case 1:
        {
            f32 zero = 0.0f;

            work->starState = 2;
            {
                f32 neg = -1000.0f;

                work->starCounter = 0;
                {
                    f32 one = 1.0f;

                    work->effect = effStarStoneEntry(6, zero, neg, zero, one);
                }
            }
        }
        BtlUnit_GetPos(mario, &work->starCur.x, &work->starCur.y, &work->starCur.z);
        work->starCur.y += mario->mSizeMultiplier * (f32)mario->mHeight + 37.0f;
        work->starStart = work->starCur;
        work->starEnd = work->starCur;
        work->starEnd.y += 50.0f;
        work->starEnd.z += -1.0f;
        work->scale1 = (Vec){ 0.0f, 0.0f, 0.0f };
        work->scale2 = (Vec){ 0.0f, 0.0f, 0.0f };
        // fallthrough
    case 2:
        work->starCounter++;
        if (work->starCounter <= 0x64) {
            work->starCur.x = intplGetValue(0, work->starCounter, 0x64, work->starStart.x, work->starEnd.x);
            work->starCur.y = intplGetValue(0, work->starCounter, 0x64, work->starStart.y, work->starEnd.y);
            work->starCur.z = intplGetValue(0, work->starCounter, 0x64, work->starStart.z, work->starEnd.z);
            scale = intplGetValue(0, work->starCounter, 0x64, 0.0f, 2.0f);
            work->scale1.z = scale;
            work->scale1.y = scale;
            work->scale1.x = scale;
        } else {
            f32 two = 2.0f;

            work->starCur.x = work->starEnd.x;
            work->starCur.y = work->starEnd.y;
            work->starCur.z = work->starEnd.z;
            work->scale1.z = two;
            work->scale1.y = two;
            work->scale1.x = two;
        }
        work->scale2.y = intplGetValue(4, work->starCounter, 0x78, 0.0f, 2160.0f);
        if (work->starCounter >= 0x78) {
            work->starState = 3;
            work->starCounter = 0;
            work->starStart = work->starCur;
            work->starEnd.y = 300.0f;
        }
        break;
    case 3:
        work->starCounter++;
        work->starCur.y = intplGetValue(1, work->starCounter, 0x3C, work->starStart.y, work->starEnd.y);
        if (work->starCounter >= 0x3C) {
            work->starState = 4;
            work->starCounter = 0;
        }
        break;
    case 4:
        break;
    }
    if (work->effect != NULL) {
        ZubaStarEffectData* data = work->effect->userdata;
        f32 s = 1.5f;

        data->positionX = work->starCur.x;
        data->positionY = work->starCur.y;
        data->positionZ = work->starCur.z;
        data->scaleX = work->scale2.x;
        data->scaleY = work->scale2.y;
        data->scaleZ = work->scale2.z;
        data->scale = s * work->scale1.x;
    }
    dispEntry(4, 0.0f, 2, zubaStarDispStar, NULL);
    return 0;
}

// .text:0x23C4 | 0x80230954 | size: 0x38
s32 wait_star_stone_up(EventEntry* event, BOOL isFirstCall) {
    ZubaStarWork* work = GetZubaStarPtr();

    return (work->starState == 4) ? 2 : 0;
}

// .text:0x23FC | 0x8023098C | size: 0x2C
static s32 star_stone_appear(EventEntry* event, BOOL isFirstCall) {
    ZubaStarWork* work = GetZubaStarPtr();

    work->starState = 1;
    return 2;
}
