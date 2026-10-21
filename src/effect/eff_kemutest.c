#include "eff/eff_kemutest.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "eff/eff_confetti.h"
#include "eff/eff_hit.h"
#include "eff/n64/eff_stardust2_n64.h"
#include "memory.h"
#include "pmario_sound.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "math_ext.h"

#ifdef MUST_MATCH
/* The retail unit owns this zero-initialized colour template and its padding. */
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_804232d8;
#pragma force_active on
__declspec(section ".sdata") u32 gap_10_804232DC_sbss2;
#pragma force_active reset
#pragma section ".sdata" ".sbss"
#else
GXColor unk_804232d8;
#endif

typedef struct {
	f32 v[4];
} effKemuTestQuad;

static f32 bom_dir[10] = { 10.0f, 40.0f, 80.0f, 170.0f, 140.0f, 100.0f, 25.0f, 155.0f, 60.0f, 120.0f };
static f32 bom_spd[10] = { 2.2f, 2.7f, 3.0f, 2.2f, 2.7f, 3.0f, 1.9f, 1.9f, 1.5f, 1.5f };
const Vec vec3_802fdc88 = { 0.5f, 0.3f, 0.8f };
const Vec vec3_802fdc94 = { 0.0f, 10.0f, 10.0f };
const Vec vec3_802fdca0 = { 0.0f, 5.0f, 20.0f };
const effKemuTestQuad dat_802fdcac = { { 0.0f, 120.0f, 180.0f, 240.0f } };
const Vec vec3_802fdcbc = { 0.0f, 120.0f, 240.0f };
const Vec vec3_802fdcc8 = { 0.0f, 5.0f, -5.0f };
const Vec vec3_802fdcd4 = { 5.0f, 0.0f, -2.0f };
const Vec vec3_802fdce0 = { 0.0f, 1.0f, 2.0f };
// effKemuTestEntry: case 4/13 uses an explicit direction pointer under MUST_MATCH so MWCC
// assigns bom_dir to r22 and bom_spd to r21 while retaining the original FP instruction order.
EffectEntry* effKemuTestEntry(s32 type, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry;
	effKemuTestData* data;
	effKemuTestData* work;
	s32 count;
	s32 i;
	f32 angle;
	f32 spd;

	entry = effEntry();
	count = 1;
	switch (type) {
	case 0:
	case 1:
	case 15:
	case 18:
		count = 2;
		break;
	case 2:
	case 16:
		count = 5;
		break;
	case 3:
		count = 33;
		break;
	case 4:
		count = 11;
		break;
	case 5:
		count = 4;
		break;
	case 6:
		count = 7;
		break;
	case 7:
		count = 7;
		break;
	case 8:
		count = 65;
		break;
	case 9:
		count = 21;
		break;
	case 10:
		count = 7;
		break;
	case 11:
		count = 9;
		break;
	case 12:
		count = irand(3) + 4;
		break;
	case 13:
		count = 11;
		break;
	case 14:
		count = 5;
		break;
	case 17:
		count = 11;
		scale *= 0.5f;
		break;
	case 19:
		count = 4;
		break;
	case 20:
		count = 11;
		scale *= 0.5f;
		break;
	case 21:
		count = 3;
		break;
	case 22:
		count = 4;
		break;
	}

#ifdef MUST_MATCH
	/* Retail keeps three additional NUL bytes in this pooled string. */
	entry->type = "KemuTest\0\0\0";
#else
	entry->type = "KemuTest";
#endif
	entry->count = count;
	data = __memAlloc(HEAP_EFFECT, entry->count * sizeof(effKemuTestData));
	entry->userdata = data;
	entry->callback = effKemuTestMain;

	data->type = type;
	data->posX = x;
	data->posY = y;
	data->posZ = z;
	data->age = 0;
	data->delay = 0;
	data->scale = scale;
	data->camId = 4;
	data->rotX = 0.0f;
	data->rotZ = 0.0f;

	work = data;
	work++;
	for (i = 1; i < (count = entry->count); i++, work++) {
		switch (type) {
		case 0:
		case 15:
		case 18:
			work->posX = 0.0f;
			work->posY = 0.0f;
			work->posZ = 0.0f;
			work->life = 0x20;
			work->age = 0;
			work->delay = 0;
			work->colorR = 0xFF;
			work->colorG = 0xFF;
			work->colorB = 0xFF;
			work->scale = 1.0f;
			if (type == 15) {
				work->colorR = 0x50;
				work->colorG = 0x4B;
				work->colorB = 0x4B;
			}
			break;
		case 1:
		case 19:
			work->posX = 0.0f;
			work->posY = 0.0f;
			work->posZ = 0.0f;
			work->life = 0x80;
			work->age = 0;
			work->speed = 0.95f;
			work->accel = -0.04f;
			work->damp = 0.94f;
			work->delay = 0;
			work->colorR = 0xFF;
			work->colorG = 0xFF;
			work->colorB = 0xFF;
			work->scale = 1.0f;
			if (type == 19) {
				/* Each rodata vector is copied to a scratch Vec and indexed from there, in this
				   declaration order (the copies' store scheduling follows it). */
				Vec v88 = vec3_802fdc88;
				Vec v94 = vec3_802fdc94;
				Vec va0 = vec3_802fdca0;

				work->posX = (&v94.x)[i - 1];
				work->posY = (&va0.x)[i - 1];
				work->scale = (&v88.x)[i - 1];
				work->delay = 4 * i;
				work->colorR = 0x78;
				work->colorG = 0x78;
				work->colorB = 0x78;
				work->speed *= 0.8f;
			}
			break;
		case 2:
		case 16:
			angle = 6.2831855f * (f32)irand(0x168) / 360.0f;
			work->posX = 20.0f * (f32)sin(angle);
			work->posY = 20.0f * (f32)cos(angle);
			work->posZ = 0.0f;
			work->life = 0x20;
			work->age = 0;
			work->delay = (i - 1) * 6;
			work->colorR = 0xFF;
			work->colorG = 0xFF;
			work->colorB = 0xFF;
			work->scale = 1.0f;
			break;
		case 3:
			angle = 6.2831855f * (f32)irand(0x168) / 360.0f;
			work->posX = 20.0f * (f32)sin(angle);
			work->posY = 20.0f * (f32)cos(angle);
			work->posZ = 0.0f;
			work->life = 0x20;
			work->age = 0;
			work->delay = (i - 1) * 3;
			work->speed = 0.95f;
			work->accel = -0.04f;
			work->damp = 0.94f;
			work->colorR = 0xFF;
			work->colorG = 0xFF;
			work->colorB = 0xFF;
			work->scale = 1.0f;
			break;
		case 4:
		case 5:
		case 9:
		case 10:
		case 13:
		case 14:
		case 17:
		case 20:
		case 21:
			work->posX = 0.0f;
			work->posY = 0.0f;
			work->posZ = 0.0f;
			work->life = 0x20;
			work->age = 0;
			work->delay = 0;
			work->colorR = 0xFF;
			work->colorG = 0xFF;
			work->colorB = 0xFF;
			work->delay = 0;
			if (type == 4 || type == 13) {
				f32 angle, spd;
#ifdef MUST_MATCH
				/* Preserve the direction pointer before the speed calculation so MWCC
				   assigns the two array induction registers in retail order. */
				f32* dirptr = bom_dir + i;
				dirptr = dirptr + 0;
				spd = 2.0f * bom_spd[i - 1];
				angle = 6.2831855f * (90.0f + dirptr[-1]) / 360.0f;
#else
				spd = 2.0f * bom_spd[i - 1];
				angle = 6.2831855f * (90.0f + bom_dir[i - 1]) / 360.0f;
#endif
				work->velX = spd * (f32)sin(angle);
				work->velY = -spd * (f32)cos(angle);
				work->scale = 1.0f;
				work->delay = irand(0x1E);
				if (type == 13) {
					work->speed = 0.59999996f;
					work->accel = -0.08f;
					work->damp = 0.94f;
					work->colorR = 0xAA;
					work->colorG = 0x82;
					work->colorB = 0xE6;
					work->velX *= 0.8f;
					work->velY *= 0.8f;
					work->life *= 2;
				}
			}
			else if (type == 5) {
				effKemuTestQuad vac = dat_802fdcac;

				spd = 3.0f;
				work->velX = spd * (f32)sin(6.2831855f * vac.v[i] / 360.0f);
				work->velY = -spd * (f32)cos(6.2831855f * vac.v[i] / 360.0f);
				work->scale = 0.5f;
			}
			else if (type == 21) {
				Vec vbc = vec3_802fdcbc;

				spd = 3.0f;
				work->velX = spd * (f32)sin(6.2831855f * (&vbc.x)[i] / 360.0f);
				work->velY = -spd * (f32)cos(6.2831855f * (&vbc.x)[i] / 360.0f);
				work->scale = 0.5f;
			}
			else if (type == 9 || type == 17 || type == 20) {
				f32 angle, spd;
				f32 raw = (f32)irand(0x168);

				if (i < (entry->count - 1) / 2 + 1) {
					spd = 4.5f;
				}
				else {
					spd = 3.0f;
				}
				angle = 6.2831855f * raw / 360.0f;
				work->velX = spd * (f32)sin(angle);
				work->velY = -spd * (f32)cos(angle);
				work->scale = 1.0f;
				work->delay = (i < (entry->count - 1) / 2 + 1) ? 0 : 8;
				if (type == 9) {
					work->colorR = 0xF8;
					work->colorG = 0xC0;
					work->colorB = 0xDE;
				}
				else if (type == 17) {
					work->colorR = 0xFF;
					work->colorG = 0xFF;
					work->colorB = 0xFF;
				}
				else if (type == 20) {
					work->colorR = 0xFF;
					work->colorG = 0x97;
					work->colorB = 0x10;
				}
			}
			else if (type == 10 || type == 14) {
				work->speed = 0.95f;
				work->accel = -0.04f;
				work->damp = 0.94f;
				work->life = 0x40;
				work->scale = 1.0f;
				work->delay = 0;
				if (type == 10) {
					work->colorR = 0xAF;
					work->colorG = 0xA5;
					work->colorB = 0x9B;
				}
			}
			break;
		case 6:
			work->posX = (f32)(irand(0x28) - 0x14);
			work->posY = (f32)(irand(0x14) - 0xA);
			work->posZ = 0.0f;
			work->life = 0x40;
			work->age = 0;
			work->delay = irand(0xA);
			work->speed = 0.59999996f;
			work->accel = -0.04f;
			work->damp = 0.94f;
			work->colorR = 0xFF;
			work->colorG = 0xFF;
			work->colorB = 0xFF;
			work->scale = 0.6f;
			angle = angleABf(0.0f, 0.0f, work->posX, work->posY);
			spd = 1.0f;
			work->velX = spd * (f32)sin(6.2831855f * angle / 360.0f);
			break;
		case 7:
			{
				f32 angle, spd;
				spd = 1.0f + (f32)irand(0x28) / 10.0f;

				work->posX = 0.0f;
				work->posY = 0.0f;
				work->posZ = 0.0f;
				work->life = 0x3C;
				work->age = 0;
				angle = 6.2831855f * (135.0f - 90.0f * ((f32)(i - 1) / (f32)(count - 2))) / 360.0f;
				work->velX = spd * (f32)cos(angle);
				work->velY = spd * (f32)sin(angle);
			}
			work->delay = irand(0x10);
			work->colorR = 0xDC;
			work->colorG = 0xC3;
			work->colorB = 0xE9;
			work->scale = 1.0f;
			break;
		case 8:
			{
				f32 angle, spd;
				f32 raw = (f32)irand(0x168);

				spd = 2.0f * ((f32)irand(0x28) / 10.0f) + 1.0f;
				angle = 6.2831855f * raw / 360.0f;
				work->posX = 0.0f;
				work->posY = 0.0f;
				work->posZ = 0.0f;
				work->life = 0x20;
				work->age = 0;
				work->velX = spd * (f32)cos(angle);
				work->velY = spd * (f32)sin(angle);
			}
			work->delay = i % 0x20;
			work->colorR = 0xFF;
			work->colorG = 0xFF;
			work->colorB = 0xFF;
			work->scale = 1.0f;
			break;
		case 11: {
			f32 angle;
			angle = 6.2831855f * (360.0f * (f32)(i - 1) / (f32)(count - 2)) / 360.0f;
			work->posX = 0.0f;
			work->posY = 0.0f;
			work->posZ = 0.0f;
			work->velX = 5.0f * (f32)sin(angle);
			work->velY = 0.0f;
			work->velZ = 5.0f * (f32)cos(angle);
			work->life = 0x40;
			work->age = 0;
			work->delay = 0;
			work->colorR = 0xFF;
			work->colorG = 0xFF;
			work->colorB = 0xFF;
			work->scale = 1.0f;
			break;
		}
		case 12:
			work->speed = 0.95f;
			work->accel = -0.04f;
			work->damp = 0.94f;
			work->life = 0x60;
			work->scale = 0.0f;
			work->delay = 0;
			work->colorR = 0xFF;
			work->colorG = 0xFF;
			work->colorB = 0xFF;
			break;
		case 22: {
			Vec vc8 = vec3_802fdcc8;
			Vec vd4 = vec3_802fdcd4;
			Vec ve0 = vec3_802fdce0;

			work->posX = (&vc8.x)[i - 1];
			work->posY = (&vd4.x)[i - 1];
			work->posZ = (&ve0.x)[i - 1];
			work->velX = 0.0f;
			work->velY = 0.0f;
			work->velZ = 0.0f;
			work->life = 0x20;
			work->age = 0;
			work->colorR = 0xFF;
			work->colorG = 0xFF;
			work->colorB = 0xFF;
			work->delay = 0;
			work->scale = 0.5f;
			break;
		}
		default:
			break;
		}
	}

	if (type == 3) {
		Vec position = (Vec){ 0.0f, 0.0f, 0.0f };

		position.x = x;
		position.y = y;
		position.z = z;
		psndSFXOn_3D("SFX_BTL_STAGE_BURST1", &position);
		effConfettiEntry(6, 0x3C, x, y, z);
	}
	if (type == 16) {
		effHitEntry(1, 3, x, y, z, scale);
	}
	if (type == 20) {
		effStardust2N64Entry(0, 8, 0x1E, x, y, z, 50.0f * scale, 50.0f * scale);
	}

	return entry;
}
void effKemuTestMain(EffectEntry* effect) {
	/* position is a second copy of disp, used by the sound, confetti and dispCalcZ calls. */
	Vec position;
	Vec disp = (Vec){ 0.0f, 0.0f, 0.0f };
	effKemuTestData* data = effect->userdata;
	s32 type;
	s32 done = 0;
	s32 i;
	s32 confetti = 0;
	s32 camId;
	f32 angle;

	disp.x = data->posX;
	disp.y = data->posY;
	disp.z = data->posZ;
	position = disp;
	type = data->type;
	camId = data->camId;
	data++;
	for (i = 1; i < effect->count; i++, data++) {
		if (data->delay != 0) {
			data->delay--;
			continue;
		}
		data->frame = (s32)(16.0f * (f32)data->age / (f32)data->life);
		data->nextFrame = (s32)(16.0f * (f32)(data->age + 1) / (f32)data->life);
		if (data->nextFrame > 0xF) {
			data->nextFrame = 0xF;
		}
		data->age++;
		switch (type) {
		case 1:
		case 3:
		case 19:
			data->posY += data->speed;
			data->speed += data->accel;
			data->accel *= data->damp;
			break;
		case 10:
		case 14:
			angle = (90.0f + 180.0f * (f32)(i - 1) / (f32)(effect->count - 2)) * 6.2831855f / 360.0f;
			data->posX += data->speed * (f32)sin(angle);
			data->posY += -data->speed * (f32)cos(angle);
			data->speed += data->accel;
			data->accel *= data->damp;
			break;
		case 4:
		case 9:
		case 17:
		case 20:
			data->posX += data->velX;
			data->posY += data->velY;
			data->velX *= 0.9f;
			data->velY *= 0.9f;
			break;
		case 5:
		case 21:
			data->posX += data->velX;
			data->posY += data->velY;
			data->velX *= 0.9f;
			data->velY *= 0.9f;
			break;
		case 6:
			data->posX += data->velX;
			data->velX *= 0.95f;
			data->posY += data->speed;
			data->speed += data->accel;
			data->accel *= data->damp;
			break;
		case 7:
		case 8:
		case 16:
			data->posX += data->velX;
			data->posY += data->velY;
			data->velX *= 0.9f;
			data->velY *= 0.9f;
			break;
		case 11:
			data->posX += data->velX;
			data->posY += data->velY;
			data->posZ += data->velZ;
			data->velX *= 0.9f;
			data->velY *= 0.9f;
			data->velZ *= 0.9f;
			break;
		case 12:
			angle = (90.0f + 360.0f * (f32)(i - 1) / (f32)(effect->count - 2)) * 6.2831855f / 360.0f;
			data->posX += data->speed * (f32)sin(angle);
			data->posY += -data->speed * (f32)cos(angle);
			data->speed += data->accel;
			data->accel *= data->damp;
			data->scale += 0.05f;
			break;
		case 13:
			data->posX += data->velX;
			data->posY += data->velY;
			data->velX *= 0.9f;
			data->velY *= 0.9f;
			data->posY += data->speed;
			data->speed += data->accel;
			data->accel *= data->damp;
			break;
		default:
			break;
		}
		switch (type) {
		case 3:
		case 6:
		case 11:
		case 16:
		case 18: {
			s32 r;
			s32 g;
			s32 b;

			g = 255;
			r = 255;
			b = 255 - data->age * 60;
			if (b < 0) {
				g = (s32)(0.8f * (f32)b) + 255;
				b = 0;
				if (g < 0) {
					r = (s32)(0.4f * (f32)g) + 255;
					g = 0;
					if (r < 0) {
						r = 0;
					}
				}
			}
			data->colorR = r;
			data->colorG = g;
			data->colorB = b;
			if (type == 3 && i == 1 && data->age == 30) {
				confetti = 1;
			}
			break;
		}
		case 7:
			data->colorR += (s32)((0xBD - data->colorR) / 10.0f);
			data->colorG += (s32)((0x10 - data->colorG) / 10.0f);
			data->colorB += (s32)((0xAA - data->colorB) / 10.0f);
			break;
		case 12:
			data->colorR += (s32)((0x82 - data->colorR) / 64.0f);
			data->colorG += (s32)((0x82 - data->colorG) / 64.0f);
			data->colorB += (s32)((0x82 - data->colorB) / 64.0f);
			break;
		default:
			break;
		}
		if (data->age >= data->life) {
			done++;
		}
	}
	if (confetti) {
		psndSFXOn_3D("SFX_BTL_STAGE_BURST1", &position);
		effConfettiEntry(6, 0x3C, position.x, position.y, position.z);
	}
	if (done >= effect->count - 1) {
		effDelete(effect);
	} else {
		dispEntry(camId, dispCalcZ(&position), 2, effKemuTestDisp, effect);
	}
}

static inline GXColor kemuTestColor(effKemuTestData* data, u8 alpha) {
	GXColor color = unk_804232d8;

	color.r = data->colorR;
	color.g = data->colorG;
	color.b = data->colorB;
	color.a = alpha;
	return color;
}

/* Matches using the named .sdata2 constants above (so the compiler emits the
   `-32 * 0.5` multiply at runtime instead of folding it into -16), and a helper
   that returns the channel-built GXColor so it lands in the original's stack slot. */
// The original keeps dsize/dsize_half/size in FPRs but computes them where they are written: with MWCC's loop-invariant
// hoisting on they move to the function preheader (98.1%); off, the function is 100% (the constants are still pooled).
#pragma opt_loop_invariants off
void effKemuTestDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effKemuTestData* data = effect->userdata;
	CameraEntry* camera = camGetPtr(camId);
	Mtx mtx;
	Mtx scale;
	Mtx rot;
	Mtx view;
	GXTexObj tex;
	s32 type = data->type;
	s32 i;
	s32 flag = 0;
	u8 alpha = 0xFF;
	s32 j;

	PSMTXTrans(mtx, data->posX, data->posY, data->posZ);
	if (type != 0xB) {
		PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(camId)->viewYaw);
	}
	else {
		PSMTXRotRad(rot, 'x', 0.017453292f * data->rotX);
		PSMTXRotRad(scale, 'z', 0.017453292f * data->rotZ);
		PSMTXConcat(rot, scale, rot);
	}
	PSMTXScale(scale, data->scale, data->scale, data->scale);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXConcat(mtx, scale, mtx);
	PSMTXConcat(camera->view, mtx, view);
	effGetTexObj(0x28, &tex);
	GXLoadTexObj(&tex, GX_TEXMAP0);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);
	GXSetNumTevStages(3);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_TEXC, GX_CC_C0, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_APREV, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);
	GXSetTevColor(GX_TEVREG0, (GXColor){ 0x80, 0x80, 0x80, 0x80 });
	GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	switch (type) {
	case 1:
		flag = 1;
		alpha = 0x40;
		break;
	case 3:
		alpha = 0xC8;
		break;
	case 10:
	case 12:
		flag = 1;
		alpha = 0xC8;
		break;
	default:
		break;
	}

	data++;
	{
		f32 p32 = 32.0f;
		f32 dsize = -p32;

		for (i = 1; i < effect->count; i++, data++) {
			if (data->delay != 0) {
				continue;
			}
			if (data->frame > 0xF) {
				continue;
			}
			PSMTXTrans(mtx, 0.0625f * (f32)data->frame, 0.0f, 0.0f);
			PSMTXScale(scale, 0.0625f, 1.0f, 1.0f);
			PSMTXConcat(mtx, scale, mtx);
			GXLoadTexMtxImm(mtx, GX_TEXMTX0, GX_MTX2x4);
			PSMTXTrans(mtx, 0.0625f * (f32)data->nextFrame, 0.0f, 0.0f);
			PSMTXScale(scale, 0.0625f, 1.0f, 1.0f);
			PSMTXConcat(mtx, scale, mtx);
			GXLoadTexMtxImm(mtx, GX_TEXMTX1, GX_MTX2x4);
			for (j = flag; j < 2; j++) {
				if (j == 0) {
					GXSetChanMatColor(GX_COLOR0A0, (GXColor){ 0x00, 0x00, 0x00, 0x60 });
					PSMTXTrans(mtx, data->posX + 2.0f, data->posY - 2.0f, data->posZ);
					PSMTXScale(scale, data->scale, data->scale, data->scale);
					PSMTXConcat(view, mtx, mtx);
					if (type == 0xB) {
						PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(camId)->viewYaw);
						PSMTXConcat(mtx, rot, mtx);
					}
					PSMTXConcat(mtx, scale, mtx);
					GXLoadPosMtxImm(mtx, 0);
				}
				else {
					GXSetChanMatColor(GX_COLOR0A0, kemuTestColor(data, alpha));
					PSMTXTrans(mtx, data->posX, data->posY, data->posZ);
					PSMTXScale(scale, data->scale, data->scale, data->scale);
					PSMTXConcat(view, mtx, mtx);
					if (type == 0xB) {
						PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(camId)->viewYaw);
						PSMTXConcat(mtx, rot, mtx);
					}
					PSMTXConcat(mtx, scale, mtx);
					GXLoadPosMtxImm(mtx, 0);
				}
				{
					f32 half = 0.5f;
					f32 dsize_half = dsize * half;
					f32 size = 16.0f;

					GXBegin(GX_QUADS, GX_VTXFMT0, 4);
					GXPosition3f32(dsize_half, size, 0.0f);
					GXTexCoord2f32(0.0f, 0.0f);
					GXPosition3f32(size, size, 0.0f);
					GXTexCoord2f32(1.0f, 0.0f);
					GXPosition3f32(size, dsize_half, 0.0f);
					GXTexCoord2f32(1.0f, 1.0f);
					GXPosition3f32(dsize_half, dsize_half, 0.0f);
					GXTexCoord2f32(0.0f, 1.0f);
				}
			}
		}
	}
}
#pragma opt_loop_invariants reset
void effKemuTestDrawCam(EffectEntry* effect, s32 camId) {
	effKemuTestData* data = effect->userdata;

	data->camId = camId;
}


void effKemuTestSetRxRz(EffectEntry* effect, f32 rx, f32 rz) {
	effKemuTestData* data = effect->userdata;

	data->rotX = rx;
	data->rotZ = rz;
}
