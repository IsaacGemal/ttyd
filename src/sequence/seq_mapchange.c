#include "seq/seq_mapchange.h"
#include "mario/mario_sbr.h"
#include "evt/evt_cam.h"
#include "evt/evt_bero.h"
#include "eff/eff_nice.h"
#include "driver/fadedrv.h"
#include "manager/filemgr.h"
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
#include "evt/evt_seq.h"
#include "evt/evt_fade.h"
#include "evt/evt_mario.h"
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

// .rodata pool prefix: the original's table_802C1028 is an 8-byte { "jp", "us" } pair at .rodata+0.
// It is also the base MWCC uses for every pooled .rodata string reference in this unit.
static const char* const table_802C1028[2] = { "jp", "us" };

// .sbss, declared lowest address last (MWCC lays uninitialised globals out last-declared-first).
char _next_area[0x20];
char _next_map[0x20];
char _next_bero[0x20];

// .sbss2 gap after the fade colour's template.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") u8 gap_10_80422F64_sbss2[4];
#pragma section ".sdata" ".sbss"

// .bss
#define REL_BSS_SIZE 0xF10
u8 rel_bss[REL_BSS_SIZE];

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

void seq_mapChangeExit(SequenceWork* work) {
    GXResetOverflowCount();
    dbg_lotteryinfo = FALSE;
}

#define STR_SYS_00 "sys_00"
#define STR_SYS_01 "sys_01"
#define STR_DIG_00 "dig_00"

// The original .rodata string-pool base for this function's strings (see _unload).
// tou_03 and %s/rel/%s.rel are explicit objects in the parse-time part of the pool (str_tou_03/
// str_relPathFormat), so they must stay pool-relative; every other string here is a normal generated pool
// entry and is emitted by using the literal.
#define SMC_STR(off, lit) (lit)
#define SMC_STR_OFF(off, lit) (lit)

extern void SoundSetOutputMode(s32 mode);
extern s32 key_off[10];

// Two-pointer view of table_802C1028; the original copies both words in one 64-bit move.
void seq_mapChangeMain(SequenceWork* work) {
    BOOL newarea;
    const char* area;
    const char* lang[2];
    s32* ev;

    newarea = strcmp(_next_area, gp->currentAreaName) != 0;
    switch (work->state) {
    case 0:
        if (strcmp(_next_map, SMC_STR(0x88, "sys_01")) == 0) {
            if (gp->areaFadeInType != 10) {
                fadeEntry(gp->areaFadeInType, gp->areaFadeInDuration, (GXColor){ 0, 0, 0, 0xFF });
            }
            fadeEntry(0x11, 2000, (GXColor){ 0, 0, 0, 0xFF });
        } else if (!newarea) {
            fadeEntry(gp->mapFadeInType, gp->mapFadeInDuration, (GXColor){ 0, 0, 0, 0xFF });
        } else {
            fadeEntry(gp->areaFadeInType, gp->areaFadeInDuration, (GXColor){ 0, 0, 0, 0xFF });
        }
        gp->mapFadeInType = 10;
        gp->mapFadeInDuration = 300;
        gp->areaFadeInType = 10;
        gp->areaFadeInDuration = 300;
        psndSFXAllOff();
        if (!newarea) {
            gp->didAreaChange = 0;
        } else {
            gp->didAreaChange = 1;
        }
        if (strcmp(_next_map, str_tou_03) != 0) {
            area = _next_area;
            if (strcmp(_next_area, str_tou) == 0) {
                if (seqGetSeq() == 1) {
                    area = str_tou2;
                } else if (strcmp(_next_map, str_tou_03) == 0) {
                    area = str_tou2;
                }
            }
            fileAsyncf(FILE_TYPE_RAW, 0, str_relPathFormat, getMarioStDvdRoot(), area);
        }
        work->state++;
        return;
    case 1:
        psndSFXAllOff();
        if (fadeIsFinish()) {
            _relUnLoad();
            _unload(gp->currentMapName, _next_map, _next_bero);
            if (strcmp(_next_map, SMC_STR(0x50, "dmo_00")) != 0 && strcmp(_next_map, "title") != 0) {
                if (OSGetSoundMode() == 0) {
                    SoundSetOutputMode(0);
                } else if (gp->soundSurround == 0) {
                    SoundSetOutputMode(1);
                } else {
                    SoundSetOutputMode(2);
                }
            }
            work->state++;
        }
    default:
        return;
    case 2:
        if (strcmp(_next_map, "title") == 0) {
            strcpy(gp->lastMapName, gp->currentMapName);
            strcpy(gp->currentAreaName, "");
            strcpy(gp->currentMapName, "");
            seqSetSeq(1, 0, 0);
            return;
        }
        area = _next_area;
        if (strcmp(_next_area, str_tou) == 0) {
            if (seqGetSeq() == 1) {
                area = str_tou2;
            } else if (strcmp(_next_map, str_tou_03) == 0) {
                area = str_tou2;
            }
        }
        {
            u32 r = fileAsyncf(FILE_TYPE_RAW, 0, str_relPathFormat, getMarioStDvdRoot(), area);
            s32 ready;

            if (r == 0) {
                ready = 0;
            } else if ((s32)r == -1) {
                ready = 1;
            } else {
                ready = 1;
            }
            if (ready != 0 && mapPreLoad(_next_map) != 0) {
                FileEntry* file;

                area = _next_area;
                if (strcmp(_next_area, str_tou) == 0) {
                    if (seqGetSeq() == 1) {
                        area = str_tou2;
                    } else if (strcmp(_next_map, str_tou_03) == 0) {
                        area = str_tou2;
                    }
                }
                file = fileAllocf(FILE_TYPE_RAW, str_relPathFormat, getMarioStDvdRoot(), area);
                if (file != NULL) {
                    if (strncmp(area, str_tst, 3) == 0) {
                        gp->relocationBase = _mapAlloc((u32)file->data[1]);
                    } else if (strncmp(area, str_jon, 3) == 0) {
                        gp->relocationBase = _mapAlloc((u32)file->data[1]);
                    } else {
                        gp->relocationBase = gp->mapAlloc;
                    }
                    memcpy(gp->relocationBase, file->data[0], (u32)file->data[1]);
                    fileFree(file);
                }
                if (gp->relocationBase != NULL) {
                    memset(rel_bss, 0, 0x3C4);
                    OSLink(gp->relocationBase, rel_bss);
                    ((void (*)(void))((OSModuleHeader*)gp->relocationBase)->prolog)();
                }
                _load(gp->currentMapName, _next_map, _next_bero);
                work->state++;
                return;
            }
        }
        break;
    case 3:
        if (setupDataCheck() != 0) {
            if (psndGetFlag() & 0x1000) {
                if (psndBGMPlayTime(0) < 0xBB8) {
                    break;
                }
                psndClearFlag(0x1000);
            }
            if (psndGetFlag() & 0x2000) {
                if (psndBGMPlayTime(1) < 0xBB8) {
                    break;
                }
                psndClearFlag(0x2000);
            }
            setupDataBase(_next_area, _next_map);
            psndMapChange();
            ev = mapDataPtr(_next_map)->eventAddr;
            if (ev != NULL) {
                gp->mapInitEvtId = (u32)evtEntry(ev, 0, 0)->eventId;
            } else {
                gp->mapInitEvtId = 0;
            }
            work->state++;
            return;
        }
        break;
    case 4:
        if ((s32)gp->mapInitEvtId == 0) {
            camCtrlOn(4);
            work->state++;
            return;
        }
        if (evtCheckID(gp->mapInitEvtId) == 0) {
            camCtrlOn(4);
            gp->mapInitEvtId = 0;
            work->state++;
            return;
        }
        break;
    case 5:
        npcExecAllInitEvt();
        work->state++;
        return;
    case 6:
        if (!(camGetPtr(8)->flags & 0x200) && !(bero_get_BeroEXEC() & 1) && (marioChkKey() == 0 || marioChkCtrl() == 0)) {
            camGetPtr(8)->flags |= 0x500;
        }
        if (npcWaitAllInitEvtEnd() != 0) {
            npcExecAllReglEvt();
            work->state++;
            return;
        }
        break;
    case 7:
        if (strncmp(_next_map, "dmo", 3) != 0 && strncmp(_next_map, "dig", 3) != 0 && strncmp(_next_map, "end", 3) != 0
            && strncmp(_next_map, "sys", 3) != 0 && strncmp(_next_map, SMC_STR(0x98, "gor_10"), 6) != 0) {
#ifdef MUST_MATCH
            *(MapLangPair*)lang = *(const MapLangPair*)table_802C1028;
#else
            lang[0] = "jp";
            lang[1] = "us";
#endif
            fileAsyncf(FILE_TYPE_TPL, 0, SMC_STR(0xA0, "%s/window/%s/win.tpl"), getMarioStDvdRoot(), lang[gp->language]);
            animGroupBaseAsync(SMC_STR(0xB8, "p_jyabara"), 0, NULL);
            animGroupBaseAsync(SMC_STR(0xC4, "p_kaiten_h"), 0, NULL);
            animGroupBaseAsync(SMC_STR(0xD0, "p_b_st"), 0, NULL);
            animGroupBaseAsync(SMC_STR(0xD8, "OFF_d_maku_up_down"), 0, NULL);
            effNiceAsync(0);
            fileAsyncf(FILE_TYPE_TPL, 0, SMC_STR(0xEC, "%s/battle/common/battle_common.tpl"), getMarioStDvdRoot());
            fileAsyncf(FILE_TYPE_TPL, 0, SMC_STR(0x110, "%s/battle/audience/audience_normal.tpl"), getMarioStDvdRoot());
            fileAsyncf(FILE_TYPE_TPL, 0, SMC_STR(0x138, "%s/battle/audience/audience_luigi.tpl"), getMarioStDvdRoot());
        }
        if (gp->flags & 1) {
            MarioWork* mario = marioGetPtr();
            PartyEntry* party;
            PartyEntry* extraParty;

            party = partyGetPtr(marioGetPartyId());
            extraParty = partyGetPtr(marioGetExtraPartyId());
            // Written as a `switch` with one `case 0` and a `default` because that makes mwcc emit the
            // target's `beq then; b else` layout; a plain `if (swByteGet(4) == 0) ... else ...` comes out as
            // `bne else` and mismatches the branch pair.
            switch (swByteGet(4)) {
            case 0:
                mario->position = gp->savePlayerPos;
                evt_cam_road_reset(NULL, FALSE);
                if (party != NULL) {
                    party->position = gp->savePlayerPos;
                    party->position.x = 30.0f * (f32)cos(camGetPtr(4)->viewYaw) + party->position.x;
                    party->position.z = 30.0f * (f32)sin(camGetPtr(4)->viewYaw) + party->position.z;
                }
                if (extraParty != NULL) {
                    extraParty->position = gp->savePlayerPos;
                    extraParty->position.x =
                        -30.0f * (f32)cos(camGetPtr(4)->viewYaw) + extraParty->position.x;
                    extraParty->position.z =
                        30.0f * (f32)sin(camGetPtr(4)->viewYaw) + extraParty->position.z;
                }
                if (strcmp(gp->currentMapName, SMC_STR(0x160, "dou_05")) == 0
                    || strcmp(gp->currentMapName, SMC_STR(0x58, "gor_01")) == 0
                    || strcmp(gp->currentMapName, SMC_STR(0x168, "muj_10")) == 0
                    || strcmp(gp->currentMapName, SMC_STR(0x170, "tik_07")) == 0) {
                    mario->position.x -= 5.0f;
                    if (party != NULL) {
                        party->position = mario->position;
                        party->position.x -= 30.0f;
                        if (extraParty != NULL) {
                            extraParty->position = party->position;
                            extraParty->position.x -= 30.0f;
                        }
                    }
                }
                N_marioSetBottomlessRespawnPos(mario->position.x, 10.0f + mario->position.y,
                                               mario->position.z);
                marioKeyOff();
                evtEntryType(key_off, 0, 0, 0);
                break;
            default:
                fadeEntry(0x1C, 0, (GXColor){ 0, 0, 0, 0 });
                break;
            }
            statusWinForceUpdate();
            gp->flags &= ~1;
            marioStSystemLevel(0);
        }
        marioEntry();
        seqSetSeq(2, 0, 0);
        break;
    }
}

// .sdata evt command table; the original keeps this before dir_str in the section.
s32 key_off[10] = {
    0x0002005B, (s32)evt_seq_wait,     0x00000002, 0x0001005B, (s32)evt_fade_end_wait,
    0x0002005B, (s32)evt_mario_key_onoff, 0x00000001, 0x00000002, 0x00000001,
};

static const char* dir_str[8] = { "e", "ne", "n", "nw", "w", "sw", "s", "se" };

// Short map-name strings that the original keeps ahead of _load's pooled zero and floats. Naming them places
// them in the parse-time part of .sdata2 and keeps seq_mapChangeMain from pooling its own copies.
const char str_tou[] = "tou";
const char str_tou2[] = "tou2";
const char str_tst[] = "tst";
const char str_jon[] = "jon";

void _unload(const char* prevMapName, const char* newMapName, const char* arg2) {
    BOOL unload;
    s32 i;


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
                f32 one = 0.125f;
                f32 base = 2.0f * (f32)i * 3.141592f;
                f32 angle = base * one;

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
            if (hitCheckFilter(NULL, pos.x, pos.y + 10.0f, pos.z, 0.0f, -1.0f, 0.0f, &hx, &hy, &hz, &dist, &nx,
                               &ny, &nz)) {
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

// The original .rodata's seq_mapChangeMain strings; emitted here (after the code) so MWCC keeps the pool's
// original slots: they land between _load's pooled zero-vecs and its pooled map-name strings.
const char str_tou_03[8] = "tou_03";
const char str_relPathFormat[16] = "%s/rel/%s.rel";
