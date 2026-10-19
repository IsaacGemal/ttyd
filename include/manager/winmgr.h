#pragma once

#include "evt/evt_cmd.h"

typedef struct EventEntry EventEntry;

#include "driver/dispdrv.h"
#include <dolphin/types.h>

typedef struct WinMgrDesc WinMgrDesc;
typedef struct WinMgrEntry WinMgrEntry;

struct WinMgrDesc {
	s32 openStyle; //0x0, winMgrSeq: 0 zoom with rotation, 1 fade in alpha, 2 instant
	s32 frameStyle; //0x4, winMgrDisp: which frame drawn (1, 3, 4/5)
	s32 camId; //0x8, CameraId
	s32 x; //0xC, copied to entry->x
	s32 field_0x10; //0x10, entry->y (read by get10 under MUST_MATCH)
	s32 width; //0x14, copied to entry->width
	s32 height; //0x18, copied to entry->height
	u8 color[0x20 - 0x1C]; //0x1C, frame RGB (winMgrDispWaku0); alpha is entry->alpha
	void (*main)(WinMgrEntry* entry); //0x20
	void* disp; //0x24
};

struct WinMgrEntry {
	s32 flags; //0x0
	s32 state; //0x4, 0 idle, 1 opening (winMgrOpen), 2 closing (winMgrClose)
	s32 animStep; //0x8, 0..8 through the intplGetValue ramps in winMgrSeq
	s32 alpha; //0xC, frame colour alpha: 0..255
	f32 scale; //0x10
	f32 rotation; //0x14
	s32 x; //0x18
	s32 y; //0x1C
	s32 width; //0x20
	s32 height; //0x24
	WinMgrDesc* description; //0x28
	void* param; //0x2C
	s32 priority; //0x30, gets cast to float for dispEntry
	const char* message; //0x34
	s32 scrollPos; //0x38, help text scroll, 2 rows per step
	s32 scrollRows; //0x3C, lines + 1 of the help text; the scroll arrows show while > 2
	u8 field_0x40[0x44 - 0x40]; //0x40
};

typedef struct WinMgrWork {
	u32 mNumEntries; //0x0
	WinMgrEntry* mEntries;
} WinMgrWork;

typedef struct WinMgrSelectItem {
	u16 flags; //0x0, bit 0: drawn grey, bit 1: row has no second column (winMgrSelectDisp)
	u16 id; //0x2
} WinMgrSelectItem;

typedef struct WinMgrSelect {
	u16 flags; //0x0, 0x100: winMgrSelectEntry's a3, 0x1000: result ready, 0x2000: cancelled
	u16 pad_2; //0x2, TODO: not pad?
	s32 kind; //0x4, winMgrSelectEntry's a1: picks the item list and the callbacks
	s32 state; //0x8, select_main's step (0 while the menu is running)
	s32 cursor; //0xC, index into items
	s32 scrollTop; //0x10, first item shown while the list scrolls
	f32 iconX; //0x14, icon 0x1F0 is drawn at (iconX, iconY) every frame
	f32 iconY; //0x18
	f32 baseY; //0x1C, added to every row's y
	s32 numDescriptors; //0x20
	s32 descriptorId[3]; //0x24
	WinMgrSelectItem* items; //0x30
	s32 numItems; //0x34
	s32 field_0x38; //0x38
} WinMgrSelect;

void winMgrInit(void);
void winMgrReInit(void);
void winMgrMain(void);
s32 winMgrEntry(WinMgrDesc* desc);
void winMgrSetParam(s32 id, void* param);
void winMgrOpen(s32 id);
void winMgrClose(s32 id);
void winMgrCloseAutoDelete(s32 id);
void winMgrDelete(s32 id);
BOOL winMgrAction(s32 id);
void winMgrSetSize(s32 id, s32 a2, s32 a3, s32 a4, s32 a5); //TODO: x0, y0, x1, y1?
void winMgrSetPriority(s32 id, s32 priority);
WinMgrEntry* winMgrGetPtr(s32 id);
void winMgrHelpInit(WinMgrEntry* entry);
void winMgrHelpMain(WinMgrEntry* entry);
void winMgrHelpDraw(WinMgrEntry* entry);
void winMgrSelectDelete(WinMgrSelect* select);
WinMgrSelect* winMgrSelectEntry(s32 a1, s32 a2, BOOL a3);
s32 winMgrSelect(WinMgrSelect* select);
s32 winMgrSelectOther(WinMgrSelect* select, EventEntry* event);
s32 unk_80237448(WinMgrSelect* select);
s32 unk_802379e4(void* a, void* b);
s32 unk_80237a2c(void* a, void* b);
void winMgrDisp(CameraId camId, void* param);
void winMgrSeq(WinMgrEntry* entry);

extern WinMgrDesc select_desc[];
extern WinMgrDesc select_party_desc[];
extern WinMgrDesc select_mario_desc[];
extern WinMgrDesc select_luigi_desc[];
extern WinMgrDesc select_johoya_perfect_desc[];

void select_disp3_mario(WinMgrEntry* entry);
void select_disp3_party(WinMgrEntry* entry);
void select_disp3(WinMgrEntry* entry);
void select_main3(WinMgrEntry* entry);
void select_disp2(WinMgrEntry* entry);
void select_disp_luigi(WinMgrEntry* entry);
void select_disp_mario(WinMgrEntry* entry);
void select_disp_party(WinMgrEntry* entry);
void select_disp(WinMgrEntry* entry);
void select_main(WinMgrEntry* entry);
s32 unk_802399f8(EventEntry* event);
s32 unk_80239b2c(EventEntry* event);
