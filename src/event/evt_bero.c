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
	length = bero->length;
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
					switch (entry->kinddirArg1 & 0xFFF) {
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
					switch (entry->kinddirArg1 & 0xFFF) {
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
			switch (entry->kinddirArg1 & 0xFFF) {
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
		switch (entry->kinddirArg1 & 0xFFF) {
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

void bero_get_position_pipe_sub(s32 reverse, s32 length, const char* hitName, s32 kind, Vec* start, Vec* end) {
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
			hitObjGetPos(hitName, start);
			start->x -= off;
			end->x = start->x + (f32)length;
			end->y = start->y;
			end->z = start->z;
			hitCheckFilter(NULL, end->x, end->y, end->z, 0.0f, -1.0f, 0.0f, &hitX, &hitY,
			               &hitZ, &distance, &hitNX, &hitNY, &hitNZ);
		} else {
			hitObjGetPos(hitName, end);
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
			hitObjGetPos(hitName, start);
			end->x = start->x;
			end->y = start->y + (f32)length;
			end->z = start->z;
		} else {
			hitObjGetPos(hitName, end);
			start->x = end->x;
			start->y = end->y + (f32)length;
			start->z = end->z;
		}
		break;
	case 7:
	case 0x2710:
	default:
		hitObjGetPos(hitName, start);
		hitObjGetPos(hitName, end);
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
	bero_get_position_pipe_sub(reverse, length, (const char*)event->lwData[3], kind, &start, &end);
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

	bero_get_position_pipe_sub(reverse, length, (const char*)event->lwData[3], event->lwData[5], &start, &end);
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

EVT_BEGIN(evt_bero_nonride_check)
    IF_INT_NOT_EQUAL(LW(10), 6)
        SET_VALUE(LW(0), LW(3))
        CALL_CHILD(bero_case_switch_on)
        RETURN
    END_IF
    LOOP(0)
        CALL(evt_mario_get_motion, LW(0))
        IF_INT_EQUAL(LW(0), 3)
            BREAK_LOOP
        END_IF
        IF_INT_EQUAL(LW(0), 10)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    DEBUG_PUT_MSG(STRING("土管から降りた"))
    SET_VALUE(LW(0), LW(3))
    CALL_CHILD(bero_case_switch_on)
    RETURN
EVT_END()

EVT_BEGIN(bero_open_door_play_se)
    CALL(evt_bero_get_info_num, LW(1))
    SET_VALUE(LW(4), 0)
    SET_VALUE(LW(5), 0)
    SET_VALUE(LW(6), 0)
    IF_INT_NOT_EQUAL(LW(3), 0)
        CALL(evt_hit_get_position, LW(3), LW(4), LW(5), LW(6))
    END_IF
    CALL(evt_bero_get_info_kinddir, LW(1), LW(10), LW(11), LW(12))
    SWITCH(LW(11))
        CASE_EQUAL(0)
        CASE_EQUAL(1)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN1"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(2)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_WOOD1"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(3)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_WOOD2"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(4)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_WOOD3"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(5)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_IRON1"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(6)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_IRON2"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(7)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_GRILLE1"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(8)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_SLIDE1"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(9)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_TURN1"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(10)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_ELEC1"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(11)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_STONE1"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(12)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_CASTLE1"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(13)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_CASTLE2"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(14)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_METAL1"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(15)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_AUTO_DOOR1"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(16)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_LOCKER_OPEN1"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(17)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_SLIDE1_3"), LW(4), LW(5), LW(6), 0)
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(bero_close_door_play_se)
    CALL(evt_bero_get_info_num, LW(1))
    SET_VALUE(LW(4), 0)
    SET_VALUE(LW(5), 0)
    SET_VALUE(LW(6), 0)
    IF_INT_NOT_EQUAL(LW(3), 0)
        CALL(evt_hit_get_position, LW(3), LW(4), LW(5), LW(6))
    END_IF
    CALL(evt_bero_get_info_kinddir, LW(1), LW(10), LW(11), LW(12))
    SWITCH(LW(11))
        CASE_EQUAL(0)
        CASE_EQUAL(1)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT1"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(2)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_WOOD1"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(3)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_WOOD2"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(4)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_WOOD2"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(5)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_IRON1"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(6)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_IRON2"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(7)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_GRILLE1"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(8)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_SLIDE1"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(9)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_TURN1"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(10)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_ELEC1"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(11)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_OPEN_STONE1"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(12)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_CASTLE1"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(13)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_CASTLE2"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(14)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_METAL1"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(15)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_AUTO_DOOR1"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(16)
        CALL(evt_snd_sfxon_3d, STRING("SFX_STG3_LOCKER_CLOSE1"), LW(4), LW(5), LW(6), 0)
        CASE_EQUAL(17)
        CALL(evt_snd_sfxon_3d, STRING("SFX_DOOR_SHUT_SLIDE1_3"), LW(4), LW(5), LW(6), 0)
    END_SWITCH
    RETURN
EVT_END()

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

EVT_BEGIN(bero_play_enter_anime)
    CALL(evt_bero_get_info_anime, LW(1), LW(10), LW(11), LW(12), LW(13))
    SWITCH(LW(10))
        CASE_EQUAL(0)
        CASE_EQUAL(1)
        BEGIN_INLINE_EVENT
            WAIT_MS(200)
            CALL(evt_bero_mario_go)
        END_INLINE_EVENT
        IF_INT_NOT_EQUAL(LW(12), 0)
            CALL(evt_map_playanim, LW(12), 0, 0)
            CALL(evt_map_pauseanim, 0, LW(12))
            WAIT_MS(700)
            SPAWN_EVENT(bero_close_door_play_se)
            CALL(evt_map_replayanim, 0, LW(12))
        END_IF
        CASE_EQUAL(2)
        IF_INT_EQUAL(LW(12), 0)
            BREAK_SWITCH
        END_IF
        SET_VALUE(LW(8), LW(12))
        CALL(evt_mapobj_rotate, 1, LW(8), 0, -80, 0)
        CALL(evt_bero_mario_go)
        WAIT_MS(500)
        SPAWN_EVENT(bero_close_door_play_se)
        CALL(evt_sub_intpl_msec_init, 11, -80, 0, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(evt_mapobj_rotate, 1, LW(8), 0, LW(0), 0)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CASE_EQUAL(4)
        IF_INT_NOT_EQUAL(LW(12), 0)
            SPAWN_EVENT(LW(12))
        END_IF
        CASE_EQUAL(5)
        IF_INT_NOT_EQUAL(LW(12), 0)
            SET_FLOAT_VALUE(LW(0), FLOAT(180.0f))
            CALL_CHILD(LW(12))
            CALL(evt_bero_mario_go)
            WAIT_MS(500)
            SPAWN_EVENT(bero_close_door_play_se)
            SET_VALUE(LW(9), LW(12))
            CALL(evt_sub_intpl_msec_init, 11, 180, 0, 500)
            LOOP(0)
                CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(10))
                IF_INT_EQUAL(LW(10), 0)
                    SET_FLOAT_VALUE(LW(0), FLOAT(0.0f))
                    CALL_CHILD(LW(9))
                    BREAK_LOOP
                ELSE
                    CALL_CHILD(LW(9))
                END_IF
            END_LOOP
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(bero_play_out_anime)
    CALL(evt_bero_get_info_anime, LW(1), LW(10), LW(11), LW(12), LW(13))
    SWITCH(LW(11))
        CASE_EQUAL(0)
        CASE_EQUAL(1)
        IF_INT_NOT_EQUAL(LW(13), 0)
            SPAWN_EVENT(bero_open_door_play_se)
            CALL(evt_map_playanim, LW(13), 0, 0)
            BEGIN_INLINE_EVENT
                CALL(evt_map_checkanim, LW(13), LW(8), LW(9))
                WAIT_MS(LW(9))
                CALL(evt_bero_mario_go)
            END_INLINE_EVENT
        ELSE
            BEGIN_INLINE_EVENT
                WAIT_MS(500)
                CALL(evt_bero_mario_go)
            END_INLINE_EVENT
        END_IF
        CASE_EQUAL(2)
        IF_INT_EQUAL(LW(12), 0)
            BREAK_SWITCH
        END_IF
        SET_VALUE(LW(8), LW(12))
        SPAWN_EVENT(bero_open_door_play_se)
        CALL(evt_sub_intpl_msec_init, 11, 0, 80, 500)
        LOOP(0)
            CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(1))
            CALL(evt_mapobj_rotate, 1, LW(8), 0, LW(0), 0)
            IF_INT_EQUAL(LW(1), 0)
                BREAK_LOOP
            END_IF
            WAIT_FRAMES(1)
        END_LOOP
        CALL(evt_bero_mario_go)
        CASE_EQUAL(4)
        IF_INT_NOT_EQUAL(LW(13), 0)
            SPAWN_EVENT(LW(13))
        END_IF
        CASE_EQUAL(5)
        IF_INT_NOT_EQUAL(LW(13), 0)
            SPAWN_EVENT(bero_open_door_play_se)
            SET_VALUE(LW(9), LW(13))
            CALL(evt_sub_intpl_msec_init, 11, 0, 180, 500)
            LOOP(0)
                CALL(evt_sub_intpl_msec_get_value_para, LW(0), LW(10))
                IF_INT_EQUAL(LW(10), 0)
                    SET_FLOAT_VALUE(LW(0), FLOAT(180.0f))
                    CALL_CHILD(LW(9))
                    BREAK_LOOP
                ELSE
                    CALL_CHILD(LW(9))
                END_IF
            END_LOOP
            CALL(evt_bero_mario_go)
        END_IF
    END_SWITCH
    RETURN
EVT_END()

EVT_BEGIN(bero_case_init_check)
    CALL(evt_bero_exec_onoff, 1, 65536)
    CALL(evt_bero_exec_get, LW(0))
    AND_RAW(LW(0), 1)
    IF_INT_NOT_EQUAL(LW(0), 0)
        SET_VALUE(LW(0), 1)
        RETURN
    END_IF
    CALL(evt_npc_change_fbat_mode, 7)
    CALL(evt_npc_stop_for_event)
    SET_VALUE(LW(0), LW(4))
    AND_RAW(LW(0), 4095)
    SWITCH(LW(0))
        CASE_OR_EQUAL(0)
        CASE_OR_EQUAL(1)
        CALL(evt_bero_get_info_length, LW(1), LW(0))
        CALL(bero_get_position_normal, 0, LW(0))
        CASE_END
        CASE_OR_EQUAL(3840)
        CASE_OR_EQUAL(3)
        CALL(bero_get_position_normal, 0, FLOAT(0.0f))
        CASE_END
        CASE_EQUAL(2)
        CALL(evt_bero_get_info_length, LW(1), LW(0))
        CALL(bero_get_position_pipe_pure, 0, LW(0))
    END_SWITCH
    CALL(evt_bero_set_now_number, LW(1))
    CALL(evt_bero_exec_onoff, 0, 1)
    SET_VALUE(LW(15), 0)
    CALL_CHILD(LW(11))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_npc_change_fbat_mode, 1)
        CALL(evt_npc_start_for_event)
        CALL(evt_bero_exec_onoff, 0, 65536)
        CALL(evt_bero_exec_onoff, 1, 1)
        SET_VALUE(LW(0), 1)
        RETURN
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(bero_case_exec_none)
    CALL(evt_mario_key_onoff, 0)
    CALL_CHILD(bero_case_init_check)
    CALL(evt_mario_key_onoff, 1)
    IF_INT_EQUAL(LW(0), 1)
        RETURN
    END_IF
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_bero_exec_onoff, 0, 65536)
    CALL(evt_bero_exec_onoff, 1, 1)
    CALL(evt_bero_exec_onoff, 0, 8)
    WAIT_FRAMES(2)
    CALL(evt_bero_1stcheck, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        WAIT_MS(1000)
    END_IF
    CALL(evt_bero_mapchange, LW(13), LW(14))
    RETURN
EVT_END()

EVT_BEGIN(bero_case_exec_road)
    CALL(evt_mario_key_onoff, 0)
    CALL_CHILD(bero_case_init_check)
    CALL(evt_mario_key_onoff, 1)
    IF_INT_EQUAL(LW(0), 1)
        RETURN
    END_IF
    CALL(evt_mario_key_onoff, 0)
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), LW(5))
        MULTIPLY(LW(0), 45)
        CALL(evt_mario_set_force_dir, LW(0))
        CALL(evt_mario_wait_landon)
        CALL(bero_get_position_normal, 0, FLOAT(150.0f))
        PEEK_FLOAT(LW(10), PTR(&BeroEX))
        PEEK_FLOAT(LW(12), PTR(&BeroEZ))
        CALL(evt_mario_mov_pos2, LW(10), LW(12), FLOAT(150.0f))
        CALL(evt_bero_exec_onoff, 0, 65536)
        CALL(evt_bero_exec_onoff, 1, 1)
    END_INLINE_EVENT
    WAIT_MS(150)
    CALL(evt_bero_exec_onoff, 0, 8)
    WAIT_FRAMES(2)
    CALL(evt_bero_1stcheck, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        WAIT_MS(1000)
    END_IF
    CALL(evt_bero_mapchange, LW(13), LW(14))
    RETURN
EVT_END()

EVT_BEGIN(bero_case_exec_door)
    CALL(evt_mario_key_onoff, 0)
    CALL_CHILD(bero_case_init_check)
    CALL(evt_mario_key_onoff, 1)
    IF_INT_EQUAL(LW(0), 1)
        RETURN
    END_IF
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_hitobj_attr_onoff, 0, 1, LW(3), 8388608)
    CALL(evt_bero_mario_go_init)
    SPAWN_EVENT(bero_play_out_anime)
    BEGIN_INLINE_EVENT
        SET_VALUE(LW(0), LW(5))
        MULTIPLY(LW(0), 45)
        CALL(evt_mario_set_force_dir, LW(0))
        CALL(evt_mario_wait_landon)
        CALL(evt_bero_mario_go_wait)
        CALL(bero_get_position_normal, 0, FLOAT(150.0f))
        PEEK_FLOAT(LW(10), PTR(&BeroEX))
        PEEK_FLOAT(LW(12), PTR(&BeroEZ))
        CALL(evt_mario_mov_pos2, LW(10), LW(12), FLOAT(150.0f))
        CALL(evt_bero_exec_onoff, 0, 65536)
        CALL(evt_bero_exec_onoff, 1, 1)
    END_INLINE_EVENT
    CALL(evt_bero_mario_go_wait)
    WAIT_MS(150)
    CALL(evt_bero_exec_onoff, 0, 8)
    WAIT_FRAMES(2)
    CALL(evt_bero_1stcheck, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        WAIT_MS(1000)
    END_IF
    CALL(evt_bero_mapchange, LW(13), LW(14))
    RETURN
EVT_END()

EVT_BEGIN(bero_case_exec_pipe_sub)
    BEGIN_INLINE_EVENT
        CALL(evt_camera_change_event_from_road)
    END_INLINE_EVENT
    IF_INT_EQUAL(LW(13), 0)
        BEGIN_INLINE_EVENT
            SET_VALUE(LW(0), LW(4))
            AND_RAW(LW(0), 4096)
            IF_INT_EQUAL(LW(0), 0)
                SET_VALUE(LF(0), 0)
            ELSE
                SET_VALUE(LF(0), 1)
            END_IF
            CALL(evt_bero_get_number, LW(14))
            CALL(evt_bero_set_now_number, LW(1))
            CALL(evt_bero_get_info_num, LW(1))
            SET_VALUE(LW(0), LW(4))
            AND_RAW(LW(0), 4096)
            IF_INT_EQUAL(LW(0), 0)
                SET_VALUE(LF(1), 0)
            ELSE
                SET_VALUE(LF(1), 1)
            END_IF
            SET_VALUE(LF(2), 0)
            IF_INT_EQUAL(LF(0), 0)
                IF_INT_EQUAL(LF(1), 1)
                    SET_VALUE(LF(2), 1)
                END_IF
            END_IF
            IF_INT_EQUAL(LF(2), 0)
                POKE_INT(0, PTR(&BeroCAM))
                CALL(evt_bero_get_info_length, LW(1), LW(10))
                CALL(bero_get_position_pipe2, 1, LW(10))
                LOOP(0)
                    PEEK_INT(LW(0), PTR(&BeroCAM))
                    IF_INT_EQUAL(LW(0), 1)
                        BREAK_LOOP
                    END_IF
                    WAIT_FRAMES(1)
                END_LOOP
                CALL(evt_bero_cam3d_change, 500, 11)
            END_IF
        END_INLINE_EVENT
    END_IF
    CALL(evt_bero_disppos_save)
    CALL(evt_bero_get_info_length, LW(1), LW(10))
    CALL(bero_get_position_pipe_pure, 0, LW(10))
    CALL(bero_set_disp_position_pipe)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_DOKAN1"), LW(0), LW(1), LW(2), 0)
    SWITCH(LW(5))
        CASE_EQUAL(6)
        CALL(evt_mario_paper_pose_dokan, 5)
        CALL(evt_bero_move_mario_speed, FLOAT(60.0f))
        CASE_EQUAL(2)
        CALL(evt_mario_paper_pose_dokan, 5)
        CALL(evt_bero_move_mario_speed, FLOAT(60.0f))
        CASE_EQUAL(8)
        CALL(evt_mario_paper_pose_dokan, 2)
        CALL(evt_bero_move_mario_speed, FLOAT(60.0f))
        CASE_EQUAL(9)
        CALL(evt_mario_paper_pose_dokan, 3)
        CALL(evt_bero_move_mario_speed, FLOAT(60.0f))
    END_SWITCH
    POKE_INT(1, PTR(&BeroCAM))
    IF_INT_EQUAL(LW(13), 0)
        SET_VALUE(LW(0), LW(4))
        AND_RAW(LW(0), 4096)
        IF_INT_EQUAL(LW(0), 0)
            SET_VALUE(LF(0), 0)
        ELSE
            SET_VALUE(LF(0), 1)
        END_IF
        WAIT_MS(500)
        CALL(evt_bero_get_number, LW(14))
        CALL(evt_bero_get_info_num, LW(1))
        CALL_CHILD(LW(7))
        SET_VALUE(LW(0), LW(4))
        AND_RAW(LW(0), 4096)
        IF_INT_EQUAL(LW(0), 0)
            SET_VALUE(LF(1), 0)
        ELSE
            SET_VALUE(LF(1), 1)
        END_IF
        CALL(evt_bero_disppos_load)
        CALL(evt_mario_paper_pose_off)
        CALL(evt_bero_disppos_save)
        CALL(evt_bero_exec_get, LW(0))
        IF_BITS_CLEAR(LW(0), 4)
            CALL(evt_bero_exec_onoff, 0, 2)
        END_IF
        CALL(evt_bero_get_info_length, LW(1), LW(10))
        CALL(bero_get_position_pipe_pure, 1, LW(10))
        CALL(bero_set_disp_position_pipe)
        WAIT_MS(500)
        IF_INT_NOT_EQUAL(LF(1), 0)
            CALL(evt_mario_bgmode_on)
        ELSE
            CALL(evt_mario_bgmode_off)
        END_IF
        PEEK_FLOAT(LW(0), PTR(&BeroEX))
        PEEK_FLOAT(LW(1), PTR(&BeroEY))
        PEEK_FLOAT(LW(2), PTR(&BeroEZ))
        CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_DOKAN1"), LW(0), LW(1), LW(2), 0)
        CALL_CHILD(evt_bero_info_run_pipe_sub00)
        CALL(evt_bero_exec_get, LW(0))
        IF_BITS_CLEAR(LW(0), 4)
            CALL(evt_bero_exec_onoff, 1, 2)
        END_IF
        CALL(evt_bero_disppos_load)
        CALL(evt_mario_cont_onoff, 1)
        CALL(evt_mario_key_onoff, 1)
        SET_VALUE(LW(0), LW(3))
        CALL_CHILD(bero_case_switch_off)
        SPAWN_EVENT(evt_bero_nonride_check)
        CALL(evt_bero_cam_shift_init)
        CALL(evt_cam3d_evt_off, 500, 11)
    ELSE
        CALL(evt_bero_disppos_load)
        CALL(evt_mario_paper_pose_off)
        SET_VALUE(LW(0), LW(4))
        AND_RAW(LW(0), 4096)
        IF_INT_NOT_EQUAL(LW(0), 0)
            CALL(evt_mario_bgmode_on)
        ELSE
            CALL(evt_mario_bgmode_off)
        END_IF
    END_IF
    CALL(evt_npc_change_fbat_mode, 1)
    CALL(evt_npc_start_for_event)
    CALL(evt_bero_exec_onoff, 0, 65536)
    CALL(evt_bero_exec_onoff, 1, 1)
    RETURN
EVT_END()

EVT_BEGIN(bero_case_exec_pipe)
    CALL(evt_mario_get_mode, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        RETURN
    END_IF
    CALL(evt_mario_key_onoff, 0)
    CALL_CHILD(bero_case_init_check)
    CALL(evt_mario_key_onoff, 1)
    IF_INT_EQUAL(LW(0), 1)
        RETURN
    END_IF
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_wait_landon)
    CALL(evt_mario_cont_onoff, 0)
    IF_INT_EQUAL(LW(13), 0)
        SPAWN_EVENT(bero_case_exec_pipe_sub)
        RETURN
    END_IF
    CALL(evt_mario_cam_off)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_DOKAN1"), LW(0), LW(1), LW(2), 0)
    SWITCH(LW(5))
        CASE_EQUAL(6)
        CALL(evt_mario_paper_pose_dokan, 5)
        CALL(evt_bero_move_mario_speed, FLOAT(60.0f))
        CASE_EQUAL(2)
        CALL(evt_mario_paper_pose_dokan, 5)
        CALL(evt_bero_move_mario_speed, FLOAT(60.0f))
        CASE_EQUAL(8)
        CALL(evt_mario_paper_pose_dokan, 2)
        CALL(evt_bero_move_mario_speed, FLOAT(60.0f))
        CASE_EQUAL(9)
        CALL(evt_mario_paper_pose_dokan, 3)
        CALL(evt_bero_move_mario_speed, FLOAT(60.0f))
    END_SWITCH
    IF_BITS_CLEAR(LW(4), 8192)
        SWITCH(LW(5))
            CASE_EQUAL(6)
            CALL(evt_fade_set_mapchange_type, 1, 45, 0, -1, -1)
            CALL(evt_fade_set_mapchange_type, 0, 45, 0, -1, -1)
            CASE_EQUAL(2)
            CALL(evt_fade_set_mapchange_type, 1, 47, 0, -1, -1)
            CALL(evt_fade_set_mapchange_type, 0, 47, 0, -1, -1)
            CASE_EQUAL(8)
            CALL(evt_fade_set_mapchange_type, 1, 41, 0, -1, -1)
            CALL(evt_fade_set_mapchange_type, 0, 41, 0, -1, -1)
            CASE_EQUAL(9)
            CALL(evt_fade_set_mapchange_type, 1, 43, 0, -1, -1)
            CALL(evt_fade_set_mapchange_type, 0, 43, 0, -1, -1)
        END_SWITCH
    END_IF
    CALL(evt_bero_exec_onoff, 0, 8)
    WAIT_FRAMES(2)
    CALL(evt_bero_1stcheck, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        WAIT_MS(1000)
    END_IF
    CALL(evt_bero_mapchange, LW(13), LW(14))
    RETURN
EVT_END()

EVT_BEGIN(bero_case_exec_gate)
    CALL(evt_mario_key_onoff, 0)
    CALL_CHILD(bero_case_init_check)
    CALL(evt_mario_key_onoff, 1)
    IF_INT_EQUAL(LW(0), 1)
        RETURN
    END_IF
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_bero_exec_onoff, 0, 65536)
    CALL(evt_bero_exec_onoff, 1, 1)
    CALL(evt_bero_exec_onoff, 0, 8)
    WAIT_FRAMES(2)
    CALL(evt_bero_1stcheck, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        WAIT_MS(1000)
    END_IF
    CALL(evt_bero_mapchange, LW(13), LW(14))
    RETURN
EVT_END()

EVT_BEGIN(bero_case_entry)
    PEEK_INT(LW(2), PTR(&BeroNUM))
    SET_VALUE(LW(1), 0)
    LOOP(0)
        CALL(evt_bero_get_info_num, LW(1))
        IF_INT_EQUAL(LW(9), -1)
            SET_VALUE(LW(0), LW(4))
            AND_RAW(LW(0), 4095)
            SWITCH(LW(0))
                CASE_EQUAL(3840)
                CALL(evt_run_case_evt_bero, LW(10), 1, LW(3), 0, PTR(bero_case_exec_none), LW(15))
                CASE_EQUAL(0)
                CALL(evt_hitobj_attr_onoff, 0, 1, LW(3), 8388608)
                CALL(evt_run_case_evt_bero, LW(10), 1, LW(3), 0, PTR(bero_case_exec_road), LW(15))
                CASE_EQUAL(1)
                CALL(evt_run_case_evt_bero, LW(10), 1, LW(3), 0, PTR(bero_case_exec_door), LW(15))
                CASE_EQUAL(2)
                CALL(evt_run_case_evt_bero, LW(10), 1, LW(3), 0, PTR(bero_case_exec_pipe), LW(15))
                CASE_EQUAL(3)
                CALL(evt_run_case_evt_bero, LW(10), 1, LW(3), 0, PTR(bero_case_exec_gate), LW(15))
            END_SWITCH
            CALL(evt_bero_case_id_save, LW(1), LW(15))
            BEGIN_INLINE_EVENT
                CALL(evt_bero_switch_on, LW(1))
            END_INLINE_EVENT
        END_IF
        ADD(LW(1), 1)
        IF_INT_EQUAL(LW(1), LW(2))
            BREAK_LOOP
        END_IF
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(bero_case_entry_sub)
    SET_VALUE(LW(0), LW(4))
    AND_RAW(LW(0), 4095)
    SWITCH(LW(0))
        CASE_EQUAL(3840)
        CALL(evt_run_case_evt_bero, LW(10), 1, LW(3), 0, PTR(bero_case_exec_none), LW(15))
        CASE_EQUAL(0)
        CALL(evt_hitobj_attr_onoff, 0, 1, LW(3), 8388608)
        CALL(evt_run_case_evt_bero, LW(10), 1, LW(3), 0, PTR(bero_case_exec_road), LW(15))
        CASE_EQUAL(1)
        CALL(evt_run_case_evt_bero, LW(10), 1, LW(3), 0, PTR(bero_case_exec_door), LW(15))
        CASE_EQUAL(2)
        CALL(evt_run_case_evt_bero, LW(10), 1, LW(3), 0, PTR(bero_case_exec_pipe), LW(15))
        CASE_EQUAL(3)
        CALL(evt_run_case_evt_bero, LW(10), 1, LW(3), 0, PTR(bero_case_exec_gate), LW(15))
    END_SWITCH
    CALL(evt_bero_case_id_save, LW(1), LW(15))
    RETURN
EVT_END()

EVT_BEGIN(bero_case_switch_on)
    CALL(evt_bero_switch_on, LW(0))
    CALL(evt_bero_get_info_num, LW(1))
    IF_INT_EQUAL(LW(0), 0)
        DEBUG_PUT_MSG(STRING("ベロ登録"))
        CALL_CHILD(bero_case_entry_sub)
    ELSE
        DEBUG_PUT_MSG(STRING("ベロの出て行くデータの二重登録防止をしています"))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(bero_case_switch_off)
    CALL(evt_bero_switch_off, LW(0))
    CALL(evt_bero_get_info_num, LW(1))
    IF_INT_EQUAL(LW(0), 0)
        DEBUG_PUT_MSG(STRING("ベロ削除しました"))
        CALL(evt_bero_case_id_load, LW(1), LW(15))
        CALL(evt_del_case_evt, 0, LW(15))
        CALL(evt_hitobj_attr_onoff, 0, 0, LW(3), 8388608)
    ELSE
        DEBUG_PUT_MSG(STRING("ベロの削除予約もしくはベロの出て行くデータの二重削除防止です"))
    END_IF
    RETURN
EVT_END()

EVT_BEGIN(evt_bero_info_run_road)
    PEEK_FLOAT(LW(13), PTR(&BeroEX))
    PEEK_FLOAT(LW(14), PTR(&BeroEY))
    PEEK_FLOAT(LW(15), PTR(&BeroEZ))
    CALL(evt_party_set_homing_dist, 0, FLOAT(20.0f))
    CALL(evt_party_set_homing_dist, 1, FLOAT(20.0f))
    CALL(evt_mario_mov_pos2, LW(13), LW(15), FLOAT(150.0f))
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_party_run, 0)
    CALL(evt_party_run, 1)
    CALL(evt_party_set_homing_dist, 0, FLOAT(50.0f))
    CALL(evt_party_set_homing_dist, 1, FLOAT(50.0f))
    RETURN
EVT_END()

EVT_BEGIN(evt_bero_info_run_door)
    CALL(evt_bero_mario_go_init)
    SPAWN_EVENT(bero_play_enter_anime)
    CALL(evt_bero_mario_go_wait)
    PEEK_FLOAT(LW(13), PTR(&BeroEX))
    PEEK_FLOAT(LW(14), PTR(&BeroEY))
    PEEK_FLOAT(LW(15), PTR(&BeroEZ))
    CALL(evt_party_set_homing_dist, 0, FLOAT(20.0f))
    CALL(evt_party_set_homing_dist, 1, FLOAT(20.0f))
    CALL(evt_mario_party_bero_move, LW(13), LW(15), FLOAT(150.0f))
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_party_run, 0)
    CALL(evt_party_run, 1)
    CALL(evt_party_set_homing_dist, 0, FLOAT(50.0f))
    CALL(evt_party_set_homing_dist, 1, FLOAT(50.0f))
    RETURN
EVT_END()

EVT_BEGIN(evt_bero_info_run_pipe_sub00)
    CALL(evt_party_stop, 0)
    CALL(evt_party_stop, 1)
    SWITCH(LW(5))
        CASE_EQUAL(6)
        CALL(evt_mario_set_dispdir, 180)
        CALL(evt_mario_paper_pose_dokan, 4)
        CALL(evt_bero_move_mario_speed, FLOAT(60.0f))
        CASE_EQUAL(2)
        CALL(evt_mario_set_dispdir, 0)
        CALL(evt_mario_paper_pose_dokan, 4)
        CALL(evt_bero_move_mario_speed, FLOAT(60.0f))
        CASE_EQUAL(9)
        CALL(evt_mario_paper_pose_dokan, 1)
        CALL(evt_bero_move_mario_speed, FLOAT(60.0f))
        CASE_EQUAL(8)
        CALL(evt_mario_paper_pose_dokan, 0)
        CALL(evt_bero_move_mario_speed, FLOAT(60.0f))
    END_SWITCH
    CALL(evt_mario_dokan_end)
    RETURN
EVT_END()

EVT_BEGIN(evt_bero_info_run_pipe)
    PEEK_FLOAT(LW(0), PTR(&BeroEX))
    PEEK_FLOAT(LW(1), PTR(&BeroEY))
    PEEK_FLOAT(LW(2), PTR(&BeroEZ))
    CALL(evt_cam_road_reset2, LW(0), LW(1), LW(2))
    CALL(evt_mario_cam_off)
    CALL(evt_mario_cont_onoff, 0)
    CALL(evt_fade_end_wait)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_DOKAN1"), LW(0), LW(1), LW(2), 0)
    CALL_CHILD(evt_bero_info_run_pipe_sub00)
    CALL(evt_mario_cont_onoff, 1)
    SET_VALUE(LW(0), LW(3))
    CALL_CHILD(bero_case_switch_off)
    SPAWN_EVENT(evt_bero_nonride_check)
    CALL(evt_mario_cam_on)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_bero_info_run)
    CALL(evt_bero_get_into_info)
    CALL(evt_bero_set_now_number, LW(1))
    CALL(evt_bero_read_mario_pera)
    CALL(evt_sub_get_system_flag, LW(0))
    IF_BITS_SET(LW(0), 1)
        DEBUG_PUT_MSG(STRING("セーブブロックからのロード時の処理"))
        CALL(evt_bero_exec_onoff, 0, 1)
        WAIT_FRAMES(1)
        CALL(evt_bero_exec_get, LW(0))
        IF_BITS_CLEAR(LW(0), 4)
            CALL(evt_bero_exec_onoff, 0, 2)
        END_IF
        WAIT_FRAMES(1)
        CALL(evt_bero_exec_get, LW(0))
        IF_BITS_CLEAR(LW(0), 4)
            CALL(evt_bero_exec_onoff, 1, 2)
        END_IF
        CALL(evt_npc_change_fbat_mode, 1)
        CALL(evt_npc_start_for_event)
        CALL(evt_bero_exec_onoff, 1, 1)
        CALL(evt_bero_exec_onoff, 0, 65536)
        CALL_CHILD(bero_case_entry)
        RETURN
    END_IF
    CALL(evt_bero_exec_get, LW(0))
    IF_INT_EQUAL(LW(0), 1)
        RETURN
    END_IF
    CALL(evt_bero_exec_onoff, 0, 1)
    CALL(evt_mario_key_onoff, 0)
    SET_VALUE(LW(0), LW(4))
    AND_RAW(LW(0), 4096)
    IF_INT_NOT_EQUAL(LW(0), 0)
        DEBUG_PUT_MSG(STRING("背景に入るときのマリオの初期処理"))
        CALL(evt_mario_bgmode_on)
        CALL(evt_mario_kill_party, 0)
        CALL(evt_mario_kill_party, 1)
        CALL(evt_mario_cam_y_off)
    ELSE
        DEBUG_PUT_MSG(STRING("普通に入るときのマリオの初期処理"))
        CALL(evt_mario_bgmode_off)
        SET_VALUE(LW(0), LW(4))
        AND_RAW(LW(0), 4095)
        IF_INT_EQUAL(LW(0), 2)
            CALL(evt_mario_set_prev_party_dokan, 0)
            CALL(evt_mario_set_prev_party_dokan, 1)
        ELSE
            CALL(evt_mario_set_prev_party, 0)
            CALL(evt_mario_set_prev_party, 1)
        END_IF
    END_IF
    SET_VALUE(LW(0), LW(4))
    AND_RAW(LW(0), 4095)
    IF_INT_EQUAL(LW(0), 2)
        SWITCH(LW(5))
            CASE_EQUAL(6)
            CALL(evt_mario_dokan_prepare, 4)
            CASE_EQUAL(2)
            CALL(evt_mario_dokan_prepare, 4)
            CASE_EQUAL(9)
            CALL(evt_mario_dokan_prepare, 1)
            CASE_EQUAL(8)
            CALL(evt_mario_dokan_prepare, 0)
        END_SWITCH
    END_IF
    CALL_CHILD(evt_bero_initial_pos)
    IF_BITS_CLEAR(LW(4), 8192)
        SET_VALUE(LW(0), LW(4))
        AND_RAW(LW(0), 4095)
        IF_INT_EQUAL(LW(0), 2)
            SWITCH(LW(5))
                CASE_EQUAL(6)
                CALL(evt_fade_set_mapchange_type, 1, -1, -1, 46, 0)
                CALL(evt_fade_set_mapchange_type, 0, -1, -1, 46, 0)
                CASE_EQUAL(2)
                CALL(evt_fade_set_mapchange_type, 1, -1, -1, 48, 0)
                CALL(evt_fade_set_mapchange_type, 0, -1, -1, 48, 0)
                CASE_EQUAL(8)
                CALL(evt_fade_set_mapchange_type, 1, -1, -1, 42, 0)
                CALL(evt_fade_set_mapchange_type, 0, -1, -1, 42, 0)
                CASE_EQUAL(9)
                CALL(evt_fade_set_mapchange_type, 1, -1, -1, 44, 0)
                CALL(evt_fade_set_mapchange_type, 0, -1, -1, 44, 0)
            END_SWITCH
        END_IF
    END_IF
    DEBUG_PUT_MSG(STRING("ほかの初期設定開始"))
    SPAWN_EVENT(evt_bero_mario_enter)
    RETURN
EVT_END()

EVT_BEGIN(evt_bero_initial_pos)
    CALL(evt_bero_get_into_info)
    SET_VALUE(LW(0), LW(4))
    AND_RAW(LW(0), 4095)
    SWITCH(LW(0))
        CASE_EQUAL(3840)
        CASE_EQUAL(3)
        CALL(bero_get_position_normal, 0, FLOAT(0.0f))
        PEEK_FLOAT(LW(10), PTR(&BeroSX))
        PEEK_FLOAT(LW(11), PTR(&BeroSY))
        PEEK_FLOAT(LW(12), PTR(&BeroSZ))
        CALL(evt_mario_set_pos, LW(10), LW(11), LW(12))
        CALL(evt_party_set_pos, 0, LW(10), LW(11), LW(12))
        CALL(evt_party_set_pos, 1, LW(10), LW(11), LW(12))
        CASE_END
        CASE_EQUAL(0)
        CALL(evt_bero_get_info_length, LW(1), LW(10))
        CALL(bero_get_position_normal, 1, LW(10))
        PEEK_FLOAT(LW(10), PTR(&BeroSX))
        PEEK_FLOAT(LW(11), PTR(&BeroSY))
        PEEK_FLOAT(LW(12), PTR(&BeroSZ))
        CALL(evt_mario_set_pos, LW(10), LW(11), LW(12))
        CALL(evt_party_set_pos, 0, LW(10), LW(11), LW(12))
        CALL(evt_party_set_pos, 1, LW(10), LW(11), LW(12))
        CASE_EQUAL(1)
        CALL(evt_bero_get_info_length, LW(1), LW(10))
        CALL(bero_get_position_normal, 1, LW(10))
        PEEK_FLOAT(LW(10), PTR(&BeroSX))
        PEEK_FLOAT(LW(11), PTR(&BeroSY))
        PEEK_FLOAT(LW(12), PTR(&BeroSZ))
        CALL(evt_mario_set_pos, LW(10), LW(11), LW(12))
        CALL(evt_party_set_pos, 0, LW(10), LW(11), LW(12))
        CALL(evt_party_set_pos, 1, LW(10), LW(11), LW(12))
        CASE_EQUAL(2)
        CALL(evt_bero_get_info_length, LW(1), LW(10))
        CALL(bero_get_position_pipe_pure, 1, LW(10))
        PEEK_FLOAT(LW(10), PTR(&BeroSX))
        PEEK_FLOAT(LW(11), PTR(&BeroSY))
        PEEK_FLOAT(LW(12), PTR(&BeroSZ))
        CALL(evt_mario_set_pos, LW(10), LW(11), LW(12))
        CALL(evt_party_set_pos, 0, LW(10), LW(11), LW(12))
        CALL(evt_party_set_pos, 1, LW(10), LW(11), LW(12))
    END_SWITCH
    CALL(evt_cam_road_reset)
    RETURN
EVT_END()

EVT_BEGIN(evt_bero_mario_enter)
    CALL(evt_seq_wait, 2)
    CALL(evt_npc_change_fbat_mode, 7)
    CALL(evt_npc_stop_for_event)
    CALL_CHILD(LW(7))
    IF_INT_EQUAL(LW(0), 1)
        CALL(evt_mario_key_onoff, 1)
        GOTO(100)
    END_IF
    CALL(evt_bero_exec_get, LW(0))
    IF_BITS_CLEAR(LW(0), 4)
        CALL(evt_bero_exec_onoff, 0, 2)
    END_IF
    CALL(evt_bero_get_into_info)
    SET_VALUE(LW(0), LW(4))
    AND_RAW(LW(0), 4095)
    SWITCH(LW(0))
        CASE_EQUAL(3840)
        CALL_CHILD(evt_bero_info_run_none)
        CASE_EQUAL(0)
        CALL_CHILD(evt_bero_info_run_road)
        CASE_EQUAL(1)
        CALL_CHILD(evt_bero_info_run_door)
        CASE_EQUAL(2)
        CALL_CHILD(evt_bero_info_run_pipe)
        CASE_EQUAL(3)
        CALL_CHILD(evt_bero_info_run_gate)
        CASE_DEFAULT
        CALL(evt_mario_key_onoff, 1)
    END_SWITCH
    CALL(evt_bero_exec_get, LW(0))
    IF_BITS_CLEAR(LW(0), 4)
        CALL(evt_bero_exec_onoff, 1, 2)
    END_IF
    LABEL(100)
    CALL(evt_npc_change_fbat_mode, 1)
    CALL(evt_npc_start_for_event)
    CALL_CHILD(bero_case_entry)
    CALL(evt_bero_exec_onoff, 1, 1)
    CALL(evt_bero_exec_onoff, 0, 65536)
    RETURN
EVT_END()

// 4-byte zero word that ends the unit's .data.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".data") u32 gap_05_8030DA24_data = 0;
#pragma force_active reset
#pragma explicit_zero_data reset

EVT_BEGIN(evt_bero_info_run_none)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_bero_info_run_gate)
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

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
