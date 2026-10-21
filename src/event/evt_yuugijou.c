#include "evt/evt_yuugijou.h"
#include "evt/evt_cmd.h"
#include "evt/evt_bero.h"
#include "evt/evt_mario.h"
#include "evt/evt_msg.h"
#include "driver/icondrv.h"
#include "driver/windowdrv.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "pmario_sound.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <string.h>

__declspec(section ".sdata2") GXColor waku_col = { 0xFF, 0xFF, 0xFF, 0x99 };
__declspec(section ".sdata2") GXColor icon_col_left = { 0xFF, 0xFF, 0xFF, 0xFF };
__declspec(section ".sdata2") GXColor icon_col_right = { 0xFF, 0xFF, 0xFF, 0xFF };
__declspec(section ".sdata2") GXColor number_col = { 0xFF, 0xFF, 0xFF, 0xFF };
// explicit_zero_data: a zero float would otherwise be placed in .bss and dropped from .sdata2.
#pragma explicit_zero_data on
#pragma explicit_zero_data reset

s32 minigame_end[] = {
	0x00020032, 0xFD050F80, 0x00000002, 0x00020032,
	0xF840604D, 0x00000001, 0x0005005B, (s32)evt_mario_set_prev_party_pos,
	0x00000000, 0x00000000, 0xFFFFFC18, 0x00000000,
	0x0005005B, (s32)evt_mario_set_prev_party_pos, 0x00000001, 0x00000000,
	0xFFFFFC18, 0x00000000, 0x0002005B, (s32)evt_mario_set_enable_key,
	0x00000000, 0x0003005B, (s32)evt_bero_mapchange, (s32)(char*)"gor_03",
	(s32)(char*)"A_dokan_02", 0x00000002, 0x00000001,
};

s32 window_open[] = {
	0x0002005B, (s32)evt_mario_cont_onoff, 0x00000000, 0x0000006E,
	0x0005005B, (s32)evt_mario_get_pos, 0x00000000, 0xFE363C81,
	0xFE363C82, 0xFE363C83, 0x00010005, 0x00000000,
	0x00010009, 0x00000001, 0x0004005B, (s32)evt_mario_set_pos,
	0xFE363C81, 0xFE363C82, 0xFE363C83, 0x00000006,
	0x00000070, 0x0002005B, (s32)evt_yuu_get_window_evt, 0xFE363C80,
	0x0001005E, 0xFE363C80, 0x0005005B, (s32)evt_msg_print,
	0x00000000, (s32)(char*)"sysyuu_all_01", 0x00000000, 0x00000000,
	0x0003005B, (s32)evt_msg_select, 0x00000000, (s32)(char*)"sysyuu_all_02",
	0x0001005B, (s32)evt_msg_continue, 0x00020018, 0xFE363C80,
	0x00000000, 0x0001005B, (s32)yuuminigame_end, 0x00000021,
	0x0001005B, (s32)yuuminigame_window_end, 0x0002005B, (s32)evt_mario_cont_onoff,
	0x00000001, 0x00000002, 0x00000001,
};

// .sbss: the trailing 4 bytes after yuwp in the original; a plain uninitialised static lands in .sbss.
static u32 gap_08_80418B5C_sbss;

PiantaParlorWork* yuwp;
s32 yuuWindowEvt; // script window_open runs as a child event (the yuu counter sets it per mode)
PiantaParlorWork yuugijouwork;

static inline s32 scale700(u32 v) {
	s32 r = (0x2BC - (s32)v) * 15 / 0x2BC;
	return (r >= 0 ? r : 0);
}

static inline s32 scale2000b(u32 v) {
	s32 q = ((s32)v - 0x12110) / 2000;
	return (q >= 0 ? q : 0);
}

// The rankMonte[1][1] element needs the assignment repeated in the true arm: with the plain ternary MWCC colours
// yuwp r6/scaled r7 instead of the target's r7/r6. The helper form would be natural but it hoists the
// rankScore[2] constants four instructions early (and four inlined scale700 calls make mwcceppc hang).
void yuugijou_init(void) {
	s32 scaled;

	yuwp = &yuugijouwork;
	memset(yuwp, 0, sizeof(PiantaParlorWork));

	yuwp->rankUsed[0][0] = 1;
	yuwp->rankUsed[0][1] = 1;
	yuwp->rankUsed[0][2] = 1;
	yuwp->rankUsed[0][3] = 1;
	yuwp->rankUsed[0][4] = 1;

	yuwp->rankScore[0][0] = 0x5B964;
	yuwp->rankScore[0][1] = 0x5825A;
	yuwp->rankScore[0][2] = 0x4F826;
	yuwp->rankScore[0][3] = 0x4C3D8;
	yuwp->rankScore[0][4] = 0x4237E;

	yuwp->rankMonte[0][0] = 0xE;
	yuwp->rankMonte[0][1] = 0xC;
	yuwp->rankMonte[0][2] = 0xF;
	yuwp->rankMonte[0][3] = 0xB;
	yuwp->rankMonte[0][4] = 0xA;

	yuwp->rankUsed[1][0] = 1;
	yuwp->rankUsed[1][1] = 1;
	yuwp->rankUsed[1][2] = 1;
	yuwp->rankUsed[1][3] = 1;
	yuwp->rankUsed[1][4] = 1;

	yuwp->rankScore[1][0] = 2;
	yuwp->rankScore[1][1] = 5;
	yuwp->rankScore[1][2] = 0x12;
	yuwp->rankScore[1][3] = 0x1B;
	yuwp->rankScore[1][4] = 0x8A;

	yuwp->rankMonte[1][0] = scale700(yuwp->rankScore[1][0]) + 1;
#ifdef MUST_MATCH
	// MUST_MATCH: the repeated assignment only shapes the register allocator (it CSEs to one quotient).
	yuwp->rankMonte[1][1] = (scaled = (0x2BC - (s32)yuwp->rankScore[1][1]) * 15 / 0x2BC) >= 0 ? (scaled = (0x2BC - (s32)yuwp->rankScore[1][1]) * 15 / 0x2BC) : 0;
#else
	yuwp->rankMonte[1][1] = ((scaled = (0x2BC - (s32)yuwp->rankScore[1][1]) * 15 / 0x2BC) >= 0 ? scaled : 0);
#endif
	yuwp->rankMonte[1][2] = ((scaled = (0x2BC - (s32)yuwp->rankScore[1][2]) * 15 / 0x2BC) >= 0 ? scaled : 0);
	yuwp->rankMonte[1][3] = ((scaled = (0x2BC - (s32)yuwp->rankScore[1][3]) * 15 / 0x2BC) >= 0 ? scaled : 0);
	yuwp->rankMonte[1][4] = ((scaled = (0x2BC - (s32)yuwp->rankScore[1][4]) * 15 / 0x2BC) >= 0 ? scaled : 0);

	yuwp->rankUsed[2][0] = 1;
	yuwp->rankUsed[2][1] = 1;
	yuwp->rankUsed[2][2] = 1;
	yuwp->rankUsed[2][3] = 1;
	yuwp->rankUsed[2][4] = 1;

	yuwp->rankScore[2][0] = 0x20850;
	yuwp->rankScore[2][1] = 0x20436;
	yuwp->rankScore[2][2] = 0x20184;
	yuwp->rankScore[2][3] = 0x1F91E;
	yuwp->rankScore[2][4] = 0x1E3AC;

	yuwp->rankMonte[2][0] = ((scaled = ((s32)yuwp->rankScore[2][0] - 0x1A9C8) / 2000) >= 0 ? scaled : 0) + 7;
	yuwp->rankMonte[2][1] = ((scaled = ((s32)yuwp->rankScore[2][1] - 0x1A9C8) / 2000) >= 0 ? scaled : 0) + 6;
	yuwp->rankMonte[2][2] = ((scaled = ((s32)yuwp->rankScore[2][2] - 0x1A9C8) / 2000) >= 0 ? scaled : 0) + 4;
	yuwp->rankMonte[2][3] = ((scaled = ((s32)yuwp->rankScore[2][3] - 0x1A9C8) / 2000) >= 0 ? scaled : 0) + 4;
	yuwp->rankMonte[2][4] = ((scaled = ((s32)yuwp->rankScore[2][4] - 0x1A9C8) / 2000) >= 0 ? scaled : 0) + 1;

	yuwp->rankUsed[3][0] = 1;
	yuwp->rankUsed[3][1] = 1;
	yuwp->rankUsed[3][2] = 1;
	yuwp->rankUsed[3][3] = 1;
	yuwp->rankUsed[3][4] = 1;

	yuwp->rankScore[3][0] = 0x16BDE;
	yuwp->rankScore[3][1] = 0x16968;
	yuwp->rankScore[3][2] = 0x16382;
	yuwp->rankScore[3][3] = 0x1627E;
	yuwp->rankScore[3][4] = 0x15950;

	yuwp->rankMonte[3][0] = scale2000b(yuwp->rankScore[3][0]) + 7;
	yuwp->rankMonte[3][1] = scale2000b(yuwp->rankScore[3][1]) + 6;
	yuwp->rankMonte[3][2] = scale2000b(yuwp->rankScore[3][2]) + 4;
	yuwp->rankMonte[3][3] = scale2000b(yuwp->rankScore[3][3]) + 3;
	yuwp->rankMonte[3][4] = scale2000b(yuwp->rankScore[3][4]) + 4;
}

// The original splits the visibility test as `flag1 == 0 || flag2 == 0`; two separate early
// returns would fold into a single `beq` instead of the target's `bne body; b end`.
void monteCountDisp(void) {
	static s32 monte_snd_flag;
	s32 baseY = -180;
	Mtx mtx;
	s32 delta;

	if (yuwp->flags & 4) {
		baseY = -120;
	}
	if ((yuwp->flags & 1) == 0 || (yuwp->flags & 2) == 0) return;
	windowDispGX_Waku_col(-288.0f, (f32)(baseY + 44), 200.0f, 44.0f, 16.0f, 0, *(GXColor*)&waku_col);

	PSMTXTrans(mtx, -260.0f, (f32)(baseY + 2), 0.0f);
	iconDispGxCol(mtx, 0x10, 0x13F, *(GXColor*)&icon_col_left);

	PSMTXTrans(mtx, -230.0f, (f32)(baseY + 2), 0.0f);
	iconDispGxCol(mtx, 0x10, 0x1D6, *(GXColor*)&icon_col_right);

	PSMTXTrans(mtx, -128.0f, (f32)(baseY + 2), 0.0f);
	iconNumberDispGx(mtx, yuwp->dispTokens, 0, *(GXColor*)&number_col);

	yuwp->dispTimer++;
	if ((yuwp->dispTimer & 3) == 0) {
		delta = yuwp->tokens - yuwp->dispTokens;
		yuwp->dispTimer = 0;
		if (monte_snd_flag == 0 && delta != 0) {
			psndSFXOn((const char*)0x8CA);
			monte_snd_flag = 8;
		}
		if (delta != 0) {
			delta /= 10;
			if (delta == 0) {
				delta = (yuwp->tokens > (s32)yuwp->dispTokens) ? 1 : -1;
			}
			yuwp->dispTokens += delta;
		}
	}
	if (monte_snd_flag != 0) {
		monte_snd_flag--;
	}
}

USER_FUNC(yuugijou_add_montemedal) {
	s32* args = event->args;
	s32 syncDisp = evtGetValue(event, args[0]);
	s32 amount = evtGetValue(event, args[1]);
	s32 sum = yuwp->tokens + amount;

	if (sum < 99999) {
		yuwp->tokens = sum;
	} else {
		yuwp->tokens = 99999;
	}
	if (syncDisp != 0) {
		yuwp->dispTokens = yuwp->tokens;
	}
	return 2;
}

USER_FUNC(yuugijou_get_montemedal) {
	evtSetValue(event, event->args[0], yuwp->tokens);
	return 2;
}

USER_FUNC(yuugijou_get_montemedal_keep) {
	s32* args = event->args;

	if (yuwp->flags & 2) {
		evtSetValue(event, args[0], 0);
	} else {
		evtSetValue(event, args[0], yuwp->keptTokens);
	}
	return 2;
}

USER_FUNC(yuugijou_add_montemedal_moto) {
	s32* args = event->args;

	evtGetValue(event, args[0]);
	if (yuwp->flags & 2) {
		yuwp->keptTokens += evtGetValue(event, args[1]);
	}
	return 2;
}

USER_FUNC(yuugijou_montemedal_disp_onoff) {
	if (evtGetValue(event, event->args[0]) != 0) {
		yuwp->flags |= 1;
		yuwp->dispTokens = yuwp->tokens;
	} else {
		yuwp->flags &= ~1;
	}
	return 2;
}

USER_FUNC(yuugijou_montemedal_wait) {
	if ((s32)yuwp->dispTokens != yuwp->tokens) {
		return 0;
	}
	return 2;
}

USER_FUNC(yuugijou_montemedal_keep) {
	if (yuwp->flags & 2) {
		yuwp->keptTokens = yuwp->tokens;
		yuwp->tokens = 0;
		yuwp->dispTokens = 0;
		yuwp->flags &= ~2;
		yuwp->flags &= ~1;
	}
	return 2;
}

USER_FUNC(yuugijou_montemedal_return) {
	if (!(yuwp->flags & 2)) {
		s32 kept = yuwp->keptTokens;
		yuwp->tokens = kept;
		yuwp->dispTokens = kept;
		yuwp->flags |= 2;
		yuwp->flags |= 1;
	}
	return 2;
}

USER_FUNC(yuugijou_get_takeoutflag) {
	s32* args = event->args;

	if (yuwp->flags & 2) {
		evtSetValue(event, args[0], 1);
	} else {
		evtSetValue(event, args[0], 0);
	}
	return 2;
}

USER_FUNC(yuugijou_set_yupflag) {
	if (evtGetValue(event, event->args[0]) != 0) {
		yuwp->flags |= 4;
	} else {
		yuwp->flags &= ~4;
	}
	return 2;
}

USER_FUNC(yuuminigame_window_end) {
	yuwp->flags |= 8;
	return 2;
}

USER_FUNC(yuuminigame_end) {
	evtEntryType(minigame_end, 0, 0, 0xEF);
	return 2;
}

USER_FUNC(evt_yuu_get_window_evt) {
	evtSetValue(event, event->args[0], yuuWindowEvt);
	return 2;
}

void yuuSetWindowEvt(s32 value) {
	yuuWindowEvt = value;
}

EventEntry* evtYuuWindow(void) {
	yuwp->flags &= ~8;
	return evtEntryType(window_open, 0, 0, 0);
}

BOOL evtYuuWindowEndChk(void) {
	return (yuwp->flags >> 3) & 1;
}