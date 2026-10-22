#include "eff/n64/eff_flame_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <dolphin/os/OSFastCast.h>
#include <string.h>

// The unit's .data in address order. `lbl_8039CDA8` is the section's first object and the compiler's
// `.data` base; the eight float pairs keep the three vertex tables at 0x40/0xB0/0x120. The four pairs the
// original names with a `$NNN` local (`negone_one$374`, ...) are declared here with plain names (cf.
// eff_booboo_n64); the `lbl_8039CDxx` globals keep the disassembly's names. Porting the tables (instead of
// addressing them as raw offsets) is what fixes the two-add vertex copy in effFlameDisp below.
#pragma force_active on
__declspec(section ".data") u32 lbl_8039CDA8[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") static u32 negone_one_374[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") u32 lbl_8039CDB8[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") static u32 one_negone_373[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") u32 lbl_8039CDC8[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") static u32 negone_one_360[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") u32 lbl_8039CDD8[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") static u32 one_negone_359[2] = { 0x3F800000, 0xBF800000 };

__declspec(section ".data") u32 stg_flame_v[28] = {
	0xFFCEFFEC, 0x00020000, 0x0000FFFF, 0xFFFF0032, 0xFFEC0002, 0x04000000, 0xFFFFFFFF,
	0x00320064, 0x00020400, 0x0400FFFF, 0xFFFFFFCE, 0x00640002, 0x00000400, 0xFFFFFFFF,
	0xFFCEFFEC, 0xFFFE0000, 0x0000FFFF, 0xFFFF0032, 0xFFECFFFE, 0x04000000, 0xFFFFFFFF,
	0x00320064, 0xFFFE0400, 0x0400FFFF, 0xFFFFFFCE, 0x0064FFFE, 0x00000400, 0xFFFFFFFF,
};

__declspec(section ".data") u32 stg_flame_2_v[28] = {
	0xFFC40000, 0x00120000, 0x0000FFFF, 0xFFFF003C, 0x00000012, 0x08000000, 0xFFFFFFFF,
	0x003C0078, 0x00120800, 0x0800FFFF, 0xFFFFFFC4, 0x00780012, 0x00000800, 0xFFFFFFFF,
	0xFFC40000, 0xFFEE0000, 0x0000FFFF, 0xFFFF003C, 0x0000FFEE, 0x08000000, 0xFFFFFFFF,
	0x003C0078, 0xFFEE0800, 0x0800FFFF, 0xFFFFFFC4, 0x0078FFEE, 0x00000800, 0xFFFFFFFF,
};

__declspec(section ".data") u32 stg_flame_3_v[28] = {
	0xFFCEFFCE, 0x00020000, 0x0000FFFF, 0xFFFF0032, 0xFFCE0002, 0x04000000, 0xFFFFFFFF,
	0x00320032, 0x00020400, 0x0400FFFF, 0xFFFFFFCE, 0x00320002, 0x00000400, 0xFFFFFFFF,
	0xFFCEFFCE, 0xFFFE0000, 0x0000FFFF, 0xFFFF0032, 0xFFCEFFFE, 0x04000000, 0xFFFFFFFF,
	0x00320032, 0xFFFE0400, 0x0400FFFF, 0xFFFFFFCE, 0x0032FFFE, 0x00000400, 0xFFFFFFFF,
};
#pragma force_active reset

/* Retail inlines a per-vertex helper; each inlining gets its own Vec slot. Passing
   the destination as a pointer (not `data` + `idx`) keeps the address out of a sixth callee-saved
   register; the table base is passed as a named table symbol. Addressing the source as `&tbl[idx]`
   directly (NOT through a named `src` local) is part of what lets `effFlameDisp` put the unit's
   `.data` base in r27, retail's register - a named `src` local shifts it to r30. */
static inline void effFlameSetVtx(effFlameVtx* dst, Mtx mtx, effFlameVtx* tbl, s32 idx) {
	Vec v;
	memcpy(dst, &tbl[idx], sizeof(effFlameVtx));


	OSInitFastCast();
	OSs16tof32(&dst->pos[0], &v.x);
	OSs16tof32(&dst->pos[1], &v.y);
	OSs16tof32(&dst->pos[2], &v.z);
	PSMTXMultVec(mtx, &v, &v);
	OSInitFastCast();
	OSf32tos16(&v.x, &dst->pos[0]);
	OSf32tos16(&v.y, &dst->pos[1]);
	OSf32tos16(&v.z, &dst->pos[2]);
}
EffectEntry* effFlameN64Entry(s32 type, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* entry;
	effFlameData* data;

	entry = effEntry();
#ifdef MUST_MATCH
	// the original's .rodata is a 12-byte "FlameN64" string plus a 4-byte zero gap_04 object;
	// a 16-byte literal leaves the same 4 trailing zero bytes at the section end
	entry->type = "FlameN64\0\0\0\0\0\0\0";
#else
	entry->type = "FlameN64";
#endif
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effFlameData));
	entry->userdata = data;
	entry->callback = effFlameMain;
	entry->flags |= 2;

	data->type = type;
	data->life = 100;
	data->counter = 0;
	data->alpha = 0;
	data->position.x = x;
	data->position.y = y;
	data->position.z = z;
	data->rot = 0.0f;
	data->scrollV = 0.0f;
	data->scrollU = 0.0f;
	data->scroll2V = 0.0f;
	data->scroll2U = 0.0f;

	switch (type) {
	case 0:
		data->tev0Rgb[0] = 0x4B;
		data->tev0Rgb[1] = 0x4B;
		data->tev0Rgb[2] = 0x4B;
		data->tev1Rgba[0] = 0xFF;
		data->tev1Rgba[1] = 0xF7;
		data->tev1Rgba[2] = 0x9B;
		data->tev1Rgba[3] = 0;
		data->scaleX = 2.0f * scale;
		data->scaleY = 0.6f * scale;
		data->baseScale = scale;
		data->scrollUSpeed = -0.9f;
		data->scrollVSpeed = 0.04f;
		data->scrollUSpeedTarget = 0.04f;
		data->scrollVSpeedTarget = 0.9f;
		data->scroll2USpeed = -0.9f;
		data->scroll2VSpeed = 0.04f;
		data->scroll2USpeedTarget = 0.04f;
		data->scroll2VSpeedTarget = 0.9f;
		break;
	case 1:
		data->tev0Rgb[0] = 0x4B;
		data->tev0Rgb[1] = 0x4B;
		data->tev0Rgb[2] = 0x4B;
		data->tev1Rgba[0] = 0xFF;
		data->tev1Rgba[1] = 0;
		data->tev1Rgba[2] = 0;
		data->tev1Rgba[3] = 0;
		data->scaleX = scale;
		data->scaleY = scale;
		data->baseScale = scale;
		data->scrollUSpeed = 0.0f;
		data->scrollVSpeed = 0.04f;
		data->scrollUSpeedTarget = 0.04f;
		data->scrollVSpeedTarget = 0.9f;
		data->scroll2USpeed = 0.0f;
		data->scroll2VSpeed = 0.04f;
		data->scroll2USpeedTarget = 0.04f;
		data->scroll2VSpeedTarget = 0.9f;
		break;
	case 2:
		data->tev0Rgb[0] = 0x19;
		data->tev0Rgb[1] = 0x79;
		data->tev0Rgb[2] = 0x8E;
		data->tev1Rgba[0] = 0x22;
		data->tev1Rgba[1] = 0;
		data->tev1Rgba[2] = 0xFF;
		data->tev1Rgba[3] = 0x51;
		data->scaleX = scale;
		data->scaleY = scale;
		data->baseScale = scale;
		data->scrollUSpeed = 0.0f;
		data->scrollVSpeed = 0.04f;
		data->scrollUSpeedTarget = 0.2f;
		data->scrollVSpeedTarget = 1.3f;
		data->scroll2USpeed = 0.0f;
		data->scroll2VSpeed = 0.04f;
		data->scroll2USpeedTarget = 0.16000001f;
		data->scroll2VSpeedTarget = 1.04f;
		break;
	default:
		data->tev0Rgb[0] = 0xB3;
		data->tev0Rgb[1] = 0xA9;
		data->tev0Rgb[2] = 0x55;
		data->tev1Rgba[0] = 0x22;
		data->tev1Rgba[1] = 0;
		data->tev1Rgba[2] = 0xFF;
		data->tev1Rgba[3] = 0x3F;
		data->scaleX = scale;
		data->scaleY = scale;
		data->baseScale = scale;
		data->scrollUSpeed = 0.0f;
		data->scrollVSpeed = 0.04f;
		data->scrollUSpeedTarget = 0.1f;
		data->scrollVSpeedTarget = 1.7f;
		data->scroll2USpeed = 0.0f;
		data->scroll2VSpeed = 0.04f;
		data->scroll2USpeedTarget = 0.080000006f;
		data->scroll2VSpeedTarget = 1.36f;
		break;
	}

	data->lagX = data->position.x;
	data->lagY = data->position.y;
	data->lagZ = data->position.z;

	return entry;
}
void effFlameMain(EffectEntry* effect) {
	effFlameData* data = effect->userdata;
	Vec position;
	Vec disp = { 0 };
	s32 life;
	s32 type;
	s32 counter;

	disp.x = data->position.x;
	disp.y = data->position.y;
	disp.z = data->position.z;
	position = disp;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		data->life = 16;
	}
	data->counter++;
	if (data->counter > 0x4F1A0) {
		data->counter = 0x100;
	}
	if (data->life <= 16) {
		data->life--;
	}
	life = data->life;
	if (life < 0) {
		effDelete(effect);
		return;
	}

	type = data->type;
	counter = data->counter;
	if (type <= 1) {
		if (counter <= 10) {
			data->alpha = data->alpha + (0x80 - data->alpha) * 0.5f;
		}
	}
	else {
		if (counter <= 10) {
			data->alpha = counter * 0xFF / 10;
		}
	}
	if (life < 16) {
		data->alpha = life * 16;
	}

	if (type == 0) {
		data->scrollUSpeed += 0.02f * (data->scrollUSpeedTarget - data->scrollUSpeed);
		data->scrollVSpeed += 0.02f * (data->scrollVSpeedTarget - data->scrollVSpeed);
		data->scaleX += 0.04f * (data->baseScale - data->scaleX);
		data->scaleY += 0.04f * (data->baseScale - data->scaleY);
	}
	else {
		data->scrollUSpeed += 0.02f * (data->scrollUSpeedTarget - data->scrollUSpeed);
		data->scrollVSpeed += 0.02f * (data->scrollVSpeedTarget - data->scrollVSpeed);
		data->scroll2USpeed += 0.02f * (data->scroll2USpeedTarget - data->scroll2USpeed);
		data->scroll2VSpeed += 0.02f * (data->scroll2VSpeedTarget - data->scroll2VSpeed);
	}

	data->scrollU += data->scrollUSpeed;
	if (data->scrollU < 0.0f) {
		data->scrollU += 64.0f;
	}
	else if (data->scrollU > 64.0f) {
		data->scrollU -= 64.0f;
	}

	data->scrollV += data->scrollVSpeed;
	if (data->scrollV < 0.0f) {
		data->scrollV += 128.0f;
	}
	else if (data->scrollV > 128.0f) {
		data->scrollV -= 128.0f;
	}

	data->scroll2U += data->scroll2USpeed;
	if (data->scroll2U < 0.0f) {
		data->scroll2U += 64.0f;
	}
	else if (data->scroll2U > 64.0f) {
		data->scroll2U -= 64.0f;
	}

	data->scroll2V += data->scroll2VSpeed;
	if (data->scroll2V < 0.0f) {
		data->scroll2V += 128.0f;
	}
	else if (data->scroll2V > 128.0f) {
		data->scroll2V -= 128.0f;
	}

	data->lagX += 0.1f * (data->position.x - data->lagX);
	data->lagY += 0.1f * (data->position.y - data->lagY);
	data->lagZ += 0.1f * (data->position.z - data->lagZ);

	dispEntry(CAMERA_3D, dispCalcZ(&position), 2, effFlameDisp, effect);
}

// The 24 inlined vertex transforms convert the s16 positions with the SDK's paired-single inlines
// (`OSs16tof32`/`OSf32tos16` in OSFastCast.h, GQR5 type 7) after `OSInitFastCast()`, as the original does.
// The three vertex tables are defined in this unit as the named globals `stg_flame_v`/`stg_flame_2_v`/
// `stg_flame_3_v` (`.data` 0x40/0xB0/0x120). Referencing them by name (not through a local pointer) is what
// makes MWCC keep the unit's `.data` base in one register and emit the original's two adds per vertex:
// `addi r4, rBase, 0x40` for the table, then `addi r4, r4, idx*14` for the vertex. A local
// `table + 0x40` folds the two into one add; three local table pointers make MWCC emit a `lis` per table.
// `alpha` is declared (uninitialised) before `type` and assigned as a statement after the other
// declarations: that gives MWCC retail's `type`/`alpha` colouring (r29/r30) while the loads stay `type`
// (`0x0`) first, `alpha` (`0x28`) second. Together with the `effFlameSetVtx` memcpy shape above (no named
// `src` local) the `.data` base lands in r27 and `effect`/`cam` share r28, exactly as retail.
// The two `GXColor` values are built inside the `effFlameSetColor1/2` void inlines above; that is the shape that
// gives MWCC the original's interleaved temp/arg slots (see those helpers).
void effFlameDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	CameraEntry* cam = camGetPtr(camId);
	effFlameData* data = effect->userdata;
	s32 alpha;
	s32 type = data->type;
	Mtx mtxRot;
	Mtx mtx2;
	Mtx mtx1;
	Mtx mtx3;
	GXTexObj texObj;

	alpha = data->alpha;
	GXSetNumChans(0);
	GXSetNumTexGens(2);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);

	switch (type) {
	case 0:
	case 1:
		GXSetNumTevStages(4);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG2);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG2);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
		GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_C2, GX_CC_C2, GX_CC_ONE);
		GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_TEXA, GX_CA_ZERO, GX_CA_A2, GX_CA_TEXA);
		GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_C2, GX_CC_TEXC, GX_CC_CPREV);
		GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_A2, GX_CA_APREV, GX_CA_ZERO);
		GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE3, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_C0, GX_CC_CPREV, GX_CC_C1);
		GXSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_A0, GX_CA_APREV, GX_CA_ZERO);
		effGetTexObjN64(0x2F, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		effGetTexObjN64(0x30, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP1);
		break;
	case 2:
		GXSetNumTevStages(5);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG2);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG2);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
		GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_C0);
		GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_A2, GX_CA_TEXA, GX_CA_A2);
		GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
		GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_A1, GX_CA_A2, GX_CA_APREV);
		GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
		GXSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);
		GXSetTevOrder(GX_TEVSTAGE4, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE4, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE4, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE4, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
		GXSetTevAlphaIn(GX_TEVSTAGE4, GX_CA_ZERO, GX_CA_A1, GX_CA_A0, GX_CA_APREV);
		effGetTexObjN64(0x31, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
		effGetTexObjN64(0x32, &texObj);
		GXLoadTexObj(&texObj, GX_TEXMAP1);
		break;
	case 3:
		break;
	}

	// first half of the vertices
	PSMTXTrans(mtx1, data->position.x, data->position.y, data->position.z);
	if (type == 2) {
		PSMTXRotRad(mtxRot, 'y', data->rot * 0.017453292f);
	}
	else {
		PSMTXRotRad(mtxRot, 'y', -camGetPtr(CAMERA_3D)->viewYaw * 0.017453292f);
	}
	PSMTXConcat(mtx1, mtxRot, mtx3);
	PSMTXScale(mtx2, data->scaleX, data->scaleY, 1.0f);
	PSMTXConcat(mtx3, mtx2, mtx3);
	if (type == 0) {
		f32 d = data->baseScale;
		PSMTXTrans(mtx1, 10.0f * (-(data->scaleX - d) / d), 0.0f, 0.0f);
		PSMTXConcat(mtx3, mtx1, mtx3);
	}
	switch (type) {
	case 0:
		effFlameSetVtx(&data->vtx[2], mtx3, (effFlameVtx*)stg_flame_v, 2);
		effFlameSetVtx(&data->vtx[3], mtx3, (effFlameVtx*)stg_flame_v, 3);
		effFlameSetVtx(&data->vtx[6], mtx3, (effFlameVtx*)stg_flame_v, 6);
		effFlameSetVtx(&data->vtx[7], mtx3, (effFlameVtx*)stg_flame_v, 7);
		break;
	case 1:
		effFlameSetVtx(&data->vtx[2], mtx3, (effFlameVtx*)stg_flame_3_v, 2);
		effFlameSetVtx(&data->vtx[3], mtx3, (effFlameVtx*)stg_flame_3_v, 3);
		effFlameSetVtx(&data->vtx[6], mtx3, (effFlameVtx*)stg_flame_3_v, 6);
		effFlameSetVtx(&data->vtx[7], mtx3, (effFlameVtx*)stg_flame_3_v, 7);
		break;
	default:
		effFlameSetVtx(&data->vtx[2], mtx3, (effFlameVtx*)stg_flame_2_v, 2);
		effFlameSetVtx(&data->vtx[3], mtx3, (effFlameVtx*)stg_flame_2_v, 3);
		effFlameSetVtx(&data->vtx[6], mtx3, (effFlameVtx*)stg_flame_2_v, 6);
		effFlameSetVtx(&data->vtx[7], mtx3, (effFlameVtx*)stg_flame_2_v, 7);
		break;
	}

	// second half of the vertices
	PSMTXTrans(mtx1, data->position.x, data->position.y, data->position.z);
	if (type == 2) {
		PSMTXRotRad(mtxRot, 'y', data->rot * 0.017453292f);
	}
	else {
		PSMTXRotRad(mtxRot, 'y', -camGetPtr(CAMERA_3D)->viewYaw * 0.017453292f);
	}
	PSMTXConcat(mtx1, mtxRot, mtx3);
	PSMTXScale(mtx2, data->scaleX, data->scaleY, 1.0f);
	PSMTXConcat(mtx3, mtx2, mtx3);
	if (type == 0) {
		f32 d = data->baseScale;
		PSMTXTrans(mtx1, 10.0f * (-(data->scaleX - d) / d), 0.0f, 0.0f);
		PSMTXConcat(mtx3, mtx1, mtx3);
	}
	switch (type) {
	case 0:
		effFlameSetVtx(&data->vtx[0], mtx3, (effFlameVtx*)stg_flame_v, 0);
		effFlameSetVtx(&data->vtx[1], mtx3, (effFlameVtx*)stg_flame_v, 1);
		effFlameSetVtx(&data->vtx[4], mtx3, (effFlameVtx*)stg_flame_v, 4);
		effFlameSetVtx(&data->vtx[5], mtx3, (effFlameVtx*)stg_flame_v, 5);
		break;
	case 1:
		effFlameSetVtx(&data->vtx[0], mtx3, (effFlameVtx*)stg_flame_3_v, 0);
		effFlameSetVtx(&data->vtx[1], mtx3, (effFlameVtx*)stg_flame_3_v, 1);
		effFlameSetVtx(&data->vtx[4], mtx3, (effFlameVtx*)stg_flame_3_v, 4);
		effFlameSetVtx(&data->vtx[5], mtx3, (effFlameVtx*)stg_flame_3_v, 5);
		break;
	default:
		effFlameSetVtx(&data->vtx[0], mtx3, (effFlameVtx*)stg_flame_2_v, 0);
		effFlameSetVtx(&data->vtx[1], mtx3, (effFlameVtx*)stg_flame_2_v, 1);
		effFlameSetVtx(&data->vtx[4], mtx3, (effFlameVtx*)stg_flame_2_v, 4);
		effFlameSetVtx(&data->vtx[5], mtx3, (effFlameVtx*)stg_flame_2_v, 5);
		break;
	}

	GXInvalidateVtxCache();
	DCFlushRange(data->vtx, 0x70);
	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64((effVtxArray*)data->vtx);
	GXLoadPosMtxImm(cam->view, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);

	GXSetTevColor(GX_TEVREG0, (GXColor){ data->tev0Rgb[0], data->tev0Rgb[1], data->tev0Rgb[2], alpha });
	GXSetTevColor(GX_TEVREG1, (GXColor){ data->tev1Rgba[0], data->tev1Rgba[1], data->tev1Rgba[2], data->tev1Rgba[3] });

	PSMTXScale(mtx2, 0.03125f, 0.03125f, 0.0f);
	GXLoadTexMtxImm(mtx2, GX_TEXMTX0, GX_MTX2x4);
	PSMTXScale(mtx2, 0.03125f, 0.0078125f, 0.0f);
	PSMTXTrans(mtx1, data->scrollU, data->scrollV, 0.0f);
	PSMTXConcat(mtx2, mtx1, mtx1);
	GXLoadTexMtxImm(mtx1, GX_TEXMTX1, GX_MTX2x4);
	GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
	tri2(4, 5, 6, 0, 4, 6, 7, 0);

	PSMTXScale(mtx2, 0.03125f, 0.0078125f, 0.0f);
	PSMTXTrans(mtx1, 30.0f + data->scroll2U, 30.0f + data->scroll2V, 0.0f);
	PSMTXConcat(mtx2, mtx1, mtx1);
	GXLoadTexMtxImm(mtx1, GX_TEXMTX1, GX_MTX2x4);
	switch (type) {
	case 0:
	case 1:
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
		tri2(0, 1, 2, 0, 0, 2, 3, 0);
		break;
	default:
		GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
		tri2(2, 1, 0, 0, 3, 2, 0, 0);
		break;
	}
}