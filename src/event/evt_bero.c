#include "evt/evt_bero.h"
#include "driver/hitdrv.h"
#include "driver/seqdrv.h"
#include "evt/evt_cmd.h"
#include "evt/evt_npc.h"
#include "evt/evt_seq.h"
#include "mario/mario.h"
#include "mario/mario_sbr.h"
#include "mario/mariost.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include <dolphin/os.h>
#include "math_ext.h"
#include <string.h>
#include "cam_road.h"
#include "cam_shift.h"
#include "system.h"
#include "battle/ac/ac_table.h"
#include "battle/battle_ac.h"
#include "battle/battle_unit.h"
#include "driver/mobjdrv.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_fade.h"
#include "evt/evt_hit.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_party.h"
#include "evt/evt_snd.h"
#include "evt/evt_sub.h"

#ifdef MUST_MATCH
// the shared backing store of the local name tables; MWCC pools the three identical tables
// into this one object, and each use copies it to the stack.
__declspec(section ".rodata") const char* jumptable_802C4970[8] = {"n_", "ne_", "e_", "se_", "s_", "sw_", "w_", "nw_"};
#endif

extern GlobalWork* gp;

//.bss
BeroEntry* BeroINFOARR[16];
s32 BeroSW[16]; //switches
s32 BeroID[16];

s32 bero_null[15] = {
	0x00000000, 0x0F000000, 0x00004E20, 0x000186A0, 0x00000000, 0x00000000,
	0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000,
	0x00010001, 0x00000000, 0x00000000,
};

s32 bero_case_switch_on[19];
s32 bero_case_switch_off[31];
s32 evt_bero_info_run_pipe_sub00[51];
s32 evt_bero_initial_pos[158];
s32 evt_bero_mario_enter[95];
s32 evt_bero_info_run_none[5];
s32 evt_bero_info_run_gate[5];

// clang-format off
EVT_BEGIN(default_entevt)
SET_VALUE(LW(0), 0)
RETURN
EVT_END()

EVT_BEGIN(default_outevt)
SET_VALUE(LW(0), 0)
RETURN
EVT_END()

//.sbss, NOTE: all are reversed because of -inline deferred
s32 BeroEXEC;
s32 BeroNUM, BeroNOWNUM;
s32 BeroCAM; //only seems to be set in scripts
f32 BeroOX, BeroOY, BeroOZ; //offset position
f32 BeroSX, BeroSY, BeroSZ; //start position
f32 BeroEX, BeroEY, BeroEZ; //end position
Vec BeroPT, BeroAT;
BOOL BeroMarioGO;



//new in retail, f32* x, f32* y, f32* z
USER_FUNC(evt_bero_set_reset_position) {
	s32* args = event->args;
	s32 x, y, z;

	x = evtGetValue(event, args[0]);
	y = evtGetValue(event, args[1]);
	z = evtGetValue(event, args[2]);
	marioSetBottomlessResetPosition((f32)x, (f32)y, (f32)z);
	return EVT_RETURN_DONE;
}

//new in retail, no args
USER_FUNC(evt_bero_set_reset_position_current) {
	MarioWork* wp = marioGetPtr();
	marioSetBottomlessResetPosition(wp->position.x, wp->position.y, wp->position.z);
	return EVT_RETURN_DONE;
}

USER_FUNC(N_evt_bero_set_mario_bottomless_respawn_pos_on_bero_entry) {
	s32* args = event->args;
	s32 x = evtGetValue(event, args[0]);
	s32 y = evtGetValue(event, args[1]);
	s32 z = evtGetValue(event, args[2]);

	N_marioSetBottomlessRespawnPosOnBeroEntry((f32)x, (f32)y, (f32)z);
	return EVT_RETURN_DONE;
}

USER_FUNC(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos) {
	MarioWork* wp = marioGetPtr();

	N_marioSetBottomlessRespawnPosOnBeroEntry(wp->position.x, wp->position.y, wp->position.z);
	return EVT_RETURN_DONE;
}

s32 bero_get_BeroEXEC(void) {
	return BeroEXEC;
}

s32 bero_get_BeroNUM(void) {
	return BeroNUM;
}

f32 bero_get_BeroSX(void) {
	return BeroSX;
}

f32 bero_get_BeroSY(void) {
	return BeroSY;
}

f32 bero_get_BeroSZ(void) {
	return BeroSZ;
}

f32 bero_get_BeroEX(void) {
	return BeroEX;
}

f32 bero_get_BeroEY(void) {
	return BeroEY;
}

f32 bero_get_BeroEZ(void) {
	return BeroEZ;
}

BeroEntry** bero_get_ptr(void) {
	return BeroINFOARR;
}

void bero_clear_Offset(void) {
	BeroOX = 0.0f;
	BeroOY = 0.0f;
	BeroOZ = 0.0f;
}

s32 bero_id_filter(s32 id) { //1:1, cursed function, do not touch
	BeroEntry** temp, **table = BeroINFOARR;
	s32 newId = id;
	const char *string, *test;

	if (id < 0 || id >= 16) {
		test = (const char*)id;
		if (&newId != NULL) {
			newId = 0;
		}
		temp = table;
		while (1) {
			string = (*temp)->hitName;
			if (!string || !strcmp(string, test)) {
				break;
			}
			temp++;
			if (&newId != NULL) {
				newId++;
			}
		}
	}
	return newId;
}

//const char** mapName, const char** beroName
USER_FUNC(evt_bero_mapchange) {
	s32* args = event->args;
	const char *map, *bero;

	map = (const char*)evtGetValue(event, args[0]);
	bero = (const char*)evtGetValue(event, args[1]);
	if (isFirstCall) {
		seqSetSeq(SEQ_MAP_CHANGE, map, bero);
	}
	return EVT_RETURN_BLOCK;
}

//s32 retIndex
USER_FUNC(evt_bero_get_entername) {
	evtSetValue(event, *event->args, (s32)&gp->beroEnterName);
	return EVT_RETURN_DONE;
}

//BOOL* on/off, s32* mask
USER_FUNC(evt_bero_exec_onoff) {
	s32* args = event->args;
	BOOL onoff;
	s32 mask, value;

	onoff = evtGetValue(event, args[0]);
	mask = evtGetValue(event, args[1]);
	value = BeroEXEC | mask;
	if (onoff) {
		value = BeroEXEC & ~mask;
	}
	BeroEXEC = value;
	return EVT_RETURN_DONE;
}

//s32 retIndex
USER_FUNC(evt_bero_exec_get) {
	evtSetValue(event, *event->args, BeroEXEC);
	return EVT_RETURN_DONE;
}

//s32* mask, TODO: add states, change EvtStatus to s32 and do #defines
USER_FUNC(evt_bero_exec_wait) {
	return evtGetValue(event, *event->args) & BeroEXEC ? EVT_RETURN_DONE : 0;
}

//f32 retX, f32 retY, f32 retZ
USER_FUNC(evt_bero_get_start_position) {
	s32* args = event->args;

	evtSetFloat(event, args[0], BeroSX);
	evtSetFloat(event, args[1], BeroSY);
	evtSetFloat(event, args[2], BeroSZ);
	return EVT_RETURN_DONE;
}

//f32 retX, f32 retY, f32 retZ
USER_FUNC(evt_bero_get_end_position) {
	s32* args = event->args;

	evtSetFloat(event, args[0], BeroEX);
	evtSetFloat(event, args[1], BeroEY);
	evtSetFloat(event, args[2], BeroEZ);
	return EVT_RETURN_DONE;
}

//s32* id, s32 retIndex1, s32 retIndex2, s32 retIndex3, s32 retIndex4
USER_FUNC(evt_bero_get_info_anime) {
	BeroEntry* bero;
	s32* args = event->args;

	bero = BeroINFOARR[evtGetValue(event, args[0])];
	evtSetValue(event, args[1], bero->animeArg1);
	evtSetValue(event, args[2], bero->animeArg2);
	evtSetValue(event, args[3], (s32)bero->animeArg3);
	evtSetValue(event, args[4], (s32)bero->animeArg4);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bero_get_info_length) {
	s32* args = event->args;
	BeroEntry* bero;
	s32 length;
	f32 defaultLength;

	args = event->args;
	bero = BeroINFOARR[evtGetValue(event, args[0])];
	length = bero->field_0x18;
	if (length != -1) {
		evtSetFloat(event, args[1], (f32)length);
		return EVT_RETURN_DONE;
	}
	switch ((bero->kinddirArg1) & 0xFFF) {
		case 0:
		case 3:
		case 0xF00:
			defaultLength = 30.0f;
			break;
		case 1:
			defaultLength = 60.0f;
			break;
		case 2:
			defaultLength = 60.0f;
			break;
		default:
			defaultLength = 60.0f;
			break;
	}
	evtSetFloat(event, args[1], defaultLength);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bero_get_info_kinddir) {
	BeroEntry* bero;
	s32* args = event->args;

	bero = BeroINFOARR[evtGetValue(event, args[0])];
	evtSetValue(event, args[1], bero->kinddirArg1);
	evtSetValue(event, args[2], bero->kinddirArg2);
	evtSetValue(event, args[3], bero->kinddirArg3);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bero_get_info_nextarea) {
	BeroEntry* bero;
	s32* args = event->args;

	bero = BeroINFOARR[evtGetValue(event, args[0])];
	evtSetValue(event, args[1], (s32)bero->nextareaArg1);
	evtSetValue(event, args[2], (s32)bero->nextareaArg2);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bero_set_now_number) {
	BeroNOWNUM = evtGetValue(event, *event->args);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bero_get_now_number) {
	evtSetValue(event, *event->args, BeroNOWNUM);
	return EVT_RETURN_DONE;
}

	// NONMATCHING (94.05%): the 16-entry zero fill uses stwu/mr instead of the original's
	// addi base + offset stores, and in the fill loop entry/num are in r5/r6 where the
	// original has them in r6/r5.
USER_FUNC(evt_bero_get_info) {
	BeroEntry** table = BeroINFOARR;
	BeroEntry* entry;
	HitEntry* hit;
	s32* switches;
	int i;

	for (i = 0; i < 16; i++) {
		*table++ = 0;
	}
	table = BeroINFOARR;
	for (entry = (BeroEntry*)event->lwData[0];; entry++) {
		hit = hitNameToPtr(entry->hitName);
		if (!entry->hitName) {
			break;
		}
		if (!hit) {
			mapErrorEntry(1, entry->hitName);
		}
	}
	entry = (BeroEntry*)event->lwData[0];
	switches = BeroSW;
	i = 0;
	while (1) {
		if (!entry->hitName) {
			break;
		}
		*table = entry;
		switches[i] = -1;
		table++;
		i++;
		entry++;
	}
	BeroNUM = i;
	BeroEXEC = 0;
	return EVT_RETURN_DONE;
}

// Matched (100%): using the array-index loop form (no `table` pointer) colours the
// prologue's BeroINFOARR directly into r31, and reusing the outer counter `i` as the name-list
// counter makes the compiler keep the switch's kind in r11 (split from the r31 loop counter).
USER_FUNC(evt_bero_get_into_info) {
	s32 kind = 0;
	s32 i;
	BeroEntry* entry;

	for (i = 0; ; i++) {
		entry = BeroINFOARR[i];
		if (entry == NULL || !strcmp(gp->beroEnterName, "")) {
			i = 0;
			entry = (BeroEntry*)bero_null;
			break;
		}
		if (!strcmp(entry->hitName, gp->beroEnterName)) {
			break;
		}
	}
	if ((gp->flags & 2) && !strcmp(gp->beroEnterName, "")) {
		entry = BeroINFOARR[0];
		event->lwData[1] = 0;
		event->lwData[2] = BeroNUM;
		event->lwData[3] = (s32)entry->hitName;
		event->lwData[4] = entry->kinddirArg1;
		kind = entry->kinddirArg3;
		{
#ifdef MUST_MATCH
			// the screen name table; the original's compiler shares one backing store across the three uses
			const char* names[8];
			names[0] = jumptable_802C4970[0];
			names[1] = jumptable_802C4970[1];
			names[2] = jumptable_802C4970[2];
			names[3] = jumptable_802C4970[3];
			names[4] = jumptable_802C4970[4];
			names[5] = jumptable_802C4970[5];
			names[6] = jumptable_802C4970[6];
			names[7] = jumptable_802C4970[7];
#else
			const char* names[8] = {"n_", "ne_", "e_", "se_", "s_", "sw_", "w_", "nw_"};
#endif
			switch (kind) {
			case 0x4E20:
				if (kind == 0x2710) {
					kind = 0x2710;
				} else {
					switch (*(u16*)&entry->kinddirArg1 & 0xFFF) {
					case 0:
					case 1:
						for (i = 0; (u32)i < 8U; i++) {
							if (!strncmp(names[i], entry->hitName, strlen(names[i]))) {
								break;
							}
						}
						break;
					default:
						i = 0x2710;
						break;
					}
					kind = i;
				}
				break;
			}
		}
		event->lwData[5] = kind;
		event->lwData[6] = (s32)&entry->field_0xC;
		// a pointer temp rather than a ternary gives the original's cmplwi/bne layout
		{
			s32* ev = entry->enterEvent;
			if (ev == NULL) {
				ev = default_entevt;
			}
			event->lwData[7] = (s32)ev;
		}
		event->lwData[8] = 0xffff0000;
		return EVT_RETURN_DONE;
	} else {
		event->lwData[1] = i;
		event->lwData[2] = BeroNUM;
		event->lwData[3] = (s32)gp->beroEnterName;
		event->lwData[4] = entry->kinddirArg1;
		kind = entry->kinddirArg3;
		{
#ifdef MUST_MATCH
			// the screen name table; the original's compiler shares one backing store across the three uses
			const char* names[8];
			names[0] = jumptable_802C4970[0];
			names[1] = jumptable_802C4970[1];
			names[2] = jumptable_802C4970[2];
			names[3] = jumptable_802C4970[3];
			names[4] = jumptable_802C4970[4];
			names[5] = jumptable_802C4970[5];
			names[6] = jumptable_802C4970[6];
			names[7] = jumptable_802C4970[7];
#else
			const char* names[8] = {"n_", "ne_", "e_", "se_", "s_", "sw_", "w_", "nw_"};
#endif
			switch (kind) {
			case 0x4E20:
				if (kind == 0x2710) {
					kind = 0x2710;
				} else {
					switch (*(u16*)&entry->kinddirArg1 & 0xFFF) {
					case 0:
					case 1:
						for (kind = 0; (u32)kind < 8U; kind++) {
							if (!strncmp(names[kind], entry->hitName, strlen(names[kind]))) {
								break;
							}
						}
						break;
					default:
						kind = 0x2710;
						break;
					}
				}
				break;
			}
		}
		event->lwData[5] = kind;
		event->lwData[6] = (s32)&entry->field_0xC;
		// a pointer temp rather than a ternary gives the original's cmplwi/bne layout
		{
			s32* ev = entry->enterEvent;
			if (ev == NULL) {
				ev = default_entevt;
			}
			event->lwData[7] = (s32)ev;
		}
		event->lwData[8] = 0xffff0000;
		return EVT_RETURN_DONE;
	}
}

USER_FUNC(evt_bero_read_mario_pera) {
	if (isFirstCall) {
		event->userdata[0] = 0;
	}
	switch (event->userdata[0]) {
	default:
		break;
	// the empty case 2 makes MWCC emit the retail switch decision tree
	// (cmpwi/beq/bge/b) instead of folding the default edges together.
	case 2:
		break;
	case 0:
#ifdef MUST_MATCH
		// MUST_MATCH: the original string object carries two trailing padding bytes.
		if (animGroupBaseAsync("p_dokan_x\0\0", 0, 0)) {
#else
		if (animGroupBaseAsync("p_dokan_x", 0, 0)) {
#endif
			event->userdata[0] = 1;
		}
		break;
	case 1:
#ifdef MUST_MATCH
		if (animGroupBaseAsync("p_dokan_y\0\0", 0, 0)) {
#else
		if (animGroupBaseAsync("p_dokan_y", 0, 0)) {
#endif
			event->userdata[0] = 100;
		}
		break;
	}
	return event->userdata[0] == 100 ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

// NOTE (100%): the filter body is inlined from bero_id_filter(id); writing it inline
// directly gave the wrong callee-saved assignment (args r31, test r29).
USER_FUNC(evt_bero_id_filter) {
	s32* args = event->args;
	s32 id = evtGetValue(event, args[0]);

	evtSetValue(event, args[0], bero_id_filter(id));
	return EVT_RETURN_DONE;
}

// NOTE (100%): the local names[] backing store is emitted as an anonymous .rodata symbol
// (@NNN) instead of the original's local jumptable_802C4970; objdiff scores this 100% but
// still flags the two relocations. The enter/exitEvent null-checks are shaped for match
// only (see the comments at those sites).
USER_FUNC(evt_bero_get_info_num) {
	BeroEntry* entry;
	s32 kind;
	s32 id;

	id = evtGetValue(event, event->args[0]);
	entry = BeroINFOARR[id];
	event->lwData[3] = (s32)entry->hitName;
	event->lwData[4] = *(u16*)&entry->kinddirArg1;
	kind = entry->kinddirArg3;
	{
		
#ifdef MUST_MATCH
			// the screen name table; the original's compiler shares one backing store across the three uses
			const char* names[8];
			names[0] = jumptable_802C4970[0];
			names[1] = jumptable_802C4970[1];
			names[2] = jumptable_802C4970[2];
			names[3] = jumptable_802C4970[3];
			names[4] = jumptable_802C4970[4];
			names[5] = jumptable_802C4970[5];
			names[6] = jumptable_802C4970[6];
			names[7] = jumptable_802C4970[7];
#else
			const char* names[8] = {"n_", "ne_", "e_", "se_", "s_", "sw_", "w_", "nw_"};
#endif
	switch (kind) {
	case 0x4E20:
		if (kind == 0x2710) {
			kind = 0x2710;
		} else {
			switch (*(u16*)&entry->kinddirArg1 & 0xFFF) {
			case 0:
			case 1:
				for (kind = 0; (u32)kind < 8U; kind++) {
					if (!strncmp(names[kind], entry->hitName, strlen(names[kind]))) {
						break;
					}
				}
				break;
			default:
				kind = 0x2710;
				break;
			}
		}
		break;
	}
	}
	event->lwData[5] = kind;
	event->lwData[6] = (s32)&entry->field_0xC;
	// a pointer temp rather than a ternary gives the original's cmplwi/bne layout
	{
		s32* ev = entry->enterEvent;
		if (ev == NULL) {
			ev = default_entevt;
		}
		event->lwData[7] = (s32)ev;
	}
	event->lwData[8] = 0xffff0000;
	event->lwData[9] = BeroSW[id];
	event->lwData[10] = entry->field_0x20;
	if (entry->field_0x20 == -1) {
		switch (*(u16*)&entry->kinddirArg1 & 0xFFF) {
		case 0xF00:
			event->lwData[10] = 0;
			break;
		case 0:
			event->lwData[10] = 10;
			break;
		case 1:
			event->lwData[10] = 9;
			break;
		case 2:
			event->lwData[10] = 4;
			break;
		case 3:
			event->lwData[10] = 7;
			break;
		}
	}
	// a pointer temp rather than a ternary gives the original's cmplwi/bne layout
	{
		s32* ev = entry->exitEvent;
		if (ev == NULL) {
			ev = default_outevt;
		}
		event->lwData[11] = (s32)ev;
	}
	event->lwData[12] = 0xffff0000;
	event->lwData[13] = (s32)entry->nextareaArg1;
	event->lwData[14] = (s32)entry->nextareaArg2;
	return EVT_RETURN_DONE;
}

USER_FUNC(bero_get_position_normal) {
	s32* args = event->args;
	MarioWork* mario = marioGetPtr();
	s32 reverse;
	f32 length;
	s32 kind;
	s32* p;
	const char* hitName;
	Vec start, end;
	// the compiler lays locals out in reverse declaration order, so this order
	// reproduces the original's stack map (hitX at 0x08, e at 0x30).
	Vec e, ray;
	f32 hitNZ, hitNY, hitNX, distance;
	f32 hitZ, hitY, hitX;
	f32 sn, cs, rad;
	f32 angle;
	f32 len;

	reverse = evtGetValue(event, args[0]);
	length = evtGetFloat(event, args[1]);
	p = (s32*)event->lwData[6];
	hitName = (const char*)event->lwData[3];
	kind = event->lwData[5];

	start.x = (f32)p[0];
	start.y = (f32)p[1];
	start.z = (f32)p[2];
	end.x = (f32)p[0];
	end.y = (f32)p[1];
	end.z = (f32)p[2];

	if (p[0] == 0x186A0) {
		hitObjGetPos(hitName, &start);
	}
	if (length != 0.0f) {
		hitX = 0.0f;
		hitY = 0.0f;
		hitZ = 0.0f;
		distance = 1000000.0f;
		angle = 360.0f * (f32)kind / 8.0f;
		if (reverse == 1) {
			angle += 180.0f;
		}
		rad = 6.2831855f * angle / 360.0f;
		sn = (f32)sin(rad);
		ray.x = start.x - 30.0f * sn;
		ray.y = start.y;
		cs = (f32)cos(rad);
		ray.z = start.z - 30.0f * -cs;
		// the original writes distance twice; the compiler keeps both stores.
		distance = 1000000.0f;
		if (hitCheckFilter(NULL, ray.x, ray.y, ray.z, 0.0f, -1.0f, 0.0f, &hitX, &hitY,
		                   &hitZ, &distance, &hitNX, &hitNY, &hitNZ) != NULL) {
			ray.y = hitY;
		}
		e = ray;
		sn = (f32)sin(rad);
		len = 30.0f + length;
		e.x += len * sn;
		cs = (f32)cos(rad);
		e.z += len * -cs;
		start = ray;
		end = e;
	}
	BeroSX = start.x;
	BeroSY = start.y;
	BeroSZ = start.z;
	BeroEX = end.x;
	BeroEY = end.y;
	BeroEZ = end.z;
	N_marioSetBottomlessRespawnPosOnBeroEntry(BeroEX, BeroEY, BeroEZ);
	if (mario->currMotionId == MARIO_MOTION_SHIP) {
		N_marioReloadMapOnBottomlessOn();
	}
	return EVT_RETURN_DONE;
}

void bero_get_position_pipe_sub(s32 reverse, s32 length, s32 hitName, s32 kind, Vec* start, Vec* end) {
	f32 hitX = 0.0f;
	f32 hitY = 0.0f;
	f32 hitZ = 0.0f;
	f32 distance = 10000.0f;
	f32 hitNX;
	f32 hitNY;
	f32 hitNZ;
	// hitY goes through a plain temp so the compiler loads it once for the two stores
	// instead of reloading it from its stack slot.
	f32 y;
	f32 off = 20.0f;

	switch (kind) {
	case 6:
		length = -length;
		off = -off;
	case 2:
		if (reverse == 0) {
			hitObjGetPos((const char*)hitName, start);
			start->x -= off;
			end->x = start->x + (f32)length;
			end->y = start->y;
			end->z = start->z;
			hitCheckFilter(NULL, end->x, end->y, end->z, 0.0f, -1.0f, 0.0f, &hitX, &hitY,
			               &hitZ, &distance, &hitNX, &hitNY, &hitNZ);
		} else {
			hitObjGetPos((const char*)hitName, end);
			end->x -= off;
			start->x = end->x + (f32)length;
			start->y = end->y;
			start->z = end->z;
			hitCheckFilter(NULL, start->x, start->y, start->z, 0.0f, -1.0f, 0.0f, &hitX,
			               &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ);
		}
		y = hitY;
		start->y = y;
		end->y = y;
		break;
	case 9:
		length = -length;
	case 8:
		if (reverse == 0) {
			hitObjGetPos((const char*)hitName, start);
			end->x = start->x;
			end->y = start->y + (f32)length;
			end->z = start->z;
		} else {
			hitObjGetPos((const char*)hitName, end);
			start->x = end->x;
			start->y = end->y + (f32)length;
			start->z = end->z;
		}
		break;
	case 7:
	case 0x2710:
	default:
		hitObjGetPos((const char*)hitName, start);
		hitObjGetPos((const char*)hitName, end);
		break;
	}
}

	// NONMATCHING (97.50%): kind/reverse are in r31/r30 where the original has r30/r31, and the
	// BeroEY ternary is stored directly from f2 instead of the original's f0 + frsp f2,f0.
USER_FUNC(bero_get_position_pipe_pure) {
	s32* args = event->args;
	s32 reverse;
	s32 kind;
	s32 length;
	Vec start, end;

	reverse = evtGetValue(event, args[0]);
	length = (s32)evtGetFloat(event, args[1]);
	kind = event->lwData[5];
	bero_get_position_pipe_sub(reverse, length, event->lwData[3], kind, &start, &end);
	BeroSX = start.x;
	BeroSY = kind == 8 ? start.y - 37.5f : start.y;
	BeroSZ = start.z;
	BeroEX = end.x;
	if (kind == 8) {
		BeroEY = end.y - 37.5f;
	} else {
		BeroEY = end.y;
	}
	BeroEZ = end.z;
	N_marioSetBottomlessRespawnPosOnBeroEntry(BeroEX, BeroEY, BeroEZ);
	return EVT_RETURN_DONE;
}

// NOTE (100%): BeroPT/BeroAT go through local Vecs assigned a (Vec){0,0,0} compound
// literal at the store site, then field-written from work; the direct struct copy
// (BeroPT = work.pos) drops the two zero-vector temps the original keeps.
USER_FUNC(bero_get_position_pipe2) {
	s32* args = event->args;
	s32 reverse = evtGetValue(event, args[0]);
	s32 length = (s32)evtGetFloat(event, args[1]);
	Vec start, end;
	Vec bpt, bat;
	CameraParams work;

	bero_get_position_pipe_sub(reverse, length, event->lwData[3], event->lwData[5], &start, &end);
	end.y += 1.0f;
	camRoadReset();
	camShiftReset();
	camRoadMain(&work.fovY, end.x, end.y, end.z, end.x, end.y, end.z, 0.0f);
	bpt = (Vec){ 0.0f, 0.0f, 0.0f };
	bpt.x = work.pos.x;
	bpt.y = work.pos.y;
	bpt.z = work.pos.z;
	bat = (Vec){ 0.0f, 0.0f, 0.0f };
	bat.x = work.at.x;
	bat.y = work.at.y;
	bat.z = work.at.z;
	BeroPT = bpt;
	BeroAT = bat;
	return EVT_RETURN_DONE;
}

USER_FUNC(bero_set_disp_position_pipe) {
	MarioWork* wp;

	switch (event->lwData[5]) {
	case 2:
	case 6:
		if (!(event->lwData[4] & 0x1000)) {
			wp = marioGetPtr();
			wp->dispOfs.y += 3.0f;
			wp = marioGetPtr();
			wp->dispOfs.z -= 15.0f;
		}
		break;
	case 8:
	case 9:
		if (!(event->lwData[4] & 0x1000)) {
			wp = marioGetPtr();
			wp->dispOfs.z -= 13.0f;
		} else {
			wp = marioGetPtr();
			wp->dispOfs.z -= 20.0f;
		}
		break;
	case 7:
	case 0x2710:
	default:
		break;
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bero_switch_on) {
	BeroEntry** temp, **table = BeroINFOARR;
	s32* args = event->args;
	s32 id = evtGetValue(event, *args++);
	s32 newId = id;
	const char *string, *test;

	if (id < 0 || id >= 16) {
		test = (const char*)id;
		if (&newId != NULL) {
			newId = 0;
		}
		temp = table;
		while (1) {
			string = (*temp)->hitName;
			if (!string || !strcmp(string, test)) {
				break;
			}
			temp++;
			if (&newId != NULL) {
				newId++;
			}
		}
	}
	if (BeroSW[newId] == 1) {
		event->lwData[0] = 1;
	} else {
		event->lwData[0] = 0;
	}
	BeroSW[newId] = 1;
	event->lwData[1] = newId;
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bero_switch_off) {
	BeroEntry** temp, **table = BeroINFOARR;
	s32* args = event->args;
	s32 id = evtGetValue(event, *args++);
	s32 newId = id;
	const char *string, *test;

	if (id < 0 || id >= 16) {
		test = (const char*)id;
		if (&newId != NULL) {
			newId = 0;
		}
		temp = table;
		while (1) {
			string = (*temp)->hitName;
			if (!string || !strcmp(string, test)) {
				break;
			}
			temp++;
			if (&newId != NULL) {
				newId++;
			}
		}
	}
	if (BeroSW[newId] != 1) {
		event->lwData[0] = 1;
	} else {
		event->lwData[0] = 0;
	}
	BeroSW[newId] = 0;
	event->lwData[1] = newId;
	return EVT_RETURN_DONE;
}

	// NONMATCHING (99.58%): register allocation / branch layout differs from the original
USER_FUNC(evt_bero_get_number) {
	BeroEntry** temp, **table = BeroINFOARR;
	s32* args = event->args;
	s32 id = evtGetValue(event, *args++);
	s32 newId = id;
	const char *string, *test;

	if (id < 0 || id >= 16) {
		test = (const char*)id;
		if (&newId != NULL) {
			newId = 0;
		}
		temp = table;
		while (1) {
			string = (*temp)->hitName;
			if (!string || !strcmp(string, test)) {
				break;
			}
			temp++;
			if (&newId != NULL) {
				newId++;
			}
		}
	}
	event->lwData[1] = newId;
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bero_case_id_save) {
	s32* args = event->args;
	s32 id = evtGetValue(event, args[0]);

	BeroID[id] = evtGetValue(event, args[1]);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bero_case_id_load) {
	s32* args = event->args;
	s32 id = evtGetValue(event, args[0]);

	evtSetValue(event, args[1], BeroID[id]);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bero_disppos_save) {
	MarioWork* wp;

	wp = marioGetPtr();
	BeroOX = wp->dispOfs.x;
	wp = marioGetPtr();
	BeroOY = wp->dispOfs.y;
	wp = marioGetPtr();
	BeroOZ = wp->dispOfs.z;
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bero_disppos_load) {
	MarioWork* wp;
	f32 pos;

	pos = BeroOX;
	wp = marioGetPtr();
	wp->dispOfs.x = pos;
	pos = BeroOY;
	wp = marioGetPtr();
	wp->dispOfs.y = pos;
	pos = BeroOZ;
	wp = marioGetPtr();
	wp->dispOfs.z = pos;
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bero_move_mario_speed) {
	f32 speed = evtGetFloat(event, event->args[0]);
	OSTime now = gp->inBattle ? gp->renderTime : gp->renderFieldTime;
	MarioWork* mario;
	f32 sec, sn, cs, rad, dist, dx, dy, vx, vy, bsx, bsy;
	s32 frames;
	BOOL reachedX, reachedY;

	if (isFirstCall) {
		event->savedTime = now;
	}
	sec = sysFrame2SecFloat(speed);
	rad = 6.2831855f * reviseAngle(angleABf(BeroSX, BeroSY, BeroEX, BeroEY)) / 360.0f;
	sn = (f32)sin(rad);
	cs = (f32)cos(rad);
	frames = sysMsec2Frame((s32)OSTicksToMilliseconds((u64)(s64)(s32)((s32)now - (s32)event->savedTime)));
	vx = sec * sn;
	vy = -(sec * cs);
	dist = sec * (f32)frames;
	dx = sn * dist;
	dy = -(cs * dist);
	// loading BeroSX/BeroSY into locals here extends their scratch webs so the
	// allocator keeps the int->double magic in f1 and the products in f2, retail's colours.
	bsx = BeroSX;
	marioGetPtr()->position.x = bsx + dx;
	bsy = BeroSY;
	marioGetPtr()->position.y = bsy + dy;
	marioGetPtr()->position.z = BeroSZ;
	reachedX = 0;
	if (vx >= 0 && BeroEX <= marioGetPtr()->position.x) {
		reachedX = 1;
	}
	if (vx < 0 && BeroEX >= marioGetPtr()->position.x) {
		reachedX = 1;
	}
	reachedY = 0;
	if (vy >= 0 && BeroEY <= marioGetPtr()->position.y) {
		reachedY = 1;
	}
	if (vy < 0 && BeroEY >= marioGetPtr()->position.y) {
		reachedY = 1;
	}
	if (reachedX + reachedY != 2) {
		return 0;
	}
	marioGetPtr()->position.x = BeroEX;
	marioGetPtr()->position.y = BeroEY;
	marioGetPtr()->position.z = BeroEZ;
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bero_overwrite) {
	BeroEntry** temp, **table = BeroINFOARR;
	s32* args = event->args;
	s32 id = evtGetValue(event, args[0]);
	s32 newId = id;
	const char *string, *test;

	if (id < 0 || id >= 16) {
		test = (const char*)id;
		if (&newId != NULL) {
			newId = 0;
		}
		temp = table;
		while (1) {
			string = (*temp)->hitName;
			if (!string || !strcmp(string, test)) {
				break;
			}
			temp++;
			if (&newId != NULL) {
				newId++;
			}
		}
	}
	BeroINFOARR[newId] = (BeroEntry*)evtGetValue(event, args[1]);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bero_cam3d_change) {
	s32* args = event->args;
	CameraEntry* cam = camGetPtr(CAMERA_3D);
	s32 time = evtGetValue(event, args[0]);
	s32 curve = evtGetValue(event, args[1]);

	cam->presetPos = cam->cameraPos;
	cam->presetAt = cam->cameraAt;
	cam->storedPos = BeroPT;
	cam->storedAt = BeroAT;
	cam->startTimeA = gp->renderTime;
	cam->durationA = OSMillisecondsToTicks(time);
	cam->stateA = 3;
	cam->curveA = curve;
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bero_cam_shift_init) {
	camRoadReset();
	camShiftReset();
	return EVT_RETURN_DONE;
}

// ---- reconstructed functions ----

USER_FUNC(evt_bero_mario_go_init) {
	BeroMarioGO = 0;
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bero_mario_go) {
	BeroMarioGO = 1;
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bero_mario_go_wait) {
	return BeroMarioGO == 0 ? EVT_RETURN_BLOCK : EVT_RETURN_DONE;
}

USER_FUNC(evt_bero_1stcheck) {
	s32* args = event->args;

	if (!strcmp((const char*)event->lwData[13], "sys_01")) {
		evtSetValue(event, args[0], 1);
	} else {
		evtSetValue(event, args[0], 0);
	}
	return EVT_RETURN_DONE;
}

// TODO: type
s32 evt_bero_nonride_check[37] = {
    0x00020019, 0xFE363C8A, 0x00000006, 0x00020032,
    0xFE363C80, 0xFE363C83, 0x0001005E, (s32)&bero_case_switch_on,
    0x00000002, 0x00000021, 0x00010005, 0x00000000,
    0x0002005B, (s32)evt_mario_get_motion, 0xFE363C80, 0x00020018,
    0xFE363C80, 0x00000003, 0x00000007, 0x00000021,
    0x00020018, 0xFE363C80, 0x0000000A, 0x00000007,
    0x00000021, 0x00010009, 0x00000001, 0x00000006,
    0x00010071, (s32)"土管から降りた", 0x00020032, 0xFE363C80,
    0xFE363C83, 0x0001005E, (s32)&bero_case_switch_on, 0x00000002,
    0x00000001,
};

// TODO: type
s32 bero_open_door_play_se[188] = {
    0x0002005B, (s32)evt_bero_get_info_num, 0xFE363C81, 0x00020032,
    0xFE363C84, 0x00000000, 0x00020032, 0xFE363C85,
    0x00000000, 0x00020032, 0xFE363C86, 0x00000000,
    0x00020019, 0xFE363C83, 0x00000000, 0x0005005B,
    (s32)evt_hit_get_position, 0xFE363C83, 0xFE363C84, 0xFE363C85,
    0xFE363C86, 0x00000021, 0x0005005B, (s32)evt_bero_get_info_kinddir,
    0xFE363C81, 0xFE363C8A, 0xFE363C8B, 0xFE363C8C,
    0x00010022, 0xFE363C8B, 0x00010024, 0x00000000,
    0x00010024, 0x00000001, 0x0006005B, (s32)evt_snd_sfxon_3d,
    (s32)"SFX_DOOR_OPEN1", 0xFE363C84, 0xFE363C85, 0xFE363C86,
    0x00000000, 0x00010024, 0x00000002, 0x0006005B,
    (s32)evt_snd_sfxon_3d, (s32)"SFX_DOOR_OPEN_WOOD1", 0xFE363C84, 0xFE363C85,
    0xFE363C86, 0x00000000, 0x00010024, 0x00000003,
    0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_DOOR_OPEN_WOOD2", 0xFE363C84,
    0xFE363C85, 0xFE363C86, 0x00000000, 0x00010024,
    0x00000004, 0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_DOOR_OPEN_WOOD3",
    0xFE363C84, 0xFE363C85, 0xFE363C86, 0x00000000,
    0x00010024, 0x00000005, 0x0006005B, (s32)evt_snd_sfxon_3d,
    (s32)"SFX_DOOR_OPEN_IRON1", 0xFE363C84, 0xFE363C85, 0xFE363C86,
    0x00000000, 0x00010024, 0x00000006, 0x0006005B,
    (s32)evt_snd_sfxon_3d, (s32)"SFX_DOOR_OPEN_IRON2", 0xFE363C84, 0xFE363C85,
    0xFE363C86, 0x00000000, 0x00010024, 0x00000007,
    0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_DOOR_OPEN_GRILLE1", 0xFE363C84,
    0xFE363C85, 0xFE363C86, 0x00000000, 0x00010024,
    0x00000008, 0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_DOOR_OPEN_SLIDE1",
    0xFE363C84, 0xFE363C85, 0xFE363C86, 0x00000000,
    0x00010024, 0x00000009, 0x0006005B, (s32)evt_snd_sfxon_3d,
    (s32)"SFX_DOOR_OPEN_TURN1", 0xFE363C84, 0xFE363C85, 0xFE363C86,
    0x00000000, 0x00010024, 0x0000000A, 0x0006005B,
    (s32)evt_snd_sfxon_3d, (s32)"SFX_DOOR_OPEN_ELEC1", 0xFE363C84, 0xFE363C85,
    0xFE363C86, 0x00000000, 0x00010024, 0x0000000B,
    0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_DOOR_OPEN_STONE1", 0xFE363C84,
    0xFE363C85, 0xFE363C86, 0x00000000, 0x00010024,
    0x0000000C, 0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_DOOR_OPEN_CASTLE1",
    0xFE363C84, 0xFE363C85, 0xFE363C86, 0x00000000,
    0x00010024, 0x0000000D, 0x0006005B, (s32)evt_snd_sfxon_3d,
    (s32)"SFX_DOOR_OPEN_CASTLE2", 0xFE363C84, 0xFE363C85, 0xFE363C86,
    0x00000000, 0x00010024, 0x0000000E, 0x0006005B,
    (s32)evt_snd_sfxon_3d, (s32)"SFX_DOOR_OPEN_METAL1", 0xFE363C84, 0xFE363C85,
    0xFE363C86, 0x00000000, 0x00010024, 0x0000000F,
    0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_STG3_AUTO_DOOR1", 0xFE363C84,
    0xFE363C85, 0xFE363C86, 0x00000000, 0x00010024,
    0x00000010, 0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_STG3_LOCKER_OPEN1",
    0xFE363C84, 0xFE363C85, 0xFE363C86, 0x00000000,
    0x00010024, 0x00000011, 0x0006005B, (s32)evt_snd_sfxon_3d,
    (s32)"SFX_DOOR_OPEN_SLIDE1_3", 0xFE363C84, 0xFE363C85, 0xFE363C86,
    0x00000000, 0x00000031, 0x00000002, 0x00000001,
};

// TODO: type
s32 bero_close_door_play_se[188] = {
    0x0002005B, (s32)evt_bero_get_info_num, 0xFE363C81, 0x00020032,
    0xFE363C84, 0x00000000, 0x00020032, 0xFE363C85,
    0x00000000, 0x00020032, 0xFE363C86, 0x00000000,
    0x00020019, 0xFE363C83, 0x00000000, 0x0005005B,
    (s32)evt_hit_get_position, 0xFE363C83, 0xFE363C84, 0xFE363C85,
    0xFE363C86, 0x00000021, 0x0005005B, (s32)evt_bero_get_info_kinddir,
    0xFE363C81, 0xFE363C8A, 0xFE363C8B, 0xFE363C8C,
    0x00010022, 0xFE363C8B, 0x00010024, 0x00000000,
    0x00010024, 0x00000001, 0x0006005B, (s32)evt_snd_sfxon_3d,
    (s32)"SFX_DOOR_SHUT1", 0xFE363C84, 0xFE363C85, 0xFE363C86,
    0x00000000, 0x00010024, 0x00000002, 0x0006005B,
    (s32)evt_snd_sfxon_3d, (s32)"SFX_DOOR_SHUT_WOOD1", 0xFE363C84, 0xFE363C85,
    0xFE363C86, 0x00000000, 0x00010024, 0x00000003,
    0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_DOOR_SHUT_WOOD2", 0xFE363C84,
    0xFE363C85, 0xFE363C86, 0x00000000, 0x00010024,
    0x00000004, 0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_DOOR_SHUT_WOOD2",
    0xFE363C84, 0xFE363C85, 0xFE363C86, 0x00000000,
    0x00010024, 0x00000005, 0x0006005B, (s32)evt_snd_sfxon_3d,
    (s32)"SFX_DOOR_SHUT_IRON1", 0xFE363C84, 0xFE363C85, 0xFE363C86,
    0x00000000, 0x00010024, 0x00000006, 0x0006005B,
    (s32)evt_snd_sfxon_3d, (s32)"SFX_DOOR_SHUT_IRON2", 0xFE363C84, 0xFE363C85,
    0xFE363C86, 0x00000000, 0x00010024, 0x00000007,
    0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_DOOR_SHUT_GRILLE1", 0xFE363C84,
    0xFE363C85, 0xFE363C86, 0x00000000, 0x00010024,
    0x00000008, 0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_DOOR_SHUT_SLIDE1",
    0xFE363C84, 0xFE363C85, 0xFE363C86, 0x00000000,
    0x00010024, 0x00000009, 0x0006005B, (s32)evt_snd_sfxon_3d,
    (s32)"SFX_DOOR_SHUT_TURN1", 0xFE363C84, 0xFE363C85, 0xFE363C86,
    0x00000000, 0x00010024, 0x0000000A, 0x0006005B,
    (s32)evt_snd_sfxon_3d, (s32)"SFX_DOOR_SHUT_ELEC1", 0xFE363C84, 0xFE363C85,
    0xFE363C86, 0x00000000, 0x00010024, 0x0000000B,
    0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_DOOR_OPEN_STONE1", 0xFE363C84,
    0xFE363C85, 0xFE363C86, 0x00000000, 0x00010024,
    0x0000000C, 0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_DOOR_SHUT_CASTLE1",
    0xFE363C84, 0xFE363C85, 0xFE363C86, 0x00000000,
    0x00010024, 0x0000000D, 0x0006005B, (s32)evt_snd_sfxon_3d,
    (s32)"SFX_DOOR_SHUT_CASTLE2", 0xFE363C84, 0xFE363C85, 0xFE363C86,
    0x00000000, 0x00010024, 0x0000000E, 0x0006005B,
    (s32)evt_snd_sfxon_3d, (s32)"SFX_DOOR_SHUT_METAL1", 0xFE363C84, 0xFE363C85,
    0xFE363C86, 0x00000000, 0x00010024, 0x0000000F,
    0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_STG3_AUTO_DOOR1", 0xFE363C84,
    0xFE363C85, 0xFE363C86, 0x00000000, 0x00010024,
    0x00000010, 0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_STG3_LOCKER_CLOSE1",
    0xFE363C84, 0xFE363C85, 0xFE363C86, 0x00000000,
    0x00010024, 0x00000011, 0x0006005B, (s32)evt_snd_sfxon_3d,
    (s32)"SFX_DOOR_SHUT_SLIDE1_3", 0xFE363C84, 0xFE363C85, 0xFE363C86,
    0x00000000, 0x00000031, 0x00000002, 0x00000001,
};

// NOTE (100%): like bero_get_position_pipe2, cam->storedPos/storedAt go through two
// local Vecs assigned a (Vec){0,0,0} compound literal at the store site and then
// field-written from work; the direct struct copy drops the original's zero temps.
USER_FUNC(evt_camera_change_event_from_road) {
	MarioWork* mario = marioGetPtr();
	CameraEntry* cam = camGetPtr(CAMERA_3D);
	CameraParams work;
	Vec bpt, bat;
	f32 out;

	camShiftMain(cam, mario, &out);
	camRoadMain(&work.fovY, mario->position.x, cam->marioPos.y, mario->position.z,
	            mario->position.x, cam->marioPos.y, mario->position.z, out);
	bpt = (Vec){ 0.0f, 0.0f, 0.0f };
	bpt.x = work.pos.x;
	bpt.y = work.pos.y;
	bpt.z = work.pos.z;
	bat = (Vec){ 0.0f, 0.0f, 0.0f };
	bat.x = work.at.x;
	bat.y = work.at.y;
	bat.z = work.at.z;
	cam->presetPos = cam->cameraPos;
	cam->presetAt = cam->cameraAt;
	cam->storedPos = bpt;
	cam->storedAt = bat;
	cam->startTimeA = gp->renderTime;
	cam->durationA = OSMillisecondsToTicks(500);
	cam->stateA = 3;
	cam->curveA = 0xb;
	return EVT_RETURN_DONE;
}

// TODO: type
s32 bero_play_enter_anime[147] = {
    0x0006005B, (s32)evt_bero_get_info_anime, 0xFE363C81, 0xFE363C8A,
    0xFE363C8B, 0xFE363C8C, 0xFE363C8D, 0x00010022,
    0xFE363C8A, 0x00010024, 0x00000000, 0x00010024,
    0x00000001, 0x0000006B, 0x0001000A, 0x000000C8,
    0x0001005B, (s32)evt_bero_mario_go, 0x0000006D, 0x00020019,
    0xFE363C8C, 0x00000000, 0x0004005B, (s32)evt_map_playanim,
    0xFE363C8C, 0x00000000, 0x00000000, 0x0003005B,
    (s32)evt_map_pauseanim, 0x00000000, 0xFE363C8C, 0x0001000A,
    0x000002BC, 0x0001005C, (s32)&bero_close_door_play_se, 0x0003005B,
    (s32)evt_map_replayanim, 0x00000000, 0xFE363C8C, 0x00000021,
    0x00010024, 0x00000002, 0x00020018, 0xFE363C8C,
    0x00000000, 0x00000030, 0x00000021, 0x00020032,
    0xFE363C88, 0xFE363C8C, 0x0006005B, (s32)evt_mapobj_rotate,
    0x00000001, 0xFE363C88, 0x00000000, 0xFFFFFFB0,
    0x00000000, 0x0001005B, (s32)evt_bero_mario_go, 0x0001000A,
    0x000001F4, 0x0001005C, (s32)&bero_close_door_play_se, 0x0005005B,
    (s32)evt_sub_intpl_msec_init, 0x0000000B, 0xFFFFFFB0, 0x00000000,
    0x000001F4, 0x00010005, 0x00000000, 0x0003005B,
    (s32)evt_sub_intpl_msec_get_value_para, 0xFE363C80, 0xFE363C81, 0x0006005B,
    (s32)evt_mapobj_rotate, 0x00000001, 0xFE363C88, 0x00000000,
    0xFE363C80, 0x00000000, 0x00020018, 0xFE363C81,
    0x00000000, 0x00000007, 0x00000021, 0x00010009,
    0x00000001, 0x00000006, 0x00010024, 0x00000004,
    0x00020019, 0xFE363C8C, 0x00000000, 0x0001005C,
    0xFE363C8C, 0x00000021, 0x00010024, 0x00000005,
    0x00020019, 0xFE363C8C, 0x00000000, 0x00020034,
    0xFE363C80, 0xF24D4A80, 0x0001005E, 0xFE363C8C,
    0x0001005B, (s32)evt_bero_mario_go, 0x0001000A, 0x000001F4,
    0x0001005C, (s32)&bero_close_door_play_se, 0x00020032, 0xFE363C89,
    0xFE363C8C, 0x0005005B, (s32)evt_sub_intpl_msec_init, 0x0000000B,
    0x000000B4, 0x00000000, 0x000001F4, 0x00010005,
    0x00000000, 0x0003005B, (s32)evt_sub_intpl_msec_get_value_para, 0xFE363C80,
    0xFE363C8A, 0x00020018, 0xFE363C8A, 0x00000000,
    0x00020034, 0xFE363C80, 0xF24A7A80, 0x0001005E,
    0xFE363C89, 0x00000007, 0x00000020, 0x0001005E,
    0xFE363C89, 0x00000021, 0x00000006, 0x00000021,
    0x00000031, 0x00000002, 0x00000001,
};

// TODO: type
s32 bero_play_out_anime[133] = {
    0x0006005B, (s32)evt_bero_get_info_anime, 0xFE363C81, 0xFE363C8A,
    0xFE363C8B, 0xFE363C8C, 0xFE363C8D, 0x00010022,
    0xFE363C8B, 0x00010024, 0x00000000, 0x00010024,
    0x00000001, 0x00020019, 0xFE363C8D, 0x00000000,
    0x0001005C, (s32)&bero_open_door_play_se, 0x0004005B, (s32)evt_map_playanim,
    0xFE363C8D, 0x00000000, 0x00000000, 0x0000006B,
    0x0004005B, (s32)evt_map_checkanim, 0xFE363C8D, 0xFE363C88,
    0xFE363C89, 0x0001000A, 0xFE363C89, 0x0001005B,
    (s32)evt_bero_mario_go, 0x0000006D, 0x00000020, 0x0000006B,
    0x0001000A, 0x000001F4, 0x0001005B, (s32)evt_bero_mario_go,
    0x0000006D, 0x00000021, 0x00010024, 0x00000002,
    0x00020018, 0xFE363C8C, 0x00000000, 0x00000030,
    0x00000021, 0x00020032, 0xFE363C88, 0xFE363C8C,
    0x0001005C, (s32)&bero_open_door_play_se, 0x0005005B, (s32)evt_sub_intpl_msec_init,
    0x0000000B, 0x00000000, 0x00000050, 0x000001F4,
    0x00010005, 0x00000000, 0x0003005B, (s32)evt_sub_intpl_msec_get_value_para,
    0xFE363C80, 0xFE363C81, 0x0006005B, (s32)evt_mapobj_rotate,
    0x00000001, 0xFE363C88, 0x00000000, 0xFE363C80,
    0x00000000, 0x00020018, 0xFE363C81, 0x00000000,
    0x00000007, 0x00000021, 0x00010009, 0x00000001,
    0x00000006, 0x0001005B, (s32)evt_bero_mario_go, 0x00010024,
    0x00000004, 0x00020019, 0xFE363C8D, 0x00000000,
    0x0001005C, 0xFE363C8D, 0x00000021, 0x00010024,
    0x00000005, 0x00020019, 0xFE363C8D, 0x00000000,
    0x0001005C, (s32)&bero_open_door_play_se, 0x00020032, 0xFE363C89,
    0xFE363C8D, 0x0005005B, (s32)evt_sub_intpl_msec_init, 0x0000000B,
    0x00000000, 0x000000B4, 0x000001F4, 0x00010005,
    0x00000000, 0x0003005B, (s32)evt_sub_intpl_msec_get_value_para, 0xFE363C80,
    0xFE363C8A, 0x00020018, 0xFE363C8A, 0x00000000,
    0x00020034, 0xFE363C80, 0xF24D4A80, 0x0001005E,
    0xFE363C89, 0x00000007, 0x00000020, 0x0001005E,
    0xFE363C89, 0x00000021, 0x00000006, 0x0001005B,
    (s32)evt_bero_mario_go, 0x00000021, 0x00000031, 0x00000002,
    0x00000001,
};

// TODO: type
s32 bero_case_init_check[99] = {
    0x0003005B, (s32)evt_bero_exec_onoff, 0x00000001, 0x00010000,
    0x0002005B, (s32)evt_bero_exec_get, 0xFE363C80, 0x0002004E,
    0xFE363C80, 0x00000001, 0x00020019, 0xFE363C80,
    0x00000000, 0x00020032, 0xFE363C80, 0x00000001,
    0x00000002, 0x00000021, 0x0002005B, (s32)evt_npc_change_fbat_mode,
    0x00000007, 0x0001005B, (s32)evt_npc_stop_for_event, 0x00020032,
    0xFE363C80, 0xFE363C84, 0x0002004E, 0xFE363C80,
    0x00000FFF, 0x00010022, 0xFE363C80, 0x0001002B,
    0x00000000, 0x0001002B, 0x00000001, 0x0003005B,
    (s32)evt_bero_get_info_length, 0xFE363C81, 0xFE363C80, 0x0003005B,
    (s32)bero_get_position_normal, 0x00000000, 0xFE363C80, 0x0000002E,
    0x0001002B, 0x00000F00, 0x0001002B, 0x00000003,
    0x0003005B, (s32)bero_get_position_normal, 0x00000000, 0xF24A7A80,
    0x0000002E, 0x00010024, 0x00000002, 0x0003005B,
    (s32)evt_bero_get_info_length, 0xFE363C81, 0xFE363C80, 0x0003005B,
    (s32)bero_get_position_pipe_pure, 0x00000000, 0xFE363C80, 0x00000031,
    0x0002005B, (s32)evt_bero_set_now_number, 0xFE363C81, 0x0003005B,
    (s32)evt_bero_exec_onoff, 0x00000000, 0x00000001, 0x00020032,
    0xFE363C8F, 0x00000000, 0x0001005E, 0xFE363C8B,
    0x00020018, 0xFE363C80, 0x00000001, 0x0002005B,
    (s32)evt_npc_change_fbat_mode, 0x00000001, 0x0001005B, (s32)evt_npc_start_for_event,
    0x0003005B, (s32)evt_bero_exec_onoff, 0x00000000, 0x00010000,
    0x0003005B, (s32)evt_bero_exec_onoff, 0x00000001, 0x00000001,
    0x00020032, 0xFE363C80, 0x00000001, 0x00000002,
    0x00000021, 0x00000002, 0x00000001,
};

// TODO: type
s32 bero_case_exec_none[45] = {
    0x0002005B, (s32)evt_mario_key_onoff, 0x00000000, 0x0001005E,
    (s32)&bero_case_init_check, 0x0002005B, (s32)evt_mario_key_onoff, 0x00000001,
    0x00020018, 0xFE363C80, 0x00000001, 0x00000002,
    0x00000021, 0x0002005B, (s32)evt_mario_key_onoff, 0x00000000,
    0x0003005B, (s32)evt_bero_exec_onoff, 0x00000000, 0x00010000,
    0x0003005B, (s32)evt_bero_exec_onoff, 0x00000001, 0x00000001,
    0x0003005B, (s32)evt_bero_exec_onoff, 0x00000000, 0x00000008,
    0x00010009, 0x00000002, 0x0002005B, (s32)evt_bero_1stcheck,
    0xFE363C80, 0x00020018, 0xFE363C80, 0x00000001,
    0x0001000A, 0x000003E8, 0x00000021, 0x0003005B,
    (s32)evt_bero_mapchange, 0xFE363C8D, 0xFE363C8E, 0x00000002,
    0x00000001,
};

// TODO: type
s32 bero_case_exec_road[75] = {
    0x0002005B, (s32)evt_mario_key_onoff, 0x00000000, 0x0001005E,
    (s32)&bero_case_init_check, 0x0002005B, (s32)evt_mario_key_onoff, 0x00000001,
    0x00020018, 0xFE363C80, 0x00000001, 0x00000002,
    0x00000021, 0x0002005B, (s32)evt_mario_key_onoff, 0x00000000,
    0x0000006B, 0x00020032, 0xFE363C80, 0xFE363C85,
    0x00020037, 0xFE363C80, 0x0000002D, 0x0002005B,
    (s32)evt_mario_set_force_dir, 0xFE363C80, 0x0001005B, (s32)evt_mario_wait_landon,
    0x0003005B, (s32)bero_get_position_normal, 0x00000000, 0xF24CD280,
    0x00020056, 0xFE363C8A, (s32)&BeroEX, 0x00020056,
    0xFE363C8C, (s32)&BeroEZ, 0x0004005B, (s32)evt_mario_mov_pos2,
    0xFE363C8A, 0xFE363C8C, 0xF24CD280, 0x0003005B,
    (s32)evt_bero_exec_onoff, 0x00000000, 0x00010000, 0x0003005B,
    (s32)evt_bero_exec_onoff, 0x00000001, 0x00000001, 0x0000006D,
    0x0001000A, 0x00000096, 0x0003005B, (s32)evt_bero_exec_onoff,
    0x00000000, 0x00000008, 0x00010009, 0x00000002,
    0x0002005B, (s32)evt_bero_1stcheck, 0xFE363C80, 0x00020018,
    0xFE363C80, 0x00000001, 0x0001000A, 0x000003E8,
    0x00000021, 0x0003005B, (s32)evt_bero_mapchange, 0xFE363C8D,
    0xFE363C8E, 0x00000002, 0x00000001,
};

// TODO: type
s32 bero_case_exec_door[89] = {
    0x0002005B, (s32)evt_mario_key_onoff, 0x00000000, 0x0001005E,
    (s32)&bero_case_init_check, 0x0002005B, (s32)evt_mario_key_onoff, 0x00000001,
    0x00020018, 0xFE363C80, 0x00000001, 0x00000002,
    0x00000021, 0x0002005B, (s32)evt_mario_key_onoff, 0x00000000,
    0x0005005B, (s32)evt_hitobj_attr_onoff, 0x00000000, 0x00000001,
    0xFE363C83, 0x00800000, 0x0001005B, (s32)evt_bero_mario_go_init,
    0x0001005C, (s32)&bero_play_out_anime, 0x0000006B, 0x00020032,
    0xFE363C80, 0xFE363C85, 0x00020037, 0xFE363C80,
    0x0000002D, 0x0002005B, (s32)evt_mario_set_force_dir, 0xFE363C80,
    0x0001005B, (s32)evt_mario_wait_landon, 0x0001005B, (s32)evt_bero_mario_go_wait,
    0x0003005B, (s32)bero_get_position_normal, 0x00000000, 0xF24CD280,
    0x00020056, 0xFE363C8A, (s32)&BeroEX, 0x00020056,
    0xFE363C8C, (s32)&BeroEZ, 0x0004005B, (s32)evt_mario_mov_pos2,
    0xFE363C8A, 0xFE363C8C, 0xF24CD280, 0x0003005B,
    (s32)evt_bero_exec_onoff, 0x00000000, 0x00010000, 0x0003005B,
    (s32)evt_bero_exec_onoff, 0x00000001, 0x00000001, 0x0000006D,
    0x0001005B, (s32)evt_bero_mario_go_wait, 0x0001000A, 0x00000096,
    0x0003005B, (s32)evt_bero_exec_onoff, 0x00000000, 0x00000008,
    0x00010009, 0x00000002, 0x0002005B, (s32)evt_bero_1stcheck,
    0xFE363C80, 0x00020018, 0xFE363C80, 0x00000001,
    0x0001000A, 0x000003E8, 0x00000021, 0x0003005B,
    (s32)evt_bero_mapchange, 0xFE363C8D, 0xFE363C8E, 0x00000002,
    0x00000001,
};

// TODO: type
s32 bero_case_exec_pipe_sub[333] = {
    0x0000006B, 0x0001005B, (s32)evt_camera_change_event_from_road, 0x0000006D,
    0x00020018, 0xFE363C8D, 0x00000000, 0x0000006B,
    0x00020032, 0xFE363C80, 0xFE363C84, 0x0002004E,
    0xFE363C80, 0x00001000, 0x00020018, 0xFE363C80,
    0x00000000, 0x00020032, 0xFBD3E280, 0x00000000,
    0x00000020, 0x00020032, 0xFBD3E280, 0x00000001,
    0x00000021, 0x0002005B, (s32)evt_bero_get_number, 0xFE363C8E,
    0x0002005B, (s32)evt_bero_set_now_number, 0xFE363C81, 0x0002005B,
    (s32)evt_bero_get_info_num, 0xFE363C81, 0x00020032, 0xFE363C80,
    0xFE363C84, 0x0002004E, 0xFE363C80, 0x00001000,
    0x00020018, 0xFE363C80, 0x00000000, 0x00020032,
    0xFBD3E281, 0x00000000, 0x00000020, 0x00020032,
    0xFBD3E281, 0x00000001, 0x00000021, 0x00020032,
    0xFBD3E282, 0x00000000, 0x00020018, 0xFBD3E280,
    0x00000000, 0x00020018, 0xFBD3E281, 0x00000001,
    0x00020032, 0xFBD3E282, 0x00000001, 0x00000021,
    0x00000021, 0x00020018, 0xFBD3E282, 0x00000000,
    0x00020053, 0x00000000, (s32)&BeroCAM, 0x0003005B,
    (s32)evt_bero_get_info_length, 0xFE363C81, 0xFE363C8A, 0x0003005B,
    (s32)bero_get_position_pipe2, 0x00000001, 0xFE363C8A, 0x00010005,
    0x00000000, 0x00020055, 0xFE363C80, (s32)&BeroCAM,
    0x00020018, 0xFE363C80, 0x00000001, 0x00000007,
    0x00000021, 0x00010009, 0x00000001, 0x00000006,
    0x0003005B, (s32)evt_bero_cam3d_change, 0x000001F4, 0x0000000B,
    0x00000021, 0x0000006D, 0x00000021, 0x0001005B,
    (s32)evt_bero_disppos_save, 0x0003005B, (s32)evt_bero_get_info_length, 0xFE363C81,
    0xFE363C8A, 0x0003005B, (s32)bero_get_position_pipe_pure, 0x00000000,
    0xFE363C8A, 0x0001005B, (s32)bero_set_disp_position_pipe, 0x0005005B,
    (s32)evt_mario_get_pos, 0x00000000, 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_MARIO_DOKAN1",
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x00000000,
    0x00010022, 0xFE363C85, 0x00010024, 0x00000006,
    0x0002005B, (s32)evt_mario_paper_pose_dokan, 0x00000005, 0x0002005B,
    (s32)evt_bero_move_mario_speed, 0xF24B6A80, 0x00010024, 0x00000002,
    0x0002005B, (s32)evt_mario_paper_pose_dokan, 0x00000005, 0x0002005B,
    (s32)evt_bero_move_mario_speed, 0xF24B6A80, 0x00010024, 0x00000008,
    0x0002005B, (s32)evt_mario_paper_pose_dokan, 0x00000002, 0x0002005B,
    (s32)evt_bero_move_mario_speed, 0xF24B6A80, 0x00010024, 0x00000009,
    0x0002005B, (s32)evt_mario_paper_pose_dokan, 0x00000003, 0x0002005B,
    (s32)evt_bero_move_mario_speed, 0xF24B6A80, 0x00000031, 0x00020053,
    0x00000001, (s32)&BeroCAM, 0x00020018, 0xFE363C8D,
    0x00000000, 0x00020032, 0xFE363C80, 0xFE363C84,
    0x0002004E, 0xFE363C80, 0x00001000, 0x00020018,
    0xFE363C80, 0x00000000, 0x00020032, 0xFBD3E280,
    0x00000000, 0x00000020, 0x00020032, 0xFBD3E280,
    0x00000001, 0x00000021, 0x0001000A, 0x000001F4,
    0x0002005B, (s32)evt_bero_get_number, 0xFE363C8E, 0x0002005B,
    (s32)evt_bero_get_info_num, 0xFE363C81, 0x0001005E, 0xFE363C87,
    0x00020032, 0xFE363C80, 0xFE363C84, 0x0002004E,
    0xFE363C80, 0x00001000, 0x00020018, 0xFE363C80,
    0x00000000, 0x00020032, 0xFBD3E281, 0x00000000,
    0x00000020, 0x00020032, 0xFBD3E281, 0x00000001,
    0x00000021, 0x0001005B, (s32)evt_bero_disppos_load, 0x0001005B,
    (s32)evt_mario_paper_pose_off, 0x0001005B, (s32)evt_bero_disppos_save, 0x0002005B,
    (s32)evt_bero_exec_get, 0xFE363C80, 0x0002001F, 0xFE363C80,
    0x00000004, 0x0003005B, (s32)evt_bero_exec_onoff, 0x00000000,
    0x00000002, 0x00000021, 0x0003005B, (s32)evt_bero_get_info_length,
    0xFE363C81, 0xFE363C8A, 0x0003005B, (s32)bero_get_position_pipe_pure,
    0x00000001, 0xFE363C8A, 0x0001005B, (s32)bero_set_disp_position_pipe,
    0x0001000A, 0x000001F4, 0x00020019, 0xFBD3E281,
    0x00000000, 0x0001005B, (s32)evt_mario_bgmode_on, 0x00000020,
    0x0001005B, (s32)evt_mario_bgmode_off, 0x00000021, 0x00020056,
    0xFE363C80, (s32)&BeroEX, 0x00020056, 0xFE363C81,
    (s32)&BeroEY, 0x00020056, 0xFE363C82, (s32)&BeroEZ,
    0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_MARIO_DOKAN1", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000000, 0x0001005E,
    (s32)&evt_bero_info_run_pipe_sub00, 0x0002005B, (s32)evt_bero_exec_get, 0xFE363C80,
    0x0002001F, 0xFE363C80, 0x00000004, 0x0003005B,
    (s32)evt_bero_exec_onoff, 0x00000001, 0x00000002, 0x00000021,
    0x0001005B, (s32)evt_bero_disppos_load, 0x0002005B, (s32)evt_mario_cont_onoff,
    0x00000001, 0x0002005B, (s32)evt_mario_key_onoff, 0x00000001,
    0x00020032, 0xFE363C80, 0xFE363C83, 0x0001005E,
    (s32)&bero_case_switch_off, 0x0001005C, (s32)&evt_bero_nonride_check, 0x0001005B,
    (s32)evt_bero_cam_shift_init, 0x0003005B, (s32)evt_cam3d_evt_off, 0x000001F4,
    0x0000000B, 0x00000020, 0x0001005B, (s32)evt_bero_disppos_load,
    0x0001005B, (s32)evt_mario_paper_pose_off, 0x00020032, 0xFE363C80,
    0xFE363C84, 0x0002004E, 0xFE363C80, 0x00001000,
    0x00020019, 0xFE363C80, 0x00000000, 0x0001005B,
    (s32)evt_mario_bgmode_on, 0x00000020, 0x0001005B, (s32)evt_mario_bgmode_off,
    0x00000021, 0x00000021, 0x0002005B, (s32)evt_npc_change_fbat_mode,
    0x00000001, 0x0001005B, (s32)evt_npc_start_for_event, 0x0003005B,
    (s32)evt_bero_exec_onoff, 0x00000000, 0x00010000, 0x0003005B,
    (s32)evt_bero_exec_onoff, 0x00000001, 0x00000001, 0x00000002,
    0x00000001,
};

// TODO: type
s32 bero_case_exec_pipe[178] = {
    0x0002005B, (s32)evt_mario_get_mode, 0xFE363C80, 0x00020019,
    0xFE363C80, 0x00000000, 0x00000002, 0x00000021,
    0x0002005B, (s32)evt_mario_key_onoff, 0x00000000, 0x0001005E,
    (s32)&bero_case_init_check, 0x0002005B, (s32)evt_mario_key_onoff, 0x00000001,
    0x00020018, 0xFE363C80, 0x00000001, 0x00000002,
    0x00000021, 0x0002005B, (s32)evt_mario_key_onoff, 0x00000000,
    0x0001005B, (s32)evt_mario_wait_landon, 0x0002005B, (s32)evt_mario_cont_onoff,
    0x00000000, 0x00020018, 0xFE363C8D, 0x00000000,
    0x0001005C, (s32)&bero_case_exec_pipe_sub, 0x00000002, 0x00000021,
    0x0001005B, (s32)evt_mario_cam_off, 0x0005005B, (s32)evt_mario_get_pos,
    0x00000000, 0xFE363C80, 0xFE363C81, 0xFE363C82,
    0x0006005B, (s32)evt_snd_sfxon_3d, (s32)"SFX_MARIO_DOKAN1", 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x00000000, 0x00010022,
    0xFE363C85, 0x00010024, 0x00000006, 0x0002005B,
    (s32)evt_mario_paper_pose_dokan, 0x00000005, 0x0002005B, (s32)evt_bero_move_mario_speed,
    0xF24B6A80, 0x00010024, 0x00000002, 0x0002005B,
    (s32)evt_mario_paper_pose_dokan, 0x00000005, 0x0002005B, (s32)evt_bero_move_mario_speed,
    0xF24B6A80, 0x00010024, 0x00000008, 0x0002005B,
    (s32)evt_mario_paper_pose_dokan, 0x00000002, 0x0002005B, (s32)evt_bero_move_mario_speed,
    0xF24B6A80, 0x00010024, 0x00000009, 0x0002005B,
    (s32)evt_mario_paper_pose_dokan, 0x00000003, 0x0002005B, (s32)evt_bero_move_mario_speed,
    0xF24B6A80, 0x00000031, 0x0002001F, 0xFE363C84,
    0x00002000, 0x00010022, 0xFE363C85, 0x00010024,
    0x00000006, 0x0006005B, (s32)evt_fade_set_mapchange_type, 0x00000001,
    0x0000002D, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF,
    0x0006005B, (s32)evt_fade_set_mapchange_type, 0x00000000, 0x0000002D,
    0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00010024,
    0x00000002, 0x0006005B, (s32)evt_fade_set_mapchange_type, 0x00000001,
    0x0000002F, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF,
    0x0006005B, (s32)evt_fade_set_mapchange_type, 0x00000000, 0x0000002F,
    0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00010024,
    0x00000008, 0x0006005B, (s32)evt_fade_set_mapchange_type, 0x00000001,
    0x00000029, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF,
    0x0006005B, (s32)evt_fade_set_mapchange_type, 0x00000000, 0x00000029,
    0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00010024,
    0x00000009, 0x0006005B, (s32)evt_fade_set_mapchange_type, 0x00000001,
    0x0000002B, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF,
    0x0006005B, (s32)evt_fade_set_mapchange_type, 0x00000000, 0x0000002B,
    0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000031,
    0x00000021, 0x0003005B, (s32)evt_bero_exec_onoff, 0x00000000,
    0x00000008, 0x00010009, 0x00000002, 0x0002005B,
    (s32)evt_bero_1stcheck, 0xFE363C80, 0x00020018, 0xFE363C80,
    0x00000001, 0x0001000A, 0x000003E8, 0x00000021,
    0x0003005B, (s32)evt_bero_mapchange, 0xFE363C8D, 0xFE363C8E,
    0x00000002, 0x00000001,
};

// TODO: type
s32 bero_case_exec_gate[45] = {
    0x0002005B, (s32)evt_mario_key_onoff, 0x00000000, 0x0001005E,
    (s32)&bero_case_init_check, 0x0002005B, (s32)evt_mario_key_onoff, 0x00000001,
    0x00020018, 0xFE363C80, 0x00000001, 0x00000002,
    0x00000021, 0x0002005B, (s32)evt_mario_key_onoff, 0x00000000,
    0x0003005B, (s32)evt_bero_exec_onoff, 0x00000000, 0x00010000,
    0x0003005B, (s32)evt_bero_exec_onoff, 0x00000001, 0x00000001,
    0x0003005B, (s32)evt_bero_exec_onoff, 0x00000000, 0x00000008,
    0x00010009, 0x00000002, 0x0002005B, (s32)evt_bero_1stcheck,
    0xFE363C80, 0x00020018, 0xFE363C80, 0x00000001,
    0x0001000A, 0x000003E8, 0x00000021, 0x0003005B,
    (s32)evt_bero_mapchange, 0xFE363C8D, 0xFE363C8E, 0x00000002,
    0x00000001,
};

// TODO: type
s32 bero_case_entry[100] = {
    0x00020055, 0xFE363C82, (s32)&BeroNUM, 0x00020032,
    0xFE363C81, 0x00000000, 0x00010005, 0x00000000,
    0x0002005B, (s32)evt_bero_get_info_num, 0xFE363C81, 0x00020018,
    0xFE363C89, 0xFFFFFFFF, 0x00020032, 0xFE363C80,
    0xFE363C84, 0x0002004E, 0xFE363C80, 0x00000FFF,
    0x00010022, 0xFE363C80, 0x00010024, 0x00000F00,
    0x0007005B, (s32)evt_run_case_evt_bero, 0xFE363C8A, 0x00000001,
    0xFE363C83, 0x00000000, (s32)&bero_case_exec_none, 0xFE363C8F,
    0x00010024, 0x00000000, 0x0005005B, (s32)evt_hitobj_attr_onoff,
    0x00000000, 0x00000001, 0xFE363C83, 0x00800000,
    0x0007005B, (s32)evt_run_case_evt_bero, 0xFE363C8A, 0x00000001,
    0xFE363C83, 0x00000000, (s32)&bero_case_exec_road, 0xFE363C8F,
    0x00010024, 0x00000001, 0x0007005B, (s32)evt_run_case_evt_bero,
    0xFE363C8A, 0x00000001, 0xFE363C83, 0x00000000,
    (s32)&bero_case_exec_door, 0xFE363C8F, 0x00010024, 0x00000002,
    0x0007005B, (s32)evt_run_case_evt_bero, 0xFE363C8A, 0x00000001,
    0xFE363C83, 0x00000000, (s32)&bero_case_exec_pipe, 0xFE363C8F,
    0x00010024, 0x00000003, 0x0007005B, (s32)evt_run_case_evt_bero,
    0xFE363C8A, 0x00000001, 0xFE363C83, 0x00000000,
    (s32)&bero_case_exec_gate, 0xFE363C8F, 0x00000031, 0x0003005B,
    (s32)evt_bero_case_id_save, 0xFE363C81, 0xFE363C8F, 0x0000006B,
    0x0002005B, (s32)evt_bero_switch_on, 0xFE363C81, 0x0000006D,
    0x00000021, 0x00020035, 0xFE363C81, 0x00000001,
    0x00020018, 0xFE363C81, 0xFE363C82, 0x00000007,
    0x00000021, 0x00000006, 0x00000002, 0x00000001,
};

// TODO: type
s32 bero_case_entry_sub[71] = {
    0x00020032, 0xFE363C80, 0xFE363C84, 0x0002004E,
    0xFE363C80, 0x00000FFF, 0x00010022, 0xFE363C80,
    0x00010024, 0x00000F00, 0x0007005B, (s32)evt_run_case_evt_bero,
    0xFE363C8A, 0x00000001, 0xFE363C83, 0x00000000,
    (s32)&bero_case_exec_none, 0xFE363C8F, 0x00010024, 0x00000000,
    0x0005005B, (s32)evt_hitobj_attr_onoff, 0x00000000, 0x00000001,
    0xFE363C83, 0x00800000, 0x0007005B, (s32)evt_run_case_evt_bero,
    0xFE363C8A, 0x00000001, 0xFE363C83, 0x00000000,
    (s32)&bero_case_exec_road, 0xFE363C8F, 0x00010024, 0x00000001,
    0x0007005B, (s32)evt_run_case_evt_bero, 0xFE363C8A, 0x00000001,
    0xFE363C83, 0x00000000, (s32)&bero_case_exec_door, 0xFE363C8F,
    0x00010024, 0x00000002, 0x0007005B, (s32)evt_run_case_evt_bero,
    0xFE363C8A, 0x00000001, 0xFE363C83, 0x00000000,
    (s32)&bero_case_exec_pipe, 0xFE363C8F, 0x00010024, 0x00000003,
    0x0007005B, (s32)evt_run_case_evt_bero, 0xFE363C8A, 0x00000001,
    0xFE363C83, 0x00000000, (s32)&bero_case_exec_gate, 0xFE363C8F,
    0x00000031, 0x0003005B, (s32)evt_bero_case_id_save, 0xFE363C81,
    0xFE363C8F, 0x00000002, 0x00000001,
};

// TODO: type
s32 bero_case_switch_on[19] = {
    0x0002005B, (s32)evt_bero_switch_on, 0xFE363C80, 0x0002005B,
    (s32)evt_bero_get_info_num, 0xFE363C81, 0x00020018, 0xFE363C80,
    0x00000000, 0x00010071, (s32)"ベロ登録", 0x0001005E,
    (s32)&bero_case_entry_sub, 0x00000020, 0x00010071, (s32)"ベロの出て行くデータの二重登録防止をしています",
    0x00000021, 0x00000002, 0x00000001,
};

// TODO: type
s32 bero_case_switch_off[31] = {
    0x0002005B, (s32)evt_bero_switch_off, 0xFE363C80, 0x0002005B,
    (s32)evt_bero_get_info_num, 0xFE363C81, 0x00020018, 0xFE363C80,
    0x00000000, 0x00010071, (s32)"ベロ削除しました", 0x0003005B,
    (s32)evt_bero_case_id_load, 0xFE363C81, 0xFE363C8F, 0x0003005B,
    (s32)evt_del_case_evt, 0x00000000, 0xFE363C8F, 0x0005005B,
    (s32)evt_hitobj_attr_onoff, 0x00000000, 0x00000000, 0xFE363C83,
    0x00800000, 0x00000020, 0x00010071, (s32)"ベロの削除予約もしくはベロの出て行くデータの二重削除防止です",
    0x00000021, 0x00000002, 0x00000001,
};

// TODO: type
s32 evt_bero_info_run_road[41] = {
    0x00020056, 0xFE363C8D, (s32)&BeroEX, 0x00020056,
    0xFE363C8E, (s32)&BeroEY, 0x00020056, 0xFE363C8F,
    (s32)&BeroEZ, 0x0003005B, (s32)evt_party_set_homing_dist, 0x00000000,
    0xF24ACA80, 0x0003005B, (s32)evt_party_set_homing_dist, 0x00000001,
    0xF24ACA80, 0x0004005B, (s32)evt_mario_mov_pos2, 0xFE363C8D,
    0xFE363C8F, 0xF24CD280, 0x0002005B, (s32)evt_mario_key_onoff,
    0x00000001, 0x0002005B, (s32)evt_party_run, 0x00000000,
    0x0002005B, (s32)evt_party_run, 0x00000001, 0x0003005B,
    (s32)evt_party_set_homing_dist, 0x00000000, 0xF24B4280, 0x0003005B,
    (s32)evt_party_set_homing_dist, 0x00000001, 0xF24B4280, 0x00000002,
    0x00000001,
};

// TODO: type
s32 evt_bero_info_run_door[47] = {
    0x0001005B, (s32)evt_bero_mario_go_init, 0x0001005C, (s32)&bero_play_enter_anime,
    0x0001005B, (s32)evt_bero_mario_go_wait, 0x00020056, 0xFE363C8D,
    (s32)&BeroEX, 0x00020056, 0xFE363C8E, (s32)&BeroEY,
    0x00020056, 0xFE363C8F, (s32)&BeroEZ, 0x0003005B,
    (s32)evt_party_set_homing_dist, 0x00000000, 0xF24ACA80, 0x0003005B,
    (s32)evt_party_set_homing_dist, 0x00000001, 0xF24ACA80, 0x0004005B,
    (s32)evt_mario_party_bero_move, 0xFE363C8D, 0xFE363C8F, 0xF24CD280,
    0x0002005B, (s32)evt_mario_key_onoff, 0x00000001, 0x0002005B,
    (s32)evt_party_run, 0x00000000, 0x0002005B, (s32)evt_party_run,
    0x00000001, 0x0003005B, (s32)evt_party_set_homing_dist, 0x00000000,
    0xF24B4280, 0x0003005B, (s32)evt_party_set_homing_dist, 0x00000001,
    0xF24B4280, 0x00000002, 0x00000001,
};

// TODO: type
s32 evt_bero_info_run_pipe_sub00[51] = {
    0x0002005B, (s32)evt_party_stop, 0x00000000, 0x0002005B,
    (s32)evt_party_stop, 0x00000001, 0x00010022, 0xFE363C85,
    0x00010024, 0x00000006, 0x0002005B, (s32)evt_mario_set_dispdir,
    0x000000B4, 0x0002005B, (s32)evt_mario_paper_pose_dokan, 0x00000004,
    0x0002005B, (s32)evt_bero_move_mario_speed, 0xF24B6A80, 0x00010024,
    0x00000002, 0x0002005B, (s32)evt_mario_set_dispdir, 0x00000000,
    0x0002005B, (s32)evt_mario_paper_pose_dokan, 0x00000004, 0x0002005B,
    (s32)evt_bero_move_mario_speed, 0xF24B6A80, 0x00010024, 0x00000009,
    0x0002005B, (s32)evt_mario_paper_pose_dokan, 0x00000001, 0x0002005B,
    (s32)evt_bero_move_mario_speed, 0xF24B6A80, 0x00010024, 0x00000008,
    0x0002005B, (s32)evt_mario_paper_pose_dokan, 0x00000000, 0x0002005B,
    (s32)evt_bero_move_mario_speed, 0xF24B6A80, 0x00000031, 0x0001005B,
    (s32)evt_mario_dokan_end, 0x00000002, 0x00000001,
};

// TODO: type
s32 evt_bero_info_run_pipe[53] = {
    0x00020056, 0xFE363C80, (s32)&BeroEX, 0x00020056,
    0xFE363C81, (s32)&BeroEY, 0x00020056, 0xFE363C82,
    (s32)&BeroEZ, 0x0004005B, (s32)evt_cam_road_reset2, 0xFE363C80,
    0xFE363C81, 0xFE363C82, 0x0001005B, (s32)evt_mario_cam_off,
    0x0002005B, (s32)evt_mario_cont_onoff, 0x00000000, 0x0001005B,
    (s32)evt_fade_end_wait, 0x0005005B, (s32)evt_mario_get_pos, 0x00000000,
    0xFE363C80, 0xFE363C81, 0xFE363C82, 0x0006005B,
    (s32)evt_snd_sfxon_3d, (s32)"SFX_MARIO_DOKAN1", 0xFE363C80, 0xFE363C81,
    0xFE363C82, 0x00000000, 0x0001005E, (s32)&evt_bero_info_run_pipe_sub00,
    0x0002005B, (s32)evt_mario_cont_onoff, 0x00000001, 0x00020032,
    0xFE363C80, 0xFE363C83, 0x0001005E, (s32)&bero_case_switch_off,
    0x0001005C, (s32)&evt_bero_nonride_check, 0x0001005B, (s32)evt_mario_cam_on,
    0x0002005B, (s32)evt_mario_key_onoff, 0x00000001, 0x00000002,
    0x00000001,
};

// TODO: type
s32 evt_bero_info_run[249] = {
    0x0001005B, (s32)evt_bero_get_into_info, 0x0002005B, (s32)evt_bero_set_now_number,
    0xFE363C81, 0x0001005B, (s32)evt_bero_read_mario_pera, 0x0002005B,
    (s32)evt_sub_get_system_flag, 0xFE363C80, 0x0002001E, 0xFE363C80,
    0x00000001, 0x00010071, (s32)"セーブブロックからのロード時の処理", 0x0003005B,
    (s32)evt_bero_exec_onoff, 0x00000000, 0x00000001, 0x00010009,
    0x00000001, 0x0002005B, (s32)evt_bero_exec_get, 0xFE363C80,
    0x0002001F, 0xFE363C80, 0x00000004, 0x0003005B,
    (s32)evt_bero_exec_onoff, 0x00000000, 0x00000002, 0x00000021,
    0x00010009, 0x00000001, 0x0002005B, (s32)evt_bero_exec_get,
    0xFE363C80, 0x0002001F, 0xFE363C80, 0x00000004,
    0x0003005B, (s32)evt_bero_exec_onoff, 0x00000001, 0x00000002,
    0x00000021, 0x0002005B, (s32)evt_npc_change_fbat_mode, 0x00000001,
    0x0001005B, (s32)evt_npc_start_for_event, 0x0003005B, (s32)evt_bero_exec_onoff,
    0x00000001, 0x00000001, 0x0003005B, (s32)evt_bero_exec_onoff,
    0x00000000, 0x00010000, 0x0001005E, (s32)&bero_case_entry,
    0x00000002, 0x00000021, 0x0002005B, (s32)evt_bero_exec_get,
    0xFE363C80, 0x00020018, 0xFE363C80, 0x00000001,
    0x00000002, 0x00000021, 0x0003005B, (s32)evt_bero_exec_onoff,
    0x00000000, 0x00000001, 0x0002005B, (s32)evt_mario_key_onoff,
    0x00000000, 0x00020032, 0xFE363C80, 0xFE363C84,
    0x0002004E, 0xFE363C80, 0x00001000, 0x00020019,
    0xFE363C80, 0x00000000, 0x00010071, (s32)"背景に入るときのマリオの初期処理",
    0x0001005B, (s32)evt_mario_bgmode_on, 0x0002005B, (s32)evt_mario_kill_party,
    0x00000000, 0x0002005B, (s32)evt_mario_kill_party, 0x00000001,
    0x0001005B, (s32)evt_mario_cam_y_off, 0x00000020, 0x00010071,
    (s32)"普通に入るときのマリオの初期処理", 0x0001005B, (s32)evt_mario_bgmode_off, 0x00020032,
    0xFE363C80, 0xFE363C84, 0x0002004E, 0xFE363C80,
    0x00000FFF, 0x00020018, 0xFE363C80, 0x00000002,
    0x0002005B, (s32)evt_mario_set_prev_party_dokan, 0x00000000, 0x0002005B,
    (s32)evt_mario_set_prev_party_dokan, 0x00000001, 0x00000020, 0x0002005B,
    (s32)evt_mario_set_prev_party, 0x00000000, 0x0002005B, (s32)evt_mario_set_prev_party,
    0x00000001, 0x00000021, 0x00000021, 0x00020032,
    0xFE363C80, 0xFE363C84, 0x0002004E, 0xFE363C80,
    0x00000FFF, 0x00020018, 0xFE363C80, 0x00000002,
    0x00010022, 0xFE363C85, 0x00010024, 0x00000006,
    0x0002005B, (s32)evt_mario_dokan_prepare, 0x00000004, 0x00010024,
    0x00000002, 0x0002005B, (s32)evt_mario_dokan_prepare, 0x00000004,
    0x00010024, 0x00000009, 0x0002005B, (s32)evt_mario_dokan_prepare,
    0x00000001, 0x00010024, 0x00000008, 0x0002005B,
    (s32)evt_mario_dokan_prepare, 0x00000000, 0x00000031, 0x00000021,
    0x0001005E, (s32)&evt_bero_initial_pos, 0x0002001F, 0xFE363C84,
    0x00002000, 0x00020032, 0xFE363C80, 0xFE363C84,
    0x0002004E, 0xFE363C80, 0x00000FFF, 0x00020018,
    0xFE363C80, 0x00000002, 0x00010022, 0xFE363C85,
    0x00010024, 0x00000006, 0x0006005B, (s32)evt_fade_set_mapchange_type,
    0x00000001, 0xFFFFFFFF, 0xFFFFFFFF, 0x0000002E,
    0x00000000, 0x0006005B, (s32)evt_fade_set_mapchange_type, 0x00000000,
    0xFFFFFFFF, 0xFFFFFFFF, 0x0000002E, 0x00000000,
    0x00010024, 0x00000002, 0x0006005B, (s32)evt_fade_set_mapchange_type,
    0x00000001, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000030,
    0x00000000, 0x0006005B, (s32)evt_fade_set_mapchange_type, 0x00000000,
    0xFFFFFFFF, 0xFFFFFFFF, 0x00000030, 0x00000000,
    0x00010024, 0x00000008, 0x0006005B, (s32)evt_fade_set_mapchange_type,
    0x00000001, 0xFFFFFFFF, 0xFFFFFFFF, 0x0000002A,
    0x00000000, 0x0006005B, (s32)evt_fade_set_mapchange_type, 0x00000000,
    0xFFFFFFFF, 0xFFFFFFFF, 0x0000002A, 0x00000000,
    0x00010024, 0x00000009, 0x0006005B, (s32)evt_fade_set_mapchange_type,
    0x00000001, 0xFFFFFFFF, 0xFFFFFFFF, 0x0000002C,
    0x00000000, 0x0006005B, (s32)evt_fade_set_mapchange_type, 0x00000000,
    0xFFFFFFFF, 0xFFFFFFFF, 0x0000002C, 0x00000000,
    0x00000031, 0x00000021, 0x00000021, 0x00010071,
    (s32)"ほかの初期設定開始", 0x0001005C, (s32)&evt_bero_mario_enter, 0x00000002,
    0x00000001,
};

// TODO: type
s32 evt_bero_initial_pos[158] = {
    0x0001005B, (s32)evt_bero_get_into_info, 0x00020032, 0xFE363C80,
    0xFE363C84, 0x0002004E, 0xFE363C80, 0x00000FFF,
    0x00010022, 0xFE363C80, 0x00010024, 0x00000F00,
    0x00010024, 0x00000003, 0x0003005B, (s32)bero_get_position_normal,
    0x00000000, 0xF24A7A80, 0x00020056, 0xFE363C8A,
    (s32)&BeroSX, 0x00020056, 0xFE363C8B, (s32)&BeroSY,
    0x00020056, 0xFE363C8C, (s32)&BeroSZ, 0x0004005B,
    (s32)evt_mario_set_pos, 0xFE363C8A, 0xFE363C8B, 0xFE363C8C,
    0x0005005B, (s32)evt_party_set_pos, 0x00000000, 0xFE363C8A,
    0xFE363C8B, 0xFE363C8C, 0x0005005B, (s32)evt_party_set_pos,
    0x00000001, 0xFE363C8A, 0xFE363C8B, 0xFE363C8C,
    0x0000002E, 0x00010024, 0x00000000, 0x0003005B,
    (s32)evt_bero_get_info_length, 0xFE363C81, 0xFE363C8A, 0x0003005B,
    (s32)bero_get_position_normal, 0x00000001, 0xFE363C8A, 0x00020056,
    0xFE363C8A, (s32)&BeroSX, 0x00020056, 0xFE363C8B,
    (s32)&BeroSY, 0x00020056, 0xFE363C8C, (s32)&BeroSZ,
    0x0004005B, (s32)evt_mario_set_pos, 0xFE363C8A, 0xFE363C8B,
    0xFE363C8C, 0x0005005B, (s32)evt_party_set_pos, 0x00000000,
    0xFE363C8A, 0xFE363C8B, 0xFE363C8C, 0x0005005B,
    (s32)evt_party_set_pos, 0x00000001, 0xFE363C8A, 0xFE363C8B,
    0xFE363C8C, 0x00010024, 0x00000001, 0x0003005B,
    (s32)evt_bero_get_info_length, 0xFE363C81, 0xFE363C8A, 0x0003005B,
    (s32)bero_get_position_normal, 0x00000001, 0xFE363C8A, 0x00020056,
    0xFE363C8A, (s32)&BeroSX, 0x00020056, 0xFE363C8B,
    (s32)&BeroSY, 0x00020056, 0xFE363C8C, (s32)&BeroSZ,
    0x0004005B, (s32)evt_mario_set_pos, 0xFE363C8A, 0xFE363C8B,
    0xFE363C8C, 0x0005005B, (s32)evt_party_set_pos, 0x00000000,
    0xFE363C8A, 0xFE363C8B, 0xFE363C8C, 0x0005005B,
    (s32)evt_party_set_pos, 0x00000001, 0xFE363C8A, 0xFE363C8B,
    0xFE363C8C, 0x00010024, 0x00000002, 0x0003005B,
    (s32)evt_bero_get_info_length, 0xFE363C81, 0xFE363C8A, 0x0003005B,
    (s32)bero_get_position_pipe_pure, 0x00000001, 0xFE363C8A, 0x00020056,
    0xFE363C8A, (s32)&BeroSX, 0x00020056, 0xFE363C8B,
    (s32)&BeroSY, 0x00020056, 0xFE363C8C, (s32)&BeroSZ,
    0x0004005B, (s32)evt_mario_set_pos, 0xFE363C8A, 0xFE363C8B,
    0xFE363C8C, 0x0005005B, (s32)evt_party_set_pos, 0x00000000,
    0xFE363C8A, 0xFE363C8B, 0xFE363C8C, 0x0005005B,
    (s32)evt_party_set_pos, 0x00000001, 0xFE363C8A, 0xFE363C8B,
    0xFE363C8C, 0x00000031, 0x0001005B, (s32)evt_cam_road_reset,
    0x00000002, 0x00000001,
};

// TODO: type
s32 evt_bero_mario_enter[95] = {
    0x0002005B, (s32)evt_seq_wait, 0x00000002, 0x0002005B,
    (s32)evt_npc_change_fbat_mode, 0x00000007, 0x0001005B, (s32)evt_npc_stop_for_event,
    0x0001005E, 0xFE363C87, 0x00020018, 0xFE363C80,
    0x00000001, 0x0002005B, (s32)evt_mario_key_onoff, 0x00000001,
    0x00010004, 0x00000064, 0x00000021, 0x0002005B,
    (s32)evt_bero_exec_get, 0xFE363C80, 0x0002001F, 0xFE363C80,
    0x00000004, 0x0003005B, (s32)evt_bero_exec_onoff, 0x00000000,
    0x00000002, 0x00000021, 0x0001005B, (s32)evt_bero_get_into_info,
    0x00020032, 0xFE363C80, 0xFE363C84, 0x0002004E,
    0xFE363C80, 0x00000FFF, 0x00010022, 0xFE363C80,
    0x00010024, 0x00000F00, 0x0001005E, (s32)&evt_bero_info_run_none,
    0x00010024, 0x00000000, 0x0001005E, (s32)&evt_bero_info_run_road,
    0x00010024, 0x00000001, 0x0001005E, (s32)&evt_bero_info_run_door,
    0x00010024, 0x00000002, 0x0001005E, (s32)&evt_bero_info_run_pipe,
    0x00010024, 0x00000003, 0x0001005E, (s32)&evt_bero_info_run_gate,
    0x0000002A, 0x0002005B, (s32)evt_mario_key_onoff, 0x00000001,
    0x00000031, 0x0002005B, (s32)evt_bero_exec_get, 0xFE363C80,
    0x0002001F, 0xFE363C80, 0x00000004, 0x0003005B,
    (s32)evt_bero_exec_onoff, 0x00000001, 0x00000002, 0x00000021,
    0x00010003, 0x00000064, 0x0002005B, (s32)evt_npc_change_fbat_mode,
    0x00000001, 0x0001005B, (s32)evt_npc_start_for_event, 0x0001005E,
    (s32)&bero_case_entry, 0x0003005B, (s32)evt_bero_exec_onoff, 0x00000001,
    0x00000001, 0x0003005B, (s32)evt_bero_exec_onoff, 0x00000000,
    0x00010000, 0x00000002, 0x00000001,
};

// 4-byte zero word that ends the unit's .data.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".data") u32 gap_05_8030DA24_data = 0;
#pragma force_active reset
#pragma explicit_zero_data reset

// TODO: type
s32 evt_bero_info_run_none[5] = {
    0x0002005B, (s32)evt_mario_key_onoff, 0x00000001, 0x00000002,
    0x00000001,
};

// TODO: type
s32 evt_bero_info_run_gate[5] = {
    0x0002005B, (s32)evt_mario_key_onoff, 0x00000001, 0x00000002,
    0x00000001,
};
// clang-format on


#ifdef MUST_MATCH
// An immediately-generated helper flushes the deferred functions' pooled .sdata2 entries ahead of
// the force_active tail gap below, so the gap lands at the end of the section. Generating a helper
// also makes MWCC lay out .bss by first reference, so the body names the three .bss globals in the
// original's address order (BeroID, BeroSW, BeroINFOARR); a plain body would instead use the code's
// reference order. An integer body avoids adding a conversion double to .rodata.
#pragma defer_codegen off
static s32 flush_pools(s32 v) {
    if (v) {
        BeroID[0] = BeroSW[0] = (s32)BeroINFOARR[0];
    }
    return v + 1;
}
#pragma defer_codegen reset
#endif

// 4-byte zero word that ends the unit's .sdata2.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata2") static u32 gap_09_8041A42C_sdata2[1] = {0};
#pragma force_active reset
#pragma explicit_zero_data reset
