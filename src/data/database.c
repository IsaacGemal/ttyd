#include "data/database.h"
#include "data/mapdata.h"
#include "data/npc_data.h"
#include "driver/animdrv.h"
#include "driver/arcdrv.h"
#include "manager/dvdmgr.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mariost.h"
#include "memory.h"
#include "parse.h"
#include "system.h"
#include <stdio.h>
#include <string.h>

// Data objects defined later in this file or in other units, declared before their first use.
extern const char zero_8041a6b8[4];
extern const char str_dir[8];
extern const f32 float_0_8041a6c4;
extern const char str_Npc[8];


// clang-format off
// TODO: type
u32 _map_attr_data[597] = {
    (u32)"tik_00", 0x00000010, 0x00000007, (u32)"tik_01",
    0x00000010, 0x00000007, (u32)"tik_02", 0x00000010,
    0x00000007, (u32)"tik_03", 0x00000010, 0x00000007,
    (u32)"tik_04", 0x00000010, 0x00000007, (u32)"tik_05",
    0x00000010, 0x00000007, (u32)"tik_06", 0x00000010,
    0x00000007, (u32)"tik_07", 0x00000010, 0x00000007,
    (u32)"tik_08", 0x00000010, 0x00000007, (u32)"tik_09",
    0x00000010, 0x00000007, (u32)"tik_10", 0x00000010,
    0x00000007, (u32)"tik_11", 0x00000010, 0x00000007,
    (u32)"tik_12", 0x00000010, 0x00000007, (u32)"tik_13",
    0x00000010, 0x00000007, (u32)"tik_14", 0x00000010,
    0x00000007, (u32)"tik_15", 0x00000010, 0x00000007,
    (u32)"tik_16", 0x00000010, 0x00000007, (u32)"tik_17",
    0x00000010, 0x00000007, (u32)"tik_18", 0x00000010,
    0x00000007, (u32)"tik_19", 0x00000010, 0x00000007,
    (u32)"tik_20", 0x00000010, 0x00000007, (u32)"tik_21",
    0x00000010, 0x00000007, (u32)"hei_00", 0x00000001,
    0x00000005, (u32)"hei_01", 0x00000001, 0x00000005,
    (u32)"hei_02", 0x00000001, 0x00000005, (u32)"hei_03",
    0x00000001, 0x00000005, (u32)"hei_04", 0x00000001,
    0x00000005, (u32)"hei_05", 0x00000001, 0x00000005,
    (u32)"hei_06", 0x00000001, 0x00000005, (u32)"hei_07",
    0x00000001, 0x00000005, (u32)"hei_08", 0x00000005,
    0x00000007, (u32)"hei_09", 0x00000005, 0x00000007,
    (u32)"hei_10", 0x00000005, 0x00000007, (u32)"hei_11",
    0x00000005, 0x00000007, (u32)"hei_12", 0x00000005,
    0x00000007, (u32)"hei_13", 0x00000001, 0x00000005,
    (u32)"gon_00", 0x00000002, 0x00000007, (u32)"gon_01",
    0x00000002, 0x00000007, (u32)"gon_02", 0x00000002,
    0x00000007, (u32)"gon_03", 0x00000002, 0x00000007,
    (u32)"gon_04", 0x00000002, 0x00000007, (u32)"gon_05",
    0x00000002, 0x00000007, (u32)"gon_06", 0x00000002,
    0x00000007, (u32)"gon_07", 0x00000002, 0x00000007,
    (u32)"gon_08", 0x00000002, 0x00000007, (u32)"gon_09",
    0x00000002, 0x00000007, (u32)"gon_10", 0x00000002,
    0x00000007, (u32)"gon_11", 0x00000002, 0x00000007,
    (u32)"gon_12", 0x00000002, 0x00000007, (u32)"gon_13",
    0x00000002, 0x00000007, (u32)"win_00", 0x00000003,
    0x00000005, (u32)"win_01", 0x00000003, 0x00000005,
    (u32)"win_02", 0x00000003, 0x00000005, (u32)"win_03",
    0x00000003, 0x00000005, (u32)"win_04", 0x00000003,
    0x00000005, (u32)"win_05", 0x00000003, 0x00000005,
    (u32)"win_06", 0x00000003, 0x00000005, (u32)"mri_00",
    0x00000003, 0x00000005, (u32)"mri_01", 0x00000004,
    0x00000007, (u32)"mri_02", 0x00000004, 0x00000007,
    (u32)"mri_03", 0x00000004, 0x00000007, (u32)"mri_04",
    0x00000004, 0x00000007, (u32)"mri_05", 0x00000004,
    0x00000007, (u32)"mri_06", 0x00000004, 0x00000007,
    (u32)"mri_07", 0x00000004, 0x00000007, (u32)"mri_08",
    0x00000004, 0x00000007, (u32)"mri_09", 0x00000004,
    0x00000007, (u32)"mri_10", 0x00000004, 0x00000007,
    (u32)"mri_11", 0x00000004, 0x00000007, (u32)"mri_12",
    0x00000004, 0x00000007, (u32)"mri_13", 0x00000004,
    0x00000007, (u32)"mri_14", 0x00000004, 0x00000007,
    (u32)"mri_15", 0x00000004, 0x00000007, (u32)"mri_16",
    0x00000004, 0x00000007, (u32)"mri_17", 0x00000004,
    0x00000007, (u32)"mri_18", 0x00000004, 0x00000007,
    (u32)"mri_19", 0x00000004, 0x00000007, (u32)"mri_20",
    0x00000004, 0x00000007, (u32)"gra_00", 0x00000006,
    0x00000005, (u32)"gra_01", 0x00000006, 0x00000005,
    (u32)"gra_02", 0x00000006, 0x00000005, (u32)"gra_03",
    0x00000006, 0x00000005, (u32)"gra_04", 0x00000006,
    0x00000005, (u32)"gra_05", 0x00000006, 0x00000005,
    (u32)"gra_06", 0x00000006, 0x00000005, (u32)"jin_00",
    0x00000007, 0x00000007, (u32)"jin_01", 0x00000007,
    0x00000007, (u32)"jin_02", 0x00000007, 0x00000007,
    (u32)"jin_03", 0x00000007, 0x00000007, (u32)"jin_04",
    0x00000007, 0x00000007, (u32)"jin_05", 0x00000007,
    0x00000007, (u32)"jin_06", 0x00000007, 0x00000007,
    (u32)"jin_07", 0x00000007, 0x00000007, (u32)"jin_08",
    0x00000007, 0x00000007, (u32)"jin_09", 0x00000007,
    0x00000007, (u32)"jin_10", 0x00000007, 0x00000007,
    (u32)"jin_11", 0x00000007, 0x00000007, (u32)"muj_00",
    0x00000008, 0x00000005, (u32)"muj_01", 0x00000008,
    0x00000005, (u32)"muj_02", 0x00000008, 0x00000005,
    (u32)"muj_03", 0x00000008, 0x00000005, (u32)"muj_04",
    0x00000008, 0x00000005, (u32)"muj_05", 0x00000008,
    0x00000005, (u32)"muj_10", 0x00000009, 0x00000007,
    (u32)"muj_11", 0x00000009, 0x00000007, (u32)"muj_12",
    0x00000009, 0x00000007, (u32)"dou_00", 0x00000009,
    0x00000007, (u32)"dou_01", 0x00000009, 0x00000007,
    (u32)"dou_02", 0x00000009, 0x00000007, (u32)"dou_03",
    0x00000009, 0x00000007, (u32)"dou_04", 0x00000009,
    0x00000007, (u32)"dou_05", 0x00000009, 0x00000007,
    (u32)"dou_06", 0x00000009, 0x00000007, (u32)"dou_07",
    0x00000009, 0x00000007, (u32)"dou_08", 0x00000009,
    0x00000007, (u32)"dou_09", 0x00000009, 0x00000007,
    (u32)"dou_10", 0x00000009, 0x00000007, (u32)"dou_11",
    0x00000009, 0x00000007, (u32)"dou_12", 0x00000009,
    0x00000007, (u32)"dou_13", 0x00000009, 0x00000007,
    (u32)"eki_00", 0x0000000A, 0x00000007, (u32)"eki_01",
    0x0000000A, 0x00000007, (u32)"eki_02", 0x0000000A,
    0x00000007, (u32)"eki_03", 0x0000000A, 0x00000007,
    (u32)"eki_04", 0x0000000A, 0x00000007, (u32)"eki_05",
    0x0000000A, 0x00000007, (u32)"eki_06", 0x0000000A,
    0x00000007, (u32)"pik_00", 0x00000011, 0x00000007,
    (u32)"pik_01", 0x00000011, 0x00000007, (u32)"pik_02",
    0x00000011, 0x00000007, (u32)"pik_03", 0x00000011,
    0x00000007, (u32)"pik_04", 0x00000011, 0x00000007,
    (u32)"bom_03", 0x0000000B, 0x00000005, (u32)"bom_04",
    0x0000000B, 0x00000005, (u32)"eki_00", 0x0000000C,
    0x00000007, (u32)"eki_01", 0x0000000C, 0x00000007,
    (u32)"eki_02", 0x0000000C, 0x00000007, (u32)"eki_03",
    0x0000000C, 0x00000007, (u32)"eki_04", 0x0000000C,
    0x00000007, (u32)"eki_05", 0x0000000C, 0x00000007,
    (u32)"eki_06", 0x0000000C, 0x00000007, (u32)"moo_00",
    0x0000000D, 0x00000005, (u32)"moo_01", 0x0000000D,
    0x00000005, (u32)"moo_02", 0x0000000D, 0x00000005,
    (u32)"moo_05", 0x0000000D, 0x00000005, (u32)"moo_06",
    0x0000000D, 0x00000005, (u32)"moo_07", 0x0000000D,
    0x00000005, (u32)"aji_00", 0x0000000E, 0x00000007,
    (u32)"aji_01", 0x0000000E, 0x00000007, (u32)"aji_02",
    0x0000000E, 0x00000007, (u32)"aji_03", 0x0000000E,
    0x00000007, (u32)"aji_04", 0x0000000E, 0x00000007,
    (u32)"aji_05", 0x0000000E, 0x00000007, (u32)"aji_06",
    0x0000000E, 0x00000007, (u32)"aji_07", 0x0000000E,
    0x00000007, (u32)"aji_08", 0x0000000E, 0x00000007,
    (u32)"aji_09", 0x0000000E, 0x00000007, (u32)"aji_10",
    0x0000000E, 0x00000007, (u32)"aji_11", 0x0000000E,
    0x00000007, (u32)"aji_12", 0x0000000E, 0x00000007,
    (u32)"aji_13", 0x0000000E, 0x00000007, (u32)"aji_14",
    0x0000000E, 0x00000007, (u32)"aji_15", 0x0000000E,
    0x00000007, (u32)"aji_16", 0x0000000E, 0x00000007,
    (u32)"aji_17", 0x0000000E, 0x00000007, (u32)"aji_18",
    0x0000000E, 0x00000007, (u32)"aji_19", 0x0000000E,
    0x00000007, (u32)"las_00", 0x0000000F, 0x00000007,
    (u32)"las_01", 0x0000000F, 0x00000007, (u32)"las_02",
    0x0000000F, 0x00000007, (u32)"las_03", 0x0000000F,
    0x00000007, (u32)"las_04", 0x0000000F, 0x00000007,
    (u32)"las_05", 0x0000000F, 0x00000007, (u32)"las_06",
    0x0000000F, 0x00000007, (u32)"las_07", 0x0000000F,
    0x00000007, (u32)"las_08", 0x0000000F, 0x00000007,
    (u32)"las_09", 0x0000000F, 0x00000007, (u32)"las_10",
    0x0000000F, 0x00000007, (u32)"las_11", 0x0000000F,
    0x00000007, (u32)"las_12", 0x0000000F, 0x00000007,
    (u32)"las_13", 0x0000000F, 0x00000007, (u32)"las_14",
    0x0000000F, 0x00000007, (u32)"las_15", 0x0000000F,
    0x00000007, (u32)"las_16", 0x0000000F, 0x00000007,
    (u32)"las_17", 0x0000000F, 0x00000007, (u32)"las_18",
    0x0000000F, 0x00000007, (u32)"las_19", 0x0000000F,
    0x00000007, (u32)"las_20", 0x0000000F, 0x00000007,
    (u32)"las_21", 0x0000000F, 0x00000007, (u32)"las_22",
    0x0000000F, 0x00000007, (u32)"las_23", 0x0000000F,
    0x00000007, (u32)"las_24", 0x0000000F, 0x00000007,
    (u32)"las_25", 0x0000000F, 0x00000007, (u32)"las_26",
    0x0000000F, 0x00000007, (u32)"las_27", 0x0000000F,
    0x00000007, (u32)"las_28", 0x0000000F, 0x00000007,
    (u32)"las_29", 0x0000000F, 0x00000007, (u32)"las_30",
    0x0000000F, 0x00000007, 0x00000000, 0xFFFFFFFF,
    0x00000000,
};

// .data ends with a 4-byte zero before the next unit's data.
__declspec(section ".data") u32 gap_05_8030E58C_data[] = { 0x00000000 };

//.sbss
void* ptr; //but why
u32 size;
void setupDataLoad(const char* mapName) {
    char path[64];
    DVDEntry* dvd;
    u32 filesize;

    ptr = NULL;
    sprintf(path, "%s/m/%s/s", getMarioStDvdRoot(), mapName);
    // The original keeps this string as an 8-byte object: "muj_20" followed by two NULs.
    if (strcmp(mapName, "muj_20\0") || evtGetValue(NULL, GSW(0)) != 2) {
        arcOpen(path, &ptr, &size);
        if (!ptr) {
            dvd = DVDMgrOpen(path, 2, 0);
            if (dvd) {
                filesize = DVDMgrGetLength(dvd);
                size = filesize;
                if (filesize) {
                    ptr = _mapAlloc(OSRoundUp32B(filesize));
                    DVDMgrRead(dvd, ptr, OSRoundUp32B(size), 0);
                }
                DVDMgrClose(dvd);
            }
        }
    }
}
BOOL setupDataCheck(void) {
    char tribeName[64];
    BOOL flag;

    if (ptr == NULL) {
        return TRUE;
    }
    parseInit(ptr, size);
    while (parsePush(str_Npc)) {
        parseTagGet1("<tribe>", 0, tribeName);
        parsePopNext();
        flag = gp->inBattle != 0;
        if (!animGroupBaseAsync(npcGetTribe(tribeName)->modelName, flag, NULL)) {
            return FALSE;
        }
    }
    parsePop();
    return TRUE;
}

// The attribute-table lookup is a helper so its pointer gets its own web; inlined three times this
// reproduces the original's callee-saved colours (attr r28, dead r29, the string base r31).
static inline MapAttrData* findMapAttr(const char* name) {
    MapAttrData* p = (MapAttrData*)_map_attr_data;
    for (;; p++) {
        if (p->name == NULL) {
            break;
        }
        if (strcmp(p->name, name) == 0) {
            break;
        }
    }
    return p;
}

// The declaration order below gives the original's callee-saved colours: flag in r27 and npcIndex in r26
// (the natural order splits their live ranges the other way).
void setupDataBase(const char* areaName, const char* mapName) {
    NpcTribeInfo* tribe;
    FieldBattleData* fbat;
    s32 flag;
    s32 npcIndex;
    NpcDeadInfo tmp;
    NpcDeadInfo* dead;
    MapAttrData* attr3;
    MapAttrData* attr;
    NpcEntry* npc;
    s32 i;
    AreaData* area;
    char* deadMap;
    s32 j;
    s32 battleInfoId;
    MapAttrData* attr2;
    NpcAiInfo* ai;
    char name[64];
    char tribeName[64];
    char aitype[64];
    char territoryType[64];
    char btlSetupNo[64];
    Vec position;
    Vec territoryBase;
    Vec territoryLoiter;
    Vec territoryHoming;
    f32 direction;
    f32 searchRange;
    f32 searchAngle;
    f32 homingRange;
    f32 homingAngle;

    fbat = fbatGetPointer();
    dead = fbat->deadInfo;
    deadMap = fbat->deadInfo[1].mapName;

    attr = findMapAttr(mapName);

    for (i = 0; i < 64; i++) {
        if (strcmp(fbat->deadInfo[i].mapName, zero_8041a6b8) == 0) {
            memset(&fbat->deadInfo[i], 0, sizeof(NpcDeadInfo));
            strcpy(fbat->deadInfo[i].mapName, mapName);
            break;
        }
        if (strcmp(fbat->deadInfo[i].mapName, mapName) == 0) {
            break;
        }
    }

    if (i > 0) {
        tmp = fbat->deadInfo[i];
        for (j = i; j >= 1; j--) {
            fbat->deadInfo[j] = fbat->deadInfo[j - 1];
        }
        fbat->deadInfo[0] = tmp;
    }

    if (attr->field_0x8 & 1) {
        attr2 = findMapAttr(mapName);
        attr3 = findMapAttr(deadMap);
        if (attr2->field_0x4 != attr3->field_0x4) {
            memset(dead, 0, 0x500);
            strcpy(dead[0].mapName, mapName);
        } else if (!(attr->field_0x8 & 2) && (attr->field_0x8 & 4)) {
            memset(&fbat->deadInfo[2], 0, 0x4D8);
        }
    }

    npcIndex = 0;
    flag = FALSE;
    if (attr->field_0x8 & 1) {
        flag = TRUE;
    }
    if (ptr == NULL) {
        return;
    }
    parseInit(ptr, size);
    for (; parsePush(str_Npc); npcIndex++) {
        parseTagGet1("<name>", 0, name);
        parseTagGet1("<tribe>", 0, tribeName);
        parseTagGet1("<position>", 3, &position);
        parseTagGet1(str_dir, 2, &direction);
        parseTagGet1("<aitype>", 0, aitype);
        parseTagGet1("<territory_type>", 0, territoryType);
        parseTagGet1("<territory_base>", 3, &territoryBase);
        parseTagGet1("<territory_loiter>", 3, &territoryLoiter);
        parseTagGet1("<territory_homing>", 3, &territoryHoming);
        parseTagGet1("<search_range>", 2, &searchRange);
        parseTagGet1("<search_angle>", 2, &searchAngle);
        parseTagGet1("<homing_range>", 2, &homingRange);
        parseTagGet1("<homing_angle>", 2, &homingAngle);
        parseTagGet1("<btl_setup_no>", 0, btlSetupNo);
        parsePopNext();
        if (flag && (dead[0].mask & (1 << (u8)npcIndex))) {
            continue;
        }
        tribe = npcGetTribe(tribeName);
        npcEntry(name, tribe->modelName);
        npc = npcNameToPtr(name);
        npc->setupIndex = npcIndex;
        npc->flags |= 4;
        npc->position = position;
        npc->direction = direction;
        npc->tribe = tribe;
        npc->width = tribe->width;
        npc->height = tribe->height;
        npc->flags &= ~2;
        for (ai = npc_ai_type_table;;) {
            if (ai->description == NULL) {
                goto no_ai; // skips copying the AI's events; moving the copy into the loop changes the object
            }
            if (strcmp(ai->description, aitype) == 0) {
                break;
            }
            ai++;
        }
        npc->flags |= ai->flags;
        npc->initEvent = ai->initEvent;
        npc->moveEvent = ai->moveEvent;
        npc->deadEvent = ai->deadEvent;
        npc->findEvent = ai->findEvent;
        npc->lostEvent = ai->lostEvent;
        npc->returnEvent = ai->returnEvent;
        npc->blowEvent = ai->blowEvent;
no_ai:
        npc->territoryType = dbGetDefData(npc_define_territory_type, territoryType);
        npc->territoryBase = territoryBase;
        npc->territoryLoiter = territoryLoiter;
        npc->territoryHoming = territoryHoming;
        npc->searchRange = searchRange;
        npc->searchAngle = searchAngle;
        npc->homingRange = homingRange;
        npc->homingAngle = homingAngle;
        npc->footstepSfxA = tribe->footstepSfxA;
        npc->footstepSfxB = tribe->footstepSfxB;
        npc->jumpSfx = tribe->jumpSfx;
        npc->landSfx = tribe->landSfx;
        npc->footstepStride = tribe->footstepStride;
        npc->footstepToggle = 1;
        npc->footstepAccum = float_0_8041a6c4;
        battleInfoId = -1;
        area = areaDataPtr(areaName);
        if (strcmp(btlSetupNo, "BTLNO_INVALID") != 0) {
            battleInfoId = dbGetDefData(area->btlSetupNoTbl, btlSetupNo);
        }
        npcSetBattleInfo(npc, battleInfoId);
        if (tribe->stayAnim) {
            strcpy(npc->stayAnim, tribe->stayAnim);
        }
        if (tribe->talkAnim) {
            strcpy(npc->talkAnim, tribe->talkAnim);
        }
        if (tribe->initialAnim) {
            strcpy(npc->currentAnim, tribe->initialAnim);
            animPoseSetAnim(npc->poseId, npc->currentAnim, 1);
        }
    }
    parsePop();
}
// clang-format on

// The original keeps these four .sdata2 words as named symbols; see config/G8MJ01/symbols.txt.
__declspec(section ".sdata2") const char zero_8041a6b8[4] = "";
__declspec(section ".sdata2") const char str_dir[8] = "<dir>";
__declspec(section ".sdata2") const f32 float_0_8041a6c4 = 0.0f;
__declspec(section ".sdata2") const char str_Npc[8] = "<Npc>";
