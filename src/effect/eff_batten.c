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

// .rodata
const Vec vec3_802ffa50 = { 0.0f, 0.0f, 0.0f };
const char str_802FFA5C[] = "EFF_batten_x";
const char str_802FFA6C[] = "EFF_batten_rocket";

// .sdata2
extern u32 str_80421980;
extern const u32 dat_80421984;
extern const f32 float_deg2rad_80421988;
extern const f32 float_0_8042198c;
extern const f32 float_2_80421990;
extern const f32 float_32_80421994;
extern const f32 float_0p5_80421998;
extern const f32 float_16_8042199c;
extern const char str_804219A0[4];
extern const char str_804219A4[4];
extern const f32 float_1_804219a8;
extern const f32 float_0p125_804219ac;
extern const f32 float_10_804219b0;
extern const f32 float_3_804219b4;

#ifdef MUST_MATCH
static inline GXColor battenColor(u8 alpha) {
	GXColor c = *(GXColor*)&dat_80421984;
	c.a = alpha;
	return c;
}
#endif

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
		animPoseDrawMtx(data->animPoseId, mtx, 1, float_0_8042198c, float_2_80421990);
		animPoseDrawMtx(data->animPoseId, mtx, 2, float_0_8042198c, float_2_80421990);
		animPoseDrawMtx(data->animPoseId, mtx, 3, float_0_8042198c, float_2_80421990);
	}
}

#ifdef MUST_MATCH
static inline GXColor battenHeaderColor(u8 alpha) {
	GXColor c = *(GXColor*)&str_80421980;
	c.a = alpha;
	return c;
}
#endif

// NONMATCHING (98.47%): the vertex float literals become anonymous @248/@327 sdata2 symbols instead of
// float_0_8042198c/float_2_80421990 (declaring them volatile matches but breaks Disp2), the color temp slot is
// swapped as in Disp2, and a few integer registers differ (data in r29 not r27, element-ptr setup order).
void effBattenDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	BattenData* data = effect->userdata;
	BattenData* elem;
	CameraEntry* cam;
	GXTexObj texObj;
	Mtx mtxBase;
	Mtx mtxRot;
	Mtx mtxScale;
	Mtx mtxFinal;
	GXColor color;
	u8 headerAlpha;
	f32 x1;
	f32 x0;
	s32 i;

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

	x0 = -float_32_80421994 * float_0p5_80421998;
	x1 = float_16_8042199c;

	for (i = 1, elem = &data[1]; i < effect->count; i++, elem++) {
		PSMTXTrans(mtxBase, elem->position.x, elem->position.y, elem->position.z);
		PSMTXScale(mtxScale, elem->scale, elem->scale, elem->scale);
		PSMTXConcat(mtxBase, mtxScale, mtxScale);
		PSMTXConcat(mtxFinal, mtxScale, mtxScale);
		GXLoadPosMtxImm(mtxScale, 0);
		GXSetCurrentMtx(0);

#ifdef MUST_MATCH
		color = battenHeaderColor((headerAlpha * elem->alpha) / 255);
#else
		color = *(GXColor*)&str_80421980;
		color.a = (headerAlpha * elem->alpha) / 255;
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

// NONMATCHING (94.66%): vec3 relocates through the section base ...rodata.0 instead of vec3_802ffa50, the
// int->float double constants sit at 0x38/0x40 not 0x30/0x38, and several r/f registers are named differently
// (r28/r29 swap, f30/f31 fmadds operand order).
void effBattenMain(EffectEntry* effect) {
	BattenData* data = effect->userdata;
	BattenData* elem;
	Vec pos2;
	Vec pos;
	s32 type;
	s32 inBattle;
	s32 completed = 0;
	s32 i;

	pos = vec3_802ffa50;
	pos.x = data->position.x;
	pos.y = data->position.y;
	pos.z = data->position.z;
	pos2 = pos;
	type = data->type;
	inBattle = gp->inBattle != 0;

	switch (type) {
		case 1:
			if (!animGroupBaseAsync(str_802FFA5C, inBattle, NULL)) {
				return;
			}
			if (data->animPoseId == -1) {
				data->animPoseId = animPoseEntry(str_802FFA5C, inBattle);
				animPoseSetAnim(data->animPoseId, str_804219A0, TRUE);
			}
			break;
		case 2:
			if (!animGroupBaseAsync(str_802FFA5C, inBattle, NULL)) {
				return;
			}
			if (data->animPoseId == -1) {
				data->animPoseId = animPoseEntry(str_802FFA5C, inBattle);
				animPoseSetAnim(data->animPoseId, str_804219A4, TRUE);
			}
			break;
		case 3:
			if (!animGroupBaseAsync(str_802FFA6C, inBattle, NULL)) {
				return;
			}
			if (data->animPoseId == -1) {
				data->animPoseId = animPoseEntry(str_802FFA6C, inBattle);
				animPoseSetAnim(data->animPoseId, str_804219A0, TRUE);
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
		elem = &data[1];
		for (; i < effect->count; i++, elem++) {
			switch (elem->state) {
				case 0:
					if (--elem->timer < 0) {
						elem->timer = 0;
						elem->state++;
					}
					break;
				case 1:
					if (elem->timer < 30) {
						elem->scale += (float_1_804219a8 - elem->scale) * float_0p125_804219ac;
						elem->alpha += (0x80 - elem->alpha) * float_0p125_804219ac;
						elem->timer++;
					} else {
						elem->timer = (3 - i) * 10;
						elem->state++;
					}
					break;
				case 2:
					if (--elem->timer < 0) {
						elem->timer = 0;
						elem->state++;
					}
					break;
				case 3:
					if (elem->timer < 30) {
						elem->scale += (float_10_804219b0 - elem->scale) * float_0p125_804219ac;
						elem->alpha += (0 - elem->alpha) * float_0p125_804219ac;
						elem->timer++;
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

const char str_802FFA90[] = "Batten";

EffectEntry* effBattenEntry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry;
	BattenData* data;
	BattenData* elem;
	s32 num;
	s32 i;

	entry = effEntry();
	num = (type == 0) ? 4 : 1;
	entry->type = str_802FFA90;
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
		elem->scale = float_3_804219b4;
		elem->state = 0;
		elem->timer = (i - 1) * 10;
	}

	return entry;
}
