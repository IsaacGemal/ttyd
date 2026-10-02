#include "evt/evt_mario.h"
#include "mario/mario_motion.h"
#include "evt/evt_cmd.h"

#include "mario/mario_motion.h"
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
#include <string.h>
#include "mario/mario_hit.h"
#include "koopa.h"
#include "motion/mot_roll.h"
#include "mario/mario_cam.h"
#include "peach.h"
#include "motion/mot_plane.h"
#include "mario/mario_balloon.h"
#include "koopa_motion.h"
#include "motion/mot_slit.h"
#include "party/party_yoshi.h"
#include "party/party_vivian.h"
#include "party/party_motion.h"
#include "party/party_move.h"
#include "eff/eff_kemuri.h"

// Locally declared prototypes (move to headers later).

// Data symbols defined in this translation unit's assembly (see evt_mario.s).
extern const char* a_mario_group[60];
const char str_8041B750[] = "M_Z_1";
const char str_8041B760[] = "M_S_1";
const char str_8041B770[] = "M_W_1";
const char str_8041B778[] = "M_R_1";
const char str_802E5928[0x08] = "pik_00";
const char str_802E5930[0x08] = "muj_01";
const char str_802E5878[0x08] = "M_J_1C";

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
#ifdef MUST_MATCH
        // MUST_MATCH: the original copies each Vec with an unscheduled per-component load/store pair;
        // volatile on both sides stops MWCC from hoisting the third load above the second store.
        *(volatile Vec*)&mario->scale = *(volatile Vec*)&vecs[0];
#else
        mario->scale = vecs[0];
#endif
        mario->dispFlags |= 0x1000;
        party = partyGetPtr(marioGetPartyId());
        if (party != NULL) {
            partyShadowOff(party);
#ifdef MUST_MATCH
            *(volatile Vec*)((u8*)party + 0x70) = *(volatile Vec*)&vecs[1];
#else
            *(Vec*)((u8*)party + 0x70) = vecs[1];
#endif
            party->flags2 |= 1;
            party->flags |= 0x04000000;
            party->flags |= 0x02000000;
        }
        party = partyGetPtr(marioGetExtraPartyId());
        if (party != NULL) {
            partyShadowOff(party);
#ifdef MUST_MATCH
            *(volatile Vec*)((u8*)party + 0x70) = *(volatile Vec*)&vecs[2];
#else
            *(Vec*)((u8*)party + 0x70) = vecs[2];
#endif
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

USER_FUNC(evt_mario_paper_pose_dokan) {
    const u8* base = (const u8*)vec3_802e5730;
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    s32 type;
    PartyEntry* party;
    s32 ret;

    type = evtGetValue(event, args[0]);
    if (isFirstCall) {
        event->userdata[1] = 0;
        if (mario->currMotionId == 0x1d) {
            marioPaperOff();
            *(Vec*)((u8*)mario + 0xC8) = *(Vec*)(base + 0x90);
            *(Vec*)((u8*)mario + 0xBC) = *(Vec*)(base + 0x9C);
            *(Vec*)((u8*)mario + 0xB0) = *(Vec*)(base + 0xA8);
            *(Vec*)((u8*)mario + 0x98) = *(Vec*)(base + 0xB4);
        } else {
            switch (type) {
            case 4:
            case 5:
                event->userdata[1] = 20;
                ret = 2;
                break;
            case 1:
                if (!(mario->flags & 0x02000000)) {
                    marioUpdateCamPos();
                    camFollowYOn();
                }
                event->userdata[1] = 0;
                break;
            case 0:
                event->userdata[1] = 2;
                break;
            case 2:
                event->userdata[1] = 10;
                break;
            case 3:
                event->userdata[1] = 10;
                break;
            }
        }

        if (type == 2 || type == 3) {
        Vec pos;
        f32 zero;

        marioSlitForceCancel();
        marioRollForceCancel();
        partyShadowOff(partyGetPtr(marioGetPartyId()));
        partyShadowOff(partyGetPtr(marioGetExtraPartyId()));
        marioPaperOn((const char*)(base + 0x208));
        marioPaperLightOff();
        marioChgPaper((const char*)(base + 0x214));
        marioChgPose(str_8041B750);
        mario->dispFlags |= 0x8;
        mario->dispFlags |= 0x100;
        mario->unk1A8 = *(f32*)((u8*)mario + 0x1AC); // 0x1AC: field not declared in MarioWork
        zero = 0.0f;
        *(f32*)((u8*)mario + 0x2A0) = zero; // 0x2A0: field not declared in MarioWork
        pos = *(Vec*)(base + 0xC0);
        pos.z = (mario->flags & 0x02000000) ? -16.0f : zero;
        *(Vec*)((u8*)mario + 0x98) = pos;
        event->userdata[0] = 0;
        marioNoUpdateCamPos();
        if (!(mario->flags & 0x02000000)) {
            camFollowYOff();
        }
        } else if (type == 5) {
            partyGoodbye(marioGetPartyId());
            partyGoodbye(marioGetExtraPartyId());
        }
    }

    switch (event->userdata[1]) {
    case 2:
        marioSlitForceCancel();
        marioRollForceCancel();
        marioPaperOn((const char*)(base + 0x208));
        marioPaperLightOff();
        marioChgPaper((const char*)(base + 0x214));
        marioChgPose(str_8041B750);
        mario->dispFlags |= 0x8;
        mario->dispFlags |= 0x100;
        *(Vec*)((u8*)mario + 0xC8) = *(Vec*)(base + 0xCC);
        *(f32*)((u8*)mario + 0x2A0) = 30.0f;
        event->userdata[0] = 8;
        event->userdata[1] = 3;
        ret = 2;
        break;
    case 3:
        ret = 0;
        if (--event->userdata[0] <= 0) {
            event->userdata[0] = 10;
            event->userdata[1] = 4;
        }
        break;
    case 4:
        ret = 0;
        if (--event->userdata[0] <= 0) {
            ret = 2;
        }
        break;
    case 0: {
        Vec pos;

        *(Vec*)((u8*)mario + 0xC8) = *(Vec*)(base + 0xD8);
        pos = *(Vec*)(base + 0xE4);
        pos.z = (mario->flags & 0x02000000) ? -16.0f : 0.0f;
        *(Vec*)((u8*)mario + 0x98) = pos;
        if (!(mario->flags & 0x02000000)) {
            mario->position.z += -8.0f;
        }
        party = partyGetPtr(marioGetPartyId());
        if (party != NULL) {
            *(Vec*)((u8*)party + 0x58) = mario->position;
            *(Vec*)((u8*)party + 0x70) = mario->scale;
            *(Vec*)((u8*)party + 0x7C) = *(Vec*)(base + 0xF0);
            *(Vec*)((u8*)party + 0x64) = *(Vec*)(base + 0xFC);
            *(Vec*)((u8*)party + 0x88) = *(Vec*)(base + 0x108);
            *(f32*)((u8*)party + 0x8C) = *(f32*)((u8*)mario + 0xB4);
            *(f32*)((u8*)party + 0x80) = *(f32*)((u8*)mario + 0x9C);
            *(f32*)((u8*)party + 0x84) = *(f32*)((u8*)mario + 0xA0);
        }
        party = partyGetPtr(marioGetExtraPartyId());
        if (party != NULL) {
            *(Vec*)((u8*)party + 0x58) = mario->position;
            *(Vec*)((u8*)party + 0x70) = mario->scale;
            *(Vec*)((u8*)party + 0x7C) = *(Vec*)(base + 0x114);
            *(Vec*)((u8*)party + 0x64) = *(Vec*)(base + 0x120);
            *(Vec*)((u8*)party + 0x88) = *(Vec*)(base + 0x12C);
            *(f32*)((u8*)party + 0x8C) = *(f32*)((u8*)mario + 0xB4);
            *(f32*)((u8*)party + 0x80) = *(f32*)((u8*)mario + 0x9C);
            *(f32*)((u8*)party + 0x84) = *(f32*)((u8*)mario + 0xA0);
        }
        ret = 2;
        break;
    }
    case 10:
        *(f32*)((u8*)mario + 0x2A0) += 1.0f;
        if (*(f32*)((u8*)mario + 0x2A0) > 30.0f) {
            *(f32*)((u8*)mario + 0x2A0) = 30.0f;
        }
        event->userdata[0]++;
        if (event->userdata[0] > 10) {
            *(f32*)((u8*)mario + 0x1A8) += *(f32*)((u8*)mario + 0x2A0);
            *(f32*)((u8*)mario + 0x2A0) += 0.1f;
            if (*(f32*)((u8*)mario + 0x2A0) > 30.0f) {
                *(f32*)((u8*)mario + 0x2A0) = 30.0f;
            }
        }
        if (event->userdata[0] < sysMsec2Frame(300)) {
            ret = 0;
        } else {
            partyGoodbye(marioGetPartyId());
            partyGoodbye(marioGetExtraPartyId());
            ret = 2;
        }
        break;
    default:
        break;
    }

    if (ret == 2) {
        *(u8*)((u8*)mario + 0x3E) = type;
        marioChgMot(0x1d);
    }
    return ret;
}

USER_FUNC(evt_mario_dokan_end) {
    MarioWork* mario = marioGetPtr();

    if (isFirstCall && mario->currMotionId == MARIO_MOTION_DOKAN) {
        mario->currSubMotionId = 100;
        return EVT_RETURN_BLOCK;
    }
    if (mario->currMotionId == MARIO_MOTION_DOKAN) {
        return EVT_RETURN_BLOCK;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_paper_pose_off) {
    marioPaperOff();
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

USER_FUNC(evt_mario_balloon_tenten) {
    MarioWork* mario = marioGetPtr();

    if (!(mario->dispFlags & 0x04000000)) {
        marioBalloonOn(1);
        mario->dispFlags |= 0x04000000;
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

USER_FUNC(evt_mario_cont_onoff) {
    s32* args = event->args;

    if (evtGetValue(event, args[0]) == 0) {
        marioCtrlOff();
    } else {
        marioCtrlOn();
    }
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

USER_FUNC(evt_mario_set_enable_key) {
    s32* args = event->args;
    s32 val = evtGetValue(event, args[0]);

    *(s32*)((u8*)marioGetPtr() + 0x284) = val;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_bgmode_on) {
    marioBgmodeOn();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_bgmode_off) {
    marioBgmodeOff();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_cam_on) {
    marioUpdateCamPos();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_cam_off) {
    marioNoUpdateCamPos();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_cam_keep_on) {
    camFollowYOff();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_cam_keep_off) {
    camFollowYOn();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_cam_y_on) {
    camFollowYOn();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_cam_y_off) {
    camFollowYOff();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_init_camid) {
    marioInitCamId();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_set_camid) {
    s32* args = event->args;

    marioSetCamId(evtGetValue(event, args[0]));
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

USER_FUNC(evt_mario_set_pos) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();

    mario->position.x = evtGetFloat(event, args[0]);
    mario->position.y = evtGetFloat(event, args[1]);
    mario->position.z = evtGetFloat(event, args[2]);
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

USER_FUNC(evt_mario_set_mov_spd) {
    f32 speed = evtGetFloat(event, event->args[0]);
    MarioWork* mario;

    speed = speed / (f32)gp->framerate;
    mario = marioGetPtr();
    mario->baseSpeed = speed;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_get_mov_spd) {
    s32 arg = event->args[0];

    evtSetFloat(event, arg, marioGetPtr()->baseSpeed * (f32)gp->framerate);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_get_dir) {
    s32 arg = event->args[0];
    MarioWork* mario = marioGetPtr();

    evtSetFloat(event, arg, mario->directionView);
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
        f32 start;   // temps keep the target's separate fmuls+fadds (no fp_contract fusion)
        f32 delta;
        s32 n;       // temp keeps the increment result live without a reload, matching the allocator

        if (a > 270.0f) {
            a -= 360.0f;
        }
        start = event->userdataf[1];
        delta = (a - start) * ((f32)event->userdata[0] / (f32)frames);
        mario->unk1A8 = revise360(start + delta);
        *(f32*)((u8*)mario + 0x1AC) = mario->unk1A8;
        mario->directionView = toMovedirSimple(mario->unk1A8);
        *(f32*)((u8*)mario + 0x19C) = mario->directionView;
        n = event->userdata[0] + 1;
        event->userdata[0] = n;
        if (n > frames) {
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

USER_FUNC(evt_mario_set_reardir) {
    MarioWork* mario = marioGetPtr();

    marioAdjustMoveDir();
    mario->directionView = 0.0f;
    *(f32*)((u8*)mario + 0x19C) = 0.0f;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_get_dispdir) {
    s32 arg = event->args[0];
    MarioWork* mario = marioGetPtr();

    evtSetFloat(event, arg, mario->unk1A8);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_set_dispdir) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();

    *(f32*)((u8*)mario + 0x1AC) = reviseAngle(evtGetFloat(event, args[0]));
    mario->unk1A8 = *(f32*)((u8*)mario + 0x1AC);
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

USER_FUNC(evt_mario_adjust_dir) {
    marioAdjustMoveDir();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_set_normal_pose) {
    MarioWork* mario = marioGetPtr();
    s32 i;
    s32 found;
    const char* name;

    if (mario->currMotionId == MARIO_MOTION_PLANE || mario->currMotionId == MARIO_MOTION_SHIP ||
        mario->currMotionId == MARIO_MOTION_ROLL || mario->currMotionId == MARIO_MOTION_SLIT) {
        return EVT_RETURN_DONE;
    }
    if (mario->currMotionId == MARIO_MOTION_WALK) {
        name = str_8041B770;
    } else if (mario->currMotionId == MARIO_MOTION_DASH) {
        name = str_8041B778;
    } else {
        name = str_8041B760;
    }
    if (mario->flags & 0x80000000) {
        const char* dot = toDotMarioPose(name);

        if (dot == NULL) {
            return EVT_RETURN_DONE;
        }
        mario->animName = dot;
        mario->trigFlags |= 0x1000;
    } else {
        found = TRUE;
        for (i = 0; a_mario_group[i] != NULL; i++) {
            if (strcmp(name, a_mario_group[i]) == 0) {
                found = FALSE;
                break;
            }
        }
        if (!found) {
            mario->animName = name;
            mario->trigFlags |= 0x1000;
            mario->dispFlags &= ~0x10000000;
        } else {
            mario->animName = name;
            mario->trigFlags |= 0x1000;
            mario->dispFlags |= 0x10000000;
        }
    }
    if (mario->dispFlags & 0x01000000) {
        mario->trigFlags |= 0x8000;
    }
    return EVT_RETURN_DONE;
}

// Returns TRUE if `name` is one of the pose names listed in a_mario_group.
static inline s32 evt_mario_group_found(const char* name) {
    s32 found = TRUE;
    const char** group = a_mario_group;

    while (*group != NULL) {
        if (strcmp(name, *group) == 0) {
            found = FALSE;
            break;
        }
        group++;
    }
    return found;
}

USER_FUNC(evt_mario_set_pose) {
    s32 found;
    const char* name;
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();

    if (mario->currMotionId == MARIO_MOTION_PLANE || mario->currMotionId == MARIO_MOTION_SHIP ||
        mario->currMotionId == MARIO_MOTION_ROLL || mario->currMotionId == MARIO_MOTION_SLIT) {
        return EVT_RETURN_DONE;
    }
    name = (const char*)evtGetValue(event, args[0]);
    if (mario->flags & 0x80000000) {
        const char* dot = toDotMarioPose(name);

        if (dot == NULL) {
            return EVT_RETURN_DONE;
        }
        mario->animName = dot;
        mario->trigFlags |= 0x1000;
    } else {
        found = evt_mario_group_found(name);
        if (!found) {
            mario->animName = name;
            mario->trigFlags |= 0x1000;
            mario->dispFlags &= ~0x10000000;
        } else {
            mario->animName = name;
            mario->trigFlags |= 0x1000;
            mario->dispFlags |= 0x10000000;
        }
    }
    if (mario->dispFlags & 0x01000000) {
        mario->trigFlags |= 0x8000;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_normalize) {
    s32 ret = 0;
    MarioWork* mario = marioGetPtr();
    PartyEntry* party;

    if (isFirstCall) {
        party = partyGetPtr(marioGetPartyId());
        if (party != NULL) {
            party->field_37[0] = 0;
        }
        party = partyGetPtr(marioGetExtraPartyId());
        if (party != NULL) {
            party->field_37[0] = 0;
        }
        mario->trigFlags |= 0x80000000;
        event->userdata[1] = 0;
        {
            BOOL ctrl = (marioChkCtrl() == 0);

            if (ctrl) {
                event->userdata[1] = 1;
                marioCtrlOn();
                partyCtrlOn();
            }
        }
        event->userdata[0] = 0;
        if (mario->currMotionId != MARIO_MOTION_YOSHI && mario->currMotionId != MARIO_MOTION_ROLL &&
            mario->currMotionId != MARIO_MOTION_VIVIAN && !(mario->dispFlags & 0x01000000)) {
            marioChgMot(0);
            if (partyRideChk(partyGetPtr(marioGetPartyId())) != 0 ||
                partyRideChk(partyGetPtr(marioGetExtraPartyId())) != 0) {
                event->userdata[0] = 2;
            } else {
                if (event->userdata[1] != 0) {
                    marioCtrlOff();
                    partyCtrlOff();
                }
                mario->trigFlags &= ~0x80000000;
                return 2;
            }
        }
    }
    switch (event->userdata[0]) {
    case 0:
        ret = 0;
        if (mario->currMotionId == MARIO_MOTION_ROLL) {
            marioRollCancel();
            event->userdata[0] = 1;
        } else if (mario->currMotionId == MARIO_MOTION_SLIT) {
            if (mario->flags & MARIO_FLAG_PAPER_MODE) {
                motSlitCancel2();
                event->userdata[0] = 1;
            }
        } else if (mario->currMotionId == MARIO_MOTION_YOSHI) {
            if (yoshiGetStatus() != 2) {
                if (marioGetoffYoshi() != 0) {
                    event->userdata[0] = 3;
                }
            }
        } else if (mario->currMotionId == MARIO_MOTION_VIVIAN) {
            if (vivianGetStatus() != 0) {
                if (vivianGetStatus() == 2) {
                    vivianUnhold(partyGetPtr(marioGetPartyId()));
                    event->userdata[0] = 5;
                }
            }
        } else if (mario->currMotionId == MARIO_MOTION_STAY) {
            if (!(mario->dispFlags & 0x01000000)) {
                ret = 2;
            }
        }
        break;
    case 1:
        ret = 0;
        if (mario->currMotionId == MARIO_MOTION_ROLL) {
            if (mario->field_0x8 != 0) {
                break;
            }
            event->userdata[0] = 2;
        } else if (mario->currMotionId == MARIO_MOTION_SLIT) {
            if (marioGetMotSlitCancel3() != 0) {
                ret = 0;
            } else {
                ret = 2;
            }
        } else {
            ret = 2;
        }
        break;
    }
    switch (event->userdata[0]) {
    case 0:
    case 1:
        break;
    case 2:
        ret = 0;
        if (partyRideChk(partyGetPtr(marioGetPartyId())) == 0) {
            if (partyRideChk(partyGetPtr(marioGetExtraPartyId())) == 0) {
                ret = 2;
            }
        }
        break;
    case 3:
        if (mario->currMotionId != MARIO_MOTION_YOSHI) {
            event->userdata[0] = 4;
        } else if (yoshiGetStatus() == 0) {
            event->userdata[0] = 4;
        }
        break;
    case 4:
        if (mario->hitobjStandOn != NULL) {
            event->userdata[0] = 10;
            event->userdata[2] = 3;
        }
        break;
    case 5:
        if (mario->currMotionId != MARIO_MOTION_VIVIAN) {
            event->userdata[0] = 6;
        } else if (vivianGetStatus() == 0) {
            event->userdata[0] = 6;
        }
        break;
    case 6:
        if (mario->hitobjStandOn != NULL) {
            event->userdata[0] = 10;
            event->userdata[2] = 3;
        }
        break;
    case 10:
        if (--event->userdata[2] > 0) {
            ret = 2;
        }
        break;
    }
    if (ret == 2) {
        mario->trigFlags &= ~0x80000000;
        if (event->userdata[1] != 0) {
            marioCtrlOff();
            partyCtrlOff();
        }
    }
    return ret;
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

USER_FUNC(evt_mario_set_motion) {
    marioChgMot(evtGetValue(event, event->args[0]));
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

USER_FUNC(evt_mario_set_talk_motion) {
    marioChgTalkMotion();
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

USER_FUNC(evt_mario_get_pose) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();

    evtSetValue(event, args[0], (s32)mario->animName);
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

USER_FUNC(evt_mario_mov_pos) {
    s32* args = event->args;
    s32 first = isFirstCall;
    MarioWork* mario = marioGetPtr();

    if (first) {
        f32 x = evtGetFloat(event, args[0]);
        f32 z = evtGetFloat(event, args[1]);
        f32 speed;

        event->userdata[0] = sysMsec2Frame(evtGetValue(event, args[2]));
        mario->directionView = angleABf(mario->position.x, mario->position.z, x, z);
        if (event->userdata[0] == 0) {
            event->userdata[0] = (s32)(distABf(mario->position.x, mario->position.z, x, z) / mario->baseSpeed);
            speed = mario->baseSpeed;
        } else {
            speed = distABf(mario->position.x, mario->position.z, x, z) / (f32)event->userdata[0];
        }
        marioSetForceMove(event->userdata[0], mario->directionView, speed);
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

// The case-0 first argument must be built as a named float product, cast to s32, and only then is
// `speed` divided by the framerate: this keeps the two `(f32)gp->framerate` conversions separate and
// puts the magic double in f2 (the #else order lets MWCC coalesce the third argument into f2 and
// land the double in f5). See evt_party_move_pos2 for the same idiom.
USER_FUNC(evt_mario_party_door_move) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    f32 x = evtGetFloat(event, args[0]);
    f32 z = evtGetFloat(event, args[1]);
    f32 speed = evtGetFloat(event, args[2]);
    f32 dist;
    f32 tmp;
    f32 sx;
    f32 sy;
    f32 sz;
    f32 viewYaw;
    f32 angle;
    PartyEntry* party;
    s32 ret;

    if (isFirstCall) {
        party = partyGetPtr(marioGetPartyId());
        if (party != NULL) {
            marioGetScreenPos(party->position, &sx, &sy, &sz);
            if (!(0.0f <= sx && sx <= float_600_8041b8d4 && 0.0f <= sy && sy <= float_535_8041b8d8)) {
                party->position = mario->position;
            }
            if (mario->position.y > party->position.y) {
                party->position = mario->position;
            }
            party->field_8 |= 0x00040000;
            N_partyFollowCloseOn(party);
            party->flags |= 0x01000000;
            party->field_8 |= 0x00200000;
        }
        party = partyGetPtr(marioGetExtraPartyId());
        if (party != NULL) {
            marioGetScreenPos(party->position, &sx, &sy, &sz);
            if (!(0.0f <= sx && sx <= float_600_8041b8d4 && 0.0f <= sy && sy <= float_535_8041b8d8)) {
                party->position = mario->position;
            }
            if (mario->position.y > party->position.y) {
                party->position = mario->position;
            }
            party->field_8 |= 0x00040000;
            N_partyFollowCloseOn(party);
            party->flags |= 0x01000000;
            party->field_8 |= 0x00200000;
        }
        event->userdata[0] = 0;
        event->userdata[2] = 0;
    }

    ret = 0;
    switch (event->userdata[0]) {
    case 0:
        dist = distABf(mario->position.x, mario->position.z, x, z);
        mario->directionView = angleABf(mario->position.x, mario->position.z, x, z);
        if (mario->flags & MARIO_FLAG_BG_MODE) {
            speed *= float_0p4_8041b8dc;
        }
        {
#ifdef MUST_MATCH
            // MUST_MATCH: the named float product cast to int, then the speed division, is what
            // separates the two framerate conversions and lands the double in f2.
            f32 fr = (f32)gp->framerate * (dist / speed);
            s32 n;

            n = (s32)fr;
            speed = speed / (f32)gp->framerate;
            marioSetForceMove(n, mario->directionView, speed);
#else
            tmp = dist / speed;
            speed = speed / (f32)gp->framerate;
            marioSetForceMove((s32)((f32)gp->framerate * tmp), mario->directionView, speed);
#endif
        }
        event->userdata[0] = 2;
        break;
    case 2:
        if (mario->flags & MARIO_FLAG_FORCED_MOVEMENT) {
            break;
        }
        event->userdata[1] = sysMsec2Frame(10);
        event->userdata[0] = 4;
        /* fallthrough */
    case 4:
        if (--event->userdata[1] > 0) {
            break;
        }
        event->userdata[1] = sysMsec2Frame(400);
        event->userdata[0] = 6;
        viewYaw = camGetPtr(CAMERA_3D)->viewYaw;
        party = partyGetPtr(marioGetPartyId());
        if (party != NULL) {
            if (strcmp(gp->currentMapName, str_802E5928) == 0) {
                angle = party->position.x > float_0_8041b8ac ? float_270_8041b8ec : float_90_8041b8e8;
            } else if (strcmp(gp->currentMapName, str_802E5930) == 0) {
                angle = float_330_8041b8f8;
            } else if (viewYaw > float_60_8041b8fc && viewYaw < float_120_8041b900) {
                angle = float_180_8041b904;
            } else if (viewYaw > float_240_8041b908 && viewYaw < float_300_8041b90c) {
                angle = float_180_8041b904;
            } else {
                angle = float_270_8041b8ec;
            }
            partyMoveBehindMario3(party, 0, angle);
            if (party->field_32 == 1) {
                party->flags2 |= 0x10;
            }
        }
        party = partyGetPtr(marioGetExtraPartyId());
        if (party != NULL) {
            if (strcmp(gp->currentMapName, str_802E5928) == 0) {
                angle = party->position.x > float_0_8041b8ac ? float_270_8041b8ec : float_90_8041b8e8;
            } else if (strcmp(gp->currentMapName, str_802E5930) == 0) {
                angle = float_30_8041b910;
            } else {
                angle = float_270_8041b8ec;
            }
            partyMoveBehindMario3(party, 0, angle);
            if (party->field_32 == 1) {
                party->flags2 |= 0x10;
            }
        }
        break;
    case 6:
        if (--event->userdata[1] > 0) {
            break;
        }
        viewYaw = camGetPtr(CAMERA_3D)->viewYaw;
        party = partyGetPtr(marioGetPartyId());
        if (party != NULL) {
            if (strcmp(gp->currentMapName, str_802E5928) == 0) {
                angle = party->position.x > float_0_8041b8ac ? float_270_8041b8ec : float_90_8041b8e8;
            } else if (strcmp(gp->currentMapName, str_802E5930) == 0) {
                angle = float_330_8041b8f8;
            } else if (viewYaw > float_60_8041b8fc && viewYaw < float_120_8041b900) {
                angle = float_180_8041b904;
            } else if (viewYaw > float_240_8041b908 && viewYaw < float_300_8041b90c) {
                angle = float_180_8041b904;
            } else {
                angle = float_270_8041b8ec;
            }
            partyMoveBehindMario3(party, 0, angle);
            if (party->field_32 == 1) {
                party->flags2 |= 0x10;
            }
            N_partyFollowCloseOff(party);
            party->flags &= ~0x01000000;
            party->field_8 &= ~0x00040000;
        }
        party = partyGetPtr(marioGetExtraPartyId());
        if (party != NULL) {
            if (strcmp(gp->currentMapName, str_802E5928) == 0) {
                angle = party->position.x > float_0_8041b8ac ? float_270_8041b8ec : float_90_8041b8e8;
            } else if (strcmp(gp->currentMapName, str_802E5930) == 0) {
                angle = float_30_8041b910;
            } else {
                angle = float_270_8041b8ec;
            }
            partyMoveBehindMario3(party, 0, angle);
            if (party->field_32 == 1) {
                party->flags2 |= 0x10;
            }
            N_partyFollowCloseOff(party);
            party->flags &= ~0x01000000;
            party->field_8 &= ~0x00040000;
        }
        ret = 2;
        break;
    }
    return ret;
}

USER_FUNC(evt_mario_party_bero_move) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    f32 x = evtGetFloat(event, args[0]);
    f32 z = evtGetFloat(event, args[1]);
    f32 speed = evtGetFloat(event, args[2]);
    f32 dist;
    f32 tmp;
    f32 sx;
    f32 sy;
    f32 sz;
    PartyEntry* party;
    s32 ret;

    if (isFirstCall) {
        party = partyGetPtr(marioGetPartyId());
        if (party != NULL) {
            marioGetScreenPos(party->position, &sx, &sy, &sz);
            if (!(0.0f <= sx && sx <= float_600_8041b8d4 && 0.0f <= sy && sy <= float_535_8041b8d8)) {
                party->position = mario->position;
            }
            if (mario->position.y > party->position.y) {
                party->position = mario->position;
            }
            N_partyFollowCloseOn(party);
            party->flags |= 0x01000000;
            party->field_8 |= 0x00200000;
        }
        party = partyGetPtr(marioGetExtraPartyId());
        if (party != NULL) {
            marioGetScreenPos(party->position, &sx, &sy, &sz);
            if (!(0.0f <= sx && sx <= float_600_8041b8d4 && 0.0f <= sy && sy <= float_535_8041b8d8)) {
                party->position = mario->position;
            }
            if (mario->position.y > party->position.y) {
                party->position = mario->position;
            }
            N_partyFollowCloseOn(party);
            party->flags |= 0x01000000;
            party->field_8 |= 0x00200000;
        }
        event->userdata[0] = 0;
        event->userdata[2] = 0;
    }

    ret = 0;
    switch (event->userdata[0]) {
    case 0:
        dist = distABf(mario->position.x, mario->position.z, x, z);
        mario->directionView = angleABf(mario->position.x, mario->position.z, x, z);
        if (mario->flags & MARIO_FLAG_BG_MODE) {
            speed *= float_0p4_8041b8dc;
        }
#ifdef MUST_MATCH
        {
            f32 fr = (f32)gp->framerate * (dist / speed);
            s32 n;

            n = (s32)fr;
            speed = speed / (f32)gp->framerate;
            marioSetForceMove(n, mario->directionView, speed);
        }
#else
        tmp = dist / speed;
        speed = speed / (f32)gp->framerate;
        marioSetForceMove((s32)((f32)gp->framerate * tmp), mario->directionView, speed);
#endif
        event->userdata[0] = 2;
        break;
    case 2:
        if (mario->flags & MARIO_FLAG_FORCED_MOVEMENT) {
            break;
        }
        event->userdata[1] = sysMsec2Frame(10);
        event->userdata[0] = 4;
        /* fallthrough */
    case 4:
        if (--event->userdata[1] > 0) {
            break;
        }
        party = partyGetPtr(marioGetPartyId());
        if (party != NULL) {
            f32 angle;

            if (party->field_32 == 0) { // 0x1AC below: field not declared in MarioWork
                angle = (*(f32*)((u8*)mario + 0x1AC) >= float_90_8041b8e8 && *(f32*)((u8*)mario + 0x1AC) <= float_270_8041b8ec) ? float_220_8041b8e0 : float_140_8041b8e4;
                angle = revise360(mario->targetYaw + angle);
            } else {
                angle = (*(f32*)((u8*)mario + 0x1AC) >= float_90_8041b8e8 && *(f32*)((u8*)mario + 0x1AC) <= float_270_8041b8ec) ? float_225_8041b8f0 : float_135_8041b8f4;
                angle = revise360(mario->targetYaw + angle);
            }
            partyMoveBehindMario3(party, 0, angle);
            if (party->field_32 == 1) {
                party->flags2 |= 0x10;
            }
        }
        party = partyGetPtr(marioGetExtraPartyId());
        if (party != NULL) {
            f32 angle;

            if (party->field_32 == 0) {
                angle = (*(f32*)((u8*)mario + 0x1AC) >= float_90_8041b8e8 && *(f32*)((u8*)mario + 0x1AC) <= float_270_8041b8ec) ? float_220_8041b8e0 : float_140_8041b8e4;
                angle = revise360(mario->targetYaw + angle);
            } else {
                angle = (*(f32*)((u8*)mario + 0x1AC) >= float_90_8041b8e8 && *(f32*)((u8*)mario + 0x1AC) <= float_270_8041b8ec) ? float_225_8041b8f0 : float_135_8041b8f4;
                angle = revise360(mario->targetYaw + angle);
            }
            partyMoveBehindMario3(party, 0, angle);
            if (party->field_32 == 1) {
                party->flags2 |= 0x10;
            }
        }
        event->userdata[1] = sysMsec2Frame(400);
        event->userdata[0] = 6;
        break;
    case 6:
        party = partyGetPtr(marioGetPartyId());
        if (party != NULL && (party->flags & 0x00080000)) {
            event->userdata[1] = 0;
        }
        party = partyGetPtr(marioGetExtraPartyId());
        if (party != NULL && (party->flags & 0x00080000)) {
            event->userdata[1] = 0;
        }
        if (--event->userdata[1] > 0) {
            break;
        }
        party = partyGetPtr(marioGetPartyId());
        if (party != NULL) {
            N_partyFollowCloseOff(party);
            party->flags &= ~0x01000000;
            party->field_8 &= ~0x00200000;
            if (party->field_32 == 1) {
                unk_800c8e74(party);
                party->flags2 &= ~0x10;
            }
        }
        party = partyGetPtr(marioGetExtraPartyId());
        if (party != NULL) {
            N_partyFollowCloseOff(party);
            party->flags &= ~0x01000000;
            party->field_8 &= ~0x00200000;
            if (party->field_32 == 1) {
                unk_800c8e74(party);
                party->flags2 &= ~0x10;
            }
        }
        ret = 2;
        break;
    }
    return ret;
}

// NONMATCHING (92.58%): the two `accel = (dy+dy)/(half*half-half)` blocks reload `float_0`
// instead of keeping it in f0, so their temporaries land in f0/f1 instead of f1/f2.
USER_FUNC(evt_mario_jump_pos) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    f32 half;
    f32 x;
    f32 y;
    f32 z;
    s32 frames;
    s32 flag;
    f32 gravity;
    f32 framesF;
    f32 dy;
    f32 accel;
    void* ground;
    s32 landed;

    if (isFirstCall) {
        event->userdata[0] = 0;
    }
    x = evtGetFloat(event, args[0]);
    y = evtGetFloat(event, args[1]);
    z = evtGetFloat(event, args[2]);
    frames = evtGetValue(event, args[3]);
    flag = evtGetValue(event, args[4]);
    gravity = evtGetFloat(event, args[5]);
    if (gravity <= float_neg1000_8041b8d0) {
        gravity = float_20_8041b8cc;
    }
    framesF = (f32)sysMsec2Frame(frames);

    switch (event->userdata[0]) {
    case 0:
        mario->baseSpeed = distABf(mario->position.x, mario->position.z, x, z) / framesF;
        mario->directionView = angleABf(mario->position.x, mario->position.z, x, z);
        if (flag == 0) {
            *(f32*)((u8*)mario + 0x19C) = mario->directionView;
        }
        half = float_0p5_8041b8c8 * framesF;
        dy = y - mario->position.y;
        if (fabsf(dy) < float_1_8041b8a8) {
            accel = (gravity + gravity) / (half * half - half);
            *(f32*)((u8*)mario + 0x7C) = accel * half;
            *(f32*)((u8*)mario + 0x80) = -accel;
            *(f32*)((u8*)mario + 0x84) = float_0_8041b8ac;
            *(f32*)((u8*)mario + 0x88) = float_0_8041b8ac;
        } else {
            if (dy >= float_0_8041b8ac) {
                if (mario->characterId == 2) {
                    dy = dy + float_18p5_8041b8b0;
                } else {
                    dy = dy + float_0p5_8041b8c8 * *(f32*)((u8*)mario + 0x1B8);
                }
            } else {
#ifdef MUST_MATCH
                // MUST_MATCH: the original's `Vec vec` (12 bytes at 0x14) overlaps MWCC's
                // int<->float conversion temporary, which then lives at 0x18; a full Vec pushes
                // that temp to 0x20 (frame 0xa0 instead of 0x90). searchUnder2 only writes
                // out->x, so a one-float home reproduces the frame (same idiom as evt_party.c).
                f32 vec;
#else
                Vec vec;
#endif

#ifdef MUST_MATCH
                // MUST_MATCH: the header declares searchUnder2 as s32 but it returns a pointer;
                // the cast restores the original's unsigned (cmplwi) null test.
                if ((void*)searchUnder2((Vec*)&vec, x, y, z)) {
#else
                if (searchUnder2(&vec, x, y, z)) {
#endif
                    dy = gravity;
                } else {
                    dy = gravity;
                }
            }
            accel = (dy + dy) / (half * half - half);
            *(f32*)((u8*)mario + 0x7C) = accel * half;
            *(f32*)((u8*)mario + 0x80) = -accel;
            *(f32*)((u8*)mario + 0x84) = float_0_8041b8ac;
            *(f32*)((u8*)mario + 0x88) = float_0_8041b8ac;
        }
        if (!(mario->flags & MARIO_FLAG_TUBE_MODE)) {
            marioChgMot(8);
        } else {
            rollEvtJumpSetup();
            mario->unk1A8 = mario->directionView;
            *(f32*)((u8*)mario + 0x1AC) = mario->directionView;
        }
        event->userdata[2] = (s32)(float_0p5_8041b8c8 + half);
        event->userdata[0] = 1;
        /* fallthrough */
    case 1:
        if (mario->flags & MARIO_FLAG_TUBE_MODE) {
            setRollEvtFlag();
            movePos(&mario->position.x, &mario->position.z, mario->baseSpeed, mario->directionView);
        } else {
            movePos(&mario->position.x, &mario->position.z, mario->baseSpeed, mario->directionView);
        }
        mario->position.y += *(f32*)((u8*)mario + 0x7C);
        marioMakeJumpPara();
        if (--event->userdata[2] > 0) {
            break;
        }
        half = float_0p5_8041b8c8 * framesF;
        *(f32*)((u8*)mario + 0x7C) = float_0_8041b8ac;
        dy = y - mario->position.y;
        accel = (dy + dy) / (half * half - half);
        *(f32*)((u8*)mario + 0x80) = accel;
        *(f32*)((u8*)mario + 0x84) = float_0_8041b8ac;
        *(f32*)((u8*)mario + 0x88) = float_0_8041b8ac;
        event->userdata[2] = (s32)(float_0p5_8041b8c8 + half);
        event->userdata[0] = 2;
        break;
    case 2: {
        f32 v2;
        f32 v0;
        f32 v1;
        MarioWork* mario2;

        movePos(&mario->position.x, &mario->position.z, mario->baseSpeed, mario->directionView);
        marioMakeJumpPara();
        dy = *(f32*)((u8*)mario + 0x7C);
        mario2 = marioGetPtr();
        if (mario2->flags & MARIO_FLAG_TUBE_MODE) {
            ground = (void*)marioSearchGroundRoll(&v0, &v1, &v2, float_11_8041b8c0, *(f32*)((u8*)mario2 + 0x7C));
        } else {
            ground = (void*)marioSearchGround(&v0, &v1, &v2, dy, dy);
        }
        if (ground != NULL && mario2->position.y + dy <= v0) {
            marioClearJumpPara();
            mario2->hitobjStandOn = ground;
            mario2->position.y = v0;
            mario2->flags &= ~(MARIO_FLAG_IS_JUMPING | MARIO_FLAG_IS_FALLING |
                               MARIO_FLAG_IS_STEPPING | MARIO_FLAG_HAS_INPUT_JUMP);
            *(f32*)((u8*)mario2 + 0x128) = fabsf(distABf(mario2->position.x, mario2->position.z,
                                                          *(f32*)((u8*)mario2 + 0x11C), *(f32*)((u8*)mario2 + 0x124)));
            mario2->airTimer = 0;
            *(s16*)((u8*)mario2 + 0x52) = 0;
            mario2->flags &= ~MARIO_FLAG_STOP_AIR_MOVEMENT;
            camFollowYOn();
            landed = 1;
        } else {
            landed = 0;
        }
        if (landed) {
            event->userdata[2] = 0;
        } else {
            mario->position.y += *(f32*)((u8*)mario + 0x7C);
        }
        if (--event->userdata[2] > 0) {
            break;
        }
        *(f32*)((u8*)mario + 0x7C) = float_0_8041b8ac;
        *(f32*)((u8*)mario + 0x80) = float_0_8041b8ac;
        *(f32*)((u8*)mario + 0x84) = float_0_8041b8ac;
        *(f32*)((u8*)mario + 0x88) = float_0_8041b8ac;
        *(f32*)((u8*)mario + 0x180) = float_0_8041b8ac;
        *(f32*)((u8*)mario + 0x180) = float_0_8041b8ac;
        camFollowYOn();
        if (!(mario->flags & MARIO_FLAG_TUBE_MODE)) {
            marioChgMot(0);
        } else {
            marioResetRollSpd();
            clrRollEvtFlag();
        }
        return 1;
    }
    }
    return 0;
}

// The float locals below are declared in the order that reproduces the original's f22-f31 assignment.
// Logic and callee-saved registers now match; only FP temp-register allocation and instruction scheduling
// in case 0 / the second switch differ
// NONMATCHING (93.36%): FP temp-register allocation + scheduling in case 0 and the second switch; the
// original loads float_0p5/float_1 before the position load and keeps 0.0 in one register across 0x84/0x88.
USER_FUNC(evt_mario_jump_jumpstand) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    f32 ty;
    f32 tx;
    f32 tz;
    f32 h;
    f32 ax;
    f32 bx;
    f32 t;
    f32 dy;
    f32 by;
    f32 ay;
    f32 tmp;
    f32 bz;
    f32 az;
    s32 time;
    s32 mode;
    HitEntry* hit;
    s32 landed;

    evtGetFloat(event, args[0]);
    h = evtGetFloat(event, args[1]);
    evtGetFloat(event, args[2]);
    ax = evtGetFloat(event, args[3]);
    ay = evtGetFloat(event, args[4]);
    az = evtGetFloat(event, args[5]);
    bx = evtGetFloat(event, args[6]);
    by = evtGetFloat(event, args[7]);
    bz = evtGetFloat(event, args[8]);
    time = evtGetValue(event, args[9]);
    mode = evtGetValue(event, args[10]);

    if (isFirstCall) {
        event->userdata[0] = 0;
        event->userdata[3] = 0;
        if (mode == 3) {
            event->userdata[3] = 2;
        } else if (*(f32*)((u8*)mario + 0xD8) < mario->position.y) {
            event->userdata[3] = 2;
        } else if (*(f32*)((u8*)mario + 0xD8) <= h + float_1p5_8041b8c4 * *(f32*)((u8*)mario + 0x1B8)) {
            event->userdata[3] = 2;
        } else {
            event->userdata[3] = 1;
        }
        *(u32*)((u8*)gp + 0x18) |= 0x10;
    }

    if (event->userdata[3] & 1) {
        tx = bx;
        ty = by;
        tz = bz;
    } else if (event->userdata[3] & 2) {
        tx = ax;
        ty = ay;
        tz = az;
    }

    switch (event->userdata[0]) {
    case 0: {
        mario->dispFlags |= 0x4;
        if (marioChkSts(0x10)) {
            effKemuriEntry(0, mario->position.x, mario->position.y, mario->position.z, float_0_8041b8ac);
        }
        marioChgMot(8);
        if (event->userdata[3] & 1) {
            time = 0x226;
        }
        event->userdata[1] = sysMsec2Frame(time);
        mario->baseSpeed = distABf(mario->position.x, mario->position.z, tx, tz) / (f32)event->userdata[1];
        mario->directionView = angleABf(mario->position.x, mario->position.z, tx, tz);
        *(f32*)((u8*)mario + 0x19C) = mario->directionView;
        dy = ty - mario->position.y;
        t = float_0p5_8041b8c8 * (f32)event->userdata[1];
        if (fabsf(dy) < float_1_8041b8a8) {
            dy = dy + float_20_8041b8cc;
        } else if (dy >= float_0_8041b8ac) {
            dy = float_0p5_8041b8c8 * *(f32*)((u8*)mario + 0x1B8) + dy;
        } else {
            dy = float_20_8041b8cc;
        }
        tmp = (dy + dy) / (t * t - t);
        *(f32*)((u8*)mario + 0x7C) = tmp * t;
        *(f32*)((u8*)mario + 0x80) = -tmp;
        *(f32*)((u8*)mario + 0x88) = *(f32*)((u8*)mario + 0x84) = float_0_8041b8ac;
        if (event->userdata[3] & 2) {
            event->userdata[0] = 0xA;
        }
        event->userdata[2] = (s32)(float_0p5_8041b8c8 * (f32)event->userdata[1] + float_0p5_8041b8c8);
        if (event->userdata[0] == 0xA) {
            break;
        }
        event->userdata[0] = 1;
        /* fallthrough */
    }
    case 1: {
        movePos(&mario->position.x, &mario->position.z, mario->baseSpeed, mario->directionView);
        mario->position.y += *(f32*)((u8*)mario + 0x7C);
        marioMakeJumpPara();
        if (--event->userdata[2] > 0) {
            break;
        }
        dy = ty - mario->position.y;
        t = float_0p5_8041b8c8 * (f32)event->userdata[1];
        tmp = (dy + dy) / (t * t - t);
        *(f32*)((u8*)mario + 0x7C) = float_0_8041b8ac;
        *(f32*)((u8*)mario + 0x80) = tmp;
        *(f32*)((u8*)mario + 0x88) = *(f32*)((u8*)mario + 0x84) = float_0_8041b8ac;
        event->userdata[2] = (s32)(float_0p5_8041b8c8 * (f32)event->userdata[1] + float_0p5_8041b8c8);
        event->userdata[0] = 2;
        if (!(mario->flags & 0x01000000)) {
            marioChgPose(str_802E5878);
        }
        break;
    }
    case 2: {
        MarioWork* player;
        f32 outZ;
        f32 outY;
        f32 outX;

        movePos(&mario->position.x, &mario->position.z, mario->baseSpeed, mario->directionView);
        marioMakeJumpPara();
        dy = *(f32*)((u8*)mario + 0x7C);
        player = marioGetPtr();
        if (player->flags & 0x01000000) {
            hit = (HitEntry*)marioSearchGroundRoll(&outY, &outX, &outZ, float_11_8041b8c0, *(f32*)((u8*)player + 0x7C));
        } else {
            hit = marioSearchGround(&outY, &outX, &outZ, dy, dy);
        }
        if (hit != NULL && (player->position.y + dy) <= outY) {
            marioClearJumpPara();
            player->hitobjStandOn = hit;
            player->position.y = outY;
            player->flags &= ~0x000F0000;
            *(f32*)((u8*)player + 0x128) = fabsf(distABf(player->position.x, player->position.z, *(f32*)((u8*)player + 0x11C), *(f32*)((u8*)player + 0x124)));
            *(s16*)((u8*)player + 0x50) = 0;
            *(s16*)((u8*)player + 0x52) = 0;
            player->flags &= ~0x80;
            camFollowYOn();
            landed = 1;
        } else {
            landed = 0;
        }
        if (landed) {
            event->userdata[2] = 0;
        } else {
            mario->position.y += *(f32*)((u8*)mario + 0x7C);
        }
        if (--event->userdata[2] <= 0) {
            event->userdata[0] = 0x14;
        }
        break;
    }
    default:
        break;
    }

    switch (event->userdata[0]) {
    case 0xA:
        if (mario->position.y < ay) {
            if (!(mode & 1)) {
                movePos(&mario->position.x, &mario->position.z, mario->baseSpeed, mario->directionView);
            }
            mario->position.y += *(f32*)((u8*)mario + 0x7C);
            marioMakeJumpPara();
            event->userdata[2]--;
            break;
        }
        event->userdata[2] = 0x36;
        event->userdata[1] = event->userdata[2] / 2;
        camFollowYOff();
        ty = ay;
        mario->baseSpeed = distABf(mario->position.x, mario->position.z, ax, az) / (f32)event->userdata[1];
        mario->directionView = angleABf(mario->position.x, mario->position.z, ax, az);
        *(f32*)((u8*)mario + 0x19C) = mario->directionView;
        t = float_0p5_8041b8c8 * (f32)event->userdata[1];
        tmp = float_20_8041b8cc / (t * t - t);
        *(f32*)((u8*)mario + 0x7C) = tmp * t;
        *(f32*)((u8*)mario + 0x80) = -tmp;
        *(f32*)((u8*)mario + 0x88) = *(f32*)((u8*)mario + 0x84) = float_0_8041b8ac;
        event->userdata[2] = (s32)(float_0p5_8041b8c8 * (f32)event->userdata[1] + float_0p5_8041b8c8);
        event->userdata[0] = 0xB;
        /* fallthrough */
    case 0xB:
        movePos(&mario->position.x, &mario->position.z, mario->baseSpeed, mario->directionView);
        mario->position.y += *(f32*)((u8*)mario + 0x7C);
        marioMakeJumpPara();
        if (--event->userdata[2] > 0) {
            break;
        }
        dy = ty - mario->position.y;
        t = float_0p5_8041b8c8 * (f32)event->userdata[1];
        tmp = (dy + dy) / (t * t - t);
        *(f32*)((u8*)mario + 0x7C) = float_0_8041b8ac;
        *(f32*)((u8*)mario + 0x80) = tmp;
        *(f32*)((u8*)mario + 0x88) = *(f32*)((u8*)mario + 0x84) = float_0_8041b8ac;
        event->userdata[2] = (s32)(float_0p5_8041b8c8 * (f32)event->userdata[1] + float_0p5_8041b8c8);
        event->userdata[0] = 0xC;
        marioChgPose(str_802E5878);
        break;
    case 0xC: {
        MarioWork* player;
        f32 outZ;
        f32 outY;
        f32 outX;

        movePos(&mario->position.x, &mario->position.z, mario->baseSpeed, mario->directionView);
        marioMakeJumpPara();
        dy = *(f32*)((u8*)mario + 0x7C);
        player = marioGetPtr();
        if (player->flags & 0x01000000) {
            hit = (HitEntry*)marioSearchGroundRoll(&outY, &outX, &outZ, float_11_8041b8c0, *(f32*)((u8*)player + 0x7C));
        } else {
            hit = marioSearchGround(&outY, &outX, &outZ, dy, dy);
        }
        if (hit != NULL && (player->position.y + dy) <= outY) {
            marioClearJumpPara();
            player->hitobjStandOn = hit;
            player->position.y = outY;
            player->flags &= ~0x000F0000;
            *(f32*)((u8*)player + 0x128) = fabsf(distABf(player->position.x, player->position.z, *(f32*)((u8*)player + 0x11C), *(f32*)((u8*)player + 0x124)));
            *(s16*)((u8*)player + 0x50) = 0;
            *(s16*)((u8*)player + 0x52) = 0;
            player->flags &= ~0x80;
            camFollowYOn();
            landed = 1;
        } else {
            landed = 0;
        }
        if (landed) {
            event->userdata[2] = 0;
        } else {
            mario->position.y += *(f32*)((u8*)mario + 0x7C);
        }
        if (--event->userdata[2] <= 0) {
            event->userdata[0] = 0x14;
        }
        break;
    }
    default:
        break;
    }

    if (event->userdata[0] == 0x14) {
        mario->baseSpeed = mario->baseSpeed = *(f32*)((u8*)mario + 0x88) = *(f32*)((u8*)mario + 0x84) = *(f32*)((u8*)mario + 0x80) = *(f32*)((u8*)mario + 0x7C) = float_0_8041b8ac;
        marioChgMot(0);
        mario->dispFlags &= ~0x4;
        marioAdjustMoveDir();
        camFollowYOn();
        *(u32*)((u8*)gp + 0x18) &= ~0x10;
        return 1;
    }
    return 0;
}

USER_FUNC(evt_mario_wait_landon) {
    if (marioGetPtr()->hitobjStandOn != NULL) {
        MarioWork* mario = marioGetPtr();
        s32 landed;

        if (mario->currMotionId <= MARIO_MOTION_WALK || (s32)mario->currMotionId == MARIO_MOTION_DASH) {
            landed = 1;
        } else if ((s32)mario->currMotionId == MARIO_MOTION_ROLL && mario->airTimer == 0) {
            landed = 1;
        } else if ((s32)mario->currMotionId == MARIO_MOTION_SLIT && mario->airTimer == 0) {
            landed = 1;
        } else if ((s32)mario->currMotionId == MARIO_MOTION_SHIP && mario->airTimer == 0) {
            landed = 1;
        } else if ((s32)mario->currMotionId == MARIO_MOTION_YOSHI) {
            landed = 1;
        } else {
            landed = 0;
        }

        if (landed) {
            return EVT_RETURN_DONE;
        }
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(evt_mario_wait_rideon) {
    if (marioGetPtr()->hitobjStandOn != NULL) {
        MarioWork* mario = marioGetPtr();
        s32 landed;

        if (mario->currMotionId <= MARIO_MOTION_WALK || (s32)mario->currMotionId == MARIO_MOTION_DASH) {
            landed = 1;
        } else if ((s32)mario->currMotionId == MARIO_MOTION_ROLL && mario->airTimer == 0) {
            landed = 1;
        } else if ((s32)mario->currMotionId == MARIO_MOTION_SLIT && mario->airTimer == 0) {
            landed = 1;
        } else if ((s32)mario->currMotionId == MARIO_MOTION_SHIP && mario->airTimer == 0) {
            landed = 1;
        } else if ((s32)mario->currMotionId == MARIO_MOTION_YOSHI) {
            landed = 1;
        } else {
            landed = 0;
        }

        if (landed) {
            return EVT_RETURN_DONE;
        }
    }
    return EVT_RETURN_BLOCK;
}

USER_FUNC(evt_mario_check_landon) {
    s32* args = event->args;

    if (marioGetPtr()->hitobjStandOn != NULL) {
        MarioWork* mario = marioGetPtr();
        s32 landed;

        if (mario->currMotionId <= MARIO_MOTION_WALK || (s32)mario->currMotionId == MARIO_MOTION_DASH) {
            landed = 1;
        } else if ((s32)mario->currMotionId == MARIO_MOTION_ROLL && mario->airTimer == 0) {
            landed = 1;
        } else if ((s32)mario->currMotionId == MARIO_MOTION_SLIT && mario->airTimer == 0) {
            landed = 1;
        } else if ((s32)mario->currMotionId == MARIO_MOTION_SHIP && mario->airTimer == 0) {
            landed = 1;
        } else if ((s32)mario->currMotionId == MARIO_MOTION_YOSHI) {
            landed = 1;
        } else {
            landed = 0;
        }

        if (landed) {
            evtSetValue(event, args[0], 1);
            return EVT_RETURN_YIELD;
        }
    }
    evtSetValue(event, args[0], 0);
    return EVT_RETURN_DONE;
}
// NONMATCHING (95.28%): the case 0 float block keeps `float_0` in f0 where we use f0 for the
// `(dy+dy)/(half*half-half)` temporaries, plus one case 1 constant-load/store interleave.
USER_FUNC(evt_koopa_hip_attack) {
    MarioWork* mario;
    s32 first = isFirstCall;
    s32* args = event->args;
    f32 x;
    f32 y;
    f32 z;
    f32 angle;
    f32 frames;
    f32 v2;
    f32 v0;
    f32 v1;

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
        f32 denom;
        f32 v;

        camFollowYOff();
        mario->baseSpeed = distABf(mario->position.x, mario->position.z, x, z) / frames;
        mario->directionView = angleABf(mario->position.x, mario->position.z, x, z);
        *(f32*)((u8*)mario + 0x19C) = mario->directionView;
        dy = y - mario->position.y;
        if (fabsf(dy) < float_1_8041b8a8) {
            dy = dy + angle;
        } else if (dy >= float_0_8041b8ac) {
            dy = dy + float_18p5_8041b8b0;
        } else {
            dy = angle;
        }
        dy = dy + dy;
        denom = frames * frames - frames;
        v = dy / denom;
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
        MarioWork* mario2;
        u8* mp;
        void* ground;
        s32 landed;

        movePos(&mario->position.x, &mario->position.z, mario->baseSpeed, mario->directionView);
        marioMakeJumpPara();
        vy = *(f32*)((u8*)mario + 0x7C);
        mario2 = marioGetPtr();
        mp = (u8*)mario2;
        if (mario2->flags & MARIO_FLAG_TUBE_MODE) {
            ground = (void*)marioSearchGroundRoll(&v0, &v1, &v2, float_11_8041b8c0, *(f32*)(mp + 0x7C));
        } else {
            ground = (void*)marioSearchGround(&v0, &v1, &v2, vy, vy);
        }
        if (ground != NULL && mario2->position.y + vy <= v0) {
            marioClearJumpPara();
            mario2->hitobjStandOn = ground;
            mario2->position.y = v0;
            mario2->flags &= ~(MARIO_FLAG_IS_JUMPING | MARIO_FLAG_IS_FALLING |
                              MARIO_FLAG_IS_STEPPING | MARIO_FLAG_HAS_INPUT_JUMP);
            *(f32*)(mp + 0x128) = fabsf(distABf(mario2->position.x, mario2->position.z,
                                                         *(f32*)(mp + 0x11C), *(f32*)(mp + 0x124)));
            mario2->airTimer = 0;
            *(s16*)(mp + 0x52) = 0;
            mario2->flags &= ~MARIO_FLAG_STOP_AIR_MOVEMENT;
            camFollowYOn();
            landed = 1;
        } else {
            landed = 0;
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

    switch (event->userdata[0]) {
    case 0:
        if (--event->userdata[2] != 0) {
            *(s32*)(*(u8**)((u8*)mario + 0x28C) + 0x144) = 1;
            return EVT_RETURN_BLOCK;
        }
        break;
    }
    *(s32*)(*(u8**)((u8*)mario + 0x28C) + 0x144) = 0;
    return EVT_RETURN_YIELD;
}

USER_FUNC(evt_koopa_chk_dead) {
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    u32* kpa = *(u32**)((u8*)mario + 0x28C);

    evtSetValue(event, args[0], (*kpa >> 28) & 1);
    return EVT_RETURN_DONE;
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

USER_FUNC(evt_mario_wait_move_end) {
    MarioWork* mario = marioGetPtr();

    return mario->forceMoveTimer ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_wait_movable) {
    MarioWork* mario = marioGetPtr();

    return (mario->flags & MARIO_FLAG_KEY_OFF) ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_get_state) {
    s32 arg;
    s32* args = event->args;
    s32 state;
    MarioWork* mario = marioGetPtr();
    arg = args[0];

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

USER_FUNC(evt_mario_get_mode) {
    s32 arg = event->args[0];
    s8 value = marioGetPtr()->characterId;

    evtSetValue(event, arg, value);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_set_mode) {
    marioSetCharMode(evtGetValue(event, event->args[0]));
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_koopa_get_level) {
    s32* args = event->args;
    s32 level = kpaGetLevel();

    evtSetValue(event, args[0], level);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_koopa_set_level) {
    kpaSetLevel(evtGetValue(event, event->args[0]));
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_peach_transform_gundan_on) {
    peachTransformOn();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_peach_transform_gundan_off) {
    peachTransformOff();
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

USER_FUNC(evt_mario_get_party) {
    s32 out = event->args[0];
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    evtSetValue(event, out, party ? party->currentMemberId : 0);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_get_exparty) {
    s32 out = event->args[0];
    PartyEntry* party = partyGetPtr(marioGetExtraPartyId());

    evtSetValue(event, out, party ? party->currentMemberId : 0);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_chk_join_party) {
    s32* args = event->args;
    s32 out = args[0];
    s32 member = evtGetValue(event, args[1]);

    evtSetValue(event, out, partyChkJoin(member));
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

USER_FUNC(evt_mario_set_prev_party_dokan) {
    s32 first = isFirstCall;
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    s32 slot = args[0];
    s32 prev;
    s32 ret;
    PartyEntry* party;
    Vec* pos;

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
        if (ret >= 0) {
            mario->currPartySlotId[slot] = ret;
            party = partyGetPtr(ret);
            partyChgRunMode(party, 0);
#ifdef MUST_MATCH
            // MUST_MATCH: volatile on both sides of the Vec copy (and on the flags updates that
            // surround it) reproduces the original's unscheduled load/store pair without hoisting.
            *(volatile u32*)&party->flags = *(volatile u32*)&party->flags | 0x04000000;
            *(volatile u32*)&party->flags = *(volatile u32*)&party->flags | 0x02000000;
            pos = (Vec*)((u8*)party + 0x70);
            *(volatile Vec*)pos = *(volatile Vec*)&vec3_802e58e8;
#else
            party->flags |= 0x04000000;
            party->flags |= 0x02000000;
            pos = (Vec*)((u8*)party + 0x70);
            *pos = vec3_802e58e8;
#endif
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_hello_party) {
    s32 slot;
    s32* args = event->args;
    s32 first = isFirstCall;
    MarioWork* mario = marioGetPtr();
    slot = args[0];

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
        if ((party->flags & PARTY_FLAG_IS_ACTIVE) == 0 || party->field_34 != 1) {
            return EVT_RETURN_BLOCK;
        }
        return EVT_RETURN_DONE;
    }
}

USER_FUNC(evt_mario_hello_exparty_pos) {
    s32 member;
    s32 first = isFirstCall;
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();

    if (first) {
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
        if ((party->flags & PARTY_FLAG_IS_ACTIVE) == 0 || party->field_34 != 1) {
            return EVT_RETURN_BLOCK;
        }
        return EVT_RETURN_DONE;
    }
}

USER_FUNC(evt_mario_set_party) {
    s32 slot;
    s32* args = event->args;
    s32 member;
    s32 ret;
    MarioWork* mario = marioGetPtr();
    slot = args[0];

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

USER_FUNC(evt_mario_set_party_pos) {
    s32 member;
    s32 slot;
    s32* args = event->args;
    f32 x;
    f32 y;
    f32 z;
    s32 ret;
    MarioWork* mario = marioGetPtr();
    slot = args[0];

    if (isFirstCall) {
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

USER_FUNC(evt_mario_set_prev_party) {
    s32 slot;
    s32* args = event->args;
    s32 prev;
    s32 ret;
    MarioWork* mario = marioGetPtr();
    slot = args[0];

    if (isFirstCall) {
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

USER_FUNC(evt_mario_set_prev_party_pos) {
    s32* args = event->args;
    s32 first = isFirstCall;
    s32 prev;
    s32 slot;
    f32 x;
    f32 y;
    f32 z;
    s32 ret;
    MarioWork* mario = marioGetPtr();
    slot = args[0];

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

USER_FUNC(evt_mario_push) {
    MarioWork* mario = marioGetPtr();
    mario->flags |= MARIO_FLAG_IS_PUSHING;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_sleep_on) {
    marioChgEvtPose(str_802E58FC);
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

USER_FUNC(evt_mario_dump_item) {
    s32* args = event->args;
    pouchRemoveItem(evtGetValue(event, args[0]));
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

USER_FUNC(L_evt_mario_keyon_wait) {
    marioGetPtr();
    return marioKeyOffChk() ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

USER_FUNC(evt_mario_paper_plane) {
    s32 first = isFirstCall;
    s32 arg;
    s32* args = event->args;
    MarioWork* mario = marioGetPtr();
    arg = args[0];

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
