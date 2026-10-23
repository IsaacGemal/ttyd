#include "eff/n64/eff_thunderflare_n64.h"
#include "eff/eff_n64.h"
#include "memory.h"
#include "driver/dispdrv.h"
#include "math_ext.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .rodata objects owned by this unit.
// The pooled int->float conversion double, named so the `lfd` reloc resolves like the original.
// .sbss2 data symbols owned by this unit.

// .data: retail 16x16 quad vertex array (local to this unit).
static u32 size16x16_tex32x32_vtx[28] = {
	0xFFF8FFF8, 0x00000000, 0x0000FAFA, 0xFAFF0008,
	0xFFF80000, 0x04000000, 0xFAFAFAFF, 0x00080008,
	0x00000400, 0x0400FA00, 0x00FFFFF8, 0x00080000,
	0x00000400, 0xFA0000FF, 0xFFF4FFF4, 0x0000FE00,
	0x0200FAFA, 0xFAFF000C, 0xFFF40000, 0x0200FE00,
	0xFAFAFAFF, 0x000C000C, 0x00000600, 0x0200FA00,
	0x00FFFFF4, 0x000C0000, 0x02000600, 0xFA0000FF,
};

// The element index-to-value expression must read `i + j - 1`, not `i - 1 + j`:
// the former lets MWCC fold the second iteration back to `i` and reuse r10, the
// latter makes it keep a copy of the pre-increment `i` in r11.
EffectEntry* effThunderflareN64Entry(s32 type, s32 timer, f32 x, f32 y, f32 z, f32 unused) {
	EffectEntry* entry;
	EffThunderflareData* data;
	EffThunderflareData* elem;
	s32 i;
	s32 j;

	entry = effEntry();
	entry->type = "ThunderflareN64";
	entry->count = 0x1f;
	data = __memAlloc(HEAP_EFFECT, 0x8b8);
	entry->userdata = data;
	entry->callback = effThunderflareMain;
	data->type = type;
	data->counter = 0;
	if (timer <= 0) {
		data->timer = 100;
	} else {
		data->timer = timer;
	}
	data->alpha = 0;
	data->x = x;
	data->y = y;
	data->z = z;

	switch (type) {
	case 0:
		data->tevReg0R = 0xff;
		data->tevReg0G = 0;
		data->tevReg0B = 0xff;
		data->tevReg1R = 0xff;
		data->tevReg1G = 0xff;
		data->tevReg1B = 0xff;
		break;
	case 1:
	default:
		data->tevReg0R = 0xff;
		data->tevReg0G = 0x14;
		data->tevReg0B = 0x21;
		data->tevReg1R = 0x32;
		data->tevReg1G = 0x14;
		data->tevReg1B = 0x0a;
		break;
	}

	for (i = 1; i < 0x1f; i += 2) {
		elem = &data[i];
		for (j = 0; j < 2; j++, elem++) {
			s32 particle = i + j - 1;
			elem->angle1 = (f32)(particle * 0xca8 / 30);
			elem->angle2 = 0.0f;
			elem->radius = (f32)(particle * 100 / 30);
			elem->field_0x00 = 0;
		}
	}
	return entry;
}

void effThunderflareMain(EffectEntry* effect) {
	EffThunderflareData* data;
	Vec pos;
	s32 phase;
	s32 i;
	s32 counter;
	f32 sin1;
	f32 cos1;
	f32 sin2;
	f32 cos2;
	f32 angle2;
	f32 angle2raw;
	f32 radius;
	f32 angle1;

	data = effect->userdata;
	pos = (Vec){ data->x, data->y, data->z };

	if (data->timer < 100) {
		data->timer--;
	}
	data->counter++;
	if (data->timer < 0) {
		effDelete(effect);
		return;
	}
	counter = data->counter;
	if (counter <= 20) {
		data->alpha = counter * 12;
	} else {
		data->alpha = 0xff;
	}
	if (counter & 1) {
		data->alpha = (s32)(0.5f * (f32)data->alpha);
	}

	phase = data->counter & 1;
	data++;
	for (i = 1; i < effect->count; i++, data++) {
		angle1 = 6.2831855f * data->angle1 / 360.0f;
		radius = data->radius;
		angle2raw = data->angle2;
		sin1 = sin(angle1);
		cos1 = cos(angle1);
		angle2 = 6.2831855f * angle2raw / 360.0f;
		sin2 = sin(angle2);
		cos2 = cos(angle2);

		data->x = cos2 * (radius * sin1);
		data->y = cos2 * (radius * cos1);
		data->z = radius * sin2;
		data->alpha = (s32)((100.0f - data->radius) * 255.0f * 2.0f / 100.0f);
		if (data->alpha > 0xff) {
			data->alpha = 0xff;
		}
		data->scale = 0.2f + data->radius / 80.0f;
		data->radius -= (f32)(2 + (i & 3) * 2);
		if (data->radius < 0.0f) {
			data->radius = 0.0f;
			data->scale = 0.0f;
		}
		data->angle1 += phase ? 5 : 2;
		data->angle2 += phase ? 2 : 5;
	}
	dispEntry(4, dispCalcZ(&pos), 2, effThunderflareDisp, effect);
}
void effThunderflareDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	CameraEntry* cam;
	EffThunderflareData* data;
	GXTexObj texObj;
	Mtx trans;
	Mtx work;
	Mtx rot;
	Mtx base;
	s32 r;
	s32 g;
	s32 b;
	s32 i;
	s32 idx0;
	s32 idx1;
	s32 idx2;
	s32 idx3;
	f32 scale;

	cam = camGetPtr(camId);
	data = effect->userdata;
	r = data->tevReg0R;
	g = data->tevReg0G;
	b = data->tevReg0B;

	PSMTXTrans(trans, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(trans, rot, trans);
	PSMTXConcat(cam->view, trans, base);

	GXSetNumChans(0);
	GXSetNumTevStages(2);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_C0, GX_CC_C1, GX_CC_CPREV, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_A0, GX_CA_APREV, GX_CA_ZERO);

	GXSetTevColor(GX_TEVREG1, (GXColor){ data->tevReg1R, data->tevReg1G, data->tevReg1B, 0 });

	data++;
	effGetTexObjN64(0x55, &texObj);
	GXLoadTexObj(&texObj, 0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	PSMTXScale(work, 0.03125f, 0.03125f, 0.0f);
	GXLoadTexMtxImm(work, 0x1E, 1);
	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64(size16x16_tex32x32_vtx);

	for (i = 1; i < effect->count; i++, data++) {
		scale = data->scale;
		if (scale) {
			GXSetTevColor(GX_TEVREG0, (GXColor){ r, g, b, data->alpha });

			PSMTXTrans(trans, data->x, data->y, data->z);
			PSMTXScale(work, scale, scale, scale);
			PSMTXConcat(trans, work, trans);
			PSMTXConcat(base, trans, trans);
			GXLoadPosMtxImm(trans, 0);
			GXSetCurrentMtx(0);
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
			idx0 = (i & 1) * 4;
			idx1 = idx0 + 1;
			idx2 = idx0 + 2;
			idx3 = idx0 + 3;
			tri2(idx0, idx1, idx2, 0, idx0, idx2, idx3, 0);
		}
	}
}

