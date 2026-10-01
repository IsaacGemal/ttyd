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
extern u8 recovery_effAnimDt[];

// .rodata (0x802F3508..0x802F3678)
const Vec vec3_802f3508 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3514 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3520 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f352c = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3538 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3544 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_802f3550 = 4503601774854144.0;
const char str_802F3558[0x18] = "SFX_HP_RECOVER_SHINE1";
const char str_802F3570[0x18] = "SFX_HP_RECOVER_SHINE2";
const char str_802F3588[0x18] = "SFX_HP_RECOVER_SHINE3";
const char str_802F35A0[0x18] = "SFX_FP_RECOVER_SHINE1";
const char str_802F35B8[0x18] = "SFX_FP_RECOVER_SHINE2";
const char str_802F35D0[0x18] = "SFX_FP_RECOVER_SHINE3";
const char str_802F35E8[0xC] = "Recovery";
const char str_802F35F4[0x18] = "SFX_HP_RECOVER_SMALL1";
const char str_802F360C[0x14] = "SFX_HP_RECOVER_MID1";
const char str_802F3620[0x14] = "SFX_HP_RECOVER_BIG1";
const char str_802F3634[0x18] = "SFX_FP_RECOVER_SMALL1";
const char str_802F364C[0x14] = "SFX_FP_RECOVER_MID1";
const char str_802F3660[0x14] = "SFX_FP_RECOVER_BIG1";

const char str_8041CB08[] = "%d";

// NONMATCHING (71.68%): register allocation in the element loop and the
// recovery_effAnimDt reference (lis/addi instead of li @sda21) differ.
EffectEntry* effRecoveryEntry(s32 type, s32 count, f32 x, f32 y, f32 z) {
	EffectEntry* effect;
	RecoveryData* data;
	RecoveryData* ptr;
	Vec pos;
	Vec disp;
	s32 field;
	s32 i;
	s32 n;
	s32 angle;

	effect = effEntry();
	field = count;
	switch (type) {
	case 6:
		field = 0xB;
		break;
	case 4:
	case 5:
		field = 0x14;
		break;
	case 2:
	case 3:
		field = 0;
		break;
	}
	effect->type = str_802F35E8;
	effect->count = field + 1;
	data = __memAlloc(HEAP_EFFECT, effect->count * 0x44);
	effect->userdata = data;
	if (type == 6) {
		effect->callback = effRecoveryMain2;
	} else {
		effect->callback = effRecoveryMain;
	}
	data->anim = effMayaAnimAlloc(recovery_effAnimDt);
	effPlayMayaAnim(data->anim);
	data->type = type;
	data->field_1 = field;
	data->x = x;
	data->y = y;
	data->z = z;
	data->timer = 0;
	data->timeBase = gp->retraceCount;
	data->color = 0xFF;
	data->field_40 = float_neg30_8041cb48;

	pos = vec3_802f3508;
	pos.x = data->x;
	pos.y = data->y;
	pos.z = data->z;
	disp = pos;

	switch (type) {
	case 0:
	case 7:
		if (field <= 5) {
			psndSFXOn_3D(str_802F35F4, &disp);
		} else if (field <= 0xA) {
			psndSFXOn_3D(str_802F360C, &disp);
		} else {
			psndSFXOn_3D(str_802F3620, &disp);
		}
		break;
	case 1:
	case 8:
		if (field <= 5) {
			psndSFXOn_3D(str_802F3634, &disp);
		} else if (field <= 0xA) {
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

	if (type == 6) {
		effDeleteMayaAnim(data->anim);
		data->timer = 0xB4;
	}

	n = 8;
	ptr = data + 1;
	for (i = 1; i < effect->count; i++, ptr++) {
		ptr->x = float_0_8041cadc;
		if (type >= 4 && type < 6) {
			ptr->y = (i < 10) ? float_0_8041cadc : float_neg15_8041cb4c;
		} else {
			ptr->y = float_0_8041cadc;
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
		if (type == 6) {
			if (i == 1) {
				ptr->scaleX = float_1p5_8041cb50;
				ptr->scaleY = float_1p5_8041cb50;
				ptr->scaleZ = float_1p5_8041cb50;
				ptr->color = 0;
				ptr->delay = 0;
			} else {
				ptr->scaleX = float_0p3_8041cb54;
				ptr->scaleY = float_0p3_8041cb54;
				ptr->scaleZ = float_0p3_8041cb54;
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
		}
		n += 8;
	}
	return effect;
}

// NONMATCHING (79.98%): state-machine register allocation and the tail
// dispEntry slots differ from the target.
void effRecoveryMain(EffectEntry* effect) {
	RecoveryData* data = effect->userdata;
	RecoveryData* ptr;
	Vec pos;
	s8 type;
	s32 field;
	s32 field_1;
	s32 srcState;
	s32 total;
	s32 i;
	f32 f31;
	f32 f30;
	f32 f29;

	type = data->type;
	f31 = data->x;
	f30 = data->y;
	f29 = data->z;
	srcState = 0;
	total = 0;
	field = 0;
	effCalcMayaAnim(data->anim);
	if (gp->retraceCount > data->timeBase) {
		field = gp->retraceCount - data->timeBase;
	}
	if (field < 1) {
		field = 1;
	}
	while (field != 0) {
		field--;
		data = effect->userdata;
		if (type == 4 || type == 5) {
			field_1 = 10;
		} else {
			field_1 = data->field_1;
		}
		data->timer++;
		if (data->timer > 10) {
			srcState = 1;
		}
		for (i = 1, ptr = data + 1; i < effect->count; i++, ptr++) {
			switch (ptr->state) {
			case 0:
				if (srcState == 0) {
					break;
				}
				{
					f32 ang = ((f32)(i - 1) * 360.0f / (f32)field_1 + 90.0f) * float_6p2832_8041cb30 / 360.0f;
					ptr->velX = 12.0f * (f32)sin(ang);
					ptr->velY = 0.0f;
					ptr->velZ = -12.0f * (f32)cos(ang);
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
					ptr->scaleX = 1.0f;
					ptr->scaleY = 1.0f;
					ptr->scaleZ = 1.0f;
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
					f32 t;
					ptr->timer++;
					t = intplGetValue(0xB, ptr->timer, 0x28, 0.0f, 360.0f);
					{
						f32 radius = distABf(0.0f, 0.0f, ptr->x, ptr->z) * 0.99f;
						f32 ang = ((f32)(i - 2) * 360.0f / (f32)field_1 + t + 90.0f) * float_6p2832_8041cb30 / 360.0f;
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
					ptr->y = intplGetValue(1, ptr->timer, 0x10, ptr->y, data->field_40);
					ptr->z = intplGetValue(1, ptr->timer, 0x14, ptr->z, 0.0f);
					s = intplGetValue(1, ptr->timer, 0x10, 1.0f, 0.0f);
					ptr->scaleX = s;
					ptr->scaleY = s;
					ptr->scaleZ = s;
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
			if (type != 7 && type != 8) {
				effStardustEntry(2, 2, 0x1E, f31, f30 - 20.0f, f29, 30.0f, 30.0f);
				pos = vec3_802f3514;
				pos.x = f31;
				pos.y = f30;
				pos.z = f29;
				if (type == 4) {
					psndSFXOn_3D(str_802F3588, &pos);
				} else if (type == 5) {
					psndSFXOn_3D(str_802F35D0, &pos);
				} else if (type == 0) {
					if (field_1 <= 5) {
						psndSFXOn_3D(str_802F3558, &pos);
					} else if (field_1 <= 10) {
						psndSFXOn_3D(str_802F3570, &pos);
					} else {
						psndSFXOn_3D(str_802F3588, &pos);
					}
				} else {
					if (field_1 <= 5) {
						psndSFXOn_3D(str_802F35A0, &pos);
					} else if (field_1 <= 10) {
						psndSFXOn_3D(str_802F35B8, &pos);
					} else {
						psndSFXOn_3D(str_802F35D0, &pos);
					}
				}
			}
			effDeleteMayaAnim(((RecoveryData*)effect->userdata)->anim);
			effDelete(effect);
			return;
		}
	}
	data = effect->userdata;
	data->timeBase = gp->retraceCount;
	pos = vec3_802f3520;
	pos.x = f31;
	pos.y = f30;
	pos.z = f29;
	dispEntry(CAMERA_3D, dispCalcZ(&pos), 1, effRecoveryDisp, effect);
	if ((u32)type <= 1 || type == 7 || type == 8) {
		pos = vec3_802f352c;
		pos.x = f31;
		pos.y = f30;
		pos.z = f29;
		dispEntry(CAMERA_3D, dispCalcZ(&pos), 2, effRecoveryDisp2, effect);
	}
	if (type == 4 || type == 5) {
		pos = vec3_802f3538;
		pos.x = f31;
		pos.y = f30;
		pos.z = f29;
		dispEntry(CAMERA_3D, dispCalcZ(&pos), 2, effRecoveryDisp3, effect);
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
	pos = *(const volatile Vec*)&vec3_802f3544;
#else
	pos = vec3_802f3544;
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

// NONMATCHING (81.32%): several matrix-stack slot choices and the second
// render pass differ from the target.
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
	s8 type;
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
	ptr = data;
	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXRotRad(tmp, 'y', -camGetPtr(camId)->viewYaw * float_deg2rad_8041cad0);
	PSMTXConcat(mtx, tmp, mtx);
	PSMTXScale(scale, float_10_8041caf4, float_10_8041caf4, float_10_8041caf4);
	PSMTXConcat(mtx, scale, mtx);
	PSMTXTrans(scale, float_0_8041cadc, float_2_8041cae8, float_0_8041cadc);
	PSMTXConcat(mtx, scale, mtx);
	if ((u32)type <= 1 || (u32)(type - 7) <= 1 || type == 4 || type == 5) {
		PSMTXConcat(mtx, data->anim->mtx, mtx);
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
	GXPosition3f32(float_neg16_8041cb18, float_16_8041caf0, float_0_8041cadc);
	GXTexCoord2f32(float_0_8041cadc, float_0_8041cadc);
	GXPosition3f32(float_16_8041caf0, float_16_8041caf0, float_0_8041cadc);
	GXTexCoord2f32(float_1_8041cae0, float_0_8041cadc);
	GXPosition3f32(float_16_8041caf0, float_neg16_8041cb18, float_0_8041cadc);
	GXTexCoord2f32(float_1_8041cae0, float_1_8041cae0);
	GXPosition3f32(float_neg16_8041cb18, float_neg16_8041cb18, float_0_8041cadc);
	GXTexCoord2f32(float_0_8041cadc, float_1_8041cae0);
	if (type == 2 || type == 3) {
		return;
	}
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
	for (i = 1, ptr = data + 1; i < entry->count; i++, ptr++) {
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
		GXPosition3f32(float_neg6_8041cb1c, float_6_8041cb20, float_0_8041cadc);
		GXTexCoord2f32(float_0_8041cadc, float_0_8041cadc);
		GXPosition3f32(float_6_8041cb20, float_6_8041cb20, float_0_8041cadc);
		GXTexCoord2f32(float_2_8041cae8, float_0_8041cadc);
		GXPosition3f32(float_6_8041cb20, float_neg6_8041cb1c, float_0_8041cadc);
		GXTexCoord2f32(float_2_8041cae8, float_1_8041cae0);
		GXPosition3f32(float_neg6_8041cb1c, float_neg6_8041cb1c, float_0_8041cadc);
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

// NONMATCHING (78.53%): frame is 0x120 vs the target 0x130 (one extra callee-
// saved GPR), so every stack-relative matrix offset is 4 bytes low.
void effRecoveryDisp4(CameraId camId, void* param) {
	EffectEntry* entry = param;
	RecoveryData* data = entry->userdata;
	RecoveryData* ptr;
	CameraEntry* cam;
	Mtx mtx;
	Mtx rot;
	Mtx scale;
	Mtx base;
	GXTexObj tex;
	s32 color;
	s32 i;
	f32 y;
	f32 s;
	f32 t;

	cam = camGetPtr(camId);
	color = data->color;
	PSMTXTrans(mtx, data->x, data->y, data->z);
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
	for (i = 1, ptr = data + 1; i < entry->count; i++, ptr++) {
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
			GXColor chanColor = *(GXColor*)&unk_80423044;
			s32 idx = (i + gp->retraceCount) % 12;
			effGetTexObj(0x14, &tex);
			chanColor.r = color_rotation_data[idx * 3 + 0];
			chanColor.g = color_rotation_data[idx * 3 + 1];
			chanColor.b = color_rotation_data[idx * 3 + 2];
			chanColor.a = (color * ptr->color) / 255;
			GXLoadTexObj(&tex, GX_TEXMAP0);
			GXSetChanMatColor(GX_COLOR0A0, chanColor);
		}
		PSMTXTrans(mtx, ptr->x, ptr->y, ptr->z);
		PSMTXScale(scale, ptr->scaleX, ptr->scaleY, ptr->scaleZ);
		if (i == 1) {
			PSMTXRotRad(rot, 'z', float_0_8041cadc);
		} else {
			PSMTXRotRad(rot, 'z', (ptr->angle - float_90_8041cad8) * float_deg2rad_8041cad0);
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
		GXTexCoord2f32(t, float_1_8041cae0);
		GXPosition3f32(y * float_0p5_8041caec, y * float_0p5_8041caec + s, float_0_8041cadc);
		GXTexCoord2f32(float_0_8041cadc, float_1_8041cae0);
	}
}
