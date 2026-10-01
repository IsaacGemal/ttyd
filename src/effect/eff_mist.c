#include "eff/eff_mist.h"
#include "driver/dispdrv.h"
#include "driver/mapdrv.h"
#include "memory.h"
#include "system.h"
#include "math_ext.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

extern const f32 float_deg2rad_804214e0;
extern const f32 float_32_804214e4;
extern const f32 float_255_804214e8;
extern const f32 float_0p5_804214ec;
extern const f32 float_16_804214f0;
extern const f32 float_0_804214f4;
extern const f32 float_1_804214f8;
extern const f32 float_0p9_804214fc;
extern const f32 float_0p88_80421500;
extern const f32 float_1p02_80421504;
extern const f32 float_2_80421508;
extern const f32 float_0p8_80421514;
extern const f32 float_10_80421518;
extern const f32 float_20_8042151c;
extern const f32 float_6p2832_80421520;
extern const f32 float_360_80421524;
extern const f32 float_1p1_80421528;
extern const f32 float_neg2_8042152c;
extern const f32 float_0p95_80421530;
extern const f32 float_neg0p04_80421534;
extern const f32 float_0p94_80421538;
extern const f32 float_0p3_8042153c;
extern const f32 float_200_80421540;
extern GXColor unk_80423320;

const Vec vec3_802fe908 = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_802fe918 = 4503601774854144.0;
const char str_8042150C[] = "Mist";

EffectEntry* effMistEntry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 w) {
	EffectEntry* entry = effEntry();
	MistWork* work;
	MistWork* ptr;
	s32 numEffects;
	s32 i;
	f32 c;

	if (type == 4) {
		numEffects = 2;
	} else {
		numEffects = 0xb;
	}
	entry->type = str_8042150C;
	entry->count = numEffects;
	work = __memAlloc(HEAP_EFFECT, numEffects * sizeof(MistWork));
	entry->userdata = work;
	entry->callback = effMistMain;
	entry->flags |= 2;
	work->type = type;
	work->x = x;
	work->y = y;
	work->z = z;
	if (count <= 0) {
		work->life = 0x3e8;
	} else {
		work->life = count;
	}
	work->counter = 0;
	work->alpha = float_255_804214e8;
	work->field_0x40 = w - camGetPtr(CAMERA_3D)->viewYaw;
	for (i = 1, ptr = work + 1; i < entry->count; i++, ptr++) {
		ptr->x = (f32)(irand(0x32) - 0x19);
		ptr->y = (f32)(irand(0x32) - 0x19);
		ptr->z = float_0_804214f4;
		ptr->scaleX = ptr->scaleY = ptr->scaleZ = float_0p8_80421514 + (f32)irand(0xc) / float_10_80421518;
		ptr->velX = (f32)(irand(0xa) - 5) / float_20_8042151c;
		ptr->velY = (f32)(irand(0xa) - 5) / float_20_8042151c;
		ptr->velZ = float_0_804214f4;
		ptr->alpha = float_0_804214f4;
		ptr->life = irand(0x1e) + 0x1e;
		ptr->counter = 0;
		ptr->field_0x40 = w - camGetPtr(CAMERA_3D)->viewYaw;
		switch (type) {
		case 0:
			ptr->r = 0xff;
			ptr->g = 0xff;
			ptr->b = 0xff;
			break;
		case 1:
			ptr->r = 0xb;
			ptr->g = 9;
			ptr->b = 0x60;
			break;
		case 2:
			ptr->r = 0xe1;
			ptr->g = 0x99;
			ptr->b = 0xaf;
			break;
		case 3:
			ptr->r = 0x49;
			ptr->g = 0x65;
			ptr->b = 0;
			break;
		case 4:
			ptr->x = float_0_804214f4;
			ptr->y = float_0_804214f4;
			ptr->z = float_1_804214f8;
			ptr->velX = float_1p1_80421528 * (f32)sin(float_6p2832_80421520 * ptr->field_0x40 / float_360_80421524);
			ptr->velY = float_neg2_8042152c;
			c = (f32)cos(float_6p2832_80421520 * ptr->field_0x40 / float_360_80421524);
			ptr->velZ = float_1p1_80421528 * -c;
			ptr->field_0x28 = float_0p95_80421530;
			ptr->field_0x2C = float_neg0p04_80421534;
			ptr->field_0x30 = float_0p94_80421538;
			ptr->r = 0xff;
			ptr->g = 0xff;
			ptr->b = 0xff;
			ptr->scaleX = float_0p3_8042153c;
			ptr->scaleY = float_0p3_8042153c;
			ptr->scaleZ = float_0p3_8042153c;
			ptr->alpha = float_200_80421540;
			break;
		case 5:
			ptr->r = 0x46;
			ptr->g = 0;
			ptr->b = 0x32;
			break;
		case 6:
			ptr->r = 0;
			ptr->g = 0;
			ptr->b = 0;
			break;
		}
	}
	return entry;
}

// NONMATCHING (95.64%): register allocation and the Vec setup instruction schedule differ; the
// int->float magic constant is @240 instead of double_to_int_802fe918 (known pool-naming limitation).
void effMistMain(EffectEntry* effect) {
	MistWork* work = effect->userdata;
	Vec pos;
	Vec sp;
	MistWork* ptr;
	s32 type = work->type;
	s32 i;

	sp = vec3_802fe908;
	sp.x = work->x;
	sp.y = work->y;
	sp.z = work->z;
	pos = sp;
	if (effect->flags & 4) {
		effect->flags &= ~4;
		work->life = 0x20;
	}
	if (work->life < 1000) {
		work->life--;
	}
	work->counter++;
	if (work->life < 0) {
		effDelete(effect);
		return;
	}
	if (work->life < 0x20) {
		work->alpha = (f32)(work->life * 8);
	}
	for (i = 1, ptr = work + 1; i < effect->count; i++, ptr++) {
		switch (type) {
		case 4:
			ptr->x += ptr->velX;
			ptr->y += ptr->velY;
			ptr->z += ptr->velZ;
			ptr->velX *= float_0p9_804214fc;
			ptr->velY *= float_0p88_80421500;
			ptr->velZ *= float_0p9_804214fc;
			ptr->y += ptr->field_0x28;
			ptr->field_0x28 += ptr->field_0x2C;
			ptr->field_0x2C *= ptr->field_0x30;
			ptr->scaleX *= float_1p02_80421504;
			ptr->scaleY *= float_1p02_80421504;
			ptr->scaleZ *= float_1p02_80421504;
			ptr->alpha -= float_2_80421508;
			if (ptr->alpha < float_0_804214f4) {
				ptr->alpha = float_0_804214f4;
			}
			break;
		default:
			if (ptr->counter < 0xff) {
				ptr->counter += irand(5);
				ptr->alpha = (f32)ptr->counter;
				if (ptr->alpha > float_255_804214e8) {
					ptr->alpha = float_255_804214e8;
				}
			} else {
				ptr->alpha -= (f32)irand(5);
				if (ptr->alpha < float_0_804214f4) {
					ptr->counter = 0;
					ptr->alpha = float_0_804214f4;
				}
			}
			ptr->x += ptr->velX;
			ptr->y += ptr->velY;
			ptr->z += ptr->velZ;
			break;
		}
	}
	dispEntry(CAMERA_3D, dispCalcZ(&pos), 2, effMistDisp, effect);
}

#ifdef MUST_MATCH
// Build the GXColor in a helper so the by-value argument lands in stack 0xc, as retail does.
static inline GXColor mistColor(u8 r, u8 g, u8 b, u8 a) {
	GXColor c;
	u8* p = (u8*)&c;
	c = unk_80423320;
	p[0] = r;
	p[1] = g;
	p[2] = b;
	p[3] = a;
	return c;
}
#endif

// NONMATCHING (99.89%): the loop's colour bytes are loaded/scheduled r,b,g with r->r0/g->r5/b->r6,
// where retail loads r,g,b with r->r6/g->r5/b->r0 (a register-colouring tie-break). The
// int->float magic and the 0.0f/1.0f vertex literals also pool as anonymous @NN instead of the named
// double_to_int_802fe918/float_0_804214f4/float_1_804214f8 (pool naming is not scored: effMistEntry
// is 100% with the same anonymous-@ mismatch).
void effMistDisp(CameraId cameraId, void* param) {
	EffectEntry* entry = param;
	MistWork* work;
	CameraEntry* cam;
	GXTexObj texObj;
	GXColor color;
	Mtx mtxTrans;
	Mtx mtxRot;
	Mtx mtxScale;
	Mtx mtxView;
	s32 i;
	s32 alpha;
	f32 half;
	f32 size;

	work = entry->userdata;
	cam = camGetPtr(cameraId);
	alpha = (s32)work->alpha;
	if (work->type == 4) {
		if (mapGetWork()->entries[0].flags & 2) {
			return;
		}
	}
	effGetTexObj(0x44, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
	PSMTXTrans(mtxTrans, work->x, work->y, work->z);
	PSMTXRotRad(mtxRot, 'y', -camGetPtr(cameraId)->viewYaw * float_deg2rad_804214e0);
	PSMTXConcat(mtxTrans, mtxRot, mtxView);
	PSMTXConcat(cam->view, mtxView, mtxView);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	work = &work[1];
	for (i = 1; i < entry->count; i++, work++) {
		PSMTXTrans(mtxTrans, work->x, work->y, work->z);
		PSMTXScale(mtxScale, work->scaleX, work->scaleY, work->scaleZ);
		PSMTXConcat(mtxTrans, mtxScale, mtxTrans);
		PSMTXConcat(mtxView, mtxTrans, mtxTrans);
		GXLoadPosMtxImm(mtxTrans, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
#ifdef MUST_MATCH
		color = mistColor(work->r, work->g, work->b, (s32)(work->alpha * (f32)alpha / float_255_804214e8));
#else
		color = unk_80423320;
		color.r = work->r;
		color.g = work->g;
		color.b = work->b;
		color.a = (s32)(work->alpha * (f32)alpha / float_255_804214e8);
#endif
		size = float_16_804214f0;
		half = -float_32_804214e4 * float_0p5_804214ec;
		GXSetChanMatColor(GX_COLOR0A0, color);
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(half, size, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(size, size, 0.0f);
		GXTexCoord2f32(1.0f, 0.0f);
		GXPosition3f32(size, half, 0.0f);
		GXTexCoord2f32(1.0f, 1.0f);
		GXPosition3f32(half, half, 0.0f);
		GXTexCoord2f32(0.0f, 1.0f);
	}
}
