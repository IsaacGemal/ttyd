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

USER_FUNC(mail_evt_gor_04_keyoff_check) {
	s32* args = event->args;
	s32 ctrlOff = marioCtrlOffChk();
	s32 keyOff = marioKeyOffChk();
	evtSetValue(event, args[0], keyOff + ctrlOff);
	return EVT_RETURN_DONE;
}

static s32 bgm_start_wait(void) {
	return psndBGMStartCheck(1) ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

USER_FUNC(get_stop) {
	s32* args = event->args;
	evtSetValue(event, args[0], stop);
	return EVT_RETURN_DONE;
}

USER_FUNC(set_stop) {
	s32* args = event->args;
	stop = evtGetValue(event, args[0]);
	return EVT_RETURN_DONE;
}

USER_FUNC(stageclear_wait) {
	return effStageClearEndChk(effNameToPtr(str_802C3464)) ? EVT_RETURN_DONE : EVT_RETURN_BLOCK;
}

USER_FUNC(stone_bg_open) {
	EffectEntry* eff = effNameToPtr(str_802C33A4);
	*(s32*)((u8*)eff->userdata + 0x20) = 1;
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

USER_FUNC(evt_sub_countdown_set_restart_time) {
	s32* args = event->args;
	countDownSetRestartTime(evtGetValue(event, args[0]));
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

USER_FUNC(evt_sub_countdown_end) {
	countDownEnd();
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_countdown_get_status) {
	s32* args = event->args;
	s32 status = countDownGetStatus();
	evtSetValue(event, args[0], status);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_countdown_start) {
	s32* args = event->args;
	s32 type = evtGetValue(event, args[0]);
	s32 time = evtGetValue(event, args[1]);
	countDownStart(type, time);
	return EVT_RETURN_DONE;
}

// NONMATCHING (82.45%): 3 instructions differ, 1 missing, 1 extra, plus register differences.
USER_FUNC(evt_sub_animgroup_async) {
	s32* args = event->args;
	const char* name = (const char*)evtGetValue(event, args[0]);
	s32 ret = animGroupBaseAsync(name, gp->inBattle != 0, NULL);
	s32 result;
	if (ret == 0) {
		result = EVT_RETURN_BLOCK;
	} else {
		result = EVT_RETURN_DONE;
	}
	return result;
}

USER_FUNC(evt_sub_get_language) {
	s32* args = event->args;
	evtSetValue(event, args[0], gp->language);
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

USER_FUNC(evt_sub_status_gauge_check_update) {
	s32* args = event->args;
	if (statusWinCheckUpdate()) {
		evtSetValue(event, args[0], 1);
	} else {
		evtSetValue(event, args[0], 0);
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_status_gauge_force_close) {
	statusWinForceClose();
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_status_gauge_force_open) {
	statusWinForceOpen();
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_get_areaname) {
	s32* args = event->args;
	evtSetValue(event, args[0], (s32)gp->currentAreaName);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_get_mapname) {
	s32* args = event->args;
	evtSetValue(event, args[0], (s32)gp->currentMapName);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_key_get_buttontrg) {
	s32* args = event->args;
	u32 chan = evtGetValue(event, args[0]);
	evtSetValue(event, args[1], keyGetButtonTrg(chan));
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_key_get_button) {
	s32* args = event->args;
	u32 chan = evtGetValue(event, args[0]);
	evtSetValue(event, args[1], keyGetButton(chan));
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_key_get_dir) {
	s32* args = event->args;
	u32 chan = evtGetValue(event, args[0]);
	evtSetValue(event, args[1], keyGetDir(chan));
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

USER_FUNC(evt_sub_get_system_flag) {
	s32* args = event->args;
	evtSetValue(event, args[0], gp->flags);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_spline_free) {
	void* work = (void*)event->lwData[15];
	_mapFree(*(void**)((u8*)work + 4));
	_mapFree(*(void**)((u8*)work + 0xC));
	_mapFree(work);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_sub_random) {
	s32* args = event->args;
	s32 max = evtGetValue(event, args[0]);
	evtSetValue(event, args[1], rand() % (max + 1));
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

USER_FUNC(evt_sub_get_sincos) {
	s32* args = event->args;
	f32 angle = evtGetFloat(event, args[0]);
	f32 rad = angle * 3.141592f / 180.0f;
	evtSetValue(event, args[1], (s32)((f32)sin(rad) * 1024.0f) + 0xF24A7A80);
	evtSetValue(event, args[2], (s32)((f32)cos(rad) * 1024.0f) + 0xF24A7A80);
	return EVT_RETURN_DONE;
}

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

// NONMATCHING (85.72%): 2 instructions differ, 5 missing, 6 extra, plus register differences.
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

// NONMATCHING (94.29%): 4 missing, 4 extra, plus register differences.
USER_FUNC(evt_sub_spline_get_value) {
	s32* args = event->args;
	SplineWork* work = (SplineWork*)event->lwData[15];
	f32 f1;
	f32 f31;
	f32 out[3];

	if (work->timeBased == 0) {
		f1 = intplGetValue(work->mode, work->step, work->steps, 0.0f, 1.0f);
	} else {
		u64 diff = (u64)event->lifetime - (u64)work->time;
		f31 = (f32)(diff / (OS_TIMER_CLOCK / 1000));
		f1 = intplGetValue(work->mode, (s32)f31, work->field_1C, 0.0f, 1.0f);
	}
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

USER_FUNC(evt_sub_get_dir) {
	s32* args = event->args;
	f32 f31 = (f32)evtGetValue(event, args[0]);
	f32 f30 = (f32)evtGetValue(event, args[1]);
	f32 f29 = (f32)evtGetValue(event, args[2]);
	f32 f4 = (f32)evtGetValue(event, args[3]);
	evtSetFloat(event, args[4], angleABf(f31, f30, f29, f4));
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

extern WinMgrDesc coinget_window_desc;

// NONMATCHING (82.55%): 5 instructions differ, 19 missing, 24 extra, plus register differences.
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

	pos1 = mario->position;
	pos2 = pos1;

	cam = camGetPtr(4);
	f1 = (100.0f * cam->viewYaw + 5.0f) / 100.0f;
	f6 = (6.2832f * reviseAngle(f1)) / 360.0f;

	f4 = 0.00761f;
	f3 = 0.16605f;
	f2 = 1.0f;
	f1 = 3.1416f;
	if (f6 <= f1) {
		f1 = 1.5708f;
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
		f0 = 4.7124f;
		if (f6 < f0) {
			f1 = f6 - f1;
			f0 = f1 * f1;
			f4 = f4 * f0 - f3;
			f4 = f4 * f0 + f2;
			f4 = f4 * f1;
			f4 = -f4;
		} else {
			f0 = f6 - f0;
			f1 = 1.5708f;
			f1 = f1 - f0;
			f0 = f1 * f1;
			f4 = f4 * f0 - f3;
			f4 = f4 * f0 + f2;
			f4 = f4 * f1;
			f4 = -f4;
		}
	}

	f1 = 3.1416f;
	f0 = 10.0f;
	f5 = 0.03705f;
	f4 = f0 * f4;
	f3 = 0.4967f;
	f2 = 1.0f;
	if (f6 <= f1) {
		f1 = 1.5708f;
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
		f0 = 4.7124f;
		if (f6 < f0) {
			f0 = f6 - f1;
			f0 = f0 * f0;
			f5 = f5 * f0 - f3;
			f5 = f5 * f0 + f2;
			f5 = -f5;
		} else {
			f0 = f6 - f0;
			f1 = 1.5708f;
			f0 = f1 - f0;
			f0 = f0 * f0;
			f5 = f5 * f0 - f3;
			f5 = f5 * f0 + f2;
		}
	}
	f0 = 10.0f;
	f5 = f0 * f5;

	pos1.x += f4;
	pos1.z -= f5;
	pos1.y += 62.0f;
	pos2.x += f4;
	pos2.y += 52.0f;
	pos2.z -= f5;

	if (isFirstCall) {
		event->userdata[0] = 0;
		event->userdata[1] = (s32)effItemGetEntry(7);
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
		if (OSTicksToMilliseconds((OSTick)gp->renderTime - (OSTick)event->savedTime) > event->userdata[2]) {
			iconFlagOff(str_802C35D8, 2);
			work[1] = 1;
			event->userdata[2] = 0x1F4;
			event->savedTime = gp->renderTime;
			event->userdata[0] = 2;
		}
		return 0;
	case 2:
		if (OSTicksToMilliseconds((OSTick)gp->renderTime - (OSTick)event->savedTime) > event->userdata[2]) {
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

// NONMATCHING (98.25%): same instructions; only register numbers or operands differ.
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
		f32 t1 = (b - d) * (a - e) + (a - c) * (f - b);
		f32 t2 = (b - d) * (a - g) + (a - c) * (h - b);
		if (t1 * t2 > 0.0f) {
			evtSetValue(event, args[8], 0);
			return 2;
		}
		{
			f32 t3 = (f - h) * (e - a) + (e - g) * (b - f);
			f32 t4 = (f - h) * (e - c) + (e - g) * (d - f);
			if (t3 * t4 > 0.0f) {
				evtSetValue(event, args[8], 0);
				return 2;
			}
		}
	}
	evtSetValue(event, args[8], 1);
	return 2;
}
