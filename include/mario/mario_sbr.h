#pragma once

typedef struct PartyEntry PartyEntry;

#include <dolphin/types.h>

void marioSetBottomlessResetPosition(f32 x, f32 y, f32 z);
f32 revise360(f32 angle);
f32 toMovedir(f32 angle);
void marioSetForceMove(s32 timer, f32 direction, f32 speed);
void marioAdjustMoveDir(void);
struct HitEntry;
BOOL marioChkPlanePlace(struct HitEntry* hit);
void N_marioSetBottomlessRespawnPosOnBeroEntry(f32 x, f32 y, f32 z);
f32 toMovedirSimple(f32 angle);
void marioSetPeakYpos(void);
void marioForceMoveMain(void);
void marioBottomless(void);
s32 marioGetRub(u32 mask, s32* out1, s32* out2, f32* out3);
void* marioGetTouchNpcPtr(void);
s32 checkOneRevolution(s32 dir);
s32 chkRevolveDir(s32* left, s32* right);
void marioResetGravity(void);
void marioGetRubInit(void);
s32 N_marioGetMovementSpeed(void);
f32 marioGetMoveRate(f32 angle);
f32 toMovedir2(f32 a, f32 b);
f32 partyToMovedir(PartyEntry* party, f32 dir);
void N_marioReloadMapOnBottomlessOn(void);
void N_marioReloadMapOnBottomlessOff(void);
void marioGetStick(f32* outAngle, f32* outMag);
