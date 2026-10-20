#include "eff/n64/eff_radiation_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "system.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sdata vertex array (0x2C = 11 words); the trailing zero word is the section gap.
#pragma force_active on
__declspec(section ".sdata") u32 size300x40_tex32x32[0x2C / 4] = {
    0x00000000, 0x00000010, 0x0000FFFF, 0xFFFF012C, 0x00140000,
    0x00000400, 0xEBFFF500, 0x012CFFEC, 0x00000400, 0x0400EBFF, 0xF5000000,
};
#pragma explicit_zero_data on
__declspec(section ".sdata") u32 gap_07_80413CCC_sdata = 0;
#pragma explicit_zero_data reset
#pragma force_active reset

#pragma force_active on
__declspec(section ".data") u32 size150x20_tex32x32[0x38 / 4] = {
    0x0000000A, 0x00000000, 0x0000FFFF, 0xFFFF0000, 0xFFF60000, 0x04000000, 0xFFFFFFFF,
    0x0096FFF6, 0x00000400, 0x0400FFFF, 0xFFFF0096, 0x000A0000, 0x00000400, 0xFFFFFFFF,
};
__declspec(section ".data") s32 r_data[0x140 / 4] = {
    0x00000000, 0xFFFFFFEC, 0x00000000, 0x00000001, 0x00000012,
    0xFFFFFFF6, 0xFFFFFFC4, 0x0000002D, 0xFFFFFFFF, 0x00000004,
    0xFFFFFFEC, 0xFFFFFFEC, 0x0000005A, 0x00000001, 0x00000001,
    0xFFFFFFF6, 0xFFFFFFC4, 0x00000087, 0x00000001, 0xFFFFFFFC,
    0x00000000, 0xFFFFFFEC, 0x000000B4, 0xFFFFFFF8, 0x00000001,
    0xFFFFFFF6, 0xFFFFFFC4, 0x000000E1, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFEC, 0xFFFFFFEC, 0x0000010E, 0x00000001, 0xFFFFFFF7,
    0xFFFFFFF6, 0xFFFFFFC4, 0x0000013B, 0x00000004, 0x00000004,
    0x00000000, 0x0000000A, 0x00000016, 0x00000004, 0x00000001,
    0xFFFFFFF6, 0x00000014, 0x00000043, 0x00000001, 0xFFFFFFFF,
    0xFFFFFFEC, 0x0000000A, 0x00000070, 0xFFFFFFFF, 0xFFFFFFFF,
    0xFFFFFFF6, 0x00000014, 0x0000009D, 0xFFFFFFF8, 0x00000001,
    0x00000000, 0x0000000A, 0x000000CA, 0x00000001, 0xFFFFFFFC,
    0xFFFFFFF6, 0x00000014, 0x000000F7, 0x00000001, 0xFFFFFFEE,
    0xFFFFFFEC, 0x0000000A, 0x00000124, 0xFFFFFFFF, 0x00000009,
    0xFFFFFFF6, 0x00000014, 0x00000151, 0x00000001, 0x00000001,
};
#pragma force_active reset

// Local copy of system.c's inline helper so it inlines here (as in eff_guruguru.c).
inline Vec initializeVec(Vec* input) {
	Vec temp = { 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

// The declaration order `row, n, i, flag, count` and the `n += 2, i++` update order are what
// match the original's flag/type (r24/r25) and n/i (r9/r10) register picks. With dead-assignment
// elimination off, the table row's post-increments keep the original's index chain (row, +1, +1,
// then +2 for the fifth word) instead of folding it into the load's displacement.
#pragma opt_dead_assignments off
EffectEntry* effRadiationN64Entry(s32 type, f32 x, f32 y, f32 z, f32 a5) {
	EffectEntry* effect;
	EffRadiationData* data;
	s32 row;
	s32 n;
	s32 i;
	s32 k;
	s32 flag;
	s32 count;

	effect = effEntry();
	flag = 0;
	if (type >= 10) {
		flag = 1;
		type %= 10;
	}

	count = 17;
	if (type <= 1) {
		count = 9;
	}

#ifdef MUST_MATCH
	// str_802FB490: the original object is 16 bytes, i.e. the 12-character text plus the
	// terminator plus three zero pad bytes, so the literal spells the pad out.
	effect->type = "RadiationN64\0\0\0";
#else
	effect->type = "RadiationN64";
#endif
	effect->count = count;
	data = __memAlloc(HEAP_EFFECT, count * sizeof(EffRadiationData));
	effect->userdata = data;
	effect->callback = effRadiationMain;
	effect->flags |= 2;

	data->type = type;
	data->age = 0;
	data->life = 100;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x20 = a5;
	data->field_0x1C = a5;
	data->noDepth = flag;
	data->camId = CAMERA_3D;

	switch (type) {
	case 0:
	case 1:
		data++;
		for (k = 1; k < effect->count; k++, data++) {
			data->startOffset = (f32)((k - 1) * 30 / (count - 1) - 10);
			data->lifetime = 50 - (k - 1) * 20 / (count - 1);
			data->speed = 2.5f + 25.0f / (f32)data->lifetime;
			data->startRotX = (f32)(s32)effTblRandN64(180, k * 3);
			data->startRotY = (f32)(s32)effTblRandN64(180, k * 3 + 1);
			data->startRotZ = (f32)(s32)effTblRandN64(180, k * 3 + 2);
			data->offset = data->startOffset;
			data->life = data->lifetime;
			data->offsetVel = data->speed;
			data->rotX = data->startRotX;
			data->rotY = data->startRotY;
			data->rotZ = data->startRotZ;
			data->alpha = 0;
			data->age = 0;
		}
		break;
	default:
		data++;
		for (i = 1, n = 32; i < effect->count; n += 2, i++, data++) {
			data->offset = 0.0f;
			data->life = n;
			row = (i - 1) * 5;
			data->rotX = (f32)r_data[row++];
			data->rotY = (f32)r_data[row++];
			data->rotZ = (f32)r_data[row++];
			data->alpha = 0;
			data->age = 0;
			data->field_0x38 = 0.0f;
			data->field_0x34 = 0.0f;
			data->field_0x48 = 0.0f;
			data->field_0x4C = -0.6f;
			data->field_0x3C = 10.0f;
			data->field_0x40 = 0.0f;
			data->field_0x50 = 0.0f;
			data->field_0x54 = 0.6f;
			data->rotXVel = 0.0f;
			data->rotYVel = 0.0f;
			data->rotZVel = (f32)r_data[++row] * 0.1f;
			data->holdTimer = 0;
		}
		break;
	}

	return effect;
}

// The default loop reads its table row with the same post-increments as effRadiationN64Entry (still
// under opt_dead_assignments off). The countdown skips the rest of the iteration while it is
// running, which is what lets the `<= 0` path jump straight to the position update.
void effRadiationMain(EffectEntry* effect) {
	s32 n1;
	s32 n2;
	s32 n;
	Vec position;
	EffRadiationData* data;
	s32 i;
	s32 type;
	s32 life;
	s32 row;
	s32 counter;

	data = effect->userdata;
	position = initializeVec(&data->unsetPos);

	type = data->type;
	if (data->life <= 10) {
		data->life--;
	}
	data->age++;
	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->life = 10;
	}
	life = data->life;
	if (life < 0) {
		effDelete(effect);
		return;
	}
	counter = data->age;

	switch (type) {
	case 0:
	case 1: {
		n = (counter * 10 + 1) * 3;
		n1 = n + 1;
		n2 = n + 2;
		data++;
		for (i = 1; i < effect->count; i++, data++) {
			data->life--;
			data->age++;
			if (data->age < 10) {
				data->alpha = data->age * 12;
			}
			if (data->life < 5) {
				data->alpha = data->life * 25;
			}
			data->offset += data->offsetVel;
			if (data->life <= 0) {
				data->startRotX = (f32)(s32)effTblRandN64(180, n);
				data->startRotY = (f32)(s32)effTblRandN64(180, n1);
				data->startRotZ = (f32)(s32)effTblRandN64(180, n2);
				data->offset = data->startOffset;
				data->life = data->lifetime;
				data->offsetVel = data->speed;
				data->rotX = data->startRotX;
				data->rotY = data->startRotY;
				data->rotZ = data->startRotZ;
				data->alpha = 0;
				data->age = 0;
			}
		}
		break;
	}
	default: {
		i = 1;
		n = 32;
		data++;
		for (; i < effect->count; n += 2, i++, data++) {
			data->field_0x34 += data->field_0x48;
			if (data->field_0x34 > 256.0f) {
				data->field_0x34 -= 256.0f;
			}
			if (data->field_0x34 < 0.0f) {
				data->field_0x34 += 256.0f;
			}
			data->field_0x3C += data->field_0x50;
			if (data->field_0x3C > 256.0f) {
				data->field_0x3C -= 256.0f;
			}
			if (data->field_0x3C < 0.0f) {
				data->field_0x3C += 256.0f;
			}
			data->field_0x38 += data->field_0x4C;
			if (data->field_0x38 > 256.0f) {
				data->field_0x38 -= 256.0f;
			}
			if (data->field_0x38 < 0.0f) {
				data->field_0x38 += 256.0f;
			}
			data->field_0x40 += data->field_0x54;
			if (data->field_0x40 > 256.0f) {
				data->field_0x40 -= 256.0f;
			}
			if (data->field_0x40 < 0.0f) {
				data->field_0x40 += 256.0f;
			}
			data->life--;
			data->age++;
			if (data->age < 5) {
				data->alpha = data->age * 50;
			}
			if (data->life < 5) {
				data->alpha = data->life * 50;
			}
			if (life < 10) {
				data->alpha = data->alpha * life / 10;
			}
			if (data->life <= 0) {
				data->offset = 0.0f;
				data->life = n;
				row = (i - 1) * 5;
				data->rotX = (f32)r_data[row++];
				data->rotY = (f32)r_data[row++];
				data->rotZ = (f32)r_data[row++];
				data->alpha = 0;
				data->age = 0;
				data->field_0x38 = 0.0f;
				data->field_0x34 = 0.0f;
				data->field_0x48 = 0.0f;
				data->field_0x4C = -0.6f;
				data->field_0x3C = 10.0f;
				data->field_0x40 = 0.0f;
				data->field_0x50 = 0.0f;
				data->field_0x54 = 0.6f;
				data->rotXVel = 0.0f;
				data->rotYVel = 0.0f;
				data->rotZVel = (f32)r_data[++row] * 0.1f;
				data->holdTimer = 0;
			}
			if (data->holdTimer > 0) {
				data->holdTimer--;
				if (data->holdTimer > 0) {
					continue;
				}
			}
			data->rotX += data->rotXVel;
			data->rotY += data->rotYVel;
			data->rotZ += data->rotZVel;
		}
		break;
	}
	}

	{
		EffRadiationData* data2 = effect->userdata;

		if (data2->noDepth == 0) {
			dispEntry(data2->camId, dispCalcZ(&position), 2, effRadiationDisp, effect);
		} else {
			dispEntry(data2->camId, dispCalcZ(&position), 8, effRadiationDisp, effect);
		}
	}
}
#pragma opt_dead_assignments reset

// .sdata2/.rodata constants and .data/.sdata vertex arrays used by effRadiationDisp
// (original symbols from config/G8MJ01/symbols.txt).
extern const GXColor dat_8041fa90;
extern const GXColor dat_8041fa94;
extern u32 size150x20_tex32x32[14];
extern u32 size300x40_tex32x32[11];

// MUST_MATCH: building the colour in an inline is what makes the two call sites keep
// their own GXColor temporary (0x10/0x14 and 0x8/0xc); the natural assignment reuses one slot.
#ifdef MUST_MATCH
static inline GXColor radiationSetColor(s32 alpha, GXColor color) {
	color.a = alpha;
	return color;
}
#endif

// The retail tri1 keeps a 4th, unused argument (eff_balloon_n64.s passes 0/1 there), but
// include/eff/eff_n64.h and eff_n64.c's definition only declare 3. This caller passes 0.
#ifdef MUST_MATCH
static inline void tri1WithUnused(u16 x, u16 y, u16 z, s32 unused) {
	((void (*)(u16, u16, u16, s32))tri1)(x, y, z, unused);
}
#endif

void effRadiationDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	EffRadiationData* data;
	CameraEntry* cam;
	Mtx trans;
	Mtx scale;
	Mtx rot;
	Mtx work;
	Mtx trans2;
	GXTexObj texObj;
	effVtxArray* vtx;
	f32 alpha;
	f32 scaleY;
	f32 scaleX;
	f32 scaleZ;
	s32 i;
	s32 type;

	cam = camGetPtr(camId);
	data = effect->userdata;
	type = data->type;

	if (data->noDepth != 0) {
		GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
		GXSetZCompLoc(GX_TRUE);
		GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
		GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
	}
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	switch (type) {
	case 0:
	case 1:
		GXSetNumTevStages(1);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C0, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
		effGetTexObjN64(0x2c, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		GXSetNumTexGens(1);
		GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
		PSMTXScale(scale, 0.03125f, 0.03125f, 0.0f);
		GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
		break;
	default:
		GXSetNumTevStages(1);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
		GXSetNumTexGens(0);
		break;
	}

	GXSetCullMode(GX_CULL_NONE);
	PSMTXTrans(trans, data->x, data->y, data->z);

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		alpha = (f32)data->alpha / 255.0f;

		PSMTXRotRad(rot, 'x', 0.017453292f * data->rotX);
		PSMTXConcat(trans, rot, work);

		if (type >= 2) {
			PSMTXRotRad(rot, 'z', 0.017453292f * (data->rotZ + (f32)(type == 3 ? 45 : 0)));
			PSMTXConcat(work, rot, work);
			PSMTXRotRad(rot, 'y', 0.017453292f * data->rotY);
			PSMTXConcat(work, rot, work);
			PSMTXTrans(trans2, data->offset, 0.0f, 0.0f);
			PSMTXConcat(work, trans2, work);
#ifdef MUST_MATCH
			scaleX = 0.25f * (3.0f + alpha);
			scaleZ = 0.25f * (3.0f + alpha);
#else
			scaleX = 0.25f * (3.0f + alpha);
			scaleZ = scaleX;
#endif
			switch (i & 3) {
			case 0:
				scaleY = 0.3f * alpha;
				break;
			case 1:
				scaleY = 0.5f * alpha;
				break;
			case 2:
				scaleY = 0.9f * alpha;
				break;
			default:
				scaleY = alpha;
				break;
			}
			PSMTXScale(scale, scaleX, scaleY, scaleZ);
			PSMTXConcat(work, scale, work);
#ifdef MUST_MATCH
			GXSetTevColor(GX_TEVREG0, radiationSetColor(data->alpha, dat_8041fa90));
#else
			{
				GXColor color = dat_8041fa90;
				color.a = data->alpha;
				GXSetTevColor(GX_TEVREG0, color);
			}
#endif
		} else {
			PSMTXRotRad(rot, 'y', 0.017453292f * data->rotY);
			PSMTXConcat(work, rot, work);
			PSMTXRotRad(rot, 'z', 0.017453292f * (data->rotZ + (f32)(type == 3 ? 45 : 0)));
			PSMTXConcat(work, rot, work);
			PSMTXTrans(trans2, data->offset, 0.0f, 0.0f);
			PSMTXConcat(work, trans2, work);
#ifdef MUST_MATCH
			GXSetTevColor(GX_TEVREG0, radiationSetColor(data->alpha, dat_8041fa94));
#else
			{
				GXColor color = dat_8041fa94;
				color.a = data->alpha;
				GXSetTevColor(GX_TEVREG0, color);
			}
#endif
		}

		PSMTXConcat(cam->view, work, work);
		GXLoadPosMtxImm(work, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		switch (type) {
		case 0:
		case 1:
			vtx = (effVtxArray*)size150x20_tex32x32;
			break;
		default:
			vtx = (effVtxArray*)size300x40_tex32x32;
			break;
		}
		effSetVtxDescN64(vtx);
		switch (type) {
		case 0:
		case 1:
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
			tri2(0, 1, 2, 0, 0, 2, 3, 0);
			break;
		default:
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 3);
#ifdef MUST_MATCH
			tri1WithUnused(0, 1, 2, 0);
#else
			tri1(0, 1, 2);
#endif
			break;
		}
	}
}

// .sdata2 colour constants (0xFFFFFA00 / 0xFFFFB500 as GXColor bytes). Defined after the code
// so they stay real objects and land before the compiler's pooled float literals.
const GXColor dat_8041fa90 = { 0xFF, 0xFF, 0xFA, 0x00 };
const GXColor dat_8041fa94 = { 0xFF, 0xFF, 0xB5, 0x00 };
