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
extern u32 diamond_1_vertex_tbl[];
extern u32 diamond_1_normal_tbl[];
extern u32 diamond_1_texcoord0_tbl[];
extern u8 diamond_1_color0_tbl[];
extern u32 diamond_1_dl_0_tbl[];
extern u32 diamond_1_dl_1_tbl[];
extern u32 diamond_7_vertex_tbl[];
extern u32 diamond_7_normal_tbl[];
extern u32 diamond_7_texcoord0_tbl[];
extern u8 diamond_7_color0_tbl[];
extern u32 diamond_7_dl_0_tbl[];
extern u32 diamond_7_dl_1_tbl[];
extern u32 diamond_2_2_vertex_tbl[];
extern u32 diamond_2_2_normal_tbl[];
extern u32 diamond_2_2_texcoord0_tbl[];
extern u32 diamond_2_2_dl_0_tbl[];
extern u32 diamond_2_2_dl_1_tbl[];
extern u8 diamond_1_dl_0_size_tbl[36];
extern u8 diamond_1_dl_1_size_tbl[36];
extern u8 diamond_2_2_dl_0_size_tbl[20];
extern u8 diamond_2_2_dl_1_size_tbl[20];
extern u8 diamond_7_dl_0_size_tbl[36];
extern u8 diamond_7_dl_1_size_tbl[36];

u32 diamond_1_dl_0_0[8];
u32 diamond_1_dl_0_1[8];
u32 diamond_1_dl_0_2[8];
u32 diamond_1_dl_0_3[8];
u32 diamond_1_dl_0_4[8];
u32 diamond_1_dl_0_5[8];
u32 diamond_1_dl_0_6[8];
u32 diamond_1_dl_0_7[8];
u32 diamond_1_dl_0_8[8];
u32 diamond_1_dl_0_9[8];
u32 diamond_1_dl_0_10[8];
u32 diamond_1_dl_0_11[8];
u32 diamond_1_dl_0_12[8];
u32 diamond_1_dl_0_13[8];
u32 diamond_1_dl_0_14[8];
u32 diamond_1_dl_0_15[8];
u32 diamond_1_dl_0_16[8];
u32 diamond_1_dl_0_17[8];
u32 diamond_1_dl_0_18[8];
u32 diamond_1_dl_0_19[8];
u32 diamond_1_dl_0_20[8];
u32 diamond_1_dl_0_21[8];
u32 diamond_1_dl_0_22[8];
u32 diamond_1_dl_0_23[8];
u32 diamond_1_dl_0_24[8];
u32 diamond_1_dl_0_25[8];
u32 diamond_1_dl_0_26[8];
u32 diamond_1_dl_0_27[8];
u32 diamond_1_dl_0_28[8];
u32 diamond_1_dl_0_29[8];
u32 diamond_1_dl_0_30[8];
u32 diamond_1_dl_0_31[8];
u32 diamond_1_dl_0_32[8];
u32 diamond_1_dl_0_33[8];
u32 diamond_1_dl_1_0[8];
u32 diamond_1_dl_1_1[8];
u32 diamond_1_dl_1_2[8];
u32 diamond_1_dl_1_3[8];
u32 diamond_1_dl_1_4[8];
u32 diamond_1_dl_1_5[8];
u32 diamond_1_dl_1_6[8];
u32 diamond_1_dl_1_7[8];
u32 diamond_1_dl_1_8[8];
u32 diamond_1_dl_1_9[8];
u32 diamond_1_dl_1_10[8];
u32 diamond_1_dl_1_11[8];
u32 diamond_1_dl_1_12[8];
u32 diamond_1_dl_1_13[8];
u32 diamond_1_dl_1_14[8];
u32 diamond_1_dl_1_15[8];
u32 diamond_1_dl_1_16[8];
u32 diamond_1_dl_1_17[8];
u32 diamond_1_dl_1_18[8];
u32 diamond_1_dl_1_19[8];
u32 diamond_1_dl_1_20[8];
u32 diamond_1_dl_1_21[8];
u32 diamond_1_dl_1_22[8];
u32 diamond_1_dl_1_23[8];
u32 diamond_1_dl_1_24[8];
u32 diamond_1_dl_1_25[8];
u32 diamond_1_dl_1_26[8];
u32 diamond_1_dl_1_27[8];
u32 diamond_1_dl_1_28[8];
u32 diamond_1_dl_1_29[8];
u32 diamond_1_dl_1_30[8];
u32 diamond_1_dl_1_31[8];
u32 diamond_1_dl_1_32[8];
u32 diamond_1_dl_1_33[8];
u32 diamond_2_2_dl_0_0[8];
u32 diamond_2_2_dl_0_1[8];
u32 diamond_2_2_dl_0_2[8];
u32 diamond_2_2_dl_0_3[8];
u32 diamond_2_2_dl_0_4[8];
u32 diamond_2_2_dl_0_5[8];
u32 diamond_2_2_dl_0_6[8];
u32 diamond_2_2_dl_0_7[8];
u32 diamond_2_2_dl_0_8[8];
u32 diamond_2_2_dl_0_9[8];
u32 diamond_2_2_dl_0_10[8];
u32 diamond_2_2_dl_0_11[8];
u32 diamond_2_2_dl_0_12[8];
u32 diamond_2_2_dl_0_13[8];
u32 diamond_2_2_dl_0_14[8];
u32 diamond_2_2_dl_0_15[8];
u32 diamond_2_2_dl_0_16[8];
u32 diamond_2_2_dl_0_17[8];
u32 diamond_2_2_dl_0_18[8];
u32 diamond_2_2_dl_1_0[8];
u32 diamond_2_2_dl_1_1[8];
u32 diamond_2_2_dl_1_2[8];
u32 diamond_2_2_dl_1_3[8];
u32 diamond_2_2_dl_1_4[8];
u32 diamond_2_2_dl_1_5[8];
u32 diamond_2_2_dl_1_6[8];
u32 diamond_2_2_dl_1_7[8];
u32 diamond_2_2_dl_1_8[8];
u32 diamond_2_2_dl_1_9[8];
u32 diamond_2_2_dl_1_10[8];
u32 diamond_2_2_dl_1_11[8];
u32 diamond_2_2_dl_1_12[8];
u32 diamond_2_2_dl_1_13[8];
u32 diamond_2_2_dl_1_14[8];
u32 diamond_2_2_dl_1_15[8];
u32 diamond_2_2_dl_1_16[8];
u32 diamond_2_2_dl_1_17[8];
u32 diamond_2_2_dl_1_18[8];
u32 diamond_7_dl_0_0[8];
u32 diamond_7_dl_0_1[8];
u32 diamond_7_dl_0_2[8];
u32 diamond_7_dl_0_3[8];
u32 diamond_7_dl_0_4[8];
u32 diamond_7_dl_0_5[8];
u32 diamond_7_dl_0_6[8];
u32 diamond_7_dl_0_7[8];
u32 diamond_7_dl_0_8[8];
u32 diamond_7_dl_0_9[8];
u32 diamond_7_dl_0_10[8];
u32 diamond_7_dl_0_11[8];
u32 diamond_7_dl_0_12[8];
u32 diamond_7_dl_0_13[8];
u32 diamond_7_dl_0_14[8];
u32 diamond_7_dl_0_15[8];
u32 diamond_7_dl_0_16[8];
u32 diamond_7_dl_0_17[8];
u32 diamond_7_dl_0_18[8];
u32 diamond_7_dl_0_19[8];
u32 diamond_7_dl_0_20[8];
u32 diamond_7_dl_0_21[8];
u32 diamond_7_dl_0_22[8];
u32 diamond_7_dl_0_23[8];
u32 diamond_7_dl_0_24[8];
u32 diamond_7_dl_0_25[8];
u32 diamond_7_dl_0_26[8];
u32 diamond_7_dl_0_27[8];
u32 diamond_7_dl_0_28[8];
u32 diamond_7_dl_0_29[8];
u32 diamond_7_dl_0_30[8];
u32 diamond_7_dl_0_31[8];
u32 diamond_7_dl_0_32[8];
u32 diamond_7_dl_0_33[8];
u32 diamond_7_dl_1_0[8];
u32 diamond_7_dl_1_1[8];
u32 diamond_7_dl_1_2[8];
u32 diamond_7_dl_1_3[8];
u32 diamond_7_dl_1_4[8];
u32 diamond_7_dl_1_5[8];
u32 diamond_7_dl_1_6[8];
u32 diamond_7_dl_1_7[8];
u32 diamond_7_dl_1_8[8];
u32 diamond_7_dl_1_9[8];
u32 diamond_7_dl_1_10[8];
u32 diamond_7_dl_1_11[8];
u32 diamond_7_dl_1_12[8];
u32 diamond_7_dl_1_13[8];
u32 diamond_7_dl_1_14[8];
u32 diamond_7_dl_1_15[8];
u32 diamond_7_dl_1_16[8];
u32 diamond_7_dl_1_17[8];
u32 diamond_7_dl_1_18[8];
u32 diamond_7_dl_1_19[8];
u32 diamond_7_dl_1_20[8];
u32 diamond_7_dl_1_21[8];
u32 diamond_7_dl_1_22[8];
u32 diamond_7_dl_1_23[8];
u32 diamond_7_dl_1_24[8];
u32 diamond_7_dl_1_25[8];
u32 diamond_7_dl_1_26[8];
u32 diamond_7_dl_1_27[8];
u32 diamond_7_dl_1_28[8];
u32 diamond_7_dl_1_29[8];
u32 diamond_7_dl_1_30[8];
u32 diamond_7_dl_1_31[8];
u32 diamond_7_dl_1_32[8];

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u8 lbl_803AC080[8] = { 0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00 };

extern u8 diamond_1_color0_tbl[];
extern u8 diamond_2_color0_tbl[];
extern u8 diamond_3_color0_tbl[];
extern u8 diamond_4_color0_tbl[];
extern u8 diamond_5_color0_tbl[];
extern u8 diamond_6_color0_tbl[];
extern u8 diamond_7_color0_tbl[];

extern Vec edge_color[7];

static u8* color_tbl[7];

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

	data->starType = idx % 7;
	data->flags = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->rotY = 0.0f;
	data->scale = 1.5f * scale;
	data->scrollU = 0.0f;
	data->scrollV = 0.0f;
	data->camId = 4;
	data->state = 0;
	data->spreadAZ = 0.0f;
	data->spreadAX = 0.0f;
	data->spreadBZ = 0.0f;
	data->spreadBX = 0.0f;
	data->field_0x58 = 90.0f;
	data->tevReg0R = 0xFF;
	data->tevReg0G = 0xFF;
	data->tevReg0B = 0xFF;
	data->tevReg0A = 0xFF;
	data->sfxHandle = 0;

	{
		GXColor color = { 0, 0, 0, 0xFF };
		color.r = 255.0f * edge_color[data->starType].x;
		color.g = 255.0f * edge_color[data->starType].y;
		color.b = 255.0f * edge_color[data->starType].z;
		data->matColor = color;
	}

	if (idx >= 7) {
		data->flags |= 2;
	}
	if (!(data->flags & 2)) {
		u8* clr = color_tbl[data->starType];
		data->dustEffect = effSnowDustN64Entry(2, x, y - 10.0f * data->scale, z, 4, 0,
		                                       20.0f * data->scale, 20.0f * data->scale, 1.0f);
		((effSnowDustData*)data->dustEffect->userdata)->topR = clr[0];
		((effSnowDustData*)data->dustEffect->userdata)->topG = clr[1];
		((effSnowDustData*)data->dustEffect->userdata)->topB = clr[2];
	}
	((effStarStoneData*)entry->userdata)->flags |= 4;
	if (idx >= 7) {
		((effStarStoneData*)entry->userdata)->flags &= ~4;
	}
	if (gp->inBattle != 0) {
		((effStarStoneData*)entry->userdata)->flags &= ~4;
	}
	return entry;
}

void effStarStoneMain(EffectEntry* effect) {
	effStarStoneData* data = effect->userdata;
	Vec position;
	Vec tmp = (Vec){ 0.0f, 0.0f, 0.0f };
	s32 type;
	s32 camId;
	s32 flag;
	f32 hitX, hitY, hitZ, distance, hitNX, hitNY, hitNZ;

	tmp.x = data->x;
	tmp.y = data->y;
	tmp.z = data->z;
	position = tmp;
	type = data->starType;
	camId = data->camId;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		if (data->dustEffect != NULL) {
			effSoftDelete(data->dustEffect);
		}
		if (data->flags & 8) {
			psndSFXOff(data->sfxHandle);
		}
		effDelete(effect);
		return;
	}

	if (data->flags & 4) {
		if (!(data->flags & 8)) {
			data->flags |= 8;
			data->sfxHandle = psndSFXOn_3D("SFX_EVT_STARSTONE_SHINE1", &position);
		}
		if (data->flags & 8) {
			psndSFX_pos(data->sfxHandle, &position);
		}
	} else {
		if (data->flags & 8) {
			data->flags &= ~8;
			psndSFXOff(data->sfxHandle);
		}
	}

	switch (data->state) {
	case 0:
		switch (type) {
		case 6:
			data->scrollU -= 0.01f;
			if (data->scrollU <= 0.0f) {
				data->scrollU += 1.0f;
			}
			break;
		default:
			data->scrollU += 0.01f;
			if (data->scrollU >= 1.0f) {
				data->scrollU -= 1.0f;
			}
			break;
		}
		break;
	case 1:
		data->flags |= 1;
		data->velY = 0.0f;
		data->gravity = 0.4f;
		data->state++;
		break;
	case 2:
	case 3:
	case 4:
	case 5:
		flag = 0;
		if (data->velY < 0.0f) {
			distance = 10.0f + __fabsf(data->velY);
			if (hitCheckFilter(0, data->x, data->y, data->z, 0.0f, -1.0f, 0.0f, &hitX,
			                   &hitY, &hitZ, &distance,
			                   &hitNX, &hitNY, &hitNZ) != NULL) {
				flag = 1;
				data->y = 10.0f + hitY;
				data->velY *= -0.5f;
				data->state++;
				if (data->flags & 8) {
					data->flags &= ~8;
					psndSFXOff(data->sfxHandle);
					((effStarStoneData*)effect->userdata)->flags &= ~4;
				}
				if (!(data->flags & 0x10)) {
					psndSFXOn_3D("SFX_EVT_STARSTONE_JUMP1", &position);
				}
				if (!(data->flags & 2) && data->state == 3) {
					f32 scale = data->scale;
					EffectEntry* entry = effStardustN64Entry(4, data->x,
					                                          data->y + 10.0f * scale + 5.0f * scale,
					                                          data->z, 20.0f * scale);
					effStardustN64SetColor(entry, color_tbl[data->starType][0] / 255.0f,
					                       color_tbl[data->starType][1] / 255.0f,
					                       color_tbl[data->starType][2] / 255.0f);
				}
			}
		}
		if (flag == 0) {
			data->y += data->velY;
			data->velY -= data->gravity;
		}
		if (data->state > 2) {
			data->spreadAX -= (f32)sin(6.2831855f * (90.0f + camGetPtr(data->camId)->viewYaw) / 360.0f);
			data->spreadAZ -= (f32)cos(6.2831855f * (90.0f + camGetPtr(data->camId)->viewYaw) / 360.0f);
			data->spreadBX += (f32)sin(6.2831855f * (90.0f + camGetPtr(data->camId)->viewYaw) / 360.0f);
			data->spreadBZ += (f32)cos(6.2831855f * (90.0f + camGetPtr(data->camId)->viewYaw) / 360.0f);
			data->tipRotZ = data->tipRotZ + (90.0f - data->tipRotZ) / 10.0f;
		}
		if (data->state > 3) {
			data->tipRotY = data->tipRotY + (-90.0f - data->tipRotY) / 10.0f;
		}
		break;
	default:
		break;
	}

	data->timer = data->timer + 1;
	data->timer = data->timer % 0x3C;
	if (!(data->flags & 2)) {
		if (data->timer % 10 == 0 && data->state == 0) {
			f32 s = (f32)sin(6.2831855f * camGetPtr(CAMERA_3D)->viewYaw / 360.0f);
			f32 c = (f32)cos(6.2831855f * camGetPtr(CAMERA_3D)->viewYaw / 360.0f);
			f32 scale = data->scale;
			f32 dsx = 10.0f * s;
			f32 dsz = 10.0f * -c;
			f32 dsy = 10.0f * scale;
			EffectEntry* entry = effStardustN64Entry(3, data->x + dsx,
			                                          data->y - dsy,
			                                          data->z + dsz,
			                                          dsy);
			effStardustN64SetColor(entry, color_tbl[data->starType][0] / 255.0f,
			                       color_tbl[data->starType][1] / 255.0f,
			                       color_tbl[data->starType][2] / 255.0f);
			effStardustN64SetDrawCam(entry, camId);
		}

		if (data->state == 0) {
			((effSnowDustData*)data->dustEffect->userdata)->camId = camId;
			((effSnowDustData*)data->dustEffect->userdata)->x = data->x;
			((effSnowDustData*)data->dustEffect->userdata)->y = data->y - 10.0f * data->scale;
			((effSnowDustData*)data->dustEffect->userdata)->z = data->z;
		} else {
			((effSnowDustData*)data->dustEffect->userdata)->x = 0.0f;
			((effSnowDustData*)data->dustEffect->userdata)->y = -1000.0f;
			((effSnowDustData*)data->dustEffect->userdata)->z = 0.0f;
		}

		if (data->state < 3 && camId == CAMERA_3D) {
			U_shadowSetMode(shadowEntry(data->x, data->y, data->z, 8.0f * data->scale), data->tevReg0A);
		}
	}

	if (!(data->flags & 0x20)) {
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


#define STARSTONE_VTX_COMMON(pos, nrm, tex) \
	GXClearVtxDesc(); \
	GXSetVtxDesc(GX_VA_POS, GX_INDEX8); \
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0xb); \
	GXSetArray(GX_VA_POS, (pos), 6); \
	GXSetVtxDesc(GX_VA_NRM, GX_INDEX8); \
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6); \
	GXSetArray(GX_VA_NRM, (nrm), 3)

#define STARSTONE_VTX_CLR(clr) \
	GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8); \
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0); \
	GXSetArray(GX_VA_CLR0, (clr), 4)

#define STARSTONE_VTX_TAIL(tex) \
	GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8); \
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0xd); \
	GXSetArray(GX_VA_TEX0, (tex), 4)

// The original walks both tables with pointer increments: i first, then dl, then the size pointer in
// the branch delay slot, with the counter in r27 and dl/sz in r25/r26.
#define STARSTONE_DL_LOOP(tbl, sztbl, n) \
	{ \
		u8* sz; \
		u32* dl; \
		for (dl = (tbl), i = 0, sz = (sztbl); i < (n); i++, sz++, dl++) { \
			GXCallDisplayList((void*)*dl, *sz << 5); \
		} \
	}


void effStarStoneDisp_1(CameraId camId, void* param) {
	u32 i;
	EffectEntry* effect = param;
	effStarStoneData* data = effect->userdata;
	CameraEntry* cam = camGetPtr(camId);
	s32 type = data->starType;
	Mtx mtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	GXTexObj texObj;
	mapSetMaterialFog();
	GXSetArray(GX_VA_CLR0, color_tbl[data->starType], 4);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
	GXSetChanMatColor(GX_COLOR0A0, data->matColor);
	GXSetNumTexGens(0);
	GXSetNumTevStages(2);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_C0, GX_CC_CPREV, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_A0, GX_CA_APREV, GX_CA_ZERO);
	GXSetTevColor(GX_TEVREG0, (GXColor){ data->tevReg0R, data->tevReg0G, data->tevReg0B, data->tevReg0A });

	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXScale(scaleMtx, 1.1f * data->scale, 1.1f * data->scale, 1.1f * data->scale);
	PSMTXConcat(mtx, scaleMtx, mtx);
	PSMTXRotRad(rotMtx, 'y', 0.017453292f * data->rotY);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXConcat(cam->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
	GXSetCullMode(GX_CULL_BACK);

	if (type == 0) {
		STARSTONE_VTX_COMMON(diamond_1_vertex_tbl, diamond_1_normal_tbl, diamond_1_texcoord0_tbl);
		STARSTONE_VTX_CLR(diamond_1_color0_tbl);
		STARSTONE_VTX_TAIL(diamond_1_texcoord0_tbl);
		STARSTONE_DL_LOOP(diamond_1_dl_0_tbl, diamond_1_dl_0_size_tbl, 0x22);
		STARSTONE_VTX_COMMON(diamond_1_vertex_tbl, diamond_1_normal_tbl, diamond_1_texcoord0_tbl);
		GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
		STARSTONE_VTX_TAIL(diamond_1_texcoord0_tbl);
		STARSTONE_DL_LOOP(diamond_1_dl_1_tbl, diamond_1_dl_1_size_tbl, 0x22);
	} else {
		STARSTONE_VTX_COMMON(diamond_7_vertex_tbl, diamond_7_normal_tbl, diamond_7_texcoord0_tbl);
		STARSTONE_VTX_CLR(diamond_7_color0_tbl);
		STARSTONE_VTX_TAIL(diamond_7_texcoord0_tbl);
		STARSTONE_DL_LOOP(diamond_7_dl_0_tbl, diamond_7_dl_0_size_tbl, 0x22);
		STARSTONE_VTX_COMMON(diamond_7_vertex_tbl, diamond_7_normal_tbl, diamond_7_texcoord0_tbl);
		STARSTONE_VTX_CLR(diamond_7_color0_tbl);
		STARSTONE_VTX_TAIL(diamond_7_texcoord0_tbl);
		STARSTONE_DL_LOOP(diamond_7_dl_1_tbl, diamond_7_dl_1_size_tbl, 0x21);
	}

	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	PSMTXTrans(mtx, data->scrollU, data->scrollV, 0.0f);
	GXLoadTexMtxImm(mtx, GX_TEXMTX0, GX_MTX2x4);
	GXSetNumTevStages(2);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_C0, GX_CC_CPREV, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_A0, GX_CA_APREV, GX_CA_ZERO);
	GXSetTevColor(GX_TEVREG0, (GXColor){ data->tevReg0R, data->tevReg0G, data->tevReg0B, data->tevReg0A });

	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXScale(scaleMtx, data->scale, data->scale, data->scale);
	PSMTXConcat(mtx, scaleMtx, mtx);
	PSMTXRotRad(rotMtx, 'y', 0.017453292f * data->rotY);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXConcat(cam->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
	if (type == 0) {
		effGetTexObj(0x2e, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
	} else {
		effGetTexObj(0x2f, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
	}
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);

	if (type == 0) {
		STARSTONE_VTX_COMMON(diamond_1_vertex_tbl, diamond_1_normal_tbl, diamond_1_texcoord0_tbl);
		STARSTONE_VTX_CLR(diamond_1_color0_tbl);
		STARSTONE_VTX_TAIL(diamond_1_texcoord0_tbl);
		STARSTONE_DL_LOOP(diamond_1_dl_0_tbl, diamond_1_dl_0_size_tbl, 0x22);
	} else {
		STARSTONE_VTX_COMMON(diamond_7_vertex_tbl, diamond_7_normal_tbl, diamond_7_texcoord0_tbl);
		STARSTONE_VTX_CLR(diamond_7_color0_tbl);
		STARSTONE_VTX_TAIL(diamond_7_texcoord0_tbl);
		STARSTONE_DL_LOOP(diamond_7_dl_0_tbl, diamond_7_dl_0_size_tbl, 0x22);
	}

	effGetTexObj(0x2d, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);

	if (type == 0) {
		STARSTONE_VTX_COMMON(diamond_1_vertex_tbl, diamond_1_normal_tbl, diamond_1_texcoord0_tbl);
		GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
		STARSTONE_VTX_TAIL(diamond_1_texcoord0_tbl);
		STARSTONE_DL_LOOP(diamond_1_dl_1_tbl, diamond_1_dl_1_size_tbl, 0x22);
	} else {
		STARSTONE_VTX_COMMON(diamond_7_vertex_tbl, diamond_7_normal_tbl, diamond_7_texcoord0_tbl);
		STARSTONE_VTX_CLR(diamond_7_color0_tbl);
		STARSTONE_VTX_TAIL(diamond_7_texcoord0_tbl);
		STARSTONE_DL_LOOP(diamond_7_dl_1_tbl, diamond_7_dl_1_size_tbl, 0x21);
	}
}

// The original walks each display-list table with a small inline helper.  With the loop written as
// an inlined helper (its parameters become the induction variables), MWCC colours the first loop's
// counter r26 and its size induction r23; written directly in the caller the two come out swapped.
static inline void starStoneDLLoop(u32* tbl, effStarStoneData* data, u8* sztbl, u32 n) {
	u32 ctr;
	for (ctr = 0; ctr < n; ctr++) {
		if ((data->flags & 1) || (s32)ctr != 0) {
			GXCallDisplayList((void*)tbl[ctr], sztbl[ctr] << 5);
		}
	}
}

void effStarStoneDisp_2(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effStarStoneData* data = effect->userdata;
	CameraEntry* cam = camGetPtr(camId);
	s32 type = data->starType;
	Mtx mtx;
	Mtx scaleMtx;
	Mtx rotMtx;
	GXTexObj texObj;

	mapSetMaterialFog();
	GXSetArray(GX_VA_CLR0, color_tbl[data->starType], 4);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
	GXSetChanMatColor(GX_COLOR0A0, data->matColor);
	GXSetNumTexGens(0);
	GXSetNumTevStages(2);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_C0, GX_CC_CPREV, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_A0, GX_CA_APREV, GX_CA_ZERO);
	GXSetTevColor(GX_TEVREG0, (GXColor){ data->tevReg0R, data->tevReg0G, data->tevReg0B, data->tevReg0A });
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
	GXSetCullMode(GX_CULL_BACK);

	PSMTXTrans(mtx, data->x + data->spreadAX, data->y, data->z + data->spreadAZ);
	PSMTXScale(scaleMtx, 1.1f * data->scale, 1.1f * data->scale, 1.1f * data->scale);
	PSMTXConcat(mtx, scaleMtx, mtx);
	PSMTXRotRad(rotMtx, 'y', 0.017453292f * data->rotY);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXRotRad(rotMtx, 'z', 0.017453292f * data->tipRotZ);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXRotRad(rotMtx, 'y', 0.017453292f * data->tipRotY);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXConcat(cam->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0xb);
	GXSetArray(GX_VA_POS, diamond_2_2_vertex_tbl, 6);
	GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
	GXSetArray(GX_VA_NRM, diamond_2_2_normal_tbl, 3);
	GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0xe);
	GXSetArray(GX_VA_TEX0, diamond_2_2_texcoord0_tbl, 4);
	starStoneDLLoop(diamond_2_2_dl_0_tbl, data, diamond_2_2_dl_0_size_tbl, 0x13);

	PSMTXTrans(mtx, data->x + data->spreadBX, data->y, data->z + data->spreadBZ);
	PSMTXScale(scaleMtx, 1.1f * data->scale, 1.1f * data->scale, 1.1f * data->scale);
	PSMTXConcat(mtx, scaleMtx, mtx);
	PSMTXRotRad(rotMtx, 'y', 0.017453292f * data->rotY);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXRotRad(rotMtx, 'z', 0.017453292f * -data->tipRotZ);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXRotRad(rotMtx, 'y', 0.017453292f * -data->tipRotY);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXConcat(cam->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0xb);
	GXSetArray(GX_VA_POS, diamond_2_2_vertex_tbl, 6);
	GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
	GXSetArray(GX_VA_NRM, diamond_2_2_normal_tbl, 3);
	GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0xe);
	GXSetArray(GX_VA_TEX0, diamond_2_2_texcoord0_tbl, 4);
	starStoneDLLoop(diamond_2_2_dl_1_tbl, data, diamond_2_2_dl_1_size_tbl, 0x13);

	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	PSMTXTrans(mtx, data->scrollU, data->scrollV, 0.0f);
	GXLoadTexMtxImm(mtx, GX_TEXMTX0, GX_MTX2x4);
	GXSetNumTevStages(2);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_C0, GX_CC_CPREV, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_A0, GX_CA_APREV, GX_CA_ZERO);
	GXSetTevColor(GX_TEVREG0, (GXColor){ data->tevReg0R, data->tevReg0G, data->tevReg0B, data->tevReg0A });
	if (type != 2) {
		effGetTexObj(0x29, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
	} else {
		effGetTexObj(0x2e, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
	}
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);

	PSMTXTrans(mtx, data->x + data->spreadAX, data->y, data->z + data->spreadAZ);
	PSMTXScale(scaleMtx, data->scale, data->scale, data->scale);
	PSMTXConcat(mtx, scaleMtx, mtx);
	PSMTXRotRad(rotMtx, 'y', 0.017453292f * data->rotY);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXRotRad(rotMtx, 'z', 0.017453292f * data->tipRotZ);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXRotRad(rotMtx, 'y', 0.017453292f * data->tipRotY);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXConcat(cam->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0xb);
	GXSetArray(GX_VA_POS, diamond_2_2_vertex_tbl, 6);
	GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
	GXSetArray(GX_VA_NRM, diamond_2_2_normal_tbl, 3);
	GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0xe);
	GXSetArray(GX_VA_TEX0, diamond_2_2_texcoord0_tbl, 4);
	starStoneDLLoop(diamond_2_2_dl_0_tbl, data, diamond_2_2_dl_0_size_tbl, 0x13);

	PSMTXTrans(mtx, data->x + data->spreadBX, data->y, data->z + data->spreadBZ);
	PSMTXScale(scaleMtx, data->scale, data->scale, data->scale);
	PSMTXConcat(mtx, scaleMtx, mtx);
	PSMTXRotRad(rotMtx, 'y', 0.017453292f * data->rotY);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXRotRad(rotMtx, 'z', 0.017453292f * -data->tipRotZ);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXRotRad(rotMtx, 'y', 0.017453292f * -data->tipRotY);
	PSMTXConcat(mtx, rotMtx, mtx);
	PSMTXConcat(cam->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0xb);
	GXSetArray(GX_VA_POS, diamond_2_2_vertex_tbl, 6);
	GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
	GXSetArray(GX_VA_NRM, diamond_2_2_normal_tbl, 3);
	GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0xe);
	GXSetArray(GX_VA_TEX0, diamond_2_2_texcoord0_tbl, 4);
	starStoneDLLoop(diamond_2_2_dl_1_tbl, data, diamond_2_2_dl_1_size_tbl, 0x13);
}


void effStarStoneDraw(CameraId camId, EffectEntry* entry) {
	effStarStoneData* data = entry->userdata;

	switch (data->starType) {
	case 0:
	case 6:
		effStarStoneDisp_1(camId, entry);
		break;
	default:
		effStarStoneDisp_2(camId, entry);
		break;
	}
}

#pragma force_active on

// TODO: type
static __declspec(section ".data") u32 negone_one_374[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_803AC090[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
static __declspec(section ".data") u32 one_negone_373[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_803AC0A0[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
static __declspec(section ".data") u32 negone_one_360[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_803AC0B0[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
static __declspec(section ".data") u32 one_negone_359[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
u32 diamond_1_vertex_tbl[64] = {
    0x47D21677, 0x00001C71, 0x23AF0000, 0x00004AA5,
    0x0000E38F, 0x23AF0000, 0xB82E1677, 0x0000D1FB,
    0xED960000, 0xD39DC209, 0x00000000, 0xCC270000,
    0x2C63C209, 0x00002E05, 0xED960000, 0x1A1405B4,
    0xE5710000, 0x18A7E571, 0xE5EC05B4, 0xE571EFE2,
    0xE70DE571, 0x101EE70D, 0xE5711A14, 0x05B41A8F,
    0x000018A7, 0x1A8FE5EC, 0x05B41A8F, 0xEFE2E70D,
    0x1A8F101E, 0xE70D1A8F, 0x48C516C3, 0x00001CD1,
    0x24280000, 0x00004BA2, 0x0000E32F, 0x24280000,
    0xB73B16C3, 0x0000D15F, 0xED580000, 0xD307C138,
    0x00000000, 0xCB770000, 0x2CF9C138, 0x00002EA1,
    0xED580000, 0x1A6C05C7, 0xE5170000, 0x18FAE517,
    0xE59405C7, 0xE517EFAC, 0xE6B8E517, 0x1054E6B8,
    0xE5171A6C, 0x05C71AE9, 0x000018FA, 0x1AE9E594,
    0x05C71AE9, 0xEFACE6B8, 0x1AE91054, 0xE6B81AE9,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_normal_tbl[32] = {
    0x001C3919, 0x232F1B09, 0x39001E38, 0xE50939E7,
    0x232FE40A, 0x38EFE939, 0xD7F42FED, 0xE73711E9,
    0x3900D62F, 0x13E73729, 0xF42F1C0A, 0x381B25D4,
    0x001CC71A, 0x08C7001E, 0xC82219D1, 0xE525D4E6,
    0x08C7E40A, 0xC8F429D1, 0xD5F2D3F0, 0xEAC7EDE7,
    0xC9D6FFD1, 0x00D4D310, 0xEAC713E7, 0xC9F7D7D1,
    0x2BF2D31C, 0x0AC824EA, 0xD12AFFD1, 0x09D7D1DC,
    0xEAD1DE19, 0xD10C29D1, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_texcoord0_tbl[40] = {
    0x11990B33, 0x0CCC1199, 0x0CCCFB34, 0x1333FE67,
    0x13330999, 0x04CC0000, 0x066604CC, 0x20000666,
    0x0CCC1000, 0x1CCC0B33, 0x1E660000, 0x14CC0999,
    0x0CCC0800, 0x0CCC0B33, 0x06661666, 0x04CC0CCC,
    0x33331000, 0x3E662333, 0x2B330E66, 0x40000CCC,
    0x29992000, 0x2999192D, 0x29990000, 0x219904CD,
    0x21992000, 0x21990000, 0x28000CCC, 0x24CC0CCC,
    0x2CCC1000, 0x3B331666, 0x38001E66, 0x40001000,
    0x2E661000, 0x30000CCC, 0x24CC2999, 0x29990999,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u8 diamond_1_color0_tbl[68] = { 0xD7, 0xE0, 0xC2, 0xFF, 0xBD, 0xC2, 0xA6, 0xFF, 0xF0, 0xF0, 0xE4, 0xFF, 0xAB, 0xAC, 0x8F, 0xFF, 0xF5, 0xFF, 0xD9, 0xFF, 0xEE, 0xF5, 0xCF, 0x00, 0xE8, 0xEE, 0xC4, 0x4C, 0xEB, 0xF2, 0xCA, 0x00, 0xE7, 0xED, 0xC5, 0x4C, 0xEA, 0xEF, 0xC9, 0x00, 0xF9, 0xFF, 0xE0, 0x4C, 0xDB, 0xDE, 0xB5, 0x4C, 0xE0, 0xE4, 0xBA, 0x4C, 0xE0, 0xE5, 0xB9, 0x00, 0xD4, 0xD9, 0xA8, 0x4C, 0xC9, 0xCC, 0x9B, 0x4C, 0xDC, 0xE2, 0xB3, 0x4C };

// TODO: type
u32 diamond_1_dl_0_tbl[34] = {
    (u32)&diamond_1_dl_0_0, (u32)&diamond_1_dl_0_1, (u32)&diamond_1_dl_0_2, (u32)&diamond_1_dl_0_3,
    (u32)&diamond_1_dl_0_4, (u32)&diamond_1_dl_0_5, (u32)&diamond_1_dl_0_6, (u32)&diamond_1_dl_0_7,
    (u32)&diamond_1_dl_0_8, (u32)&diamond_1_dl_0_9, (u32)&diamond_1_dl_0_10, (u32)&diamond_1_dl_0_11,
    (u32)&diamond_1_dl_0_12, (u32)&diamond_1_dl_0_13, (u32)&diamond_1_dl_0_14, (u32)&diamond_1_dl_0_15,
    (u32)&diamond_1_dl_0_16, (u32)&diamond_1_dl_0_17, (u32)&diamond_1_dl_0_18, (u32)&diamond_1_dl_0_19,
    (u32)&diamond_1_dl_0_20, (u32)&diamond_1_dl_0_21, (u32)&diamond_1_dl_0_22, (u32)&diamond_1_dl_0_23,
    (u32)&diamond_1_dl_0_24, (u32)&diamond_1_dl_0_25, (u32)&diamond_1_dl_0_26, (u32)&diamond_1_dl_0_27,
    (u32)&diamond_1_dl_0_28, (u32)&diamond_1_dl_0_29, (u32)&diamond_1_dl_0_30, (u32)&diamond_1_dl_0_31,
    (u32)&diamond_1_dl_0_32, (u32)&diamond_1_dl_0_33,
};

// TODO: type
u32 diamond_1_dl_1_tbl[34] = {
    (u32)&diamond_1_dl_1_0, (u32)&diamond_1_dl_1_1, (u32)&diamond_1_dl_1_2, (u32)&diamond_1_dl_1_3,
    (u32)&diamond_1_dl_1_4, (u32)&diamond_1_dl_1_5, (u32)&diamond_1_dl_1_6, (u32)&diamond_1_dl_1_7,
    (u32)&diamond_1_dl_1_8, (u32)&diamond_1_dl_1_9, (u32)&diamond_1_dl_1_10, (u32)&diamond_1_dl_1_11,
    (u32)&diamond_1_dl_1_12, (u32)&diamond_1_dl_1_13, (u32)&diamond_1_dl_1_14, (u32)&diamond_1_dl_1_15,
    (u32)&diamond_1_dl_1_16, (u32)&diamond_1_dl_1_17, (u32)&diamond_1_dl_1_18, (u32)&diamond_1_dl_1_19,
    (u32)&diamond_1_dl_1_20, (u32)&diamond_1_dl_1_21, (u32)&diamond_1_dl_1_22, (u32)&diamond_1_dl_1_23,
    (u32)&diamond_1_dl_1_24, (u32)&diamond_1_dl_1_25, (u32)&diamond_1_dl_1_26, (u32)&diamond_1_dl_1_27,
    (u32)&diamond_1_dl_1_28, (u32)&diamond_1_dl_1_29, (u32)&diamond_1_dl_1_30, (u32)&diamond_1_dl_1_31,
    (u32)&diamond_1_dl_1_32, (u32)&diamond_1_dl_1_33,
};

// TODO: type
u32 diamond_2_2_vertex_tbl[48] ATTRIBUTE_ALIGN(32) = {
    0x00004BA2, 0x0000E32F, 0x24280000, 0xB73B16C3,
    0x0000D15F, 0xED580000, 0xD307C138, 0x00000000,
    0xCB770000, 0x000018FA, 0xE517E594, 0x05C7E517,
    0xEFACE6B8, 0xE5170000, 0x18FA1AE9, 0xE59405C7,
    0x1AE9EFAC, 0xE6B81AE9, 0x0000E6B8, 0x1AE90000,
    0xE6B8E517, 0x48C516C3, 0x00001CD1, 0x24280000,
    0x2CF9C138, 0x00002EA1, 0xED580000, 0x1A6C05C7,
    0xE5171054, 0xE6B8E517, 0x1A6C05C7, 0x1AE91054,
    0xE6B81AE9, 0x00004BA2, 0x00000000, 0x18FA1AE9,
    0x0000E6B8, 0x1AE90000, 0xCB770000, 0x000018FA,
    0xE5170000, 0xE6B8E517, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_normal_tbl[40] = {
    0xE50939E7, 0x232FEA16, 0x37E40A38, 0xEFE939D7,
    0xF42FEDE7, 0x3704D52E, 0xDE192FE5, 0x25D4E608,
    0xC7F214C6, 0xE40AC8F4, 0x29D1D5F2, 0xD3F0EAC7,
    0xEDE7C9D6, 0xFFD109D7, 0xD1DCEAD1, 0xDE19D100,
    0xE83B00D4, 0xD300E8C5, 0x40000016, 0x16371923,
    0x2F1B0939, 0x22192F11, 0xE939FCD5, 0x2E13E737,
    0x29F42F1C, 0x0A381B25, 0xD40E14C6, 0x1A08C722,
    0x19D110EA, 0xC713E7C9, 0xF7D7D12B, 0xF2D31C0A,
    0xC824EAD1, 0x2AFFD10C, 0x29D1C000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_texcoord0_tbl[28] = {
    0x1999F667, 0x2666FCCD, 0x26661333, 0x09990000,
    0x19991000, 0x19991666, 0x0CCC2CCC, 0x3CCC0000,
    0x0CCC0999, 0x40A62286, 0x10D112A6, 0x49AE062D,
    0x1FF536C6, 0xF02226E7, 0xF65239D2, 0x23331666,
    0x19992333, 0x40000CCC, 0x19992000, 0x39991666,
    0x29991333, 0x09991999, 0x15441235, 0x3A58238A,
    0x4541078B, 0xF80D2563, 0x1D2036B8, 0xFABF3874,
};

// TODO: type
u32 diamond_2_2_dl_0_tbl[19] = {
    (u32)&diamond_2_2_dl_0_0, (u32)&diamond_2_2_dl_0_1, (u32)&diamond_2_2_dl_0_2, (u32)&diamond_2_2_dl_0_3,
    (u32)&diamond_2_2_dl_0_4, (u32)&diamond_2_2_dl_0_5, (u32)&diamond_2_2_dl_0_6, (u32)&diamond_2_2_dl_0_7,
    (u32)&diamond_2_2_dl_0_8, (u32)&diamond_2_2_dl_0_9, (u32)&diamond_2_2_dl_0_10, (u32)&diamond_2_2_dl_0_11,
    (u32)&diamond_2_2_dl_0_12, (u32)&diamond_2_2_dl_0_13, (u32)&diamond_2_2_dl_0_14, (u32)&diamond_2_2_dl_0_15,
    (u32)&diamond_2_2_dl_0_16, (u32)&diamond_2_2_dl_0_17, (u32)&diamond_2_2_dl_0_18,
};

// TODO: type
u32 diamond_2_2_dl_1_tbl[19] = {
    (u32)&diamond_2_2_dl_1_0, (u32)&diamond_2_2_dl_1_1, (u32)&diamond_2_2_dl_1_2, (u32)&diamond_2_2_dl_1_3,
    (u32)&diamond_2_2_dl_1_4, (u32)&diamond_2_2_dl_1_5, (u32)&diamond_2_2_dl_1_6, (u32)&diamond_2_2_dl_1_7,
    (u32)&diamond_2_2_dl_1_8, (u32)&diamond_2_2_dl_1_9, (u32)&diamond_2_2_dl_1_10, (u32)&diamond_2_2_dl_1_11,
    (u32)&diamond_2_2_dl_1_12, (u32)&diamond_2_2_dl_1_13, (u32)&diamond_2_2_dl_1_14, (u32)&diamond_2_2_dl_1_15,
    (u32)&diamond_2_2_dl_1_16, (u32)&diamond_2_2_dl_1_17, (u32)&diamond_2_2_dl_1_18,
};

// TODO: type
u32 diamond_7_vertex_tbl[64] ATTRIBUTE_ALIGN(32) = {
    0x47D21677, 0x00001C71, 0x23AF0000, 0x00004AA5,
    0x0000E38F, 0x23AF0000, 0xB82E1677, 0x0000D1FB,
    0xED960000, 0xD39DC209, 0x00000000, 0xCC270000,
    0x2C63C209, 0x00002E05, 0xED960000, 0x1A1405B4,
    0xE5710000, 0x18A7E571, 0xE5EC05B4, 0xE571EFE2,
    0xE70DE571, 0x101EE70D, 0xE5711A14, 0x05B41A8F,
    0x000018A7, 0x1A8FE5EC, 0x05B41A8F, 0xEFE2E70D,
    0x1A8F101E, 0xE70D1A8F, 0x48C516C3, 0x00001CD1,
    0x24280000, 0x00004BA2, 0x0000E32F, 0x24280000,
    0xB73B16C3, 0x0000D15F, 0xED580000, 0xD307C138,
    0x00000000, 0xCB770000, 0x2CF9C138, 0x00002EA1,
    0xED580000, 0x1A6C05C7, 0xE5170000, 0x18FAE517,
    0xE59405C7, 0xE517EFAC, 0xE6B8E517, 0x1054E6B8,
    0xE5171A6C, 0x05C71AE9, 0x000018FA, 0x1AE9E594,
    0x05C71AE9, 0xEFACE6B8, 0x1AE91054, 0xE6B81AE9,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_normal_tbl[32] = {
    0x001C3919, 0x232F1B09, 0x39001E38, 0xE50939E7,
    0x232FE40A, 0x38EFE939, 0xD7F42FED, 0xE73711E9,
    0x3900D62F, 0x13E73729, 0xF42F1C0A, 0x381B25D4,
    0x001CC71A, 0x08C7001E, 0xC82219D1, 0xE525D4E6,
    0x08C7E40A, 0xC8F429D1, 0xD5F2D3F0, 0xEAC7EDE7,
    0xC9D6FFD1, 0x00D4D310, 0xEAC713E7, 0xC9F7D7D1,
    0x2BF2D31C, 0x0AC824EA, 0xD12AFFD1, 0x09D7D1DC,
    0xEAD1DE19, 0xD10C29D1, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_texcoord0_tbl[72] = {
    0x11990B33, 0x0CCC1199, 0x0CCCFB34, 0x1333FE67,
    0x13330999, 0x04CC0000, 0x066604CC, 0x20000666,
    0x0CCC1000, 0x1CCC0B33, 0x1E660000, 0x14CC0999,
    0x0CCC0800, 0x0CCC0B33, 0x06661666, 0x04CC0CCC,
    0x2C8901B2, 0x3B8C1136, 0x2DE62400, 0x3865F9F6,
    0x40C80210, 0x38FF0D5C, 0x24A30E33, 0x32D2FA88,
    0x36A11A5F, 0x3B822000, 0x247D0E1C, 0x3A510000,
    0x3AD7F6B2, 0x44630698, 0x3AF01856, 0x2FCF1143,
    0x1B7610F5, 0x2A42FCCE, 0x2C1727EC, 0x21B707BC,
    0x36380649, 0x2E220000, 0x36D02000, 0x292F1741,
    0x2CCC1000, 0x20000D1E, 0x40000000, 0x2DF32000,
    0x2C052000, 0x29900C39, 0x30000000, 0x366F0C39,
    0x33FA2000, 0x3D2CF16D, 0x3B00072C, 0x3068F78A,
    0x2D61FFD5, 0x3B160B24, 0x2C39280B, 0x270F00DF,
    0x3C2F0751, 0x299026E7, 0x20002000, 0x300018EF,
    0x2DC01BE3, 0x293F102E, 0x30820043, 0x3865F9F6,
    0x33F81AF0, 0x3D2DF16D, 0x40002000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u8 diamond_7_color0_tbl[76] = { 0xCA, 0xDA, 0xEB, 0xFF, 0xB4, 0xD0, 0xE4, 0xFF, 0xD9, 0xEC, 0xF5, 0xFF, 0x8B, 0xA5, 0xBF, 0xFF, 0xE2, 0xF6, 0xFF, 0xFF, 0xD2, 0xE6, 0xF3, 0x66, 0xCA, 0xE0, 0xEF, 0x66, 0xCE, 0xE3, 0xF1, 0x66, 0xCB, 0xE3, 0xF1, 0x66, 0xC1, 0xD6, 0xE6, 0x66, 0xC2, 0xD8, 0xE6, 0x66, 0xA0, 0xBA, 0xD2, 0x66, 0xBD, 0xD2, 0xE4, 0x66, 0xAD, 0xC5, 0xDA, 0x66, 0xBB, 0xD3, 0xE6, 0x66, 0xD2, 0xE7, 0xF3, 0x66, 0xCF, 0xE3, 0xF1, 0x66, 0xC1, 0xD7, 0xE7, 0x66, 0xBE, 0xD3, 0xE5, 0x66 };

// TODO: type
u32 diamond_7_dl_0_tbl[34] = {
    (u32)&diamond_7_dl_0_0, (u32)&diamond_7_dl_0_1, (u32)&diamond_7_dl_0_2, (u32)&diamond_7_dl_0_3,
    (u32)&diamond_7_dl_0_4, (u32)&diamond_7_dl_0_5, (u32)&diamond_7_dl_0_6, (u32)&diamond_7_dl_0_7,
    (u32)&diamond_7_dl_0_8, (u32)&diamond_7_dl_0_9, (u32)&diamond_7_dl_0_10, (u32)&diamond_7_dl_0_11,
    (u32)&diamond_7_dl_0_12, (u32)&diamond_7_dl_0_13, (u32)&diamond_7_dl_0_14, (u32)&diamond_7_dl_0_15,
    (u32)&diamond_7_dl_0_16, (u32)&diamond_7_dl_0_17, (u32)&diamond_7_dl_0_18, (u32)&diamond_7_dl_0_19,
    (u32)&diamond_7_dl_0_20, (u32)&diamond_7_dl_0_21, (u32)&diamond_7_dl_0_22, (u32)&diamond_7_dl_0_23,
    (u32)&diamond_7_dl_0_24, (u32)&diamond_7_dl_0_25, (u32)&diamond_7_dl_0_26, (u32)&diamond_7_dl_0_27,
    (u32)&diamond_7_dl_0_28, (u32)&diamond_7_dl_0_29, (u32)&diamond_7_dl_0_30, (u32)&diamond_7_dl_0_31,
    (u32)&diamond_7_dl_0_32, (u32)&diamond_7_dl_0_33,
};

// TODO: type
u32 diamond_7_dl_1_tbl[33] = {
    (u32)&diamond_7_dl_1_0, (u32)&diamond_7_dl_1_1, (u32)&diamond_7_dl_1_2, (u32)&diamond_7_dl_1_3,
    (u32)&diamond_7_dl_1_4, (u32)&diamond_7_dl_1_5, (u32)&diamond_7_dl_1_6, (u32)&diamond_7_dl_1_7,
    (u32)&diamond_7_dl_1_8, (u32)&diamond_7_dl_1_9, (u32)&diamond_7_dl_1_10, (u32)&diamond_7_dl_1_11,
    (u32)&diamond_7_dl_1_12, (u32)&diamond_7_dl_1_13, (u32)&diamond_7_dl_1_14, (u32)&diamond_7_dl_1_15,
    (u32)&diamond_7_dl_1_16, (u32)&diamond_7_dl_1_17, (u32)&diamond_7_dl_1_18, (u32)&diamond_7_dl_1_19,
    (u32)&diamond_7_dl_1_20, (u32)&diamond_7_dl_1_21, (u32)&diamond_7_dl_1_22, (u32)&diamond_7_dl_1_23,
    (u32)&diamond_7_dl_1_24, (u32)&diamond_7_dl_1_25, (u32)&diamond_7_dl_1_26, (u32)&diamond_7_dl_1_27,
    (u32)&diamond_7_dl_1_28, (u32)&diamond_7_dl_1_29, (u32)&diamond_7_dl_1_30, (u32)&diamond_7_dl_1_31,
    (u32)&diamond_7_dl_1_32,
};

// TODO: type
u32 diamond_1_dl_0_0[8] ATTRIBUTE_ALIGN(32) = {
    0x9800040D, 0x19040A0E, 0x1D040A0C, 0x15040A0B,
    0x10040A00, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_1[8] ATTRIBUTE_ALIGN(32) = {
    0x9800040F, 0x02020A13, 0x0A020A10, 0x00020A11,
    0x04020A00, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_2[8] ATTRIBUTE_ALIGN(32) = {
    0x98000311, 0x04020203, 0x05020210, 0x00020200,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_3[8] ATTRIBUTE_ALIGN(32) = {
    0x98000311, 0x04020304, 0x06020303, 0x05020300,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_4[8] ATTRIBUTE_ALIGN(32) = {
    0x98000312, 0x07000405, 0x08000411, 0x04000400,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_5[8] ATTRIBUTE_ALIGN(32) = {
    0x98000312, 0x07020506, 0x09020505, 0x08020500,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_6[8] ATTRIBUTE_ALIGN(32) = {
    0x98000313, 0x0A000607, 0x0B000612, 0x07000600,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_7[8] ATTRIBUTE_ALIGN(32) = {
    0x98000313, 0x0A030708, 0x0C030707, 0x0B030700,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_8[8] ATTRIBUTE_ALIGN(32) = {
    0x98000313, 0x0A00080F, 0x02000809, 0x0D000800,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_9[8] ATTRIBUTE_ALIGN(32) = {
    0x9800030F, 0x02010900, 0x0E010909, 0x0D010900,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_10[8] ATTRIBUTE_ALIGN(32) = {
    0x98000313, 0x0A010B09, 0x0D010B08, 0x0C010B00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_11[8] ATTRIBUTE_ALIGN(32) = {
    0x98000312, 0x07010C07, 0x0B010C06, 0x09010C00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_12[8] ATTRIBUTE_ALIGN(32) = {
    0x98000311, 0x04030D05, 0x08030D04, 0x06030D00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_13[8] ATTRIBUTE_ALIGN(32) = {
    0x98000310, 0x00020E03, 0x05020E02, 0x03020E00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_14[8] ATTRIBUTE_ALIGN(32) = {
    0x9800030F, 0x02020F01, 0x01020F00, 0x0E020F00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_15[8] ATTRIBUTE_ALIGN(32) = {
    0x98000312, 0x07020A11, 0x04020A13, 0x0A020A00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_16[8] ATTRIBUTE_ALIGN(32) = {
    0x9800030A, 0x11040A0B, 0x10040A0E, 0x1D040A00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_17[8] ATTRIBUTE_ALIGN(32) = {
    0x98000301, 0x27040F0A, 0x21040F00, 0x21040F00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_18[8] ATTRIBUTE_ALIGN(32) = {
    0x98000303, 0x26020E0B, 0x12020E02, 0x12020E00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_19[8] ATTRIBUTE_ALIGN(32) = {
    0x98000305, 0x25030D0C, 0x16030D04, 0x16030D00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_20[8] ATTRIBUTE_ALIGN(32) = {
    0x98000307, 0x24010C0D, 0x1A010C06, 0x1A010C00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_21[8] ATTRIBUTE_ALIGN(32) = {
    0x98000309, 0x23010B0E, 0x1E010B08, 0x1E010B00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_22[8] ATTRIBUTE_ALIGN(32) = {
    0x98000300, 0x2101090A, 0x21010909, 0x22010900,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_23[8] ATTRIBUTE_ALIGN(32) = {
    0x9800030A, 0x1100080E, 0x1D000809, 0x20000800,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_24[8] ATTRIBUTE_ALIGN(32) = {
    0x98000308, 0x1E03070E, 0x1E030707, 0x1F030700,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_25[8] ATTRIBUTE_ALIGN(32) = {
    0x98000307, 0x1C00060E, 0x1D00060D, 0x19000600,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_26[8] ATTRIBUTE_ALIGN(32) = {
    0x98000306, 0x1A04050D, 0x1A040505, 0x1B040500,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_27[8] ATTRIBUTE_ALIGN(32) = {
    0x98000305, 0x1800040D, 0x1900040C, 0x15000400,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_28[8] ATTRIBUTE_ALIGN(32) = {
    0x98000304, 0x1602030C, 0x16020303, 0x17020300,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_29[8] ATTRIBUTE_ALIGN(32) = {
    0x98000303, 0x1402020C, 0x1502020B, 0x10020200,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_30[8] ATTRIBUTE_ALIGN(32) = {
    0x98000302, 0x1201010B, 0x12010101, 0x13010100,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_31[8] ATTRIBUTE_ALIGN(32) = {
    0x98000301, 0x0F00000B, 0x1000000A, 0x11000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_32[8] ATTRIBUTE_ALIGN(32) = {
    0x98000310, 0x00000001, 0x0100000F, 0x02000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_0_33[8] ATTRIBUTE_ALIGN(32) = {
    0x98000310, 0x00010102, 0x03010101, 0x01010100,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
u8 diamond_1_dl_0_size_tbl[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
};

// TODO: type
u32 diamond_1_dl_1_0[8] ATTRIBUTE_ALIGN(32) = {
    0x98000421, 0x19071E22, 0x1D0D1E20, 0x15091E1F,
    0x10051E00, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_1[8] ATTRIBUTE_ALIGN(32) = {
    0x98000423, 0x02071E27, 0x0A0D1E24, 0x00051E25,
    0x04091E00, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_2[8] ATTRIBUTE_ALIGN(32) = {
    0x98000325, 0x04091217, 0x050A1224, 0x00051200,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_3[8] ATTRIBUTE_ALIGN(32) = {
    0x98000325, 0x04091318, 0x060B1317, 0x050A1300,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_4[8] ATTRIBUTE_ALIGN(32) = {
    0x98000326, 0x07071419, 0x080C1525, 0x04091600,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_5[8] ATTRIBUTE_ALIGN(32) = {
    0x98000326, 0x0707171A, 0x09081819, 0x08081900,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_6[8] ATTRIBUTE_ALIGN(32) = {
    0x98000327, 0x0A0D1A1B, 0x0B0E1A26, 0x07071A00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_7[8] ATTRIBUTE_ALIGN(32) = {
    0x98000327, 0x0A0D1B1C, 0x0C0F1B1B, 0x0B0E1B00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_8[8] ATTRIBUTE_ALIGN(32) = {
    0x98000327, 0x0A0D1C23, 0x02071C1D, 0x0D101C00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_9[8] ATTRIBUTE_ALIGN(32) = {
    0x98000323, 0x02071D14, 0x0E081D1D, 0x0D101D00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_10[8] ATTRIBUTE_ALIGN(32) = {
    0x98000327, 0x0A0D1F1D, 0x0D101F1C, 0x0C0F1F00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_11[8] ATTRIBUTE_ALIGN(32) = {
    0x98000326, 0x0707201B, 0x0B0E201A, 0x09082000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_12[8] ATTRIBUTE_ALIGN(32) = {
    0x98000325, 0x04092119, 0x080C2118, 0x060B2100,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_13[8] ATTRIBUTE_ALIGN(32) = {
    0x98000324, 0x00052217, 0x050A2216, 0x03082200,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_14[8] ATTRIBUTE_ALIGN(32) = {
    0x98000323, 0x02072315, 0x01062314, 0x0E082300,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_15[8] ATTRIBUTE_ALIGN(32) = {
    0x98000326, 0x07071E25, 0x04091E27, 0x0A0D1E00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_16[8] ATTRIBUTE_ALIGN(32) = {
    0x9800031E, 0x11071E1F, 0x10051E22, 0x1D0D1E00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_17[8] ATTRIBUTE_ALIGN(32) = {
    0x98000315, 0x2706231E, 0x21072314, 0x21082300,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_18[8] ATTRIBUTE_ALIGN(32) = {
    0x98000317, 0x260A221F, 0x12052216, 0x12082200,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_19[8] ATTRIBUTE_ALIGN(32) = {
    0x98000319, 0x250C2120, 0x16092118, 0x160B2100,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_20[8] ATTRIBUTE_ALIGN(32) = {
    0x9800031B, 0x240E2021, 0x1A07201A, 0x1A082000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_21[8] ATTRIBUTE_ALIGN(32) = {
    0x9800031D, 0x23101F22, 0x1E0D1F1C, 0x1E0F1F00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_22[8] ATTRIBUTE_ALIGN(32) = {
    0x98000314, 0x21081D1E, 0x21071D1D, 0x22101D00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_23[8] ATTRIBUTE_ALIGN(32) = {
    0x9800031E, 0x11071C22, 0x1D0D1C1D, 0x20101C00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_24[8] ATTRIBUTE_ALIGN(32) = {
    0x9800031C, 0x1E0F1B22, 0x1E0D1B1B, 0x1F0E1B00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_25[8] ATTRIBUTE_ALIGN(32) = {
    0x9800031B, 0x1C0E1A22, 0x1D0D1A21, 0x19071A00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_26[8] ATTRIBUTE_ALIGN(32) = {
    0x9800031A, 0x1A081821, 0x1A071719, 0x1B0C1900,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_27[8] ATTRIBUTE_ALIGN(32) = {
    0x98000319, 0x180C1521, 0x19071420, 0x15091600,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_28[8] ATTRIBUTE_ALIGN(32) = {
    0x98000318, 0x160B1320, 0x16091317, 0x170A1300,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_29[8] ATTRIBUTE_ALIGN(32) = {
    0x98000317, 0x140A1220, 0x1509121F, 0x10051200,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_30[8] ATTRIBUTE_ALIGN(32) = {
    0x98000316, 0x1208111F, 0x12051115, 0x13061100,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_31[8] ATTRIBUTE_ALIGN(32) = {
    0x98000315, 0x0F06101F, 0x1005101E, 0x11071000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_32[8] ATTRIBUTE_ALIGN(32) = {
    0x98000324, 0x00051015, 0x01061023, 0x02071000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_1_dl_1_33[8] ATTRIBUTE_ALIGN(32) = {
    0x98000324, 0x00051116, 0x03081115, 0x01061100,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
u8 diamond_1_dl_1_size_tbl[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
};

// TODO: type
u32 diamond_2_2_dl_0_0[8] ATTRIBUTE_ALIGN(32) = {
    0x98000605, 0x18040E0C, 0x18040D0D, 0x18040C09,
    0x18040A06, 0x18040900, 0x18040B00, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_0_1[8] ATTRIBUTE_ALIGN(32) = {
    0x9800040D, 0x17000706, 0x0B000708, 0x0F000707,
    0x0A000700, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_0_2[8] ATTRIBUTE_ALIGN(32) = {
    0x98000409, 0x0200070C, 0x1500070A, 0x0000070B,
    0x04000700, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_0_3[8] ATTRIBUTE_ALIGN(32) = {
    0x98000301, 0x14000606, 0x14000600, 0x14000600,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_0_4[8] ATTRIBUTE_ALIGN(32) = {
    0x98000303, 0x13030507, 0x0C030502, 0x0C030500,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_0_5[8] ATTRIBUTE_ALIGN(32) = {
    0x98000305, 0x12030408, 0x10030404, 0x10030400,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_0_6[8] ATTRIBUTE_ALIGN(32) = {
    0x98000304, 0x10000308, 0x10000303, 0x11000300,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_0_7[8] ATTRIBUTE_ALIGN(32) = {
    0x98000303, 0x0E020208, 0x0F020207, 0x0A020200,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_0_8[8] ATTRIBUTE_ALIGN(32) = {
    0x98000302, 0x0C010107, 0x0C010101, 0x0D010100,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_0_9[8] ATTRIBUTE_ALIGN(32) = {
    0x98000301, 0x09000007, 0x0A000006, 0x0B000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_0_10[8] ATTRIBUTE_ALIGN(32) = {
    0x98000309, 0x02000601, 0x01000600, 0x08000600,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_0_11[8] ATTRIBUTE_ALIGN(32) = {
    0x9800030A, 0x00030503, 0x05030502, 0x03030500,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_0_12[8] ATTRIBUTE_ALIGN(32) = {
    0x9800030B, 0x04030405, 0x07030404, 0x06030400,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_0_13[8] ATTRIBUTE_ALIGN(32) = {
    0x9800030B, 0x04000304, 0x06000303, 0x05000300,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_0_14[8] ATTRIBUTE_ALIGN(32) = {
    0x9800030B, 0x04020203, 0x0502020A, 0x00020200,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_0_15[8] ATTRIBUTE_ALIGN(32) = {
    0x9800030A, 0x00010102, 0x03010101, 0x01010100,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_0_16[8] ATTRIBUTE_ALIGN(32) = {
    0x9800030A, 0x00000001, 0x01000009, 0x02000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_0_17[8] ATTRIBUTE_ALIGN(32) = {
    0x9800030C, 0x15020805, 0x0702080B, 0x04020800,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_0_18[8] ATTRIBUTE_ALIGN(32) = {
    0x98000305, 0x1602080D, 0x17020808, 0x0F020800,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
u8 diamond_2_2_dl_0_size_tbl[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
};

// TODO: type
u32 diamond_2_2_dl_1_0[8] ATTRIBUTE_ALIGN(32) = {
    0x98000619, 0x2E051B1B, 0x2E051A18, 0x2E05191A,
    0x2E051717, 0x2E051616, 0x2E051800, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_1_1[8] ATTRIBUTE_ALIGN(32) = {
    0x9800041B, 0x17000713, 0x2600071A, 0x23000712,
    0x24000700, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_1_2[8] ATTRIBUTE_ALIGN(32) = {
    0x98000414, 0x1B000715, 0x1D000717, 0x19000718,
    0x15000700, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_1_3[8] ATTRIBUTE_ALIGN(32) = {
    0x9800030E, 0x2A031312, 0x2A031311, 0x2B031300,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_1_4[8] ATTRIBUTE_ALIGN(32) = {
    0x98000312, 0x24021213, 0x26021211, 0x29021200,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_1_5[8] ATTRIBUTE_ALIGN(32) = {
    0x98000310, 0x27011113, 0x27011119, 0x28011100,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_1_6[8] ATTRIBUTE_ALIGN(32) = {
    0x98000319, 0x16020813, 0x2602081B, 0x17020800,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_1_7[8] ATTRIBUTE_ALIGN(32) = {
    0x98000316, 0x2503101A, 0x2503100F, 0x25031000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_1_8[8] ATTRIBUTE_ALIGN(32) = {
    0x9800030F, 0x22020F1A, 0x23020F12, 0x24020F00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_1_9[8] ATTRIBUTE_ALIGN(32) = {
    0x98000314, 0x1B00150F, 0x1A00150E, 0x21001500,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_1_10[8] ATTRIBUTE_ALIGN(32) = {
    0x98000315, 0x1D031411, 0x20031410, 0x1F031400,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_1_11[8] ATTRIBUTE_ALIGN(32) = {
    0x98000314, 0x1B03130E, 0x21031311, 0x20031300,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_1_12[8] ATTRIBUTE_ALIGN(32) = {
    0x98000315, 0x1D021214, 0x1B021211, 0x20021200,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_1_13[8] ATTRIBUTE_ALIGN(32) = {
    0x98000315, 0x1D011110, 0x1F011119, 0x1E011100,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_1_14[8] ATTRIBUTE_ALIGN(32) = {
    0x98000315, 0x1D020819, 0x1E020818, 0x15020800,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_1_15[8] ATTRIBUTE_ALIGN(32) = {
    0x98000317, 0x19031016, 0x1C03100F, 0x1A031000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_1_16[8] ATTRIBUTE_ALIGN(32) = {
    0x98000317, 0x19020F0F, 0x1A020F14, 0x1B020F00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_1_17[8] ATTRIBUTE_ALIGN(32) = {
    0x98000311, 0x2C031413, 0x27031410, 0x27031400,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_2_2_dl_1_18[8] ATTRIBUTE_ALIGN(32) = {
    0x9800030F, 0x2D001512, 0x2A00150E, 0x2A001500,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
u8 diamond_2_2_dl_1_size_tbl[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
};

// TODO: type
u32 diamond_7_dl_0_0[8] ATTRIBUTE_ALIGN(32) = {
    0x9800040D, 0x19040A0E, 0x1D040A0C, 0x15040A0B,
    0x10040A00, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_1[8] ATTRIBUTE_ALIGN(32) = {
    0x9800040F, 0x02040A13, 0x0A040A10, 0x00040A11,
    0x04040A00, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_2[8] ATTRIBUTE_ALIGN(32) = {
    0x98000311, 0x04020203, 0x05020210, 0x00020200,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_3[8] ATTRIBUTE_ALIGN(32) = {
    0x98000311, 0x04030304, 0x06030303, 0x05030300,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_4[8] ATTRIBUTE_ALIGN(32) = {
    0x98000312, 0x07000405, 0x08000411, 0x04000400,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_5[8] ATTRIBUTE_ALIGN(32) = {
    0x98000312, 0x07040506, 0x09040505, 0x08040500,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_6[8] ATTRIBUTE_ALIGN(32) = {
    0x98000313, 0x0A000607, 0x0B000612, 0x07000600,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_7[8] ATTRIBUTE_ALIGN(32) = {
    0x98000313, 0x0A030708, 0x0C030707, 0x0B030700,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_8[8] ATTRIBUTE_ALIGN(32) = {
    0x98000313, 0x0A00080F, 0x02000809, 0x0D000800,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_9[8] ATTRIBUTE_ALIGN(32) = {
    0x9800030F, 0x02010900, 0x0E010909, 0x0D010900,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_10[8] ATTRIBUTE_ALIGN(32) = {
    0x98000313, 0x0A010B09, 0x0D010B08, 0x0C010B00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_11[8] ATTRIBUTE_ALIGN(32) = {
    0x98000312, 0x07010C07, 0x0B010C06, 0x09010C00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_12[8] ATTRIBUTE_ALIGN(32) = {
    0x98000311, 0x04010D05, 0x08010D04, 0x06010D00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_13[8] ATTRIBUTE_ALIGN(32) = {
    0x98000310, 0x00040E03, 0x05040E02, 0x03040E00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_14[8] ATTRIBUTE_ALIGN(32) = {
    0x9800030F, 0x02040F01, 0x01040F00, 0x0E040F00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_15[8] ATTRIBUTE_ALIGN(32) = {
    0x98000312, 0x07040A11, 0x04040A13, 0x0A040A00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_16[8] ATTRIBUTE_ALIGN(32) = {
    0x9800030A, 0x11040A0B, 0x10040A0E, 0x1D040A00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_17[8] ATTRIBUTE_ALIGN(32) = {
    0x98000301, 0x27040F0A, 0x21040F00, 0x21040F00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_18[8] ATTRIBUTE_ALIGN(32) = {
    0x98000303, 0x26040E0B, 0x12040E02, 0x12040E00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_19[8] ATTRIBUTE_ALIGN(32) = {
    0x98000305, 0x25010D0C, 0x16010D04, 0x16010D00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_20[8] ATTRIBUTE_ALIGN(32) = {
    0x98000307, 0x24010C0D, 0x1A010C06, 0x1A010C00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_21[8] ATTRIBUTE_ALIGN(32) = {
    0x98000309, 0x23010B0E, 0x1E010B08, 0x1E010B00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_22[8] ATTRIBUTE_ALIGN(32) = {
    0x98000300, 0x2101090A, 0x21010909, 0x22010900,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_23[8] ATTRIBUTE_ALIGN(32) = {
    0x9800030A, 0x1100080E, 0x1D000809, 0x20000800,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_24[8] ATTRIBUTE_ALIGN(32) = {
    0x98000308, 0x1E03070E, 0x1E030707, 0x1F030700,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_25[8] ATTRIBUTE_ALIGN(32) = {
    0x98000307, 0x1C00060E, 0x1D00060D, 0x19000600,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_26[8] ATTRIBUTE_ALIGN(32) = {
    0x98000306, 0x1A04050D, 0x1A040505, 0x1B040500,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_27[8] ATTRIBUTE_ALIGN(32) = {
    0x98000305, 0x1804040D, 0x1904040C, 0x15040400,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_28[8] ATTRIBUTE_ALIGN(32) = {
    0x98000304, 0x1603030C, 0x16030303, 0x17030300,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_29[8] ATTRIBUTE_ALIGN(32) = {
    0x98000303, 0x1404020C, 0x1504020B, 0x10040200,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_30[8] ATTRIBUTE_ALIGN(32) = {
    0x98000302, 0x1201010B, 0x12010101, 0x13010100,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_31[8] ATTRIBUTE_ALIGN(32) = {
    0x98000301, 0x0F00000B, 0x1000000A, 0x11000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_32[8] ATTRIBUTE_ALIGN(32) = {
    0x98000310, 0x00000001, 0x0100000F, 0x02000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_0_33[8] ATTRIBUTE_ALIGN(32) = {
    0x98000310, 0x00010102, 0x03010101, 0x01010100,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
u8 diamond_7_dl_0_size_tbl[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
};

// TODO: type
u32 diamond_7_dl_1_0[8] ATTRIBUTE_ALIGN(32) = {
    0x98000415, 0x27063D1E, 0x21103C14, 0x21083E1D,
    0x220E4000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_1[8] ATTRIBUTE_ALIGN(32) = {
    0x98000421, 0x19102C22, 0x1D123020, 0x15112D1F,
    0x100F2E00, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_2[8] ATTRIBUTE_ALIGN(32) = {
    0x98000423, 0x02072F27, 0x0A0C3024, 0x00052E25,
    0x04092D00, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_3[8] ATTRIBUTE_ALIGN(32) = {
    0x98000325, 0x04091918, 0x060B1A17, 0x050A1B00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_4[8] ATTRIBUTE_ALIGN(32) = {
    0x98000326, 0x07071C19, 0x08061D25, 0x04091E00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_5[8] ATTRIBUTE_ALIGN(32) = {
    0x98000326, 0x07071F1A, 0x09082019, 0x08082100,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_6[8] ATTRIBUTE_ALIGN(32) = {
    0x98000327, 0x0A0C221B, 0x0B0D2326, 0x07072400,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_7[8] ATTRIBUTE_ALIGN(32) = {
    0x98000327, 0x0A0C251C, 0x0C0B261B, 0x0B0D2700,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_8[8] ATTRIBUTE_ALIGN(32) = {
    0x98000327, 0x0A0C2823, 0x0207281D, 0x0D0E2800,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_9[8] ATTRIBUTE_ALIGN(32) = {
    0x98000323, 0x02072914, 0x0E082A1D, 0x0D0E2B00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_10[8] ATTRIBUTE_ALIGN(32) = {
    0x98000327, 0x0A0C311D, 0x0D0E321C, 0x0C0B3300,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_11[8] ATTRIBUTE_ALIGN(32) = {
    0x98000326, 0x0707341B, 0x0B0D351A, 0x09083600,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_12[8] ATTRIBUTE_ALIGN(32) = {
    0x98000325, 0x04093719, 0x08063818, 0x060B3900,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_13[8] ATTRIBUTE_ALIGN(32) = {
    0x98000324, 0x00053A17, 0x050A3B16, 0x03082E00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_14[8] ATTRIBUTE_ALIGN(32) = {
    0x98000323, 0x02073C15, 0x01063D14, 0x0E083E00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_15[8] ATTRIBUTE_ALIGN(32) = {
    0x98000326, 0x07072C25, 0x04092D27, 0x0A0C3000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_16[8] ATTRIBUTE_ALIGN(32) = {
    0x9800031E, 0x11102F1F, 0x100F2E22, 0x1D123000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_17[8] ATTRIBUTE_ALIGN(32) = {
    0x98000325, 0x04091617, 0x050A1724, 0x00051800,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_18[8] ATTRIBUTE_ALIGN(32) = {
    0x98000324, 0x00051316, 0x03081415, 0x01061500,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_19[8] ATTRIBUTE_ALIGN(32) = {
    0x98000324, 0x00051015, 0x01061123, 0x02071200,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_20[8] ATTRIBUTE_ALIGN(32) = {
    0x98000315, 0x0F06111F, 0x100F101E, 0x11101200,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_21[8] ATTRIBUTE_ALIGN(32) = {
    0x98000316, 0x1208141F, 0x120F3F15, 0x13061500,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_22[8] ATTRIBUTE_ALIGN(32) = {
    0x98000317, 0x140A1720, 0x1511161F, 0x100F1800,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_23[8] ATTRIBUTE_ALIGN(32) = {
    0x98000318, 0x160B1A20, 0x16111917, 0x170A1B00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_24[8] ATTRIBUTE_ALIGN(32) = {
    0x98000319, 0x18061D21, 0x19101C20, 0x15111E00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_25[8] ATTRIBUTE_ALIGN(32) = {
    0x9800031A, 0x1A082021, 0x1A101F19, 0x1B062100,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_26[8] ATTRIBUTE_ALIGN(32) = {
    0x9800031B, 0x1C0D2322, 0x1D122221, 0x19102400,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_27[8] ATTRIBUTE_ALIGN(32) = {
    0x9800031C, 0x1E0B2622, 0x1E12251B, 0x1F0D2700,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_28[8] ATTRIBUTE_ALIGN(32) = {
    0x9800031E, 0x11102822, 0x1D12281D, 0x200E2800,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_29[8] ATTRIBUTE_ALIGN(32) = {
    0x9800031D, 0x230E3222, 0x1E12411C, 0x1E0B3300,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_30[8] ATTRIBUTE_ALIGN(32) = {
    0x9800031B, 0x240D3521, 0x1A10341A, 0x1A083600,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_31[8] ATTRIBUTE_ALIGN(32) = {
    0x98000319, 0x25063820, 0x16113718, 0x160B3900,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 diamond_7_dl_1_32[8] ATTRIBUTE_ALIGN(32) = {
    0x98000317, 0x260A3B1F, 0x120F4216, 0x12082E00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
u8 diamond_7_dl_1_size_tbl[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
};

// TODO: type
u8 diamond_2_color0_tbl[32] ATTRIBUTE_ALIGN(32) = { 0x4B, 0xA5, 0x4F, 0xFF, 0x44, 0x95, 0x47, 0xFF, 0x53, 0xB7, 0x57, 0xFF, 0x3F, 0x8B, 0x42, 0xFF, 0x4B, 0xA5, 0x4F, 0xFF, 0x44, 0x95, 0x47, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// TODO: type
u8 diamond_3_color0_tbl[32] = { 0xCC, 0x84, 0x11, 0xFF, 0xBF, 0x7B, 0x10, 0xFF, 0xE1, 0xAF, 0x13, 0xFF, 0xCA, 0x83, 0x11, 0xFF, 0xE1, 0xC6, 0x11, 0xFF, 0xCC, 0x84, 0x11, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// TODO: type
u8 diamond_4_color0_tbl[32] = { 0xD7, 0x18, 0x35, 0xFF, 0x9F, 0x00, 0x2D, 0xFF, 0xF5, 0x5F, 0x84, 0xFF, 0xC2, 0x00, 0x33, 0xFF, 0xD7, 0x18, 0x35, 0xFF, 0x9F, 0x00, 0x2D, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// TODO: type
u8 diamond_5_color0_tbl[32] = { 0x4D, 0x71, 0xB4, 0xFF, 0x3B, 0x63, 0x9A, 0xFF, 0x6E, 0x8F, 0xC4, 0xFF, 0x43, 0x45, 0x88, 0xFF, 0x4D, 0x71, 0xB4, 0xFF, 0x3B, 0x63, 0x9A, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

// TODO: type
u8 diamond_6_color0_tbl[24] = { 0xF0, 0x76, 0x19, 0xFF, 0x7E, 0x2D, 0x15, 0xFF, 0xB5, 0x41, 0x1E, 0xFF, 0x98, 0x36, 0x19, 0xFF, 0xD1, 0x67, 0x16, 0xFF, 0xF0, 0x8A, 0x08, 0xFF };
static u8* color_tbl[7] = {
	diamond_1_color0_tbl,
	diamond_2_color0_tbl,
	diamond_3_color0_tbl,
	diamond_4_color0_tbl,
	diamond_5_color0_tbl,
	diamond_6_color0_tbl,
	diamond_7_color0_tbl,
};
// clang-format on

Vec edge_color[7] = {
	{ 0.3310000002384186, 0.3149999976158142, 0.23199999332427979 },
	{ 0.1589999943971634, 0.26100000739097595, 0.16200000047683716 },
	{ 0.28700000047683716, 0.2409999966621399, 0.09300000220537186 },
	{ 0.32499998807907104, 0.13199999928474426, 0.14100000262260437 },
	{ 0.14499999582767487, 0.2029999941587448, 0.3059999942779541 },
	{ 0.31200000643730164, 0.18199999630451202, 0.11500000208616257 },
	{ 0.25600001215934753, 0.31299999356269836, 0.3440000116825104 },
};

#pragma explicit_zero_data on
__declspec(section ".data") u32 gap_05_803ACB0C_data = 0;
__declspec(section ".sdata") u32 gap_07_804159B4_sdata = 0;
#pragma explicit_zero_data reset
#pragma force_active reset
