#include "parse.h"

//.bss
ParseWork parse;
static u8 buf[128];

void parseInit(void* ptr, u32 size) {
	parse.ptr = ptr;
	parse.field_0x54 = 0;
	parse.field_0x4 = 0;
	parse.size = size;
}





// NONMATCHING (0.84%): empty placeholder, not decompiled yet.
#pragma dont_inline on // stub body must stay a real call for callers to match
BOOL parsePush(const char* tag) {

}
#pragma dont_inline reset





// .text:0x320 | 0x80083D8C | size: 0x18
void parsePop(void) {
	parse.field_0x54--;
}





// .text:0x2F8 | 0x80083D64 | size: 0x28
void parsePopNext(void) {
	parse.field_0x54--;
	((s32*)&parse.field_0x4)[parse.field_0x54] = ((s32*)&parse.field_0x30)[parse.field_0x54];
}





// NONMATCHING (2.89%): 2 instructions differ, 158 missing, plus register differences.
BOOL parseGet1Next(s32 type, void* data) {
	BOOL result;

	if (type == 3) {
		result = parseGet1Next(2, data);
		result *= parseGet1Next(2, (void*)((u32)data + 4));
		result *= parseGet1Next(2, (void*)((u32)data + 4));
	}
	else {

		switch (type) {
		default:
			return TRUE;
		}
	}
	return result;
}





// .text:0x0 | 0x80083A6C | size: 0x68
BOOL parseTagGet1(const char* tag, s32 type, void* data) {
	if (parsePush(tag)) {
		BOOL result = parseGet1Next(type, data);

		parse.field_0x54--;
		return result;
	}
	return FALSE;
}
