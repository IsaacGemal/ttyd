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
extern Vec vec3_802c5d40[];
