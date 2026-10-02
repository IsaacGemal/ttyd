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

// The original .rodata string-pool base for this function's strings (see _unload).
#ifdef MUST_MATCH
#define SMC_STR(off, lit) (base + (off))
#else
#define SMC_STR(off, lit) (lit)
#endif

extern u8 rel_bss[];
extern u32 OSGetSoundMode(void);
extern void SoundSetOutputMode(s32 mode);
extern BOOL OSLink(OSModuleInfo* module, void* bss);
extern const GXColor dat_804193f0;
extern const GXColor dat_804193f4;
extern const GXColor dat_804193f8;
extern const GXColor dat_804193fc;
extern const GXColor unk_80422f60;
extern const f32 float_30_8041944c;
extern const f32 float_neg30_80419448;
extern const f32 float_5_8041947c;
extern const f32 float_10_80419454;
extern s32 key_off[10];

// NONMATCHING (99.84%): behaviour complete, checked against the asm; one branch polarity in the field-save block
// (`if (swByteGet(4) == 0)` here compiles to bne where the target has beq + b).
void seq_mapChangeMain(SequenceWork* work) {
    const char* const base = table_802C1028;
    BOOL newarea;
    const char* area;
    const char* lang[2];
    s32* ev;

    newarea = strcmp(_next_area, gp->currentAreaName) != 0;
    switch (work->state) {
    case 0:
        if (strcmp(_next_map, SMC_STR(0x88, "sys_01")) == 0) {
            if (gp->areaFadeInType != 10) {
                fadeEntry(gp->areaFadeInType, gp->areaFadeInDuration, dat_804193f0);
            }
            fadeEntry(0x11, 2000, dat_804193f4);
        } else if (!newarea) {
            fadeEntry(gp->mapFadeInType, gp->mapFadeInDuration, dat_804193f8);
        } else {
            fadeEntry(gp->areaFadeInType, gp->areaFadeInDuration, dat_804193fc);
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
        if (strcmp(_next_map, SMC_STR(0x38, "tou_03")) != 0) {
            area = _next_area;
            if (strcmp(_next_area, "tou") == 0) {
                if (seqGetSeq() == 1) {
                    area = "tou2";
                } else if (strcmp(_next_map, SMC_STR(0x38, "tou_03")) == 0) {
                    area = "tou2";
                }
            }
            fileAsyncf(0, 0, SMC_STR(0x40, "%s/rel/%s.rel"), getMarioStDvdRoot(), area);
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
                } else if (gp->unk1274 == 0) {
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
            strcpy((char*)gp + 0x14C, gp->currentMapName);
            strcpy(gp->currentAreaName, "");
            strcpy(gp->currentMapName, "");
            seqSetSeq(1, 0, 0);
            return;
        }
        area = _next_area;
        if (strcmp(_next_area, "tou") == 0) {
            if (seqGetSeq() == 1) {
                area = "tou2";
            } else if (strcmp(_next_map, SMC_STR(0x38, "tou_03")) == 0) {
                area = "tou2";
            }
        }
        {
            u32 r = (u32)fileAsyncf(0, 0, SMC_STR(0x40, "%s/rel/%s.rel"), getMarioStDvdRoot(), area);
            s32 ready;

            if (r == 0) {
                ready = 0;
            } else if ((s32)r == -1) {
                ready = 1;
            } else {
                ready = 1;
            }
            if (ready != 0 && ((BOOL (*)(const char*))mapPreLoad)(_next_map) != 0) {
                FileEntry* file;

                area = _next_area;
                if (strcmp(_next_area, "tou") == 0) {
                    if (seqGetSeq() == 1) {
                        area = "tou2";
                    } else if (strcmp(_next_map, SMC_STR(0x38, "tou_03")) == 0) {
                        area = "tou2";
                    }
                }
                file = (FileEntry*)fileAllocf(0, SMC_STR(0x40, "%s/rel/%s.rel"), getMarioStDvdRoot(), area);
                if (file != NULL) {
                    if (strncmp(area, "tst", 3) == 0) {
                        gp->relocationBase = _mapAlloc((u32)file->data[1]);
                    } else if (strncmp(area, "jon", 3) == 0) {
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
                gp->field_0x164 = (u32)evtEntry(ev, 0, 0)->eventId;
            } else {
                gp->field_0x164 = 0;
            }
            work->state++;
            return;
        }
        break;
    case 4:
        if ((s32)gp->field_0x164 == 0) {
            camCtrlOn(4);
            work->state++;
            return;
        }
        if (evtCheckID(gp->field_0x164) == 0) {
            camCtrlOn(4);
            gp->field_0x164 = 0;
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
            lang[0] = ((const char* const*)base)[0];
            lang[1] = ((const char* const*)base)[1];
#else
            lang[0] = "jp";
            lang[1] = "us";
#endif
            fileAsyncf(4, 0, SMC_STR(0xA0, "%s/window/%s/win.tpl"), getMarioStDvdRoot(), lang[gp->language]);
            animGroupBaseAsync(SMC_STR(0xB8, "p_jyabara"), 0, NULL);
            animGroupBaseAsync(SMC_STR(0xC4, "p_kaiten_h"), 0, NULL);
            animGroupBaseAsync(SMC_STR(0xD0, "p_b_st"), 0, NULL);
            animGroupBaseAsync(SMC_STR(0xD8, "OFF_d_maku_up_down"), 0, NULL);
            effNiceAsync(0);
            fileAsyncf(4, 0, SMC_STR(0xEC, "%s/battle/common/battle_common.tpl"), getMarioStDvdRoot());
            fileAsyncf(4, 0, SMC_STR(0x110, "%s/battle/audience/audience_normal.tpl"), getMarioStDvdRoot());
            fileAsyncf(4, 0, SMC_STR(0x138, "%s/battle/audience/audience_luigi.tpl"), getMarioStDvdRoot());
        }
        if (gp->flags & 1) {
            MarioWork* mario = marioGetPtr();
            PartyEntry* party;
            PartyEntry* extraParty;

            party = partyGetPtr(marioGetPartyId());
            extraParty = partyGetPtr(marioGetExtraPartyId());
            if (swByteGet(4) == 0) {
                mario->position = gp->savePlayerPos;
                evt_cam_road_reset(NULL, FALSE);
                if (party != NULL) {
                    party->position = gp->savePlayerPos;
                    party->position.x = float_30_8041944c * (f32)cos(camGetPtr(4)->viewYaw) + party->position.x;
                    party->position.z = float_30_8041944c * (f32)sin(camGetPtr(4)->viewYaw) + party->position.z;
                }
                if (extraParty != NULL) {
                    extraParty->position = gp->savePlayerPos;
                    extraParty->position.x =
                        float_neg30_80419448 * (f32)cos(camGetPtr(4)->viewYaw) + extraParty->position.x;
                    extraParty->position.z =
                        float_30_8041944c * (f32)sin(camGetPtr(4)->viewYaw) + extraParty->position.z;
                }
                if (strcmp(gp->currentMapName, SMC_STR(0x160, "dou_05")) == 0
                    || strcmp(gp->currentMapName, SMC_STR(0x58, "gor_01")) == 0
                    || strcmp(gp->currentMapName, SMC_STR(0x168, "muj_10")) == 0
                    || strcmp(gp->currentMapName, SMC_STR(0x170, "tik_07")) == 0) {
                    mario->position.x -= float_5_8041947c;
                    if (party != NULL) {
                        party->position = mario->position;
                        party->position.x -= float_30_8041944c;
                        if (extraParty != NULL) {
                            extraParty->position = party->position;
                            extraParty->position.x -= float_30_8041944c;
                        }
                    }
                }
                N_marioSetBottomlessRespawnPos(mario->position.x, float_10_80419454 + mario->position.y,
                                               mario->position.z);
                marioKeyOff();
                evtEntryType(key_off, 0, 0, 0);
            } else {
                fadeEntry(0x1C, 0, unk_80422f60);
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

#ifdef MUST_MATCH
// The original .rodata's seq_mapChangeMain strings; emitted here (after the code) so MWCC keeps the pool's
// original slots: they land between _load's pooled zero-vecs and its pooled map-name strings.
const char str_802C1060[8] = "tou_03";
const char str_802C1068[16] = "%s/rel/%s.rel";
#endif
