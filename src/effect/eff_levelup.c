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

static u32 texid_tbl[12] = {
	0x00000092, 0x00000093, 0x00000094, 0x00000095, 0x00000096, 0x00000097,
	0x00000092, 0x00000093, 0x00000094, 0x00000095, 0x00000096, 0x00000097
};
u8 y_scale[20] = {
	0x0D, 0x28, 0x45, 0x62, 0x7E, 0x95, 0xA6, 0xAF, 0xA5, 0x88,
	0x64, 0x43, 0x32, 0x49, 0x75, 0x82, 0x71, 0x64, 0x00, 0x00
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

// NONMATCHING (70.22%): target copies the 10-word `dat_802fe5a0` table to the stack each iteration and indexes that; we index the global. Frame 0x60 vs 0xa0 (stmw r22) and the `gp->language != 0` bool is neg/or/srwi vs a double cntlzw.
EffectEntry* effLevelupEntry(s32 type, s32 count, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	effLevelupData* data;
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

	for (i = 1; i < entry->count; i++) {
		effLevelupData* elem = &data[i];

		elem->position.x = (f32)(dat_802fe5a0[((gp->language != 0) * 5) + i] - dat_802fe5a0[((gp->language != 0) * 5) + 4]);
		elem->position.y = (f32)y_trans[0];
		elem->position.z = 0.0f;
		elem->xOffset = 0.0f;
		elem->state = 0;
		elem->scaleX = 1.0f;
		elem->scaleY = (i < 3) ? 1.0f : 0.0f;
	}
	return entry;
}

// NONMATCHING (95.28%): control flow/states/values match; only the zero Vec (@155 vs vec3_802fe5c8), the u16->f32 mask (@313 vs double_to_int_mask_802fe600), and the `lwz r4,0x14(r31)` scheduling in the two intplGetValue calls differ.
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
		case 3:
			if (i < 2) {
				data->xOffset = intplGetValue(1, data->timer, 0x1E, 0.0f, 400.0f);
			} else {
				data->xOffset = intplGetValue(1, data->timer, 0x1E, 0.0f, -400.0f);
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

// NONMATCHING (78.24%): target copies the 8-word `dat_802fe5d4` width table to the stack each inner iteration; we index the global. Callee-saved GPR block starts at r23 vs r19 (frame 0x180 vs 0x1b0); f27-f31 FPR block, GX state and quad geometry match.
void effLevelupDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effLevelupData* data = effect->userdata;
	CameraEntry* camView;
	CameraEntry* cam;
	Mtx mtxTrans;
	Mtx mtxView;
	Mtx mtxScale;
	Mtx mtxRot;
	GXTexObj texObj;
	GXColor color;
	s32 count;
	s32 layer;
	s32 i;
	f32 halfConst = 0.5f;
	f32 yConst = 48.0f;

	camView = camGetPtr(camId);
	PSMTXTrans(mtxTrans, data->position.x, data->position.y, data->position.z);
	cam = camGetPtr(camId);
	PSMTXRotRad(mtxRot, 'y', 0.017453292f * -cam->viewYaw);
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

	count = effect->count;
	for (layer = 1; layer >= 0; layer--) {
		f32 xBase = (f32)(layer * 4);
		f32 yBase = (f32)(-layer * 4);

		for (i = count - 1; i >= 1; i--) {
			effLevelupData* elem = &data[i];
			f32 tableVal;
			f32 half;
			f32 neghalf;
			s32 texIdx;

			if (layer != 0) {
				texIdx = ((gp->language != 0) ? 6 : 0) + i + 2;
			} else {
				texIdx = ((gp->language != 0) ? 6 : 0) + i - 1;
			}
			effGetTexObj(texid_tbl[texIdx], &texObj);
			GXLoadTexObj(&texObj, 0);
			if (layer != 0) {
				color = dat_80421080;
			} else {
				color = dat_80421084;
			}
			GXSetChanMatColor(4, color);

			PSMTXTrans(mtxTrans, xBase + (elem->position.x + elem->xOffset), yBase + elem->position.y, elem->position.z);
			PSMTXConcat(mtxView, mtxTrans, mtxTrans);
			PSMTXScale(mtxScale, elem->scaleX, elem->scaleY, 1.0f);
			PSMTXConcat(mtxTrans, mtxScale, mtxTrans);
			GXLoadPosMtxImm(mtxTrans, 0);
			GXSetCurrentMtx(0);
			GXBegin(0x80, 0, 4);

			tableVal = (f32)dat_802fe5d4[((gp->language != 0) ? 4 : 0) + i];
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
