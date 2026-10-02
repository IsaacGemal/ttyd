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
	0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02,
	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0xFF, 0x00,
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

void calc_pos(effFukidashiData* data, s32 index) {
	CameraEntry* cam;
	Vec axis;
	Vec temp;
	Mtx mtx;
	MarioWork* mario;
	PartyEntry* party;

	cam = camGetPtr(CAMERA_3D);
	temp = vec3_802f3048;
	temp.x = sin(MTXDegToRad(cam->viewYaw));
	temp.z = -(f32)cos(MTXDegToRad(cam->viewYaw));
	axis = temp;

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

// NONMATCHING (86.62%): all instructions match except the integer registers and the .rodata base
// register. The original reaches the four sfx Vecs and three strings through r31 = vec3_802f3048
// (+0x0c..+0x90); ours emits separate lis/addi, which shifts the ints and shrinks the frame by 0x10
EffectEntry* effFukidashiEntry(s32 a, s32 b, s32 c, s32 d, f32 e, f32 f, f32 g, f32 h, f32 i) {
	EffectEntry* entry;
	effFukidashiData* data;

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

	if ((s32)data->field_0x44 == -1) {
		MarioWork* mario = marioGetPtr();
		e += mario->position.x;
		f += mario->position.y;
		g += mario->position.z;
	} else if ((s32)data->field_0x44 == -2) {
		PartyEntry* party = partyGetPtr(marioGetPartyId());
		if (party != NULL) {
			e += party->position.x;
			f += party->position.y;
			g += party->position.z;
		}
	} else if ((s32)data->field_0x44 == -3) {
		PartyEntry* party = partyGetPtr(marioGetExtraPartyId());
		if (party != NULL) {
			e += party->position.x;
			f += party->position.y;
			g += party->position.z;
		}
	} else if (data->field_0x44 != NULL) {
		MarioWork* mario = (MarioWork*)data->field_0x44;
		e = mario->position.x;
		f = mario->position.y;
		g = mario->position.z;
	}

	if (c == -2 || c == -3) {
		switch (a) {
		case 0: {
			Vec vec = vec3_802f3054;
			vec.x = e;
			vec.y = f;
			vec.z = g;
			psndSFXOn_3D((const char*)0x5D, &vec);
			break;
		}
		case 1: {
			Vec vec = vec3_802f3060;
			vec.x = e;
			vec.y = f;
			vec.z = g;
			psndSFXOn_3D((const char*)0x5E, &vec);
			break;
		}
		}
	} else if (c != 0 && c != -1) {
		switch (a) {
		case 0: {
			Vec vec = vec3_802f306C;
			vec.x = e;
			vec.y = f;
			vec.z = g;
			psndSFXOn_3D("SFX_EVT_NPC_BIKKURI_MARK1", &vec);
			break;
		}
		case 1: {
			Vec vec = vec3_802f3078;
			vec.x = e;
			vec.y = f;
			vec.z = g;
			psndSFXOn_3D("SFX_EVT_NPC_QUESTION_MARK1", &vec);
			break;
		}
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

#ifdef MUST_MATCH
// Builds a GXColor from a base constant with the alpha byte overridden; returning it by value
// gives each call site its own compiler temporary, as the original's frame layout shows.
static inline GXColor fukidashiColor(const u32* base, u8 a) {
	GXColor c = *(const GXColor*)base;
	c.a = a;
	return c;
}
#endif

// The remaining differences are stack/register allocation. The frame is 0x10
// smaller (0x380 vs 0x390): the display-list loop's pointers land in r20-r22 where the original uses
// r19-r21, so the prologue saves 12 GPRs instead of 13. The 15 GXColor temporaries are packed (all
// source temps in 0x8-0x40, all call copies in 0x44-0x80) instead of the original's interleaved 8-byte
// [source, copy] pairs, and the GXLightObj lands at 0xd8 (reusing a dead first-loop Mtx slot) instead
// of 0x108, which rotates the Mtx slot assignment (the original's 0xd8 matrix is at 0x2c8 here). The
// original also reaches moja_model_vertex_tbl / moja_model_normal_tbl / moja_model_dl_0_tbl / chuchu_dt
// through one base register (r29 = lbl_8036BEA0); those arrays are extern here, so the compiler emits
// lis/addi pairs instead (this resolves once the unit's .data is defined in this file). Everything
// else matches the original's layout and code: the 10 Mtx slots, vec/v2, buf, tex, the case-2 Vec
// temp+copy, s32 alpha, the gp->retraceCount reads, the FontGetMessageWidth float, and the u32
// display-list counter (cmplwi, not cmpwi).
// NONMATCHING (98.51%): stack/register allocation and the .data base register differ; see above.
void effFukidashiDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	effFukidashiData* data = effect->userdata;
	CameraEntry* cam;
	GXTexObj tex;
	GXColor color;
	Mtx m0, m1, m2, m3;
	Mtx m4, m5, m6, m7, m8, m9;
	GXLightObj light;
	Vec vec;
	Vec v2;
	s32 alpha;
	s32 alphaA, alphaB;
	s32 count;
	s32 i, j;
	u32 n;
	s32 lang;
	f32 width;
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
		PSMTXRotRad(m1, 'y', MTXDegToRad(-camGetPtr(camId)->viewYaw));
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
				#ifdef MUST_MATCH
				color = fukidashiColor(&unk_80422ff8, alpha);
				#else
				color = *(GXColor*)&unk_80422ff8;
				color.a = alpha;
				#endif
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
					#ifdef MUST_MATCH
					color = fukidashiColor(&dat_8041c5b0, alpha);
					#else
					color = *(GXColor*)&dat_8041c5b0;
					color.a = alpha;
					#endif
					GXSetChanMatColor(GX_COLOR0A0, color);
				} else {
					#ifdef MUST_MATCH
					color = fukidashiColor(&dat_8041c5b4, alpha);
					#else
					color = *(GXColor*)&dat_8041c5b4;
					color.a = alpha;
					#endif
					GXSetChanMatColor(GX_COLOR0A0, color);
				}
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
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
				switch (data->field_0x0) {
					case 0:
						effGetTexObj(6, &tex);
						GXLoadTexObj(&tex, GX_TEXMAP0);
						PSMTXScale(m3, 0.16666667f, 1.0f, 1.0f);
						PSMTXTrans(m0, (f32)data->field_0x38, 0.0f, 0.0f);
						PSMTXConcat(m3, m0, m3);
						GXLoadTexMtxImm(m3, 0x1E, 1);
						break;
					case 1:
						effGetTexObj(7, &tex);
						GXLoadTexObj(&tex, GX_TEXMAP0);
						PSMTXScale(m3, 0.25f, 1.0f, 1.0f);
						PSMTXTrans(m0, (f32)data->field_0x38, 0.0f, 0.0f);
						PSMTXConcat(m3, m0, m3);
						GXLoadTexMtxImm(m3, 0x1E, 1);
						break;
				}
				#ifdef MUST_MATCH
				color = fukidashiColor(&unk_80422ffc, alphaA);
				#else
				color = *(GXColor*)&unk_80422ffc;
				color.a = alphaA;
				#endif
				GXSetTevColor(GX_TEVREG0, color);
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
					#ifdef MUST_MATCH
					color = fukidashiColor(&dat_8041c5b8, alphaB);
					#else
					color = *(GXColor*)&dat_8041c5b8;
					color.a = alphaB;
					#endif
					GXSetTevColor(GX_TEVREG0, color);
				} else {
					#ifdef MUST_MATCH
					color = fukidashiColor(&dat_8041c5bc, alphaB);
					#else
					color = *(GXColor*)&dat_8041c5bc;
					color.a = alphaB;
					#endif
					GXSetTevColor(GX_TEVREG0, color);
				}
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
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
				GXSetNumChans(1);
				GXSetChanCtrl(4, GX_TRUE, GX_SRC_REG, GX_SRC_VTX, 1, 2, 2);
				#ifdef MUST_MATCH
				color = fukidashiColor(&dat_8041c5c0, alphaB);
				#else
				color = *(GXColor*)&dat_8041c5c0;
				color.a = alphaB;
				#endif
				GXSetChanAmbColor(GX_COLOR0A0, color);
				v2 = *(Vec*)&vec3_802f3090;
				v2.x = data->position.x + 10.0f * (f32)cos((f32)gp->retraceCount / 10.0f);
				v2.y = data->position.y;
				v2.z = data->position.z - 10.0f * (f32)sin((f32)gp->retraceCount / 10.0f);
				vec = v2;
				PSMTXMultVec(camGetPtr(camId)->view, &vec, &vec);
				GXInitLightPos(&light, vec.x, vec.y, vec.z);
				#ifdef MUST_MATCH
				color = fukidashiColor(&dat_8041c5c4, alphaB);
				#else
				color = *(GXColor*)&dat_8041c5c4;
				color.a = alphaB;
				#endif
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
				PSMTXInvXpose(m8, m9);
				GXLoadNrmMtxImm(m9, 0);
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
				for (n = 0; n < 0x15; n++) {
					GXCallDisplayList((void*)moja_model_dl_0_tbl[n], moja_model_dl_0_size_tbl[n] << 5);
				}
				break;

			case 3:
				if (data->field_0x3C >= 0 && data->field_0x3C <= 0x63) {
					sprintf(buf, "%d", data->field_0x3C);
					width = 10.0f + (f32)FontGetMessageWidth(buf);
					PSMTXRotRad(m4, 'y', MTXDegToRad(data->field_0x24));
					PSMTXScale(m3, 0.5f, 0.5f, 0.5f);
					PSMTXTrans(m0, -width * 0.5f, -10.0f, 0.0f);
					PSMTXConcat(m2, m4, m8);
					PSMTXConcat(m8, m3, m8);
					PSMTXConcat(m8, m0, m8);
					if (i == 0) {
						#ifdef MUST_MATCH
						color = fukidashiColor(&dat_8041c5c8, alpha);
						#else
						color = *(GXColor*)&dat_8041c5c8;
						color.a = alpha;
						#endif
						iconDispGxCol(m8, 0x400, 0x18B, color);
					} else {
						#ifdef MUST_MATCH
						color = fukidashiColor(&dat_8041c5cc, alpha);
						#else
						color = *(GXColor*)&dat_8041c5cc;
						color.a = alpha;
						#endif
						iconDispGxCol(m8, 0x800, 0x18B, color);
					}
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
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
				#ifdef MUST_MATCH
				color = fukidashiColor(&unk_80423000, alphaB);
				#else
				color = *(GXColor*)&unk_80423000;
				color.a = alphaB;
				#endif
				GXSetTevColor(GX_TEVREG0, color);
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
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
				effGetTexObj(9, &tex);
				GXLoadTexObj(&tex, GX_TEXMAP0);
				PSMTXScale(m3, 0.25f, 1.0f, 1.0f);
				PSMTXTrans(m0, (f32)data->field_0x38, 0.0f, 0.0f);
				PSMTXConcat(m3, m0, m3);
				GXLoadTexMtxImm(m3, 0x1E, 1);
				#ifdef MUST_MATCH
				color = fukidashiColor(&unk_80423004, alphaB);
				#else
				color = *(GXColor*)&unk_80423004;
				color.a = alphaB;
				#endif
				GXSetTevColor(GX_TEVREG0, color);
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
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
				effGetTexObj(0xA, &tex);
				GXLoadTexObj(&tex, GX_TEXMAP0);
				if (i == 0) {
					#ifdef MUST_MATCH
					color = fukidashiColor(&unk_80423008, alphaA);
					#else
					color = *(GXColor*)&unk_80423008;
					color.a = alphaA;
					#endif
					GXSetTevColor(GX_TEVREG0, color);
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
						#ifdef MUST_MATCH
						color = fukidashiColor(&dat_8041c5d0, alphaB);
						#else
						color = *(GXColor*)&dat_8041c5d0;
						color.a = alphaB;
						#endif
						GXSetTevColor(GX_TEVREG0, color);
					} else {
						#ifdef MUST_MATCH
						color = fukidashiColor(&dat_8041c5d4, alphaB);
						#else
						color = *(GXColor*)&dat_8041c5d4;
						color.a = alphaB;
						#endif
						GXSetTevColor(GX_TEVREG0, color);
					}
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

