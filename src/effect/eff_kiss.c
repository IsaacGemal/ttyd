#include "eff/eff_kiss.h"
#include "driver/dispdrv.h"
#include "eff/n64/eff_stamp_n64.h"
#include "eff/n64/eff_stardust_n64.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <math.h>

// The original has these constants in .sdata2. Declared const, MWCC folds their values into its own pool;
// the matching build keeps them writable so the loads reference the original symbols.
#ifdef MUST_MATCH
#define SDATA2_CONST
#else
#define SDATA2_CONST const
#endif
SDATA2_CONST char str_804213F0[4] = "<<<";
SDATA2_CONST char str_804213F4[4] = "<<<";
SDATA2_CONST u32 dat_804213f8 = 0xFFB5DA00;
SDATA2_CONST f32 float_deg2rad_804213fc = 0.017453292f;
SDATA2_CONST f32 float_8_80421400 = 8.0f;
SDATA2_CONST f32 float_1_80421404 = 1.0f;
SDATA2_CONST f32 float_0p5_80421408 = 0.5f;
SDATA2_CONST f32 float_0_8042140c = 0.0f;
SDATA2_CONST f32 float_4_80421410 = 4.0f;
SDATA2_CONST f32 float_2_80421414 = 2.0f;
SDATA2_CONST f32 float_1p5_80421418 = 1.5f;
SDATA2_CONST f32 float_32_8042141c = 32.0f;
SDATA2_CONST f32 float_40_80421420 = 40.0f;
SDATA2_CONST f32 float_20_80421424 = 20.0f;
SDATA2_CONST f32 float_5_80421428 = 5.0f;
SDATA2_CONST f32 float_1p15_8042142c = 1.15f;
SDATA2_CONST f32 float_10_80421430 = 10.0f;
SDATA2_CONST f32 float_0p9_80421434 = 0.9f;
SDATA2_CONST f32 float_6p2832_80421438 = 6.2831855f;
SDATA2_CONST f32 float_0p03125_8042143c = 0.03125f;
SDATA2_CONST f32 float_1p2_80421440 = 1.2f;
SDATA2_CONST f32 float_360_80421444 = 360.0f;
SDATA2_CONST f32 float_0p2_80421448 = 0.2f;
SDATA2_CONST f32 float_9p4248_8042144c = 9.424778f;
SDATA2_CONST f32 float_60_80421450 = 60.0f;
SDATA2_CONST f32 float_12_80421454 = 12.0f;
SDATA2_CONST char str_80421458[8] = "kiss";
SDATA2_CONST f32 float_3_80421460 = 3.0f;
SDATA2_CONST f32 float_neg1_80421464 = -1.0f;

// .rodata
const Vec vec3_802fe780 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_mask_802fe790 = 4503599627370496.0;
const f64 double_to_int_802fe798 = 4503601774854144.0;

// .data
// Each record is 3 bytes: radius, scaleX, scaleY (divided by 32 by the reader).
u8 kuri_kiss[] = {
	0x43, 0x20, 0x12, 0x4F, 0x20, 0x14, 0x5E, 0x20, 0x16, 0x6C, 0x20, 0x19,
	0x7A, 0x20, 0x1B, 0x86, 0x20, 0x1D, 0x8F, 0x20, 0x1F, 0x95, 0x20, 0x20,
	0x97, 0x20, 0x20, 0x99, 0x20, 0x20, 0x9A, 0x20, 0x21, 0x9B, 0x20, 0x21,
	0x9B, 0x20, 0x21, 0x9B, 0x20, 0x21, 0x9A, 0x20, 0x21, 0x99, 0x20, 0x21,
	0x97, 0x20, 0x21, 0x96, 0x20, 0x20, 0x94, 0x20, 0x20, 0x92, 0x20, 0x20,
	0x91, 0x20, 0x20, 0x8F, 0x20, 0x20, 0x8E, 0x20, 0x20, 0x8D, 0x20, 0x20,
};

u8 kura_kiss[] = {
	0x43, 0x20, 0x20, 0x42, 0x22, 0x1F, 0x42, 0x24, 0x1F, 0x41, 0x26, 0x1F,
	0x40, 0x28, 0x1F, 0x40, 0x2A, 0x1E, 0x3F, 0x2C, 0x1E, 0x3D, 0x2E, 0x1D,
	0x3C, 0x31, 0x1D, 0x3C, 0x33, 0x1C, 0x3B, 0x35, 0x1C, 0x3A, 0x37, 0x1C,
	0x3A, 0x39, 0x1C, 0x3A, 0x3B, 0x1C, 0x3A, 0x3E, 0x1C, 0x3B, 0x40, 0x1C,
	0x3C, 0x42, 0x1D, 0x3E, 0x44, 0x1E, 0x40, 0x47, 0x1F, 0x43, 0x49, 0x21,
	0x4C, 0x4C, 0x26, 0x5D, 0x50, 0x2F, 0x71, 0x54, 0x3A, 0x83, 0x57, 0x44,
	0x8D, 0x59, 0x49, 0x92, 0x5A, 0x4B, 0x95, 0x5A, 0x4C, 0x98, 0x5B, 0x4D,
	0x9A, 0x5B, 0x4D, 0x9C, 0x5B, 0x4E, 0x9D, 0x5B, 0x4D, 0x9E, 0x5B, 0x4D,
	0x9E, 0x5A, 0x4D, 0x9F, 0x5A, 0x4C, 0x9F, 0x5A, 0x4B, 0x9F, 0x5A, 0x4B,
	0xA0, 0x59, 0x4A, 0xA0, 0x59, 0x4A, 0xA1, 0x59, 0x49, 0x00, 0x00, 0x00,
};

// .sdata
s16 kuri_kiss_r[] = {
	0x0000, 0xFFFF, 0xFFFA, 0xFFF5, 0xFFF2, 0xFFF1, 0xFFF5, 0x0000,
	0x000F, 0x0020, 0x0032, 0x0046, 0x005A, 0x006F, 0x0085, 0x009B,
	0x00B2, 0x00C9, 0x00E0, 0x00F7, 0x010E, 0x0125, 0x013C, 0x0152,
};

u32 kiss_col[] = {
	0xEB263400, 0xF56B7500, 0xDA000000,
};

static u32 color_rotation_data[] = {
	0xFEACACFE, 0xACD5FEB4, 0x9AD5B4FE, 0xB4B4FEB4, 0xDDFEB4FE,
	0xFEB4FED5, 0xB4FEB4D5, 0xFEB4FEFE, 0xB4FED5AC,
};

// This file's own .sbss2 (eff_kiss.s).
extern u32 unk_80423310;
extern u32 unk_80423314;
extern u32 unk_80423318;
extern u32 unk_8042331c;

// NONMATCHING (99.84%): all instructions are in place; MWCC pools the float literals into its own
// .sdata2 @NNN entries, so the sda21 displacement differs from the original's named float_* symbols.
// Using the named globals instead breaks the schedule (they are writable, so the compiler reloads them).
EffectEntry* effKissEntry(s32 type, f32 x, f32 y, f32 z, f32 angle) {
	EffectEntry* entry;
	KissEffectWork* data;
	s32 i;
	s32 count;
	f32 twoPi;

	entry = effEntry();
	count = 2;
	if (type == 5) {
		effStampN64Entry(2, x, y, z);
		count = 7;
	}
	entry->type = str_80421458;
	entry->count = count;
	data = __memAlloc(HEAP_EFFECT, sizeof(KissEffectWork) * entry->count);
	entry->userdata = data;
	entry->callback = effKissMain;

	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scaleY = 1.0f;
	data->scaleX = 1.0f;
	data->life = (type != 4) ? 40 : 60;
	data->frame = 0;
	twoPi = 6.2831855f * angle;
	data->alpha = 0xFF;
	data->angle = angle;
	data->rot = 0.0f;

	data++;
	for (i = 1; i < entry->count; i++, data++) {
		data->x = 0.0f;
		data->y = 0.0f;
		data->z = 0.0f;
		data->rot = 0.0f;
		data->life = 0;
		data->frame = 0;
		switch (type) {
		case 0:
		case 3:
			data->scaleY = 1.0f;
			data->scaleX = 1.0f;
			data->velX = 3.0f * (f32)sin(twoPi / 360.0f);
			data->velY = 3.0f * (f32)cos(twoPi / 360.0f);
			break;
		case 1:
		case 2:
			data->scaleY = 0.0f;
			data->scaleX = 0.0f;
			data->velX = 3.0f * (f32)sin(twoPi / 360.0f);
			data->velY = 3.0f * (f32)cos(twoPi / 360.0f);
			break;
		case 4:
			data->scaleY = 1.0f;
			data->scaleX = 1.0f;
			break;
		case 5:
			data->scaleY = 1.5f;
			data->scaleX = 1.5f;
			data->rot = 360.0f * (f32)i / (f32)(entry->count - 1);
			data->velX = 3.0f * (f32)sin(6.2831855f * data->rot / 360.0f);
			data->velY = 3.0f * (f32)cos(6.2831855f * data->rot / 360.0f);
			data->rot = data->rot * -1.0f;
			break;
		}
	}
	return entry;
}

// NONMATCHING (99.52%): every branch and call matches; left are the pooled .sdata2 relocation slots
// (anonymous @NNN vs the original's named float_*/double_to_int_* globals) and the rotated f2/f3/f0
// registers in the kuri/kura radius math. See REPORT.md.
void effKissMain(EffectEntry* effect) {
	KissEffectWork* data = effect->userdata;
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch and copies it to a second local before
	   dispCalcZ; a single local folds the copy away. */
	Vec position;
#endif
	Vec disp = vec3_802fe780;
	s32 type;
	s32 i;
	f32 angle;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
#ifdef MUST_MATCH
	position = disp;
#endif

	type = data->type;
	angle = data->angle;
	data->frame++;
	data->life--;
	if (type == 3) {
		data->frame = data->frame % 12;
	}

	if (data->life < 0) {
		if (type == 4) {
			EffectEntry* entry = effStardustN64Entry(1, data->x + data[1].x, data->y + data[1].y + 5.0f, data->z + data[1].z, 10.0f);
			effStardustN64SetScale(entry, 0.5f);
		}
		effDelete(effect);
		return;
	}

	if (type != 4 && data->life < 16) {
		data->alpha = data->life * 15;
	}

	data++;
	switch (type) {
	case 0:
	case 3: {
		f32 decay = 0.9f;
		for (i = 1; i < effect->count; i++, data++) {
			data->x += data->velX;
			data->y += data->velY;
			data->velX *= decay;
			data->velY *= decay;
		}
		break;
	}
	case 1: {
		f32 base = 6.2831855f * angle;
		for (i = 1; i < effect->count; i++, data++) {
			if ((u32)data->frame < 24) {
				f32 t = base / 360.0f;
				f32 radius = (f32)kuri_kiss[data->frame * 3] * 0.03125f * 1.2f;
				data->x = radius * (f32)sin(t);
				data->y = radius * (f32)cos(t);
				data->scaleX = (f32)kuri_kiss[data->frame * 3 + 1] * 0.03125f;
				data->scaleY = (f32)kuri_kiss[data->frame * 3 + 2] * 0.03125f;
				data->rot = (f32)kuri_kiss_r[data->frame];
				data->frame++;
			}
		}
		break;
	}
	case 2: {
		f32 base = 6.2831855f * angle;
		for (i = 1; i < effect->count; i++, data++) {
			if ((u32)data->frame < 39) {
				f32 radius = (f32)kura_kiss[data->frame * 3] * 0.03125f * 1.2f;
				f32 t = base / 360.0f;
				data->x = radius * (f32)sin(t);
				data->y = radius * (f32)cos(t);
				data->scaleX = (f32)kura_kiss[data->frame * 3 + 1] * 0.03125f;
				data->scaleY = (f32)kura_kiss[data->frame * 3 + 2] * 0.03125f;
				data->frame++;
			}
		}
		break;
	}
	case 4: {
		f32 s;
		data->x += 0.2f;
		s = (f32)sin(9.424778f * (f32)data->frame / 60.0f);
		data->y = 2.0f * fabsf(s);
		data->rot = 12.0f * (f32)sin(9.424778f * (f32)data->frame / 60.0f);
		data->frame++;
		break;
	}
	case 5: {
		f32 decay = 0.9f;
		for (i = 1; i < effect->count; i++, data++) {
			data->x += data->velX;
			data->y += data->velY;
			data->velX *= decay;
			data->velY *= decay;
		}
		break;
	}
	}

#ifdef MUST_MATCH
	if (type == 4 || type == 5) {
		dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effKissDisp2, effect);
	} else {
		dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effKissDisp, effect);
	}
#else
	if (type == 4 || type == 5) {
		dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effKissDisp2, effect);
	} else {
		dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effKissDisp, effect);
	}
#endif
}

// NONMATCHING (96.45%): structure/control flow/GX/MTX/vertex stream match; frame 0x1e0 and matrices at
// retail offsets. Remaining is register allocation only: cdata/cdata1/cdata2 in r25/r24/r23 (retail
// r29/r26/r25), so pass/elem/i in r27/r26/r22 (retail r22/r24/r23); angleDeg/deg f31/f30 swapped.
void effKissDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	KissEffectWork* data = effect->userdata;
	CameraEntry* camera;
	GXTexObj tex;
	Mtx mtxA;
	Mtx scl;
	Mtx rot;
	Mtx view;
	KissEffectWork* elem;
	u8* cdata;
	u8* cdata1;
	u8* cdata2;
	s32 pass;
	s32 i;
	s32 alpha;
	s32 type;
	s32 frameIdx;
	f32 angle;
	f32 angleDeg;
	f32 negAngleDeg;
	f32 deg;
	f32 offset;
	f32 rectW;
	f32 rectH;
	f32 texU;
	f32 texV;

	camera = camGetPtr(camId);
	frameIdx = data->frame * 3;
	alpha = data->alpha;
	angle = data->angle;
	type = data->type;

	PSMTXTrans(mtxA, data->x, data->y, data->z);
	PSMTXScale(scl, data->scaleX, data->scaleY, data->scaleY);
	PSMTXRotRad(rot, 'y', MTXDegToRad(-camGetPtr(CAMERA_3D)->viewYaw));
	PSMTXConcat(mtxA, rot, mtxA);
	PSMTXConcat(mtxA, scl, view);

	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, 0x7d);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	deg = float_deg2rad_804213fc;
	angleDeg = deg * angle;
	negAngleDeg = deg * -angle;
	cdata = (u8*)color_rotation_data + frameIdx;
	cdata1 = cdata + 1;
	cdata2 = cdata + 2;

	for (pass = 0; pass < 2; pass++) {
		elem = effect->userdata;
		elem++;
		for (i = 1; i < effect->count; i++, elem++) {
			offset = 0.0f;
			switch (type) {
			case 0:
			case 1:
			case 2:
				effGetTexObj(0x14, &tex);
				GXLoadTexObj(&tex, GX_TEXMAP0);
				if (pass == 0) {
					GXColor color = *(GXColor*)&unk_80423310;
					color.a = alpha / 2;
					GXSetChanMatColor(GX_COLOR0A0, color);
					offset = 1.5f;
				} else {
					((u8*)&kiss_col[type])[3] = alpha;
					GXSetChanMatColor(GX_COLOR0A0, *(GXColor*)&kiss_col[type]);
				}
				rectH = 8.0f;
				texU = 2.0f;
				rectW = 8.0f;
				texV = 1.0f;
				break;
			case 3:
				if (pass == 0) {
					GXColor color;
					effGetTexObj(0x59, &tex);
					GXLoadTexObj(&tex, GX_TEXMAP0);
					color = *(GXColor*)&unk_80423314;
					color.r = cdata[0];
					color.g = cdata1[0];
					color.b = cdata2[0];
					color.a = alpha;
					GXSetChanMatColor(GX_COLOR0A0, color);
					rectW = 32.0f;
					rectH = 40.0f;
				} else {
					GXColor color;
					effGetTexObj(0x58, &tex);
					GXLoadTexObj(&tex, GX_TEXMAP0);
					color = *(GXColor*)&str_804213F0;
					color.a = alpha;
					GXSetChanMatColor(GX_COLOR0A0, color);
					rectW = 20.0f;
					rectH = 32.0f;
				}
				texV = 1.0f;
				texU = texV;
				rectW *= 0.5f;
				rectH *= 0.5f;
				break;
			default:
				break;
			}

			PSMTXTrans(mtxA, elem->x + offset, elem->y - offset, elem->z);
			PSMTXRotRad(rot, 'z', negAngleDeg);
			PSMTXConcat(mtxA, rot, mtxA);
			PSMTXRotRad(rot, 'y', deg * elem->rot);
			PSMTXConcat(mtxA, rot, mtxA);
			if (type == 3) {
				PSMTXRotRad(rot, 'z', angleDeg);
				PSMTXConcat(mtxA, rot, mtxA);
			}
			PSMTXScale(scl, elem->scaleX, elem->scaleY, elem->scaleY);
			PSMTXConcat(mtxA, scl, mtxA);
			if (type == 3) {
				PSMTXTrans(scl, 0.0f, 5.0f, 0.0f);
				PSMTXConcat(mtxA, scl, mtxA);
			}
			PSMTXConcat(view, mtxA, mtxA);
			PSMTXConcat(camera->view, mtxA, mtxA);
			GXLoadPosMtxImm(mtxA, 0);
			GXSetCurrentMtx(0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			if (type == 3) {
				GXPosition3f32(-rectW * 0.5f, rectH * 0.5f, 0.0f);
				GXTexCoord2f32(0.0f, 0.0f);
				GXPosition3f32(rectW * 0.5f, rectH * 0.5f, 0.0f);
				GXTexCoord2f32(texU, 0.0f);
				GXPosition3f32(rectW * 0.5f, -rectH * 0.5f, 0.0f);
				GXTexCoord2f32(texU, texV);
				GXPosition3f32(-rectW * 0.5f, -rectH * 0.5f, 0.0f);
				GXTexCoord2f32(0.0f, texV);
			} else {
				GXPosition3f32(-rectW * 0.5f, rectH, 0.0f);
				GXTexCoord2f32(0.0f, 0.0f);
				GXPosition3f32(rectW * 0.5f, rectH, 0.0f);
				GXTexCoord2f32(texU, 0.0f);
				GXPosition3f32(rectW * 0.5f, 0.0f, 0.0f);
				GXTexCoord2f32(texU, texV);
				GXPosition3f32(-rectW * 0.5f, 0.0f, 0.0f);
				GXTexCoord2f32(0.0f, texV);
			}
		}
	}

	if (type == 0 || type == 1 || type == 3) {
		deg = 1.15f;
		texV = 0.5f;
		for (i = 0; i < 2; i++) {
			offset = 0.0f;
			elem = effect->userdata;
			switch (type) {
			case 0:
			case 1:
			case 2:
				effGetTexObj(0x42, &tex);
				GXLoadTexObj(&tex, GX_TEXMAP0);
				if (i == 0) {
					GXColor color = *(GXColor*)&unk_80423318;
					color.a = alpha / 2;
					GXSetChanMatColor(GX_COLOR0A0, color);
					offset = 1.5f;
				} else {
					((u8*)&kiss_col[type])[3] = alpha;
					GXSetChanMatColor(GX_COLOR0A0, *(GXColor*)&kiss_col[type]);
				}
				break;
			case 3:
				if (i == 0) {
					GXColor color;
					effGetTexObj(0x5a, &tex);
					GXLoadTexObj(&tex, GX_TEXMAP0);
					color = *(GXColor*)&unk_8042331c;
					color.r = cdata[0];
					color.g = cdata1[0];
					color.b = cdata2[0];
					color.a = alpha;
					GXSetChanMatColor(GX_COLOR0A0, color);
				} else {
					GXColor color;
					effGetTexObj(0x42, &tex);
					GXLoadTexObj(&tex, GX_TEXMAP0);
					color = *(GXColor*)&str_804213F4;
					color.a = alpha;
					GXSetChanMatColor(GX_COLOR0A0, color);
				}
				break;
			default:
				break;
			}

			PSMTXTrans(mtxA, offset, -offset, 0.0f);
			PSMTXRotRad(rot, 'z', negAngleDeg);
			PSMTXConcat(rot, mtxA, mtxA);
			PSMTXConcat(view, mtxA, mtxA);
			PSMTXConcat(camera->view, mtxA, mtxA);
			GXLoadPosMtxImm(mtxA, 0);
			GXSetCurrentMtx(0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			rectH = deg * elem[1].y;
			texU = 1.0f;
			if (type == 3 && i == 0) {
				texU = 2.0f;
			}
			GXPosition3f32(-texU * 0.5f, rectH, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(texU * 0.5f, rectH, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);
			GXPosition3f32(texU * 0.5f, 0.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);
			GXPosition3f32(-texU * 0.5f, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
		}
	}
}

#ifdef MUST_MATCH
// MUST_MATCH: building the GXColor through an inline return puts the colour temp where retail keeps it.
static inline GXColor effKissDisp2Color(u8 alpha) {
	GXColor c = *(GXColor*)&dat_804213f8;
	c.a = alpha;
	return c;
}
#endif

#pragma opt_common_subs off // MUST_MATCH: keep the .sdata2 vertex constants' individual loads

// 100% matched. `#pragma opt_common_subs off` stops MWCC from CSE-ing the vertex .sdata2
// constants (retail reloads each one before every FIFO store); the colour inline puts the by-value
// GXColor temp in retail's slot; splitting the yaw angle into two statements lands the negate and
// multiply in retail's f1/f0; declaring i before alpha and deg before eight fixes the GPR/FPR picks.
void effKissDisp2(CameraId camId, void* param) {
	EffectEntry* effect = param;
	KissEffectWork* data = effect->userdata;
	CameraEntry* cam = camGetPtr(camId);
	CameraEntry* cam3d;
	GXColor color;
	Mtx trans;
	Mtx scale;
	Mtx rot;
	Mtx local;
	GXTexObj texObj;
	f32 deg;
	f32 eight;
	f32 negfour;
	f32 four;
	f32 ang;
	s32 i;
	s32 alpha;

	alpha = data->alpha;

	PSMTXTrans(trans, data->x, data->y, data->z);
	PSMTXScale(scale, data->scaleX, data->scaleY, data->scaleY);
	cam3d = camGetPtr(CAMERA_3D);
	ang = -cam3d->viewYaw;
	ang = float_deg2rad_804213fc * ang;
	PSMTXRotRad(rot, 'y', ang);
	PSMTXConcat(trans, rot, trans);
	PSMTXConcat(trans, scale, local);

	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C, GX_FALSE, 0x7D);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	effGetTexObj(0x14, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
#ifdef MUST_MATCH
	color = effKissDisp2Color(alpha);
#else
	color = *(GXColor*)&dat_804213f8;
	color.a = alpha;
#endif
	GXSetChanMatColor(GX_COLOR0A0, color);

	eight = float_8_80421400;
	deg = float_deg2rad_804213fc;
	negfour = -eight * float_0p5_80421408;
	four = float_4_80421410;
	data++;
	for (i = 1; i < effect->count; i++, data++) {
		PSMTXTrans(trans, data->x, data->y, data->z);
		PSMTXScale(scale, data->scaleX, data->scaleY, float_1_80421404);
		PSMTXRotRad(rot, 'z', deg * data->rot);
		PSMTXConcat(trans, scale, trans);
		PSMTXConcat(trans, rot, trans);
		PSMTXConcat(local, trans, trans);
		PSMTXConcat(cam->view, trans, trans);
		GXLoadPosMtxImm(trans, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(negfour, eight, float_0_8042140c);
		GXTexCoord2f32(float_0_8042140c, float_0_8042140c);
		GXPosition3f32(four, eight, float_0_8042140c);
		GXTexCoord2f32(float_2_80421414, float_0_8042140c);
		GXPosition3f32(four, float_0_8042140c, float_0_8042140c);
		GXTexCoord2f32(float_2_80421414, float_1_80421404);
		GXPosition3f32(negfour, float_0_8042140c, float_0_8042140c);
		GXTexCoord2f32(float_0_8042140c, float_1_80421404);
	}
}
