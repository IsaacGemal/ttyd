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

__declspec(section ".sdata2") GXColor dat_804214b0 = { 0xFF, 0xFF, 0xFF, 0x99 };
__declspec(section ".sdata2") GXColor dat_804214b4 = { 0xFF, 0xFF, 0xFF, 0xFF };
__declspec(section ".sdata2") GXColor dat_804214b8 = { 0xFF, 0xFF, 0xFF, 0xFF };
__declspec(section ".sdata2") GXColor dat_804214bc = { 0xFF, 0xFF, 0xFF, 0xFF };
__declspec(section ".sdata2") f32 float_neg288_804214c0 = -288.0f;
__declspec(section ".sdata2") f32 float_200_804214c4 = 200.0f;
__declspec(section ".sdata2") f32 float_44_804214c8 = 44.0f;
__declspec(section ".sdata2") f32 float_16_804214cc = 16.0f;
__declspec(section ".sdata2") f32 float_neg260_804214d0 = -260.0f;
// explicit_zero_data: a zero float would otherwise be placed in .bss and dropped from .sdata2.
#pragma explicit_zero_data on
__declspec(section ".sdata2") f32 float_0_804214d4 = 0.0f;
#pragma explicit_zero_data reset
__declspec(section ".sdata2") f32 float_neg230_804214d8 = -230.0f;
__declspec(section ".sdata2") f32 float_neg128_804214dc = -128.0f;

const char str_802FE8C8[8] = "gor_03";
const char str_802FE8D0[12] = "A_dokan_02";
const char str_802FE8DC[16] = "sysyuu_all_01";
const char str_802FE8EC[16] = "sysyuu_all_02";

s32 minigame_end[] = {
	0x00020032, 0xFD050F80, 0x00000002, 0x00020032,
	0xF840604D, 0x00000001, 0x0005005B, (s32)evt_mario_set_prev_party_pos,
	0x00000000, 0x00000000, 0xFFFFFC18, 0x00000000,
	0x0005005B, (s32)evt_mario_set_prev_party_pos, 0x00000001, 0x00000000,
	0xFFFFFC18, 0x00000000, 0x0002005B, (s32)evt_mario_set_enable_key,
	0x00000000, 0x0003005B, (s32)evt_bero_mapchange, (s32)(char*)str_802FE8C8,
	(s32)(char*)str_802FE8D0, 0x00000002, 0x00000001,
};

s32 window_open[] = {
	0x0002005B, (s32)evt_mario_cont_onoff, 0x00000000, 0x0000006E,
	0x0005005B, (s32)evt_mario_get_pos, 0x00000000, 0xFE363C81,
	0xFE363C82, 0xFE363C83, 0x00010005, 0x00000000,
	0x00010009, 0x00000001, 0x0004005B, (s32)evt_mario_set_pos,
	0xFE363C81, 0xFE363C82, 0xFE363C83, 0x00000006,
	0x00000070, 0x0002005B, (s32)unk_802243a4, 0xFE363C80,
	0x0001005E, 0xFE363C80, 0x0005005B, (s32)evt_msg_print,
	0x00000000, (s32)(char*)str_802FE8DC, 0x00000000, 0x00000000,
	0x0003005B, (s32)evt_msg_select, 0x00000000, (s32)(char*)str_802FE8EC,
	0x0001005B, (s32)evt_msg_continue, 0x00020018, 0xFE363C80,
	0x00000000, 0x0001005B, (s32)yuuminigame_end, 0x00000021,
	0x0001005B, (s32)yuuminigame_window_end, 0x0002005B, (s32)evt_mario_cont_onoff,
	0x00000001, 0x00000002, 0x00000001,
};

// .sbss: the trailing 4 bytes after yuwp in the original; a plain uninitialised static lands in .sbss.
static u32 gap_08_80418B5C_sbss;

PiantaParlorWork* yuwp;
s32 unk_80418b54;
PiantaParlorWork yuugijouwork;

static inline s32 scale700(u32 v) {
	s32 r = (0x2BC - (s32)v) * 15 / 0x2BC;
	return (r >= 0 ? r : 0);
}

static inline s32 scale2000b(u32 v) {
	s32 q = ((s32)v - 0x12110) / 2000;
	return (q >= 0 ? q : 0);
}

// The 0x90 element needs the assignment repeated in the true arm: with the plain ternary MWCC colours
// yuwp r6/q r7 instead of the target's r7/r6. The helper form would be natural but it hoists the
// 0xA4..0xB4 constants four instructions early (and four inlined scale700 calls make mwcceppc hang).
void yuugijou_init(void) {
	s32 q;

	yuwp = &yuugijouwork;
	memset(yuwp, 0, sizeof(PiantaParlorWork));

	yuwp->field_0x14 = 1;
	yuwp->field_0x18 = 1;
	yuwp->field_0x1C = 1;
	yuwp->field_0x20 = 1;
	yuwp->field_0x24 = 1;

	yuwp->field_0x74 = 0x5B964;
	yuwp->field_0x78 = 0x5825A;
	yuwp->field_0x7C = 0x4F826;
	yuwp->field_0x80 = 0x4C3D8;
	yuwp->field_0x84 = 0x4237E;

	yuwp->field_0xD4 = 0xE;
	yuwp->field_0xD8 = 0xC;
	yuwp->field_0xDC = 0xF;
	yuwp->field_0xE0 = 0xB;
	yuwp->field_0xE4 = 0xA;

	yuwp->field_0x2C = 1;
	yuwp->field_0x30 = 1;
	yuwp->field_0x34 = 1;
	yuwp->field_0x38 = 1;
	yuwp->field_0x3C = 1;

	yuwp->field_0x8C = 2;
	yuwp->field_0x90 = 5;
	yuwp->field_0x94 = 0x12;
	yuwp->field_0x98 = 0x1B;
	yuwp->field_0x9C = 0x8A;

	yuwp->field_0xEC = scale700(yuwp->field_0x8C) + 1;
#ifdef MUST_MATCH
	// MUST_MATCH: the repeated assignment only shapes the register allocator (it CSEs to one quotient).
	yuwp->field_0xF0 = (q = (0x2BC - (s32)yuwp->field_0x90) * 15 / 0x2BC) >= 0 ? (q = (0x2BC - (s32)yuwp->field_0x90) * 15 / 0x2BC) : 0;
#else
	yuwp->field_0xF0 = ((q = (0x2BC - (s32)yuwp->field_0x90) * 15 / 0x2BC) >= 0 ? q : 0);
#endif
	yuwp->field_0xF4 = ((q = (0x2BC - (s32)yuwp->field_0x94) * 15 / 0x2BC) >= 0 ? q : 0);
	yuwp->field_0xF8 = ((q = (0x2BC - (s32)yuwp->field_0x98) * 15 / 0x2BC) >= 0 ? q : 0);
	yuwp->field_0xFC = ((q = (0x2BC - (s32)yuwp->field_0x9C) * 15 / 0x2BC) >= 0 ? q : 0);

	yuwp->field_0x44 = 1;
	yuwp->field_0x48 = 1;
	yuwp->field_0x4C = 1;
	yuwp->field_0x50 = 1;
	yuwp->field_0x54 = 1;

	yuwp->field_0xA4 = 0x20850;
	yuwp->field_0xA8 = 0x20436;
	yuwp->field_0xAC = 0x20184;
	yuwp->field_0xB0 = 0x1F91E;
	yuwp->field_0xB4 = 0x1E3AC;

	yuwp->field_0x104 = ((q = ((s32)yuwp->field_0xA4 - 0x1A9C8) / 2000) >= 0 ? q : 0) + 7;
	yuwp->field_0x108 = ((q = ((s32)yuwp->field_0xA8 - 0x1A9C8) / 2000) >= 0 ? q : 0) + 6;
	yuwp->field_0x10C = ((q = ((s32)yuwp->field_0xAC - 0x1A9C8) / 2000) >= 0 ? q : 0) + 4;
	yuwp->field_0x110 = ((q = ((s32)yuwp->field_0xB0 - 0x1A9C8) / 2000) >= 0 ? q : 0) + 4;
	yuwp->field_0x114 = ((q = ((s32)yuwp->field_0xB4 - 0x1A9C8) / 2000) >= 0 ? q : 0) + 1;

	yuwp->field_0x5C = 1;
	yuwp->field_0x60 = 1;
	yuwp->field_0x64 = 1;
	yuwp->field_0x68 = 1;
	yuwp->field_0x6C = 1;

	yuwp->field_0xBC = 0x16BDE;
	yuwp->field_0xC0 = 0x16968;
	yuwp->field_0xC4 = 0x16382;
	yuwp->field_0xC8 = 0x1627E;
	yuwp->field_0xCC = 0x15950;

	yuwp->field_0x11C = scale2000b(yuwp->field_0xBC) + 7;
	yuwp->field_0x120 = scale2000b(yuwp->field_0xC0) + 6;
	yuwp->field_0x124 = scale2000b(yuwp->field_0xC4) + 4;
	yuwp->field_0x128 = scale2000b(yuwp->field_0xC8) + 3;
	yuwp->field_0x12C = scale2000b(yuwp->field_0xCC) + 4;
}

// The original splits the visibility test as `flag1 == 0 || flag2 == 0`; two separate early
// returns would fold into a single `beq` instead of the target's `bne body; b end`.
void monteCountDisp(void) {
	static s32 monte_snd_flag;
	s32 x = -180;
	Mtx mtx;
	s32 delta;

	if (yuwp->field_0x0 & 4) {
		x = -120;
	}
	if ((yuwp->field_0x0 & 1) == 0 || (yuwp->field_0x0 & 2) == 0) return;
	windowDispGX_Waku_col(0, *(GXColor*)&dat_804214b0, float_neg288_804214c0, (f32)(x + 44), float_200_804214c4, float_44_804214c8, float_16_804214cc);

	PSMTXTrans(mtx, float_neg260_804214d0, (f32)(x + 2), float_0_804214d4);
	iconDispGxCol(mtx, 0x10, 0x13F, *(GXColor*)&dat_804214b4);

	PSMTXTrans(mtx, float_neg230_804214d8, (f32)(x + 2), float_0_804214d4);
	iconDispGxCol(mtx, 0x10, 0x1D6, *(GXColor*)&dat_804214b8);

	PSMTXTrans(mtx, float_neg128_804214dc, (f32)(x + 2), float_0_804214d4);
	iconNumberDispGx(mtx, yuwp->field_0x10, 0, *(GXColor*)&dat_804214bc);

	yuwp->field_0xC++;
	if ((yuwp->field_0xC & 3) == 0) {
		delta = yuwp->tokens - yuwp->field_0x10;
		yuwp->field_0xC = 0;
		if (monte_snd_flag == 0 && delta != 0) {
			psndSFXOn((const char*)0x8CA);
			monte_snd_flag = 8;
		}
		if (delta != 0) {
			delta /= 10;
			if (delta == 0) {
				delta = ((s32)yuwp->tokens > (s32)yuwp->field_0x10) ? 1 : -1;
			}
			yuwp->field_0x10 += delta;
		}
	}
	if (monte_snd_flag != 0) {
		monte_snd_flag--;
	}
}

USER_FUNC(yuugijou_add_montemedal) {
	s32* args = event->args;
	s32 flag = evtGetValue(event, args[0]);
	s32 amount = evtGetValue(event, args[1]);
	s32 sum = yuwp->tokens + amount;

	if (sum < 99999) {
		yuwp->tokens = sum;
	} else {
		yuwp->tokens = 99999;
	}
	if (flag != 0) {
		yuwp->field_0x10 = yuwp->tokens;
	}
	return 2;
}

USER_FUNC(yuugijou_get_montemedal) {
	evtSetValue(event, event->args[0], yuwp->tokens);
	return 2;
}

USER_FUNC(yuugijou_get_montemedal_keep) {
	s32* args = event->args;

	if (yuwp->field_0x0 & 2) {
		evtSetValue(event, args[0], 0);
	} else {
		evtSetValue(event, args[0], yuwp->field_0x4);
	}
	return 2;
}

USER_FUNC(yuugijou_add_montemedal_moto) {
	s32* args = event->args;

	evtGetValue(event, args[0]);
	if (yuwp->field_0x0 & 2) {
		yuwp->field_0x4 += evtGetValue(event, args[1]);
	}
	return 2;
}

USER_FUNC(yuugijou_montemedal_disp_onoff) {
	if (evtGetValue(event, event->args[0]) != 0) {
		yuwp->field_0x0 |= 1;
		yuwp->field_0x10 = yuwp->tokens;
	} else {
		yuwp->field_0x0 &= ~1;
	}
	return 2;
}

USER_FUNC(yuugijou_montemedal_wait) {
	if ((s32)yuwp->field_0x10 != (s32)yuwp->tokens) {
		return 0;
	}
	return 2;
}

USER_FUNC(yuugijou_montemedal_keep) {
	if (yuwp->field_0x0 & 2) {
		yuwp->field_0x4 = yuwp->tokens;
		yuwp->tokens = 0;
		yuwp->field_0x10 = 0;
		yuwp->field_0x0 &= ~2;
		yuwp->field_0x0 &= ~1;
	}
	return 2;
}

USER_FUNC(yuugijou_montemedal_return) {
	if (!(yuwp->field_0x0 & 2)) {
		s32 tmp = yuwp->field_0x4;
		yuwp->tokens = tmp;
		yuwp->field_0x10 = tmp;
		yuwp->field_0x0 |= 2;
		yuwp->field_0x0 |= 1;
	}
	return 2;
}

USER_FUNC(yuugijou_get_takeoutflag) {
	s32* args = event->args;

	if (yuwp->field_0x0 & 2) {
		evtSetValue(event, args[0], 1);
	} else {
		evtSetValue(event, args[0], 0);
	}
	return 2;
}

USER_FUNC(yuugijou_set_yupflag) {
	if (evtGetValue(event, event->args[0]) != 0) {
		yuwp->field_0x0 |= 4;
	} else {
		yuwp->field_0x0 &= ~4;
	}
	return 2;
}

USER_FUNC(yuuminigame_window_end) {
	yuwp->field_0x0 |= 8;
	return 2;
}

USER_FUNC(yuuminigame_end) {
	evtEntryType(minigame_end, 0, 0, 0xEF);
	return 2;
}

USER_FUNC(unk_802243a4) {
	evtSetValue(event, event->args[0], unk_80418b54);
	return 2;
}

void unk_8022439c(s32 value) {
	unk_80418b54 = value;
}

EventEntry* evtYuuWindow(void) {
	yuwp->field_0x0 &= ~8;
	return evtEntryType(window_open, 0, 0, 0);
}

BOOL evtYuuWindowEndChk(void) {
	return (yuwp->field_0x0 >> 3) & 1;
}