#include "evt/evt_sub.h"
#include "mario/mario_pouch.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "driver/swdrv.h"
#include "driver/effdrv.h"
#include "driver/itemdrv.h"
#include "driver/npcdrv.h"
#include "driver/camdrv.h"
#include "driver/animdrv.h"
#include "countdown.h"
#include "manager/winmgr.h"
#include "driver/icondrv.h"
#include "pmario_sound.h"
#include "system.h"
#include <stdlib.h>
#include <math.h>
#include "statuswindow.h"
#include "manager/fontmgr.h"
#include "mario/mario_motion.h"
#include "driver/msgdrv.h"
#include "mario/mario_cam.h"
#include "driver/mapdrv.h"
#include "eff/eff_stageclear.h"
#include "eff/eff_itemget.h"
#include <stdio.h>
#include "battle/ac/ac_table.h"
#include "battle/battle_ac.h"
#include "driver/dispdrv.h"
#include "driver/extdrv.h"
#include "driver/fadedrv.h"
#include "driver/mobjdrv.h"
#include "driver/seqdrv.h"
#include "driver/windowdrv.h"
#include "eff/eff_mahojin.h"
#include "event.h"
#include "evt/evt_bero.h"
#include "evt/evt_cam.h"
#include "evt/evt_case.h"
#include "evt/evt_eff.h"
#include "evt/evt_fade.h"
#include "evt/evt_item.h"
#include "evt/evt_map.h"
#include "evt/evt_mario.h"
#include "evt/evt_memcard.h"
#include "evt/evt_msg.h"
#include "evt/evt_npc.h"
#include "evt/evt_party.h"
#include "evt/evt_pouch.h"
#include "evt/evt_snd.h"
#include "manager/evtmgr.h"
#include "manager/filemgr.h"
#include "party/party_motion.h"
#include "sound.h"


// Strings and data owned by other units (evt_npc.c)
extern const char str_802C3464[];
extern const char str_802C33A4[];
extern const char str_802C3328[];
extern const char str_802C35D8[];
extern const char str_802C35E4[];
extern const char str_802C35F8[];

// Status window helpers, not yet declared in statuswindow.h

// Countdown helpers, not yet declared in countdown.h

// Misc functions not yet in any header
extern u32 dat_80419d38;

typedef struct SplineWork {
	s32 type;      // 0x0
	s32 field_4;   // 0x4
	s32 field_8;   // 0x8
	s32 field_C;   // 0xC
	s32 step;      // 0x10
	s32 steps;     // 0x14
	s32 mode;      // 0x18
	s32 field_1C;  // 0x1C
	OSTime time;   // 0x20
	s32 timeBased; // 0x28
} SplineWork;

// .sbss
extern s32 stop;

extern s32 evt_win_one_message[];
extern const char str_802C32C0[];
extern const char str_802C32D0[];
extern const char str_802C32E8[];
extern const char str_802C32F8[];
extern const char str_802C330C[];
extern const char str_802C3334[];
extern const char str_802C334C[];
extern const char str_802C3368[];
extern const char str_802C3380[];
extern const char str_802C3388[];
extern const char str_802C3394[];
extern const char str_802C33AC[];
extern const char str_802C33B4[];
extern const char str_802C33C4[];
extern const char str_802C33D4[];
extern const char str_802C33E4[];
extern const char str_802C33F0[];
extern const char str_802C3400[];
extern const char str_802C340C[];
extern const char str_802C341C[];
extern const char str_802C342C[];
extern const char str_802C3444[];
extern const char str_802C3450[];
extern const char str_802C3458[];
extern const char str_802C3470[];
extern const char str_802C347C[];
extern const char str_802C3494[];
extern const char str_802C34A0[];
extern const char str_802C34B0[];
extern const char str_802C34C0[];
extern const char str_802C34D0[];
extern const char str_802C34E0[];
extern const char str_802C34F0[];
extern const char str_802C3500[];
extern const char str_802C3510[];
extern const char str_802C3518[];
extern const char str_802C3520[];
extern const char str_802C3528[];
extern const char str_802C3530[];
extern const char str_802C3538[];
extern const char str_802C3550[];
static s32 stone_bg(EventEntry* event, BOOL isFirstCall);
static s32 bgm_start_wait(void);
static u32 kusa_data[12];

// clang-format off


#define IRAI_MAIL_CHECK(f1, f2, base, sf, mail)                          \
	do {                                                                 \
		if (swGet(f1) != 0 && swGet(f2) == 0) {                          \
			for (i = 0; i < 8; i++) {                                    \
				saved[i] = swByteGet((base) + i);                        \
			}                                                            \
			d = OSTicksToSeconds(gp->retraceLocalTime - *(s64*)saved); \
			if (d > 7200) {                                              \
				result = 1;                                              \
				if (target != 0) {                                       \
					swSet(sf);                                           \
					pouchReceiveMail(mail);                              \
				}                                                        \
			}                                                            \
		}                                                                \
	} while (0)


USER_FUNC(evt_sub_intpl_init) {
	s32* args = event->args;
	s32 a = evtGetValue(event, args[0]);
	s32 b = evtGetValue(event, args[1]);
	s32 c = evtGetValue(event, args[2]);
	s32 d = evtGetValue(event, args[3]);
	event->lwData[11] = a;
	event->lwData[12] = b;
	event->lwData[13] = c;
	event->lwData[14] = 0;
	event->lwData[15] = d;
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_intpl_get_value) {
	event->lwData[0] = (s32)intplGetValue(event->lwData[11], event->lwData[14], event->lwData[15], (f32)event->lwData[12], (f32)event->lwData[13]);
	if (event->lwData[14] >= event->lwData[15]) {
		event->lwData[1] = 0;
	} else {
		event->lwData[1] = 1;
	}
	event->lwData[14]++;
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_intpl_get_value_para) {
	s32* args = event->args;
	evtSetFloat(event, args[0], intplGetValue(event->lwData[11], event->lwData[14], event->lwData[15], (f32)event->lwData[12], (f32)event->lwData[13]));
	if (event->lwData[14] >= event->lwData[15]) {
		evtSetValue(event, args[1], 0);
	} else {
		evtSetValue(event, args[1], 1);
	}
	event->lwData[14]++;
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_intpl_init_float) {
	s32* args = event->args;
	s32 a = evtGetValue(event, args[0]);
	f32 b = evtGetFloat(event, args[1]);
	f32 c = evtGetFloat(event, args[2]);
	s32 d = evtGetValue(event, args[3]);
	event->lwData[11] = a;
	event->lwData[12] = (s32)(1024.0f * b) + 0xF24A7A80;
	event->lwData[13] = (s32)(1024.0f * c) + 0xF24A7A80;
	event->lwData[14] = 0;
	event->lwData[15] = d;
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_intpl_get_float) {
	s32* args = event->args;
	s32 destFloat = args[0];
	s32 destFlag = args[1];
	f32 start = evtGetFloat(event, 0xFE363C8C);
	f32 end = evtGetFloat(event, 0xFE363C8D);
	f32 value = intplGetValue(event->lwData[11], event->lwData[14], event->lwData[15], start, end);
	if (destFloat != -250000000) {
		evtSetFloat(event, destFloat, value);
	}
	if (destFlag != -250000000) {
		if (event->lwData[14] >= event->lwData[15]) {
			evtSetValue(event, destFlag, 0);
		} else {
			evtSetValue(event, destFlag, 1);
		}
	}
	event->lwData[14]++;
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_intpl_msec_init) {
	s32* args = event->args;
	s32 a = evtGetValue(event, args[0]);
	s32 b = evtGetValue(event, args[1]);
	s32 c = evtGetValue(event, args[2]);
	s32 d = evtGetValue(event, args[3]);
	event->lwData[11] = a;
	event->lwData[12] = b;
	event->lwData[13] = c;
	event->lwData[15] = d;
	event->savedTime = event->lifetime;
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_intpl_msec_get_value) {
	s32 elapsed = (s32)((u64)((s32)event->lifetime - (s32)event->savedTime) / (OS_TIMER_CLOCK / 1000));
	if (elapsed < event->lwData[15]) {
		event->lwData[0] = (s32)intplGetValue(event->lwData[11], elapsed, event->lwData[15], (f32)event->lwData[12], (f32)event->lwData[13]);
		event->lwData[1] = 1;
	} else {
		event->lwData[0] = event->lwData[13];
		event->lwData[1] = 0;
	}
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_intpl_msec_get_value_para) {
	s32* args = event->args;
	s32 elapsed = (s32)((u64)((s32)event->lifetime - (s32)event->savedTime) / (OS_TIMER_CLOCK / 1000));
	if (elapsed < event->lwData[15]) {
		evtSetFloat(event, args[0], intplGetValue(event->lwData[11], elapsed, event->lwData[15], (f32)event->lwData[12], (f32)event->lwData[13]));
		evtSetValue(event, args[1], 1);
	} else {
		evtSetFloat(event, args[0], (f32)event->lwData[13]);
		evtSetValue(event, args[1], 0);
	}
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_spline_init) {
	s32* args = event->args;
	s32 a = evtGetValue(event, args[0]);
	s32 b = evtGetValue(event, args[1]);
	s32 c = evtGetValue(event, args[2]);
	s32 count = evtGetValue(event, args[3]);
	s32 d = evtGetValue(event, args[4]);
	s32 e = evtGetValue(event, args[5]);
	SplineWork* work = _mapAlloc(0x30);
	event->lwData[15] = (s32)work;
	work->type = count;
	work->field_4 = (s32)_mapAlloc(count * 4);
	work->field_8 = c;
	work->field_C = (s32)_mapAlloc(count * 0xC);
	spline_maketable(count, work->field_8, (s32*)work->field_4, (s32*)work->field_C);
	work->timeBased = a;
	work->mode = b;
	work->step = 0;
	work->steps = d;
	work->field_1C = e;
	work->time = event->lifetime;
	return EVT_RETURN_DONE;
}


// The original called intplGetValue through a prototype that listed the two float bounds
// before the step/duration words, so MWCC evaluates (and schedules) the float constants
// before the int words. The ABI is identical (ints->r3-r5, floats->f1-f2); only the source
// argument order differs. Same shape as battleAcMain_PowerGauge.
USER_FUNC(evt_sub_spline_get_value) {
	s32* args = event->args;
	SplineWork* work = (SplineWork*)event->lwData[15];
	f32 f1;
	f32 f31;
	f32 out[3];

#ifdef MUST_MATCH
	if (work->timeBased == 0) {
		f1 = ((f32 (*)(s32, f32, f32, s32, s32))intplGetValue)(work->mode, 0.0f, 1.0f, work->step, work->steps);
	} else {
		u64 diff = (u64)event->lifetime - (u64)work->time;
		f31 = (f32)(diff / (OS_TIMER_CLOCK / 1000));
		f1 = ((f32 (*)(s32, f32, f32, s32, s32))intplGetValue)(work->mode, 0.0f, 1.0f, (s32)f31, work->field_1C);
	}
#else
	if (work->timeBased == 0) {
		f1 = intplGetValue(work->mode, work->step, work->steps, 0.0f, 1.0f);
	} else {
		u64 diff = (u64)event->lifetime - (u64)work->time;
		f31 = (f32)(diff / (OS_TIMER_CLOCK / 1000));
		f1 = intplGetValue(work->mode, (s32)f31, work->field_1C, 0.0f, 1.0f);
	}
#endif
	spline_getvalue(out, work->type, work->field_8, work->field_4, work->field_C, f1);
	evtSetValue(event, args[0], (s32)(1024.0f * out[0]) + 0xF24A7A80);
	evtSetValue(event, args[1], (s32)(1024.0f * out[1]) + 0xF24A7A80);
	evtSetValue(event, args[2], (s32)(1024.0f * out[2]) + 0xF24A7A80);
	work->step++;
	if (work->timeBased == 0) {
		if (work->step <= work->steps) {
			evtSetValue(event, args[3], 1);
			return EVT_RETURN_DONE;
		}
	} else {
		if (f31 <= (f32)work->field_1C) {
			evtSetValue(event, args[3], 1);
			return EVT_RETURN_DONE;
		}
	}
	_mapFree((void*)work->field_4);
	_mapFree((void*)work->field_C);
	_mapFree(work);
	evtSetValue(event, args[3], 0);
	return EVT_RETURN_DONE;
}


// `#pragma opt_propagation off`: with propagation on, the int->float conversion of `t`
// is folded into each use and the fp register allocation no longer matches.
#pragma opt_propagation off
USER_FUNC(evt_sub_spline_get_value_manual) {
	s32* args = event->args;
	SplineWork* work = (SplineWork*)event->lwData[15];
	s32 t = evtGetValue(event, args[0]);
	f32 f2 = (f32)t;
	f32 f0;
	f32 f1;
	f32 out[3];

	if (work->timeBased != 0) {
		f0 = (f32)work->field_1C;
	} else {
		f0 = (f32)work->steps;
	}
	f1 = f2 / f0;
	spline_getvalue(out, work->type, work->field_8, work->field_4, work->field_C, f1);
	evtSetValue(event, args[1], (s32)(1024.0f * out[0]) + 0xF24A7A80);
	evtSetValue(event, args[2], (s32)(1024.0f * out[1]) + 0xF24A7A80);
	evtSetValue(event, args[3], (s32)(1024.0f * out[2]) + 0xF24A7A80);
	return EVT_RETURN_DONE;
}
#pragma opt_propagation reset


USER_FUNC(evt_sub_spline_free) {
	void* work = (void*)event->lwData[15];
	_mapFree(*(void**)((u8*)work + 4));
	_mapFree(*(void**)((u8*)work + 0xC));
	_mapFree(work);
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_get_sincos) {
	s32* args = event->args;
	f32 angle = evtGetFloat(event, args[0]);
	f32 rad = angle * 3.141592f / 180.0f;
	evtSetValue(event, args[1], (s32)((f32)sin(rad) * 1024.0f) + 0xF24A7A80);
	evtSetValue(event, args[2], (s32)((f32)cos(rad) * 1024.0f) + 0xF24A7A80);
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_rumble_onoff) {
	s32* args = event->args;
	s32 type = evtGetValue(event, args[0]);
	s32 chan = evtGetValue(event, args[1]);
	switch (type) {
	case 0:
		padRumbleOn(chan);
		break;
	case 1:
		padRumbleOff(chan);
		break;
	case 2:
		padRumbleHardOff(chan);
		break;
	}
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_random) {
	s32* args = event->args;
	s32 max = evtGetValue(event, args[0]);
	evtSetValue(event, args[1], rand() % (max + 1));
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_get_stopwatch) {
	s32* args = event->args;
	EventEntry* entry = evtGetPtrID(evtGetValue(event, args[0]));
	s32 elapsed = (s32)OSTicksToMilliseconds(entry->lifetime);
	if (elapsed > 600000) {
		elapsed = 600000;
	}
	evtSetValue(event, args[1], elapsed);
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_area_check) {
	s32* args = event->args;
	s32 type = evtGetValue(event, args[0]);
	f32 b = (f32)evtGetValue(event, args[1]);
	f32 c = (f32)evtGetValue(event, args[2]);
	f32 d = (f32)evtGetValue(event, args[3]);
	f32 e = (f32)evtGetValue(event, args[4]);
	f32 f = (f32)evtGetValue(event, args[5]);
	s32 result;

	switch (type) {
	case 0:
		if (d < distABf(b, c, e, f)) {
			result = 0;
		} else {
			result = 1;
		}
		break;
	case 1:
		result = 1;
		if (b - d > e) {
			result = 0;
		}
		if (b + d < e) {
			result = 0;
		}
		if (c - d > f) {
			result = 0;
		}
		if (c + d < f) {
			result = 0;
		}
		break;
	default:
		result = 0;
		break;
	}
	evtSetValue(event, args[6], result);
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_get_dist) {
	s32* args = event->args;
	f32 f31 = (f32)evtGetValue(event, args[0]);
	f32 f30 = (f32)evtGetValue(event, args[1]);
	f32 f29 = (f32)evtGetValue(event, args[2]);
	f32 f4 = (f32)evtGetValue(event, args[3]);
	evtSetFloat(event, args[4], distABf(f31, f30, f29, f4));
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_get_dir) {
	s32* args = event->args;
	f32 f31 = (f32)evtGetValue(event, args[0]);
	f32 f30 = (f32)evtGetValue(event, args[1]);
	f32 f29 = (f32)evtGetValue(event, args[2]);
	f32 f4 = (f32)evtGetValue(event, args[3]);
	evtSetFloat(event, args[4], angleABf(f31, f30, f29, f4));
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_get_system_flag) {
	s32* args = event->args;
	evtSetValue(event, args[0], gp->flags);
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_system_flag_onoff) {
	s32* args = event->args;
	s32 result = evtGetValue(event, args[0]);
	u32 flag = args[1];
	if (result != 0) {
		gp->flags |= flag;
	} else {
		gp->flags &= ~flag;
	}
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_key_get_dir) {
	s32* args = event->args;
	u32 chan = evtGetValue(event, args[0]);
	evtSetValue(event, args[1], keyGetDir(chan));
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_key_get_button) {
	s32* args = event->args;
	u32 chan = evtGetValue(event, args[0]);
	evtSetValue(event, args[1], keyGetButton(chan));
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_key_get_buttontrg) {
	s32* args = event->args;
	u32 chan = evtGetValue(event, args[0]);
	evtSetValue(event, args[1], keyGetButtonTrg(chan));
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_get_mapname) {
	s32* args = event->args;
	evtSetValue(event, args[0], (s32)gp->currentMapName);
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_get_areaname) {
	s32* args = event->args;
	evtSetValue(event, args[0], (s32)gp->currentAreaName);
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_status_gauge_force_open) {
	statusWinForceOpen();
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_status_gauge_force_close) {
	statusWinForceClose();
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_status_gauge_check_update) {
	s32* args = event->args;
	if (statusWinCheckUpdate()) {
		evtSetValue(event, args[0], 1);
	} else {
		evtSetValue(event, args[0], 0);
	}
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_save_playtime) {
	s32 arg = event->args[0];
	u8 buf[8];
	*(OSTime*)buf = gp->retraceLocalTime;
	swByteSet(0xA21FE80 + arg + 0, buf[0]);
	swByteSet(0xA21FE80 + arg + 1, buf[1]);
	swByteSet(0xA21FE80 + arg + 2, buf[2]);
	swByteSet(0xA21FE80 + arg + 3, buf[3]);
	swByteSet(0xA21FE80 + arg + 4, buf[4]);
	swByteSet(0xA21FE80 + arg + 5, buf[5]);
	swByteSet(0xA21FE80 + arg + 6, buf[6]);
	swByteSet(0xA21FE80 + arg + 7, buf[7]);
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_load_progresstime) {
	s32* args = event->args;
	u8 saved[8];
	s64 diff;
	s32 base = args[0];
	saved[0] = swByteGet(base + 0xA21FE80 + 0);
	saved[1] = swByteGet(base + 0xA21FE80 + 1);
	saved[2] = swByteGet(base + 0xA21FE80 + 2);
	saved[3] = swByteGet(base + 0xA21FE80 + 3);
	saved[4] = swByteGet(base + 0xA21FE80 + 4);
	saved[5] = swByteGet(base + 0xA21FE80 + 5);
	saved[6] = swByteGet(base + 0xA21FE80 + 6);
	saved[7] = swByteGet(base + 0xA21FE80 + 7);
	diff = (gp->retraceLocalTime - *(s64*)saved) / (OS_TIMER_CLOCK / 1000);
	if (diff > 86400000) {
		diff = 86400000;
	}
	evtSetValue(event, args[1], (s32)diff);
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_get_language) {
	s32* args = event->args;
	evtSetValue(event, args[0], gp->language);
	return EVT_RETURN_DONE;
}


// The original keeps the zero-test as a branch rather than folding it into a branchless
// bool-to-int. The one-iteration loop (removed by the optimizer) stops the peephole that would
// fold it. Without the loop this scores 82%.
USER_FUNC(evt_sub_animgroup_async) {
	s32* args = event->args;
	const char* name = (const char*)evtGetValue(event, args[0]);
	BOOL ret = animGroupBaseAsync(name, gp->inBattle != 0, NULL);
#ifdef MUST_MATCH
	s32 i;
	for (i = 0; i < 1; i++) {
		if (ret == 0) {
			return EVT_RETURN_BLOCK;
		}
	}
	return EVT_RETURN_DONE;
#else
	switch (ret) {
	default:
		return EVT_RETURN_DONE;
	case 0:
		return EVT_RETURN_BLOCK;
	}
#endif
}


USER_FUNC(evt_sub_countdown_start) {
	s32* args = event->args;
	s32 type = evtGetValue(event, args[0]);
	s32 time = evtGetValue(event, args[1]);
	countDownStart(type, time);
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_countdown_get_status) {
	s32* args = event->args;
	s32 status = countDownGetStatus();
	evtSetValue(event, args[0], status);
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_countdown_end) {
	countDownEnd();
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_countdown_flag_onoff) {
	s32* args = event->args;
	s32 on = evtGetValue(event, args[0]);
	s32 flag = evtGetValue(event, args[1]);
	if (on != 0) {
		countDownFlagOn(flag);
	} else {
		countDownFlagOff(flag);
	}
	return EVT_RETURN_DONE;
}


USER_FUNC(evt_sub_countdown_set_restart_time) {
	s32* args = event->args;
	countDownSetRestartTime(evtGetValue(event, args[0]));
	return EVT_RETURN_DONE;
}


void coingetDisp(WinMgrEntry* entry) {
	s32* param = entry->param;
	if (param[1] != 0) {
		char buf[0x400];
		GXColor color;
		sprintf(buf, msgSearch(str_802C35F8), param[0]);
		FontDrawStart();
		*(u32*)&color = dat_80419d38;
		FontDrawColor(&color);
		FontDrawMessage(-FontGetMessageWidth(buf) / 2, entry->field_0x1C - 8, buf);
	}
}


extern WinMgrDesc coinget_window_desc;

static inline f32 evt_sub_sin_approx(f32 angle) {
	f32 v = 0.00761f;
	f32 b = 0.16605f;
	f32 one = 1.0f;
	f32 t, u;

	if (angle <= 3.1415927f) {
		if (angle <= 1.5707964f) {
			t = angle * angle;
			v = v * t - b;
			v = v * t + one;
			v = v * angle;
		} else {
			t = angle - 1.5707964f;
			u = 1.5707964f - t;
			t = u * u;
			v = v * t - b;
			v = v * t + one;
			v = v * u;
		}
	} else if (angle < 4.712389f) {
		t = angle - 3.1415927f;
		u = t * t;
		v = v * u - b;
		v = v * u + one;
		v = v * t;
		v = -v;
	} else {
		t = angle - 4.712389f;
		u = 1.5707964f - t;
		t = u * u;
		v = v * t - b;
		v = v * t + one;
		v = v * u;
		v = -v;
	}
	return v;
}

static inline f32 evt_sub_cos_approx(f32 angle) {
	f32 v = 0.03705f;
	f32 b = 0.4967f;
	f32 one = 1.0f;
	f32 t, u;

	if (angle <= 3.1415927f) {
		if (angle <= 1.5707964f) {
			t = angle * angle;
			v = v * t - b;
			v = v * t + one;
		} else {
			t = angle - 1.5707964f;
			u = 1.5707964f - t;
			t = u * u;
			v = v * t - b;
			v = v * t + one;
			v = -v;
		}
	} else if (angle < 4.712389f) {
		t = angle - 3.1415927f;
		t = t * t;
		v = v * t - b;
		v = v * t + one;
		v = -v;
	} else {
		t = angle - 4.712389f;
		u = 1.5707964f - t;
		t = u * u;
		v = v * t - b;
		v = v * t + one;
	}
	return v;
}

#pragma opt_propagation off
#pragma opt_lifetimes off
// The polynomial is written inline in evt_sub_get_coin with explicit f0-f5 locals (the
// evt_item_get_important_item technique) so MWCC colours the scratch in f0 and the 1.5708
// pivot/wrapped angle in f1, as the target does. The two static inline helpers above are no
// longer called; they are kept for reference. opt_propagation off stops MWCC rematerialising the
// coefficients into every branch; opt_lifetimes off stops the default lifetime pass from shifting
// the seed/coefficient FPRs.
USER_FUNC(evt_sub_get_coin) {
	s32* args = event->args;
	s32 coin = evtGetValue(event, args[0]);
	MarioWork* mario = marioGetPtr();
	Vec pos1;
	Vec pos2;
	CameraEntry* cam;
	EffectEntry* eff;
	s32* work;
	f32 f0, f1, f2, f3, f4, f5, f6, f7;
	f32 yaw;

	pos1 = mario->position;
	pos2 = pos1;

	cam = camGetPtr(4);
	yaw = cam->viewYaw;
	f1 = (100.0f * yaw + 5.0f) / 100.0f;
	f2 = 6.2831855f * reviseAngle(f1);
	f6 = f2 / 360.0f;

	f4 = 0.00761f;
	f3 = 0.16605f;
	f2 = 1.0f;
	f1 = 3.1415927f;
	if (f6 <= f1) {
		f1 = 1.5707964f;
		if (f6 <= f1) {
			f0 = f6 * f6;
			f4 = f4 * f0 - f3;
			f4 = f4 * f0 + f2;
			f4 = f4 * f6;
		} else {
			f0 = f6 - f1;
			f1 = f1 - f0;
			f0 = f1 * f1;
			f4 = f4 * f0 - f3;
			f4 = f4 * f0 + f2;
			f4 = f4 * f1;
		}
	} else {
		f0 = 4.712389f;
		if (f6 < f0) {
			f1 = f6 - f1;
			f0 = f1 * f1;
			f4 = f4 * f0 - f3;
			f4 = f4 * f0 + f2;
			f4 = f4 * f1;
			f4 = -f4;
		} else {
			f0 = f6 - f0;
			f1 = 1.5707964f;
			f1 = f1 - f0;
			f0 = f1 * f1;
			f4 = f4 * f0 - f3;
			f4 = f4 * f0 + f2;
			f4 = f4 * f1;
			f4 = -f4;
		}
	}
	f4 = 10.0f * f4;

	f5 = 0.03705f;
	f3 = 0.4967f;
	f2 = 1.0f;
	f1 = 3.1415927f;
	if (f6 <= f1) {
		f1 = 1.5707964f;
		if (f6 <= f1) {
			f0 = f6 * f6;
			f5 = f5 * f0 - f3;
			f5 = f5 * f0 + f2;
		} else {
			f0 = f6 - f1;
			f0 = f1 - f0;
			f0 = f0 * f0;
			f5 = f5 * f0 - f3;
			f5 = f5 * f0 + f2;
			f5 = -f5;
		}
	} else {
		f0 = 4.712389f;
		if (f6 < f0) {
			f0 = f6 - f1;
			f0 = f0 * f0;
			f5 = f5 * f0 - f3;
			f5 = f5 * f0 + f2;
			f5 = -f5;
		} else {
			f0 = f6 - f0;
			f1 = 1.5707964f;
			f0 = f1 - f0;
			f0 = f0 * f0;
			f5 = f5 * f0 - f3;
			f5 = f5 * f0 + f2;
		}
	}
	f5 = 10.0f * f5;

	pos1.x += f4;
	pos1.z -= f5;
	pos1.y += 62.0f;
	pos2.x += f4;
	pos2.y += 52.0f;
	pos2.z -= f5;

	if (isFirstCall) {
		event->userdata[0] = 0;
		event->userdata[1] = (s32)effItemGetEntry(7, pos1.x, pos1.y, pos1.z);
		event->userdata[2] = 0;
		event->field_84 = 0;
		*(s32**)((u8*)event + 0x88) = _mapAlloc(8);
	}

	eff = (EffectEntry*)event->userdata[1];
	work = *(s32**)((u8*)event + 0x88);

	switch (event->userdata[0]) {
	case 0:
		iconEntry(str_802C35D8, 0x18B);
		iconSetPos(str_802C35D8, pos2.x, pos2.y, pos2.z);
		iconFlagOn(str_802C35D8, 0x102);
		*(s32*)((u8*)eff->userdata + 0x38) = 5;
		marioChgGetItemMotion();
		event->field_84 = winMgrEntry(&coinget_window_desc);
		winMgrOpen(event->field_84);
		winMgrSetParam(event->field_84, work);
		work[0] = coin;
		work[1] = 0;
		psndBGMOff(0x3800);
		psndBGMOn(0x201, (char*)str_802C35E4);
		event->userdata[2] = 0xC8;
		event->savedTime = gp->renderTime;
		event->userdata[0] = 1;
		return 0;
	case 1:
		if ((s32)OSTicksToMilliseconds((OSTick)gp->renderTime - (OSTick)event->savedTime) > event->userdata[2]) {
			iconFlagOff(str_802C35D8, 2);
			work[1] = 1;
			event->userdata[2] = 0x1F4;
			event->savedTime = gp->renderTime;
			event->userdata[0] = 2;
		}
		return 0;
	case 2:
		if ((s32)OSTicksToMilliseconds((OSTick)gp->renderTime - (OSTick)event->savedTime) > event->userdata[2]) {
			event->userdata[0] = 0xA;
		}
		return 0;
	case 0xA:
		if (keyGetButtonTrg(0) & 0x100) {
			event->userdata[0] = 0x64;
		}
		return 0;
	case 0x64:
		pouchAddCoin(coin);
		effSoftDelete(eff);
		iconDelete(str_802C35D8);
		marioChgStayMotion();
		psndBGMOn(0xA0, 0);
		psndBGMOff(0x201);
		winMgrCloseAutoDelete(event->field_84);
		work[1] = 0;
		event->userdata[0] = 0x65;
		return 2;
	case 0x65:
		if (winMgrAction(event->field_84) == 0) {
			_mapFree(work);
			return 2;
		}
		return 0;
	default:
		return 2;
	}
}
#pragma opt_lifetimes reset


USER_FUNC(stone_ry) {
	s32* args = event->args;
	s32 item = evtGetValue(event, args[0]);
	s32 angle = evtGetValue(event, args[1]);
	ItemEntry* itemPtr = itemNameToPtr((char*)item);
	EffectEntry* eff = itemPtr->effect;
	*(u16*)((u8*)eff->userdata + 4) &= ~4;
	*(f32*)((u8*)eff->userdata + 0x1C) = (f32)(angle % 360);
	return EVT_RETURN_DONE;
}


static s32 stone_bg(EventEntry* event, BOOL isFirstCall) {
	s32* args = event->args;
	s32 item = evtGetValue(event, args[0]);
	CameraEntry* cam = camGetPtr(8);
	NpcEntry* npc;
	EffectEntry* eff;
	ItemEntry* itemPtr;

	cam->letterBoxCamId = 5;
	eff = effNameToPtr(str_802C33A4);
	*(s32*)((u8*)eff->userdata + 0x38) = 5;
	itemPtr = itemNameToPtr((char*)item);
	*(s32*)((u8*)itemPtr->effect->userdata + 0x38) = 5;
	npc = npcNameToPtr_NoAssert(str_802C3328);
	if (npc != NULL && evtGetValue(event, 0xF5DE0180) < 0xD3) {
		npcNameToPtr_NoAssert(str_802C3328)->cameraId = 5;
	} else {
		marioSetCamId(5);
	}
	return EVT_RETURN_DONE;
}


USER_FUNC(stone_bg_open) {
	EffectEntry* eff = effNameToPtr(str_802C33A4);
	*(s32*)((u8*)eff->userdata + 0x20) = 1;
	return EVT_RETURN_DONE;
}


USER_FUNC(stageclear_wait) {
	return effStageClearEndChk(effNameToPtr(str_802C3464)) ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}


USER_FUNC(set_stop) {
	s32* args = event->args;
	stop = evtGetValue(event, args[0]);
	return EVT_RETURN_DONE;
}


USER_FUNC(get_stop) {
	s32* args = event->args;
	evtSetValue(event, args[0], stop);
	return EVT_RETURN_DONE;
}


static s32 bgm_start_wait(void) {
	return psndBGMStartCheck(1) ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}


USER_FUNC(evt_sub_check_intersect) {
	s32* args = event->args;
	f32 a, b, c, d, e, f, g, h;

	a = evtGetFloat(event, args[0]);
	b = evtGetFloat(event, args[1]);
	c = evtGetFloat(event, args[2]);
	d = evtGetFloat(event, args[3]);
	e = evtGetFloat(event, args[4]);
	f = evtGetFloat(event, args[5]);
	g = evtGetFloat(event, args[6]);
	h = evtGetFloat(event, args[7]);

	if (a >= c) {
		if ((a < e && a < g) || (c > e && c > g)) {
			evtSetValue(event, args[8], 0);
			return 2;
		}
	} else {
		if ((c < e && c < g) || (a > e && a > g)) {
			evtSetValue(event, args[8], 0);
			return 2;
		}
	}

	if (b >= d) {
		if ((b < f && b < h) || (d > f && d > h)) {
			evtSetValue(event, args[8], 0);
			return 2;
		}
	} else {
		if ((d < f && d < h) || (b > f && b > h)) {
			evtSetValue(event, args[8], 0);
			return 2;
		}
	}

	{
		f32 t1 = (a - c) * (f - b) + (b - d) * (a - e);
		f32 t2 = (a - c) * (h - b) + (b - d) * (a - g);
		if (t1 * t2 > 0.0f) {
			evtSetValue(event, args[8], 0);
			return 2;
		}
		{
			f32 t3 = (e - g) * (b - f) + (f - h) * (e - a);
			f32 t4 = (e - g) * (d - f) + (f - h) * (e - c);
			if (t3 * t4 > 0.0f) {
				evtSetValue(event, args[8], 0);
				return 2;
			}
		}
	}
	evtSetValue(event, args[8], 1);
	return 2;
}
#pragma opt_propagation reset


USER_FUNC(irai_mail_check) {
	s32* args = event->args;
	u8 saved[8];
	s64 d;
	s32 i;
	s32 target = evtGetValue(event, args[0]);
	s32 result = 0;
	IRAI_MAIL_CHECK(0x1505, 0x17, 0x58D, 0x17, 0x25);
	IRAI_MAIL_CHECK(0x150A, 0x18, 0x595, 0x18, 0x26);
	IRAI_MAIL_CHECK(0x150B, 0x19, 0x59D, 0x19, 0x27);
	IRAI_MAIL_CHECK(0x150D, 0x1A, 0x5A5, 0x1A, 0x28);
	IRAI_MAIL_CHECK(0x1512, 0x1B, 0x5AD, 0x1B, 0x29);
	IRAI_MAIL_CHECK(0x1518, 0x1C, 0x5B5, 0x1C, 0x2A);
	evtSetValue(event, args[1], result);
	return EVT_RETURN_DONE;
}

USER_FUNC(mail_evt_gor_04_keyoff_check) {
	s32* args = event->args;
	s32 ctrlOff = marioCtrlOffChk();
	s32 keyOff = marioKeyOffChk();
	evtSetValue(event, args[0], keyOff + ctrlOff);
	return EVT_RETURN_DONE;
}

u32 tree_data[75] = {
    FLOAT(0.0), FLOAT(0.0), FLOAT(0.0), FLOAT(-0.5341796875),
    FLOAT(1.4462890625), FLOAT(1.623046875), FLOAT(-1.1708984375), FLOAT(3.1748046875),
    FLOAT(3.56640625), FLOAT(-1.6044921875), FLOAT(4.3388671875), FLOAT(4.87109375),
    FLOAT(-1.533203125), FLOAT(4.0908203125), FLOAT(4.580078125), FLOAT(-0.685546875),
    FLOAT(1.74609375), FLOAT(1.892578125), FLOAT(0.7158203125), FLOAT(-2.0673828125),
    FLOAT(-2.482421875), FLOAT(2.228515625), FLOAT(-6.2314453125), FLOAT(-7.2392578125),
    FLOAT(3.4111328125), FLOAT(-9.6240234375), FLOAT(-11.072265625), FLOAT(3.822265625),
    FLOAT(-11.1240234375), FLOAT(-12.6748046875), FLOAT(3.1015625), FLOAT(-9.98828125),
    FLOAT(-11.146484375), FLOAT(1.5439453125), FLOAT(-6.962890625), FLOAT(-7.357421875),
    FLOAT(-0.3095703125), FLOAT(-3.1650390625), FLOAT(-2.658203125), FLOAT(-1.9228515625),
    FLOAT(0.2880859375), FLOAT(1.59765625), FLOAT(-2.75390625), FLOAT(2.279296875),
    FLOAT(4.060546875), FLOAT(-2.5517578125), FLOAT(2.4150390625), FLOAT(4.2607421875),
    FLOAT(-1.6767578125), FLOAT(1.439453125), FLOAT(3.0986328125), FLOAT(-0.5048828125),
    FLOAT(-0.0546875), FLOAT(1.2783203125), FLOAT(0.583984375), FLOAT(-1.4775390625),
    FLOAT(-0.4921875), FLOAT(1.21484375), FLOAT(-2.2353515625), FLOAT(-1.509765625),
    FLOAT(1.3037109375), FLOAT(-2.2197265625), FLOAT(-1.6611328125), FLOAT(1.1015625),
    FLOAT(-1.8271484375), FLOAT(-1.416015625), FLOAT(0.734375), FLOAT(-1.2177734375),
    FLOAT(-0.943359375), FLOAT(0.3251953125), FLOAT(-0.5546875), FLOAT(-0.4150390625),
    FLOAT(0.0), FLOAT(0.0), FLOAT(0.0),
};

EVT_BEGIN(tree_evt)
    SET_TYPE(0)
    SET_VALUE(LF(0), 0)
    IF_INT_NOT_EQUAL(LW(3), 0)
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_NOT_EQUAL(LW(4), 0)
        SET_VALUE(LF(0), 1)
    END_IF
    IF_INT_EQUAL(LF(0), 1)
        CALL(evt_mapobj_get_position, LW(2), LW(13), LW(14), LW(15))
        CALL(evt_snd_sfxon_3d, STRING((void*)str_802C32C0), LW(13), LW(14), LW(15), 0)
        CALL(evt_snd_sfxon_3d, STRING((void*)str_802C32D0), LW(13), LW(14), LW(15), 0)
        CALL(evt_mapobj_get_position, LW(2), LW(13), LW(14), LW(15))
        CALL(evt_mapobj_get_position, LW(3), LW(10), LW(11), LW(12))
        SET_VALUE(LW(14), LW(11))
        CALL(evt_mario_get_pos, 0, LW(10), LW(11), LW(12))
        SET_VALUE(LW(15), LW(12))
        SUBTRACT(LW(14), 25)
        SWITCH(LW(0))
            CASE_EQUAL(0)
            CALL(evt_eff64, 0, STRING((void*)str_802C32E8), 0, LW(13), LW(14), LW(15), 60, 0, 0, 0, 0, 0, 0, 0)
            CASE_EQUAL(1)
            CALL(evt_eff64, 0, STRING((void*)str_802C32E8), 3, LW(13), LW(14), LW(15), 60, 0, 0, 0, 0, 0, 0, 0)
            CASE_EQUAL(2)
            CALL(evt_eff64, 0, STRING((void*)str_802C32E8), 4, LW(13), LW(14), LW(15), 60, 0, 0, 0, 0, 0, 0, 0)
            CASE_EQUAL(3)
            CALL(evt_eff64, 0, STRING((void*)str_802C32E8), 4, LW(13), LW(14), LW(15), 60, 0, 0, 0, 0, 0, 0, 0)
            CASE_EQUAL(4)
            CALL(evt_eff64, 0, STRING((void*)str_802C32E8), 3, LW(13), LW(14), LW(15), 60, 0, 0, 0, 0, 0, 0, 0)
            CASE_EQUAL(5)
            CALL(evt_eff64, 0, STRING((void*)str_802C32E8), 2, LW(13), LW(14), LW(15), 60, 0, 0, 0, 0, 0, 0, 0)
            CASE_EQUAL(6)
            CALL(evt_eff64, 0, STRING((void*)str_802C32E8), 2, LW(13), LW(14), LW(15), 60, 0, 0, 0, 0, 0, 0, 0)
        END_SWITCH
    END_IF
    IF_INT_NOT_EQUAL(LW(5), 0)
        SPAWN_EVENT(LW(5))
    END_IF
    SET_INT_READ_ADDR(PTR(tree_data))
    LOOP(25)
        READ_INT3(LW(10), LW(11), LW(12))
        IF_INT_NOT_EQUAL(LW(2), 0)
            CALL(evt_mapobj_rotate, 1, LW(2), FLOAT(0.0f), FLOAT(0.0f), LW(10))
        END_IF
        IF_INT_NOT_EQUAL(LW(3), 0)
            CALL(evt_mapobj_trans, 1, LW(3), LW(11), FLOAT(0.0f), FLOAT(0.0f))
        END_IF
        IF_INT_NOT_EQUAL(LW(4), 0)
            CALL(evt_mapobj_trans, 1, LW(4), LW(12), FLOAT(0.0f), FLOAT(0.0f))
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_sub_tree_access_entry)
    SET_INT_READ_ADDR(LW(0))
    LOOP(0)
        READ_INT3(LW(0), LW(1), LW(2))
        READ_INT3(LW(3), LW(4), LW(5))
        IF_INT_EQUAL(LW(0), 7)
            BREAK_LOOP
        END_IF
        CALL(evt_run_case_evt, 13, 1, LW(1), 0, PTR(tree_evt), 0)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(kusa_evt)
    SET_TYPE(0)
    CALL(evt_mario_get_pos, 0, LW(10), LW(11), LW(12))
    CALL(evt_snd_sfxon_3d, STRING((void*)str_802C32F8), LW(10), LW(11), LW(12), 0)
    IF_INT_NOT_EQUAL(LW(3), 0)
        SPAWN_EVENT(LW(3))
    END_IF
    SET_INT_READ_ADDR(PTR(kusa_data))
    LOOP(12)
        READ_INT(LW(10))
        IF_INT_NOT_EQUAL(LW(2), 0)
            CALL(evt_mapobj_trans, 1, LW(2), LW(10), FLOAT(0.0f), FLOAT(0.0f))
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_sub_kusa_access_entry)
    SET_INT_READ_ADDR(LW(0))
    LOOP(0)
        READ_INT3(LW(0), LW(1), LW(2))
        READ_INT(LW(3))
        IF_INT_EQUAL(LW(0), 4)
            BREAK_LOOP
        END_IF
        CALL(evt_run_case_evt, 9, 1, LW(1), 0, PTR(kusa_evt), 0)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(evt_sub_starstone)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_party_stop, 0)
    SWITCH(LW(0))
        CASE_EQUAL(1)
        CALL(evt_pouch_get_starstone, 1)
        CALL(evt_pouch_add_item, 114, LW(2))
        CASE_EQUAL(2)
        CALL(evt_pouch_get_starstone, 2)
        CALL(evt_pouch_add_item, 115, LW(2))
        CASE_EQUAL(3)
        CALL(evt_pouch_get_starstone, 3)
        CALL(evt_pouch_add_item, 116, LW(2))
        CASE_EQUAL(4)
        CALL(evt_pouch_get_starstone, 4)
        CALL(evt_pouch_add_item, 117, LW(2))
        CASE_EQUAL(5)
        CALL(evt_pouch_get_starstone, 5)
        CALL(evt_pouch_add_item, 118, LW(2))
        CASE_EQUAL(6)
        CALL(evt_pouch_get_starstone, 6)
        CALL(evt_pouch_add_item, 119, LW(2))
        CASE_EQUAL(7)
        CALL(evt_pouch_get_starstone, 7)
        CALL(evt_pouch_add_item, 120, LW(2))
        CASE_EQUAL(-1)
    END_SWITCH
    IF_INT_EQUAL(LW(0), 1)
        SET_VALUE(LF(1), 1)
    END_IF
    IF_INT_EQUAL(LW(0), 2)
        SET_VALUE(LF(2), 1)
    END_IF
    IF_INT_EQUAL(LW(0), 3)
        SET_VALUE(LF(3), 1)
    END_IF
    IF_INT_EQUAL(LW(0), 4)
        SET_VALUE(LF(4), 1)
    END_IF
    IF_INT_EQUAL(LW(0), 5)
        SET_VALUE(LF(5), 1)
    END_IF
    IF_INT_EQUAL(LW(0), 6)
        SET_VALUE(LF(6), 1)
    END_IF
    IF_INT_EQUAL(LW(0), 7)
        SET_VALUE(LF(7), 1)
    END_IF
    IF_INT_EQUAL(LW(0), -1)
        SET_VALUE(LF(8), 1)
    END_IF
    CALL(set_stop, 0)
    BEGIN_INLINE_EVENT
        WAIT_MS(9700)
        CALL(set_stop, 1)
    END_INLINE_EVENT
    CALL(evt_snd_sfxon, STRING((void*)str_802C330C), 0)
    CALL(evt_fade_entry, 10, 300, 255, 255, 255)
    CALL(evt_fade_end_wait)
    IF_INT_EQUAL(LF(8), 1)
        CALL(evt_npc_get_position, STRING((void*)str_802C3328), LW(2), LW(3), LW(4))
    ELSE
        CALL(evt_mario_get_pos, 0, LW(2), LW(3), LW(4))
    END_IF
    CALL(evt_sub_get_sincos, 0, LW(5), LW(6))
    MULTIPLY_FLOAT(LW(5), FLOAT(40.0f))
    MULTIPLY_FLOAT(LW(6), FLOAT(40.0f))
    ADD_FLOAT(LW(5), LW(2))
    ADD_FLOAT(LW(6), LW(4))
    CALL(evt_item_set_position, LW(1), LW(5), LW(3), LW(6))
    CALL(evt_mario_adjust_dir)
    WAIT_MS(500)
    IF_INT_EQUAL(LF(8), 0)
        CALL(evt_mario_set_pose, STRING("M_N_3"))
    END_IF
    CALL(evt_fade_entry, 9, 300, 255, 255, 255)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING((void*)str_802C3334))
    CALL(bgm_start_wait)
    IF_INT_EQUAL(LF(8), 1)
        CALL(evt_cam3d_evt_set_npc_rel, STRING((void*)str_802C3328), 0, 50, 397, 0, 70, 25, 2000, 11)
    ELSE
        CALL(evt_cam3d_evt_set_rel, 0, 50, 397, 0, 70, 25, 2000, 11)
    END_IF
    CALL(evt_snd_sfxon_3d, STRING((void*)str_802C334C), LW(5), LW(3), LW(6), 0)
    SET_VALUE(LW(7), LW(1))
    SET_FLOAT_VALUE(LW(8), FLOAT(35.0f))
    SET_FLOAT_VALUE(LW(9), LW(3))
    CALL(evt_sub_intpl_init, 4, 0, 3600, 170)
    LOOP(171)
        CALL(evt_sub_intpl_get_value)
        IF_INT_EQUAL(LF(8), 1)
            CALL(evt_npc_get_position, STRING((void*)str_802C3328), LW(2), LW(3), LW(4))
        ELSE
            CALL(evt_mario_get_pos, 0, LW(2), LW(3), LW(4))
        END_IF
        CALL(evt_sub_get_sincos, LW(0), LW(5), LW(6))
        MULTIPLY_FLOAT(LW(5), LW(8))
        MULTIPLY_FLOAT(LW(6), LW(8))
        ADD_FLOAT(LW(5), LW(2))
        ADD_FLOAT(LW(6), LW(4))
        CALL(evt_item_set_position, LW(7), LW(5), LW(9), LW(6))
        CALL(stone_ry, LW(7), LW(0))
        SUBTRACT_FLOAT(LW(8), FLOAT(0.2051f))
        ADD_FLOAT(LW(9), FLOAT(0.28223f))
        WAIT_FRAMES(1)
    END_LOOP
    IF_INT_NOT_EQUAL(LF(8), 1)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING((void*)str_802C3368), LW(0), LW(1), LW(2), 0)
    END_IF
    IF_INT_EQUAL(LF(8), 1)
        CALL(evt_npc_set_anim, STRING((void*)str_802C3328), STRING((void*)str_802C3380))
    ELSE
        CALL(evt_mario_set_pose, STRING("M_P_2"))
    END_IF
    CALL(evt_item_get_position, LW(7), LW(0), LW(1), LW(2))
    ADD(LW(1), 16)
    CALL(evt_eff64, STRING((void*)str_802C3388), STRING((void*)str_802C3394), 12, LW(0), LW(1), LW(2), FLOAT(1.0f), 0, 0, 0, 0, 0, 0, 0)
    ADD(LW(2), -10)
    CALL(evt_eff, STRING((void*)str_802C33A4), STRING((void*)str_802C33AC), 0, LW(0), LW(1), LW(2), 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(stone_bg, LW(7))
    IF_INT_EQUAL(LF(1), 1)
        CALL(evt_msg_print, 0, STRING((void*)str_802C33B4), 0, 0)
    END_IF
    IF_INT_EQUAL(LF(2), 1)
        CALL(evt_msg_print, 0, STRING((void*)str_802C33C4), 0, 0)
    END_IF
    IF_INT_EQUAL(LF(3), 1)
        CALL(evt_msg_print, 0, STRING((void*)str_802C33D4), 0, 0)
    END_IF
    IF_INT_EQUAL(LF(4), 1)
        CALL(evt_msg_print, 0, STRING((void*)str_802C33E4), 0, 0)
    END_IF
    IF_INT_EQUAL(LF(5), 1)
        CALL(evt_msg_print, 0, STRING((void*)str_802C33F0), 0, 0)
    END_IF
    IF_INT_EQUAL(LF(6), 1)
        CALL(evt_msg_print, 0, STRING((void*)str_802C3400), 0, 0)
    END_IF
    IF_INT_EQUAL(LF(7), 1)
        CALL(evt_msg_print, 0, STRING((void*)str_802C340C), 0, 0)
    END_IF
    IF_INT_EQUAL(LF(8), 1)
        CALL(evt_msg_print, 0, STRING((void*)str_802C341C), 0, 0)
    END_IF
    LOOP(0)
        CALL(get_stop, LW(0))
        IF_INT_EQUAL(LW(0), 1)
            BREAK_LOOP
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    CALL(evt_snd_bgmon, 512, STRING((void*)str_802C342C))
    CALL(evt_snd_envoff, 512)
    CALL(evt_eff_softdelete, STRING((void*)str_802C3388))
    CALL(stone_bg_open, LW(7))
    BEGIN_INLINE_EVENT
        IF_INT_EQUAL(LF(1), 1)
            SET_VALUE(LW(0), STRING((void*)str_802C3444))
        END_IF
        IF_INT_EQUAL(LF(2), 1)
            SET_VALUE(LW(0), STRING("S"))
        END_IF
        IF_INT_EQUAL(LF(3), 1)
            SET_VALUE(LW(0), STRING("S"))
        END_IF
        IF_INT_EQUAL(LF(4), 1)
            SET_VALUE(LW(0), STRING((void*)str_802C3450))
        END_IF
        IF_INT_EQUAL(LF(5), 1)
            SET_VALUE(LW(0), STRING("S"))
        END_IF
        IF_INT_EQUAL(LF(6), 1)
            SET_VALUE(LW(0), STRING("S"))
        END_IF
        IF_INT_EQUAL(LF(7), 1)
            SET_VALUE(LW(0), STRING((void*)str_802C3458))
        END_IF
        IF_INT_EQUAL(LF(8), 1)
            SET_VALUE(LW(0), STRING((void*)str_802C3450))
        END_IF
        WAIT_FRAMES(10)
        CALL(evt_mapobj_flag_onoff, 1, 1, LW(0), 1)
    END_INLINE_EVENT
    CALL(evt_fade_entry, 34, 0, 0, 0, 0)
    CALL(evt_fade_end_wait)
    WAIT_MS(1000)
    CALL(evt_eff, STRING((void*)str_802C3464), STRING((void*)str_802C3470), 10, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0)
    CALL(stageclear_wait)
    IF_INT_NOT_EQUAL(LF(8), 1)
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_snd_sfxon_3d, STRING((void*)str_802C347C), LW(0), LW(1), LW(2), 0)
    END_IF
    WAIT_MS(2000)
    CALL(evt_fade_entry, 30, 0, 0, 0, 0)
    CALL(evt_fade_end_wait)
    CALL(evt_item_delete, LW(7))
    WAIT_MS(1000)
    IF_INT_EQUAL(LF(1), 1)
        CALL(evt_msg_print, 0, STRING((void*)str_802C3494), 0, 0)
    END_IF
    IF_INT_EQUAL(LF(2), 1)
        CALL(evt_msg_print, 0, STRING((void*)str_802C34A0), 0, 0)
    END_IF
    IF_INT_EQUAL(LF(3), 1)
        CALL(evt_msg_print, 0, STRING((void*)str_802C34B0), 0, 0)
    END_IF
    IF_INT_EQUAL(LF(4), 1)
        CALL(evt_msg_print, 0, STRING((void*)str_802C34C0), 0, 0)
    END_IF
    IF_INT_EQUAL(LF(5), 1)
        CALL(evt_msg_print, 0, STRING((void*)str_802C34D0), 0, 0)
    END_IF
    IF_INT_EQUAL(LF(6), 1)
        CALL(evt_msg_print, 0, STRING((void*)str_802C34E0), 0, 0)
    END_IF
    IF_INT_EQUAL(LF(7), 1)
        CALL(evt_msg_print, 0, STRING((void*)str_802C34F0), 0, 0)
    END_IF
    IF_INT_EQUAL(LF(8), 1)
        CALL(evt_msg_print, 0, STRING((void*)str_802C3500), 0, 0)
    END_IF
    WAIT_MS(1000)
    IF_INT_EQUAL(LF(1), 1)
        SET_VALUE(LW(0), 1)
    END_IF
    IF_INT_EQUAL(LF(2), 1)
        SET_VALUE(LW(0), 4)
    END_IF
    IF_INT_EQUAL(LF(3), 1)
        SET_VALUE(LW(0), 7)
    END_IF
    IF_INT_EQUAL(LF(4), 1)
        SET_VALUE(LW(0), 10)
    END_IF
    IF_INT_EQUAL(LF(5), 1)
        SET_VALUE(LW(0), 13)
    END_IF
    IF_INT_EQUAL(LF(6), 1)
        SET_VALUE(LW(0), 16)
    END_IF
    IF_INT_EQUAL(LF(7), 1)
        SET_VALUE(LW(0), 19)
    END_IF
    IF_INT_EQUAL(LF(8), 1)
        SET_VALUE(LW(0), 22)
    END_IF
    SET_VALUE(LF(10), 0)
    CALL_CHILD(evt_memcard_save)
    CALL(evt_mario_init_camid)
    CALL(evt_party_init_camid, 0)
    CALL(evt_party_init_camid, 1)
    IF_INT_EQUAL(LF(1), 1)
        CALL(evt_snd_bgmoff_f, 512, 2000)
        CALL(evt_snd_envoff_f, 512, 2000)
        CALL(evt_snd_sfx_all_stop)
        CALL(evt_fade_entry, 17, 1000, 0, 0, 0)
        CALL(evt_fade_end_wait)
        WAIT_MS(2000)
        CALL(evt_fade_set_mapchange_type, 1, 55, 0, 54, 0)
        CALL(evt_bero_mapchange, STRING((void*)str_802C3510), 0)
    END_IF
    IF_INT_EQUAL(LF(2), 1)
        CALL(evt_snd_bgmoff_f, 512, 2000)
        CALL(evt_snd_envoff_f, 512, 2000)
        CALL(evt_snd_sfx_all_stop)
        CALL(evt_fade_entry, 17, 1000, 0, 0, 0)
        CALL(evt_fade_end_wait)
        WAIT_MS(2000)
        CALL(evt_fade_set_mapchange_type, 1, 55, 0, 54, 0)
        CALL(evt_bero_mapchange, STRING((void*)str_802C3510), 0)
    END_IF
    IF_INT_EQUAL(LF(3), 1)
        CALL(evt_snd_bgmoff_f, 512, 2000)
        CALL(evt_snd_envoff_f, 512, 2000)
        CALL(evt_snd_sfx_all_stop)
        CALL(evt_fade_entry, 17, 1000, 0, 0, 0)
        CALL(evt_fade_end_wait)
        WAIT_MS(2000)
        CALL(evt_fade_set_mapchange_type, 1, 55, 0, 54, 0)
        CALL(evt_bero_mapchange, STRING((void*)str_802C3510), 0)
    END_IF
    IF_INT_EQUAL(LF(4), 1)
        CALL(evt_fade_set_mapchange_type, 1, 17, 800, 16, 800)
        CALL(evt_bero_mapchange, STRING((void*)str_802C3518), 0)
    END_IF
    IF_INT_EQUAL(LF(5), 1)
        CALL(evt_fade_set_mapchange_type, 1, 17, 800, 16, 800)
        CALL(evt_bero_mapchange, STRING((void*)str_802C3520), 0)
    END_IF
    IF_INT_EQUAL(LF(6), 1)
        CALL(evt_fade_set_mapchange_type, 0, 17, 800, 16, 800)
        CALL(evt_bero_mapchange, STRING((void*)str_802C3528), STRING("next"))
    END_IF
    IF_INT_EQUAL(LF(7), 1)
        CALL(evt_snd_bgmoff_f, 512, 2000)
        CALL(evt_snd_envoff_f, 512, 2000)
        CALL(evt_snd_sfx_all_stop)
        CALL(evt_fade_entry, 17, 1000, 0, 0, 0)
        CALL(evt_fade_end_wait)
        WAIT_MS(2000)
        CALL(evt_fade_set_mapchange_type, 1, 55, 0, 54, 0)
        CALL(evt_bero_mapchange, STRING((void*)str_802C3530), 0)
    END_IF
    IF_INT_EQUAL(LF(8), 1)
        CALL(evt_snd_bgmoff_f, 512, 2000)
        CALL(evt_snd_envoff_f, 512, 2000)
        CALL(evt_snd_sfx_all_stop)
        CALL(evt_fade_entry, 17, 1000, 0, 0, 0)
        CALL(evt_fade_end_wait)
        WAIT_MS(2000)
        CALL(evt_eff_delete, STRING((void*)str_802C33A4))
        CALL(evt_eff_delete, STRING((void*)str_802C3464))
        CALL(evt_mapobj_flag_onoff, 1, 0, STRING((void*)str_802C3450), 1)
        CALL(evt_fade_reset, 1)
        CALL(evt_npc_set_camid, STRING((void*)str_802C3328), 4)
        CALL(evt_mario_set_pose, STRING("M_D_2"))
        CALL(evt_fade_entry, 16, 1000, 0, 0, 0)
        CALL(evt_fade_end_wait)
    END_IF
    CALL(evt_mario_key_onoff, 1)
    RETURN
EVT_END()

EVT_BEGIN(evt_sub_mail_vibration)
    LOOP(10)
        LOOP(10)
            CALL(evt_sub_rumble_onoff, 0, 0)
            WAIT_MS(50)
            CALL(evt_sub_rumble_onoff, 1, 0)
            WAIT_MS(50)
        END_LOOP
        CALL(evt_sub_rumble_onoff, 2, 0)
        WAIT_MS(2000)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(mail_evt)
    CALL(evt_cam_letter_box_disable, 1)
    CALL(evt_mario_key_onoff, 0)
    CALL(evt_mario_normalize)
    CALL(evt_party_yoshi_ride, 0, 0)
    CALL(evt_mario_wait_rideon)
    CALL(evt_mario_balloon_bikkuri)
    CALL(evt_snd_bgmoff, 1024)
    CALL(evt_snd_bgmon, 1, STRING((void*)str_802C3538))
    SPAWN_EVENT_GET_ID(PTR(evt_sub_mail_vibration), LW(10))
    WAIT_MS(500)
    CALL(evt_win_one_message, 0)
    DELETE_EVENT(LW(10))
    WAIT_FRAMES(1)
    CALL(evt_sub_rumble_onoff, 1, 0)
    CALL(evt_mario_balloon_off)
    CALL(evt_snd_bgmoff, 513)
    CALL(evt_snd_bgmon, 288, 0)
    IF_INT_EQUAL(GSWF(37), 0)
        SET_VALUE(GSWF(37), 1)
        CALL(evt_msg_print, 0, STRING((void*)str_802C3550), 0, 0)
    ELSE
        IF_INT_EQUAL(GSWF(38), 0)
            CALL(evt_msg_print, 0, STRING((void*)str_802C3550), 0, 0)
        END_IF
    END_IF
    CALL(evt_mario_key_onoff, 1)
    CALL(evt_cam_letter_box_disable, 0)
    RETURN
EVT_END()

EVT_BEGIN(mail_evt_nok_01)
    IF_INT_EQUAL(GSWF(7), 1)
        RETURN
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    LOOP(0)
        SET_VALUE(LW(10), LW(0))
        SET_VALUE(LW(11), LW(1))
        SET_VALUE(LW(12), LW(2))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_sub_check_intersect, LW(0), LW(2), LW(10), LW(12), 190, -2000, 190, 2000, LW(5))
        IF_INT_EQUAL(GSW(0), 27)
            IF_INT_EQUAL(LW(5), 1)
                BREAK_LOOP
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    SET_VALUE(GSWF(7), 1)
    CALL(evt_pouch_receive_mail, 16)
    CALL(evt_pouch_receive_mail, 26)
    CALL_CHILD(mail_evt)
    RETURN
EVT_END()

EVT_BEGIN(mail_evt_gor_03)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    LOOP(0)
        SET_VALUE(LW(10), LW(0))
        SET_VALUE(LW(11), LW(1))
        SET_VALUE(LW(12), LW(2))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_sub_check_intersect, LW(0), LW(2), LW(10), LW(12), -100, -2000, -100, 2000, LW(5))
        IF_INT_EQUAL(LW(5), 1)
            SET_VALUE(LF(0), 0)
            IF_INT_EQUAL(GSW(0), 125)
                IF_INT_EQUAL(GSWF(8), 0)
                    SET_VALUE(GSWF(8), 1)
                    CALL(evt_pouch_receive_mail, 17)
                    SET_VALUE(LF(0), 1)
                END_IF
            END_IF
            CALL(irai_mail_check, 0, LW(5))
            IF_INT_EQUAL(LW(5), 1)
                CALL(irai_mail_check, 1, LW(5))
                SET_VALUE(LF(0), 1)
            END_IF
            IF_INT_EQUAL(LF(0), 1)
                CALL_CHILD(mail_evt)
                SET_VALUE(LF(0), 0)
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(mail_evt_gor_02)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    LOOP(0)
        SET_VALUE(LW(10), LW(0))
        SET_VALUE(LW(11), LW(1))
        SET_VALUE(LW(12), LW(2))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_sub_check_intersect, LW(0), LW(2), LW(10), LW(12), -280, -50, -230, 220, LW(5))
        IF_INT_EQUAL(LW(5), 1)
            SET_VALUE(LF(0), 0)
            IF_INT_EQUAL(GSW(0), 175)
                IF_INT_EQUAL(GSWF(9), 0)
                    SET_VALUE(GSWF(9), 1)
                    CALL(evt_pouch_receive_mail, 18)
                    SET_VALUE(LF(0), 1)
                END_IF
            END_IF
            CALL(irai_mail_check, 0, LW(5))
            IF_INT_EQUAL(LW(5), 1)
                CALL(irai_mail_check, 1, LW(5))
                SET_VALUE(LF(0), 1)
            END_IF
            IF_INT_EQUAL(LF(0), 1)
                CALL_CHILD(mail_evt)
                SET_VALUE(LF(0), 0)
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(mail_evt_gor_01)
    IF_INT_EQUAL(GSWF(10), 1)
        RETURN
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    LOOP(0)
        SET_VALUE(LW(10), LW(0))
        SET_VALUE(LW(11), LW(1))
        SET_VALUE(LW(12), LW(2))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_sub_check_intersect, LW(0), LW(2), LW(10), LW(12), 350, -2000, 350, 2000, LW(5))
        IF_INT_EQUAL(GSW(0), 228)
            IF_INT_EQUAL(LW(5), 1)
                BREAK_LOOP
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    SET_VALUE(GSWF(10), 1)
    CALL(evt_pouch_receive_mail, 19)
    CALL_CHILD(mail_evt)
    RETURN
EVT_END()

EVT_BEGIN(mail_evt_gor_04)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    LOOP(0)
        SET_VALUE(LW(10), LW(0))
        SET_VALUE(LW(11), LW(1))
        SET_VALUE(LW(12), LW(2))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_sub_check_intersect, LW(0), LW(2), LW(10), LW(12), 30, -2000, 30, 2000, LW(5))
        CALL(mail_evt_gor_04_keyoff_check, LW(3))
        IF_INT_GREATER(LW(3), 0)
            SET_VALUE(LW(5), 0)
        END_IF
        IF_INT_EQUAL(LW(5), 1)
            SET_VALUE(LF(0), 0)
            IF_INT_EQUAL(GSW(0), 280)
                IF_INT_EQUAL(GSWF(11), 0)
                    SET_VALUE(GSWF(11), 1)
                    CALL(evt_pouch_receive_mail, 20)
                    SET_VALUE(LF(0), 1)
                END_IF
            END_IF
            CALL(irai_mail_check, 0, LW(5))
            IF_INT_EQUAL(LW(5), 1)
                CALL(irai_mail_check, 1, LW(5))
                SET_VALUE(LF(0), 1)
            END_IF
            IF_INT_EQUAL(LF(0), 1)
                CALL_CHILD(mail_evt)
                SET_VALUE(LF(0), 0)
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(mail_evt_rsh_03_a)
    IF_INT_EQUAL(GSWF(12), 1)
        RETURN
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    LOOP(0)
        SET_VALUE(LW(10), LW(0))
        SET_VALUE(LW(11), LW(1))
        SET_VALUE(LW(12), LW(2))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_sub_check_intersect, LW(0), LW(2), LW(10), LW(12), 400, -2000, 400, 2000, LW(5))
        IF_INT_GREATER_OR_EQUAL(GSW(0), 328)
            IF_INT_EQUAL(LW(5), 1)
                BREAK_LOOP
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    SET_VALUE(GSWF(12), 1)
    CALL(evt_pouch_receive_mail, 21)
    CALL_CHILD(mail_evt)
    RETURN
EVT_END()

EVT_BEGIN(mail_evt_hom_00)
    IF_INT_EQUAL(GSWF(13), 1)
        RETURN
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    LOOP(0)
        SET_VALUE(LW(10), LW(0))
        SET_VALUE(LW(11), LW(1))
        SET_VALUE(LW(12), LW(2))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_sub_check_intersect, LW(0), LW(2), LW(10), LW(12), -600, -2000, -600, 2000, LW(5))
        IF_INT_EQUAL(GSW(0), 325)
            IF_INT_EQUAL(LW(5), 1)
                BREAK_LOOP
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    SET_VALUE(GSWF(13), 1)
    CALL(evt_pouch_receive_mail, 22)
    CALL_CHILD(mail_evt)
    RETURN
EVT_END()

EVT_BEGIN(mail_evt_gor_02_2)
    IF_INT_EQUAL(GSWF(14), 1)
        RETURN
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    LOOP(0)
        SET_VALUE(LW(10), LW(0))
        SET_VALUE(LW(11), LW(1))
        SET_VALUE(LW(12), LW(2))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_sub_check_intersect, LW(0), LW(2), LW(10), LW(12), -1000, -4226, 1000, 7374, LW(5))
        IF_INT_EQUAL(GSW(0), 348)
            IF_INT_EQUAL(LW(5), 1)
                BREAK_LOOP
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    SET_VALUE(GSWF(14), 1)
    CALL(evt_pouch_receive_mail, 23)
    CALL_CHILD(mail_evt)
    RETURN
EVT_END()

EVT_BEGIN(mail_evt_bom_01)
    IF_INT_EQUAL(GSWF(15), 1)
        RETURN
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    LOOP(0)
        SET_VALUE(LW(10), LW(0))
        SET_VALUE(LW(11), LW(1))
        SET_VALUE(LW(12), LW(2))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_sub_check_intersect, LW(0), LW(2), LW(10), LW(12), -230, -2000, -230, 2000, LW(5))
        IF_INT_EQUAL(GSW(0), 354)
            IF_INT_EQUAL(GSWF(3874), 1)
                IF_INT_EQUAL(LW(5), 1)
                    BREAK_LOOP
                END_IF
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    SET_VALUE(GSWF(15), 1)
    CALL(evt_pouch_receive_mail, 24)
    CALL_CHILD(mail_evt)
    RETURN
EVT_END()

EVT_BEGIN(mail_evt_tik_00)
    IF_INT_EQUAL(GSWF(16), 1)
        RETURN
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    LOOP(0)
        SET_VALUE(LW(10), LW(0))
        SET_VALUE(LW(11), LW(1))
        SET_VALUE(LW(12), LW(2))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_sub_check_intersect, LW(0), LW(2), LW(10), LW(12), 470, -2000, 470, 2000, LW(5))
        CALL(evt_sub_check_intersect, LW(0), LW(2), LW(10), LW(12), -500, -2000, -500, 2000, LW(6))
        IF_INT_EQUAL(GSW(0), 378)
            IF_INT_EQUAL(LW(5), 1)
                BREAK_LOOP
            END_IF
            IF_INT_EQUAL(LW(6), 1)
                BREAK_LOOP
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    SET_VALUE(GSWF(16), 1)
    CALL(evt_pouch_receive_mail, 25)
    CALL(evt_pouch_receive_mail, 33)
    CALL_CHILD(mail_evt)
    RETURN
EVT_END()

EVT_BEGIN(mail_evt_win_01)
    IF_INT_EQUAL(GSWF(17), 1)
        RETURN
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    LOOP(0)
        SET_VALUE(LW(10), LW(0))
        SET_VALUE(LW(11), LW(1))
        SET_VALUE(LW(12), LW(2))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_sub_check_intersect, LW(0), LW(2), LW(10), LW(12), 390, -2000, 390, 2000, LW(5))
        IF_INT_EQUAL(GSW(0), 87)
            IF_INT_EQUAL(LW(5), 1)
                BREAK_LOOP
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    SET_VALUE(GSWF(17), 1)
    CALL(evt_pouch_receive_mail, 27)
    CALL_CHILD(mail_evt)
    RETURN
EVT_END()

EVT_BEGIN(mail_evt_tou_01)
    IF_INT_EQUAL(GSWF(18), 1)
        RETURN
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    LOOP(0)
        SET_VALUE(LW(10), LW(0))
        SET_VALUE(LW(11), LW(1))
        SET_VALUE(LW(12), LW(2))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_sub_check_intersect, LW(0), LW(2), LW(10), LW(12), -50, -2000, -50, 2000, LW(5))
        IF_INT_EQUAL(GSW(0), 137)
            IF_INT_EQUAL(LW(5), 1)
                BREAK_LOOP
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    SET_VALUE(GSWF(18), 1)
    CALL(evt_pouch_receive_mail, 28)
    SET_VALUE(GSWF(250), 1)
    CALL(evt_sub_save_playtime, GSW(5))
    CALL_CHILD(mail_evt)
    RETURN
EVT_END()

EVT_BEGIN(mail_evt_usu_01)
    IF_INT_EQUAL(GSWF(19), 1)
        RETURN
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    LOOP(0)
        SET_VALUE(LW(10), LW(0))
        SET_VALUE(LW(11), LW(1))
        SET_VALUE(LW(12), LW(2))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_sub_check_intersect, LW(0), LW(2), LW(10), LW(12), 110, -2000, 110, 2000, LW(5))
        IF_INT_EQUAL(GSW(0), 187)
            IF_INT_EQUAL(LW(5), 1)
                BREAK_LOOP
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    SET_VALUE(GSWF(19), 1)
    CALL(evt_pouch_receive_mail, 29)
    CALL_CHILD(mail_evt)
    RETURN
EVT_END()

EVT_BEGIN(mail_evt_gor_01_2)
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    LOOP(0)
        SET_VALUE(LW(10), LW(0))
        SET_VALUE(LW(11), LW(1))
        SET_VALUE(LW(12), LW(2))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_sub_check_intersect, LW(0), LW(2), LW(10), LW(12), -2000, 200, 2000, 200, LW(5))
        IF_INT_EQUAL(LW(5), 1)
            SET_VALUE(LF(0), 0)
            IF_INT_EQUAL(GSW(0), 270)
                IF_INT_EQUAL(GSWF(20), 0)
                    SET_VALUE(GSWF(20), 1)
                    CALL(evt_pouch_receive_mail, 30)
                    SET_VALUE(GSWF(251), 1)
                    CALL(evt_sub_save_playtime, GSW(14))
                    SET_VALUE(LF(0), 1)
                END_IF
            END_IF
            CALL(irai_mail_check, 0, LW(5))
            IF_INT_EQUAL(LW(5), 1)
                CALL(irai_mail_check, 1, LW(5))
                SET_VALUE(LF(0), 1)
            END_IF
            IF_INT_EQUAL(LF(0), 1)
                CALL_CHILD(mail_evt)
                SET_VALUE(LF(0), 0)
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

EVT_BEGIN(mail_evt_rsh_03_a_2)
    IF_INT_EQUAL(GSWF(21), 1)
        RETURN
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    LOOP(0)
        SET_VALUE(LW(10), LW(0))
        SET_VALUE(LW(11), LW(1))
        SET_VALUE(LW(12), LW(2))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_sub_check_intersect, LW(0), LW(2), LW(10), LW(12), 400, -2000, 400, 2000, LW(5))
        IF_INT_EQUAL(GSW(0), 303)
            IF_INT_EQUAL(LW(5), 1)
                BREAK_LOOP
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    SET_VALUE(GSWF(21), 1)
    CALL(evt_pouch_receive_mail, 31)
    CALL_CHILD(mail_evt)
    RETURN
EVT_END()

EVT_BEGIN(mail_evt_pik_00)
    IF_INT_EQUAL(GSWF(22), 1)
        RETURN
    END_IF
    CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
    LOOP(0)
        SET_VALUE(LW(10), LW(0))
        SET_VALUE(LW(11), LW(1))
        SET_VALUE(LW(12), LW(2))
        CALL(evt_mario_get_pos, 0, LW(0), LW(1), LW(2))
        CALL(evt_sub_check_intersect, LW(0), LW(2), LW(10), LW(12), -60, -2000, -60, 2000, LW(5))
        IF_INT_EQUAL(GSW(0), 354)
            IF_INT_EQUAL(GSWF(3874), 1)
                IF_INT_EQUAL(LW(5), 1)
                    BREAK_LOOP
                END_IF
            END_IF
        END_IF
        WAIT_FRAMES(1)
    END_LOOP
    SET_VALUE(GSWF(22), 1)
    CALL(evt_pouch_receive_mail, 32)
    SET_VALUE(GSWF(252), 1)
    CALL(evt_sub_save_playtime, GSW(23))
    CALL_CHILD(mail_evt)
    RETURN
EVT_END()

// The END word closing this script is lbl_803080CC (a separate symbol in the original).
EVT_BEGIN(evt_sub_mail_event)
    CALL(evt_sub_get_system_flag, LW(0))
    IF_BITS_SET(LW(0), 4096)
        RETURN
    END_IF
    CALL(evt_mario_get_mode, LW(0))
    IF_INT_NOT_EQUAL(LW(0), 0)
        RETURN
    END_IF
    CALL(evt_sub_get_mapname, LW(0))
    IF_STR_EQUAL(LW(0), STRING("nok_01"))
        CALL_CHILD(mail_evt_nok_01)
        RETURN
    END_IF
    IF_STR_EQUAL(LW(0), STRING("gor_03"))
        CALL_CHILD(mail_evt_gor_03)
        RETURN
    END_IF
    IF_STR_EQUAL(LW(0), STRING("gor_02"))
        SPAWN_EVENT(mail_evt_gor_02)
        CALL_CHILD(mail_evt_gor_02_2)
        RETURN
    END_IF
    IF_STR_EQUAL(LW(0), STRING("gor_01"))
        SPAWN_EVENT(mail_evt_gor_01_2)
        CALL_CHILD(mail_evt_gor_01)
        RETURN
    END_IF
    IF_STR_EQUAL(LW(0), STRING("gor_04"))
        CALL_CHILD(mail_evt_gor_04)
        RETURN
    END_IF
    IF_STR_EQUAL(LW(0), STRING("rsh_03_a"))
        CALL_CHILD(mail_evt_rsh_03_a_2)
        CALL_CHILD(mail_evt_rsh_03_a)
        RETURN
    END_IF
    IF_STR_EQUAL(LW(0), STRING("hom_00"))
        CALL_CHILD(mail_evt_hom_00)
        RETURN
    END_IF
    IF_STR_EQUAL(LW(0), STRING("bom_01"))
        CALL_CHILD(mail_evt_bom_01)
        RETURN
    END_IF
    IF_STR_EQUAL(LW(0), STRING("tik_00"))
        CALL_CHILD(mail_evt_tik_00)
        RETURN
    END_IF
    IF_STR_EQUAL(LW(0), STRING("win_01"))
        CALL_CHILD(mail_evt_win_01)
        RETURN
    END_IF
    IF_STR_EQUAL(LW(0), STRING("tou_01"))
        CALL_CHILD(mail_evt_tou_01)
        RETURN
    END_IF
    IF_STR_EQUAL(LW(0), STRING("usu_01"))
        CALL_CHILD(mail_evt_usu_01)
        RETURN
    END_IF
    IF_STR_EQUAL(LW(0), STRING("pik_00"))
        CALL_CHILD(mail_evt_pik_00)
        RETURN
    END_IF
    RETURN
};

__declspec(section ".data") EVT_BEGIN(lbl_803080CC)
EVT_END()

EVT_BEGIN(evt_sub_stopwatch)
    LOOP(0)
        WAIT_FRAMES(1)
    END_LOOP
    RETURN
EVT_END()

static u32 kusa_data[12] = {
    FLOAT(0.0), FLOAT(-1.873046875), FLOAT(-2.998046875), FLOAT(-2.2197265625),
    FLOAT(-0.775390625), FLOAT(0.0), FLOAT(-0.775390625), FLOAT(-2.2197265625),
    FLOAT(-2.998046875), FLOAT(-2.4423828125), FLOAT(-1.2197265625), FLOAT(0.0),
};

WinMgrDesc coinget_window_desc = {
    { 0, 0, 0, 0, 0, 0, 0, 0 }, 8, -175, 120, 350, 42, { 0xE1, 0xD7, 0x9B, 0xFF }, NULL, coingetDisp,
};
// clang-format on
