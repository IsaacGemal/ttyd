#include "eff/eff_levelup.h"
#include "driver/dispdrv.h"
#include "evt/evt_cam.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "mario/mariost.h"
#include "memory.h"
#include "pmario_sound.h"
#include "system.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

const s32 dat_802fe5a0[10] = {
	0x00000000, 0x00000040, 0x000000BC, 0x00000104, 0x00000088,
	0x00000000, 0x00000058, 0x000000D2, 0x00000100, 0x00000086
};
const Vec vec3_802fe5c8 = { 0.0f, 0.0f, 0.0f };
const s32 dat_802fe5d4[8] = {
	0x00000000, 0x00000080, 0x00000078, 0x00000018,
	0x00000000, 0x000000B0, 0x00000044, 0x00000018
};
const f64 double_to_int_802fe5f8 = 4503601774854144.0;
const f64 double_to_int_mask_802fe600 = 4503599627370496.0;
const char str_802FE608[] = "Levelup";

u16 y_trans[46] = {
	0x01F4, 0x01F4, 0x01DF, 0x01CA, 0x01B5, 0x01A0, 0x018B, 0x0177, 0x0162, 0x014D, 0x0138, 0x0123,
	0x010E, 0x00FA, 0x00E5, 0x00D0, 0x00BB, 0x00A6, 0x0091, 0x007D, 0x0068, 0x0053, 0x003E, 0x0029,
	0x0014, 0x0000, 0x0005, 0x000A, 0x000F, 0x0014, 0x0019, 0x0014, 0x000F, 0x000A, 0x0004, 0x0000,
	0x0005, 0x000A, 0x000F, 0x0014, 0x0019, 0x0014, 0x000F, 0x000A, 0x0004, 0x0000
};

static u32 texid_tbl[2][6] = {
	{ 0x00000092, 0x00000093, 0x00000094, 0x00000095, 0x00000096, 0x00000097 },
	{ 0x00000092, 0x00000093, 0x00000094, 0x00000095, 0x00000096, 0x00000097 }
};
u8 y_scale[20] = {
	0x00, 0x0D, 0x28, 0x45, 0x62, 0x7E, 0x95, 0xA6, 0xAF, 0xA5,
	0x88, 0x64, 0x43, 0x32, 0x49, 0x75, 0x82, 0x71, 0x64, 0x00,
};
static s32 evt_shake[8] = {
	EVT_CMD(5, OPCODE_USER_FUNC), (s32)evt_cam_shake, 4, FLOAT(0.001953125f), FLOAT(0.001953125f), 0x32,
	EVT_CMD(0, OPCODE_END_EVENT), OPCODE_END_SCRIPT
};

const GXColor dat_80421080 = { 0x00, 0x00, 0x00, 0x80 };
const GXColor dat_80421084 = { 0xFF, 0xFF, 0xFF, 0xFF };

inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

EffectEntry* effLevelupEntry(s32 type, s32 count, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	effLevelupData* data;
	effLevelupData* elem;
	s32 i;

	entry = effEntry();
	entry->type = str_802FE608;
	entry->count = 4;
	data = __memAlloc(HEAP_EFFECT, sizeof(effLevelupData) * entry->count);
	entry->userdata = data;
	entry->callback = effLevelupMain;

	data->type = type;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->timer = count;
	data->state = 0;

	elem = data + 1;
	for (i = 1; i < entry->count; i++, elem++) {
		s32 tbl[2][5] = {
			{ 0x00000000, 0x00000040, 0x000000BC, 0x00000104, 0x00000088 },
			{ 0x00000000, 0x00000058, 0x000000D2, 0x00000100, 0x00000086 }
		};

		elem->position.x = (f32)(tbl[(!gp->language) ? 0 : 1][i] - tbl[(!gp->language) ? 0 : 1][4]);
		elem->position.y = (f32)y_trans[0];
		elem->position.z = 0.0f;
		elem->xOffset = 0.0f;
		elem->state = 0;
		elem->scaleX = 1.0f;
		elem->scaleY = (i < 3) ? 1.0f : 0.0f;
	}
	return entry;
}

void effLevelupMain(EffectEntry* effect) {
	effLevelupData* data = effect->userdata;
	Vec disp = initializeVec(&data->position);
	s32 max = data->timer;
	s32 i;
	s32 numDone = 0;

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		switch (data->state) {
		case 0:
			data->timer++;
			if ((u32)data->timer < 0x2E) {
				data->position.y = (f32)y_trans[data->timer];
				if (i == 1 && data->position.y == 0.0f) {
					evtEntry(evt_shake, 0, 0);
					psndSFXOn((const char*)0xA57);
				}
			} else {
				data->state++;
				data->timer = 0;
			}
			break;
		case 1:
			data->timer++;
			if ((u32)data->timer < 0x13) {
				if (i > 2) {
					data->scaleY = (f32)y_scale[data->timer] / 100.0f;
				}
			} else {
				data->state++;
				data->timer = 0;
			}
			break;
		case 2: {
			s32 t = data->timer;

			data->timer = t + 1;
			if (t > max) {
				data->timer = 0;
				data->state++;
			}
			break;
		}
		case 3: {
			if (i < 2) {
				data->xOffset = intplGetValue(1, data->timer, 0x1E, 0.0f, 400.0f);
			} else {
				data->xOffset = intplGetValue(1, data->timer, 0x1E, 0.0f, -400.0f);
			}
		}
			data->timer++;
			if (data->timer > 0x1E) {
				data->state++;
				data->timer = 0;
			}
			break;
		case 4:
			numDone++;
			break;
		}
	}

	if (numDone >= effect->count - 1) {
		effDelete(effect);
	} else {
		dispEntry(CAMERA_2D, dispCalcZ(&disp), 2, effLevelupDisp, effect);
	}
}

// NONMATCHING (99.93%): the `elem; i; layer` declaration order plus a separate `layer` local puts effect/layer/i/xBaseI/elem in the target's r21/r22/r23/r25/r24. Splitting the element address into `elem = userdata; elem += i;` makes the compiler load userdata straight into elem's r24 instead of a scratch. What is left: the two inner-loop-latch decrements, `i--` (subi r23) and the strength-reduced `i*0x24` offset (subi r27), are scheduled in the opposite order (target runs i-- first, then the offset); every other instruction matches. The local 2x4 table's initializer is an anonymous @218 rodata pool standing in for the named dat_802fe5d4 (both 0x20 bytes, identical here).
void effLevelupDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effLevelupData* data = effect->userdata;
	CameraEntry* camView;
	Mtx mtxTrans;
	Mtx mtxRot;
	Mtx mtxScale;
	Mtx mtxView;
	GXTexObj texObj;
	effLevelupData* elem;
	s32 i;
	s32 layer;
	f32 halfConst = 0.5f;
	f32 yConst = 48.0f;

	camView = camGetPtr(camId);
	PSMTXTrans(mtxTrans, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(mtxRot, 'y', 0.017453292f * -camGetPtr(camId)->viewYaw);
	PSMTXConcat(mtxTrans, mtxRot, mtxTrans);
	PSMTXConcat(camView->view, mtxTrans, mtxView);

	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 4);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevOp(0, 0);
	GXSetCullMode(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xD, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);

	for (layer = 1; layer >= 0; layer--) {
		for (i = effect->count - 1; i >= 1; i--) {
			s32 xBaseI = layer * 4;
			s32 yBaseI = -layer * 4;
			f32 xBase = (f32)xBaseI;
			f32 yBase = (f32)yBaseI;
			f32 tableVal;
			f32 half;
			f32 neghalf;

			elem = (effLevelupData*)effect->userdata;
			elem += i;

			if (layer != 0) {
				effGetTexObj(texid_tbl[gp->language != 0][i + 2], &texObj);
				GXLoadTexObj(&texObj, 0);
				GXSetChanMatColor(4, dat_80421080);
			} else {
				effGetTexObj(texid_tbl[gp->language != 0][i - 1], &texObj);
				GXLoadTexObj(&texObj, 0);
				GXSetChanMatColor(4, dat_80421084);
			}

			PSMTXTrans(mtxTrans, xBase + (elem->position.x + elem->xOffset), elem->position.y + yBase, elem->position.z);
			PSMTXConcat(mtxView, mtxTrans, mtxTrans);
			PSMTXScale(mtxScale, elem->scaleX, elem->scaleY, 1.0f);
			PSMTXConcat(mtxTrans, mtxScale, mtxTrans);
			GXLoadPosMtxImm(mtxTrans, 0);
			GXSetCurrentMtx(0);
			GXBegin(0x80, 0, 4);

			{
				s32 tbl[2][4] = {
					{ 0x00000000, 0x00000080, 0x00000078, 0x00000018 },
					{ 0x00000000, 0x000000B0, 0x00000044, 0x00000018 }
				};
				tableVal = (f32)tbl[gp->language != 0][i];
			}
			half = tableVal * halfConst;
			neghalf = -tableVal * halfConst;
			GXPosition3f32(neghalf, yConst, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(half, yConst, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);
			GXPosition3f32(half, 0.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);
			GXPosition3f32(neghalf, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
		}
	}
}
