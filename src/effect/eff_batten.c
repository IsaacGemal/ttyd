#include "eff/eff_batten.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sdata2
extern u32 str_80421980;
extern const u32 dat_80421984;
extern const f32 float_deg2rad_80421988;

// Builds the temporary position; the compiler emits the struct copy this way
// (matches effVaporMain). The anonymous zero literal is deliberate: it must
// land at .rodata:0 before the two EFF_batten strings.
static inline Vec makeVec(f32 x, f32 y, f32 z) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = x;
	temp.y = y;
	temp.z = z;
	return temp;
}

#ifdef MUST_MATCH
static inline GXColor battenColor(u8 alpha) {
	GXColor c = *(GXColor*)&dat_80421984;
	c.a = alpha;
	return c;
}
#endif

EffectEntry* effBattenEntry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry;
	BattenData* data;
	BattenData* elem;
	s32 num;
	s32 i;

	entry = effEntry();
	num = (type == 0) ? 4 : 1;
	entry->type = "Batten";
	entry->count = num;
	data = __memAlloc(HEAP_EFFECT, num * sizeof(BattenData));
	entry->userdata = data;
	entry->callback = effBattenMain;
	entry->flags |= 2;

	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->scale = scale;
	data->alpha = 0xFF;
	if (duration <= 0) {
		data->timer = 0x3E8;
	} else {
		data->timer = duration;
	}
	data->animPoseId = -1;
	PSMTXIdentity(data->matrix);

	i = 1;
	elem = &data[1];
	for (; i < entry->count; i++, elem++) {
		elem->position.x = 0.0f;
		elem->position.y = 0.0f;
		elem->position.z = 0.0f;
		elem->alpha = 0;
		elem->scale = 3.0f;
		elem->state = 0;
		elem->timer = (i - 1) * 10;
	}

	return entry;
}

#ifdef MUST_MATCH
static inline GXColor battenHeaderColor(u8 alpha) {
	GXColor c = *(GXColor*)&str_80421980;
	c.a = alpha;
	return c;
}
#endif

void effBattenMain(EffectEntry* effect) {
	BattenData* data = effect->userdata;
	Vec pos2;
	Vec pos;
	s32 completed = 0;
	s32 type;
	s32 inBattle;
	s32 i;

	pos = makeVec(data->position.x, data->position.y, data->position.z);
	pos2 = pos;
	type = data->type;
	inBattle = gp->inBattle != 0;

	switch (type) {
		case 1:
			if (!animGroupBaseAsync("EFF_batten_x", inBattle, NULL)) {
				return;
			}
			if (data->animPoseId == -1) {
				data->animPoseId = animPoseEntry("EFF_batten_x", inBattle);
				animPoseSetAnim(data->animPoseId, "A_1", TRUE);
			}
			break;
		case 2:
			if (!animGroupBaseAsync("EFF_batten_x", inBattle, NULL)) {
				return;
			}
			if (data->animPoseId == -1) {
				data->animPoseId = animPoseEntry("EFF_batten_x", inBattle);
				animPoseSetAnim(data->animPoseId, "A_2", TRUE);
			}
			break;
		case 3:
			if (!animGroupBaseAsync("EFF_batten_rocket", inBattle, NULL)) {
				return;
			}
			if (data->animPoseId == -1) {
				data->animPoseId = animPoseEntry("EFF_batten_rocket", inBattle);
				animPoseSetAnim(data->animPoseId, "A_1", TRUE);
			}
			break;
	}

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->timer = 0x20;
	}
	if (data->timer < 1000) {
		data->timer--;
	}
	if (data->timer < 0) {
		if (data->animPoseId != -1) {
			animPoseRelease(data->animPoseId);
		}
		effDelete(effect);
		return;
	}
	if (data->timer < 0x20) {
		data->alpha = data->timer * 8;
	}

	if (type == 0) {
		i = 1;
		data++;
		for (; i < effect->count; i++, data++) {
			switch (data->state) {
				case 0:
					if (--data->timer < 0) {
						data->timer = 0;
						data->state++;
					}
					break;
				case 1:
					if (data->timer < 30) {
						data->scale += (1.0f - data->scale) / 8.0f;
						data->alpha += (0x80 - data->alpha) / 8.0f;
						data->timer++;
					} else {
						data->timer = (3 - i) * 10;
						data->state++;
					}
					break;
				case 2:
					if (--data->timer < 0) {
						data->timer = 0;
						data->state++;
					}
					break;
				case 3:
					if (data->timer < 30) {
						f32 t = 10.0f - data->scale;
						data->scale += t / 8.0f;
						data->alpha += (0 - data->alpha) / 8.0f;
						data->timer++;
					} else {
						completed++;
					}
					break;
			}
		}

		if (completed >= effect->count - 1) {
			effDelete(effect);
			return;
		}

		dispEntry(CAMERA_3D, dispCalcZ(&pos2), 2, effBattenDisp, effect);
	} else {
		dispEntry(CAMERA_3D, dispCalcZ(&pos2), 2, effBattenDisp2, effect);
	}
}

void effBattenDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	BattenData* data = effect->userdata;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx mtxBase;
	Mtx mtxRot;
	Mtx mtxScale;
	Mtx mtxFinal;
	GXColor color;
	f32 x1;
	f32 x0;
	s32 i;
	u8 headerAlpha;

	cam = camGetPtr(camId);
	headerAlpha = data->alpha;

	effGetTexObj(0x56, &texObj);
	GXLoadTexObj(&texObj, 0);
	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 4);
	GXSetTevOp(0, 0);
	GXSetCullMode(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xD, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);

	PSMTXTrans(mtxBase, data->position.x, data->position.y, data->position.z);
	PSMTXScale(mtxScale, data->scale, data->scale, data->scale);
	PSMTXRotRad(mtxRot, 'y', -camGetPtr(camId)->viewYaw * float_deg2rad_80421988);
	PSMTXConcat(mtxBase, mtxScale, mtxScale);
	PSMTXConcat(mtxScale, mtxRot, mtxFinal);
	PSMTXConcat(cam->view, mtxFinal, mtxFinal);

	x0 = 32.0f;
	x0 = -x0 / 2.0f;
	x1 = 16.0f;

	for (i = 1, data = &data[1]; i < effect->count; i++, data++) {
		PSMTXTrans(mtxBase, data->position.x, data->position.y, data->position.z);
		PSMTXScale(mtxScale, data->scale, data->scale, data->scale);
		PSMTXConcat(mtxBase, mtxScale, mtxScale);
		PSMTXConcat(mtxFinal, mtxScale, mtxScale);
		GXLoadPosMtxImm(mtxScale, 0);
		GXSetCurrentMtx(0);

#ifdef MUST_MATCH
		color = battenHeaderColor((headerAlpha * data->alpha) / 255);
#else
		color = *(GXColor*)&str_80421980;
		color.a = (headerAlpha * data->alpha) / 255;
#endif
		GXSetChanMatColor(4, color);

		GXBegin(0x80, 0, 4);
		GXPosition3f32(x0, x1, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(x1, x1, 0.0f);
		GXTexCoord2f32(2.0f, 0.0f);
		GXPosition3f32(x1, x0, 0.0f);
		GXTexCoord2f32(2.0f, 2.0f);
		GXPosition3f32(x0, x0, 0.0f);
		GXTexCoord2f32(0.0f, 2.0f);
	}
}

void effBattenDisp2(CameraId camId, void* param) {
	EffectEntry* effect = param;
	BattenData* data = effect->userdata;
	Mtx mtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	GXColor color;

	if (data->animPoseId != -1) {
		PSMTXTrans(mtx, data->position.x, data->position.y, data->position.z);
		PSMTXScale(scaleMtx, data->scale, data->scale, data->scale);
		PSMTXRotRad(rotMtx, 'y', -camGetPtr(camId)->viewYaw * float_deg2rad_80421988);
		PSMTXConcat(mtx, rotMtx, mtx);
		PSMTXConcat(mtx, scaleMtx, mtx);
		PSMTXConcat(mtx, data->matrix, mtx);
		animPoseSetMaterialFlagOn(data->animPoseId, 0x40);
#ifdef MUST_MATCH
		color = battenColor(data->alpha);
#else
		color = *(GXColor*)&dat_80421984;
		color.a = data->alpha;
#endif
		animPoseSetMaterialEvtColor(data->animPoseId, color);
		animPoseMain(data->animPoseId);
		animPoseDrawMtx(data->animPoseId, mtx, 1, 0.0f, 2.0f);
		animPoseDrawMtx(data->animPoseId, mtx, 2, 0.0f, 2.0f);
		animPoseDrawMtx(data->animPoseId, mtx, 3, 0.0f, 2.0f);
	}
}

// The three named .sdata2 objects; defined after the code so they are emitted before the
// functions' literal pool, in the original's order. Every other .sdata2 word is a pooled
// literal written at its use (see the / 2.0f and / 8.0f reciprocals).
__declspec(section ".sdata2") u32 str_80421980 = 0xE8CD2600;
const u32 dat_80421984 = 0xFFFFFF00;
const f32 float_deg2rad_80421988 = 0.017453292f;
