#pragma once

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os.h>
#include "memory.h"

typedef struct WindowEntry WindowEntry;

typedef enum WindowType {
	WINDOW_TYPE_NORMAL = 0,
	WINDOW_TYPE_SMALL = 1,
	WINDOW_TYPE_PLATE = 2,
	WINDOW_TYPE_SYSTEM = 3,
	WINDOW_TYPE_KANBAN = 4, //kanban = sign/billboard
	WINDOW_TYPE_SELECT = 5,
	WINDOW_TYPE_BOSS = 6,
	WINDOW_TYPE_DIARY = 7,
	WINDOW_TYPE_TEC = 8,
	WINDOW_TYPE_MAJO = 9, //majo = witch, shadow sirens
	WINDOW_TYPE_HOUSOU = 10, //housou = broadcast/announcement
	WINDOW_TYPE_CLEAR = 11
} WindowType;

struct WindowEntry {
	u16 state; //0x0, msgdrv window state machine (values 1-11, see msgWindow_Main)
	u16 flags; //0x2
	s16 priority; //0x4, dispEntry depth is 400 - priority (msgWindow_Main, msgWindow_Clear_Main, selectWindow_Main)
	s16 alpha; //0x6, window alpha
	s32 type; //0x8, WindowType
	f32 x; //0xC, window xpos
	f32 y; //0x10, window ypos
	f32 height; //0x14
	f32 width; //0x18
	u8 pad_0x1C[0x20 - 0x1C]; //0x1C, never read
	OSTime stateTime; //0x20, renderTime base for the fade ramps (stamped on state change) and history scroll (re-stamped each frame)
	smartEntry* userdata; //0x28, TODO: verify? could be wrong
	s16 dispFlags; //0x2C, passed to windowDispGX_Message (0x1 draw tail, 0x4/0x8 tail placement); msgWindow_Disp: 0x10 anchor already in screen space, 0x8 y = pos.y - 80 instead of pos.y + 60
	u8 pad_0x2E[0x30 - 0x2E]; //0x2E
	Vec anchor; //0x30, world point the message window hangs from
	void (*main)(WindowEntry* entry); //0x3C
	void (*delete)(WindowEntry* entry); //0x40
	u8 pad_0x44[0x48 - 0x44]; //0x44
};

void windowInit(void);
void windowTexSetup(void);
void windowReInit(void);
s32 windowEntry(u16 priority);
BOOL windowDelete(WindowEntry* entry);
BOOL windowDeleteID(s32 id);
void windowMain(void);
void windowDispGX_Kanban(s32 type, u8 alpha, f32 x, f32 y, f32 height, f32 width);
void windowDispGX_System(s32 type, u8 alpha, f32 x, f32 y, f32 height, f32 width);
void windowDispGX_Message(s32 type, s32 a2, u8 alpha, f32 x, f32 y, f32 height, f32 width, f32 a9, f32 a10);
void windowDispGX_Waku_col(u16 gxTexMapID, GXColor color, f32 x, f32 y, f32 width, f32 height, f32 curve);
s32 windowCheckID(s32 id);
WindowEntry* windowGetPointer(s32 id);
GXTexObj* getWakuTexObj(s32 id);
void windowDispGX2_Waku_col(Mtx mtx, u16 gxTexMapID, GXColor color, f32 x, f32 y, f32 width, f32 height, f32 curve);
void _windowDispGX_Message(s32 type, s32 a2, u8 alpha, BOOL dark, f32 x, f32 y, f32 height, f32 width, f32 a9, f32 a10);
void unk_8007f204(s32 error, DVDFileInfo* info);
