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


extern GlobalWork* gp;

// .rodata strings and .sdata2 constants this unit references by name in retail
extern const char str_802C15D8[];
extern const char str_802C15E4[];
extern const char str_804197C4[];
extern const f32 float_deg2rad_804197c0;
extern const f32 float_1_804197ac;
extern const f32 float_10_80419794;
extern const f32 float_8_80419798;
extern const f32 float_16_804197a4;
extern const f32 float_20_804197a0;
extern const f32 float_200_804197c8;
extern const f32 float_0_8041979c;
extern u32 dat_80419780;
extern u32 dat_80419778;
extern u32 dat_80419784;
extern Vec vec3_802c1598;

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
                    dispEntry(CAMERA_2D, float_200_804197c8, 1, iconDisp, entry);
                } else if (entry->flags & 0x100) {
                    shadowEntry(entry->position.x, entry->position.y, entry->position.z, float_10_80419794);
                    dispEntry(CAMERA_3D_EFFECTA, dispCalcZ(&entry->position), 1, iconDisp, entry);
                } else if (entry->flags & 0x200) {
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
    entry->scale = float_1_804197ac;
    entry->color = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
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
    entry->scale = float_1_804197ac;
    entry->color = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
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
    MTXRotRad(rot, 'y', float_deg2rad_804197c0 * -camera->viewYaw);
    MTXConcat(trans, rot, mtx);
    MTXConcat(mtx, scale, mtx);
    MTXConcat(camera->view, mtx, mtx);
    iconGX(mtx, entry);
}

// NONMATCHING (98.56%): `data` is in r3 vs r4 in retail; all loop temps shift by one register.
// dont_inline keeps iconDispGxAlpha out of iconDispGx (retail emits a real `bl`).
#pragma dont_inline on
void iconDispGxAlpha(Vec* position, s16 flags, s16 iconId, u8 alpha, f32 scale) {
    ICON_FRAMES_DECL
    CameraEntry* camera;
    IconBinaryEntry* data;
    IconEntry entry;
    GXColor color;
    IconEntry* entryp = &entry;
    Mtx transmtx, scalemtx, rotmtx, mtx;

    camera = camGetCurPtr();
    if (!icon_tpl_ok || !icon_bin_ok)
        return;
    MTXTrans(transmtx, position->x, position->y, position->z);
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

void iconDispGx(Vec* position, s16 flags, s16 iconId, f32 scale) {
    iconDispGxAlpha(position, flags, iconId, 0xFF, scale);
}

// NONMATCHING (96.00%): register allocation of data/loop temps and the branch layout differ.
void iconDispGx2(Mtx mtx, s16 flags, s16 iconId) {
    ICON_FRAMES_DECL
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
    PSMTXConcat(camera->view, mtx, view);
    iconGX(view, entryp);
}

// NONMATCHING (95.50%): register allocation of data/loop temps and the branch layout differ.
void iconDispGxCol(Mtx pos, s16 flags, s32 iconId, GXColor color) {
    ICON_FRAMES_DECL
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
    PSMTXConcat(camera->view, pos, view);
    iconGX(view, entryp);
}

// The frame table is reached through a typed accessor: a cast at each use moves `data` into another register.
void iconGetTexObj(GXTexObj* texObj, u16 iconId) {
    u16 tplId;
    ICON_FRAMES_DECL
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
    GXInitTexObjLOD(texObj, GX_LINEAR, GX_LINEAR, float_0_8041979c, float_0_8041979c, float_0_8041979c,
                    GX_FALSE, GX_FALSE, GX_ANISO_1);
}

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

// NONMATCHING (92.1%): the digit table, division and loop are the same; the temp GXColor copy is emitted after the
// icon-id select (retail before it) and the callee-saved registers are assigned in another order.
s32 iconNumberDispGx(Mtx mtx, s32 number, BOOL small, GXColor color) {
    BOOL big;
    s32 width;
    s32 i;
    s32 id;
    s32 digits[5];
    s32 rem;
    f32 spacing;
    Mtx mtx2;
    s32 count;

    width = 0;
    big = (small == 0);
    spacing = big ? float_20_804197a0 : float_16_804197a4;

    rem = number % 100000;
    digits[4] = rem / 10000;
    rem %= 10000;
    digits[3] = rem / 1000;
    rem %= 1000;
    digits[2] = rem / 100;
    rem %= 100;
    digits[1] = rem / 10;
    digits[0] = rem % 10;
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
        if (big) {
            id = suuji[digits[i]];
        } else {
            id = suuji_s[digits[i]];
        }
        iconDispGxCol(mtx2, 0x10, id, color);
        width = (s32)((f32)width + spacing);
    }
    return width;
}

// NONMATCHING (92.09%): same as iconNumberDispGx (spacing 10/8, flags 0).
s32 iconNumberDispGx3D(Mtx mtx, s32 number, BOOL small, GXColor color) {
    BOOL big;
    s32 width;
    s32 i;
    s32 id;
    s32 digits[5];
    s32 rem;
    f32 spacing;
    Mtx mtx2;
    s32 count;

    width = 0;
    big = (small == 0);
    spacing = big ? float_10_80419794 : float_8_80419798;

    rem = number % 100000;
    digits[4] = rem / 10000;
    rem %= 10000;
    digits[3] = rem / 1000;
    rem %= 1000;
    digits[2] = rem / 100;
    rem %= 100;
    digits[1] = rem / 10;
    digits[0] = rem % 10;
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
        if (big) {
            id = suuji[digits[i]];
        } else {
            id = suuji_s[digits[i]];
        }
        iconDispGxCol(mtx2, 0, id, color);
        width = (s32)((f32)width + spacing);
    }
    return width;
}
