#include "eff/n64/eff_snow_dust_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "system.h"
#include <dolphin/mtx.h>
#include <dolphin/gx.h>
#include "math_ext.h"
#include <stdlib.h>
#include "evt/evt_cmd.h"

extern s32 negone_one[];
extern s32 one_negone[];

// clang-format off

//.sdata
static u8 scale_data[32] = {
	0x02, 0x04, 0x08, 0x0D, 0x0F, 0x0D, 0x09, 0x06,
	0x04, 0x03, 0x02, 0x04, 0x08, 0x0D, 0x0F, 0x0D,
	0x09, 0x06, 0x04, 0x03, 0x02, 0x04, 0x08, 0x0D,
	0x0F, 0x0D, 0x09, 0x06, 0x04, 0x03, 0x00, 0x00
};


static u8 scale_data2[32] = {
	0x28, 0x5A, 0x51, 0x48, 0x41, 0x3B, 0x35, 0x2F,
	0x2B, 0x26, 0x22, 0x1F, 0x1C, 0x19, 0x16, 0x14,
	0x12, 0x10, 0x0F, 0x0D, 0x0C, 0x0A, 0x09, 0x08,
	0x07, 0x07, 0x06, 0x05, 0x05, 0x04, 0x00, 0x00
};


static u8 a_data[32] = {
	0xC8, 0xFA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFA, 0xC8, 0x00, 0x00,
};

// No code references these float pairs; without this the linker would drop them.
#pragma force_active on
// TODO: type
__declspec(section ".data") u32 lbl_803A8AA0[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
static __declspec(section ".data") u32 negone_one_374[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_803A8AB0[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
static __declspec(section ".data") u32 one_negone_373[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_803A8AC0[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
static __declspec(section ".data") u32 negone_one_360[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_803A8AD0[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
static __declspec(section ".data") u32 one_negone_359[2] = {
    0x3F800000, 0xBF800000,
};

//.data
u32 size8x8_tex64x64_vtx[14] = {
	0xFFFCFFFC, 0x00000000, 0x00000000, 0x00FF0004,
	0xFFFC0000, 0x08000000, 0x000000FF, 0x00040004,
	0x00000800, 0x08000000, 0x00FFFFFC, 0x00040000,
	0x00000800, 0x000000FF,
};

u32 size4x4_tex64x64_vtx[14] = {
	0xFFFEFFFE, 0x00000000, 0x00000000, 0x00FF0002,
	0xFFFE0000, 0x08000000, 0x000000FF, 0x00020002,
	0x00000800, 0x08000000, 0x00FFFFFE, 0x00020000,
	0x00000800, 0x000000FF,
};

static u32 size12x12_tex64x64_vtx[14] = {
	0xFFFAFFFA, 0x00000000, 0x00000000, 0x00FF0006,
	0xFFFA0000, 0x08000000, 0x000000FF, 0x00060006,
	0x00000800, 0x08000000, 0x00FFFFFA, 0x00060000,
	0x00000800, 0x000000FF,
};

static u32 size16x16_tex64x64_vtx[14] = {
	0xFFF8FFF8, 0x00000000, 0x00000000, 0x00FF0008,
	0xFFF80000, 0x08000000, 0x000000FF, 0x00080008,
	0x00000800, 0x08000000, 0x00FFFFF8, 0x00080000,
	0x00000800, 0x000000FF,
};

static u32 size32x32_tex32x32_vtx[18] = {
	0xFFE00000, 0x00000000, 0x0000FFFF, 0xFF000000,
	0xFFE00000, 0x04000000, 0xFFFFFF00, 0x00200000,
	0x00000400, 0x0400FFFF, 0xFF000000, 0x00200000,
	0x00000400, 0xFFFFFF00, 0x00000000, 0x00000000,
	0x0000FFFF, 0xFFFF0000,
};
#pragma force_active reset

EffectEntry* effSnowDustN64Entry(s32 type, f32 x, f32 y, f32 z, s32 count, s32 life, f32 a6, f32 a7, f32 scale) {
	EffectEntry* effect;
	effSnowDustData* data;
	effSnowDustData* ptr;
	f32 ratio;
	s32 i;

	effect = effEntry();
	effect->type = "SnowDustN64";
	effect->count = count + 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effSnowDustData) * (count + 1));
	effect->userdata = data;
	effect->callback = effSnowDustMain;
	effect->flags |= 2;
	data->type = type;
	data->angle = 0;
	if (life <= 0) {
		data->life = 1000;
	}
	else {
		data->life = life;
	}
	data->alpha = 0xff;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->spreadX = a6;
	data->spreadY = a7;
	data->camId = 4;
	data->colorR = 0xff;
	data->colorG = 0xff;
	data->colorB = 0xff;
	switch (type) {
		case 0:
			data->topR = 0xff;
			data->topG = 0xff;
			data->topB = 0xff;
			data->topA = 0xff;
			break;
		case 1:
			data->topR = 0;
			data->topG = 0x7f;
			data->topB = 0xff;
			data->topA = 0xff;
			break;
		case 2:
		case 3:
			data->topR = 0xff;
			data->topG = 0xff;
			data->topB = 0x80;
			data->topA = 0xff;
			break;
		case 6:
			data->colorR = 0xcb;
			data->colorG = 0x94;
			data->colorB = 0xcd;
			data->topR = 0xcb;
			data->topG = 0x94;
			data->topB = 0xcd;
			break;
		default:
			data->colorR = 128 + rand() % 127;
			data->colorG = 128 + rand() % (0x7F - (data->colorR - 0x80));
			data->colorB = 128 + rand() % (0x7F - (data->colorR - 0x80) - (data->colorG - 0x80));
			data->topR = 0x7f;
			data->topG = 0x7f;
			data->topB = 0x7f;
			data->topA = 0x7f;
			break;
	}
	if (data->life < 0x1e) {
		ratio = (f32)data->life / (f32)count;
	}
	else {
		ratio = 30.0f / (f32)count;
	}
	ptr = &data[1];
	for (i = 1; i < count + 1; i++, ptr++) {
		ptr->frame = (s32)(-(f32)i * ratio) - 1;
	}
	return effect;
}


// The f32 declaration order gives the target's callee-saved FPRs (cosYaw f31, sinYaw f30, spreadX and
// halfSpreadX f29, baseX/Y/Z f28..f26, spreadY/offset/dist f25). The `+=` easing steps and the split
// distABf*0.97 multiply keep the volatile FPRs. The two compiler-generated int->float magic doubles
// keep anonymous pool names (@307/@367) instead of the target's double_to_int_*; objdiff ignores
// those relocs.
void effSnowDustMain(EffectEntry* effect) {
	s32 rangeX;
	s32 rangeY;
	s32 life;
	effSnowDustData* data;
	s32 i;
	s32 type;
	s32 colorR;
	s32 colorG;
	s32 colorB;
	s32 camera;
	Vec position2;
	Vec position;
	f32 cosYaw;
	f32 sinYaw;
	f32 spreadX;
	f32 baseX;
	f32 baseY;
	f32 baseZ;
	f32 spreadY;
	f32 halfSpreadX;
	f32 offset;
	f32 dist;

	data = effect->userdata;
	position = (Vec){ 0.0f, 0.0f, 0.0f };
	position.x = data->x;
	position.y = data->y;
	position.z = data->z;
	position2 = position;
	camera = data->camId;
	type = data->type;
	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->life = 0x10;
	}
	if (data->life < 1000) {
		data->life--;
	}
	data->angle++;
	life = data->life;
	if (life < 0) {
		effDelete(effect);
		return;
	}
	if (life < 8) {
		data->alpha = life << 5;
	}
	spreadX = data->spreadX;
	spreadY = data->spreadY;
	baseX = data->x;
	baseY = data->y;
	baseZ = data->z;
	cosYaw = (f32)cos(6.2831855f * camGetPtr(CAMERA_3D)->viewYaw / 360.0f);
	sinYaw = (f32)sin(6.2831855f * camGetPtr(CAMERA_3D)->viewYaw / 360.0f);
	colorR = data->colorR;
	colorG = data->colorG;
	colorB = data->colorB;
	rangeX = (s32)spreadX;
	halfSpreadX = spreadX * 0.5f;
	rangeY = (s32)spreadY;
	i = 1;
	data = data + 1;
	for (; i < effect->count; i++, data++) {
		data->frame++;
		if (data->frame >= 0x1e) {
			if (life < 0x1e) {
				data->frame = 0x14;
			}
			else {
				data->frame = 0;
			}
		}
		if (data->frame >= 0) {
			if (data->frame == 0) {
				offset = (f32)(rand() % rangeX) - halfSpreadX;
				data->x = offset * cosYaw + baseX - sinYaw;
				data->y = baseY + (f32)(rand() % rangeY);
				data->z = cosYaw + (offset * sinYaw + baseZ);
				data->colorR = colorR;
				data->colorG = colorG;
				data->colorB = colorB;
			}
			switch (type) {
				case 0:
				case 1:
					if (data->frame == 0) {
						data->velX = 0.0f;
						data->velY = 0.0f;
						data->targetVelX = 0.0f;
						data->targetVelY = 0.05f * (f32)(-(rand() % 10) - 2);
						data->texScrollX = 0.0f;
						data->texScrollY = 0.0f;
						data->scale = 0.05f * (f32)(rand() % 10) + 0.5f;
					}
					data->alpha = a_data[data->frame];
					data->x += data->velX;
					data->y += data->velY;
					data->texScrollX += 0.2f;
					data->texScrollY += 0.6f;
					data->velX += 0.04f * (data->targetVelX - data->velX);
					data->velY += 0.04f * (data->targetVelY - data->velY);
					break;
				case 2:
				case 3:
					if (data->frame == 0) {
						if (type == 2) {
							data->velX = 0.05f * (f32)(rand() % 20 - 10);
							data->velY = 0.05f * (f32)(rand() % 20 - 10);
						}
						else {
							data->velX = 0.0f;
							data->velY = 0.0f;
						}
						data->targetVelX = 0.0f;
						data->targetVelY = 0.0f;
						data->texScrollX = (f32)(rand() % 15);
						data->texScrollY = (f32)(rand() % 15);
						data->alpha = 0xff;
					}
					data->scale = 0.1f * (f32)scale_data[data->frame];
					data->x += data->velX;
					data->y += data->velY;
					data->texScrollX += 0.2f;
					data->texScrollY += 0.6f;
					data->velX += 0.04f * (data->targetVelX - data->velX);
					data->velY += 0.04f * (data->targetVelY - data->velY);
					break;
				case 6:
					if (data->frame == 0) {
						data->velX = 0.0f;
						data->velY = 0.0f;
						data->targetVelX = 0.0f;
						data->targetVelY = 0.05f * (f32)(-(rand() % 10) - 2);
						data->texScrollX = 0.0f;
						data->texScrollY = 0.0f;
						data->scale = 0.05f * (f32)(rand() % 10) + 0.5f;
						data->angle = irand(0x168);
					}
					data->alpha = a_data[data->frame];
					data->alpha = 0xff;
					data->y += data->velY;
					data->texScrollX += 0.2f;
					data->texScrollY += 0.6f;
					data->velY += 0.04f * (data->targetVelY - data->velY);
					dist = distABf(position2.x, position2.z, data->x, data->z);
					dist *= 0.97f;
					data->x = dist * (f32)sin(6.2831855f * (f32)data->angle / 360.0f) + position2.x;
					data->z = dist * (f32)cos(6.2831855f * (f32)data->angle / 360.0f) + position2.z;
					data->angle += 4;
					data->angle %= 360;
					break;
				default:
					if (data->frame == 0) {
						data->texScrollX = 0.0f;
						data->texScrollY = 0.0f;
						data->alpha = 0xff;
						data->scale = 1.0f;
						data->colorR = 128 + rand() % 127;
						data->colorG = 128 + rand() % (0x7F - (data->colorR - 0x80));
						data->colorB = 128 + rand() % (0x7F - (data->colorR - 0x80) - (data->colorG - 0x80));
					}
					if (type == 4) {
						data->scale = 0.01f * (f32)scale_data2[data->frame];
					}
					else {
						data->scale = 0.01f * (f32)scale_data[data->frame];
					}
					break;
			}
		}
	}
	dispEntry(camera, dispCalcZ(&position2), 2, effSnowDustDisp, effect);
}


// The loop walks the single `data` pointer as the original does; the texture offset wraps every 32
// items (`(i & 0x1f) * 64`, the target's clrlslwi).
void effSnowDustDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effSnowDustData* data;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx mtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	Mtx texMtxA;
	Mtx texMtxB;
	s32 i;
	s32 type;
	f32 f31;
	f32 f;

	cam = camGetPtr(camId);
	data = effect->userdata;
	f31 = (data->alpha / 255.0f) * data->scale;
	type = data->type;
	GXSetTevColor(GX_TEVREG1, (GXColor){ data->topR, data->topG, data->topB, data->topA });
	switch (type) {
		case 0:
		case 6:
			GXSetNumChans(0);
			GXSetNumTevStages(2);
			GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
			GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
			GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);
			GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
			GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_C1, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
			GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
			effGetTexObjN64(8, &texObj);
			GXLoadTexObj(&texObj, GX_TEXMAP0);
			effGetTexObjN64(0x65, &texObj);
			GXLoadTexObj(&texObj, GX_TEXMAP1);
			GXSetNumTexGens(2);
			GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
			GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);
			PSMTXScale(texMtxA, 0.015625f, 0.015625f, 0.0f);
			PSMTXScale(texMtxB, 0.015625f, 0.015625f, 0.0f);
			break;
		case 1:
			GXSetNumChans(0);
			GXSetNumTevStages(2);
			GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
			GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
			GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);
			GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
			GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_C1, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
			GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
			GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
			GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_CPREV, GX_CC_ONE, GX_CC_TEXC, GX_CC_ZERO);
			GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
			effGetTexObjN64(0x66, &texObj);
			GXLoadTexObj(&texObj, GX_TEXMAP0);
			effGetTexObjN64(0x65, &texObj);
			GXLoadTexObj(&texObj, GX_TEXMAP1);
			GXSetNumTexGens(2);
			GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
			GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);
			PSMTXScale(texMtxA, 0.03125f, 0.03125f, 0.0f);
			PSMTXScale(texMtxB, 0.03125f, 0.03125f, 0.0f);
			break;
		case 2:
		case 3:
			GXSetNumChans(0);
			GXSetNumTevStages(1);
			GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
			GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_C1, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
			GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);
			effGetTexObjN64(0x67, &texObj);
			GXLoadTexObj(&texObj, GX_TEXMAP0);
			GXSetNumTexGens(1);
			GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
			PSMTXScale(texMtxA, 0.00390625f, 0.015625f, 0.0f);
			break;
		case 4:
		case 5:
			GXSetNumChans(0);
			GXSetNumTevStages(2);
			GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
			GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_A1, GX_CC_C0);
			GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);
			GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
			GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_C1, GX_CC_A1, GX_CC_CPREV);
			GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
			effGetTexObjN64(0x68, &texObj);
			GXLoadTexObj(&texObj, GX_TEXMAP0);
			GXSetNumTexGens(1);
			GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
			PSMTXScale(texMtxA, 0.015625f, 0.015625f, 0.0f);
			break;
		default:
			break;
	}
	GXSetCullMode(GX_CULL_NONE);
	i = 1;
	data++;
	for (; i < effect->count; i++, data++) {
		if (data->frame < 0) {
			continue;
		}
		PSMTXTrans(mtx, data->x, data->y, data->z);
		PSMTXRotRad(rotMtx, 'y', -camGetPtr(CAMERA_3D)->viewYaw * 0.017453292f);
		PSMTXConcat(mtx, rotMtx, mtx);
		f = data->scale * f31;
		PSMTXScale(scaleMtx, f, f, f);
		PSMTXConcat(mtx, scaleMtx, mtx);
		PSMTXConcat(cam->view, mtx, mtx);
		GXLoadPosMtxImm(mtx, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		GXSetTevColor(GX_TEVREG0,
		              (GXColor){ data->colorR, data->colorG, data->colorB, (u8)(s32)(f31 * (f32)data->alpha) });
		GXSetTevKColor(GX_KCOLOR0,
		               (GXColor){ 0x50 - data->frame, 0x50 - data->frame, 0x50 - data->frame, 0x50 - data->frame });
		GXLoadTexMtxImm(texMtxA, GX_TEXMTX0, GX_MTX2x4);
		PSMTXTrans(mtx, data->texScrollX, data->texScrollY, 0.0f);
		PSMTXConcat(texMtxB, mtx, mtx);
		GXLoadTexMtxImm(mtx, GX_TEXMTX1, GX_MTX2x4);
		if (type == 2) {
			PSMTXTrans(mtx, (f32)((i & 0x1f) * 64), 64.0f, 0.0f);
			PSMTXConcat(texMtxA, mtx, mtx);
			GXLoadTexMtxImm(mtx, GX_TEXMTX0, GX_MTX2x4);
		}
		switch (type) {
			case 0:
			case 6:
				effSetVtxDescN64(size4x4_tex64x64_vtx);
				GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
				tri2(0, 1, 2, 0, 0, 2, 3, 0);
				break;
			case 1:
				effSetVtxDescN64(size8x8_tex64x64_vtx);
				GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
				tri2(0, 1, 2, 0, 0, 2, 3, 0);
				break;
			case 2:
			case 3:
				effSetVtxDescN64(size12x12_tex64x64_vtx);
				GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
				tri2(0, 1, 2, 0, 0, 2, 3, 0);
				break;
			case 4:
			case 5:
				GXSetNumChans(0);
				GXSetNumTevStages(3);
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_A1, GX_CC_C0);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);
				GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_C1, GX_CC_A1, GX_CC_CPREV);
				GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
				GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_KONST, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
				GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
				GXSetTevKColorSel(GX_TEVSTAGE2, GX_TEV_KCSEL_K0);
				effSetVtxDescN64(size16x16_tex64x64_vtx);
				GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
				tri2(0, 1, 2, 0, 0, 2, 3, 0);
				GXSetNumChans(1);
				GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
				GXSetNumTevStages(1);
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ONE);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_KONST, GX_CA_ZERO);
				GXSetTevKAlphaSel(GX_TEVSTAGE0, GX_TEV_KASEL_K0_A);
				effSetVtxDescN64(size32x32_tex32x32_vtx);
				GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0xc);
				tri2(0, 1, 4, 0, 1, 2, 4, 0);
				tri2(0, 4, 3, 0, 4, 2, 3, 0);
				break;
			default:
				break;
		}
	}
}

// clang-format on
