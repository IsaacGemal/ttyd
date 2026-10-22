// Ported with non-deferred codegen: retail's .data is sound_evt first, then the compiler's switch jump
// table, then the trailing zero gap.  With `-inline auto,deferred` (the flag this file is currently built
// with) MWCC emits the parsed .data before the jump table, so the gap lands at 0x803B2F4C instead of
// 0x803B2F84.  The functions are also in asm order (not the reverse) to match the non-deferred .text order.
// The durable fix is cflags_static_nodefer on this Object; `#pragma defer_codegen off` is the file-local
// equivalent while configure.py may not be changed beyond the Matching flip.
#pragma defer_codegen off
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

inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };

	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

#ifdef MUST_MATCH
// MUST_MATCH: a second copy of the zero-Vec inline so Main and Entry each pool their own
// .rodata zero Vec3 at the offsets the original has (target has (Vec){ 0.0f, 0.0f, 0.0f } and (Vec){ 0.0f, 0.0f, 0.0f }).
static inline Vec initializeVec2(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };

	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}
#endif

// Building the GXColor through a local and setting its alpha puts the colour temp in the slot retail uses.
static inline GXColor effNanigaMaterialColor(u8 alpha) {
	GXColor color = (GXColor){ 0xFF, 0xFF, 0xFF, 0x00 };
	color.a = alpha;
	return color;
}

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
	PSMTXTrans(trans, data->posX, data->posY - 16.0f, data->posZ);
	PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(camId)->viewYaw);
	PSMTXScale(scale, data->scale, data->scale, data->scale);
	PSMTXConcat(trans, rot, trans);
	PSMTXConcat(trans, scale, mtx);

	if (data->state < 4) {
		iconDispGxCol(mtx, 0, 0x184, (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
	}
	else {
		iconDispGxCol(mtx, 0, ICON_ID(item->icon_id), (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF });
	}
}

void effNanigaDisp2(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effNanigaData* data = effect->userdata;
	Mtx trans;
	Mtx rot;
	Mtx scale;
	Mtx mtx;
	GXColor color;

	camGetPtr(camId);
	PSMTXTrans(trans, data->posX, data->posY - 20.f, data->posZ);
	PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(camId)->viewYaw);
	PSMTXScale(scale, data->scale, data->scale, data->scale);
	PSMTXConcat(trans, rot, trans);
	PSMTXConcat(trans, scale, mtx);
	animPoseSetMaterialFlagOn(data->poseId, 0x40);
	color = effNanigaMaterialColor(data->color);
	animPoseSetMaterialEvtColor(data->poseId, color);
	animPoseMain(data->poseId);
	animPoseDrawMtx(data->poseId, mtx, 1, 0.0f, 10.f);
	animPoseDrawMtx(data->poseId, mtx, 2, 0.0f, 10.f);
	animPoseDrawMtx(data->poseId, mtx, 3, 0.0f, 10.f);
}

// The target loads the four single-precision literals (0.5/0.0/1.0/0.9) from the named .sdata2 objects, which
// are the compiler's own pooled literal entries; referencing them by name makes MWCC keep them in callee-saved
// FPRs (f26+), so the literals stay literal here and pool to anonymous @440-@443 slots at different offsets.
// The two compiler-generated int<->float magic doubles also sit 8 bytes later in .rodata, because the target's
// .rodata has two unreferenced zero Vec3 objects before them (a Main/Entry inlining difference). Those pool
// symbol names do not count against the score (objdiff resolves them by value).
// The size conversion's `fmuls` is fixed by holding data->scale in a local across the conversion call, which
// also makes MWCC emit the target's `lfs f28`/`lfs f0` order and the `fmuls f1, f0, f28` operand order.
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
	f32 half;
	f32 iconSize;
	f32 scaleBoost;
	u32 centerX;
	u32 centerY;
	u32 scissorSize;
	f32 extent;
	f32 quadHalf;
	f32 negQuadHalf;
	f32 savedScale;
	f32 sizeScale;
	s32 i;

	camera = camGetPtr(camId);
	savedScale = data->scale;
	scaleBoost = 1.5f;
	data->scale = savedScale * scaleBoost;
	PSMTXTrans(mtx, data->field_0x10.x, data->field_0x10.y, data->field_0x10.z);
	PSMTXRotRad(rot, 'y', 0.017453292f * -camGetPtr(camId)->viewYaw);
	PSMTXScale(scale, data->scale, data->scale, data->scale);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXConcat(mtx, scale, base);
	GXGetScissor(&scissorLeft, &scissorTop, &scissorWidth, &scissorHeight);
	sizeScale = data->scale;
	iconSize = 40.0f;
	scissorSize = (u32)(iconSize * sizeScale);
	extent = 20.f * sizeScale;
	half = 0.5f;
	centerY = (u32)((f32)gp->efbHeight * half - data->field_0x10.y);
	centerX = (u32)((f32)gp->fbWidth * half + data->field_0x10.x);
	GXSetScissor((u32)((f32)centerX - extent), (u32)((f32)centerY - extent), scissorSize, scissorSize);

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
	quadHalf = 20.f;
	negQuadHalf = -iconSize * half;
	GXPosition3f32(negQuadHalf, quadHalf, 0.0f);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3f32(quadHalf, quadHalf, 0.0f);
	GXTexCoord2f32(1.0f, 0.0f);
	GXPosition3f32(quadHalf, negQuadHalf, 0.0f);
	GXTexCoord2f32(1.0f, 1.0f);
	GXPosition3f32(negQuadHalf, negQuadHalf, 0.0f);
	GXTexCoord2f32(0.0f, 1.0f);

	for (i = 0; i < data->count + 5; i++) {
		iconGetTexObj(&tex, ICON_ID(itemDataTable[data->itemList[i % data->count]].icon_id));
		GXLoadTexObj(&tex, 0);
		PSMTXTrans(mtx, 0.0f, data->field_0x34 + -40.f * (f32)i, 0.0f);
		PSMTXScale(scale, 0.9f, 0.9f, 0.9f);
		PSMTXConcat(base, mtx, mtx);
		PSMTXConcat(mtx, scale, mtx);
		PSMTXConcat(camera->view, mtx, mtx);
		GXLoadPosMtxImm(mtx, 0);
		GXSetCurrentMtx(0);
		GXBegin(0x80, 0, 4);
		GXPosition3f32(negQuadHalf, quadHalf, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(quadHalf, quadHalf, 0.0f);
		GXTexCoord2f32(1.0f, 0.0f);
		GXPosition3f32(quadHalf, negQuadHalf, 0.0f);
		GXTexCoord2f32(1.0f, 1.0f);
		GXPosition3f32(negQuadHalf, negQuadHalf, 0.0f);
		GXTexCoord2f32(0.0f, 1.0f);
	}

	GXSetScissor(scissorLeft, scissorTop, scissorWidth, scissorHeight);
	data->scale = savedScale;
}

// `s32` (signed long), not `BOOL` (int), for `inBattle`: MWCC colors `gp`/`effect->flags` into r5/r4 and the
// first switch's state/jumptable base into r5/r4 the way the original does; with `int` they land one register
// higher.  The second while loop writes its constant as the literal `40.0f` (the same value as
// 40.f, which the first loop still names) so MWCC hoists that load before the shared int->float
// magic pair, matching the original's loop-setup order.
void effNanigaMain(EffectEntry* effect) {
	effNanigaData* data = effect->userdata;
	s32 inBattle;
	Vec vec;
	EventEntry* event;
	s32 i;
	f32 scale;
	s32 state;

#ifdef MUST_MATCH
	vec = initializeVec2((Vec*)&data->posX);
#else
	vec = initializeVec((Vec*)&data->posX);
#endif
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
		data->state++;
		data->timer = 30;
		// fallthrough
	case 1:
		if (data->timer == 30) {
			for (i = 0; i < 4; i++) {
				scale = 0.5f + (f32)irand(200) / 100.f;
				effKemuTestDrawCam(effKemuTestEntry(2, data->field_0x10.x, data->field_0x10.y, data->field_0x10.z, scale), 8);
			}
			psndSFXOn_3D("SFX_ITEM_WHAT1", &vec);
		}
		if (--data->timer < 0) {
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
			data->field_0x34 -= 4.0f;
			data->timer++;
		}
		else {
			data->field_0x34 = data->end - 40.f;
			data->timer = 60;
			data->state = 10;
		}
		break;
	case 5:
		if (data->timer < 10) {
			data->timer++;
		}
		else if (data->timer < 20) {
			data->field_0x34 += 4.0f;
			data->timer++;
		}
		else {
			// The addend is on the left so MWCC loads float_40 before data->end.
			data->field_0x34 = 40.f + data->end;
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
			event->lwData[0] = (s32)(1024.0f * vec.x) + 0xF24A7A80;
			event->lwData[1] = (s32)(1024.0f * vec.y) + 0xF24A7A80;
			event->lwData[2] = (s32)(1024.0f * vec.z) + 0xF24A7A80;
		}
		break;
	case 11:
		if (animPoseGetLoopTimes(data->poseId) > 0.8f) {
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

	while (data->field_0x34 < 0.0f) {
		data->field_0x34 += 40.f * (f32)data->count;
	}
	// Writing this loop's constant as the literal 40.0f (== 40.f) makes MWCC hoist
	// that load before the shared int->float magic pair, matching the original's loop-setup order.
	while (data->field_0x34 >= 40.0f * (f32)(data->count + 2)) {
		data->field_0x34 -= 40.0f * (f32)data->count;
	}

	// A local keeps the second switch's selector live into the default case (the original has no reload).
	state = data->state;
	switch (state) {
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
		if (state != 11 || !(animPoseGetLoopTimes(data->poseId) < 0.8f)) {
			dispEntry(CAMERA_3D, dispCalcZ(&vec), 1, effNanigaDisp3, effect);
		}
		break;
	}
	if (data->poseId != -1) {
		dispEntry(CAMERA_3D, dispCalcZ(&vec), 2, effNanigaDisp2, effect);
	}
}

EffectEntry* effNanigaEntry(s32 type, s32* itemList, s32 itemId, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	effNanigaData* data;
	s32 inBattle;
	Vec vec;
	s32 index;
	s32 i;
	s32 randResult;
	f32 half;
	s32* ptr;

	entry = effEntry();
	inBattle = gp->inBattle != 0;
	entry->type = "Naniga";
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effNanigaData));
	entry->userdata = data;
	entry->callback = effNanigaMain;
	entry->flags |= 2;
	data->type = type;
	data->posX = x;
	data->posY = 20.f + y;
	data->posZ = z;
	data->scale = 1.0f;
	data->state = 0;
	data->timer = 0;
	data->itemList = itemList;
	data->count = 0;
	while (itemList[data->count] != 0) {
		data->count++;
	}
	data->itemId = itemId;
	ptr = itemList;
	data->start = 0.0f;
	data->field_0x34 = 0.0f;
	index = 0;
	for (i = data->count; i > 0; i--) {
		if (itemId == *ptr) {
			break;
		}
		ptr++;
		index++;
	}
	data->end = 40.f * (f32)index + 3.0f * (40.f * (f32)data->count);
	randResult = rand() % 3;
	if (randResult == 0) {
		data->mode = 1;
		data->end += 40.f;
	}
	else if (randResult == 1) {
		data->mode = 2;
		data->end -= 40.f;
	}
	else {
		data->mode = 0;
	}
	animGroupBaseAsync("EFF_naniga", inBattle, 0);
	data->poseId = -1;
	data->color = 0xFF;
	vec = initializeVec((Vec*)&data->posX);
	data->field_0x10 = vec;
	PSMTXMultVec(camGetPtr(CAMERA_3D)->view, &data->field_0x10, &data->field_0x10);
	PSMTX44MultVec(camGetPtr(CAMERA_3D)->projection, &data->field_0x10, &data->field_0x10);
	half = 0.5f;
	data->field_0x10.x *= gp->fbWidth * half;
	data->field_0x10.y *= gp->efbHeight * half;
	return entry;
}

// 28-byte zero tail at 0x803B2F84: retail has it after the switch jump table (which this non-deferred
// build now also emits here), and the linker does not invent it from the next unit's alignment, so it has
// to be an explicit `.data` object.  force_active keeps the unreferenced all-zero object.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".data") u32 gap_05_803B2F84_data[7] = { 0 };
#pragma force_active reset
#pragma explicit_zero_data reset
