#include "seq/seq_mapchange.h"
#include "data/mapdata.h"
#include "driver/animdrv.h"
#include "driver/bgdrv.h"
#include "driver/casedrv.h"
#include "driver/effdrv.h"
#include "driver/extdrv.h"
#include "driver/icondrv.h"
#include "driver/imgdrv.h"
#include "driver/itemdrv.h"
#include "driver/mapdrv.h"
#include "driver/mobjdrv.h"
#include "driver/msgdrv.h"
#include "driver/npcdrv.h"
#include "driver/offscreendrv.h"
#include "driver/swdrv.h"
#include "driver/windowdrv.h"
#include "evt/evt_badgeshop.h"
#include "evt/evt_nannpc.h"
#include "manager/evtmgr.h"
#include "manager/winmgr.h"
#include "mario/mario.h"
#include "mario/mario_cam.h"
#include "mario/mario_party.h"
#include "mario/mariost.h"
#include "memory.h"
#include "party.h"
#include "pmario_sound.h"
#include "statuswindow.h"
#include "system.h"
#include "win/win_main.h"
#include <stdio.h>
#include <string.h>
#include "database.h"

extern BadgeShopWork* bdsw;
extern BOOL dbg_lotteryinfo; // evt_lottery

//.sbss
char _next_bero[0x20];
char _next_map[0x20];
char _next_area[0x20];

// NONMATCHING (0.15%): empty placeholder, not decompiled yet.
void seq_mapChangeMain(SequenceWork* work) {
    /*
    BOOL newarea;
    char* area_name;

    newarea = strcmp(_next_area, gp->mCurrentAreaName) != 0;
    switch (work->field_0x4) {
    case 0:
            if (strcmp(_next_map, "sys_01")) {
                    if (newarea) {
                            fadeEntry(gp->mNextAreaChangeFadeInType, gp->mNextAreaChangeFadeInDuration, (GXColor) { 0,
    0, 0, 0xFF });
                    }
                    else {
                            fadeEntry(gp->mNextMapChangeFadeInType, gp->mNextMapChangeFadeInDuration, (GXColor) { 0, 0,
    0, 0xFF });
                    }
            }
            else {
                    if (gp->mNextAreaChangeFadeInType != 10) {
                            fadeEntry(gp->mNextAreaChangeFadeInType, gp->mNextAreaChangeFadeInDuration, (GXColor) { 0,
    0, 0, 0xFF });
                    }
                    fadeEntry(17, 2000, (GXColor) { 0, 0, 0, 0xFF });
            }
            gp->mNextMapChangeFadeInType = 10;
            gp->mNextMapChangeFadeInDuration = 300;
            gp->mNextAreaChangeFadeInType = 10;
            gp->mNextAreaChangeFadeInDuration = 300;
            psndSFXAllOff();
            gp->mbAreaChanged = newarea;
            if (strcmp(_next_map, "tou_03")) {
                    area_name = _next_area;
                    if (!strcmp(_next_area, "tou")) {
                            if (seqGetSeq() == 1) {
                                    area_name = "tou2";
                            }
                            else if (!strcmp(_next_map, "tou_03")) {
                                    area_name = "tou2";
                            }
                    }
                    fileAsyncf(0, 0, "%s/rel/%s.rel", getMarioStDvdRoot(), area_name);
            }
            work->field_0x4++;
            break;

    case 1:
            psndSFXAllOff();
            if (fadeIsFinish()) {
                    if (gp->mpRelFileBase) {
                            ((void(*)(void))gp->mpRelFileBase->epilog)();
                            OSUnlink(&gp->mpRelFileBase->info);
                    }
            }
            break;
    }*/
}

void seq_mapChangeExit(SequenceWork* work) {
    GXResetOverflowCount();
    dbg_lotteryinfo = FALSE;
}

// NONMATCHING (77.63%): 2 instructions differ, 8 missing, 8 extra, plus register differences.
void seq_mapChangeInit(SequenceWork* work) {
    strcpy(_next_area, "");
    strcpy(_next_map, "");
    strcpy(_next_bero, "");
    if (work->mapName) {
        strncpy(_next_area, work->mapName, 3);
        strncpy(_next_map, work->mapName, 0x20);
    }
    if (work->beroName) {
        strncpy(_next_bero, work->beroName, 0x20);
    }
    psndENVOff(0x200);
    psndENVOff(0x201);
    if (strcmp(gp->currentMapName, "") && strcmp(gp->currentMapName, "gor_01") && strcmp(_next_map, "gor_01")) {
        bdsw->field_0x114 |= 1;
    } else if (!strcmp(_next_map, "gor_01") && bdsw->field_0x114 & 1) {
        badgeShop_bargainGeneration();
        badgeShop_bottakuruGeneration();
        bdsw->field_0x114 &= 0xFFFE;
    }
}

// table_802C1028 is the original file's .rodata string-pool base (declared, not defined here, because the
// rest of the TU's pool - the 4 zero vec3s, the seq_mapChangeMain/_load literals and double_to_int - is
// emitted by the not-yet-decompiled functions and isn't in our .rodata). The three map-name strings sit
// at +0x80/+0x88/+0x90 in that pool; naming the base and using pool-relative addends makes the reference
// shape (one `lis`/`addi` into r31, then `addi r4, r31, 0x80/0x88/0x90`) match. Without it the compiler
// anchors on our own truncated .rodata and emits 0x24/0x2c/0x34.
extern const char table_802C1028[]; // this file's rodata pool base; original strings are pool-relative
#ifdef MUST_MATCH
#define STR_SYS_00 (base + 0x80)
#define STR_SYS_01 (base + 0x88)
#define STR_DIG_00 (base + 0x90)
#else
#define STR_SYS_00 "sys_00"
#define STR_SYS_01 "sys_01"
#define STR_DIG_00 "dig_00"
#endif

void _unload(const char* prevMapName, const char* newMapName, const char* arg2) {
    BOOL unload;
    s32 i;
#ifdef MUST_MATCH
    const char* const base = table_802C1028;
#endif

    unload = FALSE;
    if (prevMapName != NULL && newMapName != NULL && strncmp(prevMapName, newMapName, 3) != 0) {
        unload = TRUE;
    }
    marioReInit();
    marioInitCamId();
    mapUnLoad();
    npcReset(FALSE);
    extReset();
    mobjReset(FALSE);
    offscreenReset(FALSE);
    imgAutoRelease(0);
    effAutoRelease(FALSE);
    animPoseAutoRelease(0);
    evtmgrReInit();
    caseReInit();
    itemReInit();
    iconReInit();
    windowReInit();
    winReInit();
    statusWinReInit();
    bgReInit();
    winMgrReInit();
    if (seqGetPrevSeq() != SEQ_LOAD && seqGetPrevSeq() != SEQ_TITLE &&
        strcmp(prevMapName, STR_SYS_00) != 0 &&
        strcmp(prevMapName, STR_SYS_01) != 0 &&
        strcmp(prevMapName, STR_DIG_00) != 0) {
        fadeReset(1);
        gp->flags &= ~0x10;
    }
    for (i = 0; i < 4; i++) {
        padRumbleHardOff(i);
    }
    if (unload) {
        swReInit();
    }
    memClear(HEAP_MALLOC);
    memClear(HEAP_EXT);
    smartAutoFree(1);
    nanNPCWork = NULL;
}

// NONMATCHING (31.83%): 208 missing, 1 extra, plus register differences.
void _load(const char* prevMapName, const char* newMapName, const char* beroName) {
    MapData* map;
    char mapFile[64];

    map = mapDataPtr(newMapName);
    strcpy(gp->beroEnterName, beroName ? beroName : "");
    strcpy(gp->currentMapName, map->mapName);
    strncpy(gp->currentAreaName, map->mapName, 3);
    if (!strcmp(newMapName, "dmo_00")) {
        mapLoad("gor_01");
    } else {
        mapLoad(map->mapName);
    }
    camCtrlOff(4);
    if (gp->field_C) {
        msgLoad("gon_11", 0);
    } else {
        memset(mapFile, 0, sizeof(mapFile));
        strncpy(mapFile, map->mapName, 6);
        msgLoad(mapFile, 0);
    }
    setupDataLoad(map->mapName);
    sprintf(mapFile, "./b/%s.tpl", newMapName);
    bgEntry(mapFile);
    if (gp->flags & 1) {
        MarioWork* mario;
        s32 partyId;
        PartyEntry* party;
        // TODO: verify this is e.g. Flavio and the Egg from Glitzville and rename accordingly
        s32 extraPartyId;
        PartyEntry* extraParty;

        mario = marioGetPtr();
        partyId = marioGetPartyId();
        party = partyGetPtr(partyId);
        extraPartyId = marioGetExtraPartyId();
        extraParty = partyGetPtr(extraPartyId);
        mario->field_EC = mario->position = gp->savePlayerPos;
    }
}

void _relUnLoad(void) {
    if (gp->relocationBase) {
        ((void (*)(void))((OSModuleHeader*)gp->relocationBase)->epilog)();
        OSUnlink(gp->relocationBase);
        if (gp->relocationBase != gp->mapAlloc) {
            _mapFree(gp->relocationBase);
        }
        gp->relocationBase = NULL;
    }
}
