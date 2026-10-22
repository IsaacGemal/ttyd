#include <string.h>

#include <dolphin/types.h>
#include <dolphin/mtx.h>
#include <dolphin/gx.h>

#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "driver/dispdrv.h"
#include "manager/fontmgr.h"
void FontDrawColor(GXColor color);
#include "memory.h"
#include "evt/evt_twoddisp.h"
#include <stdio.h>


EVT_DEF(twoddisp_main_event);

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
	s32 data;        // 0x24, text pointer or icon id
	f32 scale;       // 0x28
	u32 field_0x2C;  // 0x2c
	u32 field_0x30;  // 0x30
} TwoDDisplayer;     // 0x34

TwoDDisplayer* twodPtr;
u32 twodID;

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

void twoddisp_disp(CameraId cameraId, void* param) {
	s32 i;
	TwoDDisplayer* entry;
	Vec pos;
	char text[0x100];

	for (i = 0; i < 100; i++) {
		entry = &twodPtr[i];
		if (entry->name[0] == 0) {
			continue;
		}
		if (!(entry->flags & 1)) {
			continue;
		}
		if (!(entry->data & 0x80000000)) {
			pos.x = entry->baseX + entry->x;
			pos.y = entry->baseY + entry->y;
			pos.z = 0.0f;
			GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){ 0, 0, 0, 0 });
			iconDispGx(entry->scale, &pos, 0x10, (u16)entry->data);
		} else {
			FontDrawStart();
			FontDrawScale(entry->scale);
			GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){ 0, 0, 0, 0 });
			sprintf(text, (char*)entry->data);
			FontDrawColor((GXColor){ 0x00, 0x00, 0x00, 0xFF });
			FontDrawString(entry->baseX + entry->x, entry->baseY + entry->y, text);
			FontDrawColor((GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
			FontDrawString(entry->baseX + entry->x + 1.0f, entry->baseY + entry->y - 1.0f, text);
		}
	}
}

USER_FUNC(twoddisp_main) {
	u32 i;

	for (i = 0; (s32)i < 100; i++) {
	}
	dispEntry(8, 1000.f, 0, twoddisp_disp, NULL);
	return EVT_RETURN_DONE;
}

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
	twodPtr = _mapAlloc(0x1450);
	for (i = 0; (s32)i < 100; i++) {
		TwoDDisplayer* entry = &twodPtr[i];
		entry->name[0] = 0;
		entry->flags = 0;
		entry->field_0x12 = 0;
		entry->x = 0.0f;
		entry->baseX = 0.0f;
		entry->y = 0.0f;
		entry->baseY = 0.0f;
		entry->data = 0;
		entry->scale = 1.0f;
	}
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
		if (entry->name[0] == 0) {
			strcpy(entry->name, name);
			entry->flags = 1;
			entry->field_0x12 = 0;
			entry->x = 0.0f;
			entry->baseX = 0.0f;
			entry->y = 0.0f;
			entry->baseY = 0.0f;
			entry->data = data;
			entry->scale = 1.0f;
			break;
		}
	}
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

EVT_BEGIN(twoddisp_main_event)
LOOP(0)
CALL(twoddisp_main)
WAIT_FRAMES(1)
END_LOOP
RETURN
EVT_END()

#pragma explicit_zero_data on
__declspec(section ".sdata") u32 gap_07_80411DDC_sdata = 0;
#pragma explicit_zero_data reset
