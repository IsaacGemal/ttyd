#include "eff/n64/eff_confetti_n64.h"
#include "driver/effdrv.h"
#include "driver/camdrv.h"
#include "eff/eff_n64.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .rodata int->float conversion constant the original references by name.
extern const f64 double_to_int_802fa448;

// .sdata2 float constants (named after their original addresses).
extern const f32 float_0p011489_8041ebf0;
extern const f32 float_deg2rad_8041ebf4;
extern const f32 float_0p0625_8041ebf8;
extern const f32 float_0p0039062_8041ebfc;
extern const f32 float_0_8041ec00;

// .sbss2 scratch GXColor the original builds its TEV colour from.
extern GXColor unk_804230c8;

// .sdata tables and .data vertex arrays belonging to this unit.
extern u8 color_table[36];
extern u8 anm_seq[8];
extern u32 size6x6_tex16x16_vtx[];
extern u32 size6x6_tex16x16_2_vtx[];

void effConfettiN64SetCamId(EffectEntry* entry, s32 camId) {
	((effConfettiN64Data*)entry->userdata)->camId = camId;
}

// The instruction sequence and stack frame are identical; three allocation/labelling artifacts remain:
//  (1) register numbering: the original keeps the frame value and the loop counter in r26 (reusing it after
//      the frame dies) with count in r24; this build reuses r24 for seq and puts the loop counter in r29
//      (data/type/alpha are r27/r28/r25 in both);
//  (2) the GXColor for the first GXSetTevColor is built at r1+0xc and word-copied to the outgoing slot at
//      r1+0x10, while this build builds it at r1+0x10 and copies down to r1+0xc (known mwcc quirk, also in
//      eff_akari_pose_n64.c / eff_booboo_n64.c / eff_batten.c);
//  (3) the implicit int->float constant gets the local label @194 instead of double_to_int_802fa448.
// NONMATCHING (99.61%): only (1) the frame/loop-counter register numbering, (2) the GXColor stack slot
// direction, and (3) the @194 label differ.
void effConfettiDisp(CameraId camId, void* param) {
	EffectEntry* effect;
	s32 type;
	effConfettiN64Data* data;
	s32 count;
	s32 alpha;
	s32 i;
	s32 seq;
	s32 idx2;
	u8* clr;
	CameraEntry* cam;
	GXTexObj texObj;
	GXColor color;
	Mtx mtx0;
	Mtx scaleMtx;
	Mtx mtx1;
	Mtx mtx2;

	effect = param;
	cam = camGetPtr(camId);
	data = effect->userdata;
	i = *(s32*)((u8*)data + 0x24);
	type = data->type;
	count = *(s32*)((u8*)data + 0x2C);
	alpha = *(s32*)((u8*)data + 0x28);
	PSMTXTrans(mtx0, data->x, data->y, data->z);
	PSMTXRotRad(mtx1, 'y', float_deg2rad_8041ebf4 * -camGetPtr(CAMERA_3D)->viewYaw);
	PSMTXConcat(mtx0, mtx1, mtx2);
	PSMTXConcat(cam->view, mtx2, mtx2);
	data++;
	effGetTexObjN64(0x4B, &texObj);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetCullMode(GX_CULL_NONE);
	GXSetNumChans(0);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_C1);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, 0x7D);
	seq = (i >> 1) % 6;
	type %= 10;
	i = 0;
	for (; i < count; i++, data++) {
		clr = &color_table[(i % 12) * 3];
		if (*(s32*)((u8*)data + 0x30) > 0) {
			continue;
		}
		PSMTXTrans(mtx0, data->x, data->y, data->z);
		PSMTXConcat(mtx2, mtx0, mtx0);
		GXLoadPosMtxImm(mtx0, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		color = unk_804230c8;
		color.r = clr[0];
		color.g = clr[1];
		color.b = clr[2];
		color.a = alpha;
		GXSetTevColor(GX_TEVREG0, color);
		GXSetTevColor(GX_TEVREG1, *(GXColor*)&float_0p011489_8041ebf0);
		idx2 = ((i + anm_seq[seq]) & 0xF) << 6;
		PSMTXScale(scaleMtx, float_0p0625_8041ebf8, float_0p0039062_8041ebfc, float_0_8041ec00);
		PSMTXTrans(mtx0, float_0_8041ec00, (f32)idx2, float_0_8041ec00);
		PSMTXConcat(scaleMtx, mtx0, scaleMtx);
		GXLoadTexMtxImm(scaleMtx, GX_TEXMTX0, GX_MTX2x4);
		switch (type) {
		case 3:
			effSetVtxDescN64((effVtxArray*)size6x6_tex16x16_2_vtx);
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 0x18);
			tri2(0, 1, 2, 0, 0, 2, 3, 0);
			tri2(4, 5, 6, 4, 4, 6, 7, 4);
			tri2(8, 9, 10, 8, 8, 10, 11, 8);
			tri2(12, 13, 14, 12, 12, 14, 15, 12);
			break;
		default:
			effSetVtxDescN64((effVtxArray*)size6x6_tex16x16_vtx);
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
			tri2(0, 1, 2, 0, 0, 2, 3, 0);
			break;
		}
	}
}
