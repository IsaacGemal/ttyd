#include "evt/evt_party.h"
#include "evt/evt_cmd.h"
#include "evt/evt_npc.h"
#include "manager/evtmgr_cmd.h"
#include "party.h"
#include "party/party_vivian.h"
#include "mario/mario.h"
#include "mario/mario_cam.h"
#include "mario/mario_sbr.h"
#include "mario/mariost.h"
#include "driver/camdrv.h"
#include "driver/hitdrv.h"
#include "system.h"
#include <dolphin/mtx.h>
#include "party/party_yoshi.h"
#include "party/party_move.h"
#include "party/party_hit.h"
#include "party/party_nokonoko.h"
#include "party/party_bomhei.h"
#include "mario/mario_hit.h"

// Raw field accessors for PartyEntry fields that aren't declared in the shared header yet.
#define P8(p, off) (*(u8*)((u8*)(p) + (off)))
#define PS8(p, off) (*(s8*)((u8*)(p) + (off)))
#define P16(p, off) (*(s16*)((u8*)(p) + (off)))
#define P32(p, off) (*(u32*)((u8*)(p) + (off)))
#define PF32(p, off) (*(f32*)((u8*)(p) + (off)))

// local prototypes (not in headers yet)
extern s32 partyCtrlNo;
extern const f32 float_1_8041bb10;
extern const f32 float_0p5_8041bb14;
extern const f32 float_20_8041bb18;
extern const f32 float_0_8041bb1c;
extern const f32 float_37_8041bb20;
extern const f32 float_neg1000_8041bb24;
extern const f32 float_180_8041bb28;
extern const f32 float_270_8041bb2c;
extern const f32 float_neg5000_8041bb30;
extern const f32 float_100_8041bb34;
extern const f32 float_50_8041bb38;

// Data owned by this translation unit (see config/G8MJ01/splits.txt).
const Vec vec3_802e9668 = {0.0f, -1.0f, 0.0f};

// party_cloud.h declares cloudGetStatus as returning u8 (its definition masks the result), but the original
// evt_party_cloud_breathout uses the result unmasked, as if this file saw it declared int.
// LOCAL_PROTO(cloudGetStatus): Retail evt_party_cloud_breathout consumes the result as s32 without masking; the implementation/header return u8.
s32 cloudGetStatus(void);

USER_FUNC(evt_party_flg_onoff) {
    PartyEntry* party;
    s32 mask;
    s32* args;

    args = event->args;
    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    mask = args[1];
    if (evtGetValue(event, args[2])) {
        party->flags |= mask;
    } else {
        party->flags &= ~mask;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_dispflg_onoff) {
    PartyEntry* party;
    s32 mask;
    s32* args;

    args = event->args;
    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    mask = args[1];
    if (evtGetValue(event, args[2])) {
        party->flags2 |= mask;
    } else {
        party->flags2 &= ~mask;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(unk_800e71bc) {
    PartyEntry* party;
    s32 mask;
    s32* args;

    args = event->args;
    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    mask = args[1];
    if (evtGetValue(event, args[2])) {
        party->field_8 |= mask;
    } else {
        party->field_8 &= ~mask;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_cont_onoff) {
    s32* args = event->args;
    PartyEntry* party;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    if (evtGetValue(event, args[1]) == 0) {
        partyCtrlOff();
    } else {
        partyCtrlOn();
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_init_camid) {
    s32* args = event->args;
    PartyEntry* party;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    partyInitCamId(party);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_set_camid) {
    PartyEntry* party;
    s32* args = event->args;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    partySetCamId(party, evtGetValue(event, args[1]));
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_force_reset_outofscreen) {
    s32* args = event->args;
    MarioWork* mario;
    PartyEntry* party;
    Vec tmp;
    Vec dir;
    Vec hitPos;
    Vec hitNormal;
    f32 sx, sy, sz;
    f32 dist;
    f32 angle;
    f32 up;
    f32 maxDist;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    marioGetScreenPos(party->position, &sx, &sy, &sz);
    if (marioChkInScreen((s32)sx, (s32)sy)) {
        return EVT_RETURN_DONE;
    }
    partyClearFootmark();
    mario = party->playerPtr;
    maxDist = float_100_8041bb34;
    up = float_50_8041bb38;
    dir = vec3_802e9668;
    while (TRUE) {
        marioGetScreenPos(party->position, &sx, &sy, &sz);
        if (marioChkInScreen((s32)sx, (s32)sy)) {
            break;
        }
        angle = angleABf(party->position.x, party->position.z, mario->position.x, mario->position.z);
        movePos(&party->position.x, &party->position.z, float_1_8041bb10, angle);
        dist = maxDist;
        tmp = party->position;
        tmp.y += up;
        if (partyHitCheck(party, &tmp, &dir, &hitPos, &hitNormal, &dist)) {
            party->position.y = hitPos.y;
        } else {
            party->position.y = mario->position.y;
        }
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_outofscreen) {
    s32* args = event->args;
    PartyEntry* party;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    evtSetFloat(event, args[1], (f32)((party->flags2 & 0x800000) ? 0 : 1));
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_set_pos) {
    s32* args = event->args;
    PartyEntry* party;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    party->position.x = evtGetFloat(event, args[1]);
    party->position.y = evtGetFloat(event, args[2]);
    party->position.z = evtGetFloat(event, args[3]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_set_hosei_xyz) {
    s32* args = event->args;
    PartyEntry* party;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    PF32(party, 0xAC) = evtGetFloat(event, args[1]);
    PF32(party, 0xB0) = evtGetFloat(event, args[2]);
    PF32(party, 0xB4) = evtGetFloat(event, args[3]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_get_pos) {
    s32* args = event->args;
    PartyEntry* party;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        evtSetFloat(event, args[1], float_neg5000_8041bb30);
        evtSetFloat(event, args[2], float_neg5000_8041bb30);
        evtSetFloat(event, args[3], float_neg5000_8041bb30);
        return EVT_RETURN_DONE;
    }
    evtSetFloat(event, args[1], party->position.x);
    evtSetFloat(event, args[2], party->position.y);
    evtSetFloat(event, args[3], party->position.z);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_set_dir) {
    s32* args = event->args;
    PartyEntry* party;
    f32 target, next, dest, step;
    s32 frames;
    s32 msec;

    if (isFirstCall) {
        args += 1;
        event->userdata[0] = args[-1] ? 1 : partyCtrlNo;
        party = partyGetPtr(event->userdata[0]);
        if (!party) {
            return EVT_RETURN_DONE;
        }
        event->userdata[2] = 0;
        event->userdataf[1] = party->field_10C;
    } else {
        args += 1;
    }
    target = evtGetFloat(event, args[0]);
    msec = evtGetValue(event, args[1]);
    if (msec >= 0) {
        frames = sysMsec2Frame(msec);
    } else {
        frames = msec;
    }
    party = partyGetPtr(event->userdata[0]);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    if (frames > 0) {
        dest = revise360(float_270_8041bb2c - target);
        step = (dest - event->userdataf[1]) * ((f32)event->userdata[2] / (f32)frames);
        next = revise360(event->userdataf[1] + step);
        party->field_10C = next;
        party->field_110 = party->field_10C;
        party->dir = toMovedirSimple(party->field_110);
        return (++event->userdata[2] > frames) ? 1 : 0;
    }
    if (frames < 0) {
        next = revise360(float_270_8041bb2c - target);
        party->field_110 = next;
        party->dir = toMovedirSimple(party->field_110);
    } else if (frames == 0) {
        next = revise360(float_270_8041bb2c - target);
        party->field_10C = next;
        party->field_110 = party->field_10C;
        party->dir = toMovedirSimple(party->field_110);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_set_dir_npc) {
    PartyEntry* party;
    s32* args = event->args;
    NpcEntry* npc;
    f32 x, z, angle, delta, yaw;
    CameraEntry* camera;
    s32 name;

    party = partyGetPtr(args[0]);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    if (isFirstCall) {
        name = evtGetValue(event, args[1]);
        npc = evtNpcNameToPtr(event, (const char*)name);
        if (!npc) {
            return EVT_RETURN_DONE;
        }
        x = npc->position.x;
        z = npc->position.z;
        camera = camGetPtr(CAMERA_3D);
        yaw = camera->viewYaw;
        angle = angleABf(party->position.x, party->position.z, x, z);
        delta = revise360(angle - yaw);
        party->field_110 = (delta < float_180_8041bb28) ? float_180_8041bb28 : float_0_8041bb1c;
        party->dir = delta;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_set_dir_pos) {
    PartyEntry* party;
    s32* args;
    f32 x, z, angle, delta, yaw;
    CameraEntry* camera;

    args = event->args;
    party = partyGetPtr(args[0]);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    x = (f32)evtGetValue(event, args[1]);
    z = (f32)evtGetValue(event, args[2]);
    camera = camGetPtr(CAMERA_3D);
    yaw = camera->viewYaw;
    angle = angleABf(party->position.x, party->position.z, x, z);
    delta = revise360(angle - yaw);
    party->field_110 = (delta < float_180_8041bb28) ? float_180_8041bb28 : float_0_8041bb1c;
    party->dir = delta;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_get_dispdir) {
    s32* args = event->args;
    PartyEntry* party;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        evtSetFloat(event, args[1], float_0_8041bb1c);
    } else {
        evtSetFloat(event, args[1], party->field_10C);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_set_dispdir) {
    s32* args = event->args;
    PartyEntry* party;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    party->field_110 = reviseAngle(evtGetFloat(event, args[1]));
    party->field_10C = party->field_110;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_stop) {
    s32* args = event->args;
    PartyEntry* party;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (party) {
        partyStop(party);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_run) {
    s32* args = event->args;
    PartyEntry* party;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (party) {
        partyRun(party);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_move_pos) {
    PartyEntry* party;
    s32* args = event->args;
    f32 x, z, spd;
    u8 breed;

    if (isFirstCall) {
        event->userdata[2] = 0;
    }
    event->userdata[0] = args[0] ? 1 : partyCtrlNo;
    party = partyGetPtr(event->userdata[0]);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    if (event->userdata[2] == 0) {
        breed = party->field_34;
        if (breed != 0 && (u8)(breed - 1) > 1) {
            return 0;
        }
        event->userdata[2] = 1;
        x = evtGetFloat(event, args[1]);
        z = evtGetFloat(event, args[2]);
        event->userdata[1] = sysMsec2Frame(evtGetValue(event, args[3]));
        party->dir = angleABf(party->position.x, party->position.z, x, z);
        if (event->userdata[1] == 0) {
            event->userdata[1] = (s32)(distABf(party->position.x, party->position.z, x, z) / party->speed);
            spd = party->speed;
        } else {
            spd = distABf(party->position.x, party->position.z, x, z) / (f32)event->userdata[1];
        }
        partySetForceMove(party, event->userdata[1], party->dir, spd);
    }
    if (--event->userdata[1] < 0) {
        return 1;
    }
    return 0;
}

// `#pragma opt_propagation off`: with propagation on MWCC folds the two `(f32)gp->framerate`
// conversions into one and the int->float constant lands in f5 instead of f4 (same pragma as
// evt_sub_spline_get_value_manual).
#pragma opt_propagation off
USER_FUNC(evt_party_move_pos2) {
    PartyEntry* party;
    s32* args = event->args;
    f32 x, z, len, dist, spd;
    s32 frames;
    u8 breed;

    if (isFirstCall) {
        event->userdata[2] = 0;
    }
    event->userdata[0] = args[0] ? 1 : partyCtrlNo;
    party = partyGetPtr(event->userdata[0]);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    if (event->userdata[2] == 0) {
        breed = party->field_34;
        if (breed != 0 && (u8)(breed - 1) > 1) {
            return 0;
        }
        event->userdata[2] = 1;
        x = evtGetFloat(event, args[1]);
        z = evtGetFloat(event, args[2]);
        len = evtGetFloat(event, args[3]);
        dist = distABf(party->position.x, party->position.z, x, z);
        party->dir = angleABf(party->position.x, party->position.z, x, z);
#ifdef MUST_MATCH
        // MUST_MATCH: casting the frames product after `spd`, not in the one expression under
        // #else, places the int->float temp at 0x10 and the fctiwz slot at 0x18; the #else form
        // swaps the two slots.
        {
            f32 fr = (f32)gp->framerate * (dist / len);
            spd = len / (f32)gp->framerate;
            frames = (s32)fr;
        }
#else
        frames = (s32)((f32)gp->framerate * (dist / len));
        spd = len / (f32)gp->framerate;
#endif
        partySetForceMove(party, frames, party->dir, spd);
    }
    if (party->field_28 == 0) {
        return EVT_RETURN_DONE;
    }
    return 0;
}

#pragma opt_propagation reset

USER_FUNC(evt_party_wait_landon) {
    s32* args = event->args;
    PartyEntry* party;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    return (party->flags & PARTY_FLAG_IS_GROUNDED) ? EVT_RETURN_DONE : 0;
}

// The float constants are written as literals (MWCC pools them in .sdata2 in the original's
// order); using the named non-const f32 globals instead makes MWCC reload them across the
// party stores, shifting the whole case 0/1 schedule.
USER_FUNC(evt_party_jump_pos) {
    s32* args = event->args;
    PartyEntry* party;
    HitEntry* hit;
    f32 tmp, dy, ty, frames, tx, tz, d;
    s32 grounded;
    s32 msec, flag;
#ifdef MUST_MATCH
    // MUST_MATCH: the original's `Vec pos` (12 bytes at 0x8) overlaps MWCC's int<->float
    // conversion temporary, which then lives at 0x10; a full Vec pushes that temp to 0x18.
    // searchUnder2 only writes out->x, so a one-float home is enough to reproduce the frame.
    f32 pos;
#else
    Vec pos;
#endif

    if (isFirstCall) {
        PartyEntry* p;
        args += 1;
        event->userdata[0] = args[-1] ? 1 : partyCtrlNo;
        p = partyGetPtr(event->userdata[0]);
        if (!p) {
            return EVT_RETURN_DONE;
        }
        if (p->field_34 == 0xB) {
            return EVT_RETURN_DONE;
        }
        event->userdata[3] = 0;
        partyChgMot(p, 8);
        partyChgRunMode(p, 9);
    } else {
        args += 1;
    }
    party = partyGetPtr(event->userdata[0]);
    tx = evtGetFloat(event, args[0]);
    ty = evtGetFloat(event, args[1]);
    tz = evtGetFloat(event, args[2]);
    msec = evtGetValue(event, args[3]);
    flag = evtGetValue(event, args[4]);
    d = evtGetFloat(event, args[5]);
    if (d <= float_neg1000_8041bb24) {
        d = float_20_8041bb18;
    }
    frames = (f32)sysMsec2Frame(msec);
    switch (event->userdata[3]) {
        case 0:
            P32(party, 0x13C) = P32(party, 0x138);
            P32(party, 0x138) = 0;
            party->speed = distABf(party->position.x, party->position.z, tx, tz) / frames;
            party->dir = angleABf(party->position.x, party->position.z, tx, tz);
            if (flag == 0) {
                party->field_FC = party->dir;
            }
            partyChgPoseId(party, 4);
            dy = ty - party->position.y;
            tmp = float_0p5_8041bb14 * frames;
            if (__fabsf(dy) < float_1_8041bb10) {
                dy = d;
            } else if (dy >= float_0_8041bb1c) {
                dy += float_37_8041bb20;
#ifdef MUST_MATCH
            // MUST_MATCH: the header declares searchUnder2 as s32, but it returns a pointer;
            // the cast restores the original's unsigned (cmplwi) null test.
            } else if ((HitEntry*)searchUnder2((Vec*)&pos, tx, ty, tz)) {
#else
            } else if (searchUnder2(&pos, tx, ty, tz)) {
#endif
                dy = d;
            }
            dy = (dy + dy) / (tmp * tmp - tmp);
            party->jumpPara1 = dy * tmp;
            PF32(party, 0x174) = -dy;
            event->userdata[2] = (s32)(float_0p5_8041bb14 + tmp);
            event->userdata[3] = 1;
            /* fallthrough */
        case 1:
            movePos(&party->position.x, &party->position.z, party->speed, party->dir);
            party->position.y += party->jumpPara1;
            party->jumpPara1 += PF32(party, 0x174);
            if (--event->userdata[2] <= 0) {
                tmp = float_0p5_8041bb14 * frames;
                dy = ty - party->position.y;
                party->jumpPara1 = float_0_8041bb1c;
                PF32(party, 0x174) = (dy + dy) / (tmp * tmp - tmp);
                party->jumpPara3 = float_0_8041bb1c;
                event->userdata[2] = (s32)(float_0p5_8041bb14 + tmp);
                event->userdata[3] = 2;
            }
            break;
        case 2:
            movePos(&party->position.x, &party->position.z, party->speed, party->dir);
            party->jumpPara1 += PF32(party, 0x174);
            frames = party->jumpPara1;
            grounded = 0;
            hit = partySearchGround(party, __fabsf(frames), __fabsf(frames));
            if (hit) {
                if (party->position.y + frames <= party->groundY) {
                    P32(party, 0x138) = (u32)hit;
                    grounded = 1;
                    party->position.y = party->groundY;
                    party->flags &= ~(PARTY_FLAG_IS_JUMPING | PARTY_FLAG_IS_FALLING | PARTY_FLAG_IS_STEPPING);
                }
            }
            if (grounded) {
                event->userdata[2] = 0;
            } else {
                party->position.y += party->jumpPara1;
            }
            if (--event->userdata[2] <= 0) {
                partyChgPoseId(party, 1);
                party->jumpPara3 = float_0_8041bb1c;
                PF32(party, 0x174) = float_0_8041bb1c;
                party->speed = float_0_8041bb1c;
                unk_800ba2dc(party);
                return 1;
            }
            break;
        default:
            break;
    }
    return 0;
}

USER_FUNC(evt_party_move_behind_mario) {
    PartyEntry* party;
    s32* args = event->args;
    s32 unk;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    unk = evtGetValue(event, args[1]);
    partyMoveBehindMario(party, unk);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_move_beside_mario) {
    PartyEntry* party;
    s32* args = event->args;
    s32 unk;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    unk = evtGetValue(event, args[1]);
    partyMoveBesideMario(party, unk);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_set_homing_dist) {
    s32* args = event->args;
    PartyEntry* party;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    PF32(party, 0x40) = (f32)evtGetValue(event, args[1]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_set_pose) {
    PartyEntry* party;
    s32* args = event->args;
    s32 pose;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    pose = evtGetValue(event, args[1]);
    partyChgPose(party, (char*)pose);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_sleep_on) {
    s32* args = event->args;
    PartyEntry* party;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (party) {
        partyChgPoseId(party, 6);
        party->flags |= PARTY_FLAG_SLEEP_ON;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_sleep_off) {
    s32* args = event->args;
    PartyEntry* party;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (party) {
        partyChgPoseId(party, 1);
        party->flags &= ~PARTY_FLAG_SLEEP_ON;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_set_breed_pose) {
    PartyEntry* party;
    s32* args = event->args;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (party) {
        s32 pose = evtGetValue(event, args[1]);
        partyChgPoseId(party, pose);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(L_evt_party_dokan) {
    s32* args = event->args;
    PartyEntry* party;
    s32 value;
    u8 mode;
    s32 arg;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    mode = party->field_34;
    arg = args[1];
    if (mode == 0xB || mode == 0xC) {
        value = 0;
    } else {
        value = 1;
    }
    evtSetValue(event, arg, value);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_get_status) {
    s32* args = event->args;
    s32 type = args[0];
    s32 out = args[1];
    s32 vivian;

    switch (type) {
        case 6:
            vivian = vivianGetStatus();
            evtSetValue(event, out, vivian);
            break;
        case 3:
            evtSetValue(event, out, bomheiGetStatus());
            break;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_get_name_hitobj_head) {
    s32* args = event->args;
    PartyEntry* party;
    const char* name = NULL;
    HitEntry* hit;
    s32 arg;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    hit = (HitEntry*)P32(party, 0x140);
    arg = args[1];
    if (hit) {
        name = hitGetName(hit);
    }
    evtSetValue(event, arg, (s32)name);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_get_name_hitobj_ride) {
    s32* args = event->args;
    PartyEntry* party;
    const char* name = NULL;
    HitEntry* hit;
    s32 arg;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    hit = (HitEntry*)P32(party, 0x138);
    arg = args[1];
    if (hit) {
        name = hitGetName(hit);
    }
    evtSetValue(event, arg, (s32)name);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_thunders_use) {
    s32* args = event->args;
    PartyEntry* party;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    if (isFirstCall) {
        if (bomheiGetStatus() != 0) {
            return EVT_RETURN_DONE;
        }
        party->flags |= PARTY_FLAG_IS_EVT_USE;
        party->flags |= PARTY_FLAG_IS_BEING_USED;
        partyChgRunMode(party, 3);
    }
    if (bomheiGetStatus() != 0) {
        return 0;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_vivian_hold) {
    s32* args = event->args;
    PartyEntry* party;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    if (isFirstCall) {
        if (vivianGetStatus() != 0) {
            return EVT_RETURN_DONE;
        }
        party->flags |= PARTY_FLAG_IS_EVT_USE;
        party->flags |= PARTY_FLAG_IS_BEING_USED;
        partyChgRunMode(party, 3);
    }
    if (vivianGetStatus() != 2) {
        return 0;
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_vivian_unhold) {
    s32* args = event->args;
    PartyEntry* party;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    if (isFirstCall && vivianGetStatus() == 2) {
        vivianUnhold(party);
    }
    return (vivianGetStatus() == 0) ? EVT_RETURN_DONE : 0;
}

USER_FUNC(L_evt_party_vivian_tail) {
    s32* args = event->args;
    PartyEntry* party;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    if (args[1] == 0) {
        L_partyVivianTailStop(party);
    } else {
        L_partyVivianTailStart(party);
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_yoshi_ride) {
    s32* args = event->args;
    PartyEntry* party;
    s32 flag;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    flag = args[1];
    if (isFirstCall) {
        if (flag != 0) {
            if (yoshiGetStatus() != 0) {
                return EVT_RETURN_DONE;
            }
            party->flags |= PARTY_FLAG_IS_EVT_USE;
            party->flags |= PARTY_FLAG_IS_BEING_USED;
            partyChgRunMode(party, 3);
        } else {
            marioGetoffYoshi();
        }
    }
    if (flag != 0) {
        if (yoshiGetStatus() == 1) {
            party->flags &= ~PARTY_FLAG_IS_EVT_USE;
            return EVT_RETURN_DONE;
        }
    } else {
        if (yoshiGetStatus() == 0) {
            party->flags &= ~PARTY_FLAG_IS_EVT_USE;
            return EVT_RETURN_DONE;
        }
    }
    return 0;
}

USER_FUNC(evt_party_yoshi_fly) {
    s32* args = event->args;
    PartyEntry* party;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    if (isFirstCall) {
        if (yoshiGetStatus() != 1) {
            return EVT_RETURN_DONE;
        }
        party->flags |= PARTY_FLAG_IS_EVT_USE;
        evt_set_yoshi_spd(evtGetFloat(event, args[1]));
        evt_set_yoshi_frm(sysMsec2Frame(evtGetValue(event, args[2])));
    }
    if (evt_get_yoshi_frm() >= 0) {
        return 0;
    }
    party->flags &= ~PARTY_FLAG_IS_EVT_USE;
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_nokotaro_kick) {
    s32* args = event->args;
    PartyEntry* party;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    if (isFirstCall) {
        if (party->currentMemberId != PARTY_MEMBER_KOOPS) {
            return EVT_RETURN_DONE;
        }
        if (party->flags & PARTY_FLAG_IS_BEING_USED) {
            return EVT_RETURN_DONE;
        }
        party->unk16C = 0;
        party->flags |= PARTY_FLAG_IS_EVT_USE;
        party->flags |= PARTY_FLAG_IS_BEING_USED;
        partyChgRunMode(party, 3);
    }
    if (nokonokoGetStatus(party) == 0) {
        party->flags &= ~PARTY_FLAG_IS_EVT_USE;
        return EVT_RETURN_DONE;
    }
    return 0;
}

USER_FUNC(evt_party_nokotaro_kick_hold) {
    s32* args = event->args;
    PartyEntry* party;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    if (isFirstCall) {
        if (party->currentMemberId != PARTY_MEMBER_KOOPS) {
            return EVT_RETURN_DONE;
        }
        if (party->flags & PARTY_FLAG_IS_BEING_USED) {
            return EVT_RETURN_DONE;
        }
        party->unk16C = 5000;
        party->flags |= PARTY_FLAG_IS_EVT_USE;
        party->flags |= PARTY_FLAG_IS_BEING_USED;
        partyChgRunMode(party, 3);
    }
    if (nokonokoGetStatus(party) == 0) {
        party->flags &= ~PARTY_FLAG_IS_EVT_USE;
        return EVT_RETURN_DONE;
    }
    return (nokonokoGetStatus(party) == 3) ? EVT_RETURN_DONE : 0;
}

USER_FUNC(evt_party_nokotaro_hold_cancel) {
    s32* args = event->args;
    PartyEntry* party;
    s32 ret = 0;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    if (isFirstCall) {
        if (party->currentMemberId != PARTY_MEMBER_KOOPS) {
            return EVT_RETURN_DONE;
        }
        if (nokonokoGetStatus(party) == 0) {
            return EVT_RETURN_DONE;
        }
        event->userdata[0] = 0;
    }
    switch (event->userdata[0]) {
        case 0:
            if (nokonokoGetStatus(party) != 3) {
                ret = 0;
                break;
            }
            nokotaro_hold_cancel(party);
            event->userdata[0] = 1;
            break;
        case 1:
            if (nokonokoGetStatus(party) != 0) {
                ret = 0;
                break;
            }
            party->flags &= ~PARTY_FLAG_IS_EVT_USE;
            ret = EVT_RETURN_DONE;
            break;
    }
    return ret;
}

USER_FUNC(evt_party_nokotaro_get_status) {
    s32* args = event->args;
    PartyEntry* party;
    s32 arg;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    if (party->currentMemberId != PARTY_MEMBER_KOOPS) {
        return EVT_RETURN_DONE;
    }
    arg = args[1];
    evtSetValue(event, arg, nokonokoGetStatus(party));
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_cloud_breathout) {
    s32* args = event->args;
    PartyEntry* party;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    if (isFirstCall) {
        if (party->currentMemberId != PARTY_MEMBER_FLURRIE) {
            return EVT_RETURN_DONE;
        }
        if (party->flags & PARTY_FLAG_IS_BEING_USED) {
            return EVT_RETURN_DONE;
        }
        party->unk16C = args[1];
        party->flags |= PARTY_FLAG_IS_EVT_USE;
        party->flags |= PARTY_FLAG_IS_BEING_USED;
        partyChgRunMode(party, 3);
    }
    if (cloudGetStatus() == 0) {
        party->flags &= ~PARTY_FLAG_IS_EVT_USE;
        return EVT_RETURN_DONE;
    }
    return 0;
}

#pragma explicit_zero_data on
const f32 float_1_8041bb10 = 1.0f;
const f32 float_0p5_8041bb14 = 0.5f;
const f32 float_20_8041bb18 = 20.0f;
const f32 float_0_8041bb1c = 0.0f;
const f32 float_37_8041bb20 = 37.0f;
const f32 float_neg1000_8041bb24 = -1000.0f;
const f32 float_180_8041bb28 = 180.0f;
const f32 float_270_8041bb2c = 270.0f;
const f32 float_neg5000_8041bb30 = -5000.0f;
const f32 float_100_8041bb34 = 100.0f;
const f32 float_50_8041bb38 = 50.0f;
#pragma explicit_zero_data reset
