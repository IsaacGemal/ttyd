#include "driver/shadowdrv.h"
#include "driver/dispdrv.h"
#include "driver/hitdrv.h"
#include "driver/mobjdrv.h"
#include <string.h>

extern const Vec vec3_802c1a90;
extern const Vec vec3_802c1acc;
extern const Vec vec3_802c1ae4;

typedef struct ShadowConfigFields {
    u8 pad00[0x28];
    f32 unk28;
    f32 unk2C;
    f32 unk30;
    u8 pad34[4];
    s32 unk38;
    u8 pad3C[4];
    s32 unk40;
} ShadowConfigFields;

Mtx tmp_view;
u32 tmp_sci[4];
f32 tmp_vp[6];

extern u8 shadowConfig[];
extern u8 lbl_80303460[];
MapObject* mobjHitObjPtrToPtr(HitEntry* hitObj);
void shadowDisp(CameraId cameraId, void* param);

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

void shadowInit(void) { //almost 1:1, minor regalloc
    u8* ramp8;
    u16* ramp16;
    u32 i;
    u32 bufferSize;
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
        MapObject* obj = mobjHitObjPtrToPtr(hit);
        if (*(u32*)obj->field_0x0 & 0x800) {
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

// NONMATCHING (95.08%): instruction sequence identical; only scratch register allocation differs.
void shadowEntry(f32 x, f32 y, f32 z, f32 size) {
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

// NONMATCHING (99.94%): body identical; only the 0.0f .sdata2 relocation differs (pool offsets).
// NONMATCHING (98.91%): body matches; only float-constant pool labels and a couple of operand orders differ.
void shadowCharShadowDisp_Collision(void) {
    ShadowEntry* entry = cswp->entries;
    HitCheckQuery query;
    HitEntry* hit;
    f32 (*mtx)[4];
    f32 size;
    f32 unk14;
    s32 i;

    camGetCurPtr();
    for (i = 0; i < cswp->unk104; i++, entry++) {
        mtx = entry->matrix;
        if (entry->size == 0.0f) {
            continue;
        }
        if (entry->flags & 2) {
            continue;
        }
        if (entry->unk2 != 3 && (entry->flags & 1) != 0) {
            query.targetPos = entry->position;
            query.targetPos.y += 1.0f;
            query.targetDir = vec3_802c1a90;
            query.targetDistance = 150.0f;
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
            mtx[2][0] = 0.0f;
            mtx[2][1] = query.hitNormal.z * size;
            mtx[2][2] = -query.hitNormal.y * size;
            mtx[0][0] = size * (-query.hitNormal.y * query.hitNormal.y - query.hitNormal.z * query.hitNormal.z);
            mtx[0][1] = size * (query.hitNormal.x * query.hitNormal.y);
            mtx[0][2] = size * (query.hitNormal.x * query.hitNormal.z);
        }
        else {
            PSMTXTrans(mtx, entry->position.x, 1.5f + entry->position.y, entry->position.z);
            mtx[2][2] = entry->size;
            mtx[0][0] = entry->size;
            mtx[1][1] = entry->unk14;
            entry->flags |= 2;
        }
    }
}

void shadowMain(void) {
    dispEntry(CAMERA_3D, 0.0f, 9, shadowDisp, NULL);
    dispEntry(CAMERA_OFFSCREEN2, 0.0f, 9, shadowDisp, NULL);
    dispEntry(CAMERA_3D_EFFECTA, 0.0f, 9, shadowDisp, NULL);
    dispEntry(CAMERA_3D_EFFECTB, 0.0f, 9, shadowDisp, NULL);
    if (cswp->unk10C != 0) {
        dispEntry(CAMERA_SHADOW, 0.0f, 5, shadowDisp, NULL);
        dispEntry(CAMERA_SHADOW, 0.0f, 0, shadowDisp, NULL);
        dispEntry(CAMERA_SHADOW, 0.0f, 2, shadowDisp, NULL);
        dispEntry(CAMERA_SHADOW, 0.0f, 7, shadowDisp, NULL);
    }
    cswp->unk104 = 0;
    cswp->unk0 &= ~1;
    cswp->unk4 = 0;
    pswp->unk0 &= ~1;
    pswp->unk4 = 0;
    dswp->unk0 &= ~1;
    dswp->unk4 = 0;
}
