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
#include "math_ext.h"
#include <string.h>
#include "driver/mapdrv.h"
#include "driver/offscreendrv.h"
#include "sdk/DEMOInit.h"
#include "system.h"
#include "driver/shadowdrv.h"

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

// The second flag argument to animSetPaperTexObj is evaluated before the first in the original;
// the MUST_MATCH branch spells the flags in the shape that reproduces that evaluation order.
void imgDisp(CameraId camId, void* param) {
	ImageEntry* entry = param;
	ImageShadow* shadow;
	Mtx mats[3];
	Mtx mtxT2;
	Mtx mtxS2;
	Mtx mtxS;
	Mtx mtxT;
	GXTexObj* texObjs[3];
	void* texDatas[3];
	f32 (*matPtrs[3])[4];
	Vec vec;
	Vec normal;
	AnimationPoseData* myData;
	AnimationPoseData* otherData;
	ImageEntry* other;
	f32 b;
	f32 a;
	f32 d;
	f32 c;
	f32 scale, z;
	f32 y;
	f32 dist;
	f32 focalLength;
	f32 ra, rb, rc, rd;
	int i;
	int index;

	index = -1;
	for (i = 0; i < 3; i++) {
		shadow = &entry->shadows[i];
		if (!(shadow->flags & 4)) {
			texObjs[i] = NULL;
			texDatas[i] = NULL;
		} else if (shadow->flags & 0x10) {
			texObjs[i] = NULL;
			texDatas[i] = NULL;
		} else {
			if (entry->flags & 4) {
				a = (f32)shadow->bboxX;
				b = (f32)shadow->bboxY;
				c = (f32)shadow->bboxW;
				d = (f32)shadow->bboxH;
			} else {
				a = (f32)shadow->left;
				b = (f32)shadow->top;
				c = (f32)entry->captureWidth;
				d = (f32)entry->captureHeight;
			}
			PSMTXTrans(mtxT,
			           -((f32)shadow->bboxX - a) / (f32)shadow->bboxW,
			           -((f32)shadow->bboxY - b) / (f32)shadow->bboxH, 0.0f);
			PSMTXScale(mtxS, c / (f32)shadow->bboxW,
			           d / (f32)shadow->bboxH, 1.0f);
			PSMTXConcat(mtxT, mtxS, mats[i]);
			texObjs[i] = &shadow->texObjData;
			texDatas[i] = shadow->imageData;
		}
		if (texObjs[i] != NULL) {
			matPtrs[i] = mats[i];
			if (index == -1) index = i;
		} else {
			matPtrs[i] = NULL;
		}
	}

	if (index == -1) {
		entry->flags |= 0x10000000;
		return;
	}
	entry->flags &= ~0x10000000;

	animSetPaperTexMtx(matPtrs[0], matPtrs[1], matPtrs[2]);
#ifdef MUST_MATCH
	animSetPaperTexObj(texObjs[0], texObjs[1], texObjs[2], texDatas[0], texDatas[1], texDatas[2],
	                   entry->shadows[0].offscreenId != -1 ? 1 : 0, entry->shadows[1].offscreenId != -1 ? 1 : 0);
#else
	animSetPaperTexObj(texObjs[0], texObjs[1], texObjs[2], texDatas[0], texDatas[1], texDatas[2],
	                   entry->shadows[0].offscreenId != -1, entry->shadows[1].offscreenId != -1);
#endif

	if (entry->animName != NULL) {
		animPoseSetAnim(entry->poseId, entry->animName, 0);
	}
	animPoseSetStartTime(entry->poseId, entry->startTime);
	animPoseMain(entry->poseId);
	animPoseSetMaterialLightFlagOn(entry->poseId, 1);
	entry->loopTimes = animPoseGetLoopTimes(entry->poseId);

	if (entry->flags & 8) {
		zFill();
		scale = 1.0f;
		z = 0.0f;
	} else {
		focalLength = 24.0f / (f32)tan(3.141592f * ((camGetPtr(CAMERA_3D)->fovY / 2.0f) / 180.0f));
		PSVECSubtract(&camGetPtr(CAMERA_3D)->cameraPos, &entry->point, &vec);
		normal = *(Vec*)camGetPtr(CAMERA_3D)->view[2];
		PSVECNormalize(&normal, &normal);
		vec.x = vec.x * normal.x;
		vec.y = vec.y * normal.y;
		vec.z = vec.z * normal.z;
		dist = PSVECMag(&vec);
		scale = dist / focalLength;
		z = focalLength - dist;
	}

	if (entry->flags & 4) {
		ra = (f32)entry->shadows[index].bboxX;
		rb = (f32)entry->shadows[index].bboxY;
		rc = (f32)entry->shadows[index].bboxW;
		rd = (f32)entry->shadows[index].bboxH;
	} else {
		ra = (f32)entry->shadows[index].left;
		rb = (f32)entry->shadows[index].top;
		rc = (f32)entry->captureWidth;
		rd = (f32)entry->captureHeight;
	}

	PSMTXTrans(mtxT2, 64.0f * ((entry->posX + ra) / (f32)DEMOGetRenderModeObj()->fbWidth),
	           48.0f * ((-(entry->posY + rb)) / (f32)DEMOGetRenderModeObj()->efbHeight), z);
	PSMTXTrans(mtxS2, -32.0f, 24.0f, 0.0f);
	PSMTXConcat(mtxT2, mtxS2, mtxT2);

	PSMTXScale(mtxS2, rc / (f32)DEMOGetRenderModeObj()->fbWidth,
	           rd / (f32)DEMOGetRenderModeObj()->efbHeight,
	           rc / (f32)DEMOGetRenderModeObj()->fbWidth);
	PSMTXConcat(mtxT2, mtxS2, mtxT2);
	PSMTXTrans(mtxS2, 32.0f, -24.0f, 0.0f);
	PSMTXConcat(mtxT2, mtxS2, mtxT2);
	PSMTXConcat(entry->matrix, mtxT2, mtxT2);

	animPoseDrawMtx(entry->poseId, mtxT2, 1, 0.0f, scale);
	animPoseDrawMtx(entry->poseId, mtxT2, 2, 0.0f, scale);
	animPoseDrawMtx(entry->poseId, mtxT2, 3, 0.0f, scale);
	animSetPaperTexMtx(NULL, NULL, NULL);

	i = 0;
	other = wp->entries;
	for (; i < wp->count; i++, other++) {
		if ((other->flags & 1) && other->poseId != -1 && !(other->flags & 2)) {
			if (strcmp(other->name, entry->name) != 0) {
				myData = animPoseGetAnimBaseDataPtr(entry->poseId);
				otherData = animPoseGetAnimBaseDataPtr(other->poseId);
				if (!strcmp(otherData->fileName, myData->fileName)) break;
			}
		}
	}

	if (i < wp->count) {
		sysWaitDrawSync();
	}
}

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
			PSMTXTrans(mtx, 0.0f, 0.0f, entry->depthPlaneOffset);
			GXSetBlendMode(GX_BM_SUBTRACT, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
			break;
	}

	lightPos = (Vec){ 0.0f, 0.0f, 0.0f };
	PSMTXMultVec(mtx, &lightPos, &lightPos);
	mapSetMaterialLight(0, lightPos);
	color1 = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
	color2 = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
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

	pose = shadow->offscreenId;
	shadow->flags |= 0x10;

	if (pose != -1) {
		if (shadow->captureMode != 0) {
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
					if (shadow->captureMode == 1) {
						shadow->imageData = smartAlloc(sizeA, 1);
					} else if (shadow->captureMode == 2) {
						shadow->imageData = smartAlloc(sizeA, 0);
					}
					memcpy(shadow->imageData->address, imageA->address, sizeA);
					DCFlushRange(shadow->imageData->address, sizeA);
					GXInitTexObj(&shadow->texObjData, shadow->imageData->address, widthA, heightA, fmtTexA, GX_CLAMP, GX_CLAMP, GX_FALSE);
					GXInitTexObjLOD(&shadow->texObjData, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
					offscreenGetBoundingBox(pose, &shadow->bboxX, &shadow->bboxY, &shadow->bboxW, &shadow->bboxH);
					shadow->bboxW -= shadow->bboxX;
					shadow->bboxH -= shadow->bboxY;
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
				shadow->texObjData = *texObj;
				shadow->imageData = image;
				offscreenGetBoundingBox(pose, &shadow->bboxX, &shadow->bboxY, &shadow->bboxW, &shadow->bboxH);
				shadow->bboxW -= shadow->bboxX;
				shadow->bboxH -= shadow->bboxY;
				shadow->flags &= ~0x10;
			}
		}
	} else {
		if (shadow->captureMode != 0) {
			if (shadow->flags & 0x100) {
				GXTexFmt fmtTex, fmtDst;
				u32 size;
				s32 mode;

				shadow->bboxX = shadow->left;
				shadow->bboxY = shadow->top;
				shadow->bboxW = entry->captureWidth;
				shadow->bboxH = entry->captureHeight;
				height = shadow->bboxH;
				width = shadow->bboxW;
				fmtTex = shadow->texFormat;
				fmtDst = shadow->dstFormat;
				mode = shadow->captureMode;
				GXSetTexCopySrc(shadow->bboxX, shadow->bboxY, width, height);
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
				GXInitTexObj(&shadow->texObjData, image->address, width, height, fmtTex, GX_CLAMP, GX_CLAMP, GX_FALSE);
				GXInitTexObjLOD(&shadow->texObjData, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
				shadow->imageData = image;
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

			shadow->bboxX = shadow->left;
			shadow->bboxY = shadow->top;
			shadow->bboxW = entry->captureWidth;
			shadow->bboxH = entry->captureHeight;
			heightB = shadow->bboxH;
			widthB = shadow->bboxW;
			fmtTexB = shadow->texFormat;
			fmtDstB = shadow->dstFormat;
			modeB = shadow->captureMode;
			GXSetTexCopySrc(shadow->bboxX, shadow->bboxY, widthB, heightB);
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
			GXInitTexObj(&shadow->texObjData, imageB->address, widthB, heightB, fmtTexB, GX_CLAMP, GX_CLAMP, GX_FALSE);
			GXInitTexObjLOD(&shadow->texObjData, GX_NEAR, GX_NEAR, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
			shadow->imageData = imageB;
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
			if (entry->captureCamId == camId) {
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
	entry->captureWidth = 0;
	entry->captureHeight = 0;
	entry->posX = 0.0f;
	entry->posY = 0.0f;
	entry->order = 0.0f;
	entry->loopTimes = 0.0f;
	entry->shadowId = 0;
	entry->depthPlaneOffset = 0.0f;
	MTXIdentity(entry->matrix);
	entry->camId = CAMERA_3D_IMAGE;
	entry->captureCamId = 4;

	entry->shadows[0].flags = 0;
	entry->shadows[0].left = 0;
	entry->shadows[0].top = 0;
	entry->shadows[0].offscreenId = -1;
	entry->shadows[0].dstFormat = 6;
	entry->shadows[0].texFormat = 6;
	entry->shadows[0].captureMode = 0;

	entry->shadows[1].flags = 0;
	entry->shadows[1].left = 0;
	entry->shadows[1].top = 0;
	entry->shadows[1].offscreenId = -1;
	entry->shadows[1].dstFormat = 6;
	entry->shadows[1].texFormat = 6;
	entry->shadows[1].captureMode = 0;

	entry->shadows[2].flags = 0;
	entry->shadows[2].left = 0;
	entry->shadows[2].top = 0;
	entry->shadows[2].offscreenId = -1;
	entry->shadows[2].dstFormat = 6;
	entry->shadows[2].texFormat = 6;
	entry->shadows[2].captureMode = 0;
	entry->shadows[2].dstFormat = 39;
	entry->shadows[2].texFormat = 1;

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
		else if (!shadow->captureMode) {
			shadow->flags = 0;
		}
		else {
			shadow->flags = 0;
			smartFree(shadow->imageData);
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

void imgShadowDisp(CameraId camId, void* param) {
	ImageEntry* entry;
	ImageShadow* shadow;
	Mtx mtx[3];
	Mtx mtxTmpB;
	Mtx mtxTmpA;
	Mtx mtxScale;
	Mtx mtxTrans;
	GXTexObj* texObj[3];
	void* texData[3];
	MtxPtr mtxPtr[3];
	s32 firstShadow;
	s32 i;
	f32 v1, v3, v2;
	f32 w0, w1, w2, w3;
	f32 o, z;

	entry = param;
	firstShadow = -1;
	for (i = 0; i < 3; i++) {
		shadow = &entry->shadows[i];
		if (!(shadow->flags & 4)) {
			texObj[i] = NULL;
			texData[i] = NULL;
		}
		else {
			if (shadow->flags & 0x10) {
				texObj[i] = NULL;
				texData[i] = NULL;
			}
			else {
				f32 v0;
				if (entry->flags & 4) {
					v0 = shadow->bboxX; // capture bounds: x, y, width, height
					v1 = shadow->bboxY;
					v2 = shadow->bboxW;
					v3 = shadow->bboxH;
				}
				else {
					v0 = shadow->left;
					v1 = shadow->top;
					v2 = entry->captureWidth;
					v3 = entry->captureHeight;
				}
				PSMTXTrans(mtxTrans,
					-((f32)shadow->bboxX - v0) / (f32)shadow->bboxW,
					-((f32)shadow->bboxY - v1) / (f32)shadow->bboxH,
					0.0f);
				PSMTXScale(mtxScale, v2 / (f32)shadow->bboxW, v3 / (f32)shadow->bboxH, 1.0f);
				PSMTXConcat(mtxTrans, mtxScale, mtx[i]);
				texObj[i] = &shadow->texObjData;
				texData[i] = shadow->imageData;
			}
		}
		if (texObj[i] != NULL) {
			mtxPtr[i] = mtx[i];
			if (firstShadow == -1) {
				firstShadow = i;
			}
		}
		else {
			mtxPtr[i] = NULL;
		}
	}

	if (firstShadow == -1) {
		entry->flags |= 0x10000000;
		return;
	}
	entry->flags &= ~0x10000000;

	animSetPaperTexMtx(mtxPtr[0], mtxPtr[1], mtxPtr[2]);
	// MUST_MATCH: the original materialises each boolean argument through a ?-expression, which
	// lands the two srwi sign-extractions in the opposite argument registers from the plain
	// comparison. The natural form is under #else.
#ifdef MUST_MATCH
	animSetPaperTexObj(texObj[0], texObj[1], texObj[2], texData[0], texData[1], texData[2],
		(entry->shadows[0].offscreenId != -1) ? 1 : 0, (entry->shadows[1].offscreenId != -1) ? 1 : 0);
#else
	animSetPaperTexObj(texObj[0], texObj[1], texObj[2], texData[0], texData[1], texData[2],
		entry->shadows[0].offscreenId != -1, entry->shadows[1].offscreenId != -1);
#endif

	if (entry->animName != NULL) {
		animPoseSetAnim(entry->poseId, entry->animName, 0);
	}
	animPoseSetStartTime(entry->poseId, entry->startTime);
	animPoseMain(entry->poseId);
	animPoseSetMaterialLightFlagOn(entry->poseId, 1);
	entry->loopTimes = animPoseGetLoopTimes(entry->poseId);

	// MUST_MATCH: the original keeps an if/else on flags bit 3 whose two arms assign the same
	// constants, and the branch survives in the object; the natural form is under #else.
#ifdef MUST_MATCH
	if (entry->flags & 8) {
		o = 1.0f;
		z = 0.0f;
	}
	else {
		o = 1.0f;
		z = 0.0f;
	}
#else
	o = 1.0f;
	z = 0.0f;
#endif

	if (entry->flags & 4) {
		w0 = entry->shadows[firstShadow].bboxX; // capture bounds: x, y, width, height
		w1 = entry->shadows[firstShadow].bboxY;
		w2 = entry->shadows[firstShadow].bboxW;
		w3 = entry->shadows[firstShadow].bboxH;
	}
	else {
		w0 = entry->shadows[firstShadow].left;
		w1 = entry->shadows[firstShadow].top;
		w2 = entry->captureWidth;
		w3 = entry->captureHeight;
	}

	PSMTXTrans(mtxTmpB, 64.0f * ((entry->posX + w0) / (f32)DEMOGetRenderModeObj()->fbWidth),
	           48.0f * (-(entry->posY + w1) / (f32)DEMOGetRenderModeObj()->efbHeight), z);
	PSMTXTrans(mtxTmpA, -32.0f, 24.0f, 0.0f);
	PSMTXConcat(mtxTmpB, mtxTmpA, mtxTmpB);
	PSMTXScale(mtxTmpA, w2 / (f32)DEMOGetRenderModeObj()->fbWidth,
	           w3 / (f32)DEMOGetRenderModeObj()->efbHeight,
	           w2 / (f32)DEMOGetRenderModeObj()->fbWidth);
	PSMTXConcat(mtxTmpB, mtxTmpA, mtxTmpB);
	PSMTXTrans(mtxTmpA, 32.0f, -24.0f, 0.0f);
	PSMTXConcat(mtxTmpB, mtxTmpA, mtxTmpB);

	animPoseDrawMtx(entry->poseId, mtxTmpB, 1, 0.0f, o);
	animPoseDrawMtx(entry->poseId, mtxTmpB, 2, 0.0f, o);

	switch (entry->shadowId) {
	case 1:
		shadowProjShadowDrawCountUp();
		break;
	case 2:
		shadowDepthShadowDrawCountUp();
		break;
	}
}
