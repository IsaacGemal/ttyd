#include "win/win_main.h"
#include "data/item_data.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "driver/seqdrv.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "manager/fontmgr.h"
#include "driver/msgdrv.h"
#include "mario/mario.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "memory.h"
#include "party.h"
#include "system.h"
#include <dolphin/pad.h>
#include <demo/DEMOPad.h>
#include <string.h>
#include <stdarg.h>
#include "math_ext.h"
#include "evt/evt_yuugijou.h"
#include "driver/swdrv.h"
#include "driver/fadedrv.h"
#include "driver/mobjdrv.h"
#include "driver/npcdrv.h"
#include "driver/animdrv.h"
#include "manager/filemgr.h"
#include "mario/mario_party.h"
#include "koopa.h"
#include "window/win_log.h"
#include "pmario_sound.h"
#include "eff/eff_recovery.h"
#include "statuswindow.h"
#include "window/win_main.h"
#include "window/win_root.h"
#include "party/party_motion.h"
#include <stdio.h>
#include "battle/ac/ac_table.h"
#include "battle/battle_ac.h"
#include "battle/battle_unit.h"
#include "evt/evt_eff.h"
#include "evt/evt_env.h"
#include "evt/evt_fade.h"
#include "evt/evt_item.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_seq.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"

// Data objects defined later in this file or in other units, declared before their first use.
extern volatile f32 float_0p5_8041dc98;

#ifdef MUST_MATCH
// The original evaluates each GXPosition component and stores it immediately, so a
// component read from memory is reloaded before its store instead of being hoisted.
#undef GXPosition3f32
#define GXPosition3f32(x, y, z) ((void)(GXWGFifo.f32 = (x), GXWGFifo.f32 = (y), GXWGFifo.f32 = (z)))
#endif

// Data objects defined later in this file or in other units, declared before their first use.
extern const Vec vec3_802f68c0;
extern const Vec vec3_802f68cc;
extern const GXColor dat_8041dc00;
extern const GXColor dat_8041dc90;
extern const GXColor dat_8041dc94;
extern s32 evt_lecture_msg[];

extern ItemData itemDataTable[];

__declspec(section ".sdata2") const GXColor dat_8041dc00 = { 0xFF, 0xFF, 0xFF, 0xFF };

//.sbss
static WindowWork* wp;
TEXPalette* winTexTpl;

// local prototypes

// functions defined in this unit but not declared in any header

// mario_pouch.h declares the pouch accessors with s16 parameters, but the
// original win_main.c treated them as s32 (the callees sign-extend their own
// s16 returns). Call through the wider signature so MWCC does not insert
// spurious extsh conversions.
#define POUCH_GETHP() (*(s32(*)(void))pouchGetHP)()
#define POUCH_SETHP(v) (*(void(*)(s32))pouchSetHP)(v)
#define POUCH_GETFP() (*(s32(*)(void))pouchGetFP)()
#define POUCH_SETFP(v) (*(void(*)(s32))pouchSetFP)(v)
#define POUCH_GETAP() (*(s32(*)(void))pouchGetAP)()
#define POUCH_SETAP(v) (*(void(*)(s32))pouchSetAP)(v)
#define POUCH_GETPARTYHP(id) (*(s32(*)(s32))pouchGetPartyHP)(id)
#define POUCH_SETPARTYHP(id, v) (*(void(*)(s32, s32))pouchSetPartyHP)(id, v)

// WindowWork.currentEvent is actually at 0x18; the header wrongly places it
// at 0x3C (0x14 is a s32 counter used by winLecture*). Use a raw offset.
#define WP_CURRENT_EVENT(w) (*(EventEntry**)((u8*)(w) + 0x18))

static s32 cam_r(void);
s32 evt_diary_read[259];

// clang-format off

const Vec vec3_802f68c0 = { 0.0f, 50.0f, 0.0f };
const Vec vec3_802f68cc = { 1.0f, 1.0f, 1.0f };
// Building the fog colour in an inlined helper puts the fog by-value temp in the
// lower stack slot and winTexSet's later colour temp in the higher one; written
// inline the two temps swap slots.
#ifdef MUST_MATCH
static inline void winDispKoopaSetFog(void) {
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, dat_8041dc90);
}
#endif

// Building the fog colour inside an inlined helper gives the fog by-value temp the
// lower stack slot and the later GXSetTevColor temp the higher one, matching the target;
// written inline the two temps swap slots.
#ifdef MUST_MATCH
static inline void winIconGraySetFog(void) {
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, dat_8041dc90);
}
#endif

WindowWork* winGetPtr(void) {
    return wp;
}

void winInit(void) {
    wp = __memAlloc(HEAP_DEFAULT, sizeof(WindowWork));
    memset(wp, 0, sizeof(WindowWork));
}

void winReInit(void) {
    memset(wp, 0, sizeof(WindowWork));
}

BOOL winCheck(void) {
    s32 state;

    if (wp == NULL) {
        return FALSE;
    }

    if (wp->flags & 0x1000) {
        return FALSE;
    }

    state = wp->menuState;
    if (state != 0 && state < 200) {
        return TRUE;
    }
    return FALSE;
}

void winOpenEnable(void) {
    if (wp) {
        wp->flags &= ~1;
    }
}

void winOpenDisable(void) {
    if (wp) {
        wp->flags |= 1;
    }
}

#define WM_S32(off) (*(s32*)((u8*)wp + (off)))
#define WM_PARTY() (((s32*)((u8*)wp + 0x1BC))[WM_S32(0x1DC)])
#define WM_PTR(off) (*(void**)((u8*)wp + (off)))

extern WindowPartyDt winPartyDt[];
extern PartyData partyDataTbl[];
// evt_item_use (.data 0x803759D0) is part of this unit's still-unported .data (the script plus kime_pose*, str_8041DC5C..,
// evt_eff*); it is not defined in C yet.
extern s32 evt_item_use[];

// NONMATCHING (94.29%): calls and immediates match; the open-menu goto layout, the final characterId branches, and the pooled-string/names-table base register differ.
void winMain(void) {
    MarioWork* mario;
    WindowPartyDt* list[7];
    const char* names[2] = {"jp", "us"};

    mario = marioGetPtr();
    if (!(wp->flags & 1) && seqGetSeq() == SEQ_GAME && !seqCheckSeq()) {
        wp->buttonTrg = keyGetButtonTrg(0);
        wp->buttonRep = keyGetButtonRep(0);
        wp->dirTrg = keyGetDirTrg(0);
        wp->dirRep = keyGetDirRep(0);

        if (keyGetButtonTrg(0) & PAD_BUTTON_LEFT) {
            wp->dirTrg |= DEMO_STICK_LEFT;
        }
        if (keyGetButtonTrg(0) & PAD_BUTTON_RIGHT) {
            wp->dirTrg |= DEMO_STICK_RIGHT;
        }
        if (keyGetButtonTrg(0) & PAD_BUTTON_UP) {
            wp->dirTrg |= DEMO_STICK_UP;
        }
        if (keyGetButtonTrg(0) & PAD_BUTTON_DOWN) {
            wp->dirTrg |= DEMO_STICK_DOWN;
        }

        if (keyGetButtonRep(0) & PAD_BUTTON_LEFT) {
            wp->dirRep |= DEMO_STICK_LEFT;
        }
        if (keyGetButtonRep(0) & PAD_BUTTON_RIGHT) {
            wp->dirRep |= DEMO_STICK_RIGHT;
        }
        if (keyGetButtonRep(0) & PAD_BUTTON_UP) {
            wp->dirRep |= DEMO_STICK_UP;
        }
        if (keyGetButtonRep(0) & PAD_BUTTON_DOWN) {
            wp->dirRep |= DEMO_STICK_DOWN;
        }

        winLectureKeyMask();

        switch (wp->menuState) {
            case 0:
                if (!(wp->flags & 0x1000)) {
                    if ((u32)marioStGetSystemLevel() == 0 && !marioCheckMenuDisable() && !mobjCheckExec() && fadeIsFinish()) {
                        if (mario->characterId == 2) {
                            if (wp->buttonTrg & 0x1000) {
                                WM_S32(0x1C) = 0;
                                goto check_open;
                            }
                        } else {
                            if (wp->buttonTrg & 0x1000) {
                                WM_S32(0x1C) = 0;
                                goto check_open;
                            }
                            if (!(keyGetButton(0) & 0x10) && strcmp(gp->currentAreaName, "yuu") != 0) {
                                if ((wp->buttonTrg & 1) && (swByteGet(0) >= 7 || partyChkJoin(1))) {
                                    WM_S32(0x1C) = 1;
                                    goto check_open;
                                }
                                if (wp->buttonTrg & 8) {
                                    WM_S32(0x1C) = 2;
                                    goto check_open;
                                }
                                if ((wp->buttonTrg & 2) && (swByteGet(0) >= 0x14 || pouchGetHaveBadgeCnt())) {
                                    WM_S32(0x1C) = 3;
                                    goto check_open;
                                }
                                if (wp->buttonTrg & 4) {
                                    WM_S32(0x1C) = 4;
                                check_open:
                                    if (!(keyGetButton(0) & 0x40)) {
                                        goto open_menu;
                                    }
                                }
                            }
                        }
                    }
                } else {
                    WM_S32(0x1C) = 0;
                open_menu:
                    *(u16*)camGetPtr(CAMERA_2D) |= 0x200;
                    if (gp->flags & 0x1000) {
                        WM_S32(0x1C) = 1;
                    }
                    if (!(wp->flags & 0x1000)) {
                        marioStSystemLevel(4);
                    } else {
                        npcStopForEvent();
                    }
                    fbatChangeMode(7);
                    if (mario->characterId == 2) {
                        if (kpaGetStageViewType() == 0) {
                            marioCtrlOff();
                        } else {
                            marioCtrlOff2();
                        }
                    } else {
                        marioCtrlOff2();
                    }
                    partyCtrlOff();
                    psndSetFlag(0x80);
                    if (mario->characterId == 2) {
                        kpaPauseSE2();
                    }
                    if (mario->flags & 0x80000000) {
                        wp->flags |= 0x2000;
                    } else {
                        wp->flags &= ~0x2000;
                    }
                    {
                        s32 i = 0;
                        do {
                            padRumbleHardOff(i);
                            i++;
                        } while (i < 4);
                    }
                    fileAsyncf(4, 0, "%s/w/jp/mail.tpl", getMarioStDvdRoot());
                    {
                        const char* tpl = winGetMapTplName();
                        fileAsyncf(4, 0, "%s/w/jp/%s", getMarioStDvdRoot(), tpl);
                    }
                    WM_S32(0x24) = 0;
                    if (strncmp(gp->currentMapName, "yuu_", 4) == 0) {
                        wp->menuState = 0x190;
                    } else {
                        wp->menuState += 1;
                    }
                }
                break;

            case 1:
                {
                    s32 done;

                    if ((u32)fileAsyncf(4, 0, "%s/w/%s/win.tpl", getMarioStDvdRoot(), names[gp->language]) == 0) {
                        done = 0;
                    } else {
                        done = 1;
                        WM_PTR(0x28) = fileAllocf(4, "%s/w/%s/win.tpl", getMarioStDvdRoot(), names[gp->language]);
                    }
                    if (done != 0) {
                        wp->menuState += 1;
                    }
                }
                break;

            case 2:
                if (marioGetPtr()->characterId == 0) {
                    s32 r = winRootMain(wp);

                    switch (r) {
                        case -1:
                            wp->menuState += 1;
                            break;
                        case -2:
                            wp->menuState = 0x64;
                            break;
                        case -3:
                            if (WM_S32(0x2D4) == 0x4E) {
                                wp->menuState = 0x12C;
                            } else {
                                wp->menuState = 0xC8;
                            }
                            break;
                    }
                } else if (wp->buttonTrg & 0x1000) {
                    kpaPauseSE2();
                    wp->menuState = 3;
                }
                break;

            case 3:
                *(u16*)camGetPtr(CAMERA_2D) &= ~0x200;
                if (mario->characterId == 2) {
                    if (kpaGetStageViewType() == 0) {
                        marioCtrlOn();
                    } else {
                        marioCtrlOn2();
                    }
                } else {
                    marioCtrlOn2();
                }
                partyCtrlOn();
                fbatChangeMode(1);
                psndClearFlag(0x80);
                statusWinForceUpdate();
                if (!(wp->flags & 0x1000)) {
                    marioStSystemLevel(0);
                } else {
                    npcStartForEvent();
                }
                if (WM_PTR(0x28) != NULL) {
                    fileFree(WM_PTR(0x28));
                }
                WM_PTR(0x28) = NULL;
                wp->flags = 0;
                wp->menuState = 0;
                break;

            case 0x64:
                if (marioBgmodeChk()) {
                    mario = marioGetPtr();
                    mario->prevPartyMemberId[0] = WM_PARTY();
                    wp->menuState = 3;
                } else if (animGroupBaseAsync(partyDataTbl[WM_PARTY()].name, 2, NULL)) {
                    wp->menuState += 1;
                }
                break;

            case 0x65:
                marioPartyGoodbye();
                wp->menuState += 1;
                break;

            case 0x66:
                if (marioGetParty() == 0) {
                    marioPartyHello(WM_PARTY());
                    wp->menuState = 3;
                }
                break;

            case 0xC8: {
                s32 party;
                s32 cnt = 0;
                s32 i;

                if (marioBgmodeChk() == 0) {
                    party = marioGetParty();
                } else {
                    party = marioGetPtr()->prevPartyMemberId[0];
                }
                for (i = 0; i < 7; i++) {
                    if (party == winPartyDt[i].field_0x0) {
                        list[cnt++] = &winPartyDt[i];
                    }
                }
                for (i = 0; i < 7; i++) {
                    if (party != winPartyDt[i].field_0x0) {
                        list[cnt++] = &winPartyDt[i];
                    }
                }
                wp->currentEvent = evtEntryType(&evt_item_use, 0, 0, 0);
                *(s32*)((u8*)wp->currentEvent + 0xC4) = WM_S32(0x2D4);
                *(s32*)((u8*)wp->currentEvent + 0xC8) = (WM_S32(0x2DC) == 0) ? 0 : (*(s32**)&(list - 1)[WM_S32(0x2DC)])[0];
                psndClearFlag(0x80);
                wp->menuState += 1;
                pouchRemoveItemIndex(WM_S32(0x2D4), WM_S32(0x2D8));
                break;
            }

            case 0xC9:
                if (!evtCheckID(wp->currentEvent->eventId)) {
                    wp->menuState = 3;
                }
                break;

            case 0x12C:
                wp->currentEvent = evtEntryType(&evt_diary_read, 0, 0, 0);
                wp->menuState += 1;
                break;

            case 0x12D:
                if (!evtCheckID(wp->currentEvent->eventId)) {
                    wp->menuState = 3;
                }
                break;

            case 0x190:
                evtYuuWindow();
                wp->menuState += 1;
                break;

            case 0x191:
                if (evtYuuWindowEndChk()) {
                    wp->menuState = 3;
                }
                break;
        }

        if (wp->menuState == 2) {
            u8 cid = marioGetPtr()->characterId;
            s8 characterId = cid;
            if (characterId != 1) {
                if (characterId < 1) {
                    if (characterId < 0) {
                        return;
                    }
                    if (!(wp->flags & 0x1000)) {
                        dispEntry(CAMERA_2D, 250.0f, 0, winDisp, NULL);
                    } else {
                        dispEntry(CAMERA_2D, 500.0f, 0, winDisp, NULL);
                    }
                    return;
                }
                if (characterId < 3) {
                    dispEntry(CAMERA_2D, 250.0f, 0, winDispKoopa, NULL);
                }
            }
        }
    }
}

void winDisp(CameraId camId, void* param) {
    winRootDisp(camId, wp);
}

void winDispKoopa(CameraId camId, void* param) {
    winTexTpl = **(TEXPalette***)(*(u8**)((u8*)wp + 0x28) + 0xA0);
    GXSetCullMode(GX_CULL_NONE);
#ifdef MUST_MATCH
    winDispKoopaSetFog();
#else
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, dat_8041dc90);
#endif
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetNumTexGens(1u);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    winTexSet(0xB7, vec3_802f68c0, vec3_802f68cc, dat_8041dc00);
}

USER_FUNC(itemUseFunc) {
    static s32 cnt;
    PartyEntry* party;
    MarioWork* mario;
    ItemData* item;

    item = &itemDataTable[event->lwData[10]];
    mario = marioGetPtr();
    party = partyGetPtr(0);
    if (isFirstCall) {
        cnt = 0;
    }

    switch (cnt) {
        case 0:
            statusWinForceOpen();
            if (event->lwData[11] == 0) {
                POUCH_SETHP(POUCH_GETHP() + item->hp_restored);
                POUCH_SETFP(POUCH_GETFP() + item->fp_restored);
                POUCH_SETAP(POUCH_GETAP() + (u8)item->sp_restored);
            } else {
                POUCH_SETPARTYHP(event->lwData[11], POUCH_GETPARTYHP(event->lwData[11]) + item->hp_restored);
                POUCH_SETFP(POUCH_GETFP() + item->fp_restored);
                POUCH_SETAP(POUCH_GETAP() + (u8)item->sp_restored);
            }
            cnt++;
            break;

        case 1:
            if (event->lwData[11] != 0) {
                cnt = 0xC8;
                break;
            }
            if (item->hp_restored == 0) {
                cnt = 0x64;
                break;
            }
            effRecoveryEntry(0, item->hp_restored, mario->position.x, mario->position.y + mario->wPlayerCollisionBox.y, mario->position.z);
            cnt++;
            break;

        case 0x1F:
            cnt++;
            break;

        case 0x3C:
            cnt = 0x64;
            break;

        case 0x64:
            if (item->fp_restored == 0) {
                cnt = 0x3E7;
                break;
            }
            effRecoveryEntry(1, item->fp_restored, mario->position.x, mario->position.y + mario->wPlayerCollisionBox.y, mario->position.z);
            /* fallthrough */
        case 0x82:
            cnt = 0x3E7;
            break;

        case 0xC8: {
            f32* target;
            if (item->hp_restored == 0) {
                cnt = 0x12C;
                break;
            }
            target = (f32*)(*(u8**)((u8*)effRecoveryEntry(0, item->hp_restored, party->position.x, party->position.y + *(f32*)((u8*)party + 0xF0), party->position.z) + 0xC) + 0x40);
            *target = (-(*(f32*)&party->field_E8[8])) * float_0p5_8041dc98;
            cnt++;
            break;
        }

        case 0xE6:
            cnt++;
            break;

        case 0x104:
            cnt = 0x12C;
            break;

        case 0x12C:
            if (item->fp_restored == 0) {
                cnt = 0x3E7;
                break;
            }
            effRecoveryEntry(1, item->fp_restored, party->position.x, party->position.y + *(f32*)((u8*)party + 0xF0), party->position.z);
            cnt++;
            break;

        case 0x14A:
            cnt = 0x3E7;
            break;

        case 0x3E7:
        case 0x3E8:
            return 2;

        default:
            cnt++;
            break;
    }
    return 0;
}

USER_FUNC(itemUseFunc2) {
    statusWinForceOff();
    return EVT_RETURN_DONE;
}

USER_FUNC(famicom_check) {
    s32* args = event->args;

    if (wp->flags & 0x2000) {
        evtSetValue(event, args[0], 1);
    } else {
        evtSetValue(event, args[0], 0);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(party_color) {
    s32* args = event->args;
    s32 color = pouchGetPartyColor(PARTNER_YOSHI);

    evtSetValue(event, args[0], color);
    return EVT_RETURN_DONE;
}

s32 unk_80177cf8(void) {
    allPartyForceSlitOff();
    return 2;
}

s32 cam_r(void) {
    CameraEntry* cam = camGetPtr(CAMERA_3D);
    Vec v;
    f32 s;

    cam->rollAngle += 3.0f;
    s = sin((f32)gp->retraceCount / 10.0f);
    PSVECSubtract(&cam->cameraAt, &cam->cameraPos, &v);
    PSVECScale(&v, &v, 0.2f * s);
    cam->viewOffset.x = v.x;
    cam->viewOffset.y = v.y;
    cam->viewOffset.z = v.z;
    return 0;
}

s32 winGhostDiaryChk(void) {
    if (wp->menuState == 301) {
        return 1;
    }
    return seqGetSeq() == SEQ_GAME_OVER;
}

void winFontInit(void) {
    FontDrawStart();
}

void winFontSet(Vec* pos, Vec* scale, GXColor* color, const char* format, ...) {
    char buf[0x400];
    Mtx mtx1;
    Mtx mtx2;
    GXColor c;
    va_list args;

    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);
    PSMTXTrans(mtx1, pos->x, pos->y, pos->z);
    PSMTXScale(mtx2, scale->x, scale->y, scale->z);
    PSMTXConcat(mtx1, mtx2, mtx1);
    c = *color;
    FontDrawColor(&c);
    FontDrawMessageMtx(mtx1, buf);
}

void winFontSetR(Vec* pos, Vec* scale, GXColor* color, const char* format, ...) {
    char buf[0x400];
    Mtx mtx1;
    Mtx mtx2;
    GXColor c;
    s32 diff;
    va_list args;

    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);
    diff = FontGetMessageWidth("\202X\202X\202X") - FontGetMessageWidth(buf);
    PSMTXTrans(mtx1, pos->x + scale->x * (f32)diff, pos->y, pos->z);
    PSMTXScale(mtx2, scale->x, scale->y, scale->z);
    PSMTXConcat(mtx1, mtx2, mtx1);
    c = *color;
    FontDrawColor(&c);
    FontDrawStringMtx(mtx1, buf);
}

void winFontSetEdge(Vec* pos, Vec* scale, GXColor* color, const char* format, ...) {
    char buf[0x400];
    Mtx mtx1;
    Mtx mtx2;
    GXColor c;
    va_list args;

    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);
    PSMTXTrans(mtx1, pos->x, pos->y, pos->z);
    PSMTXScale(mtx2, scale->x, scale->y, scale->z);
    PSMTXConcat(mtx1, mtx2, mtx1);
    c = *color;
    FontDrawColor(&c);
    FontDrawEdge();
    FontDrawStringMtx(mtx1, buf);
    FontDrawEdgeOff();
}

// Font drawing helpers defined in manager/fontmgr.c but not declared in a header

void winFontSetLabel(Vec* pos, Vec* scale, GXColor* color, const char* tag) {
    Mtx mtx1;
    Mtx mtx2;
    GXColor c;

    if (tag != NULL) {
        PSMTXTrans(mtx1, pos->x, pos->y, pos->z);
        PSMTXScale(mtx2, scale->x, scale->y, scale->z);
        PSMTXConcat(mtx1, mtx2, mtx1);
        c = *color;
        FontDrawColor(&c);
        FontDrawMessageMtx(mtx1, msgSearch(tag));
    }
}

void winFontSetPitch(Vec* pos, Vec scale, GXColor* color, f32 pitch, const char* format, ...) {
    char buf[0x400];
    GXColor c;
    va_list args;
    Vec scaleCopy;

    va_start(args, format);
    vsprintf(buf, format, args);
    va_end(args);
    c = *color;
    FontDrawColor(&c);
    scaleCopy = scale;
    FontDrawScaleVec(scaleCopy);
    FontDrawStringVecPitch(pos, buf, pitch);
}

void winTexInit(TEXPalette* texture) {
    winTexTpl = texture;
    GXSetCullMode(GX_CULL_NONE);
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, dat_8041dc90);
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetNumTexGens(1u);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
}

void winTexSet(u32 textureId, Vec translate, Vec scale, GXColor color) {
    GXTexObj texObj;
    Mtx mtx1;
    Mtx mtx2;
    Mtx mtx3;
    f32 height;

    if (color.a != 0xFF) {
        GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
        GXSetZCompLoc(GX_TRUE);
        GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
        GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    } else {
        GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
        GXSetZCompLoc(GX_FALSE);
        GXSetAlphaCompare(GX_GEQUAL, 0x80, GX_AOP_OR, GX_NEVER, 0);
        GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    }

    TEXGetGXTexObjFromPalette(winTexTpl, &texObj, textureId);
    GXInitTexObjLOD(&texObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetTevColor(GX_TEVREG0, color);
    PSMTXTrans(mtx1, translate.x, translate.y, translate.z);
    height = scale.y * (f32)GXGetTexObjHeight(&texObj);
    PSMTXScale(mtx2, scale.x * (f32)GXGetTexObjWidth(&texObj), height, scale.z);
    PSMTXConcat(mtx1, mtx2, mtx3);
    PSMTXConcat(camGetPtr(CAMERA_2D)->view, mtx3, mtx3);
    GXLoadPosMtxImm(mtx3, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(-0.5f, -0.5f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(float_0p5_8041dc98, -0.5f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(float_0p5_8041dc98, float_0p5_8041dc98, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(-0.5f, float_0p5_8041dc98, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXEnd();
}

void unk_80177008(u32 textureId, Vec translate, Vec scale, GXColor color) {
    GXTexObj texObj;
    Mtx mtx1;
    Mtx mtx2;
    Mtx mtx3;
    f32 height;

    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetZCompLoc(GX_TRUE);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    TEXGetGXTexObjFromPalette(winTexTpl, &texObj, textureId);
    GXInitTexObjLOD(&texObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetTevColor(GX_TEVREG0, color);
    PSMTXTrans(mtx1, translate.x, translate.y, translate.z);
    height = scale.y * (f32)GXGetTexObjHeight(&texObj);
    PSMTXScale(mtx2, scale.x * (f32)GXGetTexObjWidth(&texObj), height, scale.z);
    PSMTXConcat(mtx1, mtx2, mtx3);
    PSMTXConcat(camGetPtr(CAMERA_2D)->view, mtx3, mtx3);
    GXLoadPosMtxImm(mtx3, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(-0.5f, -0.5f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(float_0p5_8041dc98, -0.5f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(float_0p5_8041dc98, float_0p5_8041dc98, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(-0.5f, float_0p5_8041dc98, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXEnd();
}

void winTexSetRot(u32 textureId, Vec translate, Vec scale, f32 rot, GXColor color) {
    GXTexObj texObj;
    Mtx mtx1;
    Mtx mtx2;
    Mtx mtx3;
    Mtx mtx4;
    f32 height;

    if (color.a != 0xFF) {
        GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
        GXSetZCompLoc(GX_TRUE);
        GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
        GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    } else {
        GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
        GXSetZCompLoc(GX_FALSE);
        GXSetAlphaCompare(GX_GEQUAL, 0x80, GX_AOP_OR, GX_NEVER, 0);
        GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    }

    TEXGetGXTexObjFromPalette(winTexTpl, &texObj, textureId);
    GXInitTexObjLOD(&texObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    GXSetTevColor(GX_TEVREG0, color);
    PSMTXTrans(mtx1, translate.x, translate.y, translate.z);
    PSMTXRotRad(mtx3, 'z', MTXDegToRad(rot));
    height = scale.y * (f32)GXGetTexObjHeight(&texObj);
    PSMTXScale(mtx2, scale.x * (f32)GXGetTexObjWidth(&texObj), height, scale.z);
    PSMTXConcat(mtx1, mtx3, mtx4);
    PSMTXConcat(mtx4, mtx2, mtx4);
    PSMTXConcat(camGetPtr(CAMERA_2D)->view, mtx4, mtx4);
    GXLoadPosMtxImm(mtx4, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(-0.5f, -0.5f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(float_0p5_8041dc98, -0.5f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(float_0p5_8041dc98, float_0p5_8041dc98, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(-0.5f, float_0p5_8041dc98, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXEnd();
}

void winTexInit_x2(TEXPalette* texture) {
    winTexTpl = texture;
    GXSetCullMode(GX_CULL_NONE);
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, dat_8041dc90);
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetNumTexGens(1u);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXSetNumTexGens(2u);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C);
    GXSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C);
    GXSetNumTevStages(2);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_C0, GX_CC_CPREV, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
}

void winTexSet_x2(u32 textureId, u32 textureId2, Vec translate, Vec scale, GXColor color) {
    GXTexObj texObj;
    Mtx mtx1;
    Mtx mtx2;
    Mtx mtx3;
    f32 height;

    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetZCompLoc(GX_TRUE);
    GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
    GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    TEXGetGXTexObjFromPalette(winTexTpl, &texObj, textureId);
    GXInitTexObjLOD(&texObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&texObj, GX_TEXMAP0);
    TEXGetGXTexObjFromPalette(winTexTpl, &texObj, textureId2);
    GXInitTexObjLOD(&texObj, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&texObj, GX_TEXMAP1);
    GXSetTevColor(GX_TEVREG0, color);
    PSMTXTrans(mtx1, translate.x, translate.y, translate.z);
    height = scale.y * (f32)GXGetTexObjHeight(&texObj);
    PSMTXScale(mtx2, scale.x * (f32)GXGetTexObjWidth(&texObj), height, scale.z);
    PSMTXConcat(mtx1, mtx2, mtx3);
    PSMTXConcat(camGetPtr(CAMERA_2D)->view, mtx3, mtx3);
    GXLoadPosMtxImm(mtx3, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(-0.5f, -0.5f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(float_0p5_8041dc98, -0.5f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(float_0p5_8041dc98, float_0p5_8041dc98, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(-0.5f, float_0p5_8041dc98, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXEnd();
}

void winIconInit(void) {
    winTexTpl = NULL;
    GXSetCullMode(GX_CULL_NONE);
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, dat_8041dc90);
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetNumTexGens(1u);
    GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
}

void winIconGrayInit(void) {
    winTexTpl = NULL;
    GXSetCullMode(GX_CULL_NONE);
#ifdef MUST_MATCH
    winIconGraySetFog();
#else
    GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, dat_8041dc90);
#endif
    GXSetNumChans(0);
    GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C, GX_FALSE, 0x7D);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
    GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_RED, GX_CH_RED, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP2, GX_CH_GREEN, GX_CH_GREEN, GX_CH_GREEN, GX_CH_ALPHA);
    GXSetTevSwapModeTable(GX_TEV_SWAP3, GX_CH_BLUE, GX_CH_BLUE, GX_CH_BLUE, GX_CH_ALPHA);
    GXSetTevColor(GX_TEVREG1, dat_8041dc94);
    GXSetNumTevStages(4);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C1, GX_CC_TEXC, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP1);
    GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_C1, GX_CC_TEXC, GX_CC_CPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP2);
    GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_C1, GX_CC_TEXC, GX_CC_CPREV);
    GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
    GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP3);
    GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
    GXSetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
    GXSetTevColorIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_C0, GX_CC_CPREV, GX_CC_ZERO);
    GXSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_A0, GX_CA_APREV, GX_CA_ZERO);
    GXSetTevSwapMode(GX_TEVSTAGE3, GX_TEV_SWAP0, GX_TEV_SWAP0);
}

void winIconSet(s32 iconId, Vec* translate, Vec* scale, GXColor* color) {
    GXTexObj texObj;
    Mtx mtx1;
    Mtx mtx2;
    Mtx mtx3;
    f32 height;

    if (iconId == 0x115) {
        iconId = 0x114;
    }
    (*(void(*)(GXTexObj*, u16))iconGetTexObj)(&texObj, iconId);
    GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
    GXLoadTexObj(&texObj, GX_TEXMAP0);

    if (color->a != 0xFF) {
        GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
        GXSetZCompLoc(GX_TRUE);
        GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
        GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    } else {
        GXSetBlendMode(GX_BM_NONE, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
        GXSetZCompLoc(GX_FALSE);
        GXSetAlphaCompare(GX_GEQUAL, 0x80, GX_AOP_OR, GX_NEVER, 0);
        GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
    }

    GXSetTevColor(GX_TEVREG0, *color);
    PSMTXTrans(mtx1, translate->x, translate->y, translate->z);
    height = scale->y * (f32)GXGetTexObjHeight(&texObj);
    PSMTXScale(mtx2, scale->x * (f32)GXGetTexObjWidth(&texObj), height, scale->z);
    PSMTXConcat(mtx1, mtx2, mtx3);
    PSMTXConcat(camGetPtr(CAMERA_2D)->view, mtx3, mtx3);
    GXLoadPosMtxImm(mtx3, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    GXPosition3f32(-0.5f, -0.5f, 0.0f);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(float_0p5_8041dc98, -0.5f, 0.0f);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(float_0p5_8041dc98, float_0p5_8041dc98, 0.0f);
    GXTexCoord2f32(1.0f, 0.0f);
    GXPosition3f32(-0.5f, float_0p5_8041dc98, 0.0f);
    GXTexCoord2f32(0.0f, 0.0f);
    GXEnd();
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
}

s32 unk_80175eb0(EventEntry* event, BOOL isFirstCall) {
    s32 value = evtGetValue(event, event->args[0]);

    if (isFirstCall) {
        wp->flags &= ~0x800;
    }
    if (*(s32*)wp->field_0x14 < value) {
        return 0;
    }
    wp->flags |= 0x800;
    return 2;
}

s32 unk_80175e98(void) {
    wp->flags &= ~0x800;
    return 2;
}

void winLectureKeyMask(void) {
    if (!(wp->flags & 0x1000)) {
        return;
    }
    if (wp->flags & 0x800) {
        wp->buttonTrg = 0;
        wp->buttonRep = 0;
        wp->dirTrg = 0;
        wp->dirRep = 0;
        return;
    }
    switch (*(s32*)wp->field_0x14) {
        case 0:
        case 1:
            wp->buttonTrg = 0;
            wp->buttonRep = 0;
            wp->dirTrg = 0;
            wp->dirRep = 0;
            if (keyGetButtonTrg(0) & PAD_BUTTON_A) {
                (*(s32*)wp->field_0x14)++;
            }
            break;

        case 2:
        case 3:
        case 4:
            wp->buttonTrg = 0;
            wp->buttonRep = 0;
            wp->dirTrg &= DEMO_STICK_RIGHT;
            wp->dirRep &= DEMO_STICK_RIGHT;
            if ((s32)wp->dirTrg != 0 || (s32)wp->dirRep != 0) {
                (*(s32*)wp->field_0x14)++;
            }
            break;

        case 5:
        case 6:
        case 7:
        case 8:
            wp->buttonTrg &= PAD_BUTTON_A;
            wp->buttonRep &= PAD_BUTTON_A;
            wp->dirTrg = 0;
            wp->dirRep = 0;
            if ((s32)wp->buttonTrg != 0 || (s32)wp->buttonRep != 0) {
                (*(s32*)wp->field_0x14)++;
            }
            break;

        case 9:
            wp->buttonTrg &= PAD_BUTTON_START;
            wp->buttonRep &= PAD_BUTTON_START;
            wp->dirTrg = 0;
            wp->dirRep = 0;
            if ((s32)wp->buttonTrg != 0 || (s32)wp->buttonRep != 0) {
                (*(s32*)wp->field_0x14)++;
            }
            break;
    }
}

void winLectureOn(void) {
    wp->flags |= 0x1000;
    wp->flags |= 0x800;
    *(s32*)wp->field_0x14 = 0;
    WP_CURRENT_EVENT(wp) = evtEntry(evt_lecture_msg, 0, 0);
}

s32 winLectureCheck(void) {
    if ((wp->flags & 0x1000) && evtCheckID(WP_CURRENT_EVENT(wp)->eventId)) {
        return 1;
    }
    wp->flags &= ~0x1000;
    return 0;
}

// TODO: type
s32 kime_pose[61] = {
    0x0003005B, (s32)evt_party_nokotaro_get_status, 0x00000000, 0xFE363C80,
    0x00020019, 0xFE363C80, 0x00000000, 0x00000002,
    0x00000021, 0x0002005B, (s32)evt_mario_get_party, 0xFE363C80,
    0x00010022, 0xFE363C80, 0x00010024, 0x00000000,
    0x00010024, 0x00000001, 0x0003005B, (s32)evt_party_set_pose,
    0x00000000, (s32)"PKR_I_1", 0x00010024, 0x00000002,
    0x0003005B, (s32)evt_party_set_pose, 0x00000000, (s32)"PNK_I_1",
    0x00010024, 0x00000003, 0x0003005B, (s32)evt_party_set_pose,
    0x00000000, (s32)"I_1", 0x00010024, 0x00000004,
    0x0003005B, (s32)evt_party_set_pose, 0x00000000, (s32)"PYS_I_1",
    0x00010024, 0x00000005, 0x0003005B, (s32)evt_party_set_pose,
    0x00000000, (s32)"PWD_I_1", 0x00010024, 0x00000006,
    0x0003005B, (s32)evt_party_set_pose, 0x00000000, (s32)"PTR_I_1",
    0x00010024, 0x00000007, 0x0003005B, (s32)evt_party_set_pose,
    0x00000000, (s32)"PCH_I_1", 0x00000031, 0x00000002,
    0x00000001,
};

// TODO: type
s32 kime_pose2[61] = {
    0x0003005B, (s32)evt_party_nokotaro_get_status, 0x00000000, 0xFE363C80,
    0x00020019, 0xFE363C80, 0x00000000, 0x00000002,
    0x00000021, 0x0002005B, (s32)evt_mario_get_party, 0xFE363C80,
    0x00010022, 0xFE363C80, 0x00010024, 0x00000000,
    0x00010024, 0x00000001, 0x0003005B, (s32)evt_party_set_pose,
    0x00000000, (s32)"PKR_Y_1", 0x00010024, 0x00000002,
    0x0003005B, (s32)evt_party_set_pose, 0x00000000, (s32)"PNK_Y_1",
    0x00010024, 0x00000003, 0x0003005B, (s32)evt_party_set_pose,
    0x00000000, (s32)"Y_1", 0x00010024, 0x00000004,
    0x0003005B, (s32)evt_party_set_pose, 0x00000000, (s32)"PYS_Y_1",
    0x00010024, 0x00000005, 0x0003005B, (s32)evt_party_set_pose,
    0x00000000, (s32)"PWD_Y_1", 0x00010024, 0x00000006,
    0x0003005B, (s32)evt_party_set_pose, 0x00000000, (s32)"PTR_Y_1",
    0x00010024, 0x00000007, 0x0003005B, (s32)evt_party_set_pose,
    0x00000000, (s32)"PCH_Y_1", 0x00000031, 0x00000002,
    0x00000001,
};

// TODO: type
s32 unk_evt_803757f0[15] = {
    0x0003005B, (s32)evt_party_nokotaro_get_status, 0x00000000, 0xFE363C80,
    0x00020019, 0xFE363C80, 0x00000000, 0x00000002,
    0x00000021, 0x0003005B, (s32)evt_party_set_breed_pose, 0x00000000,
    0x00000001, 0x00000002, 0x00000001,
};

// TODO: type
s32 kime_pose_famicom[105] = {
    0x0003005B, (s32)evt_party_nokotaro_get_status, 0x00000000, 0xFE363C80,
    0x00020019, 0xFE363C80, 0x00000000, 0x00000002,
    0x00000021, 0x0002005B, (s32)evt_mario_get_party, 0xFE363C80,
    0x00010022, 0xFE363C80, 0x00010024, 0x00000000,
    0x00010024, 0x00000001, 0x0003005B, (s32)evt_party_set_pose,
    0x00000000, (s32)"R_2", 0x00010024, 0x00000002,
    0x0003005B, (s32)evt_party_set_pose, 0x00000000, (s32)"R_3",
    0x00010024, 0x00000003, 0x0003005B, (s32)evt_party_set_pose,
    0x00000000, (s32)"R_13", 0x00010024, 0x00000004,
    0x0002005B, (s32)party_color, 0xFE363C80, 0x00010022,
    0xFE363C80, 0x00010024, 0x00000000, 0x0003005B,
    (s32)evt_party_set_pose, 0x00000000, (s32)"R_5", 0x00010024,
    0x00000001, 0x0003005B, (s32)evt_party_set_pose, 0x00000000,
    (s32)"R_6", 0x00010024, 0x00000002, 0x0003005B,
    (s32)evt_party_set_pose, 0x00000000, (s32)"R_7", 0x00010024,
    0x00000003, 0x0003005B, (s32)evt_party_set_pose, 0x00000000,
    (s32)"R_8", 0x00010024, 0x00000004, 0x0003005B,
    (s32)evt_party_set_pose, 0x00000000, (s32)"R_9", 0x00010024,
    0x00000005, 0x0003005B, (s32)evt_party_set_pose, 0x00000000,
    (s32)"R_10", 0x00010024, 0x00000006, 0x0003005B,
    (s32)evt_party_set_pose, 0x00000000, (s32)"R_11", 0x00000031,
    0x00010024, 0x00000005, 0x0003005B, (s32)evt_party_set_pose,
    0x00000000, (s32)"R_4", 0x00010024, 0x00000006,
    0x0003005B, (s32)evt_party_set_pose, 0x00000000, (s32)"R_12",
    0x00010024, 0x00000007, 0x0003005B, (s32)evt_party_set_pose,
    0x00000000, (s32)"R_15", 0x00000031, 0x00000002,
    0x00000001,
};

// TODO: type
s32 evt_item_use[240] = {
    0x00010063, 0x00000000, 0x0001005B, (s32)evt_mario_normalize,
    0x0001005B, (s32)unk_80177cf8, 0x00010009, 0x00000001,
    0x0001005B, (s32)evt_mario_adjust_dir, 0x0002005B, (s32)evt_mario_set_pose,
    (s32)"M_S_1", 0x0001005E, (s32)&unk_evt_803757f0, 0x00020018,
    0xFE363C8B, 0x00000000, 0x0005005B, (s32)evt_mario_get_pos,
    0x00000000, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00020035, 0xFE363C81, 0x00000032, 0x0009005B,
    (s32)evt_item_entry, (s32)"menu_dummy", 0xFE363C8A, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000011, 0xFFFFFFFF,
    0x00000000, 0x00020035, 0xFE363C81, 0x0000000A,
    0x000F005B, (s32)evt_eff, 0x00000000, (s32)"itemget",
    0x0000000A, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_ITEM_MARIO_USE1", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000000, 0x0002005B,
    (s32)evt_mario_set_pose, (s32)"M_I_1", 0x0001000A, 0x000003E8,
    0x0002005B, (s32)evt_item_delete, (s32)"menu_dummy", 0x0005005B,
    (s32)evt_mario_get_pos, 0x00000000, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_ITEM_RECOVERY_SHINE1",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000000,
    0x000F005B, (s32)evt_eff64, 0x00000000, (s32)"stardust2_n64",
    0x00000004, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000028, 0x00000028, 0x0000001E, 0x00000078,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0001000A, 0x00000578, 0x0002005B, (s32)evt_mario_set_pose,
    (s32)"M_V_2", 0x0001005B, (s32)itemUseFunc, 0x0001000A,
    0x000005DC, 0x0001005B, (s32)itemUseFunc2, 0x0001005B,
    (s32)evt_mario_set_normal_pose, 0x00000002, 0x00000021, 0x0002005B,
    (s32)evt_mario_get_party, 0xFE363C80, 0x00020018, 0xFE363C80,
    0xFE363C8B, 0x0005005B, (s32)evt_party_get_pos, 0x00000000,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00020035,
    0xFE363C81, 0x00000032, 0x0009005B, (s32)evt_item_entry,
    (s32)"menu_dummy", 0xFE363C8A, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000011, 0xFFFFFFFF, 0x00000000,
    0x00020035, 0xFE363C81, 0x0000000A, 0x000F005B,
    (s32)evt_eff, 0x00000000, (s32)"itemget", 0x0000000A,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x0006005B,
    (s32)evt_snd_sfxon_3d, (s32)"SFX_ITEM_MARIO_USE1", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000000, 0x0002005B, (s32)famicom_check,
    0xFE363C80, 0x00020018, 0xFE363C80, 0x00000000,
    0x0001005E, (s32)&kime_pose, 0x00000020, 0x0001005E,
    (s32)&kime_pose_famicom, 0x00000021, 0x0001000A, 0x000003E8,
    0x0002005B, (s32)evt_item_delete, (s32)"menu_dummy", 0x0005005B,
    (s32)evt_party_get_pos, 0x00000000, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_ITEM_RECOVERY_SHINE1",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000000,
    0x000F005B, (s32)evt_eff64, 0x00000000, (s32)"stardust2_n64",
    0x00000004, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000028, 0x00000028, 0x0000001E, 0x00000078,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x0001000A, 0x00000578, 0x0002005B, (s32)famicom_check,
    0xFE363C80, 0x00020018, 0xFE363C80, 0x00000000,
    0x0001005E, (s32)&kime_pose2, 0x00000020, 0x0001005E,
    (s32)&kime_pose_famicom, 0x00000021, 0x0001005B, (s32)itemUseFunc,
    0x0001000A, 0x000005DC, 0x0001005B, (s32)itemUseFunc2,
    0x0001005E, (s32)&unk_evt_803757f0, 0x0001005B, (s32)evt_mario_set_normal_pose,
    0x00000002, 0x00000021, 0x00000002, 0x00000001,
};

// TODO: type
s32 evt_diary_read[259] = {
    0x00010063, 0x00000000, 0x0003005B, (s32)evt_party_cont_onoff,
    0x00000000, 0x00000001, 0x00010009, 0x00000001,
    0x0001005B, (s32)evt_mario_normalize, 0x0005005B, (s32)evt_msg_print,
    0x00000000, (s32)"stg6_rsh_diary_01", 0x00000000, 0x00000000,
    0x0003005B, (s32)evt_msg_select, 0x00000000, (s32)"stg6_rsh_diary_01_yn",
    0x00020019, 0xFE363C80, 0x00000000, 0x0001005B,
    (s32)evt_msg_continue, 0x0003005B, (s32)evt_party_cont_onoff, 0x00000000,
    0x00000000, 0x00000002, 0x00000021, 0x0003005B,
    (s32)evt_msg_print_add, 0x00000000, (s32)"stg6_rsh_diary_02", 0x0003005B,
    (s32)evt_msg_select, 0x00000000, (s32)"stg6_rsh_diary_01_yn", 0x00020019,
    0xFE363C80, 0x00000000, 0x0001005B, (s32)evt_msg_continue,
    0x0003005B, (s32)evt_party_cont_onoff, 0x00000000, 0x00000000,
    0x00000002, 0x00000021, 0x0003005B, (s32)evt_msg_print_add,
    0x00000000, (s32)"stg6_rsh_diary_03", 0x0003005B, (s32)evt_msg_select,
    0x00000000, (s32)"stg6_rsh_diary_01_yn", 0x00020019, 0xFE363C80,
    0x00000000, 0x0001005B, (s32)evt_msg_continue, 0x0003005B,
    (s32)evt_party_cont_onoff, 0x00000000, 0x00000000, 0x00000002,
    0x00000021, 0x0001005B, (s32)evt_msg_continue, 0x0003005B,
    (s32)evt_snd_bgmoff_f, 0x00000200, 0x000007D0, 0x0003005B,
    (s32)evt_mario_dispflag_onoff, 0x00000001, 0x00000002, 0x0001005B,
    (s32)evt_mario_adjust_dir, 0x0002005B, (s32)evt_mario_set_pose, (s32)"M_L_2",
    0x0005005B, (s32)evt_msg_print, 0x00000000, (s32)"stg6_rsh_diary_04",
    0x00000000, 0x00000000, 0x0002005B, (s32)evt_mario_set_pose,
    (s32)"M_S_1", 0x0003005B, (s32)evt_npc_check, (s32)"\202\344\202\244\202\352\202\242",
    0xFE363C80, 0x00020018, 0xFE363C80, 0x00000000,
    0x0003005B, (s32)evt_npc_entry, (s32)"\202\344\202\244\202\352\202\242", (s32)"c_ghost_k",
    0x0005005B, (s32)evt_npc_set_position, (s32)"\202\344\202\244\202\352\202\242", 0x00000000,
    0xFFFFFC18, 0x00000000, 0x0003005B, (s32)evt_npc_set_tribe,
    (s32)"\202\344\202\244\202\352\202\242", (s32)"\202\344\202\244\202\352\202\242", 0x0003005B, (s32)evt_npc_set_anim,
    (s32)"\202\344\202\244\202\352\202\242", (s32)"O_1", 0x0004005B, (s32)evt_npc_set_autotalkpose,
    (s32)"\202\344\202\244\202\352\202\242", (s32)"O_2", (s32)"T_2", 0x00000021,
    0x0004005B, (s32)evt_npc_flag_onoff, 0x00000001, (s32)"\202\344\202\244\202\352\202\242",
    0x00040020, 0x0000006B, 0x00010005, 0x00000000,
    0x0005005B, (s32)evt_sub_intpl_msec_init, 0x0000000B, 0x000000B4,
    0x00000040, 0x000003E8, 0x00010003, 0x00000001,
    0x0001005B, (s32)evt_sub_intpl_msec_get_value, 0x0006005B, (s32)evt_npc_set_color,
    (s32)"\202\344\202\244\202\352\202\242", 0x000000FF, 0x000000FF, 0x000000FF,
    0xFE363C80, 0x00010009, 0x00000001, 0x00020018,
    0xFE363C81, 0x00000001, 0x00010004, 0x00000001,
    0x00000021, 0x0005005B, (s32)evt_sub_intpl_msec_init, 0x0000000B,
    0x00000040, 0x000000B4, 0x000003E8, 0x00010003,
    0x00000002, 0x0001005B, (s32)evt_sub_intpl_msec_get_value, 0x0006005B,
    (s32)evt_npc_set_color, (s32)"\202\344\202\244\202\352\202\242", 0x000000FF, 0x000000FF,
    0x000000FF, 0xFE363C80, 0x00010009, 0x00000001,
    0x00020018, 0xFE363C81, 0x00000001, 0x00010004,
    0x00000002, 0x00000021, 0x00000006, 0x0000006D,
    0x0003005B, (s32)evt_snd_bgmon, 0x00000200, (s32)"BGM_EVT_STG5_GHOST1",
    0x0005005B, (s32)evt_mario_get_pos, 0x00000000, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00020035, 0xFE363C81,
    0x00000014, 0x00020035, 0xFE363C82, 0x00000014,
    0x0005005B, (s32)evt_npc_set_position, (s32)"\202\344\202\244\202\352\202\242", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0006005B, (s32)evt_snd_sfxon_3d,
    0x000206A5, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x00000000, 0x0001000A, 0x000000C8, 0x0002005B,
    (s32)evt_mario_set_pose, (s32)"M_I_O", 0x0005005B, (s32)evt_msg_print,
    0x00000000, (s32)"stg6_rsh_diary_05", 0x00000000, (s32)"\202\344\202\244\202\352\202\242",
    0x0001000A, 0x0000012C, 0x0002005B, (s32)evt_snd_bgmoff,
    0x00000200, 0x0000006B, 0x0003005B, (s32)evt_env_blur_on,
    0x00000000, 0x00000000, 0x0000006D, 0x0003005B,
    (s32)evt_snd_sfxon, 0x000206A2, 0xFE363C83, 0x0000006B,
    0x0001005B, (s32)cam_r, 0x0000006D, 0x0001000A,
    0x00000FA0, 0x0006005B, (s32)evt_fade_set_mapchange_type, 0x00000001,
    0xFFFFFFFF, 0x00000258, 0xFFFFFFFF, 0xFFFFFFFF,
    0x0004005B, (s32)evt_seq_set_seq, 0x00000005, 0x00000000,
    0x00000000, 0x00000002, 0x00000001,
};

// TODO: type
s32 evt_lecture_msg[96] = {
    0x0001000A, 0x000003E8, 0x0005005B, (s32)evt_msg_print,
    0x00000000, (s32)"lecture_badge_00", 0x00000000, 0x00000000,
    0x0002005B, (s32)unk_80175eb0, 0x00000001, 0x0001005B,
    (s32)evt_msg_continue, 0x0005005B, (s32)evt_msg_print, 0x00000000,
    (s32)"lecture_badge_01", 0x00000000, 0x00000000, 0x0002005B,
    (s32)unk_80175eb0, 0x00000002, 0x0001005B, (s32)evt_msg_continue,
    0x0005005B, (s32)evt_msg_print, 0x00000000, (s32)"lecture_badge_02",
    0x00000000, 0x00000000, 0x0002005B, (s32)unk_80175eb0,
    0x00000005, 0x0001005B, (s32)evt_msg_continue, 0x0005005B,
    (s32)evt_msg_print, 0x00000000, (s32)"lecture_badge_03", 0x00000000,
    0x00000000, 0x0002005B, (s32)unk_80175eb0, 0x00000006,
    0x0001005B, (s32)evt_msg_continue, 0x0005005B, (s32)evt_msg_print,
    0x00000000, (s32)"lecture_badge_04", 0x00000000, 0x00000000,
    0x0002005B, (s32)unk_80175eb0, 0x00000007, 0x0001005B,
    (s32)evt_msg_continue, 0x0005005B, (s32)evt_msg_print, 0x00000000,
    (s32)"lecture_badge_05", 0x00000000, 0x00000000, 0x0002005B,
    (s32)unk_80175eb0, 0x00000008, 0x0001005B, (s32)evt_msg_continue,
    0x0005005B, (s32)evt_msg_print, 0x00000000, (s32)"lecture_badge_06",
    0x00000000, 0x00000000, 0x0002005B, (s32)unk_80175eb0,
    0x00000009, 0x0001005B, (s32)evt_msg_continue, 0x0005005B,
    (s32)evt_msg_print, 0x00000000, (s32)"lecture_badge_07", 0x00000000,
    0x00000000, 0x0002005B, (s32)unk_80175eb0, 0x0000000A,
    0x0001005B, (s32)evt_msg_continue, 0x0001000A, 0x000001F4,
    0x0001005B, (s32)unk_80175e98, 0x00000002, 0x00000001,
};

__declspec(section ".sdata2") const GXColor dat_8041dc90 = { 0xFF, 0xFF, 0xFF, 0xFF };
__declspec(section ".sdata2") const GXColor dat_8041dc94 = { 0x55, 0x55, 0x55, 0xFF };
__declspec(section ".sdata2") volatile f32 float_0p5_8041dc98 = 0.5f;

#ifdef MUST_MATCH
// The original pools both int->float conversion doubles ahead of the deferred functions' strings.
#pragma defer_codegen off
static f32 pool_i2f(s32 v) {
    return (f32)v;
}
static f32 pool_u2f(u32 v) {
    return (f32)v;
}
#pragma defer_codegen reset
#endif
// clang-format on


// 4-byte zero word that ends the unit's .data: explicit_zero_data + force_active keep the zero
// word instead of dropping it, and __declspec(section ".data") keeps it out of .bss.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".data") static u32 gap_05_8037631C_data[1] = {0};
#pragma force_active reset
#pragma explicit_zero_data reset

// 4-byte zero word that ends the unit's .sdata2 (same treatment as the .data gap).
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata2") static u32 gap_09_8041DCCC_sdata2[1] = {0};
#pragma force_active reset
#pragma explicit_zero_data reset
