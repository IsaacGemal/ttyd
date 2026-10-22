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
        mario->actionHitObj = NULL;
        mario->hitobjFront = NULL;
        mario->hitobjStandOn = NULL;
        mario->hitobjPush = NULL;
        mario->hitobjWall = NULL;
        mario->hitobjHead = NULL;
        mario->hitobjUnder = NULL;
    }
}

void mot_yoshi_post(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());
    MarioWork* mario = marioGetPtr();

    if ((u32)party->yoshiPsndSFXId != 0xFFFFFFFF) {
        psndSFXOff(party->yoshiPsndSFXId);
        party->yoshiPsndSFXId = -1;
    }
    bero_clear_Offset();
    mario->dispFlags &= ~0x4;
    mario->dispFlags &= ~0x1000;
    mario->dispOfs = (Vec){ 0.0f, 0.0f, 0.0f };
    mario->dispOfs2 = (Vec){ 0.0f, 0.0f, 0.0f };
}
