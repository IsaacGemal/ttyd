#include "evt/evt_cmd.h"

#include "driver/camdrv.h"
#include "driver/hitdrv.h"
#include "driver/npcdrv.h"
#include "evt/evt_npc.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "mario/mario_pouch.h"
#include "mario/mario_sbr.h"
#include "mario/mariost.h"
#include "manager/evtmgr_cmd.h"
#include "party.h"
#include "system.h"

#include <math.h>

// Locally declared prototypes (move to headers later).
extern void mario_plane_cancel(void);
extern s32 marioGetPlaneStatus(void);
extern void marioChgTalkMotion(void);
extern void marioSetCamId(CameraId camId);
extern void marioInitCamId(void);
extern void marioUpdateCamPos(void);
extern void marioNoUpdateCamPos(void);
extern void camFollowYOn(void);
extern void camFollowYOff(void);
extern s32 partyKill(s32 memberId);
extern s32 partyKill2(s32 memberId);
extern s32 partyGoodbye(s32 memberId);
extern s32 partyEntry(s32 memberId);
extern s32 partyEntry2(s32 memberId);
extern s32 partyEntryPos(s32 memberId, f32 x, f32 y, f32 z);
extern s32 partyEntry2Pos(s32 memberId, f32 x, f32 y, f32 z);
extern s32 partyEntryHello(s32 memberId);
extern s32 partyEntry2Hello(s32 memberId);
extern s32 partyChkJoin(s32 memberId);
extern void partyChgRunMode(PartyEntry* party, s32 mode);
extern void partyKeyOn(void);
extern void partyKeyOff(void);
extern void marioBalloonOn(s32 type);
extern void marioBalloonOff(void);
extern void kpaSetLevel(s32 level);
extern s32 kpaGetLevel(void);
extern void peachTransformOn(void);
extern void peachTransformOff(void);
extern void peachSetStsNormal(void);
extern void peachSetStsAftershower(void);
extern void peachSetStsInvisible(void);
extern void peachSetStsHappy(void);
extern void peachSetStsUneasy(void);
extern void peachSetStsAngry(void);
extern void peachSetStsOrdinary(void);
extern void peachSetStsCarry(void);
extern s32 marioChkSlitThrouhEnd(void);
extern void unk_8009f9c8(void);
extern void marioChgMot(s32 motion);
extern f32 toMovedirSimple(f32 angle);
extern void marioSetForceMove(s32 frames, f32 angle, f32 speed);
extern void kpaAttackStart(void);
extern void kpaChgPose(const char* a, const char* b);
extern void partyShadowOff(PartyEntry* party);
extern void quake_kpaHipAttack(void);
extern s32 marioMakeJumpPara(void);
extern void marioClearJumpPara(void);
extern s32 marioSearchGround(f32* outY, f32* outX, f32* outZ, f32 a, f32 b);
extern s32 marioSearchGroundRoll(f32* outY, f32* outX, f32* outZ, f32 a, f32 b);

// .rodata
const char str_802E58F4[] = "M_N_5B";
const char str_802E58FC[] = "M_N_5A";
const Vec vec3_802e58e8 = {0.0f, 0.0f, 0.0f};
const char str_802E5910[] = "KPA_A_2D";
const char str_802E591C[] = "KPA2_J_1C";

const Vec vec3_802e5730[3] = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}};
f64 double_to_int_802e5908 = 4503599627370496.0;

// .sdata2
f32 float_0_8041b8ac = 0.0f;
f32 float_1_8041b8a8 = 1.0f;
f32 float_18p5_8041b8b0 = 18.5f;
f32 float_neg0p4744_8041b8b4 = -0.4744f;
f32 float_0p4704_8041b8b8 = 0.4704f;
f32 float_neg0p1904_8041b8bc = -0.1904f;
f32 float_11_8041b8c0 = 11.0f;
f32 float_1p5_8041b8c4 = 1.5f;
f32 float_0p5_8041b8c8 = 0.5f;
f32 float_20_8041b8cc = 20.0f;
f32 float_neg1000_8041b8d0 = -1000.0f;
f32 float_600_8041b8d4 = 600.0f;
f32 float_535_8041b8d8 = 535.0f;
f32 float_0p4_8041b8dc = 0.4f;
f32 float_220_8041b8e0 = 220.0f;
f32 float_140_8041b8e4 = 140.0f;
f32 float_90_8041b8e8 = 90.0f;
f32 float_270_8041b8ec = 270.0f;
f32 float_225_8041b8f0 = 225.0f;
f32 float_135_8041b8f4 = 135.0f;
f32 float_330_8041b8f8 = 330.0f;
f32 float_60_8041b8fc = 60.0f;
f32 float_120_8041b900 = 120.0f;
f32 float_180_8041b904 = 180.0f;
f32 float_240_8041b908 = 240.0f;
f32 float_300_8041b90c = 300.0f;
f32 float_30_8041b910 = 30.0f;
f32 float_360_8041b914 = 360.0f;
f32 float_neg16_8041b918 = -16.0f;
f32 float_neg8_8041b91c = -8.0f;
f32 float_0p1_8041b920 = 0.1f;

USER_FUNC(evt_mario_paper_plane) {
    s32 first = isFirstCall;
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    s32 arg = args[0];

    if (first) {
        if (arg == 0) {
            mario_plane_cancel();
        } else {
            marioChgMot(MARIO_MOTION_PLANE);
            mario->trigFlags |= MARIO_TRIG_FLAG_SHIP_PLANE_EVT_ON;
        }
    }

    if (arg == 0) {
        if (mario->currMotionId == MARIO_MOTION_STAY && mario->hitobjStandOn != NULL) {
            mario->trigFlags &= ~MARIO_TRIG_FLAG_SHIP_PLANE_EVT_ON;
            return EVT_RETURN_DONE;
        }
    } else if (marioGetPlaneStatus() == 2) {
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(L_evt_mario_keyon_wait) {
    marioGetPtr();
    return marioKeyOffChk() ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_get_name_hitobj_push) {
    s32 arg = event->args[0];
    const char* name = NULL;
    MarioWork* mario = marioGetPtr();

    if (mario->hitobjPush != NULL) {
        name = hitGetName(mario->hitobjPush);
    }
    evtSetValue(event, arg, (s32)name);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_get_name_hitobj_front) {
    s32 arg = event->args[0];
    const char* name = NULL;
    MarioWork* mario = marioGetPtr();
    void* hit = *(void**)((u8*)mario + 0x1E0);

    if (hit != NULL) {
        name = hitGetName(hit);
    }
    evtSetValue(event, arg, (s32)name);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_get_name_hitobj_under) {
    s32 arg = event->args[0];
    const char* name = NULL;
    MarioWork* mario = marioGetPtr();
    void* hit = *(void**)((u8*)mario + 0x1F8);

    if (hit != NULL) {
        name = hitGetName(hit);
    }
    evtSetValue(event, arg, (s32)name);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_get_name_hitobj_ride) {
    s32 arg = event->args[0];
    const char* name = NULL;
    MarioWork* mario = marioGetPtr();

    if (mario->hitobjStandOn != NULL) {
        name = hitGetName(mario->hitobjStandOn);
    }
    evtSetValue(event, arg, (s32)name);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_get_name_hitobj_head) {
    s32 arg = event->args[0];
    const char* name = NULL;
    MarioWork* mario = marioGetPtr();
    void* hit = *(void**)((u8*)mario + 0x1F4);

    if (hit != NULL) {
        name = hitGetName(hit);
    }
    evtSetValue(event, arg, (s32)name);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_dump_item) {
    s32* args = event->args;
    pouchRemoveItem(evtGetValue(event, args[0]));
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_check_key_item) {
    s32* args = event->args;
    s32 item = evtGetValue(event, args[0]);
    s32 dst = args[1];
    s32 held = pouchCheckItem(item);

    if (held != 0) {
        evtSetValue(event, dst, TRUE);
    } else {
        evtSetValue(event, dst, FALSE);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_sleep_off) {
    if (isFirstCall) {
        marioChgEvtPose(str_802E58F4);
        event->userdata[0] = 0x20;
    }
    if (--event->userdata[0] > 0) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_sleep_on) {
    marioChgEvtPose(str_802E58FC);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_push) {
    MarioWork* mario = marioGetPtr();
    mario->flags |= MARIO_FLAG_IS_PUSHING;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_clear_party) {
    MarioWork* mario = marioGetPtr();
    s32 slot = event->args[0];
    s8 memberId = mario->currPartySlotId[slot];

    if (memberId < 0) {
        return EVT_RETURN_DONE;
    }
    if (slot == 0) {
        partyKill(memberId);
    } else {
        partyKill2(1);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_kill_party) {
    MarioWork* mario = marioGetPtr();
    s32 slot = event->args[0];
    s8 memberId = mario->currPartySlotId[slot];

    if (memberId < 0) {
        return EVT_RETURN_DONE;
    }
    if (slot == 0) {
        partyKill(memberId);
    } else {
        partyKill(1);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_goodbye_party) {
    MarioWork* mario = marioGetPtr();
    s32 slot = event->args[0];

    if (isFirstCall) {
        s8 memberId = mario->currPartySlotId[slot];

        if (memberId < 0) {
            return EVT_RETURN_DONE;
        }
        partyGoodbye(memberId);
    }

    if (mario->currPartySlotId[slot] >= 0) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_set_prev_party_pos) {
    s32* args = event->args;
    s32 slot = args[0];
    s32 first = isFirstCall;
    MarioWork* mario = marioGetPtr();
    f32 x;
    f32 y;
    f32 z;
    s8 prev;
    s32 ret;

    if (first) {
        if (mario->currPartySlotId[slot] >= 0) {
            return EVT_RETURN_DONE;
        }

        x = evtGetValue(event, args[1]);
        y = evtGetValue(event, args[2]);
        z = evtGetValue(event, args[3]);
        prev = mario->prevPartyMemberId[slot];

        if (prev == 0) {
            return EVT_RETURN_DONE;
        }

        if (slot == 0) {
            ret = partyEntryPos(prev, x, y, z);
        } else {
            ret = partyEntry2Pos(prev, x, y, z);
        }
        if (ret < 0) {
            return EVT_RETURN_DONE;
        }
        mario->currPartySlotId[slot] = ret;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_set_prev_party) {
    s32 first = isFirstCall;
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    s32 slot = args[0];
    s8 prev;
    s32 ret;

    if (first) {
        if (mario->currPartySlotId[slot] >= 0) {
            return EVT_RETURN_DONE;
        }
        prev = mario->prevPartyMemberId[slot];
        if (prev == 0) {
            return EVT_RETURN_DONE;
        }

        if (slot == 0) {
            ret = partyEntry(prev);
        } else {
            ret = partyEntry2(prev);
        }
        if (ret < 0) {
            return EVT_RETURN_DONE;
        }
        mario->currPartySlotId[slot] = ret;
    }

    if (partyGetPtr(mario->currPartySlotId[slot]) == NULL) {
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_set_camid) {
    s32* args = event->args;

    marioSetCamId(evtGetValue(event, args[0]));
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_init_camid) {
    marioInitCamId();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_cam_y_off) {
    camFollowYOff();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_cam_y_on) {
    camFollowYOn();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_cam_keep_off) {
    camFollowYOn();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_cam_keep_on) {
    camFollowYOff();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_cam_off) {
    marioNoUpdateCamPos();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_cam_on) {
    marioUpdateCamPos();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_bgmode_off) {
    marioBgmodeOff();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_bgmode_on) {
    marioBgmodeOn();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_set_enable_key) {
    s32* args = event->args;
    s32 val = evtGetValue(event, args[0]);

    *(s32*)((u8*)marioGetPtr() + 0x284) = val;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_key_onoff) {
    s32* args = event->args;

    marioGetPtr();
    if (evtGetValue(event, args[0]) == 0) {
        marioKeyOff();
        partyKeyOff();
    } else {
        marioKeyOn();
        partyKeyOn();
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_cont_onoff) {
    s32* args = event->args;

    if (evtGetValue(event, args[0]) == 0) {
        marioCtrlOff();
    } else {
        marioCtrlOn();
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_trigflag_onoff) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();

    if (evtGetValue(event, args[0]) != 0) {
        mario->trigFlags |= evtGetValue(event, args[1]);
    } else {
        mario->trigFlags &= ~evtGetValue(event, args[1]);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_dispflag_onoff) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();

    if (evtGetValue(event, args[0]) != 0) {
        mario->dispFlags |= evtGetValue(event, args[1]);
    } else {
        mario->dispFlags &= ~evtGetValue(event, args[1]);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_flag_onoff) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();

    if (evtGetValue(event, args[0]) != 0) {
        mario->flags |= evtGetValue(event, args[1]);
    } else {
        mario->flags &= ~evtGetValue(event, args[1]);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_balloon_off) {
    MarioWork* mario = marioGetPtr();

    if (mario->dispFlags & 0x04000000) {
        marioBalloonOff();
        mario->dispFlags &= ~0x04000000;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_balloon_tenten) {
    MarioWork* mario = marioGetPtr();

    if (!(mario->dispFlags & 0x04000000)) {
        marioBalloonOn(1);
        mario->dispFlags |= 0x04000000;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_balloon_bikkuri) {
    MarioWork* mario = marioGetPtr();

    if (!(mario->dispFlags & 0x04000000)) {
        marioBalloonOn(0);
        mario->dispFlags |= 0x04000000;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_paper_pose_off) {
    marioPaperOff();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_set_talk_motion) {
    marioChgTalkMotion();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_adjust_dir) {
    marioAdjustMoveDir();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_peach_transform_gundan_off) {
    peachTransformOff();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_peach_transform_gundan_on) {
    peachTransformOn();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_peach_set_condition) {
    switch (evtGetValue(event, event->args[0])) {
        case 0:
            peachSetStsNormal();
            break;
        case 1:
            peachSetStsAftershower();
            break;
        case 2:
            peachSetStsInvisible();
            break;
        case 3:
            peachSetStsHappy();
            break;
        case 4:
            peachSetStsUneasy();
            break;
        case 5:
            peachSetStsAngry();
            break;
        case 6:
            peachSetStsOrdinary();
            break;
        case 7:
            peachSetStsCarry();
            break;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_koopa_set_level) {
    kpaSetLevel(evtGetValue(event, event->args[0]));
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_koopa_get_level) {
    s32* args = event->args;
    s32 level = kpaGetLevel();

    evtSetValue(event, args[0], level);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_set_mode) {
    marioSetCharMode(evtGetValue(event, event->args[0]));
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_get_mode) {
    s32 arg = event->args[0];
    MarioWork* mario = marioGetPtr();
    s8 value = mario->characterId;

    evtSetValue(event, arg, value);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_get_state) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    s32 arg = args[0];
    s32 state;

    if (marioChkSlitThrouhEnd()) {
        evtSetValue(event, arg, 2);
        return EVT_RETURN_DONE;
    }
    state = 1;
    if (mario->currMotionId == MARIO_MOTION_STAY) {
        state = 0;
    }
    evtSetValue(event, arg, state);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_wait_movable) {
    MarioWork* mario = marioGetPtr();

    return (mario->flags & MARIO_FLAG_KEY_OFF) ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_wait_move_end) {
    MarioWork* mario = marioGetPtr();

    return mario->forceMoveTimer ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

USER_FUNC(evt_koopa_weary) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();

    if (args[0] != 0) {
        *(u32*)(*(u8**)((u8*)mario + 0x28C)) |= 0x01000000;
    } else {
        *(u32*)(*(u8**)((u8*)mario + 0x28C)) &= ~0x01000000;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_koopa_chk_dead) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    u32* kpa = *(u32**)((u8*)mario + 0x28C);

    evtSetValue(event, args[0], (*kpa >> 28) & 1);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_set_force_dir) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();

    if (mario->airTimer > 0) {
        mario->flags |= MARIO_FLAG_USE_FORCE_DIRECTION;
        mario->unk1A4 = (f32)evtGetValue(event, args[0]);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_get_pose) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();

    evtSetValue(event, args[0], (s32)mario->animName);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_cancel_roll_motion) {
    MarioWork* mario;

    if (isFirstCall) {
        unk_8009f9c8();
    }
    mario = marioGetPtr();
    if (mario->flags & MARIO_FLAG_TUBE_MODE) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_chk_hipbump) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    s32 result = 0;
    s32 arg = args[0];

    if (!(mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION)) {
        if (marioGetPtr()->currMotionId == MARIO_MOTION_HIP_2) {
            result = 1;
        }
    }
    evtSetValue(event, arg, result);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_set_motion) {
    marioChgMot(evtGetValue(event, event->args[0]));
    return EVT_RETURN_DONE;
}
USER_FUNC(evt_mario_get_motion) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    s32 arg = args[0];

    if (mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION) {
        evtSetValue(event, arg, marioGetPtr()->prevMotionId);
    } else {
        evtSetValue(event, arg, marioGetPtr()->currMotionId);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_set_dispdir) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();

    *(f32*)((u8*)mario + 0x1AC) = reviseAngle(evtGetFloat(event, args[0]));
    mario->unk1A8 = *(f32*)((u8*)mario + 0x1AC);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_get_dispdir) {
    s32 arg = event->args[0];
    MarioWork* mario = marioGetPtr();

    evtSetFloat(event, arg, mario->unk1A8);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_set_reardir) {
    MarioWork* mario = marioGetPtr();

    marioAdjustMoveDir();
    mario->directionView = 0.0f;
    *(f32*)((u8*)mario + 0x19C) = 0.0f;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_get_dir) {
    s32 arg = event->args[0];
    MarioWork* mario = marioGetPtr();

    evtSetFloat(event, arg, mario->directionView);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_get_mov_spd) {
    s32 arg = event->args[0];
    MarioWork* mario = marioGetPtr();

    evtSetFloat(event, arg, mario->baseSpeed * (f32)gp->framerate);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_set_mov_spd) {
    f32 speed = evtGetFloat(event, event->args[0]);
    MarioWork* mario;

    speed = speed / (f32)gp->framerate;
    mario = marioGetPtr();
    mario->baseSpeed = speed;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_set_hosei_xyz) {
    MarioWork* mario = marioGetPtr();
    s32* args = event->args;

    *(f32*)((u8*)mario + 0x174) = evtGetFloat(event, args[0]);
    *(f32*)((u8*)mario + 0x178) = evtGetFloat(event, args[1]);
    *(f32*)((u8*)mario + 0x17C) = evtGetFloat(event, args[2]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_set_pos) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();

    mario->position.x = evtGetFloat(event, args[0]);
    mario->position.y = evtGetFloat(event, args[1]);
    mario->position.z = evtGetFloat(event, args[2]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_get_pos) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();

    if (evtGetValue(event, args[0]) == 0) {
        evtSetFloat(event, args[1], mario->position.x);
        evtSetFloat(event, args[2], mario->position.y);
        evtSetFloat(event, args[3], mario->position.z);
    } else {
        evtSetFloat(event, args[1], mario->position.x);
        evtSetFloat(event, args[2], mario->position.y - 1.0f);
        evtSetFloat(event, args[3], mario->position.z);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_set_dir) {
    s32* args = event->args;
    s32 first = isFirstCall;
    MarioWork* mario = marioGetPtr();
    f32 angle = evtGetFloat(event, args[0]);
    s32 frames = sysMsec2Frame(evtGetValue(event, args[1]));
    s32 loop = evtGetValue(event, args[2]);

    if (first) {
        event->userdata[0] = 0;
        event->userdata[2] = 0;
        event->userdataf[1] = mario->unk1A8;
        if (angle > 270.0f) {
            angle -= 360.0f;
        }
    }
    if (event->userdata[2] == 1) {
        mario->directionView = 0.0f;
        *(f32*)((u8*)mario + 0x19C) = 0.0f;
        return EVT_RETURN_DONE;
    }
    if (frames > 0) {
        f32 a = revise360(270.0f - angle);

        if (a > 270.0f) {
            a -= 360.0f;
        }
        mario->unk1A8 = revise360(event->userdataf[1] + (a - event->userdataf[1]) * ((f32)event->userdata[0] / (f32)frames));
        *(f32*)((u8*)mario + 0x1AC) = mario->unk1A8;
        mario->directionView = toMovedirSimple(mario->unk1A8);
        *(f32*)((u8*)mario + 0x19C) = mario->directionView;
        event->userdata[0]++;
        if ((s32)event->userdata[0] > frames) {
            if (loop == 0) {
                return 1;
            }
            event->userdata[2] = 1;
        }
        return EVT_RETURN_BLOCK;
    }
    mario->unk1A8 = revise360(270.0f - angle);
    *(f32*)((u8*)mario + 0x1AC) = mario->unk1A8;
    mario->directionView = toMovedirSimple(mario->unk1A8);
    *(f32*)((u8*)mario + 0x19C) = mario->directionView;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_mov_pos) {
    s32* args = event->args;
    s32 first = isFirstCall;
    MarioWork* mario = marioGetPtr();

    if (first) {
        f32 x = evtGetFloat(event, args[0]);
        f32 z = evtGetFloat(event, args[1]);

        event->userdata[0] = sysMsec2Frame(evtGetValue(event, args[2]));
        mario->directionView = angleABf(mario->position.x, mario->position.z, x, z);
        if (event->userdata[0] == 0) {
            event->userdata[0] = (s32)(distABf(mario->position.x, mario->position.z, x, z) / mario->baseSpeed);
            marioSetForceMove(event->userdata[0], mario->directionView, mario->baseSpeed);
        } else {
            marioSetForceMove(event->userdata[0], mario->directionView,
                              distABf(mario->position.x, mario->position.z, x, z) / (f32)event->userdata[0]);
        }
    }
    if (--event->userdata[0] < 0) {
        return 1;
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(evt_mario_mov_pos2) {
    s32* args = event->args;
    s32 first = isFirstCall;
    MarioWork* mario = marioGetPtr();
    f32 x;
    f32 z;
    f32 speed;
    f32 dist;

    x = evtGetFloat(event, args[0]);
    z = evtGetFloat(event, args[1]);
    speed = evtGetFloat(event, args[2]);

    if (first) {
        dist = distABf(mario->position.x, mario->position.z, x, z);
        mario->directionView = angleABf(mario->position.x, mario->position.z, x, z);
        if (mario->flags & MARIO_FLAG_BG_MODE) {
            speed *= 0.4f;
        }
        speed /= (f32)gp->framerate;
        marioSetForceMove((s32)(1.0f / speed + dist / speed), mario->directionView, speed);
    }
    if (mario->flags & MARIO_FLAG_FORCED_MOVEMENT) {
        return EVT_RETURN_BLOCK;
    }
    mario->position.x = x;
    mario->position.z = z;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_set_party_pos) {
    s32 first = isFirstCall;
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    s32 slot = args[0];
    f32 x;
    f32 y;
    f32 z;
    s32 member;
    s32 ret;

    if (first) {
        if (mario->currPartySlotId[slot] >= 0) {
            return EVT_RETURN_DONE;
        }
        member = evtGetValue(event, args[1]);
        x = evtGetValue(event, args[2]);
        y = evtGetValue(event, args[3]);
        z = evtGetValue(event, args[4]);
        if (slot == 0) {
            ret = partyEntryPos(member, x, y, z);
        } else {
            ret = partyEntry2Pos(member, x, y, z);
        }
        if (ret < 0) {
            return EVT_RETURN_DONE;
        }
        mario->currPartySlotId[slot] = ret;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_set_party) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    s32 slot = args[0];
    s32 member;
    s32 ret;

    if (isFirstCall) {
        if (mario->currPartySlotId[slot] >= 0) {
            return EVT_RETURN_DONE;
        }
        member = evtGetValue(event, args[1]);
        if (slot == 0) {
            ret = partyEntry(member);
        } else {
            ret = partyEntry2(member);
        }
        if (ret < 0) {
            return EVT_RETURN_DONE;
        }
        mario->currPartySlotId[slot] = ret;
    }

    if (partyGetPtr(mario->currPartySlotId[slot]) == NULL) {
        return EVT_RETURN_DONE;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_hello_exparty_pos) {
    s32 first = isFirstCall;
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();

    if (first) {
        s32 member;
        s32 ret;

        if (mario->currPartySlotId[1] >= 0) {
            return EVT_RETURN_DONE;
        }
        member = evtGetValue(event, args[0]);
        evtGetValue(event, args[1]);
        evtGetValue(event, args[2]);
        evtGetValue(event, args[3]);
        ret = partyEntry2Hello(member);
        if (ret < 0) {
            return EVT_RETURN_DONE;
        }
        mario->currPartySlotId[1] = ret;
        return EVT_RETURN_BLOCK;
    }

    {
        PartyEntry* party = partyGetPtr(1);

        if (party == NULL) {
            return EVT_RETURN_DONE;
        }
        if (party->flags & PARTY_FLAG_IS_ACTIVE && ((u8*)party)[0x34] == 1) {
            return EVT_RETURN_DONE;
        }
        return EVT_RETURN_BLOCK;
    }
}

USER_FUNC(evt_mario_hello_party) {
    s32* args = event->args;
    s32 first = isFirstCall;
    MarioWork* mario = marioGetPtr();
    s32 slot = args[0];

    if (first) {
        s32 member;
        s32 ret;

        if (mario->currPartySlotId[slot] >= 0) {
            return EVT_RETURN_DONE;
        }
        member = evtGetValue(event, args[1]);
        if (slot == 0) {
            ret = partyEntryHello(member);
        } else {
            ret = partyEntry2Hello(member);
        }
        if (ret < 0) {
            return EVT_RETURN_DONE;
        }
        mario->currPartySlotId[slot] = ret;
        return EVT_RETURN_BLOCK;
    }

    {
        PartyEntry* party = partyGetPtr(slot);

        if (party == NULL) {
            return EVT_RETURN_DONE;
        }
        if (party->flags & PARTY_FLAG_IS_ACTIVE && ((u8*)party)[0x34] == 1) {
            return EVT_RETURN_DONE;
        }
        return EVT_RETURN_BLOCK;
    }
}

USER_FUNC(evt_mario_set_prev_party_dokan) {
    s32 first = isFirstCall;
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    s32 slot = args[0];
    s8 prev;
    s32 ret;
    PartyEntry* party;

    if (first) {
        if (mario->currPartySlotId[slot] >= 0) {
            return EVT_RETURN_DONE;
        }
        prev = mario->prevPartyMemberId[slot];
        if (prev == 0) {
            return EVT_RETURN_DONE;
        }
        if (slot == 0) {
            ret = partyEntry(prev);
        } else {
            ret = partyEntry2(prev);
        }
        if (ret < 0) {
            return EVT_RETURN_DONE;
        }
        mario->currPartySlotId[slot] = ret;
        party = partyGetPtr(ret);
        partyChgRunMode(party, 0);
        party->flags |= 0x04000000;
        party->flags |= 0x02000000;
        *(Vec*)((u8*)party + 0x70) = vec3_802e58e8;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_party_use_check) {
    s32 arg = event->args[0];
    s32 result = 0;
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (party != NULL && (party->flags & PARTY_FLAG_IS_BEING_USED)) {
        result = party->currentMemberId;
    }
    evtSetValue(event, arg, result);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_chk_join_party) {
    s32* args = event->args;
    s32 out = args[0];
    s32 member = evtGetValue(event, args[1]);

    evtSetValue(event, out, partyChkJoin(member));
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_get_exparty) {
    s32 out = event->args[0];
    PartyEntry* party = partyGetPtr(marioGetExtraPartyId());

    evtSetValue(event, out, party ? party->currentMemberId : 0);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_get_party) {
    s32 out = event->args[0];
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    evtSetValue(event, out, party ? party->currentMemberId : 0);
    return EVT_RETURN_DONE;
}

USER_FUNC(N_evt_mario_party_door_halve_hitbox) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (party != NULL) {
        party->field_8 |= 0x00040000;
    }
    party = partyGetPtr(marioGetExtraPartyId());
    if (party != NULL) {
        party->field_8 |= 0x00040000;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_dokan_end) {
    MarioWork* mario = marioGetPtr();

    if (isFirstCall && mario->currMotionId == MARIO_MOTION_DOKAN) {
        *(s32*)((u8*)mario + 0x44) = 100;
        return EVT_RETURN_BLOCK;
    }
    if (mario->currMotionId == MARIO_MOTION_DOKAN) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_check_landon) {
    s32* args = event->args;

    if (marioGetPtr()->hitobjStandOn != NULL) {
        MarioWork* mario = marioGetPtr();
        s32 landed = 0;

        if (mario->currMotionId <= MARIO_MOTION_WALK || mario->currMotionId == MARIO_MOTION_DASH) {
            landed = 1;
        } else if (mario->currMotionId == MARIO_MOTION_ROLL && mario->airTimer == 0) {
            landed = 1;
        } else if (mario->currMotionId == MARIO_MOTION_SLIT && mario->airTimer == 0) {
            landed = 1;
        } else if (mario->currMotionId == MARIO_MOTION_SHIP && mario->airTimer == 0) {
            landed = 1;
        } else if (mario->currMotionId == MARIO_MOTION_YOSHI) {
            landed = 1;
        }

        if (landed) {
            evtSetValue(event, args[0], 1);
            return EVT_RETURN_YIELD;
        }
    }
    evtSetValue(event, args[0], 0);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_wait_rideon) {
    if (marioGetPtr()->hitobjStandOn != NULL) {
        MarioWork* mario = marioGetPtr();
        s32 landed = 0;

        if (mario->currMotionId <= MARIO_MOTION_WALK || mario->currMotionId == MARIO_MOTION_DASH) {
            landed = 1;
        } else if (mario->currMotionId == MARIO_MOTION_ROLL && mario->airTimer == 0) {
            landed = 1;
        } else if (mario->currMotionId == MARIO_MOTION_SLIT && mario->airTimer == 0) {
            landed = 1;
        } else if (mario->currMotionId == MARIO_MOTION_SHIP && mario->airTimer == 0) {
            landed = 1;
        } else if (mario->currMotionId == MARIO_MOTION_YOSHI) {
            landed = 1;
        }

        if (landed) {
            return EVT_RETURN_DONE;
        }
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(evt_mario_wait_landon) {
    if (marioGetPtr()->hitobjStandOn != NULL) {
        MarioWork* mario = marioGetPtr();
        s32 landed = 0;

        if (mario->currMotionId <= MARIO_MOTION_WALK || mario->currMotionId == MARIO_MOTION_DASH) {
            landed = 1;
        } else if (mario->currMotionId == MARIO_MOTION_ROLL && mario->airTimer == 0) {
            landed = 1;
        } else if (mario->currMotionId == MARIO_MOTION_SLIT && mario->airTimer == 0) {
            landed = 1;
        } else if (mario->currMotionId == MARIO_MOTION_SHIP && mario->airTimer == 0) {
            landed = 1;
        } else if (mario->currMotionId == MARIO_MOTION_YOSHI) {
            landed = 1;
        }

        if (landed) {
            return EVT_RETURN_DONE;
        }
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(unk_800e23e0) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    f32 x = evtGetValue(event, args[0]);
    f32 z = evtGetValue(event, args[1]);
    f32 camYaw = camGetPtr(CAMERA_3D)->viewYaw;
    f32 angle = revise360(angleABf(mario->position.x, mario->position.z, x, z) - camYaw);

    *(f32*)((u8*)mario + 0x1AC) = (angle < float_180_8041b904) ? float_180_8041b904 : float_0_8041b8ac;
    mario->directionView = angle;
    *(f32*)((u8*)mario + 0x19C) = angle;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_set_dir_npc) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    const char* name = (const char*)evtGetValue(event, args[0]);
    NpcEntry* npc = evtNpcNameToPtr(event, name);
    f32 camYaw;
    f32 angle;

    if (npc == NULL) {
        return EVT_RETURN_DONE;
    }
    camYaw = camGetPtr(CAMERA_3D)->viewYaw;
    angle = revise360(angleABf(mario->position.x, mario->position.z, npc->position.x, npc->position.z) - camYaw);
    *(f32*)((u8*)mario + 0x1AC) = (angle < float_180_8041b904) ? float_180_8041b904 : float_0_8041b8ac;
    *(f32*)((u8*)mario + 0x19C) = toMovedirSimple(*(f32*)((u8*)mario + 0x1AC));
    mario->directionView = *(f32*)((u8*)mario + 0x19C);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_koopa_fire) {
    s32 first = isFirstCall;
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();

    if (first) {
        event->userdata[0] = 0;
        event->userdata[2] = (s32)(f32)sysMsec2Frame(evtGetValue(event, args[0]));
        kpaAttackStart();
        marioChgMot(0);
    }

    if (event->userdata[0] != 0) {
        *(s32*)(*(u8**)((u8*)mario + 0x28C) + 0x144) = 0;
        return EVT_RETURN_YIELD;
    }
    if (--event->userdata[2] != 0) {
        *(s32*)(*(u8**)((u8*)mario + 0x28C) + 0x144) = 1;
        return EVT_RETURN_BLOCK;
    }
    *(s32*)(*(u8**)((u8*)mario + 0x28C) + 0x144) = 0;
    return EVT_RETURN_YIELD;
}

USER_FUNC(evt_mario_dokan_prepare) {
    s32* args = event->args;
    const Vec* vecs = vec3_802e5730;
    MarioWork* mario;
    PartyEntry* party;
    s32 type;

    mario = marioGetPtr();
    mario->flags |= 0x100;
    type = evtGetValue(event, args[0]);
    if (type == 4) {
        mario->scale = vecs[0];
        mario->dispFlags |= 0x1000;
        party = partyGetPtr(marioGetPartyId());
        if (party != NULL) {
            partyShadowOff(party);
            *(Vec*)((u8*)party + 0x70) = vecs[1];
            party->flags2 |= 1;
            party->flags |= 0x04000000;
            party->flags |= 0x02000000;
        }
        party = partyGetPtr(marioGetExtraPartyId());
        if (party != NULL) {
            partyShadowOff(party);
            *(Vec*)((u8*)party + 0x70) = vecs[2];
            party->flags2 |= 1;
            party->flags |= 0x04000000;
            party->flags |= 0x02000000;
        }
    } else if (type == 5) {
        mario->dispFlags |= 0x1000;
        partyShadowOff(partyGetPtr(marioGetPartyId()));
        partyShadowOff(partyGetPtr(marioGetExtraPartyId()));
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_koopa_hip_attack) {
    MarioWork* mario;
    s32 first = isFirstCall;
    s32* args = event->args;
    f32 x;
    f32 y;
    f32 z;
    f32 angle;
    f32 frames;
    f32 v0;
    f32 v1;
    f32 v2;

    mario = marioGetPtr();
    if (first) {
        event->userdata[0] = 0;
    }
    x = evtGetFloat(event, args[0]);
    y = evtGetFloat(event, args[1]);
    z = evtGetFloat(event, args[2]);
    angle = evtGetFloat(event, args[3]);
    frames = sysMsec2Frame(evtGetValue(event, args[4]));

    switch (event->userdata[0]) {
    case 0: {
        f32 dy;
        f32 d;
        f32 denom;
        f32 v;

        camFollowYOff();
        mario->baseSpeed = distABf(mario->position.x, mario->position.z, x, z) / frames;
        mario->directionView = angleABf(mario->position.x, mario->position.z, x, z);
        *(f32*)((u8*)mario + 0x19C) = mario->directionView;
        dy = y - mario->position.y;
        if (fabsf(dy) < float_1_8041b8a8) {
            d = dy + angle;
        } else if (dy >= float_0_8041b8ac) {
            d = dy + float_18p5_8041b8b0;
        } else {
            d = angle;
        }
        d = d + d;
        denom = frames * frames - frames;
        v = d / denom;
        *(f32*)((u8*)mario + 0x7C) = v * frames;
        *(f32*)((u8*)mario + 0x80) = -v;
        *(f32*)((u8*)mario + 0x84) = float_0_8041b8ac;
        *(f32*)((u8*)mario + 0x88) = float_0_8041b8ac;
        marioChgMot(8);
        event->userdata[2] = (s32)frames;
        event->userdata[0] = 1;
    }
        /* fallthrough */
    case 1:
        movePos(&mario->position.x, &mario->position.z, mario->baseSpeed, mario->directionView);
        mario->position.y += *(f32*)((u8*)mario + 0x7C);
        marioMakeJumpPara();
        if (--event->userdata[2] <= 0) {
            mario->baseSpeed = float_0_8041b8ac;
            *(f32*)((u8*)mario + 0x80) = float_neg0p4744_8041b8b4;
            *(f32*)((u8*)mario + 0x84) = float_0p4704_8041b8b8;
            *(f32*)((u8*)mario + 0x88) = float_neg0p1904_8041b8bc;
            *(f32*)((u8*)mario + 0x7C) = *(f32*)((u8*)mario + 0x80);
            kpaChgPose(str_802E5910, str_802E591C);
            event->userdata[0] = 2;
        }
        break;
    case 2: {
        f32 vy;
        void* ground;
        s32 landed = 0;

        movePos(&mario->position.x, &mario->position.z, mario->baseSpeed, mario->directionView);
        marioMakeJumpPara();
        vy = *(f32*)((u8*)mario + 0x7C);
        mario = marioGetPtr();
        if (mario->flags & MARIO_FLAG_TUBE_MODE) {
            ground = (void*)marioSearchGroundRoll(&v0, &v1, &v2, float_11_8041b8c0, *(f32*)((u8*)mario + 0x7C));
        } else {
            ground = (void*)marioSearchGround(&v0, &v1, &v2, vy, vy);
        }
        if (ground != NULL && mario->position.y + vy <= v0) {
            marioClearJumpPara();
            mario->hitobjStandOn = ground;
            mario->position.y = v0;
            mario->flags &= ~(MARIO_FLAG_PRE_BATTLE_PHYSICS | MARIO_FLAG_IS_CARRYING_BOBBERY |
                              MARIO_FLAG_ITEM_GET_DISABLED | MARIO_FLAG_IS_PUSHING);
            *(f32*)((u8*)mario + 0x128) = fabsf(distABf(mario->position.x, mario->position.z,
                                                        *(f32*)((u8*)mario + 0x11C), *(f32*)((u8*)mario + 0x124)));
            mario->airTimer = 0;
            *(s16*)((u8*)mario + 0x52) = 0;
            mario->flags &= ~MARIO_FLAG_STOP_AIR_MOVEMENT;
            camFollowYOn();
            landed = 1;
        }
        if (!landed) {
            mario->position.y += *(f32*)((u8*)mario + 0x7C);
        } else {
            quake_kpaHipAttack();
            event->userdata[0] = 3;
            event->userdata[2] = 10;
        }
        break;
    }
    case 3:
        if (--event->userdata[2] <= 0) {
            camFollowYOn();
            marioChgMot(0);
            return EVT_RETURN_YIELD;
        }
        break;
    }
    return EVT_RETURN_BLOCK;
}
