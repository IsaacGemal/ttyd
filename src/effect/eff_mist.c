#include "eff/eff_mist.h"
#include "driver/dispdrv.h"
#include "driver/mapdrv.h"
#include "memory.h"
#include "system.h"
#include "math_ext.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sdata2 named prefix loaded by name (definitions at the end of the file). The 0.0f and every
// later constant are literals in the code so the compiler pools them, which reproduces the
// original's mixed named/pooled .sdata2 order.
extern const f32 float_deg2rad_804214e0;
extern const f32 float_32_804214e4;
extern const f32 float_255_804214e8;
extern const f32 float_0p5_804214ec;

// .sbss2 4-byte gap after the unit's colour template.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor gap_10_80423324_sbss2;
#pragma section ".sdata" ".sbss"

EffectEntry* effMistEntry(s32 type, s32 count, f32 x, f32 y, f32 z, f32 yaw) {
	EffectEntry* entry = effEntry();
	MistWork* work;
	MistWork* ptr;
	s32 numEffects;
	s32 i;
	f32 cosYaw;
	f32 sinYaw;

	if (type == 4) {
		numEffects = 2;
	} else {
		numEffects = 0xb;
	}
	entry->type = "Mist";
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
	work->yaw = yaw - camGetPtr(CAMERA_3D)->viewYaw;
	for (i = 1, ptr = work + 1; i < entry->count; i++, ptr++) {
		ptr->x = (f32)(irand(0x32) - 0x19);
		ptr->y = (f32)(irand(0x32) - 0x19);
		ptr->z = 0.0f;
		ptr->scaleX = ptr->scaleY = ptr->scaleZ = 0.8f + (f32)irand(0xc) / 10.0f;
		ptr->velX = (f32)(irand(0xa) - 5) / 20.0f;
		ptr->velY = (f32)(irand(0xa) - 5) / 20.0f;
		ptr->velZ = 0.0f;
		ptr->alpha = 0.0f;
		ptr->life = irand(0x1e) + 0x1e;
		ptr->counter = 0;
		ptr->yaw = yaw - camGetPtr(CAMERA_3D)->viewYaw;
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
			ptr->x = 0.0f;
			ptr->y = 0.0f;
			ptr->z = 1.0f;
			sinYaw = (f32)sin(6.2831855f * ptr->yaw / 360.0f);
			ptr->velX = 1.1f * sinYaw;
			ptr->velY = -2.0f;
			cosYaw = (f32)cos(6.2831855f * ptr->yaw / 360.0f);
			ptr->velZ = 1.1f * -cosYaw;
			ptr->riseSpeed = 0.95f;
			ptr->riseAccel = -0.04f;
			ptr->riseDamp = 0.94f;
			ptr->r = 0xff;
			ptr->g = 0xff;
			ptr->b = 0xff;
			ptr->scaleX = 0.3f;
			ptr->scaleY = 0.3f;
			ptr->scaleZ = 0.3f;
			ptr->alpha = 200.0f;
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

static inline Vec mistMakeVec(f32 x, f32 y, f32 z) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = x;
	temp.y = y;
	temp.z = z;
	return temp;
}

// Matches the original: the Vec copy is built through mistMakeVec so its word loads are not hoisted
// into the prologue, `work` doubles as the loop pointer, and `type`/`i` land in r29/r30.
void effMistMain(EffectEntry* effect) {
	MistWork* work;
	Vec pos;
	Vec origin;
	s32 i;
	s32 type;

	work = effect->userdata;
	origin = mistMakeVec(work->x, work->y, work->z);
	pos = origin;
	type = work->type;
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
	for (i = 1, work = work + 1; i < effect->count; i++, work++) {
		switch (type) {
		case 4:
			work->x += work->velX;
			work->y += work->velY;
			work->z += work->velZ;
			work->velX *= 0.9f;
			work->velY *= 0.88f;
			work->velZ *= 0.9f;
			work->y += work->riseSpeed;
			work->riseSpeed += work->riseAccel;
			work->riseAccel *= work->riseDamp;
			work->scaleX *= 1.02f;
			work->scaleY *= 1.02f;
			work->scaleZ *= 1.02f;
			work->alpha -= 2.0f;
			if (work->alpha < 0.0f) {
				work->alpha = 0.0f;
			}
			break;
		default:
			if (work->counter < 0xff) {
				work->counter += irand(5);
				work->alpha = (f32)work->counter;
				if (work->alpha > float_255_804214e8) {
					work->alpha = float_255_804214e8;
				}
			} else {
				work->alpha -= (f32)irand(5);
				if (work->alpha < 0.0f) {
					work->counter = 0;
					work->alpha = 0.0f;
				}
			}
			work->x += work->velX;
			work->y += work->velY;
			work->z += work->velZ;
			break;
		}
	}
	dispEntry(CAMERA_3D, dispCalcZ(&pos), 2, effMistDisp, effect);
}

// Matches the original. The int->float magic pools as its anonymous double (@261), which the linker
// names double_to_int_802fe918 from the map.
void effMistDisp(CameraId cameraId, void* param) {
	EffectEntry* entry = param;
	MistWork* work;
	CameraEntry* cam;
	GXTexObj texObj;
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
		if (mapGetWork()->entries[0].flags & MAPENTRY_FLAG_BLEND) {
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
		GXSetChanMatColor(GX_COLOR0A0, (GXColor){ work->r, work->g, work->b, (u8)(work->alpha * (f32)alpha / float_255_804214e8) });
		size = 16.0f;
		half = -float_32_804214e4 * float_0p5_804214ec;
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

// .sdata2 constants the code loads by name, defined after it (defined first, the compiler folds them).
// Only the prefix the original has before its 0.0 is named; every later constant is a literal in the
// code so the compiler pools it in the original's order instead of duplicating the named zero/one.
const f32 float_deg2rad_804214e0 = 0.017453292f;
const f32 float_32_804214e4 = 32.0f;
const f32 float_255_804214e8 = 255.0f;
const f32 float_0p5_804214ec = 0.5f;
