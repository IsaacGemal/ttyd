#include "eff/eff_pointget.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdio.h>
#include <string.h>

// .sdata2 explicit color globals
const GXColor dat_80420ec8 = { 0x00, 0x00, 0x00, 0x80 };
const GXColor dat_80420ecc = { 0xFF, 0xFF, 0xFF, 0xFF };
const GXColor dat_80420ed0 = { 0x00, 0x00, 0x00, 0x80 };
const GXColor dat_80420ed4 = { 0xFF, 0xFF, 0xFF, 0xFF };

// .rodata
#ifdef MUST_MATCH
// MUST_MATCH: the original addresses this object with lis/addi (it is not in .sdata)
// and, as a non-const object in .rodata, MWCC does not hoist its three loads above the
// effect->userdata load in effPointGetMain (the natural `const` form does, and scores 96%).
__declspec(section ".rodata") Vec vec3_802fe390 = { 0.0f, 0.0f, 0.0f };
#else
const Vec vec3_802fe390 = { 0.0f, 0.0f, 0.0f };
#endif
const f64 double_to_int_mask_802fe3a0 = 4503599627370496.0;
const f64 double_to_int_802fe3a8 = 4503601774854144.0;
const char str_802FE3B0[] = "PointGet";

// .sdata texture ids indexed [language != 0][texture]
static u32 texid_tbl[2][4] = {
	{ 0x98, 0x99, 0x9A, 0x9B },
	{ 0x98, 0x99, 0x9A, 0x9B },
};

EffectEntry* effPointGetEntry(s32 type, s32 value, s32 wait, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	PointGetWork* work;

	entry = effEntry();
	entry->type = str_802FE3B0;
	entry->count = 1;
	work = __memAlloc(HEAP_EFFECT, entry->count * sizeof(PointGetWork));
	entry->userdata = work;
	entry->callback = effPointGetMain;
	entry->flags |= 2;
	work->type = type;
	work->x = x - 2000.0f;
	work->y = y;
	work->z = z;
	work->value = value;
	work->scale = 1.0f;
	work->wait = wait;
	work->frame = 0;
	work->state = 0;
	return entry;
}
void effPointGetMain(EffectEntry* effect) {
	PointGetWork* work;
	Vec position;
	Vec disp;

	work = effect->userdata;
	disp = vec3_802fe390;
	disp.x = work->x;
	disp.y = work->y;
	disp.z = work->z;
	position = disp;

	work->frame++;
	{
	switch (work->state) {
	case 0:
		work->x = intplGetValue(0xB, work->frame, 0x2D, -1000.0f, 0.0f);
		if (work->frame >= 0x2D) {
			work->frame = 0;
			work->state++;
		}
		break;
	case 1:
		if (work->wait == 0) {
			if (effect->flags & 4) {
				effect->flags &= ~4;
				work->frame = 0;
				work->state++;
			}
		} else if (work->frame >= work->wait) {
			work->frame = 0;
			work->state++;
		}
		break;
	case 2:
		work->x = intplGetValue(0xB, work->frame, 0x3C, 0.0f, 1000.0f);
		if (work->frame >= 0x3C) {
			effDelete(effect);
			return;
		}
		break;
	default:
		break;
	}
	}

	dispEntry(8, dispCalcZ(&position), 2, effPointGetDisp, effect);
}

// The frame (0x1e0), the callee-saved set (f19-f31/r26-r31), the order of the constant
// loads and the mtx offsets (0xb8/0x88/0x58) all match. Only the callee-saved FP colouring
// differs: the original holds 48/32/dconst/half/basehalf in f23/f24/f25/f26/f27 with
// scale->f19 and s->f20. With the declaration order below ours puts 48/32/dconst/half in
// f24/f26/f27/f25 and basehalf in f23 (nx now correctly reuses the dead scale register f19).
// That declaration order is what moves nx onto the freed f19: the natural order puts basehalf
// there and gives the loop's nx temp f23, which is two more instructions off. The remaining
// five registers are one 5-cycle over f23-f27 that no declaration order, assignment order or
// opt pragma (opt_lifetimes/opt_common_subs/opt_propagation/optimization_level/scheduling)
// tried so far changes. Writing part2's `base / 2.0f` as a division stops the compiler
// CSEing it with the precomputed basehalf, which keeps the 13-register frame.
// The @NNN vs named double_to_int_* pool labels are benign (see window/win_mario.c).
// NONMATCHING (99.79%): only the callee-saved FP register colouring differs (5-cycle f23-f27).
void effPointGetDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	PointGetWork* work = effect->userdata;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx mtxB8;
	Mtx mtx88;
	Mtx mtx58;
	char buf[32];
	f32 f26;
	f32 f27;
	f32 f23;
	f32 f22;
	f32 scale;
	f32 f28;
	f32 f31;
	f32 f20;
	f32 f30;
	f32 f21;
	f32 ox;
	f32 nx;
	f32 f24;
	s32 i;

	cam = camGetPtr(camId);
	scale = work->scale;
	sprintf(buf, "%d", work->value);
	f20 = 32.0f * (f32)strlen(buf);
	PSMTXTrans(mtxB8, work->x, work->y, work->z);
	PSMTXScale(mtx88, scale, scale, scale);
	PSMTXConcat(mtxB8, mtx88, mtx58);
	PSMTXConcat(cam->view, mtx58, mtx58);

	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 4);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevOp(0, 0);
	GXSetCullMode(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xd, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xd, 1, 4, 0);

	f30 = 10.0f;
	f26 = 0.5f;
	f28 = 24.0f;
	f24 = 32.0f;
	f23 = 48.0f;
	f21 = -f23 * f26;
	f22 = -(362.0f + f20);
	f31 = 352.0f;
	f27 = f22 * f26;

	for (i = 1; i >= 0; i--) {
		s32 ofs = i * 4;
		ox = (f32)ofs;
		nx = (f32)(-i * 4);

		if (i != 0) {
			effGetTexObj(texid_tbl[gp->language != 0][3], &texObj);
			GXLoadTexObj(&texObj, 0);
			GXSetChanMatColor(4, dat_80420ec8);
		} else {
			effGetTexObj(texid_tbl[gp->language != 0][2], &texObj);
			GXLoadTexObj(&texObj, 0);
			GXSetChanMatColor(4, dat_80420ecc);
		}

		{
			s32 j;

			for (j = 0; j < strlen(buf); j++) {
			{
				f32 t = f24 * (f32)j + f27;
				PSMTXTrans(mtxB8, ox + t, nx, 0.0f);
			}
				PSMTXConcat(mtx58, mtxB8, mtxB8);
				GXLoadPosMtxImm(mtxB8, 0);
				GXSetCurrentMtx(0);
				PSMTXScale(mtx88, 0.1f, 1.0f, 1.0f);
				PSMTXTrans(mtxB8, (f32)((s8)buf[j] - '0'), 0.0f, 0.0f);
				PSMTXConcat(mtx88, mtxB8, mtxB8);
				GXLoadTexMtxImm(mtxB8, 0x1e, 1);
				GXBegin(0x80, 0, 4);
				GXPosition3f32(0.0f, f28, 0.0f);
				GXTexCoord2f32(0.0f, 0.0f);
				GXPosition3f32(f23, f28, 0.0f);
				GXTexCoord2f32(1.0f, 0.0f);
				GXPosition3f32(f23, f21, 0.0f);
				GXTexCoord2f32(1.0f, 1.0f);
				GXPosition3f32(0.0f, f21, 0.0f);
				GXTexCoord2f32(0.0f, 1.0f);
			}
		}

		if (i != 0) {
			effGetTexObj(texid_tbl[gp->language != 0][1], &texObj);
			GXLoadTexObj(&texObj, 0);
			GXSetChanMatColor(4, dat_80420ed0);
		} else {
			effGetTexObj(texid_tbl[gp->language != 0][0], &texObj);
			GXLoadTexObj(&texObj, 0);
			GXSetChanMatColor(4, dat_80420ed4);
		}

		{
			f32 t = f22 / 2.0f + f24 * (f32)strlen(buf);
			t = ox + t;
			t = f30 + t;
			PSMTXTrans(mtxB8, t, nx, 0.0f);
		}
		PSMTXConcat(mtx58, mtxB8, mtxB8);
		GXLoadPosMtxImm(mtxB8, 0);
		GXSetCurrentMtx(0);
		PSMTXIdentity(mtxB8);
		GXLoadTexMtxImm(mtxB8, 0x1e, 1);
		GXBegin(0x80, 0, 4);
		GXPosition3f32(0.0f, f28, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(f31, f28, 0.0f);
		GXTexCoord2f32(1.0f, 0.0f);
		GXPosition3f32(f31, f21, 0.0f);
		GXTexCoord2f32(1.0f, 1.0f);
		GXPosition3f32(0.0f, f21, 0.0f);
		GXTexCoord2f32(0.0f, 1.0f);
	}
}

