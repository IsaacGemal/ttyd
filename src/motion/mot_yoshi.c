#include "dolphin/types.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "evt/evt_bero.h"
#include "party.h"
#include "pmario_sound.h"

void mot_yoshi(void) {
    MarioWork* mario = marioGetPtr();

    if (mario->trigFlags & MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION) {
        mario->trigFlags &= ~MARIO_TRIG_FLAG_IS_STARTING_NEW_MOTION;
        mario->flags &= ~0x000F0000;
        mario->dispFlags |= 0x4;
        mario->dispFlags |= 0x1000;
        mario->multiTimer = 0;
        mario->airTimer = 0;
        mario->currSubMotionId = 0;
        mario->baseSpeed = 0.0f;
        *(u32*)mario->field_0x1DC = 0;
        *(u32*)(mario->field_0x1DC + 4) = 0;
        mario->hitobjStandOn = NULL;
        mario->hitobjPush = NULL;
        *(u32*)mario->field_0x1F0 = 0;
        *(u32*)(mario->field_0x1F0 + 4) = 0;
        *(u32*)(mario->field_0x1F0 + 8) = 0;
    }
}

const Vec vec3_802f32e0 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f32ec = { 0.0f, 0.0f, 0.0f };

// The two dispFlags read-modify-writes must run before the zero-vector copies, and the vector loads must
// not be hoisted above the flag stores: volatile accesses pin the original's instruction order (rule 7).
void mot_yoshi_post(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    MarioWork* mario = marioGetPtr();

    if ((u32)party->yoshiPsndSFXId != 0xFFFFFFFF) {
        psndSFXOff(party->yoshiPsndSFXId);
        party->yoshiPsndSFXId = -1;
    }
    bero_clear_Offset();
#ifdef MUST_MATCH
    *(volatile s32*)&mario->dispFlags &= ~0x4;
    *(volatile s32*)&mario->dispFlags &= ~0x1000;
    *(volatile Vec*)&mario->field_0x98[0] = *(volatile Vec*)&vec3_802f32e0;
    *(volatile Vec*)&mario->field_0x98[0xC] = *(volatile Vec*)&vec3_802f32ec;
#else
    mario->dispFlags &= ~0x4;
    mario->dispFlags &= ~0x1000;
    *(Vec*)&mario->field_0x98[0] = vec3_802f32e0;
    *(Vec*)&mario->field_0x98[0xC] = vec3_802f32ec;
#endif
}
