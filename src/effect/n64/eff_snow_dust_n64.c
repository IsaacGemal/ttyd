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

// .sbss2: the default .sbss2 is only used for compiler-generated pool templates, so pair it with a small-data section.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_804232d0;
__declspec(section ".sdata") GXColor unk_804232cc;
__declspec(section ".sdata") GXColor unk_804232c8;


// First object of this unit's .data (the float pair that used to end the previous unit's .data).
extern u32 lbl_803A8AA0[2];

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

// The vertex tables are reached through the one base pointer, the linker would drop the others.
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



//.sbss2

#ifdef MUST_MATCH
// MUST_MATCH: the original builds GXSetTevColor/GXSetTevKColor's by-value GXColor argument in a
// scratch slot and word-copies it up into the outgoing argument slot (local below the copy).  Keeping
// the build and the call together in an inline reproduces that layout (AGENT_BRIEF GXColor quirk).
static inline void snowDustSetTopColor(effSnowDustData* data) {
	GXColor color = unk_804232c8;
	color.r = data->topR;
	color.g = data->topG;
	color.b = data->topB;
	color.a = data->topA;
	GXSetTevColor(GX_TEVREG1, color);
}


static inline void snowDustSetItemColor(effSnowDustData* ptr, f32 f31) {
	GXColor color = unk_804232cc;
	color.r = ptr->colorR;
	color.g = ptr->colorG;
	color.b = ptr->colorB;
	color.a = (u8)(s32)(f31 * (f32)ptr->alpha);
	GXSetTevColor(GX_TEVREG0, color);
}


static inline void snowDustSetItemKColor(effSnowDustData* ptr) {
	GXColor kcolor = unk_804232d0;
	kcolor.r = 0x50 - ptr->frame;
	kcolor.g = 0x50 - ptr->frame;
	kcolor.b = 0x50 - ptr->frame;
	kcolor.a = 0x50 - ptr->frame;
	GXSetTevKColor(GX_KCOLOR0, kcolor);
}

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


extern const Vec vec3_802fb968;


// MWCC's default lifetime analysis and common-subexpression elimination each pick a different
// callee-saved / volatile FPR colouring for this function's seven live f32 values. Turning both off
// and declaring them in reverse (f31..f25) reproduces the target's f29/f25/f28/f27/f26/f31/f30 order;
// splitting the distABf*0.97 multiply and hoisting the 0.5f scale fix the volatile FPRs in the
// int->float conversion block. The two compiler-generated int->float magic doubles keep anonymous
// pool names (@307/@367) instead of the target's double_to_int_*; objdiff ignores those relocs.
#pragma opt_lifetimes off // default lifetime analysis picks a different, worse-matching FPR assignment
#pragma opt_common_subs off // CSE of the pooled constant loads picks different volatile FPRs
void effSnowDustMain(EffectEntry* effect) {
	s32 r31;
	s32 r30;
	s32 life;
	effSnowDustData* data;
	s32 i;
	s32 type;
	s32 r25;
	s32 r24;
	s32 r23;
	s32 camera;
	Vec position2;
	Vec position;
	f32 f31;
	f32 f30;
	f32 f29;
	f32 f28;
	f32 f27;
	f32 f26;
	f32 f25;

	data = effect->userdata;
	position = vec3_802fb968;
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
	f29 = data->spreadX;
	f25 = data->spreadY;
	f28 = data->x;
	f27 = data->y;
	f26 = data->z;
	f31 = (f32)cos(6.2831855f * camGetPtr(CAMERA_3D)->viewYaw / 360.0f);
	f30 = (f32)sin(6.2831855f * camGetPtr(CAMERA_3D)->viewYaw / 360.0f);
	r25 = data->colorR;
	r24 = data->colorG;
	r23 = data->colorB;
	r31 = (s32)f29;
	f29 = f29 * 0.5f;
	r30 = (s32)f25;
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
				f25 = (f32)(rand() % r31) - f29;
				data->x = f25 * f31 + f28 - f30;
				data->y = f27 + (f32)(rand() % r30);
				data->z = f31 + (f25 * f30 + f26);
				data->colorR = r25;
				data->colorG = r24;
				data->colorB = r23;
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
					data->velX = 0.04f * (data->targetVelX - data->velX) + data->velX;
					data->velY = 0.04f * (data->targetVelY - data->velY) + data->velY;
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
					data->velX = 0.04f * (data->targetVelX - data->velX) + data->velX;
					data->velY = 0.04f * (data->targetVelY - data->velY) + data->velY;
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
					data->velY = 0.04f * (data->targetVelY - data->velY) + data->velY;
					f25 = distABf(position2.x, position2.z, data->x, data->z);
					f25 *= 0.97f;
					data->x = f25 * (f32)sin(6.2831855f * (f32)data->angle / 360.0f) + position2.x;
					data->z = f25 * (f32)cos(6.2831855f * (f32)data->angle / 360.0f) + position2.z;
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

#endif

// 100%.  The GXColor arguments are built in the snowDustSet*Color inlines above (scratch slot below
// the outgoing copy, the known MWCC GXColor quirk).  The loop walks the single `data` pointer as the
// original does; the texture offset wraps every 32 items (`(i & 0x1f) * 64`, the target's clrlslwi).
void effSnowDustDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effSnowDustData* data;
	CameraEntry* cam;
	u8* base;
	GXColor color;
	GXColor kcolor;
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

	base = (u8*)lbl_803A8AA0;
	cam = camGetPtr(camId);
	data = effect->userdata;
	f31 = (data->alpha / 255.0f) * data->scale;
	type = data->type;
#ifdef MUST_MATCH
	snowDustSetTopColor(data);
#else
	color = unk_804232c8;
	color.r = data->topR;
	color.g = data->topG;
	color.b = data->topB;
	color.a = data->topA;
	GXSetTevColor(GX_TEVREG1, color);
#endif
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
#ifdef MUST_MATCH
		snowDustSetItemColor(data, f31);
		snowDustSetItemKColor(data);
#else
		color = unk_804232cc;
		color.r = data->colorR;
		color.g = data->colorG;
		color.b = data->colorB;
		color.a = (u8)(s32)(f31 * (f32)data->alpha);
		GXSetTevColor(GX_TEVREG0, color);
		kcolor = unk_804232d0;
		kcolor.r = 0x50 - data->frame;
		kcolor.g = 0x50 - data->frame;
		kcolor.b = 0x50 - data->frame;
		kcolor.a = 0x50 - data->frame;
		GXSetTevKColor(GX_KCOLOR0, kcolor);
#endif
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
				effSetVtxDescN64((effVtxArray*)(base + 0x78));
				GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
				tri2(0, 1, 2, 0, 0, 2, 3, 0);
				break;
			case 1:
				effSetVtxDescN64((effVtxArray*)(base + 0x40));
				GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
				tri2(0, 1, 2, 0, 0, 2, 3, 0);
				break;
			case 2:
			case 3:
				effSetVtxDescN64((effVtxArray*)(base + 0xb0));
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
				effSetVtxDescN64((effVtxArray*)(base + 0xe8));
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
				effSetVtxDescN64((effVtxArray*)(base + 0x120));
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

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
const Vec vec3_802fb968 = { 0.0f, 0.0f, 0.0f };
