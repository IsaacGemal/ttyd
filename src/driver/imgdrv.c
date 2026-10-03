/* "imgdrv" - Image Driver
 * Status: All functions added, needs Disps done, struct names, and a couple register tweaks
 *
 * Function: still not quite sure, TODO something about shadows that shadowdrv doesn't handle
 *
 * Last Update: 5/17/2021 decompile most of the file
 */
#include "driver/imgdrv.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "mario/mariost.h"
#include <math.h>
#include <string.h>
#include "driver/mapdrv.h"
#include "driver/offscreendrv.h"

extern GlobalWork* gp;

//.data
static s32 depth_image[32] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

//.sbss
static ImageWork work;

//.sdata
static ImageWork* wp = &work;

//local prototypes
static void zFill(void);



static void zFill(void) { //1:1
	CameraEntry* camera;
	f32 y0, y1, x0, x1;
	f32 sum, half;
	f32 zpos;
	GXTexObj obj;
	Mtx mtx;

	camera = camGetCurPtr();
	half = 0.5f;
	sum = camera->near + camera->far;
	y0 = sum * half;
	y0 = y0 * (f32)tan(half * ((6.2831855f * camera->fovY) / 360.0f));
	x1 = (y0 * camera->aspect);
	y1 = -y0;
	x0 = -x1;
	/*x0 = (camera->mNearZ + camera->mFarZ) * 0.5f;
	x1 = tan(((6.2831855f * camera->mFovY) / 360.0f) * 0.5f);
	y0 = (x0 * x1);
	x1 = (y0 * camera->mAspect);
	y1 = -y0;
	x0 = -x1;*/
	GXInitTexObj(&obj, depth_image, 4, 4, GX_TF_Z8, GX_REPEAT, GX_REPEAT, GX_FALSE);
	GXLoadTexObj(&obj, GX_TEXMAP0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);
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
	MTXIdentity(mtx);
	GXLoadPosMtxImm(camera->view, 0);
	GXSetCurrentMtx(0);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	zpos = -1000.0f;
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

// NONMATCHING (0.20%): 490 missing.
void imgDisp(CameraId camId, void* param) {
	ImageEntry* entry = param;
	int i;

	for (i = 0; i < 3; i++) {

	}

}

// .rodata in the original; left extern because <math.h>'s inline sqrt statics already fill this
// file's .rodata, and defining it here would shift them (see report).
extern const Vec vec3_802c4808;
// Objects the original unit defines in .sdata2/.rodata.
const GXColor dat_8041a278 = {0xFF, 0xFF, 0xFF, 0xFF};
const GXColor dat_8041a27c = {0xFF, 0xFF, 0xFF, 0xFF};
void imgDisp_ProjPlane(CameraId camId, void* param) {
	ImageEntry* entry = param;
	Mtx mtx;
	Mtx concat;
	Vec lightPos;
	GXColor color1;
	GXColor color2;
	CameraEntry* camera;
	u32 flags;

	switch (entry->shadowId) {
		case 0:
			return;
		case 1:
			flags = 0x20000;
			PSMTXIdentity(mtx);
			GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
			break;
		case 2:
			flags = 0x40000;
			PSMTXTrans(mtx, 0.0f, 0.0f, entry->field_0x11C);
			GXSetBlendMode(GX_BM_SUBTRACT, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
			break;
	}

	lightPos = vec3_802c4808;
	PSMTXMultVec(mtx, &lightPos, &lightPos);
	mapSetMaterialLight(0, lightPos);
	color1 = dat_8041a278;
	color2 = dat_8041a27c;
	mapSetMaterialLastStageBlend(flags, color1, color2);
	mapSetMaterialTev(0, 0, flags, mtx);
	camera = camGetCurPtr();
	PSMTXConcat(camera->view, mtx, concat);
	GXLoadPosMtxImm(concat, 0);
	GXSetCurrentMtx(GX_PNMTX0);
	GXSetAlphaUpdate(GX_TRUE);
	GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetCullMode(GX_CULL_NONE);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3f32(-32.0f, 24.0f, 0.0f);
	GXColor4u8(0, 0, 0, 0);
	GXPosition3f32(32.0f, 24.0f, 0.0f);
	GXColor4u8(0, 0, 0, 0);
	GXPosition3f32(32.0f, -24.0f, 0.0f);
	GXColor4u8(0, 0, 0, 0);
	GXPosition3f32(-32.0f, -24.0f, 0.0f);
	GXColor4u8(0, 0, 0, 0);
}

//1:1
void imgCapture_Prim(ImageEntry* entry, s32 id) {
	ImageShadow* shadow;
	s32 pose;
	smartEntry* image;
	u16 width, height;

	shadow = &entry->shadows[id];
	if (!(shadow->flags & 4)) {
		return;
	}

	pose = shadow->field_0x10;
	shadow->flags |= 0x10;

	if (pose != -1) {
		if (shadow->field_0x40 != 0) {
			if (shadow->flags & 0x100) {
				GXTexObj* texObj;
				smartEntry* imageA;
				GXTexFmt fmtTexA;
				u32 sizeA;
				u16 heightA, widthA;

				offscreenGetTexObj(pose, &texObj, &imageA);
				if (texObj != NULL) {
					widthA = GXGetTexObjWidth(texObj);
					heightA = GXGetTexObjHeight(texObj);
					fmtTexA = GXGetTexObjFmt(texObj);
					sizeA = GXGetTexBufferSize(widthA, heightA, fmtTexA, GX_FALSE, 0);
					if (shadow->field_0x40 == 1) {
						shadow->field_0x3C = smartAlloc(sizeA, 1);
					} else if (shadow->field_0x40 == 2) {
						shadow->field_0x3C = smartAlloc(sizeA, 0);
					}
					memcpy(shadow->field_0x3C->address, imageA->address, sizeA);
					DCFlushRange(shadow->field_0x3C->address, sizeA);
					GXInitTexObj((GXTexObj*)&shadow->field_0x1C[0], shadow->field_0x3C->address, widthA, heightA, fmtTexA, GX_CLAMP, GX_CLAMP, GX_FALSE);
					GXInitTexObjLOD((GXTexObj*)&shadow->field_0x1C[0], GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
					offscreenGetBoundingBox(pose, (u16*)&shadow->field_0x8[0], (u16*)&shadow->field_0x8[2], (u16*)&shadow->field_0x8[4], (u16*)&shadow->field_0x8[6]);
					*(u16*)&shadow->field_0x8[4] -= *(u16*)&shadow->field_0x8[0];
					*(u16*)&shadow->field_0x8[6] -= *(u16*)&shadow->field_0x8[2];
					shadow->flags &= ~0x100;
				}
			}
			if (shadow->flags & 0x100) {
				return;
			}
			shadow->flags &= ~0x10;
		} else {
			GXTexObj* texObj;
			smartEntry* image;

			offscreenGetTexObj(pose, &texObj, &image);
			if (texObj != NULL) {
				*(GXTexObj*)&shadow->field_0x1C[0] = *texObj;
				shadow->field_0x3C = image;
				offscreenGetBoundingBox(pose, (u16*)&shadow->field_0x8[0], (u16*)&shadow->field_0x8[2], (u16*)&shadow->field_0x8[4], (u16*)&shadow->field_0x8[6]);
				*(u16*)&shadow->field_0x8[4] -= *(u16*)&shadow->field_0x8[0];
				*(u16*)&shadow->field_0x8[6] -= *(u16*)&shadow->field_0x8[2];
				shadow->flags &= ~0x10;
			}
		}
	} else {
		if (shadow->field_0x40 != 0) {
			if (shadow->flags & 0x100) {
				GXTexFmt fmtTex, fmtDst;
				u32 size;
				s32 mode;

				*(u16*)&shadow->field_0x8[0] = shadow->field_0x4;
				*(u16*)&shadow->field_0x8[2] = shadow->field_0x6;
				*(u16*)&shadow->field_0x8[4] = entry->field_0xE4;
				*(u16*)&shadow->field_0x8[6] = entry->field_0xE6;
				height = *(u16*)&shadow->field_0x8[6];
				width = *(u16*)&shadow->field_0x8[4];
				fmtTex = shadow->field_0x18;
				fmtDst = shadow->field_0x14;
				mode = shadow->field_0x40;
				GXSetTexCopySrc(*(u16*)&shadow->field_0x8[0], *(u16*)&shadow->field_0x8[2], width, height);
				GXSetTexCopyDst(width, height, fmtDst, GX_FALSE);
				size = GXGetTexBufferSize(width, height, fmtTex, GX_FALSE, 0);
				if (mode == 1) {
					image = smartAlloc(size, 1);
				} else if (mode == 2) {
					image = smartAlloc(size, 0);
				} else {
					image = smartAlloc(size, 3);
				}
				GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
				GXCopyTex(image->address, GX_FALSE);
				GXPixModeSync();
				GXInitTexObj((GXTexObj*)&shadow->field_0x1C[0], image->address, width, height, fmtTex, GX_CLAMP, GX_CLAMP, GX_FALSE);
				GXInitTexObjLOD((GXTexObj*)&shadow->field_0x1C[0], GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
				shadow->field_0x3C = image;
				shadow->flags &= ~0x100;
			}
			if (shadow->flags & 0x100) {
				return;
			}
			shadow->flags &= ~0x10;
		} else {
			smartEntry* imageB;
			u16 widthB, heightB;
			u32 sizeB;
			s32 modeB;
			GXTexFmt fmtDstB, fmtTexB;

			*(u16*)&shadow->field_0x8[0] = shadow->field_0x4;
			*(u16*)&shadow->field_0x8[2] = shadow->field_0x6;
			*(u16*)&shadow->field_0x8[4] = entry->field_0xE4;
			*(u16*)&shadow->field_0x8[6] = entry->field_0xE6;
			heightB = *(u16*)&shadow->field_0x8[6];
			widthB = *(u16*)&shadow->field_0x8[4];
			fmtTexB = shadow->field_0x18;
			fmtDstB = shadow->field_0x14;
			modeB = shadow->field_0x40;
			GXSetTexCopySrc(*(u16*)&shadow->field_0x8[0], *(u16*)&shadow->field_0x8[2], widthB, heightB);
			GXSetTexCopyDst(widthB, heightB, fmtDstB, GX_FALSE);
			sizeB = GXGetTexBufferSize(widthB, heightB, fmtTexB, GX_FALSE, 0);
			if (modeB == 1) {
				imageB = smartAlloc(sizeB, 1);
			} else if (modeB == 2) {
				imageB = smartAlloc(sizeB, 0);
			} else {
				imageB = smartAlloc(sizeB, 3);
			}
			GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
			GXCopyTex(imageB->address, GX_FALSE);
			GXPixModeSync();
			GXInitTexObj((GXTexObj*)&shadow->field_0x1C[0], imageB->address, widthB, heightB, fmtTexB, GX_CLAMP, GX_CLAMP, GX_FALSE);
			GXInitTexObjLOD((GXTexObj*)&shadow->field_0x1C[0], GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
			shadow->field_0x3C = imageB;
			shadow->flags &= ~0x10;
		}
	}
}

void imgCapture(CameraId camId, void* param) {
	//param is NULL
	s32 type = gp->inBattle != 0;
	int i, count;
	ImageEntry* entry;
	int shadow;

	for (i = 0, count = wp->count, entry = wp->entries; i < count; i++, entry++) {
		if (entry->flags & 1 && entry->poseId != -1 && (entry->type == 2 || entry->type == type)) {
			if (entry->field_0x154 == camId) {
				for (shadow = 0; shadow < 3; shadow++) {
					imgCapture_Prim(entry, shadow);
				}
			}
		}
	}
}

void imgInit(void) { //1:1
	wp->count = 10;
	wp->entries = __memAlloc(HEAP_DEFAULT, wp->count * sizeof(ImageEntry));
	memset(wp->entries, 0, wp->count * sizeof(ImageEntry));
}

void imgAutoRelease(s32 type) {
	int i, count;
	ImageEntry* entry;

	for (i = 0, count = wp->count, entry = wp->entries; i < count; i++, entry++) {
		if (entry->flags & 1 && entry->type == type) {
			imgRelease(entry, type);
		}
	}
}

s32 imgEntry(const char* name, s32 type) { //1:1, needs variable names
	int i, count;
	ImageEntry* entry;

	//check if already created
	for (i = 0, count = wp->count, entry = wp->entries; i < count; i++, entry++) {
		if (entry->flags & 1 && entry->type == type) {
			if (!strcmp(entry->name, name)) break;
		}
	}

	//find new slot
	for (i = 0, entry = wp->entries; i < count; i++, entry++) {
		if (!(entry->flags & 1)) break;
	}

	entry->flags = 9;
	entry->type = type;
	strcpy(entry->name, name);
	entry->poseId = -1;
	entry->animName = NULL;
	entry->field_0xE4 = 0;
	entry->field_0xE6 = 0;
	entry->field_0xE8 = 0.0f;
	entry->field_0xEC = 0.0f;
	entry->order = 0.0f;
	entry->field_0x118 = 0.0f;
	entry->shadowId = 0;
	entry->field_0x11C = 0.0f;
	MTXIdentity(entry->field_0x120);
	entry->camId = CAMERA_3D_IMAGE;
	entry->field_0x154 = 4;

	entry->shadows[0].flags = 0;
	entry->shadows[0].field_0x4 = 0;
	entry->shadows[0].field_0x6 = 0;
	entry->shadows[0].field_0x10 = -1;
	entry->shadows[0].field_0x14 = 6;
	entry->shadows[0].field_0x18 = 6;
	entry->shadows[0].field_0x40 = 0;

	entry->shadows[1].flags = 0;
	entry->shadows[1].field_0x4 = 0;
	entry->shadows[1].field_0x6 = 0;
	entry->shadows[1].field_0x10 = -1;
	entry->shadows[1].field_0x14 = 6;
	entry->shadows[1].field_0x18 = 6;
	entry->shadows[1].field_0x40 = 0;

	entry->shadows[2].flags = 0;
	entry->shadows[2].field_0x4 = 0;
	entry->shadows[2].field_0x6 = 0;
	entry->shadows[2].field_0x10 = -1;
	entry->shadows[2].field_0x14 = 6;
	entry->shadows[2].field_0x18 = 6;
	entry->shadows[2].field_0x40 = 0;
	entry->shadows[2].field_0x14 = 39;
	entry->shadows[2].field_0x18 = 1;

	return i;
}

void imgMain(void) { //1:1
	s32 type = gp->inBattle != 0;
	int shadow, i, count;
	ImageEntry* entry;

	i = 0;
	count = wp->count;
	for (entry = wp->entries; i < count; i++, entry++) {
		if (entry->flags & 1 && entry->poseId != -1 && !(entry->flags & 2)) {
			if (entry->type == 2 || entry->type == type) {
				for (shadow = 0; shadow < 3; shadow++) {
					if (entry->shadows[shadow].flags & 4) break;
				}
				if (shadow != 3) {
					dispEntry(entry->camId, entry->order, 1, imgDisp, entry);
					switch (entry->shadowId) {
						case 0:
							animPoseSetMaterialFlagOff(entry->poseId, 0x10000);
							break;
						case 1:
							dispEntry(CAMERA_SHADOW, 0.0f, 1, imgShadowDisp, entry);
							dispEntry(CAMERA_3D_IMAGE, 0.0f, 5, imgDisp_ProjPlane, entry);
							break;
						case 2:
							animPoseSetMaterialFlagOn(entry->poseId, 0x10000);
							dispEntry(CAMERA_SHADOW, 0.0f, 6, imgShadowDisp, entry);
							dispEntry(CAMERA_3D_IMAGE, 0.0f, 5, imgDisp_ProjPlane, entry);
							break;
					}
				}
			}
		}
	}

	for (i = 0; i < 13; i++) {
		dispEntry(i, 0.0f, 7, imgCapture, NULL);
	}
}

ImageEntry* imgNameToPtr(const char* name, s32 type) { //1:1
	int i, count;
	ImageEntry* entry;

	i = 0;
	count = wp->count;
	for (entry = wp->entries; i < count; i++, entry++) {
		if (entry->flags & 1 && (entry->type == 2 || entry->type == type)) {
			if (!strcmp(entry->name, name)) break;
		}
	}
	return entry;
}

void imgFreeCapture(ImageEntry* entry, s32 shadowId) { //1:1, needs enums
	ImageShadow* shadow = &entry->shadows[shadowId];
	if (shadow->flags & 4) {
		shadow->flags &= ~0x10;
		if (shadow->flags & 0x100) {
			shadow->flags = 0;
		}
		else if (!shadow->field_0x40) {
			shadow->flags = 0;
		}
		else {
			shadow->flags = 0;
			smartFree(shadow->field_0x3C);
		}
	}
}

void imgSetVirtualPoint(ImageEntry* entry, const Vec* point) { //1:1
	entry->flags &= ~8;
	entry->point = *point;
}

void imgClearVirtualPoint(ImageEntry* entry) { //1:1
	entry->flags |= 8;
}

void imgRelease(ImageEntry* entry, s32 type) { //1:1 after inlining
	int i;

	for (i = 0; i < 3; i++) {
		imgFreeCapture(entry, i); //TODO: double check this inlines correctly
	}
	entry->flags = 0;
	memset(entry, 0, sizeof(ImageEntry));
}

void imgSetShadow(ImageEntry* entry, s32 shadowId) { //1:1
	entry->shadowId = shadowId;
}

// NONMATCHING (0.25%): 402 missing.
void imgShadowDisp(CameraId camId, void* param) {
	ImageEntry* entry = param;

}
