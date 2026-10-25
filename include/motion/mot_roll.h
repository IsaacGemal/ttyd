#pragma once

#include <dolphin/types.h>
BOOL unk_8009f9c8(void);
void marioForceRollAnime(void);
void marioReInit_roll(void);
BOOL marioRollKeyDisable(void);
void marioResetRollSpd(void);
void mot_fall_roll(void);
BOOL getRollEvtFlag(void);
BOOL marioRollCancel(void);
void rollEvtJumpSetup(void);
void setRollEvtFlag(void);
void clrRollEvtFlag(void);
void marioRollForceCancel(void);
BOOL unk_8009fcbc(void);
void marioRollFixOn(void);
void marioSetRollDispDir(void);
void roll_upstairs_on(void);
extern f32 roll_grx;
extern f32 roll_grz;
typedef struct MotRoll {
    f32 velX;       // 0x0
    f32 velY;       // 0x4
    s32 field_0x8;  // 0x8
    s32 state;      // 0xC
    f32 field_0x10; // 0x10
} MotRoll;
void roll_upstairs(void);
void mot_roll(void);
void mot_roll_post(void);
