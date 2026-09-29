#include "driver/shadowdrv.h"
#include "driver/dispdrv.h"
#include "driver/hitdrv.h"
#include "driver/mobjdrv.h"
#include "driver/offscreendrv.h"
#include "eff/eff_n64.h"
#include "system.h"
#include <string.h>

extern const Vec vec3_802c1a90;
extern const Vec vec3_802c1acc;
extern const Vec vec3_802c1ae4;
extern Vec testKagePosArray[];
extern GXColor unk_80422f98;
extern GXColor unk_80422f9c;
extern GXColor unk_80422fa0;
extern GXColor unk_80422fa4;
extern u8 cube_dl_top[0x40];
extern u8 cube_dl_bottom[0x20];
extern u8 cube_dl_side[0x20];
extern const f32 float_0_804198f4;
extern const f32 float_0p5_804198f8;
extern const f32 float_1_80419904;
extern const f32 float_150_80419920;
extern const f32 float_1p5_80419924;
extern const f32 float_1p25_8041991c;
extern const f32 float_2p5_80419918;
extern const f32 float_256_804198fc;
extern const f32 float_16_80419900;
extern const f32 float_neg1_80419908;

typedef struct ShadowConfigFields {
    u8 pad00[0x28];
    f32 unk28;
    f32 unk2C;
    f32 unk30;
    f32 unk34;
    s32 unk38;
    u32 unk3C;
    s32 unk40;
    f32 unk44;
} ShadowConfigFields;

Mtx tmp_view;
u32 tmp_sci[4];
f32 tmp_vp[6];

extern u8 shadowConfig[];
extern u8 lbl_80303460[];

//.bss
CharShadowWork char_shadow_work;
ProjShadowWork proj_shadow_work;
DepthShadowWork depth_shadow_work;

//.sbss
GXTexObj rampTex8;
GXTexObj rampTex16;

//.sdata
static GXColor color_tbl[8] = {
    {0, 0, 0, 0x6E}, {0, 0, 0, 0}, {0, 0, 0, 0x16}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
};
GXColor color_tbl_nodecay[8] = {
    {0, 0, 0, 0x6E}, {0, 0, 0, 0x6E}, {0, 0, 0, 0x6E}, {0, 0, 0, 0},
    {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0},
};
GXColor color_tbl_cube[3] = {
    {0, 0, 0, 0x8E}, {0, 0, 0, 0}, {0, 0, 0, 0x16},
};
u16 ShadowSizeTbl[4] = {0x190, 0x100, 0x80, 0x40};
DepthShadowWork* dswp = &depth_shadow_work;
ProjShadowWork* pswp = &proj_shadow_work;
CharShadowWork* cswp = &char_shadow_work;

// NONMATCHING (99.47%): body matches. `bufferSize` lands in r31 as the original, but the first assignment has
// an extra `mr r0,r3; mr r31,r0` and the second `bufferSize`/`ramp16` pair are swapped (r31/r30).
void shadowInit(void) { //almost 1:1, minor regalloc
    u32 bufferSize;
    u8* ramp8;
    u16* ramp16;
    u32 i;
    CharShadowWork* wp;

    cswp->unk0 = 0;
    cswp->unk4 = 0;
    cswp->unk8 = 1;
    cswp->unk100 = (GXColor){0, 0, 0, 0x6E};
    cswp->unk10C = 1;
    cswp->unk110 = 2;
    cswp->unk114 = 20.0f;
    cswp->unk118 = 1;
    cswp->unk104 = 0;
    cswp->entries = __memAlloc(HEAP_DEFAULT, sizeof(ShadowEntry) * 64);
    memset(cswp->entries, 0, sizeof(ShadowEntry) * 64);
    wp = cswp;
    wp->unkFC = smartAlloc(GXGetTexBufferSize(ShadowSizeTbl[0], ShadowSizeTbl[0], 2, 0, 0), 0);
    dswp->unk0 = 2;
    dswp->unk4 = 0;
    dswp->unk8 = 0;
    dswp->unk100 = (GXColor){0, 0, 0, 0x40};

    bufferSize = GXGetTexBufferSize(0x10, 0x10, GX_TF_I8, GX_FALSE, 0);
    ramp8 = __memAlloc(HEAP_DEFAULT, GXGetTexBufferSize(0x10, 0x10, GX_TF_I8, GX_FALSE, 0));
    for (i = 0; i < 256; i++) {
        ramp8[(((i & 0x3) * 8) + (((i & 0xC) * 16) + (((i & 0x80) / 4) + ((i & 0x70) / 16))))] = i;
    }
    GXInitTexObj(&rampTex8, ramp8, 0x10, 0x10, GX_TF_I8, GX_CLAMP, GX_REPEAT, GX_FALSE);
    GXInitTexObjLOD(&rampTex8, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    DCFlushRange(ramp8, bufferSize);

    bufferSize = GXGetTexBufferSize(0x100, 0x100, GX_TF_IA8, GX_FALSE, 0);
    ramp16 = __memAlloc(HEAP_DEFAULT, GXGetTexBufferSize(0x100, 0x100, GX_TF_IA8, GX_FALSE, 0));
    for (i = 0; i < 0x10000; i++) {
        ramp16[(((i & 0x3) * 4) + (((i & 0xFC) * 256) + (((i & 0xFC00) / 64) + ((i & 0x300) / 256))))] = i;
    }
    GXInitTexObj(&rampTex16, ramp16, 0x100, 0x100, GX_TF_IA8, GX_CLAMP, GX_REPEAT, GX_FALSE);
    GXInitTexObjLOD(&rampTex16, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    DCFlushRange(ramp16, bufferSize);

    pswp->unk0 = 2;
    pswp->unk4 = 0;
    pswp->unk8 = 0;
    pswp->unk100 = (GXColor){0, 0, 0, 0x40};
}

GXTexObj* shadowGetRamp8TexObj(void) {
    return &rampTex8;
}

GXTexObj* shadowGetRamp16TexObj(void) {
    return &rampTex16;
}

void* shadowGetCharShadowProjTexMtx(void) {
    return &cswp->field_0xC[0x70];
}

GXColor shadowGetCharShadowColor(void) {
    return cswp->unk100;
}

s32 shadowGetCharShadowProjection(void) {
    return cswp->unk10C;
}

void* shadowGetDepthShadowDepthMtx(void) {
    return &dswp->unkC[0xA0];
}

void* shadowGetDepthShadowProjTexMtx(void) {
    return &dswp->unkC[0x70];
}

void* shadowGetProjShadowProjTexMtx(void) {
    return &pswp->unk7C;
}

GXColor shadowGetProjShadowColor(void) {
    return pswp->unk100;
}

void shadowDepthShadowDrawCountUp(void) {
    dswp->unk4++;
}

void shadowProjShadowDrawCountUp(void) {
    pswp->unk4++;
}

// The setters below address entries by byte offset (id * sizeof(ShadowEntry) + field offset): the original
// adds the field offset to the index before the store (stbx), which entries[id].field doesn't reproduce.
void shadowSetType(s32 id, s32 type) {
    ((u8*)cswp->entries)[id * 0x5C + 3] = type;
}

void U_shadowSetMode(s32 id, s32 mode) {
    ((u8*)cswp->entries)[id * 0x5C + 0x28] = mode;
}

void shadowSetSize(s32 id, f32 size) {
    *(f32*)((u8*)cswp->entries + id * 0x5C + 0x10) = size;
}

void shadowSetCamId(s32 id, s32 camId) {
    *(u16*)((u8*)cswp->entries + id * 0x5C + 0x1A) = camId;
}

void shadowFlagOn(s32 id, u16 flag) {
    ShadowEntry* entry = cswp->entries;
    entry[id].flags |= flag;
}

void* shadowGetDepthShadowConfig(void) {
    return (u8*)shadowConfig + dswp->unk8 * 0x48;
}

void* shadowGetProjShadowConfig(void) {
    return (u8*)shadowConfig + pswp->unk8 * 0x48;
}

void shadowSetVolumeDecay(s32 id, f32 x, f32 y, f32 z) {
    ShadowEntry* entry = cswp->entries;
    entry[id].decayX = x;
    entry[id].decayY = y;
    entry[id].decayZ = z;
}

void shadowSetCollision(s32 id, BOOL value) {
    s32 offset = id * 0x5C;
    u8* base = (u8*)cswp->entries;
    if (value) {
        *(u16*)(base + offset) |= 1;
    }
    else {
        *(u16*)(base + offset) &= ~1;
    }
}

GXTexObj* shadowGetCharShadowTexObj(void) {
    if (cswp->unk0 & 1) {
        return smartTexObj((GXTexObj*)&cswp->field_0xC[0xD0], (void**)cswp->unkFC);
    }
    return NULL;
}

GXTexObj* shadowGetDepthShadowTexObj(void) {
    if (dswp->unk0 & 1) {
        return smartTexObj((GXTexObj*)&dswp->unkC[0xD0], *(void**)((u8*)dswp + 0xFC));
    }
    return NULL;
}

GXTexObj* shadowGetProjShadowTexObj(void) {
    if (pswp->unk0 & 1) {
        return smartTexObj(&pswp->texObj, &pswp->unkFC->address);
    }
    return NULL;
}

BOOL _filter(HitCheckQuery* query, HitEntry* hit) {
    if (hitGetAttr(hit) & 0x80000000) {
        MapObjectEntry* obj = mobjHitObjPtrToPtr(hit);
        if (obj->flags & 0x800) {
            return FALSE;
        }
    }
    return TRUE;
}

GXColor shadowGetDepthShadowColor(void) {
    GXColor color;
    color.r = (0xFF - dswp->unk100.r) * dswp->unk100.a / 0xFF;
    color.g = (0xFF - dswp->unk100.g) * dswp->unk100.a / 0xFF;
    color.b = (0xFF - dswp->unk100.b) * dswp->unk100.a / 0xFF;
    color.a = 0;
    return color;
}

s32 shadowEntryMode(s32 unk2, f32 x, f32 y, f32 z, f32 size) {
    ShadowEntry* entries = cswp->entries;
    s32 id = cswp->unk104;
    ShadowEntry* entry = &entries[id];
    Vec pos;

    entry->flags = 0;
    if (cswp->unk118 != 0) {
        entry->flags |= 1;
    }
    pos = vec3_802c1ae4;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    entry->position = pos;
    entry->size = size;
    entry->unk14 = cswp->unk114;
    entry->unk2 = unk2;
    entry->type = 0;
    entry->unk18 = -1;
    entry->camId = 4;
    entry->decayX = 1.0f;
    entry->decayY = 0.2f;
    entry->decayZ = 0.4f;
    entry->mode = 0xFF;
    cswp->unk104++;
    return id;
}

s32 shadowEntry(f32 x, f32 y, f32 z, f32 size) {
    ShadowEntry* entries = cswp->entries;
    s32 id = cswp->unk104;
    ShadowEntry* entry = &entries[id];
    Vec pos;

    entry->flags = 0;
    if (cswp->unk118 != 0) {
        entry->flags |= 1;
    }
    pos = vec3_802c1acc;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    entry->position = pos;
    entry->size = size;
    entry->unk14 = cswp->unk114;
    entry->unk2 = cswp->unk110;
    entry->type = 0;
    entry->unk18 = -1;
    entry->camId = 4;
    entry->decayX = 1.0f;
    entry->decayY = 0.2f;
    entry->decayZ = 0.4f;
    entry->mode = 0xFF;
    cswp->unk104++;
    return id;
}

// NONMATCHING (99.31%): body is identical; only FP scratch register allocation differs.
void cylinder(ShadowEntry* entry) {
    u8* base = lbl_80303460;

    if (entry->flags & 4) {
        GXSetArray(0xB, color_tbl_nodecay, 4);
        GXCallDisplayList(base + 0x140, 0x40);
        GXCallDisplayList(base + 0x180, 0x40);
        GXCallDisplayList(base + 0x1C0, 0x40);
    } else {
        f32 alpha = (f32)cswp->unk100.a;
        f32 f2 = alpha * entry->decayX / 255.0f;
        f32 f31 = 1.0f - entry->decayZ;
        f32 f1 = f2 * entry->decayY;

        if (f31 < 0.0f) {
            f31 = 0.0f;
        }
        else if (f31 > 1.0f) {
            f31 = 1.0f;
        }
        if (f2 <= 0.0f) {
            return;
        }
        if (f1 <= 0.0f) {
            f1 = 0.0f;
        }
        if (f1 >= 1.0f) {
            f1 = 1.0f;
        }
        {
            s32 c = (s32)(255.0f * f1);
            GXSetArray(0xB, color_tbl, 4);
            if (f31 != 0.0f) {
                GXCallDisplayList(base + 0x200, 0x40);
            }
            if (f31 != 1.0f && (c & 0xFF) != 0) {
                GXCallDisplayList(base + 0x240, 0x40);
            }
        }
        GXCallDisplayList(base + 0x180, 0x40);
        GXCallDisplayList(base + 0x1C0, 0x40);
    }
}

// The three material passes each draw the cube display lists for type 1 and the cylinder otherwise; the
// original expands this identically three times, so it was a macro in the source.
#define SHADOW_ENTRY_DRAW(entry)                    \
    do {                                            \
        switch ((entry)->type) {                    \
        case 0: break;                              \
        case 1:                                     \
            GXSetArray(0xB, color_tbl_cube, 4);     \
            GXCallDisplayList(cube_dl_top, 0x40);   \
            GXCallDisplayList(cube_dl_bottom, 0x20);\
            GXCallDisplayList(cube_dl_side, 0x20);  \
            break;                                  \
        }                                           \
        cylinder(entry);                            \
    } while (0)

void cylinderShadowDraw(CameraId camId, void* param) {
    ShadowEntry* entry = cswp->entries;
    f32 (*view)[4];
    Mtx mtx;
    u16 left, top, right, bottom;
    f32 zero;
    s32 i;
    GXColor color;

    (void)param;
    view = camGetCurPtr()->view;
    zero = float_0_804198f4;
    for (i = 0; i < cswp->unk104; i++, entry++) {
        if (entry->size == zero) {
            continue;
        }
        if (entry->unk2 != 2) {
            continue;
        }
        if (!(entry->flags & 2)) {
            continue;
        }
        if (*(s16*)&entry->camId != camId) {
            continue;
        }
        if (camId == 1) {
            sysWaitDrawSync();
            GXClearBoundingBox();
        }
        PSMTXConcat(view, entry->matrix, mtx);
        GXLoadPosMtxImm(mtx, 0);
        color = unk_80422fa0;
        color.a = entry->mode;
        GXSetTevColor(1, color);
        GXSetZMode(1, 3, 0);
        GXSetCullMode(2);
        GXSetBlendMode(1, 1, 1, 0);
        GXSetBlendMode(2, 1, 1, 7);
        GXSetAlphaUpdate(1);
        GXSetColorUpdate(0);
        GXSetChanCtrl(4, 0, 0, 1, 0, 2, 2);
        SHADOW_ENTRY_DRAW(entry);
        GXSetZMode(1, 3, 0);
        GXSetCullMode(1);
        GXSetBlendMode(0, 4, 5, 0);
        GXSetAlphaUpdate(1);
        GXSetColorUpdate(0);
        GXSetChanMatColor(4, unk_80422fa4);
        GXSetChanCtrl(4, 0, 0, 0, 0, 2, 2);
        SHADOW_ENTRY_DRAW(entry);
        GXSetCullMode(2);
        GXSetBlendMode(1, 6, 7, 0);
        GXSetAlphaUpdate(0);
        GXSetColorUpdate(1);
        GXSetChanMatColor(4, cswp->unk100);
        SHADOW_ENTRY_DRAW(entry);
        if (camId == 1) {
            sysWaitDrawSync();
            GXReadBoundingBox(&left, &top, &right, &bottom);
            offscreenAddBoundingBox(entry->unk18, left, top, right, bottom);
        }
    }
}

// NONMATCHING (96.18%): body matches; remaining diffs are call-argument register scheduling.
void projShadowEnd(s32 arg0, ProjShadowWork* work) {
    ShadowConfigFields* config;
    CameraEntry* cam;
    Mtx mtx;
    s32 size;
    s32 bufferSize;

    cam = camGetCurPtr();
    PSMTXCopy(tmp_view, cam->view);
    GXSetViewport(tmp_vp[0], tmp_vp[1], tmp_vp[2], tmp_vp[3], tmp_vp[4], tmp_vp[5]);
    GXSetScissor(tmp_sci[0], tmp_sci[1], tmp_sci[2], tmp_sci[3]);

    if (work->unk4 != 0) {
        config = (ShadowConfigFields*)(shadowConfig + work->unk8 * 0x48);
        size = ShadowSizeTbl[config->unk40];
        if (work->unk0 & 2) {
            if (work->unk0 & 4) {
                bufferSize = GXGetTexBufferSize((size >> 1) & 0x7FFF, (size >> 1) & 0x7FFF, 2, 0, 0);
            }
            else {
                bufferSize = GXGetTexBufferSize(size, size, 2, 0, 0);
            }
            work->unkFC = smartAlloc(bufferSize, 3);
            GXSetTexCopySrc(0, 0, size, size);
            if (work->unk0 & 4) {
                GXSetTexCopyDst((size >> 1) & 0x7FFF, (size >> 1) & 0x7FFF, 2, 1);
            }
            else {
                GXSetTexCopyDst(size, size, 2, 0);
            }
            GXSetZMode(1, 3, 1);
            GXCopyTex(work->unkFC->address, 1);
            GXPixModeSync();
            if (work->unk0 & 4) {
                GXInitTexObj(&work->texObj, work->unkFC->address, (size >> 1) & 0x7FFF, (size >> 1) & 0x7FFF, 2, 0, 0, 0);
            }
            else {
                GXInitTexObj(&work->texObj, work->unkFC->address, size, size, 2, 0, 0, 0);
            }
            GXInitTexObjLOD(&work->texObj, 1, 1, 0.0f, 0.0f, 0.0f, 0, 0, 0);
        }
        config = (ShadowConfigFields*)(shadowConfig + work->unk8 * 0x48);
        if (&work->unk7C != NULL) {
            if (config->unk38 == 0) {
                C_MTXLightFrustum(mtx, -config->unk2C, config->unk2C, config->unk28, -config->unk28, config->unk30, 0.5f, 0.5f, 0.5f, 0.5f);
            }
            else {
                C_MTXLightOrtho(mtx, -config->unk2C, config->unk2C, config->unk28, -config->unk28, 0.5f, 0.5f, 0.5f, 0.5f);
            }
            PSMTXConcat(mtx, work->unkC, work->unk7C);
        }
        work->unk0 |= 1;
    }
}

// NONMATCHING (95.81%): body matches. This compiler hoists `negSize * 0.5f` out of the entry loop into f31
// (the original keeps -2.5 in f30 and computes the product per iteration), which shifts the immediate-mode
// FP registers; the two GXColor argument-copy stack slots are also swapped (0x10/0x14 vs 0x14/0x10).
void shadowCharShadowDisp_Texture(CameraId camId) {
    ShadowEntry* entry = cswp->entries;
    f32 (*view)[4];
    u16 left, top, right, bottom;
    GXColor color;
    GXTexObj texObj;
    Mtx mtx;
    f32 zero = 0.0f;
    f32 size = 2.5f;
    f32 negSize = -size;
    s32 i;

    view = camGetCurPtr()->view;
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 0, 0, 2, 2);
    color = cswp->unk100;
    GXSetChanMatColor(4, color);
    effGetTexObjN64(0x2C, &texObj);
    GXLoadTexObj(&texObj, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0xA);
    GXSetTevAlphaIn(0, 7, 4, 5, 7);
    GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, unk_80422f9c);
    GXSetBlendMode(1, 4, 5, 0);
    GXSetZMode(1, 3, 0);
    GXSetZCompLoc(1);
    GXSetAlphaCompare(7, 0, 0, 7, 0);
    GXSetCurrentMtx(0);
    GXSetCullMode(0);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    for (i = 0; i < cswp->unk104; i++, entry++) {
        if (entry->size == zero) {
            continue;
        }
        if (entry->unk2 != 1) {
            continue;
        }
        if (!(entry->flags & 2)) {
            continue;
        }
        if (*(s16*)&entry->camId != camId) {
            continue;
        }
        if (camId == 1) {
            sysWaitDrawSync();
            GXClearBoundingBox();
        }
        PSMTXConcat(view, entry->matrix, mtx);
        GXLoadPosMtxImm(mtx, 0);
        GXBegin(GX_QUADS, GX_VTXFMT0, 4);
        GXPosition3f32(negSize * 0.5f, 0.0f, 1.25f);
        GXTexCoord2f32(0.0f, 0.0f);
        GXPosition3f32(1.25f, 0.0f, 1.25f);
        GXTexCoord2f32(1.0f, 0.0f);
        GXPosition3f32(1.25f, 0.0f, negSize * 0.5f);
        GXTexCoord2f32(1.0f, 1.0f);
        GXPosition3f32(negSize * 0.5f, 0.0f, negSize * 0.5f);
        GXTexCoord2f32(0.0f, 1.0f);
        if (camId == 1) {
            sysWaitDrawSync();
            GXReadBoundingBox(&left, &top, &right, &bottom);
            offscreenAddBoundingBox(entry->unk18, left, top, right, bottom);
        }
    }
}

// NONMATCHING (99.98%): only the two GXColor argument-copy stack slots are swapped (original 0x10/0x14,
// ours 0x14/0x10) and the 0.0f .sdata2 pool label differs; body/control flow match.
void shadowCharShadowDisp_Polygon(CameraId camId) {
    ShadowEntry* entry = cswp->entries;
    f32 (*view)[4];
    u16 left, top, right, bottom;
    GXColor color;
    Mtx mtx;
    f32 zero;
    s32 i;

    view = camGetCurPtr()->view;
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 0, 0, 2, 2);
    color = cswp->unk100;
    GXSetChanMatColor(4, color);
    GXSetNumTexGens(0);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0xFF, 0xFF, 4);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    GXSetTevColorIn(0, 0xF, 0xF, 0xF, 0xA);
    GXSetTevAlphaIn(0, 7, 1, 5, 7);
    GXSetFog(0, float_0_804198f4, float_0_804198f4, float_0_804198f4, float_0_804198f4, unk_80422f98);
    GXSetBlendMode(1, 4, 5, 0);
    GXSetZMode(1, 3, 0);
    GXSetZCompLoc(1);
    GXSetAlphaCompare(7, 0, 0, 7, 0);
    GXSetCurrentMtx(0);
    GXSetCullMode(0);
    GXSetArray(9, testKagePosArray, 0xC);
    GXClearVtxDesc();
    GXSetVtxDesc(9, 2);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    zero = float_0_804198f4;
    for (i = 0; i < cswp->unk104; i++, entry++) {
        if (entry->size == zero) {
            continue;
        }
        if (entry->unk2 != 0) {
            continue;
        }
        if (!(entry->flags & 2)) {
            continue;
        }
        if (*(s16*)&entry->camId != camId) {
            continue;
        }
        if (camId == 1) {
            sysWaitDrawSync();
            GXClearBoundingBox();
        }
        PSMTXConcat(view, entry->matrix, mtx);
        GXLoadPosMtxImm(mtx, 0);
        GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, 6);
        GXPosition1x8(0);
        GXPosition1x8(1);
        GXPosition1x8(2);
        GXPosition1x8(3);
        GXPosition1x8(4);
        GXPosition1x8(5);
        if (camId == 1) {
            sysWaitDrawSync();
            GXReadBoundingBox(&left, &top, &right, &bottom);
            offscreenAddBoundingBox(entry->unk18, left, top, right, bottom);
        }
    }
}

// NONMATCHING (99.77%): body matches; only float-constant pool labels (TU-global .sdata2 layout) and the
// _filter/_query argument-setup register choice differ.
void shadowCharShadowDisp_Collision(void) {
    ShadowEntry* entry = cswp->entries;
    HitCheckQuery query;
    HitEntry* hit;
    f32 (*mtx)[4];
    f32 size;
    f32 unk14;
    f32 zero;
    s32 i;

    camGetCurPtr();
    zero = float_0_804198f4;
    for (i = 0; i < cswp->unk104; i++, entry++) {
        mtx = entry->matrix;
        if (entry->size == zero) {
            continue;
        }
        if (entry->flags & 2) {
            continue;
        }
        if (entry->unk2 != 3 && (entry->flags & 1) != 0) {
            query.targetPos = entry->position;
            query.targetPos.y += float_1_80419904;
            query.targetDir = vec3_802c1a90;
            query.targetDistance = float_150_80419920;
            hit = hitCheckVecFilter(&query, _filter);
            if (hit == NULL) {
                continue;
            }
            size = entry->size;
            unk14 = entry->unk14;
            mtx[0][3] = query.hitPos.x + query.hitNormal.x;
            mtx[1][3] = query.hitPos.y + query.hitNormal.y;
            mtx[2][3] = query.hitPos.z + query.hitNormal.z;
            mtx[1][0] = query.hitNormal.x * size;
            mtx[1][1] = query.hitNormal.y * unk14;
            mtx[1][2] = query.hitNormal.z * size;
            mtx[2][0] = float_0_804198f4;
            mtx[2][1] = query.hitNormal.z * size;
            mtx[2][2] = -query.hitNormal.y * size;
            mtx[0][0] = size * (-query.hitNormal.y * query.hitNormal.y - query.hitNormal.z * query.hitNormal.z);
            mtx[0][1] = size * (query.hitNormal.x * query.hitNormal.y);
            mtx[0][2] = size * (query.hitNormal.x * query.hitNormal.z);
        }
        else {
            PSMTXTrans(mtx, entry->position.x, float_1p5_80419924 + entry->position.y, entry->position.z);
            size = entry->size;
            mtx[2][2] = size;
            mtx[0][0] = size;
            mtx[1][1] = entry->unk14;
        }
        entry->flags |= 2;
    }
}

void shadowMain(void) {
    dispEntry(CAMERA_3D, float_0_804198f4, 9, shadowDisp, NULL);
    dispEntry(CAMERA_OFFSCREEN2, float_0_804198f4, 9, shadowDisp, NULL);
    dispEntry(CAMERA_3D_EFFECTA, float_0_804198f4, 9, shadowDisp, NULL);
    dispEntry(CAMERA_3D_EFFECTB, float_0_804198f4, 9, shadowDisp, NULL);
    if (cswp->unk10C != 0) {
        dispEntry(CAMERA_SHADOW, float_0_804198f4, 5, shadowDisp, NULL);
    }
    dispEntry(CAMERA_SHADOW, float_0_804198f4, 0, shadowDisp, NULL);
    dispEntry(CAMERA_SHADOW, float_0_804198f4, 2, shadowDisp, NULL);
    dispEntry(CAMERA_SHADOW, float_0_804198f4, 7, shadowDisp, NULL);
    cswp->unk104 = 0;
    cswp->unk0 &= ~1;
    cswp->unk4 = 0;
    pswp->unk0 &= ~1;
    pswp->unk4 = 0;
    dswp->unk0 &= ~1;
    dswp->unk4 = 0;
}

// NONMATCHING (85.96%): 5 instructions differ, 15 missing, 9 extra, plus register differences.
void depthShadowEnd(s32 arg0, DepthShadowWork* work) {
    ShadowConfigFields* config;
    CameraEntry* cam;
    Mtx mtx;
    Mtx mtx2;
    GXTexObj* texObj;
    smartEntry* smart;
    f32 f29;
    f32 f30;
    f32 f31;
    f32 f4;
    s32 fmt;
    s32 sizeFmt;
    s32 size;
    s32 flag;

    cam = camGetCurPtr();
    PSMTXCopy(tmp_view, cam->view);
    GXSetColorUpdate(1);
    GXSetViewport(tmp_vp[0], tmp_vp[1], tmp_vp[2], tmp_vp[3], tmp_vp[4], tmp_vp[5]);
    GXSetScissor(tmp_sci[0], tmp_sci[1], tmp_sci[2], tmp_sci[3]);

    if (work->unk4 != 0) {
        config = (ShadowConfigFields*)(shadowConfig + work->unk8 * 0x48);
        if (config->unk3C <= 2) {
            fmt = 0x11;
            sizeFmt = 1;
        }
        else {
            fmt = 0x13;
            sizeFmt = 3;
        }
        if (work->unk0 & 2) {
            size = ShadowSizeTbl[config->unk40];
            smart = smartAlloc(GXGetTexBufferSize(size, size, sizeFmt, 0, 0), 3);
            *(smartEntry**)((u8*)work + 0xFC) = smart;
        }
        GXSetTexCopySrc(0, 0, size, size);
        GXSetTexCopyDst(size, size, fmt, 0);
        GXSetZMode(1, 3, 1);
        smart = *(smartEntry**)((u8*)work + 0xFC);
        GXCopyTex(smart->address, 1);
        GXPixModeSync();
        texObj = (GXTexObj*)((u8*)work + 0xDC);
        GXInitTexObj(texObj, smart->address, size, size, sizeFmt, 0, 0, 0);
        GXInitTexObjLOD(texObj, 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
        config = (ShadowConfigFields*)(shadowConfig + work->unk8 * 0x48);
        if (&work->unkC[0x70] != NULL) {
            flag = (config->unk3C > 2);
            if (config->unk38 == 0) {
                C_MTXLightFrustum(mtx, -config->unk2C, config->unk2C, config->unk28, -config->unk28, config->unk30, 0.5f, 0.5f, 0.5f, 0.5f);
            }
            else {
                C_MTXLightOrtho(mtx, -config->unk2C, config->unk2C, config->unk28, -config->unk28, 0.5f, 0.5f, 0.5f, 0.5f);
            }
            PSMTXConcat(mtx, (f32(*)[4])work->unkC, (f32(*)[4])&work->unkC[0x70]);
        }
        if (&work->unkC[0xA0] != NULL) {
            f29 = config->unk30;
            f30 = config->unk34;
            f31 = f30 - f29;
            PSMTXScale(mtx2, 0.0f, 0.0f, 0.0f);
            if (flag) {
                f4 = float_256_804198fc;
            }
            else {
                f4 = float_16_80419900;
            }
            if (config->unk38 == 0) {
                mtx2[0][2] = f30 / f31;
                mtx2[0][3] = f30 * f29 / f31;
                mtx2[1][2] = (f30 / f31) * f4;
                mtx2[1][3] = (f30 * f29 / f31) * f4;
                mtx2[2][2] = float_1_80419904;
                mtx2[2][3] = config->unk44;
            }
            else {
                mtx2[0][2] = float_neg1_80419908 / f31;
                mtx2[0][3] = -f29 / f31;
                mtx2[1][2] = (float_neg1_80419908 / f31) * f4;
                mtx2[1][3] = (-f29 / f31) * f4;
                mtx2[2][3] = float_1_80419904 + config->unk44;
            }
            PSMTXConcat(mtx2, (f32(*)[4])work->unkC, (f32(*)[4])&work->unkC[0xA0]);
        }
        work->unk0 |= 1;
    }
}
