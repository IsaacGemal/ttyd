#include "eff/n64/eff_hokori_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <stdlib.h>
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sdata2 / .rodata constants used only by effHokoriN64Entry, named after their
// original addresses. The string lives in this unit's own .rodata.
extern const f32 float_0_8041f110;
extern const f32 float_neg1_8041f120;
extern const f32 float_1_8041f124;
extern const f32 float_200_8041f128;
extern const f32 float_0p1_8041f12c;
extern const f32 float_0p2_8041f130;
extern const f32 float_0p4_8041f134;
extern const f64 double_to_int_802fa7b8;
extern const char str_802FA7C0[];

// .sbss2 colour bases the retail unit leaves uninitialised; the fields are
// overwritten one by one before each GXSetTevColor.
GXColor unk_80423140;
GXColor unk_80423144;

// Float pairs that precede the vertex arrays in the retail unit's .data; kept so
// the unit's data section is complete. The arrays themselves are reached through
// lbl_8039D5D0 (declared in the header) in effHokoriDisp under MUST_MATCH.
static f32 negone_one_374[2] = { -1.0f, 1.0f };
f32 lbl_8039D5E0[2] = { 1.0f, -1.0f };
static f32 one_negone_373[2] = { 1.0f, -1.0f };
f32 lbl_8039D5F0[2] = { -1.0f, 1.0f };
static f32 negone_one_360[2] = { -1.0f, 1.0f };
f32 lbl_8039D600[2] = { 1.0f, -1.0f };
static f32 one_negone_359[2] = { 1.0f, -1.0f };

// .data vertex arrays.
u32 size16x32_tex32x64_vtx[14] = {
	0xFFF8FFE0, 0x00000000, 0x04000000, 0x00FF0008,
	0xFFE00000, 0x04000400, 0x000000FF, 0x00080000,
	0x00000400, 0x0BE00000, 0x00FFFFF8, 0x00000000,
	0x00000BE0, 0x000000FF,
};
static u32 size16x16_tex32x32_vtx[14] = {
	0xFFF8FFF0, 0x00000000, 0x00000000, 0x00FF0008,
	0xFFF00000, 0x04000000, 0x000000FF, 0x00080000,
	0x00000400, 0x04000000, 0x00FFFFF8, 0x00000000,
	0x00000400, 0x000000FF,
};
static u32 size8x8_tex32x32_vtx[14] = {
	0xFFFCFFFC, 0x00000000, 0x00000000, 0x00FF0004,
	0xFFFC0000, 0x04000000, 0x000000FF, 0x00040004,
	0x00000400, 0x04000000, 0x00FFFFFC, 0x00040000,
	0x00000400, 0x000000FF,
};

EffectEntry* effHokoriN64Entry(s32 type, s32 count, f32 x, f32 y, f32 z) {
	EffectEntry* effect;
	effHokoriData* data;
	effHokoriData* p;
	s32 num;
	s32 i;

	effect = effEntry();
	num = (type <= 1) ? 5 : 4;
	effect->type = str_802FA7C0;
	effect->count = num;
	data = __memAlloc(HEAP_EFFECT, num * 0x48);
	effect->userdata = data;
	effect->callback = effHokoriMain;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->life = count;
	data->field_0x18 = 0xff;
	data->field_0x30 = 0xa0;
	data->field_0x34 = 0xa0;
	data->field_0x38 = 0x8c;
	data->field_0x3C = 0xa0;
	data->field_0x40 = 0xa0;
	data->field_0x44 = 0x8c;

	for (i = 1, p = data + 1; i < num; i++, p++) {
		p->x = (f32)(rand() % 200 - 100);
		p->y = (f32)(rand() % 50);
		p->z = float_0_8041f110;
		p->field_0x10 = float_neg1_8041f120;
		p->field_0x14 = float_0_8041f110;
		p->field_0x1C = float_1_8041f124;
		p->field_0x20 = float_1_8041f124;
		p->field_0x24 = (f32)(rand() % 10) / float_200_8041f128;
		switch (type) {
		case 0:
			p->field_0x28 = float_0p1_8041f12c * p->field_0x24;
			break;
		case 1:
			p->field_0x28 = float_0p2_8041f130 * p->field_0x24;
			break;
		default:
			p->field_0x24 = float_0p4_8041f134;
			p->field_0x28 = float_0p4_8041f134;
			p->field_0x10 = float_0_8041f110;
			p->x = float_0p1_8041f12c * (f32)(rand() % 40 - 20);
			p->y = float_0p1_8041f12c * (f32)(rand() % 40 - 20);
			break;
		}
	}
	return effect;
}

inline Vec makeVec(f32 x, f32 y, f32 z) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = x;
	temp.y = y;
	temp.z = z;
	return temp;
}

void effHokoriMain(EffectEntry* effect) {
	effHokoriData* data;
	Vec position2;
	Vec position;
	s32 type;
	s32 i;

	data = effect->userdata;
	position = makeVec(data->x, data->y, data->z);
	position2 = position;
	data->life--;
	if (data->life < 0) {
		effDelete(effect);
	}
	else {
		type = data->type;
		if (data->life < 0x18) {
			data->field_0x18 = (s32)((f32)data->field_0x18 * float_0p9_8041f118);
		}
		i = 1;
		data++;
		if (type <= 1) {
			for (; i < effect->count; i++, data++) {
				data->field_0x10 += data->field_0x14;
				data->y += data->field_0x10;
				data->field_0x24 += float_0p005_8041f11c;
				data->field_0x1C += data->field_0x24;
				data->field_0x20 += data->field_0x28;
			}
		}
		else {
			for (; i < effect->count; i++, data++) {
				data->field_0x10 += data->field_0x14;
				data->y += data->field_0x10;
				data->field_0x1C += data->field_0x24;
				data->field_0x20 += data->field_0x28;
			}
		}
		dispEntry(CAMERA_3D, dispCalcZ(&position2), 2, effHokoriDisp, effect);
	}
}

static inline void hokoriSetColor1(effHokoriData* data) {
	GXColor color = unk_80423140;
	color.r = data->field_0x30;
	color.g = data->field_0x34;
	color.b = data->field_0x38;
	color.a = data->field_0x18;
	GXSetTevColor(1, color);
}

static inline void hokoriSetColor2(effHokoriData* data) {
	GXColor color = unk_80423144;
	color.r = data->field_0x3C;
	color.g = data->field_0x40;
	color.b = data->field_0x44;
	GXSetTevColor(2, color);
}

void effHokoriDisp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	effHokoriData* data;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx trans;
	Mtx scale;
	Mtx rot;
	Mtx work;
	effVtxArray* vtx;
#ifdef MUST_MATCH
	u8* vtxBase = (u8*)lbl_8039D5D0;
#endif
	s32 i;
	s32 type;

	cam = camGetPtr(cameraId);
	data = effect->userdata;

	GXSetNumChans(0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	PSMTXScale(scale, float_0p03125_8041f108, float_0p010417_8041f10c, float_0_8041f110);
	GXLoadTexMtxImm(scale, 0x1e, 1);

	hokoriSetColor1(data);
	hokoriSetColor2(data);

	switch (data->type) {
	case 0:
		GXSetNumTevStages(1);
		GXSetTevOrder(0, 0, 0, 0xff);
		GXSetTevColorOp(0, 0, 0, 0, 1, 0);
		GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
		GXSetTevColorIn(0, 0xf, 0xf, 0xf, 2);
		GXSetTevAlphaIn(0, 7, 1, 4, 7);
		effGetTexObjN64(0x15, &texObj);
		GXLoadTexObj(&texObj, 0);
		break;
	default:
		GXSetNumTevStages(1);
		GXSetTevOrder(0, 0, 0, 0xff);
		GXSetTevColorOp(0, 0, 0, 0, 1, 0);
		GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
		GXSetTevColorIn(0, 4, 2, 8, 0xf);
		GXSetTevAlphaIn(0, 7, 1, 4, 7);
		effGetTexObjN64(0x16, &texObj);
		GXLoadTexObj(&texObj, 0);
		break;
	}

	PSMTXTrans(trans, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', float_deg2rad_8041f114 * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(trans, rot, work);
	PSMTXConcat(cam->view, work, work);

	type = data->type;
	data++;
	GXSetCullMode(0);

	for (i = 1; i < effect->count; i++, data++) {
		PSMTXTrans(trans, data->x, data->y, data->z);
		PSMTXScale(scale, data->field_0x20, data->field_0x1C, data->field_0x20);
		PSMTXConcat(trans, scale, trans);
		PSMTXConcat(work, trans, trans);
		GXLoadPosMtxImm(trans, 0);
		GXSetCurrentMtx(0);
		if (type <= 1) {
			if (i & 1) {
#ifdef MUST_MATCH
				vtx = (effVtxArray*)(vtxBase + 0x40);
#else
				vtx = (effVtxArray*)size16x32_tex32x64_vtx;
#endif
			} else {
#ifdef MUST_MATCH
				vtx = (effVtxArray*)(vtxBase + 0x78);
#else
				vtx = (effVtxArray*)size16x16_tex32x32_vtx;
#endif
			}
		} else {
#ifdef MUST_MATCH
			vtx = (effVtxArray*)(vtxBase + 0xb0);
#else
			vtx = (effVtxArray*)size8x8_tex32x32_vtx;
#endif
		}
		effSetVtxDescN64(vtx);
		GXBegin(0x90, 0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
	}
}
