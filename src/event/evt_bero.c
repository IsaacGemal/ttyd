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
#include <string.h>

extern GlobalWork* gp;

//.bss
s32 BeroSW[16]; //switches
BeroEntry* BeroINFOARR[16];
s32 BeroID[16];

s32 bero_null[15] = {
	0x00000000, 0x0F000000, 0x00004E20, 0x000186A0, 0x00000000, 0x00000000,
	0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000,
	0x00010001, 0x00000000, 0x00000000,
};

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

EVT_BEGIN(evt_bero_mario_enter)
CALL(evt_seq_wait, SEQ_GAME)
CALL(evt_npc_change_fbat_mode, 7)



EVT_END()

EVT_BEGIN(evt_bero_info_run)
CALL(evt_bero_get_into_info)


EVT_END()

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
	evtSetValue(event, args[3], bero->animeArg3);
	evtSetValue(event, args[4], bero->animeArg4);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bero_get_info_length) {
	s32* args = event->args;
	BeroEntry* bero;
	s32 v5;
	f32 storeme;

	args = event->args;
	bero = BeroINFOARR[evtGetValue(event, args[0])];
	v5 = bero->field_0x18;
	if (v5 != -1) {
		evtSetFloat(event, args[1], (f32)v5);
		return EVT_RETURN_DONE;
	}
	switch ((*(u16*)((u8*)bero + 0x4)) & 0xFFF) {
		case 0:
		case 3:
		case 0xF00:
			storeme = 30.0f;
			break;
		case 1:
			storeme = 60.0f;
			break;
		case 2:
			storeme = 60.0f;
			break;
		default:
			storeme = 60.0f;
			break;
	}
	evtSetFloat(event, args[1], storeme);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bero_get_info_kinddir) {
	BeroEntry* bero;
	s32* args = event->args;

	bero = BeroINFOARR[evtGetValue(event, args[0])];
	evtSetValue(event, args[1], *(u16*)((u8*)bero + 0x4));
	evtSetValue(event, args[2], *(u16*)((u8*)bero + 0x6));
	evtSetValue(event, args[3], bero->kinddirArg3);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bero_get_info_nextarea) {
	BeroEntry* bero;
	s32* args = event->args;

	bero = BeroINFOARR[evtGetValue(event, args[0])];
	evtSetValue(event, args[1], bero->nextareaArg1);
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

	// NONMATCHING (94.05%): register allocation / branch layout differs from the original
USER_FUNC(evt_bero_get_info) {
	BeroEntry** table = BeroINFOARR;
	BeroEntry* entry;
	HitEntry* hit;
	s32* switches;
	s32 num, val;
	int i;

	for (i = 0; i < 16; i++) {
		table[i] = 0;
	}
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
	num = 0;
	while (1) {
		if (!entry->hitName) {
			break;
		}
		*table = entry;
		switches[num] = -1;
		table++;
		num++;
		entry++;
	}
	BeroNUM = num;
	BeroEXEC = 0;
	return EVT_RETURN_DONE;
}

	// NONMATCHING (86.42%): register allocation / branch layout differs from the original
USER_FUNC(evt_bero_get_into_info) {
	BeroEntry** table = BeroINFOARR;
	BeroEntry* entry;
	s32 kind = 0;
	s32 i;

	for (i = 0; ; i++, table++) {
		entry = *table;
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
		event->lwData[1] = 0;
		event->lwData[2] = BeroNUM;
		event->lwData[3] = (s32)entry->hitName;
		event->lwData[4] = *(u16*)((u8*)entry + 0x4);
		kind = entry->kinddirArg3;
		{
			const char* names[8] = {"n_", "ne_", "e_", "se_", "s_", "sw_", "w_", "nw_"};
			if (kind == 0x4E20) {
				if (kind == 0x2710) {
					kind = 0x2710;
				} else if ((*(u16*)((u8*)entry + 0x4) & 0xFFF) < 2) {
					for (kind = 0; (u32)kind < 8U; kind++) {
						if (!strncmp(names[kind], entry->hitName, strlen(names[kind]))) {
							break;
						}
					}
				} else {
					kind = 0x2710;
				}
			}
		}
		event->lwData[5] = kind;
		event->lwData[6] = (s32)&entry->field_0xC;
		event->lwData[7] = entry->enterEvent ? (s32)entry->enterEvent : (s32)default_entevt;
		event->lwData[8] = 0xffff0000;
	} else {
		event->lwData[1] = i;
		event->lwData[2] = BeroNUM;
		event->lwData[3] = (s32)gp->beroEnterName;
		event->lwData[4] = *(u16*)((u8*)entry + 0x4);
		kind = entry->kinddirArg3;
		{
			const char* names[8] = {"n_", "ne_", "e_", "se_", "s_", "sw_", "w_", "nw_"};
			if (kind == 0x4E20) {
				if (kind == 0x2710) {
					kind = 0x2710;
				} else if ((*(u16*)((u8*)entry + 0x4) & 0xFFF) < 2) {
					for (kind = 0; (u32)kind < 8U; kind++) {
						if (!strncmp(names[kind], entry->hitName, strlen(names[kind]))) {
							break;
						}
					}
				} else {
					kind = 0x2710;
				}
			}
		}
		event->lwData[5] = kind;
		event->lwData[6] = (s32)&entry->field_0xC;
		event->lwData[7] = entry->enterEvent ? (s32)entry->enterEvent : (s32)default_entevt;
		event->lwData[8] = 0xffff0000;
	}
	return EVT_RETURN_DONE;
}

// ---- reconstructed functions ----
void camRoadReset(void);
void camShiftReset(void);
void N_marioSetBottomlessRespawnPosOnBeroEntry(f32 x, f32 y, f32 z);

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

USER_FUNC(evt_bero_cam_shift_init) {
	camRoadReset();
	camShiftReset();
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bero_disppos_save) {
	MarioWork* wp;

	wp = marioGetPtr();
	BeroOX = *(f32*)((u8*)wp + 0x98);
	wp = marioGetPtr();
	BeroOY = *(f32*)((u8*)wp + 0x9C);
	wp = marioGetPtr();
	BeroOZ = *(f32*)((u8*)wp + 0xA0);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bero_disppos_load) {
	MarioWork* wp;
	f32 pos;

	pos = BeroOX;
	wp = marioGetPtr();
	*(f32*)((u8*)wp + 0x98) = pos;
	pos = BeroOY;
	wp = marioGetPtr();
	*(f32*)((u8*)wp + 0x9C) = pos;
	pos = BeroOZ;
	wp = marioGetPtr();
	*(f32*)((u8*)wp + 0xA0) = pos;
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bero_case_id_load) {
	s32* args = event->args;
	s32 id = evtGetValue(event, args[0]);

	evtSetValue(event, args[1], BeroID[id]);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_bero_case_id_save) {
	s32* args = event->args;
	s32 id = evtGetValue(event, args[0]);

	BeroID[id] = evtGetValue(event, args[1]);
	return EVT_RETURN_DONE;
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

USER_FUNC(N_evt_bero_set_mario_bottomless_respawn_pos_to_current_mario_pos) {
	MarioWork* wp = marioGetPtr();

	N_marioSetBottomlessRespawnPosOnBeroEntry(wp->position.x, wp->position.y, wp->position.z);
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

	// NONMATCHING (90.39%): register allocation / branch layout differs from the original
USER_FUNC(evt_bero_id_filter) {
	s32* args = event->args;
	s32 id = evtGetValue(event, args[0]);
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
	evtSetValue(event, args[0], newId);
	return EVT_RETURN_DONE;
}

	// NONMATCHING (99.58%): register allocation / branch layout differs from the original
USER_FUNC(evt_bero_get_number) {
	BeroEntry** temp, **table;
	s32 id;
	s32 newId;
	const char *string, *test;

	table = BeroINFOARR;
	id = evtGetValue(event, event->args[0]);
	newId = id;

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

	// NONMATCHING (99.69%): register allocation / branch layout differs from the original
USER_FUNC(evt_bero_switch_off) {
	BeroEntry** temp, **table = BeroINFOARR;
	s32 id = evtGetValue(event, event->args[0]);
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

	// NONMATCHING (99.69%): register allocation / branch layout differs from the original
USER_FUNC(evt_bero_switch_on) {
	BeroEntry** temp, **table = BeroINFOARR;
	s32 id = evtGetValue(event, event->args[0]);
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

	// NONMATCHING (97.92%): register allocation / branch layout differs from the original
USER_FUNC(evt_bero_read_mario_pera) {
	if (isFirstCall) {
		event->userdata[0] = 0;
	}
	switch (event->userdata[0]) {
	default:
		break;
	case 0:
		if (animGroupBaseAsync("p_dokan_x", 0, 0)) {
			event->userdata[0] = 1;
		}
		break;
	case 1:
		if (animGroupBaseAsync("p_dokan_y", 0, 0)) {
			event->userdata[0] = 100;
		}
		break;
	}
	return event->userdata[0] == 100 ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
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

USER_FUNC(bero_set_disp_position_pipe) {
	MarioWork* wp;

	switch (event->lwData[5]) {
	case 2:
	case 6:
		if (!(event->lwData[4] & 0x1000)) {
			wp = marioGetPtr();
			*(f32*)((u8*)wp + 0x9C) += 3.0f;
			wp = marioGetPtr();
			*(f32*)((u8*)wp + 0xA0) -= 15.0f;
		}
		break;
	case 8:
	case 9:
		if (!(event->lwData[4] & 0x1000)) {
			wp = marioGetPtr();
			*(f32*)((u8*)wp + 0xA0) -= 13.0f;
		} else {
			wp = marioGetPtr();
			*(f32*)((u8*)wp + 0xA0) -= 20.0f;
		}
		break;
	case 7:
	case 0x2710:
	default:
		break;
	}
	return EVT_RETURN_DONE;
}

void bero_get_position_pipe_sub(s32 a, s32 b, s32 c, s32 d, Vec* start, Vec* end);

	// NONMATCHING (97.50%): register allocation / branch layout differs from the original
USER_FUNC(bero_get_position_pipe_pure) {
	s32* args = event->args;
	s32 kind;
	s32 a;
	s32 b;
	Vec start, end;

	a = evtGetValue(event, args[0]);
	b = (s32)evtGetFloat(event, args[1]);
	kind = event->lwData[5];
	bero_get_position_pipe_sub(a, b, event->lwData[3], event->lwData[5], &start, &end);
	BeroSX = start.x;
	BeroSY = kind == 8 ? start.y - 37.5f : start.y;
	BeroSZ = start.z;
	BeroEX = end.x;
	BeroEY = kind == 8 ? end.y - 37.5f : end.y;
	BeroEZ = end.z;
	N_marioSetBottomlessRespawnPosOnBeroEntry(BeroEX, BeroEY, BeroEZ);
	return EVT_RETURN_DONE;
}

	// NONMATCHING (95.07%): register allocation / branch layout differs from the original
USER_FUNC(evt_bero_get_info_num) {
	BeroEntry* entry;
	s32 kind;
	s32 id;

	id = evtGetValue(event, event->args[0]);
	entry = BeroINFOARR[id];
	event->lwData[3] = (s32)entry->hitName;
	event->lwData[4] = *(u16*)((u8*)entry + 0x4);
	kind = entry->kinddirArg3;
	{
		const char* names[8] = {"n_", "ne_", "e_", "se_", "s_", "sw_", "w_", "nw_"};
	if (kind == 0x4E20) {
		if (kind == 0x2710) {
			kind = 0x2710;
		} else if ((*(u16*)((u8*)entry + 0x4) & 0xFFF) < 2) {
			for (kind = 0; (u32)kind < 8U; kind++) {
				if (!strncmp(names[kind], entry->hitName, strlen(names[kind]))) {
					break;
				}
			}
		} else {
			kind = 0x2710;
		}
	}
	}
	event->lwData[5] = kind;
	event->lwData[6] = (s32)&entry->field_0xC;
	event->lwData[7] = entry->enterEvent ? (s32)entry->enterEvent : (s32)default_entevt;
	event->lwData[8] = 0xffff0000;
	event->lwData[9] = BeroSW[id];
	event->lwData[10] = entry->field_0x20;
	if (entry->field_0x20 == -1) {
		switch (*(u16*)((u8*)entry + 0x4) & 0xFFF) {
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
	event->lwData[11] = entry->exitEvent ? (s32)entry->exitEvent : (s32)default_outevt;
	event->lwData[12] = 0xffff0000;
	event->lwData[13] = entry->nextareaArg1;
	event->lwData[14] = (s32)entry->nextareaArg2;
	return EVT_RETURN_DONE;
}
