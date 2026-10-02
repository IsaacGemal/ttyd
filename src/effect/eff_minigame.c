#include "eff/eff_minigame.h"
#include "driver/dispdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include "pmario_sound.h"
#include <dolphin/mtx.h>
#include <dolphin/gx.h>

// .sdata
u8 start_tbl[16] = "hifgdez{}~jkwx}~";
u8 finish_tbl[12] = "norsuvrs|{pq";
u8 retire_tbl[12] = {
	0x79, 0x78, 0x6C, 0x6D, 0x7F, 0x7E, 0x74, 0x73, 0x79, 0x78, 0x6C, 0x6D,
};
u8 start_type[8] = {
	0x00, 0x00, 0x00, 0x14, 0x14, 0x14, 0x14, 0x14,
};
u8 finish_type[8] = { 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x0A, 0x00, 0x00 };
u8 retire_type[8] = { 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x00, 0x00 };
u8* type_tbl[3] = { start_type, finish_type, retire_type };
u8 poyon_scale[40] = {
	0x64, 0x64, 0x69, 0x5E, 0x6E, 0x59, 0x75, 0x52,
	0x7B, 0x4C, 0x81, 0x46, 0x86, 0x41, 0x8A, 0x3D,
	0x8C, 0x3B, 0x8C, 0x3C, 0x85, 0x42, 0x77, 0x50,
	0x67, 0x60, 0x58, 0x6F, 0x50, 0x78, 0x4E, 0x79,
	0x52, 0x75, 0x58, 0x6F, 0x5E, 0x69, 0x64, 0x64
};

GXColor unk_80423308;
const GXColor dat_804212b0 = { 0xFF, 0xFF, 0xFF, 0x00 };

EffectEntry* effMiniGameEntry(s32 type, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	effMiniGameWork* data;
	s32 count;
	s32 i;

	entry = effEntry();
	count = 1;
	switch (type) {
	case 0:
		count = 9;
		break;
	case 1:
		count = 7;
		break;
	case 2:
		count = 7;
		break;
	}
	entry->type = "miniGame";
	entry->count = count;
	data = __memAlloc(HEAP_EFFECT, sizeof(effMiniGameWork) * entry->count);
	entry->userdata = data;
	entry->callback = effMiniGameMain;
	entry->flags |= 2;
	data->type = type;
	data->field_0x04 = x;
	data->field_0x08 = y;
	data->field_0x0C = z;
	data->field_0x14 = 1.0f;
	data->field_0x10 = 1.0f;
	data->field_0x1C = 0.0f;
	data->field_0x28 = 0;
	data->field_0x2C = 0;
	data++;
	for (i = 1; i < entry->count; i++, data++) {
		data->field_0x30 = type_tbl[type][i - 1];
		data->field_0x1C = 0.0f;
		data->field_0x34 = 0xFF;
		switch (data->field_0x30) {
		case 0:
			data->field_0x04 = 0.0f;
			data->field_0x28 = (i - 1) * gp->framerate;
			data->field_0x08 = 0.0f;
			data->field_0x0C = 0.0f;
			data->field_0x14 = 0.0f;
			data->field_0x10 = 0.0f;
			data->field_0x18 = 0.0f;
			data->field_0x2C = 0;
			data->field_0x34 = 0;
			break;
		case 0x1E:
		case 0x28:
// Yoda comparison: the natural `data->field_0x30 == 0x28` makes mwcc reload the field before the
// test; the original keeps the switch value in r3. The count terms below are written as
// `* 26.0f / 2.0f` rather than `* 26.0f * 0.5f`, which keeps the product as the first fmuls operand
// (same idiom as effMiniGameMain).
#ifdef MUST_MATCH
			if (0x28 == data->field_0x30) {
#else
			if (data->field_0x30 == 0x28) {
#endif
				data->field_0x04 =
					(f32)(i - 1) * 26.0f - (f32)(entry->count - 1) * 26.0f / 2.0f;
				data->field_0x28 = (i - 1) * 10;
				data->field_0x30 = 50;
			} else {
				data->field_0x04 = 0.0f;
				data->field_0x28 = (i - 1) * gp->framerate;
			}
			data->field_0x08 = 500.0f;
			data->field_0x0C = 0.0f;
			data->field_0x14 = 1.0f;
			data->field_0x10 = 1.0f;
			data->field_0x18 = 0.0f;
			data->field_0x2C = 0;
			data->field_0x20 = 0.0f;
			data->field_0x24 = 1.0f;
			break;
		case 0x0A:
		case 0x14:
			data->field_0x04 = 1000.0f;
			data->field_0x08 = 0.0f;
			data->field_0x0C = 0.0f;
			data->field_0x14 = 1.0f;
			data->field_0x10 = 1.0f;
			data->field_0x18 = 0.0f;
			if (data->field_0x30 == 0x0A) {
				data->field_0x28 = (i - 1) * 10;
			} else {
				data->field_0x28 = gp->framerate * 3;
				data->field_0x14 = 0.0f;
				data->field_0x10 = 0.0f;
				data->field_0x04 =
					(f32)(i - 3) * 26.0f - (f32)(entry->count - 3) * 26.0f / 2.0f;
				data->field_0x34 = 0;
			}
			data->field_0x2C = 0;
			data->field_0x20 = 0.0f;
			data->field_0x24 = 1.0f;
			break;
		}
	}
	return entry;
}
static inline Vec initializeMiniGameVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

// The affine-scale updates are written as division by 4.0f / 2.0f rather than * 0.25f / * 0.5f:
// the compiler folds both to the same multiply, but the division form keeps the product as the
// first fmadd/fmuls operand, which is what the original does.
void effMiniGameMain(EffectEntry* effect) {
	effMiniGameWork* data = effect->userdata;
	Vec disp = initializeMiniGameVec((Vec*)&data->field_0x04);
	s32 i;
	s32 retired = 0;

	data++;
	for (i = 1; i < effect->count; i++, data++) {
		switch (data->field_0x30) {
		case 0:
			data->field_0x28--;
			if (data->field_0x28 < 0) {
				data->field_0x28 = 0;
				data->field_0x30++;
				psndSFXOn((const char*)0x8D1);
			}
			break;
		case 1:
			data->field_0x10 += (1.0f - data->field_0x10) / 4.0f;
			data->field_0x14 += (1.0f - data->field_0x14) / 4.0f;
			data->field_0x34 += (s32)((255 - data->field_0x34) / 4.0f);
			if (data->field_0x34 > 0xFA) {
				data->field_0x34 = 0xFF;
				data->field_0x30++;
			}
			break;
		case 2:
			data->field_0x28 = (s32)(gp->framerate * 0.7f);
			data->field_0x30++;
			break;
		case 3:
			data->field_0x28--;
			if (data->field_0x28 < 0) {
				data->field_0x28 = 0;
				data->field_0x30++;
			}
			break;
		case 4:
			data->field_0x10 += (10.0f - data->field_0x10) / 4.0f;
			data->field_0x14 += (10.0f - data->field_0x14) / 4.0f;
			data->field_0x34 += (s32)((f32)(-data->field_0x34) / 4.0f);
			if (data->field_0x34 < 5) {
				data->field_0x34 = 0;
				data->field_0x30++;
			}
			break;
		case 5:
			retired++;
			break;
		case 10:
			data->field_0x28--;
			if (data->field_0x28 < 0) {
				data->field_0x28 = 0;
				data->field_0x30++;
			}
			break;
		case 11:
			data->field_0x04 += data->field_0x20;
			data->field_0x20 -= data->field_0x24;
			if (data->field_0x04 <
				(f32)(i - 1) * 26.0f - (f32)(effect->count - 1) * 26.0f / 2.0f) {
				data->field_0x04 =
					(f32)(i - 1) * 26.0f - (f32)(effect->count - 1) * 26.0f / 2.0f;
				data->field_0x28 = 0;
				data->field_0x30++;
			}
			break;
		case 12:
			if ((u32)data->field_0x28 < 0x14) {
				data->field_0x10 = (f32)poyon_scale[data->field_0x28 * 2 + 1] / 100.0f;
				data->field_0x14 = (f32)poyon_scale[data->field_0x28 * 2] / 100.0f;
				data->field_0x28++;
			} else {
				data->field_0x24 = 2.0f;
				data->field_0x20 = 0.0f;
				data->field_0x28 = 0x78;
				data->field_0x30++;
			}
			break;
		case 13:
			data->field_0x28--;
			if (data->field_0x28 < 0) {
				data->field_0x28 = 0;
				data->field_0x30++;
			}
			break;
		case 14:
			data->field_0x04 += data->field_0x20;
			data->field_0x20 -= data->field_0x24;
			if (data->field_0x04 < -1000.0f) {
				data->field_0x30++;
			}
			break;
		case 15:
			retired++;
			break;
		case 20:
			if (data->field_0x28 == 0) {
				psndSFXOn((const char*)0x8D2);
			}
			data->field_0x28--;
			if (data->field_0x28 < 0) {
				data->field_0x28 = 0;
				data->field_0x30++;
			}
			break;
		case 21:
			data->field_0x10 += (1.0f - data->field_0x10) / 4.0f;
			data->field_0x14 += (1.0f - data->field_0x14) / 4.0f;
			data->field_0x34 += (s32)((255 - data->field_0x34) / 4.0f);
			if (data->field_0x34 > 0xFA) {
				data->field_0x34 = 0xFF;
				data->field_0x30++;
			}
			break;
		case 22:
			data->field_0x28 = 0x3C;
			data->field_0x30++;
			break;
		case 23:
			data->field_0x28--;
			if (data->field_0x28 < 0) {
				data->field_0x28 = 0;
				data->field_0x30++;
			}
			break;
		case 24:
			data->field_0x04 += data->field_0x20;
			data->field_0x20 -= data->field_0x24;
			if (data->field_0x04 < -1000.0f) {
				data->field_0x30++;
			}
			break;
		case 25:
			retired++;
			break;
		case 30:
			data->field_0x28--;
			if (data->field_0x28 < 0) {
				data->field_0x28 = 0;
				data->field_0x30++;
			}
			break;
		case 31:
			data->field_0x08 += data->field_0x20;
			data->field_0x20 -= data->field_0x24;
			if (data->field_0x08 < 0.0f) {
				data->field_0x08 = 0.0f;
				data->field_0x28 = 0;
				data->field_0x30++;
			}
			break;
		case 32:
			if ((u32)data->field_0x28 < 0x14) {
				data->field_0x10 = (f32)poyon_scale[data->field_0x28 * 2] / 100.0f;
				data->field_0x14 = (f32)poyon_scale[data->field_0x28 * 2 + 1] / 100.0f;
				data->field_0x28++;
			} else {
				data->field_0x24 = 1.0f;
				data->field_0x20 = 0.0f;
				data->field_0x28 = (s32)(gp->framerate * 0.7f);
				data->field_0x30++;
			}
			break;
		case 33:
			data->field_0x28--;
			if (data->field_0x28 < 0) {
				data->field_0x28 = 0;
				data->field_0x30++;
			}
			break;
		case 34:
			data->field_0x04 -= 10.0f;
			data->field_0x08 += 10.0f;
			data->field_0x1C += 30.0f;
			if (data->field_0x08 > 300.0f) {
				data->field_0x30++;
			}
			break;
		case 35:
			retired++;
			break;
		case 50:
			data->field_0x28--;
			if (data->field_0x28 < 0) {
				data->field_0x28 = 0;
				data->field_0x30++;
			}
			break;
		case 51:
			data->field_0x08 += data->field_0x20;
			data->field_0x20 -= data->field_0x24;
			if (data->field_0x08 < 0.0f) {
				data->field_0x08 = 0.0f;
				data->field_0x28 = 0;
				data->field_0x30++;
			}
			break;
		case 52:
			if ((u32)data->field_0x28 < 0x14) {
				data->field_0x10 = (f32)poyon_scale[data->field_0x28 * 2] / 100.0f;
				data->field_0x14 = (f32)poyon_scale[data->field_0x28 * 2 + 1] / 100.0f;
				data->field_0x28++;
			} else {
				data->field_0x24 = 1.0f;
				data->field_0x20 = 0.0f;
				data->field_0x28 = (s32)(gp->framerate * 0.7f);
				data->field_0x30++;
			}
			break;
		case 53:
			data->field_0x28--;
			if (data->field_0x28 < 0) {
				data->field_0x28 = 0;
				data->field_0x30++;
			}
			break;
		case 54:
			data->field_0x08 += data->field_0x20;
			data->field_0x20 -= data->field_0x24;
			data->field_0x18 += 3.0f;
			if (data->field_0x08 < -500.0f) {
				data->field_0x30++;
			}
			break;
		case 55:
			retired++;
			break;
		}
	}
	if (retired >= effect->count - 1) {
		effDelete(effect);
	} else if (effect->flags & 4) {
		effect->flags &= ~4;
		effDelete(effect);
	} else {
		dispEntry(CAMERA_2D, dispCalcZ(&disp), 2, effMiniGameDisp, effect);
	}
}

#ifdef MUST_MATCH
// Match-only: returning the colour from an inline instead of building it in the caller puts the
// by-value temporary and its outgoing copy in the original's stack slots. The start helper takes
// the base colour as its second parameter so the division operands stay in r0/r6 and the temporary
// lands at 0x10; the finish helper builds its own local at 0x8.
static inline GXColor makeMiniGameStartColor(u8 alpha, GXColor color) {
	color.a = alpha;
	return color;
}
#endif

#ifdef MUST_MATCH
static inline GXColor makeMiniGameFinishColor(u8 alpha) {
	GXColor color = *(GXColor*)&dat_804212b0;
	color.a = alpha;
	return color;
}
#endif

// Matched with the MUST_MATCH colour helpers above, by reloading `effect->userdata` into the
// outer-loop entry pointer and holding the three texture tables as base pointers, and by keeping
// `f29 = -f30` inside the inner loop.
void effMiniGameDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effMiniGameWork* data = effect->userdata;
	CameraEntry* cam;
	Mtx mtx;
	Mtx tmp;
	Mtx rot;
	Mtx base;
	GXTexObj tex;
	f32 f27;
	f32 f28;
	f32 f29;
	f32 f30;
	effMiniGameWork* entry;
	s32 j;
	s32 i;
	s32 type;
	u32 texId;
	s32 tblOff;
	u8* startPtr;
	u8* finishPtr;
	u8* retirePtr;

	cam = camGetPtr(camId);
	type = data->type;
	GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
	PSMTXTrans(mtx, data->field_0x04, data->field_0x08, data->field_0x0C);
	PSMTXRotRad(rot, 'y', MTXDegToRad(-camGetPtr(camId)->viewYaw));
	PSMTXScale(tmp, data->field_0x10, data->field_0x14, 1.0f);
	PSMTXConcat(mtx, rot, base);
	PSMTXConcat(base, tmp, base);
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
	GXSetVtxDesc(13, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 13, 1, 4, 0);
	f30 = 40.0f;
	for (i = 0; i < 2; i++) {
		entry = (effMiniGameWork*)effect->userdata;
		entry++;
		tblOff = 1 - i;
		startPtr = start_tbl + tblOff;
		finishPtr = finish_tbl + tblOff;
		retirePtr = retire_tbl + tblOff;

		for (j = 1; j < effect->count; j++, entry++) {
			f32 v2;
			f32 v3;
			f32 v4;

			PSMTXTrans(mtx, 0.0f, 20.0f, 0.0f);
			PSMTXRotRad(rot, 'z', MTXDegToRad(entry->field_0x1C));
			PSMTXConcat(mtx, rot, rot);
			PSMTXTrans(mtx, 0.0f, -20.0f, 0.0f);
			PSMTXConcat(rot, mtx, rot);
			PSMTXTrans(mtx, entry->field_0x04, entry->field_0x08, entry->field_0x0C);
			PSMTXConcat(mtx, rot, mtx);
			PSMTXScale(tmp, entry->field_0x10, entry->field_0x14, 1.0f);
			PSMTXRotRad(rot, 'z', MTXDegToRad(entry->field_0x18));
			PSMTXConcat(mtx, rot, mtx);
			PSMTXConcat(mtx, tmp, mtx);
			PSMTXConcat(base, mtx, mtx);
			if (i == 0) {
				PSMTXTrans(tmp, 3.0f, -3.0f, 0.0f);
				PSMTXConcat(mtx, tmp, mtx);
			}
			PSMTXConcat(cam->view, mtx, mtx);
			GXLoadPosMtxImm(mtx, 0);
			GXSetCurrentMtx(0);
			switch (type) {
			case 0:
				texId = startPtr[2 * (j - 1)];
				break;
			case 1:
				texId = finishPtr[2 * (j - 1)];
				break;
			case 2:
				texId = retirePtr[2 * (j - 1)];
				break;
			}
			effGetTexObj(texId, &tex);
			if (i != 0 && entry->field_0x10 <= 1.0f) {
				GXInitTexObjLOD(&tex, 0, 0, 0.0f, 0.0f, 0.0f, 0, 0, 0);
			}
			GXLoadTexObj(&tex, 0);
#ifdef MUST_MATCH
			if (i == 0) {
				GXSetChanMatColor(4, makeMiniGameStartColor(entry->field_0x34 * 140 / 255, unk_80423308));
			} else {
				GXSetChanMatColor(4, makeMiniGameFinishColor(entry->field_0x34 * 255 / 255));
			}
#else
			if (i == 0) {
				GXColor color = unk_80423308;
				color.a = entry->field_0x34 * 140 / 255;
				GXSetChanMatColor(4, color);
			} else {
				GXColor color = dat_804212b0;
				color.a = entry->field_0x34 * 255 / 255;
				GXSetChanMatColor(4, color);
			}
#endif
			GXBegin(0x80, 0, 4);
			f27 = 0.5f;
			f28 = 20.0f;
			v2 = 0.0f;
			v4 = v2;
			f29 = -f30;
			if (type == 0) {
				v4 = f29 * f27;
			} else if (type == 1) {
				v2 = 20.0f;
				v4 = f29 * f27;
			}
			v3 = f29 * f27 + v2;
			v2 = f28 + v2;
			GXPosition3f32(v3, f30 + v4, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(v2, f30 + v4, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);
			GXPosition3f32(v2, v4, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);
			GXPosition3f32(v3, v4, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
		}
	}
}

