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
#include <math.h>

extern GXColor unk_804232d8;

const GXColor dat_80420ab0 = { 0x80, 0x80, 0x80, 0x80 };
const GXColor dat_80420ab4 = { 0x60, 0x00, 0x00, 0x00 };

void effKemuTestSetRxRz(EffectEntry* effect, f32 rx, f32 rz) {
	effKemuTestData* data = effect->userdata;

	data->field_0x50 = rx;
	data->field_0x54 = rz;
}

void effKemuTestDrawCam(EffectEntry* effect, s32 camId) {
	effKemuTestData* data = effect->userdata;

	data->field_0x1C = camId;
}

// NONMATCHING (91.6%): retail duplicates the per-quad mat/pos/scale/draw block into both
// branches of the `j == 0` test; this compiler merges them, and the quad extent is
// constant-folded to -16 instead of kept as a hoisted -32 * 0.5.
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
		PSMTXRotRad(rot, 'x', 0.017453292f * data->field_0x50);
		PSMTXRotRad(scale, 'z', 0.017453292f * data->field_0x54);
		PSMTXConcat(rot, scale, rot);
	}
	PSMTXScale(scale, data->scale, data->scale, data->scale);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXConcat(mtx, scale, mtx);
	PSMTXConcat(camera->view, mtx, view);
	effGetTexObj(0x28, &tex);
	GXLoadTexObj(&tex, 0);
	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(0, 1, 4, 0x1E, 0, 0x7D);
	GXSetTexCoordGen2(1, 1, 4, 0x21, 0, 0x7D);
	GXSetNumTevStages(3);
	GXSetTevOrder(0, 0, 0, 0xFF);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevColorIn(0, 0xF, 0xF, 0xF, 8);
	GXSetTevAlphaIn(0, 7, 7, 7, 4);
	GXSetTevOrder(1, 1, 0, 0xFF);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 0, 8, 2, 0xF);
	GXSetTevAlphaIn(1, 0, 4, 1, 7);
	GXSetTevColor(1, dat_80420ab0);
	GXSetTevOrder(2, 0xFF, 0xFF, 4);
	GXSetTevColorOp(2, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
	GXSetTevColorIn(2, 0xF, 0, 0xA, 0xF);
	GXSetTevAlphaIn(2, 7, 0, 5, 7);
	GXSetCullMode(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xD, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);

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
		f32 dsize = -32.0f;

		for (i = 1; i < effect->count; i++, data++) {
			if (data->field_0x44 != 0) {
				continue;
			}
			if (data->field_0x20 > 0xF) {
				continue;
			}
			PSMTXTrans(mtx, 0.0625f * (f32)data->field_0x20, 0.0f, 0.0f);
			PSMTXScale(scale, 0.0625f, 1.0f, 1.0f);
			PSMTXConcat(mtx, scale, mtx);
			GXLoadTexMtxImm(mtx, 0x1E, 1);
			PSMTXTrans(mtx, 0.0625f * (f32)data->field_0x24, 0.0f, 0.0f);
			PSMTXScale(scale, 0.0625f, 1.0f, 1.0f);
			PSMTXConcat(mtx, scale, mtx);
			GXLoadTexMtxImm(mtx, 0x21, 1);
			{
				f32 size = 16.0f;

				for (j = flag; j < 2; j++) {
					if (j == 0) {
						GXSetChanMatColor(4, dat_80420ab4);
						PSMTXTrans(mtx, data->posX + 2.0f, data->posY - 2.0f, data->posZ);
					}
					else {
						GXColor color = unk_804232d8;

						color.r = data->field_0x4C;
						color.g = data->field_0x4D;
						color.b = data->field_0x4E;
						color.a = alpha;
						GXSetChanMatColor(4, color);
						PSMTXTrans(mtx, data->posX, data->posY, data->posZ);
					}
					PSMTXScale(scale, data->scale, data->scale, data->scale);
					PSMTXConcat(view, mtx, mtx);
					if (type == 0xB) {
						PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(camId)->viewYaw);
						PSMTXConcat(mtx, rot, mtx);
					}
					PSMTXConcat(mtx, scale, mtx);
					GXLoadPosMtxImm(mtx, 0);
					GXBegin(0x80, 0, 4);
					GXPosition3f32(dsize * 0.5f, size, 0.0f);
					GXTexCoord2f32(0.0f, 0.0f);
					GXPosition3f32(size, size, 0.0f);
					GXTexCoord2f32(1.0f, 0.0f);
					GXPosition3f32(size, dsize * 0.5f, 0.0f);
					GXTexCoord2f32(1.0f, 1.0f);
					GXPosition3f32(dsize * 0.5f, dsize * 0.5f, 0.0f);
					GXTexCoord2f32(0.0f, 1.0f);
				}
			}
		}
	}
}

void effKemuTestMain(EffectEntry* effect) {
#ifdef MUST_MATCH
	/* MUST_MATCH: retail builds the Vec in a scratch and copies it to a second local before
	   dispCalcZ; a single local folds the copy away. */
	Vec position;
#endif
	Vec disp = { 0 };
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
#ifdef MUST_MATCH
	position = disp;
#endif
	type = data->type;
	camId = data->field_0x1C;
	data++;
	for (i = 1; i < effect->count; i++, data++) {
		if (data->field_0x44 != 0) {
			data->field_0x44--;
			continue;
		}
		data->field_0x20 = (s32)(16.0f * (f32)data->field_0x28 / (f32)data->field_0x30);
		data->field_0x24 = (s32)(16.0f * (f32)(data->field_0x28 + 1) / (f32)data->field_0x30);
		if (data->field_0x24 > 0xF) {
			data->field_0x24 = 0xF;
		}
		data->field_0x28++;
		switch (type) {
		case 1:
		case 3:
		case 19:
			data->posY += data->field_0x34;
			data->field_0x34 += data->field_0x38;
			data->field_0x38 *= data->field_0x3C;
			break;
		case 10:
		case 14:
			angle = (90.0f + 180.0f * (f32)(i - 1) / (f32)(effect->count - 2)) * 6.2831855f / 360.0f;
			data->posX += data->field_0x34 * (f32)sin(angle);
			data->posY += -data->field_0x34 * (f32)cos(angle);
			data->field_0x34 += data->field_0x38;
			data->field_0x38 *= data->field_0x3C;
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
			data->posY += data->field_0x34;
			data->field_0x34 += data->field_0x38;
			data->field_0x38 *= data->field_0x3C;
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
			data->posX += data->field_0x34 * (f32)sin(angle);
			data->posY += -data->field_0x34 * (f32)cos(angle);
			data->field_0x34 += data->field_0x38;
			data->field_0x38 *= data->field_0x3C;
			data->scale += 0.05f;
			break;
		case 13:
			data->posX += data->velX;
			data->posY += data->velY;
			data->velX *= 0.9f;
			data->velY *= 0.9f;
			data->posY += data->field_0x34;
			data->field_0x34 += data->field_0x38;
			data->field_0x38 *= data->field_0x3C;
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
			b = 255 - data->field_0x28 * 60;
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
			data->field_0x4C = r;
			data->field_0x4D = g;
			data->field_0x4E = b;
			if (type == 3 && i == 1 && data->field_0x28 == 30) {
				confetti = 1;
			}
			break;
		}
		case 7:
			data->field_0x4C += (s32)((0xBD - data->field_0x4C) / 10.0f);
			data->field_0x4D += (s32)((0x10 - data->field_0x4D) / 10.0f);
			data->field_0x4E += (s32)((0xAA - data->field_0x4E) / 10.0f);
			break;
		case 12:
			data->field_0x4C += (s32)((0x82 - data->field_0x4C) / 64.0f);
			data->field_0x4D += (s32)((0x82 - data->field_0x4D) / 64.0f);
			data->field_0x4E += (s32)((0x82 - data->field_0x4E) / 64.0f);
			break;
		default:
			break;
		}
		if (data->field_0x28 >= data->field_0x30) {
			done++;
		}
	}
	if (confetti) {
#ifdef MUST_MATCH
		psndSFXOn_3D("SFX_BTL_STAGE_BURST1", &position);
		effConfettiEntry(6, 0x3C, position.x, position.y, position.z);
#else
		psndSFXOn_3D("SFX_BTL_STAGE_BURST1", &disp);
		effConfettiEntry(6, 0x3C, disp.x, disp.y, disp.z);
#endif
	}
	if (done >= effect->count - 1) {
		effDelete(effect);
	} else {
#ifdef MUST_MATCH
		dispEntry(camId, dispCalcZ(&position), 2, effKemuTestDisp, effect);
#else
		dispEntry(camId, dispCalcZ(&disp), 2, effKemuTestDisp, effect);
#endif
	}
}

static f32 bom_dir[10] = { 10.0f, 40.0f, 80.0f, 170.0f, 140.0f, 100.0f, 25.0f, 155.0f, 60.0f, 120.0f };
static f32 bom_spd[10] = { 2.2f, 2.7f, 3.0f, 2.2f, 2.7f, 3.0f, 1.9f, 1.9f, 1.5f, 1.5f };
const f32 dat_802fdcac[4] = { 0.0f, 120.0f, 180.0f, 240.0f };
const f32 vec3_802fdc88[3] = { 0.5f, 0.3f, 0.8f };
const f32 vec3_802fdc94[3] = { 0.0f, 10.0f, 10.0f };
const f32 vec3_802fdca0[3] = { 0.0f, 5.0f, 20.0f };
const f32 vec3_802fdcc8[3] = { 0.0f, 5.0f, -5.0f };
const f32 vec3_802fdcd4[3] = { 5.0f, 0.0f, -2.0f };
const f32 vec3_802fdce0[3] = { 0.0f, 1.0f, 2.0f };

// NONMATCHING (68.3%): retail interleaves per-type initialisers through a walking set of
// local constant arrays; this is a faithful re-implementation of the same values but
// the compiler schedules/allocates it differently.
EffectEntry* effKemuTestEntry(s32 type, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry;
	effKemuTestData* data;
	effKemuTestData* work;
	s32 count;
	s32 i;
	f32 angle;

	entry = effEntry();
	switch (type) {
	case 0:
	case 1:
	case 15:
	case 18:
		count = 2;
		break;
	case 2:
	case 14:
	case 16:
		count = 5;
		break;
	case 3:
		count = 33;
		break;
	case 4:
	case 13:
		count = 11;
		break;
	case 5:
		count = 4;
		break;
	case 6:
	case 7:
	case 10:
		count = 7;
		break;
	case 8:
		count = 65;
		break;
	case 9:
		count = 21;
		break;
	case 11:
		count = 9;
		break;
	case 12:
		count = irand(3) + 4;
		break;
	case 17:
	case 20:
		count = 11;
		scale *= 0.5f;
		break;
	case 19:
		count = 4;
		break;
	case 21:
		count = 3;
		break;
	case 22:
		count = 4;
		break;
	default:
		count = 1;
		break;
	}

	entry->type = "KemuTest";
	entry->count = count;
	data = __memAlloc(HEAP_EFFECT, count * sizeof(effKemuTestData));
	entry->userdata = data;
	entry->callback = effKemuTestMain;

	data->type = type;
	data->posX = x;
	data->posY = y;
	data->posZ = z;
	data->field_0x28 = 0;
	data->field_0x44 = 0;
	data->scale = scale;
	data->field_0x1C = 4;
	data->field_0x50 = 0.0f;
	data->field_0x54 = 0.0f;

	work = data + 1;
	for (i = 1; i < count; i++, work++) {
		switch (type) {
		case 0:
		case 15:
		case 18:
			work->posX = 0.0f;
			work->posY = 0.0f;
			work->posZ = 0.0f;
			work->field_0x30 = 0x20;
			work->field_0x28 = 0;
			work->field_0x44 = 0;
			work->field_0x4C = 0xFF;
			work->field_0x4D = 0xFF;
			work->field_0x4E = 0xFF;
			work->scale = 1.0f;
			if (type == 15) {
				work->field_0x4C = 0x50;
				work->field_0x4D = 0x4B;
				work->field_0x4E = 0x4B;
			}
			break;
		case 1:
		case 19:
			work->posX = 0.0f;
			work->posY = 0.0f;
			work->posZ = 0.0f;
			work->field_0x30 = 0x80;
			work->field_0x34 = 0.95f;
			work->field_0x38 = -0.04f;
			work->field_0x3C = 0.94f;
			work->field_0x28 = 0;
			work->field_0x44 = 0;
			work->field_0x4C = 0xFF;
			work->field_0x4D = 0xFF;
			work->field_0x4E = 0xFF;
			work->scale = 1.0f;
			if (type == 19) {
				work->posX = vec3_802fdc94[i - 1];
				work->posY = vec3_802fdca0[i - 1];
				work->scale = vec3_802fdc88[i - 1];
				work->field_0x44 = 4;
				work->field_0x4C = 0x78;
				work->field_0x4D = 0x78;
				work->field_0x4E = 0x78;
				work->field_0x34 *= 0.8f;
			}
			break;
		case 2:
		case 16:
			angle = 6.2831855f * (f32)irand(0x168) / 360.0f;
			work->posX = 20.0f * (f32)sin(angle);
			work->posY = 20.0f * (f32)cos(angle);
			work->posZ = 0.0f;
			work->field_0x30 = 0x20;
			work->field_0x28 = 0;
			work->field_0x44 = (i - 1) * 6;
			work->field_0x4C = 0xFF;
			work->field_0x4D = 0xFF;
			work->field_0x4E = 0xFF;
			work->scale = 1.0f;
			break;
		case 3:
			angle = 6.2831855f * (f32)irand(0x168) / 360.0f;
			work->posX = 20.0f * (f32)sin(angle);
			work->posY = 20.0f * (f32)cos(angle);
			work->posZ = 0.0f;
			work->field_0x30 = 0x20;
			work->field_0x28 = 0;
			work->field_0x44 = (i - 1) * 3;
			work->field_0x34 = 0.95f;
			work->field_0x38 = -0.04f;
			work->field_0x3C = 0.94f;
			work->field_0x4C = 0xFF;
			work->field_0x4D = 0xFF;
			work->field_0x4E = 0xFF;
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
			work->field_0x30 = 0x20;
			work->field_0x28 = 0;
			work->field_0x44 = 0;
			work->field_0x4C = 0xFF;
			work->field_0x4D = 0xFF;
			work->field_0x4E = 0xFF;
			if (type == 4 || type == 13) {
				f32 spd;

				angle = 6.2831855f * (90.0f + bom_dir[i - 1]) / 360.0f;
				spd = 2.0f * bom_spd[i - 1];
				work->velX = spd * (f32)sin(angle);
				work->velY = -spd * (f32)cos(angle);
				work->scale = 1.0f;
				work->field_0x44 = irand(0x1E);
				if (type == 13) {
					work->field_0x34 = 0.6f;
					work->field_0x38 = -0.08f;
					work->field_0x3C = 0.94f;
					work->field_0x4C = 0xAA;
					work->field_0x4D = 0x82;
					work->field_0x4E = 0xE6;
					work->velX *= 0.8f;
					work->velY *= 0.8f;
					work->field_0x30 *= 2;
				}
			}
			else if (type == 5) {
				angle = 6.2831855f * dat_802fdcac[0] / 360.0f;
				work->velX = 3.0f * (f32)sin(angle);
				work->velY = -3.0f * (f32)cos(angle);
				work->scale = 0.5f;
			}
			else if (type == 21) {
				angle = 6.2831855f * vec3_802fdcd4[0] / 360.0f;
				work->velX = 3.0f * (f32)sin(angle);
				work->velY = -3.0f * (f32)cos(angle);
				work->scale = 0.5f;
			}
			else if (type == 9 || type == 17 || type == 20) {
				f32 spd;

				angle = 6.2831855f * (f32)irand(0x168) / 360.0f;
				if (i < (count - 1) / 2 + 1) {
					spd = 4.5f;
					work->field_0x44 = 8;
				}
				else {
					spd = 3.0f;
					work->field_0x44 = 0;
				}
				work->velX = spd * (f32)sin(angle);
				work->velY = -spd * (f32)cos(angle);
				work->scale = 1.0f;
				if (type == 9) {
					work->field_0x4C = 0xF8;
					work->field_0x4D = 0xC0;
					work->field_0x4E = 0xDE;
				}
				else if (type == 17) {
					work->field_0x4C = 0xFF;
					work->field_0x4D = 0xFF;
					work->field_0x4E = 0xFF;
				}
				else {
					work->field_0x4C = 0xFF;
					work->field_0x4D = 0x97;
					work->field_0x4E = 0x10;
				}
			}
			else {
				work->field_0x34 = 0.95f;
				work->field_0x38 = -0.04f;
				work->field_0x3C = 0.94f;
				work->field_0x30 = 0x40;
				work->scale = 1.0f;
				work->field_0x44 = 0;
				if (type == 10) {
					work->field_0x4C = 0xAF;
					work->field_0x4D = 0xA5;
					work->field_0x4E = 0x9B;
				}
			}
			break;
		case 6:
			work->posX = (f32)(irand(0x28) - 0x14);
			work->posY = (f32)(irand(0x14) - 0xA);
			work->posZ = 0.0f;
			work->field_0x30 = 0x40;
			work->field_0x28 = 0;
			work->field_0x44 = irand(0xA);
			work->field_0x34 = 0.6f;
			work->field_0x38 = -0.04f;
			work->field_0x3C = 0.94f;
			work->field_0x4C = 0xFF;
			work->field_0x4D = 0xFF;
			work->field_0x4E = 0xFF;
			work->scale = 0.6f;
			angle = angleABf(0.0f, 0.0f, work->posX, work->posY);
			work->velX = 1.0f * (f32)sin(6.2831855f * angle / 360.0f);
			break;
		case 7:
			work->posX = 0.0f;
			work->posY = 0.0f;
			work->posZ = 0.0f;
			angle = 6.2831855f * (135.0f - 90.0f * ((f32)(i - 1) / (f32)(count - 2))) / 360.0f;
			{
				f32 spd = 1.0f + (f32)irand(0x28) / 10.0f;

				work->velX = spd * (f32)cos(angle);
				work->velY = spd * (f32)sin(angle);
			}
			work->field_0x44 = irand(0x10);
			work->field_0x30 = 0x3C;
			work->field_0x28 = 0;
			work->field_0x4C = 0xDC;
			work->field_0x4D = 0xC3;
			work->field_0x4E = 0xE9;
			work->scale = 1.0f;
			break;
		case 8:
			work->posX = 0.0f;
			work->posY = 0.0f;
			work->posZ = 0.0f;
			angle = 6.2831855f * (f32)irand(0x168) / 360.0f;
			{
				f32 spd = 2.0f * ((f32)irand(0x28) / 10.0f) + 1.0f;

				work->velX = spd * (f32)cos(angle);
				work->velY = spd * (f32)sin(angle);
			}
			work->field_0x44 = i & 0x1F;
			work->field_0x30 = 0x20;
			work->field_0x28 = 0;
			work->field_0x4C = 0xFF;
			work->field_0x4D = 0xFF;
			work->field_0x4E = 0xFF;
			work->scale = 1.0f;
			break;
		case 11:
			work->posX = 0.0f;
			work->posY = 0.0f;
			work->posZ = 0.0f;
			angle = 6.2831855f * (360.0f * ((f32)(i - 1) / (f32)(count - 2))) / 360.0f;
			work->velX = 5.0f * (f32)sin(angle);
			work->velY = 0.0f;
			work->velZ = 5.0f * (f32)cos(angle);
			work->field_0x30 = 0x40;
			work->field_0x28 = 0;
			work->field_0x44 = 0;
			work->field_0x4C = 0xFF;
			work->field_0x4D = 0xFF;
			work->field_0x4E = 0xFF;
			work->scale = 1.0f;
			break;
		case 12:
			work->field_0x34 = 0.95f;
			work->field_0x38 = -0.04f;
			work->field_0x3C = 0.94f;
			work->field_0x30 = 0x60;
			work->scale = 0.0f;
			work->field_0x44 = 0;
			work->field_0x4C = 0xFF;
			work->field_0x4D = 0xFF;
			work->field_0x4E = 0xFF;
			break;
		case 22:
			work->posX = vec3_802fdcc8[i - 1];
			work->posY = vec3_802fdcd4[i - 1];
			work->posZ = vec3_802fdce0[i - 1];
			work->velX = 0.0f;
			work->velY = 0.0f;
			work->velZ = 0.0f;
			work->field_0x30 = 0x20;
			work->field_0x28 = 0;
			work->field_0x44 = 0;
			work->field_0x4C = 0xFF;
			work->field_0x4D = 0xFF;
			work->field_0x4E = 0xFF;
			work->scale = 0.5f;
			break;
		default:
			break;
		}
	}

	if (type == 3) {
		Vec position;

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
