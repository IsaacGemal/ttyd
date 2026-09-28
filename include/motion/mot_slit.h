#pragma once

#include <dolphin/types.h>
void marioReInit_slit(void);
void marioSlitForceCancel(void);
BOOL marioSlitKeyDisable(void);
void N_marioSlitSetAfterLand(void);
void unk_800a4e40(void);
void motSlitCancel(void);
s32 marioGetMotSlitCancel3(void);
void marioClearSlitFloor(void);
BOOL marioChkSlitThrouhEnd(void);
BOOL marioChkSlitEnd(void);
#include <dolphin/mtx.h>
BOOL marioChkSlitThrouh(void);
BOOL marioGetSlitFloorPos(Vec* pos);
void motSlitCancel2(void);
