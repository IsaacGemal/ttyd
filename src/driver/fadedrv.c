#include "driver/fadedrv.h"
#include "driver/animdrv.h"
#include "driver/arcdrv.h"
#include "driver/camdrv.h"
#include "driver/imgdrv.h"
#include "manager/dvdmgr.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"
#include <dolphin/dvd.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

static f32 z(void);  // local to this file in the original (fadedrv.s: .fn z, local)


u8 i4tex = 0xFF; //TODO: align 32-byte

//.data
static s32 depth_image[32] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

static FadeWork* wp;

//local prototypes
static void zFill(void);
static void _callback(s32 error, DVDFileInfo* info) {
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
// Frees the resources held by an entry and re-initialises it.
static inline void fadeEntryReset(FadeEntry* entry) {
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

// NONMATCHING (68.46%): being reconstructed.
void fadeEntry(s32 type, s32 duration, GXColor color) {
	FadeEntry* entry;
	s32 index;

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

	switch (type) {
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			entry = &wp->entries[0];
			index = 0;
			fadeEntryReset(&wp->entries[2]);
			entry->field_0xA4 = 0;
			switch (type) {
				case 1:
				case 3:
				case 5:
				case 9:
				case 11:
				case 14:
					if (duration != 0) {
						entry->field_0xA4 = 2;
					}
					break;
			}
			break;
		case 22:
		case 23:
		case 24:
		case 25:
		case 26:
		case 27:
		case 28:
		case 29:
		case 30:
		case 31:
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
		case 38:
		case 39:
		case 40:
			entry = &wp->entries[1];
			index = 1;
			break;
		case 41:
		case 42:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
		case 48:
			entry = &wp->entries[2];
			index = 2;
			fadeEntryReset(&wp->entries[0]);
			break;
		case 49:
		case 50:
		case 52:
		case 53:
		case 54:
		case 55:
		case 56:
		case 57:
		case 58:
		case 59:
		case 60:
		case 61:
		case 62:
			entry = &wp->entries[3];
			index = 3;
			switch (type) {
				case 50:
				case 53:
				case 54:
				case 56:
				case 58:
					fadeEntryReset(&wp->entries[0]);
					break;
			}
			break;
		case 51:
			entry = &wp->entries[1];
			index = 1;
			break;
		case 63:
		case 64:
			entry = &wp->entries[4];
			index = 4;
			break;
		default:
			break;
	}

	entry->flags = 0;
	entry->flags |= 1;
	entry->type = type;
	entry->field_0x14 = duration;
	entry->field_0x10 = duration >> 31;
	entry->color = color;
	entry->field_0x24 = 0;
	entry->field_0x28 = 0;
	entry->field_0x5C = 0;
	entry->field_0x8 = 0;
	entry->animOffsetX = 0.0f;
	entry->animOffsetY = 0.0f;
	entry->field_0x90 = 1.0f;
	if (type != 50 && type != 53) {
		entry->callback = NULL;
	}
	wp->field_0x350 = index;
	if (type == 34 || type == 40) {
		entry->field_0x90 = 3.0f;
	}

	switch (type) {
		case 22:
			entry->field_0x60 = "OFF_d_maku_up_down";
			entry->field_0x64 = "A_1";
			entry->imageType = 2;
			break;
		case 23:
			entry->field_0x60 = "OFF_d_maku_up_down";
			entry->field_0x64 = "A_2";
			entry->imageType = 2;
			break;
		case 24:
		case 27:
		case 63:
			entry->field_0x60 = "OFF_d_maku_left_right";
			entry->field_0x64 = "A_1";
			entry->imageType = 2;
			break;
		case 25:
		case 28:
		case 64:
			entry->field_0x60 = "OFF_d_maku_left_right";
			entry->field_0x64 = "A_2";
			entry->imageType = 2;
			break;
		case 26:
			entry->field_0x60 = "OFF_d_maku_left_right";
			entry->field_0x64 = "Z_1";
			entry->imageType = 2;
			break;
		case 29:
			entry->field_0x60 = "OFF_d_maku_center";
			entry->field_0x64 = "A_1";
			entry->imageType = 2;
			break;
		case 30:
			entry->field_0x60 = "OFF_d_maku_center";
			entry->field_0x64 = "A_2";
			entry->imageType = 2;
			break;
		case 31:
		case 34:
		case 37:
		case 40:
			entry->field_0x60 = "OFF_d_maku_center";
			entry->field_0x64 = "Z_2";
			entry->imageType = 2;
			break;
		case 32:
			entry->field_0x60 = "OFF_d_maku_center";
			entry->field_0x64 = "Z_1";
			entry->imageType = 2;
			break;
		case 35:
			entry->field_0x60 = "OFF_d_maku_center";
			entry->field_0x64 = "A_1";
			entry->imageType = 2;
			break;
		case 36:
			entry->field_0x60 = "OFF_d_maku_center";
			entry->field_0x64 = "A_2";
			entry->imageType = 2;
			break;
		case 38:
			entry->field_0x60 = "OFF_d_maku_center";
			entry->field_0x64 = "Z_1";
			entry->imageType = 2;
			break;
		case 50:
		case 53:
			entry->field_0x60 = "OFF_d_meku";
			entry->field_0x64 = "A_1";
			entry->imageType = 2;
			break;
		case 41:
			entry->field_0x60 = "OFF_d_dokan_down";
			entry->field_0x64 = "A_1";
			entry->imageType = 2;
			break;
		case 42:
			entry->field_0x60 = "OFF_d_dokan_down";
			entry->field_0x64 = "A_2";
			entry->imageType = 2;
			break;
		case 43:
			entry->field_0x60 = "OFF_d_dokan_up";
			entry->field_0x64 = "A_1";
			entry->imageType = 2;
			break;
		case 44:
			entry->field_0x60 = "OFF_d_dokan_up";
			entry->field_0x64 = "A_2";
			entry->imageType = 2;
			break;
		case 45:
			entry->field_0x60 = "OFF_d_dokan_left";
			entry->field_0x64 = "A_1";
			entry->imageType = 2;
			break;
		case 46:
			entry->field_0x60 = "OFF_d_dokan_left";
			entry->field_0x64 = "A_2";
			entry->imageType = 2;
			break;
		case 47:
			entry->field_0x60 = "OFF_d_dokan_right";
			entry->field_0x64 = "A_1";
			entry->imageType = 2;
			break;
		case 48:
			entry->field_0x60 = "OFF_d_dokan_right";
			entry->field_0x64 = "A_2";
			entry->imageType = 2;
			break;
		case 49:
		case 52:
			entry->field_0x60 = "OFF_d_meku";
			entry->field_0x64 = "Z_1";
			entry->imageType = 2;
			break;
		case 51:
			entry->field_0x60 = "OFF_d_four_meku_5sec";
			entry->field_0x64 = "A_1";
			entry->imageType = 2;
			break;
		case 54:
			entry->field_0x60 = "OFF_f_in_out_mario";
			entry->field_0x64 = "A_3";
			entry->imageType = 2;
			break;
		case 55:
			entry->field_0x60 = "OFF_f_in_out_mario";
			entry->field_0x64 = "A_1";
			entry->imageType = 2;
			break;
		case 56:
			entry->field_0x60 = "OFF_f_in_out_peach";
			entry->field_0x64 = "A_3";
			entry->imageType = 2;
			break;
		case 57:
			entry->field_0x60 = "OFF_f_in_out_peach";
			entry->field_0x64 = "A_1";
			entry->imageType = 2;
			break;
		case 58:
			entry->field_0x60 = "OFF_f_in_out_kuppa";
			entry->field_0x64 = "A_3";
			entry->imageType = 2;
			break;
		case 59:
			entry->field_0x60 = "OFF_f_in_out_kuppa";
			entry->field_0x64 = "A_1";
			entry->imageType = 2;
			break;
		case 60:
			entry->field_0x60 = "OFF_f_in_out_mario";
			entry->field_0x64 = "A_2";
			entry->imageType = 2;
			break;
		case 61:
			entry->field_0x60 = "OFF_f_in_out_peach";
			entry->field_0x64 = "A_2";
			entry->imageType = 2;
			break;
		case 62:
			entry->field_0x60 = "OFF_f_in_out_kuppa";
			entry->field_0x64 = "A_2";
			entry->imageType = 2;
			break;
		default:
			break;
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

// NONMATCHING (99.97%): being reconstructed.
void disp_texture(CameraId camId, void* param) {
	CameraEntry* camera;
	FadeEntry* entry;
	GXTexObj texObj;
	f32 projv[7];
	Mtx mtx1;
	Mtx mtx2;
	Mtx mtx3;
	Mtx mtx4;
	f32 f4;
	f32 f31;

	camera = camGetPtr(CAMERA_2D);
	entry = &wp->entries[0];

	switch (entry->type) {
		case 1:
		case 3:
		case 5:
		case 9:
		case 11:
		case 14:
			if (entry->flags & 2) {
				return;
			}
			break;
	}

	GXGetProjectionv(projv);
	GXSetProjection(camera->projection, camera->projType);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_OR);
	GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
	GXSetNumChans(0);
	GXSetNumTevStages(1);
	GXSetNumTexGens(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
	GXSetCullMode(GX_CULL_BACK);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	switch (entry->type) {
		case 1:
		case 2:
			GXLoadPosMtxImm(camera->view, GX_PNMTX0);
			GXSetCurrentMtx(GX_PNMTX0);
			TEXGetGXTexObjFromPalette(wp->texture, &texObj, 0);
			GXLoadTexObj(&texObj, GX_TEXMAP0);
			if (!(entry->flags & 2)) {
				f4 = entry->field_0x24;
				f31 = 100.0f * f4 * f4 * f4 * f4 * f4;
				PSMTXTrans(mtx1, 0.5f, 0.5f, 0.0f);
				PSMTXScale(mtx2, f31, f31, 1.0f);
				PSMTXTrans(mtx3, -0.5f, -0.5f, 0.0f);
				PSMTXConcat(mtx1, mtx2, mtx2);
				PSMTXConcat(mtx2, mtx3, (f32(*)[4])entry->field_0x2C);
			}
			else if (entry->type == 2) {
				PSMTXTrans((f32(*)[4])entry->field_0x2C, 1.0f, 1.0f, 0.0f);
			}
			break;
		case 3:
		case 4:
		case 5:
		case 6:
			GXLoadPosMtxImm(camera->view, GX_PNMTX0);
			GXSetCurrentMtx(GX_PNMTX0);
			TEXGetGXTexObjFromPalette(wp->texture, &texObj, 1);
			GXLoadTexObj(&texObj, GX_TEXMAP0);
			if (!(entry->flags & 2)) {
				f4 = entry->field_0x24;
				f31 = 100.0f * f4 * f4 * f4 * f4 * f4;
				PSMTXTrans(mtx1, 0.5f, 0.5f, 0.0f);
				PSMTXScale(mtx2, f31, f31, 1.0f);
				PSMTXRotRad(mtx4, 'z', 0.017453292f * entry->field_0x28);
				PSMTXTrans(mtx3, -0.5f - entry->fadeSpotX / (f32)gp->fbWidth,
				           -0.5f + entry->fadeSpotY / (f32)gp->efbHeight, 0.0f);
				PSMTXConcat(mtx1, mtx2, mtx2);
				PSMTXConcat(mtx2, mtx4, mtx2);
				PSMTXConcat(mtx2, mtx3, (f32(*)[4])entry->field_0x2C);
			}
			else if (entry->type == 4 || entry->type == 6) {
				PSMTXTrans((f32(*)[4])entry->field_0x2C, 1.0f, 1.0f, 0.0f);
			}
			break;
		case 7:
		case 8:
			break;
		case 9:
		case 10:
		case 11:
		case 12:
		case 14:
		case 15:
			GXLoadPosMtxImm(camera->view, GX_PNMTX0);
			GXSetCurrentMtx(GX_PNMTX0);
			GXInitTexObj(&texObj, &i4tex, 1, 1, GX_TF_I4, GX_CLAMP, GX_CLAMP, GX_FALSE);
			GXLoadTexObj(&texObj, GX_TEXMAP0);
			PSMTXIdentity((f32(*)[4])entry->field_0x2C);
			entry->color.a = (u8)(255.0f * entry->field_0x24);
			break;
		case 13:
			GXLoadPosMtxImm(camera->view, GX_PNMTX0);
			GXSetCurrentMtx(GX_PNMTX0);
			GXInitTexObj(&texObj, &i4tex, 1, 1, GX_TF_I4, GX_CLAMP, GX_CLAMP, GX_FALSE);
			GXLoadTexObj(&texObj, GX_TEXMAP0);
			PSMTXIdentity((f32(*)[4])entry->field_0x2C);
			break;
	}

	GXLoadTexMtxImm((f32(*)[4])entry->field_0x2C, 0x1e, GX_MTX2x4);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, 0x1e, GX_FALSE, GX_PTIDENTITY);
	GXSetTevColor(GX_TEVREG0, entry->color);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	f4 = 304.0f;
	GXPosition3f32(-f4, f4, 0.0f);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3f32(f4, f4, 0.0f);
	GXTexCoord2f32(1.0f, 0.0f);
	GXPosition3f32(f4, -f4, 0.0f);
	GXTexCoord2f32(1.0f, 1.0f);
	GXPosition3f32(-f4, -f4, 0.0f);
	GXTexCoord2f32(0.0f, 1.0f);
	GXSetProjectionv(projv);
}

#pragma dont_inline on // keeps z a real bl from disp_maku, as the original does
static f32 z(void) {
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
#pragma dont_inline reset
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



// NONMATCHING (93.04%): being reconstructed.
void disp_maku(CameraId camId, void* param) {
	FadeEntry* entry = param;
	CameraEntry* camera;
	Mtx mtx1;
	Mtx mtx2;
	Vec pos;
	Vec normal;
	f32 f30;
	f32 f31;
	f32 tx;
	f32 ty;

	camGetPtr(camId);

	switch (entry->type) {
		case 23:
		case 25:
		case 28:
		case 64:
			if (entry->flags & 2) {
				return;
			}
			break;
	}

	switch (entry->type) {
		case 22:
		case 23:
		case 24:
		case 25:
		case 26:
		case 27:
		case 28:
		case 29:
		case 30:
		case 31:
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
		case 38:
		case 39:
		case 40:
		case 51:
		case 63:
		case 64:
			break;
		default:
			return;
	}

	if (entry->field_0x6C < 0) {
		return;
	}
	if (entry->flags & 4) {
		f32 mag;

		camera = camGetPtr(CAMERA_3D);
		f30 = (camera->fovY * 0.5f) / 180.0f;
		f30 = 3.141592f * f30;
		f30 = 24.0f / (f32)tan(f30);
		camera = camGetPtr(CAMERA_3D);
		PSVECSubtract(&camera->cameraPos, &entry->VirtualPos, &pos);
		camera = camGetPtr(CAMERA_3D);
		normal = *(Vec*)&camera->view[2];
		PSVECNormalize(&normal, &normal);
		pos.x *= normal.x;
		pos.y *= normal.y;
		pos.z *= normal.z;
		mag = PSVECMag(&pos);
		f31 = mag / f30;
		f30 = f30 - mag;
		animPoseSetGXFunc(entry->field_0x6C, 0, 0);
		GXSetZScaleOffset(z(), 0.0f);
	}
	else {
		f31 = entry->field_0x90;
		f30 = 0.0f;
		zFill();
	}

	tx = entry->animOffsetX * f31 + 0.0f;
	ty = entry->animOffsetY * f31 + 0.0f;

	PSMTXTrans(mtx1, 64.0f * (tx / (f32)gp->fbWidth),
	           48.0f * (ty / (f32)gp->efbHeight), f30);
	PSMTXTrans(mtx2, -32.0f, 24.0f, 0.0f);
	PSMTXConcat(mtx1, mtx2, mtx1);
	PSMTXScale(mtx2, 608.0f / (f32)gp->fbWidth, 480.0f / (f32)gp->efbHeight, 608.0f / (f32)gp->fbWidth);
	PSMTXConcat(mtx1, mtx2, mtx1);
	PSMTXTrans(mtx2, 32.0f, -24.0f, 0.0f);
	PSMTXConcat(mtx1, mtx2, mtx1);
	animPoseDrawMtx(entry->field_0x6C, mtx1, 1, 0.0f, f31);
	animPoseDrawMtx(entry->field_0x6C, mtx1, 2, 0.0f, f31);
	animPoseDrawMtx(entry->field_0x6C, mtx1, 3, 0.0f, f31);
	sysWaitDrawSync();
}

// NONMATCHING (99.96%): being reconstructed.
void disp_tec(CameraId camId, void* param) {
	CameraEntry* camera;
	GXTexObj texObj;
	GXColor color;

	camera = camGetPtr(camId);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_OR);
	GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
	GXSetNumChans(0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);

	if (wp->flags & 0x1000) {
		TEXGetGXTexObjFromPalette(wp->texture, &texObj, 2);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
	}
	else {
		TEXGetGXTexObjFromPalette(wp->texture, &texObj, 3);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
	}
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_A0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_ZERO);

	if (wp->flags & 0x1000) {
		GXSetTevColor(GX_TEVREG0, (GXColor){0x19, 0x23, 0x2a, 0xff});
	}
	else {
		GXSetTevColor(GX_TEVREG0, (GXColor){0xe6, 0xe6, 0xe6, 0xff});
	}

	if (wp->flags & 0x30) {
		if (wp->flags & 0x10) {
			wp->field_0x354 += 0.04f;
			if (wp->field_0x354 > 1.0f) {
				wp->field_0x354 = 1.0f;
				wp->flags &= ~0x10;
			}
		}
		else {
			wp->field_0x354 -= 0.04f;
			if (wp->field_0x354 < 0.0f) {
				wp->field_0x354 = 0.0f;
				wp->flags &= ~0x20;
				wp->flags &= ~0x3000;
			}
		}
		color = (GXColor){0, 0, 0, 0};
		color.a = (u8)(255.0f * wp->field_0x354);
		GXSetTevColor(GX_TEVREG1, color);
		GXSetNumTevStages(2);
		GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
		GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_A1, GX_CA_ZERO);
	}

	GXSetCullMode(GX_CULL_BACK);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GXLoadPosMtxImm(camera->view, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3s16(-308, 244, 0);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3s16(308, 244, 0);
	GXTexCoord2f32(2.0f, 0.0f);
	GXPosition3s16(308, -244, 0);
	GXTexCoord2f32(2.0f, 2.0f);
	GXPosition3s16(-308, -244, 0);
	GXTexCoord2f32(0.0f, 2.0f);
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
