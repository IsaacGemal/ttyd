#include "pmario_sound.h"
#include "party.h"
#include "mario/mario.h"
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

// .sbss
f32 unk_804189e8;
s32 unk_804189ec;
f32 touch_wall_ct;

// Local declarations: not yet in a header.
void psndSFXAllOff(void);
s32 psndSFXOn_3D(const char* name, Vec* position);
BOOL psndENVOff(u32 r3);
void psndSetReverb(s32 mode);
s32 psndENV_LPF(s32 id, u16 freq);
s32 psndENVOff_f_d(s32 id, s32 value, s32 flag);
s32 psndENVOn_f_d(s32 id, s32 a, s32 b, s32 c);
s32 psndSFX_dist(s32 id, u16 dist);
void psndSFX_pos(s32 id, Vec* position);
void psndSFX_vol(s32 id, u8 vol);
void psndSFX_pit(s32 id, s16 pitch);
s32 psndSFXChk(u32 id);
s32 psndSFXOnEx_3D(const char* name, Vec* position, u8 vol, u8 pan, u16 pitch, u16 flags);
s32 psndSFXOn(const char* name);
void psndBGMScope(s32 id, s32 scope);
void psndBGM_rate(s32 id, u16 rate);
BOOL psndBGMOff(s32 flags);
BOOL psndENVOn(s32 a, s32 b);
s32 psndBGMOn_f_d(s32 a, const char* b, s32 c, s32 d, s32 e);
s32 unk_800d7270(s32 a, char* b, s32 c, s32 d);
BOOL psndBGMOn(u32 r3, char* bgm_name);
void partyPaperOff(PartyEntry* party);
void partyChgPaper(PartyEntry* party, void* paper);
void partyPaperLightOff(PartyEntry* party);
char* partyChgPoseId(PartyEntry* party, s32 poseId);
void partyChgMot(PartyEntry* party, s32 mot);
HitEntry* partySearchGround(PartyEntry* party, f32 a, f32 b);
HitEntry* partySearchHead(PartyEntry* party, Vec* position, f32* out, f32 angle);
void partyChgRunMode(PartyEntry* party, s32 mode);
u32 partyGetFootmarkId(u32* flags);
void partyNextReadId(u32 index);
void partyGetAppearPos4(PartyEntry* party, Vec* position);
void partySearchFrontWall(PartyEntry* party, Vec* position, f32 a, f32 b);
void N_partyPlaySfxPartyLanding1(PartyEntry* party);
void partyGetAppearPos5(PartyEntry* party, HitEntry* hit, Vec* position);
s32 unk_800c8dd0(HitEntry* hit);
void partyClearFootmark(void);
void partyClearFootmark2(void);
void partyChkWall(PartyEntry* party);
void partyChkGnd(PartyEntry* party);
void unk_800c9044(PartyEntry* party);
void partyMoveBehindMario(PartyEntry* party, s32 arg);
void partyChgPose(PartyEntry* party, const char* name);
void party_slit(PartyEntry* party);
s32 party_roll(PartyEntry* party);
void unk_8014b824(PartyEntry* party);
s32 marioChkDeepSleep(void);
s32 chuchu_searchObject(void);
void partyGetReturnPos(Vec* position);
void partyMove(PartyEntry* party);
void unk_800c9398(PartyEntry* party, f32 value);

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

// NONMATCHING (92.24%): register allocation differs - original keeps party/attr/hit in r29/r30/r31,
// mine only r30/r31 (attr not held in a callee-saved reg).
void unk_8014ba3c(PartyEntry* party) {
    Vec position = party->position;
    f32 angle = *(f32*)((u8*)party + 0xF0);
    HitEntry* hit;
    s32 attr;

    hit = partySearchHead(party, &position, &angle, party->field_10C);
    if (hit != NULL) {
        *(HitEntry**)((u8*)party + 0x140) = hit;
    }
    if (party->flags & 0x04000000) {
        return;
    }
    party->flags = (party->flags & ~0x80);
    hit = partySearchGround(party, 15.0f, -15.0f);
    attr = 0;
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
        *(u8*)((u8*)party + 0x37) = 1;
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
            *(u8*)((u8*)party + 0x37) = 1;
        }
    }
    party = partyGetPtr(marioGetExtraPartyId());
    if (party != NULL) {
        if ((party->playerPtr->trigFlags & 0x80000000) == 0) {
            party->field_8 = (party->field_8 | 1);
            *(u8*)((u8*)party + 0x37) = 1;
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
        *(u8*)((u8*)party + 0x37) = 0;
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
        *(u8*)((u8*)party + 0x37) = 0;
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
        *(u8*)((u8*)party + 0x37) = 0;
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

// NONMATCHING (98.98%): switch dispatch tree compares cases in the other order (orig 1 then 3).
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

// NONMATCHING (96.44%): hitGetAttr boolean materialisation uses a callee-saved reg in mine;
// original keeps it in r0. Everything else matches.
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
            s32 hitFlag = 0;
            if (*(HitEntry**)((u8*)party + 0x138) != NULL) {
                if ((hitGetAttr(*(HitEntry**)((u8*)party + 0x138)) & 0x200) == 0) {
                    hitFlag = 1;
                }
            }
            if (hitFlag != 0) {
                *(u8*)((u8*)party + 0x38) = 0xC;
                *(s32*)((u8*)party + 0x24) = 0x1E;
            }
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

// NONMATCHING (86.63%): register allocation (orig party/mario/hit = r31/r29/r30),
// flag-condition branch shape, and one fsubs dest register differ.
void unk_8014b824(PartyEntry* party) {
    HitEntry* hit;
    MarioWork* mario = party->playerPtr;
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
    if (mario->airTimer == 0 || mario->baseSpeed != 0.0f) {
        if (mario->currMotionId == 0x1F || mario->currMotionId == 0x20
            || mario->currMotionId == 0x10 || mario->currMotionId == 0x11) {
            flag = 0;
        } else {
            flag = 1;
        }
    } else {
        flag = 0;
    }
    if (flag != 0) {
        f1 = party->field_E4;
        f3 = 0.05f;
        if (hit != NULL) {
            f3 = 0.1f;
            if (mario->position.y > f1) {
                f2 = mario->position.y;
            } else {
                f2 = f1;
            }
            if (party->position.y < f1) {
                f2 = 18.5f + f1;
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
        f1 = party->field_E4;
        if (hit != NULL) {
            f2 = mario->position.y;
            if (mario->position.y <= f1) {
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
        if (f2 > f0 || 0.0f > f2) {
            party->position.y += f2 * 0.05f;
        }
    }
}

// NONMATCHING (98.50%): remaining diff is float register allocation in the
// ratio/diff/frames block (orig ratio in f1, mine f2) and one mr r3,r30 position.
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
        party->speed = mario->playerGravity * (d / ff);
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
                d = distABf(party->position.x, party->position.z, *(f32*)((u8*)party + 0x94), *(f32*)((u8*)party + 0x9C));
                partyChgMot(party, (d < 5.0f) ? 0 : 1);
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

// NONMATCHING (77.76%): register rotation vec/ret/party (orig r31/r30/r29) and a few
// constant-register/scheduling differences remain.
s32 party_roll(PartyEntry* party) {
    extern u8 vec3_802f3e40[];
    s32 ret = 1;
    u8* vec = vec3_802f3e40;
    f32 target;
    f32 cur;
    f32 next;
    f32 cond;

    if (*(u8*)((u8*)party + 0x37) != 2) {
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
            } else {
                target = 270.0f;
            }
            cur = party->field_10C;
            next = 0.1f * (target - cur) + cur;
            cond = (target - next < 1.0f);
            if (fabsf(cond) != 0.0f) {
                next = target;
            }
            party->field_10C = next;
            party->field_110 = next;
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
        *(u8*)((u8*)party + 0x37) = 0;
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
