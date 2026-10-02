#include "driver/shadowdrv.h"
#include "driver/dispdrv.h"
#include "driver/hitdrv.h"
#include "driver/mobjdrv.h"
#include "driver/offscreendrv.h"
#include "eff/eff_n64.h"
#include "mario/mariost.h"
#include "system.h"
#include <string.h>

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u8 lbl_80303460[8] = { 0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00 };

extern const Vec vec3_802c1a90;
extern const Vec vec3_802c1a9c;
extern const Vec vec3_802c1aa8;
extern const Vec vec3_802c1ab4;
extern const Vec vec3_802c1acc;
extern const Vec vec3_802c1ae4;
extern GXColor dat_804198e4;
extern GXColor dat_804198e8;
extern Vec testKagePosArray[];
extern GXColor unk_80422f98;
extern GXColor unk_80422f9c;
extern GXColor unk_80422fa0;
extern GXColor unk_80422fa4;
extern u8 cube_dl_top[0x40];
extern u8 cube_dl_bottom[0x20];
extern u8 cube_dl_side[0x20];
extern Vec vertex_tbl[];
extern const f32 float_1000_8041992c;
extern const f32 float_0_804198f4;
extern const f32 float_0p5_804198f8;
extern const f32 float_0p2_8041990c;
extern const f32 float_1_80419904;
extern const f32 float_150_80419920;
extern const f32 float_1p5_80419924;
extern const f32 float_1p25_8041991c;
extern const f32 float_2p5_80419918;
extern const f32 float_256_804198fc;
extern const f32 float_16_80419900;
extern const f32 float_neg1_80419908;

typedef struct ShadowConfigFields {
    u8 pad00[0x4];
    Vec camPos;  //0x4
    Vec up;      //0x10
    Vec target;  //0x1C
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
extern GXColor dat_804198e0;
extern GXColor dat_804198ec;
extern GXColor dat_804198f0;
extern const f32 float_20_80419928;

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

#ifdef MUST_MATCH
// MUST_MATCH: returning the size through an inline lets MWCC coalesce the call result into the
// destination register instead of staging it through r0 (`mr r0,r3; mr rN,r0`). The natural direct-call
// form is under the #else at each use.
static inline u32 shadowTexBufferSize(u16 w, u16 h, u32 fmt) {
    return GXGetTexBufferSize(w, h, fmt, GX_FALSE, 0);
}
#endif

// The instruction stream, call sequence and block1 registers match. The block2 ramp rotation differs:
// MWCC colours `bufferSize2`/`ramp16` r31/r30 where retail has r30/r31 (the loop's sthx base and the
// second DCFlushRange args follow the swap). Data references use the original symbols, not pool constants.
// NONMATCHING (99.80%): block2 `bufferSize2`/`ramp16` register swap (r31/r30 vs retail r30/r31).
void shadowInit(void) { //almost 1:1, minor regalloc
    u32 bufferSize;
    u8* ramp8;
    u32 bufferSize2;
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

#ifdef MUST_MATCH
    bufferSize = shadowTexBufferSize(0x10, 0x10, GX_TF_I8);
#else
    bufferSize = GXGetTexBufferSize(0x10, 0x10, GX_TF_I8, GX_FALSE, 0);
#endif
    ramp8 = __memAlloc(HEAP_DEFAULT, GXGetTexBufferSize(0x10, 0x10, GX_TF_I8, GX_FALSE, 0));
    for (i = 0; i < 256; i++) {
        ramp8[(((i & 0x3) * 8) + (((i & 0xC) * 16) + (((i & 0x80) / 4) + ((i & 0x70) / 16))))] = i;
    }
    GXInitTexObj(&rampTex8, ramp8, 0x10, 0x10, GX_TF_I8, GX_CLAMP, GX_REPEAT, GX_FALSE);
    GXInitTexObjLOD(&rampTex8, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    DCFlushRange(ramp8, bufferSize);

#ifdef MUST_MATCH
    bufferSize2 = shadowTexBufferSize(0x100, 0x100, GX_TF_IA8);
#else
    bufferSize2 = GXGetTexBufferSize(0x100, 0x100, GX_TF_IA8, GX_FALSE, 0);
#endif
    ramp16 = __memAlloc(HEAP_DEFAULT, GXGetTexBufferSize(0x100, 0x100, GX_TF_IA8, GX_FALSE, 0));
    for (i = 0; i < 0x10000; i++) {
        ramp16[(((i & 0x3) * 4) + (((i & 0xFC) * 256) + (((i & 0xFC00) / 64) + ((i & 0x300) / 256))))] = i;
    }
    GXInitTexObj(&rampTex16, ramp16, 0x100, 0x100, GX_TF_IA8, GX_CLAMP, GX_REPEAT, GX_FALSE);
    GXInitTexObjLOD(&rampTex16, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    DCFlushRange(ramp16, bufferSize2);

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

void cylinder(ShadowEntry* entry) {
    u8* base = lbl_80303460;

    if (entry->flags & 4) {
        GXSetArray(0xB, color_tbl_nodecay, 4);
        GXCallDisplayList(base + 0x140, 0x40);
        GXCallDisplayList(base + 0x180, 0x40);
        GXCallDisplayList(base + 0x1C0, 0x40);
    } else {
        f32 alpha = (f32)cswp->unk100.a;
#ifdef MUST_MATCH
        // MUST_MATCH: declaring the fade clamp before the alpha ramp makes MWCC keep the u8->f32
        // conversion scratch in f3 and reuse it, giving retail's FP register allocation.
        f32 f31 = 1.0f - entry->decayZ;
        f32 f2 = alpha * entry->decayX / 255.0f;
#else
        f32 f2 = alpha * entry->decayX / 255.0f;
        f32 f31 = 1.0f - entry->decayZ;
#endif
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

#ifdef MUST_MATCH
// MUST_MATCH: building the by-value GXColor inside an inlined helper puts its scratch slot below the
// GXSetTevColor argument copy and gives the helper's slot the lower address (r1+0x18, argument r1+0x1c),
// as the original does; written inline the two swap. Same for shadowMatColor: the by-value color is built
// in the helper so its scratch slot lands where retail's does.
static inline void shadowTevColor(u8 a) {
    GXColor c = unk_80422fa0;
    c.a = a;
    GXSetTevColor(1, c);
}

static inline void shadowMatColor(GXColor c) {
    GXSetChanMatColor(4, c);
}
#endif

void cylinderShadowDraw(CameraId camId, void* param) {
    ShadowEntry* entry = cswp->entries;
    f32 (*view)[4];
    Mtx mtx;
    u16 left, top, right, bottom;
    f32 zero;
    s32 i;
#ifndef MUST_MATCH
    GXColor color;
#endif

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
#ifdef MUST_MATCH
        shadowTevColor(entry->mode);
#else
        color = unk_80422fa0;
        color.a = entry->mode;
        GXSetTevColor(1, color);
#endif
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
#ifdef MUST_MATCH
        shadowMatColor(unk_80422fa4);
#else
        GXSetChanMatColor(4, unk_80422fa4);
#endif
        GXSetChanCtrl(4, 0, 0, 0, 0, 2, 2);
        SHADOW_ENTRY_DRAW(entry);
        GXSetCullMode(2);
        GXSetBlendMode(1, 6, 7, 0);
        GXSetAlphaUpdate(0);
        GXSetColorUpdate(1);
#ifdef MUST_MATCH
        shadowMatColor(cswp->unk100);
#else
        GXSetChanMatColor(4, cswp->unk100);
#endif
        SHADOW_ENTRY_DRAW(entry);
        if (camId == 1) {
            sysWaitDrawSync();
            GXReadBoundingBox(&left, &top, &right, &bottom);
            offscreenAddBoundingBox(entry->unk18, left, top, right, bottom);
        }
    }
}

void projShadowEnd(s32 arg0, ProjShadowWork* work) {
    ShadowConfigFields* config;
    CameraEntry* cam;
    Mtx mtx;
    u16 size;
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
        }
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
void shadowCharShadowDisp_Texture(CameraId camId, void* param) {
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

#ifdef MUST_MATCH
// MUST_MATCH: building the by-value GXColor inside an inlined helper makes MWCC put its local in the
// lower scratch slot (r1+0x10) and the GXSetFog copy in r1+0x14, as the original does; writing the
// colour inline puts them the other way round. The independent form is under #else.
static inline void shadowPolyMatColor(void) {
    GXColor c = cswp->unk100;
    GXSetChanMatColor(4, c);
}
#endif

void shadowCharShadowDisp_Polygon(CameraId camId, void* param) {
    ShadowEntry* entry = cswp->entries;
    f32 (*view)[4];
    u16 left, top, right, bottom;
    Mtx mtx;
    f32 zero;
    s32 i;

    view = camGetCurPtr()->view;
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 0, 0, 2, 2);
#ifdef MUST_MATCH
    shadowPolyMatColor();
#else
    {
        GXColor color = cswp->unk100;
        GXSetChanMatColor(4, color);
    }
#endif
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

#ifdef MUST_MATCH
// MUST_MATCH: retail builds the modified GXColor in one scratch slot and then copies it to the argument
// slot; returning it from an inline reproduces that instead of writing the channels in place.
static inline GXColor shadowProjColor(u8 v) {
    GXColor c = dat_804198e8;

    c.r = c.g = c.b = v;
    return c;
}
#endif

// Body, control flow and the work/config register pair (r29/r30) match. MUST_MATCH allocation choices:
// `work` is `void*` with `(CharShadowWork*)` casts at each use (the natural `CharShadowWork* work`
// makes MWCC pick r30/r29 and drops to 96.80%); and the C_MTXLookAt sources are copied through nine
// explicit u32 temps, which is what makes MWCC batch them as "load all nine words, then store all
// nine" like retail instead of interleaving (that alone is +2.3%). The two .sdata2 `~~` pool labels
// vanish when the unit links. Still differs only in MWCC allocation/scheduling: inside that batched
// Vec copy MWCC colours the nine temps in the opposite register order from retail (ours
// r7..r12,r26,r27,r0 vs retail r27,r26,r12..r7,r0), the GXColor argument scratch slots are permuted
// (ours 0x18/0x14/0x10/0x8/0xc vs retail 0x8/0xc/0x18/0x10/0x14), and one `fsubs f3,f2,f29` is
// scheduled one slot later.
// NONMATCHING (99.27%): register/stack-slot allocation and one scheduler slot, as detailed above.
void shadowCharShadowDisp_Projection(CameraId camId, void* param) {
    ShadowConfigFields* config;
    void* work;
    ShadowEntry* entry;
#ifdef MUST_MATCH
    const u8* vecs;
#endif
    f32 (*view)[4];
    u8 alpha;
    Mtx mtx;
    Mtx44 projMtx;
    Mtx lookAt;
    Vec camPos;
    Vec camUp;
    Vec camTarget;
    f32 zero;
    f32 w;
    f32 d;
    f32 we;
    f32 de;
    s32 i;

    (void)camId;
    (void)param;
    work = cswp;
#ifdef MUST_MATCH
    vecs = (const u8*)&vec3_802c1a90;
#endif
    config = (ShadowConfigFields*)(shadowConfig + ((CharShadowWork*)work)->unk8 * 0x48);
    entry = ((CharShadowWork*)work)->entries;
    view = camGetCurPtr()->view;
    alpha = cswp->unk100.a;

    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 0, 0, 2, 2);
    GXSetChanMatColor(4, cswp->unk100);
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
    GXSetBlendMode(0, 4, 5, 0);
    if (gp->inBattle != 0) {
        GXSetChanMatColor(4, dat_804198e4);
    }
    else {
#ifdef MUST_MATCH
        GXSetChanMatColor(4, shadowProjColor(alpha));
#else
        GXColor color = dat_804198e8;

        color.r = color.g = color.b = alpha;
        GXSetChanMatColor(4, color);
#endif
    }
    GXSetZMode(0, 3, 0);
    for (i = 0; i < cswp->unk104; i++, entry++) {
        if (entry->size == float_0_804198f4) {
            continue;
        }
        if (entry->unk2 != 3) {
            continue;
        }
        if (!(entry->flags & 2)) {
            continue;
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
        ((CharShadowWork*)work)->unk4++;
    }
    if (((CharShadowWork*)work)->unk4 != 0) {
        C_MTXOrtho(projMtx, config->unk2C, -config->unk2C, config->unk28, -config->unk28, config->unk30, config->unk34);
        GXSetProjection(projMtx, 1);
        GXSetBlendMode(1, 4, 5, 0);
        GXSetChanCtrl(4, 0, 0, 1, 0, 2, 2);
        GXClearVtxDesc();
        GXSetVtxDesc(9, 1);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetVtxDesc(0xB, 1);
        GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
#ifdef MUST_MATCH
        {
            /* Copy the three camera basis vectors through explicit word temps: this forces MWCC to
               emit retail's "nine loads, then nine stores" instead of interleaving load/store. */
            const u32* vp = (const u32*)(vecs + 0xC);
            s32 v0 = vp[0], v1 = vp[1], v2 = vp[2];
            s32 v3 = vp[3], v4 = vp[4], v5 = vp[5];
            s32 v6 = vp[6], v7 = vp[7], v8 = vp[8];

            ((u32*)&camPos)[0] = v0; ((u32*)&camPos)[1] = v1; ((u32*)&camPos)[2] = v2;
            ((u32*)&camUp)[0] = v3; ((u32*)&camUp)[1] = v4; ((u32*)&camUp)[2] = v5;
            ((u32*)&camTarget)[0] = v6; ((u32*)&camTarget)[1] = v7; ((u32*)&camTarget)[2] = v8;
        }
#else
        camPos = vec3_802c1a9c;
        camUp = vec3_802c1aa8;
        camTarget = vec3_802c1ab4;
#endif
        C_MTXLookAt(lookAt, &camPos, &camUp, &camTarget);
        GXLoadPosMtxImm(lookAt, 0);
        w = -config->unk28;
        d = config->unk2C;
        de = float_0p2_8041990c * d;
        we = float_0p2_8041990c * w;
        GXBegin(GX_QUADS, GX_VTXFMT0, 16);
        {
        GXPosition3f32(float_0_804198f4 - w, float_0_804198f4, float_0_804198f4 - d);
        GXColor4u8(0, 0, 0, 0xFF);
        GXPosition3f32(float_0_804198f4 + w, float_0_804198f4, float_0_804198f4 - d);
        GXColor4u8(0, 0, 0, 0xFF);
        GXPosition3f32(float_0_804198f4 + w, float_0_804198f4, de + (float_0_804198f4 - d));
        GXColor4u8(0, 0, 0, 0);
        GXPosition3f32(float_0_804198f4 - w, float_0_804198f4, de + (float_0_804198f4 - d));
        GXColor4u8(0, 0, 0, 0);
        GXPosition3f32(float_0_804198f4 - w, float_0_804198f4, float_0_804198f4 + d);
        GXColor4u8(0, 0, 0, 0xFF);
        GXPosition3f32(float_0_804198f4 + w, float_0_804198f4, float_0_804198f4 + d);
        GXColor4u8(0, 0, 0, 0xFF);
        GXPosition3f32(float_0_804198f4 + w, float_0_804198f4, (float_0_804198f4 + d) - de);
        GXColor4u8(0, 0, 0, 0);
        GXPosition3f32(float_0_804198f4 - w, float_0_804198f4, (float_0_804198f4 + d) - de);
        GXColor4u8(0, 0, 0, 0);
        GXPosition3f32(float_0_804198f4 - w, float_0_804198f4, float_0_804198f4 - d);
        GXColor4u8(0, 0, 0, 0xFF);
        GXPosition3f32(float_0_804198f4 - w, float_0_804198f4, float_0_804198f4 + d);
        GXColor4u8(0, 0, 0, 0xFF);
        GXPosition3f32(we + (float_0_804198f4 - w), float_0_804198f4, float_0_804198f4 + d);
        GXColor4u8(0, 0, 0, 0);
        GXPosition3f32(we + (float_0_804198f4 - w), float_0_804198f4, float_0_804198f4 - d);
        GXColor4u8(0, 0, 0, 0);
        GXPosition3f32(float_0_804198f4 + w, float_0_804198f4, float_0_804198f4 - d);
        GXColor4u8(0, 0, 0, 0xFF);
        GXPosition3f32(float_0_804198f4 + w, float_0_804198f4, float_0_804198f4 + d);
        GXColor4u8(0, 0, 0, 0xFF);
        GXPosition3f32((float_0_804198f4 + w) - we, float_0_804198f4, float_0_804198f4 + d);
        GXColor4u8(0, 0, 0, 0);
        GXPosition3f32((float_0_804198f4 + w) - we, float_0_804198f4, float_0_804198f4 - d);
        GXColor4u8(0, 0, 0, 0);
        }
    }
}

// The `q` pointer plus `opt_propagation off` are both required (and only together): with either one
// missing MWCC materialises `_filter` through r3 and schedules its low addi before `&query`, instead of
// reserving r3 for `&query` and putting `_filter` straight in r4. The declaration order below then keeps
// mtx in r29 and i in r30.
#pragma opt_propagation off // needed, with `q`, so MWCC reserves r3 for &query and puts _filter in r4
void shadowCharShadowDisp_Collision(CameraId cameraId, void* param) {
    ShadowEntry* entry = cswp->entries;
    s32 i;
    HitCheckQuery query;
    f32 (*mtx)[4];
    HitEntry* hit;
#ifdef MUST_MATCH
    HitCheckQuery* q;
#endif
    f32 size;
    f32 unk14;
    f32 zero;

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
#ifdef MUST_MATCH
            q = &query;
            q->targetPos = entry->position;
            q->targetPos.y += 1.0f;
            q->targetDir = vec3_802c1a90;
            q->targetDistance = 150.0f;
            hit = hitCheckVecFilter(q, _filter);
#else
            query.targetPos = entry->position;
            query.targetPos.y += 1.0f;
            query.targetDir = vec3_802c1a90;
            query.targetDistance = 150.0f;
            hit = hitCheckVecFilter(&query, _filter);
#endif
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
#pragma opt_propagation reset

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

// The viewport/scissor/look-at/projection block the original expands identically for the
// proj, proj+depth and char shadow draw paths; it was a macro in the source.
// The 0x4/0x10/0x1C offsets are the camera vectors in the config blob's unnamed head
// (ShadowConfigFields only names 0x28+); 0x3C is ProjShadowWork's pad3C. No field exists for either.
#define SHADOW_DISP_PROJ_SETUP(w, extra)                                                                                                   \
    do {                                                                                                                                   \
        u16 size;                                                                                                                          \
        Mtx44* proj;                                                                                                                       \
        ShadowConfigFields* cfg;                                                                                                           \
        Vec camPos;                                                                                                                        \
        GXInvalidateTexAll();                                                                                                              \
        size = ShadowSizeTbl[((ShadowConfigFields*)(shadowConfig + (w)->unk8 * 0x48))->unk40];                                               \
        extra;                                                                                                                             \
        GXGetViewportv(tmp_vp);                                                                                                            \
        GXGetScissor(&tmp_sci[0], &tmp_sci[1], &tmp_sci[2], &tmp_sci[3]);                                                                  \
        GXSetViewport(0.0f, 0.0f, (f32)size, (f32)size, 0.0f, 1.0f);                                                                        \
        GXSetScissor(1, 1, size - 2, size - 2);                                                                                            \
        cfg = (ShadowConfigFields*)(shadowConfig + (w)->unk8 * 0x48);                                                                       \
        proj = (Mtx44*)((u8*)(w) + 0x3C);                                                                                                  \
        camPos = cfg->camPos;                                                                                                              \
        C_MTXLookAt(w->unkC, &camPos, &cfg->up, &cfg->target);                                                              \
        if (cfg->unk38 == 0) {                                                                                                             \
            C_MTXFrustum(*proj, cfg->unk2C, -cfg->unk2C, cfg->unk28, -cfg->unk28, cfg->unk30, cfg->unk34);                                 \
        }                                                                                                                                  \
        else {                                                                                                                             \
            C_MTXOrtho(*proj, cfg->unk2C, -cfg->unk2C, cfg->unk28, -cfg->unk28, cfg->unk30, cfg->unk34);                                   \
        }                                                                                                                                  \
        GXSetProjection(*shadowDispProjMtx(w), ((ShadowConfigFields*)(shadowConfig + (w)->unk8 * 0x48))->unk38);                                                                                                \
        PSMTXCopy(camGetCurPtr()->view, tmp_view);                                                                                         \
        PSMTXCopy(w->unkC, camGetCurPtr()->view);                                                                          \
    } while (0)

#ifdef MUST_MATCH
// MUST_MATCH: building the by-value GXColor in an inlined helper is what puts its copy in the lower
// scratch slot (r1+0x8) and the GXSetFog copy in r1+0xc, as the original does; writing the colour
// inline puts them the other way round. The independent form is under #else.
static inline void shadowDispMatColor(void) {
    GXColor c = cswp->unk100;
    GXSetChanMatColor(4, c);
}
#endif

// The projection matrix as a fresh base+0x3C expression (writing it through the `proj`
// local CSEs and the original recomputes it for GXSetProjection).
static inline Mtx44* shadowDispProjMtx(ProjShadowWork* w) { return (Mtx44*)((u8*)w->unkC + 0x30); }

// The body and all constants match; the only diff is the .data base register. The original reaches
// `shadowConfig + unk8*0x48` (and testKagePosArray / vertex_tbl) as `addi rX, r31, 0x40` off the
// shared .data base r31 = lbl_80303460, because shadowConfig (+0x40), testKagePosArray (+0xE0) and
// vertex_tbl (+0x2C0) are defined in this unit's .data. They are only `extern` here, so MWCC
// materialises each with `lis/addi shadowConfig`; that is the whole diff (the base setup plus the
// register pairing it forces). With stand-in .data objects at those offsets the function reaches
// 99.25% (only the size register and one `mr`/`addi` on GXSetProjection remain). See REPORT.md.
// NONMATCHING (92.38%): only the .data base register (shadowConfig/testKagePosArray/vertex_tbl are extern).
void shadowDisp(CameraId cameraId, void* param) {
    DispEntry* work = dispGetCurWork();
    ShadowConfigFields* config;
    ProjShadowWork* workp;

    if (cameraId == CAMERA_OFFSCREEN2 || cameraId == CAMERA_3D || cameraId == CAMERA_3D_EFFECTA || cameraId == CAMERA_3D_EFFECTB) {
        shadowCharShadowDisp_Polygon(cameraId, param);
        camGetCurPtr();
        GXSetNumChans(1);
        GXSetChanCtrl(4, 0, 0, 0, 0, 2, 2);
#ifdef MUST_MATCH
        shadowDispMatColor();
#else
        {
            GXColor color = cswp->unk100;
            GXSetChanMatColor(4, color);
        }
#endif
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
        GXClearVtxDesc();
        GXSetVtxDesc(9, 2);
        GXSetVtxDesc(0xB, 2);
        GXSetVtxAttrFmt(0, 9, 1, 4, 0);
        GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
        GXSetArray(9, vertex_tbl, 0xC);
        GXSetArray(0xB, color_tbl, 4);
        cylinderShadowDraw(cameraId, param);
        shadowCharShadowDisp_Texture(cameraId, param);
    }
    else if (cameraId == CAMERA_SHADOW) {
        switch (work->renderMode) {
        case 5: {
            shadowCharShadowDisp_Collision(cameraId, param);
            config = (ShadowConfigFields*)(shadowConfig + cswp->unk8 * 0x48);
            if (*(s32*)config == 1) {
                CameraEntry* cam = camGetPtr(CAMERA_3D);

                config->camPos.x = cam->cameraAt.x;
                config->camPos.y = float_1000_8041992c + cam->cameraAt.y;
                config->camPos.z = cam->cameraAt.z;
                config->up.x = float_0_804198f4;
                config->up.y = float_0_804198f4;
                config->up.z = float_neg1_80419908;
                config->target.x = cam->cameraAt.x;
                config->target.y = cam->cameraAt.y;
                config->target.z = cam->cameraAt.z;
            }
            workp = (ProjShadowWork*)cswp;
            SHADOW_DISP_PROJ_SETUP(workp, (void)0);
            shadowCharShadowDisp_Projection(cameraId, param);
            projShadowEnd(cameraId, (ProjShadowWork*)cswp);
            break;
        }
        case 0: {
            workp = pswp;
            SHADOW_DISP_PROJ_SETUP(workp, (void)0);
            break;
        }
        case 2: {
            projShadowEnd(cameraId, pswp);
            workp = (ProjShadowWork*)dswp;
            SHADOW_DISP_PROJ_SETUP(workp, GXSetColorUpdate(0));
            break;
        }
        case 7:
            depthShadowEnd(cameraId, dswp);
            break;
        }
    }
}

void depthShadowEnd(s32 arg0, DepthShadowWork* work) {
    u32 flag;
    ShadowConfigFields* config;
    ShadowConfigFields* config2;
    CameraEntry* cam;
    Mtx mtx2;
    Mtx mtx;
    f32 f31;
    f32 f30;
    f32 f29;
    f32 f4;
    s32 sizeFmt;
    s32 fmt;
    u16 size;

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
            *(smartEntry**)((u8*)work + 0xFC) = smartAlloc(GXGetTexBufferSize(size, size, sizeFmt, 0, 0), 3);
        }
        GXSetTexCopySrc(0, 0, size, size);
        GXSetTexCopyDst(size, size, fmt, 0);
        GXSetZMode(1, 3, 1);
        GXCopyTex(((smartEntry*)*(void**)((u8*)work + 0xFC))->address, 1);
        GXPixModeSync();
        GXInitTexObj((GXTexObj*)&work->unkC[0xD0], ((smartEntry*)*(void**)((u8*)work + 0xFC))->address, size, size, sizeFmt, 0, 0, 0);
        GXInitTexObjLOD((GXTexObj*)&work->unkC[0xD0], 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
        config2 = (ShadowConfigFields*)(shadowConfig + work->unk8 * 0x48);
        flag = (config2->unk3C > 2);
        if (&work->unkC[0x70] != NULL) {
            if (config2->unk38 == 0) {
                C_MTXLightFrustum(mtx, -config2->unk2C, config2->unk2C, config2->unk28, -config2->unk28, config2->unk30, 0.5f, 0.5f, 0.5f, 0.5f);
            }
            else {
                C_MTXLightOrtho(mtx, -config2->unk2C, config2->unk2C, config2->unk28, -config2->unk28, 0.5f, 0.5f, 0.5f, 0.5f);
            }
            PSMTXConcat(mtx, (f32(*)[4])work->unkC, (f32(*)[4])&work->unkC[0x70]);
        }
        if (&work->unkC[0xA0] != NULL) {
            f29 = config2->unk30;
            f30 = config2->unk34;
            f31 = f30 - f29;
            PSMTXScale(mtx2, 0.0f, 0.0f, 0.0f);
            if (flag) {
                f4 = float_256_804198fc;
            }
            else {
                f4 = float_16_80419900;
            }
            if (config2->unk38 == 0) {
                f32 d = f30 / f31;
                f32 n = f30 * f29 / f31;

                mtx2[0][2] = d;
                mtx2[0][3] = n;
                mtx2[1][2] = d * f4;
                mtx2[1][3] = n * f4;
                mtx2[2][2] = float_1_80419904;
                mtx2[2][3] = config2->unk44;
            }
            else {
                f32 d = float_neg1_80419908 / f31;
                f32 n = -f29 / f31;

                mtx2[0][2] = d;
                mtx2[0][3] = n;
                mtx2[1][2] = d * f4;
                mtx2[1][3] = n * f4;
                mtx2[2][3] = float_1_80419904 + config2->unk44;
            }
            PSMTXConcat(mtx2, (f32(*)[4])work->unkC, (f32(*)[4])&work->unkC[0xA0]);
        }
        work->unk0 |= 1;
    }
}
