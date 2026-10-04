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

extern const f32 float_108_8041cafc;
extern const f32 float_0p5_8041caec;

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

// 100%: the colour strings are string literals (not named `const char[]` objects), so the compiler pools
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
	data->field_40 = -30.0f;

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
		ptr->x = 0.0f;
		switch (type) {
		case 4:
		case 5:
			ptr->y = (i < 10) ? 0.0f : -15.0f;
			break;
		default:
			ptr->y = 0.0f;
			break;
		}
		ptr->z = 0.0f;
		ptr->scaleX = 0.0f;
		ptr->scaleY = 0.0f;
		ptr->scaleZ = 0.0f;
		ptr->velX = 0.0f;
		ptr->velY = 0.0f;
		ptr->velZ = 0.0f;
		ptr->timer = 0;
		ptr->state = 0;
		ptr->color = 0xFF;
		ptr->angle = 0.0f;
		switch (type) {
		case 6:
			if (i == 1) {
				ptr->scaleZ = 1.5f;
				ptr->scaleY = 1.5f;
				ptr->scaleX = 1.5f;
				ptr->color = 0;
				ptr->delay = 0;
			} else {
				ptr->scaleZ = 0.3f;
				ptr->scaleY = 0.3f;
				ptr->scaleX = 0.3f;
				if (i & 1) {
					angle = irand(0x50);
				} else {
					angle = -irand(0x50);
				}
				angle += 0x5A;
				ptr->angle = angle;
				ptr->velX = 1.8f * (f32)cos(6.2831855f * ptr->angle / 360.0f);
				ptr->velY = 1.8f * (f32)sin(6.2831855f * ptr->angle / 360.0f);
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
					f32 ang = ((f32)(i - 1) * 360.0f / (f32)field_1 + 90.0f) * 6.2831855f / 360.0f;
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
						f32 ang = ((f32)(i - 2) * 360.0f / (f32)field_1 + t + 90.0f) * 6.2831855f / 360.0f;
						radius *= 0.99f;
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
// them the compiler hoists the vec3_802f3544 loads into the callee-save prologue, ahead of the
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
		effStardustEntry(2, 6, 0x1E, ptr->x, ptr->y, ptr->z, 40.0f, 45.0f);
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
		ptr->velX *= 0.95f;
		ptr->velY *= 0.95f;
		ptr->velZ *= 0.95f;
		if (i == 1) {
			ptr->color += 5;
			if (ptr->color > 0x80) {
				ptr->color = 0x80;
			}
		} else {
			ptr->color -= 8;
			if (ptr->color < 0) {
				f32 zero = 0.0f;
				ptr->x = zero;
				ptr->y = zero;
				ptr->z = zero;
				ptr->velX = (f32)cos(6.2831855f * ptr->angle / 360.0f) * 2.0f;
				ptr->velY = (f32)sin(6.2831855f * ptr->angle / 360.0f) * 2.0f;
				ptr->color = 0xFF;
			}
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&disp), 2, effRecoveryDisp4, effect);
}

// The vertex float literals (-16/16 on the leading vertices, -6/6/2 in the loop) keep the compiler from
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
	PSMTXRotRad(tmp, 'y', -camGetPtr(camId)->viewYaw * 0.017453292f);
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
	PSMTXRotRad(tmp, 'y', -camGetPtr(camId)->viewYaw * 0.017453292f);
	PSMTXConcat(mtx, tmp, mtx);
	PSMTXScale(scale, 10.0f, 10.0f, 10.0f);
	PSMTXConcat(mtx, scale, mtx);
	PSMTXTrans(scale, 0.0f, 2.0f, 0.0f);
	PSMTXConcat(mtx, scale, mtx);
	if ((u32)type <= 1 || (u32)(type - 7) <= 1 || type == 4 || type == 5) {
		PSMTXConcat(mtx, ptr->anim->mtx, mtx);
	}
	PSMTXConcat(cam->view, mtx, mtx);
	PSMTXScale(scale, 0.1f, 0.1f, 0.1f);
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
	GXPosition3f32((-16.0f), (16.0f), 0.0f);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3f32((16.0f), (16.0f), 0.0f);
	GXTexCoord2f32(1.0f, 0.0f);
	GXPosition3f32((16.0f), (-16.0f), 0.0f);
	GXTexCoord2f32(1.0f, 1.0f);
	GXPosition3f32((-16.0f), (-16.0f), 0.0f);
	GXTexCoord2f32(0.0f, 1.0f);
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
		GXPosition3f32(-6.0f, 6.0f, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(6.0f, 6.0f, 0.0f);
		GXTexCoord2f32(2.0f, 0.0f);
		GXPosition3f32(6.0f, -6.0f, 0.0f);
		GXTexCoord2f32(2.0f, 1.0f);
		GXPosition3f32(-6.0f, -6.0f, 0.0f);
		GXTexCoord2f32(0.0f, 1.0f);
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
	f32 k24;
	f32 k05;

	camGetPtr(camId);
	num = data->field_1;
	type = data->type;
	sprintf(buf, "%d", num);
	len = strlen(buf);
	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', -camGetPtr(camId)->viewYaw * 0.017453292f);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXTrans(tmp, 0.0f, 0.0f, 1.0f);
	PSMTXConcat(mtx, tmp, mtx);
	PSMTXScale(tmp, 10.0f, 10.0f, 10.0f);
	PSMTXConcat(mtx, tmp, mtx);
	PSMTXTrans(tmp, 0.0f, 2.0f, 0.0f);
	PSMTXConcat(mtx, tmp, mtx);
	PSMTXConcat(mtx, data->anim->mtx, mtx);
	PSMTXScale(tmp, 0.035f, 0.035f, 0.035f);
	PSMTXConcat(mtx, tmp, mtx);
#ifdef MUST_MATCH
	// split the x scale into a temp: keeps the int->float conversion and *24 in the original's registers
	k24 = 24.0f;
	k05 = 0.5f;
	transX = k24 * (f32)(-(len - 1));
	PSMTXTrans(tmp, transX * k05, 0.0f, 0.0f);
#else
	PSMTXTrans(tmp, 24.0f * -(len - 1) * 0.5f, 0.0f, 0.0f);
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
// The half-size block below is under MUST_MATCH: writing it as one expression makes the compiler load the pool
// constant into f2 and negate into f1, swapping the target's f1/f2. Declaring the negated value and giving it
// a dead use after the fadds extends its live range so the compiler uses the target's f1 (constant) / f2 (negated temp).
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
	f32 p108;
	f32 half05;
	f32 negHalf;

	cam = camGetPtr(camId);
	type = data->type;
	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXRotRad(tmp, 'y', -camGetPtr(camId)->viewYaw * 0.017453292f);
	PSMTXConcat(mtx, tmp, mtx);
	PSMTXTrans(tmp2, 0.0f, 0.0f, 1.0f);
	PSMTXConcat(mtx, tmp2, mtx);
	PSMTXScale(tmp2, 10.0f, 10.0f, 10.0f);
	PSMTXConcat(mtx, tmp2, mtx);
	PSMTXTrans(tmp2, 0.0f, 2.0f, 0.0f);
	PSMTXConcat(mtx, tmp2, mtx);
	PSMTXConcat(mtx, data->anim->mtx, base);
	PSMTXConcat(cam->view, base, base);
	PSMTXScale(tmp2, 0.05f, 0.05f, 0.05f);
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
		// Read the two loop-invariant constants through their named objects so the original's
		// f1/f0/f2 register assignment comes out; the values are the target's 108 and 0.5.
		f32 negHalf = -float_108_8041cafc;
		f30 = negHalf * float_0p5_8041caec;
		f29 = f30 + float_108_8041cafc;
		f27 = negHalf;
	}
#else
	f30 = -108.0f * 0.5f;
	f29 = f30 + 108.0f;
#endif
#ifndef MUST_MATCH
	f27 = 20.0f;
	f28 = -20.0f;
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
		PSMTXTrans(mtx, (f32)((1 - i) * 2), (f32)(-(1 - i) * 2), 0.0f);
		PSMTXConcat(base, mtx, mtx);
		GXLoadPosMtxImm(mtx, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
#ifdef MUST_MATCH
		// Assigning the loop-invariant +-20 pair inside the loop (not above it) makes the compiler hoist it
		// together with the int->float magic instead of into the loop preheader, matching the original's
		// lfd/f27/f28 load order. It changes no value; without it only the load order differs.
		f27 = 20.0f;
		f28 = -20.0f;
#endif
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(f30, f27, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(f29, f27, 0.0f);
		GXTexCoord2f32(1.0f, 0.0f);
		GXPosition3f32(f29, f28, 0.0f);
		GXTexCoord2f32(1.0f, 1.0f);
		GXPosition3f32(f30, f28, 0.0f);
		GXTexCoord2f32(0.0f, 1.0f);
	}
}

#ifdef MUST_MATCH
// Match-only: the original inlined both colour builders. Passing the GXColor by value through a
// helper in this shape is what places the colour stack slots exactly as the original (else build
// 0xc / outgoing copy 0x8, i==1 build 0x10 / outgoing copy 0x14).
static inline GXColor rcA(u8 a, GXColor c) {
	c.a = a;
	return c;
}
static inline GXColor rcC(s32 idx, u8 a, GXColor c) {
	c.r = color_rotation_data[idx * 3 + 0];
	c.g = color_rotation_data[idx * 3 + 1];
	c.b = color_rotation_data[idx * 3 + 2];
	c.a = a;
	return c;
}
#endif

#pragma opt_loop_invariants off // keep `half = y * 0.5f` in the loop so 0.5 pools after the vertex sizes
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
	volatile GXColor saved;
#endif
	GXTexObj tex;
	s32 i;
	s32 color;
	f32 y;
	f32 s;
	f32 t;
	f32 one = 1.0f;
#ifdef MUST_MATCH
	// Match-only: naming the half-offset keeps the compiler from commuting the fadds that adds it to s.
	f32 half;
	f32 k05;
	f32 k16;
#endif

	cam = camGetPtr(camId);
	color = ptr->color;
	PSMTXTrans(mtx, ptr->x, ptr->y, ptr->z);
	PSMTXRotRad(rot, 'y', -camGetPtr(camId)->viewYaw * 0.017453292f);
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
		{
		f32 p32 = 32.0f;
		y = -p32;
	}
	i = 1;
	ptr++;
	for (; i < entry->count; i++, ptr++) {
		if (ptr->delay != 0) {
			continue;
		}
		if (i == 1) {
#ifdef MUST_MATCH
			effGetTexObj(0x12, &tex);
			GXLoadTexObj(&tex, GX_TEXMAP0);
			GXSetChanMatColor(GX_COLOR0A0, rcA((color * ptr->color) / 255, *(GXColor*)&dat_8041cacc));
#else
			GXColor chanColor;
			effGetTexObj(0x12, &tex);
			GXLoadTexObj(&tex, GX_TEXMAP0);
			*(u32*)&chanColor = dat_8041cacc;
			chanColor.a = (color * ptr->color) / 255;
			GXSetChanMatColor(GX_COLOR0A0, chanColor);
#endif
		} else {
			s32 idx = (i + gp->retraceCount) % 12;
#ifdef MUST_MATCH
			saved = rcC(idx, (color * ptr->color) / 255, *(GXColor*)&unk_80423044);
			effGetTexObj(0x14, &tex);
			GXLoadTexObj(&tex, GX_TEXMAP0);
			GXSetChanMatColor(GX_COLOR0A0, saved);
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
			PSMTXRotRad(rot, 'z', (ptr->angle - 90.0f) * 0.017453292f);
		} else {
			PSMTXRotRad(rot, 'z', 0.0f);
		}
		PSMTXConcat(base, mtx, mtx);
		PSMTXConcat(mtx, scale, mtx);
		PSMTXConcat(mtx, rot, mtx);
		PSMTXConcat(cam->view, mtx, mtx);
		GXLoadPosMtxImm(mtx, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		t = 1.0f;
		s = 0.0f;
		if (i == 1) {
			s = 8.0f;
		}
		if (i != 1) {
			t = 2.0f;
		}
#ifdef MUST_MATCH
		k05 = 0.5f;
		k16 = 16.0f;
		GXPosition3f32(y * k05, k16 + s, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(k16, k16 + s, 0.0f);
		GXTexCoord2f32(t, 0.0f);
		half = y * k05;
		GXPosition3f32(k16, half + s, 0.0f);
		GXTexCoord2f32(t, one);
		GXPosition3f32(half, half + s, 0.0f);
#else
		GXPosition3f32(y * 0.5f, 16.0f + s, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(16.0f, 16.0f + s, 0.0f);
		GXTexCoord2f32(t, 0.0f);
		GXPosition3f32(16.0f, y * 0.5f + s, 0.0f);
		GXTexCoord2f32(t, one);
		GXPosition3f32(y * 0.5f, y * 0.5f + s, 0.0f);
#endif
		GXTexCoord2f32(0.0f, one);
	}
}
#pragma opt_loop_invariants reset

// .sdata objects (0x80411BE8..0x80411C60), after the code in the original's address order.
extern u32 recovery_effHermit_0[445];
u32 recovery_effFace_pPlane2_q007_0[11] = {
	0x0004FD00, 0xFD000000, 0x00000200, 0x0300FD00, 0x00000200, 0x02000300, 0x03000000, 0x02000000,
	0xFD000300, 0x00000000, 0x00000000,
};
u32 recovery_effFaceDt_pPlane2_q007[1] = { (u32)recovery_effFace_pPlane2_q007_0 };
u32 recovery_effAnimRST[4] = {
	0x0000000A, (u32)recovery_effHermit_0, 0x00000001, (u32)recovery_effFaceDt_pPlane2_q007,
};
u32 recovery_effAnimRSTDt[2] = { 0x00000001, (u32)recovery_effAnimRST };
u32 recovery_effAnimDt[3] = { 0x00000000, 0x429E0000, (u32)recovery_effAnimRSTDt };

// .data animation joint data (0x8036F628..0x8036FD1C).
u32 recovery_effHermit_0[445] = {
	0x00000000, 0x00000000, 0x00000140, 0x0000FFDD, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000007, 0x00000000, 0x00000007,
	0x00000000, 0x00000007, 0x00000000, 0x00000000, 0x0000FFA1, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x0000FFA1, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000009, 0x00000000, 0x00000000, 0x0000FFEA,
	0xFFEA0000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00400005, 0x00050000, 0x00400002, 0x00020000, 0x00400004, 0x00040000, 0x00000000,
	0x00000000, 0xFFA10000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFA10000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x000E0000, 0x00000000, 0x00000000, 0x00040004, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000053, 0xFFFFFFFF, 0x00000020,
	0x00010001, 0x00000040, 0x00000000, 0x00000000, 0x00000000, 0x0000FFA1, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x0000FFA1, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000013, 0x00000000, 0x00000000,
	0x002C0000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x0033FFFF, 0xFFFF0000, 0x004C0003, 0x00030000, 0x00400000, 0x00000000,
	0x00000000, 0x00000000, 0xFFA10000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFA10000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00180000, 0x00000000, 0x00000000, 0xFFFDFFFE, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000040, 0x00000000,
	0x00000040, 0x00000000, 0x00000040, 0x00000000, 0x00000000, 0x00000000, 0x0000FFA1, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x0000FFA1, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000022, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0xFFF50000, 0x00000000, 0x00400000, 0x00000000, 0x00400000, 0x00000000, 0x00400000,
	0x00000000, 0x00000000, 0x00000000, 0xFFA10000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFA10000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00310000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0000000B, 0x00000000, 0x00000040,
	0x00000000, 0x00000040, 0x00000000, 0x00000040, 0x00000000, 0x00000000, 0x00000000, 0x0000FFA1,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x0000FFA1, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000036,
	0x00000000, 0x00000000, 0x0000FFFF, 0xFFFF0000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00400000, 0x00000000, 0x0040FFFF, 0xFFFF0000,
	0x00400000, 0x00000000, 0x00000000, 0x00000000, 0xFFA10000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0xFFA10000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00450000, 0x00000000, 0x0000FFE0, 0x00140014,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x0000004C, 0xFFFEFFFE, 0x00000026, 0x00010001, 0x00000040, 0xFFFEFFFE, 0x00000000, 0x00000000,
	0x0000FFA1, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0000FFA1, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x0000004F, 0x00000000, 0x00000000, 0x02000036, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0000FFF9, 0x00000000, 0x00600005,
	0x00000000, 0x0000FFFA, 0x00000000, 0x00000000, 0x00000000, 0xFFA10000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0xFFA10000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// .sdata2 objects (0x8041CAC0..0x8041CB5C), defined after the code.
const u32 dat_8041cac0 = 0x000000A0;
const u32 dat_8041cac4 = 0xF5989DFF;
const u32 dat_8041cac8 = 0x5DC100FF;
const u32 dat_8041cacc = 0xFFFFFF00;

#pragma section ".sdata" ".sbss2" // put the two scratch colours in .sbss2
__declspec(section ".sdata") GXColor unk_80423044; // original is in .sbss2; section redirect above
__declspec(section ".sdata") GXColor unk_80423040; // declared in the original's reverse address order
#pragma section ".sdata" ".sbss" // restore .sbss for later zero-initialised objects

// 108 and 0.5 are emitted before the pool; the code reads them by name.
__declspec(section ".sdata2") const f32 float_108_8041cafc = 108.0f;
__declspec(section ".sdata2") const f32 float_0p5_8041caec = 0.5f;

// Zero tail words the original section has after its last object (the next unit's first word).
#pragma explicit_zero_data on // emit the zero-valued objects below
#pragma force_active on // keep them even though nothing references them
__declspec(section ".data") static u32 gap_77C_8036FD18_data[1] = { 0 };
#pragma force_active reset
#pragma explicit_zero_data reset
