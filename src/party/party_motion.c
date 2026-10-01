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
#include "math.h"
#include "party/party_move.h"
#include "party/party_hit.h"
#include "party/party_motion.h"
#include "party/party_bomhei.h"
#include "motion/mot_stay.h"
#include "motion/mot_slit.h"
#include "party/party_chuchu.h"
#include "party/party_nokonoko.h"

// LOCAL_PROTO(psndSFXOnEx_3D): retail event callers narrow volume and pan to u8; the implementation uses s32 and sharing that signature regresses matched event code.
s32 psndSFXOnEx_3D(const char* name, Vec* position, u8 vol, u8 pan, u16 pitch, u16 flags);

// LOCAL_PROTO(psndSFX_pit): retail callers disagree on pitch width and signedness; the shared u16 signature regresses matched sound commands.
void psndSFX_pit(s32 id, s16 pitch);

// LOCAL_PROTO(psndSFX_vol): retail units disagree on volume width; a shared u8 prototype adds narrowing instructions in battle callers.
extern void psndSFX_vol(s32 id, u8 vol);

// .sbss
f32 unk_804189e8;
s32 unk_804189ec;
f32 touch_wall_ct;

// Local declarations: not yet in a header.

void party_motion_upstairs(PartyEntry* party) {
    if (party->field_8 & 0x2) {
        party->field_8 = (party->field_8 & ~0x2);
        party->flags = (party->flags | 0x40);
        unk_804189ec = 3;
        unk_804189e8 = (*(f32*)((u8*)party + 0x120) - party->position.y) / 3.0f;
    }
    party->position.y = party->position.y + unk_804189e8;
    if (--unk_804189ec <= 0) {
        party->flags = (party->flags & ~0x40);
        partyChgMot(party, 1);
    }
}

// NONMATCHING (93.99%): register allocation differs - original keeps party/attr/hit in r29/r30/r31,
// mine only r30/r31 (attr not held in a callee-saved reg).
void unk_8014ba3c(PartyEntry* party) {
    HitEntry* hit;
    s32 attr;
    Vec position = party->position;
    f32 angle = *(f32*)((u8*)party + 0xF0);

    hit = partySearchHead(party, &position, &angle, party->field_10C);
    if (hit != NULL) {
        *(HitEntry**)((u8*)party + 0x140) = hit;
    }
    if (party->flags & 0x04000000) {
        return;
    }
    party->flags = (party->flags & ~0x80);
    hit = partySearchGround(party, 15.0f, -15.0f);
    if (hit != NULL) {
        attr = hitGetAttr(hit);
        if (attr & 0x200) {
            if (party->field_4C == 0) {
                party->field_8 = (party->field_8 | 0x1000000);
                party->flags = (party->flags | 0x02000000);
            }
            hit = NULL;
        }
    }
    *(HitEntry**)((u8*)party + 0x138) = hit;
    if (hit == NULL) {
        party->flags = (party->flags & ~0x2);
        partyChgMot(party, 3);
        party->jumpPara3 = -1.5f;
        return;
    }
    party->position.y = party->field_E4;
    party->flags = (party->flags | 0x2);
    if (attr & 0x100) {
        party->flags = (party->flags | 0x80);
    } else {
        party->flags = (party->flags & ~0x80);
    }
    if (attr & 0x40000000) {
        *(HitEntry**)((u8*)party + 0x14C) = hit;
    }
    if (unk_800c8dd0(hit) != 0) {
        partyGetAppearPos5(party, hit, (Vec*)((u8*)party + 0x94));
        party->field_8 = (party->field_8 | 0x400000);
        *(f32*)((u8*)party + 0x98) = party->position.y;
        partyChgMot(party, 2);
    }
}

void partyChgMotSlitFloor(PartyEntry* party) {
    if (party->flags & PARTY_FLAG_MOT_SLIT_FLOOR) {
        return;
    }
    if ((party->playerPtr->trigFlags & 0x80000000) == 0) {
        party->field_8 = (party->field_8 | 1);
        party->field_37[0] = 1;
    }
    party->flags = (party->flags | PARTY_FLAG_MOT_SLIT_FLOOR);
}

void allPartySlitOff(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    if (party != NULL && (party->flags & 0x1000)) {
        if (party->field_3A < 0x28) {
            party->field_3A = 0x28;
        }
    }
    party = partyGetPtr(marioGetExtraPartyId());
    if (party != NULL && (party->flags & 0x1000)) {
        if (party->field_3A < 0x28) {
            party->field_3A = 0x28;
        }
    }
}

void allPartySlitOn(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    if (party != NULL) {
        if ((party->playerPtr->trigFlags & 0x80000000) == 0) {
            party->field_8 = (party->field_8 | 1);
            party->field_37[0] = 1;
        }
    }
    party = partyGetPtr(marioGetExtraPartyId());
    if (party != NULL) {
        if ((party->playerPtr->trigFlags & 0x80000000) == 0) {
            party->field_8 = (party->field_8 | 1);
            party->field_37[0] = 1;
        }
    }
}

void L_partyForceSlitOff(PartyEntry* party) {
    if ((party->flags & 0x1000) || (party->flags2 & 0x100)) {
        f32 angle = revise360(party->dir - *(f32*)((u8*)party + 0xF8));
        f32 val = 180.0f;
        if (angle >= 180.0f) {
            val = 0.0f;
        }
        party->field_10C = val;
        party->field_110 = party->field_10C;
        party->field_FC = revise360(270.0f - party->field_10C);
        party->dir = party->field_FC;
        party->flags2 = (party->flags2 & ~0x3C0);
        partyPaperOff(party);
        party->flags = (party->flags & ~0x3000);
        party->field_37[0] = 0;
        partyChgPoseId(party, 1);
    }
}

void allPartyForceSlitOff(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    if (party != NULL && ((party->flags & 0x1000) || (party->flags2 & 0x100))) {
        f32 angle = revise360(party->dir - *(f32*)((u8*)party + 0xF8));
        f32 val = 180.0f;
        if (angle >= 180.0f) {
            val = 0.0f;
        }
        party->field_10C = val;
        party->field_110 = party->field_10C;
        party->field_FC = revise360(270.0f - party->field_10C);
        party->dir = party->field_FC;
        party->flags2 = (party->flags2 & ~0x3C0);
        partyPaperOff(party);
        party->flags = (party->flags & ~0x3000);
        party->field_37[0] = 0;
        partyChgPoseId(party, 1);
    }
    party = partyGetPtr(marioGetExtraPartyId());
    if (party != NULL && ((party->flags & 0x1000) || (party->flags2 & 0x100))) {
        f32 angle = revise360(party->dir - *(f32*)((u8*)party + 0xF8));
        f32 val = 180.0f;
        if (angle >= 180.0f) {
            val = 0.0f;
        }
        party->field_10C = val;
        party->field_110 = party->field_10C;
        party->field_FC = revise360(270.0f - party->field_10C);
        party->dir = party->field_FC;
        party->flags2 = (party->flags2 & ~0x3C0);
        partyPaperOff(party);
        party->flags = (party->flags & ~0x3000);
        party->field_37[0] = 0;
        partyChgPoseId(party, 1);
    }
}

void unk_8014c47c(void) {
    touch_wall_ct = 60.0f;
}

void N_partyFollowCloseOff(PartyEntry* party) {
    party->field_8 = (party->field_8 & ~0x20000);
}

void N_partyFollowCloseOn(PartyEntry* party) {
    party->field_8 = (party->field_8 | 0x20000);
}

void party_set_behind_dir(PartyEntry* party, f32 dir) {
    CameraEntry* camera = camGetPtr(CAMERA_3D);
    *(f32*)((u8*)party + 0x54) = revise360(dir + camera->viewYaw);
    party->field_8 = (party->field_8 | 0x100000);
}

USER_FUNC(evt_snd_sfx_all_stop) {
    psndSFXAllOff();
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_set_rev_mode) {
    psndSetReverb((s32)event->args[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_env_lpf) {
    s32* args = event->args;
    s32 id = args[0];
    u16 freq = (u16)evtGetValue(event, args[1]);
    psndENV_LPF(id, freq);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_envoff_f) {
    s32* args = event->args;
    s32 id = args[0];
    s32 value = evtGetValue(event, args[1]);
    psndENVOff_f_d(id, value, 0);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_envoff) {
    psndENVOff((u32)event->args[0]);
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

USER_FUNC(evt_snd_envon) {
    psndENVOn((s32)event->args[0], (s32)event->args[1]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfx_dist) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    u16 dist = (u16)evtGetValue(event, args[1]);
    psndSFX_dist(id, dist);
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

USER_FUNC(evt_snd_sfx_vol) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    s32 vol = evtGetValue(event, args[1]);
    psndSFX_vol(id, (u8)vol);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfx_pit) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    s32 pitch = evtGetValue(event, args[1]);
    psndSFX_pit(id, (s16)pitch);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfxchk) {
    s32* args = event->args;
    s32 id = evtGetValue(event, args[0]);
    s32 result = psndSFXChk((u32)id);
    evtSetValue(event, args[1], result);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_sfxoff) {
    s32 id = evtGetValue(event, event->args[0]);
    psndSFXOff(id);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_bgm_scope) {
    s32* args = event->args;
    s32 id = args[0];
    s32 scope = evtGetValue(event, args[1]);
    psndBGMScope(id, scope);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_bgm_freq) {
    s32* args = event->args;
    s32 id = args[0];
    s32 rate = evtGetValue(event, args[1]);
    psndBGM_rate(id, (u16)rate);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_bgmoff_f) {
    s32* args = event->args;
    s32 id = args[0];
    s32 value = evtGetValue(event, args[1]);
    psndBGMOff_f_d(id, value, 0);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_bgmoff) {
    psndBGMOff((s32)event->args[0]);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_bgmon_f) {
    s32* args = event->args;
    s32 a = args[0];
    s32 b = args[1];
    s32 c = evtGetValue(event, args[2]);
    unk_800d7270(a, (char*)b, c, 0);
    return EVT_RETURN_DONE;
}

USER_FUNC(evt_snd_bgmon) {
    psndBGMOn((u32)event->args[0], (char*)event->args[1]);
    return EVT_RETURN_DONE;
}

USER_FUNC(unk_8014cc24) {
    s32* args = event->args;
    s32 a = args[0];
    s32 b = args[1];
    s32 c = evtGetValue(event, args[2]);
    evtGetValue(event, args[3]);
    psndBGMOn_f_d(a, (const char*)b, c, c, 0);
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

void unk_8014bbc8(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;

    if (party->field_8 & 0x2) {
        party->field_8 = (party->field_8 & ~0x2);
        partyChgPoseId(party, 1);
        if (!(party->flags & 0x8)) {
            partyClearFootmark();
        } else {
            partyClearFootmark2();
        }
        party->yoshiPsndSFXId = -1;
    }
    party_slit(party);
    party_roll(party);
    unk_800c9044(party);
    if (!(party->flags & 0x01000000)) {
        partyChkWall(party);
    }
    partyChkGnd(party);
    if (distABf(party->position.x, party->position.z, mario->position.x, mario->position.z) > 31.0f) {
        partyChgRunMode(party, 2);
    }
    if (mario->currMotionId == 0xE && !(party->flags & 0x00080000)) {
        partyMoveBehindMario(party, 0);
    } else if (party->field_32 == 1) {
        unk_8014b824(party);
    } else {
        unk_8014ba3c(party);
        if (party->currentMemberId == 7 && !(party->flags2 & 0x8000)) {
            if (!marioKeyOffChk() && marioChkDeepSleep() != 1 && chuchu_searchObject() != 0) {
                switch (party->useMotionId) {
                case 1:
                    if (--*(s32*)((u8*)party + 0x24) <= 0) {
                        partyChgPoseId(party, 1);
                        *(s32*)((u8*)party + 0x24) = 0x1E;
                        party->useMotionId = 2;
                    }
                    break;
                case 2:
                    if (--*(s32*)((u8*)party + 0x24) <= 0) {
                        party->useMotionId = 0;
                    }
                    break;
#ifdef MUST_MATCH
                // `case 0` is redundant with `default` but is what makes mwcc
                // build the 3-entry {0,1,2} search tree the original has.
                case 0:
#endif
                default:
                    partyChgPose(party, "PCH_A2_1");
                    party->useMotionId = 1;
                    *(s32*)((u8*)party + 0x24) = 0x1E;
                    break;
                }
            }
        }
    }
}

// The original re-tests `hit` inside the `hit != NULL` guard (the compiler kept the
// dead inner test and a dead `li r0,0`); the MUST_MATCH body reproduces that
// materialisation, and the natural short-circuit form is under #else.
void party_motion_damage(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    f32 x, y, z;

    if (party->field_8 & 0x2) {
        party->field_8 = (party->field_8 & ~0x2);
        *(u8*)((u8*)party + 0x38) = 0;
    }
    switch (*(u8*)((u8*)party + 0x38)) {
    case 0:
        partyChgPoseId(party, 8);
        party->jumpPara1 = 15.0f;
        party->jumpPara2 = 0.0f;
        *(s32*)((u8*)party + 0x24) = 0x3C;
        *(u8*)((u8*)party + 0x38) = 1;
        // fallthrough
    case 1:
        party->position.y = party->position.y + party->jumpPara1;
        {
            Vec position = party->position;
            marioGetScreenPos(position, &x, &y, &z);
            if (y <= -300.0f) {
                *(s32*)((u8*)party + 0x24) = 0x14;
                *(u8*)((u8*)party + 0x38) = 0xA;
            }
        }
        break;
    case 10:
        if (mario->currMotionId == 0x1F || mario->currMotionId == 0x20) {
            break;
        }
        if (--*(s32*)((u8*)party + 0x24) > 0) {
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
        unk_800c9398(party, 0.0f);
        party->flags = (party->flags | 0x20);
        party->flags = (party->flags | 0x02000000);
        *(u8*)((u8*)party + 0x38) = 0xB;
        break;
    case 11:
        {
            Vec position = party->position;
            marioGetScreenPos(position, &x, &y, &z);
            if (y > 240.0f) {
                party->flags = (party->flags & ~0x02000000);
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
            HitEntry* hit = *(HitEntry**)((u8*)party + 0x138);
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
                    *(u8*)((u8*)party + 0x38) = 0xC;
                    *(s32*)((u8*)party + 0x24) = 0x1E;
                }
            }
#else
            if (hit != NULL && !(hitGetAttr(hit) & 0x200)) {
                *(u8*)((u8*)party + 0x38) = 0xC;
                *(s32*)((u8*)party + 0x24) = 0x1E;
            }
#endif
        }
        break;
    case 12:
        if (--*(s32*)((u8*)party + 0x24) > 0) {
            break;
        }
        partyChgRunMode(party, 2);
        party->field_3B = 0;
        party->flags2 = (party->flags2 & ~0x10);
        break;
    }
}

s32 party_motion_fall(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    HitEntry* hit;
    s32 attr;
    f32 frontWall;

    if (!(party->flags & 0x8)) {
        partyNextReadId(partyGetFootmarkId((u32*)party));
    }
    if (party->field_8 & 0x2) {
        party->field_8 = (party->field_8 & ~0x2);
        partyChgPoseId(party, 4);
        party->flags = (party->flags & ~0x80000);
    }
    hit = NULL;
    if (!(party->flags & 0x02000000)) {
        hit = partySearchGround(party, 10.0f, fabsf(party->jumpPara1));
    }
    if (hit != NULL) {
        attr = hitGetAttr(hit);
        if (attr & 0x200) {
            hit = NULL;
            if (party->field_32 == 0 && party->field_4C == 0) {
                party->field_8 = (party->field_8 | 0x1000000);
                party->flags = (party->flags | 0x02000000);
            }
        }
    }
    *(HitEntry**)((u8*)party + 0x138) = hit;
    if (hit == NULL) {
        party->flags = (party->flags & ~0x2);
        if (!(party->flags & 0x20)) {
            party->flags = (party->flags | 0x20);
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
            *(f32*)((u8*)party + 0x54) = revise360(180.0f + toMovedir(*(f32*)((u8*)mario + 0x1AC)));
            partyGetAppearPos4(party, &party->position);
            party->position.y += 37.0f;
            partyChgMot(party, 0);
            party->flags = (party->flags & ~0x02000000);
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
    if (party->flags & 0x20) {
        party->flags = (party->flags & ~0x70);
        party->flags = (party->flags | 0x2);
        party->jumpPara1 = 0.0f;
        party->position.y = party->field_E4;
        if (party->speed == 0.0f) {
            partyChgMot(party, 0);
        } else {
            partyChgMot(party, 1);
        }
        N_partyPlaySfxPartyLanding1(party);
        return 3;
    }
    party->flags = (party->flags & ~0x2000);
    party->flags = (party->flags & ~0x70);
    party->flags = (party->flags | 0x2);
    party->jumpPara1 = 0.0f;
    party->position.y = party->field_E4;
    if (party->speed == 0.0f) {
        partyChgMot(party, 0);
    } else {
        partyChgMot(party, 1);
    }
    N_partyPlaySfxPartyLanding1(party);
    if (!(party->flags & 0x8)) {
        partyClearFootmark();
    } else {
        partyClearFootmark2();
    }
    return 3;
}

// The `f3 = 0.1f` reloads in each arm and the repeated `f2 = mario->position.y`
// are needed to reproduce the original scheduler. In the else arm, assigning
// `f1 = 0.05f` to a variable (rather than using the literal inline) makes the
// final `fmadds` emit `f0,f2,f1` instead of `f0,f1,f2`.
void unk_8014b824(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    HitEntry* hit;
    s32 flag;
    s32 attr;
    f32 f1, f2, f3, f0;

    if (party->flags & 0x04000000) {
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
            party->flags = (party->flags | 0x80);
        } else {
            party->flags = (party->flags & ~0x80);
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
            f1 = party->field_E4;
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
        f0 = 5.0f;
        f1 = party->position.y;
        f1 = f2 - f1;
        if (f1 > f0 || 0.0f > f1) {
            party->position.y += f1 * f3;
        }
    } else {
        if (hit != NULL) {
            f2 = mario->position.y;
            f1 = party->field_E4;
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
        f0 = 5.0f;
        f1 = party->position.y;
        f2 = f2 - f1;
        f1 = 0.05f;
        if (f2 > f0 || 0.0f > f2) {
            party->position.y += f2 * f1;
        }
    }
}

void party_motion_jump2(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    f32 d;
    f32 ratio;
    f32 diff;
    f32 ff;
    HitEntry* hit;

    if (party->field_8 & 0x2) {
        party->field_8 = (party->field_8 & ~0x2);
        *(s32*)((u8*)party + 0x24) = 0;
        party->jumpPara3 = -1.0f * mario->playerGravity;
        party->speed = 2.25f * mario->playerGravity;
        party->dir = angleABf(party->position.x, party->position.z, *(f32*)((u8*)party + 0x94), *(f32*)((u8*)party + 0x9C));
        d = distABf(party->position.x, party->position.z, *(f32*)((u8*)party + 0x94), *(f32*)((u8*)party + 0x9C));
        if (d > 50.0f) {
            d = 50.0f;
        }
        *(f32*)((u8*)party + 0x128) = d;
        ratio = d / party->speed;
        diff = *(f32*)((u8*)party + 0x98) - party->position.y;
        {
            s32 frames = (s32)((10.0f * ratio + 0.5f) / 10.0f);
            if (frames < 10) {
                frames = 10;
            }
            *(s32*)((u8*)party + 0x24) = frames;
            ff = (f32)frames;
        }
        party->speed = mario->playerGravity * (*(f32*)((u8*)party + 0x128) / ff);
        party->jumpPara1 = (0.5f * (ff * (ff * fabsf(party->jumpPara3))) + diff) / ff;
        party->jumpPara1 = party->jumpPara1 * mario->playerGravity;
        if (party->jumpPara1 > 20.0f) {
            party->jumpPara1 = 20.0f;
        }
        if (party->jumpPara1 < 0.0f) {
            party->jumpPara1 = 0.0f;
        }
        partyChgPoseId(party, 4);
        party->flags = (party->flags | 0x10);
        party->flags = (party->flags & ~0x2);
        psndSFXOn_3D((const char*)0x927, &party->position);
        *(u8*)((u8*)party + 0x38) = 0;
    }
    switch (*(u8*)((u8*)party + 0x38)) {
    case 0:
        *(u8*)((u8*)party + 0x38) = 1;
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
            position.y = party->field_E4;
            if (hit != NULL && (hitGetAttr(hit) & 0x200)) {
                if (party->field_4C == 0) {
                    party->field_8 = (party->field_8 | 0x1000000);
                    party->flags = (party->flags | 0x02000000);
                }
                *(HitEntry**)((u8*)party + 0x138) = NULL;
                partyChgMot(party, 3);
                party->jumpPara3 = -1.0f;
                return;
            }
            party->position.y = position.y;
            *(HitEntry**)((u8*)party + 0x138) = hit;
            if (hit != NULL) {
                N_partyPlaySfxPartyLanding1(party);
                party->flags = (party->flags & ~0x70);
                party->flags = (party->flags | 0x2);
                party->jumpPara1 = 0.0f;
                partyChgMot(party, (distABf(party->position.x, party->position.z, *(f32*)((u8*)party + 0x94), *(f32*)((u8*)party + 0x9C)) < 5.0f) ? 0 : 1);
            }
        }
        break;
    }
    }
    if (party->position.y < -2000.0f) {
        party->flags = (party->flags & ~0x10);
        party->jumpPara1 = 0.0f;
        party->dir = angleABf(party->position.x, party->position.z, mario->position.x, mario->position.z);
        party->position = mario->position;
        partyChgMot(party, 0);
    }
}

void party_force_reset_outofscreen(PartyEntry* party) {
    // Declaration order is load-bearing: the Vecs in this order, `dist` before
    // sx/sy/sz, and `mario` last give the original's frame and callee-saved set.
    extern Vec vec3_802f3ef4[];
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
    if (!(party->flags & 0x8)) {
        partyClearFootmark();
    } else {
        partyClearFootmark();
    }
    mario = party->playerPtr;
    outer = 0;
    cur = party->position;
    if (party->field_32 == 0) {
        up = party->field_F4;
    } else if (party->currentMemberId == 5) {
        up = 10.0f;
        limit = 70;
    } else {
        up = 10.0f;
        limit = 50;
    }
    dir = vec3_802f3ef4[0];
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

// Match shapes below are marked MUST_MATCH: the natural forms compile to the
// same logic but differ in mwcc register allocation / block layout.
extern const f64 double_180_802f3fa0;
BOOL party_slit(PartyEntry* party) {
    extern u8 vec3_802f3e40[];
    u8* vec = vec3_802f3e40;
    s32 ret = 1;
    MarioWork* mario = party->playerPtr;
    f32 angle;

    if (party->field_37[0] != 1) {
        return 1;
    }
    if (party->field_8 & 0x1) {
        party->field_8 = (party->field_8 & ~0x1);
        party->flags2 = (party->flags2 | 0x100);
        partyPaperOn(party, (const char*)(vec + 0x148));
        partyChgPaper(party, (char*)(vec + 0x150));
        *(s32*)((u8*)party + 0x24) = 0x1E;
        unk_800c9398(party, 0.0f);
        angle = revise360(party->dir - mario->targetYaw);
        if (angle <= 180.0f) {
            party->flags2 = (party->flags2 & ~0x200);
            party->field_3A = 0;
        } else {
            party->flags2 = (party->flags2 | 0x200);
            party->field_3A = 2;
        }
    }
    switch (party->field_3A) {
    case 0:
        if (--*(s32*)((u8*)party + 0x24) < 0) {
            party->flags = (party->flags | 0x1000);
            party->field_10C = 270.0f;
            party->field_110 = party->field_10C;
            partyChgPaper(party, (char*)(vec + 0x158));
            if (!(party->flags & 0x2000)) {
                party->field_3A = 0xA;
                ret = 0;
            } else {
                party->field_3A = 0x14;
            }
        }
        break;
    case 2:
        if (--*(s32*)((u8*)party + 0x24) < 0) {
            party->flags = (party->flags | 0x1000);
            party->field_10C = 270.0f;
            party->field_110 = party->field_10C;
            partyChgPaper(party, (char*)(vec + 0x158));
            if (!(party->flags & 0x2000)) {
                party->field_3A = 0xA;
                ret = 0;
            } else {
                party->field_3A = 0x14;
            }
        }
        break;
    case 0xA:
#ifdef MUST_MATCH
        // `!field` makes mwcc load the field into f1 and 0.0f into f0, as the original does.
        if (!party->speed) {
#else
        if (party->speed == 0.0f) {
#endif
            partyChgPaper(party, (char*)(vec + 0x158));
            party->field_10C = 270.0f;
        } else {
            angle = revise360(party->dir - mario->targetYaw);
#ifdef MUST_MATCH
            // retail reads the 180.0 double (double_180_802f3fa0) as vec+0x160
            if (angle >= *(f64*)(vec + 0x160)) {
#else
            if (angle >= double_180_802f3fa0) {
#endif
                party->flags2 = (party->flags2 | 0x200);
            } else {
                party->flags2 = (party->flags2 & ~0x200);
            }
            party->field_10C = (party->flags2 & 0x200) ? 280.0f : 260.0f;
            angle = revise360(party->dir - mario->targetYaw);
#ifdef MUST_MATCH
            // One compound test: mwcc shares a single 0x168 body at the end;
            // the natural if / else-if form duplicates that body.
            if (angle >= 315.0f || angle <= 45.0f || (angle >= 135.0f && angle <= 225.0f)) {
                partyChgPaper(party, (char*)(vec + 0x168));
            } else {
                partyChgPaper(party, (char*)(vec + 0x170));
            }
#else
            if (angle >= 315.0f || angle <= 45.0f) {
                partyChgPaper(party, (char*)(vec + 0x168));
            } else if (angle < 135.0f || angle > 225.0f) {
                partyChgPaper(party, (char*)(vec + 0x170));
            } else {
                partyChgPaper(party, (char*)(vec + 0x168));
            }
#endif
        }
        if (!(mario->flags & 0x100000)) {
            if (!(hitGetAttr(*(HitEntry**)((u8*)party + 0x12C)) & 0x2000)) {
                if (!(hitGetAttr(*(HitEntry**)((u8*)party + 0x134)) & 0x2000)) {
                    party->field_3A = 0x28;
                }
            }
        }
        break;
    default:
        break;
    }
    switch (party->field_3A) {
    case 0x14:
        partyChgPaper(party, (char*)(vec + 0x178));
        party->flags = (party->flags | 0x02000000);
        party->flags = (party->flags | 0x20);
        *(s32*)((u8*)party + 0x24) = 0x14;
        unk_800c9398(party, 0.0f);
        party->field_3A = 0x15;
        break;
    case 0x15:
        angle = revise360(party->dir - *(f32*)((u8*)party + 0xF8));
        party->field_10C = (angle <= 180.0f) ? 260.0f : 280.0f;
#ifdef MUST_MATCH
        // Staging -3.0f in a temp makes mwcc put position.y in f1 and the
        // constant in f0, matching the original's fadds operand order.
        angle = -3.0f;
        party->position.y = party->position.y + angle;
#else
        party->position.y = party->position.y + -3.0f;
#endif
        if (--*(s32*)((u8*)party + 0x24) <= 0) {
            party->field_3A = 0x16;
        }
        break;
    case 0x16:
        party->field_3A = 0xA;
        party->flags = (party->flags & ~0x02000000);
        marioClearSlitFloor();
        break;
    default:
        break;
    }
    switch (party->field_3A) {
    case 0x28:
        partyChgPaper(party, (char*)(vec + 0x180));
        *(s32*)((u8*)party + 0x24) = 0x1E;
        party->field_3A = 0x29;
        angle = revise360(party->dir - mario->targetYaw);
        party->field_10C = (angle >= 180.0f) ? 0.0f : 180.0f;
        party->field_110 = party->field_10C;
        // fallthrough
    case 0x29:
        if (--*(s32*)((u8*)party + 0x24) <= 0) {
            party->field_3A = 0x2A;
        }
        break;
    case 0x2A:
        party->flags2 = (party->flags2 & ~0x3C0);
        partyPaperOff(party);
        party->flags = (party->flags & ~0x3000);
        party->field_37[0] = 0;
        break;
    default:
        break;
    }
    return ret;
}

// NONMATCHING (95.40%): original keeps the reloaded mario in r29, `fadds` operand order, and the target-Y temp lands in f31 (orig f30).
void party_motion_behind_mario_continue(PartyEntry* party) {
    MarioWork* mario;
    HitEntry* hit;
    PartyEntry* other;
    f32 x, z, y;
    f32 dir;
    f32 dist;
    f32 limit;
    f32 hnz, hny, hnx, hd, hz, hy, hx;
    f32 targetY;

    mario = party->playerPtr;
    if (!(party->flags & 0x8)) {
        dir = toMovedir(mario->unk1A8);
    } else {
        other = partyGetPtr(marioGetPartyId());
        if (other != NULL) {
            dir = partyToMovedir(party, other->field_10C);
        } else {
            dir = toMovedir(mario->unk1A8);
        }
    }
    *(f32*)((u8*)party + 0x54) = revise360(dir + 180.0f);
    if (party->field_8 & 0x2) {
        party->field_8 = (party->field_8 & ~0x2);
        x = mario->position.x;
        z = mario->position.z;
        movePos(&x, &z, 1.5f * mario->wPlayerCollisionBox.x, *(f32*)((u8*)party + 0x54));
        *(s32*)((u8*)party + 0x24) = (s32)distABf(party->position.x, party->position.z, x, z);
        if (*(s32*)((u8*)party + 0x24) <= 0) {
            *(s32*)((u8*)party + 0x24) = 1;
        }
    }
    if (marioKeyOffChk()) {
        mario = party->playerPtr;
        if (!(party->flags & 0x8)) {
            dir = toMovedir(mario->unk1A8);
        } else {
            other = partyGetPtr(marioGetPartyId());
            if (other != NULL) {
                dir = partyToMovedir(party, other->field_10C);
            } else {
                dir = toMovedir(mario->unk1A8);
            }
        }
        *(f32*)((u8*)party + 0x54) = revise360(dir + 180.0f);
    }
    if (!(party->flags & 0x8)) {
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
    movePos(&x, &z, limit, *(f32*)((u8*)party + 0x54));
    angleABf(party->position.x, party->position.z, x, z);
    hd = 1000.0f;
    hit = hitCheckFilter(NULL, &hx, &hy, &hz, &hd, &hnx, &hny, &hnz, x, targetY + touch_wall_ct, z, 0.0f, -1.0f, 0.0f);
    if (hit == NULL) {
        targetY = -2000.0f;
    } else {
        targetY = (f32)(s32)(100.0f * hy + 0.5f) / 100.0f;
        hitGetAttr(hit);
    }
    if (targetY <= -2000.0f) {
        limit = 1.5f * mario->wPlayerCollisionBox.x;
        movePos(&x, &z, limit, toMovedir(mario->unk1A8));
    }
    if (mario->baseSpeed > 1.0f) {
        dist = 10.0f;
    } else {
        dist = 2.0f;
    }
    limit = distABf(party->position.x, party->position.z, x, z);
    *(s32*)((u8*)party + 0x24) = *(s32*)((u8*)party + 0x24) - 1;
    if (limit >= dist) {
        party->dir = angleABf(party->position.x, party->position.z, x, z);
        limit = distABf(party->position.x, party->position.z, x, z) / 10.0f;
        if (limit > 2.0f) {
            limit = 2.0f;
        }
        if (limit < 1.0f) {
            limit = 1.0f;
        }
        unk_800c9398(party, limit);
        partyMoveCheckWall(party);
        unk_800c8f24(party, party->speed);
        hit = partySearchGround(party, 10.0f, 0.0f);
        if (hit != NULL) {
            if (hitGetAttr(hit) & 0x200) {
                hit = NULL;
                if (party->field_4C == 0) {
                    party->field_8 = (party->field_8 | 0x1000000);
                    party->flags = (party->flags | 0x02000000);
                }
            }
        }
        party->field_138 = hit;
        if (hit == NULL) {
            partyChgMot(party, 3);
            party->jumpPara3 = -1.0f;
        }
        party->position.y = party->field_E4;
    } else {
        *(s32*)((u8*)party + 0x24) = 0;
        if (mario->baseSpeed == 0.0f) {
            party->flags = (party->flags | 0x00080000);
            partyChgMoveMode(party, 1);
            partyChgMot(party, 0);
            partySetDir(party, mario->position.x, mario->position.z);
            *(f32*)((u8*)party + 0x40) = 50.0f;
            party->flags2 = (party->flags2 & ~0x10);
        }
    }
    if (party->field_134 != NULL) {
        partyChgMoveMode(party, 1);
        partyChgMot(party, 0);
        partySetDir(party, x, z);
        *(f32*)((u8*)party + 0x40) = 50.0f;
        party->flags2 = (party->flags2 & ~0x10);
    }
}

// NONMATCHING (94.54%): register allocation and the partyGetMoveDirSpd/partySearchFrontWall scratch slots.
void party_motion_behind_mario_fly(PartyEntry* party) {
    MarioWork* mario;
    HitEntry* hit;
    PartyEntry* other;
    f32 speed;
    f32 angle;
    f32 limit;
    f32 f31;
    f32 f29;
    Vec pos;

    mario = party->playerPtr;
    if (party->field_8 & 0x2) {
        party->field_8 = (party->field_8 & ~0x2);
        party->field_8 = (party->field_8 | 0x40000);
        if (!(party->field_8 & 0x100000)) {
            if (!(party->field_8 & 0x10000)) {
                if (!(party->flags & 0x8)) {
                    angle = toMovedir(mario->unk1A8);
                } else {
                    other = partyGetPtr(marioGetPartyId());
                    if (other != NULL) {
                        angle = partyToMovedir(party, other->field_10C);
                    } else {
                        angle = toMovedir(mario->unk1A8);
                    }
                }
                angle = revise360(angle + 180.0f);
            } else {
                party->field_8 = (party->field_8 & ~0x10000);
                angle = revise360(180.0f + angleABf(mario->position.x, mario->position.z, *(f32*)((u8*)party + 0x94), *(f32*)((u8*)party + 0x9C)));
            }
            *(f32*)((u8*)party + 0x54) = angle;
        }
        partyGetAppearPos4(party, (Vec*)((u8*)party + 0x94));
        *(s32*)((u8*)party + 0x24) = (s32)distABf(party->position.x, party->position.z, *(f32*)((u8*)party + 0x94), *(f32*)((u8*)party + 0x9C));
        if (*(s32*)((u8*)party + 0x24) <= 0) {
            *(s32*)((u8*)party + 0x24) = 1;
        }
    }
    if (!(party->field_8 & 0x100000)) {
        if (marioKeyOffChk()) {
            mario = party->playerPtr;
            if (!(party->flags & 0x8)) {
                angle = toMovedir(mario->unk1A8);
            } else {
                other = partyGetPtr(marioGetPartyId());
                if (other != NULL) {
                    angle = partyToMovedir(party, other->field_10C);
                } else {
                    angle = toMovedir(mario->unk1A8);
                }
            }
            *(f32*)((u8*)party + 0x54) = revise360(angle + 180.0f);
        }
    }
    angleABf(party->position.x, party->position.z, *(f32*)((u8*)party + 0x94), *(f32*)((u8*)party + 0x9C));
    if (mario->baseSpeed > 1.0f) {
        limit = 6.0f;
    } else {
        limit = 2.0f;
    }
    f31 = distABf(party->position.x, party->position.z, *(f32*)((u8*)party + 0x94), *(f32*)((u8*)party + 0x9C));
    *(s32*)((u8*)party + 0x24) = *(s32*)((u8*)party + 0x24) - 1;
    if (f31 >= limit || fabsf(*(f32*)((u8*)party + 0x98) - party->position.y) > 2.0f) {
        if (*(s32*)((u8*)party + 0x24) > 0) {
            party->dir = angleABf(party->position.x, party->position.z, *(f32*)((u8*)party + 0x94), *(f32*)((u8*)party + 0x9C));
            distABf(party->position.x, party->position.z, *(f32*)((u8*)party + 0x94), *(f32*)((u8*)party + 0x9C));
            if (party->field_8 & 0x100000) {
                f29 = 3.6f;
            } else {
                f29 = 2.0f;
            }
            if (f31 < limit) {
                f29 = 0.0f;
                unk_800c9398(party, f29);
            }
            unk_800c93d0(party, f29);
            pos = party->position;
            partyGetMoveDirSpd(party, &angle, &speed);
            hit = partySearchFrontWall(party, &pos, speed, angle);
            if (hit != NULL) {
                party->field_12C = hit;
                if (party->speed != 0.0f) {
                    party->field_134 = hit;
                }
            }
            party->position.x = pos.x;
            party->position.z = pos.z;
            party->position.y = 0.05f * (*(f32*)((u8*)party + 0x98) - party->position.y) + party->position.y;
            unk_800c8f24(party, party->speed);
        } else {
            goto restore;
        }
    } else {
    restore:
        *(s32*)((u8*)party + 0x24) = 0;
        if (party->field_8 & 0x200000) {
            party->field_8 = (party->field_8 & ~0x20000);
            if (*(s8*)((u8*)party + 0x32) == 1) {
                party->flags2 = (party->flags2 & ~0x10);
            }
        }
        partyChgMoveMode(party, 1);
        partyChgMot(party, 0);
        partySetDir(party, mario->position.x, mario->position.z);
        *(f32*)((u8*)party + 0x40) = 50.0f;
        party->flags2 = (party->flags2 & ~0x10);
        unk_800c9398(party, 0.0f);
        party->flags = (party->flags | 0x00080000);
        party->field_8 = (party->field_8 & ~0x100000);
        party->field_8 = (party->field_8 & ~0x40000);
        unk_800c8e74(party);
        if (party->field_134 != NULL) {
            party->flags = (party->flags | 0x00080000);
            partyChgMoveMode(party, 1);
            partyChgMot(party, 0);
            partySetDir(party, mario->position.x, mario->position.z);
            *(f32*)((u8*)party + 0x40) = 50.0f;
            party->flags2 = (party->flags2 & ~0x10);
            party->field_8 = (party->field_8 & ~0x40000);
        }
        hit = partySearchGround(party, 100.0f, 100.0f);
        if (hit != NULL) {
            if (hitGetAttr(hit) & 0x200) {
                hit = NULL;
                if (party->field_4C == 0) {
                    party->field_8 = (party->field_8 | 0x1000000);
                    party->flags = (party->flags | 0x02000000);
                }
            }
            if (f31 > *(f32*)((u8*)party + 0xF0)) {
                hit = NULL;
            }
            party->position.y = 0.05f * (party->field_E4 - party->position.y) + party->position.y;
        }
        party->field_138 = hit;
    }
}

void party_motion_stay(PartyEntry* party) {
    extern u8 vec3_802f3e40[];
    u8* vec = vec3_802f3e40;
    MarioWork* mario;
    HitEntry* hit;
    PartyEntry* other;
    u32 id;
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
    if (party->field_8 & 0x2) {
        party->field_8 = (party->field_8 & ~0x2);
        partyChgPoseId(party, 1);
        if (!(party->field_8 & 0x800000)) {
            if (!(party->flags & 0x8)) {
                partyClearFootmark();
            } else {
                partyClearFootmark2();
            }
        }
        party->field_8 = (party->field_8 & ~0x800000);
        party->yoshiPsndSFXId = -1;
        unk_800c9398(party, 0.0f);
    }
    if (mario->currMotionId != 0xe && (party->flags & 0x80000)) {
        party->flags = (party->flags & ~0x80000);
    }
    f31 = *(f32*)((u8*)party + 0x40);
    if (!mario->controlStickSensitivity) {
        if (mario->baseSpeed < 1.0f) {
            f31 = 100.0f;
        }
    }
    if (mario->characterId == 2) {
        f31 = 70.0f;
    }
    if (strcmp(gp->currentMapName, (char*)(vec + 0x188)) == 0 ||
        strcmp(gp->currentMapName, (char*)(vec + 0x190)) == 0 ||
        strcmp(gp->currentMapName, (char*)(vec + 0x198)) == 0) {
        if (!marioKeyOffChk()) {
            if (party->field_32 == 0) {
                f30 = 18.0f;
            } else if ((s8)party->currentMemberId == 5) {
                if (party->field_8 & 0x200000) {
                    f30 = 8.0f;
                } else {
                    f30 = 30.0f;
                }
            } else if ((s8)party->currentMemberId == 6) {
                if (party->field_8 & 0x200000) {
                    f30 = 8.0f;
                } else {
                    f30 = 30.0f;
                }
            }
            f31 = f30;
            party->field_8 = (party->field_8 | 0x20000);
        }
    }
    if ((party->field_8 >> 17) & 1) {
        if (party->field_32 == 0) {
            f29 = 18.0f;
        } else if ((s8)party->currentMemberId == 5) {
            if (party->field_8 & 0x200000) {
                f29 = 8.0f;
            } else {
                f29 = 30.0f;
            }
        } else if ((s8)party->currentMemberId == 6) {
            if (party->field_8 & 0x200000) {
                f29 = 8.0f;
            } else {
                f29 = 30.0f;
            }
        }
        f31 = f29;
    }
    party_slit(party);
    party_roll(party);
    unk_800c9044(party);
    if (mario->currMotionId == 0xe && !(party->flags & 0x80000)) {
        partyMoveBehindMario(party, 0);
        return;
    }
    if (party->field_34 != 0) {
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
            if (flag == 0) {
                goto skip_target;
            }
            if (party->field_32 == 0) {
                targetPos = party->position;
                f29 = partyGetTargetDist3D(id, &targetPos);
                partyGetTargetDist(id, party->position.x, party->position.z);
            } else {
                px = party->position.x;
                pz = party->position.z;
                movePos(&px, &pz, 0.5f * *(f32*)((u8*)party + 0xF4), angleABf(party->position.x, party->position.z, mario->position.x, mario->position.z));
                f29 = distABf(px, pz, mario->position.x, mario->position.z);
            }
            if (marioChkSlitThrouh() || f29 >= f31) {
                if (!marioChkDeepSleep()) {
                    partyChgMot(party, 1);
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
                } else if (other->currentMemberId == 3 && other->field_34 == 3) {
                    if (mario->currMotionId != 0x1f && mario->currMotionId != 0x10 && mario->currMotionId != 0x11) {
                        if (distABf(party->position.x, party->position.z, mario->position.x, mario->position.z) >= f31) {
                            partyChgMot(party, 1);
                        }
                    }
                } else if (!(other->flags & 0x100) || (s8)other->currentMemberId == 4) {
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
skip_target:
    pos = party->position;
    partyGetMoveDirSpd(party, &angle, &speed);
    hit = partySearchFrontWall(party, &pos, speed, angle);
    if (hit != NULL) {
        party->field_12C = hit;
        if (party->speed) {
            party->field_134 = hit;
        }
    }
    party->position.x = pos.x;
    party->position.z = pos.z;
    partyGetMoveDirSpd(party, &angle, &speed);
    marioChkWallAround(&pos, 1, speed, angle, *(f32*)((u8*)party + 0xF4), *(f32*)((u8*)party + 0xF0));
    party->position.x = pos.x;
    party->position.z = pos.z;
    if (party->field_32 == 1) {
        unk_8014b824(party);
        return;
    }
    unk_8014ba3c(party);
    party_force_reset_outofscreen(party);
    if ((s8)party->currentMemberId == 7 && !(party->flags2 & 0x8000) && !marioKeyOffChk() && marioChkDeepSleep() != 1 && chuchu_searchObject()) {
        switch (party->useMotionId) {
        case 1:
            if (--*(s32*)((u8*)party + 0x24) <= 0) {
                partyChgPoseId(party, 1);
                *(s32*)((u8*)party + 0x24) = 0x1e;
                party->useMotionId = 2;
            }
            break;
        case 2:
            if (--*(s32*)((u8*)party + 0x24) <= 0) {
                party->useMotionId = 0;
            }
            break;
#ifdef MUST_MATCH
        // `case 0` is redundant with `default` but is what makes mwcc
        // build the 3-entry {0,1,2} search tree the original has.
        case 0:
#endif
        default:
            partyChgPose(party, (char*)(vec + 0xcc));
            party->useMotionId = 1;
            *(s32*)((u8*)party + 0x24) = 0x1e;
            break;
        }
    }
}

// Retail stages the y probe argument in a named temp with constant propagation off, so
// mwcc schedules the hd = 1000.0f load after the y add (into f0, not f3); the second wall
// probe uses a fresh local `w` so its height lands in f28 rather than f29. Both shapes
// are MUST_MATCH-only.
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
#ifdef MUST_MATCH
    f32 ytmp;
#endif

    mario = party->playerPtr;
    if (party->field_8 & 0x2) {
        party->field_8 = (party->field_8 & ~0x2);
        party->field_8 = (party->field_8 | 0x40000);
        if (!(party->field_8 & 0x100000)) {
            if (!(party->field_8 & 0x10000)) {
                mario2 = party->playerPtr;
                if (!(party->flags & 0x8)) {
                    angle = toMovedir(mario2->unk1A8);
                } else {
                    other = partyGetPtr(marioGetPartyId());
                    if (other != NULL) {
                        angle = partyToMovedir(party, other->field_10C);
                    } else {
                        angle = toMovedir(mario2->unk1A8);
                    }
                }
#ifdef MUST_MATCH
                angle += 180.0f;
                angle = revise360(angle);
#else
                angle = revise360(angle + 180.0f);
#endif
            } else {
                party->field_8 = (party->field_8 & ~0x10000);
                angle = revise360(180.0f + angleABf(mario->position.x, mario->position.z, *(f32*)((u8*)party + 0x94), *(f32*)((u8*)party + 0x9C)));
            }
            *(f32*)((u8*)party + 0x54) = angle;
        }
        partyGetAppearPos4(party, &appear);
        *(s32*)((u8*)party + 0x24) = (s32)distABf(party->position.x, party->position.z, appear.x, appear.z);
        if (*(s32*)((u8*)party + 0x24) <= 0) {
            *(s32*)((u8*)party + 0x24) = 1;
        }
    }
    if (!(party->field_8 & 0x100000)) {
        if (marioKeyOffChk()) {
            mario2 = party->playerPtr;
            if (!(party->flags & 0x8)) {
                angle = toMovedir(mario2->unk1A8);
            } else {
                other = partyGetPtr(marioGetPartyId());
                if (other != NULL) {
                    angle = partyToMovedir(party, other->field_10C);
                } else {
                    angle = toMovedir(mario2->unk1A8);
                }
            }
#ifdef MUST_MATCH
            angle += 180.0f;
            *(f32*)((u8*)party + 0x54) = revise360(angle);
#else
            *(f32*)((u8*)party + 0x54) = revise360(angle + 180.0f);
#endif
        }
    }
    partyGetAppearPos4(party, &appear);
    f30 = appear.y;
    x = appear.x;
    z = appear.z;
    f31 = angleABf(party->position.x, party->position.z, appear.x, appear.z);
#ifdef MUST_MATCH
    ytmp = f30 + touch_wall_ct;
    hd = 1000.0f;
    hit = hitCheckFilter(NULL, &hx, &hy, &hz, &hd, &hnx, &hny, &hnz, x, ytmp, z, 0.0f, -1.0f, 0.0f);
#else
    hd = 1000.0f;
    hit = hitCheckFilter(NULL, &hx, &hy, &hz, &hd, &hnx, &hny, &hnz, x, f30 + touch_wall_ct, z, 0.0f, -1.0f, 0.0f);
#endif
    if (hit == NULL) {
        f29 = -2000.0f;
    } else {
        f29 = (f32)(s32)(100.0f * hy + 0.5f) / 100.0f;
        hitGetAttr(hit);
    }
    if (f29 <= -2000.0f) {
        f29 = 1.5f * mario->wPlayerCollisionBox.x;
        movePos(&x, &z, f29, toMovedir(mario->unk1A8));
    }
    f28 = 2.0f;
    if (mario->baseSpeed > 1.0f) {
        f28 = 10.0f;
    }
    f29 = distABf(party->position.x, party->position.z, x, z);
    *(s32*)((u8*)party + 0x24) = *(s32*)((u8*)party + 0x24) - 1;
    if (f29 >= f28 && *(s32*)((u8*)party + 0x24) > 0) {
        party->dir = angleABf(party->position.x, party->position.z, x, z);
        distABf(party->position.x, party->position.z, x, z);
        unk_800c9398(party, (party->field_8 & 0x100000) ? 3.6f : 2.0f);
        partyMoveCheckWall(party);
        unk_800c8f24(party, party->speed);
        if (party->field_12C != NULL) {
            f32 bhnz, bhny, bhnx, bhd, bhz, bhy, bhx;
            x = party->position.x;
            z = party->position.z;
            movePos(&x, &z, f29, f31);
            angleABf(party->position.x, party->position.z, x, z);
#ifdef MUST_MATCH
            ytmp = f30 + touch_wall_ct;
            bhd = 1000.0f;
            hit = hitCheckFilter(NULL, &bhx, &bhy, &bhz, &bhd, &bhnx, &bhny, &bhnz, x, ytmp, z, 0.0f, -1.0f, 0.0f);
#else
            bhd = 1000.0f;
            hit = hitCheckFilter(NULL, &bhx, &bhy, &bhz, &bhd, &bhnx, &bhny, &bhnz, x, f30 + touch_wall_ct, z, 0.0f, -1.0f, 0.0f);
#endif
            if (hit == NULL) {
                fWall = -2000.0f;
            } else {
                fWall = (f32)(s32)(100.0f * bhy + 0.5f) / 100.0f;
                hitGetAttr(hit);
            }
            if (fabsf(party->position.y - fWall) > 1.0f) {
                *(f32*)((u8*)party + 0x94) = x;
                *(f32*)((u8*)party + 0x98) = fWall;
                *(f32*)((u8*)party + 0x9C) = z;
                partyChgMot(party, 2);
                return;
            }
        } else if (f30 > party->position.y) {
#ifdef MUST_MATCH
            f32 chnz, chny, chnx, chd, chz, chy, chx, w;
#else
            f32 chnz, chny, chnx, chd, chz, chy, chx;
#endif
            x = party->position.x;
            z = party->position.z;
            movePos(&x, &z, f29, f31);
            angleABf(party->position.x, party->position.z, x, z);
#ifdef MUST_MATCH
            ytmp = f30 + touch_wall_ct;
            chd = 1000.0f;
            hit = hitCheckFilter(NULL, &chx, &chy, &chz, &chd, &chnx, &chny, &chnz, x, ytmp, z, 0.0f, -1.0f, 0.0f);
            if (hit == NULL) {
                w = -2000.0f;
            } else {
                w = (f32)(s32)(100.0f * chy + 0.5f) / 100.0f;
                hitGetAttr(hit);
            }
            if (fabsf(party->position.y - w) > 1.0f) {
                *(f32*)((u8*)party + 0x94) = x;
                *(f32*)((u8*)party + 0x98) = w;
                *(f32*)((u8*)party + 0x9C) = z;
                partyChgMot(party, 2);
                return;
            }
#else
            chd = 1000.0f;
            hit = hitCheckFilter(NULL, &chx, &chy, &chz, &chd, &chnx, &chny, &chnz, x, f30 + touch_wall_ct, z, 0.0f, -1.0f, 0.0f);
            if (hit == NULL) {
                f28 = -2000.0f;
            } else {
                f28 = (f32)(s32)(100.0f * chy + 0.5f) / 100.0f;
                hitGetAttr(hit);
            }
            if (fabsf(party->position.y - f28) > 1.0f) {
                *(f32*)((u8*)party + 0x94) = x;
                *(f32*)((u8*)party + 0x98) = f28;
                *(f32*)((u8*)party + 0x9C) = z;
                partyChgMot(party, 2);
                return;
            }
#endif
        }
        hit = partySearchGround(party, 10.0f, 0.0f);
        if (hit != NULL) {
            if (hitGetAttr(hit) & 0x200) {
                hit = NULL;
                if (party->field_4C == 0) {
                    party->field_8 = (party->field_8 | 0x1000000);
                    party->flags = (party->flags | 0x02000000);
                }
            }
        }
        party->field_138 = hit;
        if (hit == NULL) {
            partyChgMot(party, 3);
            party->jumpPara3 = -1.0f;
        }
        party->position.y = party->field_E4;
    } else {
        *(s32*)((u8*)party + 0x24) = 0;
        if (party->field_8 & 0x200000) {
            party->field_8 = (party->field_8 & ~0x20000);
        }
        partyChgMoveMode(party, 1);
        partyChgMot(party, 0);
        partySetDir(party, mario->position.x, mario->position.z);
        *(f32*)((u8*)party + 0x40) = 50.0f;
        party->flags2 = (party->flags2 & ~0x10);
        party->flags = (party->flags | 0x00080000);
        party->field_8 = (party->field_8 & ~0x100000);
        party->field_8 = (party->field_8 & ~0x40000);
    }
    if (party->field_134 != NULL) {
        party->flags = (party->flags | 0x00080000);
        partyChgMoveMode(party, 1);
        partyChgMot(party, 0);
        partySetDir(party, x, z);
        *(f32*)((u8*)party + 0x40) = 50.0f;
        party->flags2 = (party->flags2 & ~0x10);
        party->field_8 = (party->field_8 & ~0x40000);
    }
}
#pragma opt_propagation reset

// NONMATCHING (81.24%): original copies the appear position through a scratch Vec seeded from vec3_802f3e40+0x18 (a compiler struct-assignment idiom) which this elides; frame size differs.
void party_motion_jump(PartyEntry* party) {
    MarioWork* mario;
    HitEntry* hit;
    Vec floor;
    Vec wall;
    f32 tx, ty, tz;
    f32 sinV, cosV;
    f32 angle;
    f32 f31, f30;
    f32 f0, f1, f3, f4, f5, f6;
    s32 id;
    s32 writeId;

    mario = party->playerPtr;
    id = partyGetFootmarkId(&party->flags);
    if (party->field_8 & 0x2) {
        party->field_8 = (party->field_8 & ~0x2);
        *(u8*)((u8*)party + 0x38) = 0;
    }
    switch (*(u8*)((u8*)party + 0x38)) {
    case 0:
        tx = *(f32*)((u8*)party + 0x94);
        ty = *(f32*)((u8*)party + 0x98);
        tz = *(f32*)((u8*)party + 0x9C);
        if (party->field_28 != 0 || party->field_34 == 9) {
            tx = party->position.x;
            ty = party->position.y;
            tz = party->position.z;
            movePos(&tx, &tz, party->speed, party->dir);
            *(f32*)((u8*)party + 0x94) = tx;
            *(f32*)((u8*)party + 0x98) = ty;
            *(f32*)((u8*)party + 0x9C) = tz;
        } else if (!(party->field_8 & 0x400000)) {
            writeId = partyGetFootmarkWriteId(id);
            if (partyGetFootmarkReadId(id) == writeId) {
                if (partyGetTargetDist(id, party->position.x, party->position.z) > 2.0f * mario->wPlayerCollisionBox.x) {
                    f30 = partyGetTargetDir(id, party->position.x, party->position.z);
                    getFrontFloor(party, &floor, f30);
                    ty = floor.y + 10.0f;
                    tx = party->position.x;
                    tz = party->position.z;
                    movePos(&tx, &tz, 0.6f * *(f32*)((u8*)party + 0xF4), f30);
                }
            } else {
                partyGetReadId(id, &tx, &ty, &tz);
                f30 = 50.0f;
                f31 = distABf(party->position.x, party->position.z, tx, tz);
                f0 = 1.4f * (ty - party->position.y);
                if (f0 < 50.0f) {
                    f30 = f0;
                }
                angle = angleABf(party->position.x, party->position.z, tx, tz);
                sincosf(&sinV, &cosV, angle);
                if (f31 > f30) {
                    f31 = f30;
                    if (partyGetTargetDistY(id) == 0.0f) {
                        f31 = 0.0f;
                    }
                    tx = party->position.x + f31 * sinV;
                    tz = party->position.z + f31 * cosV;
                    ty = ty + 10.0f;
                    *(f32*)((u8*)party + 0x94) = tx;
                    *(f32*)((u8*)party + 0x98) = ty;
                    *(f32*)((u8*)party + 0x9C) = tz;
                } else {
                    if (partyGetTargetDistY(id) == 0.0f) {
                        f31 = 0.0f;
                    }
                    tx = party->position.x + f31 * sinV;
                    tz = party->position.z + f31 * cosV;
                    *(f32*)((u8*)party + 0x94) = tx;
                    *(f32*)((u8*)party + 0x98) = ty;
                    *(f32*)((u8*)party + 0x9C) = tz;
                }
                if (f31 < f30) {
                    partyNextReadId(id);
                }
            }
        }
        *(f32*)((u8*)party + 0x128) = distABf(party->position.x, party->position.z, *(f32*)((u8*)party + 0x94), *(f32*)((u8*)party + 0x9C));
        if (party->field_28 == 0) {
            party->dir = angleABf(party->position.x, party->position.z, *(f32*)((u8*)party + 0x94), *(f32*)((u8*)party + 0x9C));
            party->speed = 2.25f * mario->playerGravity;
        }
        f5 = *(f32*)((u8*)party + 0x128);
        f1 = 0.9f * (f5 / party->speed);
        f0 = (10.0f * f1 + 0.5f) / 10.0f;
        *(s32*)((u8*)party + 0x24) = (s32)f0;
        if (*(s32*)((u8*)party + 0x24) < 0xc) {
            *(s32*)((u8*)party + 0x24) = 0xc;
        }
        if (*(s32*)((u8*)party + 0x24) > 0x28) {
            *(s32*)((u8*)party + 0x24) = 0x28;
        }
        f4 = 0.5f * (f32)*(s32*)((u8*)party + 0x24);
        f6 = *(f32*)((u8*)party + 0x98) - party->position.y;
        if (fabsf(f6) < 1.0f) {
            f6 = 25.0f;
        } else if (f6 < 0.0f) {
            f6 = 10.0f;
        }
        f3 = (f6 + f6) / (f4 * f4 - f4);
        party->jumpPara1 = mario->playerGravity * (f3 * f4);
        if (party->jumpPara1 > 13.0f) {
            party->jumpPara1 = 13.0f;
        }
        if (party->jumpPara1 < 3.0f) {
            party->jumpPara1 = 3.0f;
        }
        party->jumpPara3 = -f3 * mario->playerGravity;
        party->flags = (party->flags | 0x10);
        party->flags = (party->flags & ~0x2);
        partyChgPoseId(party, 4);
        psndSFXOn_3D((const char*)0x927, &party->position);
        *(u8*)((u8*)party + 0x38) = 0xa;
        if (!(party->flags & 0x8)) {
            partyNextReadId(partyGetFootmarkId(&party->flags));
        }
        break;
    case 10:
        if (!(party->flags & 0x8)) {
            partyNextReadId(partyGetFootmarkId(&party->flags));
        }
        party->jumpPara1 = party->jumpPara1 + party->jumpPara3;
        party->position.y = party->position.y + party->jumpPara1;
        if (party->position.y < -2000.0f) {
            party->flags = (party->flags & ~0x10);
            party->jumpPara1 = 0.0f;
            party->dir = angleABf(party->position.x, party->position.z, mario->position.x, mario->position.z);
            party->position = mario->position;
            partyChgMot(party, 0);
            break;
        }
        if (party->jumpPara1 <= 0.0f) {
            party->flags = (party->flags & ~0x10);
            party->flags = (party->flags | 0x20);
            hit = partySearchGround(party, fabsf(party->jumpPara1), 0.0f);
            if (hit != NULL && (hitGetAttr(hit) & 0x200)) {
                if (party->field_4C == 0) {
                    party->field_8 = (party->field_8 | 0x1000000);
                    party->flags = (party->flags | 0x02000000);
                }
                party->field_138 = NULL;
                partyChgMot(party, 3);
                party->jumpPara3 = -1.0f;
                break;
            }
            party->field_138 = hit;
            party->position.y = party->field_E4;
            if (hit != NULL) {
                party->flags = (party->flags | 0x2);
                party->flags = (party->flags & ~0x70);
                N_partyPlaySfxPartyLanding1(party);
                party->jumpPara1 = 0.0f;
                partyChgMot(party, 0);
                party->field_8 = (party->field_8 | 0x00800000);
                break;
            }
        }
        if (party->flags & 0x01000000) {
            movePos(&wall.x, &wall.z, party->speed, party->dir);
        } else {
            wall = party->position;
            hit = partySearchFrontWall(party, &wall, party->speed, party->dir);
            if (hit != NULL) {
                party->field_12C = hit;
                if (party->speed != 0.0f) {
                    party->field_134 = hit;
                }
            }
            party->position.x = wall.x;
            party->position.z = wall.z;
        }
        break;
    default:
        break;
    }
}

// Inline square root used by the original (same expansion as party_hit.c).
#define PT_FP_NAN 1
#define PT_FP_INFINITE 2
#define PT_FP_ZERO 3
#define PT_FP_NORMAL 4
#define PT_FP_SUBNORMAL 5
extern int __float_nan[];
#define PT_NAN (*(f32*)__float_nan)

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

#define pt_fpclassify(x) pt_fpclassifyf(x)
#define pt_isnan(x) (pt_fpclassify(x) == PT_FP_NAN)

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

// NONMATCHING (81.01%): the two HitCheckQuery locals and two sincos pairs get the opposite stack slots from the original; sqrtf doubles load from local symbols instead of vec3_802f3e40+0x108.
s32 getFrontFloor(PartyEntry* party, Vec* pos, f32 angle) {
    extern u8 vec3_802f3e40[];
    u8* vec = vec3_802f3e40;
    HitCheckQuery query1;
    HitCheckQuery query2;
    f32 sin1, cos1;
    f32 sin2, cos2;
    f32 floorY;
    f32 scale;
    f32 h;
    s32 i;
    HitEntry* hit;

    query1.targetDir = *(Vec*)(vec + 0x60);
    scale = 2.0f * *(f32*)((u8*)party + 0xF4) / 3.0f;
    sincosf(&sin1, &cos1, angle);
    floorY = -2000.0f;
    for (i = 0; i < 3; i++) {
        f32 fi = (f32)i;
        query1.targetPos.x = party->position.x + fi * (sin1 * scale);
        query1.targetPos.y = party->position.y + *(f32*)((u8*)party + 0xF0);
        query1.targetPos.z = party->position.z + fi * (cos1 * scale);
        query1.targetDistance = 100.0f;
        hit = hitCheckVecFilter(&query1, NULL);
        if (hit != NULL) {
            if (pt_sqrtf(query1.hitNormal.x * query1.hitNormal.x + query1.hitNormal.z * query1.hitNormal.z) < 45.0f) {
                floorY = query1.hitPos.y;
                break;
            }
        }
    }
    if (floorY > -2000.0f) {
        f32 a = party->position.y + touch_wall_ct;
        f32 b = floorY - 2.0f;
        query2.targetPos.y = (a < b) ? a : b;
    } else {
        query2.targetPos.y = party->position.y + touch_wall_ct;
    }
    h = 0.75f * *(f32*)((u8*)party + 0xF4);
    sincosf(&sin2, &cos2, angle);
    query2.targetPos.x = party->position.x + sin2 * h;
    query2.targetPos.z = party->position.z + cos2 * h;
    query2.targetDir = *(Vec*)(vec + 0x6C);
    query2.targetDistance = 300.0f;
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

// NONMATCHING (91.85%): original reloads playerPtr into r29 inside the first `if`, `fadds` operand order, and 1.5f*collbox.x scheduling.
void party_motion_beside_mario(PartyEntry* party) {
    MarioWork* mario;
    HitEntry* hit;
    PartyEntry* other;
    f32 x, z, y;
    f32 dir;
    f32 dist;
    f32 limit;
    f32 hnz, hny, hnx, hd, hz, hy, hx;

    mario = party->playerPtr;
    if (party->field_8 & 0x2) {
        party->field_8 = (party->field_8 & ~0x2);
        mario = party->playerPtr;
        if (!(party->flags & 0x8)) {
            dir = toMovedir(mario->unk1A8);
        } else {
            other = partyGetPtr(marioGetPartyId());
            if (other != NULL) {
                dir = partyToMovedir(party, other->field_10C);
            } else {
                dir = toMovedir(mario->unk1A8);
            }
        }
        *(f32*)((u8*)party + 0x54) = revise360(dir + 90.0f);
        x = mario->position.x;
        z = mario->position.z;
        movePos(&x, &z, 1.5f * mario->wPlayerCollisionBox.x, *(f32*)((u8*)party + 0x54));
        *(s32*)((u8*)party + 0x24) = (s32)distABf(party->position.x, party->position.z, x, z);
        if (*(s32*)((u8*)party + 0x24) <= 0) {
            *(s32*)((u8*)party + 0x24) = 1;
        }
    }
    if (marioKeyOffChk()) {
        mario = party->playerPtr;
        if (!(party->flags & 0x8)) {
            dir = toMovedir(mario->unk1A8);
        } else {
            other = partyGetPtr(marioGetPartyId());
            if (other != NULL) {
                dir = partyToMovedir(party, other->field_10C);
            } else {
                dir = toMovedir(mario->unk1A8);
            }
        }
        *(f32*)((u8*)party + 0x54) = revise360(dir + 90.0f);
    }
    x = mario->position.x;
    z = mario->position.z;
    y = mario->position.y;
    movePos(&x, &z, 1.5f * mario->wPlayerCollisionBox.x, *(f32*)((u8*)party + 0x54));
    angleABf(party->position.x, party->position.z, x, z);
    hd = 1000.0f;
    hit = hitCheckFilter(NULL, &hx, &hy, &hz, &hd, &hnx, &hny, &hnz, x, y + touch_wall_ct, z, 0.0f, -1.0f, 0.0f);
    if (hit == NULL) {
        dist = -2000.0f;
    } else {
        dist = (f32)(s32)(100.0f * hy + 0.5f) / 100.0f;
        hitGetAttr(hit);
    }
    if (dist <= -2000.0f) {
        x = mario->position.x;
        z = mario->position.z;
        dist = 1.5f * mario->wPlayerCollisionBox.x;
        movePos(&x, &z, dist, toMovedir(mario->unk1A8));
    }
    if (mario->baseSpeed > 1.0f) {
        limit = 10.0f;
    } else {
        limit = 2.0f;
    }
    dist = distABf(party->position.x, party->position.z, x, z);
    (*(s32*)((u8*)party + 0x24)) = (*(s32*)((u8*)party + 0x24)) - 1;
    if (dist >= limit && (*(s32*)((u8*)party + 0x24)) > 0) {
        party->dir = angleABf(party->position.x, party->position.z, x, z);
        dist = distABf(party->position.x, party->position.z, x, z) / 10.0f;
        if (dist > 2.0f) {
            dist = 2.0f;
        }
        if (dist < 1.0f) {
            dist = 1.0f;
        }
        unk_800c9398(party, dist);
        partyMoveCheckWall(party);
        unk_800c8f24(party, party->speed);
        hit = partySearchGround(party, 10.0f, 0.0f);
        if (hit != NULL) {
            if (hitGetAttr(hit) & 0x200) {
                hit = NULL;
                if (party->field_4C == 0) {
                    party->field_8 = (party->field_8 | 0x1000000);
                    party->flags = (party->flags | 0x02000000);
                }
            }
        }
        party->field_138 = hit;
        if (hit == NULL) {
            partyChgMot(party, 3);
            party->jumpPara3 = -1.0f;
        }
        party->position.y = party->field_E4;
    } else {
        (*(s32*)((u8*)party + 0x24)) = 0;
        partyChgMoveMode(party, 1);
        partyChgMot(party, 0);
        partySetDir(party, mario->position.x, mario->position.z);
        *(f32*)((u8*)party + 0x40) = 50.0f;
        party->flags2 = (party->flags2 & ~0x10);
        party->flags = (party->flags | 0x00080000);
    }
    if (party->field_134 != NULL) {
        partyChgMoveMode(party, 1);
        partyChgMot(party, 0);
        partySetDir(party, mario->position.x, mario->position.z);
        *(f32*)((u8*)party + 0x40) = 50.0f;
        party->flags2 = (party->flags2 & ~0x10);
    }
}

// Register rotation vec/ret/party (orig r31/r30/r29, mine r29/r31/r30) and the
// `next = 0.1f * (target - cur) + cur` block (orig emits fsubs/fmuls/fadds, mine
// emits fmadds) remain. Duplicating the target computation into each arm of the
// `dir < 180` test fixed most of the block-level differences.
// NONMATCHING (86.22%): see above.
s32 party_roll(PartyEntry* party) {
    extern u8 vec3_802f3e40[];
    u8* vec = vec3_802f3e40;
    s32 ret = 1;
    f32 target;
    f32 cur;
    f32 next;
    f32 cond;

    if (party->field_37[0] != 2) {
        return 1;
    }
    if (party->field_8 & 0x1) {
        party->field_8 = (party->field_8 & ~0x1);
        partyPaperOn(party, (const char*)(vec + 0x128));
        partyChgPaper(party, (void*)(vec + 0x130));
        partyChgPoseId(party, 0);
        *(f32*)((u8*)party + 0x88) = -6.0f;
        *(f32*)((u8*)party + 0x8C) = 6.0f;
        *(f32*)((u8*)party + 0x7C) = 6.0f;
        *(f32*)((u8*)party + 0x80) = 2.0f;
        *(s32*)((u8*)party + 0x24) = 0x41;
        party->field_3A = 0;
        unk_800c9398(party, 0.0f);
    }
    switch (party->field_3A) {
    case 0:
        *(s32*)((u8*)party + 0x24) = *(s32*)((u8*)party + 0x24) - 1;
        if (*(s32*)((u8*)party + 0x24) <= 0) {
            partyChgPaper(party, (void*)(vec + 0x138));
            party->field_3A = 0xA;
        }
        break;
    case 0xA:
        party->field_10C = revise360(270.0f - party->dir);
        if (revise360(party->field_FC + party->field_10C) > 180.0f) {
            *(f32*)((u8*)party + 0x6C) = revise360(7.0f * party->speed + *(f32*)((u8*)party + 0x6C));
        } else {
            *(f32*)((u8*)party + 0x6C) = revise360(*(f32*)((u8*)party + 0x6C) - 7.0f * party->speed);
        }
        break;
    }
    switch (party->field_3A) {
    case 0x64:
        party->speed = 0.0f;
        *(f32*)((u8*)party + 0x64) = 0.0f;
        *(f32*)((u8*)party + 0x6C) = 0.0f;
        *(f32*)((u8*)party + 0x88) = 0.0f;
        *(f32*)((u8*)party + 0x8C) = 0.0f;
        if (party->dir < 180.0f) {
            *(f32*)((u8*)party + 0x7C) = 3.0f;
        } else {
            *(f32*)((u8*)party + 0x7C) = -3.0f;
        }
        *(f32*)((u8*)party + 0x80) = 0.0f;
        *(s32*)((u8*)party + 0x24) = 0x41;
        party->field_3A = 0x65;
        partyChgPaper(party, (void*)(vec + 0x140));
        partyPaperLightOff(party);
        // fallthrough
    case 0x65:
        ret = 0;
        *(s32*)((u8*)party + 0x24) = *(s32*)((u8*)party + 0x24) - 1;
        if (*(s32*)((u8*)party + 0x24) > 0) {
            if (party->dir < 180.0f) {
                target = 90.0f;
                cur = party->field_10C;
                next = 0.1f * (target - cur) + cur;
                cond = (target - next < 1.0f);
                if (fabsf(cond) != 0.0f) {
                    next = target;
                }
                party->field_10C = next;
                party->field_110 = next;
            } else {
                target = 270.0f;
                cur = party->field_10C;
                next = 0.1f * (target - cur) + cur;
                cond = (target - next < 1.0f);
                if (fabsf(cond) != 0.0f) {
                    next = target;
                }
                party->field_10C = next;
                party->field_110 = next;
            }
            break;
        }
        ret = 1;
        if (party->dir >= 180.0f) {
            party->field_10C = 0.0f;
            party->field_110 = 0.0f;
            party->field_FC = 270.0f;
            party->dir = 270.0f;
        } else {
            party->field_10C = 180.0f;
            party->field_110 = 180.0f;
            party->field_FC = 90.0f;
            party->dir = 90.0f;
        }
        {
            u8* p = vec3_802f3e40;
            *(u32*)((u8*)party + 0x88) = *(u32*)(p + 0x48);
            *(u32*)((u8*)party + 0x8C) = *(u32*)(p + 0x4C);
            *(u32*)((u8*)party + 0x90) = *(u32*)(p + 0x50);
            *(u32*)((u8*)party + 0x7C) = *(u32*)(p + 0x54);
            *(u32*)((u8*)party + 0x80) = *(u32*)(p + 0x58);
            *(u32*)((u8*)party + 0x84) = *(u32*)(p + 0x5C);
        }
        party->field_37[0] = 0;
        party->flags = (party->flags & ~0x1000);
        party->flags2 = (party->flags2 & ~0x100);
        party->flags = (party->flags & ~0x200);
        party->flags2 = (party->flags2 & ~0x10);
        party->flags2 = (party->flags2 & ~0x8);
        party->field_3A = 0x64;
        partyPaperOff(party);
        break;
    }
    return ret;
}
