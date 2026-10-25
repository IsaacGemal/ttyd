#include "pmario_sound.h"
#include "party.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "mario/mario_hit.h"
#include "mario/mario_party.h"
#include "mario/mario_sbr.h"
#include "driver/camdrv.h"
#include "driver/hitdrv.h"
#include "system.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "dolphin/types.h"
#include "math_ext.h"

#define fabs(x) __fabs(x)
#define fabsf(x) __fabsf(x)
#include "party/party_move.h"
#include "party/party_hit.h"
#include "party/party_motion.h"
#include "party/party_bomhei.h"
#include "motion/mot_stay.h"
#include "motion/mot_slit.h"
#include "party/party_chuchu.h"
#include "party/party_nokonoko.h"

// The unit's named .rodata objects, in the original's order ahead of the pooled literals.
#ifdef MUST_MATCH
// MUST_MATCH: non-const so MWCC keeps the loads and copies instead of folding the known contents.
#define PM_RODATA __declspec(section ".rodata")
#else
#define PM_RODATA const
#endif
PM_RODATA Vec vec3_802f3e40 = { 0.0f, 0.0f, 0.0f };
PM_RODATA Vec vec3_802f3e4c = { 0.0f, 0.0f, 0.0f };
PM_RODATA Vec vec3_802f3e58 = { 0.0f, 0.0f, 0.0f };
PM_RODATA Vec vec3_802f3e64 = { 0.0f, 0.0f, 0.0f };
PM_RODATA Vec vec3_802f3e70 = { 0.0f, 0.0f, 0.0f };
PM_RODATA Vec vec3_802f3e7c = { 0.0f, 0.0f, 0.0f };
PM_RODATA Vec vec3_802f3e88 = { 0.0f, 0.0f, 0.0f };
PM_RODATA Vec vec3_802f3e94 = { 0.0f, 0.0f, 0.0f };
PM_RODATA Vec vec3_802f3ea0 = { 0.0f, 1.0f, 0.0f };
PM_RODATA Vec vec3_802f3eac = { 0.0f, -1.0f, 0.0f };
PM_RODATA Vec vec3_802f3eb8 = { 0.0f, -1.0f, 0.0f };
PM_RODATA Vec vec3_802f3ec4 = { 0.0f, -1.0f, 0.0f };
PM_RODATA Vec vec3_802f3ed0 = { 0.0f, -1.0f, 0.0f };
PM_RODATA Vec vec3_802f3edc = { 0.0f, -1.0f, 0.0f };
PM_RODATA Vec vec3_802f3ee8 = { 0.0f, -1.0f, 0.0f };
PM_RODATA Vec vec3_802f3ef4 = { 0.0f, -1.0f, 0.0f };
PM_RODATA Vec vec3_802f3f00 = { 122.0f, 130.0f, -313.0f };
PM_RODATA char str_PCH_A2_1[12] = "PCH_A2_1";
PM_RODATA char str_802F3F18[8] = "gor_01";
PM_RODATA char str_A_ie_03_yuka_f1_C[20] = "A_ie_03_yuka_f1_C";
PM_RODATA char str_A_ie_03_yuka_f2_C[20] = "A_ie_03_yuka_f2_C";

#pragma pool_data on // the original addresses its .rodata through one base register

// LOCAL_PROTO(psndSFXOnEx_3D): retail event callers narrow volume and pan to u8; the implementation uses s32 and sharing that signature regresses matched event code.
s32 psndSFXOnEx_3D(const char* name, Vec* position, u8 vol, u8 pan, u16 pitch, u16 flags);

// LOCAL_PROTO(psndSFX_pit): retail callers disagree on pitch width and signedness; the shared u16 signature regresses matched sound commands.
void psndSFX_pit(s32 id, s16 pitch);

// LOCAL_PROTO(psndSFX_vol): retail units disagree on volume width; a shared u8 prototype adds narrowing instructions in battle callers.
extern void psndSFX_vol(s32 id, u8 vol);

// .sbss
// MUST_MATCH: MWCC emits uninitialised globals in reverse declaration order.
#ifdef MUST_MATCH
// Keep the original section tail even though no code references it.
#pragma force_active on
static u32 gap_08_804189F4_sbss;
#pragma force_active reset
#endif
f32 touch_wall_ct;
s32 upstairs_frames;
f32 upstairs_step_y;

#ifdef MUST_MATCH
// MUST_MATCH: retail reads these named .rodata objects (the compiler reaches them off the section base).
#define FB(sym, val) (sym)
#define FV(sym, val) (sym)
#else
#define FB(sym, val) (val)
#define FV(sym, val) (val)
#endif
#define PT_FP_NAN 1
#define PT_FP_INFINITE 2
#define PT_FP_ZERO 3
#define PT_FP_NORMAL 4
#define PT_FP_SUBNORMAL 5
extern int __float_nan[];
#define PT_NAN (*(f32*)__float_nan)
#define pt_fpclassify(x) pt_fpclassifyf(x)
#define pt_isnan(x) (pt_fpclassify(x) == PT_FP_NAN)

USER_FUNC(evt_snd_bgmon) {
    psndBGMOn((u32)event->args[0], (char*)event->args[1]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_bgmon_f) {
    s32* args = event->args;
    s32 a = args[0];
    s32 b = args[1];
    s32 c = evtGetValue(event, args[2]);
    psndBGMOn_f(a, (char*)b, c, 0);
    return EVT_RETURN_DONE;
}

USER_FUNC(unk_8014cc24) {
    s32* args = event->args;
    s32 a = args[0];
    s32 b = args[1];
    s32 c = evtGetValue(event, args[2]);
    evtGetValue(event, args[3]);
    psndBGMOn_f_d(a, b, c, c, 0);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_bgmoff) {
    psndBGMOff(event->args[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_bgmoff_f) {
    s32* args = event->args;
    s32 id = args[0];
    s32 value = evtGetValue(event, args[1]);
    psndBGMOff_f_d(id, value, 0);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_bgm_freq) {
    s32* args = event->args;
    s32 id = args[0];
    s32 rate = evtGetValue(event, args[1]);
    psndBGM_rate(id, (u16)rate);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_bgm_scope) {
    s32* args = event->args;
    s32 id = args[0];
    s32 scope = evtGetValue(event, args[1]);
    psndBGMScope(id, scope);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfxon) {
    s32* args = event->args;
    s32 index = args[1];
    const char* name = (const char*)args[0];
    if (index == 0) {
        psndSFXOn(name);
    } else {
        evtSetValue(event, index, psndSFXOn(name));
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfxon__) {
    s32* args = event->args;
    s32 value = evtGetValue(event, args[0]);
    s32 index = args[1];
    if (index == 0) {
        psndSFXOn((const char*)value);
    } else {
        evtSetValue(event, index, psndSFXOn((const char*)value));
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfxon_) {
    s32* args = event->args;
    s32 index = args[1];
    const char* name = (const char*)args[0];
    if (index == 0) {
        psndSFXOn_(name);
    } else {
        evtSetValue(event, index, psndSFXOn_(name));
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfxon_3d) {
    Vec position;
    s32* args = event->args;
    const char* name = (const char*)args[0];
    s32 index;
    position.x = evtGetFloat(event, args[1]);
    position.y = evtGetFloat(event, args[2]);
    position.z = evtGetFloat(event, args[3]);
    index = args[4];
    if (index == 0 || (u32)(index + 0x0EE70000) == 0x4D80U) {
        psndSFXOn_3D(name, &position);
    } else {
        evtSetValue(event, index, psndSFXOn_3D(name, &position));
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfxon_3d_ex) {
    Vec position;
    s32* args = event->args;
    const char* name = (const char*)args[0];
    u16 vol;
    u16 pan;
    u16 pitch;
    u16 flags;
    s32 index;
    position.x = evtGetFloat(event, args[1]);
    position.y = evtGetFloat(event, args[2]);
    position.z = evtGetFloat(event, args[3]);
    vol = (u16)evtGetValue(event, args[4]);
    pan = (u16)evtGetValue(event, args[5]);
    pitch = (u16)evtGetValue(event, args[6]);
    flags = (u16)evtGetValue(event, args[7]);
    index = args[8];
    if (index == 0) {
        psndSFXOnEx_3D(name, &position, vol, pan, pitch, flags);
    } else {
        evtSetValue(event, index, psndSFXOnEx_3D(name, &position, vol, pan, pitch, flags));
    }
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfxoff) {
    s32 id = evtGetValue(event, event->args[0]);
    psndSFXOff(id);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfxchk) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    s32 result = psndSFXChk((u32)id);
    evtSetValue(event, args[1], result);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfx_pit) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    s32 pitch = evtGetValue(event, args[1]);
    psndSFX_pit(id, (s16)pitch);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfx_vol) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    s32 vol = evtGetValue(event, args[1]);
    psndSFX_vol(id, (u8)vol);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfx_pos) {
    Vec position;
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    position.x = evtGetFloat(event, args[1]);
    position.y = evtGetFloat(event, args[2]);
    position.z = evtGetFloat(event, args[3]);
    psndSFX_pos(id, &position);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfx_dist) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    u16 dist = (u16)evtGetValue(event, args[1]);
    psndSFX_dist(id, dist);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_envon) {
    psndENVOn(event->args[0], event->args[1]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_envon_f) {
    s32* args = event->args;
    s32 id = args[0];
    s32 a = args[1];
    s32 b = evtGetValue(event, args[2]);
    psndENVOn_f_d(id, a, b, 0);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_envoff) {
    psndENVOff((u32)event->args[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_envoff_f) {
    s32* args = event->args;
    s32 id = args[0];
    s32 value = evtGetValue(event, args[1]);
    psndENVOff_f_d(id, value, 0);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_env_lpf) {
    s32* args = event->args;
    s32 id = args[0];
    u16 freq = (u16)evtGetValue(event, args[1]);
    psndENV_LPF(id, freq);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_set_rev_mode) {
    psndSetReverb(event->args[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfx_all_stop) {
    psndSFXAllOff();
    return EVT_RETURN_DONE;
}

void partyResetTouchWallCt(void) {
    touch_wall_ct = 60.0f;
}

void N_partyFollowCloseOn(PartyEntry* party) {
    party->trigFlags = (party->trigFlags | 0x20000);
}

void N_partyFollowCloseOff(PartyEntry* party) {
    party->trigFlags = (party->trigFlags & ~0x20000);
}

void party_motion_stay(PartyEntry* party) {
    MarioWork* mario;
    HitEntry* hit;
    PartyEntry* other;
    s32 id;
    f32 f31;
    f32 f30;
    f32 f29;
    f32 px;
    f32 pz;
    f32 angle;
    f32 speed;
    Vec pos;
    Vec targetPos;

    mario = party->playerPtr;
    id = partyGetFootmarkId(&party->flags);
    if (party->trigFlags & 0x2) {
        party->trigFlags = (party->trigFlags & ~0x2);
        partyChgPoseId(party, 1);
        if (!(party->trigFlags & 0x800000)) {
            if (!(party->flags & PARTY_FLAG_IS_PARTNER)) {
                partyClearFootmark();
            } else {
                partyClearFootmark2();
            }
        }
        party->trigFlags = (party->trigFlags & ~0x800000);
        party->yoshiPsndSFXId = -1;
        partySetSpeed(party, 0.0f);
    }
    if (mario->currMotionId != 0xe && (party->flags & PARTY_FLAG_IS_BEHIND_MARIO)) {
        party->flags = (party->flags & ~PARTY_FLAG_IS_BEHIND_MARIO);
    }
    f31 = party->homingDist;
    if (!mario->controlStickSensitivity) {
        if (mario->baseSpeed < 1.0f) {
            f31 = 100.0f;
        }
    }
    if (mario->characterId == 2) {
        f31 = 70.0f;
    }
    if (strcmp(gp->currentMapName, "aji_02") == 0 ||
        strcmp(gp->currentMapName, "aji_05") == 0 ||
        strcmp(gp->currentMapName, "aji_12") == 0) {
        if (!marioKeyOffChk()) {
            if (party->isFlyer == 0) {
                f30 = 18.0f;
            } else if (party->currentMemberId == PARTY_MEMBER_FLURRIE) {
                if (party->trigFlags & 0x200000) {
                    f30 = 8.0f;
                } else {
                    f30 = 30.0f;
                }
            } else if (party->currentMemberId == PARTY_MEMBER_VIVIAN) {
                if (party->trigFlags & 0x200000) {
                    f30 = 8.0f;
                } else {
                    f30 = 30.0f;
                }
            }
            f31 = f30;
            party->trigFlags = (party->trigFlags | 0x20000);
        }
    }
    if ((party->trigFlags >> 17) & 1) {
        if (party->isFlyer == 0) {
            f29 = 18.0f;
        } else if (party->currentMemberId == PARTY_MEMBER_FLURRIE) {
            if (party->trigFlags & 0x200000) {
                f29 = 8.0f;
            } else {
                f29 = 30.0f;
            }
        } else if (party->currentMemberId == PARTY_MEMBER_VIVIAN) {
            if (party->trigFlags & 0x200000) {
                f29 = 8.0f;
            } else {
                f29 = 30.0f;
            }
        }
        f31 = f29;
    }
    party_slit(party);
    party_roll(party);
    partyChgIdlePose(party);
    if (mario->currMotionId == 0xe && !(party->flags & PARTY_FLAG_IS_BEHIND_MARIO)) {
        partyMoveBehindMario(party, 0);
        return;
    }
    if (party->runMode != 0) {
        if (party->currentSlotId == 0 || partyGetPtr(marioGetPartyId()) == NULL) {
            s32 flag;
            if (party->playerPtr->airTimer != 0 && party->playerPtr->baseSpeed == 0.0f) {
                flag = 0;
            } else if (party->playerPtr->currMotionId == 0x1f ||
                       party->playerPtr->currMotionId == 0x20 ||
                       party->playerPtr->currMotionId == 0x10 ||
                       party->playerPtr->currMotionId == 0x11) {
                flag = 0;
            } else {
                flag = 1;
            }
            if (flag != 0) {
                if (party->isFlyer == 0) {
                    targetPos = party->position;
                    f29 = partyGetTargetDist3D(id, &targetPos);
                    partyGetTargetDist(id, party->position.x, party->position.z);
                } else {
                    px = party->position.x;
                    pz = party->position.z;
                    movePos(&px, &pz, 0.5f * party->radius, angleABf(party->position.x, party->position.z, mario->position.x, mario->position.z));
                    f29 = distABf(px, pz, mario->position.x, mario->position.z);
                }
                if (marioChkSlitThrouh() || f29 >= f31) {
                    if (!marioChkDeepSleep()) {
                        partyChgMot(party, 1);
                    }
                }
            }
        } else {
            other = anotherPartyGetPtr(party->currentSlotId);
            if (other != NULL) {
                if (nokonokoGetStatus(other) == 3 || PSVECDistance(&mario->position, &other->position) >= 150.0f) {
                    if (partyGetTargetDist(id, party->position.x, party->position.z) >= f31) {
                        partyChgMot(party, 1);
                        partyClearFootmark2();
                    }
                } else if (other->currentMemberId == PARTY_MEMBER_BOBBERY && other->runMode == 3) {
                    if (mario->currMotionId != 0x1f && mario->currMotionId != 0x10 && mario->currMotionId != 0x11) {
                        if (distABf(party->position.x, party->position.z, mario->position.x, mario->position.z) >= f31) {
                            partyChgMot(party, 1);
                        }
                    }
#ifdef MUST_MATCH
                // No-op cast: the value is already s8, but the cast changes the object.
                } else if (!(other->flags & PARTY_FLAG_IS_BEING_USED) || (s8)other->currentMemberId == PARTY_MEMBER_YOSHI) {
#else
                } else if (!(other->flags & PARTY_FLAG_IS_BEING_USED) || other->currentMemberId == PARTY_MEMBER_YOSHI) {
#endif
                    if (distABf(party->position.x, party->position.z, other->position.x, other->position.z) >= f31) {
                        partyChgMot(party, 1);
                    }
                }
                if (marioChkSlitThrouh()) {
                    partyChgMot(party, 1);
                }
            }
        }
    }
    pos = party->position;
    partyGetMoveDirSpd(party, &angle, &speed);
    hit = partySearchFrontWall(party, &pos, speed, angle);
    if (hit != NULL) {
        party->frontWallHit = hit;
        if (party->speed) {
            party->pushWallHit = hit;
        }
    }
    party->position.x = pos.x;
    party->position.z = pos.z;
    partyGetMoveDirSpd(party, &angle, &speed);
    marioChkWallAround(&pos, 1, speed, angle, party->radius, party->height);
    party->position.x = pos.x;
    party->position.z = pos.z;
    if (party->isFlyer == 1) {
        partyFollowHeight(party);
        return;
    }
    partyUpdateFloor(party);
    party_force_reset_outofscreen(party);
    if (party->currentMemberId == PARTY_MEMBER_MS_MOWZ && !(party->flags2 & 0x8000) && !marioKeyOffChk() && marioChkDeepSleep() != 1 && chuchu_searchObject()) {
        switch (party->useMotionId) {
        case 1:
            if (--party->timer <= 0) {
                partyChgPoseId(party, 1);
                party->timer = 0x1e;
                party->useMotionId = 2;
            }
            break;
        case 2:
            if (--party->timer <= 0) {
                party->useMotionId = 0;
            }
            break;
        // `case 0` is redundant with `default` but is what makes mwcc
        // build the 3-entry {0,1,2} search tree the original has.
        case 0:
        default:
            partyChgPose(party, FB(str_PCH_A2_1, "PCH_A2_1"));
            party->useMotionId = 1;
            party->timer = 0x1e;
            break;
        }
    }
}

void party_motion_idle(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;

    if (party->trigFlags & 0x2) {
        party->trigFlags = (party->trigFlags & ~0x2);
        partyChgPoseId(party, 1);
        if (!(party->flags & PARTY_FLAG_IS_PARTNER)) {
            partyClearFootmark();
        } else {
            partyClearFootmark2();
        }
        party->yoshiPsndSFXId = -1;
    }
    party_slit(party);
    party_roll(party);
    partyChgIdlePose(party);
    if (!(party->flags & PARTY_FLAG_DISABLE_FRONT_WALL_CHK)) {
        partyChkWall(party);
    }
    partyChkGnd(party);
    if (distABf(party->position.x, party->position.z, mario->position.x, mario->position.z) > 31.0f) {
        partyChgRunMode(party, 2);
    }
    if (mario->currMotionId == 0xE && !(party->flags & PARTY_FLAG_IS_BEHIND_MARIO)) {
        partyMoveBehindMario(party, 0);
    } else if (party->isFlyer == 1) {
        partyFollowHeight(party);
    } else {
        partyUpdateFloor(party);
        if (party->currentMemberId == PARTY_MEMBER_MS_MOWZ && !(party->flags2 & 0x8000)) {
            if (!marioKeyOffChk() && marioChkDeepSleep() != 1 && chuchu_searchObject() != 0) {
                switch (party->useMotionId) {
                case 1:
                    if (--party->timer <= 0) {
                        partyChgPoseId(party, 1);
                        party->timer = 0x1E;
                        party->useMotionId = 2;
                    }
                    break;
                case 2:
                    if (--party->timer <= 0) {
                        party->useMotionId = 0;
                    }
                    break;
                // `case 0` is redundant with `default` but is what makes mwcc
                // build the 3-entry {0,1,2} search tree the original has.
                case 0:
                default:
                    partyChgPose(party, FB(str_PCH_A2_1, "PCH_A2_1"));
                    party->useMotionId = 1;
                    party->timer = 0x1E;
                    break;
                }
            }
        }
    }
}

// 100%: hoisting `hit = NULL;` to the top of the `if (attr & 0x200)` block (before the
// `splashTimer == 0` check) is what the original did; it also made MWCC colour `party`=r29,
// `attr`=r30, `hit`=r31 instead of the swapped assignment.
void partyUpdateFloor(PartyEntry* party) {
    HitEntry* hit;
    s32 attr;
    Vec position = party->position;
    f32 angle = party->height;

    hit = partySearchHead(party, &position, &angle, party->dispDir);
    if (hit != NULL) {
        party->headHit = hit;
    }
    if (party->flags & PARTY_FLAG_DISABLE_FLOORS) {
        return;
    }
    party->flags = (party->flags & ~PARTY_FLAG_IN_SHALLOW_WATER);
    hit = partySearchGround(party, 15.0f, -15.0f);
    if (hit != NULL) {
        attr = hitGetAttr(hit);
        if (attr & 0x200) {
            hit = NULL;
            if (party->splashTimer == 0) {
                party->trigFlags = (party->trigFlags | 0x1000000);
                party->flags = (party->flags | PARTY_FLAG_FORCE_FALL);
            }
        }
    }
    party->groundHit = hit;
    if (hit == NULL) {
        party->flags = (party->flags & ~PARTY_FLAG_IS_GROUNDED);
        partyChgMot(party, 3);
        party->jumpPara3 = -1.5f;
        return;
    }
    party->position.y = party->groundY;
    party->flags = (party->flags | PARTY_FLAG_IS_GROUNDED);
    if (attr & 0x100) {
        party->flags = (party->flags | PARTY_FLAG_IN_SHALLOW_WATER);
    } else {
        party->flags = (party->flags & ~PARTY_FLAG_IN_SHALLOW_WATER);
    }
    if (attr & 0x40000000) {
        party->field_0x14C = hit;
    }
    if (partyHitIsBoxMobj(hit) != 0) {
        partyGetAppearPos5(party, hit, &party->targetPos);
        party->trigFlags = (party->trigFlags | 0x400000);
        party->targetPos.y = party->position.y;
        partyChgMot(party, 2);
    }
}

// The `f3 = 0.1f` reloads in each arm and the repeated `f2 = mario->position.y`
// are needed to reproduce the original scheduler. In the else arm, assigning
// `f1 = 0.05f` to a variable (rather than using the literal inline) makes the
// final `fmadds` emit `f0,f2,f1` instead of `f0,f1,f2`.
void partyFollowHeight(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    HitEntry* hit;
    s32 flag;
    s32 attr;
    f32 f1, f2, f3, f0;

    if (party->flags & PARTY_FLAG_DISABLE_FLOORS) {
        return;
    }
    hit = partySearchGround(party, 10.0f, 0.0f);
    if (hit != NULL) {
        attr = hitGetAttr(hit);
        if (attr & 0x800) {
            partyChgRunMode(party, 0xD);
            return;
        }
        if (attr & 0x100) {
            party->flags = (party->flags | PARTY_FLAG_IN_SHALLOW_WATER);
        } else {
            party->flags = (party->flags & ~PARTY_FLAG_IN_SHALLOW_WATER);
        }
    }
    if (party->playerPtr->airTimer != 0 && party->playerPtr->baseSpeed == 0.0f) {
        flag = 0;
    } else {
        if (party->playerPtr->currMotionId == 0x1F || party->playerPtr->currMotionId == 0x20
            || party->playerPtr->currMotionId == 0x10 || party->playerPtr->currMotionId == 0x11) {
            flag = 0;
        } else {
            flag = 1;
        }
    }
    if (flag != 0) {
        f3 = 0.05f;
        if (hit != NULL) {
            f0 = mario->position.y;
            f1 = party->groundY;
            if (f0 > f1) {
                f2 = f0;
                f3 = 0.1f;
            } else {
                f2 = f1;
                f3 = 0.1f;
            }
            if (party->position.y < f1) {
                f2 = 18.5f + f1;
                f3 = 0.1f;
            }
        } else {
            f2 = mario->position.y;
        }
        f0 = 5.0f;
        if (party->flags2 & 0x8000) {
            f0 = 1.0f;
        }
        f2 = f2 + f0;
        f1 = f2 - party->position.y;
        if (f1 > 5.0f || 0.0f > f1) {
            party->position.y += f1 * f3;
        }
    } else {
        if (hit != NULL) {
            f2 = mario->position.y;
            f1 = party->groundY;
            if (f2 > f1) {
                f2 = mario->position.y;
            } else {
                f2 = f1;
            }
            if (party->position.y < f1) {
                f2 = 18.5f + f1;
            }
        } else {
            f2 = party->position.y;
        }
        f0 = 5.0f;
        if (party->flags2 & 0x8000) {
            f0 = 1.0f;
        }
        f2 = f2 + f0;
        f2 = f2 - party->position.y;
        f1 = 0.05f;
        if (f2 > 5.0f || 0.0f > f2) {
            party->position.y += f2 * f1;
        }
    }
}

// Attribute of the floor `r` ahead of the partner along its facing, probed from `r` above it.
static inline s32 partyGetFrontAttr(PartyEntry* party, f32 r) {
    HitCheckQuery q;
    f32 s, c;
    HitEntry* hit;

    sincosf(&s, &c, party->dir);
    q.targetDistance = 100.0f;
    q.targetPos.x = party->position.x + s * r;
    q.targetPos.z = party->position.z + c * r;
    q.targetPos.y = party->position.y + r;
    q.targetDir = FV(vec3_802f3edc, ((Vec){0.0f, -1.0f, 0.0f}));
    hit = hitCheckVecFilter(&q, NULL);
    return (hit != NULL) ? hitGetAttr(hit) : 0;
}

#pragma opt_propagation off // keep the 1000.0f load after the ternary branch, as retail does
// The floor probe's result in its own `fy` (declared last) gets retail's f24, and the ypos/y94 pair
// declared in that order and assigned after the declarations gives the f1/f3 colouring.
// Local probe radii keep the forward component first in the fused products.
// Assigning each jump-target radius at its use lets MWCC rematerialise the same load.
void party_motion_homing_walk(PartyEntry* party) {
    Vec vA0;
    s32 attr;
    f32 f31;
    f32 dist;
    f32 f;
    HitEntry* groundHit = NULL; // retail inits groundHit (r28), which partyHitIsBoxMobj reads even when the flags guard skips its assignment
    s32 id;
    MarioWork* mario;
    MarioWork* mm;
    HitEntry* hit;
    f32 speed;
    Vec v94;
    f32 angle;
    Vec v88;
    f32 s3;
    s32 i;
    f32 c3;
    HitCheckQuery q;
    f32 f30;
    Vec v7c;
    HitCheckQuery q2;
    f32 rx;
    f32 ry;
    f32 rz;
    f32 t0;
    f32 off;
    s32 noChg;
    Footmark* fmark;
    f32 f29;
    PartyEntry* target;
    f32 t1;
    Vec v70;
    Vec v64;
    f32 t2;
    f32 px;
    f32 f25;
    f32 f24;
    f32 ang;
    f32 tw;
    f32 headHeight;
    f32 c1;
    f32 py;
    Vec v58;
    f32 s1;
    s32 writeId;
    s32 readId;
    f32 c2;
    Vec v4c;
    f32 s2;
    f32 pz;
    f32 fy;

    id = partyGetFootmarkId(&party->flags);
    mario = party->playerPtr;
    if (party->trigFlags & 0x2) {
        party->trigFlags &= ~0x2;
        party->flags &= ~PARTY_FLAG_IS_BEHIND_MARIO;
    }
    f31 = (party->forceMoveFrames != 0) ? 28.0f : 37.5f;
    if (strcmp(gp->currentMapName, "aji_02") == 0 ||
        strcmp(gp->currentMapName, "aji_05") == 0 ||
        strcmp(gp->currentMapName, "aji_12") == 0) {
        if (marioKeyOffChk() == 0) {
            if (party->isFlyer == 0) {
                f24 = 18.0f;
            } else if (party->currentMemberId == PARTY_MEMBER_FLURRIE) {
                f24 = (party->trigFlags & 0x200000) ? 8.0f : 30.0f;
            } else if (party->currentMemberId == PARTY_MEMBER_VIVIAN) {
                f24 = (party->trigFlags & 0x200000) ? 8.0f : 30.0f;
            }
            f31 = f24;
            party->trigFlags |= 0x20000;
        }
    }
    if (partyGetTargetDistY(id)) {
        f30 = 50.0f;
    } else {
        f30 = 35.0f;
    }
    if (mario->characterId == 2) {
        f30 = 70.0f;
    }
    if ((party->trigFlags >> 17) & 1) {
        if (party->isFlyer == 0) {
            f25 = 18.0f;
        } else if (party->currentMemberId == PARTY_MEMBER_FLURRIE) {
            f25 = (party->trigFlags & 0x200000) ? 8.0f : 30.0f;
        } else if (party->currentMemberId == PARTY_MEMBER_VIVIAN) {
            f25 = (party->trigFlags & 0x200000) ? 8.0f : 30.0f;
        }
        f30 = f25;
    }
    if (party_slit(party) == 0) {
        return;
    }
    if (party_roll(party) == 0) {
        return;
    }
    if (mario->currMotionId == 0xE && !(party->flags & PARTY_FLAG_IS_BEHIND_MARIO)) {
        partyMoveBehindMario(party, 0);
        return;
    }
    {
        MarioWork* m = party->playerPtr;
        if (m->airTimer != 0 && m->baseSpeed == 0.0f) {
            noChg = 0;
        } else if (m->currMotionId == 0x1F || m->currMotionId == 0x20 ||
                   m->currMotionId == 0x10 || m->currMotionId == 0x11) {
            noChg = 0;
        } else {
            noChg = 1;
        }
    }
    if (noChg == 0) {
        partyChgMot(party, 0);
        return;
    }
    f29 = partyGetTargetMovespd2(party);
    if (!(party->flags & PARTY_FLAG_IS_PARTNER)) {
        dist = PSVECDistance(&mario->position, &party->position);
        if (dist < 35.0f) {
            f29 = 1.0f;
        } else if (dist >= 50.0f) {
            f29 = 3.0f;
        }
        if (party->playerPtr->flags & 0x8000) {
            f29 = 1.0f;
            f31 = 100.0f;
        }
        partySetTargetSpeed(party, f29);
    } else {
        target = partyGetPtr(marioGetPartyId());
        if (target != NULL) {
            vA0 = target->position;
            if (nokonokoGetStatus(target) == 3 || PSVECDistance(&mario->position, &target->position) >= 150.0f) {
                dist = PSVECDistance(&mario->position, &party->position);
                if (dist < 35.0f) {
                    f29 = 1.0f;
                } else if (dist >= 50.0f) {
                    f29 = 3.0f;
                }
                if (party->playerPtr->flags & 0x8000) {
                    f29 = 1.0f;
                }
                partySetTargetSpeed(party, f29);
            } else {
                dist = PSVECDistance(&target->position, &party->position);
                if (dist < 35.0f) {
                    f29 = 1.0f;
                } else if (dist >= 50.0f) {
                    f29 = 3.0f;
                }
                if (party->playerPtr->flags & 0x8000) {
                    f29 = 1.0f;
                }
                partySetTargetSpeed(party, f29);
            }
        }
    }
    px = party->position.x;
    i = 0;
    py = party->position.y;
    pz = party->position.z;
    f24 = 40.0f;
    f25 = 20.0f;
    for (;;) {
        fmark = partyGetFootmarkPtr(id);
        v94 = fmark->pos;
        angle = angleABf(px, pz, v94.x, v94.z);
        sincosf(&s1, &c1, angle);
        q.targetDistance = f24;
        q.targetPos.x = party->position.x + f25 * s1;
        q.targetPos.z = party->position.z + f25 * c1;
        q.targetPos.y = f25 + party->position.y;
        q.targetDir = FV(vec3_802f3ed0, ((Vec){0.0f, -1.0f, 0.0f}));
        hit = hitCheckVecFilter(&q, NULL);
        attr = (hit != NULL) ? hitGetAttr(hit) : 0;
        if (attr & 0x800) {
            partyNextReadId(id);
            partyGetReadId2(id, &v94);
            i++;
            if (i <= 10) {
                continue;
            }
            v94 = marioGetPtr()->position;
        }
        v70 = party->position;
        f25 = partyGetTargetDist3D(id, &v70);
        if (f25 >= f31) {
            partySetTargetSpeed(party, f29);
        }
        if (f30 < 20.0f) {
            partySetTargetSpeed(party, f29);
        }
        for (;;) {
            f25 = distABf(px, pz, v94.x, v94.z);
            angle = angleABf(px, pz, v94.x, v94.z);
            if (mario->flags & 0x8000) {
                f = party->radius + mario->wPlayerCollisionBox.x;
            } else {
                f = 0.5f * party->radius + party->speed;
            }
            if (f25 > f) {
                v58 = FV(vec3_802f3e40, ((Vec){0.0f, 0.0f, 0.0f}));
                v58.x = px;
                v58.y = py;
                v58.z = pz;
                v64 = v58;
                f25 = partyGetTargetDist3D(id, &v64);
                if (f25 >= f31) {
                    if (party->trigFlags & 0x20000) {
                        f = 0.1f * (f25 - f30);
                        if (f > 4.0f) {
                            f = 4.0f;
                        }
                        if (f < 0.5f) {
                            f = 0.0f;
                        }
                        partySetSpeed(party, mario->baseSpeed + f);
                    }
                    break;
                }
                if (marioGetSlitFloorPos(&vA0)) {
                    partySetTargetSpeed(party, 1.5f);
                    partyNextReadId(id);
                    if (marioGetSlitFloorPos(&vA0)) {
                        v94 = vA0;
                        py = distABf(px, pz, v94.x, v94.z);
                        angle = angleABf(px, pz, v94.x, v94.z);
                        if (py <= 5.0f) {
                            partyChgMotSlitFloor(party);
                        }
                    }
                    break;
                }
                if (f25 > f30) {
                    if (party->trigFlags & 0x20000) {
                        f = 0.1f * (f25 - f30);
                        if (f > 4.0f) {
                            f = 4.0f;
                        }
                        if (f < 0.5f) {
                            f = 0.0f;
                        }
                        partySetSpeed(party, mario->baseSpeed + f);
                        break;
                    }
                    f = f25 - f30;
                    if (f >= 1.0f) {
                        f = 1.0f;
                        if (f30 < 20.0f) {
                            f = f29;
                        }
                    }
                    partySetTargetSpeed(party, f);
                    break;
                }
                if (party->velX == 0.0f && party->velZ == 0.0f) {
                    partySetTargetSpeed(party, party->playerPtr->baseSpeed);
                } else {
                    partySetSpeed(party, 0.0f);
                    party->dir = angleABf(party->position.x, party->position.z, mario->position.x, mario->position.z);
                    partyChgPoseId(party, 1);
                }
            }
            writeId = partyGetFootmarkWriteId(id);
            readId = partyGetFootmarkReadId(id);
            if (readId == writeId) {
                partySetTargetSpeed(party, party->playerPtr->baseSpeed);
                if (!(party->playerPtr->baseSpeed != 0.0f && party->velX == 0.0f && party->velZ == 0.0f)) {
                    angle = party->dir;
                    partySetTargetSpeed(party, 0.0f);
                    party->jumpPara3 = 0.0f;
                    if (!(party->flags & 0x200)) {
                        partyChgPoseId(party, 1);
                    }
                    partyChgMot(party, 0);
                }
                break;
            }
            partyNextReadId(id);
            partyGetReadId2(id, &v94);
            if (!(party->flags & PARTY_FLAG_DISABLE_FLOORS)) {
                party->flags &= ~PARTY_FLAG_IN_SHALLOW_WATER;
                groundHit = partySearchGround(party, 10.0f, 0.0f);
                if (groundHit != NULL) {
                    if (hitGetAttr(groundHit) & 0x200) {
                        groundHit = NULL; // retail clears it before the splashTimer test, not only when it takes the flags
                        if (party->splashTimer == 0) {
                            party->trigFlags |= 0x01000000;
                            party->flags |= PARTY_FLAG_FORCE_FALL;
                        }
                    }
                }
                party->groundHit = groundHit;
                if (groundHit == NULL) {
                    partyChgMot(party, 3);
                    party->jumpPara3 = -1.0f;
                } else {
                    attr = hitGetAttr(groundHit);
                    if (attr & 0x100) {
                        party->flags |= PARTY_FLAG_IN_SHALLOW_WATER;
                    } else {
                        party->flags &= ~PARTY_FLAG_IN_SHALLOW_WATER;
                    }
                    if (attr & 0x800) {
                        partyChgMot(party, 0);
                        break;
                    }
                }
            }
            if ((party->flags & PARTY_FLAG_IS_GROUNDED) && (fmark->flags & 0x1)) {
                ang = toMovedir(party->dispReqDir);
                partyGetReadId(id, &rx, &ry, &rz);
                L_getTargetPos(id, &v88);
                if (v88.y < ry) {
                    ry = v88.y;
                }
                party->targetPos.x = rx;
                party->targetPos.z = rz;
                party->targetPos.y = ry;
                v4c = party->targetPos;
                tw = touch_wall_ct;
                sincosf(&s2, &c2, ang);
                {
#ifdef MUST_MATCH
                    f32 radius;
#endif
                    q2.targetPos = v4c;
#ifdef MUST_MATCH
                    // The assignment inside the product keeps 40.0f in one register for both uses.
                    q2.targetPos.x += s2 * (radius = 40.0f);
                    q2.targetPos.z += c2 * radius;
#else
                    q2.targetPos.x += s2 * 40.0f;
                    q2.targetPos.z += c2 * 40.0f;
#endif
                    q2.targetPos.y += tw;
                    q2.targetDistance = 1000.0f;
                    q2.targetDir = FV(vec3_802f3ec4, ((Vec){0.0f, -1.0f, 0.0f}));
                }
                hit = hitCheckVecFilter(&q2, NULL);
                if (hit != NULL) {
                    f = q2.hitPos.y;
                    off = (f >= 0.0f) ? 0.5f : -0.5f;
                    fy = (f32)(s32)(1000.0f * f + off) / 1000.0f;
                    hitGetAttr(hit);
                } else {
                    fy = -2000.0f;
                }
                if (fy > party->position.y) {
#ifdef MUST_MATCH
                    f32 radius;
#endif
                    angle = partyToMovedir(party, party->dispReqDir);
                    sincosf(&s3, &c3, angle);
#ifdef MUST_MATCH
                    party->targetPos.x = s3 * (radius = 40.0f);
                    party->targetPos.z = c3 * radius;
#else
                    party->targetPos.x = s3 * 40.0f;
                    party->targetPos.z = c3 * 40.0f;
#endif
                    party->targetPos.y = fy;
                }
                partyChgMot(party, 2);
                break;
            }
            if (partyHitIsBoxMobj(groundHit)) {
                partyGetAppearPos5(party, groundHit, &party->targetPos);
                party->trigFlags |= 0x00400000;
                partyChgMot(party, 2);
                break;
            }
            if (!(party->flags & PARTY_FLAG_IS_GROUNDED) || !(fmark->flags & 0x4)) {
                continue;
            }
            dist = distABf(px, pz, v94.x, v94.z);
            if (dist <= 5.0f) {
                partyChgMotSlitFloor(party);
            }
            break;
        }
        break;
    }
    if (party->speed) {
        partyChgSpeedPose(party, party->speed);
    } else if (!(fmark->flags & 0x4)) {
        partyChgMot(party, 0);
    }
    party->dir = angle;
    vA0 = party->position;
    partyGetMoveDirSpd(party, &angle, &speed);
    groundHit = partySearchFrontWall(party, &vA0, speed, angle);
    if (party->flags & PARTY_FLAG_DISABLE_FRONT_WALL_CHK) {
        groundHit = NULL;
    }
    if (groundHit == NULL) {
        party->wallHitCount = 0;
        if (!(party->trigFlags & 0x200000)) {
            getFrontFloor(party, &v94, party->dir);
        } else {
            party->moveAngle = revise360(270.0f + mario->targetYaw);
            partyGetAppearPos4(party, &v94);
        }
        {
            f32 ypos;
            f32 y94;
            f32 d;
            y94 = v94.y;
            ypos = party->position.y;
            if (y94 > ypos) {
                d = y94 - ypos;
                if (d > 12.0 && d < 60.0f) {
                    party->targetPos = v94;
                    partyChgMot(party, 2);
                    return;
                }
                if (y94 > ypos) {
                    party->position.y = y94;
                }
            }
        }
        if (party->speed) {
            attr = partyGetFrontAttr(party, 40.0f);
            if (!(attr & 0x200)) {
                vA0.x = party->position.x;
                vA0.z = party->position.z;
                movePos(&vA0.x, &vA0.z, party->speed, party->dir);
            }
        }
    } else {
        party->frontWallHit = groundHit;
        if (party->speed) {
            party->pushWallHit = groundHit;
            attr = hitGetAttr(groundHit);
            if (attr & 0x2000) {
                if (party->paperMot != 1) {
                    if (party->playerPtr->trigFlags & 0x80000000) {
                    } else {
                        party->trigFlags |= 0x1;
                        party->paperMot = 1;
                    }
                } else if (party->speed) {
                    vA0.x = party->position.x;
                    vA0.z = party->position.z;
                    movePos(&vA0.x, &vA0.z, party->speed, party->dir);
                }
            } else {
                party->wallHitCount++;
                if (party->wallHitCount > 8) {
                    party->wallHitCount = 0;
                    f = partyGetTargetDistYFrom(id, party->position.y);
                    if (f >= 1.0f) {
                        getFrontFloor(party, &v94, party->dir);
                        if (v94.y - party->position.y > 5.0f) {
                            party->targetPos = v94;
                            partyChgMot(party, 2);
                            return;
                        }
                        if (!(party->flags & 0x30)) {
                            partyChkFrontStep(party, 11.0f, &t0, &t1, &t2);
                            if (t0 - party->position.y >= 5.0f && (party->groundSlope) <= 30.0f) {
                                party->stepUpY = t0;
                                if (party->speed) {
                                    partyChgMot(party, 6);
                                    return;
                                }
                            }
                        }
                    } else {
                        angle = partyGetTargetDir(id, party->position.x, party->position.z);
                        getFrontFloor(party, &v94, angle);
                        if (v94.y - party->position.y > 100.0f) {
                            party->position = v94;
                            partyChgMot(party, 1);
                        } else if (v94.y - party->position.y >= 0.0f) {
                            party->targetPos = v94;
                            partyChgMot(party, 2);
                        } else {
                            partyChgMot(party, 0);
                        }
                    }
                }
            }
        }
    }
    party->position.x = vA0.x;
    party->position.z = vA0.z;
    headHeight = party->height;
    v7c = party->position;
    hit = partySearchHead(party, &v7c, &headHeight, party->dispDir);
    if (hit != NULL) {
        party->headHit = hit;
    }
    party->flags &= ~PARTY_FLAG_IN_SHALLOW_WATER;
    hit = partySearchGround(party, 10.0f, 10.0f);
    vA0.y = party->groundY;
    if (hit != NULL) {
        attr = hitGetAttr(hit);
        if (attr & 0x200) {
            hit = NULL;
            if (party->splashTimer == 0) {
                party->trigFlags |= 0x01000000;
                party->flags |= PARTY_FLAG_FORCE_FALL;
            }
        } else if (attr & 0x800) {
            partyChgRunMode(party, 0x0D);
            return;
        }
    }
    party->groundHit = hit;
    if (hit == NULL) {
        partyChgMot(party, 3);
        party->jumpPara3 = -1.0f;
    } else {
        attr = hitGetAttr(hit);
        if (attr & 0x100) {
            party->flags |= PARTY_FLAG_IN_SHALLOW_WATER;
        } else {
            party->flags &= ~PARTY_FLAG_IN_SHALLOW_WATER;
        }
        attr = hitGetAttr(hit);
        if (attr & 0x40000000) {
            party->field_0x14C = hit;
        }
    }
    party->position.y = vA0.y;
    party_force_reset_outofscreen(party);
    mm = party->playerPtr;
    if (strcmp(gp->currentMapName, FB(str_802F3F18, "gor_01")) == 0) {
        if (mm->hitobjStandOn != NULL && party->groundHit != NULL) {
            if (strcmp(hitGetName(mm->hitobjStandOn), FB(str_A_ie_03_yuka_f1_C, "A_ie_03_yuka_f1_C")) == 0 &&
                strcmp(hitGetName(party->groundHit), FB(str_A_ie_03_yuka_f2_C, "A_ie_03_yuka_f2_C")) == 0) {
                party->position = FV(vec3_802f3f00, ((Vec){122.0f, 130.0f, -313.0f}));
            }
        }
    }
}
#pragma opt_propagation reset

// opt_dead_assignments: keeps MWCC from dropping the `mario`/`id` copies that keep the target's GPR colouring.
#pragma opt_dead_assignments off
void party_motion_homing_fly(PartyEntry* party) {
    MarioWork* mario;
    s32 id;
    s32 flag;
    HitEntry* hit;
    f32 f31, f30, f29, f28, f27, f26, f25;
    f32 dist;
    f32 angle;
    f32 speed;
    f32 foot_x, foot_y, foot_z;
    f32 move_x, move_z;
    f32 hitNZ, hitNY, hitNX, hitDist, hitZ, hitY, hitX;
    Vec p;
    Vec q;
    Vec appear;
    Vec floor;
    Vec pos;
    Vec tv2;
    Vec tv;

    mario = party->playerPtr;
    id = partyGetFootmarkId(&party->flags);
    if (party->trigFlags & 0x2) {
        party->trigFlags = (party->trigFlags & ~0x2);
        party->flags = (party->flags & ~PARTY_FLAG_IS_BEHIND_MARIO);
        party->noFloorCount = 0;
    }
    if (party->forceMoveFrames != 0) {
        f31 = 28.0f;
    } else {
        f31 = 32.0f;
    }
    if (strcmp(gp->currentMapName, "aji_02") == 0 ||
        strcmp(gp->currentMapName, "aji_05") == 0 ||
        strcmp(gp->currentMapName, "aji_12") == 0) {
        if (!marioKeyOffChk()) {
            if (party->isFlyer == 0) {
                f25 = 18.0f;
            } else if (party->currentMemberId == PARTY_MEMBER_FLURRIE) {
                if (party->trigFlags & 0x200000) {
                    f25 = 8.0f;
                } else {
                    f25 = 30.0f;
                }
            } else if (party->currentMemberId == PARTY_MEMBER_VIVIAN) {
                if (party->trigFlags & 0x200000) {
                    f25 = 8.0f;
                } else {
                    f25 = 30.0f;
                }
            }
            f31 = f25;
            party->trigFlags = (party->trigFlags | 0x20000);
        }
    }
    if (partyGetTargetDistY(id)) {
        f30 = 50.0f;
    } else {
        f30 = 35.0f;
    }
    if ((party->trigFlags >> 17) & 1) {
        if (party->isFlyer == 0) {
            f26 = 18.0f;
        } else if (party->currentMemberId == PARTY_MEMBER_FLURRIE) {
            if (party->trigFlags & 0x200000) {
                f26 = 8.0f;
            } else {
                f26 = 30.0f;
            }
        } else if (party->currentMemberId == PARTY_MEMBER_VIVIAN) {
            if (party->trigFlags & 0x200000) {
                f26 = 8.0f;
            } else {
                f26 = 30.0f;
            }
        }
        f30 = f26;
    }
    if (!party_slit(party)) {
        return;
    }
    if (!party_roll(party)) {
        return;
    }
    if (mario->currMotionId == 0xe && !(party->flags & PARTY_FLAG_IS_BEHIND_MARIO)) {
        partyMoveBehindMario(party, 0);
        return;
    }
    {
        s32 skip;
        MarioWork* m2 = party->playerPtr;
        if (m2->airTimer != 0 && m2->baseSpeed == 0.0f) {
            skip = 0;
        } else if (m2->currMotionId == 0x1f ||
                   m2->currMotionId == 0x20 ||
                   m2->currMotionId == 0x10 ||
                   m2->currMotionId == 0x11) {
            skip = 0;
        } else {
            skip = 1;
        }
        if (skip == 0) {
            partyChgMot(party, 0);
            return;
        }
    }
    f29 = partyGetTargetMovespd2(party);
    dist = PSVECDistance(&mario->position, &party->position);
    if (dist < 35.0f) {
        f29 = 1.0f;
    } else if (dist >= 50.0f) {
        f29 = 3.0f;
    }
    if (party->playerPtr->flags & 0x8000) {
        f29 = 1.0f;
        f31 = 100.0f;
    }
    partySetTargetSpeed(party, f29);
    f28 = party->position.x;
    f27 = party->position.y;
    f26 = party->position.z;
    {
        Footmark* fp = partyGetFootmarkPtr(id);
        foot_x = fp->pos.x;
        {
            f32 yadd = 5.0f;
            if (party->flags2 & 0x8000) {
                yadd = 1.0f;
            }
            foot_y = fp->pos.y + yadd;
        }
        foot_z = fp->pos.z;
    }
    pos = party->position;
    if (partyGetTargetDist3D(id, &pos) >= f31) {
        partySetTargetSpeed(party, f29);
    }
    if (f30 < 20.0f) {
        partySetTargetSpeed(party, f29);
    }
    flag = 0;

    for (;;) {
        angle = angleABf(f28, f26, foot_x, foot_z);
        dist = distABf(f28, f26, foot_x, foot_z);
        if (dist > (mario->flags & 0x8000
                       ? party->radius + mario->wPlayerCollisionBox.x
                       : 0.5f * party->radius + party->speed)) {
            tv = FV(vec3_802f3e4c, ((Vec){0.0f, 0.0f, 0.0f}));
            tv.x = f28;
            tv.y = f27;
            tv.z = f26;
            tv2 = tv;
            f25 = partyGetTargetDist3D(id, &tv2);
            if (f25 >= f31) {
                if (party->trigFlags & 0x20000) {
                    f32 v = 0.1f * (f25 - f30);
                    if (v > 4.0f) {
                        v = 4.0f;
                    }
                    if (v < 0.5f) {
                        v = 0.0f;
                    }
                    partySetSpeed(party, mario->baseSpeed + v);
                }
                flag = 1;
                break;
            }
            if (marioGetSlitFloorPos(&p)) {
                partySetTargetSpeed(party, 1.5f);
                partyNextReadId(id);
                if (marioGetSlitFloorPos(&p)) {
                    q = p;
                    f25 = q.z;
                    f27 = distABf(f28, f26, q.x, f25);
                    angle = angleABf(f28, f26, q.x, f25);
                    if (f27 <= 5.0f) {
                        partyChgMotSlitFloor(party);
                    }
                }
                break;
            }
            if (f25 > f30) {
                if (party->trigFlags & 0x20000) {
                    f32 v = 0.1f * (f25 - f30);
                    if (v > 4.0f) {
                        v = 4.0f;
                    }
                    if (v < 0.5f) {
                        v = 0.0f;
                    }
                    partySetSpeed(party, mario->baseSpeed + v);
                } else {
                    f32 v = f25 - f30;
                    if (v >= 1.0f) {
                        v = 1.0f;
                        if (f30 < 20.0f) {
                            v = f29;
                        }
                    }
                    partySetTargetSpeed(party, v);
                    break;
                }
            } else if (party->velX == 0.0f && party->velZ == 0.0f) {
                partySetTargetSpeed(party, party->playerPtr->baseSpeed);
            } else {
                partySetSpeed(party, 0.0f);
                party->dir = angleABf(party->position.x, party->position.z, mario->position.x, mario->position.z);
                partyChgPoseId(party, 1);
            }
        }
        if (partyGetFootmarkReadId(id) == partyGetFootmarkWriteId(id)) {
            partySetTargetSpeed(party, party->playerPtr->baseSpeed);
            if (party->playerPtr->baseSpeed == 0.0f ||
                party->velX != 0.0f ||
                party->velZ != 0.0f) {
                angle = party->dir;
                partySetTargetSpeed(party, 0.0f);
                party->jumpPara3 = 0.0f;
                if (!(party->flags & 0x200)) {
                    partyChgPoseId(party, 1);
                }
                partyChgMot(party, 0);
            }
            break;
        }
        partyNextReadId(id);
        partyGetReadId(id, &foot_x, &foot_y, &foot_z);
        if (party->flags & PARTY_FLAG_DISABLE_FLOORS) {
            continue;
        }
        party->flags = (party->flags & ~PARTY_FLAG_IN_SHALLOW_WATER);
        hit = partySearchGround(party, 100.0f, 100.0f);
        if (hit != NULL) {
            if (hitGetAttr(hit) & 0x200) {
                hit = NULL;
            }
        }
        party->groundHit = hit;
        if (hit == NULL) {
            partyChgMot(party, 3);
            party->jumpPara3 = -1.0f;
            continue;
        }
        {
            s32 a = hitGetAttr(hit);
            if (a & 0x100) {
                party->flags = (party->flags | PARTY_FLAG_IN_SHALLOW_WATER);
            } else {
                party->flags = (party->flags & ~PARTY_FLAG_IN_SHALLOW_WATER);
            }
            if (a & 0x800) {
                partyChgMot(party, 0);
                break;
            }
        }
    }

    if (party->speed) {
        partyChgSpeedPose(party, party->speed);
    } else {
        partyChgMot(party, 0);
    }
    if ((s8)party->noFloorCount > 0xa) {
        angle = angleABf(party->position.x, party->position.z, mario->position.x, mario->position.z);
    }
    if (flag != 0) {
        MarioWork* mario2 = party->playerPtr;
        partyGetAppearPos3(party, &appear);
        angle = angleABf(party->position.x, party->position.z, appear.x, appear.z);
        hit = partySearchGround(party, 10.0f, 0.0f);
        appear.y = party->groundY;
        if (hit != NULL) {
            if (hitGetAttr(hit) & 0x200) {
                hit = NULL;
            }
        }
        if (hit != NULL) {
            party->position.y = appear.y;
        } else {
            f32 v = 5.0f;
            if (party->flags2 & 0x8000) {
                v = 1.0f;
            }
            party->position.y += 0.05f * ((mario2->position.y + v) - party->position.y);
        }
    }
    party->dir = angle;
    p = party->position;
    partyGetMoveDirSpd(party, &angle, &speed);
    hit = partySearchFrontWall(party, &p, speed, angle);
    if (party->flags & PARTY_FLAG_DISABLE_FRONT_WALL_CHK) {
        hit = NULL;
    }
    if (hit == NULL) {
        if (!(party->trigFlags & 0x200000)) {
            getFrontFloor(party, &floor, party->dir);
        } else {
            party->moveAngle = revise360(270.0f + mario->targetYaw);
            partyGetAppearPos4(party, &floor);
        }
        {
        f32 fy = floor.y;
        if (fy >= mario->position.y) {
            f32 v = 5.0f;
            f32 y;
            if (party->flags2 & 0x8000) {
                v = 1.0f;
            }
            y = fy + v;
            party->position.y += 0.05f * (y - party->position.y);
        }
        }
        party->noFloorCount = 0;
        if (party->speed) {
            p.x = party->position.x;
            p.z = party->position.z;
            movePos(&p.x, &p.z, party->speed, party->dir);
        }
    } else {
        party->frontWallHit = hit;
        if (party->speed) {
            party->pushWallHit = hit;
            {
            s32 b = hitGetAttr(hit);
            if (b & 0x2000) {
                if (party->paperMot != 1) {
                    if (!(party->playerPtr->trigFlags & 0x80000000)) {
                        party->trigFlags = (party->trigFlags | 0x1);
                        party->paperMot = 1;
                    }
                } else {
                    if (party->speed) {
                        p.x = party->position.x;
                        p.z = party->position.z;
                        movePos(&p.x, &p.z, party->speed, party->dir);
                    }
                }
            } else {
                f32 v;
                move_x = party->position.x;
                f25 = party->position.y;
                move_z = party->position.z;
                movePos(&move_x, &move_z, party->radius, angle);
                v = f25 + touch_wall_ct;
                hitDist = 1000.0f;
                hit = hitCheckFilter(NULL, move_x, v, move_z, 0.0f, -1.0f, 0.0f, &hitX,
                                     &hitY, &hitZ, &hitDist, &hitNX, &hitNY, &hitNZ);
                if (hit == NULL) {
                    f25 = -2000.0f;
                } else {
                    f25 = (f32)((s32)(hitY * 100.0f + 0.5f)) / 100.0f;
                    hitGetAttr(hit);
                }
                if (f25 > -2000.0f) {
                    party->position.y += 0.05f * ((f25 + party->height) - party->position.y);
                } else {
                    party->noFloorCount = party->noFloorCount + 1;
                    if (partyGetTargetDistYFrom(id, party->position.y) >= 1.0f) {
                        if (fabsf(mario->position.y - party->position.y) >= 10.0f) {
                            party->position.y = 0.1f * ((10.0f + mario->position.y) - party->position.y) + party->position.y;
                            party->position.x = p.x;
                            party->position.z = p.z;
                            return;
                        }
                    }
                }
            }
            }
        }
    }
    party->position.x = p.x;
    party->position.z = p.z;
    party->flags = (party->flags & ~PARTY_FLAG_IN_SHALLOW_WATER);
    hit = partySearchGround(party, 10.0f, 10.0f);
    if (hit != NULL) {
        s32 a = hitGetAttr(hit);
        if (a & 0x800) {
            partyChgRunMode(party, 0xd);
            return;
        }
        if (a & 0x100) {
            party->flags = (party->flags | PARTY_FLAG_IN_SHALLOW_WATER);
        } else {
            party->flags = (party->flags & ~PARTY_FLAG_IN_SHALLOW_WATER);
        }
    }
    party_force_reset_outofscreen(party);
}
#pragma opt_dead_assignments reset

void party_set_behind_dir(PartyEntry* party, f32 dir) {
    CameraEntry* camera = camGetPtr(CAMERA_3D);
    party->moveAngle = revise360(dir + camera->viewYaw);
    party->trigFlags = (party->trigFlags | 0x100000);
}

// Retail stages the y probe argument in a named temp with constant propagation off, so
// mwcc schedules the hd = 1000.0f load after the y add (into f0, not f3); the second wall
// probe uses a fresh local `w` so its height lands in f28 rather than f29. Both shapes
// are the shapes that match.
#pragma opt_propagation off // keep constant-load scheduling so hd's 1000.0f load stays after the y add
void party_motion_behind_mario(PartyEntry* party) {
    MarioWork* mario;
    MarioWork* mario2;
    HitEntry* hit;
    PartyEntry* other;
    f32 angle;
    f32 x, z;
    f32 f31, f30, f29, f28;
    f32 fWall;
    Vec appear;
    f32 hnz, hny, hnx, hd, hz, hy, hx;
    f32 ytmp;

    mario = party->playerPtr;
    if (party->trigFlags & 0x2) {
        party->trigFlags = (party->trigFlags & ~0x2);
        party->trigFlags = (party->trigFlags | 0x40000);
        if (!(party->trigFlags & 0x100000)) {
            if (!(party->trigFlags & 0x10000)) {
                mario2 = party->playerPtr;
                if (!(party->flags & PARTY_FLAG_IS_PARTNER)) {
                    angle = toMovedir(mario2->dispDir);
                } else {
                    other = partyGetPtr(marioGetPartyId());
                    if (other != NULL) {
                        angle = partyToMovedir(party, other->dispDir);
                    } else {
                        angle = toMovedir(mario2->dispDir);
                    }
                }
                angle += 180.0f;
                angle = revise360(angle);
            } else {
                party->trigFlags = (party->trigFlags & ~0x10000);
                angle = revise360(180.0f + angleABf(mario->position.x, mario->position.z, party->targetPos.x, party->targetPos.z));
            }
            party->moveAngle = angle;
        }
        partyGetAppearPos4(party, &appear);
        party->timer = (s32)distABf(party->position.x, party->position.z, appear.x, appear.z);
        if (party->timer <= 0) {
            party->timer = 1;
        }
    }
    if (!(party->trigFlags & 0x100000)) {
        if (marioKeyOffChk()) {
            mario2 = party->playerPtr;
            if (!(party->flags & PARTY_FLAG_IS_PARTNER)) {
                angle = toMovedir(mario2->dispDir);
            } else {
                other = partyGetPtr(marioGetPartyId());
                if (other != NULL) {
                    angle = partyToMovedir(party, other->dispDir);
                } else {
                    angle = toMovedir(mario2->dispDir);
                }
            }
            angle += 180.0f;
            party->moveAngle = revise360(angle);
        }
    }
    partyGetAppearPos4(party, &appear);
    f30 = appear.y;
    x = appear.x;
    z = appear.z;
    f31 = angleABf(party->position.x, party->position.z, appear.x, appear.z);
    ytmp = f30 + touch_wall_ct;
    hd = 1000.0f;
    hit = hitCheckFilter(NULL, x, ytmp, z, 0.0f, -1.0f, 0.0f, &hx, &hy, &hz, &hd, &hnx, &hny, &hnz);
    if (hit == NULL) {
        f29 = -2000.0f;
    } else {
        f29 = (f32)(s32)(100.0f * hy + 0.5f) / 100.0f;
        hitGetAttr(hit);
    }
    if (f29 <= -2000.0f) {
        f29 = 1.5f * mario->wPlayerCollisionBox.x;
        movePos(&x, &z, f29, toMovedir(mario->dispDir));
    }
    f28 = 2.0f;
    if (mario->baseSpeed > 1.0f) {
        f28 = 10.0f;
    }
    f29 = distABf(party->position.x, party->position.z, x, z);
    party->timer = party->timer - 1;
    if (f29 >= f28 && party->timer > 0) {
        party->dir = angleABf(party->position.x, party->position.z, x, z);
        distABf(party->position.x, party->position.z, x, z);
        partySetSpeed(party, (party->trigFlags & 0x100000) ? 3.6f : 2.0f);
        partyMoveCheckWall(party);
        partyChgSpeedPose(party, party->speed);
        if (party->frontWallHit != NULL) {
            f32 bhnz, bhny, bhnx, bhd, bhz, bhy, bhx;
            x = party->position.x;
            z = party->position.z;
            movePos(&x, &z, f29, f31);
            angleABf(party->position.x, party->position.z, x, z);
            ytmp = f30 + touch_wall_ct;
            bhd = 1000.0f;
            hit = hitCheckFilter(NULL, x, ytmp, z, 0.0f, -1.0f, 0.0f, &bhx, &bhy, &bhz, &bhd, &bhnx, &bhny, &bhnz);
            if (hit == NULL) {
                fWall = -2000.0f;
            } else {
                fWall = (f32)(s32)(100.0f * bhy + 0.5f) / 100.0f;
                hitGetAttr(hit);
            }
            if (fabsf(party->position.y - fWall) > 1.0f) {
                party->targetPos.x = x;
                party->targetPos.y = fWall;
                party->targetPos.z = z;
                partyChgMot(party, 2);
                return;
            }
        } else if (f30 > party->position.y) {
            f32 chnz, chny, chnx, chd, chz, chy, chx, w;
            x = party->position.x;
            z = party->position.z;
            movePos(&x, &z, f29, f31);
            angleABf(party->position.x, party->position.z, x, z);
            ytmp = f30 + touch_wall_ct;
            chd = 1000.0f;
            hit = hitCheckFilter(NULL, x, ytmp, z, 0.0f, -1.0f, 0.0f, &chx, &chy, &chz, &chd, &chnx, &chny, &chnz);
            if (hit == NULL) {
                w = -2000.0f;
            } else {
                w = (f32)(s32)(100.0f * chy + 0.5f) / 100.0f;
                hitGetAttr(hit);
            }
            if (fabsf(party->position.y - w) > 1.0f) {
                party->targetPos.x = x;
                party->targetPos.y = w;
                party->targetPos.z = z;
                partyChgMot(party, 2);
                return;
            }
        }
        hit = partySearchGround(party, 10.0f, 0.0f);
        if (hit != NULL) {
            if (hitGetAttr(hit) & 0x200) {
                hit = NULL;
                if (party->splashTimer == 0) {
                    party->trigFlags = (party->trigFlags | 0x1000000);
                    party->flags = (party->flags | PARTY_FLAG_FORCE_FALL);
                }
            }
        }
        party->groundHit = hit;
        if (hit == NULL) {
            partyChgMot(party, 3);
            party->jumpPara3 = -1.0f;
        }
        party->position.y = party->groundY;
    } else {
        party->timer = 0;
        if (party->trigFlags & 0x200000) {
            party->trigFlags = (party->trigFlags & ~0x20000);
        }
        partyChgMoveMode(party, 1);
        partyChgMot(party, 0);
        partySetDir(party, mario->position.x, mario->position.z);
        party->homingDist = 50.0f;
        party->flags2 = (party->flags2 & ~0x10);
        party->flags = (party->flags | PARTY_FLAG_IS_BEHIND_MARIO);
        party->trigFlags = (party->trigFlags & ~0x100000);
        party->trigFlags = (party->trigFlags & ~0x40000);
    }
    if (party->pushWallHit != NULL) {
        party->flags = (party->flags | PARTY_FLAG_IS_BEHIND_MARIO);
        partyChgMoveMode(party, 1);
        partyChgMot(party, 0);
        partySetDir(party, x, z);
        party->homingDist = 50.0f;
        party->flags2 = (party->flags2 & ~0x10);
        party->trigFlags = (party->trigFlags & ~0x40000);
    }
}

// `opt_propagation off` keeps the hd = 1000.0f load after the y add (into f0, not f3)
void party_motion_behind_mario_continue(PartyEntry* party) {
    MarioWork* mario;
    MarioWork* mario2;
    HitEntry* hit;
    HitEntry* hit2;
    PartyEntry* other;
    f32 x, z, y;
    f32 dir;
    f32 targetY;
    f32 ty;
    f32 dist;
    f32 limit;
    f32 hnz, hny, hnx, hd, hz, hy, hx;
    f32 ytmp;

    mario = party->playerPtr;
    if (!(party->flags & PARTY_FLAG_IS_PARTNER)) {
        dir = toMovedir(mario->dispDir);
    } else {
        other = partyGetPtr(marioGetPartyId());
        if (other != NULL) {
            dir = partyToMovedir(party, other->dispDir);
        } else {
            dir = toMovedir(mario->dispDir);
        }
    }
    dir += 180.0f;
    party->moveAngle = revise360(dir);
    if (party->trigFlags & 0x2) {
        party->trigFlags = (party->trigFlags & ~0x2);
        limit = 1.5f * mario->wPlayerCollisionBox.x;
        x = mario->position.x;
        z = mario->position.z;
        movePos(&x, &z, limit, party->moveAngle);
        party->timer = (s32)distABf(party->position.x, party->position.z, x, z);
        if (party->timer <= 0) {
            party->timer = 1;
        }
    }
    if (marioKeyOffChk()) {
        mario2 = party->playerPtr;
        if (!(party->flags & PARTY_FLAG_IS_PARTNER)) {
            dir = toMovedir(mario2->dispDir);
        } else {
            other = partyGetPtr(marioGetPartyId());
            if (other != NULL) {
                dir = partyToMovedir(party, other->dispDir);
            } else {
                dir = toMovedir(mario2->dispDir);
            }
        }
        dir += 180.0f;
        party->moveAngle = revise360(dir);
    }
    if (!(party->flags & PARTY_FLAG_IS_PARTNER)) {
        limit = 1.5f * mario->wPlayerCollisionBox.x;
        x = mario->position.x;
        z = mario->position.z;
        targetY = mario->position.y;
    } else {
        limit = 1.5f * mario->wPlayerCollisionBox.x;
        other = anotherPartyGetPtr(party->currentSlotId);
        if (other != NULL) {
            x = other->position.x;
            z = other->position.z;
            targetY = other->position.y;
        } else {
            x = mario->position.x;
            z = mario->position.z;
            targetY = mario->position.y;
        }
    }
    movePos(&x, &z, limit, party->moveAngle);
    angleABf(party->position.x, party->position.z, x, z);
    ytmp = targetY + touch_wall_ct;
    hd = 1000.0f;
    hit = hitCheckFilter(NULL, x, ytmp, z, 0.0f, -1.0f, 0.0f, &hx, &hy, &hz, &hd, &hnx, &hny, &hnz);
    if (hit == NULL) {
        ty = -2000.0f;
    } else {
        ty = (f32)(s32)(100.0f * hy + 0.5f) / 100.0f;
        hitGetAttr(hit);
    }
    if (ty <= -2000.0f) {
        limit = 1.5f * mario->wPlayerCollisionBox.x;
        movePos(&x, &z, limit, toMovedir(mario->dispDir));
    }
    dist = 2.0f;
    if (mario->baseSpeed > 1.0f) {
        dist = 10.0f;
    }
    limit = distABf(party->position.x, party->position.z, x, z);
    party->timer = party->timer - 1;
    if (limit >= dist) {
        party->dir = angleABf(party->position.x, party->position.z, x, z);
        limit = distABf(party->position.x, party->position.z, x, z) / 10.0f;
        if (limit > 2.0f) {
            limit = 2.0f;
        }
        if (limit < 1.0f) {
            limit = 1.0f;
        }
        partySetSpeed(party, limit);
        partyMoveCheckWall(party);
        partyChgSpeedPose(party, party->speed);
        hit2 = partySearchGround(party, 10.0f, 0.0f);
        if (hit2 != NULL) {
            if (hitGetAttr(hit2) & 0x200) {
                hit2 = NULL;
                if (party->splashTimer == 0) {
                    party->trigFlags = (party->trigFlags | 0x1000000);
                    party->flags = (party->flags | PARTY_FLAG_FORCE_FALL);
                }
            }
        }
        party->groundHit = hit2;
        if (hit2 == NULL) {
            partyChgMot(party, 3);
            party->jumpPara3 = -1.0f;
        }
        party->position.y = party->groundY;
    } else {
        party->timer = 0;
        if (mario->baseSpeed == 0.0f) {
            party->flags = (party->flags | PARTY_FLAG_IS_BEHIND_MARIO);
            partyChgMoveMode(party, 1);
            partyChgMot(party, 0);
            partySetDir(party, mario->position.x, mario->position.z);
            party->homingDist = 50.0f;
            party->flags2 = (party->flags2 & ~0x10);
        }
    }
    if (party->pushWallHit != NULL) {
        partyChgMoveMode(party, 1);
        partyChgMot(party, 0);
        partySetDir(party, x, z);
        party->homingDist = 50.0f;
        party->flags2 = (party->flags2 & ~0x10);
    }
}
#pragma opt_propagation reset

void party_motion_behind_mario_fly(PartyEntry* party) {
    MarioWork* mario;
    MarioWork* mario2;
    HitEntry* hit;
    PartyEntry* other;
    f32 speed;
    f32 dir;
    f32 angle;
    f32 limit;
    f32 f31;
    f32 f29;
    Vec pos;

    mario = party->playerPtr;
    if (party->trigFlags & 0x2) {
        party->trigFlags = (party->trigFlags & ~0x2);
        party->trigFlags = (party->trigFlags | 0x40000);
        if (!(party->trigFlags & 0x100000)) {
            if (!(party->trigFlags & 0x10000)) {
                mario2 = party->playerPtr;
                if (!(party->flags & PARTY_FLAG_IS_PARTNER)) {
                    dir = toMovedir(mario2->dispDir);
                } else {
                    other = partyGetPtr(marioGetPartyId());
                    if (other != NULL) {
                        dir = partyToMovedir(party, other->dispDir);
                    } else {
                        dir = toMovedir(mario2->dispDir);
                    }
                }
                dir += 180.0f;
                dir = revise360(dir);
            } else {
                party->trigFlags = (party->trigFlags & ~0x10000);
                dir = revise360(180.0f + angleABf(mario->position.x, mario->position.z, party->targetPos.x, party->targetPos.z));
            }
            party->moveAngle = dir;
        }
        partyGetAppearPos4(party, &party->targetPos);
        party->timer = (s32)distABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z);
        if (party->timer <= 0) {
            party->timer = 1;
        }
    }
    if (!(party->trigFlags & 0x100000)) {
        if (marioKeyOffChk()) {
            mario2 = party->playerPtr;
            if (!(party->flags & PARTY_FLAG_IS_PARTNER)) {
                dir = toMovedir(mario2->dispDir);
            } else {
                other = partyGetPtr(marioGetPartyId());
                if (other != NULL) {
                    dir = partyToMovedir(party, other->dispDir);
                } else {
                    dir = toMovedir(mario2->dispDir);
                }
            }
            dir += 180.0f;
            party->moveAngle = revise360(dir);
        }
    }
    angleABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z);
    if (mario->baseSpeed > 1.0f) {
        limit = 6.0f;
    } else {
        limit = 2.0f;
    }
    f31 = distABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z);
    party->timer = party->timer - 1;
    if ((f31 >= limit || fabsf(party->targetPos.y - party->position.y) > 2.0f) && party->timer > 0) {
        party->dir = angleABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z);
        distABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z);
        if (party->trigFlags & 0x100000) {
            f29 = 3.6f;
        } else {
            f29 = 2.0f;
        }
        if (f31 < limit) {
            f29 = 0.0f;
            partySetSpeed(party, f29);
        }
        partySetTargetSpeed(party, f29);
        pos = party->position;
        partyGetMoveDirSpd(party, &angle, &speed);
        hit = partySearchFrontWall(party, &pos, speed, angle);
        if (hit != NULL) {
            party->frontWallHit = hit;
            if (party->speed) {
                party->pushWallHit = hit;
            }
        }
        party->position.x = pos.x;
        party->position.z = pos.z;
        party->position.y += 0.05f * (party->targetPos.y - party->position.y);
        partyChgSpeedPose(party, party->speed);
    } else {
        party->timer = 0;
        if (party->trigFlags & 0x200000) {
            party->trigFlags = (party->trigFlags & ~0x20000);
            if (party->isFlyer == 1) {
                party->flags2 = (party->flags2 & ~0x10);
            }
        }
        partyChgMoveMode(party, 1);
        partyChgMot(party, 0);
        partySetDir(party, mario->position.x, mario->position.z);
        party->homingDist = 50.0f;
        party->flags2 = (party->flags2 & ~0x10);
        partySetSpeed(party, 0.0f);
        party->flags = (party->flags | PARTY_FLAG_IS_BEHIND_MARIO);
        party->trigFlags = (party->trigFlags & ~0x100000);
        party->trigFlags = (party->trigFlags & ~0x40000);
        partySetDispDirToMario(party);
        if (party->pushWallHit != NULL) {
            party->flags = (party->flags | PARTY_FLAG_IS_BEHIND_MARIO);
            partyChgMoveMode(party, 1);
            partyChgMot(party, 0);
            partySetDir(party, mario->position.x, mario->position.z);
            party->homingDist = 50.0f;
            party->flags2 = (party->flags2 & ~0x10);
            party->trigFlags = (party->trigFlags & ~0x40000);
        }
        hit = partySearchGround(party, 100.0f, 100.0f);
        if (hit != NULL) {
            if (hitGetAttr(hit) & 0x200) {
                hit = NULL;
                if (party->splashTimer == 0) {
                    party->trigFlags = (party->trigFlags | 0x1000000);
                    party->flags = (party->flags | PARTY_FLAG_FORCE_FALL);
                }
            }
            if (f31 > party->height) {
                hit = NULL;
            }
            party->position.y += 0.05f * (party->groundY - party->position.y);
        }
        party->groundHit = hit;
    }
}

#pragma opt_propagation off // keep constant-load scheduling so hd's 1000.0f load stays after the y add
void party_motion_beside_mario(PartyEntry* party) {
    MarioWork* mario;
    MarioWork* mario2;
    HitEntry* hit;
    PartyEntry* other;
    f32 x, z, y;
    f32 dir;
    f32 dist;
    f32 limit;
    f32 hnz, hny, hnx, hd, hz, hy, hx;
    f32 ytmp;

    mario = party->playerPtr;
    if (party->trigFlags & 0x2) {
        party->trigFlags = (party->trigFlags & ~0x2);
        mario2 = party->playerPtr;
        if (!(party->flags & PARTY_FLAG_IS_PARTNER)) {
            dir = toMovedir(mario2->dispDir);
        } else {
            other = partyGetPtr(marioGetPartyId());
            if (other != NULL) {
                dir = partyToMovedir(party, other->dispDir);
            } else {
                dir = toMovedir(mario2->dispDir);
            }
        }
        dir += 90.0f;
        party->moveAngle = revise360(dir);
        limit = 1.5f * mario->wPlayerCollisionBox.x;
        x = mario->position.x;
        z = mario->position.z;
        movePos(&x, &z, limit, party->moveAngle);
        party->timer = (s32)distABf(party->position.x, party->position.z, x, z);
        if (party->timer <= 0) {
            party->timer = 1;
        }
    }
    if (marioKeyOffChk()) {
        mario2 = party->playerPtr;
        if (!(party->flags & PARTY_FLAG_IS_PARTNER)) {
            dir = toMovedir(mario2->dispDir);
        } else {
            other = partyGetPtr(marioGetPartyId());
            if (other != NULL) {
                dir = partyToMovedir(party, other->dispDir);
            } else {
                dir = toMovedir(mario2->dispDir);
            }
        }
        dir += 90.0f;
        party->moveAngle = revise360(dir);
    }
    limit = 1.5f * mario->wPlayerCollisionBox.x;
    x = mario->position.x;
    z = mario->position.z;
    y = mario->position.y;
    movePos(&x, &z, limit, party->moveAngle);
    angleABf(party->position.x, party->position.z, x, z);
    ytmp = y + touch_wall_ct;
    hd = 1000.0f;
    hit = hitCheckFilter(NULL, x, ytmp, z, 0.0f, -1.0f, 0.0f, &hx, &hy, &hz, &hd, &hnx, &hny, &hnz);
    if (hit == NULL) {
        dist = -2000.0f;
    } else {
        dist = (f32)(s32)(100.0f * hy + 0.5f) / 100.0f;
        hitGetAttr(hit);
    }
    if (dist <= -2000.0f) {
        dist = 1.5f * mario->wPlayerCollisionBox.x;
        x = mario->position.x;
        z = mario->position.z;
        movePos(&x, &z, dist, toMovedir(mario->dispDir));
    }
    limit = 2.0f;
    if (mario->baseSpeed > 1.0f) {
        limit = 10.0f;
    }
    dist = distABf(party->position.x, party->position.z, x, z);
    party->timer = (party->timer) - 1;
    if (dist >= limit && (party->timer) > 0) {
        party->dir = angleABf(party->position.x, party->position.z, x, z);
        dist = distABf(party->position.x, party->position.z, x, z) / 10.0f;
        if (dist > 2.0f) {
            dist = 2.0f;
        }
        if (dist < 1.0f) {
            dist = 1.0f;
        }
        partySetSpeed(party, dist);
        partyMoveCheckWall(party);
        partyChgSpeedPose(party, party->speed);
        hit = partySearchGround(party, 10.0f, 0.0f);
        if (hit != NULL) {
            if (hitGetAttr(hit) & 0x200) {
                hit = NULL;
                if (party->splashTimer == 0) {
                    party->trigFlags = (party->trigFlags | 0x1000000);
                    party->flags = (party->flags | PARTY_FLAG_FORCE_FALL);
                }
            }
        }
        party->groundHit = hit;
        if (hit == NULL) {
            partyChgMot(party, 3);
            party->jumpPara3 = -1.0f;
        }
        party->position.y = party->groundY;
    } else {
        party->timer = 0;
        partyChgMoveMode(party, 1);
        partyChgMot(party, 0);
        partySetDir(party, mario->position.x, mario->position.z);
        party->homingDist = 50.0f;
        party->flags2 = (party->flags2 & ~0x10);
        party->flags = (party->flags | PARTY_FLAG_IS_BEHIND_MARIO);
    }
    if (party->pushWallHit != NULL) {
        partyChgMoveMode(party, 1);
        partyChgMot(party, 0);
        partySetDir(party, mario->position.x, mario->position.z);
        party->homingDist = 50.0f;
        party->flags2 = (party->flags2 & ~0x10);
    }
}
#pragma opt_propagation reset

// The appear-position stores are scratch Vecs seeded from vec3_802f3e40+0x18/0x24/0x30/0x3c (the natural three scalar stores elide the copies and shrink the frame from 0xb0 to 0x70). The getFrontFloor/<= path
// also shares one store, which the previous version dropped. Declaring `f32 sinV, cosV;` before `f32 tx,ty,tz;`
// puts the f32 stack slots in the original's order (0x8..0x18), moving `Vec wall;` into its case puts the scratch
// Vecs at 0x28/0x34/0x40/0x4c, `u32 id;` fixes the GPR rotation, and the negated float tests reproduce MWCC's crors.
// The big wins were behavioural. The `else` path (readId != writeId) moves from the readId target position
// (`tx`/`tz`), not party->position; the `if` path (dist > limit) seeds tx/tz from party->position first,
// exactly as the target stores x/z into the 0x10/0x8 slots. Declaring the limit/step pair as
// `f32 f31, f30, f29;` and using the early partyGetTargetDir result as the separate `f29` (declared after
// them) is what makes MWCC colour the 50/min in f30 and the distance in f31; and
// `party->playerPtr->playerGravity` reproduces the target's fresh `lwz r3, 0x160(r28)` (the local `mario` is
// not reused there).
// The rounded travel-time expression assigns its distance and scale at their point of use.
// The min block, the two `+ 10.0f` operand orders and the
// `!(h >= 0)` branch are now exact: inlining the min product, `ty += 10.0f`/`floor.y += 10.0f`, writing the
// inner test as the ternary `h = (h >= 0.0f) ? h : 10.0f`, and `#pragma opt_lifetimes off` around the
// function (just below) are all needed for the target's f0/f2 colouring and its `bne assign; b end` layout.
// The old `cmpw r27, r3` diff is gone: storing the read id in an `s32 readId` local before the test makes
// MWCC emit the target's `cmpw r3, r27`.
#pragma opt_lifetimes off
// Keep the nested travel-time assignments until backend allocation; their redundant copies then disappear.
#pragma opt_dead_assignments off
// Retail keeps the distance (f5) live into the height code, so the height takes f6. The distance
// self-stores below give the allocator those ranges; the post-allocation peephole then deletes each
// load/store pair (the #661 self-store lever).
void party_motion_jump(PartyEntry* party) {
    MarioWork* mario;
    HitEntry* hit;
    Vec floor;
    f32 sinV, cosV;
    f32 tx, ty, tz;
    f32 angle;
    f32 f31, f30, f29;
    f32 f0, f1, f3, f4, f6, h;
    s32 id;
    s32 writeId;
    s32 readId; // holds the call result so MWCC emits the target's `cmpw r3, r27` (see comment above)

    mario = party->playerPtr;
    id = partyGetFootmarkId(&party->flags);
    if (party->trigFlags & 0x2) {
        party->trigFlags = (party->trigFlags & ~0x2);
        party->motionStep = 0;
    }
    switch (party->motionStep) {
    case 0:
        tx = party->targetPos.x;
        ty = party->targetPos.y;
        tz = party->targetPos.z;
        if (party->forceMoveFrames != 0 || party->runMode == 9) {
            tx = party->position.x;
            ty = party->position.y;
            tz = party->position.z;
            movePos(&tx, &tz, party->speed, party->dir);
            {
                Vec pos = FV(vec3_802f3e58, ((Vec){0.0f, 0.0f, 0.0f}));
                pos.x = tx;
                pos.y = ty;
                pos.z = tz;
                party->targetPos = pos;
            }
        } else if (!(party->trigFlags & 0x400000)) {
            writeId = partyGetFootmarkWriteId(id);
            readId = partyGetFootmarkReadId(id);
            if (readId == writeId) {
                if (partyGetTargetDist(id, party->position.x, party->position.z) > 2.0f * mario->wPlayerCollisionBox.x) {
                    f29 = partyGetTargetDir(id, party->position.x, party->position.z);
                    getFrontFloor(party, &floor, f29);
                    floor.y += 10.0f;
                    ty = floor.y;
                    tx = party->position.x;
                    tz = party->position.z;
                    movePos(&tx, &tz, 0.6f * party->radius, f29);
                }
                {
                    Vec pos = FV(vec3_802f3e64, ((Vec){0.0f, 0.0f, 0.0f}));
                    pos.x = tx;
                    pos.y = ty;
                    pos.z = tz;
                    party->targetPos = pos;
                }
            } else {
                partyGetReadId(id, &tx, &ty, &tz);
                f30 = 50.0f;
                f31 = distABf(party->position.x, party->position.z, tx, tz);
                // Recomputing the product instead of naming a temporary keeps the constant 50
                // in f0 and the product in f1.
                if (50.0f > 1.4f * (ty - party->position.y)) {
                    f30 = 1.4f * (ty - party->position.y);
                }
                angle = angleABf(party->position.x, party->position.z, tx, tz);
                sincosf(&sinV, &cosV, angle);
                if (f31 > f30) {
                    f31 = f30;
                    if (partyGetTargetDistY(id) == 0.0f) {
                        f31 = 0.0f;
                    }
                    tx = party->position.x;
                    tz = party->position.z;
                    tx = tx + f31 * sinV;
                    tz = tz + f31 * cosV;
                    ty += 10.0f;
                    {
                        Vec pos = FV(vec3_802f3e70, ((Vec){0.0f, 0.0f, 0.0f}));
                        pos.x = tx;
                        pos.y = ty;
                        pos.z = tz;
                        party->targetPos = pos;
                    }
                } else {
                    if (partyGetTargetDistY(id) == 0.0f) {
                        f31 = 0.0f;
                    }
                    tx += f31 * sinV;
                    tz += f31 * cosV;
                    {
                        Vec pos = FV(vec3_802f3e7c, ((Vec){0.0f, 0.0f, 0.0f}));
                        pos.x = tx;
                        pos.y = ty;
                        pos.z = tz;
                        party->targetPos = pos;
                    }
                }
                if (f31 < f30) {
                    partyNextReadId(id);
                }
            }
        }
        party->targetDist = distABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z);
        if (party->forceMoveFrames == 0) {
            party->dir = angleABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z);
            party->speed = 2.25f * party->playerPtr->playerGravity;
        }
#ifdef MUST_MATCH
        // Nested assignments preserve the distance web until allocation, yielding retail's f5 load.
        f0 = (10.0f * (f1 = 0.9f * ((f6 = party->targetDist) / party->speed)) + 0.5f) / 10.0f;
        party->targetDist = f6;
#else
        f6 = party->targetDist;
        f1 = 0.9f * (f6 / party->speed);
        f0 = (10.0f * f1 + 0.5f) / 10.0f;
#endif
        party->timer = (s32)f0;
        if (party->timer < 0xc) {
            party->timer = 0xc;
        }
        if (party->timer > 0x28) {
            party->timer = 0x28;
        }
        f4 = 0.5f * (f32)party->timer;
        h = party->targetPos.y - party->position.y;
        if (fabsf(h) < 1.0f) {
#ifdef MUST_MATCH
            party->targetDist = (f6 = party->targetDist);
#endif
            h = 25.0f;
        } else {
            h = (h >= 0.0f) ? h : 10.0f;
        }
        f3 = (h + h) / (f4 * f4 - f4);
        party->jumpPara1 = mario->playerGravity * (f3 * f4);
        if (party->jumpPara1 > 13.0f) {
            party->jumpPara1 = 13.0f;
        }
        if (party->jumpPara1 < 3.0f) {
            party->jumpPara1 = 3.0f;
        }
        party->jumpPara3 = -f3 * mario->playerGravity;
        party->flags = (party->flags | PARTY_FLAG_IS_JUMPING);
        party->flags = (party->flags & ~PARTY_FLAG_IS_GROUNDED);
        partyChgPoseId(party, 4);
        psndSFXOn_3D((const char*)0x927, &party->position);
        party->motionStep = 0xa;
        if (!(party->flags & PARTY_FLAG_IS_PARTNER)) {
            partyNextReadId(partyGetFootmarkId(&party->flags));
        }
        break;
    case 10:
        if (!(party->flags & PARTY_FLAG_IS_PARTNER)) {
            partyNextReadId(partyGetFootmarkId(&party->flags));
        }
        party->jumpPara1 = party->jumpPara1 + party->jumpPara3;
        party->position.y = party->position.y + party->jumpPara1;
        if (party->position.y < -2000.0f) {
            party->flags = (party->flags & ~PARTY_FLAG_IS_JUMPING);
            party->jumpPara1 = 0.0f;
            party->dir = angleABf(party->position.x, party->position.z, mario->position.x, mario->position.z);
            party->position = mario->position;
            partyChgMot(party, 0);
            break;
        }
        if (party->jumpPara1 <= 0.0f) {
            party->flags = (party->flags & ~PARTY_FLAG_IS_JUMPING);
            party->flags = (party->flags | PARTY_FLAG_IS_FALLING);
            hit = partySearchGround(party, fabsf(party->jumpPara1), 0.0f);
            if (hit != NULL && (hitGetAttr(hit) & 0x200)) {
                if (party->splashTimer == 0) {
                    party->trigFlags = (party->trigFlags | 0x1000000);
                    party->flags = (party->flags | PARTY_FLAG_FORCE_FALL);
                }
                party->groundHit = NULL;
                partyChgMot(party, 3);
                party->jumpPara3 = -1.0f;
                break;
            }
            party->groundHit = hit;
            party->position.y = party->groundY;
            if (hit != NULL) {
                party->flags = (party->flags | PARTY_FLAG_IS_GROUNDED);
                party->flags = (party->flags & ~0x70);
                N_partyPlaySfxPartyLanding1(party);
                party->jumpPara1 = 0.0f;
                partyChgMot(party, 0);
                party->trigFlags = (party->trigFlags | 0x00800000);
                break;
            }
        }
        {
        Vec wall;
        if (party->flags & PARTY_FLAG_DISABLE_FRONT_WALL_CHK) {
            movePos(&wall.x, &wall.z, party->speed, party->dir);
        } else {
            wall = party->position;
            hit = partySearchFrontWall(party, &wall, party->speed, party->dir);
            if (hit != NULL) {
                party->frontWallHit = hit;
                if (party->speed) {
                    party->pushWallHit = hit;
                }
            }
            party->position.x = wall.x;
            party->position.z = wall.z;
        }
        }
        break;
    default:
        break;
    }
}
#pragma opt_dead_assignments reset
#pragma opt_lifetimes reset

void party_motion_jump2(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    f32 d;
    f32 ratio;
    f32 diff;
    f32 ff;
    HitEntry* hit;
    s32 mot;

    if (party->trigFlags & 0x2) {
        party->trigFlags = (party->trigFlags & ~0x2);
        party->timer = 0;
        party->jumpPara3 = -1.0f * mario->playerGravity;
        party->speed = 2.25f * mario->playerGravity;
        party->dir = angleABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z);
        d = distABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z);
        if (d > 50.0f) {
            d = 50.0f;
        }
        party->targetDist = d;
        ratio = d / party->speed;
        diff = party->targetPos.y - party->position.y;
        {
            s32 frames = (s32)((10.0f * ratio + 0.5f) / 10.0f);
            if (frames < 10) {
                frames = 10;
            }
            party->timer = frames;
            ff = (f32)frames;
        }
        party->speed = mario->playerGravity * (party->targetDist / ff);
        party->jumpPara1 = (0.5f * (ff * (ff * fabsf(party->jumpPara3))) + diff) / ff;
        party->jumpPara1 = party->jumpPara1 * mario->playerGravity;
        if (party->jumpPara1 > 20.0f) {
            party->jumpPara1 = 20.0f;
        }
        if (party->jumpPara1 < 0.0f) {
            party->jumpPara1 = 0.0f;
        }
        partyChgPoseId(party, 4);
        party->flags = (party->flags | PARTY_FLAG_IS_JUMPING);
        party->flags = (party->flags & ~PARTY_FLAG_IS_GROUNDED);
        psndSFXOn_3D((const char*)0x927, &party->position);
        party->motionStep = 0;
    }
    switch (party->motionStep) {
    case 0:
        party->motionStep = 1;
        break;
    case 1: {
        Vec position;
        party->jumpPara1 = party->jumpPara1 + party->jumpPara3;
        party->position.y = party->position.y + party->jumpPara1;
        position = party->position;
        partySearchFrontWall(party, &position, party->speed, party->dir);
        party->position.x = position.x;
        party->position.z = position.z;
        if (party->jumpPara1 <= 0.0f) {
            hit = partySearchGround(party, fabsf(party->jumpPara1), 0.0f);
            position.y = party->groundY;
            if (hit != NULL && (hitGetAttr(hit) & 0x200)) {
                if (party->splashTimer == 0) {
                    party->trigFlags = (party->trigFlags | 0x1000000);
                    party->flags = (party->flags | PARTY_FLAG_FORCE_FALL);
                }
                party->groundHit = NULL;
                partyChgMot(party, 3);
                party->jumpPara3 = -1.0f;
                return;
            }
            party->position.y = position.y;
            party->groundHit = hit;
            if (hit != NULL) {
                N_partyPlaySfxPartyLanding1(party);
                party->flags = (party->flags & ~0x70);
                party->flags = (party->flags | PARTY_FLAG_IS_GROUNDED);
                party->jumpPara1 = 0.0f;
                if (distABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z) < 5.0f) {
                    mot = 0;
                } else {
                    mot = 1;
                }
                partyChgMot(party, mot);
            }
        }
        break;
    }
    }
    if (party->position.y < -2000.0f) {
        party->flags = (party->flags & ~PARTY_FLAG_IS_JUMPING);
        party->jumpPara1 = 0.0f;
        party->dir = angleABf(party->position.x, party->position.z, mario->position.x, mario->position.z);
        party->position = mario->position;
        partyChgMot(party, 0);
    }
}

s32 party_motion_fall(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    HitEntry* hit;
    s32 attr;
    f32 frontWall;

    if (!(party->flags & PARTY_FLAG_IS_PARTNER)) {
        partyNextReadId(partyGetFootmarkId(&party->flags));
    }
    if (party->trigFlags & 0x2) {
        party->trigFlags = (party->trigFlags & ~0x2);
        partyChgPoseId(party, 4);
        party->flags = (party->flags & ~PARTY_FLAG_IS_BEHIND_MARIO);
    }
    hit = NULL;
    if (!(party->flags & PARTY_FLAG_FORCE_FALL)) {
        hit = partySearchGround(party, 10.0f, fabsf(party->jumpPara1));
    }
    if (hit != NULL) {
        attr = hitGetAttr(hit);
        if (attr & 0x200) {
            hit = NULL;
            if (party->isFlyer == 0 && party->splashTimer == 0) {
                party->trigFlags = (party->trigFlags | 0x1000000);
                party->flags = (party->flags | PARTY_FLAG_FORCE_FALL);
            }
        }
    }
    party->groundHit = hit;
    if (hit == NULL) {
        party->flags = (party->flags & ~PARTY_FLAG_IS_GROUNDED);
        if (!(party->flags & PARTY_FLAG_IS_FALLING)) {
            party->flags = (party->flags | PARTY_FLAG_IS_FALLING);
            if (party->jumpPara3 == 0.0f) {
                party->jumpPara3 = -1.4f;
            }
        }
        party->jumpPara1 = party->jumpPara1 + party->jumpPara3;
        if (party->jumpPara1 < -16.0f) {
            party->jumpPara1 = -16.0f;
        }
        party->position.y = party->position.y + party->jumpPara1;
        if (party->position.y < -2000.0f) {
            party->position.y = -2001.0f;
            if (mario->currMotionId == 0x1F || mario->currMotionId == 0x20) {
                return 2;
            }
            party->flags = (party->flags & ~0x70);
            party->jumpPara1 = 0.0f;
            party->jumpPara3 = 0.0f;
            party->dir = angleABf(party->position.x, party->position.z, mario->position.x, mario->position.z);
            party->moveAngle = revise360(180.0f + toMovedir(mario->dispReqDir));
            partyGetAppearPos4(party, &party->position);
            party->position.y += 37.0f;
            partyChgMot(party, 0);
            party->flags = (party->flags & ~PARTY_FLAG_FORCE_FALL);
        }
        frontWall = party->speed;
        if (frontWall) {
            Vec position = party->position;
            partySearchFrontWall(party, &position, 1.3f * frontWall, party->dir);
            party->position.x = position.x;
            party->position.z = position.z;
        }
        return 2;
    }
    if (party->flags & PARTY_FLAG_IS_FALLING) {
        party->flags = (party->flags & ~0x70);
        party->flags = (party->flags | PARTY_FLAG_IS_GROUNDED);
        party->jumpPara1 = 0.0f;
        party->position.y = party->groundY;
        if (party->speed == 0.0f) {
            partyChgMot(party, 0);
        } else {
            partyChgMot(party, 1);
        }
        N_partyPlaySfxPartyLanding1(party);
        return 3;
    }
    party->flags = (party->flags & ~PARTY_FLAG_MOT_SLIT_FLOOR);
    party->flags = (party->flags & ~0x70);
    party->flags = (party->flags | PARTY_FLAG_IS_GROUNDED);
    party->jumpPara1 = 0.0f;
    party->position.y = party->groundY;
    if (party->speed == 0.0f) {
        partyChgMot(party, 0);
    } else {
        partyChgMot(party, 1);
    }
    N_partyPlaySfxPartyLanding1(party);
    if (!(party->flags & PARTY_FLAG_IS_PARTNER)) {
        partyClearFootmark();
    } else {
        partyClearFootmark2();
    }
    return 3;
}

// Local declarations: not yet in a header.

void party_motion_upstairs(PartyEntry* party) {
    if (party->trigFlags & 0x2) {
        party->trigFlags = (party->trigFlags & ~0x2);
        party->flags = (party->flags | PARTY_FLAG_IS_STEPPING);
        upstairs_frames = 3;
        upstairs_step_y = (party->stepUpY - party->position.y) / 3.0f;
    }
    party->position.y = party->position.y + upstairs_step_y;
    if (--upstairs_frames <= 0) {
        party->flags = (party->flags & ~PARTY_FLAG_IS_STEPPING);
        partyChgMot(party, 1);
    }
}

// The original re-tests `hit` inside the `hit != NULL` guard (the compiler kept the
// dead inner test and a dead `li r0,0`); the MUST_MATCH body reproduces that
// materialisation, and the natural short-circuit form is under #else.
void party_motion_damage(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    f32 x, y, z;

    if (party->trigFlags & 0x2) {
        party->trigFlags = (party->trigFlags & ~0x2);
        party->motionStep = 0;
    }
    switch (party->motionStep) {
    case 0:
        partyChgPoseId(party, 8);
        party->jumpPara1 = 15.0f;
        party->jumpPara2 = 0.0f;
        party->timer = 0x3C;
        party->motionStep = 1;
        // fallthrough
    case 1:
        party->position.y = party->position.y + party->jumpPara1;
        {
            Vec position = party->position;
            marioGetScreenPos(position, &x, &y, &z);
            if (y <= -300.0f) {
                party->timer = 0x14;
                party->motionStep = 0xA;
            }
        }
        break;
    case 10:
        if (mario->currMotionId == 0x1F || mario->currMotionId == 0x20) {
            break;
        }
        if (--party->timer > 0) {
            break;
        }
        {
            Vec position;
            partyGetReturnPos(&position);
            party->position.x = position.x;
            party->position.y = position.y + 200.0f;
            party->position.z = position.z;
        }
        party->jumpPara1 = 0.0f;
        party->jumpPara2 = -1.0f;
        partySetSpeed(party, 0.0f);
        party->flags = (party->flags | PARTY_FLAG_IS_FALLING);
        party->flags = (party->flags | PARTY_FLAG_FORCE_FALL);
        party->motionStep = 0xB;
        break;
    case 11:
        {
            Vec position = party->position;
            marioGetScreenPos(position, &x, &y, &z);
            if (y > 240.0f) {
                party->flags = (party->flags & ~PARTY_FLAG_FORCE_FALL);
            }
        }
        partyMove(party);
        if (party->jumpPara1 <= -6.0f) {
            party->jumpPara2 = 0.0f;
            party->jumpPara1 = -6.0f;
        }
        if (party->position.y < -2000.0f) {
            party->position = mario->position;
            party->position.y += 200.0f;
            party->flags = (party->flags & ~0x70);
        }
        {
            HitEntry* hit = party->groundHit;
#ifdef MUST_MATCH
            // MUST_MATCH: retail re-tests the same pointer inside the guard, so the
            // boolean is materialised into r0 with a dead `li r0,0`.
            if (hit != NULL) {
                s32 hitFlag;
                if (hit == NULL) {
                    hitFlag = 0;
                } else if (hitGetAttr(hit) & 0x200) {
                    hitFlag = 1;
                } else {
                    hitFlag = 0;
                }
                if (hitFlag == 0) {
                    party->motionStep = 0xC;
                    party->timer = 0x1E;
                }
            }
#else
            if (hit != NULL && !(hitGetAttr(hit) & 0x200)) {
                party->motionStep = 0xC;
                party->timer = 0x1E;
            }
#endif
        }
        break;
    case 12:
        if (--party->timer > 0) {
            break;
        }
        partyChgRunMode(party, 2);
        party->moveMode = 0;
        party->flags2 = (party->flags2 & ~0x10);
        break;
    }
}

void allPartySlitOn(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    if (party != NULL) {
        if ((party->playerPtr->trigFlags & 0x80000000) == 0) {
            party->trigFlags = (party->trigFlags | 1);
            party->paperMot = 1;
        }
    }
    party = partyGetPtr(marioGetExtraPartyId());
    if (party != NULL) {
        if ((party->playerPtr->trigFlags & 0x80000000) == 0) {
            party->trigFlags = (party->trigFlags | 1);
            party->paperMot = 1;
        }
    }
}

void allPartySlitOff(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    if (party != NULL && (party->flags & PARTY_FLAG_PAPER_MODE)) {
        if (party->paperMotStep < 0x28) {
            party->paperMotStep = 0x28;
        }
    }
    party = partyGetPtr(marioGetExtraPartyId());
    if (party != NULL && (party->flags & PARTY_FLAG_PAPER_MODE)) {
        if (party->paperMotStep < 0x28) {
            party->paperMotStep = 0x28;
        }
    }
}

void allPartyForceSlitOff(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    if (party != NULL && ((party->flags & PARTY_FLAG_PAPER_MODE) || (party->flags2 & 0x100))) {
        f32 angle = revise360(party->dir - party->playerTargetYaw);
        f32 val = 180.0f;
        if (angle >= 180.0f) {
            val = 0.0f;
        }
        party->dispDir = val;
        party->dispReqDir = party->dispDir;
        party->moveDir = revise360(270.0f - party->dispDir);
        party->dir = party->moveDir;
        party->flags2 = (party->flags2 & ~0x3C0);
        partyPaperOff(party);
        party->flags = (party->flags & ~0x3000);
        party->paperMot = 0;
        partyChgPoseId(party, 1);
    }
    party = partyGetPtr(marioGetExtraPartyId());
    if (party != NULL && ((party->flags & PARTY_FLAG_PAPER_MODE) || (party->flags2 & 0x100))) {
        f32 angle = revise360(party->dir - party->playerTargetYaw);
        f32 val = 180.0f;
        if (angle >= 180.0f) {
            val = 0.0f;
        }
        party->dispDir = val;
        party->dispReqDir = party->dispDir;
        party->moveDir = revise360(270.0f - party->dispDir);
        party->dir = party->moveDir;
        party->flags2 = (party->flags2 & ~0x3C0);
        partyPaperOff(party);
        party->flags = (party->flags & ~0x3000);
        party->paperMot = 0;
        partyChgPoseId(party, 1);
    }
}

void L_partyForceSlitOff(PartyEntry* party) {
    if ((party->flags & PARTY_FLAG_PAPER_MODE) || (party->flags2 & 0x100)) {
        f32 angle = revise360(party->dir - party->playerTargetYaw);
        f32 val = 180.0f;
        if (angle >= 180.0f) {
            val = 0.0f;
        }
        party->dispDir = val;
        party->dispReqDir = party->dispDir;
        party->moveDir = revise360(270.0f - party->dispDir);
        party->dir = party->moveDir;
        party->flags2 = (party->flags2 & ~0x3C0);
        partyPaperOff(party);
        party->flags = (party->flags & ~0x3000);
        party->paperMot = 0;
        partyChgPoseId(party, 1);
    }
}

void partyChgMotSlitFloor(PartyEntry* party) {
    if (party->flags & PARTY_FLAG_MOT_SLIT_FLOOR) {
        return;
    }
    if ((party->playerPtr->trigFlags & 0x80000000) == 0) {
        party->trigFlags = (party->trigFlags | 1);
        party->paperMot = 1;
    }
    party->flags = (party->flags | PARTY_FLAG_MOT_SLIT_FLOOR);
}

BOOL party_slit(PartyEntry* party) {
    s32 ret = 1;
    MarioWork* mario = party->playerPtr;
    f32 angle;

    if (party->paperMot != 1) {
        return 1;
    }
    if (party->trigFlags & 0x1) {
        party->trigFlags = (party->trigFlags & ~0x1);
        party->flags2 = (party->flags2 | 0x100);
        partyPaperOn(party, "p_slit");
        partyChgPaper(party, "PM_S_1A");
        party->timer = 0x1E;
        partySetSpeed(party, 0.0f);
        angle = revise360(party->dir - mario->targetYaw);
        if (angle <= 180.0f) {
            party->flags2 = (party->flags2 & ~0x200);
            party->paperMotStep = 0;
        } else {
            party->flags2 = (party->flags2 | 0x200);
            party->paperMotStep = 2;
        }
    }
    switch (party->paperMotStep) {
    case 0:
        if (--party->timer < 0) {
            party->flags = (party->flags | PARTY_FLAG_PAPER_MODE);
            party->dispDir = 270.0f;
            party->dispReqDir = party->dispDir;
            partyChgPaper(party, "PM_S_1B");
            if (!(party->flags & PARTY_FLAG_MOT_SLIT_FLOOR)) {
                party->paperMotStep = 0xA;
                ret = 0;
            } else {
                party->paperMotStep = 0x14;
            }
        }
        break;
    case 2:
        if (--party->timer < 0) {
            party->flags = (party->flags | PARTY_FLAG_PAPER_MODE);
            party->dispDir = 270.0f;
            party->dispReqDir = party->dispDir;
            partyChgPaper(party, "PM_S_1B");
            if (!(party->flags & PARTY_FLAG_MOT_SLIT_FLOOR)) {
                party->paperMotStep = 0xA;
                ret = 0;
            } else {
                party->paperMotStep = 0x14;
            }
        }
        break;
    case 0xA:
        if (!party->speed) {
            partyChgPaper(party, "PM_S_1B");
            party->dispDir = 270.0f;
        } else {
            angle = revise360(party->dir - mario->targetYaw);
            if (angle >= 180.0) {
                party->flags2 = (party->flags2 | 0x200);
            } else {
                party->flags2 = (party->flags2 & ~0x200);
            }
            party->dispDir = (party->flags2 & 0x200) ? 280.0f : 260.0f;
            angle = revise360(party->dir - mario->targetYaw);
            if (angle >= 315.0f || angle <= 45.0f || (angle >= 135.0f && angle <= 225.0f)) {
                partyChgPaper(party, "PM_S_1E");
            } else {
                partyChgPaper(party, "PM_S_1D");
            }
        }
        if (!(mario->flags & 0x100000)) {
            if (!(hitGetAttr(party->frontWallHit) & 0x2000)) {
                if (!(hitGetAttr(party->pushWallHit) & 0x2000)) {
                    party->paperMotStep = 0x28;
                }
            }
        }
        break;
    default:
        break;
    }
    switch (party->paperMotStep) {
    case 0x14:
        partyChgPaper(party, "PM_S_1H");
        party->flags = (party->flags | PARTY_FLAG_FORCE_FALL);
        party->flags = (party->flags | PARTY_FLAG_IS_FALLING);
        party->timer = 0x14;
        partySetSpeed(party, 0.0f);
        party->paperMotStep = 0x15;
        break;
    case 0x15:
        angle = revise360(party->dir - party->playerTargetYaw);
        party->dispDir = (angle <= 180.0f) ? 260.0f : 280.0f;
        angle = -3.0f;
        party->position.y = party->position.y + angle;
        if (--party->timer <= 0) {
            party->paperMotStep = 0x16;
        }
        break;
    case 0x16:
        party->paperMotStep = 0xA;
        party->flags = (party->flags & ~PARTY_FLAG_FORCE_FALL);
        marioClearSlitFloor();
        break;
    default:
        break;
    }
    switch (party->paperMotStep) {
    case 0x28:
        partyChgPaper(party, "PM_S_1C");
        party->timer = 0x1E;
        party->paperMotStep = 0x29;
        angle = revise360(party->dir - mario->targetYaw);
        party->dispDir = (angle >= 180.0f) ? 0.0f : 180.0f;
        party->dispReqDir = party->dispDir;
        // fallthrough
    case 0x29:
        if (--party->timer <= 0) {
            party->paperMotStep = 0x2A;
        }
        break;
    case 0x2A:
        party->flags2 = (party->flags2 & ~0x3C0);
        partyPaperOff(party);
        party->flags = (party->flags & ~0x3000);
        party->paperMot = 0;
        break;
    default:
        break;
    }
    return ret;
}

// `next = rate * (target - party->dispDir); next = next + party->dispDir;` (the field read inlined
// twice, `rate = 0.1f` local) gives the target's fsubs/fmuls/fadds registers and load order;
// retail tests `fabsf(target - next < 1.0f)`: the comparison result goes through int->float, fabs and a
// compare with zero, so the misplaced parenthesis is kept.
s32 party_roll(PartyEntry* party) {
    s32 ret = 1;
    f32 target;
    f32 next;
    f32 rate = 0.1f;

    if (party->paperMot != 2) {
        return 1;
    }
    if (party->trigFlags & 0x1) {
        party->trigFlags = (party->trigFlags & ~0x1);
        partyPaperOn(party, "p_roll");
        partyChgPaper(party, "PM_R_1A");
        partyChgPoseId(party, 0);
        party->spinPivot.x = -6.0f;
        party->spinPivot.y = 6.0f;
        party->dispOffset.x = 6.0f;
        party->dispOffset.y = 2.0f;
        party->timer = 0x41;
        party->paperMotStep = 0;
        partySetSpeed(party, 0.0f);
    }
    switch (party->paperMotStep) {
    case 0:
        party->timer = party->timer - 1;
        if (party->timer <= 0) {
            partyChgPaper(party, "PM_R_1B");
            party->paperMotStep = 0xA;
        }
        break;
    case 0xA:
        party->dispDir = revise360(270.0f - party->dir);
        if (revise360(party->moveDir + party->dispDir) > 180.0f) {
            party->dispRot.z = revise360(7.0f * party->speed + party->dispRot.z);
        } else {
            party->dispRot.z = revise360(party->dispRot.z - 7.0f * party->speed);
        }
        break;
    }
    switch (party->paperMotStep) {
    case 0x64:
        party->speed = 0.0f;
        party->dispRot.x = 0.0f;
        party->dispRot.z = 0.0f;
        party->spinPivot.x = 0.0f;
        party->spinPivot.y = 0.0f;
        party->dispOffset.x = (party->dir < 180.0f) ? 3.0f : -3.0f;
        party->dispOffset.y = 0.0f;
        party->timer = 0x41;
        party->paperMotStep = 0x65;
        partyChgPaper(party, "PM_R_1C");
        partyPaperLightOff(party);
        // fallthrough
    case 0x65:
        ret = 0;
        party->timer = party->timer - 1;
        if (party->timer > 0) {
            if (party->dir < 180.0f) {
                target = 90.0f;
                next = rate * (target - party->dispDir);
                next = next + party->dispDir;
                if (fabsf(target - next < 1.0f)) {
                    next = target;
                }
                party->dispDir = next;
                party->dispReqDir = next;
            } else {
                target = 270.0f;
                next = rate * (target - party->dispDir);
                next = next + party->dispDir;
                if (fabsf(target - next < 1.0f)) {
                    next = target;
                }
                party->dispDir = next;
                party->dispReqDir = next;
            }
            break;
        }
        ret = 1;
        if (party->dir >= 180.0f) {
            party->dispDir = 0.0f;
            party->dispReqDir = 0.0f;
            party->moveDir = 270.0f;
            party->dir = 270.0f;
        } else {
            party->dispDir = 180.0f;
            party->dispReqDir = 180.0f;
            party->moveDir = 90.0f;
            party->dir = 90.0f;
        }
        party->spinPivot = FV(vec3_802f3e88, ((Vec){0.0f, 0.0f, 0.0f}));
        party->dispOffset = FV(vec3_802f3e94, ((Vec){0.0f, 0.0f, 0.0f}));
        party->paperMot = 0;
        party->flags = (party->flags & ~PARTY_FLAG_PAPER_MODE);
        party->flags2 = (party->flags2 & ~0x100);
        party->flags = (party->flags & ~0x200);
        party->flags2 = (party->flags2 & ~0x10);
        party->flags2 = (party->flags2 & ~0x8);
        party->paperMotStep = 0x64;
        partyPaperOff(party);
        break;
    }
    return ret;
}

// Partner speed on the aji conveyor maps; stay/homing_walk/homing_fly carry the same selection inline.
// Uncalled in retail (mwld strips it); its 18/8/30 pool after getFrontFloor's constants.
f32 partyGetAjiSpeed(PartyEntry* party) {
    f32 speed;

    if (party->isFlyer == 0) {
        speed = 18.0f;
    } else if (party->currentMemberId == PARTY_MEMBER_FLURRIE) {
        if (party->trigFlags & 0x200000) {
            speed = 8.0f;
        } else {
            speed = 30.0f;
        }
    } else if (party->currentMemberId == PARTY_MEMBER_VIVIAN) {
        if (party->trigFlags & 0x200000) {
            speed = 8.0f;
        } else {
            speed = 30.0f;
        }
    }
    return speed;
}

// Hover height above the floor (1 while flags2 0x8000 is set), as in partyFollowHeight.
// Uncalled in retail; its 5/1 pool ahead of partyGetAjiSpeed's.
f32 partyGetHoverOfs(PartyEntry* party) {
    f32 ofs = 5.0f;

    if (party->flags2 & 0x8000) {
        ofs = 1.0f;
    }
    return ofs;
}

// Inline square root used by the original (same expansion as party_hit.c).

static inline s32 pt_fpclassifyf(f32 x) {
    switch (*(s32*)&x & 0x7F800000) {
        case 0x7F800000:
            if (*(s32*)&x & 0x007FFFFF) {
                return PT_FP_NAN;
            } else {
                return PT_FP_INFINITE;
            }
            break;
        case 0:
            if (*(s32*)&x & 0x007FFFFF) {
                return PT_FP_SUBNORMAL;
            } else {
                return PT_FP_ZERO;
            }
            break;
        default:
            return PT_FP_NORMAL;
    }
}


static inline f32 pt_sqrtf(f32 x) {
    const f64 _half = .5;
    const f64 _three = 3.0;

    if (x > 0.0f) {
        f64 guess = __frsqrte(x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        return x * guess;
    } else if (x < 0.0) {
        return PT_NAN;
    } else if (pt_isnan(x)) {
        return PT_NAN;
    }
    return x;
}

// Stack slots, statement order/shape, calls and immediates all match, as do the FP colours (scale f29, wall f28);
// the `(a < b) ? a : b` select written as if/else drops the spurious fmr the ternary emitted.
// `wall` is read before the probes (retail loads touch_wall_ct into f28 up front), the not-found height
// is set after the loop (the goto skips it when a probe hits), and `targetDistance` is stored before
// `targetDir` in the second query.
s32 getFrontFloor(PartyEntry* party, Vec* pos, f32 angle) {
    HitCheckQuery query2;
    HitCheckQuery query1;
    f32 sin2, cos2;
    f32 cos1, sin1;
    f32 scale;
    f32 wall = touch_wall_ct;
    f32 floorY;
    f32 h;
    s32 i;
    HitEntry* hit;

    scale = 2.0f * party->radius / 3.0f;
    query1.targetDir = FV(vec3_802f3ea0, ((Vec){0.0f, 1.0f, 0.0f}));
    sincosf(&sin1, &cos1, angle);
    for (i = 0; i < 3; i++) {
        query1.targetPos.x = party->position.x + (f32)i * (sin1 * scale);
        query1.targetPos.z = party->position.z + (f32)i * (cos1 * scale);
        query1.targetPos.y = party->position.y + party->height;
        query1.targetDistance = 100.0f;
        hit = hitCheckVecFilter(&query1, NULL);
        if (hit != NULL) {
            if (pt_sqrtf(query1.hitNormal.x * query1.hitNormal.x + query1.hitNormal.z * query1.hitNormal.z) < 45.0f) {
                floorY = query1.hitPos.y;
                goto found; // skips the not-found store below; storing it before the loop changes the object
            }
        }
    }
    floorY = -2000.0f;
found:
    if (floorY > -2000.0f) {
        f32 a = party->position.y + wall;
        f32 b = floorY - 2.0f;
        if (a < b) {
            query2.targetPos.y = a;
        } else {
            query2.targetPos.y = b;
        }
    } else {
        query2.targetPos.y = party->position.y + wall;
    }
    h = 0.75f * party->radius;
    sincosf(&sin2, &cos2, angle);
    query2.targetPos.x = party->position.x + sin2 * h;
    query2.targetPos.z = party->position.z + cos2 * h;
    query2.targetDistance = 300.0f;
    query2.targetDir = FV(vec3_802f3eac, ((Vec){0.0f, -1.0f, 0.0f}));
    hit = hitCheckVecFilter(&query2, NULL);
    if (hit == NULL) {
        pos->x = party->position.x;
        pos->y = party->position.y;
        pos->z = party->position.z;
        return 0;
    }
    pos->x = (f32)(s32)(1000.0f * query2.hitPos.x + (query2.hitPos.x >= 0.0f ? 0.5f : -0.5f)) / 1000.0f;
    pos->y = (f32)(s32)(1000.0f * query2.hitPos.y + (query2.hitPos.y >= 0.0f ? 0.5f : -0.5f)) / 1000.0f;
    pos->z = (f32)(s32)(1000.0f * query2.hitPos.z + (query2.hitPos.z >= 0.0f ? 0.5f : -0.5f)) / 1000.0f;
    return hitGetAttr(hit);
}

// Floor height 40 units ahead of (x, y, z) along `angle`, rounded to 1/1000 (-2000 when nothing is hit).
// Uncalled in retail; generated right after party_force_reset_outofscreen, so 40, 1000, -1, -2000 and
// +-0.5 follow that function's constants in .sdata2.
f32 partyGetFrontFloorY(f32 x, f32 y, f32 z, f32 angle) {
    HitEntry* hit;
    f32 hx, hy, hz, hd, hnx, hny, hnz;
    f32 s, c;

    sincosf(&s, &c, angle);
    x += s * 40.0f;
    z += c * 40.0f;
    hd = 1000.0f;
    hit = hitCheckFilter(NULL, x, y + touch_wall_ct, z, 0.0f, -1.0f, 0.0f, &hx, &hy, &hz, &hd, &hnx, &hny, &hnz);
    if (hit == NULL) {
        return -2000.0f;
    }
    return (f32)(s32)(1000.0f * hy + (hy >= 0.0f ? 0.5f : -0.5f)) / 1000.0f;
}

void party_force_reset_outofscreen(PartyEntry* party) {
    // Declaration order is load-bearing: the Vecs in this order, `dist` before
    // sx/sy/sz, and `mario` last give the original's frame and callee-saved set.
    HitEntry* hit;
    Vec cur;
    Vec tmp;
    Vec dir;
    Vec hitPos;
    Vec hitNormal;
    f32 dist;
    f32 sx, sy, sz;
    f32 angle;
    f32 up;
    f32 y;
    f32 range;
    s32 outer;
    s32 limit;
    s32 inner;
    MarioWork* mario;

    mario = party->playerPtr;
    if (marioCtrlOffChk() != 0) {
        return;
    }
    if (marioKeyOffChk() != 0) {
        return;
    }
    marioGetScreenPos(party->position, &sx, &sy, &sz);
    if (distABf(mario->position.x, mario->position.z, party->position.x, party->position.z) < 20.0f && sy > 800.0f) {
        partyGetAppearPos3(party, &cur);
        party->position = cur;
        return;
    }
    limit = 100;
    if (-100.0f <= sx && sx <= 700.0f && sy <= 650.0f) {
        return;
    }
    if (!(party->flags & PARTY_FLAG_IS_PARTNER)) {
        partyClearFootmark();
    } else {
        partyClearFootmark();
    }
    mario = party->playerPtr;
    outer = 0;
    cur = party->position;
    if (party->isFlyer == 0) {
        up = party->radius;
    } else if (party->currentMemberId == PARTY_MEMBER_FLURRIE) {
        up = 10.0f;
        limit = 70;
    } else {
        up = 10.0f;
        limit = 50;
    }
    dir = FV(vec3_802f3ef4, ((Vec){0.0f, -1.0f, 0.0f}));
    for (;;) {
        marioGetScreenPos(cur, &sx, &sy, &sz);
        if (-limit <= sx && sx <= limit + 600) {
            party->position = cur;
            return;
        }
        if (outer > 100) {
            return;
        }
        angle = angleABf(cur.x, cur.z, mario->position.x, mario->position.z);
        movePos(&cur.x, &cur.z, up, angle);
        range = 200.0f;
        inner = 0;
        y = cur.y;
        for (;;) {
            dist = 300.0f;
            tmp = cur;
            tmp.y += range;
            hit = partyHitCheck(party, &tmp, &dir, &hitPos, &hitNormal, &dist);
#ifdef MUST_MATCH
            // MUST_MATCH: retail re-tests hit inside the guard, materialising the
            // boolean into r0 with a dead `li r0,0` branch.
            if (hit != NULL) {
                s32 hitFlag;
                if (hit == NULL) {
                    hitFlag = 0;
                } else if (hitGetAttr(hit) & 0x200) {
                    hitFlag = 1;
                } else {
                    hitFlag = 0;
                }
                if (hitFlag == 0) {
                    y = hitPos.y;
                }
            } else {
                y = mario->position.y;
            }
#else
            if (hit == NULL) {
                y = mario->position.y;
            } else if (!(hitGetAttr(hit) & 0x200)) {
                y = hitPos.y;
            }
#endif
            tmp.y = y;
            marioGetScreenPos(tmp, &sx, &sy, &sz);
            if (sy > 100.0f) {
                dist = 100.0f;
                tmp = cur;
                if (cur.y + 74.0f > y) {
                    tmp.y += 74.0f;
                    hit = partyHitCheck(party, &tmp, &dir, &hitPos, &hitNormal, &dist);
#ifdef MUST_MATCH
                    // MUST_MATCH: same redundant pointer re-test as above.
                    if (hit != NULL) {
                        s32 hitFlag;
                        if (hit == NULL) {
                            hitFlag = 0;
                        } else if (hitGetAttr(hit) & 0x200) {
                            hitFlag = 1;
                        } else {
                            hitFlag = 0;
                        }
                        if (hitFlag == 0) {
                            y = hitPos.y;
                        }
                    }
#else
                    if (hit != NULL && !(hitGetAttr(hit) & 0x200)) {
                        y = hitPos.y;
                    }
#endif
                }
                break;
            }
            inner++;
            if (inner > 4) {
                break;
            }
            range -= 50.0f;
            if (range < 0.0f) {
                range = 0.0f;
            }
        }
        cur.y = y;
        outer++;
    }
}
