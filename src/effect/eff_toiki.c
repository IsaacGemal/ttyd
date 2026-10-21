#include "eff/eff_toiki.h"
#include "driver/dispdrv.h"
#include "eff/eff_mist.h"
#include "memory.h"
#include "math_ext.h"
#include "evt/evt_cmd.h"

// .sbss2: the default .sbss2 is only used for compiler-generated pool templates, so pair it with a small-data section.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_80423338;


// First object of this unit's .data (the float pair that used to end the previous unit's .data).
extern u32 lbl_803B2FA0[2];

const Vec vec3_80300230 = { 0.0f, 0.0f, 0.0f };


extern u32 queen_toiki_normal_tbl[8];
extern u32 queen_toiki_color0_tbl[8];
extern u8 queen_toiki_dl_0_size_tbl[16];

static GXColor color_tbl[3];

extern s32 negone_one[];
extern s32 one_negone[];
u32 queen_toiki_dl_0_3[8];
u32 queen_toiki_dl_0_4[8];
u32 queen_toiki_dl_0_5[8];
u32 queen_toiki_dl_0_6[8];
u32 queen_toiki_dl_0_7[8];
u32 queen_toiki_dl_0_8[8];
u32 queen_toiki_dl_0_9[8];
u32 queen_toiki_dl_0_10[8];
u32 queen_toiki_dl_0_11[8];
u32 queen_toiki_dl_0_12[8];
u32 queen_toiki_dl_0_13[8];

// clang-format off

// Building the colour inside an inline that also issues GXSetTevColor makes MWCC allocate the
// temp and the by-value argument copy together in retail's stack slots (0x8/0xc). The parameter order is
// deliberate: it also lines up the field evaluation so r/g/b/a land in the original's registers.
static inline void toikiSetColor(u8 a, u8 b, u8 g, u8 r) {
	GXColor c = unk_80423338;
	c.r = r;
	c.g = g;
	c.b = b;
	c.a = a;
	GXSetTevColor(1, c);
}

// With the loop inlined at its call site MWCC colours dl/i/sz r28/r29/r30; splitting it into
// this inline function gives the original's r29/r30/r31. The size table and the increment order are also
// part of the colouring: `sz` must be initialised in the for-init and incremented before `dl`.
static inline void toikiDrawDL(u8* base) {
	u8** dl = (u8**)(base + 0x360);
	u8* sz;
	u32 j;

	for (j = 0, sz = queen_toiki_dl_0_size_tbl; j < 0xe; j++, sz++, dl++) {
		GXCallDisplayList(*dl, *sz << 5);
	}
}

EffectEntry* effToikiEntry(s32 type, s32 life, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	ToikiData* data;
	f32 angle;

	entry = effEntry();
	entry->type = "Toiki";
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x34);
	entry->userdata = data;
	entry->callback = effToikiMain;
	entry->flags |= 2;
	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = 0.5f;
	data->rotation = 0.0f;
	data->life = life;
	data->field_0x2C = 0;
	data->field_0x28 = 255.0f;
	angle = 6.2831855f * (200.0f + data->rotation) / 360.0f;
	data->velX = 2.5f * (f32)cos(angle);
	angle = 6.2831855f * (200.0f + data->rotation) / 360.0f;
	data->velY = 2.5f * (f32)sin(angle);
	data->velZ = 0.0f;
	data->field_0x24 = 0.0f;
	return entry;
}


void effToikiMain(EffectEntry* effect) {
	ToikiData* data = effect->userdata;
	Vec pos;
	Vec mistPos;
	Vec sp;
	Mtx mtx;
	s32 type;
	f32 angle;
	f32 size;

	sp = vec3_80300230;
	sp.x = data->x;
	sp.y = data->y;
	sp.z = data->z;
	pos = sp;
	type = data->type;
	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->life = 0x10;
	}
	if (data->life < 1000) {
		data->life--;
	}
	if (data->life < 0) {
		effDelete(effect);
		return;
	}
	data->x += data->velX;
	data->y += data->velY;
	data->z += data->velZ;
	data->velX *= 0.95f;
	data->velY *= 0.95f;
	data->velZ *= 0.95f;
	if (data->life < 0x10) {
		data->field_0x28 = (f32)(data->life * 16);
	}
	if (data->field_0x24 > 1.0f && data->field_0x24 < 4.0f && data->life % 20 == 0) {
		size = 320.0f * data->scale;
		angle = 6.2831855f * (200.0f + data->rotation) / 360.0f;
		mistPos.x = data->x + size * (f32)cos(angle);
		mistPos.y = data->y + size * (f32)sin(angle);
		mistPos.z = data->z + 1.0f;
		PSMTXRotRad(mtx, 'y', -camGetPtr(CAMERA_3D)->viewYaw * 0.017453292f);
		PSMTXMultVec(mtx, &mistPos, &mistPos);
		effMistEntry(type + 1, 0x3c, mistPos.x, mistPos.y, mistPos.z, 0.0f);
	}
	data->field_0x2C++;
	data->field_0x24 += 0.015f;
	if (data->field_0x24 > 4.0f) {
		data->field_0x24 = 4.0f;
	}
	dispEntry(CAMERA_3D, dispCalcZ(&pos), 2, effToikiDisp, effect);
}

void effToikiDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	ToikiData* data = effect->userdata;
	CameraEntry* cam;
	u8* tbl;
	Mtx mtx;
	Mtx rot;
	Mtx scale;
	GXTexObj texObj;
	s32 type;

	tbl = (u8*)lbl_803B2FA0;
	cam = camGetPtr(camId);
	type = data->type;

	effGetTexObj(0x5c, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
	toikiSetColor(data->field_0x28, color_tbl[type].b, color_tbl[type].g, color_tbl[type].r);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	PSMTXTrans(mtx, data->field_0x24, 0.0f, 0.0f);
	GXLoadTexMtxImm(mtx, GX_TEXMTX0, GX_MTX2x4);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_RASC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_RASA, GX_CA_ZERO);
	PSMTXTrans(mtx, data->x, data->y, data->z);
	PSMTXRotRad(rot, 'y', -camGetPtr(camId)->viewYaw * 0.017453292f);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXRotRad(rot, 'z', 0.017453292f * data->rotation);
	PSMTXConcat(mtx, rot, mtx);
	PSMTXScale(scale, data->scale, data->scale, data->scale);
	PSMTXConcat(mtx, scale, mtx);
	PSMTXConcat(cam->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, 0);
	GXSetCurrentMtx(0);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 6);
	GXSetArray(GX_VA_POS, tbl + 0x40, 6); // queen_toiki_vertex_tbl
	GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
	GXSetArray(GX_VA_NRM, queen_toiki_normal_tbl, 3);
	GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GXSetArray(GX_VA_CLR0, queen_toiki_color0_tbl, 4);
	GXSetVtxDesc(GX_VA_TEX0, GX_INDEX8);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 0xe);
	GXSetArray(GX_VA_TEX0, tbl + 0x1c0, 4); // queen_toiki_texcoord0_tbl
	toikiDrawDL(tbl);
}

// The tables are reached through one base pointer (tbl), so the linker would drop the others.
#pragma force_active on
// TODO: type
__declspec(section ".data") u32 lbl_803B2FA0[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
static __declspec(section ".data") u32 negone_one_374[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_803B2FB0[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
static __declspec(section ".data") u32 one_negone_373[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_803B2FC0[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
static __declspec(section ".data") u32 negone_one_360[2] = {
    0xBF800000, 0x3F800000,
};

// TODO: type
__declspec(section ".data") u32 lbl_803B2FD0[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
static __declspec(section ".data") u32 one_negone_359[2] = {
    0x3F800000, 0xBF800000,
};

// TODO: type
u32 queen_toiki_vertex_tbl[96] = {
    0xAF7AE5B7, 0x0000B05F, 0xE5D60000, 0xB232E751,
    0x0000B208, 0xE7C00000, 0xAFF2E792, 0x0000A99B,
    0xE4DC0000, 0xA89DE3AD, 0x0000A999, 0xE1F90000,
    0xAC11E1E0, 0x0000B0BF, 0xE2EB0000, 0xE7BEF8E7,
    0x0000ACFA, 0xE5350000, 0xB075E497, 0x0000B4D9,
    0xE7330000, 0xB428E987, 0x0000B15A, 0xE9AD0000,
    0xA734E4DD, 0x0000A60A, 0xE2FF0000, 0xA86FE0C6,
    0x0000AC02, 0xE0B60000, 0xB14BE20E, 0x0000E836,
    0xF83E0000, 0xB27EE545, 0x0000B172, 0xE6570000,
    0xAC6CE68D, 0x0000ABBB, 0xE7D90000, 0xA67EE1A9,
    0x0000A8AF, 0xE2BD0000, 0xB51FE8E6, 0x0000DDFD,
    0xF5010000, 0xDE7AF450, 0x0000C99C, 0xE7060000,
    0xCA58E78D, 0x0000CCB0, 0xE7BC0000, 0xCCDDE74F,
    0x0000CB3A, 0xE6010000, 0xC4CDE3CA, 0x0000C31C,
    0xE3FD0000, 0xC305E5DB, 0x0000C4C3, 0xE7AE0000,
    0xC8F3EA11, 0x0000FFF7, 0x00000000, 0xC773E5AC,
    0x0000C98C, 0xE8840000, 0xCE87E9A6, 0x0000CFA1,
    0xE77C0000, 0xCDB3E572, 0x0000C306, 0xE1F20000,
    0xC0E5E27F, 0x0000C11A, 0xE5C20000, 0xC3A6E844,
    0x0000C869, 0xEAEE0000, 0xFF8200A9, 0x0000CB7D,
    0xE96C0000, 0xCB78E7ED, 0x0000C7F9, 0xE4510000,
    0xC85DE2E6, 0x0000C04E, 0xE3C7FFFE, 0xC2A2E4C1,
    0x0000CFE5, 0xE89A0000, 0xF18AFB28, 0x0000F216,
    0xFA780000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 queen_toiki_texcoord0_tbl[48] = {
    0xB3344000, 0xB3340000, 0xB0010000, 0xB0014000,
    0xB99A4000, 0xB99A0000, 0xB6670000, 0xB6674000,
    0xBCCD4000, 0xBCCD0000, 0xC0004000, 0xC0000000,
    0xC3344000, 0xC3340000, 0xC99A4000, 0xC99A0000,
    0xC6670000, 0xC6674000, 0xCCCD4000, 0xCCCD0000,
    0xD0004000, 0xD0000000, 0xD3344000, 0xD3340000,
    0xF0284000, 0xF6674000, 0xF6670000, 0xF02A0000,
    0xD6670000, 0xD6674000, 0xD99A0000, 0xDCCD0000,
    0xDCCD4000, 0xD99A4000, 0xE0000000, 0xE0004000,
    0xE3340000, 0xE3344000, 0xE6670000, 0xE6674000,
    0xF9830000, 0x00000000, 0x00004000, 0xF98B4000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 queen_toiki_dl_0_0[24] = {
    0x9800151D, 0x0000181E, 0x00001B09, 0x00001614,
    0x00001708, 0x00001413, 0x00001507, 0x00001212,
    0x0000131B, 0x0000121A, 0x00001306, 0x00000E11,
    0x00000F05, 0x00001110, 0x00001018, 0x00000C19,
    0x00000D04, 0x00000A0F, 0x00000B03, 0x0000080E,
    0x0000091C, 0x00000900, 0x00000000, 0x00000000,
};

// TODO: type
u32 queen_toiki_dl_0_1[16] = {
    0x98000B2D, 0x01001522, 0x0100143B, 0x01001521,
    0x0100122C, 0x01001336, 0x01000E35, 0x01000F20,
    0x0100112B, 0x0100101F, 0x01000C2A, 0x01000D00,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 queen_toiki_dl_0_2[16] = {
    0x98000A03, 0x0000081C, 0x00000902, 0x0000040D,
    0x00000517, 0x00000716, 0x00000601, 0x0000000C,
    0x00000100, 0x0000030B, 0x00000200, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 queen_toiki_dl_0_tbl[14] = {
    (u32)&queen_toiki_dl_0_0, (u32)&queen_toiki_dl_0_1, (u32)&queen_toiki_dl_0_2, (u32)&queen_toiki_dl_0_3,
    (u32)&queen_toiki_dl_0_4, (u32)&queen_toiki_dl_0_5, (u32)&queen_toiki_dl_0_6, (u32)&queen_toiki_dl_0_7,
    (u32)&queen_toiki_dl_0_8, (u32)&queen_toiki_dl_0_9, (u32)&queen_toiki_dl_0_10, (u32)&queen_toiki_dl_0_11,
    (u32)&queen_toiki_dl_0_12, (u32)&queen_toiki_dl_0_13,
};

// TODO: type
u32 queen_toiki_normal_tbl[8] = {
    0x00004020, 0x24280000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 queen_toiki_color0_tbl[8] = {
    0xFFFFFF7F, 0xFFFFFF00, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 queen_toiki_dl_0_3[8] = {
    0x98000429, 0x01012A3D, 0x01002B34, 0x0101293C,
    0x01002800, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 queen_toiki_dl_0_4[8] = {
    0x9800043D, 0x01002B28, 0x0100273C, 0x01002833,
    0x01002600, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 queen_toiki_dl_0_5[8] = {
    0x98000428, 0x01002727, 0x01002533, 0x01002632,
    0x01002400, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 queen_toiki_dl_0_6[8] = {
    0x98000427, 0x01002526, 0x01002332, 0x01002431,
    0x01002200, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 queen_toiki_dl_0_7[8] = {
    0x98000426, 0x0100233A, 0x01002331, 0x01002239,
    0x01002200, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 queen_toiki_dl_0_8[8] = {
    0x9800043A, 0x01002325, 0x01002039, 0x01002230,
    0x01001F00, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 queen_toiki_dl_0_9[8] = {
    0x98000425, 0x01002024, 0x01002130, 0x01001F2F,
    0x01001E00, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 queen_toiki_dl_0_10[8] = {
    0x98000424, 0x01002137, 0x01001D2F, 0x01001E38,
    0x01001C00, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 queen_toiki_dl_0_11[8] = {
    0x98000437, 0x01001D23, 0x01001638, 0x01001C2E,
    0x01001700, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 queen_toiki_dl_0_12[8] = {
    0x98000423, 0x01001622, 0x0100142E, 0x0100172D,
    0x01001500, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 queen_toiki_dl_0_13[8] = {
    0x98000415, 0x00011A1E, 0x00001B0A, 0x0001191D,
    0x00001800, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u8 queen_toiki_dl_0_size_tbl[16] = {
    0x03, 0x02, 0x02, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
};

static GXColor color_tbl[3] = {
	{ 0x03, 0x02, 0x21, 0xFF },
	{ 0xD8, 0x0F, 0x51, 0xFF },
	{ 0x0B, 0x1B, 0x00, 0xFF },
};
#pragma force_active reset
// clang-format on

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
