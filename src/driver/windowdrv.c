/* TODO: macro "7" for numWindows or smth, tpl index macro/enum for easy waku editing */
#include "driver/windowdrv.h"
#include "dolphin/os.h"
#include "driver/arcdrv.h"
#include "driver/camdrv.h"
#include "memory.h"
#include "system.h"
#include "sdk/texPalette.h"
#include "math_ext.h"
#include <string.h>
#include <stdio.h>

#ifdef __MWERKS__
// Keep the SDK fabsf intrinsic without its unused sqrt inline constants.
#define fabsf(x) __fabsf(x)
#endif


static const GXColor dat_8041a430 = {0xFF, 0xFF, 0xFF, 0xFF};

//.sbss
// MUST_MATCH: MWCC emits uninitialised globals in reverse declaration order.
#ifdef MUST_MATCH
// Keep the original section tail even though no code references it.
#pragma force_active on
static u32 gap_08_8041880C_sbss;
#pragma force_active reset
#endif
WindowEntry* WinObjects;
GXTexObj* wakuTexObj;
f32 scrl;
static BOOL unk_804187fc; //new in retail
static TEXPalette* winTpl; //new in retail

//local prototypes

void windowInit(void) {
	WindowEntry* entry = __memAlloc(HEAP_DEFAULT, sizeof(WindowEntry) * 7);
	WinObjects = entry;
	entry[0].flags = 0;
	entry[0].userdata = NULL;
	entry[1].flags = 0;
	entry[1].userdata = NULL;
	entry[2].flags = 0;
	entry[2].userdata = NULL;
	entry[3].flags = 0;
	entry[3].userdata = NULL;
	entry[4].flags = 0;
	entry[4].userdata = NULL;
	entry[5].flags = 0;
	entry[5].userdata = NULL;
	entry[6].flags = 0;
	entry[6].userdata = NULL;
	winTpl = NULL;
	unk_804187fc = FALSE;
}

//new in retail
void unk_8007f204(s32 error, DVDFileInfo* info) {
	void* userdata = info->cb.userData;
	int i;

	UnpackTexPalette(winTpl);
	DVDMgrClose(userdata);
	wakuTexObj = __memAlloc(HEAP_DEFAULT, sizeof(GXTexObj) * winTpl->numDescriptors);

	for (i = 0; i < winTpl->numDescriptors; i++) {
		TEXGetGXTexObjFromPalette(winTpl, &wakuTexObj[i], i);
	}
	unk_804187fc = TRUE;
}

void windowTexSetup(void) {
	DVDEntry* entry;
	u32 size;
	char path[128];
	int i;

	winTpl = NULL;
	unk_804187fc = FALSE;
	winTpl = arcOpen("font/msgWindow.tpl", NULL, NULL);
	if (winTpl == NULL) {
		sprintf(path, "%s/f/msgWindow.tpl", getMarioStDvdRoot());
		entry = DVDMgrOpen(path, 2, 0);
		size = OSRoundUp32B(DVDMgrGetLength(entry));
		winTpl = __memAlloc(HEAP_DEFAULT, size);
		entry->info.cb.userData = entry;
		DVDMgrReadAsync(entry, winTpl, size, 0, unk_8007f204);
	}
	else {
		UnpackTexPalette(winTpl);
		wakuTexObj = __memAlloc(HEAP_DEFAULT, sizeof(GXTexObj) * winTpl->numDescriptors);

		for (i = 0; i < winTpl->numDescriptors; i++) {
			TEXGetGXTexObjFromPalette(winTpl, &wakuTexObj[i], i);
		}
	}
}

void windowReInit(void) {
	WindowEntry* entry = WinObjects;
	entry[0].flags = 0;
	entry[0].userdata = NULL;
	entry[1].flags = 0;
	entry[1].userdata = NULL;
	entry[2].flags = 0;
	entry[2].userdata = NULL;
	entry[3].flags = 0;
	entry[3].userdata = NULL;
	entry[4].flags = 0;
	entry[4].userdata = NULL;
	entry[5].flags = 0;
	entry[5].userdata = NULL;
	entry[6].flags = 0;
	entry[6].userdata = NULL;
}

s32 windowEntry(u16 a1) {
	WindowEntry* entry;
	int i;

	for (i = 0, entry = WinObjects; i < 7; i++, entry++) {
		if (!(entry->flags & 1)) {
			memset(entry, 0, sizeof(WindowEntry));
			entry->flags = 1;
			entry->field_0x4 = a1;
			return i;
		}
	}
	return -1;
}

BOOL windowDelete(WindowEntry* entry) {
	if (entry->delete) {
		entry->delete(entry);
	}
	entry->flags = 0;
	entry->userdata = NULL;
	return TRUE;
}

BOOL windowDeleteID(s32 id) {
	WindowEntry* entry = &WinObjects[id];
	if (!(entry->flags & 1)) {
		return FALSE;
	}
	if (entry->delete) {
		entry->delete(entry);
	}
	entry->flags = 0;
	entry->userdata = NULL;
	return TRUE;
}

void windowMain(void) {
	WindowEntry* entry;
	int i;

	for (i = 0, entry = WinObjects; i < 7; i++, entry++) {
		if (entry->flags & 1) {
			if (entry->main) {
				entry->main(entry);
			}
		}
	}
}

#ifdef MUST_MATCH
// MUST_MATCH: the original's outgoing GXColor argument slots need the fog colour passed through a
// by-value inline (fog temp at r1+0x8) and the tev colour passed as a direct compound literal (built at
// r1+0xc, copied to r1+0x10); the natural forms land in the other slots.
static inline void kanbanSetFog(GXColor color) {
	GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, color);
}
#endif

void windowDispGX_Kanban(s32 type, u8 alpha, f32 x, f32 y, f32 height, f32 width) {
	CameraEntry* camera;
	Mtx trans, scale;
	Mtx wood_mtx1, wood_mtx2, wood_mtx3, wood_mtx4; //+0x104, for case 2/4
	Mtx lines_mtx1, lines_mtx2, lines_mtx3, lines_mtx4; //+0x44, for case 10
	Mtx letter_mtx;
	GXColor tevColor;
	GXColor fogColor;

	camera = camGetCurPtr();
	GXSetCullMode(GX_CULL_NONE);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_OR);
	GXSetZMode(GX_TRUE, GX_ALWAYS, GX_FALSE);
	fogColor = dat_8041a430;
#ifdef MUST_MATCH
	kanbanSetFog(fogColor);
#else
	GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, fogColor);
#endif
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
	GXSetNumChans(0);
	GXSetNumTexGens(1);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
	GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
	GXSetCurrentMtx(0);
#ifdef MUST_MATCH
	GXSetTevColor(GX_TEVREG0, (GXColor){0xFF, 0xFF, 0xFF, alpha});
#else
	tevColor = (GXColor){0xFF, 0xFF, 0xFF, alpha};
	GXSetTevColor(GX_TEVREG0, tevColor);
#endif
	MTXTrans(trans, x, y, 0.0f);
	MTXScale(scale, height / 560.0f, width / 176.0f, 1.0f);
	MTXConcat(trans, scale, trans);
	MTXConcat(camera->view, trans, trans);
	GXLoadPosMtxImm(trans, 0);
	GXSetCurrentMtx(0);

	switch (type) {
		case WINDOW_TYPE_PLATE: //images 29, 30, 31, 32
		case WINDOW_TYPE_KANBAN:
			//left side of the frame
			GXSetNumTexGens(1);
			GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
			MTXScale(wood_mtx1, fabsf(72.0f) / (f32)GXGetTexObjWidth(&wakuTexObj[29]),
				fabsf(176.0f) / (f32)GXGetTexObjHeight(&wakuTexObj[29]), 1.0f);
			GXLoadTexMtxImm(wood_mtx1, 0x1E, GX_MTX2x4);
			GXLoadTexObj(&wakuTexObj[29], GX_TEXMAP0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);

			GXPosition3f32(0.0f, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);

			GXPosition3f32(72.0f, 0.0f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);

			GXPosition3f32(72.0f, -176.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);

			GXPosition3f32(0.0f, -176.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);

			//top side of the frame
			GXSetNumTexGens(1);
			GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
			MTXScale(wood_mtx2, fabsf(416.0f) / (f32)GXGetTexObjWidth(&wakuTexObj[31]),
				fabsf(136.0f) / (f32)GXGetTexObjHeight(&wakuTexObj[31]), 1.0f);
			GXLoadTexMtxImm(wood_mtx2, 0x1E, GX_MTX2x4);
			GXLoadTexObj(&wakuTexObj[31], GX_TEXMAP0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);

			GXPosition3f32(72.0f, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);

			GXPosition3f32(488.0f, 0.0f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);

			GXPosition3f32(488.0f, -136.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);

			GXPosition3f32(72.0f, -136.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
		
			//right side of the frame
			GXSetNumTexGens(1);
			GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
			MTXScale(wood_mtx3, fabsf(72.0f) / (f32)GXGetTexObjWidth(&wakuTexObj[30]),
				fabsf(176.0f) / (f32)GXGetTexObjHeight(&wakuTexObj[30]), 1.0f);
			GXLoadTexMtxImm(wood_mtx3, 0x1E, GX_MTX2x4);
			GXLoadTexObj(&wakuTexObj[30], GX_TEXMAP0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);

			GXPosition3f32(488.0f, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);

			GXPosition3f32(560.0f, 0.0f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);

			GXPosition3f32(560.0f, -176.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);

			GXPosition3f32(488.0f, -176.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);

			//bottom side of the frame
			GXSetNumTexGens(1);
			GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
			MTXScale(wood_mtx4, fabsf(416.0f) / (f32)GXGetTexObjWidth(&wakuTexObj[32]),
				fabsf(40.0f) / (f32)GXGetTexObjHeight(&wakuTexObj[32]), 1.0f);
			GXLoadTexMtxImm(wood_mtx4, 0x1E, GX_MTX2x4);
			GXLoadTexObj(&wakuTexObj[32], GX_TEXMAP0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);

			GXPosition3f32(72.0f, -136.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);

			GXPosition3f32(488.0f, -136.0f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);

			GXPosition3f32(488.0f, -176.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);

			GXPosition3f32(72.0f, -176.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
			break;

		case WINDOW_TYPE_HOUSOU: //images 25, 26, 27, 28
			//left side of the frame
			GXSetNumTexGens(1);
			GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
			MTXScale(lines_mtx1, fabsf(72.0f) / (f32)GXGetTexObjWidth(&wakuTexObj[25]),
				fabsf(176.0f) / (f32)GXGetTexObjHeight(&wakuTexObj[25]), 1.0f);
			GXLoadTexMtxImm(lines_mtx1, 0x1E, GX_MTX2x4);
			GXLoadTexObj(&wakuTexObj[25], GX_TEXMAP0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);

			GXPosition3f32(0.0f, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);

			GXPosition3f32(72.0f, 0.0f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);

			GXPosition3f32(72.0f, -176.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);

			GXPosition3f32(0.0f, -176.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);

			//top side of the frame
			GXSetNumTexGens(1);
			GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
			MTXScale(lines_mtx2, fabsf(416.0f) / (f32)GXGetTexObjWidth(&wakuTexObj[27]),
				fabsf(136.0f) / (f32)GXGetTexObjHeight(&wakuTexObj[27]), 1.0f);
			GXLoadTexMtxImm(lines_mtx2, 0x1E, GX_MTX2x4);
			GXLoadTexObj(&wakuTexObj[27], GX_TEXMAP0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);

			GXPosition3f32(72.0f, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);

			GXPosition3f32(488.0f, 0.0f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);

			GXPosition3f32(488.0f, -136.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);

			GXPosition3f32(72.0f, -136.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);

			//right side of the frame
			GXSetNumTexGens(1);
			GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
			MTXScale(lines_mtx3, fabsf(72.0f) / (f32)GXGetTexObjWidth(&wakuTexObj[26]),
				fabsf(176.0f) / (f32)GXGetTexObjHeight(&wakuTexObj[26]), 1.0f);
			GXLoadTexMtxImm(lines_mtx3, 0x1E, GX_MTX2x4);
			GXLoadTexObj(&wakuTexObj[26], GX_TEXMAP0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);

			GXPosition3f32(488.0f, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);

			GXPosition3f32(560.0f, 0.0f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);

			GXPosition3f32(560.0f, -176.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);

			GXPosition3f32(488.0f, -176.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);

			//bottom side of the frame
			GXSetNumTexGens(1);
			GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
			MTXScale(lines_mtx4, fabsf(416.0f) / (f32)GXGetTexObjWidth(&wakuTexObj[28]),
				fabsf(40.0f) / (f32)GXGetTexObjHeight(&wakuTexObj[28]), 1.0f);
			GXLoadTexMtxImm(lines_mtx4, 0x1E, GX_MTX2x4);
			GXLoadTexObj(&wakuTexObj[28], GX_TEXMAP0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);

			GXPosition3f32(72.0f, -136.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);

			GXPosition3f32(488.0f, -136.0f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);

			GXPosition3f32(488.0f, -176.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);

			GXPosition3f32(72.0f, -176.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
			break;

		case WINDOW_TYPE_DIARY: //image 6
			GXSetNumTexGens(1);
			GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
			MTXScale(letter_mtx, fabsf(560.0f) / (f32)GXGetTexObjWidth(&wakuTexObj[6]),
				fabsf(176.0f) / (f32)GXGetTexObjHeight(&wakuTexObj[6]), 1.0f);
			GXLoadTexMtxImm(letter_mtx, 0x1E, GX_MTX2x4);
			GXLoadTexObj(&wakuTexObj[6], GX_TEXMAP0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);

			GXPosition3f32(0.0f, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);

			GXPosition3f32(560.0f, 0.0f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);

			GXPosition3f32(560.0f, -176.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);

			GXPosition3f32(0.0f, -176.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
			break;

		default:
			break;
	}
}

// MUST_MATCH: a named colour passed through an inline reproduces the original's stack layout for
// GXSetFog's by-value GXColor argument (same trick as kanbanSetFog/wakuSetFog).
#ifdef MUST_MATCH
static inline void windowSetFog(GXColor color) {
	GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, color);
}
#endif

void windowDispGX_System(s32 type, u8 alpha, f32 x, f32 y, f32 height, f32 width) {
	CameraEntry* camera;
#ifdef MUST_MATCH
	f32 zero;
#endif
	f32 scroll;
	Mtx mtx;
	Mtx trans, scale;
	f32 x2, y2;
	Mtx trans2, scale2;
	f32 y3, y4;
	Mtx trans3, scale3;
	f32 x3, x4;
	Mtx trans4, scale4;
	f32 x5;

	camera = camGetCurPtr();
	scrl += 0.005f;
	if (scrl > 10.0f) {
		scrl -= 10.0f;
	}
	GXSetCullMode(GX_CULL_NONE);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_OR);
	GXSetZMode(GX_TRUE, GX_ALWAYS, GX_FALSE);
#ifdef MUST_MATCH
	windowSetFog(dat_8041a430);
#else
	GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, dat_8041a430);
#endif
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
	GXSetNumChans(0);
	GXSetNumTexGens(1);

	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
	GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
	GXSetCurrentMtx(0);

	GXSetTevColor(GX_TEVREG0, (GXColor){0xFF, 0xFF, 0xFF, alpha});
	GXSetNumTevStages(4);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
	GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);

	GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
	GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_TEXA, GX_CA_ZERO);
	GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);

	GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD1, GX_TEXMAP2, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_CPREV, GX_CC_TEXC, GX_CC_TEXA, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
	GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);

	GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_CPREV, GX_CC_C0, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_APREV, GX_CA_A0, GX_CA_ZERO);
	GXSetTevSwapMode(GX_TEVSTAGE3, GX_TEV_SWAP0, GX_TEV_SWAP0);

	MTXTrans(mtx, x, y, 0.0f);
	MTXConcat(camera->view, mtx, mtx);
	GXLoadPosMtxImm(mtx, 0);
	GXSetCurrentMtx(0);

	//top left of the frame
	scroll = scrl;
	GXLoadTexObj(&wakuTexObj[5], GX_TEXMAP0);
	GXLoadTexObj(&wakuTexObj[42], GX_TEXMAP1);
	GXLoadTexObj(&wakuTexObj[46], GX_TEXMAP2);

	GXSetNumTexGens(2);
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
	MTXTrans(trans, scroll + (0.0f / (f32)GXGetTexObjWidth(&wakuTexObj[5])),
		(-0.0f / (f32)GXGetTexObjHeight(&wakuTexObj[5])) - scroll, 0.0f);
	MTXScale(scale, fabsf(height - 32.0f) / (f32)GXGetTexObjWidth(&wakuTexObj[5]),
		fabsf(width - 32.0f) / (f32)GXGetTexObjHeight(&wakuTexObj[5]), 1.0f);
	MTXConcat(trans, scale, trans);
	GXLoadTexMtxImm(trans, 0x1E, GX_MTX2x4);
	GXSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1);
	MTXScale(scale, fabsf(height - 32.0f) / (f32)GXGetTexObjWidth(&wakuTexObj[42]),
		fabsf(width - 32.0f) / (f32)GXGetTexObjHeight(&wakuTexObj[42]), 1.0f);
	GXLoadTexMtxImm(scale, 0x21, GX_MTX2x4);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);

	GXPosition3f32(0.0f, 0.0f, 0.0f);
	GXTexCoord2f32(0.0f, 0.0f);
#ifdef MUST_MATCH
	zero = 0.0f;
	// MUST_MATCH: `zero` (a real local, not the literal 0.0f) stops MWCC folding 0.0f + x away, which
	// the original's fadds/fsubs pairs require; computing x2/y2 after the first vertex also fixes the
	// target's callee-saved FP allocation for this block.
	x2 = zero + (height - 32.0f);
	y2 = zero - (width - 32.0f);
#else
	x2 = 0.0f + (height - 32.0f);
	y2 = 0.0f - (width - 32.0f);
#endif

	GXPosition3f32(x2, 0.0f, 0.0f); //f29 = x2, 0.0f + (height - 32.0f)
	GXTexCoord2f32(1.0f, 0.0f);

	GXPosition3f32(x2, y2, 0.0f); //f28 = y2, 0.0f - (width - 32.0f)
	GXTexCoord2f32(1.0f, 1.0f);

	GXPosition3f32(0.0f, y2, 0.0f);
	GXTexCoord2f32(0.0f, 1.0f);

	//bottom left of the frame
	scroll = scrl;
	GXLoadTexObj(&wakuTexObj[5], GX_TEXMAP0);
	GXLoadTexObj(&wakuTexObj[43], GX_TEXMAP1);
	GXLoadTexObj(&wakuTexObj[47], GX_TEXMAP2);

	GXSetNumTexGens(2);
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
	MTXTrans(trans2, scroll + (0.0f / (f32)GXGetTexObjWidth(&wakuTexObj[5])),
		((width - 32.0f) / (f32)GXGetTexObjHeight(&wakuTexObj[5])) - scroll, 0.0f);
	MTXScale(scale2, fabsf(height - 32.0f) / (f32)GXGetTexObjWidth(&wakuTexObj[5]),
		fabsf(32.0f) / (f32)GXGetTexObjHeight(&wakuTexObj[5]), 1.0f);
	MTXConcat(trans2, scale2, trans2);
	GXLoadTexMtxImm(trans2, 0x1E, GX_MTX2x4);
	GXSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1);
	MTXScale(scale2, fabsf(height - 32.0f) / (f32)GXGetTexObjWidth(&wakuTexObj[43]),
		fabsf(32.0f) / (f32)GXGetTexObjHeight(&wakuTexObj[43]), 1.0f);
	GXLoadTexMtxImm(scale2, 0x21, GX_MTX2x4);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);

#ifdef MUST_MATCH
	// MUST_MATCH: interleave vertex setup with FIFO writes to reproduce the original scratch FPRs.
	GXWGFifo.f32 = 0.0f;
	y3 = -(width - 32.0f);
	GXWGFifo.f32 = y3;
	y4 = y3 - 32.0f;
	GXWGFifo.f32 = 0.0f;
#else
	y3 = -(width - 32.0f);
	y4 = y3 - 32.0f;

	GXPosition3f32(0.0f, y3, 0.0f);
#endif
	GXTexCoord2f32(0.0f, 0.0f);

	GXPosition3f32(x2, y3, 0.0f);
	GXTexCoord2f32(1.0f, 0.0f);

	GXPosition3f32(x2, y4, 0.0f); //f26 = y4, (y2 - 32.0f)
	GXTexCoord2f32(1.0f, 1.0f);

	GXPosition3f32(0.0f, y4, 0.0f);
	GXTexCoord2f32(0.0f, 1.0f);

	//top right of the frame
	scroll = scrl;
	GXLoadTexObj(&wakuTexObj[5], GX_TEXMAP0);
	GXLoadTexObj(&wakuTexObj[44], GX_TEXMAP1);
	GXLoadTexObj(&wakuTexObj[48], GX_TEXMAP2);

	GXSetNumTexGens(2);
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
	MTXTrans(trans3, scroll + ((height - 32.0f) / (f32)GXGetTexObjWidth(&wakuTexObj[5])),
		(-0.0f / (f32)GXGetTexObjHeight(&wakuTexObj[5])) - scroll, 0.0f);
	MTXScale(scale3, fabsf(32.0f) / (f32)GXGetTexObjWidth(&wakuTexObj[5]),
		fabsf(width - 32.0f) / (f32)GXGetTexObjHeight(&wakuTexObj[5]), 1.0f);
	MTXConcat(trans3, scale3, trans3);
	GXLoadTexMtxImm(trans3, 0x1E, GX_MTX2x4);
	GXSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1);
	MTXScale(scale3, fabsf(32.0f) / (f32)GXGetTexObjWidth(&wakuTexObj[44]),
		fabsf(width - 32.0f) / (f32)GXGetTexObjHeight(&wakuTexObj[44]), 1.0f);
	GXLoadTexMtxImm(scale3, 0x21, GX_MTX2x4);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);

	x3 = height - 32.0f;
#ifdef MUST_MATCH
	// MUST_MATCH: splitting the add into two statements makes MWCC encode the fadds as `x3 + 32.0f`
	// (fadds f25, f1, f0); the natural single expression comes out with the operands swapped.
	x4 = x3;
	x4 += 32.0f; //f25 = x4
#else
	x4 = x3 + 32.0f; //f25 = x4
#endif

	GXPosition3f32(x3, 0.0f, 0.0f);
	GXTexCoord2f32(0.0f, 0.0f);

	GXPosition3f32(x4, 0.0f, 0.0f);
	GXTexCoord2f32(1.0f, 0.0f);

	GXPosition3f32(x4, y2, 0.0f);
	GXTexCoord2f32(1.0f, 1.0f);

	GXPosition3f32(x3, y2, 0.0f);
	GXTexCoord2f32(0.0f, 1.0f);

	//bottom right of the frame
	scroll = scrl;
	GXLoadTexObj(&wakuTexObj[5], GX_TEXMAP0);
	GXLoadTexObj(&wakuTexObj[45], GX_TEXMAP1);
	GXLoadTexObj(&wakuTexObj[49], GX_TEXMAP2);

	GXSetNumTexGens(2);
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
	MTXTrans(trans4, scroll + ((height - 32.0f) / (f32)GXGetTexObjWidth(&wakuTexObj[5])),
		((width - 32.0f) / (f32)GXGetTexObjHeight(&wakuTexObj[5])) - scroll, 0.0f);
	MTXScale(scale4, fabsf(32.0f) / (f32)GXGetTexObjWidth(&wakuTexObj[5]),
		fabsf(32.0f) / (f32)GXGetTexObjHeight(&wakuTexObj[5]), 1.0f);
	MTXConcat(trans4, scale4, trans4);
	GXLoadTexMtxImm(trans4, 0x1E, GX_MTX2x4);
	GXSetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX1);
	MTXScale(scale4, fabsf(32.0f) / (f32)GXGetTexObjWidth(&wakuTexObj[45]),
		fabsf(32.0f) / (f32)GXGetTexObjHeight(&wakuTexObj[45]), 1.0f);
	GXLoadTexMtxImm(scale4, 0x21, GX_MTX2x4);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);

	x5 = height - 32.0f;

	GXPosition3f32(x5, y3, 0.0f);
	GXTexCoord2f32(0.0f, 0.0f);

	GXPosition3f32(x4, y3, 0.0f);
	GXTexCoord2f32(1.0f, 0.0f);

	GXPosition3f32(x4, y4, 0.0f);
	GXTexCoord2f32(1.0f, 1.0f);

	GXPosition3f32(x5, y4, 0.0f);
	GXTexCoord2f32(0.0f, 1.0f);
}

// 100% match; the parameter copies and block-local indices below are MUST_MATCH allocation/branch hints.
void _windowDispGX_Message(s32 type, s32 a2, u8 alpha, BOOL dark, f32 x, f32 y, f32 height, f32 width, f32 a9, f32 a10) {
#ifdef MUST_MATCH
	// MUST_MATCH: copying the parameters in this order reproduces the original's callee-saved FP
	// allocation (x->f27, y->f28, height->f29, width->f30, a9->f31, a10->f26). Without them the
	// allocator gives a9 f27 first and every callee-saved register is rotated.
	f32 px=x, py=y, ph=height, pw=width, pa9=a9, pa10=a10;
#else
	#define px x
	#define py y
	#define ph height
	#define pw width
	#define pa9 a9
	#define pa10 a10
#endif
	CameraEntry* camera;
	Mtx trans, scale;
	Mtx norm_mtx1, norm_mtx2, norm_mtx3, norm_mtx4;
	Mtx boss_mtx1, boss_mtx2, boss_mtx3, boss_mtx4;
	Mtx tec_mtx1, tec_mtx2;
	Mtx majo_mtx1, majo_mtx2, majo_mtx3, majo_mtx4;
	f32 y2, w;

	camera = camGetCurPtr();
	GXSetTevColor(GX_TEVREG0, (GXColor){0xFF, 0xFF, 0xFF, alpha});
	MTXTrans(trans, px, py, 0.0f);
	MTXScale(scale, ph / 560.0f, pw / 176.0f, 1.0f);
	MTXConcat(trans, scale, trans);
	MTXConcat(camera->view, trans, trans);
	GXLoadPosMtxImm(trans, 0);
	GXSetCurrentMtx(0);

	switch (type) {
		case WINDOW_TYPE_NORMAL: //images 9-12, 13-16 for light/dark
			//left side
			GXSetNumTexGens(1);
			GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
			MTXScale(norm_mtx1, fabsf(72.0f) / GXGetTexObjWidth(&wakuTexObj[dark ? 13 : 9]),
				fabsf(176.0f) / GXGetTexObjHeight(&wakuTexObj[dark ? 13 : 9]), 1.0f);
			GXLoadTexMtxImm(norm_mtx1, 0x1E, GX_MTX2x4);
			GXLoadTexObj(&wakuTexObj[dark ? 13 : 9], GX_TEXMAP0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);

			GXPosition3f32(0.0f, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);

			GXPosition3f32(72.0f, 0.0f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);

			GXPosition3f32(72.0f, -176.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);

			GXPosition3f32(0.0f, -176.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);

			//top side
			GXSetNumTexGens(1);
			GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
			MTXScale(norm_mtx2, fabsf(416.0f) / GXGetTexObjWidth(&wakuTexObj[dark ? 15 : 11]),
				fabsf(136.0f) / GXGetTexObjHeight(&wakuTexObj[dark ? 15 : 11]), 1.0f);
			GXLoadTexMtxImm(norm_mtx2, 0x1E, GX_MTX2x4);
			GXLoadTexObj(&wakuTexObj[dark ? 15 : 11], GX_TEXMAP0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);

			GXPosition3f32(72.0f, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);

			GXPosition3f32(488.0f, 0.0f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);

			GXPosition3f32(488.0f, -136.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);

			GXPosition3f32(72.0f, -136.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);

			//right side
			GXSetNumTexGens(1);
			GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
			MTXScale(norm_mtx3, fabsf(72.0f) / GXGetTexObjWidth(&wakuTexObj[dark ? 14 : 10]),
				fabsf(176.0f) / GXGetTexObjHeight(&wakuTexObj[dark ? 14 : 10]), 1.0f);
			GXLoadTexMtxImm(norm_mtx3, 0x1E, GX_MTX2x4);
			GXLoadTexObj(&wakuTexObj[dark ? 14 : 10], GX_TEXMAP0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);

			GXPosition3f32(488.0f, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);

			GXPosition3f32(560.0f, 0.0f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);

			GXPosition3f32(560.0f, -176.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);

			GXPosition3f32(488.0f, -176.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);

			//bottom side
			GXSetNumTexGens(1);
			GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
			MTXScale(norm_mtx4, fabsf(416.0f) / GXGetTexObjWidth(&wakuTexObj[dark ? 16 : 12]),
				fabsf(40.0f) / GXGetTexObjHeight(&wakuTexObj[dark ? 16 : 12]), 1.0f);
			GXLoadTexMtxImm(norm_mtx4, 0x1E, GX_MTX2x4);
			GXLoadTexObj(&wakuTexObj[dark ? 16 : 12], GX_TEXMAP0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);

			GXPosition3f32(72.0f, -136.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);

			GXPosition3f32(488.0f, -136.0f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);

			GXPosition3f32(488.0f, -176.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);

			GXPosition3f32(72.0f, -176.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
			break;

		case WINDOW_TYPE_SMALL: //images 3, 4
				// MUST_MATCH: a block-local index reproduces the original's branch instead of a select.
#ifdef MUST_MATCH
				{ GXTexObj* o = wakuTexObj; s32 i = 3; if (dark) i = 4; GXLoadTexObj(&o[i], GX_TEXMAP0); }
#else
				GXLoadTexObj(&wakuTexObj[dark ? 4 : 3], GX_TEXMAP0);
#endif
			MTXScale(trans, ph, pw, 1.0f);
			MTXTransApply(trans, trans, px, py, 0.0f);
			MTXConcat(camera->view, trans, trans);
			GXLoadPosMtxImm(trans, 0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4u);

			GXPosition3f32(0.0f, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);

			GXPosition3f32(1.0f, 0.0f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);

			GXPosition3f32(1.0f, -1.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);

			GXPosition3f32(0.0f, -1.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
			break;

		case WINDOW_TYPE_BOSS: //images 17-20, 21-24
			//left side
			GXSetNumTexGens(1);
			GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
			MTXScale(boss_mtx1, fabsf(72.0f) / GXGetTexObjWidth(&wakuTexObj[dark ? 21 : 17]),
				fabsf(176.0f) / GXGetTexObjHeight(&wakuTexObj[dark ? 21 : 17]), 1.0f);
			GXLoadTexMtxImm(boss_mtx1, 0x1E, GX_MTX2x4);
			GXLoadTexObj(&wakuTexObj[dark ? 21 : 17], GX_TEXMAP0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);

			GXPosition3f32(0.0f, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);

			GXPosition3f32(72.0f, 0.0f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);

			GXPosition3f32(72.0f, -176.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);

			GXPosition3f32(0.0f, -176.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);

			//top side
			GXSetNumTexGens(1);
			GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
			MTXScale(boss_mtx2, fabsf(416.0f) / GXGetTexObjWidth(&wakuTexObj[dark ? 23 : 19]),
				fabsf(136.0f) / GXGetTexObjHeight(&wakuTexObj[dark ? 23 : 19]), 1.0f);
			GXLoadTexMtxImm(boss_mtx2, 0x1E, GX_MTX2x4);
			GXLoadTexObj(&wakuTexObj[dark ? 23 : 19], GX_TEXMAP0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);

			GXPosition3f32(72.0f, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);

			GXPosition3f32(488.0f, 0.0f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);

			GXPosition3f32(488.0f, -136.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);

			GXPosition3f32(72.0f, -136.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);

			//right side
			GXSetNumTexGens(1);
			GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
			MTXScale(boss_mtx3, fabsf(72.0f) / GXGetTexObjWidth(&wakuTexObj[dark ? 22 : 18]),
				fabsf(176.0f) / GXGetTexObjHeight(&wakuTexObj[dark ? 22 : 18]), 1.0f);
			GXLoadTexMtxImm(boss_mtx3, 0x1E, GX_MTX2x4);
			GXLoadTexObj(&wakuTexObj[dark ? 22 : 18], GX_TEXMAP0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);

			GXPosition3f32(488.0f, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);

			GXPosition3f32(560.0f, 0.0f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);

			GXPosition3f32(560.0f, -176.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);

			GXPosition3f32(488.0f, -176.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);

			//bottom side
			GXSetNumTexGens(1);
			GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
			MTXScale(boss_mtx4, fabsf(416.0f) / GXGetTexObjWidth(&wakuTexObj[dark ? 24 : 20]),
				fabsf(40.0f) / GXGetTexObjHeight(&wakuTexObj[dark ? 24 : 20]), 1.0f);
			GXLoadTexMtxImm(boss_mtx4, 0x1E, GX_MTX2x4);
			GXLoadTexObj(&wakuTexObj[dark ? 24 : 20], GX_TEXMAP0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);

			GXPosition3f32(72.0f, -136.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);

			GXPosition3f32(488.0f, -136.0f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);

			GXPosition3f32(488.0f, -176.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);

			GXPosition3f32(72.0f, -176.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
			break;

		case WINDOW_TYPE_TEC: //image 41
			//left(?) part
			GXSetNumTexGens(1);
			GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
			MTXScale(tec_mtx1, fabsf(536.0f) / GXGetTexObjWidth(&wakuTexObj[41]),
				fabsf(176.0f) / GXGetTexObjHeight(&wakuTexObj[41]), 1.0f);
			GXLoadTexMtxImm(tec_mtx1, 0x1E, GX_MTX2x4);
			GXLoadTexObj(&wakuTexObj[41], GX_TEXMAP0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);

			GXPosition3f32(0.0f, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);

			GXPosition3f32(536.0f, 0.0f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);

			GXPosition3f32(536.0f, -176.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);

			GXPosition3f32(0.0f, -176.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);

			//right(?) part
			GXSetNumTexGens(1);
			GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
			MTXScale(tec_mtx2, fabsf(-24.0f) / GXGetTexObjWidth(&wakuTexObj[41]),
				fabsf(176.0f) / GXGetTexObjHeight(&wakuTexObj[41]), 1.0f);
			GXLoadTexMtxImm(tec_mtx2, 0x1E, GX_MTX2x4);
			GXLoadTexObj(&wakuTexObj[41], GX_TEXMAP0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);

			GXPosition3f32(560.0f, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);

			GXPosition3f32(536.0f, 0.0f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);

			GXPosition3f32(536.0f, -176.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);

			GXPosition3f32(560.0f, -176.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
			break;

		case WINDOW_TYPE_MAJO: //images 33-36, 37-40
			//left side
			GXSetNumTexGens(1);
			GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
			MTXScale(majo_mtx1, fabsf(72.0f) / GXGetTexObjWidth(&wakuTexObj[dark ? 37 : 33]),
				fabsf(176.0f) / GXGetTexObjHeight(&wakuTexObj[dark ? 37 : 33]), 1.0f);
			GXLoadTexMtxImm(majo_mtx1, 0x1E, GX_MTX2x4);
			GXLoadTexObj(&wakuTexObj[dark ? 37 : 33], GX_TEXMAP0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);

			GXPosition3f32(0.0f, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);

			GXPosition3f32(72.0f, 0.0f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);

			GXPosition3f32(72.0f, -176.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);

			GXPosition3f32(0.0f, -176.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);

			//top side
			GXSetNumTexGens(1);
			GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
			MTXScale(majo_mtx2, fabsf(416.0f) / GXGetTexObjWidth(&wakuTexObj[dark ? 39 : 35]),
				fabsf(136.0f) / GXGetTexObjHeight(&wakuTexObj[dark ? 39 : 35]), 1.0f);
			GXLoadTexMtxImm(majo_mtx2, 0x1E, GX_MTX2x4);
			GXLoadTexObj(&wakuTexObj[dark ? 39 : 35], GX_TEXMAP0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);

			GXPosition3f32(72.0f, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);

			GXPosition3f32(488.0f, 0.0f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);

			GXPosition3f32(488.0f, -136.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);

			GXPosition3f32(72.0f, -136.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);

			//right side
			GXSetNumTexGens(1);
			GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
			MTXScale(majo_mtx3, fabsf(72.0f) / GXGetTexObjWidth(&wakuTexObj[dark ? 38 : 34]),
				fabsf(176.0f) / GXGetTexObjHeight(&wakuTexObj[dark ? 38 : 34]), 1.0f);
			GXLoadTexMtxImm(majo_mtx3, 0x1E, GX_MTX2x4);
			GXLoadTexObj(&wakuTexObj[dark ? 38 : 34], GX_TEXMAP0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);

			GXPosition3f32(488.0f, 0.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);

			GXPosition3f32(560.0f, 0.0f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);

			GXPosition3f32(560.0f, -176.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);

			GXPosition3f32(488.0f, -176.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);

			//bottom side
			GXSetNumTexGens(1);
			GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
			MTXScale(majo_mtx4, fabsf(416.0f) / GXGetTexObjWidth(&wakuTexObj[dark ? 40 : 36]),
				fabsf(40.0f) / GXGetTexObjHeight(&wakuTexObj[dark ? 40 : 36]), 1.0f);
			GXLoadTexMtxImm(majo_mtx4, 0x1E, GX_MTX2x4);
			GXLoadTexObj(&wakuTexObj[dark ? 40 : 36], GX_TEXMAP0);
			GXBegin(GX_QUADS, GX_VTXFMT0, 4);

			GXPosition3f32(72.0f, -136.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);

			GXPosition3f32(488.0f, -136.0f, 0.0f);
			GXTexCoord2f32(1.0f, 0.0f);

			GXPosition3f32(488.0f, -176.0f, 0.0f);
			GXTexCoord2f32(1.0f, 1.0f);

			GXPosition3f32(72.0f, -176.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
			break;

		default:
			break;
	}
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
	//TODO: cleanup, rename vars
	if (a2 & 1) {
		if (a2 & 4) {
			px = (0.7f * ph) + px;
		}
		else {
			px = (0.38f * ph) + px;
		}
		
		if (a2 & 8) {
			y2 = -((0.175f * pw) - py);
			py = ((0.225f * pw) + py);
			if (y2 >= pa10)
#ifdef MUST_MATCH
				goto ret;
#else
				return;
#endif
		}
		else {
			y2 = -((0.8f * pw) - py);
			py = -((1.2f * pw) - py);
			if (y2 <= pa10)
#ifdef MUST_MATCH
				goto ret;
#else
				return;
#endif
		}
		w = (px - (((px - pa9) * (y2 - py)) / (y2 - pa10)));
		if (pa9 >= px) {
			if (w > pa9) {
				w = pa9;
			}
		}
		else {
			if (w < pa9) {
				w = pa9;
			}
		}
		pw = w;
		
		switch (type) {
			case WINDOW_TYPE_NORMAL:
			case WINDOW_TYPE_SMALL:
			case WINDOW_TYPE_BOSS:
				// MUST_MATCH: a block-local index reproduces the original's branch instead of a select.
#ifdef MUST_MATCH
				{ GXTexObj* o = wakuTexObj; s32 i = 1; if (dark) i = 2; GXLoadTexObj(&o[i], GX_TEXMAP0); }
#else
				GXLoadTexObj(&wakuTexObj[dark ? 2 : 1], GX_TEXMAP0);
#endif
				break;

			case WINDOW_TYPE_MAJO:
				// MUST_MATCH: a block-local index reproduces the original's branch instead of a select.
#ifdef MUST_MATCH
				{ GXTexObj* o = wakuTexObj; s32 i = 7; if (dark) i = 8; GXLoadTexObj(&o[i], GX_TEXMAP0); }
#else
				GXLoadTexObj(&wakuTexObj[dark ? 8 : 7], GX_TEXMAP0);
#endif
				break;

			default:
				break;
		}
		
		GXLoadPosMtxImm(camera->view, 0);
		GXBegin(GX_QUADS, GX_VTXFMT0, 4u);
		
		ph = (0.05f * ph);
		
		GXPosition3f32(px - ph, y2, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);
		
		GXPosition3f32(px + ph, y2, 0.0f);
		GXTexCoord2f32(1.0f, 0.0f);
		
		GXPosition3f32(pw + ph, py, 0.0f);
		GXTexCoord2f32(1.0f, 1.0f);
		
		GXPosition3f32(pw - ph, py, 0.0f);
		GXTexCoord2f32(0.0f, 1.0f);
	}
#ifdef MUST_MATCH
ret:;
#endif
}
#ifndef MUST_MATCH
#undef px
#undef py
#undef ph
#undef pw
#undef pa9
#undef pa10
#endif

void windowDispGX_Message(s32 type, s32 a2, u8 alpha, f32 x, f32 y, f32 height, f32 width, f32 a9, f32 a10) {
	GXSetCullMode(GX_CULL_NONE);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_OR);
	GXSetZMode(GX_TRUE, GX_ALWAYS, GX_FALSE);
	GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, dat_8041a430);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
	GXSetNumChans(0);
	GXSetNumTexGens(1);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
	GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
	GXSetCurrentMtx(0);
	switch (type) {
	case WINDOW_TYPE_TEC:
		_windowDispGX_Message(type, 0, alpha, 1, x, y, height, width, a9, a10);
		break;
	default:
		_windowDispGX_Message(type, a2, alpha, 1, x, y, height, width, a9, a10);
		_windowDispGX_Message(type, a2, alpha, 0, x, y, height, width, a9, a10);
		break;
	}
}

#ifdef MUST_MATCH
// MUST_MATCH: the fog colour must be passed through a by-value inline so its outgoing argument temp
// lands at r1+0x8; the direct compound literal lands at r1+0xc, swapping it with the tev colour's temp.
static inline void wakuSetFog(GXColor color) {
	GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, color);
}
#endif

void windowDispGX_Waku_col(u16 gxTexMapID, GXColor color, f32 x, f32 y, f32 width, f32 height, f32 curve) {
	CameraEntry* camera = camGetCurPtr();
	Mtx mtx;
	f32 fVar1, fVar4, fVar2, fVar6, fVar5, fVar3; //TODO: rename
	f32 oneThird, twoThirds;

	if (unk_804187fc) {
		if (gxTexMapID) {
			gxTexMapID = 0;
		}
		GXSetCullMode(GX_CULL_NONE);
		GXSetZCompLoc(GX_TRUE);
		GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
		GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_OR);
		GXSetZMode(GX_TRUE, GX_ALWAYS, GX_FALSE);
#ifdef MUST_MATCH
		wakuSetFog(dat_8041a430);
#else
		GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, dat_8041a430);
#endif
		GXClearVtxDesc();
		GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
		GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
		GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
		GXSetNumChans(0);
		GXSetNumTexGens(1);
		GXSetNumTevStages(1);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
		GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
		GXSetCurrentMtx(0);
		GXSetTevColor(GX_TEVREG0, color);
		MTXIdentity(mtx);
		MTXConcat(camera->view, mtx, mtx);
		GXLoadPosMtxImm(mtx, 0);
		GXSetCurrentMtx(0);
		GXLoadTexObj(&wakuTexObj[gxTexMapID], GX_TEXMAP0);
		GXBegin(GX_QUADS, GX_VTXFMT0, 0x24);

		oneThird = 0.33333333f;
		twoThirds = 0.66666666f;
		fVar1 = x + width;
		fVar2 = x + curve;
		fVar6 = fVar1 - curve;
		fVar4 = y - height;
		fVar5 = y - curve;
		fVar3 = curve + fVar4;

		GXPosition3f32(x, y, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);

		GXPosition3f32(fVar2, y, 0.0f);
		GXTexCoord2f32(oneThird, 0.0f);

		GXPosition3f32(fVar2, fVar5, 0.0f);
		GXTexCoord2f32(oneThird, oneThird);

		GXPosition3f32(x, fVar5, 0.0f);
		GXTexCoord2f32(0.0f, oneThird);



		GXPosition3f32(fVar2, y, 0.0f);
		GXTexCoord2f32(oneThird, 0.0f);

		GXPosition3f32(fVar6, y, 0.0f);
		GXTexCoord2f32(twoThirds, 0.0f);

		GXPosition3f32(fVar6, fVar5, 0.0f);
		GXTexCoord2f32(twoThirds, oneThird);

		GXPosition3f32(fVar2, fVar5, 0.0f);
		GXTexCoord2f32(oneThird, oneThird);



		GXPosition3f32(fVar6, y, 0.0f);
		GXTexCoord2f32(twoThirds, 0.0f);

		GXPosition3f32(fVar1, y, 0.0f);
		GXTexCoord2f32(1.0f, 0.0f);

		GXPosition3f32(fVar1, fVar5, 0.0f);
		GXTexCoord2f32(1.0f, oneThird);

		GXPosition3f32(fVar6, fVar5, 0.0f);
		GXTexCoord2f32(twoThirds, oneThird);



		GXPosition3f32(x, fVar5, 0.0f);
		GXTexCoord2f32(0.0f, oneThird);

		GXPosition3f32(fVar2, fVar5, 0.0f);
		GXTexCoord2f32(oneThird, oneThird);

		GXPosition3f32(fVar2, fVar3, 0.0f);
		GXTexCoord2f32(oneThird, twoThirds);

		GXPosition3f32(x, fVar3, 0.0f);
		GXTexCoord2f32(0.0f, twoThirds);



		GXPosition3f32(fVar2, fVar5, 0.0f);
		GXTexCoord2f32(oneThird, oneThird);

		GXPosition3f32(fVar6, fVar5, 0.0f);
		GXTexCoord2f32(twoThirds, oneThird);

		GXPosition3f32(fVar6, fVar3, 0.0f);
		GXTexCoord2f32(twoThirds, twoThirds);

		GXPosition3f32(fVar2, fVar3, 0.0f);
		GXTexCoord2f32(oneThird, twoThirds);



		GXPosition3f32(fVar6, fVar5, 0.0f);
		GXTexCoord2f32(twoThirds, oneThird);

		GXPosition3f32(fVar1, fVar5, 0.0f);
		GXTexCoord2f32(1.0f, oneThird);

		GXPosition3f32(fVar1, fVar3, 0.0f);
		GXTexCoord2f32(1.0f, twoThirds);

		GXPosition3f32(fVar6, fVar3, 0.0f);
		GXTexCoord2f32(twoThirds, twoThirds);



		GXPosition3f32(x, fVar3, 0.0f);
		GXTexCoord2f32(0.0f, twoThirds);

		GXPosition3f32(fVar2, fVar3, 0.0f);
		GXTexCoord2f32(oneThird, twoThirds);

		GXPosition3f32(fVar2, fVar4, 0.0f);
		GXTexCoord2f32(oneThird, 1.0f);

		GXPosition3f32(x, fVar4, 0.0f);
		GXTexCoord2f32(0.0f, 1.0f);



		GXPosition3f32(fVar2, fVar3, 0.0f);
		GXTexCoord2f32(oneThird, twoThirds);

		GXPosition3f32(fVar6, fVar3, 0.0f);
		GXTexCoord2f32(twoThirds, twoThirds);

		GXPosition3f32(fVar6, fVar4, 0.0f);
		GXTexCoord2f32(twoThirds, 1.0f);

		GXPosition3f32(fVar2, fVar4, 0.0f);
		GXTexCoord2f32(oneThird, 1.0f);



		GXPosition3f32(fVar6, fVar3, 0.0f);
		GXTexCoord2f32(twoThirds, twoThirds);

		GXPosition3f32(fVar1, fVar3, 0.0f);
		GXTexCoord2f32(1.0f, twoThirds);

		GXPosition3f32(fVar1, fVar4, 0.0f);
		GXTexCoord2f32(1.0f, 1.0f);

		GXPosition3f32(fVar6, fVar4, 0.0f);
		GXTexCoord2f32(twoThirds, 1.0f);
	}
}

void windowDispGX2_Waku_col(Mtx mtx, u16 gxTexMapID, GXColor color, f32 x, f32 y, f32 width, f32 height, f32 curve) {
	CameraEntry* camera = camGetCurPtr();
	Mtx mtx2;
	f32 fVar1, fVar4, fVar2, fVar6, fVar5, fVar3, dVar10, dVar9, half; //TODO: rename
	f32 oneThird, twoThirds;

	if (unk_804187fc) {
		if (gxTexMapID) {
			gxTexMapID = 0;
		}
		GXSetCullMode(GX_CULL_NONE);
		GXSetZCompLoc(GX_TRUE);
		GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
		GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_OR);
		GXSetZMode(GX_TRUE, GX_ALWAYS, GX_FALSE);
#ifdef MUST_MATCH
		wakuSetFog(dat_8041a430);
#else
		GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, dat_8041a430);
#endif
		GXClearVtxDesc();
		GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
		GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
		GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
		GXSetNumChans(0);
		GXSetNumTexGens(1);
		GXSetNumTevStages(1);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
		GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
		GXSetCurrentMtx(0);
		GXSetTevColor(GX_TEVREG0, color);
		half = 0.5f;
		dVar10 = -width * half;
		dVar9 = height * half;
		MTXTrans(mtx2, x - dVar10, y - dVar9, 0.0f);
		MTXConcat(mtx2, mtx, mtx2);
		MTXConcat(camera->view, mtx2, mtx2);
		GXLoadPosMtxImm(mtx2, 0);
		GXSetCurrentMtx(0);
		GXLoadTexObj(&wakuTexObj[gxTexMapID], GX_TEXMAP0);
		GXBegin(GX_QUADS, GX_VTXFMT0, 0x24);


		oneThird = 0.33333333f;
		twoThirds = 0.66666666f;
		fVar1 = dVar10 + width;
		fVar4 = dVar9 - height;
		fVar2 = dVar10 + curve;
		fVar6 = fVar1 - curve;
		fVar5 = dVar9 - curve;
		fVar3 = curve + fVar4;

		GXPosition3f32(dVar10, dVar9, 0.0f);
		GXTexCoord2f32(0.0f, 0.0f);

		GXPosition3f32(fVar2, dVar9, 0.0f);
		GXTexCoord2f32(oneThird, 0.0f);

		GXPosition3f32(fVar2, fVar5, 0.0f);
		GXTexCoord2f32(oneThird, oneThird);

		GXPosition3f32(dVar10, fVar5, 0.0f);
		GXTexCoord2f32(0.0f, oneThird);



		GXPosition3f32(fVar2, dVar9, 0.0f);
		GXTexCoord2f32(oneThird, 0.0f);

		GXPosition3f32(fVar6, dVar9, 0.0f);
		GXTexCoord2f32(twoThirds, 0.0f);

		GXPosition3f32(fVar6, fVar5, 0.0f);
		GXTexCoord2f32(twoThirds, oneThird);

		GXPosition3f32(fVar2, fVar5, 0.0f);
		GXTexCoord2f32(oneThird, oneThird);



		GXPosition3f32(fVar6, dVar9, 0.0f);
		GXTexCoord2f32(twoThirds, 0.0f);

		GXPosition3f32(fVar1, dVar9, 0.0f);
		GXTexCoord2f32(1.0f, 0.0f);

		GXPosition3f32(fVar1, fVar5, 0.0f);
		GXTexCoord2f32(1.0f, oneThird);

		GXPosition3f32(fVar6, fVar5, 0.0f);
		GXTexCoord2f32(twoThirds, oneThird);



		GXPosition3f32(dVar10, fVar5, 0.0f);
		GXTexCoord2f32(0.0f, oneThird);

		GXPosition3f32(fVar2, fVar5, 0.0f);
		GXTexCoord2f32(oneThird, oneThird);

		GXPosition3f32(fVar2, fVar3, 0.0f);
		GXTexCoord2f32(oneThird, twoThirds);

		GXPosition3f32(dVar10, fVar3, 0.0f);
		GXTexCoord2f32(0.0f, twoThirds);



		GXPosition3f32(fVar2, fVar5, 0.0f);
		GXTexCoord2f32(oneThird, oneThird);

		GXPosition3f32(fVar6, fVar5, 0.0f);
		GXTexCoord2f32(twoThirds, oneThird);

		GXPosition3f32(fVar6, fVar3, 0.0f);
		GXTexCoord2f32(twoThirds, twoThirds);

		GXPosition3f32(fVar2, fVar3, 0.0f);
		GXTexCoord2f32(oneThird, twoThirds);



		GXPosition3f32(fVar6, fVar5, 0.0f);
		GXTexCoord2f32(twoThirds, oneThird);

		GXPosition3f32(fVar1, fVar5, 0.0f);
		GXTexCoord2f32(1.0f, oneThird);

		GXPosition3f32(fVar1, fVar3, 0.0f);
		GXTexCoord2f32(1.0f, twoThirds);

		GXPosition3f32(fVar6, fVar3, 0.0f);
		GXTexCoord2f32(twoThirds, twoThirds);



		GXPosition3f32(dVar10, fVar3, 0.0f);
		GXTexCoord2f32(0.0f, twoThirds);

		GXPosition3f32(fVar2, fVar3, 0.0f);
		GXTexCoord2f32(oneThird, twoThirds);

		GXPosition3f32(fVar2, fVar4, 0.0f);
		GXTexCoord2f32(oneThird, 1.0f);

		GXPosition3f32(dVar10, fVar4, 0.0f);
		GXTexCoord2f32(0.0f, 1.0f);



		GXPosition3f32(fVar2, fVar3, 0.0f);
		GXTexCoord2f32(oneThird, twoThirds);

		GXPosition3f32(fVar6, fVar3, 0.0f);
		GXTexCoord2f32(twoThirds, twoThirds);

		GXPosition3f32(fVar6, fVar4, 0.0f);
		GXTexCoord2f32(twoThirds, 1.0f);

		GXPosition3f32(fVar2, fVar4, 0.0f);
		GXTexCoord2f32(oneThird, 1.0f);



		GXPosition3f32(fVar6, fVar3, 0.0f);
		GXTexCoord2f32(twoThirds, twoThirds);

		GXPosition3f32(fVar1, fVar3, 0.0f);
		GXTexCoord2f32(1.0f, twoThirds);

		GXPosition3f32(fVar1, fVar4, 0.0f);
		GXTexCoord2f32(1.0f, 1.0f);

		GXPosition3f32(fVar6, fVar4, 0.0f);
		GXTexCoord2f32(twoThirds, 1.0f);
	}
}

s32 windowCheckID(s32 id) {
#ifdef MUST_MATCH
	// the named-field form compiles to `add r3, base, off; lhz 0x2(r3)`; the original uses an index load
	return (*(u16*)((u8*)WinObjects + id * 0x48 + 2)) & 2;
#else
	return WinObjects[id].flags & 2;
#endif
}

WindowEntry* windowGetPointer(s32 id) {
	return &WinObjects[id];
}

GXTexObj* getWakuTexObj(s32 id) {
	return &wakuTexObj[id];
}
