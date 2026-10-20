#include "battle/ac/ac_timing_a.h"
#include "battle/battle_pad.h"
#include "battle/battle_unit.h"
#include "driver/camdrv.h"
#include "driver/icondrv.h"
#include "driver/icondrv_gx.h"
#include "system.h"
#include <string.h>
#include "battle/ac/ac_table.h"
#include "battle/battle_ac.h"
#include "driver/mobjdrv.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"

static void actionCommandDisp(f32 x, f32 y);

// .rodata vectors and .sdata2 constants actionCommandDisp reads.
extern const Vec vec3_802efbc8;
extern const Vec vec3_802efbd4;
extern const f32 float_neg200_8041c428;
extern const f32 float_50_8041c42c;
extern const f32 float_1_8041c430;

// TimingA's action-command work area at BattleWork+0x1F4C. No header declares it.
typedef struct AcTimingADecWork {
	s32 pad_0x00;   // 0x0, never accessed
	u32 flags;      // 0x4
	s32 endTimer;   // 0x8, frames the result display waits (0x3C)
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
extern const f32 float_0_8041c43c;

// clang-format off
// opt_propagation off: with copy propagation on MWCC folds the dec pointer into `work+0x1F54`
// and never materialises it; the original keeps `dec` in r4 (`addi r4, r30, 0x1f4c`) for the
// four `0x8(r4)` accesses.
s32 battleAcMain_TimingA(BattleWork* work) {
	BattleActions* ac = &work->actionCommands;
	AcTimingADecWork* dec = (AcTimingADecWork*)&ac->field_0x2BC[0];
	BattleACDispParams* disp = &ac->dispParams;

	switch (ac->mAcState) {
	case 0:
		memset(disp, 0, 0x2C);
		disp->iconX = float_neg300_8041c434;
		disp->iconY = float_0_8041c43c;
		disp->counter = 0x14;
		ac->mAcState = 0x63;
#ifdef MUST_MATCH
		// Match-only: the original reads the work-area flags with `lwz r3, 0x1f50(r30)` (work base),
		// not through the dec pointer, which keeps dec out of the memset call's live range so it can
		// live in volatile r4. `dec->flags` (below) makes dec live across the call and forces r31.
		*(u32*)&ac->field_0x2BC[4] &= ~1;
#else
		dec->flags &= ~1;
#endif
		ac->outputParams[0] = 0;
		break;
	case 0x64:
		break;
	case 0x3E8:
		dec->endTimer = 0x3C;
		ac->mAcState = 0x3E9;
		// fallthrough
	case 0x3E9:
		dec->endTimer--;
		if (dec->endTimer <= 0) {
			ac->mAcState = 0x3EA;
		}
		break;
	case 0x3EA:
		ac->maincb = NULL;
		ac->dispcb = NULL;
		ac->resultcb = NULL;
		ac->deletecb = NULL;
		return 0;
	default:
		break;
	}
	return 1;
}


// The header says `s8`, but the callee returns a zero-extended 4-byte value; a plain call makes
// MWCC emit `extsb r3, r3` before the `slwi` table index, so this is called through a
// function pointer under MUST_MATCH (as ac_lst_rc_keep/ac_stick_keep_left do).
// dont_inline: battleAcResult_TimingA calls this with `bl`; without it MWCC inlines the body.
#pragma dont_inline on
s32 battleAcTimingA_GetSuccessFrame(BattleWork* work) {
	AcTimingADecWork* dec = (AcTimingADecWork*)&work->actionCommands.field_0x2BC[0];
	AcTimingAEntry* entry = _ac_timing_a_timing_tbl[
#ifdef MUST_MATCH
	    // Match-only: the header says `s8`, but the callee returns a zero-extended 4-byte value;
	    // a plain call makes MWCC emit `extsb r3, r3` before the `slwi` table index.
	    ((s32 (*)(BattleWork*))BattleActionCommandGetDifficulty)(work)
#else
	    BattleActionCommandGetDifficulty(work)
#endif
	];
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

#pragma dont_inline reset

s32 battleAcResult_TimingA(BattleWork* work) {
	BattleActions* ac = &work->actionCommands;
	s32 successFrame = battleAcTimingA_GetSuccessFrame(work);
	s32 count = 0;
	s32 i;

	if ((ac->field_0x4 & 1) && (ac->mAcUnit->mTokenFlags & kConfusedAction)) {
		if (irand(100) >= 0) {
			ac->result = 0;
		}
		else {
			ac->result = 2;
			ac->mResultCount++;
		}
		return ac->result;
	}

	if (ac->mAcUnit->badgesEquipped.AutoCommand != 0) {
		ac->result = 2;
		ac->mResultCount++;
		return ac->result;
	}

	ac->result = 0;

	if (work->flags & 0x01000000) {
		ac->outputParams[0] = 3;
		ac->result = 2;
		ac->mResultCount++;
		return ac->result;
	}

	for (i = 0; i < 0x14; i++) {
		if (BattlePadCheckRecordTrigger(i, 0x100)) {
			count++;
		}
	}

	if (count >= 2) {
		ac->outputParams[0] = 1;
		return ac->result;
	}

	for (i = 0; i < successFrame; i++) {
		if (BattlePadCheckRecordTrigger(i, 0x100)) {
			break;
		}
	}

	if (i >= successFrame) {
		if (count > 0) {
			ac->outputParams[0] = 1;
		}
		else {
			ac->outputParams[0] = 2;
		}
		return ac->result;
	}

	ac->outputParams[0] = 3;
	ac->result = 2;
	ac->mResultCount++;
	return ac->result;
}


// Deriving both the state and disp parameters from one BattleActions* (rather than straight from
// the BattleWork*), state read first, is what makes MWCC materialise the disp base in r31
// (`addi r31, r4, 0x1f20`) and load the state from r4, as the original does.
void battleAcDisp_TimingA(CameraId camId, void* param) {
	BattleActions* ac = &((BattleWork*)param)->actionCommands;
	s32 state = ac->mAcState;
	BattleACDispParams* disp = &ac->dispParams;

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
		iconDispGx(float_1_8041c430, &v, 0x10, 0x65);
	}
	else {
		Vec v = vec3_802efbd4;
		v.x = float_neg200_8041c428 + x;
		v.y = float_50_8041c42c + y;
		iconDispGx(float_1_8041c430, &v, 0x10, 0x64);
	}

	(*(s32*)&disp->field_0x0[0])++;
}

// TODO: type
s32 _ac_timing_a_timing_lv_0[14] = {
    0x00000004, 0x0000000A, 0x00000005, 0x00000009,
    0x00000006, 0x00000008, 0x00000007, 0x00000007,
    0x00000008, 0x00000006, 0x00000009, 0x00000005,
    0x0001869F, 0x00000001,
};

// TODO: type
s32 _ac_timing_a_timing_lv_1[22] = {
    0x00000000, 0x0000000A, 0x00000001, 0x00000009,
    0x00000002, 0x00000008, 0x00000003, 0x00000007,
    0x00000004, 0x00000006, 0x00000005, 0x00000005,
    0x00000006, 0x00000004, 0x00000007, 0x00000004,
    0x00000008, 0x00000003, 0x00000009, 0x00000002,
    0x0001869F, 0x00000001,
};

// TODO: type
s32 _ac_timing_a_timing_lv_2[22] = {
    0x00000000, 0x00000009, 0x00000001, 0x00000008,
    0x00000002, 0x00000007, 0x00000003, 0x00000006,
    0x00000004, 0x00000005, 0x00000005, 0x00000004,
    0x00000006, 0x00000003, 0x00000007, 0x00000002,
    0x00000008, 0x00000001, 0x00000009, 0x00000001,
    0x0001869F, 0x00000001,
};

// TODO: type
s32 _ac_timing_a_timing_lv_3[20] = {
    0x00000000, 0x00000008, 0x00000001, 0x00000007,
    0x00000002, 0x00000006, 0x00000003, 0x00000005,
    0x00000004, 0x00000004, 0x00000005, 0x00000003,
    0x00000006, 0x00000002, 0x00000008, 0x00000001,
    0x00000009, 0x00000001, 0x0001869F, 0x00000001,
};

// TODO: type
s32 _ac_timing_a_timing_lv_4[18] = {
    0x00000000, 0x00000007, 0x00000001, 0x00000006,
    0x00000002, 0x00000005, 0x00000003, 0x00000004,
    0x00000004, 0x00000003, 0x00000005, 0x00000002,
    0x00000008, 0x00000001, 0x00000009, 0x00000001,
    0x0001869F, 0x00000001,
};

// TODO: type
s32 _ac_timing_a_timing_lv_5[16] = {
    0x00000000, 0x00000006, 0x00000001, 0x00000005,
    0x00000002, 0x00000004, 0x00000003, 0x00000003,
    0x00000004, 0x00000002, 0x00000008, 0x00000001,
    0x00000009, 0x00000001, 0x0001869F, 0x00000001,
};

// TODO: type
s32 _ac_timing_a_timing_lv_6[14] = {
    0x00000000, 0x00000005, 0x00000001, 0x00000004,
    0x00000002, 0x00000003, 0x00000003, 0x00000002,
    0x00000008, 0x00000001, 0x00000009, 0x00000001,
    0x0001869F, 0x00000001,
};

// TODO: type
AcTimingAEntry* _ac_timing_a_timing_tbl[7] = {
    (AcTimingAEntry*)_ac_timing_a_timing_lv_0, (AcTimingAEntry*)_ac_timing_a_timing_lv_1, (AcTimingAEntry*)_ac_timing_a_timing_lv_2, (AcTimingAEntry*)_ac_timing_a_timing_lv_3,
    (AcTimingAEntry*)_ac_timing_a_timing_lv_4, (AcTimingAEntry*)_ac_timing_a_timing_lv_5, (AcTimingAEntry*)_ac_timing_a_timing_lv_6,
};
// clang-format on

#pragma dont_inline reset

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const Vec vec3_802efbc8 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802efbd4 = { 0.0f, 0.0f, 0.0f };
const f32 float_neg200_8041c428 = -200.0f;
const f32 float_50_8041c42c = 50.0f;
const f32 float_1_8041c430 = 1.0f;
const f32 float_neg300_8041c434 = -300.0f;
const f32 float_30_8041c438 = 30.0f;
const f32 float_0_8041c43c = 0.0f;
