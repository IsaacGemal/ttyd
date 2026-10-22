#include "driver/bgdrv.h"
#include "driver/arcdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/mapdrv.h"
#include "manager/dvdmgr.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"
#include "texPalette.h"
#include "math_ext.h"
#include <string.h>
#include <stdio.h>

#define PI 3.14159265358979323846264338327950288419716939937510
#define TWO_PI 6.2831855f

extern GlobalWork* gp;

//.bss
static BackgroundWork work[2];

//local prototypes

//TODO: inline function? I see nothing in the symbol map
#define bgGetWork() (gp->inBattle ? &work[1] : &work[0])

void bgInit(void) {
	memset(&work[0], 0, sizeof(BackgroundWork));
	memset(&work[1], 0, sizeof(BackgroundWork));
}

void bgReInit(void) {
	memset(bgGetWork(), 0, sizeof(BackgroundWork));
}

void bgEntry(const char* filename) {
	BackgroundWork* bgwork;
	TEXPalette* tpl;
	DVDEntry* dvd;
	u32 size;
	char path[256]; //TODO: check
	
	bgwork = bgGetWork();
	size = 0;
	if (filename == NULL) {
		bgwork->flags &= ~1;
	}
	else {
		bgwork->data = NULL;
		arcOpen(filename, &bgwork->data, &size);
		if (!bgwork->data) {
			sprintf(path, "%s/%s", getMarioStDvdRoot(), filename);
			dvd = DVDMgrOpen(path, 2, 0);
			if (dvd) {
				size = DVDMgrGetLength(dvd);
				bgwork->data = _mapAlloc(OSRoundUp32B(size));
				DVDMgrRead(dvd, bgwork->data, OSRoundUp32B(size), 0);
				DVDMgrClose(dvd);
			}
		}
		tpl = bgwork->data;
		if (tpl) {
			UnpackTexPalette(tpl);
			bgwork->flags |= 1;
			bgwork->flags &= ~2;
			bgwork->flags &= ~4;
			bgwork->color = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
		}
		else {
			bgwork->flags |= 2u;
			bgwork->multiply = (GXColor){ 0x00, 0xFF, 0x00, 0xFF };
			bgwork->flags |= 1u;
			bgwork->color = (GXColor){ 0xFF, 0xFF, 0xFF, 0xFF };
		}
	}
}

void bgReEntry(const char* filename) {
	_mapFree(bgGetWork()->data);
	bgEntry(filename);
}

void bgSetColor(GXColor color) {
	bgGetWork()->color = color;
}

void bgDispOn(void) {
	bgGetWork()->flags |= 1;
}

void bgDispOff(void) {
	bgGetWork()->flags &= ~1;
}

void bgSetScrlOffset(f32 x, f32 y) {
	BackgroundWork* bgwork = bgGetWork();
	bgwork->scrl_x = x;
	bgwork->scrl_y = y;
}

void bgAutoScrollOff(void) {
	bgGetWork()->flags |= 4;
}

void bgAutoScrollOn(void) {
	bgGetWork()->flags &= ~4;
}

void bgTransOffsetOff(void) {
	bgGetWork()->flags |= 8;
}

void bgTransOffsetOn(void) {
	bgGetWork()->flags &= ~8;
}

void bgMain(void) {
	// declaration order matches the original's f29/f30/f31 allocation
	f32 sinangle, cosangle, angle, v8, v11;
	BackgroundWork* bgwork;
	CameraEntry* camera;
	GXTexObj texobj;
	s32 width;
		
	bgwork = bgGetWork();
	if (bgwork->flags & 1) {
		camera = camGetPtr(CAMERA_3D);
		angle = (TWO_PI * reviseAngle(-camera->viewYaw)) / 360.0f;
		sinangle = sin(angle);
		cosangle = cos(angle);
		v8 = 0.0f;
		if (!(bgwork->flags & BG_DO_MULTIPLY)) {
			TEXGetGXTexObjFromPalette(bgwork->data, &texobj, 0);
			width = gp->fbWidth / GXGetTexObjWidth(&texobj);
		}
		else {
			width = 0;
		}
		if (!(bgwork->flags & BG_NO_TRANSLATION)) {
			v8 = (0.001f * ((cosangle * camera->cameraAt.x) - (sinangle * camera->cameraAt.z))) + v8;
		}
		v11 = 4.0f * (camGetPtr(CAMERA_3D)->viewYaw / 360.0f);
		v8 = ((f32)width * v11) + v8;
		bgwork->trans_x = v8;
		dispEntry(CAMERA_BACKGROUND, 0.0f, 3, bgDisp, NULL);
	}
}

void bgDisp(CameraId camId, void* param) {
	BackgroundWork* bgwork = bgGetWork();
	CameraEntry* camera = camGetPtr(camId);
	GXColor color;
	GXTexObj texobj;
	Mtx trans;
	Mtx scale;

	if (bgwork->flags & BG_RENDERING) {
		GXSetAlphaUpdate(GX_FALSE);
		GXSetCullMode(GX_CULL_BACK);
		GXSetZCompLoc(GX_TRUE);
		GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
		GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_OR);
		GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);

		mapGetBlend(&color);
		color.g = color.g * bgwork->color.g / 255;
		color.b = color.b * bgwork->color.b / 255;
		color.a = color.a * bgwork->color.a / 255;
		color.r = color.r * bgwork->color.r / 255;
		GXSetTevColor(GX_TEVREG0, color);

		if (!(bgwork->flags & BG_DO_MULTIPLY)) {
			GXSetNumChans(0);
			GXSetNumTevStages(1);
			GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
			GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C0, GX_CC_ZERO);
			GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_A0, GX_CA_ZERO);

			TEXGetGXTexObjFromPalette(bgwork->data, &texobj, 0);
			GXLoadTexObj(&texobj, GX_TEXMAP0);
			PSMTXScale(scale, 608.0f / (f32)GXGetTexObjWidth(&texobj), 480.0f / (f32)GXGetTexObjHeight(&texobj), 1.0f);
			if (bgwork->flags & BG_NO_AUTOSCROLL) {
				PSMTXTrans(trans, bgwork->scrl_x, bgwork->scrl_y, 0.0f);
			}
			else {
				PSMTXTrans(trans, bgwork->trans_x + bgwork->scrl_x, bgwork->trans_y + bgwork->scrl_y, 0.0f);
			}
			PSMTXConcat(trans, scale, trans);
			GXLoadTexMtxImm(trans, GX_TEXMTX0, GX_MTX2x4);
			GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
			GXSetNumTexGens(1);
		}
		else {
			GXSetNumChans(1);
			GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
			GXSetChanMatColor(GX_COLOR0A0, bgwork->multiply);
			GXSetNumTevStages(1);
			GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
			GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_C0, GX_CC_ZERO);
			GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_A0, GX_CA_ZERO);
		}

		GXLoadPosMtxImm(camera->view, GX_PNMTX0);
		GXSetCurrentMtx(GX_PNMTX0);
		GXClearVtxDesc();
		GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
		GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
		GXBegin(GX_QUADS, GX_VTXFMT0, 4);
		GXPosition3s16(0, 0, 0);
		GXTexCoord2f32(0.0f, 0.0f);
		GXPosition3s16(608, 0, 0);
		GXTexCoord2f32(1.0f, 0.0f);
		GXPosition3s16(608, 480, 0);
		GXTexCoord2f32(1.0f, 1.0f);
		GXPosition3s16(0, 480, 0);
		GXTexCoord2f32(0.0f, 1.0f);
	}
}
