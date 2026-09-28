#include "eff/eff_fukidashi.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "driver/mapdrv.h"
#include "manager/fontmgr.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "mario/mariost.h"
#include "memory.h"
#include "party.h"
#include "pmario_sound.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <math.h>

extern u32 moja_model_vertex_tbl[];
extern u32 moja_model_normal_tbl[];
extern u32 moja_model_dl_0_tbl[];
extern u8 moja_model_dl_0_size_tbl[];
extern u32 moja_model_color0_tbl[];
extern u32 chuchu_dt[];
extern u32 unk_80422ff8;
extern u32 unk_80422ffc;
extern u32 unk_80423000;
extern u32 unk_80423004;
extern u32 unk_80423008;


const Vec vec3_802f3048 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3054 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3060 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f306C = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3078 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3084 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802f3090 = { 0.0f, 0.0f, 0.0f };

u32 anim_data_bikkuri[] = {
	0x00000102, 0x03050405, 0x05050505, 0x05050505, 0x05050505, 0x05050505, 0x05050505,
	0x05050505, 0x05050505, 0x0505FF00,
};

u32 anim_data_hatena[] = {
	0x00000201, 0x03020303, 0x03030303, 0x03030303, 0x03030303, 0x03030303, 0x03030303,
	0x03030303, 0x03030303, 0x03FF0000,
};

u32 anim_data_loop[] = { 0x00F60000 };

u32 anim_data_moja[] = { 0x00000101, 0x02020303, 0xF6000000 };

u8 anim_data_tenten[] = {
	0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x03, 0xFF, 0x00,
};

static void* anim_data_table[] = {
	anim_data_bikkuri, anim_data_hatena, anim_data_bikkuri, anim_data_loop,
	anim_data_tenten, anim_data_moja, anim_data_loop,
};

u32 anim_data_bikkuri_2[] = { 0x00000102, 0x03050405, 0xEFFF0000 };

u32 anim_data_hatena_2[] = { 0x00000201, 0x030203F0, 0xFF000000 };

u32 anim_data_tenten_2[] = {
	0x00000101, 0x01010101, 0x01010101, 0x02020202, 0x02020202, 0x02020303, 0x03030303,
	0x03030303, 0xE0FF0000,
};

void* anim_data_table_2[] = {
	anim_data_bikkuri_2, anim_data_hatena_2, anim_data_bikkuri_2, anim_data_loop,
	anim_data_tenten_2, anim_data_moja, anim_data_loop,
};

const u32 dat_8041c5b0 = 0xFFFFFF00;
const u32 dat_8041c5b4 = 0xA0A0A000;
const u32 dat_8041c5b8 = 0xFF000000;
const u32 dat_8041c5bc = 0xA0000000;
const u32 dat_8041c5c0 = 0xE0E0E000;
const u32 dat_8041c5c4 = 0xFFFFFF00;
const u32 dat_8041c5c8 = 0xFFFFFF00;
const u32 dat_8041c5cc = 0xA0A0A000;
const u32 dat_8041c5d0 = 0xFFCD2800;
const u32 dat_8041c5d4 = 0x7F661400;

// Local copy of system.c's inline helper so it inlines here (as in eff_queen.c).
inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

// NONMATCHING (88.6%): stack frame is 0x10 smaller and the axis Vec is not copied to the second
// temporary slot the original keeps for the PSMTXRotAxisRad call.
void calc_pos(effFukidashiData* data, s32 index) {
	CameraEntry* cam;
	Vec axis;
	Mtx mtx;
	MarioWork* mario;
	PartyEntry* party;

	cam = camGetPtr(CAMERA_3D);
	axis = vec3_802f3048;
	axis.x = sin(MTXDegToRad(cam->viewYaw));
	axis.z = -cos(MTXDegToRad(cam->viewYaw));

	PSMTXRotAxisRad(mtx, &axis, MTXDegToRad(-(data->field_0x1C - 20.0f + (f32)(index * 20))));

	if ((s32)data->field_0x44 == -1) {
		mario = marioGetPtr();
		data->position.x = mario->position.x + data->field_0x10.x + mtx[0][1] * (16.0f + data->field_0x20);
		data->position.y = mario->position.y + data->field_0x10.y + mtx[1][1] * (16.0f + data->field_0x20);
		data->position.z = mario->position.z + data->field_0x10.z + mtx[2][1] * (16.0f + data->field_0x20);
	} else if ((s32)data->field_0x44 == -2) {
		party = partyGetPtr(marioGetPartyId());
		if (party == NULL) {
			goto end;
		}
		data->position.x = party->position.x + data->field_0x10.x + mtx[0][1] * (16.0f + data->field_0x20);
		data->position.y = party->position.y + data->field_0x10.y + mtx[1][1] * (16.0f + data->field_0x20);
		data->position.z = party->position.z + data->field_0x10.z + mtx[2][1] * (16.0f + data->field_0x20);
	} else if ((s32)data->field_0x44 == -3) {
		party = partyGetPtr(marioGetExtraPartyId());
		if (party == NULL) {
			goto end;
		}
		data->position.x = party->position.x + data->field_0x10.x + mtx[0][1] * (16.0f + data->field_0x20);
		data->position.y = party->position.y + data->field_0x10.y + mtx[1][1] * (16.0f + data->field_0x20);
		data->position.z = party->position.z + data->field_0x10.z + mtx[2][1] * (16.0f + data->field_0x20);
	} else if (data->field_0x44) {
		mario = (MarioWork*)data->field_0x44;
		data->position.x = mario->position.x + data->field_0x10.x + mtx[0][1] * (16.0f + data->field_0x20);
		data->position.y = mario->position.y + data->field_0x10.y + mtx[1][1] * (16.0f + data->field_0x20);
		data->position.z = mario->position.z + data->field_0x10.z + mtx[2][1] * (16.0f + data->field_0x20);
	} else {
		data->position.x = data->field_0x10.x + mtx[0][1] * (16.0f + data->field_0x20);
		data->position.y = data->field_0x10.y + mtx[1][1] * (16.0f + data->field_0x20);
		data->position.z = data->field_0x10.z + mtx[2][1] * (16.0f + data->field_0x20);
	}

end:
	data->field_0x28 = data->field_0x1C - 20.0f + (f32)(index * 20);
	data->field_0x2C = 0.5f * (data->field_0x20 / 12.0f) + 0.5f;
}

// NONMATCHING (81.6%): stack frame is 0x30 smaller; the original gives each per-branch Vec
// temporary its own slot instead of reusing one, and register allocation differs.
EffectEntry* effFukidashiEntry(s32 a, s32 b, s32 c, s32 d, f32 e, f32 f, f32 g, f32 h, f32 i) {
	EffectEntry* entry;
	effFukidashiData* data;
	f32 x, y, z;

	entry = effEntry();
	entry->type = "Fukidashi";
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effFukidashiData));
	entry->userdata = data;
	entry->callback = effFukidashiMain;

	data->field_0x0 = a;
	data->field_0x3C = b;
	if (d < 0) {
		d = 10000;
	}
	data->field_0x40 = d;
	data->field_0x30 = d;
	data->field_0x34 = 0;
	data->field_0x44 = (void*)c;
	data->field_0x24 = 0.0f;
	data->field_0x48 = 0xFF;
	data->field_0x4C = 4;
	data->field_0x10.x = e;
	data->field_0x10.y = 3.0f + f;
	data->field_0x10.z = g;
	data->field_0x1C = i;
	data->field_0x20 = h;
	data->field_0x54 = 0;

	calc_pos(data, 1);

	if (a == 6) {
		data->field_0x24 = -450.0f;
	}

	x = e;
	y = f;
	z = g;

	if (c == -1) {
		MarioWork* mario = marioGetPtr();
		x += mario->position.x;
		y += mario->position.y;
		z += mario->position.z;
	} else if (c == -2) {
		PartyEntry* party = partyGetPtr(marioGetPartyId());
		if (party != NULL) {
			x += party->position.x;
			y += party->position.y;
			z += party->position.z;
		}
	} else if (c == -3) {
		PartyEntry* party = partyGetPtr(marioGetExtraPartyId());
		if (party != NULL) {
			x += party->position.x;
			y += party->position.y;
			z += party->position.z;
		}
	} else if (c != 0) {
		MarioWork* mario = (MarioWork*)c;
		x = mario->position.x;
		y = mario->position.y;
		z = mario->position.z;
	}

	if (c == -2 || c == -3) {
		if (a == 0) {
			Vec vec = vec3_802f3054;
			vec.x = x;
			vec.y = y;
			vec.z = z;
			psndSFXOn_3D((const char*)0x5D, &vec);
		} else if (a == 1) {
			Vec vec = vec3_802f3060;
			vec.x = x;
			vec.y = y;
			vec.z = z;
			psndSFXOn_3D((const char*)0x5E, &vec);
		}
	} else if (c != 0 && c != -1) {
		if (a == 0) {
			Vec vec = vec3_802f306C;
			vec.x = x;
			vec.y = y;
			vec.z = z;
			psndSFXOn_3D("SFX_EVT_NPC_BIKKURI_MARK1", &vec);
		} else if (a == 1) {
			Vec vec = vec3_802f3078;
			vec.x = x;
			vec.y = y;
			vec.z = z;
			psndSFXOn_3D("SFX_EVT_NPC_QUESTION_MARK1", &vec);
		}
	}

	return entry;
}

void effFukidashiMain(EffectEntry* effect) {
	effFukidashiData* data = effect->userdata;
	Vec disp;
	s32 type;
	s32 anim;

	disp = initializeVec(&data->position);
	type = data->field_0x0;

	if (data->field_0x40 == 0) {
		anim = ((s8*)anim_data_table[type])[data->field_0x34 / 2];
	} else {
		anim = ((s8*)anim_data_table_2[type])[data->field_0x34 / 2];
	}
	data->field_0x34++;
	data->field_0x38 = anim;

	if (anim < 0) {
		if (anim == -1) {
			effDelete(effect);
			return;
		}
		data->field_0x34 = (-10 - anim) * 2;
		if (data->field_0x40 == 0) {
			anim = ((s8*)anim_data_table[type])[data->field_0x34 / 2];
		} else {
			anim = ((s8*)anim_data_table_2[type])[data->field_0x34 / 2];
		}
		data->field_0x34++;
		data->field_0x38 = anim;
	}

	if (type == 6) {
		switch (data->field_0x54) {
			case 0:
				data->field_0x24 += 22.5f;
				if (data->field_0x24 >= 0.0f) {
					data->field_0x24 = 0.0f;
					data->field_0x54++;
				}
				break;
			case 0x78:
				data->field_0x24 -= 9.0f;
				if (data->field_0x24 <= -90.0f) {
					effDelete(effect);
					return;
				}
				break;
			default:
				data->field_0x54++;
				break;
		}
	} else {
		if (data->field_0x40 > 0) {
			if (data->field_0x30 < 10000) {
				data->field_0x30--;
			}
			if (data->field_0x30 < 0) {
				effDelete(effect);
				return;
			}
		}
	}

	if (data->field_0x44) {
		calc_pos(data, 1);
	}

	if (data->field_0x0 == 4) {
		dispEntry(data->field_0x4C, dispCalcZ(&disp), 2, effFukidashiDisp, effect);
	} else {
		dispEntry(data->field_0x4C, dispCalcZ(&disp), 8, effFukidashiDisp, effect);
	}
}

// NONMATCHING (91.1%): the 5980-byte draw callback. Same operations, but the stack frame is 0x90
// smaller (missing one of the original's ten Mtx temporaries) and the .data base register the
// original keeps in r29 is loaded per-reference here.
void effFukidashiDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effFukidashiData* data = effect->userdata;
	CameraEntry* cam;
	GXTexObj tex;
	GXLightObj light;
	GXColor color;
	Mtx m0, m1, m2, m3;
	Mtx m4, m5, m6, m7, m8;
	Vec vec;
	u8 alpha;
	s32 alphaA, alphaB;
	s32 count;
	s32 i, j;
	s32 lang;
	char buf[0x20];

	alpha = data->field_0x48;
	cam = camGetPtr(camId);
	mapSetMaterialFog();

	count = 1;
	if (data->field_0x24 != 0.0f) {
		count = 2;
	}
	if (data->field_0x0 == 2) {
		count = 2;
	}

	alphaA = (alpha * 0xB0) / 0xFF;
	alphaB = (alpha * 0xFF) / 0xFF;

	for (i = 0; i < count; i++) {
		PSMTXTrans(m0, data->position.x, data->position.y, data->position.z);
		cam = camGetPtr(camId);
		PSMTXRotRad(m1, 'y', MTXDegToRad(-cam->viewYaw));
		PSMTXConcat(m0, m1, m2);
		PSMTXScale(m3, 0.8f, 0.8f, 0.8f);
		PSMTXConcat(m2, m3, m2);
		PSMTXScale(m3, 1.0f, 0.8f, 1.0f);
		PSMTXRotRad(m4, 'z', MTXDegToRad(data->field_0x28));
		PSMTXConcat(m2, m3, m3);
		PSMTXConcat(m3, m4, m5);
		PSMTXConcat(cam->view, m5, m5);
		GXSetCurrentMtx(0);
		if (i == 0) {
			GXSetCullMode(GX_CULL_BACK);
		} else {
			GXSetCullMode(GX_CULL_FRONT);
		}

		PSMTXIdentity(m0);
		GXLoadTexMtxImm(m0, 0x1E, 1);
		GXSetNumTevStages(1);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, 4);
		GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
		GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
		GXSetZCompLoc(GX_TRUE);
		GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
		GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
		GXClearVtxDesc();
		GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
		GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
		GXSetNumChans(1);
		GXSetChanCtrl(4, 0, 0, 0, 0, 2, 2);
		GXSetNumTexGens(1);
		GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x1E, GX_FALSE, 0x7D);

		for (j = 0; j < 2; j++) {
			if (j == 0) {
				effGetTexObj(5, &tex);
				GXLoadTexObj(&tex, GX_TEXMAP0);
				color = *(GXColor*)&unk_80422ff8;
				color.a = alpha;
				GXSetChanMatColor(GX_COLOR0A0, color);
				PSMTXTrans(m0, 1.3f, -1.3f, 0.0f);
				PSMTXConcat(m5, m0, m0);
				PSMTXRotRad(m6, 'y', MTXDegToRad(data->field_0x24));
				PSMTXConcat(m0, m6, m0);
				GXLoadPosMtxImm(m0, 0);
				GXSetCurrentMtx(0);
			} else {
				effGetTexObj(4, &tex);
				GXLoadTexObj(&tex, GX_TEXMAP0);
				if (i == 0) {
					color = *(GXColor*)&dat_8041c5b0;
				} else {
					color = *(GXColor*)&dat_8041c5b4;
				}
				color.a = alpha;
				GXSetChanMatColor(GX_COLOR0A0, color);
				PSMTXTrans(m0, 0.0f, 0.0f, 0.0f);
				PSMTXConcat(m5, m0, m0);
				PSMTXRotRad(m6, 'y', MTXDegToRad(data->field_0x24));
				PSMTXConcat(m0, m6, m0);
				GXLoadPosMtxImm(m0, 0);
				GXSetCurrentMtx(0);
			}

			GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			GXPosition3f32(-16.0f, 13.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(16.0f, 13.0f, 0.0f);
			GXTexCoord2f32(2.0f, 0.0f);
			GXPosition3f32(16.0f, -19.0f, 0.0f);
			GXTexCoord2f32(2.0f, 1.0f);
			GXPosition3f32(-16.0f, -19.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
		}

		PSMTXScale(m7, 1.3f, 1.3f, 1.3f);
		PSMTXConcat(m5, m7, m5);

		switch (data->field_0x0) {
			case 0:
			case 1:
				GXSetNumTevStages(1);
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ONE, GX_CC_ONE, GX_CC_ONE, GX_CC_C0);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
				switch (data->field_0x0) {
					case 0:
						effGetTexObj(6, &tex);
						GXLoadTexObj(&tex, GX_TEXMAP0);
						PSMTXScale(m3, 0.16666667f, 1.0f, 1.0f);
						break;
					case 1:
						effGetTexObj(7, &tex);
						GXLoadTexObj(&tex, GX_TEXMAP0);
						PSMTXScale(m3, 0.25f, 1.0f, 1.0f);
						break;
				}
				PSMTXTrans(m0, (f32)data->field_0x38, 0.0f, 0.0f);
				PSMTXConcat(m3, m0, m3);
				GXLoadTexMtxImm(m3, 0x1E, 1);
				color = *(GXColor*)&unk_80422ffc;
				color.a = alphaA;
				GXSetTevColor(GX_TEVREG1, color);
				PSMTXTransApply(m5, m8, 1.0f, -1.0f, 0.0f);
				GXLoadPosMtxImm(m8, 0);
				if (data->field_0x0 == 0) {
					GXBegin(GX_QUADS, GX_VTXFMT0, 4);
					GXPosition3f32(-4.0f, 8.0f, 0.0f);
					GXTexCoord2f32(0.0f, 0.0f);
					GXPosition3f32(4.0f, 8.0f, 0.0f);
					GXTexCoord2f32(1.0f, 0.0f);
					GXPosition3f32(4.0f, -8.0f, 0.0f);
					GXTexCoord2f32(1.0f, 1.0f);
					GXPosition3f32(-4.0f, -8.0f, 0.0f);
					GXTexCoord2f32(0.0f, 1.0f);
				} else {
					GXBegin(GX_QUADS, GX_VTXFMT0, 4);
					GXPosition3f32(-8.0f, 16.0f, 0.0f);
					GXTexCoord2f32(0.0f, 0.0f);
					GXPosition3f32(16.0f, 16.0f, 0.0f);
					GXTexCoord2f32(1.0f, 0.0f);
					GXPosition3f32(16.0f, -8.0f, 0.0f);
					GXTexCoord2f32(1.0f, 1.0f);
					GXPosition3f32(-8.0f, -8.0f, 0.0f);
					GXTexCoord2f32(0.0f, 1.0f);
				}
				if (i == 0) {
					color = *(GXColor*)&dat_8041c5b8;
				} else {
					color = *(GXColor*)&dat_8041c5bc;
				}
				color.a = alphaB;
				GXSetTevColor(GX_TEVREG1, color);
				GXLoadPosMtxImm(m5, 0);
				if (data->field_0x0 == 0) {
					GXBegin(GX_QUADS, GX_VTXFMT0, 4);
					GXPosition3f32(-4.0f, 8.0f, 0.0f);
					GXTexCoord2f32(0.0f, 0.0f);
					GXPosition3f32(4.0f, 8.0f, 0.0f);
					GXTexCoord2f32(1.0f, 0.0f);
					GXPosition3f32(4.0f, -8.0f, 0.0f);
					GXTexCoord2f32(1.0f, 1.0f);
					GXPosition3f32(-4.0f, -8.0f, 0.0f);
					GXTexCoord2f32(0.0f, 1.0f);
				} else {
					GXBegin(GX_QUADS, GX_VTXFMT0, 4);
					GXPosition3f32(-8.0f, 16.0f, 0.0f);
					GXTexCoord2f32(0.0f, 0.0f);
					GXPosition3f32(16.0f, 16.0f, 0.0f);
					GXTexCoord2f32(1.0f, 0.0f);
					GXPosition3f32(16.0f, -8.0f, 0.0f);
					GXTexCoord2f32(1.0f, 1.0f);
					GXPosition3f32(-8.0f, -8.0f, 0.0f);
					GXTexCoord2f32(0.0f, 1.0f);
				}
				break;

			case 2:
				GXSetNumTevStages(1);
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, 4);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ONE, GX_CC_ONE, GX_CC_ONE, GX_CC_A0);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A1);
				GXSetNumChans(1);
				GXSetChanCtrl(4, GX_TRUE, GX_SRC_REG, GX_SRC_REG, GX_TRUE, 2, 2);
				color = *(GXColor*)&dat_8041c5c0;
				color.a = alphaB;
				GXSetChanAmbColor(GX_COLOR0A0, color);
				lang = gp->retraceCount;
				vec = *(Vec*)&vec3_802f3090;
				vec.x = data->position.x + 10.0f * cos((f32)lang / 10.0f);
				vec.y = data->position.y;
				vec.z = data->position.z - 10.0f * sin((f32)lang / 10.0f);
				cam = camGetPtr(camId);
				PSMTXMultVec(cam->view, &vec, &vec);
				GXInitLightPos(&light, vec.x, vec.y, vec.z);
				color = *(GXColor*)&dat_8041c5c4;
				color.a = alphaB;
				GXInitLightColor(&light, color);
				GXInitLightAttn(&light, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
				GXLoadLightObjImm(&light, GX_LIGHT0);
				PSMTXTrans(m0, 5.0f, -8.0f, 0.0f);
				PSMTXConcat(m5, m0, m8);
				PSMTXRotRad(m0, 'z', 0.43633232f);
				PSMTXConcat(m8, m0, m8);
				PSMTXRotRad(m0, 'y', MTXDegToRad((f32)(gp->retraceCount * 10)));
				PSMTXConcat(m8, m0, m8);
				PSMTXScale(m0, 0.15f, 0.15f, 0.15f);
				PSMTXConcat(m8, m0, m8);
				GXLoadPosMtxImm(m8, 0);
				PSMTXInvXpose(m8, m2);
				GXLoadNrmMtxImm(m2, 0);
				GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
				GXClearVtxDesc();
				GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
				GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 8);
				GXSetArray(GX_VA_POS, moja_model_vertex_tbl, 6);
				GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
				GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
				GXSetArray(GX_VA_NRM, moja_model_normal_tbl, 3);
				GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
				GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
				GXSetArray(GX_VA_CLR0, moja_model_color0_tbl, 4);
				for (j = 0; j < 0x15; j++) {
					GXCallDisplayList((void*)moja_model_dl_0_tbl[j], moja_model_dl_0_size_tbl[j] << 5);
				}
				break;

			case 3:
				if (data->field_0x3C >= 0 && data->field_0x3C <= 0x63) {
					sprintf(buf, "%d", data->field_0x3C);
					PSMTXRotRad(m4, 'y', MTXDegToRad(data->field_0x24));
					PSMTXScale(m3, 0.5f, 0.5f, 0.5f);
					PSMTXTrans(m0, -(10.0f + (f32)FontGetMessageWidth(buf)) * 0.5f, -10.0f, 0.0f);
					PSMTXConcat(m2, m4, m8);
					PSMTXConcat(m8, m3, m8);
					PSMTXConcat(m8, m0, m8);
					if (i == 0) {
						color = *(GXColor*)&dat_8041c5c8;
					} else {
						color = *(GXColor*)&dat_8041c5cc;
					}
					color.a = alpha;
					iconDispGxCol(m8, i == 0 ? 0x400 : 0x800, 0x18B, color);
					PSMTXTrans(m0, 10.0f, 22.0f, 0.0f);
					PSMTXConcat(m8, m0, m8);
					FontDrawStart_alpha(alpha);
					GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
					if (i == 0) {
						GXSetCullMode(GX_CULL_BACK);
					} else {
						GXSetCullMode(GX_CULL_FRONT);
					}
					FontDrawStringMtx(m8, buf);
				}
				break;

			case 4:
				PSMTXIdentity(m0);
				GXLoadTexMtxImm(m0, 0x1E, 1);
				GXSetNumTevStages(1);
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ONE, GX_CC_ONE, GX_CC_ONE, GX_CC_ONE);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
				color = *(GXColor*)&unk_80423000;
				color.a = alphaB;
				GXSetTevColor(GX_TEVREG1, color);
				effGetTexObj(8, &tex);
				GXLoadTexObj(&tex, GX_TEXMAP0);
				for (j = 0; j < data->field_0x38; j++) {
					PSMTXTransApply(m5, m8, -5.0f + 5.0f * (f32)j, 0.0f, 0.0f);
					GXLoadPosMtxImm(m8, 0);
					GXBegin(GX_QUADS, GX_VTXFMT0, 4);
					GXPosition3f32(-3.0f, 3.0f, 0.0f);
					GXTexCoord2f32(0.0f, 0.0f);
					GXPosition3f32(3.0f, 3.0f, 0.0f);
					GXTexCoord2f32(1.0f, 0.0f);
					GXPosition3f32(3.0f, -3.0f, 0.0f);
					GXTexCoord2f32(1.0f, 1.0f);
					GXPosition3f32(-3.0f, -3.0f, 0.0f);
					GXTexCoord2f32(0.0f, 1.0f);
				}
				break;

			case 5:
				GXSetNumTevStages(1);
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ONE, GX_CC_ONE, GX_CC_ONE, GX_CC_ONE);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
				effGetTexObj(9, &tex);
				GXLoadTexObj(&tex, GX_TEXMAP0);
				PSMTXScale(m3, 0.25f, 1.0f, 1.0f);
				PSMTXTrans(m0, (f32)data->field_0x38, 0.0f, 0.0f);
				PSMTXConcat(m3, m0, m3);
				GXLoadTexMtxImm(m3, 0x1E, 1);
				color = *(GXColor*)&unk_80423004;
				color.a = alphaB;
				GXSetTevColor(GX_TEVREG1, color);
				GXLoadPosMtxImm(m5, 0);
				GXBegin(GX_QUADS, GX_VTXFMT0, 4);
				GXPosition3f32(-7.0f, 7.0f, 0.0f);
				GXTexCoord2f32(0.0f, 0.0f);
				GXPosition3f32(7.0f, 7.0f, 0.0f);
				GXTexCoord2f32(1.0f, 0.0f);
				GXPosition3f32(7.0f, -7.0f, 0.0f);
				GXTexCoord2f32(1.0f, 1.0f);
				GXPosition3f32(-7.0f, -7.0f, 0.0f);
				GXTexCoord2f32(0.0f, 1.0f);
				break;

			case 6:
				lang = gp->retraceCount % 15;
				PSMTXIdentity(m0);
				GXLoadTexMtxImm(m0, 0x1E, 1);
				PSMTXTrans(m0, ((f32*)chuchu_dt)[lang * 3 + 0], 0.0f, 0.0f);
				PSMTXScale(m3, 0.7f * ((f32*)chuchu_dt)[lang * 3 + 1], 0.7f * ((f32*)chuchu_dt)[lang * 3 + 2], 0.7f);
				PSMTXRotRad(m6, 'y', MTXDegToRad(data->field_0x24));
				PSMTXConcat(m0, m6, m0);
				PSMTXConcat(m0, m3, m0);
				GXSetNumTevStages(1);
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ONE, GX_CC_C0, GX_CC_TEXC, GX_CC_ONE);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
				effGetTexObj(0xA, &tex);
				GXLoadTexObj(&tex, GX_TEXMAP0);
				if (i == 0) {
					color = *(GXColor*)&unk_80423008;
					color.a = alphaA;
					GXSetTevColor(GX_TEVREG1, color);
					PSMTXTransApply(m5, m8, 1.0f, -1.0f, 0.0f);
					PSMTXConcat(m8, m0, m8);
					GXLoadPosMtxImm(m8, 0);
					GXBegin(GX_QUADS, GX_VTXFMT0, 4);
					GXPosition3f32(-8.0f, 16.0f, 0.0f);
					GXTexCoord2f32(0.0f, 0.0f);
					GXPosition3f32(16.0f, 16.0f, 0.0f);
					GXTexCoord2f32(1.0f, 0.0f);
					GXPosition3f32(16.0f, -8.0f, 0.0f);
					GXTexCoord2f32(2.0f, 1.0f);
					GXPosition3f32(-8.0f, -8.0f, 0.0f);
					GXTexCoord2f32(0.0f, 1.0f);
					if (i == 0) {
						color = *(GXColor*)&dat_8041c5d0;
					} else {
						color = *(GXColor*)&dat_8041c5d4;
					}
					color.a = alphaB;
					GXSetTevColor(GX_TEVREG1, color);
					PSMTXConcat(m5, m0, m8);
					GXLoadPosMtxImm(m8, 0);
					GXBegin(GX_QUADS, GX_VTXFMT0, 4);
					GXPosition3f32(-8.0f, 16.0f, 0.0f);
					GXTexCoord2f32(0.0f, 0.0f);
					GXPosition3f32(16.0f, 16.0f, 0.0f);
					GXTexCoord2f32(1.0f, 0.0f);
					GXPosition3f32(16.0f, -8.0f, 0.0f);
					GXTexCoord2f32(2.0f, 1.0f);
					GXPosition3f32(-8.0f, -8.0f, 0.0f);
					GXTexCoord2f32(0.0f, 1.0f);
				}
				break;
		}
	}
}

