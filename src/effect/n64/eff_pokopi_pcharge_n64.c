#include "eff/n64/eff_pokopi_pcharge_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "math_ext.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdlib.h>

GXColor unk_80423208;
GXColor unk_8042320c;
GXColor unk_80423210;

//.sdata
u8 n_data[16] = {
	0x0B, 0x0B, 0x0B, 0x0B, 0x11, 0x0B, 0x0B, 0x15,
	0x15, 0x15, 0x15, 0x15, 0x15, 0x00, 0x00, 0x00
};

static u8 vx_data[16] = {
	0x14, 0x05, 0x14, 0x14, 0x0A, 0x14, 0x14, 0x08,
	0x08, 0x08, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00
};

//.data
__declspec(section ".data") static f32 negone_one_374[2] = { -1.0f, 1.0f };
__declspec(section ".data") f32 lbl_803A4128[2] = { -1.0f, 1.0f };
__declspec(section ".data") static f32 one_negone_373[2] = { 1.0f, -1.0f };
__declspec(section ".data") f32 lbl_803A4138[2] = { 1.0f, -1.0f };
__declspec(section ".data") static f32 negone_one_360[2] = { -1.0f, 1.0f };
__declspec(section ".data") f32 lbl_803A4148[2] = { -1.0f, 1.0f };
__declspec(section ".data") static f32 one_negone_359[2] = { 1.0f, -1.0f };
__declspec(section ".data") f32 lbl_803A4158[2] = { 1.0f, -1.0f };

static u32 size10x10_tex32x32_vtx[56] = {
	0x0000FFFB, 0x00000000, 0x00000000, 0x00FF000A, 0xFFFB0000, 0x04000000, 0x000000FF, 0x000A0005,
	0x00000400, 0x04000000, 0x00FF0000, 0x00050000, 0x00000400, 0x000000FF, 0x0000FFFB, 0x00000000,
	0x04000000, 0x00FF000A, 0xFFFB0000, 0x04000400, 0x000000FF, 0x000A0005, 0x00000400, 0x08000000,
	0x00FF0000, 0x00050000, 0x00000800, 0x000000FF, 0x0000FFFB, 0x00000000, 0x08000000, 0x00FF000A,
	0xFFFB0000, 0x04000800, 0x000000FF, 0x000A0005, 0x00000400, 0x0C000000, 0x00FF0000, 0x00050000,
	0x00000C00, 0x000000FF, 0x0000FFFB, 0x00000000, 0x0C000000, 0x00FF000A, 0xFFFB0000, 0x04000C00,
	0x000000FF, 0x000A0005, 0x00000400, 0x10000000, 0x00FF0000, 0x00050000, 0x00001000, 0x000000FF,
};

u32 size10x10_tex32x32_2_vtx[56] = {
	0x0000FFFB, 0x00000400, 0x00000000, 0x00FF000A, 0xFFFB0000, 0x00000000, 0x000000FF, 0x000A0005,
	0x00000000, 0x04000000, 0x00FF0000, 0x00050000, 0x04000400, 0x000000FF, 0x0000FFFB, 0x00000400,
	0x04000000, 0x00FF000A, 0xFFFB0000, 0x00000400, 0x000000FF, 0x000A0005, 0x00000000, 0x08000000,
	0x00FF0000, 0x00050000, 0x04000800, 0x000000FF, 0x0000FFFB, 0x00000400, 0x08000000, 0x00FF000A,
	0xFFFB0000, 0x00000800, 0x000000FF, 0x000A0005, 0x00000000, 0x0C000000, 0x00FF0000, 0x00050000,
	0x04000C00, 0x000000FF, 0x0000FFFB, 0x00000400, 0x0C000000, 0x00FF000A, 0xFFFB0000, 0x00000C00,
	0x000000FF, 0x000A0005, 0x00000000, 0x10000000, 0x00FF0000, 0x00050000, 0x04001000, 0x000000FF,
};

u32 size128x10_tex32x32_vtx[56] = {
	0x0000FFFB, 0x00000000, 0x00000000, 0x00FF0080, 0xFFFB0000, 0x04000000, 0x000000FF, 0x00800005,
	0x00000400, 0x04000000, 0x00FF0000, 0x00050000, 0x00000400, 0x000000FF, 0x0000FFFB, 0x00000000,
	0x04000000, 0x00FF0080, 0xFFFB0000, 0x04000400, 0x000000FF, 0x00800005, 0x00000400, 0x08000000,
	0x00FF0000, 0x00050000, 0x00000800, 0x000000FF, 0x0000FFFB, 0x00000000, 0x08000000, 0x00FF0080,
	0xFFFB0000, 0x04000800, 0x000000FF, 0x00800005, 0x00000400, 0x0C000000, 0x00FF0000, 0x00050000,
	0x00000C00, 0x000000FF, 0x0000FFFB, 0x00000000, 0x0C000000, 0x00FF0080, 0xFFFB0000, 0x04000C00,
	0x000000FF, 0x00800005, 0x00000400, 0x10000000, 0x00FF0000, 0x00050000, 0x00001000, 0x000000FF,
};

u32 size128x10_tex32x32_2_vtx[56] = {
	0x0000FFFB, 0x00000400, 0x00000000, 0x00FF0080, 0xFFFB0000, 0x00000000, 0x000000FF, 0x00800005,
	0x00000000, 0x04000000, 0x00FF0000, 0x00050000, 0x04000400, 0x000000FF, 0x0000FFFB, 0x00000400,
	0x04000000, 0x00FF0080, 0xFFFB0000, 0x00000400, 0x000000FF, 0x00800005, 0x00000000, 0x08000000,
	0x00FF0000, 0x00050000, 0x04000800, 0x000000FF, 0x0000FFFB, 0x00000400, 0x08000000, 0x00FF0080,
	0xFFFB0000, 0x00000800, 0x000000FF, 0x00800005, 0x00000000, 0x0C000000, 0x00FF0000, 0x00050000,
	0x04000C00, 0x000000FF, 0x0000FFFB, 0x00000400, 0x0C000000, 0x00FF0080, 0xFFFB0000, 0x00000C00,
	0x000000FF, 0x00800005, 0x00000000, 0x10000000, 0x00FF0000, 0x00050000, 0x04001000, 0x000000FF,
};


EffectEntry* effPokopiPchargeN64Entry(f32 x, f32 y, f32 z, f32 scale, s32 a, s32 b) {
	EffectEntry* effect;
	s32 count;
	effPokopiPchargeData* root;
	effPokopiPchargeData* data;
	s32 i;
	f32 angle;

	effect = effEntry();
	count = n_data[a];
	effect->type = "PokopiPchargeN64";
	effect->count = count;
	root = __memAlloc(HEAP_EFFECT, sizeof(effPokopiPchargeData) * count);
	effect->userdata = root;
	effect->callback = effPokopiPchargeMain;
	effect->flags |= 2;

	root->type = a;
	root->timer = 0;
	if (b <= 0) {
		root->lifetime = 1000;
	} else {
		root->lifetime = b;
	}
	root->a = 0;
	data = root + 1;
	root->x = x;
	root->y = y;
	root->z = z;
	root->scale = scale;
	root->r = 0xff;
	root->g = 0xff;
	root->b = 0xff;
	root->r2 = 0xff;
	root->g2 = 0xf5;
	root->b2 = 0;
	root->field_0x44 = 1.0f;
	root->field_0x38 = (f32)vx_data[a];
	angle = (f32)(rand() % 360);
	for (i = 1; i < effect->count; i++, data++) {
		data->type = a % 3;
		switch (a) {
		case 0:
		case 1:
		case 2:
			data->field_0x38 = -(0.2f + 0.2f * (f32)(rand() % 10));
			data->lifetime = 20;
			data->scale = 1.0f - 0.1f * (f32)(i % 3);
			data->field_0x40 = 0.0f;
			break;
		case 3:
		case 5:
			data->field_0x38 = 0.2f + 0.2f * (f32)(rand() % 10);
			data->lifetime = 20;
			data->scale = 1.0f - 0.1f * (f32)(i % 3);
			data->field_0x40 = 0.0f;
			break;
		case 6:
			data->field_0x38 = (f32)(-((rand() % 10) + 4));
			data->lifetime = 10;
			data->scale = 1.0f - 0.3f * (f32)(i % 3);
			data->field_0x40 = 0.0f;
			break;
		case 7:
			data->field_0x38 = (f32)(-((rand() % 10) + 4));
			data->lifetime = 10;
			data->scale = 1.0f - 0.3f * (f32)(i % 3);
			data->field_0x40 = 0.0f;
			break;
		case 8:
			data->field_0x38 = 0.6f + 0.2f * (f32)(rand() % 10);
			data->lifetime = 20;
			data->scale = 1.0f - 0.3f * (f32)(i % 3);
			data->field_0x40 = -1.0f;
			break;
		case 9:
			data->field_0x38 = -(0.6f + 0.2f * (f32)(rand() % 10));
			data->lifetime = 20;
			data->scale = 1.0f - 0.3f * (f32)(i % 3);
			data->field_0x40 = -2.0f;
			root->r = 0xff;
			root->g = 0xf5;
			root->b = 0xd7;
			root->r2 = 0xdc;
			root->g2 = 0x3c;
			root->b2 = 0x0a;
			break;
		case 10:
			data->field_0x38 = 2.0f + 0.1f * (f32)(rand() % 10);
			data->lifetime = 10;
			data->scale = 0.5f * (1.0f - 0.3f * (f32)(i % 3));
			data->field_0x40 = 0.0f;
			break;
		case 11:
			data->field_0x38 = 0.2f + 0.05f * (f32)(rand() % 10);
			data->lifetime = 30;
			data->scale = 1.0f - 0.3f * (f32)(i % 3);
			data->field_0x40 = -1.0f;
			break;
		case 4:
		case 12:
			data->field_0x38 = 0.6f + 0.2f * (f32)(rand() % 10);
			data->lifetime = 20;
			data->scale = 1.0f - 0.3f * (f32)(i % 3);
			data->field_0x40 = -2.0f;
			break;
		}
		data->timer = -(rand() % data->lifetime);
		if (data->field_0x38 < 0.0f) {
			data->x = (f32)(data->lifetime - data->timer) * -data->field_0x38;
		} else {
			data->x = (f32)(-data->timer) * data->field_0x38;
		}
		data->y = 0.0f;
		data->z = 0.0f;
		data->field_0x3C = angle + (f32)((i - 1) * 360 / (count - 1));
	}
	return effect;
}

void effPokopiPchargeMain(EffectEntry* effect) {
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch and copies it to a second local before
	   dispCalcZ; a single local folds the copy away. */
	Vec position;
#endif
	Vec disp = { 0.0f, 0.0f, 0.0f };
	effPokopiPchargeData* data = effect->userdata;
	s32 i;
#ifdef MUST_MATCH
	/* MUST_MATCH: caching data->timer in a local makes the compiler load it before the first
	   branch, matching retail's scheduling. */
	s32 timer;
#endif

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
#ifdef MUST_MATCH
	position = disp;
#endif

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 16;
	}
	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	data->timer++;
	if (data->lifetime < 0) {
		effDelete(effect);
		return;
	}
#ifdef MUST_MATCH
	timer = data->timer;
	if (data->lifetime < 16) {
		data->a = data->lifetime << 4;
	}
	if (timer < 16) {
		data->a = (timer << 4) + 0xf;
	}
#else
	if (data->lifetime < 16) {
		data->a = data->lifetime << 4;
	}
	if (data->timer < 16) {
		data->a = (data->timer << 4) + 0xf;
	}
#endif

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		data->timer++;
		if (data->timer >= data->lifetime) {
			data->timer = 0;
			if (data->field_0x38 < 0.0f) {
				data->x = (f32)(-data->lifetime) * data->field_0x38;
			} else {
				data->x = 0.0f;
			}
			data->field_0x3C = (f32)(rand() % 360);
		}
		data->x += data->field_0x38;
		data->field_0x3C += data->field_0x40;
		{
			s32 v = data->timer * 180 / data->lifetime;
			f32 f = (f32)v;
			if (f < 0.0f || f > 180.0f) {
				data->a = 0;
			} else {
				f32 ang = (f32)sin(6.2831855f * f / 360.0f);
				data->a = (s32)(ang * 255.0f);
			}
		}
	}
#ifdef MUST_MATCH
	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effPokopiPchargeDisp, effect);
#else
	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effPokopiPchargeDisp, effect);
#endif
}

static inline void effPokopiPchargeSetColor1(u32 r, u32 g, u32 b) {
	GXColor c = unk_8042320c;
	c.r = r;
	c.g = g;
	c.b = b;
	GXSetTevColor(GX_TEVREG1, c);
}

static inline void effPokopiPchargeSetColor2(u32 r, u32 g, u32 b, s32 a) {
	GXColor c = unk_80423210;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	GXSetTevColor(GX_TEVREG0, c);
}

// These build the three Tev colours retail reads from the unk_80423208/0c/10 globals.  They are void
// inlines (not a colour returned and passed on) because that is what makes MWCC pair each colour's
// build slot with its outgoing argument slot; the direct form gives a different stack layout.
static inline void effPokopiPchargeSetColor0(u32 r, u32 g, u32 b, s32 a) {
	GXColor c = unk_80423208;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	GXSetTevColor(GX_TEVREG0, c);
}

void effPokopiPchargeDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	CameraEntry* camera;
	Mtx mtx;
	Mtx scale;
	Mtx rot;
	Mtx view;
	GXTexObj tex;
	s32 base3;
	s32 base2;
	s32 base1;
	effVtxArray* tex9;
	s32 base;
	effPokopiPchargeData* data;
	s32 i;
	s32 alpha;
	s32 type;
	u32 r;
	u32 g;
	u32 b;
	f32 scaleXZ;
	f32 field38;
	f32 scl;

	camera = camGetPtr(camId);
	data = effect->userdata;
	scaleXZ = data->scale * data->field_0x44;
	alpha = data->a;
	type = data->type;
	r = data->r;
	g = data->g;
	b = data->b;
	field38 = data->field_0x38;
	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXScale(scale, scaleXZ, scaleXZ, scaleXZ);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXConcat(mtx, scale, mtx);
	PSMTXConcat(camera->view, mtx, view);

	effPokopiPchargeSetColor0(data->r, data->g, data->b, alpha);
	effPokopiPchargeSetColor1(data->r2, data->g2, data->b2);

	GXSetNumChans(0);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C1, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, 0x7d);
	PSMTXScale(scale, 0.03125f, 0.0078125f, 0.0f);
	GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
	effGetTexObjN64(0x86, &tex);
	GXLoadTexObj(&tex, GX_TEXMAP0);
	GXSetCullMode(GX_CULL_NONE);

	base = type * 4;
	base3 = base + 3;
	base2 = base + 2;
	base1 = base + 1;
	tex9 = (effVtxArray*)((u8*)size10x10_tex32x32_2_vtx + 0x70);
	data++;
	for (i = 1; i < effect->count; i++, data++) {
		u32 a = (alpha * data->a) / 255;

		scl = data->scale;
		effPokopiPchargeSetColor2(r, g, b, a);

		PSMTXRotRad(rot, 'z', 0.017453292f * data->field_0x3C);
		PSMTXTrans(mtx, data->x + field38, 0.0f, 0.0f);
		PSMTXConcat(rot, mtx, mtx);
		if (type == 11 || type == 10) {
			PSMTXScale(scale, scl, 1.0f, 1.0f);
		} else {
			PSMTXScale(scale, scl, scl, 1.0f);
		}
		PSMTXConcat(mtx, scale, mtx);
		PSMTXConcat(view, mtx, mtx);
		GXLoadPosMtxImm(mtx, 0);
		GXSetCurrentMtx(0);

		switch (type) {
		case 0:
		case 1:
		case 2:
			effSetVtxDescN64((effVtxArray*)size10x10_tex32x32_vtx);
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
			tri2(base, base1, base2, 0, base, base2, base3, 0);
			break;
		case 3:
		case 4:
		case 5:
			effSetVtxDescN64((effVtxArray*)size10x10_tex32x32_2_vtx);
			base3 -= 12;
			base2 -= 12;
			base -= 12;
			base1 -= 12;
			type -= 3;
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
			tri2(base, base1, base2, 0, base, base2, base3, 0);
			break;
		case 6:
		case 7:
		case 8:
		case 11:
			effSetVtxDescN64((effVtxArray*)size128x10_tex32x32_vtx);
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
			tri2(0, 1, 2, 0, 0, 2, 3, 0);
			break;
		case 9:
		case 12:
			effSetVtxDescN64(tex9);
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
			tri2(0, 1, 2, 0, 0, 2, 3, 0);
			break;
		case 10:
			effSetVtxDescN64((effVtxArray*)size128x10_tex32x32_2_vtx);
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
			tri2(0, 1, 2, 0, 0, 2, 3, 0);
			break;
		}
	}
}
