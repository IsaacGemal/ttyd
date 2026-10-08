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
const char str_80420ED8[];
const f32 float_32_80420edc;
const f32 float_362_80420ee0;
const f32 float_48_80420ee4;

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

// .sdata texture ids indexed [language != 0][texture]
static u32 texid_tbl[2][4] = {
	{ 0x98, 0x99, 0x9A, 0x9B },
	{ 0x98, 0x99, 0x9A, 0x9B },
};

EffectEntry* effPointGetEntry(s32 type, s32 value, s32 wait, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	PointGetWork* work;

	entry = effEntry();
	entry->type = "PointGet\0\0\0";
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

// The half-height and the number's left edge are computed per row inside the loop, as the original
// does; MWCC's frontend loop-invariant hoisting would move them (and `f22 * f26`) into early temps that
// take different FPRs. Naming float_48_80420ee4 fixes the 48<->32 assignment. Writing part2's
// `base / 2.0f` as a division stops the compiler CSEing it with the inner loop's product.
#pragma opt_loop_invariants off // the original keeps these per-iteration computations in place (as effKemuTestDisp)
void effPointGetDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	PointGetWork* work = effect->userdata;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx mtxB8;
	Mtx mtx88;
	Mtx mtx58;
	char buf[32];
	f32 f24;
	f32 f23;
	f32 scale;
	f32 f26;
	f32 f22;
	f32 f30;
	f32 f21;
	f32 ox;
	f32 nx;
	f32 f28;
	f32 f31;
	f32 f20;
	s32 i;

	cam = camGetPtr(camId);
	scale = work->scale;
	sprintf(buf, str_80420ED8, work->value);
	f20 = float_32_80420edc * (f32)strlen(buf);
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
	f24 = float_32_80420edc;
	f23 = float_48_80420ee4;
	f31 = 352.0f;

	for (i = 1; i >= 0; i--) {
		s32 ofs = i * 4;
		ox = (f32)ofs;
		nx = (f32)(-i * 4);
		f21 = -f23 * f26;
		f22 = -(float_362_80420ee0 + f20);

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
				f32 t = f24 * (f32)j + f22 * f26;
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

#pragma opt_loop_invariants reset
// Named .sdata2 constants kept by tools/pool_plan.py, defined after the code in the original's order.
const char str_80420ED8[] = "%d";
const f32 float_32_80420edc = 32.0f;
const f32 float_362_80420ee0 = 362.0f;
const f32 float_48_80420ee4 = 48.0f;
