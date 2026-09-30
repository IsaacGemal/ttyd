#include <string.h>

#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include <dolphin/gx.h>

#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "driver/icondrv.h"
#include "driver/dispdrv.h"
#include "manager/fontmgr.h"
#include "memory.h"
#include "evt/evt_twoddisp.h"
#include <stdio.h>

// FontDrawScale is defined in manager/fontmgr.c but not yet in fontmgr.h.

// 2D display entry work. Not extracted into a header yet.
typedef struct TwoDDisplayer {
	char name[16];   // 0x0
	u16 flags;       // 0x10
	u16 field_0x12;  // 0x12
	f32 x;           // 0x14
	f32 y;           // 0x18
	f32 baseX;       // 0x1c
	f32 baseY;       // 0x20
	s32 field_0x24;  // 0x24, text pointer or icon id
	f32 scale;       // 0x28
	u32 field_0x2C;  // 0x2c
	u32 field_0x30;  // 0x30
} TwoDDisplayer;     // 0x34

TwoDDisplayer* twodPtr;
u32 twodID;

// .sdata2 constants owned by this translation unit (see config/G8MJ01/symbols.txt).
extern const GXColor dat_8041cca8;
extern const GXColor dat_8041ccac;
extern const f32 float_0_8041ccb0;
extern const f32 float_1_8041ccb4;
extern const f32 float_1000_8041ccb8;

// .sbss2 fog colors.
extern GXColor unk_80423048;
extern GXColor unk_8042304c;

static TwoDDisplayer* twodNameToPtr(s32 name) {
	s32 i;
	TwoDDisplayer* entry;

	for (i = 0; i < 100; i++) {
		entry = &twodPtr[i];
		if (!strcmp(entry->name, (const char*)name)) {
			return entry;
		}
	}
	return NULL;
}

//s32* onoff, s32* name, s32* flag
USER_FUNC(evt_twoddisp_flag_onoff) {
	TwoDDisplayer* entry;
	s32* args = event->args;
	s32 onoff;
	s32 name;

	onoff = evtGetValue(event, args[0]);
	name = evtGetValue(event, args[1]);
	entry = twodNameToPtr(name);
	if (onoff) {
		entry->flags |= evtGetValue(event, args[2]);
	} else {
		entry->flags &= ~evtGetValue(event, args[2]);
	}
	return EVT_RETURN_DONE;
}

//s32* name, f32* baseX, f32* baseY
USER_FUNC(evt_twoddisp_set_base_position) {
	TwoDDisplayer* entry;
	s32* args = event->args;
	s32 name;

	name = evtGetValue(event, args[0]);
	entry = twodNameToPtr(name);
	entry->baseX = evtGetFloat(event, args[1]);
	entry->baseY = evtGetFloat(event, args[2]);
	return EVT_RETURN_DONE;
}

//s32* name, f32* x, f32* y
USER_FUNC(evt_twoddisp_set_position) {
	TwoDDisplayer* entry;
	s32* args = event->args;
	s32 name;

	name = evtGetValue(event, args[0]);
	entry = twodNameToPtr(name);
	entry->x = evtGetFloat(event, args[1]);
	entry->y = evtGetFloat(event, args[2]);
	return EVT_RETURN_DONE;
}

//s32* name, s32* data (text pointer or icon id)
USER_FUNC(evt_twoddisp_entry) {
	s32 i;
	s32* args = event->args;
	const char* name;
	s32 data;

	name = (const char*)evtGetValue(event, args[0]);
	data = evtGetValue(event, args[1]);
	for (i = 0; i < 100; i++) {
		TwoDDisplayer* entry = &twodPtr[i];
		strcmp(entry->name, name);
		if ((s8)entry->name[0] == 0) {
			strcpy(entry->name, name);
			entry->flags = 1;
			entry->field_0x12 = 0;
			entry->x = float_0_8041ccb0;
			entry->baseX = float_0_8041ccb0;
			entry->y = float_0_8041ccb0;
			entry->baseY = float_0_8041ccb0;
			entry->field_0x24 = data;
			entry->scale = float_1_8041ccb4;
			break;
		}
	}
	return EVT_RETURN_DONE;
}

EVT_DEF(twoddisp_main_event);

USER_FUNC(evt_twoddisp_init) {
	EventEntry* entry;
	u32 i;

	entry = evtEntryType(twoddisp_main_event, event->priority, 0, event->type);
	entry->unitId = event->unitId;
	entry->ownerNPC = event->ownerNPC;
	for (i = 0; i < 16; i++) {
		entry->lwData[i] = event->lwData[i];
	}
	for (i = 0; i < 3; i++) {
		entry->lfData[i] = event->lfData[i];
	}
	entry->uwBase = event->uwBase;
	entry->ufBase = event->ufBase;
	twodID = entry->eventId;
	twodPtr = (TwoDDisplayer*)_mapAlloc(0x1450);
	for (i = 0; (s32)i < 100; i++) {
		TwoDDisplayer* entry = &twodPtr[i];
		entry->name[0] = 0;
		entry->flags = 0;
		entry->field_0x12 = 0;
		entry->x = float_0_8041ccb0;
		entry->baseX = float_0_8041ccb0;
		entry->y = float_0_8041ccb0;
		entry->baseY = float_0_8041ccb0;
		entry->field_0x24 = 0;
		entry->scale = float_1_8041ccb4;
	}
	return EVT_RETURN_DONE;
}


USER_FUNC(twoddisp_main) {
	u32 i;

	for (i = 0; (s32)i < 100; i++) {
	}
	dispEntry(8, float_1000_8041ccb8, 0, twoddisp_disp, NULL);
	return EVT_RETURN_DONE;
}

#ifdef MUST_MATCH
// Match-only stack shape: passing the font colour by value makes the compiler place all four GXColor
// argument copies in the outgoing-argument area, matching the original's 0x14/0x10/0xc/0x8 slots; the
// natural address-taken locals below land in the other half of the frame.
static inline void twoddisp_fontColor(GXColor color) {
	FontDrawColor(&color);
}
#endif

// NONMATCHING (99.39%): only `clrlwi r5, r0, 16` (original) vs `extsh r5, r0` (ours) differs. The icon id
// is u16 in the original, but include/driver/icondrv.h and src/driver/icondrv.c declare iconDispGx's third
// parameter as s16. Reported, not worked around.
void twoddisp_disp(CameraId cameraId, void* param) {
	s32 offset;
	s32 i;
	TwoDDisplayer* entry;
	Vec pos;
	GXColor color;
	GXColor textColor;
#ifndef MUST_MATCH
	GXColor color1;
	GXColor color2;
#endif
	char text[0x100];

	offset = 0;
	for (i = 0; i < 100; i++, offset += 0x34) {
		entry = (TwoDDisplayer*)((u8*)twodPtr + offset);
		if ((s8)entry->name[0] == 0) {
			continue;
		}
		if (!(entry->flags & 1)) {
			continue;
		}
		if (!(entry->field_0x24 & 0x80000000)) {
			color = unk_80423048;
			pos.x = entry->baseX + entry->x;
			pos.y = entry->baseY + entry->y;
			pos.z = float_0_8041ccb0;
			GXSetFog(GX_FOG_NONE, float_0_8041ccb0, float_0_8041ccb0, float_0_8041ccb0, float_0_8041ccb0, color);
			iconDispGx(&pos, 0x10, entry->field_0x24, entry->scale);
		} else {
			FontDrawStart();
			FontDrawScale(entry->scale);
			textColor = unk_8042304c;
			GXSetFog(GX_FOG_NONE, float_0_8041ccb0, float_0_8041ccb0, float_0_8041ccb0, float_0_8041ccb0, textColor);
			sprintf(text, (char*)entry->field_0x24);
#ifdef MUST_MATCH
			twoddisp_fontColor(dat_8041cca8);
#else
			color1 = dat_8041cca8;
			FontDrawColor(&color1);
#endif
			FontDrawString(text, entry->baseX + entry->x, entry->baseY + entry->y);
#ifdef MUST_MATCH
			twoddisp_fontColor(dat_8041ccac);
#else
			color2 = dat_8041ccac;
			FontDrawColor(&color2);
#endif
			FontDrawString(text, entry->baseX + entry->x + float_1_8041ccb4, entry->baseY + entry->y - float_1_8041ccb4);
		}
	}
}

EVT_BEGIN(twoddisp_main_event)
LOOP(0)
CALL(twoddisp_main)
WAIT_FRAMES(1)
END_LOOP
RETURN
EVT_END()
