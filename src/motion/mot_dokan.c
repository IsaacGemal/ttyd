#include "dolphin/types.h"
#include "dolphin/mtx.h"
#include "mario/mario.h"
#include "mario/mario_motion.h"
#include "mario/mario_party.h"
#include "mario/mario_sbr.h"
#include "party.h"
#include "matching.h"
#include "mario/mario_cam.h"
#include "motion/mot_slit.h"
#include "motion/mot_dokan.h"

// Local prototypes for functions whose headers don't declare them yet. Listed in REPORT.md.

// Raw accessors for MarioWork and PartyEntry fields that are not named in their headers.
#define M_S8(m, off) (*(s8*)((unsigned char*)(m) + (off)))
#define M_F32(m, off) (*(f32*)((unsigned char*)(m) + (off)))
#define M_VEC(m, off) (*(Vec*)((unsigned char*)(m) + (off)))

// .rodata named constants, in the order they appear in the original.
const Vec vec3_802f3d60 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3d6c = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3d78 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3d84 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3d90 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3d9c = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3da8 = { 0.0f, 16.0f, 0.0f };
const Vec vec3_802f3db4 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3dc0 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3dcc = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3dd8 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3de4 = { 0.0f, 16.0f, 0.0f };
const Vec vec3_802f3df0 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3dfc = { 1.0f, 1.0f, 1.0f };
const Vec vec3_802f3e08 = { 1.0f, 1.0f, 1.0f };
const char str_802F3E14[] = "p_dokan_y";
const char str_802F3E20[] = "PM_D_1A";
const char str_802F3E28[] = "p_dokan_x";
const char str_802F3E34[] = "PM_D_1B";

// .sdata2
const f32 float_0_8041cd80 = 0.0f;
const char str_8041CD84[] = "M_Z_1";
const f32 float_30_8041cd8c = 30.0f;
const f32 float_neg0p005_8041cd90 = -0.005f;
const f32 float_neg2_8041cd94 = -2.0f;
const f32 float_0p03_8041cd98 = 0.03f;
const f32 float_1_8041cd9c = 1.0f;
const f32 float_0p05_8041cda0 = 0.05f;
const f32 float_neg0p02_8041cda4 = -0.02f;
const f32 float_16_8041cda8 = 16.0f;
const f32 float_neg3_8041cdac = -3.0f;
const f32 float_540_8041cdb0 = 540.0f;
const f32 float_180_8041cdb4 = 180.0f;
const f32 float_120_8041cdb8 = 120.0f;
const f32 float_neg120_8041cdbc = -120.0f;

// The original emits clean load/load/store/store/load/store per Vec copy; MWCC here hoists the third
// word load ahead of the second store (same artifact as mot_roll/mot_damage, see mot_roll.c note). The
// float mismatches are anonymous @NNN .sdata2 slots because the constants are const-defined in this TU;
// declaring them extern keeps the names but changes register allocation and scores lower (94.20%).
// NONMATCHING (94.68%): instruction scheduling + .sdata2 float symbol names.
void mot_dokan(void) {
    MarioWork* mario;
    PartyEntry* party;

    mario = marioGetPtr();
    if (mario->trigFlags & 1) {
        mario->trigFlags &= ~1;
        mario->dispFlags |= 0x1000;
        mario->dispFlags |= 8;
        mario->dispFlags |= 0x100;
        M_F32(mario, 0x2A4) = float_0_8041cd80;
        mario->wCamVal1 = mario->position.z;
        mario->multiTimer = 0;
        mario->airTimer = 0;
        mario->baseSpeed = float_0_8041cd80;
        if (M_S8(mario, 0x3E) == 2) {
            mario->currSubMotionId = 0xA;
        } else if (M_S8(mario, 0x3E) == 3) {
            mario->currSubMotionId = 0xA;
        } else if (M_S8(mario, 0x3E) == 1) {
            marioPaperOn(str_802F3E14);
            marioChgPaper(str_802F3E20);
            marioChgPose(str_8041CD84);
            marioPaperLightOff();
            mario->dispFlags |= 8;
            mario->dispFlags |= 0x100;
            mario->unk1A8 = M_F32(mario, 0x1AC);
            mario->currSubMotionId = 0x14;
            M_F32(mario, 0x2A0) = float_0_8041cd80;
            M_F32(mario, 0x2A0) = float_30_8041cd8c;
            mario->scale = vec3_802f3d78;
            _partyEntry(1);
        } else if (M_S8(mario, 0x3E) == 0) {
            _partyEntry(0);
            mario->currSubMotionId = 0x1E;
        } else if (M_S8(mario, 0x3E) == 5) {
            mario->dispFlags |= 0x1000;
            if ((party = partyGetPtr(0)) != NULL) {
                partyShadowOff(party);
            }
            if ((party = partyGetPtr(1)) != NULL) {
                partyShadowOff(party);
            }
            M_VEC(mario, 0x98) = vec3_802f3d84;
            M_VEC(mario, 0xA4) = vec3_802f3d90;
            M_VEC(mario, 0xBC) = vec3_802f3d9c;
            M_VEC(mario, 0xB0) = vec3_802f3da8;
            marioSlitForceCancel();
            marioPaperOn(str_802F3E28);
            marioPaperLightOff();
            marioChgPaper(str_802F3E20);
            marioChgPose(str_8041CD84);
            mario->unk1A8 = M_F32(mario, 0x1AC);
            M_F32(mario, 0x2A0) = float_30_8041cd8c;
            mario->multiTimer = 0x14;
            mario->currSubMotionId = 0;
        } else if (M_S8(mario, 0x3E) == 4) {
            mario->scale = vec3_802f3db4;
            M_VEC(mario, 0x98) = vec3_802f3dc0;
            M_VEC(mario, 0xA4) = vec3_802f3dcc;
            M_VEC(mario, 0xBC) = vec3_802f3dd8;
            M_VEC(mario, 0xB0) = vec3_802f3de4;
            mario->directionView = toMovedir(mario->unk1A8);
            M_F32(mario, 0x19C) = mario->directionView;
            marioPaperOn(str_802F3E28);
            marioPaperLightOff();
            marioChgPaper(str_802F3E20);
            marioChgPose(str_8041CD84);
            mario->unk1A8 = M_F32(mario, 0x1AC);
            M_F32(mario, 0x2A0) = float_30_8041cd8c;
            mario->currSubMotionId = 0x28;
            _partyEntry(1);
            mario->dispFlags |= 0x1000;
            partyShadowOff(partyGetPtr(marioGetPartyId()));
            partyShadowOff(partyGetPtr(marioGetExtraPartyId()));
        }
    }

    switch (mario->currSubMotionId) {
    case 0xA:
        mario->unk1A8 = revise360(mario->unk1A8 + M_F32(mario, 0x2A0));
        mario->scale.x += float_neg0p005_8041cd90;
        if (mario->scale.x < float_0_8041cd80) {
            mario->scale.x = float_0_8041cd80;
        }
        mario->scale.y = mario->scale.x;
        mario->scale.z = mario->scale.x;
        break;
    case 0x14:
        mario->scale = vec3_802f3df0;
        mario->currSubMotionId = 0x15;
        if (mario->flags & 0x02000000) {
            M_F32(mario, 0xA8) = float_neg2_8041cd94;
        }
        // fallthrough
    case 0x15:
        mario->unk1A8 = revise360(mario->unk1A8 + M_F32(mario, 0x2A0));
        if (mario->flags & 0x02000000) {
            mario->scale.x += float_0p03_8041cd98;
            if (mario->scale.x >= float_1_8041cd9c) {
                mario->scale.x = float_1_8041cd9c;
            }
            mario->scale.y = mario->scale.x;
            mario->scale.z = mario->scale.x;
        } else {
            mario->scale.x += float_0p05_8041cda0;
            if (mario->scale.x >= float_1_8041cd9c) {
                mario->scale.x = float_1_8041cd9c;
            }
            mario->scale.y = mario->scale.x;
            mario->scale.z = mario->scale.x;
        }
        break;
    case 0x1E:
        mario->unk1A8 = revise360(mario->unk1A8 + M_F32(mario, 0x2A0));
        break;
    case 0x0:
        if ((s32)--mario->multiTimer > 0) {
            break;
        }
        M_F32(mario, 0xBC) += M_F32(mario, 0x2A0);
        mario->scale.x += float_neg0p02_8041cda4;
        if (mario->scale.x < float_0_8041cd80) {
            mario->scale.x = float_0_8041cd80;
        }
        mario->scale.y = mario->scale.x;
        mario->scale.z = mario->scale.x;
        M_F32(mario, 0xB4) = float_16_8041cda8 * mario->scale.x;
        M_F32(mario, 0x9C) = float_16_8041cda8 - M_F32(mario, 0xB4);
        M_F32(mario, 0xA0) = float_neg3_8041cdac;
        break;
    case 0x28:
        M_F32(mario, 0xBC) += M_F32(mario, 0x2A0);
        mario->scale.x += float_0p03_8041cd98;
        if (mario->scale.x >= float_1_8041cd9c) {
            mario->scale.x = float_1_8041cd9c;
        }
        mario->scale.y = mario->scale.x;
        mario->scale.z = mario->scale.x;
        M_F32(mario, 0xB4) = float_16_8041cda8 * mario->scale.x;
        M_F32(mario, 0x9C) = float_16_8041cda8 - M_F32(mario, 0xB4);
        M_F32(mario, 0xA0) = float_neg3_8041cdac;
        party = partyGetPtr(marioGetPartyId());
        if (party != NULL) {
            party->position = mario->position;
            M_F32(party, 0x70) = mario->scale.x;
            M_F32(party, 0x74) = mario->scale.y;
            M_F32(party, 0x78) = mario->scale.z;
            M_F32(party, 0x8C) = M_F32(mario, 0xB4);
            M_F32(party, 0x80) = M_F32(mario, 0x9C);
            M_F32(party, 0x84) = M_F32(mario, 0xA0);
        }
        party = partyGetPtr(marioGetExtraPartyId());
        if (party != NULL) {
            party->position = mario->position;
            M_F32(party, 0x70) = mario->scale.x;
            M_F32(party, 0x74) = mario->scale.y;
            M_F32(party, 0x78) = mario->scale.z;
            M_F32(party, 0x8C) = M_F32(mario, 0xB4);
            M_F32(party, 0x80) = M_F32(mario, 0x9C);
            M_F32(party, 0x84) = M_F32(mario, 0xA0);
        }
        mario->field_EC.y = mario->position.y;
        break;
    case 0x64:
        if (M_S8(mario, 0x3E) == 1) {
            mario->currSubMotionId = 0x6E;
        } else if (M_S8(mario, 0x3E) == 4) {
            marioUpdateCamPos();
            M_F32(mario, 0xBC) = float_0_8041cd80;
            marioChgPaper(str_802F3E34);
            mario->multiTimer = 0x14;
            mario->currSubMotionId = 0x66;
            partyDokanEnd(marioGetPartyId(), M_S8(mario, 0x3E));
            partyDokanEnd(marioGetExtraPartyId(), M_S8(mario, 0x3E));
        } else if (M_S8(mario, 0x3E) == 1) {
            mario->currSubMotionId = 0x65;
        } else if (M_S8(mario, 0x3E) == 0) {
            marioUpdateCamPos();
            mario->currSubMotionId = 0x65;
        } else {
            mario->currSubMotionId = 0x66;
        }
        break;
    case 0x65:
        mario->unk1A8 += M_F32(mario, 0x2A0);
        if (mario->unk1A8 >= float_540_8041cdb0) {
            marioChgPaper(str_802F3E34);
            mario->multiTimer = 0x14;
            mario->currSubMotionId = 0x66;
            M_F32(mario, 0xBC) = float_0_8041cd80;
            M_F32(mario, 0x1AC) = revise360(float_180_8041cdb4 + M_F32(mario, 0x1AC));
            mario->unk1A8 = M_F32(mario, 0x1AC);
            mario->directionView = toMovedirSimple(mario->unk1A8);
            M_F32(mario, 0x19C) = mario->directionView;
            partyDokanEnd(marioGetPartyId(), M_S8(mario, 0x3E));
            partyDokanEnd(marioGetExtraPartyId(), M_S8(mario, 0x3E));
        }
        break;
    case 0x66:
        if ((s32)--mario->multiTimer <= 0) {
            mario->currSubMotionId = 0x67;
        }
        break;
    case 0x67:
        M_VEC(mario, 0xE0) = mario->position;
        mario->scale = vec3_802f3dfc;
        marioOfsRotReset();
        mario->dispFlags &= ~0x8;
        mario->dispFlags &= ~0x100;
        mario->dispFlags &= ~0x1000;
        marioPaperOff();
        marioChgMot(0);
        mario->unk1A8 = revise360(mario->unk1A8);
        M_F32(mario, 0x1AC) = mario->unk1A8;
        mario->directionView = toMovedirSimple(mario->unk1A8);
        M_F32(mario, 0x19C) = mario->directionView;
        if (M_S8(mario, 0x3E) == 4) {
            camFollowYOn();
            mario->field_EC.y = mario->position.y;
        }
        mario->flags &= ~0x100;
        break;
    default:
        break;
    }

    switch (mario->currSubMotionId) {
    case 0x6E:
        if (!(mario->flags & 0x02000000)) {
            mario->field_EC.y = mario->position.y;
        }
        marioUpdateCamPos();
        camFollowYOff();
        mario->currSubMotionId = 0x6F;
        // fallthrough
    case 0x6F:
        mario->unk1A8 += M_F32(mario, 0x2A0);
        if (mario->unk1A8 >= float_540_8041cdb0) {
            if (!(mario->flags & 0x02000000)) {
                camFollowYOn();
            }
            marioChgPaper(str_802F3E34);
            mario->multiTimer = 0x14;
            mario->currSubMotionId = 0x70;
            M_F32(mario, 0xBC) = float_0_8041cd80;
            M_F32(mario, 0x1AC) = revise360(mario->unk1A8);
            mario->unk1A8 = M_F32(mario, 0x1AC);
            mario->directionView = toMovedirSimple(mario->unk1A8);
            M_F32(mario, 0x19C) = mario->directionView;
            partyDokanEnd(marioGetPartyId(), M_S8(mario, 0x3E));
            partyDokanEnd(marioGetExtraPartyId(), M_S8(mario, 0x3E));
        }
        break;
    case 0x70:
        if ((s32)--mario->multiTimer <= 0) {
            mario->currSubMotionId = 0x71;
        }
        break;
    case 0x71:
        M_VEC(mario, 0xE0) = mario->position;
        mario->scale = vec3_802f3e08;
        marioOfsRotReset();
        mario->dispFlags &= ~0x8;
        mario->dispFlags &= ~0x100;
        mario->dispFlags &= ~0x1000;
        marioPaperOff();
        marioChgMot(0);
        mario->unk1A8 = revise360(mario->unk1A8);
        M_F32(mario, 0x1AC) = mario->unk1A8;
        mario->directionView = toMovedirSimple(mario->unk1A8);
        M_F32(mario, 0x19C) = mario->directionView;
        if (!(mario->flags & 0x02000000)) {
            camFollowYOn();
        }
        mario->flags &= ~0x100;
        break;
    default:
        break;
    }
}

// The original allocates the first word of each 0x70 Vec copy to r3 (lwz r3; lwz r0; stw r3; stw r0; lwz r0;
// stw r0); MWCC allocates it to r0 and hoists the third load. No expression shape tried changes this
// (same scheduling artifact as mot_roll.c).
// NONMATCHING (96.45%): only the two Vec copies at 0x70 differ.
void _partyEntry(s32 arg) {
    MarioWork* mario;
    PartyEntry* party;
    s32 i;
    s32 count;
    s32 member;
    f32 angle;
    f32 px;
    f32 py;
    f32 pz;

    mario = marioGetPtr();
    if (mario->flags & 0x02000000) {
        return;
    }
    count = 0;
    if (partyGetPtr(mario->currPartySlotId[0]) != NULL) {
        count = 1;
    }
    if (partyGetPtr(mario->currPartySlotId[1]) != NULL) {
        count++;
    }
    for (i = 0; i < 2; i++) {
        if (mario->currPartySlotId[i] < 0) {
            member = mario->prevPartyMemberId[i];
            px = mario->position.x;
            py = mario->position.y;
            pz = mario->position.z;
            if (member != 0) {
                if (i == 0) {
                    member = partyEntryPos(member, px, py, pz);
                } else {
                    member = partyEntry2Pos(member, px, py, pz);
                }
                if (member >= 0) {
                    mario->currPartySlotId[i] = member;
                    party = partyGetPtr(member);
                    partyChgRunMode(party, 0xA);
                    if (arg == 0) {
                        M_VEC(party, 0x70) = vec3_802f3d60;
                    }
                    party->flags |= 0x04000000;
                    party->flags |= 0x02000000;
                    if (count == 1) {
                        angle = MATCH_F32(float_180_8041cdb4);
                    } else {
                        angle = MATCH_F32(float_120_8041cdb8);
                    }
                    party->flags2 &= ~1;
                    party->field_110 = revise360(mario->unk1A8 + angle);
                    party->field_10C = mario->unk1A8;
                    party->field_FC = M_F32(mario, 0x19C);
                }
            }
        } else {
            party = partyGetPtr(mario->currPartySlotId[i]);
            partyChgRunMode(party, 0xA);
            if (arg == 0) {
                M_VEC(party, 0x70) = vec3_802f3d6c;
            }
            party->flags |= 0x04000000;
            party->flags |= 0x02000000;
            if (count == 1) {
                angle = MATCH_F32(float_180_8041cdb4);
            } else {
                angle = MATCH_F32(float_neg120_8041cdbc);
            }
            party->flags2 &= ~1;
            party->field_110 = revise360(mario->unk1A8 + angle);
            party->field_10C = mario->unk1A8;
            party->field_FC = M_F32(mario, 0x19C);
        }
    }
}
