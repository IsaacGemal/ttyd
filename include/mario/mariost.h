#pragma once

#include <dolphin/os.h>
#include "driver/fadedrv.h"
#include "driver/npcdrv.h"
#include "evt/evt_lottery.h"
#include "countdown.h"

typedef struct GlobalWork {
	s32 flags; //0x0
	s32 framerate; //0x4
	s32 field_0x8; //0x8, nameent takes the A button only while it is 0; cardmgr clears it
	s32 debugBattleFlag; //0xC, seq_battleInit copies it to debug_battle_flag; while set, seq_mapchange and the battle load
	                     //     the debug messages (gon_11, DbgBtlSel's) and the battle ends into SEQ_GAME_OVER, clearing it
	BOOL dvdError; //0x10
	s32 inBattle; //0x14, verified s32
	u32 systemLevelFlags; //0x18
	u32 retraceCount; //0x1C
	OSTime retraceLocalTime; //0x20
	OSTime retraceDeltaTime; //0x28
	OSTime retraceTime; //0x30
	OSTime renderTime; //0x38, time since boot, minus menus
	OSTime renderFieldTime; //0x40, time outside battle, minus menus
	OSTime systemLv0Time; //0x48
	OSTime systemLv1Time; //0x50
	OSTime systemLv2Time; //0x58
	OSTime systemLv3Time; //0x60
	CountdownWork countdown; //0x68
	lottery_info mLotteryInfo; //0xA8
	u8 field_0xE8[0xF8 - 0xE8]; //0xE8
	FadeType mapFadeOutType; //0xF8
	s32 mapFadeOutDuration; //0xFC
	FadeType mapFadeInType; //0x100
	s32 mapFadeInDuration; //0x104
	FadeType areaFadeOutType; //0x108
	s32 areaFadeOutDuration; //0x10C
	FadeType areaFadeInType; //0x110
	s32 areaFadeInDuration; //0x114
	BOOL didAreaChange; //0x118
	char beroEnterName[0x10]; //0x11C
	char currentMapName[0x10]; //0x12C
	char currentAreaName[0x10]; //0x13C
	char lastMapName[0x10]; //0x14C, the map left for the title screen or game over (seq_mapchange, seq_gameover)
	void* relocationBase; //0x15C
	void* mapAlloc; //0x160
	u32 mapInitEvtId; //0x164, event id of the map init script (seq_mapchange waits on it; item and npc drivers skip while set)
	FieldBattleData* fbatData; //0x168
	u32 language; //0x16C
	u16 fbWidth; //0x170
	u16 efbHeight; //0x172
	s32 mGSW0; //0x174
	u32 mGSFW[0x100]; //0x178
	u8 mGSW[0x800]; //0x578
	u32 mLSWF[0x10]; //0xD78
	u8 mLSW[0x400]; //0xDB8
	s32 lastFileSlot; //0x11B8, slot last picked on the file menu; the file menu cursor starts here (seq_load)
	char playerName[0x14]; //0x11BC, copied from nameEntGetName (up to 8 two-byte characters)
	s32 saveSlot; //0x11D0, card slot cardWrite saves to (memcard_write); also set on file select and continue
	Vec savePlayerPos; //0x11D4
    s32 savePartyId[2]; //0x11E0
    OSTime saveTime; //0x11E8
    s32 saveCount; //0x11F0
	u8 field_0x11F4[0x1274 - 0x11F4]; //0x11F4
	s32 soundSurround; //0x1274, 1 = SND_OUTPUTMODE_SURROUND when the console is in stereo (seq_load sound menu, seq_mapchange)
	s32 resetType; //0x1278, 0 = no reset, 1 = soft reset, 2 = menu
	BOOL resetButtonDown; //0x127C, reset button held (gcResetCheck)
	BOOL softResetWait; //0x1280
	u8 field_0x1284[0x1288 - 0x1284]; //0x1284
	OSTime softResetTimeout; //0x1288
	BOOL restartChecked; //0x1290, set once the restart reset check has run (gcResetCheck)
	BOOL rumbleEnabled; //0x1294, vibration option (default on, set in marioStInit; seq_load prompt; kept in the save)
	u8 field_0x1298[0x12C0 - 0x1298]; //0x1298
	u8 rumbleHeld[4]; //0x12C0, pad is rumbling (cleared 100 ms after it stops), gcRumbleCheck
	OSTime rumbleHeldSince[4]; //0x12C8
	u8 rumbleLocked[4]; //0x12E8, rumble ran 30 s straight, so the pad is locked out for 30 s
	u8 rumbleLockStarted[4]; //0x12EC
	OSTime rumbleLockedSince[4]; //0x12F0
	//u8 field_0x1300[0x1310 - 0x1300]; //0x1300
	u8 rumbleSuppressed[4]; //0x1310, motor forced off while set (system.c makeKey), set by gcRumbleCheck
	OSTick mDeltaGame; //0x1314
	OSTick mDeltaRender; //0x1318
	u8 field_0x131C[0x1324 - 0x131C]; //0x131C
// system.c "key" controller data, TODO sub-struct?
	BOOL keyDataReady; //0x1324, set at the end of makeKey (system.c); gcResetCheck waits for it
	u32 button[4]; //0x1328
	u32 buttonNew[4]; //0x1338
	u32 buttonRepeat[4]; //0x1348
	u32 buttonRepeatDelay[4]; //0x1358,
	u32 buttonUp[4]; //0x1368
	u32 dirs[4]; //0x1378
	u32 dirsNew[4]; //0x1388
	u32 dirsRepeat[4]; //0x1398
	u32 dirsRepeatDelay[4]; //0x13A8
	s8 stickX[4]; //0x13B8
	s8 stickY[4]; //0x13BC
	s8 substickX[4]; //0x13C0
	s8 substickY[4]; //0x13C4
	u8 triggerLeft[4]; //0x13C8
	u8 triggerRight[4]; //0x13CC
	u8 rumbleStatus[4]; //0x13D0
	u8 prevRumbleStatus[4]; //0x13D4
} GlobalWork;
extern GlobalWork* gp;

void marioStInit(void);
void marioStMain(void);
void marioStDisp(void);
void marioStSystemLevel(s32 level);
u32 marioStGetSystemLevel(void);
void viPostCallback(u32 retraceCount);
void* gcDvdCheckThread(void* param);
void gcRumbleCheck(void);
void gcResetCheck(void);
void L_gcResetCheck(void);
