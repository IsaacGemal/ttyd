#include "eff/n64/eff_expbom_n64.h"
#include "driver/dispdrv.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include "math_ext.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdlib.h>

// .sbss2 scratch colours: the original keeps these uninitialised GXColors in .sbss2, reached through
// .sdata declarations (declared in reverse: placed in reverse of declaration order).
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_804230e4;
__declspec(section ".sdata") GXColor unk_804230e0;

typedef struct effExpBomData {
	s16 field_0x0; //0x0
	s16 field_0x2; //0x2
	s16 field_0x4; //0x4
	s16 field_0x6; //0x6
	s32 field_0x8; //0x8
	Vec position; //0xC
	f32 field_0x18; //0x18
	f32 field_0x1C; //0x1C
	f32 field_0x20; //0x20
	f32 field_0x24; //0x24
	u8 field_0x28[0x2C - 0x28]; //0x28
} effExpBomData;

//.sdata
f32 bom_dir[10] = {
	10.0f, 40.0f, 80.0f, 170.0f, 140.0f,
	100.0f, 25.0f, 155.0f, 60.0f, 120.0f
};

f32 bom_spd[] = {
	2.2f, 2.7f, 3.0f, 2.2f, 2.7f,
	3.0f, 1.9f, 1.9f, 1.5f, 1.5f
};

f32 bom_scale[10] = {
	1.4f, 1.3f, 1.2f, 1.3f, 1.4f,
	1.3f, 1.6f, 1.6f, 1.6f, 1.6f
};

f32 bom_ry[10] = {
	0.0f, 234.0f, 468.0f, 702.0f, 936.0f,
	1260.0f, 1404.0f, 1638.0f, 1902.0f, 1976.0f
};

//local prototypes

EffectEntry* effExpBomN64Entry(f32 x, f32 y, f32 z) {
	f32 *direction, *speed;
	effExpBomData* data;
	s32 random;
	// Declaration order pins the FPR/GPR allocation: sinv/v29/v28/cosv -> f23/f22/f21/f2, and
	// effect declared last gets r31. The i=0-before-pointers and the increment order below are
	// likewise required to reproduce the original's scheduling.
	f32 sinv;
	f32 v29;
	f32 v28;
	f32 cosv;
	int i;
	EffectEntry* effect = effEntry();

	effect->type = "ExpBomN64";
	effect->count = 10;
	data = __memAlloc(HEAP_EFFECT, sizeof(effExpBomData) * 10);
	effect->userdata = data;
	effect->callback = effExpBomMain;
	i = 0;
	direction = bom_dir;
	speed = bom_spd;
	for (; i < effect->count; direction++, speed++, i++, data++) {
		data->field_0x0 = 0;
		random = rand() % 6 + 16;
		data->field_0x4 = random;
		data->field_0x2 = random;
		data->field_0x6 = 4;
		data->field_0x8 = 0;
		data->position.x = x;
		data->position.y = y;
		data->position.z = z;
		data->field_0x18 = 0.0f;
		data->field_0x1C = 0.0f;

		v28 = 2.0f * *speed;
		v29 = ((6.2831855f * (90.0f + *direction)) / 360.0f);
		sinv = (f32)sin(v29);
		cosv = (f32)cos(v29);
		data->field_0x20 = v28 * sinv;
		data->field_0x24 = -v28 * cosv;
	}
	return effect;
}

static inline Vec effExpBomMakeVec(f32 x, f32 y, f32 z) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = x;
	temp.y = y;
	temp.z = z;
	return temp;
}

void effExpBomMain(EffectEntry* effect) {
	effExpBomData* data;
	int i;
	BOOL disp;
	Vec position2;
	Vec position;

	disp = FALSE;
	data = effect->userdata;
	position = effExpBomMakeVec(data->position.x, data->position.y, data->position.z);
	position2 = position;

	for (i = 0; i < effect->count; i++, data++) {
		if (data->field_0x2 >= 0) {
			data->field_0x8--;
			if (data->field_0x8 < 0) {
				disp = TRUE;
				data->field_0x18 += data->field_0x20;
				data->field_0x1C += data->field_0x24;
				data->field_0x0++;
				data->field_0x2--;
				if (data->field_0x2 >= 0) {
					data->field_0x6--;
					if (data->field_0x6 < 0) {
						data->field_0x20 /= 1.25f;
						data->field_0x24 /= 1.25f;
					}
				}
			}
		}
	}
	if (!disp) {
		effDelete(effect);
	}
	else {
		dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effExpBomDisp, effect);
	}
}

// The colours go through inlines that return a GXColor by value: that is what produces the
// build-then-copy sequence the original has.
static inline GXColor effExpBomColor1(s32 alpha) {
	GXColor c = unk_804230e0;
	c.a = alpha;
	return c;
}

static inline GXColor effExpBomColor2(s32 count) {
	GXColor c = unk_804230e4;
	c.a = (count * 210) >> 4;
	return c;
}

// NONMATCHING (99.99%): 5 instructions differ. The two GXSetTevColor struct-by-value calls build their
// GXColor temporaries in different stack slots: original colour1 0x10->arg 0x14 and colour2 0x8->arg 0xc;
// ours colour1 0xc->arg 0x14 and colour2 0x8->arg 0x10. Only the slot offsets differ.
void effExpBomDisp(CameraId camId, void* param) {
	EffectEntry* effect;
	effExpBomData* data;
	CameraEntry* camera;
	GXColor color;
	GXTexObj texobj;
	Mtx m0;
	Mtx m1;
	Mtx m2;
	Mtx viewMtx;
	s32 count;
	s32 i;
	s32 type;

	effect = param;
	camera = camGetPtr(camId);
	data = effect->userdata;

	PSMTXTrans(m0, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(m1, 'y', -camGetPtr(camId)->viewYaw * 0.017453292f);
	PSMTXConcat(m0, m1, viewMtx);
	PSMTXConcat(camera->view, viewMtx, viewMtx);

	for (i = 0; i < effect->count; i++, data++) {
		if (data->field_0x2 >= 0 && data->field_0x8 < 0) {
			s32 alpha;
			f32 frame;
			f32 last;
			f32 t;

			count = data->field_0x2;
			if (count > 16) {
				count = 16;
			}
			PSMTXTrans(m0, data->field_0x18, data->field_0x1C, 0.0f);
			PSMTXScale(m2, bom_scale[i], bom_scale[i], bom_scale[i]);
			PSMTXRotRad(m1, 'z', bom_ry[i] * 0.017453292f);
			PSMTXConcat(m0, m2, m0);
			PSMTXConcat(m0, m1, m1);
			PSMTXConcat(viewMtx, m1, m1);
			GXLoadPosMtxImm(m1, GX_PNMTX0);

			frame = (f32)data->field_0x0;
			t = frame - 1.0f;
			last = (f32)data->field_0x4;
			if (t >= last) {
				alpha = 0xFF;
				type = 6;
			}
			else {
				f32 a = 7.0f * t;
				f32 b = 90.0f * (a / last) / 7.0f;
				f32 s = (f32)sin(6.2831855f * b / 360.0f);
				f32 af;

				s = 7.0f * s;
				af = 255.0f * s;
				alpha = (s32)af % 256;
				type = (s32)s;
			}

			color = effExpBomColor1(alpha);
			GXSetTevColor(GX_TEVREG0, color);

			color = effExpBomColor2(count);
			GXSetTevColor(GX_TEVREG1, color);

			GXSetNumChans(0);
			GXSetNumTexGens(2);
			GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
			GXSetTexCoordGen2(1, 1, 4, 0x3C, 0, 0x7D);
			GXSetNumTevStages(3);
			GXSetTevOrder(0, 0, 0, 0xFF);
			GXSetTevColorOp(0, 0, 0, 0, 1, 0);
			GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
			GXSetTevColorIn(0, 0xF, 0xF, 0xF, 8);
			GXSetTevAlphaIn(0, 7, 7, 7, 4);
			GXSetTevOrder(1, 1, 1, 0xFF);
			GXSetTevColorOp(1, 0, 0, 0, 1, 0);
			GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
			GXSetTevColorIn(1, 0, 8, 3, 0xF);
			GXSetTevAlphaIn(1, 0, 4, 1, 7);
			GXSetTevOrder(2, 0xFF, 0xFF, 0xFF);
			GXSetTevColorOp(2, 0, 0, 0, 1, 0);
			GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
			GXSetTevColorIn(2, 0xF, 0xF, 0xF, 0);
			GXSetTevAlphaIn(2, 7, 0, 2, 7);
			GXSetCullMode(0);
			GXClearVtxDesc();
			GXSetVtxDesc(9, 1);
			GXSetVtxDesc(13, 1);
			GXSetVtxAttrFmt(0, 9, 1, 4, 0);
			GXSetVtxAttrFmt(0, 13, 1, 4, 0);

			switch (type) {
			case 0:
				effGetTexObjN64(0, &texobj);
				GXLoadTexObj(&texobj, 0);
				effGetTexObjN64(1, &texobj);
				GXLoadTexObj(&texobj, 1);
				break;
			case 1:
				effGetTexObjN64(1, &texobj);
				GXLoadTexObj(&texobj, 0);
				effGetTexObjN64(2, &texobj);
				GXLoadTexObj(&texobj, 1);
				break;
			case 2:
				effGetTexObjN64(2, &texobj);
				GXLoadTexObj(&texobj, 0);
				effGetTexObjN64(3, &texobj);
				GXLoadTexObj(&texobj, 1);
				break;
			case 3:
				effGetTexObjN64(3, &texobj);
				GXLoadTexObj(&texobj, 0);
				effGetTexObjN64(4, &texobj);
				GXLoadTexObj(&texobj, 1);
				break;
			case 4:
				effGetTexObjN64(4, &texobj);
				GXLoadTexObj(&texobj, 0);
				effGetTexObjN64(5, &texobj);
				GXLoadTexObj(&texobj, 1);
				break;
			case 5:
				effGetTexObjN64(5, &texobj);
				GXLoadTexObj(&texobj, 0);
				effGetTexObjN64(6, &texobj);
				GXLoadTexObj(&texobj, 1);
				break;
			case 6:
				effGetTexObjN64(6, &texobj);
				GXLoadTexObj(&texobj, 0);
				effGetTexObjN64(6, &texobj);
				GXLoadTexObj(&texobj, 1);
				break;
			}

			GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			GXPosition3f32(-16.0f, 16.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(16.0f, 16.0f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);
			GXPosition3f32(16.0f, -16.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);
			GXPosition3f32(-16.0f, -16.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
		}
	}
}
