#include "seq/seq_mapchange.h"
#include "data/mapdata.h"
#include "driver/animdrv.h"
#include "driver/bgdrv.h"
#include "driver/camdrv.h"
#include "driver/casedrv.h"
#include "driver/effdrv.h"
#include "driver/extdrv.h"
#include "driver/hitdrv.h"
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
#include "cam_road.h"
#include "cam_shift.h"
#include "party.h"
#include "math_ext.h"
#include "pmario_sound.h"
#include "statuswindow.h"
#include "system.h"
#include "win/win_main.h"
#include <stdio.h>
#include <string.h>
#include "database.h"
#include "matching.h"

extern BadgeShopWork* bdsw;
extern BOOL dbg_lotteryinfo; // evt_lottery
#ifdef MUST_MATCH
extern f32 float_0p125_80419444; // original _load reads this named .sdata2 constant
// .rodata pool prefix: the original's table_802C1028 is an 8-byte { "jp", "us" } pair at .rodata+0.
// Emitting it first shifts _load's pooled zero-vecs to +0x8, matching the original pool offsets.
const char* const rodata_head_802C1028[2] = { "jp", "us" };
#endif

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

void seq_mapChangeInit(SequenceWork* work) {
    s32 notEmpty;
    s32 notGor;
    s32 nextNotGor;

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
    notEmpty = strcmp(gp->currentMapName, "");
    notGor = strcmp(gp->currentMapName, "gor_01");
    nextNotGor = strcmp(_next_map, "gor_01");
    if (notEmpty && notGor && nextNotGor) {
        bdsw->field_0x114 |= 1;
    } else if (!nextNotGor && bdsw->field_0x114 & 1) {
        badgeShop_bargainGeneration();
        badgeShop_bottakuruGeneration();
        bdsw->field_0x114 &= 0xFFFE;
    }
}

// table_802C1028 is the original file's .rodata string-pool base. Its first 8 bytes ({ "jp", "us" }) are
// reconstructed as rodata_head_802C1028 above; the map-name strings sit at +0x80/+0x88/+0x90, so naming the
// base and using pool-relative addends makes the reference shape (one `lis`/`addi` into r31, then
// `addi r4, r31, 0x80/0x88/0x90`) match. Without it the compiler anchors on our own pool and emits
// 0x24/0x2c/0x34.
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

static const char* dir_str[8] = { "e", "ne", "n", "nw", "w", "sw", "s", "se" };

void _load(const char* prevMapName, const char* newMapName, const char* beroName) {
    MapData* map;
    char mapFile[64];
    MarioWork* mario;
    CameraEntry* camera;
    f32 road[9];
    Vec pos;
    Vec entryPos;
    s32 i;

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
        PartyEntry* party;
        PartyEntry* extraParty;

        mario = marioGetPtr();
        party = partyGetPtr(marioGetPartyId());
        extraParty = partyGetPtr(marioGetExtraPartyId());
        mario->position = gp->savePlayerPos;
        mario->field_EC = mario->position;
        camRoadReset();
        camShiftReset();
        if (party != NULL) {
            party->position = gp->savePlayerPos;
        }
        if (extraParty != NULL) {
            extraParty->position = gp->savePlayerPos;
        }
    } else if (beroName != NULL) {
        f32 hx;
        f32 hy;
        f32 hz;
        f32 dist;
        f32 nx;
        f32 ny;
        f32 nz;

        dist = 100000.0f;
        hitObjGetPos(beroName, &pos);
        for (i = 0; i < 8; i++) {
            if (strncmp(dir_str[i], beroName, strlen(dir_str[i])) == 0) {
                f32 c;
                f32 s;
#ifdef MUST_MATCH
                f32 base = 2.0f * (f32)i * 3.141592f;
                f32 angle = base * MATCH_F32(float_0p125_80419444);
#else
                f32 angle = 2.0f * (f32)i * 3.141592f * 0.125f;
#endif

                c = cos(angle);
                pos.x += -30.0f * c;
                s = sin(angle);
                pos.z += 30.0f * s;
                break;
            }
        }
        if (i < 8) {
            hz = 0.0f;
            hy = 0.0f;
            hx = 0.0f;
            if (hitCheckFilter(NULL, &hx, &hy, &hz, &dist, &nx, &ny, &nz, pos.x, pos.y + 10.0f, pos.z, 0.0f,
                               -1.0f, 0.0f)) {
                entryPos = (Vec){ 0.0f, 0.0f, 0.0f };
                entryPos.x = hx;
                entryPos.y = hy;
                entryPos.z = hz;
                marioGetPtr()->position = entryPos;
            } else {
                marioGetPtr()->position = (Vec){ 0.0f, 0.0f, 0.0f };
            }
        } else {
            marioGetPtr()->position = (Vec){ 0.0f, 0.0f, 0.0f };
        }
    } else {
        marioGetPtr()->position = (Vec){ 0.0f, 0.0f, 0.0f };
    }
    mario = marioGetPtr();
    camera = camGetPtr(4);
    camera->marioPos.x = mario->position.x;
    camera->marioPos.y = mario->position.y + 0.01f;
    camera->marioPos.z = mario->position.z;
    camera->marioAt.x = camera->marioPos.x;
    camera->marioAt.y = camera->marioPos.y;
    camera->marioAt.z = camera->marioPos.z;
    mario->field_EC = mario->position;
    camRoadReset();
    camShiftReset();
    if (camera->flags & 4) {
        camRoadMain(road, camera->marioPos.x + 1.0f, camera->marioPos.y + 1.0f, camera->marioPos.z + 1.0f,
                    camera->marioAt.x + 1.0f, camera->marioAt.y + 1.0f, camera->marioAt.z + 1.0f, 0.0f);
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

#ifdef MUST_MATCH
// The original .rodata's seq_mapChangeMain strings; emitted here (after the code) so MWCC keeps the pool's
// original slots: they land between _load's pooled zero-vecs and its pooled map-name strings.
const char str_802C1060[8] = "tou_03";
const char str_802C1068[16] = "%s/rel/%s.rel";
#endif
