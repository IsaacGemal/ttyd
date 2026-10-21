#pragma once

#include "manager/filemgr.h"
#include <dolphin/types.h>

typedef struct NameEntryWork {
	u16 flags; //0x0, bit 0x1 accepts button input (nameEntWait), bit 0x2 the name was cancelled
	u8 pad_0x2[2]; //0x2
	s32 phase; //0x4, 0 idle, 1 load name.tpl, 2 run nameMain, 3 unload
	s32 step; //0x8, 0 edit the name, 100 yes/no confirm, 101 (0x65) done
	s32 type; //0xC, nameEntOn argument (type 2 blanks the table entry at [9])
	s32 cursorCol; //0x10
	s32 cursorRow; //0x14
	s32 nameLen; //0x18, characters entered (0-8)
	f32 x1; //0x1C
	f32 y1; //0x20
	f32 x2; //0x24
	f32 y2; //0x28
	f32 charCursorX; //0x2C, eases toward charCursorTargetX
	f32 charCursorY; //0x30, eases toward charCursorTargetY
	f32 charCursorTargetX; //0x34
	f32 charCursorTargetY; //0x38
	char name[16]; //0x3C, eight two-byte characters
	u8 pad_0x4C[0x50 - 0x4C]; //0x4C
	const char** charTable; //0x50, hiragana_tbl or katakana_tbl
	f32 maskAlpha; //0x54
	u8 pad_0x58[0x5C - 0x58]; //0x58
	s32 dialogCursor; //0x5C, yes/no confirm: 0 yes, 1 no
	FileEntry* tpl; //0x60, name.tpl
} NameEntryWork;

void nameEntInit(void);
void nameEntReInit(void);
void nameEntMain(void);
BOOL nameEntPrepare(void);
void nameEntOff(void);
char* nameEntGetName(void);
BOOL nameEntWait(void);
void nameEntOn(s32 type);
#include "driver/camdrv.h"
BOOL nameEntIsCancel(void);
void nameWinGX(f32 x, f32 y, f32 z, f32 w, void* tex, s32 arg1);
void nameMaskGX(f32 alpha);
void nameKirinukiGX(f32 x, f32 y, f32 z, f32 w);
void nameBG(void);
void nameEntDisp(CameraId cameraId, void* param);
s32 nameMain(void);
