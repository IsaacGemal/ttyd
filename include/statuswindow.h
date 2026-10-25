#pragma once

#include "driver/camdrv.h"

#include <dolphin/mtx.h>

// StatusWindowWork.flags bits. Bits 0x1/0x2 are the wanted state (open or closed); 0x4/0x8 are an open or close
// animation waiting to start; 0x10/0x20 hold the window open or closed; 0x40/0x80 are a forced transition
// waiting to start (statusWinForceOpen/Close); 0x100 is the badge panel, drawn only while it is set.
enum StatusWindowFlag {
    STATUS_WIN_OPEN = 0x1,
    STATUS_WIN_CLOSED = 0x2,
    STATUS_WIN_OPENING = 0x4,
    STATUS_WIN_CLOSING = 0x8,
    STATUS_WIN_FORCE_OPEN = 0x10,
    STATUS_WIN_FORCE_CLOSE = 0x20,
    STATUS_WIN_FORCE_OPENING = 0x40,
    STATUS_WIN_FORCE_CLOSING = 0x80,
    STATUS_WIN_BADGES = 0x100,
    STATUS_WIN_DISP_OFF = 0x1000,
};

// StatusWindowWork.blinkMask bits (set by the statusXxxBlink calls and by valueCheck, which keeps the changed
// set in changedMask until the values stop moving).
enum StatusWindowBlink {
    STATUS_BLINK_MARIO_HP = 0x1,
    STATUS_BLINK_FP = 0x2,
    STATUS_BLINK_AP = 0x4,
    STATUS_BLINK_PARTY_HP = 0x8,
    STATUS_BLINK_COIN = 0x10,
};

typedef struct StatusWindowWork {
	u8 pad_00[0x4]; //0x0
	u16 flags; //0x4, StatusWindowFlag
	u8 pad_06[0x2]; //0x6
	f32 animTime; //0x8, frames into the open/close ramp
	f32 animDuration; //0xC, frames the ramp takes at animSpeed 1
	f32 openAmount; //0x10, 0.0 closed .. 1.0 open (eased)
	f32 openFrom; //0x14, openAmount when the current ramp started
	f32 openTarget; //0x18, openAmount the current ramp is heading to
	f32 animSpeed; //0x1C, divides the ramp duration
	f32 originX; //0x20, icon base x, lerped between openX and closedX
	f32 originY; //0x24, icon base y
	u8 pad_28[0x8]; //0x28
	f32 openX; //0x30
	f32 openY; //0x34
	u8 pad_38[0x8]; //0x38
	f32 closedX; //0x40
	f32 closedY; //0x44
	u8 pad_48[0x8]; //0x48
	s16 hp; //0x50, displayed values (move toward the live ones in valueUpdate)
	s16 maxHp; //0x52
	s16 fp; //0x54
	s16 maxFp; //0x56
	s16 partyId; //0x58
	s16 partyHp; //0x5A
	s16 partyMaxHp; //0x5C
	s16 starPoint; //0x5E
	s16 coin; //0x60
	s16 ap; //0x62
	s16 maxAp; //0x64
	u8 pad_66[0x2]; //0x66
	Vec marioPos; //0x68, Mario's position last frame (idle check)
	s32 idleFrames; //0x74, frames Mario has stood still near marioPos
	s32 toggleCooldown; //0x78, frames before the window may toggle again
	s32 manualToggle; //0x7C, set when a button press opened or closed the window
	s32 blinkMask; //0x80, StatusWindowBlink
	u32 changedMask; //0x84, StatusWindowBlink bits waiting for valueCheck to see them settle
	s32 blinkTimer; //0x88, frames of blink left
} StatusWindowWork;

typedef struct StatusValue {
	u8 field_0x0[0x18]; //0x0
} StatusValue;

void statusWinInit(void);
void statusWinReInit(void);
void statusWinMain(void);

void statusWinForceUpdate(void);
BOOL statusWinCheck(void);
void statusWinOpen(void);
void statusWinClose(void);
void statusWinForceClose(void);
void statusWinForceCloseClear(void);
void statusWinForceOpen(void);
void statusWinForceUpdateCoin(void);
void gaugeDisp(s32 a1, f32 x, f32 y);
s32 valueCheck(void);
void valueUpdate(void);
void statusWinForceOff(void);
void statusWinDisp(CameraId camId, void* param);
void statusWinDispOff(void);
void statusWinDispOn(void);
BOOL statusWinCheckUpdate(void);

typedef struct StatusWindowValue StatusWindowValue;
void statusGetValue(StatusWindowValue* values);
void statusAPBlink(s32 value);
void statusMarioHPBlink(s32 value);
void statusFPBlink(s32 value);
void statusPartyHPBlink(s32 value);
void N_statusClearBlink(void);
Vec statusGetApPos(void);
