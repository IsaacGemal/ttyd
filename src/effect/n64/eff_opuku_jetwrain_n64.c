#include "eff/n64/eff_opuku_jetwrain_n64.h"
#include "eff/eff_n64.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/effdrv.h"
#include "memory.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include <dolphin/os/OSCache.h>

// .rodata
const Vec vec3_802fb248 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802fb254 = { 0.0f, 1.0f, 0.0f };
const Vec vec3_802fb260 = { 304.0f, 240.0f, 0.0f };
const Vec vec3_802fb26c = { 304.0f, 240.0f, 100.0f };
const f64 double_to_int_802fb278 = 4503601774854144.0;
const char str_802FB280[20] = "OpukuJetwrainN64";

// .sdata2 float constants (named after their original addresses).
const f32 float_0_8041f7d8 = 0.0f;
const f32 float_480_8041f7dc = 480.0f;
const f32 float_608_8041f7e0 = 608.0f;
const f32 float_1_8041f7e4 = 1.0f;
const f32 float_32768_8041f7e8 = 32768.0f;
const f32 float_0p0039062_8041f7ec = 0.00390625f;

// .data vertex array, modified in place by effOpukuJetwrainDisp.
u32 opuku_jetwrain_vtx[56] = {
	0xFF60FF88, 0xFEC00000, 0x00000000, 0x00FFFF70, 0xFF88FEC0, 0x02000000, 0x000000FF,
	0xFF60FF98, 0xFEC00000, 0x02000000, 0x00FFFF70, 0xFF98FEC0, 0x02000200, 0x000000FF,
	0xFF60FF88, 0xFEC00000, 0x00000000, 0x00FFFF70, 0xFF88FEC0, 0x02800000, 0x000000FF,
	0xFF60FF98, 0xFEC00000, 0x02000000, 0x00FFFF70, 0xFF98FEC0, 0x02800200, 0x000000FF,
	0xFF60FF88, 0xFEC00000, 0x00000000, 0x00FFFF70, 0xFF88FEC0, 0x02000000, 0x000000FF,
	0xFF60FF98, 0xFEC00000, 0x02800000, 0x00FFFF70, 0xFF98FEC0, 0x02000280, 0x000000FF,
	0xFF60FF88, 0xFEC00000, 0x00000000, 0x00FFFF70, 0xFF88FEC0, 0x02800000, 0x000000FF,
	0xFF60FF98, 0xFEC00000, 0x02800000, 0x00FFFF70, 0xFF98FEC0, 0x02800280, 0x000000FF,
};

// .sbss2
GXColor unk_804231d8;

EffectEntry* effOpukuJetwrainN64Entry(s32 type, s32 duration, f32 x, f32 y, f32 z, f32 scale) {
	EffectEntry* effect;
	effOpukuJetwrainData* data;
	s32 i;
	s32 j;

	effect = effEntry();
	effect->type = str_802FB280;
	effect->count = 1;
	data = __memAlloc(HEAP_EFFECT, 0x308);
	effect->userdata = data;
	effect->callback = effOpukuJetwrainMain;
	effect->flags |= 2;
	data->type = type;
	data->field_0x14 = 0;
	if (duration <= 0) {
		data->lifetime = 1000;
	}
	else {
		data->lifetime = duration;
	}
	data->field_0x1F = 0;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = scale;
	data->field_0x1C = 10;
	data->field_0x1D = 0x6e;
	data->field_0x1E = 0xff;
	data->field_0x20 = 0xbe;
	data->field_0x21 = 0xdc;
	data->field_0x22 = 0xff;
	for (i = 0; i < 19; i++) {
		for (j = 0; 13 > j; j++) {
			data->field_0x23[i][j] = 0;
			data->field_0x11A[i][j] = 0;
		}
	}
	return effect;
}

// Loop register allocation doesn't line up: iPos/fieldBase are swapped (r24/r26)
// and vtx/xBase/the 0x4330 double-magic temp are rotated (r29/r31/r30 vs r31/r30/r29).
// The GXColor for GXSetTevColor is built at 0x8(r1) by the original and copied to
// 0xc(r1); mwcc here builds it at 0xc(r1) and copies to 0x8(r1). mwcc also pools a
// duplicate of double_to_int_802fb278 in .rodata, so PSMTXTrans's f64 conversion
// loads it via vec3_802fb248+0x50 instead of +0x30.
// NONMATCHING (98.93%): codegen-only differences (see above).
void effOpukuJetwrainDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	s32 alpha;
	void* tex;
	effOpukuJetwrainVtx* vtx;
	s32 xBase;
	s32 jPos;
	s32 fieldEnd;
	s32 fieldBase;
	s32 iPos;
	s32 x;
	effOpukuJetwrainData* data;
	s32 j;
	s32 i;
	s32 outerEdge;
	s32 xEnd;
	s32 innerEdge;
	s32 x2;
	s32 base;
	smartEntry* entry;
	Mtx transMtx;
	Mtx texMtx;
	Mtx44 orthoMtx;
	Mtx lookAtMtx;
	GXTexObj texObj;
	f32 proj[7];
	Vec up;
	Vec target;
	Vec eye;
	GXColor color;

	camGetPtr(camId);
	data = effect->userdata;
	alpha = data->field_0x1F;
	GXGetProjectionv(proj);
	C_MTXOrtho(orthoMtx, float_0_8041f7d8, float_480_8041f7dc, float_0_8041f7d8, float_608_8041f7e0, float_1_8041f7e4, float_32768_8041f7e8);
	GXSetProjection(orthoMtx, GX_ORTHOGRAPHIC);
	GXSetZMode(0, GX_ALWAYS, 0);
	entry = smartAlloc(GXGetTexBufferSize(304, 240, GX_TF_RGB565, 0, 0), HEAP_EFFECT);
	tex = entry->address;
	GXSetTexCopySrc(0, 0, 608, 480);
	GXSetTexCopyDst(304, 240, GX_TF_RGB565, GX_TRUE);
	GXCopyTex(tex, 0);
	GXPixModeSync();
	GXInitTexObj(&texObj, tex, 304, 240, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, 0);
	GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, float_0_8041f7d8, float_0_8041f7d8, float_0_8041f7d8, 0, 0, GX_ANISO_1);
	GXLoadTexObj(&texObj, GX_TEXMAP0);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, 0, GX_SRC_REG, GX_SRC_VTX, 0, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GXSetTevColorOp(0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(0, GX_CC_ZERO, GX_CC_C0, GX_CC_A0, GX_CC_TEXC);
	GXSetTevAlphaIn(0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
	color = unk_804231d8;
	color.r = data->field_0x1C;
	color.g = data->field_0x1D;
	color.b = data->field_0x1E;
	color.a = alpha >> 1;
	GXSetTevColor(GX_TEVREG1, color);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXSetCullMode(GX_CULL_NONE);
	effSetVtxDescN64((effVtxArray*)opuku_jetwrain_vtx);
	up = vec3_802fb254;
	target = vec3_802fb260;
	eye = vec3_802fb26c;
	C_MTXLookAt(lookAtMtx, &eye, &up, &target);
	GXLoadPosMtxImm(lookAtMtx, 0);
	GXSetCurrentMtx(0);
	vtx = (effOpukuJetwrainVtx*)opuku_jetwrain_vtx;

	for (i = 0; i < 12; i++) {
		outerEdge = 0;
		if (i == 0) {
			xBase = -4;
			outerEdge = 1;
		} else {
			xBase = 0;
		}
		if (i == 11) {
			xEnd = 4;
			outerEdge = 1;
		} else {
			xEnd = 0;
		}
		iPos = 0x18 + i * 0x10;
		xBase += iPos;
		xEnd += iPos + 0x10;
		fieldBase = xBase << 2;
		fieldEnd = xEnd << 2;

		for (j = 0; j < 18; j++) {
			innerEdge = 0;
			if (j == 0) {
				x = -4;
				innerEdge = 1;
			} else {
				x = 0;
			}
			if (j == 17) {
				x2 = 4;
				innerEdge = 1;
			} else {
				x2 = 0;
			}
			jPos = 0x10 + j * 0x10;

			PSMTXScale(texMtx, float_0p0039062_8041f7ec, float_0p0039062_8041f7ec, float_1_8041f7e4);
			x += jPos;
			PSMTXTrans(transMtx, (f32)x, (f32)xBase, float_0_8041f7d8);
			PSMTXConcat(texMtx, transMtx, texMtx);
			GXLoadTexMtxImm(texMtx, GX_TEXMTX0, GX_MTX2x4);

			if (innerEdge && outerEdge) {
				base = 12;
			} else if (innerEdge) {
				base = 4;
			} else if (outerEdge) {
				base = 8;
			} else {
				base = 0;
			}

			vtx[base + 0].x = x << 2;
			vtx[base + 0].y = (s8)data->field_0x23[j][i] + fieldBase;
			vtx[base + 1].x = (jPos + x2 + 0x10) << 2;
			vtx[base + 1].y = (s8)data->field_0x23[j + 1][i] + fieldBase;
			vtx[base + 2].x = x << 2;
			vtx[base + 2].y = (s8)data->field_0x23[j][i + 1] + fieldEnd;
			vtx[base + 3].x = (jPos + x2 + 0x10) << 2;
			vtx[base + 3].y = (s8)data->field_0x23[j + 1][i + 1] + fieldEnd;
			DCFlushRange(opuku_jetwrain_vtx, 0xE0);
			GXInvalidateVtxCache();
			GXBegin(GX_TRIANGLES, GX_VTXFMT0, 6);
			tri2(base, base + 3, base + 1, 0, base, base + 2, base + 3, 0);
			sysWaitDrawSync();
		}
	}
	GXSetProjectionv(proj);
}
