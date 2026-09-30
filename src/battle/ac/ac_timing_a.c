#include "battle/ac/ac_timing_a.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "system.h"

static void actionCommandDisp(f32 x, f32 y);

// .rodata vectors and .sdata2 constants actionCommandDisp reads.
extern const Vec vec3_802efbc8;
extern const Vec vec3_802efbd4;
extern const f32 float_neg200_8041c428;
extern const f32 float_50_8041c42c;
extern const f32 float_1_8041c430;

// TimingA's action-command work area at BattleWork+0x1F4C. No header declares it.
typedef struct AcTimingADecWork {
	s32 field_0x00; // 0x0
	u32 flags;      // 0x4
	s32 field_0x08; // 0x8
} AcTimingADecWork;

// A timing window: the frame threshold and the success frame it maps to.
typedef struct AcTimingAEntry {
	s32 threshold; // 0x0
	s32 value;     // 0x4
} AcTimingAEntry;

// One window table per difficulty level (indexed by BattleActionCommandGetDifficulty).
extern AcTimingAEntry* _ac_timing_a_timing_tbl[7];

// .sdata2 interpolation endpoints.
extern const f32 float_neg300_8041c434;
extern const f32 float_30_8041c438;

// NONMATCHING (1.54%): 64 missing.
s32 battleAcMain_TimingA(BattleWork* work) {
	switch (work->actionCommands.mAcState) { //decimal values
		case 0:
			break;
	}
}

// NONMATCHING (88.39%): the header's s8 return makes MWCC emit an extra `extsb r3, r3` before the table index (the original is plain `slwi`; the definition is u8 `lbz; blr`, so this caller used full width), which also reorders the next two loads.
s32 battleAcTimingA_GetSuccessFrame(BattleWork* work) {
	AcTimingADecWork* dec = (AcTimingADecWork*)((u8*)work + 0x1F4C);
	s32 difficulty = BattleActionCommandGetDifficulty(work);
	AcTimingAEntry* entry = _ac_timing_a_timing_tbl[difficulty];
	s32 frame = work->actionCommands.mResultCount;
	s32 result = entry->value;

	if (dec->flags & 2) {
		if (frame >= 100000) {
			result = 0;
		} else {
#ifdef MUST_MATCH
			// The original's -O4,s loop is not rotated; a plain `while` rotates it here.
			for (;;) {
				if (frame <= entry->threshold) {
					break;
				}
				entry++;
			}
#else
			while (frame > entry->threshold) {
				entry++;
			}
#endif
			result = entry->value;
		}
	}
	return result;
}

// The disp base pointer is the only difference. The original materialises `disp = param + 0x1F20` in r31
// (`addi r31, r4, 0x1f20`) and reads the state from r4; MWCC folds the disp pointer into the BattleWork
// base (`mr r31, r4` plus 0x1f3x offsets) instead. Every pragma tried (optimize_for_size, opt_common_subs/
// opt_propagation/opt_lifetimes/global_optimizer off) and source form (u8*/member pointer, member order,
// register/volatile) gives the same merge or worse.
// NONMATCHING (98.81%): see above.
void battleAcDisp_TimingA(CameraId camId, void* param) {
	BattleACDispParams* disp = &((BattleWork*)param)->actionCommands.dispParams;
	s32 state = ((BattleWork*)param)->actionCommands.mAcState;

	switch (state) {
	case 0x63:
		disp->iconX = intplGetValue(4, 0x14 - disp->counter, 0x14, float_neg300_8041c434, float_30_8041c438);
		// fallthrough
	case 0x64:
		disp->iconX = float_30_8041c438;
		actionCommandDisp(disp->iconX, disp->iconY);
		if (disp->counter > 0) {
			disp->counter--;
		}
		break;
	case 0x3E8:
	case 0x3E9:
		if (disp->counter >= 0x28) {
			disp->iconX = intplGetValue(4, disp->counter - 0x28, 0x14, float_30_8041c438, float_neg300_8041c434);
		}
		else {
			disp->iconX = float_30_8041c438;
		}
		actionCommandDisp(disp->iconX, disp->iconY);
		if (disp->counter < 0x3C) {
			disp->counter++;
		}
		break;
	}
}

void battleAcDelete_TimingA(BattleWork* work) {
	work->actionCommands.mAcState = 1000;
}

// dont_inline: the original keeps actionCommandDisp as its own local function (callers emit `bl`).
#pragma dont_inline on
static void actionCommandDisp(f32 x, f32 y) {
	BattleWork* work = _battleWorkPointer;
	BattleActions* ac = &work->actionCommands;
	BattleACDispParams* disp = &ac->dispParams;
	AcTimingADecWork* dec = (AcTimingADecWork*)&ac->field_0x2BC[0];

	camGetPtr(CAMERA_2D);

	if (dec->flags & 1) {
		Vec v = vec3_802efbc8;
		v.x = float_neg200_8041c428 + x;
		v.y = float_50_8041c42c + y;
		iconDispGx(&v, 0x10, 0x65, float_1_8041c430);
	}
	else {
		Vec v = vec3_802efbd4;
		v.x = float_neg200_8041c428 + x;
		v.y = float_50_8041c42c + y;
		iconDispGx(&v, 0x10, 0x64, float_1_8041c430);
	}

	(*(s32*)&disp->field_0x0[0])++;
}
#pragma dont_inline reset

