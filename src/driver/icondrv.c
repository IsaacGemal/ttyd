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


extern GlobalWork* gp;

// .rodata strings and .sdata2 constants this unit references by name in retail
extern const char str_802C15D8[];
extern const char str_802C15E4[];
extern const char str_804197C4[];
extern const f32 float_deg2rad_804197c0;
extern const f32 float_1_804197ac;
extern const f32 float_10_80419794;
extern const f32 float_200_804197c8;
extern const f32 float_0_8041979c;
extern u32 dat_80419780;
extern u32 dat_80419778;
extern u32 dat_80419784;
extern Vec vec3_802c1598;

// Animated icons store a packed {u16 tplId; u16 frameDelay} frame table starting
// 8 bytes into the IconBinaryEntry; u16 data[][2] layout, 4 bytes per frame.
typedef struct IconFrame {
    u16 tplId;
    u16 frameDelay;
} IconFrame;

static u8 dummy_tex[1] = { 0 };

//.sbss
TEXPalette* icon_tpl;
IconBinary* icon_bin;
BOOL icon_tpl_ok;
BOOL icon_bin_ok;
static IconWork work[2];

//.sdata
u16 suuji[] = { 473, 475, 477, 479, 481, 483, 485, 487, 489, 491 };   // 0-9
u16 suuji_s[] = { 474, 476, 478, 480, 482, 484, 486, 488, 490, 492 }; // 0-9 small

// local prototypes
void _callback_tpl(s32 error, DVDFileInfo* info);
void _callback_bin(s32 error, DVDFileInfo* info);
void iconDisp(CameraId camId, void* param);
void iconGX(Mtx mtx, IconEntry* entry);
void iconGetWidthHight(u16* width, u16* height, u16 iconId);
void shadowEntry(f32 x, f32 y, f32 z, f32 size);

// icondrv.h declares IconEntry.flags as s16, but the retail code always loads it
// zero-extended (lhz) and does 16-bit read-modify-writes on it, so the field is
// really u16. Reinterpret the field for MUST_MATCH; report this header bug.
#ifdef MUST_MATCH
#define ICON_FLAGS(e) (*(u16*)&(e)->flags)
#define ICON_ID(e) (*(u16*)&(e)->iconId)
#else
#define ICON_FLAGS(e) ((u16)(e)->flags)
#define ICON_ID(e) ((u16)(e)->iconId)
#endif

// TODO: inline function? I see nothing in the symbol map
#define iconGetWork() (gp->inBattle ? &work[1] : &work[0])

// TODO: remove? no function in original symbol map
inline IconEntry* iconGetEntry(const char* name) {
    IconWork* wp = iconGetWork();
    IconEntry* entry = wp->entries;
    int i;

    for (i = 0; i < wp->count; i++, entry++) {
        if (ICON_FLAGS(entry) & 1 && !strcmp(entry->name, name)) {
            return entry;
        }
    }
    return NULL;
}

void _callback_tpl(s32 error, DVDFileInfo* info) {
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

    sprintf(path, str_802C15D8, getMarioStDvdRoot());
    entry = DVDMgrOpen(path, 2, 0);
    if (entry) {
        size = OSRoundUp32B(DVDMgrGetLength(entry));
        icon_tpl = __memAlloc(HEAP_DEFAULT, size);
        entry->info.cb.userData = entry; // this file's userData is a pointer to the entry
        DVDMgrReadAsync(entry, icon_tpl, size, 0, _callback_tpl);
    }

    sprintf(path, str_802C15E4, getMarioStDvdRoot());
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
        if (ICON_FLAGS(entry) & 1) {
            if (ICON_FLAGS(entry) & 0x2000) {
                entry->flags = ICON_FLAGS(entry) & ~0x2000;
                // warning: data may be invalid
                entry->width = data->width;
                entry->height = data->height;
                entry->numIcons = data->numIcons;
                entry->currIcon = 0;
                entry->tplId = data->tplId;
                entry->frameDelay = data->frameDelay;
                if (data->type & 1) {
                    entry->flags = ICON_FLAGS(entry) | 0x1000;
                }
            }
            if (ICON_FLAGS(entry) & 0x1000) {
                if (entry->frameDelay) {
                    entry->frameDelay--;
                }
                if (!entry->frameDelay) {
                    entry->currIcon++;
                    if (entry->currIcon >= entry->numIcons) {
                        entry->currIcon = 0;
                    }
                    {
                        u16 id = ICON_ID(entry);
                        IconBinary* bin = icon_bin;
                        IconBinaryEntry* tmp;
                        if (!icon_tpl_ok || !icon_bin_ok) {
                            tmp = NULL;
                        } else {
                            tmp = (IconBinaryEntry*)((u8*)bin + bin->offset[id]);
                        }
                        data = tmp;
                    }
                    entry->tplId = ((IconFrame*)data)[entry->currIcon + 2].tplId;
                    entry->frameDelay = ((IconFrame*)data)[entry->currIcon + 2].frameDelay;
                }
            }
            if (!(ICON_FLAGS(entry) & 2)) {
                if (ICON_FLAGS(entry) & 0x10) {
                    dispEntry(CAMERA_2D, float_200_804197c8, 1, iconDisp, entry);
                } else if (ICON_FLAGS(entry) & 0x100) {
                    shadowEntry(entry->position.x, entry->position.y, entry->position.z, float_10_80419794);
                    dispEntry(CAMERA_3D_EFFECTA, dispCalcZ(&entry->position), 1, iconDisp, entry);
                } else if (ICON_FLAGS(entry) & 0x200) {
                    shadowEntry(entry->position.x, entry->position.y, entry->position.z, float_10_80419794);
                    dispEntry(CAMERA_3D_EFFECTB, dispCalcZ(&entry->position), 1, iconDisp, entry);
                } else {
                    shadowEntry(entry->position.x, entry->position.y, entry->position.z, float_10_80419794);
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
        if ((ICON_FLAGS(entry) & 1) && !strcmp(entry->name, name)) {
            break;
        }
    }
    for (entry = wp->entries, i = 0; i < wp->count; i++, entry++) {
        if (!(ICON_FLAGS(entry) & 1)) { // looking for unused entry to allocate
            break;
        }
    }

    memset(entry, 0, sizeof(IconEntry));
    entry->position = (Vec){ 0.0f, 0.0f, 0.0f };
    entry->scale = float_1_804197ac;
    entry->color = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
    strcpy(entry->name, name);
    entry->iconId = iconId;
    {
        u16 id = ICON_ID(entry);
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
        entry->tplId = data->tplId;
        entry->frameDelay = data->frameDelay;
        if (data->type & 1) {
            entry->flags = ICON_FLAGS(entry) | 0x1000;
        }
    } else {
        entry->flags = ICON_FLAGS(entry) | 0x2000;
    }
    entry->flags = ICON_FLAGS(entry) | 1;
    if (mapGetWork()->entries[0].flags & 2) {
        if (!strncmp(gp->currentMapName, "aji", 3) && evtGetValue(0, GSW(0)) == 0x178) {
            entry->flags = ICON_FLAGS(entry) & ~0x40;
        } else {
            entry->flags = ICON_FLAGS(entry) | 0x40;
        }
    }
}

void iconEntry2D(const char* name, s16 iconId) {
    IconWork* wp = iconGetWork();
    IconEntry* entry;
    IconBinaryEntry* data;
    int i;

    for (entry = wp->entries, i = 0; i < wp->count; i++, entry++) {
        if ((ICON_FLAGS(entry) & 1) && !strcmp(entry->name, name)) {
            break;
        }
    }
    for (entry = wp->entries, i = 0; i < wp->count; i++, entry++) {
        if (!(ICON_FLAGS(entry) & 1)) { // looking for unused entry to allocate
            break;
        }
    }

    memset(entry, 0, sizeof(IconEntry));
    entry->position = (Vec){ 0.0f, 0.0f, 0.0f };
    entry->scale = float_1_804197ac;
    entry->color = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
    strcpy(entry->name, name);
    entry->iconId = iconId;
    {
        u16 id = ICON_ID(entry);
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
        entry->tplId = data->tplId;
        entry->frameDelay = data->frameDelay;
        if (data->type & 1) {
            entry->flags = ICON_FLAGS(entry) | 0x1000;
        }
    } else {
        entry->flags = ICON_FLAGS(entry) | 0x2000;
    }
    entry->flags = ICON_FLAGS(entry) | 0x11;
}

void iconDelete(const char* name) {
    IconWork* wp = iconGetWork();
    IconEntry* entry = wp->entries;
    int i;

    for (i = 0; i < wp->count; i++, entry++) {
        if (ICON_FLAGS(entry) & 1 && !strcmp(entry->name, name)) {
            entry->flags = ICON_FLAGS(entry) & 0xFFFE; // no longer in use
            break;
        }
    }
}

void iconChange(const char* name, u16 iconId) {
    IconEntry* entry = iconNameToPtr(name); // inline
    IconBinaryEntry* data;

    entry->iconId = iconId;
    {
        u16 id = ICON_ID(entry);
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
    entry->tplId = data->tplId;
    entry->frameDelay = data->frameDelay;
    if (data->type & 1) {
        entry->flags = ICON_FLAGS(entry) | 0x1000;
    }
    entry->flags = ICON_FLAGS(entry) | 1;
}

void iconDisp(CameraId camId, void* param) {
    IconEntry* entry = param; // cast to correct type
    CameraEntry* camera;
    Mtx trans, scale, rot, mtx;

    camera = camGetPtr(camId);
    MTXTrans(trans, entry->position.x, entry->position.y, entry->position.z);
    MTXScale(scale, entry->scale, entry->scale, entry->scale);
    MTXRotRad(rot, 'y', float_deg2rad_804197c0 * -camera->viewYaw);
    MTXConcat(trans, rot, mtx);
    MTXConcat(mtx, scale, mtx);
    MTXConcat(camera->view, mtx, mtx);
    iconGX(mtx, entry);
}

// NONMATCHING (92.37%): `data` is in r3 vs r4 in retail; all loop temps shift by one register.
// dont_inline keeps iconDispGxAlpha out of iconDispGx (retail emits a real `bl`).
#pragma dont_inline on
void iconDispGxAlpha(Vec position, s16 flags, s16 iconId, u8 alpha, f32 scale) {
    CameraEntry* camera;
    IconBinaryEntry* data;
    IconEntry entry;
    GXColor color;
    IconEntry* entryp = &entry;
    Mtx transmtx, scalemtx, rotmtx, mtx;

    camera = camGetCurPtr();
    if (!icon_tpl_ok || !icon_bin_ok)
        return;
    MTXTrans(transmtx, position.x, position.y, position.z);
    MTXScale(scalemtx, scale, scale, scale);
    MTXRotRad(rotmtx, 'y', float_deg2rad_804197c0 * -camera->viewYaw);
    MTXConcat(transmtx, rotmtx, mtx);
    MTXConcat(mtx, scalemtx, mtx);
    MTXConcat(camera->view, mtx, mtx);
    color = *(GXColor*)&dat_80419780;
    color.a = alpha;
    entryp->flags = flags;
    entryp->color = color;
    entryp->iconId = iconId;
    {
        u16 id = (u16)iconId;
        IconBinary* bin = icon_bin;
        if (!icon_tpl_ok || !icon_bin_ok) {
            data = NULL;
        } else {
            data = (IconBinaryEntry*)((u8*)bin + bin->offset[id]);
        }
    }
    entryp->width = data->width;
    entryp->height = data->height;
    entryp->tplId = data->tplId;
    entryp->frameDelay = data->frameDelay;
    if (data->numIcons > 1) {
        s32 total = 0;
        s32 rest;
        int i;

        for (i = 0; i < data->numIcons; i++) {
            total += ((IconFrame*)data)[i + 2].frameDelay;
        }
        rest = (s32)gp->retraceCount % total;
        for (i = 0; i < data->numIcons; i++) {
            if (rest < ((IconFrame*)data)[i + 2].frameDelay) {
                entryp->tplId = ((IconFrame*)data)[i + 2].tplId;
                break;
            }
            rest -= ((IconFrame*)data)[i + 2].frameDelay;
        }
    }
    iconGX(mtx, entryp);
}

#pragma dont_inline reset

// NONMATCHING (21.78%): the header takes `Vec` by value but retail has `Vec*`; ours copies
// the struct before calling iconDispGxAlpha, retail forwards r3 unchanged.
void iconDispGx(Vec position, s16 flags, s16 iconId, f32 scale) {
    iconDispGxAlpha(position, flags, iconId, 0xFF, scale);
}

// NONMATCHING (84.95%): register allocation of data/loop temps and the branch layout differ.
void iconDispGx2(Mtx mtx, s16 flags, s16 iconId) {
    CameraEntry* camera;
    IconBinaryEntry* data;
    IconEntry entry;
    IconEntry* entryp = &entry;
    Mtx view;

    camera = camGetCurPtr();
    if (!icon_tpl_ok || !icon_bin_ok)
        return;
    entryp->flags = flags;
    entryp->color = *(GXColor*)&dat_80419784;
    entryp->iconId = iconId;
    {
        u16 id = (u16)iconId;
        IconBinary* bin = icon_bin;
        if (!icon_tpl_ok || !icon_bin_ok) {
            data = NULL;
        } else {
            data = (IconBinaryEntry*)((u8*)bin + bin->offset[id]);
        }
    }
    entryp->width = data->width;
    entryp->height = data->height;
    entryp->tplId = data->tplId;
    entryp->frameDelay = data->frameDelay;
    if (data->numIcons > 1) {
        s32 total = 0;
        s32 rest;
        int i;

        for (i = 0; i < data->numIcons; i++) {
            total += ((IconFrame*)data)[i + 2].frameDelay;
        }
        rest = (s32)gp->retraceCount % total;
        for (i = 0; i < data->numIcons; i++) {
            if (rest < ((IconFrame*)data)[i + 2].frameDelay) {
                entryp->tplId = ((IconFrame*)data)[i + 2].tplId;
                break;
            }
            rest -= ((IconFrame*)data)[i + 2].frameDelay;
        }
    }
    PSMTXConcat(camera->view, mtx, view);
    iconGX(view, entryp);
}

// NONMATCHING (78.92%): register allocation of data/loop temps and the branch layout differ.
void iconDispGxCol(Mtx pos, s16 flags, s32 iconId, GXColor color) {
    CameraEntry* camera;
    IconBinaryEntry* data;
    IconEntry entry;
    IconEntry* entryp = &entry;
    Mtx view;

    camera = camGetCurPtr();
    if (!icon_tpl_ok || !icon_bin_ok)
        return;
    entryp->flags = flags;
    entryp->color = color;
    entryp->iconId = iconId;
    {
        u16 id = (u16)iconId;
        IconBinary* bin = icon_bin;
        if (!icon_tpl_ok || !icon_bin_ok) {
            data = NULL;
        } else {
            data = (IconBinaryEntry*)((u8*)bin + bin->offset[id]);
        }
    }
    entryp->width = data->width;
    entryp->height = data->height;
    entryp->tplId = data->tplId;
    entryp->frameDelay = data->frameDelay;
    if (data->numIcons > 1) {
        s32 total = 0;
        s32 rest;
        int i;

        for (i = 0; i < data->numIcons; i++) {
            total += ((IconFrame*)data)[i + 2].frameDelay;
        }
        rest = (s32)gp->retraceCount % total;
        for (i = 0; i < data->numIcons; i++) {
            if (rest < ((IconFrame*)data)[i + 2].frameDelay) {
                entryp->tplId = ((IconFrame*)data)[i + 2].tplId;
                break;
            }
            rest -= ((IconFrame*)data)[i + 2].frameDelay;
        }
    }
    PSMTXConcat(camera->view, pos, view);
    iconGX(view, entryp);
}

// NONMATCHING (93.22%): `data` lands in r7 in ours vs r4 in retail (the loop temps shift by
// three registers); the `dummy$651` shown as `dummy_tex` is a local 1-byte .sdata object.
void iconGetTexObj(GXTexObj* texObj, s16 iconId) {
    u16 tplId;
    IconBinaryEntry* data;

    if (!icon_tpl_ok || !icon_bin_ok) {
        GXInitTexObj(texObj, dummy_tex, 1, 1, GX_TF_I4, GX_CLAMP, GX_CLAMP, GX_FALSE);
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
    tplId = data->tplId;
    if (data->numIcons > 1) {
        s32 total = 0;
        s32 rest;
        int i;

        for (i = 0; i < data->numIcons; i++) {
            total += ((IconFrame*)data)[i + 2].frameDelay;
        }
        rest = (s32)gp->retraceCount % total;
        for (i = 0; i < data->numIcons; i++) {
            if (rest < ((IconFrame*)data)[i + 2].frameDelay) {
                tplId = ((IconFrame*)data)[i + 2].tplId;
                break;
            }
            rest -= ((IconFrame*)data)[i + 2].frameDelay;
        }
    }
    TEXGetGXTexObjFromPalette(icon_tpl, texObj, tplId);
    GXInitTexObjLOD(texObj, GX_LINEAR, GX_LINEAR, float_0_8041979c, float_0_8041979c, float_0_8041979c,
                    GX_FALSE, GX_FALSE, GX_ANISO_1);
}

// retail symbol is spelled "iconGetWidthHight"; the header's "iconGetWidthHeight" is a typo.
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

// NONMATCHING (0.16%): GX batch setup not written yet (retail is 2468 bytes).
// dont_inline keeps the iconGX call in iconDisp/iconDispGx2 (retail emits a real `bl`).
#pragma dont_inline on
void iconGX(Mtx mtx, IconEntry* entry) {
}
#pragma dont_inline reset

IconEntry* iconNameToPtr(const char* name) {
    IconWork* wp = iconGetWork();
    IconEntry* entry = wp->entries;
    int i;

    for (i = 0; i < wp->count; i++, entry++) {
        if ((ICON_FLAGS(entry) & 1) && !strcmp(entry->name, name)) {
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
        entry->flags = ICON_FLAGS(entry) | flags;
    }
}

void iconFlagOff(const char* name, s16 mask) {
    IconEntry* entry = iconNameToPtr(name); // inline
    if (entry) {
        entry->flags = ICON_FLAGS(entry) & ~mask;
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

f32 iconNumberDispGx(Mtx mtx, s32 number, BOOL small, GXColor color) {
}

f32 iconNumberDispGx3D(Mtx mtx, s32 number, BOOL small, GXColor color) {
}
