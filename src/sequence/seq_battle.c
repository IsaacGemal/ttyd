#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "battle/battle.h"
#include "battle/battle_information.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/effdrv.h"
#include "driver/extdrv.h"
#include "driver/fadedrv.h"
#include "driver/icondrv.h"
#include "driver/imgdrv.h"
#include "driver/itemdrv.h"
#include "driver/lightdrv.h"
#include "driver/mapdrv.h"
#include "driver/mobjdrv.h"
#include "driver/msgdrv.h"
#include "driver/npcdrv.h"
#include "driver/offscreendrv.h"
#include "driver/seqdrv.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "manager/filemgr.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "party.h"
#include "pmario_sound.h"
#include "seq/seq_title.h"
#include "seq/seq_battle.h"
#include "system.h"

// .rodata
const Vec vec3_802c4820 = { -200.0f, 100.0f, 100.0f };

// .sbss
s32 debug_battle_flag;

void seq_battleInit(SequenceWork* work) {
    NpcEntry* target;
    s32 mode;

    debug_battle_flag = gp->field_C;

    target = fbatGetPointer()->target;

    if (!(evtGetValue(NULL, 0xF5DE0180) >= 0x6D && evtGetValue(NULL, 0xF5DE0180) < 0x6F)) {
        if (evtGetValue(NULL, 0xF5DE0180) != 0x192) {
            psndBGMOff(0x400);
            psndBGMOn(1, *(char**)((u8*)target + 0x2F0));
        }
    }

    psndENVOff(0x400);
    padRumbleOff(0);
    evtStopAll(4);
    psndSetFlag(0x80);

    mode = *(s32*)((u8*)fbatGetPointer() + 0x10);
    switch (mode) {
        case 0x800000:
        case 0x1000000:
            if (debug_battle_flag == 0) {
                marioCtrlOff();
                partyCtrlOff();
                padRumbleOff(0);
            }
            break;
        default:
            break;
    }
}

void seq_battleExit(SequenceWork* work) {
    battle_exit();
}

// NONMATCHING (95.83%): `work` holds r31 instead of r30 and the .rodata base is the
// section symbol instead of vec3_802c4820; the rest is schedule/allocation noise.
void seq_battleMain(SequenceWork* work) {
    FieldBattleData* fbat;
    s32 mode;

    switch (work->state) {
        case 0:
            fadeEntry(0x16, 0x3E8, (GXColor){ 0, 0, 0, 0xFF });
            work->state++;
            work->field_0x10 = 0;
            break;

        case 1:
            if ((u32)fileAsyncf(4, 0, "%s/%s", getMarioStDvdRoot(), "battle/audience/audience_normal.tpl") == 0) {
                break;
            }
            if ((u32)fileAsyncf(4, 0, "%s/%s", getMarioStDvdRoot(), "battle/audience/audience_luigi.tpl") == 0) {
                break;
            }
            work->field_0x10++;
            if (!fadeIsFinish()) {
                break;
            }
            work->state++;
            break;

        case 2:
            pouchGetPtr();
            mode = *(s32*)((u8*)fbatGetPointer() + 0x10);
            switch (mode) {
                case 0x800000:
                case 0x1000000:
                    if (debug_battle_flag == 0) {
                        marioGetPtr()->flags &= ~1;
                    }
                    break;
                default:
                    if (debug_battle_flag == 0) {
                        marioCtrlOff();
                        partyCtrlOff();
                        marioGetPtr()->flags &= ~1;
                        padRumbleOff(0);
                    }
                    break;
            }
            *(u32*)((u8*)fbatGetPointer() + 0x570) = 0;
            work->state++;
            // fall through

        case 3:
            {
                u8* setup;
                PouchData* pouch;
                BOOL loadMap;

                fbat = fbatGetPointer();
                setup = *(u8**)((u8*)fbat->target + 0x230);
                pouch = pouchGetPtr();

                loadMap = TRUE;
                if (fbat->firstStrike.enabled != 0) {
                    s32 type = (s32)fbat->firstStrike.type;
                    if (type < 3) {
                        if (type >= 1) {
                            if (fbat->firstStrike.field_0x4 != 0) {
                                loadMap = FALSE;
                            }
                        }
                    }
                    if (loadMap == 0) {
                        (*(s32*)fbat->field_570)++;
                        if (*(s32*)fbat->field_570 < 0xC8) {
                            break;
                        }
                    }
                }

                switch (pouch->rank) {
                    case 0:
                        bmapLoad("bti_01", *(char**)(setup + 0x4));
                        break;
                    case 1:
                        bmapLoad("bti_02", *(char**)(setup + 0x4));
                        break;
                    case 2:
                        bmapLoad("bti_03", *(char**)(setup + 0x4));
                        break;
                    case 3:
                    default:
                        bmapLoad("bti_04", *(char**)(setup + 0x4));
                        break;
                }
            }
            work->state++;
            // fall through

        case 4:
            if (battle_init() != 0) {
                break;
            }
            work->state++;
            // fall through

        case 5:
            {
                Vec position;
                CameraEntry* camera;

                if (battleSeqEndCheck()) {
                    work->field_0x10 = 0;
                    work->state++;
                    if (gp->field_C != 0) {
                        work->state = 0xB;
                    }
                }
                marioGetPtr();
                camGetPtr(CAMERA_3D);
                position.x = 0.0f;
                position.y = 0.0f;
                position.z = 400.0f;
                if (_battleWorkPointer->flags & 0x100) {
                    position.z += 200.0f;
                }
                camera = camGetPtr(CAMERA_3D);
                psndSetPosDirListener(&position, camera->viewYaw);
            }
            break;

        case 6:
            if ((s32)++work->field_0x10 > 0x1E) {
                if ((gp->flags & 0x1000) && (gp->flags & 0x2000)) {
                    seqSetSeq(SEQ_GAME_OVER, (char*)1, 0);
                } else {
                    seqSetSeq(SEQ_GAME, (char*)1, 0);
                }
            }
            break;

        case 10:
            fadeEntry(0x16, 0xC8, (GXColor){ 0, 0, 0, 0xFF });
            work->state++;
            break;

        case 11:
            if (fadeIsFinish()) {
                gp->field_C = 0;
                seqSetSeq(SEQ_GAME_OVER, (char*)1, 0);
            }
            break;

        default:
            break;
    }
}

BOOL battle_init(void) {
    FieldBattleData* fbat;
    NpcEntry* target;
    PouchData* pouch;
    FieldBattleInfo* info;

    fbat = fbatGetPointer();
    target = fbat->target;
    pouch = pouchGetPtr();

    camSetMode(CAMERA_3D, 2);

    if (gp->field_C != 0) {
        const char* msg = DbgBtlSel_GetMsgDataPtr();
        if (msg != NULL) {
            msgLoad(msg, 0);
        } else {
            msgLoad("gon_11", 0);
        }
    }

    memClear(4);
    smartAutoFree(2);
    _battleWorkPointer = NULL;
    animPoseBattleInit();
    npcReleaseFieldNpc();
    gp->inBattle = 1;
    evtmgrReInit();
    psndSetFlag(0x10);

    info = (FieldBattleInfo*)((u8*)fbat + 0x20);
    BattleInformationInit(info);
    BattleInformationSetMode(info, 0);

    {
        PartyEntry* party = partyGetPtr(marioGetPartyId());
        if (party != NULL) {
            s8 id = party->currentMemberId;
            BattleUnitType kind = BattleTransPartyIdToUnitKind(id);
            if ((pouch->partyData[id].flags & 2) == 0) {
                BattleInformationSetParty(info, kind);
            }
        }
    }

    if (*(u32*)((u8*)fbat + 0x1C) & 1) {
        BattleInformationSetFirstAttack(info, 0);
    } else {
        switch (*(s32*)((u8*)fbat + 0x10)) {
            case 0x20000:
                BattleInformationSetFirstAttack(info, 1);
                break;
            case 0x40000:
                BattleInformationSetFirstAttack(info, 2);
                break;
            case 0x80000:
                BattleInformationSetFirstAttack(info, 3);
                break;
            case 0x100000:
                BattleInformationSetFirstAttack(info, 4);
                break;
            case 0x200000:
                BattleInformationSetFirstAttack(info, 5);
                break;
            case 0x400000:
                BattleInformationSetFirstAttack(info, 6);
                break;
            case 0x800000:
                BattleInformationSetFirstAttack(info, 7);
                break;
            case 0x1000000:
                BattleInformationSetFirstAttack(info, 8);
                break;
            case 0x10000000:
                BattleInformationSetFirstAttack(info, *(u32*)((u8*)fbat + 0x14));
                break;
            default:
                BattleInformationSetFirstAttack(info, 0);
                break;
        }
    }

    BattleInfomationSetBattleSetupInfo(info, &target->battleInfo);
    info->field_0x14[4] = *(u8*)((u8*)target + 0x2F4);
    info->field_0x14[6] = *(u8*)((u8*)target + 0x2F5);
    info->field_0x14[7] = *(u8*)((u8*)target + 0x2F6);
    BattleInit(info);
    return 0;
}

// NONMATCHING (96.30%): only the Vec copy schedule in craft->position =
// vec3_802c4820 differs: MWCC stores through r3 before the mtx setup.
void battle_exit(void) {
    LightEntry* craft;
    Mtx mtx;

    BattleEnd();
    itemReInit();
    iconReInit();
    extReset();
    bmapUnLoad();

    if (camGetPtr(CAMERA_3D)->flags & 4) {
        camSetMode(CAMERA_3D, 1);
    } else {
        camSetMode(CAMERA_3D, 3);
    }

    npcReset(1);
    mobjReset(1);
    offscreenReset(1);
    imgAutoRelease(1);
    effAutoRelease(1);
    animPoseAutoRelease(1);
    evtmgrReInit();
    npcRecoveryFieldNpc();
    gp->inBattle = 0;
    memClear(4);
    evtStartAll(4);

    if (debug_battle_flag == 0) {
        marioCtrlOn();
        partyCtrlOn();
        marioGetPtr()->flags |= 1;
    }

    L_psndBGM_stop();
    psndClearFlag(0x80);

    craft = lightGetPaperCraft();
    craft->position = vec3_802c4820;
    PSMTXScale(mtx, 3.0f, 3.0f, 3.0f);
    PSMTXMultVec(mtx, &craft->position, &craft->position);

    if (evtGetValue(NULL, 0xF5DE0180) >= 0x6D && evtGetValue(NULL, 0xF5DE0180) < 0x6F) {
        unk_800d7270(0x120, 0, 0x2EE, 1);
    } else if (evtGetValue(NULL, 0xF5DE0180) != 0x192 && seqGetNextSeq() != 5) {
        psndBGMOn(0x120, 0);
        psndENVOn(0x120, 0);
    }
}
