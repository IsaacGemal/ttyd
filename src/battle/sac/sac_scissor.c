#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <string.h>

#include "battle/battle.h"
#include "battle/battle_disp.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "eff/eff_starstone.h"
#include "driver/icondrv.h"
#include "driver/offscreendrv.h"
#include "manager/evtmgr_cmd.h"
#include "manager/fontmgr.h"
#include "mario/mariost.h"
#include "memory.h"
#include "pmario_sound.h"
#include "system.h"
#include "math_ext.h"
#include "battle/sac/sac_scissor.h"


extern BattleWork* _battleWorkPointer;

// Named .sdata2 constants (the original references these symbols, not anonymous literals).
extern f32 float_0_80421734;
extern f32 float_20_80421794;
extern f32 float_120_80421790;
extern f32 float_185_8042178c;
extern f32 float_320_80421788;
extern f32 float_deg2rad_80421798;
extern f32 float_255_8042174c;
extern f32 float_neg240_80421738;
extern f32 float_240_8042173c;
extern f32 float_neg304_80421740;
extern f32 float_304_80421744;
extern f32 float_0p5_80421748;
extern f32 float_1_80421754;
extern f32 float_1Eneg06_804217b4;
extern f32 float_neg1_80421750;
extern f32 float_9_80421758;
extern f32 float_100_8042175c;
extern f32 float_0p3_80421760;
extern f32 float_0p6_80421764;
extern f32 float_0p9_80421768;
extern f32 float_1000_804217b0;
extern f32 float_neg1000_8042179c;
extern f32 float_42_8042176c;
extern f32 float_50_80421770;
extern f32 float_0p86_80421774;
extern f32 float_neg300_80421778;
extern f32 float_300_8042177c;
extern f32 float_neg200_80421780;
extern f32 float_200_80421784;
extern f32 float_37_804217a0;
extern f32 float_2_804217a4;
extern f32 float_2160_804217a8;
extern f32 float_1p5_804217ac;
extern const Vec vec3_802ff59c;
extern const Vec vec3_802ff5a8;
extern const Vec vec3_802ff5b4;
extern const Vec vec3_802ff5c0;
extern const Vec vec3_802ff5cc;
extern char str_802FF648[];
extern u32 dat_80421718;
extern u32 dat_8042171c;
extern char str_80421728[4];
extern BattleWeapon weapon_scissor;

// Double constants emitted in this translation unit's .sdata2.
const f64 double_to_int_802ff638 = 4503601774854144.0;
const f64 double_5_802ff640 = 5.0;
const f64 double_to_int_mask_802ff660 = 4503599627370496.0;

// A 2D point in the scissor polygon buffers (8 bytes, copied by value in tess).
typedef struct ScissorPoint {
    f32 x;
    f32 y;
} ScissorPoint;

// Local view of the scissor action-command work area at _battleWorkPointer + 0x1F4C.
// No header declares this struct yet; fields are listed in REPORT.md.
typedef struct ScissorWork {
    u32 flags;                    // 0x00
    u8 field_0x04[0x08];          // 0x04
    s32 field_0x0C;               // 0x0C
    s32 field_0x10;               // 0x10
    s32 field_0x14;               // 0x14, vertex count
    s32 field_0x18[16];           // 0x18, unit ids
    s32 field_0x58[16];           // 0x58, accumulated power
    u8 field_0x98;                // 0x98
    u8 field_0x99[0x03];          // 0x99
    s32 field_0x9C;               // 0x9C, point count
    s32 field_0xA0;               // 0xA0
    f32 field_0xA4;               // 0xA4
    f32 field_0xA8;               // 0xA8
    f32 field_0xAC;               // 0xAC
    f32 field_0xB0;               // 0xB0
    f32 field_0xB4;               // 0xB4
    f32 field_0xB8;               // 0xB8
    f32 field_0xBC;               // 0xBC
    f32 field_0xC0;               // 0xC0
    u8 field_0xC4[0x10];          // 0xC4
    f32 field_0xD4;               // 0xD4
    f32 field_0xD8;               // 0xD8
    u8 field_0xDC[0x20];          // 0xDC
    s32 field_0xFC;               // 0xFC
    s32 field_0x100;              // 0x100
    s32 field_0x104;              // 0x104
    f32 field_0x108;              // 0x108
    f32 field_0x10C;              // 0x10C
    u8 field_0x110[0x04];         // 0x110
    void* field_0x114;            // 0x114
    void* field_0x118;            // 0x118
    s32 state;                    // 0x11C
    s32 field_0x120;              // 0x120
    f32 field_0x124;              // 0x124
    f32 field_0x128;              // 0x128
    f32 field_0x12C;              // 0x12C
    f32 field_0x130;              // 0x130
    f32 field_0x134;              // 0x134
    f32 field_0x138;              // 0x138
    f32 field_0x13C;              // 0x13C
    f32 field_0x140;              // 0x140
    f32 field_0x144;              // 0x144
    u8 field_0x148[0x0C];         // 0x148
    f32 field_0x154;              // 0x154
    f32 field_0x158;              // 0x158
    f32 field_0x15C;              // 0x15C
    f32 field_0x160;              // 0x160
    f32 field_0x164;              // 0x164
    f32 field_0x168;              // 0x168
    EffectEntry* field_0x16C;     // 0x16C
    smartEntry* field_0x170;      // 0x170
} ScissorWork;

// Star trail area reached through a work+0x9C pointer in the original.
// No header declares it; fields are listed in REPORT.md.
typedef struct StarTrail {
    u8 field_0x00[0x18];  // 0x00
    f32 velX;             // 0x18
    f32 velY;             // 0x1C
    u8 field_0x20[0x20];  // 0x20
    f32 trailX[4];        // 0x40
    f32 trailY[4];        // 0x50
} StarTrail;

// Star-stone effect userdata written at the end of star_control.
// No header declares it; fields are listed in REPORT.md.
typedef struct ScissorEffectData {
    u8 field_0x00[0x8];  // 0x00
    f32 posX;            // 0x08
    f32 posY;            // 0x0C
    f32 posZ;            // 0x10
    f32 scale;           // 0x14
    f32 rotX;            // 0x18
    f32 rotY;            // 0x1C
    f32 rotZ;            // 0x20
} ScissorEffectData;



s32 wait_star_standby(void) {
    ScissorWork* work = GetScissorPtr();

    return (work->state == 5) ? 2 : 0;
}

s32 star_stone_appear(void) {
    ScissorWork* work = GetScissorPtr();
    work->state = 1;
    return 2;
}

s32 scissor_end(void) {
    ScissorWork* work = GetScissorPtr();

    effDelete(*(EffectEntry**)((u8*)work->field_0x16C->userdata + 0x34));
    effDelete(work->field_0x16C);
    if (work->field_0x170 != NULL) {
        smartFree(work->field_0x170);
        work->field_0x170 = NULL;
    }
    BattleFree(work->field_0x114);
    BattleFree(work->field_0x118);
    return 2;
}

s32 scissor_init(void) {
    ScissorWork* work = GetScissorPtr();

    memset(work, 0, 0x174);
    work->field_0x104 = 0x3BF;
    work->field_0x114 = BattleAlloc(0x1000);
    work->field_0x118 = BattleAlloc(0x3000);
    work->field_0xB4 = 0.0f;
    work->field_0xB8 = 0.0f;
    work->field_0x0C = 0;
    work->field_0x98 = 0;
    return 2;
}

void scissor_disp_region(CameraId cameraId, void* param) {
    ScissorWork* work = GetScissorPtr();
    GXTexObj texObj;
    Mtx mtx;
    u16 left;
    u16 top;
    u16 right;
    u16 bottom;
    f32* tri;
    s32 i;

    sysWaitDrawSync();
    GXClearBoundingBox();
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXInitTexObj(&texObj, work->field_0x170->address, 0x260, 0x1E0, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, GX_FALSE);
    GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, float_0_80421734, float_0_80421734, float_0_80421734, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetNumChans(1);
    GXSetNumTexGens(1);
    C_MTXLightOrtho(mtx, float_neg240_80421738, float_240_8042173c, float_neg304_80421740, float_304_80421744, float_0p5_80421748, float_0p5_80421748, float_0p5_80421748, float_0p5_80421748);
    PSMTXConcat(mtx, camGetPtr(CAMERA_2D)->view, mtx);
    GXLoadTexMtxImm(mtx, 0x1E, GX_MTX3x4);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX3x4, GX_TG_POS, 0x1E, GX_FALSE, 0x7D);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_KONST);
    GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_1);
    GXLoadPosMtxImm(camGetPtr(CAMERA_2D)->view, GX_PNMTX0);
    tri = (f32*)work->field_0x118;
    GXBegin(GX_TRIANGLES, GX_VTXFMT0, (u16)(work->field_0x14 * 3));
    for (i = 0; i < work->field_0x14; i++) {
        GXPosition3f32(tri[0], tri[3], 0.0f);
        GXPosition3f32(tri[1], tri[4], 0.0f);
        GXPosition3f32(tri[2], tri[5], 0.0f);
        tri += 6;
    }
    sysWaitDrawSync();
    if (work->field_0x170 != NULL) {
        smartFree(work->field_0x170);
        work->field_0x170 = NULL;
    }
    GXReadBoundingBox(&left, &top, &right, &bottom);
    if (left == 0x3FF && right == 0 && top == 0x3FF && bottom == 0) {
        work->flags &= ~1;
    } else {
        work->flags |= 1;
    }
    offscreenAddBoundingBox(offscreenNameToId(str_80421728), left, top, right, bottom);
}

s32 scissor_offscreen(void) {
    ScissorWork* work = GetScissorPtr();

    dispEntry(CAMERA_OFFSCREEN2, 0.0f, 1, scissor_disp_region, NULL);
    return 2;
}

// NONMATCHING (96.38%): newly written; see REPORT.md.
void scissor_disp_control(CameraId cameraId, void* param) {
    ScissorWork* work = GetScissorPtr();
    f32* points = work->field_0x114;
    GXColor color;
    GXColor hsv;
    s32 i;

    work->field_0x10++;
    if (work->field_0x10 >= 0x3C) {
        work->field_0x10 -= 0x3C;
    }
    if (work->field_0x10 <= 0x1D) {
        intplGetValue(0, work->field_0x10, 0x1D, float_0_80421734, float_255_8042174c);
    } else {
        intplGetValue(0, work->field_0x10 - 0x1E, 0x3B, float_255_8042174c, float_0_80421734);
    }
    GXSetCullMode(GX_CULL_NONE);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    GXLoadPosMtxImm(camGetPtr(cameraId)->view, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    GXSetLineWidth(0x10, GX_TO_ZERO);
    for (i = 0; i < work->field_0x9C - 1; i++) {
        hsv = *(GXColor*)&dat_8042171c;
        hsv.r = (u8)(((gp->retraceCount + i * 2) % 0x7F) << 1);
        *(u32*)&color = HSV2RGB(&hsv);
        GXSetChanMatColor(GX_COLOR0A0, color);
        GXBegin(GX_LINES, GX_VTXFMT0, 2);
        GXPosition3f32(points[i * 2], points[i * 2 + 1], 0.0f);
        GXPosition3f32(points[i * 2 + 2], points[i * 2 + 3], 0.0f);
    }
}

void scissor_capture(void) {
    ScissorWork* work = GetScissorPtr();

    GXSetTexCopySrc(0, 0, 0x260, 0x1E0);
    GXSetTexCopyDst(0x260, 0x1E0, GX_TF_RGB565, GX_FALSE);
    GXCopyTex(work->field_0x170->address, GX_FALSE);
    GXPixModeSync();
}

s32 scissor_ofscreen_ok(EventEntry* event, BOOL isFirstCall) {
    s32* args = event->args;
    ScissorWork* work = GetScissorPtr();

    if (work->flags & 1) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return 2;
}

// Keep scissor_damage_sub2 out of line: scissor_damage_sub calls (bl) it in the original.
#pragma dont_inline on
// NONMATCHING (97.82%): same instructions; only register numbers or operands differ.
s32 scissor_damage_sub2(Vec* point) {
    ScissorWork* work = GetScissorPtr();
    f32 cross[3];
    s32 count = work->field_0x14;
    f32 px = point->x;
    f32 py = point->y;
    f32 zero = float_0_80421734;
    s32 i;
    s32 j;

    for (i = 0; i < count; i++) {
        f32* tri = (f32*)((u8*)work->field_0x118 + i * 0x18);
        for (j = 0; j < 3; j++) {
            cross[j] = (px - tri[j]) * (tri[((j + 1) % 3) + 3] - tri[j + 3]) -
                       (py - tri[j + 3]) * (tri[(j + 1) % 3] - tri[j]);
        }
        if ((cross[0] > zero && cross[1] > zero && cross[2] > zero) ||
            (cross[0] < zero && cross[1] < zero && cross[2] < zero)) {
            return 1;
        }
    }
    return 0;
}
#pragma dont_inline reset

// NONMATCHING (94.79%): newly written; see REPORT.md.
void scissor_damage_sub(BattleUnit* unit, s32* out) {
    Vec pos;
    Vec point;
    Vec screen;
    f32 sign;
    f32 width;
    f32 height;
    f32 ratio;
    f32 halfW;
    f32 halfH;
    f32 rowBase;
    f32 nine;
    s32 hits = 0;
    s32 i;
    s32 j;

    sign = (unit->mAttributeFlags & kCeiling) ? float_neg1_80421750 : float_1_80421754;
    width = unit->cutWidth * unit->mSizeMultiplier;
    height = unit->cutHeight * unit->mSizeMultiplier;
    BtlUnit_GetPos(unit, &pos.x, &pos.y, &pos.z);
    halfH = float_0p5_80421748 * -height;
    halfW = float_0p5_80421748 * width;
    nine = float_9_80421758;
    pos.x += unit->cutBaseOffset.x * unit->mSizeMultiplier;
    pos.y += sign * (unit->cutBaseOffset.y * unit->mSizeMultiplier);
    for (i = 0; i < 10; i++) {
        rowBase = ((f32)i * width) / nine;
        for (j = 0; j < 10; j++) {
            point.x = pos.x - halfW + rowBase;
            point.z = pos.z;
            point.y = sign * (halfH + ((f32)j * height) / nine) + pos.y;
            btlGetScreenPoint(&point, &point);
            screen = point;
            if (scissor_damage_sub2(&screen)) {
                hits++;
            }
        }
    }
    ratio = (f32)hits / float_100_8042175c;
    *out = 0;
    if (ratio >= float_0p3_80421760) {
        *out = 1;
    }
    if (ratio >= float_0p6_80421764) {
        *out = 2;
    }
    if (ratio >= float_0p9_80421768) {
        *out = 3;
    }
}

// NONMATCHING (98.28%): newly written; see REPORT.md.
s32 scissor_damage(EventEntry* event, s32 unused) {
    s32* args = event->args;
    BattleWork* battleWork = _battleWorkPointer;
    BattleUnit* mario = BattleGetMarioPtr(battleWork);
    ScissorWork* work = GetScissorPtr();
    s32 count = 0;
    s32 allHit = 1;
    s32 audienceNum = BattleAudience_GetAudienceNum();
    s32 damage = 0;
    s32 out;
    s32 i;
    s32 unitIndex;

    for (unitIndex = 0; unitIndex < 0x40; unitIndex++) {
        BattleUnit* unit = BattleGetUnitPtr(battleWork, unitIndex);
        if (unit == NULL) {
            continue;
        }
        if (BtlUnit_GetBelong(unit) != 1) {
            continue;
        }
        {
            BattleUnitPart* part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
            if (BtlUnit_CheckStatus(unit, 0x1B) == 1) {
                continue;
            }
            if ((part->attributes & kUnknown_0x4000) || (part->attributes & kUnk_ImmuneToDamageOrStatus) || (part->attributes & kNotTargetable)) {
                continue;
            }
            if ((s8)part->defenseAttr->defenseAttrs[0] == 4 || (s8)part->defenseAttr->defenseAttrs[0] == 2) {
                continue;
            }
            scissor_damage_sub(unit, &out);
            work->field_0x18[count] = unitIndex;
            work->field_0x58[count] = out;
            count++;
            if (out < 1) {
                allHit = 0;
            }
        }
    }
    if (count == 0) {
        allHit = 0;
    }
    for (i = 0; i < count; i++) {
        BattleUnit* unit;
        BattleUnitPart* part;

        if (work->field_0x18[i] == -1) {
            continue;
        }
        if (work->field_0x58[i] < 1) {
            continue;
        }
        unit = BattleGetUnitPtr(battleWork, work->field_0x18[i]);
        part = BtlUnit_GetPartsPtr(unit, BtlUnit_GetBodyPartsId(unit));
        BattleCheckDamage(mario, unit, part, &weapon_scissor, 0);
        damage = 1;
    }
    if (allHit == 1) {
        s32 value;

        work->field_0x0C++;
        value = work->field_0x0C + 1;
        if (value >= 7) {
            value = 6;
        }
        evtSetValue(event, args[0], value);
    } else {
        evtSetValue(event, args[0], -1);
    }
    if (damage == 1) {
        if (audienceNum >= 1 && audienceNum <= 0x31) {
            BattleAudienceSoundCallKind(1);
        }
        if (audienceNum >= 0x32 && audienceNum <= 0x63) {
            BattleAudienceSoundCallKind(1);
            BattleAudienceSoundHandBeat();
        }
        if (audienceNum >= 0x64 && audienceNum <= 0x95) {
            BattleAudienceSoundCallKind(2);
        }
        if (audienceNum >= 0x96) {
            BattleAudienceSoundCallKind(2);
            BattleAudienceSoundHandBeat();
        }
        work->field_0x98 += 0x14;
        if (work->field_0x98 >= 0x64) {
            work->field_0x98 = 0x64;
        }
        BattleAudienceSoundSetVol(4, work->field_0x98, 0x1E);
    }
    return 2;
}

// NONMATCHING (93.32%): newly written; see REPORT.md.
s32 scissor_control(s32 unused, s32 reset) {
    ScissorWork* work = GetScissorPtr();
    ScissorPoint* points = (ScissorPoint*)work->field_0x114;
    s32 i;
    s32 offset;

    if (reset != 0) {
        work->field_0x9C = 0;
        work->field_0xA8 = 0.0f;
        work->field_0xA4 = 0.0f;
        work->field_0xB0 = 0.0f;
        work->field_0xAC = 0.0f;
        work->field_0xA0 = 0;
        work->field_0xD4 = 0.0f;
        work->field_0xD8 = 0.0f;
        memset(points, 0, 0x1000);
        work->flags &= ~1;
    }
    work->field_0xA4 = work->field_0xAC;
    work->field_0xA8 = work->field_0xB0;
    work->field_0xAC = (f32)keyGetStickX(0);
    work->field_0xB0 = (f32)keyGetStickY(0);
    {
        f32 mag = (f32)sqrt(work->field_0xA4 * work->field_0xA4 + work->field_0xA8 * work->field_0xA8);
        mag = (f32)sqrt(work->field_0xAC * work->field_0xAC + work->field_0xB0 * work->field_0xB0);
        if (mag > float_42_8042176c) {
            work->field_0xD4 += work->field_0xAC / float_50_80421770;
            work->field_0xD8 += work->field_0xB0 / float_50_80421770;
        }
    }
    work->field_0xD4 *= float_0p86_80421774;
    work->field_0xD8 *= float_0p86_80421774;
    work->field_0xB4 += work->field_0xD4;
    work->field_0xB8 += work->field_0xD8;
    if (work->field_0xB4 < float_neg300_80421778) {
        work->field_0xB4 = float_neg300_80421778;
    }
    if (work->field_0xB4 > float_300_8042177c) {
        work->field_0xB4 = float_300_8042177c;
    }
    if (work->field_0xB8 < float_neg200_80421780) {
        work->field_0xB8 = float_neg200_80421780;
    }
    if (work->field_0xB8 > float_200_80421784) {
        work->field_0xB8 = float_200_80421784;
    }
    if (work->field_0x104 <= 0x3B) {
        work->state = 6;
        return 2;
    }
    if (gp->retraceCount % 6 == 0) {
        if (work->field_0x9C < 0x200) {
            if (work->field_0xBC != work->field_0xB4 || work->field_0xC0 != work->field_0xB8) {
                if (__fabs(work->field_0xB4 - work->field_0xBC) >= double_5_802ff640 || __fabs(work->field_0xB8 - work->field_0xC0) >= double_5_802ff640) {
                    psndSFXOn(str_802FF648);
                }
                points[work->field_0x9C].x = work->field_0xB4;
                points[work->field_0x9C].y = work->field_0xB8;
                work->field_0xBC = work->field_0xB4;
                work->field_0xC0 = work->field_0xB8;
                work->field_0x9C++;
            }
        }
    }
    if (work->field_0x9C > 3) {
        for (i = 0, offset = 0; i < work->field_0x9C - 3; i++, offset += 8) {
            ScissorPoint* p = &points[i];
            f32 ox;
            f32 oy;
            s32 n = work->field_0x9C;

            if (scissor_intersection(p->x, p->y, p[1].x, p[1].y,
                                     points[n - 2].x, points[n - 2].y, points[n - 1].x, points[n - 1].y,
                                     &ox, &oy) != 0) {
                s32 k;

                points[n - 1].x = ox;
                p->x = ox;
                points[n - 1].y = oy;
                p->y = oy;
                for (k = 0; k < i; k++) {
                    points[k] = *p;
                }
                if (work->field_0x170 != NULL) {
                    smartFree(work->field_0x170);
                    work->field_0x170 = NULL;
                }
                work->field_0x170 = smartAlloc(GXGetTexBufferSize(0x260, 0x1E0, 4, 0, 0), 1);
                dispEntry(8, 0.0f, 0, (DispCallback)scissor_capture, 0);
                return 2;
            }
        }
    }
    dispEntry(8, 0.0f, 0, (DispCallback)scissor_disp_control, 0);
    return 0;
}

// NONMATCHING (94.89%): same instructions, different schedule: case 0's two lfs
// are hoisted before the stfs in the original; cases 5/15 load f1/f2 before r4.
s32 scissor_timer_main(void) {
    ScissorWork* work = GetScissorPtr();

    switch (work->field_0xFC) {
        case 0:
            work->field_0x108 = float_320_80421788;
            work->field_0x10C = float_185_8042178c;
            work->field_0xFC = 5;
            work->field_0x100 = 0;
            // fallthrough
        case 5:
            work->field_0x108 = intplGetValue(4, work->field_0x100, 0xF, float_320_80421788, float_120_80421790);
            work->field_0x100++;
            if (work->field_0x100 > 0xF) {
                work->field_0xFC = 10;
            }
            break;
        case 10:
            work->field_0x104--;
            if (work->field_0x104 <= 0x3B) {
                work->field_0xFC = 0xF;
                work->field_0x100 = 0;
            }
            break;
        case 15:
            work->field_0x108 = intplGetValue(4, work->field_0x100, 0xF, float_120_80421790, float_320_80421788);
            work->field_0x100++;
            if (work->field_0x100 > 0xF) {
                work->field_0xFC = 0x14;
            }
            work->field_0x104--;
            if (work->field_0x104 < 0) {
                work->field_0x104 = 0;
            }
            break;
        case 20:
            work->field_0x104--;
            if (work->field_0x104 < 0) {
                work->field_0x104 = 0;
                return 2;
            }
            break;
    }
    return 0;
}

s32 scissor_timer_get(EventEntry* event, BOOL isFirstCall) {
    ScissorWork* work = GetScissorPtr();
    s32* args = event->args;

    if (work->field_0x104 <= 0x3B) {
        work->state = 6;
    }
    evtSetValue(event, args[0], work->field_0x104);
    return 2;
}

// Keep star_disp2D_sub out of line: star_disp2D must call (bl) it.
// NONMATCHING (98.77%): 3 instructions differ, 60 missing, 132 extra, plus register differences.
#pragma dont_inline on
void star_disp2D_sub(s32 index, s32 alpha) {
    ScissorWork* work = GetScissorPtr();
    f32* points = (f32*)((u8*)work + 0x9C);
    Mtx mTemp;
    Mtx mTrans;
    Mtx mRotX;
    Mtx mRotY;
    Mtx mRotZ;
    Mtx mScale;
    GXColor color;
    f32 x;
    f32 y;
    f32 z;

    if (work->state >= 6) {
        work->field_0x120++;
        if (work->field_0x120 > 0x1E) {
            work->field_0x120 = 0x1E;
        }
        alpha = (s32)intplGetValue(0, work->field_0x120, 0x1E, (f32)(u8)alpha, float_0_80421734);
    }
    *(u32*)&color = dat_80421718;
    color.a = alpha;
    if (index == 0) {
        x = points[6];
        y = points[7];
        z = float_0_80421734;
    } else {
        x = *(f32*)((u8*)points + index * 4 + 0x3C);
        y = *(f32*)((u8*)points + index * 4 + 0x4C);
        z = float_0_80421734;
    }
    PSMTXTrans(mTrans, x, y - float_20_80421794, z);
    PSMTXScale(mScale, work->field_0x154, work->field_0x158, work->field_0x15C);
    PSMTXRotRad(mRotX, 'x', float_deg2rad_80421798 * work->field_0x160);
    PSMTXRotRad(mRotY, 'y', float_deg2rad_80421798 * work->field_0x164);
    PSMTXRotRad(mRotZ, 'z', float_deg2rad_80421798 * work->field_0x168);
    PSMTXConcat(mRotZ, mRotY, mTemp);
    PSMTXConcat(mRotX, mTemp, mTemp);
    PSMTXConcat(mScale, mTemp, mTemp);
    PSMTXConcat(mTrans, mTemp, mTemp);
    iconDispGxCol(mTemp, 0x10, 0x19C, color);
}
#pragma dont_inline reset

void star_disp2D(void) {
    ScissorWork* work = GetScissorPtr();

    if (work->state >= 2 && work->state <= 6) {
        star_disp2D_sub(0, 0xFF);
        star_disp2D_sub(1, 0xC0);
        star_disp2D_sub(2, 0x80);
        star_disp2D_sub(3, 0x40);
        star_disp2D_sub(4, 0x20);
    }
}

// NONMATCHING (77.45%): the original addresses the five vectors from one common base; the int->float magic double is an anonymous pool copy (@863) instead
// of the folded double_to_int_802ff638, which shifts the FP registers through case 1;
// intplGetValue also loads r4 before f1/f2. See REPORT.md.
s32 star_control(void) {
    ScissorWork* work = GetScissorPtr();
    StarTrail* star = (StarTrail*)((u8*)work + 0x9C);
    BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
    f32 f;
    s32 i;

    if ((gp->retraceCount & 3) == 0) {
        for (i = 3; i > 0; i--) {
            star->trailX[i] = star->trailX[i - 1];
            star->trailY[i] = star->trailY[i - 1];
        }
        star->trailX[0] = star->velX;
        star->trailY[0] = star->velY;
    }
    switch (work->state) {
    case 1:
        {
            f32 zero = float_0_80421734;
            f32 neg = float_neg1000_8042179c;
            f32 one = float_1_80421754;

            work->state = 2;
            work->field_0x120 = 0;
            work->field_0x16C = effStarStoneEntry(3, zero, neg, zero, one);
        }
        BtlUnit_GetPos(mario, &work->field_0x124, &work->field_0x128, &work->field_0x12C);
        work->field_0x128 = work->field_0x128 +
            (mario->mSizeMultiplier * (f32)mario->mHeight + float_37_804217a0);
        *(Vec*)&work->field_0x130 = *(Vec*)&work->field_0x124;
        *(Vec*)&work->field_0x13C = *(Vec*)&work->field_0x124;
        work->field_0x140 += float_50_80421770;
        work->field_0x144 += float_neg1_80421750;
        *(Vec*)&work->field_0x154 = vec3_802ff59c;
        *(Vec*)&work->field_0x160 = vec3_802ff5a8;
        /* fallthrough */
    case 2:
        work->field_0x120++;
        if (work->field_0x120 <= 0x64) {
            work->field_0x124 = intplGetValue(0, work->field_0x120, 0x64, work->field_0x130, work->field_0x13C);
            work->field_0x128 = intplGetValue(0, work->field_0x120, 0x64, work->field_0x134, work->field_0x140);
            work->field_0x12C = intplGetValue(0, work->field_0x120, 0x64, work->field_0x138, work->field_0x144);
            f = intplGetValue(0, work->field_0x120, 0x64, float_0_80421734, float_2_804217a4);
            work->field_0x15C = f;
            work->field_0x158 = f;
            work->field_0x154 = f;
        } else {
            f32 two = float_2_804217a4;

            work->field_0x124 = work->field_0x13C;
            work->field_0x128 = work->field_0x140;
            work->field_0x12C = work->field_0x144;
            work->field_0x15C = two;
            work->field_0x158 = two;
            work->field_0x154 = two;
        }
        work->field_0x164 = intplGetValue(4, work->field_0x120, 0x78, float_0_80421734, float_2160_804217a8);
        if (work->field_0x120 >= 0x78) {
            work->state = 3;
            work->field_0x120 = 0;
            *(Vec*)&work->field_0x130 = *(Vec*)&work->field_0x124;
            work->field_0x140 = float_300_8042177c;
        }
        break;
    case 3:
        work->field_0x120++;
        work->field_0x128 = intplGetValue(1, work->field_0x120, 0x3C, work->field_0x134, work->field_0x140);
        if (work->field_0x120 >= 0x3C) {
            work->state = 4;
            work->field_0x120 = 0;
            {
                f32 neg = float_neg1000_8042179c;
                f32 zero = float_0_80421734;

                *(Vec*)&work->field_0x124 = vec3_802ff5b4;
                *(Vec*)&work->field_0x154 = vec3_802ff5c0;
                *(Vec*)&work->field_0x160 = vec3_802ff5cc;
                work->field_0x134 = work->field_0x128;
                work->field_0x140 = zero;
                star->velX = work->field_0x124;
                star->velY = work->field_0x128;
                for (i = 0; i < 4; i++) {
                    star->trailX[i] = zero;
                    star->trailY[i] = neg;
                }
            }
        }
        break;
    case 4:
        work->field_0x120++;
        work->field_0x128 = intplGetValue(4, work->field_0x120, 0x3C, work->field_0x134, work->field_0x140);
        star->velX = work->field_0x124;
        star->velY = work->field_0x128;
        if (work->field_0x120 >= 0x3C) {
            work->state = 5;
            work->field_0x120 = 0;
        }
        break;
    case 5:
    case 6:
        break;
    }
    if (work->field_0x16C != NULL) {
        ScissorEffectData* data = work->field_0x16C->userdata;

        if (work->state >= 4) {
            f32 zero = float_0_80421734;

            data->posX = zero;
            data->posY = float_neg1000_8042179c;
            data->posZ = zero;
        } else {
            data->posX = work->field_0x124;
            data->posY = work->field_0x128;
            data->posZ = work->field_0x12C;
        }
        {
            f32 s = float_1p5_804217ac;

            data->rotX = work->field_0x160;
            data->rotY = work->field_0x164;
            data->rotZ = work->field_0x168;
            data->scale = s * work->field_0x154;
        }
    }
    if (work->state >= 4 && work->state <= 6) {
        dispEntry(8, float_0_80421734, 2, (DispCallback)star_disp2D, NULL);
    }
    return 0;
}

s32 weaponGetPower_Scissor(BattleUnit* unit1, BattleWeapon* weapon, BattleUnit* unit2, BattleUnitPart* part) {
    ScissorWork* work = GetScissorPtr();
    s32 i;

    if (unit2 == NULL) {
        return 0;
    }
    if (unit2->mAlliance == 1) {
        for (i = 0; i < 0x10; i++) {
            if (work->field_0x18[i] == BtlUnit_GetUnitId(unit2)) {
                return work->field_0x58[i];
            }
        }
    }
    return 0;
}

s32 tess(f32* points, s32 count) {
    ScissorWork* work = GetScissorPtr();
    s32 i;
    s32 j;
    s32 k;

    if (count == 3) {
        f32* dst = (f32*)((u8*)work->field_0x118 + work->field_0x14 * 0x18);
        for (i = 0; i < 3; i++) {
            dst[i] = points[i * 2];
            dst[i + 3] = points[i * 2 + 1];
        }
        work->field_0x14++;
        BattleFree(points);
    } else {
        ScissorPoint* p = (ScissorPoint*)points;
        for (i = 0; i < count; i++) {
            for (j = 2; j < count - 1; j++) {
                s32 rem = (i + j) % count;
                if (scissor_cross(&p[i].x, &p[rem].x, points, count) == 1) {
                    s32 n1 = j + 1;
                    ScissorPoint* v1 = BattleAlloc(n1 * 8);
                    for (k = 0; k < n1; k++) {
                        s32 r0 = (i + k) % count;
                        v1[k] = p[r0];
                    }
                    {
                        s32 n2 = count - j + 1;
                        ScissorPoint* v2 = BattleAlloc(n2 * 8);
                        v2[0] = p[i];
                        for (k = 0; k < n2 - 1; k++) {
                            s32 r0 = (i + j + k) % count;
                            v2[k + 1] = p[r0];
                        }
                        BattleFree(points);
                        tess((f32*)v1, n1);
                        tess((f32*)v2, n2);
                    }
                    return;
                }
            }
        }
        {
            f32* dst = (f32*)((u8*)work->field_0x118 + work->field_0x14 * 0x18);
            for (i = 0; i < 3; i++) {
                dst[i] = points[i * 2];
                dst[i + 3] = points[i * 2 + 1];
            }
            work->field_0x14++;
            BattleFree(points);
        }
    }
}

s32 scissor_tess(void) {
    ScissorWork* work = GetScissorPtr();
    f32* points = BattleAlloc(work->field_0x9C << 3);

    memcpy(points, work->field_0x114, work->field_0x9C << 3);
    work->field_0x14 = 0;
    tess(points, work->field_0x9C);
    DCFlushRange(work->field_0x118, 0x3000);
    return 2;
}

// Matches the original byte-for-byte.
s32 scissor_cross(f32* a, f32* b, f32* c, s32 n) {
    f32 xs[0x200];
    f32 ys[0x200];
    f32 ox;
    f32 oy;
    f32 tx;
    f32 ty;
    f32 one;
    f32* p;
    f32* q;
    s32 count;
    s32 i;
    s32 j;
    s32 m;

    for (i = 0; i < n; i++) {
        s32 k = (i + 1) % n;
        if (scissor_intersection2(a[0], a[1], b[0], b[1],
                                  c[i * 2], c[i * 2 + 1],
                                  c[k * 2], c[k * 2 + 1],
                                  &ox, &oy) != 0) {
            return 0;
        }
    }
    xs[0] = float_neg1000_8042179c;
    count = 1;
    ys[0] = float_neg1000_8042179c;
    one = float_1_80421754;
    for (i = 0; i < n; i++) {
        s32 k = (i + 1) % n;
        p = &c[i * 2];
        q = &c[k * 2];
        if (scissor_intersection4(one + a[0], a[1],
                                  one + b[0], b[1],
                                  p[0], p[1],
                                  q[0], q[1],
                                  &ox, &oy) != 0) {
            s32 flag = 0;
            if (p[0] == ox && p[1] == oy) {
                f32* prev = (i == 0) ? &c[(n - 1) * 2] : &c[(i - 1) * 2];
                if (scissor_intersection3(a[0], a[1], b[0], b[1],
                                          prev[0], prev[1],
                                          q[0], q[1],
                                          &tx, &ty) != 0) {
                    flag = 1;
                }
            }
            if (flag == 1) {
                for (m = 0; m < n; m++) {
                    if (xs[m] == ox && ys[m] == oy) {
                        break;
                    }
                }
                if (m == n) {
                    xs[count] = ox;
                    ys[count] = oy;
                    count++;
                }
            } else {
                xs[count] = ox;
                ys[count] = oy;
                count++;
            }
        }
    }
    xs[count] = float_1000_804217b0;
    ys[count] = float_1000_804217b0;
    if (a[0] != b[0]) {
        for (i = 0; i < count; i++) {
            for (j = i + 1; j < count + 1; j++) {
                if (xs[i] > xs[j]) {
                    f32 t;
                    t = xs[i];
                    xs[i] = xs[j];
                    xs[j] = t;
                    t = ys[i];
                    ys[i] = ys[j];
                    ys[j] = t;
                }
            }
        }
        {
            f32 mid = (a[0] + b[0]) * float_0p5_80421748;
            for (i = 0; i < count; i++) {
                if (mid >= xs[i] && mid < xs[i + 1]) {
                    return i & 1;
                }
            }
        }
    } else {
        for (i = 0; i < count; i++) {
            for (j = i + 1; j < count + 1; j++) {
                if (ys[i] > ys[j]) {
                    f32 t;
                    t = xs[i];
                    xs[i] = xs[j];
                    xs[j] = t;
                    t = ys[i];
                    ys[i] = ys[j];
                    ys[j] = t;
                }
            }
        }
        {
            f32 mid = (a[1] + b[1]) * float_0p5_80421748;
            for (i = 0; i < count; i++) {
                if (mid >= ys[i] && mid < ys[i + 1]) {
                    return i & 1;
                }
            }
        }
    }
    return 0;
}

// Keep scissor_intersection3 out of line: the original issues a call (bl) to it
// from scissor_intersection4 rather than inlining the body.
#pragma dont_inline on
s32 scissor_intersection4(f32 ax, f32 ay, f32 bx, f32 by, f32 cx, f32 cy, f32 dx, f32 dy, f32* ox, f32* oy) {
    if (ax == cx && ay == cy) {
        *ox = ax;
        *oy = ay;
        return 1;
    }
    if (ax == dx && ay == dy) {
        *ox = ax;
        *oy = ay;
        return 1;
    }
    if (bx == cx && by == cy) {
        *ox = bx;
        *oy = by;
        return 1;
    }
    if (bx == dx && by == dy) {
        *ox = bx;
        *oy = by;
        return 1;
    }
    return scissor_intersection3(ax, ay, bx, by, cx, cy, dx, dy, ox, oy);
}

// NONMATCHING (94.87%): same instructions; the original schedules `lfs f0,
// float_0_80421734` before the prologue's `stw r0, 0x14(r1)`, ours after it.
s32 scissor_intersection3(f32 ax, f32 ay, f32 bx, f32 by, f32 cx, f32 cy, f32 dx, f32 dy, f32* ox, f32* oy) {
    f32 diff = bx - ax;

    if (float_0_80421734 != diff) {
        f32 slope = (by - ay) / diff;
        if (ax >= bx) {
            ax += float_1000_804217b0;
            bx -= float_1000_804217b0;
            ay += float_1000_804217b0 * slope;
            by -= float_1000_804217b0 * slope;
        } else {
            ax -= float_1000_804217b0;
            bx += float_1000_804217b0;
            ay -= float_1000_804217b0 * slope;
            by += float_1000_804217b0 * slope;
        }
    } else {
        if (ay >= by) {
            ay = float_1000_804217b0;
            by = float_neg1000_8042179c;
        } else {
            ay = float_neg1000_8042179c;
            by = float_1000_804217b0;
        }
    }
    return scissor_intersection(ax, ay, bx, by, cx, cy, dx, dy, ox, oy);
}
#pragma dont_inline reset

// NONMATCHING (99.85%): the two independent `fabs` at each axis test are scheduled
// |dx| before |dy|, while the original computes |dy| first. The 0.0f/1.0f literals
// also pool as anonymous @NNN constants rather than the named float_0/float_1 symbols.
s32 scissor_intersection(f32 ax, f32 ay, f32 bx, f32 by, f32 cx, f32 cy, f32 dx, f32 dy, f32* ox, f32* oy) {
    f32 d1x, d1y, d2x, d2y;
    f32 minX1, minY1, maxX1, maxY1;
    f32 minX2, minY2, maxX2, maxY2;
    s32 type1;
    s32 type2;

    if (ax <= bx) {
        minX1 = ax;
        maxX1 = bx;
    } else {
        minX1 = bx;
        maxX1 = ax;
    }
    if (ay <= by) {
        minY1 = ay;
        maxY1 = by;
    } else {
        minY1 = by;
        maxY1 = ay;
    }
    if (cx <= dx) {
        minX2 = cx;
        maxX2 = dx;
    } else {
        minX2 = dx;
        maxX2 = cx;
    }
    if (cy <= dy) {
        minY2 = cy;
        maxY2 = dy;
    } else {
        minY2 = dy;
        maxY2 = cy;
    }

    d1x = bx - ax;
    type1 = 0;
    if (d1x == 0.0f) type1 = 1;
    d1y = by - ay;
    if (d1y == 0.0f) type1 = 2;
    d2x = dx - cx;
    type2 = 0;
    if (d2x == 0.0f) type2 = 1;
    d2y = dy - cy;
    if (d2y == 0.0f) type2 = 2;

    if (type1 == 0 && type2 == 0) {
        f32 m1, m2, den;

        if (__fabs(d1x) > __fabs(d1y)) {
            m1 = d1y / d1x;
            if (__fabs(d2x) > __fabs(d2y)) {
                m2 = d2y / d2x;
                if (m1 - m2 != 0.0f) {
                    den = m1 - m2;
                } else {
                    den = float_1Eneg06_804217b4;
                }
                *ox = ((cy - ay) + (m1 * ax - m2 * cx)) / den;
                *oy = m1 * (*ox - ax) + ay;
            } else {
                m2 = d2x / d2y;
                if (m1 * m2 != 1.0f) {
                    den = 1.0f - m1 * m2;
                } else {
                    den = float_1Eneg06_804217b4;
                }
                *ox = (m2 * (-m1 * ax + ay - cy) + cx) / den;
                *oy = m1 * (*ox - ax) + ay;
            }
        } else {
            m1 = d1x / d1y;
            if (__fabs(d2x) > __fabs(d2y)) {
                m2 = d2y / d2x;
                if (m1 * m2 != 1.0f) {
                    den = 1.0f - m1 * m2;
                } else {
                    den = float_1Eneg06_804217b4;
                }
                *oy = (m2 * (-m1 * ay + ax - cx) + cy) / den;
                *ox = m1 * (*oy - ay) + ax;
            } else {
                m2 = d2x / d2y;
                if (m1 - m2 != 0.0f) {
                    den = m1 - m2;
                } else {
                    den = float_1Eneg06_804217b4;
                }
                *oy = ((cx - ax) + (m1 * ay - m2 * cy)) / den;
                *ox = m1 * (*oy - ay) + ax;
            }
        }
        if (*ox >= minX1 && *ox <= maxX1 && *oy >= minY1 && *oy <= maxY1 &&
            *ox >= minX2 && *ox <= maxX2 && *oy >= minY2 && *oy <= maxY2) {
            return 1;
        }
        return 0;
    } else if (type1 == 0 && type2 == 1) {
        f32 m1 = d1y / d1x;

        *ox = cx;
        *oy = m1 * (*ox - ax) + ay;
        if (*ox >= minX1 && *ox <= maxX1 && *oy >= minY2 && *oy <= maxY2) return 1;
        return 0;
    } else if (type1 == 1 && type2 == 0) {
        f32 m2 = d2y / d2x;

        *ox = ax;
        *oy = m2 * (*ox - cx) + cy;
        if (*ox >= minX2 && *ox <= maxX2 && *oy >= minY1 && *oy <= maxY1) return 1;
        return 0;
    } else if (type1 == 0 && type2 == 2) {
        f32 m1 = d1y / d1x;

        *oy = cy;
        *ox = (m1 * ax + (*oy - ay)) / m1;
        if (*oy >= minY1 && *oy <= maxY1 && *ox >= minX2 && *ox <= maxX2) return 1;
        return 0;
    } else if (type1 == 2 && type2 == 0) {
        f32 m2 = d2y / d2x;

        *oy = ay;
        *ox = (m2 * cx + (*oy - cy)) / m2;
        if (*oy >= minY2 && *oy <= maxY2 && *ox >= minX1 && *ox <= maxX1) return 1;
        return 0;
    } else if (type1 == 1 && type2 == 1) {
        if (ax == cx) {
            *ox = cx;
            *oy = cy;
            if ((ay >= minY2 && ay <= maxY2) || (by >= minY2 && by <= maxY2)) return 1;
        }
        return 0;
    } else if (type1 == 1 && type2 == 2) {
        *ox = ax;
        *oy = cy;
        if (ax >= minX2 && ax <= maxX2 && cy >= minY1 && cy <= maxY1) return 1;
        return 0;
    } else if (type1 == 2 && type2 == 1) {
        *ox = cx;
        *oy = ay;
        if (cx >= minX1 && cx <= maxX1 && ay >= minY2 && ay <= maxY2) return 1;
        return 0;
    } else if (type1 == 2 && type2 == 2) {
        if (ay == cy) {
            *ox = cx;
            *oy = cy;
            if ((ax >= minX2 && ax <= maxX2) || (bx >= minX2 && bx <= maxX2)) return 1;
        }
        return 0;
    }
    return 0;
}

// Keep scissor_intersection2 out of line: scissor_cross calls (bl) it in the
// original rather than inlining its body.
#pragma dont_inline on
s32 scissor_intersection2(f32 ax, f32 ay, f32 bx, f32 by, f32 cx, f32 cy, f32 dx, f32 dy, f32* ox, f32* oy) {
    if (ax == cx && ay == cy) {
        return 0;
    }
    if (ax == dx && ay == dy) {
        return 0;
    }
    if (bx == cx && by == cy) {
        return 0;
    }
    if (bx == dx && by == dy) {
        return 0;
    }
    return scissor_intersection(ax, ay, bx, by, cx, cy, dx, dy, ox, oy);
}
#pragma dont_inline reset

// Keep GetScissorPtr out of line: the original issues a call (bl) to it.
#pragma dont_inline on
ScissorWork* GetScissorPtr(void) {
    return (ScissorWork*)((u8*)_battleWorkPointer + 0x1F4C);
}
#pragma dont_inline reset
