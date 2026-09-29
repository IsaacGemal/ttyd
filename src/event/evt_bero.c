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
#include <math.h>
#include <string.h>
#include "cam_road.h"
#include "cam_shift.h"
#include "system.h"

extern GlobalWork* gp;
extern const char str_802C4DA8[];
extern const char str_802C4DB4[];
extern f32 float_37p5_8041a408;
const f64 double_to_int_802c4da0 = 4503601774854144.0;

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
	evtSetValue(event, args[3], (s32)bero->animeArg3);
	evtSetValue(event, args[4], (s32)bero->animeArg4);
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
	switch ((bero->kinddirArg1) & 0xFFF) {
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

// NONMATCHING (87.97%): 7 instructions differ, 12 missing, 6 extra, plus register differences.
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
		entry = BeroINFOARR[0];
		event->lwData[1] = 0;
		event->lwData[2] = BeroNUM;
		event->lwData[3] = (s32)entry->hitName;
		event->lwData[4] = entry->kinddirArg1;
		kind = entry->kinddirArg3;
		{
			const char* names[8] = {"n_", "ne_", "e_", "se_", "s_", "sw_", "w_", "nw_"};
			if (kind == 0x4E20) {
				if (kind == 0x2710) {
					kind = 0x2710;
				} else if ((entry->kinddirArg1 & 0xFFF) < 2) {
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
		event->lwData[4] = entry->kinddirArg1;
		kind = entry->kinddirArg3;
		{
			const char* names[8] = {"n_", "ne_", "e_", "se_", "s_", "sw_", "w_", "nw_"};
			if (kind == 0x4E20) {
				if (kind == 0x2710) {
					kind = 0x2710;
				} else if ((entry->kinddirArg1 & 0xFFF) < 2) {
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

	// NONMATCHING (90.39%): evtGetValue's args temporary is in r31 (original r29) and the
	// table/test registers also differ; logic itself matches the original.
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

USER_FUNC(evt_bero_read_mario_pera) {
	if (isFirstCall) {
		event->userdata[0] = 0;
	}
	switch (event->userdata[0]) {
	default:
		break;
#ifdef MUST_MATCH
	// MUST_MATCH: the empty case 2 makes MWCC emit the retail switch decision tree
	// (cmpwi/beq/bge/b) instead of folding the default edges together.
	case 2:
		break;
#endif
	case 0:
		if (animGroupBaseAsync(str_802C4DA8, 0, 0)) {
			event->userdata[0] = 1;
		}
		break;
	case 1:
		if (animGroupBaseAsync(str_802C4DB4, 0, 0)) {
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


// NONMATCHING (65.28%): the two Vec copies (storedPos/storedAt) are emitted directly
// from work.pos/work.at by our compiler, while the original materialises compiler
// temporaries initialised from two zero Vec .rodata objects (vec3_802c4c58/64), which
// also grows the frame to 0x60 and keeps r28-r29 live.
USER_FUNC(evt_camera_change_event_from_road) {
	MarioWork* mario = marioGetPtr();
	CameraEntry* cam = camGetPtr(CAMERA_3D);
	CameraParams work;
	f32 out;

	camShiftMain(cam, mario, &out);
	camRoadMain(&work.fovY, mario->position.x, cam->marioPos.y, mario->position.z,
	            mario->position.x, cam->marioPos.y, mario->position.z, out);
	cam->presetPos = cam->cameraPos;
	cam->presetAt = cam->cameraAt;
	cam->storedPos = work.pos;
	cam->storedAt = work.at;
	cam->startTimeA = gp->renderTime;
	cam->durationA = OSMillisecondsToTicks(500);
	cam->stateA = 3;
	cam->curveA = 0xb;
	return EVT_RETURN_DONE;
}

// NONMATCHING (66.58%): 3 instructions differ, 27 missing, plus register differences.
USER_FUNC(bero_get_position_pipe2) {
	s32* args = event->args;
	s32 a = evtGetValue(event, args[0]);
	s32 b = (s32)evtGetFloat(event, args[1]);
	Vec start, end;
	CameraParams work;

	bero_get_position_pipe_sub(a, b, event->lwData[3], event->lwData[5], &start, &end);
	end.y += 1.0f;
	camRoadReset();
	camShiftReset();
	camRoadMain(&work.fovY, end.x, end.y, end.z, end.x, end.y, end.z, 0.0f);
	BeroPT = work.pos;
	BeroAT = work.at;
	return EVT_RETURN_DONE;
}

	// NONMATCHING (97.50%): kind/a are in r31/r30 where the original has r30/r31, and the
	// BeroEY ternary is stored directly from f2 instead of the original's f0 + frsp f2,f0.
USER_FUNC(bero_get_position_pipe_pure) {
	s32* args = event->args;
	s32 a;
	s32 kind;
	s32 b;
	Vec start, end;

	a = evtGetValue(event, args[0]);
	b = (s32)evtGetFloat(event, args[1]);
	kind = event->lwData[5];
	bero_get_position_pipe_sub(a, b, event->lwData[3], event->lwData[5], &start, &end);
	BeroSX = start.x;
	BeroSY = kind == 8 ? start.y - float_37p5_8041a408 : start.y;
	BeroSZ = start.z;
	BeroEX = end.x;
	BeroEY = kind == 8 ? end.y - float_37p5_8041a408 : end.y;
	BeroEZ = end.z;
	N_marioSetBottomlessRespawnPosOnBeroEntry(BeroEX, BeroEY, BeroEZ);
	return EVT_RETURN_DONE;
}

	// NONMATCHING (98.29%): the local names[] template is emitted as an anonymous .rodata
	// symbol (@NNN) instead of jumptable_802C4970, and the enter/exitEvent ternaries branch
	// with beq/b instead of the original's bne (register allocation / branch layout).
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
		const char* names[8] = {"n_", "ne_", "e_", "se_", "s_", "sw_", "w_", "nw_"};
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
	event->lwData[7] = entry->enterEvent ? (s32)entry->enterEvent : (s32)default_entevt;
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
	event->lwData[11] = entry->exitEvent ? (s32)entry->exitEvent : (s32)default_outevt;
	event->lwData[12] = 0xffff0000;
	event->lwData[13] = (s32)entry->nextareaArg1;
	event->lwData[14] = (s32)entry->nextareaArg2;
	return EVT_RETURN_DONE;
}

// NONMATCHING (90.41%): the float temporaries land in different registers (rad in f30 not
// f27, dist reuses f29 instead of f26) and the int->double magic constant is emitted as
// an anonymous @NNN instead of double_to_int_802c4da0; brace/line layout otherwise matches.
USER_FUNC(evt_bero_move_mario_speed) {
	f32 speed = evtGetFloat(event, event->args[0]);
	OSTime now = gp->inBattle ? gp->renderTime : gp->renderFieldTime;
	MarioWork* mario;
	f32 sec, rad, sn, cs, dist, dx, dy, vx, vy;
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
	mario = marioGetPtr();
	mario->position.x = BeroSX + dx;
	mario = marioGetPtr();
	mario->position.y = BeroSY + dy;
	mario = marioGetPtr();
	mario->position.z = BeroSZ;
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
	if (reachedX + reachedY == 2) {
		marioGetPtr()->position.x = BeroEX;
		marioGetPtr()->position.y = BeroEY;
		marioGetPtr()->position.z = BeroEZ;
		return EVT_RETURN_DONE;
	}
	return 0;
}

USER_FUNC(bero_get_position_normal) {
	s32* args = event->args;
	MarioWork* mario = marioGetPtr();
	s32 a;
	f32 b;
	s32 kind;
	s32* p;
	const char* c;
	Vec start, end;
#ifdef MUST_MATCH
	// MUST_MATCH: the compiler lays locals out in reverse declaration order, so this order
	// reproduces the original's stack map (hitX at 0x08, e at 0x30). Usage below is unchanged.
	Vec e, ray;
	f32 hitNZ, hitNY, hitNX, distance;
	f32 hitZ, hitY, hitX;
#else
	Vec ray, e;
	f32 hitX, hitY, hitZ, distance;
	f32 hitNX, hitNY, hitNZ;
#endif
	f32 sn, cs, rad;
	f32 angle;
	f32 len;

	a = evtGetValue(event, args[0]);
	b = evtGetFloat(event, args[1]);
	p = (s32*)event->lwData[6];
	c = (const char*)event->lwData[3];
	kind = event->lwData[5];

	start.x = (f32)p[0];
	start.y = (f32)p[1];
	start.z = (f32)p[2];
	end.x = (f32)p[0];
	end.y = (f32)p[1];
	end.z = (f32)p[2];

	if (p[0] == 0x186A0) {
		hitObjGetPos(c, &start);
	}
	if (b != 0.0f) {
		hitX = 0.0f;
		hitY = 0.0f;
		hitZ = 0.0f;
		distance = 1000000.0f;
		angle = 360.0f * (f32)kind / 8.0f;
		if (a == 1) {
			angle += 180.0f;
		}
		rad = 6.2831855f * angle / 360.0f;
		sn = (f32)sin(rad);
		ray.x = start.x - 30.0f * sn;
		ray.y = start.y;
		cs = (f32)cos(rad);
		ray.z = start.z - 30.0f * -cs;
#ifdef MUST_MATCH
		// MUST_MATCH: the original writes distance twice; the compiler keeps both stores.
		distance = 1000000.0f;
#endif
		if (hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, ray.x,
		                   ray.y, ray.z, 0.0f, -1.0f, 0.0f) != NULL) {
			ray.y = hitY;
		}
		e = ray;
		sn = (f32)sin(rad);
		len = 30.0f + b;
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

// NONMATCHING (98.83%): 1 instruction differs, 2 extra, plus register differences.
void bero_get_position_pipe_sub(s32 a, s32 b, s32 c, s32 d, Vec* start, Vec* end) {
	f32 hitX = 0.0f;
	f32 hitY = 0.0f;
	f32 hitZ = 0.0f;
	f32 distance = 10000.0f;
	f32 hitNX;
	f32 hitNY;
	f32 hitNZ;
	f32 off = 20.0f;

	switch (d) {
	case 6:
		b = -b;
		off = -off;
	case 2:
		if (a == 0) {
			hitObjGetPos((const char*)c, start);
			start->x -= off;
			end->x = start->x + (f32)b;
			end->y = start->y;
			end->z = start->z;
			hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ, end->x,
			               end->y, end->z, 0.0f, -1.0f, 0.0f);
		} else {
			hitObjGetPos((const char*)c, end);
			end->x -= off;
			start->x = end->x + (f32)b;
			start->y = end->y;
			start->z = end->z;
			hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ,
			               start->x, start->y, start->z, 0.0f, -1.0f, 0.0f);
		}
		start->y = hitY;
		end->y = hitY;
		break;
	case 9:
		b = -b;
	case 8:
		if (a == 0) {
			hitObjGetPos((const char*)c, start);
			end->x = start->x;
			end->y = start->y + (f32)b;
			end->z = start->z;
		} else {
			hitObjGetPos((const char*)c, end);
			start->x = end->x;
			start->y = end->y + (f32)b;
			start->z = end->z;
		}
		break;
	case 7:
	case 0xA:
	case 0x2710:
	default:
		hitObjGetPos((const char*)c, start);
		hitObjGetPos((const char*)c, end);
		break;
	}
}
