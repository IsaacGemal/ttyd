#include "eff/eff_gonbaba_breath.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <math.h>

GXColor col_normal[5] = {
	{ 0xF2, 0x65, 0x22, 0xFF },
	{ 0xF6, 0x86, 0x1E, 0xFF },
	{ 0xF6, 0xAC, 0x1E, 0xFF },
	{ 0xFF, 0xF5, 0x68, 0xFF },
	{ 0x96, 0x5E, 0x1A, 0xFF },
};

GXColor col_poison[5] = {
	{ 0x67, 0x85, 0x1C, 0xFF },
	{ 0x77, 0x98, 0x26, 0xFF },
	{ 0x89, 0xAF, 0x2B, 0xFF },
	{ 0xC8, 0xE5, 0x05, 0xFF },
	{ 0x85, 0x70, 0x1C, 0xFF },
};

GXColor col_mega[5] = {
	{ 0xD6, 0xCF, 0x0A, 0xFF },
	{ 0xEB, 0xEB, 0x3E, 0xFF },
	{ 0xFF, 0xFF, 0xB4, 0xFF },
	{ 0xF7, 0xF7, 0xD7, 0xFF },
	{ 0xA9, 0xA3, 0x04, 0xFF },
};

GXColor col_cold[5] = {
	{ 0x19, 0xBC, 0xF1, 0xFF },
	{ 0x8B, 0xDE, 0xF9, 0xFF },
	{ 0xD1, 0xF1, 0xFC, 0xFF },
	{ 0xE1, 0xF5, 0xFC, 0xFF },
	{ 0x2D, 0x77, 0xCA, 0xFF },
};

GXColor col_confu[5] = {
	{ 0x24, 0xCA, 0x67, 0xFF },
	{ 0x37, 0xE3, 0x7D, 0xFF },
	{ 0x49, 0xF2, 0x8D, 0xFF },
	{ 0xC2, 0xFC, 0xEB, 0xFF },
	{ 0x3E, 0x87, 0x2B, 0xFF },
};

GXColor col_sleep[5] = {
	{ 0xD3, 0xCA, 0xD2, 0xFF },
	{ 0xEF, 0xE3, 0xEE, 0xFF },
	{ 0xFC, 0xF6, 0xFC, 0xFF },
	{ 0xF6, 0xF4, 0xF6, 0xFF },
	{ 0xA1, 0x8F, 0x9F, 0xFF },
};

GXColor col_mini[5] = {
	{ 0xAE, 0x69, 0xE0, 0xFF },
	{ 0xBF, 0x7C, 0xF0, 0xFF },
	{ 0xCD, 0x97, 0xF5, 0xFF },
	{ 0xF3, 0xC0, 0xFA, 0xFF },
	{ 0x81, 0x31, 0xBB, 0xFF },
};

static GXColor* col_tbl[8] = {
	col_normal,
	col_poison,
	col_mega,
	col_cold,
	col_confu,
	col_sleep,
	col_mini,
	col_normal,
};

// Local copy of system.c's inline helper so it inlines here (as in eff_queen.c).
inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

// NONMATCHING (92.05%): sin/cos polynomial is one FPR off (radian in f0 vs f6, coefficients in f6/f7/f8).
// `#pragma opt_propagation off` required: stops MWCC rematerialising the sin/cos coefficients per branch.
#pragma opt_propagation off
void init_breath(effGonbabaBreathData* data, s32 index, s32 type) {
	f32 f0;
	f32 f1;
	f32 f2;
	f32 f3;
	f32 f4;
	f32 f5;
	f32 f6;
	f32 f7;
	f32 angle;
	f32 speed;
	GXColor* color;

	data->type = type;
	switch (type) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
		data->field_0x44 = index << 2;
		data->position.x = 0.0f;
		data->position.y = 0.0f;
		data->position.z = 0.0f;
		data->scale = 0.0f;
		data->velX = -2.4f;
		data->velY = 0.1f;
		data->lifetime = 0x40;
		data->alpha = 0xFF;
		break;
	case 7:
		data->field_0x44 = index << 3;
		data->position.x = 0.0f;
		data->position.y = 0.0f;
		data->position.z = 0.0f;
		data->scale = 0.0f;
		data->velX = -2.0f;
		data->velY = 0.1f;
		data->lifetime = 0x40;
		data->alpha = 0xFF;
		break;
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
		angle = 240.0f + (f32)irand(60);
		f7 = ((f32)(irand(50) + 10) / 10.0f) * 1.5f;
		f6 = (6.2831855f * angle) / 360.0f;
		data->field_0x44 = index;
		data->position.x = 0.0f;
		data->position.y = 0.0f;
		data->position.z = 0.0f;

		f1 = 3.1415927f;
		f5 = 0.00761f;
		f4 = 0.16605f;
		f3 = 1.0f;
		if (f6 <= f1) {
			f1 = 1.5707964f;
			if (f6 <= f1) {
				f0 = f6 * f6;
				f5 = f5 * f0 - f4;
				f5 = f5 * f0 + f3;
				f5 = f5 * f6;
			} else {
				f0 = f6 - f1;
				f1 = f1 - f0;
				f0 = f1 * f1;
				f5 = f5 * f0 - f4;
				f5 = f5 * f0 + f3;
				f5 = f5 * f1;
			}
		} else {
			f0 = 4.712389f;
			if (f6 < f0) {
				f1 = f6 - f1;
				f0 = f1 * f1;
				f5 = f5 * f0 - f4;
				f5 = f5 * f0 + f3;
				f5 = f5 * f1;
				f5 = -f5;
			} else {
				f0 = f6 - f0;
				f1 = 1.5707964f;
				f1 = f1 - f0;
				f0 = f1 * f1;
				f5 = f5 * f0 - f4;
				f5 = f5 * f0 + f3;
				f5 = f5 * f1;
				f5 = -f5;
			}
		}
		data->velX = f7 * f5;

		f1 = 3.1415927f;
		f4 = 0.03705f;
		f3 = 0.4967f;
		f2 = 1.0f;
		if (f6 <= f1) {
			f1 = 1.5707964f;
			if (f6 <= f1) {
				f0 = f6 * f6;
				f4 = f4 * f0 - f3;
				f4 = f4 * f0 + f2;
			} else {
				f0 = f6 - f1;
				f0 = f1 - f0;
				f0 = f0 * f0;
				f4 = f4 * f0 - f3;
				f4 = f4 * f0 + f2;
				f4 = -f4;
			}
		} else {
			f0 = 4.712389f;
			if (f6 < f0) {
				f0 = f6 - f1;
				f0 = f0 * f0;
				f4 = f4 * f0 - f3;
				f4 = f4 * f0 + f2;
				f4 = -f4;
			} else {
				f0 = f6 - f0;
				f1 = 1.5707964f;
				f0 = f1 - f0;
				f0 = f0 * f0;
				f4 = f4 * f0 - f3;
				f4 = f4 * f0 + f2;
			}
		}
		data->velY = f7 * f4;
		data->scale = 1.0f;
		data->lifetime = 0x1E;
		data->alpha = 0xFF;
		color = col_tbl[type % 10];
		data->colorR = color[3].r;
		data->colorG = color[3].g;
		data->colorB = color[3].b;
		data->targetR = color[0].r;
		data->targetG = color[0].g;
		data->targetB = color[0].b;
		break;
	case 17:
		angle = 225.0f + (f32)irand(90);
		speed = ((f32)(irand(50) + 10) / 10.0f) * 1.5f;
		data->field_0x44 = irand(100);
		f6 = (6.2831855f * angle) / 360.0f;
		data->position.x = 0.0f;
		data->position.y = 0.0f;
		data->position.z = 0.0f;

		f1 = 3.1415927f;
		f5 = 0.00761f;
		f4 = 0.16605f;
		f2 = 1.0f;
		if (f6 <= f1) {
			f1 = 1.5707964f;
			if (f6 <= f1) {
				f0 = f6 * f6;
				f5 = f5 * f0 - f4;
				f5 = f5 * f0 + f2;
				f5 = f5 * f6;
			} else {
				f0 = f6 - f1;
				f1 = f1 - f0;
				f0 = f1 * f1;
				f5 = f5 * f0 - f4;
				f5 = f5 * f0 + f2;
				f5 = f5 * f1;
			}
		} else {
			f0 = 4.712389f;
			if (f6 < f0) {
				f1 = f6 - f1;
				f0 = f1 * f1;
				f5 = f5 * f0 - f4;
				f5 = f5 * f0 + f2;
				f5 = f5 * f1;
				f5 = -f5;
			} else {
				f0 = f6 - f0;
				f1 = 1.5707964f;
				f1 = f1 - f0;
				f0 = f1 * f1;
				f5 = f5 * f0 - f4;
				f5 = f5 * f0 + f2;
				f5 = f5 * f1;
				f5 = -f5;
			}
		}
		data->velX = speed * f5;

		f1 = 3.1415927f;
		f4 = 0.03705f;
		f3 = 0.4967f;
		f2 = 1.0f;
		if (f6 <= f1) {
			f1 = 1.5707964f;
			if (f6 <= f1) {
				f0 = f6 * f6;
				f4 = f4 * f0 - f3;
				f4 = f4 * f0 + f2;
			} else {
				f0 = f6 - f1;
				f0 = f1 - f0;
				f0 = f0 * f0;
				f4 = f4 * f0 - f3;
				f4 = f4 * f0 + f2;
				f4 = -f4;
			}
		} else {
			f0 = 4.712389f;
			if (f6 < f0) {
				f0 = f6 - f1;
				f0 = f0 * f0;
				f4 = f4 * f0 - f3;
				f4 = f4 * f0 + f2;
				f4 = -f4;
			} else {
				f0 = f6 - f0;
				f1 = 1.5707964f;
				f0 = f1 - f0;
				f0 = f0 * f0;
				f4 = f4 * f0 - f3;
				f4 = f4 * f0 + f2;
			}
		}
		data->velY = speed * f4;
		data->scale = 1.0f;
		data->lifetime = 0x3C;
		data->alpha = 0xFF;
		color = col_tbl[type % 10];
		data->colorR = color[3].r;
		data->colorG = color[3].g;
		data->colorB = color[3].b;
		data->targetR = color[0].r;
		data->targetG = color[0].g;
		data->targetB = color[0].b;
		break;
	}
}
#pragma opt_propagation reset

EffectEntry* effGonbabaBreathEntry(s32 type, s32 lifetime, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry;
	effGonbabaBreathData* data;
	effGonbabaBreathData* tmp;
	s32 count;
	s32 i;

	entry = effEntry();
	switch (type) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
		count = 0x21;
		break;
	case 7:
		count = 0x11;
		break;
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	case 16:
	case 17:
		count = 0x41;
		break;
	}

	entry->type = "GonbabaBreath";
	entry->count = count;
	data = __memAlloc(HEAP_EFFECT, count * sizeof(effGonbabaBreathData));
	entry->userdata = data;
	entry->callback = effGonbabaBreathMain;
	entry->flags |= 2;

	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->scale = scale;
	data->alpha = 0xFF;
	data->counter = 0;
	if (lifetime <= 0) {
		data->lifetime = 1000;
	} else {
		data->lifetime = lifetime;
	}
	data->rotation = 0.0f;
	data->child = NULL;

	if (type < 10) {
		data->child = effGonbabaBreathEntry(type + 10, lifetime, x, y, z, scale);
		((effGonbabaBreathData*)data->child->userdata)->rotation = data->rotation;
		if (((effGonbabaBreathData*)data->child->userdata)->child != NULL) {
			tmp = (effGonbabaBreathData*)((effGonbabaBreathData*)data->child->userdata)->child->userdata;
			tmp->rotation = data->rotation;
		}
	}

	data++;
	for (i = 1; i < count; i++, data++) {
		init_breath(data, i, type);
	}
	return entry;
}

// The original translation unit called intplGetValue through a prototype listing the two float
// bounds before the step/steps words, so MWCC evaluates the two int->float conversions first and
// colours them r0/r4 as retail does; the ABI is unchanged (ints r3-r5, floats f1-f2).
static inline f32 intplGetValueFM(s32 mode, f32 start, f32 end, s32 currStep, s32 steps) {
#ifdef MUST_MATCH
	return ((f32 (*)(s32, f32, f32, s32, s32))intplGetValue)(mode, start, end, currStep, steps);
#else
	return intplGetValue(mode, currStep, steps, start, end);
#endif
}

// `k`/`k2` are the named scale rates; naming them makes MWCC keep the `1.5f - scale` subtraction
// in f1 (the literal form lands it in f0) without moving the pooled 0.25f/0.125f entries.
// `intplGetValueFM` (above) restores retail's int->float conversion colouring.
void effGonbabaBreathMain(EffectEntry* effect) {
	effGonbabaBreathData* data = effect->userdata;
	Vec position;
	Vec disp = { 0 };
	f32 k = 0.25f;
	f32 k2 = 0.125f;
	s32 i;

	disp.x = data->position.x;
	disp.y = data->position.y;
	disp.z = data->position.z;
	position = disp;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->lifetime = 0x20;
		if (data->child != NULL) {
			effSoftDelete(data->child);
		}
	}

	if (data->lifetime < 1000) {
		data->lifetime--;
	}
	data->counter++;
	if (data->lifetime < 0x20) {
		data->alpha = data->lifetime << 3;
	}
	if (data->lifetime < 0) {
		effDelete(effect);
		return;
	}

	i = 1;
	data++;
	for (; i < effect->count; i++, data++) {
		if (data->field_0x44 != 0) {
			data->field_0x44--;
			continue;
		}
		switch (data->type) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			if (data->lifetime == 0) {
				init_breath(data, i, data->type);
				break;
			}
			data->lifetime--;
			if (data->lifetime < 0x20) {
				data->alpha = data->lifetime << 3;
			}
			if (data->scale < 1.4f) {
				data->scale = (1.5f - data->scale) * k + data->scale;
			} else {
				data->scale *= 1.05f;
			}
			data->velX *= 0.995f;
			data->velY *= 1.05f;
			data->position.x += data->velX;
			data->position.y += data->velY;
			break;
		case 7:
			if (data->lifetime == 0) {
				init_breath(data, 0, data->type);
				break;
			}
			data->lifetime--;
			if (data->lifetime < 0x20) {
				data->alpha = data->lifetime << 3;
			}
			if (data->scale < 1.4f) {
				data->scale = (1.5f - data->scale) * k + data->scale;
			} else {
				data->scale *= 1.03f;
			}
			data->velX *= 0.995f;
			data->velY *= 1.05f;
			data->position.x += data->velX;
			data->position.y += data->velY;
			break;
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
			if (data->lifetime == 0) {
				init_breath(data, i, data->type);
				break;
			}
			data->lifetime--;
			if (data->lifetime < 0x10) {
				data->alpha = data->lifetime << 4;
			}
			if (data->scale < 1.4f) {
				data->scale = (1.5f - data->scale) * k2 + data->scale;
			} else {
				data->scale *= 1.03f;
			}
			data->velX *= 0.9f;
			data->velY *= 0.9f;
			data->position.x += data->velX;
			data->position.y += data->velY;
			data->colorR = (s32)intplGetValueFM(0xB, (f32)data->colorR, (f32)data->targetR, 0x1E - data->lifetime, 0x1E);
			data->colorG = (s32)intplGetValueFM(0xB, (f32)data->colorG, (f32)data->targetG, 0x1E - data->lifetime, 0x1E);
			data->colorB = (s32)intplGetValueFM(0xB, (f32)data->colorB, (f32)data->targetB, 0x1E - data->lifetime, 0x1E);
			break;
		case 17:
			if (data->lifetime == 0) {
				init_breath(data, 0, data->type);
				break;
			}
			data->lifetime--;
			if (data->lifetime < 0x20) {
				data->alpha = data->lifetime << 3;
			}
			if (data->scale < 1.4f) {
				data->scale = (1.5f - data->scale) * k2 + data->scale;
			} else {
				data->scale *= 1.03f;
			}
			data->velX *= 0.9f;
			data->velY *= 0.9f;
			data->position.x += data->velX;
			data->position.y += data->velY;
			data->colorR = (s32)intplGetValueFM(0xB, (f32)data->colorR, (f32)data->targetR, 0x3C - data->lifetime, 0x3C);
			data->colorG = (s32)intplGetValueFM(0xB, (f32)data->colorG, (f32)data->targetG, 0x3C - data->lifetime, 0x3C);
			data->colorB = (s32)intplGetValueFM(0xB, (f32)data->colorB, (f32)data->targetB, 0x3C - data->lifetime, 0x3C);
			break;
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effGonbabaBreathDisp, effect);
}

// NONMATCHING (95.18%): frame/registers match; remaining diffs are scheduling/operand order in the
// first 6.2832f multiply, the case-A int->float hoist, and the camera/alpha loads around camGetPtr.
void effGonbabaBreathDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effGonbabaBreathData* data = effect->userdata;
	CameraEntry* camera;
	CameraEntry* camera3d;
	Mtx mtx;
	Mtx rot;
	Mtx scaleMtx;
	Mtx viewMtx;
	GXTexObj tex;
	GXColor color;
	f32 f0;
	f32 f1;
	f32 f2;
	f32 f3;
	f32 cosVal;
#ifdef MUST_MATCH
	// `size` keeps the shared 4.0f in a non-volatile FPR so case B computes -size * 0.5f rather
	// than folding to a -2.0 literal, matching the original; the natural form uses the literals.
	f32 size = 4.0f;
#endif
	s32 type = data->type;
	s32 alpha = data->alpha;
	s32 colorIdx;
	s32 i;
	s32 j;

	camera = camGetPtr(camId);
	if (type < 10) {
		PSMTXTrans(mtx, data->position.x, data->position.y, data->position.z);
	} else {
		camera3d = camGetPtr(CAMERA_3D);
		f1 = (6.2831855f * camera3d->viewYaw) / 360.0f;
		cosVal = (f32)cos(f1);
		camera3d = camGetPtr(CAMERA_3D);
		f2 = camera3d->viewYaw;
		f1 = (6.2831855f * f2) / 360.0f;
		f3 = (f32)sin(f1);
		f1 = data->position.x - (2.0f * f3);
		f3 = data->position.z + (2.0f * cosVal);
		f2 = data->position.y;
		PSMTXTrans(mtx, f1, f2, f3);
	}

	PSMTXRotRad(rot, 'y', 0.017453292f * (data->rotation - camGetPtr(camId)->viewYaw));

	if (data->scale > 90.0f && data->scale < 270.0f) {
		PSMTXScale(scaleMtx, data->scale, data->scale, -data->scale);
	} else {
		PSMTXScale(scaleMtx, data->scale, data->scale, data->scale);
	}
	PSMTXConcat(mtx, rot, mtx);
	PSMTXConcat(mtx, scaleMtx, mtx);
	PSMTXConcat(camera->view, mtx, viewMtx);

	effGetTexObj(0x8E, &tex);
	GXLoadTexObj(&tex, GX_TEXMAP0);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x3C, GX_FALSE, 0x7D);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	colorIdx = type % 10;
	for (i = 1, data = data + 1; i < effect->count; i++, data++) {
		if (data->field_0x44 != 0) {
			continue;
		}
		PSMTXTrans(mtx, data->position.x, data->position.y, data->position.z);
		PSMTXScale(scaleMtx, data->scale, data->scale, data->scale);
		PSMTXConcat(mtx, scaleMtx, mtx);
		PSMTXConcat(viewMtx, mtx, mtx);
		GXLoadPosMtxImm(mtx, 0);
		GXSetCurrentMtx(0);

		switch (data->type) {
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
			for (j = 0; j < 3; j++) {
				col_tbl[colorIdx][j].a = (data->alpha * alpha) / 255;
				GXSetChanMatColor(GX_COLOR0A0, col_tbl[colorIdx][j]);
				GXBegin(GX_QUADS, GX_VTXFMT0, 4);
#ifdef MUST_MATCH
				f1 = 16.0f - size * (f32)j;
#else
				f1 = 16.0f - 4.0f * (f32)j;
#endif
				f0 = -f1;
				f1 = f1 * 0.5f;
				f2 = f0 * 0.5f;
				GXPosition3f32(f2, f1, 0.0f);
				GXTexCoord2f32(0.0f, 0.0f);
				GXPosition3f32(f1, f1, 0.0f);
				GXTexCoord2f32(1.0f, 0.0f);
				GXPosition3f32(f1, f2, 0.0f);
				GXTexCoord2f32(1.0f, 1.0f);
				GXPosition3f32(f2, f2, 0.0f);
				GXTexCoord2f32(0.0f, 1.0f);
			}
			break;
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
			color.r = data->colorR;
			color.g = data->colorG;
			color.b = data->colorB;
			color.a = (data->alpha * alpha) / 255;
			GXSetChanMatColor(GX_COLOR0A0, color);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			f1 = 2.0f;
#ifdef MUST_MATCH
			f2 = -size * 0.5f;
#else
			f2 = -2.0f;
#endif
			GXPosition3f32(f2, f1, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(f1, f1, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);
			GXPosition3f32(f1, f2, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);
			GXPosition3f32(f2, f2, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
			break;
		}
	}
}
