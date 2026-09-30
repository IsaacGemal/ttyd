#include "eff/eff_torch.h"
#include "driver/effdrv.h"
#include "driver/dispdrv.h"
#include "mario/mariost.h"
#include "memory.h"

// .sdata2
const char str_8041E444[] = "Torch";

// .sdata2 constants this unit references by name in retail
extern const f32 float_0p5_8041e44c;
extern const f32 float_1_8041e43c;

// .sdata2 / .rodata symbols effTorchDisp needs (data lives in the original unit)
extern const GXColor dat_8041e420;
extern const f32 float_deg2rad_8041e424;
extern const u32 torch_obj2_texture[];
extern const u32 torch_obj22t_texture[];

EffectEntry* effTorchEntry(s32 id, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry = effEntry();
	effTorchData* data;

	entry->type = str_8041E444;
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x20);
	entry->userdata = data;
	entry->callback = effTorchMain;

	data->id = id;
	*(u8*)((u8*)data + 0x1C) = 0;
	// The nested block is not cosmetic: the original keeps this temporary at r1+8 and
	// interleaves its copy with the component writes. A function-scope declaration schedules
	// the three lwz together and does not match.
	{
		Vec position = {0.0f, 0.0f, 0.0f};
		position.x = x;
		position.y = y;
		position.z = z;
		data->position = position;
	}
	*(f32*)((u8*)data + 0x10) = float_0p5_8041e44c * scale;
	*(f32*)((u8*)data + 0x14) = float_1_8041e43c;
	*(f32*)((u8*)data + 0x18) = float_1_8041e43c;
	return entry;
}

void effTorchMain(EffectEntry* effect) {
	effTorchData* data = effect->userdata;

	dispEntry(CAMERA_3D, dispCalcZ(&data->position), 2, effTorchDisp, effect);
}

void effTorchDisp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	effTorchData* data = effect->userdata;
	CameraEntry* camView;
	GXTexObj texObj;
	Mtx mtxPos, mtxScale, mtxRot, mtxView, mtxTex;
	s32 i;
	f32 s;

	camView = camGetPtr(cameraId);
	PSMTXTrans(mtxPos, data->position.x, data->position.y, data->position.z);
	PSMTXRotRad(mtxRot, 'y', -camGetPtr(cameraId)->viewYaw * float_deg2rad_8041e424);
	s = *(f32*)((u8*)data + 0x10);
	PSMTXScale(mtxScale, s * *(f32*)((u8*)data + 0x14), s * *(f32*)((u8*)data + 0x18), s);
	PSMTXConcat(mtxPos, mtxRot, mtxView);
	PSMTXConcat(mtxView, mtxScale, mtxView);
	PSMTXConcat(camView->view, mtxView, mtxView);
	GXLoadPosMtxImm(mtxView, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
	GXSetNumChans(GX_FALSE);
	GXInitTexObj(&texObj, (void*)torch_obj2_texture, 0x20, 0x40, GX_TF_I4, GX_CLAMP, GX_CLAMP, GX_FALSE);
	GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXInitTexObj(&texObj, (void*)torch_obj22t_texture, 0x20, 0x40, GX_TF_I4, GX_CLAMP, GX_MIRROR, GX_FALSE);
	GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
	GXLoadTexObj(&texObj, GX_TEXMAP1);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXSetTevColor(GX_TEVREG0, dat_8041e420);
	GXSetNumTevStages(2);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C0, GX_CC_TEXC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
	GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_TEXA, GX_CA_ZERO);
	GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_CLEAR);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GXSetCullMode(GX_CULL_NONE);

	for (i = 0; i < 6; i++) {
		PSMTXTrans(mtxTex, 0.0f, (f32)(gp->retraceCount + i) / -100.0f, 0.0f);
		GXLoadTexMtxImm(mtxTex, GX_TEXMTX0, GX_MTX2x4);
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3f32(-40.0f, -20.0f, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3f32(40.0f, -20.0f, 0.0f);
		GXTexCoord2f32(1.0f, 0.0f);
		GXPosition3f32(40.0f, 200.0f, 0.0f);
		GXTexCoord2f32(1.0f, 1.0f);
		GXPosition3f32(-40.0f, 200.0f, 0.0f);
		GXTexCoord2f32(0.0f, 1.0f);
	}
}
