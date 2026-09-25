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

// Raw field accessors for PartyEntry fields that aren't declared in the shared header yet.
#define P8(p, off) (*(u8*)((u8*)(p) + (off)))
#define PS8(p, off) (*(s8*)((u8*)(p) + (off)))
#define P16(p, off) (*(s16*)((u8*)(p) + (off)))
#define P32(p, off) (*(u32*)((u8*)(p) + (off)))
#define PF32(p, off) (*(f32*)((u8*)(p) + (off)))

// local prototypes (not in headers yet)
extern s32 partyCtrlNo;

// Data owned by this translation unit (see config/G8MJ01/splits.txt).
const Vec vec3_802e9668 = {0.0f, -1.0f, 0.0f};
f32 float_1_8041bb10 = 1.0f;
f32 float_0p5_8041bb14 = 0.5f;
f32 float_20_8041bb18 = 20.0f;
f32 float_0_8041bb1c = 0.0f;
f32 float_37_8041bb20 = 37.0f;
f32 float_neg1000_8041bb24 = -1000.0f;
f32 float_180_8041bb28 = 180.0f;
f32 float_270_8041bb2c = 270.0f;
f32 float_neg5000_8041bb30 = -5000.0f;
f32 float_100_8041bb34 = 100.0f;
f32 float_50_8041bb38 = 50.0f;

void partyChgRunMode(PartyEntry* party, s32 mode);
void partyChgMot(PartyEntry* party, s32 mot);
void partyChgPose(PartyEntry* party, char* pose);
char* partyChgPoseId(PartyEntry* party, s32 index);
void partySetForceMove(PartyEntry* party, s32 unk, f32 x, f32 z);
void partySetCamId(PartyEntry* party, s32 cameraId);
void partyInitCamId(PartyEntry* party);
void partyRun(PartyEntry* party);
void partyStop(PartyEntry* party);
void partyCtrlOn(void);
void partyCtrlOff(void);
void partyMoveBesideMario(PartyEntry* party, s32 unk);
void partyMoveBehindMario(PartyEntry* party, s32 unk);
void partyClearFootmark(void);
void unk_800ba2dc(PartyEntry* party);
s32 nokonokoGetStatus(PartyEntry* party);
s32 cloudGetStatus(void);
void nokotaro_hold_cancel(PartyEntry* party);
s32 bomheiGetStatus(void);
s32 yoshiGetStatus(void);
void marioGetoffYoshi(void);
void evt_set_yoshi_spd(f32 spd);
void evt_set_yoshi_frm(s32 frm);
s32 evt_get_yoshi_frm(void);
void L_partyVivianTailStart(PartyEntry* party);
void L_partyVivianTailStop(PartyEntry* party);
void vivianUnhold(PartyEntry* party);
HitEntry* partySearchGround(PartyEntry* party, f32 a, f32 b);
HitEntry* partyHitCheck(PartyEntry* party, Vec* pos, Vec* dir, Vec* hitPos, Vec* hitNormal, f32* dist);
s32 searchUnder2(Vec* out, f32 x, f32 y, f32 z);
f32 toMovedirSimple(f32 angle);

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
        party->flags |= 0x80000000;
        party->flags |= 0x100;
        partyChgRunMode(party, 3);
    }
    if (cloudGetStatus() == 0) {
        party->flags &= ~0x80000000;
        return EVT_RETURN_DONE;
    }
    return 0;
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
            party->flags &= ~0x80000000;
            ret = EVT_RETURN_DONE;
            break;
    }
    return ret;
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
        if (party->flags & 0x100) {
            return EVT_RETURN_DONE;
        }
        party->unk16C = 5000;
        party->flags |= 0x80000000;
        party->flags |= 0x100;
        partyChgRunMode(party, 3);
    }
    if (nokonokoGetStatus(party) == 0) {
        party->flags &= ~0x80000000;
        return EVT_RETURN_DONE;
    }
    return (nokonokoGetStatus(party) == 3) ? EVT_RETURN_DONE : 0;
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
        if (party->flags & 0x100) {
            return EVT_RETURN_DONE;
        }
        party->unk16C = 0;
        party->flags |= 0x80000000;
        party->flags |= 0x100;
        partyChgRunMode(party, 3);
    }
    if (nokonokoGetStatus(party) == 0) {
        party->flags &= ~0x80000000;
        return EVT_RETURN_DONE;
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
        party->flags |= 0x80000000;
        evt_set_yoshi_spd(evtGetFloat(event, args[1]));
        evt_set_yoshi_frm(sysMsec2Frame(evtGetValue(event, args[2])));
    }
    if (evt_get_yoshi_frm() >= 0) {
        return 0;
    }
    party->flags &= ~0x80000000;
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
            party->flags |= 0x80000000;
            party->flags |= 0x100;
            partyChgRunMode(party, 3);
        } else {
            marioGetoffYoshi();
        }
    }
    if (flag != 0) {
        if (yoshiGetStatus() == 1) {
            party->flags &= ~0x80000000;
            return EVT_RETURN_DONE;
        }
    } else {
        if (yoshiGetStatus() == 0) {
            party->flags &= ~0x80000000;
            return EVT_RETURN_DONE;
        }
    }
    return 0;
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
        party->flags |= 0x80000000;
        party->flags |= 0x100;
        partyChgRunMode(party, 3);
    }
    if (vivianGetStatus() != 2) {
        return 0;
    }
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
        party->flags |= 0x80000000;
        party->flags |= 0x100;
        partyChgRunMode(party, 3);
    }
    if (bomheiGetStatus() != 0) {
        return 0;
    }
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
    mode = P8(party, 0x34);
    arg = args[1];
    if (mode == 0xB || mode == 0xC) {
        value = 0;
    } else {
        value = 1;
    }
    evtSetValue(event, arg, value);
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

// NONMATCHING (91.54%): float allocation for tx/tz/d vs the dy/tmp temporaries is shifted by
// two registers, party/grounded use r31/r28 instead of r28/r29, int->float conversion stack
// slots differ, and the partySearchGround result is tested with cmpwi instead of cmplwi.
USER_FUNC(evt_party_jump_pos) {
    s32* args = event->args;
    PartyEntry* party;
    HitEntry* hit;
    f32 dy, tmp, frames, tx, ty, tz, d;
    s32 msec, flag;
    s32 grounded;
    Vec pos;

    if (isFirstCall) {
        args += 1;
        event->userdata[0] = args[-1] ? 1 : partyCtrlNo;
        party = partyGetPtr(event->userdata[0]);
        if (!party) {
            return EVT_RETURN_DONE;
        }
        if (P8(party, 0x34) == 0xB) {
            return EVT_RETURN_DONE;
        }
        event->userdata[3] = 0;
        partyChgMot(party, 8);
        partyChgRunMode(party, 9);
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
            PF32(party, 0x104) = distABf(party->position.x, party->position.z, tx, tz) / frames;
            PF32(party, 0x100) = angleABf(party->position.x, party->position.z, tx, tz);
            if (flag == 0) {
                PF32(party, 0xFC) = PF32(party, 0x100);
            }
            partyChgPoseId(party, 4);
            dy = ty - party->position.y;
            tmp = float_0p5_8041bb14 * frames;
            if (__fabsf(dy) < float_1_8041bb10) {
                dy = d;
            } else if (dy >= float_0_8041bb1c) {
                dy += float_37_8041bb20;
            } else if (searchUnder2(&pos, tx, ty, tz)) {
                dy = d;
            }
            dy = (dy + dy) / (tmp * tmp - tmp);
            PF32(party, 0x114) = dy * tmp;
            PF32(party, 0x174) = -dy;
            event->userdata[2] = (s32)(float_0p5_8041bb14 + tmp);
            event->userdata[3] = 1;
            /* fallthrough */
        case 1:
            movePos(&party->position.x, &party->position.z, PF32(party, 0x104), PF32(party, 0x100));
            party->position.y += PF32(party, 0x114);
            PF32(party, 0x114) += PF32(party, 0x174);
            if (--event->userdata[2] <= 0) {
                tmp = float_0p5_8041bb14 * frames;
                PF32(party, 0x114) = float_0_8041bb1c;
                dy = ty - party->position.y;
                PF32(party, 0x174) = (dy + dy) / (tmp * tmp - tmp);
                PF32(party, 0x11C) = float_0_8041bb1c;
                event->userdata[2] = (s32)(float_0p5_8041bb14 + tmp);
                event->userdata[3] = 2;
            }
            break;
        case 2:
            movePos(&party->position.x, &party->position.z, PF32(party, 0x104), PF32(party, 0x100));
            PF32(party, 0x114) += PF32(party, 0x174);
            frames = PF32(party, 0x114);
            grounded = 0;
            hit = partySearchGround(party, __fabsf(frames), __fabsf(frames));
            if (hit) {
                if (party->position.y + frames <= PF32(party, 0xE4)) {
                    P32(party, 0x138) = (u32)hit;
                    grounded = 1;
                    party->position.y = PF32(party, 0xE4);
                    party->flags &= ~(PARTY_FLAG_IS_JUMPING | PARTY_FLAG_IS_FALLING | PARTY_FLAG_IS_STEPPING);
                }
            }
            if (grounded) {
                event->userdata[2] = 0;
            } else {
                party->position.y += PF32(party, 0x114);
            }
            if (--event->userdata[2] <= 0) {
                partyChgPoseId(party, 1);
                PF32(party, 0x11C) = float_0_8041bb1c;
                PF32(party, 0x174) = float_0_8041bb1c;
                PF32(party, 0x104) = float_0_8041bb1c;
                unk_800ba2dc(party);
                return 1;
            }
            break;
        default:
            break;
    }
    return 0;
}

USER_FUNC(evt_party_wait_landon) {
    s32* args = event->args;
    PartyEntry* party;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        return EVT_RETURN_DONE;
    }
    return (party->flags & PARTY_FLAG_IS_GROUNDED) ? EVT_RETURN_DONE : 0;
}

// NONMATCHING (98.04%): only the two int->float conversions of gp->framerate differ: the
// target reuses f4 for the 2^52 constant and stack slots 0x10/0x18; ours reloads it into f5
// and swaps the slots. The constant reloc is also anonymous @611@ vs double_to_int_802e9678.
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
        breed = P8(party, 0x34);
        if (breed != 0 && (u8)(breed - 1) > 1) {
            return 0;
        }
        event->userdata[2] = 1;
        x = evtGetFloat(event, args[1]);
        z = evtGetFloat(event, args[2]);
        len = evtGetFloat(event, args[3]);
        dist = distABf(party->position.x, party->position.z, x, z);
        PF32(party, 0x100) = angleABf(party->position.x, party->position.z, x, z);
        frames = (s32)((f32)gp->framerate * (dist / len));
        spd = len / (f32)gp->framerate;
        partySetForceMove(party, frames, PF32(party, 0x100), spd);
    }
    if (P16(party, 0x28) == 0) {
        return EVT_RETURN_DONE;
    }
    return 0;
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
        breed = P8(party, 0x34);
        if (breed != 0 && (u8)(breed - 1) > 1) {
            return 0;
        }
        event->userdata[2] = 1;
        x = evtGetFloat(event, args[1]);
        z = evtGetFloat(event, args[2]);
        event->userdata[1] = sysMsec2Frame(evtGetValue(event, args[3]));
        PF32(party, 0x100) = angleABf(party->position.x, party->position.z, x, z);
        if (event->userdata[1] == 0) {
            event->userdata[1] = (s32)(distABf(party->position.x, party->position.z, x, z) / PF32(party, 0x104));
            spd = PF32(party, 0x104);
        } else {
            spd = distABf(party->position.x, party->position.z, x, z) / (f32)event->userdata[1];
        }
        partySetForceMove(party, event->userdata[1], PF32(party, 0x100), spd);
    }
    if (--event->userdata[1] < 0) {
        return 1;
    }
    return 0;
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

USER_FUNC(evt_party_stop) {
    s32* args = event->args;
    PartyEntry* party;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (party) {
        partyStop(party);
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
    PF32(party, 0x110) = reviseAngle(evtGetFloat(event, args[1]));
    PF32(party, 0x10C) = PF32(party, 0x110);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_get_dispdir) {
    s32* args = event->args;
    PartyEntry* party;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        evtSetFloat(event, args[1], 0.0f);
    } else {
        evtSetFloat(event, args[1], PF32(party, 0x10C));
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_party_get_pos) {
    s32* args = event->args;
    PartyEntry* party;

    party = partyGetPtr(args[0] ? 1 : partyCtrlNo);
    if (!party) {
        evtSetFloat(event, args[1], -5000.0f);
        evtSetFloat(event, args[2], -5000.0f);
        evtSetFloat(event, args[3], -5000.0f);
        return EVT_RETURN_DONE;
    }
    evtSetFloat(event, args[1], party->position.x);
    evtSetFloat(event, args[2], party->position.y);
    evtSetFloat(event, args[3], party->position.z);
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
    PF32(party, 0x110) = (delta < float_180_8041bb28) ? float_180_8041bb28 : float_0_8041bb1c;
    PF32(party, 0x100) = delta;
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
        x = PF32(npc, 0x8C);
        z = PF32(npc, 0x94);
        camera = camGetPtr(CAMERA_3D);
        yaw = camera->viewYaw;
        angle = angleABf(party->position.x, party->position.z, x, z);
        delta = revise360(angle - yaw);
        PF32(party, 0x110) = (delta < float_180_8041bb28) ? float_180_8041bb28 : float_0_8041bb1c;
        PF32(party, 0x100) = delta;
    }
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
        event->userdataf[1] = PF32(party, 0x10C);
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
        PF32(party, 0x10C) = next;
        PF32(party, 0x110) = PF32(party, 0x10C);
        PF32(party, 0x100) = toMovedirSimple(PF32(party, 0x110));
        return (++event->userdata[2] > frames) ? 1 : 0;
    }
    if (frames < 0) {
        next = revise360(float_270_8041bb2c - target);
        PF32(party, 0x110) = next;
        PF32(party, 0x100) = toMovedirSimple(PF32(party, 0x110));
    } else if (frames == 0) {
        next = revise360(float_270_8041bb2c - target);
        PF32(party, 0x10C) = next;
        PF32(party, 0x110) = PF32(party, 0x10C);
        PF32(party, 0x100) = toMovedirSimple(PF32(party, 0x110));
    }
    return EVT_RETURN_DONE;
}

// NONMATCHING (91.90%): party/mario land in swapped callee-saved regs, the marioGetScreenPos
// out slots (sx/sy/sz, Vec temporaries) sit 4 bytes lower, 100.0f/50.0f land in f31/f30
// instead of f30/f31, the loop in-screen test uses `beq`+a duplicated return instead of a
// `bne` to the shared return, and the partyHitCheck Vec argument slots differ.
USER_FUNC(evt_party_force_reset_outofscreen) {
    s32* args = event->args;
    MarioWork* mario;
    PartyEntry* party;
    f32 dist;
    Vec hitNormal;
    Vec hitPos;
    Vec dir;
    Vec tmp;
    f32 sx, sy, sz;
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
            return EVT_RETURN_DONE;
        }
        angle = angleABf(party->position.x, party->position.z, mario->position.x, mario->position.z);
        movePos(&party->position.x, &party->position.z, float_1_8041bb10, angle);
        tmp = party->position;
        tmp.y += up;
        dist = maxDist;
        if (partyHitCheck(party, &tmp, &dir, &hitPos, &hitNormal, &dist)) {
            party->position.y = hitPos.y;
        } else {
            party->position.y = mario->position.y;
        }
    }
}
