#include "driver/fadedrv.h"
#include "driver/animdrv.h"
#include "driver/arcdrv.h"
#include "driver/camdrv.h"
#include "driver/imgdrv.h"
#include "manager/dvdmgr.h"
#include "memory.h"
#include "system.h"
#include <dolphin/dvd.h>
#include <math.h>
#include <string.h>

extern int sprintf(char* str, const char* format, ...);

u8 i4tex = 0xFF; //TODO: align 32-byte

//.data
static s32 depth_image[32] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

static FadeWork* wp;

//local prototypes
void _callback(s32 error, DVDFileInfo* info);
void disp_texture(CameraId camId, void* param);
f32 z(void);
static void zFill(void);
void disp_maku(CameraId camId, void* param);
void disp_tec(CameraId camId, void* param);
void disp_dummy_black(CameraId camId, void* param);

void _callback(s32 error, DVDFileInfo* info) {
	void* userData = info->cb.userData;

	UnpackTexPalette(wp->texture);
	DVDMgrClose(userData);
	wp->loaded = TRUE;
}

void fadeInit(void) {
	int i;

	wp = __memAlloc(HEAP_DEFAULT, sizeof(FadeWork));
	memset(wp, 0, sizeof(FadeWork));

	for (i = 0; i < 5; i++) {
		FadeEntry* entry = &wp->entries[i];

		entry->field_0x6C = -1;
		entry->field_0x70 = -1;

		entry->fadeSpotX = 0.0f;
		entry->fadeSpotY = 0.0f;

		entry->animOffsetX = 0.0f;
		entry->animOffsetY = 0.0f;

		entry->field_0x90 = 1.0f;

		entry->field_0xA4 = 0;
	}

	wp->field_0x350 = -1;
	wp->texture = NULL;
	wp->loaded = FALSE;
}

void fadeTexSetup(void) {
	DVDEntry* dvd;
	u32 size;
	char path[128];

	wp->texture = NULL;
	wp->texture = arcOpen("fade.tpl", NULL, NULL);
	if (wp->texture == NULL) {
		sprintf(path, "%s/fade.tpl", getMarioStDvdRoot());
		dvd = DVDMgrOpen(path, 2, 0);
		size = OSRoundUp32B(DVDMgrGetLength(dvd));
		wp->texture = __memAlloc(HEAP_DEFAULT, size);
		dvd->info.cb.userData = dvd;
		DVDMgrReadAsync(dvd, wp->texture, size, 0, _callback);
	}
	else {
		UnpackTexPalette(wp->texture);
		wp->loaded = TRUE;
	}
}

//TODO: probably lots of switch statements
// NONMATCHING (6.42%): 2 instructions differ, 431 missing, plus register differences.
void fadeEntry(s32 type, s32 duration, GXColor color) {
	if (!type) {
		return;
	}
	if (type == 16) { //type 16 is type 9 but white
		type = 9;
		color = (GXColor){0xF0, 0xF0, 0xF0, 0xFF};
	}
	if (type == 17) {
		type = 10;
		color = (GXColor){0xF0, 0xF0, 0xF0, 0xFF};
	}
	if (type == 18) {
		type = 11;
		color = (GXColor){0xF0, 0xF0, 0xF0, 0xFF};
	}
	if (type == 19) {
		type = 12;
		color = (GXColor){0xF0, 0xF0, 0xF0, 0xFF};
	}
	if (type == 20) {
		type = 14;
		color = (GXColor){0xF0, 0xF0, 0xF0, 0xFF};
	}
	if (type == 21) {
		type = 15;
		color = (GXColor){0xF0, 0xF0, 0xF0, 0xFF};
	}
	if ((type == 1 || type == 2) && duration < 1000) {
		duration = 1000;
	}
	if (type == 7) {
		type = 3;
	}
	else if (type == 8) {
		type = 4;
	}
	else if (((type - 3) <= 2 || type == 6) && duration < 1000) {
		duration = 1000;
	}

}

void fadeTecOn(void) {
	wp->flags |= 0x1000;
}

void fadeTecOff(void) {
	wp->flags &= ~0x1000;
}

void fadeTecSoftOn(void) {
	wp->flags |= 0x1010;
	wp->field_0x354 = 0.0f;
}

void fadeTecSoftOff(void) {
	wp->flags |= 0x1020;
	wp->field_0x354 = 1.0f;
}

void fadeSoftFocusOn(void) {
	wp->flags |= 0x2000;
}

void fadeSoftFocusOff(void) {
	wp->flags &= ~0x2000;
}

void fadeWhiteOutOn(void) {
	wp->flags |= 0x4000;
}

void fadeWhiteOutOff(void) {
	wp->flags &= ~0x4000;
}

void fadeSetSpotPos(f32 x, f32 y) {
	FadeEntry* entry = &wp->entries[0];

	entry->fadeSpotX = x;
	entry->fadeSpotY = y;
}

void fadeSetAnimOfsPos(f32 x, f32 y) {
	FadeEntry* entry = &wp->entries[1];

	entry->animOffsetX = x;
	entry->animOffsetY = y;
}

void fadeSetAnimVirtualPos(f32 x, f32 y, f32 z) {
	FadeEntry* entry = &wp->entries[1];
	entry->VirtualPos = (Vec){x, y, z};
	entry->flags |= 4;
}

void fadeSetOffscreenCallback(OffscreenCallback callback, void* param) {
	FadeEntry* entry = &wp->entries[3];

	entry->callback = callback;
	entry->offparam = param;
}

// NONMATCHING (0.09%): 1070 missing.
void fadeMain(void) {

}

// NONMATCHING (0.27%): 374 missing.
void disp_texture(CameraId camId, void* param) {
	//passed wp, unused
}

f32 z(void) {
	FadeEntry* entry = &wp->entries[1];
	CameraEntry* cam;
	Vec pos;
	f32 one = 1.0f;
	f32 denom;
	f32 zpos;

	if (camGetPtr(CAMERA_3D)->projType == 0) {
		return one;
	}

	cam = camGetPtr(CAMERA_FADE);
	PSMTXMultVec(cam->view, &entry->VirtualPos, &pos);
	zpos = pos.z;
	denom = (cam->viewport[5] - cam->viewport[4])
	      * ((one / -zpos) * (cam->projection[2][2] * zpos + cam->projection[2][3]))
	      + cam->viewport[5];

	cam = camGetPtr(CAMERA_3D);
	PSMTXMultVec(cam->view, &entry->VirtualPos, &pos);
	return ((cam->viewport[5] - cam->viewport[4])
	      * (one * (cam->projection[2][2] * pos.z + cam->projection[2][3]))
	      + cam->viewport[5]) / denom;
}

static void zFill(void) {
	CameraEntry* camera;
	f32 y0, y1, x0, x1;
	f32 zpos;
	f32 nf, half;
	GXTexObj obj;
	Mtx mtx;

	camera = camGetCurPtr();
	nf = camera->near + camera->far;
	half = 0.5f;
	y0 = nf * half;
	y0 = y0 * (f32)tan((half * ((6.2831855f * camera->fovY) / 360.0f)));
	x1 = (y0 * camera->aspect);
	y1 = -y0;
	x0 = -x1;

	GXInitTexObj(&obj, depth_image, 4, 4, GX_TF_Z8, GX_REPEAT, GX_REPEAT, GX_FALSE);
	GXLoadTexObj(&obj, GX_TEXMAP0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	GXSetZTexture(GX_ZT_REPLACE, GX_TF_Z8, 0);
	GXSetNumChans(0);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
	GXSetZCompLoc(GX_TRUE);
	GXSetBlendMode(GX_BM_LOGIC, GX_BL_ONE, GX_BL_ZERO, GX_LO_COPY);
	GXSetZMode(GX_TRUE, GX_ALWAYS, GX_TRUE);
	GXSetColorUpdate(GX_FALSE);
	GXSetAlphaUpdate(GX_FALSE);
	GXSetCullMode(GX_CULL_NONE);
	GXClearVtxDesc();
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U8, 0);
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	PSMTXIdentity(mtx);
	GXLoadPosMtxImm(camera->view, 0);
	GXSetCurrentMtx(0);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	zpos = -10000.0f;
	GXPosition3f32(x0, y0, zpos);
	GXTexCoord2u8(0, 0);
	GXPosition3f32(x1, y0, zpos);
	GXTexCoord2u8(1, 0);
	GXPosition3f32(x1, y1, zpos);
	GXTexCoord2u8(1, 1);
	GXPosition3f32(x0, y1, zpos);
	GXTexCoord2u8(0, 1);
	GXSetZTexture(GX_ZT_DISABLE, GX_TF_Z8, 0);
	GXSetColorUpdate(GX_TRUE);
}

// NONMATCHING (0.49%): 205 missing.
void disp_maku(CameraId camId, void* param) {
	FadeEntry* entry = param;

}

// NONMATCHING (0.39%): 256 missing.
void disp_tec(CameraId camId, void* param) {
	//passed wp, unused

}

void disp_dummy_black(CameraId camId, void* param) {
	CameraEntry* cam;
	f32 projv[7];

	cam = camGetPtr(CAMERA_FADE);
	GXGetProjectionv(projv);
	GXSetProjection(cam->projection, cam->projType);
	GXSetZCompLoc(GX_ZC_LINEAR);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_OR);
	GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
	if (wp->flags & 0x8000) {
		GXSetChanMatColor(GX_COLOR0A0, (GXColor){0, 0, 0, 255});
	}
	else {
		GXSetChanMatColor(GX_COLOR0A0, (GXColor){255, 255, 255, 255});
	}
	GXSetNumTevStages(1);
	GXSetNumTexGens(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	GXLoadPosMtxImm(cam->view, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
	GXSetCullMode(GX_CULL_BACK);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3s16(-32, 24, 0);
	GXPosition3s16(32, 24, 0);
	GXPosition3s16(32, -24, 0);
	GXPosition3s16(-32, -24, 0);
	GXSetProjectionv(projv);
}

BOOL fadeIsFinish(void) {
	FadeEntry* entry;

	if (wp->field_0x350 == -1) {
		return TRUE;
	}

	entry = &wp->entries[wp->field_0x350];
	return (entry->flags >> 1) & 1;
}

void fadeReset(s32 id) { //TODO: better param name?
	FadeEntry* entry;

	entry = &wp->entries[id];
	if (entry->field_0x6C >= 0) {
		animPoseRelease(entry->field_0x6C);
	}
	if (entry->field_0x70 >= 0) {
		animPaperPoseRelease(entry->field_0x70);
	}
	if (entry->image) {
		imgRelease(entry->image, entry->imageType);
	}
	memset(entry, 0, sizeof(FadeEntry));
	entry->field_0x6C = -1;
	entry->field_0x70 = -1;
	entry->fadeSpotX = 0.0f;
	entry->fadeSpotY = 0.0f;
	entry->animOffsetX = 0.0f;
	entry->animOffsetY = 0.0f;
	entry->field_0x90 = 1.0f;
}

TEXPalette* fadeGetTpl(void) {
	return wp->texture;
}

void fadeSetNarrowFast(void) {
	wp->entries[1].flags |= 8;
}
