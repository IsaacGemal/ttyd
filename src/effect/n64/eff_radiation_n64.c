#include "eff/n64/eff_radiation_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "system.h"
#include "memory.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

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
// NONMATCHING (99.16%): default loop's 4th table load folded into the 3rd load's scaled index;
// row/i land in r4/r8 instead of r8/r10.
EffectEntry* effRadiationN64Entry(s32 type, f32 x, f32 y, f32 z, f32 a5) {
	EffectEntry* effect;
	EffRadiationData* data;
	s32* rd;
	s32 row;
	s32 n;
	s32 i;
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

	effect->type = "RadiationN64";
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
		for (i = 1; i < effect->count; i++, data++) {
			data->field_0x68 = (f32)((i - 1) * 30 / (count - 1) - 10);
			data->field_0x7C = 50 - (i - 1) * 20 / (count - 1);
			data->field_0x6C = 2.5f + 25.0f / (f32)data->field_0x7C;
			data->field_0x70 = (f32)(s32)effTblRandN64(180, i * 3);
			data->field_0x74 = (f32)(s32)effTblRandN64(180, i * 3 + 1);
			data->field_0x78 = (f32)(s32)effTblRandN64(180, i * 3 + 2);
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
		rd = r_data;
		for (i = 1, n = 32; i < effect->count; n += 2, i++, data++) {
			data->field_0x58 = 0.0f;
			data->field_0x28 = n;
			row = (i - 1) * 5;
			data->field_0x5C = (f32)rd[row];
			data->field_0x60 = (f32)rd[++row];
			data->field_0x64 = (f32)rd[++row];
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
			data->field_0x88 = (f32)rd[row + 2] * 0.1f;
			data->field_0x90 = 0;
		}
		break;
	}

	return effect;
}

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
extern const f64 double_to_int_802fb488;
extern const f32 float_0p03125_8041fa98;
extern const f32 float_0_8041fa9c;
extern const f32 float_255_8041faa0;
extern const f32 float_deg2rad_8041faa4;
extern const f32 float_0p25_8041faa8;
extern const f32 float_3_8041faac;
extern const f32 float_0p3_8041fab0;
extern const f32 float_0p5_8041fab4;
extern const f32 float_0p9_8041fab8;
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
		PSMTXScale(scale, float_0p03125_8041fa98, float_0p03125_8041fa98, float_0_8041fa9c);
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
		alpha = (f32)data->field_0x24 / float_255_8041faa0;

		PSMTXRotRad(rot, 'x', float_deg2rad_8041faa4 * data->field_0x5C);
		PSMTXConcat(trans, rot, work);

		if (type >= 2) {
			PSMTXRotRad(rot, 'z', float_deg2rad_8041faa4 * (data->field_0x64 + (f32)(type == 3 ? 45 : 0)));
			PSMTXConcat(work, rot, work);
			PSMTXRotRad(rot, 'y', float_deg2rad_8041faa4 * data->field_0x60);
			PSMTXConcat(work, rot, work);
			PSMTXTrans(trans2, data->field_0x58, float_0_8041fa9c, float_0_8041fa9c);
			PSMTXConcat(work, trans2, work);
#ifdef MUST_MATCH
			scaleX = float_0p25_8041faa8 * (float_3_8041faac + alpha);
			scaleZ = float_0p25_8041faa8 * (float_3_8041faac + alpha);
#else
			scaleX = float_0p25_8041faa8 * (float_3_8041faac + alpha);
			scaleZ = scaleX;
#endif
			switch (i & 3) {
			case 0:
				scaleY = float_0p3_8041fab0 * alpha;
				break;
			case 1:
				scaleY = float_0p5_8041fab4 * alpha;
				break;
			case 2:
				scaleY = float_0p9_8041fab8 * alpha;
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
			PSMTXRotRad(rot, 'y', float_deg2rad_8041faa4 * data->field_0x60);
			PSMTXConcat(work, rot, work);
			PSMTXRotRad(rot, 'z', float_deg2rad_8041faa4 * (data->field_0x64 + (f32)(type == 3 ? 45 : 0)));
			PSMTXConcat(work, rot, work);
			PSMTXTrans(trans2, data->field_0x58, float_0_8041fa9c, float_0_8041fa9c);
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
