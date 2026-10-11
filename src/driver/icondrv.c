#include "driver/icondrv.h"
#include "driver/dispdrv.h"
#include "driver/mapdrv.h"
#include "evt/evt_cmd.h"
#include "manager/dvdmgr.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"
#include "sdk/texPalette.h"
#include <string.h>

#include <stdio.h>
#include "driver/shadowdrv.h"

// Data objects defined later in this file or in other units, declared before their first use.
extern u32 dat_8041977c;


extern GlobalWork* gp;

// .sdata2 constants this unit references by name in retail
extern u32 dat_80419780;
extern u32 dat_80419778;
extern u32 dat_80419784;
extern GXColor dat_80419788;
extern GXColor dat_8041978c;
extern GXColor dat_80419790;

//.sbss
TEXPalette* icon_tpl;
IconBinary* icon_bin;
BOOL icon_tpl_ok;
BOOL icon_bin_ok;
static IconWork work[2];

// local prototypes


// TODO: inline function? I see nothing in the symbol map
#define iconGetWork() (gp->inBattle ? &work[1] : &work[0])

// TODO: remove? no function in original symbol map
inline IconEntry* iconGetEntry(const char* name) {
    IconWork* wp = iconGetWork();
    IconEntry* entry = wp->entries;
    int i;

    for (i = 0; i < wp->count; i++, entry++) {
        if (entry->flags & 1 && !strcmp(entry->name, name)) {
            return entry;
        }
    }
    return NULL;
}

// MUST_MATCH: the retail code walks the frame table through a pointer cast from the entry start (frames at +2 u32 slots);
// `data->frames[i]` compiles to the same loads but the registers come out differently (the inline accessor matters too).
#ifdef MUST_MATCH
static inline IconFrame* iconFramesOf(IconBinaryEntry* e) {
    return (IconFrame*)e;
}
#define ICON_FRAMES_DECL IconFrame* frames;
#define ICON_FRAMES_INIT(d) frames = iconFramesOf(d);
#define ICON_FRAME(d, i) frames[(i) + 2]
#else
#define ICON_FRAMES_DECL
#define ICON_FRAMES_INIT(d)
#define ICON_FRAME(d, i) (d)->frames[i]
#endif

static void _callback_tpl(s32 error, DVDFileInfo* info) {
    void* userData = info->cb.userData;

    UnpackTexPalette(icon_tpl);
    DVDMgrClose(userData);
    icon_tpl_ok = TRUE;
}

void _callback_bin(s32 error, DVDFileInfo* info) {
    DVDMgrClose(info->cb.userData);
    icon_bin_ok = TRUE;
}

void iconInit(void) {
    char path[128];
    DVDEntry* entry;
    IconWork* wp;
    u32 size;

    work[0].count = 32;
    work[0].entries = __memAlloc(HEAP_DEFAULT, sizeof(IconEntry) * work[0].count);
    memset(work[0].entries, 0, sizeof(IconEntry) * work[0].count);

    wp = &work[1];
    wp->count = 32;
    wp->entries = __memAlloc(HEAP_DEFAULT, sizeof(IconEntry) * wp->count);
    memset(wp->entries, 0, sizeof(IconEntry) * wp->count);
    icon_tpl = NULL;
    icon_bin = NULL;
    icon_tpl_ok = FALSE;
    icon_bin_ok = FALSE;

    sprintf(path, "%s/icon.tpl", getMarioStDvdRoot());
    entry = DVDMgrOpen(path, 2, 0);
    if (entry) {
        size = OSRoundUp32B(DVDMgrGetLength(entry));
        icon_tpl = __memAlloc(HEAP_DEFAULT, size);
        entry->info.cb.userData = entry; // this file's userData is a pointer to the entry
        DVDMgrReadAsync(entry, icon_tpl, size, 0, _callback_tpl);
    }

    sprintf(path, "%s/icon.bin", getMarioStDvdRoot());
    entry = DVDMgrOpen(path, 2, 0);
    if (entry) {
        size = OSRoundUp32B(DVDMgrGetLength(entry));
        icon_bin = __memAlloc(HEAP_DEFAULT, size);
        entry->info.cb.userData = entry; // this file's userData is a pointer to the entry
        DVDMgrReadAsync(entry, icon_bin, size, 0, _callback_bin);
    }
}

void iconTexSetup(void) {
    ; // stubbed in retail
}

void iconReInit(void) {
    IconWork* wp = iconGetWork();
    memset(wp->entries, 0, sizeof(IconEntry) * wp->count);
}

void iconMain(void) {
    IconWork* wp;
    int i;
    IconEntry* entry;
    IconBinaryEntry* data;

    wp = iconGetWork();
    if (!icon_tpl_ok || !icon_bin_ok)
        return;

    for (entry = wp->entries, i = 0; i < wp->count; i++, entry++) {
        if (entry->flags & 1) {
            if (entry->flags & 0x2000) {
                entry->flags = entry->flags & ~0x2000;
                // warning: data may be invalid
                entry->width = data->width;
                entry->height = data->height;
                entry->numIcons = data->numIcons;
                entry->currIcon = 0;
                entry->tplId = data->frames[0].tplId;
                entry->frameDelay = data->frames[0].frameDelay;
                if (data->type & 1) {
                    entry->flags = entry->flags | 0x1000;
                }
            }
            if (entry->flags & 0x1000) {
                if (entry->frameDelay) {
                    entry->frameDelay--;
                }
                if (!entry->frameDelay) {
                    entry->currIcon++;
                    if (entry->currIcon >= entry->numIcons) {
                        entry->currIcon = 0;
                    }
                    {
                        u16 id = entry->iconId;
                        IconBinary* bin = icon_bin;
                        IconBinaryEntry* tmp;
                        if (!icon_tpl_ok || !icon_bin_ok) {
                            tmp = NULL;
                        } else {
                            tmp = (IconBinaryEntry*)((u8*)bin + bin->offset[id]);
                        }
                        data = tmp;
                    }
                    entry->tplId = data->frames[entry->currIcon].tplId;
                    entry->frameDelay = data->frames[entry->currIcon].frameDelay;
                }
            }
            if (!(entry->flags & 2)) {
                if (entry->flags & 0x10) {
                    dispEntry(CAMERA_2D, 200.0f, 1, iconDisp, entry);
                } else if (entry->flags & 0x100) {
                    shadowEntry(entry->position.x, entry->position.y, entry->position.z, 10.0f);
                    dispEntry(CAMERA_3D_EFFECTA, dispCalcZ(&entry->position), 1, iconDisp, entry);
                } else if (entry->flags & 0x200) {
                    shadowEntry(entry->position.x, entry->position.y, entry->position.z, 10.0f);
                    dispEntry(CAMERA_3D_EFFECTB, dispCalcZ(&entry->position), 1, iconDisp, entry);
                } else {
                    shadowEntry(entry->position.x, entry->position.y, entry->position.z, 10.0f);
                    if (entry->color.a != 0xFF) {
                        dispEntry(CAMERA_3D, dispCalcZ(&entry->position), 2, iconDisp, entry);
                    } else {
                        dispEntry(CAMERA_3D, dispCalcZ(&entry->position), 1, iconDisp, entry);
                    }
                }
            }
        }
    }
}

void iconEntry(const char* name, u16 iconId) {
    IconWork* wp = iconGetWork();
    IconEntry* entry;
    IconBinaryEntry* data;
    int i;

    for (entry = wp->entries, i = 0; i < wp->count; i++, entry++) {
        if ((entry->flags & 1) && !strcmp(entry->name, name)) {
            break;
        }
    }
    for (entry = wp->entries, i = 0; i < wp->count; i++, entry++) {
        if (!(entry->flags & 1)) { // looking for unused entry to allocate
            break;
        }
    }

    memset(entry, 0, sizeof(IconEntry));
    entry->position = (Vec){ 0.0f, 0.0f, 0.0f };
    entry->scale = 1.0f;
    entry->color = *(GXColor*)&dat_80419778;
    strcpy(entry->name, name);
    entry->iconId = iconId;
    {
        u16 id = entry->iconId;
        IconBinary* bin = icon_bin;
        if (!icon_tpl_ok || !icon_bin_ok) {
            data = NULL;
        } else {
            data = (IconBinaryEntry*)((u8*)bin + bin->offset[id]);
        }
    }
    if (data) {
        entry->width = data->width;
        entry->height = data->height;
        entry->numIcons = data->numIcons;
        entry->currIcon = 0;
        entry->tplId = data->frames[0].tplId;
        entry->frameDelay = data->frames[0].frameDelay;
        if (data->type & 1) {
            entry->flags = entry->flags | 0x1000;
        }
    } else {
        entry->flags = entry->flags | 0x2000;
    }
    entry->flags = entry->flags | 1;
    if (mapGetWork()->entries[0].flags & 2) {
        if (!strncmp(gp->currentMapName, "aji", 3) && evtGetValue(0, GSW(0)) == 0x178) {
            entry->flags = entry->flags & ~0x40;
        } else {
            entry->flags = entry->flags | 0x40;
        }
    }
}

void iconEntry2D(const char* name, s16 iconId) {
    IconWork* wp = iconGetWork();
    IconEntry* entry;
    IconBinaryEntry* data;
    int i;

    for (entry = wp->entries, i = 0; i < wp->count; i++, entry++) {
        if ((entry->flags & 1) && !strcmp(entry->name, name)) {
            break;
        }
    }
    for (entry = wp->entries, i = 0; i < wp->count; i++, entry++) {
        if (!(entry->flags & 1)) { // looking for unused entry to allocate
            break;
        }
    }

    memset(entry, 0, sizeof(IconEntry));
    entry->position = (Vec){ 0.0f, 0.0f, 0.0f };
    entry->scale = 1.0f;
    entry->color = *(GXColor*)&dat_8041977c;
    strcpy(entry->name, name);
    entry->iconId = iconId;
    {
        u16 id = entry->iconId;
        IconBinary* bin = icon_bin;
        if (!icon_tpl_ok || !icon_bin_ok) {
            data = NULL;
        } else {
            data = (IconBinaryEntry*)((u8*)bin + bin->offset[id]);
        }
    }
    if (data) {
        entry->width = data->width;
        entry->height = data->height;
        entry->numIcons = data->numIcons;
        entry->currIcon = 0;
        entry->tplId = data->frames[0].tplId;
        entry->frameDelay = data->frames[0].frameDelay;
        if (data->type & 1) {
            entry->flags = entry->flags | 0x1000;
        }
    } else {
        entry->flags = entry->flags | 0x2000;
    }
    entry->flags = entry->flags | 0x11;
}

void iconDelete(const char* name) {
    IconWork* wp = iconGetWork();
    IconEntry* entry = wp->entries;
    int i;

    for (i = 0; i < wp->count; i++, entry++) {
        if (entry->flags & 1 && !strcmp(entry->name, name)) {
            entry->flags = entry->flags & 0xFFFE; // no longer in use
            break;
        }
    }
}

void iconChange(const char* name, u16 iconId) {
    IconEntry* entry = iconNameToPtr(name); // inline
    IconBinaryEntry* data;

    entry->iconId = iconId;
    {
        u16 id = entry->iconId;
        IconBinary* bin = icon_bin;
        if (!icon_tpl_ok || !icon_bin_ok) {
            data = NULL;
        } else {
            data = (IconBinaryEntry*)((u8*)bin + bin->offset[id]);
        }
    }
    entry->width = data->width;
    entry->height = data->height;
    entry->numIcons = data->numIcons;
    entry->currIcon = 0;
    entry->tplId = data->frames[0].tplId;
    entry->frameDelay = data->frames[0].frameDelay;
    if (data->type & 1) {
        entry->flags = entry->flags | 0x1000;
    }
    entry->flags = entry->flags | 1;
}

void iconDisp(CameraId camId, void* param) {
    IconEntry* entry = param; // cast to correct type
    CameraEntry* camera;
    Mtx trans, scale, rot, mtx;

    camera = camGetPtr(camId);
    MTXTrans(trans, entry->position.x, entry->position.y, entry->position.z);
    MTXScale(scale, entry->scale, entry->scale, entry->scale);
    MTXRotRad(rot, 'y', 0.017453292f * -camera->viewYaw);
    MTXConcat(trans, rot, mtx);
    MTXConcat(mtx, scale, mtx);
    MTXConcat(camera->view, mtx, mtx);
    iconGX(mtx, entry);
}

// dont_inline keeps iconDispGxAlpha out of iconDispGx (retail emits a real `bl`).
#pragma dont_inline on
// The zero-extended icon id and the icon_bin pointer are assigned before the entry's fields
// (as in iconDispGxCol): that pins this block's register allocation (color r3, bin r4, id r5).
void iconDispGxAlpha(Vec* position, s16 flags, s16 iconId, u8 alpha, f32 scale) {
    ICON_FRAMES_DECL
    CameraEntry* camera;
    IconBinaryEntry* data;
    IconEntry entry;
    GXColor color;
    IconEntry* entryp;
    IconBinary* bin;
    u16 id;
    Mtx transmtx, scalemtx, rotmtx, mtx;

    camera = camGetCurPtr();
    entryp = &entry;
    if (!icon_tpl_ok || !icon_bin_ok)
        return;
    MTXTrans(transmtx, position->x, position->y, position->z);
    MTXScale(scalemtx, scale, scale, scale);
    MTXRotRad(rotmtx, 'y', 0.017453292f * -camera->viewYaw);
    MTXConcat(transmtx, rotmtx, mtx);
    MTXConcat(mtx, scalemtx, mtx);
    MTXConcat(camera->view, mtx, mtx);
    color = *(GXColor*)&dat_80419780;
    color.a = alpha;
    entryp->flags = flags;
    entryp->color = color;
    bin = icon_bin;
    id = (u16)iconId;
    entryp->iconId = id;
    {
        if (!icon_tpl_ok || !icon_bin_ok) {
            data = NULL;
        } else {
            data = (IconBinaryEntry*)((u8*)bin + bin->offset[id]);
        }
    }
    entryp->width = data->width;
    entryp->height = data->height;
    entryp->tplId = data->frames[0].tplId;
    entryp->frameDelay = data->frames[0].frameDelay;
    if (data->numIcons > 1) {
        s32 total = 0;
        s32 rest;
        int i;

        ICON_FRAMES_INIT(data)
        for (i = 0; i < data->numIcons; i++) {
            total += ICON_FRAME(data, i).frameDelay;
        }
        rest = (s32)gp->retraceCount % total;
        for (i = 0; i < data->numIcons; i++) {
            if (rest < ICON_FRAME(data, i).frameDelay) {
                entryp->tplId = ICON_FRAME(data, i).tplId;
                break;
            }
            rest -= ICON_FRAME(data, i).frameDelay;
        }
    }
    iconGX(mtx, entryp);
}

#pragma dont_inline reset

void iconDispGx(f32 scale, Vec* position, s16 flags, s16 iconId) {
    iconDispGxAlpha(position, flags, iconId, 0xFF, scale);
}

// Inlining this lookup changes MWCC's register colouring: it puts `data` in r4 (retail's
// register) instead of r6, so keep it inline.
static inline IconBinaryEntry* iconGetBinaryData(IconBinary* bin, u16 id) {
    if (!icon_tpl_ok || !icon_bin_ok) {
        return NULL;
    }
    return (IconBinaryEntry*)((u8*)bin + bin->offset[id]);
}

// NONMATCHING (98.24%): retail keeps the cached numIcons in r8 (one web: the `> 1` test and both loop
// bounds) and the loops' frame pointer in r7. Ours loads `n` into r5 for the test and copies it to r7
// for the loops, with the frame pointer in r8. rasim on a mwccdbg dump: no declaration order, temp
// key or single interference edge gives r8; retail's numIcons web has one fewer neighbour.
// Tried: a dummy `entryp->numIcons` store (same score), n as int/s32/u32/s16, reusing `id`,
// block-scope n, explicit lookup, plain `data->frames[i]`, writes to `entry.`, a frame-selection
// inline helper, opt_lifetimes/propagation/common_subs/strength_reduction/dead_assignments, permuter.
void iconDispGx2(Mtx mtx, s16 flags, s16 iconId) {
    ICON_FRAMES_DECL
    CameraEntry* camera;
    IconBinaryEntry* data;
    IconEntry entry;
    IconEntry* entryp;
    Mtx view;
    IconBinary* bin;
    u16 id;
    u16 n;

    camera = camGetCurPtr();
    entryp = &entry;
    if (!icon_tpl_ok || !icon_bin_ok)
        return;
    entryp->flags = flags;
    entryp->color = *(GXColor*)&dat_80419784;
    bin = icon_bin;
    id = (u16)iconId;
    entryp->iconId = id;
    data = iconGetBinaryData(bin, id);
    entryp->width = data->width;
    entryp->height = data->height;
    entryp->tplId = data->frames[0].tplId;
    entryp->frameDelay = data->frames[0].frameDelay;
    n = data->numIcons;
    if (n > 1) {
        s32 total = 0;
        s32 rest;
        int i;

        ICON_FRAMES_INIT(data)
        for (i = 0; i < n; i++) {
            total += ICON_FRAME(data, i).frameDelay;
        }
        rest = (s32)gp->retraceCount % total;
        for (i = 0; i < n; i++) {
            if (rest < ICON_FRAME(data, i).frameDelay) {
                entryp->tplId = ICON_FRAME(data, i).tplId;
                break;
            }
            rest -= ICON_FRAME(data, i).frameDelay;
        }
    }
    PSMTXConcat(camera->view, mtx, view);
    iconGX(view, entryp);
}

// NONMATCHING (98.17%): same register picture as iconDispGx2 (retail `numIcons` r8, `frames` r7;
// ours puts `n` in r5 plus a copy in r7, and `frames` in r8). See iconDispGx2.
void iconDispGxCol(Mtx pos, s16 flags, s32 iconId, GXColor color) {
    ICON_FRAMES_DECL
    CameraEntry* camera;
    IconBinaryEntry* data;
    IconEntry entry;
    IconEntry* entryp;
    Mtx view;
    IconBinary* bin;
    u16 id;
    u16 n;

    camera = camGetCurPtr();
    entryp = &entry;
    if (!icon_tpl_ok || !icon_bin_ok)
        return;
    entryp->flags = flags;
    entryp->color = color;
    bin = icon_bin;
    id = (u16)iconId;
    entryp->iconId = id;
    data = iconGetBinaryData(bin, id);
    entryp->width = data->width;
    entryp->height = data->height;
    entryp->tplId = data->frames[0].tplId;
    entryp->frameDelay = data->frames[0].frameDelay;
    n = data->numIcons;
    if (n > 1) {
        s32 total = 0;
        s32 rest;
        int i;

        ICON_FRAMES_INIT(data)
        for (i = 0; i < n; i++) {
            total += ICON_FRAME(data, i).frameDelay;
        }
        rest = (s32)gp->retraceCount % total;
        for (i = 0; i < n; i++) {
            if (rest < ICON_FRAME(data, i).frameDelay) {
                entryp->tplId = ICON_FRAME(data, i).tplId;
                break;
            }
            rest -= ICON_FRAME(data, i).frameDelay;
        }
    }
    PSMTXConcat(camera->view, pos, view);
    iconGX(view, entryp);
}

// The frame table is reached through a typed accessor: a cast at each use moves `data` into another register.
// The zero dummy below is a function-local static the original keeps in .sdata (32-aligned), not .sbss.
#pragma explicit_zero_data on
void iconGetTexObj(GXTexObj* texObj, u16 iconId) {
    u16 tplId;
    ICON_FRAMES_DECL
    IconBinaryEntry* data;

    if (!icon_tpl_ok || !icon_bin_ok) {
        static char dummy ATTRIBUTE_ALIGN(32) = '\0';
        GXInitTexObj(texObj, &dummy, 1, 1, GX_TF_I4, GX_CLAMP, GX_CLAMP, GX_FALSE);
        return;
    }
    {
        IconBinary* bin = icon_bin;
        if (!icon_tpl_ok || !icon_bin_ok) {
            data = NULL;
        } else {
            data = (IconBinaryEntry*)((u8*)bin + bin->offset[(u16)iconId]);
        }
    }
    tplId = data->frames[0].tplId;
    if (data->numIcons > 1) {
        s32 total = 0;
        s32 rest;
        int i;

        ICON_FRAMES_INIT(data)
        for (i = 0; i < data->numIcons; i++) {
            total += ICON_FRAME(data, i).frameDelay;
        }
        rest = (s32)gp->retraceCount % total;
        for (i = 0; i < data->numIcons; i++) {
            if (rest < ICON_FRAME(data, i).frameDelay) {
                tplId = ICON_FRAME(data, i).tplId;
                break;
            }
            rest -= ICON_FRAME(data, i).frameDelay;
        }
    }
    TEXGetGXTexObjFromPalette(icon_tpl, texObj, tplId);
    GXInitTexObjLOD(texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f,
                    GX_FALSE, GX_FALSE, GX_ANISO_1);
}
#pragma explicit_zero_data reset

void iconGetWidthHight(u16* width, u16* height, u16 iconId) {
    IconBinaryEntry* entry;
    TEXDescriptor* descriptor;
    IconBinary* bin = icon_bin;

    if (!icon_tpl_ok || !icon_bin_ok) {
        entry = NULL;
    } else {
        entry = (IconBinaryEntry*)((u8*)bin + bin->offset[iconId]);
    }
    *width = entry->width;
    *height = entry->height;
    if (*width == 0 && *height == 0) { // no tpl override
        descriptor = TEXGet(icon_tpl, iconId);
        *width = descriptor->textureHeader->width;
        *height = descriptor->textureHeader->height;
    }
}

// dont_inline keeps the iconGX call in iconDisp/iconDispGx2 (retail emits a real `bl`).
#pragma dont_inline on
// The zero dummy below is a function-local static the original keeps in .sdata (32-aligned), not .sbss.
#pragma explicit_zero_data on
void iconGX(Mtx mtx, IconEntry* entry) {
    Vec position = { 0.0f, 0.0f, 0.0f };
    GXTexObj texObj;
    Mtx scale;
    Mtx mtx2;
    CameraEntry* camera;
    f32 x;
    f32 y;
    f32 left;
    f32 top;
    f32 width;
    f32 bottom;
    f32 height;
    u16 tplId;

    position.x = mtx[0][3];
    position.y = mtx[1][3];
    position.z = mtx[2][3];
    y = (entry->flags & 8) ? -1.0f : 1.0f;
    x = (entry->flags & 4) ? -1.0f : 1.0f;
    PSMTXScale(scale, x, y, 1.0f);
    PSMTXConcat(mtx, scale, mtx2);
    camera = camGetPtr(8);
    if (camGetCurPtr() != camera && (entry->flags & 0x10)) {
        PSMTXScale(scale, 1.0f, -1.0f, 1.0f);
        PSMTXConcat(mtx2, scale, mtx2);
    }
    GXLoadPosMtxImm(mtx2, 0);
    GXSetCurrentMtx(0);
    tplId = entry->tplId;
    if (!icon_tpl_ok || !icon_bin_ok) {
        static char dummy ATTRIBUTE_ALIGN(32) = '\0';
        GXInitTexObj(&texObj, &dummy, 1, 1, GX_TF_I4, GX_CLAMP, GX_CLAMP, GX_FALSE);
    } else {
        TEXGetGXTexObjFromPalette(icon_tpl, &texObj, tplId);
        GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f,
                        GX_FALSE, GX_FALSE, GX_ANISO_1);
    }
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    if (!(entry->flags & 0x10)) {
        mapSetMaterialLight(4, position);
    } else {
        GXSetNumChans(1);
        GXSetChanMatColor(4, dat_80419788);
        GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    }
    if (entry->flags & 0x20) {
        GXSetNumTevStages(3);
        GXSetTevSwapModeTable(1, 0, 0, 0, 3);
        GXSetTevSwapModeTable(2, 1, 1, 1, 3);
        GXSetTevSwapModeTable(3, 2, 2, 2, 3);
        GXSetTevSwapMode(0, 0, 1);
        GXSetTevSwapMode(1, 0, 2);
        GXSetTevSwapMode(2, 0, 3);
        GXSetTevColor(1, dat_8041978c);
        GXSetTevColor(2, entry->color);
        GXSetTevOrder(0, 0, 0, 0xFF);
        GXSetTevColorOp(0, 0, 0, 0, 1, 0);
        GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
        GXSetTevColorIn(0, 0xF, 2, 8, 0xF);
        GXSetTevAlphaIn(0, 7, 7, 7, 4);
        GXSetTevOrder(1, 0, 0, 0xFF);
        GXSetTevColorOp(1, 0, 0, 0, 1, 0);
        GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
        GXSetTevColorIn(1, 0xF, 2, 8, 0);
        GXSetTevAlphaIn(1, 7, 7, 7, 4);
        GXSetTevOrder(2, 0, 0, 0xFF);
        GXSetTevColorOp(2, 0, 0, 0, 1, 0);
        GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
        GXSetTevColorIn(2, 0xF, 2, 8, 0);
        GXSetTevAlphaIn(2, 7, 2, 4, 7);
        GXSetNumTexGens(1);
        GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
    } else {
        GXSetNumTevStages(2);
        GXSetTevOrder(0, 0, 0, 4);
        GXSetTevOp(0, 0);
        GXSetTevSwapMode(0, 0, 0);
        GXSetTevColor(1, entry->color);
        GXSetTevOrder(1, 0xFF, 0xFF, 0xFF);
        GXSetTevColorOp(1, 0, 0, 0, 1, 0);
        GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
        GXSetTevColorIn(1, 0xF, 2, 0, 0xF);
        GXSetTevAlphaIn(1, 7, 1, 0, 7);
        if (!(entry->flags & 0x10) && !(entry->flags & 0x40) && !(entry->flags & 0x80) &&
            (mapGetWork()->entries[0].flags & 2)) {
            GXColor blend;

            mapGetBlend(&blend);
            GXSetTevColor(2, blend);
            GXSetNumTevStages(3);
            GXSetTevOrder(2, 0xFF, 0xFF, 0xFF);
            GXSetTevColorOp(2, 0, 0, 0, 1, 0);
            GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
            GXSetTevColorIn(2, 0xF, 0, 4, 0xF);
            GXSetTevAlphaIn(2, 7, 0, 2, 7);
        }
        GXSetNumTexGens(1);
        GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
    }
    if (!(entry->flags & 0x10)) {
        mapSetMaterialFog();
    } else {
        GXSetFog(0, 0.0f, 0.0f, 0.0f, 0.0f, dat_80419790);
    }
    if (entry->color.a < 0xFF) {
        GXSetBlendMode(1, 4, 5, 0);
        GXSetZCompLoc(1);
        GXSetAlphaCompare(7, 0, 0, 7, 0);
        GXSetZMode(1, 3, 0);
    } else {
        GXSetBlendMode(0, 4, 5, 0);
        GXSetZCompLoc(0);
        GXSetAlphaCompare(6, 0x80, 1, 0, 0);
        GXSetZMode(1, 3, 1);
    }
    if (entry->flags & 0x10) {
        GXSetZMode(0, 3, 0);
    }
    if (entry->flags & 0x400) {
        GXSetCullMode(2);
    } else if (entry->flags & 0x800) {
        GXSetCullMode(1);
    } else {
        GXSetCullMode(0);
    }
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    if (entry->width == 0 && entry->height == 0) {
        width = (f32)GXGetTexObjWidth(&texObj);
        height = (f32)GXGetTexObjHeight(&texObj);
    } else {
        width = (f32)entry->width;
        height = (f32)entry->height;
    }
    top = height;
    bottom = -height;
#ifdef MUST_MATCH
    // MUST_MATCH: the local pins the 0.5 load and the FP register allocation.
    {
        f32 half = 0.5f;
        left = -width * half;
    }
#else
    left = -width * 0.5f;
#endif
    if (!(entry->flags & 0x10)) {
        left *= 0.6f;
        top *= 0.6f;
        width *= 0.6f;
        bottom *= 0.6f;
        if (entry->iconId == 0x18B) {
            left *= 0.8f;
            top *= 0.8f;
            width *= 0.8f;
            bottom *= 0.8f;
        } else if (entry->iconId == 0x18D) {
            left *= 0.9f;
            top *= 0.9f;
            width *= 0.9f;
            bottom *= 0.9f;
        }
    }
    GXBegin(0x80, 0, 4);
    GXPosition3f32(left, top, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXPosition3f32(left + width, top, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(left + width, top + bottom, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(left, top + bottom, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    if (entry->flags & 0x20) {
        GXSetTevSwapMode(0, 0, 0);
        GXSetTevSwapMode(1, 0, 0);
        GXSetTevSwapMode(2, 0, 0);
    }
}
#pragma explicit_zero_data reset
#pragma dont_inline reset

// .sdata digit tables
static u16 suuji[] = { 473, 475, 477, 479, 481, 483, 485, 487, 489, 491 };   // 0-9
u16 suuji_s[] = { 474, 476, 478, 480, 482, 484, 486, 488, 490, 492 }; // 0-9 small

// trailing zero word of the unit's .sdata
// (an all-zero object needs explicit_zero_data to stay in .sdata)
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_8040F3CC_sdata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset

IconEntry* iconNameToPtr(const char* name) {
    IconWork* wp = iconGetWork();
    IconEntry* entry = wp->entries;
    int i;

    for (i = 0; i < wp->count; i++, entry++) {
        if ((entry->flags & 1) && !strcmp(entry->name, name)) {
            return entry;
        }
    }
    return NULL;
}

void iconSetPos(const char* name, f32 x, f32 y, f32 z) {
    IconWork* workptr = iconGetWork();
    IconEntry* entry = iconNameToPtr(name); // inline

    if (entry) {
        Vec temp = { 0 };
        temp.x = x;
        temp.y = y;
        temp.z = z;
        entry->position = temp;
    }
}

void iconFlagOn(const char* name, s16 flags) {
    IconEntry* entry = iconNameToPtr(name); // inline
    if (entry) {
        entry->flags = entry->flags | flags;
    }
}

void iconFlagOff(const char* name, s16 mask) {
    IconEntry* entry = iconNameToPtr(name); // inline
    if (entry) {
        entry->flags = entry->flags & ~mask;
    }
}

void iconSetScale(const char* name, f32 scale) {
    IconEntry* entry = iconNameToPtr(name); // inline
    if (entry) {
        entry->scale = scale;
    }
}

void iconSetAlpha(const char* name, u8 alpha) {
    IconEntry* entry = iconNameToPtr(name); // inline
    if (entry) {
        GXColor color = entry->color;
        color.a = alpha;
        entry->color = color;
    }
}

s32 iconNumberDispGx(Mtx mtx, s32 number, BOOL small, GXColor color) {
    Mtx mtx2;
    BOOL big;
    s32 i;
    s32 count;
    s32 width;
    s32 digits[5];
    s32 rem;
    f32 spacing;

    big = !small;
    spacing = big ? 20.0f : 16.0f;

    rem = number % 100000;
    digits[4] = rem / 10000;
    rem %= 10000;
    digits[3] = rem / 1000;
    rem %= 1000;
    digits[2] = rem / 100;
    rem %= 100;
    digits[1] = rem / 10;
    digits[0] = rem % 10;
    width = 0;
    if (number / 100000 != 0) {
        count = 5;
    } else if (digits[4] != 0) {
        count = 5;
    } else if (digits[3] != 0) {
        count = 4;
    } else if (digits[2] != 0) {
        count = 3;
    } else if (digits[1] != 0) {
        count = 2;
    } else {
        count = 1;
    }
    for (i = 0; i < count; i++) {
        PSMTXTrans(mtx2, (f32)(-i) * spacing, 0.0f, 0.0f);
        PSMTXConcat(mtx, mtx2, mtx2);
        iconDispGxCol(mtx2, 0x10, big ? (s32)suuji[digits[i]] : (s32)suuji_s[digits[i]], color);
        width = (s32)((f32)width + spacing);
    }
    return width;
}

// Same as iconNumberDispGx, but the original declares a 4-element array here and writes/reads
// index 4; the compiler aliases digits[4] with mtx2 (r1+0x1c), which is what makes the offsets match.
s32 iconNumberDispGx3D(Mtx mtx, s32 number, BOOL small, GXColor color) {
    Mtx mtx2;
    BOOL big;
    s32 i;
    s32 count;
    s32 width;
    s32 digits[4];
    s32 rem;
    f32 spacing;

    big = !small;
    spacing = big ? 10.0f : 8.0f;

    rem = number % 100000;
    digits[4] = rem / 10000;
    rem %= 10000;
    digits[3] = rem / 1000;
    rem %= 1000;
    digits[2] = rem / 100;
    rem %= 100;
    digits[1] = rem / 10;
    digits[0] = rem % 10;
    width = 0;
    if (number / 100000 != 0) {
        count = 5;
    } else if (digits[4] != 0) {
        count = 5;
    } else if (digits[3] != 0) {
        count = 4;
    } else if (digits[2] != 0) {
        count = 3;
    } else if (digits[1] != 0) {
        count = 2;
    } else {
        count = 1;
    }
    for (i = 0; i < count; i++) {
        PSMTXTrans(mtx2, (f32)(-i) * spacing, 0.0f, 0.0f);
        PSMTXConcat(mtx, mtx2, mtx2);
        iconDispGxCol(mtx2, 0, big ? (s32)suuji[digits[i]] : (s32)suuji_s[digits[i]], color);
        width = (s32)((f32)width + spacing);
    }
    return width;
}

// ---- .sdata2 ----
// Named 0xFFFFFFFF/0xFFFFFF00/colour words the unit reads by name; the float constants are
// pooled literals emitted in first-use order after these, matching the original's .sdata2.
// explicit_zero_data keeps the all-zero float_0 word out of .sbss.
#pragma explicit_zero_data on
__declspec(section ".sdata2") u32 dat_80419778 = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_8041977c = 0xFFFFFFFF;
__declspec(section ".sdata2") u32 dat_80419780 = 0xFFFFFF00;
__declspec(section ".sdata2") u32 dat_80419784 = 0xFFFFFFFF;
__declspec(section ".sdata2") GXColor dat_80419788 = { 0xFF, 0xFF, 0xFF, 0xFF };
__declspec(section ".sdata2") GXColor dat_8041978c = { 0x55, 0x55, 0x55, 0xFF };
__declspec(section ".sdata2") GXColor dat_80419790 = { 0xFF, 0xFF, 0xFF, 0xFF };
#pragma explicit_zero_data reset
