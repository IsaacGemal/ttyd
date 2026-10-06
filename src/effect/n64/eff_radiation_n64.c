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
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

// The declaration order `row, n, i, flag, count` and the `n += 2, i++` update order are what
// match the original's flag/type (r24/r25) and n/i (r9/r10) register picks. Reading the table
// through the `rd` local instead of `r_data` directly is what pins the int->float magic
// (0x4330) in r6 and the zero constant in r3, matching the original instead of swapping them
// with the row register. What still differs: MWCC folds the default loop's fourth table load
// `rd[row + 2]` into the third load's already-scaled index (`slwi r4, r4, 2; addi r0, r4, 8`)
// where the original re-derives it (`addi r8, r8, 2; slwi r0, r8, 2; lwzx`). That fold also
// drops the function to 956 bytes against the original's 960 and lands row in r4 rather than
// r8 (and the loop counter i in r8 rather than r10). Because the fold shortens row's live
// range, no source shape removes it without breaking the conversion schedule: a `row + 2` /
// `row += 2` / separate row4 temp / byte-pointer index, all declaration and update orders, the
// GC/1.3 1.3.2 2.0 2.5 2.6 2.7 compilers and every opt_* pragma combination were tried; the
// 1.2.5-family compilers cannot parse the file. Available compiler gets no closer.
#pragma opt_dead_assignments off
// NONMATCHING (99.16%): default loop's 4th table load folded into the 3rd load's scaled index;
// row/i land in r4/r8 instead of r8/r10.
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
	data->field_0x2C = 0;
	data->field_0x28 = 100;
	data->x = x;
	data->y = y;
	data->z = z;
	data->field_0x20 = a5;
	data->field_0x1C = a5;
	data->field_0x94 = flag;
	data->field_0x98 = 4;

	switch (type) {
	case 0:
	case 1:
		data++;
		for (k = 1; k < effect->count; k++, data++) {
			data->field_0x68 = (f32)((k - 1) * 30 / (count - 1) - 10);
			data->field_0x7C = 50 - (k - 1) * 20 / (count - 1);
			data->field_0x6C = 2.5f + 25.0f / (f32)data->field_0x7C;
			data->field_0x70 = (f32)(s32)effTblRandN64(180, k * 3);
			data->field_0x74 = (f32)(s32)effTblRandN64(180, k * 3 + 1);
			data->field_0x78 = (f32)(s32)effTblRandN64(180, k * 3 + 2);
			data->field_0x58 = data->field_0x68;
			data->field_0x28 = data->field_0x7C;
			data->field_0x8C = data->field_0x6C;
			data->field_0x5C = data->field_0x70;
			data->field_0x60 = data->field_0x74;
			data->field_0x64 = data->field_0x78;
			data->field_0x24 = 0;
			data->field_0x2C = 0;
		}
		break;
	default:
		data++;
		for (i = 1, n = 32; i < effect->count; n += 2, i++, data++) {
			data->field_0x58 = 0.0f;
			data->field_0x28 = n;
			row = (i - 1) * 5;
			data->field_0x5C = (f32)r_data[row++];
			data->field_0x60 = (f32)r_data[row++];
			data->field_0x64 = (f32)r_data[row++];
			data->field_0x24 = 0;
			data->field_0x2C = 0;
			data->field_0x38 = 0.0f;
			data->field_0x34 = 0.0f;
			data->field_0x48 = 0.0f;
			data->field_0x4C = -0.6f;
			data->field_0x3C = 10.0f;
			data->field_0x40 = 0.0f;
			data->field_0x50 = 0.0f;
			data->field_0x54 = 0.6f;
			data->field_0x80 = 0.0f;
			data->field_0x84 = 0.0f;
			data->field_0x88 = (f32)r_data[++row] * 0.1f;
			data->field_0x90 = 0;
		}
		break;
	}

	return effect;
}
#pragma opt_dead_assignments reset

// The prologue is matched by starting the Vec temp from the anonymous zero literal (a named
// zero vector makes MWCC hoist its loads above `data = effect->userdata`), which is why the
// file's vec3_802fb478 global is gone: the literal already lands at the same .rodata slot.
// MWCC still folds the default loop's fourth table load `r_data[row + 2]` (after two `++row`)
// into the third load's byte base (`add r5, r10, r7; lwz r5, 0x8(r5)`) where the original
// re-derives the index (`addi r11, r11, 2; slwi r5, r11, 2; lwzx`). The no-fold forms
// (`row += 2`, a row4 temp, an explicit byte offset) restore the missing instruction and the
// original size but reassociate the increment as `(row + 1) + 3` and reorder the int->float
// conversion loads, scoring worse (98.48%). Same blocker as effRadiationN64Entry.
// NONMATCHING (99.37%): default loop's 4th r_data load folded into the 3rd load's byte base.
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
	position = initializeVec((Vec*)&data->pad_0x04);

	type = data->type;
	if (data->field_0x28 <= 10) {
		data->field_0x28--;
	}
	data->field_0x2C++;
	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->field_0x28 = 10;
	}
	life = data->field_0x28;
	if (life < 0) {
		effDelete(effect);
		return;
	}
	counter = data->field_0x2C;

	switch (type) {
	case 0:
	case 1: {
		n = (counter * 10 + 1) * 3;
		n1 = n + 1;
		n2 = n + 2;
		data++;
		for (i = 1; i < effect->count; i++, data++) {
			data->field_0x28--;
			data->field_0x2C++;
			if (data->field_0x2C < 10) {
				data->field_0x24 = data->field_0x2C * 12;
			}
			if (data->field_0x28 < 5) {
				data->field_0x24 = data->field_0x28 * 25;
			}
			data->field_0x58 += data->field_0x8C;
			if (data->field_0x28 <= 0) {
				data->field_0x70 = (f32)(s32)effTblRandN64(180, n);
				data->field_0x74 = (f32)(s32)effTblRandN64(180, n1);
				data->field_0x78 = (f32)(s32)effTblRandN64(180, n2);
				data->field_0x58 = data->field_0x68;
				data->field_0x28 = data->field_0x7C;
				data->field_0x8C = data->field_0x6C;
				data->field_0x5C = data->field_0x70;
				data->field_0x60 = data->field_0x74;
				data->field_0x64 = data->field_0x78;
				data->field_0x24 = 0;
				data->field_0x2C = 0;
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
			data->field_0x28--;
			data->field_0x2C++;
			if (data->field_0x2C < 5) {
				data->field_0x24 = data->field_0x2C * 50;
			}
			if (data->field_0x28 < 5) {
				data->field_0x24 = data->field_0x28 * 50;
			}
			if (life < 10) {
				data->field_0x24 = data->field_0x24 * life / 10;
			}
			if (data->field_0x28 <= 0) {
				data->field_0x58 = 0.0f;
				data->field_0x28 = n;
				row = (i - 1) * 5;
				data->field_0x5C = (f32)r_data[row];
				data->field_0x60 = (f32)r_data[++row];
				data->field_0x64 = (f32)r_data[++row];
				data->field_0x24 = 0;
				data->field_0x2C = 0;
				data->field_0x38 = 0.0f;
				data->field_0x34 = 0.0f;
				data->field_0x48 = 0.0f;
				data->field_0x4C = -0.6f;
				data->field_0x3C = 10.0f;
				data->field_0x40 = 0.0f;
				data->field_0x50 = 0.0f;
				data->field_0x54 = 0.6f;
				data->field_0x80 = 0.0f;
				data->field_0x84 = 0.0f;
				data->field_0x88 = (f32)r_data[row + 2] * 0.1f;
				data->field_0x90 = 0;
			}
			if (data->field_0x90 > 0) {
				data->field_0x90--;
			}
			if (data->field_0x90 <= 0) {
				data->field_0x5C += data->field_0x80;
				data->field_0x60 += data->field_0x84;
				data->field_0x64 += data->field_0x88;
			}
		}
		break;
	}
	}

	{
		EffRadiationData* data2 = effect->userdata;

		if (data2->field_0x94 == 0) {
			dispEntry(data2->field_0x98, dispCalcZ(&position), 2, effRadiationDisp, effect);
		} else {
			dispEntry(data2->field_0x98, dispCalcZ(&position), 8, effRadiationDisp, effect);
		}
	}
}

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

	if (data->field_0x94 != 0) {
		GXSetBlendMode(1, 4, 5, 0);
		GXSetZCompLoc(1);
		GXSetAlphaCompare(7, 0, 0, 7, 0);
		GXSetZMode(0, 3, 0);
	}
	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
	switch (type) {
	case 0:
	case 1:
		GXSetNumTevStages(1);
		GXSetTevOrder(0, 0, 0, 0xff);
		GXSetTevColorOp(0, 0, 0, 0, 1, 0);
		GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
		GXSetTevColorIn(0, 2, 0xc, 8, 0xf);
		GXSetTevAlphaIn(0, 7, 1, 4, 7);
		effGetTexObjN64(0x2c, &texObj);
		GXLoadTexObj(&texObj, 0);
		GXSetNumTexGens(1);
		GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
		PSMTXScale(scale, 0.03125f, 0.03125f, 0.0f);
		GXLoadTexMtxImm(scale, 0x1e, 1);
		break;
	default:
		GXSetNumTevStages(1);
		GXSetTevOrder(0, 0xff, 0xff, 4);
		GXSetTevColorOp(0, 0, 0, 0, 1, 0);
		GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
		GXSetTevColorIn(0, 0xf, 0xf, 0xf, 2);
		GXSetTevAlphaIn(0, 7, 7, 7, 5);
		GXSetNumTexGens(0);
		break;
	}

	GXSetCullMode(0);
	PSMTXTrans(trans, data->x, data->y, data->z);

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		alpha = (f32)data->field_0x24 / 255.0f;

		PSMTXRotRad(rot, 'x', 0.017453292f * data->field_0x5C);
		PSMTXConcat(trans, rot, work);

		if (type >= 2) {
			PSMTXRotRad(rot, 'z', 0.017453292f * (data->field_0x64 + (f32)(type == 3 ? 45 : 0)));
			PSMTXConcat(work, rot, work);
			PSMTXRotRad(rot, 'y', 0.017453292f * data->field_0x60);
			PSMTXConcat(work, rot, work);
			PSMTXTrans(trans2, data->field_0x58, 0.0f, 0.0f);
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
			GXSetTevColor(1, radiationSetColor(data->field_0x24, dat_8041fa90));
#else
			{
				GXColor color = dat_8041fa90;
				color.a = data->field_0x24;
				GXSetTevColor(1, color);
			}
#endif
		} else {
			PSMTXRotRad(rot, 'y', 0.017453292f * data->field_0x60);
			PSMTXConcat(work, rot, work);
			PSMTXRotRad(rot, 'z', 0.017453292f * (data->field_0x64 + (f32)(type == 3 ? 45 : 0)));
			PSMTXConcat(work, rot, work);
			PSMTXTrans(trans2, data->field_0x58, 0.0f, 0.0f);
			PSMTXConcat(work, trans2, work);
#ifdef MUST_MATCH
			GXSetTevColor(1, radiationSetColor(data->field_0x24, dat_8041fa94));
#else
			{
				GXColor color = dat_8041fa94;
				color.a = data->field_0x24;
				GXSetTevColor(1, color);
			}
#endif
		}

		PSMTXConcat(cam->view, work, work);
		GXLoadPosMtxImm(work, 0);
		GXSetCurrentMtx(0);
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
			GXBegin(0x90, 0, 6);
			tri2(0, 1, 2, 0, 0, 2, 3, 0);
			break;
		default:
			GXBegin(0x90, 0, 3);
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
