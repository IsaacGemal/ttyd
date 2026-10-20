#include "cam_shift.h"
#include "cam_road.h"
#include "system.h"
#include "driver/camdrv.h"
#include "mario/mario.h"
#include "mario/mariost.h"

typedef struct CamShiftState {
    /* 0x00 */ f32 bank;           // current bank, written to the caller
    /* 0x04 */ f32 targetBank;     // road shift X rate the bank is easing towards
    /* 0x08 */ f32 stepScale;      // 0..1 scale on the remaining bank delta
    /* 0x0C */ f32 stickAccum;     // accumulated sideways stick input
    /* 0x10 */ f32 scaleActive;    // stepScale is ramping up
    /* 0x14 */ u8 pad_14[0x4];
    /* 0x18 */ s64 prevTime;       // renderTime of the previous frame
    /* 0x20 */ f32 stickAccumMax;  // clamp for stickAccum
    /* 0x24 */ f32 maxStep;        // clamp for one frame's bank change
    /* 0x28 */ f32 scaleRate;      // stepScale increment per frame
    /* 0x2C */ f32 lastMarioX;
    /* 0x30 */ f32 lastMarioZ;
} CamShiftState;

//.bss
static CamShiftState work;

//.sdata
static CamShiftState* wp = &work;

void camShiftInit(void) {
    wp->bank = 0.0f;
    wp->targetBank = 0.0f;
    wp->stepScale = 0.0f;
    wp->stickAccum = 0.0f;
    wp->scaleActive = 0.0f;
    wp->stickAccumMax = 300.0f;
    wp->maxStep = 3.0f;
    wp->scaleRate = 0.002f;
    wp->lastMarioX = 0.0f;
    wp->lastMarioZ = 0.0f;
}

void camShiftMain(CameraEntry* camera, MarioWork* mario, f32* out) {
    f32 shiftXRate;
    f32 stickInput;
    s32 camFlag;

    camFlag = 0;
    shiftXRate = camRoadGetCurrentShiftXRate();
    if (camera->flags & 0x20) {
        camFlag = 1;
    }
    if (camFlag) {
        wp->stepScale = 1.0f;
        stickInput = 0.0f;
        wp->targetBank = 0.0f;
        wp->scaleActive = 1.0f;
    } else {
        stickInput = (f32)(s8)mario->wStickDir1;
        if (stickInput > 0.0f) {
            stickInput = 25.0f;
        }
        if (stickInput < 0.0f) {
            stickInput = -25.0f;
        }
        if (mario->position.x == wp->lastMarioX && mario->position.z == wp->lastMarioZ) {
            stickInput = 0.0f;
        }
        wp->lastMarioX = mario->position.x;
        wp->lastMarioZ = mario->position.z;
    }
    if (stickInput) {
        if (stickInput < 0.0f) {
            if (wp->stickAccum > 0.0f) {
                wp->stickAccum = getV60FPS(stickInput, wp->prevTime, gp->renderTime);
            } else {
                wp->stickAccum += getV60FPS(stickInput, wp->prevTime, gp->renderTime);
            }
            if (wp->stickAccum <= -wp->stickAccumMax) {
                wp->scaleActive = 1.0f;
                if (wp->targetBank > 0.0f) {
                    wp->stepScale = 0.0f;
                }
                wp->targetBank = -shiftXRate;
                wp->stickAccum = -wp->stickAccumMax;
            }
        } else {
            if (wp->stickAccum < 0.0f) {
                wp->stickAccum = getV60FPS(stickInput, wp->prevTime, gp->renderTime);
            } else {
                wp->stickAccum += getV60FPS(stickInput, wp->prevTime, gp->renderTime);
            }
            if (wp->stickAccum >= wp->stickAccumMax) {
                wp->scaleActive = 1.0f;
                if (wp->targetBank < 0.0f) {
                    wp->stepScale = 0.0f;
                }
                wp->targetBank = shiftXRate;
                wp->stickAccum = wp->stickAccumMax;
            }
        }
    }
    if (wp->scaleActive) {
        wp->stepScale += getV60FPS(wp->scaleRate, wp->prevTime, gp->renderTime);
        if (wp->stepScale > 1.0f) {
            wp->stepScale = 1.0f;
        }
    }
    if (wp->targetBank - wp->bank == 0.0f) {
        wp->stepScale = 0.0f;
        wp->scaleActive = 0.0f;
    }
    {
        f32 bankDelta;
        f32 bankStep;

        bankStep = wp->stepScale * (bankDelta = wp->targetBank - wp->bank);

        if (bankDelta > 0.0f) {
            if (bankDelta < 0.0001f) {
                bankStep = bankDelta;
            }
            if (bankStep > wp->maxStep) {
                bankStep = wp->maxStep;
            }
        } else {
            if (bankDelta > -0.0001f) {
                bankStep = bankDelta;
            }
            if (bankStep < -wp->maxStep) {
                bankStep = -wp->maxStep;
            }
        }
        if (stickInput || camFlag) {
            wp->bank += getV60FPS(bankStep, wp->prevTime, gp->renderTime);
            if (wp->targetBank < 0.0f) {
                if (wp->bank < wp->targetBank) {
                    wp->bank = wp->targetBank;
                }
            } else {
                if (wp->bank > wp->targetBank) {
                    wp->bank = wp->targetBank;
                }
            }
        } else {
            wp->stepScale = 0.0f;
        }
    }
    *out = wp->bank;
}

void camShiftPostMain(void) {
    wp->prevTime = gp->renderTime;
}

void camShiftReset(void) {
    wp->bank = 0.0f;
    wp->targetBank = 0.0f;
    wp->stepScale = 0.0f;
    wp->stickAccum = 0.0f;
    wp->scaleActive = 0.0f;
    wp->stickAccumMax = 300.0f;
    wp->maxStep = 3.0f;
    wp->scaleRate = 0.002f;
    wp->lastMarioX = 0.0f;
    wp->lastMarioZ = 0.0f;
}
