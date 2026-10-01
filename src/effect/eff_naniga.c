#include "eff/eff_naniga.h"

#include "data/item_data.h"
#include "eff/eff_itemget.h"
#include "eff/eff_kemutest.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "evt/evt_cmd.h"
#include "evt/evt_snd.h"
#include "manager/evtmgr.h"
#include "mario/mariost.h"
#include "memory.h"
#include "pmario_sound.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <stdlib.h>

extern ItemData itemDataTable[];
extern const f32 float_16_80421b54;
extern const f32 float_deg2rad_80421b58;
extern const f32 float_20_80421b5c;
extern const f32 float_10_80421b64;
extern const f32 float_0_80421b60;
extern const f32 float_1p5_80421b68;
extern const f32 float_40_80421b6c;
extern const f32 float_0p5_80421b70;
extern const f32 float_1_80421b74;
extern const f32 float_neg40_80421b78;
extern const f32 float_0p9_80421b7c;
extern const f32 float_3_80421b94;
extern const f32 float_100_80421b80;
extern const f32 float_4_80421b84;
extern const f32 float_1024_80421b8c;
extern const f32 float_0p8_80421b90;
extern const GXColor dat_80421b48;
extern const GXColor dat_80421b4c;
extern const GXColor dat_80421b50;

// ItemData.icon_id is read unsigned (lhz) in the original; the header's s16 is kept because other files'
// matches depend on it.
#ifdef MUST_MATCH
#define ICON_ID(field) (*(u16*)&(field))
#else
#define ICON_ID(field) ((u16)(field))
#endif

EVT_BEGIN(sound_evt)
CALL(evt_snd_sfxon_3d, STRING("SFX_ITEM_WHAT3"), LW(0), LW(1), LW(2), 0)
WAIT_MS(0x341)
CALL(evt_snd_sfxon_3d, STRING("SFX_ITEM_WHAT4"), LW(0), LW(1), LW(2), 0)
WAIT_MS(0x341)
CALL(evt_snd_sfxon_3d, STRING("SFX_ITEM_WHAT5"), LW(0), LW(1), LW(2), 0)
RETURN
EVT_END()

void effNanigaDisp3(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effNanigaData* data = effect->userdata;
	ItemData* item;
	Mtx trans;
	Mtx rot;
	Mtx scale;
	Mtx mtx;

	camGetPtr(camId);
	item = &itemDataTable[data->itemId];
	PSMTXTrans(trans, data->posX, data->posY - float_16_80421b54, data->posZ);
	PSMTXRotRad(rot, 'y', float_deg2rad_80421b58 * -camGetPtr(camId)->viewYaw);
	PSMTXScale(scale, data->scale, data->scale, data->scale);
	PSMTXConcat(trans, rot, trans);
	PSMTXConcat(trans, scale, mtx);

	if (data->state < 4) {
		iconDispGxCol(mtx, 0, 0x184, dat_80421b4c);
	}
	else {
		iconDispGxCol(mtx, 0, ICON_ID(item->icon_id), dat_80421b50);
	}
}

void effNanigaDisp2(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effNanigaData* data = effect->userdata;
	Mtx trans;
	Mtx rot;
	Mtx scale;
	Mtx mtx;

	camGetPtr(camId);
	PSMTXTrans(trans, data->posX, data->posY - float_20_80421b5c, data->posZ);
	PSMTXRotRad(rot, 'y', float_deg2rad_80421b58 * -camGetPtr(camId)->viewYaw);
	PSMTXScale(scale, data->scale, data->scale, data->scale);
	PSMTXConcat(trans, rot, trans);
	PSMTXConcat(trans, scale, mtx);
	animPoseSetMaterialFlagOn(data->poseId, 0x40);
	animPoseSetMaterialEvtColor(data->poseId, (GXColor){ 0xFF, 0xFF, 0xFF, data->color });
	animPoseMain(data->poseId);
	animPoseDrawMtx(data->poseId, mtx, 1, float_0_80421b60, float_10_80421b64);
	animPoseDrawMtx(data->poseId, mtx, 2, float_0_80421b60, float_10_80421b64);
	animPoseDrawMtx(data->poseId, mtx, 3, float_0_80421b60, float_10_80421b64);
}

// The target loads the four single-precision literals (0.5/0.0/1.0/0.9) from the named .sdata2 objects, which
// are the compiler's own pooled literal entries; referencing them by name makes MWCC keep them in callee-saved
// FPRs (f26+), so the literals stay literal here and pool to anonymous @440-@443 slots at different offsets.
// The two compiler-generated int<->float magic doubles also sit 8 bytes later in .rodata, because the target's
// .rodata has two unreferenced zero Vec3 objects before them (a Main/Entry inlining difference). Those pool
// symbol names do not count against the score (objdiff resolves them by value).
// The size conversion's `fmuls` is fixed by holding data->scale in a local across the conversion call, which
// also makes MWCC emit the target's `lfs f28`/`lfs f0` order and the `fmuls f1, f0, f28` operand order.
// NONMATCHING (99.78%): the two u16->f32 conversions (centerY/centerX) get their 2^52 mask, 0.5 and the
// converted value in different caller-saved FPRs than the original (target f3/f1/f2, ours f2/f3/f1), flipping
// the `fsubs`/`fmsubs`/`fmadds` registers; reordering operands or using an extern/literal does not change it.
void effNanigaDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effNanigaData* data = effect->userdata;
	CameraEntry* camera;
	Mtx mtx;
	Mtx rot;
	Mtx scale;
	Mtx base;
	GXTexObj tex;
	u32 scissorLeft;
	u32 scissorTop;
	u32 scissorWidth;
	u32 scissorHeight;
	u32 centerX;
	u32 centerY;
	u32 size;
	f32 extent;
	f32 pos20;
	f32 halfSize;
	f32 savedScale;
	f32 sizeScale;
	s32 i;

	camera = camGetPtr(camId);
	savedScale = data->scale;
	data->scale = savedScale * float_1p5_80421b68;
	PSMTXTrans(mtx, data->field_0x10, data->field_0x14, data->field_0x18);
	PSMTXRotRad(rot, 'y', float_deg2rad_80421b58 * -camGetPtr(camId)->viewYaw);
	PSMTXScale(scale, data->scale, data->scale, data->scale);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXConcat(mtx, scale, base);
	GXGetScissor(&scissorLeft, &scissorTop, &scissorWidth, &scissorHeight);
	sizeScale = data->scale;
	size = (u32)(float_40_80421b6c * sizeScale);
	extent = float_20_80421b5c * sizeScale;
	centerY = (u32)((f32)gp->efbHeight * 0.5f - data->field_0x14);
	centerX = (u32)((f32)gp->fbWidth * 0.5f + data->field_0x10);
	GXSetScissor((u32)((f32)centerX - extent), (u32)((f32)centerY - extent), size, size);

	effGetTexObj(0x57, &tex);
	GXLoadTexObj(&tex, 0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x3c, 0, 0x7d);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 0xff);
	GXSetTevOp(0, 3);
	GXSetNumChans(0);
	GXSetCullMode(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xd, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xd, 1, 4, 0);
	PSMTXConcat(camera->view, base, mtx);
	GXLoadPosMtxImm(mtx, 0);
	GXSetCurrentMtx(0);
	GXBegin(0x80, 0, 4);
	pos20 = float_20_80421b5c;
	halfSize = -float_40_80421b6c * float_0p5_80421b70;
	GXPosition3f32(halfSize, pos20, 0.0f);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3f32(pos20, pos20, 0.0f);
	GXTexCoord2f32(1.0f, 0.0f);
	GXPosition3f32(pos20, halfSize, 0.0f);
	GXTexCoord2f32(1.0f, 1.0f);
	GXPosition3f32(halfSize, halfSize, 0.0f);
	GXTexCoord2f32(0.0f, 1.0f);

	for (i = 0; i < data->count + 5; i++) {
		iconGetTexObj(&tex, ICON_ID(itemDataTable[data->itemList[i % data->count]].icon_id));
		GXLoadTexObj(&tex, 0);
		PSMTXTrans(mtx, 0.0f, data->field_0x34 + float_neg40_80421b78 * (f32)i, 0.0f);
		PSMTXScale(scale, 0.9f, 0.9f, 0.9f);
		PSMTXConcat(base, mtx, mtx);
		PSMTXConcat(mtx, scale, mtx);
		PSMTXConcat(camera->view, mtx, mtx);
		GXLoadPosMtxImm(mtx, 0);
		GXSetCurrentMtx(0);
		GXBegin(0x80, 0, 4);
		GXPosition3f32(halfSize, pos20, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(pos20, pos20, 0.0f);
		GXTexCoord2f32(1.0f, 0.0f);
		GXPosition3f32(pos20, halfSize, 0.0f);
		GXTexCoord2f32(1.0f, 1.0f);
		GXPosition3f32(halfSize, halfSize, 0.0f);
		GXTexCoord2f32(0.0f, 1.0f);
	}

	GXSetScissor(scissorLeft, scissorTop, scissorWidth, scissorHeight);
	data->scale = savedScale;
}

inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };

	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}



// NONMATCHING (96.35%): remaining diffs are .rodata layout (the zero-Vec/double/string pool offsets),
// the compiler's switch layout, and register allocation.
void effNanigaMain(EffectEntry* effect) {
	effNanigaData* data = effect->userdata;
	BOOL inBattle;
	Vec vec;
	EventEntry* event;
	s32 i;
	f32 scale;

	vec = initializeVec((Vec*)&data->posX);
	inBattle = gp->inBattle != 0;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		if (data->poseId != -1) {
			animPoseRelease(data->poseId);
		}
		effDelete(effect);
		return;
	}

	switch (data->state) {
	case 0:
		data->state = 1;
		data->timer = 30;
		// fallthrough
	case 1:
		if (data->timer == 30) {
			for (i = 0; i < 4; i++) {
				scale = float_0p5_80421b70 + (f32)irand(200) / float_100_80421b80;
				effKemuTestDrawCam(effKemuTestEntry(2, data->field_0x10, data->field_0x14, data->field_0x18, scale), 8);
			}
			psndSFXOn_3D("SFX_ITEM_WHAT1", &vec);
		}
		data->timer--;
		if (data->timer < 0) {
			data->timer = 60;
			data->state++;
		}
		break;
	case 2:
		data->timer--;
		if (data->timer < 0) {
			data->timer = 0;
			data->state++;
			data->sfxId = psndSFXOn_3D("SFX_ITEM_WHAT2", &vec);
		}
		break;
	case 3:
		data->field_0x34 = intplGetValue(5, data->timer, 300, data->start, data->end);
		data->timer++;
		if (data->timer > 300) {
			psndSFXOff(data->sfxId);
			switch (data->mode) {
			case 0:
				data->timer = 60;
				data->state = 10;
				break;
			case 1:
				data->timer = 0;
				data->state = 4;
				break;
			case 2:
				data->timer = 0;
				data->state = 5;
				break;
			}
		}
		break;
	case 4:
		if (data->timer < 10) {
			data->timer++;
		}
		else if (data->timer < 20) {
			data->field_0x34 -= float_4_80421b84;
			data->timer++;
		}
		else {
			data->field_0x34 = data->end - float_40_80421b6c;
			data->timer = 60;
			data->state = 10;
		}
		break;
	case 5:
		if (data->timer < 10) {
			data->timer++;
		}
		else if (data->timer < 20) {
			data->field_0x34 += float_4_80421b84;
			data->timer++;
		}
		else {
			data->field_0x34 = data->end + float_40_80421b6c;
			data->timer = 60;
			data->state = 10;
		}
		break;
	case 10:
		if (data->timer != 0) {
			data->timer--;
		}
		else if (animGroupBaseAsync("EFF_naniga", inBattle, 0) != 0) {
			data->poseId = animPoseEntry("EFF_naniga", inBattle);
			animPoseSetAnim(data->poseId, "A_1", 1);
			data->state++;
			event = evtEntryType(sound_evt, 0, 0, 0);
			event->lwData[0] = (s32)(float_1024_80421b8c * vec.x) + 0xF24A7A80;
			event->lwData[1] = (s32)(float_1024_80421b8c * vec.y) + 0xF24A7A80;
			event->lwData[2] = (s32)(float_1024_80421b8c * vec.z) + 0xF24A7A80;
		}
		break;
	case 11:
		if (animPoseGetLoopTimes(data->poseId) > float_0p8_80421b90) {
			effItemGetEntry(10, data->posX, data->posY, data->posZ);
			data->state++;
		}
		break;
	case 12:
		data->color -= 4;
		if (data->color < 0) {
			animPoseRelease(data->poseId);
			data->poseId = -1;
			data->timer = 30;
			data->state++;
		}
		break;
	case 13:
		if (data->timer != 0) {
			data->timer--;
		}
		else {
			effDelete(effect);
			return;
		}
		break;
	}

	while (data->field_0x34 < float_0_80421b60) {
		data->field_0x34 += float_40_80421b6c * (f32)data->count;
	}
	while (data->field_0x34 >= float_40_80421b6c * (f32)(data->count + 2)) {
		data->field_0x34 -= float_40_80421b6c * (f32)data->count;
	}

	switch (data->state) {
	case 2:
	case 3:
	case 4:
	case 5:
	case 10:
		dispEntry(CAMERA_2D, dispCalcZ(&vec), 1, effNanigaDisp, effect);
		break;
	case 0:
	case 1:
		break;
	default:
		if (data->state != 11 || !(animPoseGetLoopTimes(data->poseId) < float_0p8_80421b90)) {
			dispEntry(CAMERA_3D, dispCalcZ(&vec), 1, effNanigaDisp3, effect);
		}
		break;
	}
	if (data->poseId != -1) {
		dispEntry(CAMERA_3D, dispCalcZ(&vec), 2, effNanigaDisp2, effect);
	}
}

// NONMATCHING (94.92%): remaining diffs are .rodata layout (the zero-Vec/double/string pool offsets)
// and register allocation/scheduling around the item-count scan.
EffectEntry* effNanigaEntry(s32 type, s32* itemList, s32 itemId, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	effNanigaData* data;
	BOOL inBattle;
	Vec vec;
	s32 index;
	s32 i;
	s32 randResult;

	entry = effEntry();
	entry->type = "Naniga";
	entry->count = 1;
	inBattle = gp->inBattle != 0;
	data = __memAlloc(HEAP_EFFECT, sizeof(effNanigaData));
	entry->userdata = data;
	entry->callback = effNanigaMain;
	entry->flags |= 2;
	data->type = type;
	data->posX = x;
	data->posY = float_20_80421b5c + y;
	data->posZ = z;
	data->scale = float_1_80421b74;
	data->state = 0;
	data->timer = 0;
	data->itemList = itemList;
	data->count = 0;
	while (itemList[data->count] != 0) {
		data->count++;
	}
	data->itemId = itemId;
	data->field_0x34 = float_0_80421b60;
	data->start = float_0_80421b60;
	index = 0;
	for (i = 0; i < data->count; i++) {
		if (data->itemList[i] == itemId) {
			break;
		}
		index++;
	}
	data->end = float_40_80421b6c * (f32)index + float_3_80421b94 * (float_40_80421b6c * (f32)data->count);
	randResult = rand() % 3;
	if (randResult == 0) {
		data->mode = 1;
		data->end += float_40_80421b6c;
	}
	else if (randResult == 1) {
		data->mode = 2;
		data->end -= float_40_80421b6c;
	}
	else {
		data->mode = 0;
	}
	animGroupBaseAsync("EFF_naniga", inBattle, 0);
	data->poseId = -1;
	data->color = 0xFF;
	vec = initializeVec((Vec*)&data->posX);
	*(Vec*)&data->field_0x10 = vec;
	PSMTXMultVec(camGetPtr(CAMERA_3D)->view, (Vec*)&data->field_0x10, (Vec*)&data->field_0x10);
	PSMTX44MultVec(camGetPtr(CAMERA_3D)->projection, (Vec*)&data->field_0x10, (Vec*)&data->field_0x10);
	data->field_0x10 *= gp->fbWidth * float_0p5_80421b70;
	data->field_0x14 *= gp->efbHeight * float_0p5_80421b70;
	return entry;
}
