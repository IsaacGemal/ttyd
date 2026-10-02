#include "eff/eff_recovery.h"
#include "eff/eff_number.h"
#include "eff/eff_stardust.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "mario/mariost.h"
#include "math_ext.h"
#include "memory.h"
#include "pmario_sound.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <string.h>
#include <stdlib.h>

typedef struct RecoveryMayaAnim {
	u16 flags;       // 0x00
	u8 pad_02[2];    // 0x02
	f32 frame;       // 0x04
	u32 timeLo;      // 0x08
	u32 timeHi;      // 0x0C
	void* animData;  // 0x10
	Mtx mtx;         // 0x14
} RecoveryMayaAnim;

typedef struct RecoveryData {
	s8 type;         // 0x00
	s8 field_1;      // 0x01
	s8 state;        // 0x02
	u8 pad_03;       // 0x03
	s32 color;       // 0x04
	f32 x;           // 0x08
	f32 y;           // 0x0C
	f32 z;           // 0x10
	f32 velX;        // 0x14
	f32 velY;        // 0x18
	f32 velZ;        // 0x1C
	f32 scaleX;      // 0x20
	f32 scaleY;      // 0x24
	f32 scaleZ;      // 0x28
	s16 timer;       // 0x2C
	u8 pad_2E[2];    // 0x2E
	f32 angle;       // 0x30
	u32 timeBase;    // 0x34
	RecoveryMayaAnim* anim; // 0x38
	s32 delay;       // 0x3C
	f32 field_40;    // 0x40
} RecoveryData;

extern const char str_8041CB08[];
extern const f32 float_deg2rad_8041cad0;
extern const f32 float_32_8041cad4;
extern const f32 float_90_8041cad8;
const f32 float_0_8041cadc = 0.0f;
const f32 float_1_8041cae0 = 1.0f;
extern const f32 float_8_8041cae4;
extern const f32 float_2_8041cae8;
extern const f32 float_0p5_8041caec;
extern const f32 float_16_8041caf0;
extern const f32 float_10_8041caf4;
extern const f32 float_0p05_8041caf8;
extern const f32 float_108_8041cafc;
extern const f32 float_20_8041cb00;
extern const f32 float_neg20_8041cb04;
extern const f32 float_0p035_8041cb0c;
extern const f32 float_24_8041cb10;
extern const f32 float_0p1_8041cb14;
extern const f32 float_neg16_8041cb18;
extern const f32 float_neg6_8041cb1c;
extern const f32 float_6_8041cb20;
extern const f32 float_40_8041cb24;
extern const f32 float_45_8041cb28;
extern const f32 float_0p95_8041cb2c;
extern const f32 float_6p2832_8041cb30;
extern const f32 float_360_8041cb34;
extern const f32 float_12_8041cb38;
extern const f32 float_0p7_8041cb3c;
extern const f32 float_0p99_8041cb40;
extern const f32 float_30_8041cb44;
extern const f32 float_neg30_8041cb48;
extern const f32 float_neg15_8041cb4c;
extern const f32 float_1p5_8041cb50;
extern const f32 float_0p3_8041cb54;
extern const f32 float_1p8_8041cb58;
extern const u32 dat_8041cac0;
extern const u32 dat_8041cac4;
extern const u32 dat_8041cac8;
extern const u32 dat_8041cacc;
extern GXColor unk_80423040;
extern GXColor unk_80423044;
static u8 color_rotation_data[] = {
	0xFE, 0xAC, 0xAC, 0xFE, 0xAC, 0xD5, 0xFE, 0xB4, 0x9A, 0xD5, 0xB4, 0xFE,
	0xB4, 0xB4, 0xFE, 0xB4, 0xDD, 0xFE, 0xB4, 0xFE, 0xFE, 0xB4, 0xFE, 0xD5,
	0xB4, 0xFE, 0xB4, 0xD5, 0xFE, 0xB4, 0xFE, 0xFE, 0xB4, 0xFE, 0xD5, 0xAC,
};

// Match-only: helper's anonymous literal is the unit's zero-vector .rodata object; building the
// temp from it keeps the copy's loads late (see entry).
static inline Vec recoveryZeroVec(void) {
	Vec v = (Vec){ 0.0f, 0.0f, 0.0f };
	return v;
}
// Match-only: helper's anonymous literal is the unit's zero-vector .rodata object; building the
// temp from it keeps the copy's loads late (see entry).
static inline Vec recoveryZeroVec2(void) {
	Vec v = (Vec){ 0.0f, 0.0f, 0.0f };
	return v;
}
// Match-only: helper's anonymous literal is the unit's zero-vector .rodata object; building the
// temp from it keeps the copy's loads late (see entry).
static inline Vec recoveryZeroVec3(void) {
	Vec v = (Vec){ 0.0f, 0.0f, 0.0f };
	return v;
}
// Match-only: helper's anonymous literal is the unit's zero-vector .rodata object; building the
// temp from it keeps the copy's loads late (see entry).
static inline Vec recoveryZeroVec4(void) {
	Vec v = (Vec){ 0.0f, 0.0f, 0.0f };
	return v;
}
// Match-only: helper's anonymous literal is the unit's zero-vector .rodata object; building the
// temp from it keeps the copy's loads late (see entry).
static inline Vec recoveryZeroVec5(void) {
	Vec v = (Vec){ 0.0f, 0.0f, 0.0f };
	return v;
}
// Match-only: helper's anonymous literal is the unit's zero-vector .rodata object; building the
// temp from it keeps the copy's loads late (see entry).
static inline Vec recoveryZeroVec6(void) {
	Vec v = (Vec){ 0.0f, 0.0f, 0.0f };
	return v;
}

// .rodata (0x802F3508..0x802F3678)
#define str_802F3558 "SFX_HP_RECOVER_SHINE1"
#define str_802F3570 "SFX_HP_RECOVER_SHINE2"
#define str_802F3588 "SFX_HP_RECOVER_SHINE3"
#define str_802F35A0 "SFX_FP_RECOVER_SHINE1"
#define str_802F35B8 "SFX_FP_RECOVER_SHINE2"
#define str_802F35D0 "SFX_FP_RECOVER_SHINE3"
#define str_802F35E8 "Recovery"
#define str_802F35F4 "SFX_HP_RECOVER_SMALL1"
#define str_802F360C "SFX_HP_RECOVER_MID1"
#define str_802F3620 "SFX_HP_RECOVER_BIG1"
#define str_802F3634 "SFX_FP_RECOVER_SMALL1"
#define str_802F364C "SFX_FP_RECOVER_MID1"
#define str_802F3660 "SFX_FP_RECOVER_BIG1"

const char str_8041CB08[] = "%d";

// 100%: the colour strings are string literals (not named `const char[]` objects), so MWCC pools
// its own int->float double as the unit's only .rodata constant right after the zero-Vec helpers,
// at +0x48, ahead of the pooled strings -- exactly the original's layout. `double_to_int_802f3550`
// is intentionally not defined by name (a named copy would add a second, unused pool entry and push
// the anonymous one past the strings). The zero Vecs come from the `recoveryZeroVecN` inlines above
// the .rodata.
EffectEntry* effRecoveryEntry(s32 type, s32 count, f32 x, f32 y, f32 z) {
	EffectEntry* effect;
	RecoveryData* data;
	Vec disp;
	Vec pos;
	s32 n;
	RecoveryData* ptr;
	s32 i;
	s32 angle;

	effect = effEntry();
	switch (type) {
	case 2:
	case 3:
		count = 0;
		break;
	case 4:
	case 5:
		count = 0x14;
		break;
	case 6:
		count = 0xB;
		break;
	}
	effect->type = str_802F35E8;
	effect->count = count + 1;
	data = __memAlloc(HEAP_EFFECT, effect->count * 0x44);
	effect->userdata = data;
	switch (type) {
	case 6:
		effect->callback = effRecoveryMain2;
		break;
	default:
		effect->callback = effRecoveryMain;
		break;
	}
	data->anim = effMayaAnimAlloc(recovery_effAnimDt);
	effPlayMayaAnim(data->anim);
	data->type = type;
	data->field_1 = count;
	data->x = x;
	data->y = y;
	data->z = z;
	data->timer = 0;
	data->timeBase = gp->retraceCount;
	data->color = 0xFF;
	data->field_40 = float_neg30_8041cb48;

	pos = recoveryZeroVec();
	pos.x = data->x;
	pos.y = data->y;
	pos.z = data->z;
	disp = pos;

	switch (data->type) {
	case 0:
	case 7:
		if (data->field_1 <= 5) {
			psndSFXOn_3D(str_802F35F4, &disp);
		} else if (data->field_1 <= 0xA) {
			psndSFXOn_3D(str_802F360C, &disp);
		} else {
			psndSFXOn_3D(str_802F3620, &disp);
		}
		break;
	case 1:
	case 8:
		if (data->field_1 <= 5) {
			psndSFXOn_3D(str_802F3634, &disp);
		} else if (data->field_1 <= 0xA) {
			psndSFXOn_3D(str_802F364C, &disp);
		} else {
			psndSFXOn_3D(str_802F3660, &disp);
		}
		break;
	case 4:
		psndSFXOn_3D(str_802F3620, &disp);
		break;
	case 5:
		psndSFXOn_3D(str_802F3660, &disp);
		break;
	}

	switch (type) {
	case 6:
		effDeleteMayaAnim(data->anim);
		data->timer = 0xB4;
		break;
	}

	ptr = data + 1;
	i = 1;
	n = 8;
	for (; i < effect->count; i++, ptr++) {
		ptr->x = float_0_8041cadc;
		switch (type) {
		case 4:
		case 5:
			ptr->y = (i < 10) ? float_0_8041cadc : float_neg15_8041cb4c;
			break;
		default:
			ptr->y = float_0_8041cadc;
			break;
		}
		ptr->z = float_0_8041cadc;
		ptr->scaleX = float_0_8041cadc;
		ptr->scaleY = float_0_8041cadc;
		ptr->scaleZ = float_0_8041cadc;
		ptr->velX = float_0_8041cadc;
		ptr->velY = float_0_8041cadc;
		ptr->velZ = float_0_8041cadc;
		ptr->timer = 0;
		ptr->state = 0;
		ptr->color = 0xFF;
		ptr->angle = float_0_8041cadc;
		switch (type) {
		case 6:
			if (i == 1) {
				ptr->scaleZ = float_1p5_8041cb50;
				ptr->scaleY = float_1p5_8041cb50;
				ptr->scaleX = float_1p5_8041cb50;
				ptr->color = 0;
				ptr->delay = 0;
			} else {
				ptr->scaleZ = float_0p3_8041cb54;
				ptr->scaleY = float_0p3_8041cb54;
				ptr->scaleX = float_0p3_8041cb54;
				if (i & 1) {
					angle = irand(0x50);
				} else {
					angle = -irand(0x50);
				}
				angle += 0x5A;
				ptr->angle = angle;
				ptr->velX = float_1p8_8041cb58 * (f32)cos(float_6p2832_8041cb30 * ptr->angle / float_360_8041cb34);
				ptr->velY = float_1p8_8041cb58 * (f32)sin(float_6p2832_8041cb30 * ptr->angle / float_360_8041cb34);
				ptr->color = 0xFF;
				ptr->delay = n;
			}
			break;
		}
		n += 8;
	}
	return effect;
}

// 100%: same string-literal / `double_to_int` .rodata layout as effRecoveryEntry above (see its
// comment); the int->float conversions now reference the compiler-pooled double at .rodata+0x48.
void effRecoveryMain(EffectEntry* effect) {
	RecoveryData* data = effect->userdata;
	RecoveryData* ptr;
	Vec soundArg;
	Vec sound;
	Vec pos;
	Vec pos2;
	Vec pos3;
	s32 i;
	s32 srcState;
	s32 field_1;
	s32 total;
	s32 type;
	u32 field;
	f32 f31;
	f32 f30;
	f32 f29;
	f32 f28;

	type = data->type;
	srcState = 0;
	total = 0;
	field = 0;
	f28 = data->field_40;
	effCalcMayaAnim(data->anim);
	if (gp->retraceCount > data->timeBase) {
		field = gp->retraceCount - data->timeBase;
	}
	if (field < 1) {
		field = 1;
	}
	while (field-- != 0) {
		ptr = effect->userdata;
		field_1 = ptr->field_1;
		f31 = ptr->x;
		f30 = ptr->y;
		f29 = ptr->z;
		if (type == 4 || type == 5) {
			field_1 = 10;
		}
		ptr->timer++;
		if (ptr->timer > 10) {
			srcState = 1;
		}
		for (i = 1, ptr = ptr + 1; i < effect->count; i++, ptr++) {
			switch (ptr->state) {
			case 0:
				if (srcState == 0) {
					break;
				}
				{
					f32 ang = ((f32)(i - 1) * 360.0f / (f32)field_1 + 90.0f) * float_6p2832_8041cb30 / 360.0f;
					ptr->velX = 12.0f * (f32)sin(ang);
					ptr->velY = 0.0f;
					{
						f32 c = (f32)cos(ang);
						ptr->velZ = 12.0f * -c;
					}
					ptr->timer = 0;
					ptr->state++;
				}
				break;
			case 1:
				if (ptr->timer < 10) {
					ptr->velX *= 0.7f;
					ptr->velY *= 0.7f;
					ptr->velZ *= 0.7f;
					ptr->x += ptr->velX;
					ptr->y += ptr->velY;
					ptr->z += ptr->velZ;
					ptr->scaleZ = 1.0f;
					ptr->scaleY = 1.0f;
					ptr->scaleX = 1.0f;
					ptr->timer++;
				} else {
					ptr->timer = 10;
					ptr->state++;
				}
				break;
			case 2:
				if (--ptr->timer <= 0) {
					ptr->timer = 0;
					ptr->state++;
				}
				break;
			case 3:
				if (ptr->timer < 0x28) {
					s16 timer = ptr->timer + 1;
					f32 t;
					ptr->timer = timer;
					t = intplGetValue(0xB, timer, 0x28, 0.0f, 360.0f);
					{
						f32 radius = distABf(0.0f, 0.0f, ptr->x, ptr->z);
						f32 ang = ((f32)(i - 2) * 360.0f / (f32)field_1 + t + 90.0f) * float_6p2832_8041cb30 / 360.0f;
						radius *= float_0p99_8041cb40;
						ptr->x = radius * (f32)sin(ang);
						ptr->z = radius * -(f32)cos(ang);
					}
				} else {
					ptr->timer = rand() % 20;
					ptr->state++;
				}
				break;
			case 4:
				if (--ptr->timer <= 0) {
					ptr->timer = 0;
					ptr->state++;
				}
				break;
			case 5:
				if (ptr->timer < 0x10) {
					f32 s;
					ptr->timer++;
					ptr->x = intplGetValue(1, ptr->timer, 0x10, ptr->x, 0.0f);
					ptr->y = intplGetValue(1, ptr->timer, 0x10, ptr->y, f28);
					ptr->z = intplGetValue(1, ptr->timer, 0x14, ptr->z, 0.0f);
					s = intplGetValue(1, ptr->timer, 0x10, 1.0f, 0.0f);
					ptr->scaleZ = s;
					ptr->scaleY = s;
					ptr->scaleX = s;
				} else {
					ptr->state++;
				}
				break;
			case 6:
				total++;
				break;
			}
		}
		if (((u32)type <= 1 || (u32)(type - 7) <= 1 || type == 4 || type == 5) && total >= field_1) {
			RecoveryData* center = effect->userdata;
			if (type != 7 && type != 8) {
				effStardustEntry(2, 2, 0x1E, f31, f30 - 20.0f, f29, 30.0f, 30.0f);
				sound = recoveryZeroVec2();
				sound.x = center->x;
				sound.y = center->y;
				sound.z = center->z;
				soundArg = sound;
				switch (center->type) {
				case 0:
					if (center->field_1 <= 5) {
						psndSFXOn_3D(str_802F3558, &soundArg);
					} else if (center->field_1 <= 10) {
						psndSFXOn_3D(str_802F3570, &soundArg);
					} else {
						psndSFXOn_3D(str_802F3588, &soundArg);
					}
					break;
				case 1:
					if (center->field_1 <= 5) {
						psndSFXOn_3D(str_802F35A0, &soundArg);
					} else if (center->field_1 <= 10) {
						psndSFXOn_3D(str_802F35B8, &soundArg);
					} else {
						psndSFXOn_3D(str_802F35D0, &soundArg);
					}
					break;
				case 4:
					psndSFXOn_3D(str_802F3588, &soundArg);
					break;
				case 5:
					psndSFXOn_3D(str_802F35D0, &soundArg);
					break;
				}
			}
			effDeleteMayaAnim(center->anim);
			effDelete(effect);
			return;
		}
	}
	data = effect->userdata;
	data->timeBase = gp->retraceCount;
	pos = recoveryZeroVec3();
	pos.x = data->x;
	pos.y = data->y;
	pos.z = data->z;
	dispEntry(CAMERA_3D, dispCalcZ(&pos), 1, effRecoveryDisp, effect);
	if ((u32)type <= 1 || type == 7 || type == 8) {
		pos2 = recoveryZeroVec4();
		pos2.x = data->x;
		pos2.y = data->y;
		pos2.z = data->z;
		dispEntry(CAMERA_3D, dispCalcZ(&pos2), 2, effRecoveryDisp2, effect);
	}
	if (type == 4 || type == 5) {
		pos3 = recoveryZeroVec5();
		pos3.x = data->x;
		pos3.y = data->y;
		pos3.z = data->z;
		dispEntry(CAMERA_3D, dispCalcZ(&pos3), 2, effRecoveryDisp3, effect);
	}
}

// The two `volatile` reads are required for the original's instruction order: without
// them MWCC hoists the vec3_802f3544 loads into the callee-save prologue, ahead of the
// `effect->userdata` load. They emit the same lwz/loads, just ordered (rule 7 trick).
void effRecoveryMain2(EffectEntry* effect) {
#ifdef MUST_MATCH
	RecoveryData* ptr = *(RecoveryData* volatile*)&effect->userdata;
#else
	RecoveryData* ptr = effect->userdata;
#endif
	Vec disp;
	Vec pos;
	s32 i;

#ifdef MUST_MATCH
	pos = recoveryZeroVec6();
#else
	pos = recoveryZeroVec6();
#endif
	pos.x = ptr->x;
	pos.y = ptr->y;
	pos.z = ptr->z;
	disp = pos;

	if (ptr->timer < 1000) {
		ptr->timer--;
	}
	if (ptr->timer < 0x20) {
		ptr->color = ptr->timer * 8;
	}
	if (ptr->timer < 0) {
		effStardustEntry(2, 6, 0x1E, ptr->x, ptr->y, ptr->z, float_40_8041cb24, float_45_8041cb28);
		effDelete(effect);
		return;
	}

	ptr++;
	for (i = 1; i < effect->count; i++, ptr++) {
		if (ptr->delay != 0) {
			ptr->delay--;
			continue;
		}
		ptr->x += ptr->velX;
		ptr->y += ptr->velY;
		ptr->z += ptr->velZ;
		ptr->velX *= float_0p95_8041cb2c;
		ptr->velY *= float_0p95_8041cb2c;
		ptr->velZ *= float_0p95_8041cb2c;
		if (i == 1) {
			ptr->color += 5;
			if (ptr->color > 0x80) {
				ptr->color = 0x80;
			}
		} else {
			ptr->color -= 8;
			if (ptr->color < 0) {
				f32 zero = float_0_8041cadc;
				ptr->x = zero;
				ptr->y = zero;
				ptr->z = zero;
				ptr->velX = (f32)cos(float_6p2832_8041cb30 * ptr->angle / float_360_8041cb34) * float_2_8041cae8;
				ptr->velY = (f32)sin(float_6p2832_8041cb30 * ptr->angle / float_360_8041cb34) * float_2_8041cae8;
				ptr->color = 0xFF;
			}
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effRecoveryDisp4, effect);
}

// The vertex float literals (-16/16 in the first pass, -6/6/2 in the loop) keep MWCC from
// hoisting the shared float constants into callee-saved f29-f31; the named externs hold the same
// pool values but stay live across the volatile FIFO stores, adding FPR-save prologue bytes.
void effRecoveryDisp(CameraId camId, void* param) {
	EffectEntry* entry = param;
	RecoveryData* data = entry->userdata;
	RecoveryData* ptr;
	CameraEntry* cam;
	Mtx mtx;
	Mtx tmp;
	Mtx scale;
	Mtx base;
	GXTexObj tex;
	GXColor c;
	s32 type;
	s32 i;

	cam = camGetPtr(camId);
	type = data->type;
	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXRotRad(tmp, 'y', -camGetPtr(camId)->viewYaw * float_deg2rad_8041cad0);
	PSMTXConcat(mtx, tmp, base);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXSetNumChans(0);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	switch (type) {
	case 0:
	case 2:
	case 4:
	case 7:
		effGetTexObj(0x12, &tex);
		GXLoadTexObj(&tex, GX_TEXMAP0);
		effGetTexObj(0x14, &tex);
		GXLoadTexObj(&tex, GX_TEXMAP1);
		break;
	case 1:
	case 3:
	case 5:
	case 8:
		effGetTexObj(0x13, &tex);
		GXLoadTexObj(&tex, GX_TEXMAP0);
		effGetTexObj(0x15, &tex);
		GXLoadTexObj(&tex, GX_TEXMAP1);
		break;
	}
	ptr = entry->userdata;
	PSMTXTrans(mtx, ptr->x, ptr->y, ptr->z);
	PSMTXRotRad(tmp, 'y', -camGetPtr(camId)->viewYaw * float_deg2rad_8041cad0);
	PSMTXConcat(mtx, tmp, mtx);
	PSMTXScale(scale, float_10_8041caf4, float_10_8041caf4, float_10_8041caf4);
	PSMTXConcat(mtx, scale, mtx);
	PSMTXTrans(scale, float_0_8041cadc, float_2_8041cae8, float_0_8041cadc);
	PSMTXConcat(mtx, scale, mtx);
	if ((u32)type <= 1 || (u32)(type - 7) <= 1 || type == 4 || type == 5) {
		PSMTXConcat(mtx, ptr->anim->mtx, mtx);
	}
	PSMTXConcat(cam->view, mtx, mtx);
	PSMTXScale(scale, float_0p1_8041cb14, float_0p1_8041cb14, float_0p1_8041cb14);
	PSMTXConcat(mtx, scale, mtx);
	GXLoadPosMtxImm(mtx, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3f32((-16.0f), (16.0f), float_0_8041cadc);
	GXTexCoord2f32(float_0_8041cadc, float_0_8041cadc);
	GXPosition3f32((16.0f), (16.0f), float_0_8041cadc);
	GXTexCoord2f32(float_1_8041cae0, float_0_8041cadc);
	GXPosition3f32((16.0f), (-16.0f), float_0_8041cadc);
	GXTexCoord2f32(float_1_8041cae0, float_1_8041cae0);
	GXPosition3f32((-16.0f), (-16.0f), float_0_8041cadc);
	GXTexCoord2f32(float_0_8041cadc, float_1_8041cae0);
	if (type == 2 || type == 3) {
		return;
	}
	// the pointer advance sits before the blend setup so its addi fills the call's argument slots,
	// as the original schedules it (a for-init advance lands after the whole setup instead)
	ptr = ptr + 1;
	GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
	GXSetZCompLoc(GX_FALSE);
	GXSetAlphaCompare(GX_GEQUAL, 0x80, GX_AOP_OR, GX_NEVER, 0);
	GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP1, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
	for (i = 1; i < entry->count; i++, ptr++) {
		s32 idx = (i + gp->retraceCount) % 12;
		c = unk_80423040;
		c.r = color_rotation_data[idx * 3 + 0];
		c.g = color_rotation_data[idx * 3 + 1];
		c.b = color_rotation_data[idx * 3 + 2];
		c.a = ptr->color;
		GXSetTevColor(GX_TEVREG0, c);
		PSMTXTrans(mtx, ptr->x, ptr->y, ptr->z);
		PSMTXScale(scale, ptr->scaleX, ptr->scaleY, ptr->scaleZ);
		PSMTXConcat(mtx, scale, mtx);
		PSMTXConcat(base, mtx, mtx);
		PSMTXConcat(camGetPtr(camId)->view, mtx, mtx);
		GXLoadPosMtxImm(mtx, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(-6.0f, 6.0f, float_0_8041cadc);
		GXTexCoord2f32(float_0_8041cadc, float_0_8041cadc);
		GXPosition3f32(6.0f, 6.0f, float_0_8041cadc);
		GXTexCoord2f32(2.0f, float_0_8041cadc);
		GXPosition3f32(6.0f, -6.0f, float_0_8041cadc);
		GXTexCoord2f32(2.0f, float_1_8041cae0);
		GXPosition3f32(-6.0f, -6.0f, float_0_8041cadc);
		GXTexCoord2f32(float_0_8041cadc, float_1_8041cae0);
	}
}

void effRecoveryDisp2(CameraId camId, void* param) {
	EffectEntry* entry = param;
	RecoveryData* data = entry->userdata;
	char buf[0x20];
	Mtx mtx;
	Mtx rot;
	Mtx tmp;
	s32 type;
	s32 num;
	s32 len;
	f32 transX;

	camGetPtr(camId);
	num = data->field_1;
	type = data->type;
	sprintf(buf, str_8041CB08, num);
	len = strlen(buf);
	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', -camGetPtr(camId)->viewYaw * float_deg2rad_8041cad0);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXTrans(tmp, float_0_8041cadc, float_0_8041cadc, float_1_8041cae0);
	PSMTXConcat(mtx, tmp, mtx);
	PSMTXScale(tmp, float_10_8041caf4, float_10_8041caf4, float_10_8041caf4);
	PSMTXConcat(mtx, tmp, mtx);
	PSMTXTrans(tmp, float_0_8041cadc, float_2_8041cae8, float_0_8041cadc);
	PSMTXConcat(mtx, tmp, mtx);
	PSMTXConcat(mtx, data->anim->mtx, mtx);
	PSMTXScale(tmp, float_0p035_8041cb0c, float_0p035_8041cb0c, float_0p035_8041cb0c);
	PSMTXConcat(mtx, tmp, mtx);
#ifdef MUST_MATCH
	// split the x scale into a temp: keeps the int->float conversion and *24 in the original's registers
	transX = float_24_8041cb10 * (f32)(-(len - 1));
	PSMTXTrans(tmp, transX * float_0p5_8041caec, float_0_8041cadc, float_0_8041cadc);
#else
	PSMTXTrans(tmp, float_24_8041cb10 * -(len - 1) * float_0p5_8041caec, float_0_8041cadc, float_0_8041cadc);
#endif
	PSMTXConcat(mtx, tmp, mtx);

	switch (type) {
	case 0:
	case 2:
	case 7:
		effNumberGX(&mtx, 0, num, 0xFF, 1);
		break;
	case 1:
	case 3:
	case 8:
		effNumberGX(&mtx, 1, num, 0xFF, 1);
		break;
	}
}

// The f27/f28 declarations are reversed so the +-20 pair lands in the target's registers, and the +-20
// assignments sit inside the loop under MUST_MATCH because above the loop they are hoisted into the preheader
// and scheduled before the int->float magic load, which the target does not do.
// The half-size block below is under MUST_MATCH: writing it as one expression makes MWCC load the pool
// constant into f2 and negate into f1, swapping the target's f1/f2. Declaring the negated value and giving it
// a dead use after the fadds extends its live range so MWCC uses the target's f1 (constant) / f2 (negated temp).
// The dead store compiles away, so no extra instruction is emitted.
void effRecoveryDisp3(CameraId camId, void* param) {
	EffectEntry* entry = param;
	RecoveryData* data = entry->userdata;
	CameraEntry* cam;
	Mtx mtx;
	Mtx tmp;
	Mtx tmp2;
	Mtx base;
	GXTexObj tex;
	s32 type;
	s32 i;
	f32 f30;
	f32 f29;
	f32 f28;
	f32 f27;

	cam = camGetPtr(camId);
	type = data->type;
	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXRotRad(tmp, 'y', -camGetPtr(camId)->viewYaw * float_deg2rad_8041cad0);
	PSMTXConcat(mtx, tmp, mtx);
	PSMTXTrans(tmp2, float_0_8041cadc, float_0_8041cadc, float_1_8041cae0);
	PSMTXConcat(mtx, tmp2, mtx);
	PSMTXScale(tmp2, float_10_8041caf4, float_10_8041caf4, float_10_8041caf4);
	PSMTXConcat(mtx, tmp2, mtx);
	PSMTXTrans(tmp2, float_0_8041cadc, float_2_8041cae8, float_0_8041cadc);
	PSMTXConcat(mtx, tmp2, mtx);
	PSMTXConcat(mtx, data->anim->mtx, base);
	PSMTXConcat(cam->view, base, base);
	PSMTXScale(tmp2, float_0p05_8041caf8, float_0p05_8041caf8, float_0p05_8041caf8);
	PSMTXConcat(base, tmp2, base);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
#ifdef MUST_MATCH
	{
		// A dead store after the half-size sum extends the negated value's live range past the fadds;
		// MWCC then keeps the pool constant in f1 and the negated temp in f2, as in the target. The
		// store is eliminated, so it emits no instruction; without it only f1/f2 are swapped.
		f32 negHalf = -float_108_8041cafc;
		f30 = negHalf * float_0p5_8041caec;
		f29 = f30 + float_108_8041cafc;
		f27 = negHalf;
	}
#else
	f30 = -float_108_8041cafc * float_0p5_8041caec;
	f29 = f30 + float_108_8041cafc;
#endif
#ifndef MUST_MATCH
	f27 = float_20_8041cb00;
	f28 = float_neg20_8041cb04;
#endif
	for (i = 0; i < 2; i++) {
		if (i == 0) {
			effGetTexObj(0x8b, &tex);
			GXLoadTexObj(&tex, GX_TEXMAP0);
			GXSetChanMatColor(GX_COLOR0A0, *(GXColor*)&dat_8041cac0);
		} else {
			effGetTexObj(0x8a, &tex);
			GXLoadTexObj(&tex, GX_TEXMAP0);
			if (type == 4) {
				GXSetChanMatColor(GX_COLOR0A0, *(GXColor*)&dat_8041cac4);
			} else {
				GXSetChanMatColor(GX_COLOR0A0, *(GXColor*)&dat_8041cac8);
			}
		}
		PSMTXTrans(mtx, (f32)((1 - i) * 2), (f32)(-(1 - i) * 2), float_0_8041cadc);
		PSMTXConcat(base, mtx, mtx);
		GXLoadPosMtxImm(mtx, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
#ifdef MUST_MATCH
		// Assigning the loop-invariant +-20 pair inside the loop (not above it) makes the compiler hoist it
		// together with the int->float magic instead of into the loop preheader, matching the original's
		// lfd/f27/f28 load order. It changes no value; without it only the load order differs.
		f27 = float_20_8041cb00;
		f28 = float_neg20_8041cb04;
#endif
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(f30, f27, float_0_8041cadc);
		GXTexCoord2f32(float_0_8041cadc, float_0_8041cadc);
		GXPosition3f32(f29, f27, float_0_8041cadc);
		GXTexCoord2f32(float_1_8041cae0, float_0_8041cadc);
		GXPosition3f32(f29, f28, float_0_8041cadc);
		GXTexCoord2f32(float_1_8041cae0, float_1_8041cae0);
		GXPosition3f32(f30, f28, float_0_8041cadc);
		GXTexCoord2f32(float_0_8041cadc, float_1_8041cae0);
	}
}

// NONMATCHING (99.93%): the frame (0x130) and matrix offsets match; MWCC swaps the else branch's
// colour build (0x14) and the i==1 branch's outgoing copy (0xc) vs the original (0xc / 0x14), plus
// one swapped fadds operand order in the last GXPosition3f32.
void effRecoveryDisp4(CameraId camId, void* param) {
	EffectEntry* entry = param;
	RecoveryData* ptr = entry->userdata;
	CameraEntry* cam;
	Mtx mtx;
	Mtx rot;
	Mtx scale;
	Mtx base;
#ifdef MUST_MATCH
	// Match-only: the original copies the else branch's colour to a second slot before loading the
	// textures and re-reads it for GXSetChanMatColor. The volatile shim reproduces that extra stack
	// slot (and the resulting 0x130 frame); it changes no value. The natural form is under #else.
	volatile u32 saved;
	GXColor chanColor;
#endif
	GXTexObj tex;
	s32 i;
	s32 color;
	f32 y;
	f32 s;
	f32 t;
	f32 one = float_1_8041cae0;

	cam = camGetPtr(camId);
	color = ptr->color;
	PSMTXTrans(mtx, ptr->x, ptr->y, ptr->z);
	PSMTXRotRad(rot, 'y', -camGetPtr(camId)->viewYaw * float_deg2rad_8041cad0);
	PSMTXConcat(mtx, rot, base);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	y = -float_32_8041cad4;
	i = 1;
	ptr++;
	for (; i < entry->count; i++, ptr++) {
		if (ptr->delay != 0) {
			continue;
		}
		if (i == 1) {
			GXColor chanColor;
			effGetTexObj(0x12, &tex);
			GXLoadTexObj(&tex, GX_TEXMAP0);
			*(u32*)&chanColor = dat_8041cacc;
			chanColor.a = (color * ptr->color) / 255;
			GXSetChanMatColor(GX_COLOR0A0, chanColor);
		} else {
			s32 idx = (i + gp->retraceCount) % 12;
#ifdef MUST_MATCH
			chanColor = *(GXColor*)&unk_80423044;
			chanColor.r = color_rotation_data[idx * 3 + 0];
			chanColor.g = color_rotation_data[idx * 3 + 1];
			chanColor.b = color_rotation_data[idx * 3 + 2];
			chanColor.a = (color * ptr->color) / 255;
			saved = *(u32*)&chanColor;
			effGetTexObj(0x14, &tex);
			GXLoadTexObj(&tex, GX_TEXMAP0);
			GXSetChanMatColor(GX_COLOR0A0, *(GXColor*)&saved);
#else
			GXColor chanColor = *(GXColor*)&unk_80423044;
			chanColor.r = color_rotation_data[idx * 3 + 0];
			chanColor.g = color_rotation_data[idx * 3 + 1];
			chanColor.b = color_rotation_data[idx * 3 + 2];
			chanColor.a = (color * ptr->color) / 255;
			effGetTexObj(0x14, &tex);
			GXLoadTexObj(&tex, GX_TEXMAP0);
			GXSetChanMatColor(GX_COLOR0A0, chanColor);
#endif
		}
		PSMTXTrans(mtx, ptr->x, ptr->y, ptr->z);
		PSMTXScale(scale, ptr->scaleX, ptr->scaleY, ptr->scaleZ);
		if (i != 1) {
			PSMTXRotRad(rot, 'z', (ptr->angle - float_90_8041cad8) * float_deg2rad_8041cad0);
		} else {
			PSMTXRotRad(rot, 'z', float_0_8041cadc);
		}
		PSMTXConcat(base, mtx, mtx);
		PSMTXConcat(mtx, scale, mtx);
		PSMTXConcat(mtx, rot, mtx);
		PSMTXConcat(cam->view, mtx, mtx);
		GXLoadPosMtxImm(mtx, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		t = float_1_8041cae0;
		s = float_0_8041cadc;
		if (i == 1) {
			s = float_8_8041cae4;
		}
		if (i != 1) {
			t = float_2_8041cae8;
		}
		GXPosition3f32(y * float_0p5_8041caec, float_16_8041caf0 + s, float_0_8041cadc);
		GXTexCoord2f32(float_0_8041cadc, float_0_8041cadc);
		GXPosition3f32(float_16_8041caf0, float_16_8041caf0 + s, float_0_8041cadc);
		GXTexCoord2f32(t, float_0_8041cadc);
		GXPosition3f32(float_16_8041caf0, y * float_0p5_8041caec + s, float_0_8041cadc);
		GXTexCoord2f32(t, one);
		GXPosition3f32(y * float_0p5_8041caec, y * float_0p5_8041caec + s, float_0_8041cadc);
		GXTexCoord2f32(float_0_8041cadc, one);
	}
}
