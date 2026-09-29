#include "dolphin/types.h"
#include "mario/mario.h"

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
