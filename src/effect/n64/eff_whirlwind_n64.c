#include "eff/n64/eff_whirlwind_n64.h"
#include "eff/eff_n64.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "mario/mario.h"

// .sbss2 zero GXColor scratch objects (declared in reverse: placed in reverse of declaration order)
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor gap_10_8042308C_sbss2;
__declspec(section ".sdata") GXColor unk_80423088;
#include <stdlib.h>

// .sdata color tables (original symbols col_r/col_g/col_b at 0x80412D28-0x80412D33).
__declspec(section ".sdata") u8 col_r[4] = { 0xFF, 0xFF, 0x40, 0x00 };
__declspec(section ".sdata") u8 col_g[4] = { 0xFF, 0x40, 0x40, 0x00 };
__declspec(section ".sdata") u8 col_b[8] = { 0xFF, 0x40, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00 };

// .data vertex array (original symbol eff_whirlwind_v, 0x16C bytes).
u32 eff_whirlwind_v[0x16C / 4] = {
	0xFFFBFFFE, 0x000903AB, 0x0000FFFF, 0xFFFFFFFB, 0x00020009, 0x03AB0200, 0xFFFFFFFF,
	0xFFF70002, 0x00050355, 0x0200FFFF, 0xFFFFFFF7, 0xFFFE0005, 0x03550000, 0xFFFFFFFF,
	0x0000FFFE, 0x000A0400, 0x0000FFFF, 0xFFFF0000, 0x0002000A, 0x04000200, 0xFFFFFFFF,
	0xFFF60002, 0x00000300, 0x0200FFFF, 0xFFFFFFF6, 0xFFFE0000, 0x03000000, 0xFFFFFFFF,
	0x0005FFFE, 0x00090055, 0x0000FFFF, 0xFFFF0005, 0x00020009, 0x00550200, 0xFFFFFFFF,
	0x00000002, 0x000A0000, 0x0200FFFF, 0xFFFF0000, 0xFFFE000A, 0x00000000, 0xFFFFFFFF,
	0x0009FFFE, 0x000500AB, 0x0000FFFF, 0xFFFF0009, 0x00020005, 0x00AB0200, 0xFFFFFFFF,
	0x000AFFFE, 0x00000100, 0x0000FFFF, 0xFFFF000A, 0x00020000, 0x01000200, 0xFFFFFFFF,
	0x0009FFFE, 0xFFFB0155, 0x0000FFFF, 0xFFFF0009, 0x0002FFFB, 0x01550200, 0xFFFFFFFF,
	0xFFF70002, 0xFFFB02AB, 0x0200FFFF, 0xFFFFFFF7, 0xFFFEFFFB, 0x02AB0000, 0xFFFFFFFF,
	0x0005FFFE, 0xFFF701AB, 0x0000FFFF, 0xFFFF0005, 0x0002FFF7, 0x01AB0200, 0xFFFFFFFF,
	0x0000FFFE, 0xFFF60200, 0x0000FFFF, 0xFFFF0000, 0x0002FFF6, 0x02000200, 0xFFFFFFFF,
	0xFFFBFFFE, 0xFFF70255, 0x0000FFFF, 0xFFFFFFFB, 0x0002FFF7, 0x02550200, 0xFFFFFFFF,
};

// Note: under MUST_MATCH the `(f32)(i - 1) / 4.0f` is exact and compiles to a multiply by 0.25; writing it as
// `* 0.25f` makes MWCC allocate the int->float conversion to f0 instead of the original's f1.
EffectEntry* effWhirlwindN64Entry(s32 type, s32 mario, s32 life, f32 size) {
	s32 v31;
	s32 v30;
	s32 v29;
	s32 v28;
	EffectEntry* effect;
	EffWhirlwindData* data;
	EffWhirlwindData* p;
	f32 f;
	s32 i;
	s32 color;
	effect = effEntry();
	effect->type = "WhirlwindN64";
	effect->count = 5;
	data = __memAlloc(HEAP_EFFECT, 0x168);
	effect->userdata = data;
	effect->callback = effWhirlwindMain;
	f = 1.2f * size;
	data->type = type;
	data->mario = (MarioWork*)mario;
	data->scale = f;
	data->baseScale = f;
	data->lifetime = life;
	data->life = life;
	data->tevReg0A = 0;
	data->timer = 0;

	p = data + 1;
	i = 1;
	v31 = 11;
	v30 = 120;
	v29 = 25;
	v28 = 38;
	for (; i < 5; i++, p++) {
		switch (type) {
		case 0:
		case 2:
		case 4:
		case 6:
			p->x = 0.0f;
			p->y = (f32)v31;
			p->z = 0.0f;
			p->rotXVel = 0.0f;
			p->rotYVel = 60.0f;
			p->rotZVel = 0.0f;
			p->rotX = 0.0f;
			p->rotY = (f32)v30;
			p->rotZ = 0.0f;
			p->scale = 1.0f;
			break;
		case 1:
		case 3:
		case 5:
		case 7:
			p->x = 0.0f;
			p->y = (f32)v31;
			p->z = 0.0f;
			p->rotXVel = 0.0f;
			p->rotYVel = -60.0f;
			p->rotZVel = 0.0f;
			p->rotX = 0.0f;
			p->rotY = (f32)v30;
			p->rotZ = 0.0f;
			p->scale = 1.0f;
			p->tevReg0B = 0xFF;
			p->tevReg0G = 0xFF;
			p->tevReg0R = 0xFF;
			break;
		default:
			p->x = 0.0f;
			p->y = 0.5f * ((MarioWork*)mario)->wPlayerCollisionBox.y;
			p->z = 0.0f;
			p->rotXVel = (f32)(((rand() % 2) * 2 - 1) * 4);
			p->rotYVel = 0.0f;
			p->rotZVel = (f32)(((rand() % 2) * 2 - 1) * 4);
			p->scale = ((f32)(i - 1) / 4.0f) * 0.5f + 0.5f;
			p->rotX = (f32)v29;
			p->rotY = (f32)((i - 1) * 360 / 4);
			p->rotZ = (f32)(360 - v28);
			break;
		}

		switch (type) {
		case 2:
		case 3:
			color = 1;
			break;
		case 4:
		case 5:
			color = 2;
			break;
		case 6:
		case 7:
			color = (i & 1) + 1;
			break;
		default:
			color = 0;
			break;
		}
		p->tevReg0R = col_r[color];
		p->tevReg0G = col_g[color];
		p->tevReg0B = col_b[color];

		v31 += 7;
		v30 += 120;
		v29 += 25;
		v28 += 38;
	}
	return effect;
}

void effWhirlwindMain(EffectEntry* effect) {
	EffWhirlwindData* data = effect->userdata;
	Vec pos2;
	Vec pos;
	s32 life;
	s32 type;
	s32 i;

	pos = (Vec){ 0.0f, 0.0f, 0.0f };
	pos.x = data->x;
	pos.y = data->y;
	pos.z = data->z;
	pos2 = pos;

	data->life--;
	data->timer++;
	life = data->life;
	if (life < 0) {
		effDelete(effect);
		return;
	}

	type = data->type;
	switch (type) {
	case 8:
		if (life > 5) {
			data->tevReg0A = (s32)(0.05f * (255 - data->tevReg0A) + data->tevReg0A);
		}
		data->scale = data->baseScale * (f32)life / (f32)data->lifetime;
		if (life < 10) {
			data->tevReg0A = life * 25;
		}
		break;
	default:
		if (life > 5) {
			data->tevReg0A = (s32)(0.3f * (100 - data->tevReg0A) + data->tevReg0A);
		}
		if (life < 10) {
			data->tevReg0A *= 0.8f;
			data->scale += 0.1f * (2.0f * data->baseScale - data->scale);
		}
		break;
	}

	if (data->mario != NULL) {
		MarioWork* ptr = data->mario;
		data->x = ptr->position.x;
		data->y = ptr->position.y;
		data->z = ptr->position.z;
	}

	for (i = 1, data++; i < effect->count; i++, data++) {
		switch (type) {
		case 8:
			if (i & 1) {
				data->rotYVel += 0.2f * (40.0f - data->rotYVel);
			} else {
				data->rotYVel += 0.2f * (-40.0f - data->rotYVel);
			}
			data->rotX += data->rotXVel;
			data->rotY += data->rotYVel;
			data->rotZ += data->rotZVel;
			break;
		default:
			if (life < 10) {
				data->rotY += data->rotYVel * (f32)life / 10.0f;
			} else {
				data->rotY += data->rotYVel;
			}
			break;
		}
	}

	dispEntry(CAMERA_3D, dispCalcZ(&pos2), 2, effWhirlwindDisp, effect);
}

// TEV colour setup for one particle; kept as an inline so the 4-byte colour scratch lands in the
// low stack slot the original uses (same idiom as eff_bush_n64's bushSetColor).
static inline void whirlwindSetColor(u8 r, u8 g, u8 b, u8 a) {
	GXColor color = unk_80423088;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	GXSetTevColor(GX_TEVREG0, color);
}

void effWhirlwindDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	CameraEntry* cam = camGetPtr(camId);
	EffWhirlwindData* data = effect->userdata;
	GXTexObj texObj;
	Mtx trans;
	Mtx scale;
	Mtx rot;
	Mtx work;
	s32 type;
	s32 alpha;
	f32 scaleA;
	f32 scaleB;
	s32 i;

	alpha = data->tevReg0A;
	scaleA = data->scale;
	scaleB = data->baseScale;
	type = data->type;

	GXSetNumChans(0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	PSMTXScale(scale, 0.03125f, 0.03125f, 0.0f);
	GXLoadTexMtxImm(scale, GX_TEXMTX0, GX_MTX2x4);
	effGetTexObjN64(0x58, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);

	PSMTXTrans(trans, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(trans, rot, work);
	PSMTXConcat(cam->view, work, work);

	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64((effVtxArray*)eff_whirlwind_v);

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		PSMTXTrans(trans, data->x, data->y, data->z);
		PSMTXRotRad(rot, 'x', 0.017453292f * data->rotX);
		PSMTXConcat(trans, rot, trans);
		PSMTXRotRad(rot, 'z', 0.017453292f * data->rotZ);
		PSMTXConcat(trans, rot, trans);
		PSMTXScale(scale, scaleA * data->scale, scaleA * data->scale, scaleA * data->scale);
		PSMTXConcat(trans, scale, trans);
		PSMTXRotRad(rot, 'y', 0.017453292f * data->rotY);
		PSMTXConcat(trans, rot, rot);

		if (type < 8) {
			PSMTXTrans(trans, 3.0f - 3.0f * (scaleA - scaleB) / scaleB, 0.0f, 0.0f);
			PSMTXConcat(rot, trans, trans);
		} else {
			PSMTXTrans(trans, 2.0f, 0.0f, 0.0f);
			PSMTXConcat(rot, trans, trans);
		}
		PSMTXConcat(work, trans, trans);

		GXLoadPosMtxImm(trans, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);

		whirlwindSetColor(data->tevReg0R, data->tevReg0G, data->tevReg0B, alpha);
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0x48);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
		tri2(4, 5, 1, 0, 4, 1, 0, 0);
		tri2(3, 2, 6, 0, 3, 6, 7, 0);
		tri2(8, 9, 10, 0, 8, 10, 11, 0);
		tri2(12, 13, 9, 0, 12, 9, 8, 0);
		tri2(14, 15, 13, 0, 14, 13, 12, 0);
		tri2(16, 17, 15, 0, 16, 15, 14, 0);
		tri2(7, 6, 18, 0, 7, 18, 19, 0);
		tri2(20, 21, 17, 0, 20, 17, 16, 0);
		tri2(22, 23, 21, 0, 22, 21, 20, 0);
		tri2(24, 25, 23, 0, 24, 23, 22, 0);
		tri2(19, 18, 25, 0, 19, 25, 24, 0);
	}
}

#pragma explicit_zero_data on
__declspec(section ".data") u32 gap_05_8039A604_data = 0;
#pragma explicit_zero_data reset
