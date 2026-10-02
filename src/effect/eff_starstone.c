#include "eff/eff_starstone.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "driver/hitdrv.h"
#include "driver/mapdrv.h"
#include "driver/shadowdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include "pmario_sound.h"
#include "math_ext.h"
#include "eff/n64/eff_snow_dust_n64.h"
#include "eff/n64/eff_stardust_n64.h"
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u8 lbl_803AC080[8] = { 0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00 };

extern u8 diamond_1_color0_tbl[];
extern u8 diamond_2_color0_tbl[];
extern u8 diamond_3_color0_tbl[];
extern u8 diamond_4_color0_tbl[];
extern u8 diamond_5_color0_tbl[];
extern u8 diamond_6_color0_tbl[];
extern u8 diamond_7_color0_tbl[];

const Vec vec3_802fdd50 = { 0.0f, 0.0f, 0.0f };
const char str_802FDD5C[] = "SFX_EVT_STARSTONE_SHINE1";
const char str_802FDD78[] = "SFX_EVT_STARSTONE_JUMP1";

Vec edge_color[7] = {
	{ 0.3310000002384186, 0.3149999976158142, 0.23199999332427979 },
	{ 0.1589999943971634, 0.26100000739097595, 0.16200000047683716 },
	{ 0.28700000047683716, 0.2409999966621399, 0.09300000220537186 },
	{ 0.32499998807907104, 0.13199999928474426, 0.14100000262260437 },
	{ 0.14499999582767487, 0.2029999941587448, 0.3059999942779541 },
	{ 0.31200000643730164, 0.18199999630451202, 0.11500000208616257 },
	{ 0.25600001215934753, 0.31299999356269836, 0.3440000116825104 },
};

u8 diamond_1_dl_0_size_tbl[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
};
u8 diamond_1_dl_1_size_tbl[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
};
u8 diamond_2_2_dl_0_size_tbl[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
};
u8 diamond_2_2_dl_1_size_tbl[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
};
u8 diamond_7_dl_0_size_tbl[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
};
u8 diamond_7_dl_1_size_tbl[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
};
static u8* color_tbl[] = {
	diamond_1_color0_tbl,
	diamond_2_color0_tbl,
	diamond_3_color0_tbl,
	diamond_4_color0_tbl,
	diamond_5_color0_tbl,
	diamond_6_color0_tbl,
	diamond_7_color0_tbl,
};
extern const u32 dat_80420b48;
extern const f32 float_1p1_80420b4c;
extern const f32 float_deg2rad_80420b50;
extern const f32 float_0_80420b54;
extern const f32 float_0p01_80420b58;
extern const f32 float_1_80420b5c;
extern const f32 float_0p4_80420b60;
extern const f32 float_10_80420b64;
extern const f32 float_neg1_80420b68;
extern const f32 float_neg0p5_80420b6c;
extern const f32 float_5_80420b70;
extern const f32 float_20_80420b74;
extern const f32 float_255_80420b78;
extern const f32 float_6p2832_80420b7c;
extern const f32 float_90_80420b80;
extern const f32 float_360_80420b84;
extern const f32 float_neg90_80420b88;
extern const f32 float_neg1000_80420b8c;
extern const f32 float_8_80420b90;
extern const f32 float_1p5_80420b94;
extern s32 unk_804232e0;
extern s32 unk_804232e4;
extern s32 unk_804232e8;
extern s32 unk_804232ec;

void effStarStoneDraw(CameraId camId, EffectEntry* entry) {
	effStarStoneData* data = entry->userdata;

	switch (data->field_0x0) {
	case 0:
	case 6:
		effStarStoneDisp_1(camId, entry);
		break;
	default:
		effStarStoneDisp_2(camId, entry);
		break;
	}
}

EffectEntry* effStarStoneEntry(s32 idx, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry;
	effStarStoneData* data;

	entry = effEntry();
	entry->type = "StarStone";
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effStarStoneData) * entry->count);
	entry->userdata = data;
	entry->callback = effStarStoneMain;
	entry->flags |= 2;

	data->field_0x0 = idx % 7;
	data->field_0x4 = 0;
	data->field_0x8 = x;
	data->field_0xC = y;
	data->field_0x10 = z;
	data->field_0x1C = float_0_80420b54;
	data->field_0x14 = float_1p5_80420b94 * scale;
	data->field_0x24 = float_0_80420b54;
	data->field_0x28 = float_0_80420b54;
	data->field_0x38 = 4;
	data->field_0x3C = 0;
	data->field_0x4C = float_0_80420b54;
	data->field_0x48 = float_0_80420b54;
	data->field_0x54 = float_0_80420b54;
	data->field_0x50 = float_0_80420b54;
	data->field_0x58 = float_90_80420b80;
	data->field_0x5C = 0xFF;
	data->field_0x5D = 0xFF;
	data->field_0x5E = 0xFF;
	data->field_0x5F = 0xFF;
	data->field_0x60 = 0;

	{
		GXColor color = { 0, 0, 0, 0xFF };
		color.r = float_255_80420b78 * edge_color[data->field_0x0].x;
		color.g = float_255_80420b78 * edge_color[data->field_0x0].y;
		color.b = float_255_80420b78 * edge_color[data->field_0x0].z;
		data->field_0x30 = *(u32*)&color;
	}

	if (idx >= 7) {
		data->field_0x4 |= 2;
	}
	if (!(data->field_0x4 & 2)) {
		u8* clr = color_tbl[data->field_0x0];
		data->field_0x34 = effSnowDustN64Entry(2, x, y - float_10_80420b64 * data->field_0x14, z, 4, 0,
		                                       float_20_80420b74 * data->field_0x14, float_20_80420b74 * data->field_0x14, float_1_80420b5c);
		*(s32*)((u8*)data->field_0x34->userdata + 0x4C) = clr[0];
		*(s32*)((u8*)data->field_0x34->userdata + 0x50) = clr[1];
		*(s32*)((u8*)data->field_0x34->userdata + 0x54) = clr[2];
	}
	((effStarStoneData*)entry->userdata)->field_0x4 |= 4;
	if (idx >= 7) {
		((effStarStoneData*)entry->userdata)->field_0x4 &= ~4;
	}
	if (gp->inBattle != 0) {
		((effStarStoneData*)entry->userdata)->field_0x4 &= ~4;
	}
	return entry;
}

// `opt_common_subs off` required: with common-subexpression merging on, the two u8->f32 color
// conversions and the field_0x24 update pick different FPRs than the original (the 2^52 mask and
// the 255.0f divisor land in f3/f4 instead of f4/f3, the update's temporaries rotate).
#pragma opt_common_subs off
void effStarStoneMain(EffectEntry* effect) {
	effStarStoneData* data = effect->userdata;
	Vec position;
	Vec tmp = vec3_802fdd50;
	s32 type;
	s32 camId;
	s32 flag;
	f32 hitX, hitY, hitZ, distance, hitNX, hitNY, hitNZ;

	tmp.x = data->field_0x8;
	tmp.y = data->field_0xC;
	tmp.z = data->field_0x10;
	position = tmp;
	type = data->field_0x0;
	camId = data->field_0x38;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		if (data->field_0x34 != NULL) {
			effSoftDelete(data->field_0x34);
		}
		if (data->field_0x4 & 8) {
			psndSFXOff(data->field_0x60);
		}
		effDelete(effect);
		return;
	}

	if (data->field_0x4 & 4) {
		if (!(data->field_0x4 & 8)) {
			data->field_0x4 |= 8;
			data->field_0x60 = psndSFXOn_3D(str_802FDD5C, &position);
		}
		if (data->field_0x4 & 8) {
			psndSFX_pos(data->field_0x60, &position);
		}
	} else {
		if (data->field_0x4 & 8) {
			data->field_0x4 &= ~8;
			psndSFXOff(data->field_0x60);
		}
	}

	switch (data->field_0x3C) {
	case 0:
		switch (type) {
		case 6:
			data->field_0x24 -= float_0p01_80420b58;
			if (data->field_0x24 <= float_0_80420b54) {
				data->field_0x24 += float_1_80420b5c;
			}
			break;
		default:
			data->field_0x24 += float_0p01_80420b58;
			if (data->field_0x24 >= float_1_80420b5c) {
				data->field_0x24 -= float_1_80420b5c;
			}
			break;
		}
		break;
	case 1:
		data->field_0x4 |= 1;
		data->field_0x44 = float_0_80420b54;
		data->field_0x40 = float_0p4_80420b60;
		data->field_0x3C++;
		break;
	case 2:
	case 3:
	case 4:
	case 5:
		flag = 0;
		if (data->field_0x44 < float_0_80420b54) {
			distance = float_10_80420b64 + __fabsf(data->field_0x44);
			if (hitCheckFilter(0, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ,
			                   data->field_0x8, data->field_0xC, data->field_0x10,
			                   float_0_80420b54, float_neg1_80420b68, float_0_80420b54) != NULL) {
				flag = 1;
				data->field_0xC = float_10_80420b64 + hitY;
				data->field_0x44 *= float_neg0p5_80420b6c;
				data->field_0x3C++;
				if (data->field_0x4 & 8) {
					data->field_0x4 &= ~8;
					psndSFXOff(data->field_0x60);
					((effStarStoneData*)effect->userdata)->field_0x4 &= ~4;
				}
				if (!(data->field_0x4 & 0x10)) {
					psndSFXOn_3D(str_802FDD78, &position);
				}
				if (!(data->field_0x4 & 2) && data->field_0x3C == 3) {
					f32 scale = data->field_0x14;
					EffectEntry* entry = effStardustN64Entry(4, data->field_0x8,
					                                          data->field_0xC + float_10_80420b64 * scale + float_5_80420b70 * scale,
					                                          data->field_0x10, float_20_80420b74 * scale);
					effStardustN64SetColor(entry, color_tbl[data->field_0x0][0] / float_255_80420b78,
					                       color_tbl[data->field_0x0][1] / float_255_80420b78,
					                       color_tbl[data->field_0x0][2] / float_255_80420b78);
				}
			}
		}
		if (flag == 0) {
			data->field_0xC += data->field_0x44;
			data->field_0x44 -= data->field_0x40;
		}
		if (data->field_0x3C > 2) {
			data->field_0x48 -= (f32)sin(float_6p2832_80420b7c * (float_90_80420b80 + camGetPtr(data->field_0x38)->viewYaw) / float_360_80420b84);
			data->field_0x4C -= (f32)cos(float_6p2832_80420b7c * (float_90_80420b80 + camGetPtr(data->field_0x38)->viewYaw) / float_360_80420b84);
			data->field_0x50 += (f32)sin(float_6p2832_80420b7c * (float_90_80420b80 + camGetPtr(data->field_0x38)->viewYaw) / float_360_80420b84);
			data->field_0x54 += (f32)cos(float_6p2832_80420b7c * (float_90_80420b80 + camGetPtr(data->field_0x38)->viewYaw) / float_360_80420b84);
			data->field_0x20 = data->field_0x20 + (float_90_80420b80 - data->field_0x20) / float_10_80420b64;
		}
		if (data->field_0x3C > 3) {
			data->field_0x18 = data->field_0x18 + (float_neg90_80420b88 - data->field_0x18) / float_10_80420b64;
		}
		break;
	default:
		break;
	}

	data->field_0x2C = data->field_0x2C + 1;
	data->field_0x2C = data->field_0x2C % 0x3C;
	if (!(data->field_0x4 & 2)) {
		if (data->field_0x2C % 10 == 0 && data->field_0x3C == 0) {
			f32 s = (f32)sin(float_6p2832_80420b7c * camGetPtr(CAMERA_3D)->viewYaw / float_360_80420b84);
			f32 c = (f32)cos(float_6p2832_80420b7c * camGetPtr(CAMERA_3D)->viewYaw / float_360_80420b84);
			f32 scale = data->field_0x14;
			f32 dsx = float_10_80420b64 * s;
			f32 dsz = float_10_80420b64 * -c;
			f32 dsy = float_10_80420b64 * scale;
			EffectEntry* entry = effStardustN64Entry(3, data->field_0x8 + dsx,
			                                          data->field_0xC - dsy,
			                                          data->field_0x10 + dsz,
			                                          dsy);
			effStardustN64SetColor(entry, color_tbl[data->field_0x0][0] / float_255_80420b78,
			                       color_tbl[data->field_0x0][1] / float_255_80420b78,
			                       color_tbl[data->field_0x0][2] / float_255_80420b78);
			effStardustN64SetDrawCam(entry, camId);
		}

		if (data->field_0x3C == 0) {
			*(s32*)((u8*)data->field_0x34->userdata + 0x68) = camId;
			*(f32*)((u8*)data->field_0x34->userdata + 0x04) = data->field_0x8;
			*(f32*)((u8*)data->field_0x34->userdata + 0x08) = data->field_0xC - float_10_80420b64 * data->field_0x14;
			*(f32*)((u8*)data->field_0x34->userdata + 0x0C) = data->field_0x10;
		} else {
			*(f32*)((u8*)data->field_0x34->userdata + 0x04) = float_0_80420b54;
			*(f32*)((u8*)data->field_0x34->userdata + 0x08) = float_neg1000_80420b8c;
			*(f32*)((u8*)data->field_0x34->userdata + 0x0C) = float_0_80420b54;
		}

		if (data->field_0x3C < 3 && camId == CAMERA_3D) {
			U_shadowSetMode(shadowEntry(data->field_0x8, data->field_0xC, data->field_0x10, float_8_80420b90 * data->field_0x14), data->field_0x5F);
		}
	}

	if (!(data->field_0x4 & 0x20)) {
		switch (type) {
		case 0:
		case 6:
			dispEntry(camId, dispCalcZ(&position), 2, effStarStoneDisp_1, effect);
			break;
		default:
			dispEntry(camId, dispCalcZ(&position), 2, effStarStoneDisp_2, effect);
			break;
		}
	}
}
#pragma opt_common_subs reset

// Passing the constant GXColor in by value keeps the by-value temporary in the slot the original
// uses; building the colour in the caller puts it 4 bytes low (same shape as effBattenDisp2).
#ifdef MUST_MATCH
static inline GXColor starStoneTevColor2(GXColor c, effStarStoneData* data) {
	c.r = data->field_0x5C;
	c.g = data->field_0x5D;
	c.b = data->field_0x5E;
	c.a = data->field_0x5F;
	return c;
}
#endif

// The original strength-reduces these indexed walks to pointer increments; `ctr = 0; for (;...)`
// and the `(s32)` cast are what make MWCC allocate the counter and pick cmpwi like the original.
#ifdef MUST_MATCH
#define STARSTONE_DL_LOOP2(off, sztbl, n, ctr) \
	{ \
		ctr = 0; \
		for (; ctr < (n); ctr++) { \
			if ((data->field_0x4 & 1) || (s32)ctr != 0) { \
				GXCallDisplayList(((void**)(base + (off)))[ctr], (sztbl)[ctr] << 5); \
			} \
		} \
	}
#else
#define STARSTONE_DL_LOOP2(off, sztbl, n, ctr) \
	for ((ctr) = 0; (ctr) < (n); (ctr)++) { \
		if ((data->field_0x4 & 1) || (ctr) != 0) { \
			GXCallDisplayList(((void**)(base + (off)))[ctr], (sztbl)[ctr] << 5); \
		} \
	}
#endif

// The original walks each display-list table with a small inline helper.  With the loop written as
// an inlined helper (its parameters become the induction variables), MWCC colours the first loop's
// counter r26 and its size induction r23; written directly in the caller the two come out swapped.
static inline void starStoneDLLoop(u8* base, effStarStoneData* data, u32 off, u8* sztbl, u32 n) {
	u32 ctr;
	for (ctr = 0; ctr < n; ctr++) {
		if ((data->field_0x4 & 1) || (s32)ctr != 0) {
			GXCallDisplayList(((void**)(base + off))[ctr], sztbl[ctr] << 5);
		}
	}
}

// The `(u32)` cast on the base address is a match-only nudge that keeps it live in r30.
void effStarStoneDisp_2(CameraId camId, void* param) {
#ifdef MUST_MATCH
	u8* base = (u8*)((u32)lbl_803AC080);
#else
	u8* base = lbl_803AC080;
#endif
	EffectEntry* effect = param;
	effStarStoneData* data = effect->userdata;
	CameraEntry* cam = camGetPtr(camId);
	s32 type = data->field_0x0;
	Mtx mtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	GXTexObj texObj;

	mapSetMaterialFog();
	GXSetArray(0xb, color_tbl[data->field_0x0], 4);
	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
	GXSetChanMatColor(4, *(GXColor*)&data->field_0x30);
	GXSetNumTexGens(0);
	GXSetNumTevStages(2);
	GXSetTevOrder(0, 0xff, 0xff, 4);
	GXSetTevOp(0, 4);
	GXSetTevOrder(1, 0xff, 0xff, 0xff);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 0xf, 2, 0, 0xf);
	GXSetTevAlphaIn(1, 7, 1, 0, 7);
#ifdef MUST_MATCH
	GXSetTevColor(1, starStoneTevColor2(*(GXColor*)&unk_804232e8, data));
#else
	{
		GXColor color = *(GXColor*)&unk_804232e8;
		color.r = data->field_0x5C;
		color.g = data->field_0x5D;
		color.b = data->field_0x5E;
		color.a = data->field_0x5F;
		GXSetTevColor(1, color);
	}
#endif
	GXSetBlendMode(1, 4, 5, 0);
	GXSetZCompLoc(1);
	GXSetAlphaCompare(7, 0, 0, 7, 0);
	GXSetZMode(1, 3, 0);
	GXSetCullMode(2);

	PSMTXTrans(mtx, data->field_0x8 + data->field_0x48, data->field_0xC, data->field_0x10 + data->field_0x4C);
	PSMTXScale(scaleMtx, float_1p1_80420b4c * data->field_0x14, float_1p1_80420b4c * data->field_0x14, float_1p1_80420b4c * data->field_0x14);
	PSMTXConcat(mtx, scaleMtx, mtx);
	PSMTXRotRad(rotMtx, 'y', float_deg2rad_80420b50 * data->field_0x1C);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXRotRad(rotMtx, 'z', float_deg2rad_80420b50 * data->field_0x20);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXRotRad(rotMtx, 'y', float_deg2rad_80420b50 * data->field_0x18);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXConcat(cam->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, 0);
	GXSetCurrentMtx(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 2);
	GXSetVtxAttrFmt(0, 9, 1, 3, 0xb);
	GXSetArray(9, base + 0x3c0, 6);
	GXSetVtxDesc(0xa, 2);
	GXSetVtxAttrFmt(0, 0xa, 0, 1, 6);
	GXSetArray(0xa, base + 0x480, 3);
	GXSetVtxDesc(0xb, 2);
	GXSetVtxAttrFmt(0, 0xb, 1, 5, 0);
	GXSetVtxDesc(0xd, 2);
	GXSetVtxAttrFmt(0, 0xd, 1, 3, 0xe);
	GXSetArray(0xd, base + 0x520, 4);
	starStoneDLLoop(base, data, 0x590, diamond_2_2_dl_0_size_tbl, 0x13);

	PSMTXTrans(mtx, data->field_0x8 + data->field_0x50, data->field_0xC, data->field_0x10 + data->field_0x54);
	PSMTXScale(scaleMtx, float_1p1_80420b4c * data->field_0x14, float_1p1_80420b4c * data->field_0x14, float_1p1_80420b4c * data->field_0x14);
	PSMTXConcat(mtx, scaleMtx, mtx);
	PSMTXRotRad(rotMtx, 'y', float_deg2rad_80420b50 * data->field_0x1C);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXRotRad(rotMtx, 'z', float_deg2rad_80420b50 * -data->field_0x20);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXRotRad(rotMtx, 'y', float_deg2rad_80420b50 * -data->field_0x18);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXConcat(cam->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, 0);
	GXSetCurrentMtx(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 2);
	GXSetVtxAttrFmt(0, 9, 1, 3, 0xb);
	GXSetArray(9, base + 0x3c0, 6);
	GXSetVtxDesc(0xa, 2);
	GXSetVtxAttrFmt(0, 0xa, 0, 1, 6);
	GXSetArray(0xa, base + 0x480, 3);
	GXSetVtxDesc(0xb, 2);
	GXSetVtxAttrFmt(0, 0xb, 1, 5, 0);
	GXSetVtxDesc(0xd, 2);
	GXSetVtxAttrFmt(0, 0xd, 1, 3, 0xe);
	GXSetArray(0xd, base + 0x520, 4);
	starStoneDLLoop(base, data, 0x5dc, diamond_2_2_dl_1_size_tbl, 0x13);

	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	PSMTXTrans(mtx, data->field_0x24, data->field_0x28, float_0_80420b54);
	GXLoadTexMtxImm(mtx, 0x1e, 1);
	GXSetNumTevStages(2);
	GXSetTevOrder(0, 0, 0, 4);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevOp(0, 0);
	GXSetTevOrder(1, 0xff, 0xff, 0xff);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 0xf, 2, 0, 0xf);
	GXSetTevAlphaIn(1, 7, 1, 0, 7);
#ifdef MUST_MATCH
	GXSetTevColor(1, starStoneTevColor2(*(GXColor*)&unk_804232ec, data));
#else
	{
		GXColor color = *(GXColor*)&unk_804232ec;
		color.r = data->field_0x5C;
		color.g = data->field_0x5D;
		color.b = data->field_0x5E;
		color.a = data->field_0x5F;
		GXSetTevColor(1, color);
	}
#endif
	if (type != 2) {
		effGetTexObj(0x29, &texObj);
		GXLoadTexObj(&texObj, 0);
	} else {
		effGetTexObj(0x2e, &texObj);
		GXLoadTexObj(&texObj, 0);
	}
	GXSetBlendMode(1, 4, 5, 0);
	GXSetZCompLoc(1);
	GXSetAlphaCompare(7, 0, 0, 7, 0);
	GXSetZMode(1, 3, 0);

	PSMTXTrans(mtx, data->field_0x8 + data->field_0x48, data->field_0xC, data->field_0x10 + data->field_0x4C);
	PSMTXScale(scaleMtx, data->field_0x14, data->field_0x14, data->field_0x14);
	PSMTXConcat(mtx, scaleMtx, mtx);
	PSMTXRotRad(rotMtx, 'y', float_deg2rad_80420b50 * data->field_0x1C);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXRotRad(rotMtx, 'z', float_deg2rad_80420b50 * data->field_0x20);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXRotRad(rotMtx, 'y', float_deg2rad_80420b50 * data->field_0x18);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXConcat(cam->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, 0);
	GXSetCurrentMtx(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 2);
	GXSetVtxAttrFmt(0, 9, 1, 3, 0xb);
	GXSetArray(9, base + 0x3c0, 6);
	GXSetVtxDesc(0xa, 2);
	GXSetVtxAttrFmt(0, 0xa, 0, 1, 6);
	GXSetArray(0xa, base + 0x480, 3);
	GXSetVtxDesc(0xb, 2);
	GXSetVtxAttrFmt(0, 0xb, 1, 5, 0);
	GXSetVtxDesc(0xd, 2);
	GXSetVtxAttrFmt(0, 0xd, 1, 3, 0xe);
	GXSetArray(0xd, base + 0x520, 4);
	starStoneDLLoop(base, data, 0x590, diamond_2_2_dl_0_size_tbl, 0x13);

	PSMTXTrans(mtx, data->field_0x8 + data->field_0x50, data->field_0xC, data->field_0x10 + data->field_0x54);
	PSMTXScale(scaleMtx, data->field_0x14, data->field_0x14, data->field_0x14);
	PSMTXConcat(mtx, scaleMtx, mtx);
	PSMTXRotRad(rotMtx, 'y', float_deg2rad_80420b50 * data->field_0x1C);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXRotRad(rotMtx, 'z', float_deg2rad_80420b50 * -data->field_0x20);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXRotRad(rotMtx, 'y', float_deg2rad_80420b50 * -data->field_0x18);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXConcat(cam->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, 0);
	GXSetCurrentMtx(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 2);
	GXSetVtxAttrFmt(0, 9, 1, 3, 0xb);
	GXSetArray(9, base + 0x3c0, 6);
	GXSetVtxDesc(0xa, 2);
	GXSetVtxAttrFmt(0, 0xa, 0, 1, 6);
	GXSetArray(0xa, base + 0x480, 3);
	GXSetVtxDesc(0xb, 2);
	GXSetVtxAttrFmt(0, 0xb, 1, 5, 0);
	GXSetVtxDesc(0xd, 2);
	GXSetVtxAttrFmt(0, 0xd, 1, 3, 0xe);
	GXSetArray(0xd, base + 0x520, 4);
	starStoneDLLoop(base, data, 0x5dc, diamond_2_2_dl_1_size_tbl, 0x13);
}

#define STARSTONE_VTX_COMMON(pos, nrm, tex) \
	GXClearVtxDesc(); \
	GXSetVtxDesc(9, 2); \
	GXSetVtxAttrFmt(0, 9, 1, 3, 0xb); \
	GXSetArray(9, base + (pos), 6); \
	GXSetVtxDesc(0xa, 2); \
	GXSetVtxAttrFmt(0, 0xa, 0, 1, 6); \
	GXSetArray(0xa, base + (nrm), 3)

#define STARSTONE_VTX_CLR(clr) \
	GXSetVtxDesc(0xb, 2); \
	GXSetVtxAttrFmt(0, 0xb, 1, 5, 0); \
	GXSetArray(0xb, base + (clr), 4)

#define STARSTONE_VTX_TAIL(tex) \
	GXSetVtxDesc(0xd, 2); \
	GXSetVtxAttrFmt(0, 0xd, 1, 3, 0xd); \
	GXSetArray(0xd, base + (tex), 4)

// The original walks both tables with pointer increments: i first, then dl, then the size pointer in
// the branch delay slot, with the counter in r27 and dl/sz in r25/r26. The block-scoped pointers and
// the explicit `i = 0` before `sz = ...` make MWCC emit that allocation; the indexed form is the
// natural equivalent.
#ifdef MUST_MATCH
#define STARSTONE_DL_LOOP(off, sztbl, n) \
	{ \
		u8* sz; \
		void** dl; \
		dl = (void**)(base + (off)); \
		i = 0; \
		sz = (sztbl); \
		for (; i < (n); i++) { \
			GXCallDisplayList(*dl, *sz << 5); \
			sz++; \
			dl++; \
		} \
	}
#else
#define STARSTONE_DL_LOOP(off, sztbl, n) \
	for (i = 0; i < (n); i++) { \
		GXCallDisplayList(((void**)(base + (off)))[i], (sztbl)[i] << 5); \
	}
#endif

// Passing the constant GXColor in by value keeps the by-value temporary in the slot the original
// uses; building the colour in the caller puts it 4 bytes low (same shape as effBattenDisp2).
#ifdef MUST_MATCH
static inline GXColor starStoneTevColor(GXColor c, effStarStoneData* data) {
	c.r = data->field_0x5C;
	c.g = data->field_0x5D;
	c.b = data->field_0x5E;
	c.a = data->field_0x5F;
	return c;
}
#endif

#pragma opt_lifetimes off // the default lifetime analysis puts the loop counter in r25, not r27
void effStarStoneDisp_1(CameraId camId, void* param) {
	u32 i;
	u8* base = lbl_803AC080;
	EffectEntry* effect = param;
	effStarStoneData* data = effect->userdata;
	CameraEntry* cam = camGetPtr(camId);
	s32 type = data->field_0x0;
	Mtx mtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	GXTexObj texObj;
	mapSetMaterialFog();
	GXSetArray(0xb, color_tbl[data->field_0x0], 4);
	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
	GXSetChanMatColor(4, *(GXColor*)&data->field_0x30);
	GXSetNumTexGens(0);
	GXSetNumTevStages(2);
	GXSetTevOrder(0, 0xff, 0xff, 4);
	GXSetTevOp(0, 4);
	GXSetTevOrder(1, 0xff, 0xff, 0xff);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 0xf, 2, 0, 0xf);
	GXSetTevAlphaIn(1, 7, 1, 0, 7);
#ifdef MUST_MATCH
	GXSetTevColor(1, starStoneTevColor(*(GXColor*)&unk_804232e0, data));
#else
	{
		GXColor color = *(GXColor*)&unk_804232e0;
		color.r = data->field_0x5C;
		color.g = data->field_0x5D;
		color.b = data->field_0x5E;
		color.a = data->field_0x5F;
		GXSetTevColor(1, color);
	}
#endif

	PSMTXTrans(mtx, data->field_0x8, data->field_0xC, data->field_0x10);
	PSMTXScale(scaleMtx, float_1p1_80420b4c * data->field_0x14, float_1p1_80420b4c * data->field_0x14, float_1p1_80420b4c * data->field_0x14);
	PSMTXConcat(mtx, scaleMtx, mtx);
	PSMTXRotRad(rotMtx, 'y', float_deg2rad_80420b50 * data->field_0x1C);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXConcat(cam->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, 0);
	GXSetCurrentMtx(0);
	GXSetBlendMode(1, 4, 5, 0);
	GXSetZCompLoc(1);
	GXSetAlphaCompare(7, 0, 0, 7, 0);
	GXSetZMode(1, 3, 0);
	GXSetCullMode(2);

	if (type == 0) {
		STARSTONE_VTX_COMMON(0x40, 0x140, 0x1c0);
		STARSTONE_VTX_CLR(0x260);
		STARSTONE_VTX_TAIL(0x1c0);
		STARSTONE_DL_LOOP(0x2a4, diamond_1_dl_0_size_tbl, 0x22);
		STARSTONE_VTX_COMMON(0x40, 0x140, 0x1c0);
		GXSetVtxDesc(0xb, 2);
		GXSetVtxAttrFmt(0, 0xb, 1, 5, 0);
		STARSTONE_VTX_TAIL(0x1c0);
		STARSTONE_DL_LOOP(0x32c, diamond_1_dl_1_size_tbl, 0x22);
	} else {
		STARSTONE_VTX_COMMON(0x640, 0x740, 0x7c0);
		STARSTONE_VTX_CLR(0x8e0);
		STARSTONE_VTX_TAIL(0x7c0);
		STARSTONE_DL_LOOP(0x92c, diamond_7_dl_0_size_tbl, 0x22);
		STARSTONE_VTX_COMMON(0x640, 0x740, 0x7c0);
		STARSTONE_VTX_CLR(0x8e0);
		STARSTONE_VTX_TAIL(0x7c0);
		STARSTONE_DL_LOOP(0x9b4, diamond_7_dl_1_size_tbl, 0x21);
	}

	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	PSMTXTrans(mtx, data->field_0x24, data->field_0x28, float_0_80420b54);
	GXLoadTexMtxImm(mtx, 0x1e, 1);
	GXSetNumTevStages(2);
	GXSetTevOrder(0, 0, 0, 4);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevOp(0, 0);
	GXSetTevOrder(1, 0xff, 0xff, 0xff);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 0xf, 2, 0, 0xf);
	GXSetTevAlphaIn(1, 7, 1, 0, 7);
#ifdef MUST_MATCH
	GXSetTevColor(1, starStoneTevColor(*(GXColor*)&unk_804232e4, data));
#else
	{
		GXColor color = *(GXColor*)&unk_804232e4;
		color.r = data->field_0x5C;
		color.g = data->field_0x5D;
		color.b = data->field_0x5E;
		color.a = data->field_0x5F;
		GXSetTevColor(1, color);
	}
#endif

	PSMTXTrans(mtx, data->field_0x8, data->field_0xC, data->field_0x10);
	PSMTXScale(scaleMtx, data->field_0x14, data->field_0x14, data->field_0x14);
	PSMTXConcat(mtx, scaleMtx, mtx);
	PSMTXRotRad(rotMtx, 'y', float_deg2rad_80420b50 * data->field_0x1C);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXConcat(cam->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, 0);
	GXSetCurrentMtx(0);
	if (type == 0) {
		effGetTexObj(0x2e, &texObj);
		GXLoadTexObj(&texObj, 0);
	} else {
		effGetTexObj(0x2f, &texObj);
		GXLoadTexObj(&texObj, 0);
	}
	GXSetBlendMode(1, 4, 5, 0);
	GXSetZCompLoc(1);
	GXSetAlphaCompare(7, 0, 0, 7, 0);
	GXSetZMode(1, 3, 0);

	if (type == 0) {
		STARSTONE_VTX_COMMON(0x40, 0x140, 0x1c0);
		STARSTONE_VTX_CLR(0x260);
		STARSTONE_VTX_TAIL(0x1c0);
		STARSTONE_DL_LOOP(0x2a4, diamond_1_dl_0_size_tbl, 0x22);
	} else {
		STARSTONE_VTX_COMMON(0x640, 0x740, 0x7c0);
		STARSTONE_VTX_CLR(0x8e0);
		STARSTONE_VTX_TAIL(0x7c0);
		STARSTONE_DL_LOOP(0x92c, diamond_7_dl_0_size_tbl, 0x22);
	}

	effGetTexObj(0x2d, &texObj);
	GXLoadTexObj(&texObj, 0);
	GXSetBlendMode(1, 4, 5, 0);
	GXSetZCompLoc(1);
	GXSetAlphaCompare(7, 0, 0, 7, 0);
	GXSetZMode(1, 3, 0);

	if (type == 0) {
		STARSTONE_VTX_COMMON(0x40, 0x140, 0x1c0);
		GXSetVtxDesc(0xb, 2);
		GXSetVtxAttrFmt(0, 0xb, 1, 5, 0);
		STARSTONE_VTX_TAIL(0x1c0);
		STARSTONE_DL_LOOP(0x32c, diamond_1_dl_1_size_tbl, 0x22);
	} else {
		STARSTONE_VTX_COMMON(0x640, 0x740, 0x7c0);
		STARSTONE_VTX_CLR(0x8e0);
		STARSTONE_VTX_TAIL(0x7c0);
		STARSTONE_DL_LOOP(0x9b4, diamond_7_dl_1_size_tbl, 0x21);
	}
}
#pragma opt_lifetimes reset
