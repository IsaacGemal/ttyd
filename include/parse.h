#pragma once

#include <dolphin/types.h>

typedef struct ParseWork {
	void* ptr; //0x0, TODO: rename
	s32 field_0x4; //0x4
	u8 field_0x8[0x2C - 0x8]; //0x8
	u32 size; //0x2C, TODO: rename
	u8 field_0x30[0x54 - 0x30]; //0x30
	s32 field_0x54; //0x54
} ParseWork;
void parseInit(void* ptr, u32 size);
BOOL parsePush(const char* tag);
void parsePop(void);
void parsePopNext(void);
BOOL parseTagGet1(const char* tag, s32 type, void* data);
int sscanf(const char* str, const char* format, ...);
