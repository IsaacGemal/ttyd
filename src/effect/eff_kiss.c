#include "eff/eff_kiss.h"
#include "driver/dispdrv.h"
#include "eff/n64/eff_stamp_n64.h"
#include "eff/n64/eff_stardust_n64.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "math_ext.h"

// .sdata2: the colour words. The float constants are literals at each use; MWCC pools them after these
// in code-generation order (effKissDisp2 first), which is retail's .sdata2 order.
#ifdef MUST_MATCH
#define SDATA2_CONST __declspec(section ".sdata2")
#else
#define SDATA2_CONST const
#endif
SDATA2_CONST char str_804213F0[4] = "<<<";
SDATA2_CONST char str_804213F4[4] = "<<<";
SDATA2_CONST u32 dat_804213f8 = 0xFFB5DA00;

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

#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".data") u32 gap_05_803B01F8_data[2] = { 0, 0 };
#pragma force_active reset
#pragma explicit_zero_data reset

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

// .sbss2: four zeroed GXColor templates, copied before each draw: 0x80423310/18 keep their black RGB and get
// only an alpha, 0x80423314/1c get RGB from color_rotation_data as well.
// Declared in reverse: MWCC places them in reverse of declaration order.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") u32 unk_8042331c;
__declspec(section ".sdata") u32 unk_80423318;
__declspec(section ".sdata") u32 unk_80423314;
__declspec(section ".sdata") u32 unk_80423310;
#pragma section ".sdata" ".sbss"

// 100% matched. `t` splits the shared twoPi/360 division out of the
// sin/cos arguments so it is computed once, as retail does. In case 5 the `negOne` local hoists the
// -1.0f load into retail's f0.
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
	entry->type = "kiss";
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
			{
				f32 t = twoPi / 360.0f;
				data->velX = 3.0f * (f32)sin(t);
				data->velY = 3.0f * (f32)cos(t);
			}
			break;
		case 1:
		case 2:
			data->scaleY = 0.0f;
			data->scaleX = 0.0f;
			{
				f32 t = twoPi / 360.0f;
				data->velX = 3.0f * (f32)sin(t);
				data->velY = 3.0f * (f32)cos(t);
			}
			break;
		case 4:
			data->scaleY = 1.0f;
			data->scaleX = 1.0f;
			break;
		case 5:
		{
			/* the local hoists the -1.0f load into retail's f0 */
			f32 negOne = -1.0f;
			data->scaleY = 1.5f;
			data->scaleX = 1.5f;
			data->rot = 360.0f * (f32)i / (f32)(entry->count - 1);
			data->velX = 3.0f * (f32)sin(6.2831855f * data->rot / 360.0f);
			data->velY = 3.0f * (f32)cos(6.2831855f * data->rot / 360.0f);
			data->rot = data->rot * negOne;
			break;
		}
		}
	}
	return entry;
}

// 100% matched. The u8 table readers write the *0.03125f as a /32.0f division: the
// constant folds to the same float and the instruction stream is identical, but the division makes the
// allocator keep the 2^52 mask, the 0.03125 reload and the u8->f32 conversion temp in retail's f4/f2/f3
// (rather than the *0.03125f form's f3/f4/f2, which coalesces the conversion into the result).
// `base`/`t` are function-scope so the two spiral cases share retail's f31 base / f30 t.
void effKissMain(EffectEntry* effect) {
	KissEffectWork* data = effect->userdata;
	/* retail builds the Vec in a scratch and copies it to a second local before
	   dispCalcZ; a single local folds the copy away. */
	Vec position;
	Vec disp = (Vec){ 0.0f, 0.0f, 0.0f };
	s32 type;
	s32 i;
	f32 angle;
	f32 base;
	f32 t;

	disp.x = data->x;
	disp.y = data->y;
	disp.z = data->z;
	position = disp;

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
		base = 6.2831855f * angle;
		for (i = 1; i < effect->count; i++, data++) {
			if ((u32)data->frame < 24) {
				/* the /32.0f division is the register-allocation nudge */
				f32 radius = (f32)kuri_kiss[data->frame * 3] / 32.0f;
				radius *= 1.2f;
				t = base / 360.0f;
				data->x = radius * (f32)sin(t);
				data->y = radius * (f32)cos(t);
				/* same /32 division nudge; the constant folds to the same float */
				data->scaleX = (f32)kuri_kiss[data->frame * 3 + 1] / 32.0f;
				data->scaleY = (f32)kuri_kiss[data->frame * 3 + 2] / 32.0f;
				data->rot = (f32)kuri_kiss_r[data->frame];
				data->frame++;
			}
		}
		break;
	}
	case 2: {
		base = 6.2831855f * angle;
		for (i = 1; i < effect->count; i++, data++) {
			if ((u32)data->frame < 39) {
				/* the /32.0f division is the register-allocation nudge */
				f32 radius = (f32)kura_kiss[data->frame * 3] / 32.0f;
				radius *= 1.2f;
				t = base / 360.0f;
				data->x = radius * (f32)sin(t);
				data->y = radius * (f32)cos(t);
				/* same /32 division nudge as case 1 */
				data->scaleX = (f32)kura_kiss[data->frame * 3 + 1] / 32.0f;
				data->scaleY = (f32)kura_kiss[data->frame * 3 + 2] / 32.0f;
				data->frame++;
			}
		}
		break;
	}
	case 4: {
		f32 s;
		data->x += 0.2f;
		s = (f32)sin(9.424778f * (f32)data->frame / 60.0f);
		data->y = 2.0f * __fabsf(s);
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

	if (type == 4 || type == 5) {
		dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effKissDisp2, effect);
	} else {
		dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effKissDisp, effect);
	}
}

/* Building the colour inside the helper that issues GXSetChanMatColor (instead of
   returning it and passing it by value) puts the helper's colour local directly below retail's
   by-value argument copy, per call. eff_teresa.c/effThunderDisp use the same idiom. */
static inline void effKissChanAlpha(const void* src, s32 a) {
	GXColor color = *(const GXColor*)src;
	color.a = a;
	GXSetChanMatColor(GX_COLOR0A0, color);
}

static inline void effKissChanKiss(s32 idx, s32 a) {
	((u8*)&kiss_col[idx])[3] = a;
	GXSetChanMatColor(GX_COLOR0A0, *(GXColor*)&kiss_col[idx]);
}

static inline void effKissChanRot(const void* src, const u8* rgb, s32 a) {
	GXColor color = *(const GXColor*)src;
	color.r = rgb[0];
	color.g = rgb[1];
	color.b = rgb[2];
	color.a = a;
	GXSetChanMatColor(GX_COLOR0A0, color);
}

// Building each GXColor inside the helper that calls GXSetChanMatColor (rather than returning it and
// letting the caller pass it by value) reproduces retail's stack layout: the helper's colour local lands
// directly below the by-value argument copy, per call (eff_teresa.c/effThunderDisp use the same idiom).
// The final switch's two extra branches are reproduced by the MUST_MATCH goto form below (the original
// reaches the shared case body through gotos and a `default: return`).
// 100% matched. The deg-to-rad factor is a 0.017453292f literal at each use: MWCC then hoists
// `0.017453292f * angle` out of the loop itself, which gives retail's FPRs and preheader schedule.
void effKissDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	KissEffectWork* data = effect->userdata;
	CameraEntry* camera;
	GXTexObj tex;
	Mtx mtxA;
	Mtx scl;
	Mtx rot;
	Mtx view;
	s32 i;
	s32 pass;
	s32 alpha;
	s32 type;
	s32 frameIdx;
	f32 angle;
	f32 negAngleDeg;
	f32 heightScale;
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
	PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(mtxA, rot, mtxA);
	PSMTXConcat(mtxA, scl, view);

	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
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

	negAngleDeg = 0.017453292f * -angle;

	for (pass = 0; pass < 2; pass++) {
		data = effect->userdata;
		data++;
		for (i = 1; i < effect->count; i++, data++) {
			offset = 0.0f;
			switch (type) {
			case 0:
			case 1:
			case 2:
				effGetTexObj(0x14, &tex);
				GXLoadTexObj(&tex, GX_TEXMAP0);
				if (pass == 0) {
					offset = 1.5f;
					effKissChanAlpha(&unk_80423310, alpha / 2);
				} else {
					effKissChanKiss(type, alpha);
				}
				rectH = 8.0f;
				texU = 2.0f;
				rectW = 8.0f;
				texV = 1.0f;
				break;
			case 3:
				if (pass == 0) {
					effGetTexObj(0x59, &tex);
					GXLoadTexObj(&tex, GX_TEXMAP0);
					effKissChanRot(&unk_80423314, (const u8*)color_rotation_data + frameIdx, alpha);
					rectW = 32.0f;
					rectH = 40.0f;
				} else {
					effGetTexObj(0x58, &tex);
					GXLoadTexObj(&tex, GX_TEXMAP0);
					effKissChanAlpha(&str_804213F0, alpha);
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

			PSMTXTrans(mtxA, data->x + offset, data->y - offset, data->z);
			PSMTXRotRad(rot, 'z', negAngleDeg);
			PSMTXConcat(mtxA, rot, mtxA);
			PSMTXRotRad(rot, 'y', 0.017453292f * data->rot);
			PSMTXConcat(mtxA, rot, mtxA);
			if (type == 3) {
				PSMTXRotRad(rot, 'z', 0.017453292f * angle);
				PSMTXConcat(mtxA, rot, mtxA);
			}
			PSMTXScale(scl, data->scaleX, data->scaleY, data->scaleY);
			PSMTXConcat(mtxA, scl, mtxA);
			if (type == 3) {
				PSMTXTrans(scl, 0.0f, 5.0f, 0.0f);
				PSMTXConcat(mtxA, scl, mtxA);
			}
			PSMTXConcat(view, mtxA, mtxA);
			PSMTXConcat(camera->view, mtxA, mtxA);
			GXLoadPosMtxImm(mtxA, GX_PNMTX0);
			GXSetCurrentMtx(GX_PNMTX0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			if (type == 3) {
				// a named 0.5f local makes MWCC emit rectW/rectH as the first fmuls operand like retail.
				f32 h = 0.5f;
				GXPosition3f32(-rectW * h, rectH * h, 0.0f);
				GXTexCoord2f32(0.0f, 0.0f);
				GXPosition3f32(rectW * h, rectH * h, 0.0f);
				GXTexCoord2f32(texU, 0.0f);
				GXPosition3f32(rectW * h, -rectH * h, 0.0f);
				GXTexCoord2f32(texU, texV);
				GXPosition3f32(-rectW * h, -rectH * h, 0.0f);
				GXTexCoord2f32(0.0f, texV);
			} else {
				// same as the type-3 branch: keeps rectW first in the fmuls.
				f32 h = 0.5f;
				GXPosition3f32(-rectW * h, rectH, 0.0f);
				GXTexCoord2f32(0.0f, 0.0f);
				GXPosition3f32(rectW * h, rectH, 0.0f);
				GXTexCoord2f32(texU, 0.0f);
				GXPosition3f32(rectW * h, 0.0f, 0.0f);
				GXTexCoord2f32(texU, texV);
				GXPosition3f32(-rectW * h, 0.0f, 0.0f);
				GXTexCoord2f32(0.0f, texV);
			}
		}
	}

	switch (type) {
	case 0:
	case 1:
	case 3:
		break;
	default:
		return;
	}

	heightScale = 1.15f;
	texV = 0.5f;
	for (i = 0; i < 2; i++) {
		// declared per iteration: a separate offset keeps its register different from the first loop's.
		f32 offset = 0.0f;
		data = effect->userdata;
		switch (type) {
		case 0:
		case 1:
		case 2:
			effGetTexObj(0x42, &tex);
			GXLoadTexObj(&tex, GX_TEXMAP0);
			if (i == 0) {
				offset = 1.5f;
				effKissChanAlpha(&unk_80423318, alpha / 2);
			} else {
				effKissChanKiss(type, alpha);
			}
			break;
		case 3:
			if (i == 0) {
				effGetTexObj(0x5a, &tex);
				GXLoadTexObj(&tex, GX_TEXMAP0);
				effKissChanRot(&unk_8042331c, (const u8*)color_rotation_data + frameIdx, alpha);
			} else {
				effGetTexObj(0x42, &tex);
				GXLoadTexObj(&tex, GX_TEXMAP0);
				effKissChanAlpha(&str_804213F4, alpha);
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
		GXLoadPosMtxImm(mtxA, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		{
			// loop-local h/w keep the vertex temps in retail's scratch FPRs and texV supplies 0.5.
			f32 h = heightScale * data[1].y;
			f32 w = 1.0f;
			if (type == 3 && i == 0) {
				w = 2.0f;
			}
			GXPosition3f32(-w * texV, h, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(w * texV, h, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);
			GXPosition3f32(w * texV, 0.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);
			GXPosition3f32(-w * texV, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
		}
	}
}

// building the GXColor through an inline return puts the colour temp where retail keeps it.
static inline GXColor effKissDisp2Color(u8 alpha) {
	GXColor c = *(GXColor*)&dat_804213f8;
	c.a = alpha;
	return c;
}

// 100% matched. The colour inline puts the by-value GXColor temp in retail's slot; the vertex half-width
// is `eight / 2.0f` in the loop (MWCC hoists it); splitting the yaw angle into two statements lands the negate and
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
	f32 ang;
	s32 i;
	s32 alpha;

	alpha = data->alpha;

	PSMTXTrans(trans, data->x, data->y, data->z);
	PSMTXScale(scale, data->scaleX, data->scaleY, data->scaleY);
	cam3d = camGetPtr(CAMERA_3D);
	ang = -cam3d->viewYaw;
	ang = 0.017453292f * ang;
	PSMTXRotRad(rot, 'y', ang);
	PSMTXConcat(trans, rot, trans);
	PSMTXConcat(trans, scale, local);

	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
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
	color = effKissDisp2Color(alpha);
	GXSetChanMatColor(GX_COLOR0A0, color);

	eight = 8.0f;
	deg = 0.017453292f;
	data++;
	for (i = 1; i < effect->count; i++, data++) {
		PSMTXTrans(trans, data->x, data->y, data->z);
		PSMTXScale(scale, data->scaleX, data->scaleY, 1.0f);
		PSMTXRotRad(rot, 'z', deg * data->rot);
		PSMTXConcat(trans, scale, trans);
		PSMTXConcat(trans, rot, trans);
		PSMTXConcat(local, trans, trans);
		PSMTXConcat(cam->view, trans, trans);
		GXLoadPosMtxImm(trans, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(-eight / 2.0f, eight, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(eight / 2.0f, eight, 0.0f);
		GXTexCoord2f32(2.0f, 0.0f);
		GXPosition3f32(eight / 2.0f, 0.0f, 0.0f);
		GXTexCoord2f32(2.0f, 1.0f);
		GXPosition3f32(-eight / 2.0f, 0.0f, 0.0f);
		GXTexCoord2f32(0.0f, 1.0f);
	}
}
