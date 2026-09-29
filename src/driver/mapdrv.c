#include "driver/mapdrv.h"
#include "driver/arcdrv.h"
#include "driver/camdrv.h"
#include "driver/offscreendrv.h"
#include "manager/dvdmgr.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"
#include "texPalette.h"
#include <math.h>
#include <string.h>
#include "driver/hitdrv.h"
#include "driver/lightdrv.h"
#include "driver/envdrv.h"
#include "manager/arammgr.h"
#include "matching.h"
#include <stdio.h>

extern s32 texmtx_tbl[];

extern f32 queen_uv[4]; //gxsub.c
extern GlobalWork* gp;

#define TWO_PI 6.28318530717958647692528676655900576839433879875021f

//.bss
MapError error_data[32];

//.sbss
MapWork* mapWork; //allocates two entries
s32 activeGroup;
BOOL mapClipOffFlag;
MapObject* current_mp;
BOOL error_flag;
s32 error_count;
u32 unk_80418458;
BOOL __mapdrv_make_dl;
//more stuff

//.sbss (unnamed in symbols.txt)
GXColor paper_ambient;
GXTexObj projTexObj;
void* projTexImgRef;
Mtx projMtx;

extern GXColor dat_8041982c;
extern GXColor dat_80419828;
extern GXColor dat_80419830;
extern GXColor dat_80419840;
extern GXColor dat_80419844;
extern GXColor dat_80419820;
extern GXColor unk_80422f8c;
extern GXColor unk_80422f90;
extern GXColor unk_80422f84;
extern f32 float_0_80419850;
extern f32 float_1000_80419874;
extern f32 float_10_80419864;
extern f32 float_deg2rad_80419868;
extern f32 float_1_80419860;
extern f32 float_2_80419858;
extern f32 float_3_8041984c;
extern f32 float_neg2_804198a0;
extern f32 float_1Eneg08_804198b8;
extern Vec vec3_802c18f4[];
extern Vec vec3_802c1900[];
extern const char str_8041986C[];
const char str_8041986C[] = "|";

//.sdata
f32 scrl_uv[4] = {0.0f, 0.0f, 0.0f, 0.0f};
s32 fog_type[5] = {2, 4, 5, 6, 7};

//forward decls for functions defined later in this file
static MapObject* mapGetMapObjSub(const char* name);

void mapGrpSetFlushColor(const char* name, u8 r, u8 g, u8 b, u8 a) {
	GXColor c;
	GXColor color;

	color = *(GXColor*)&unk_80422f90;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	c = color;
	mapGrpSetColor(name, &c);
	mapGrpFlagOff(name, 0x40);
}

// NONMATCHING (98.89%): register allocation of the first inlined search and error body
void mapObjSetFlushColor(const char* name, u8 r, u8 g, u8 b, u8 a) {
	GXColor color;
	MapObject* obj;

	color = *(GXColor*)&unk_80422f8c;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	obj = mapGetMapObjSub(name);
	if (obj) {
		*(u32*)obj->field_0xC = *(u32*)&color;
		*(u32*)obj->field_0x0 |= 0x40;
	}
	mapObjFlagOff(name, 0x40);
}

void* mapSearchDmdJoint(void* a, s8* b) {
	if (*b != 0x7C) {
		return mapSearchDmdJointSub(a, b);
	}
	else {
		return mapSearchDmdJointSub2(a, b, "|");
	}
}

void mapDispMapGrp_bbox(MapObject* obj, MapEntry* entry) {
	sysWaitDrawSync();
	GXClearBoundingBox();
	_mapDispMapGrp_NoMaterial(obj, entry);
	sysWaitDrawSync();
	GXReadBoundingBox((u16*)((u8*)entry + 0xEC), (u16*)((u8*)entry + 0xEE), (u16*)((u8*)entry + 0xF0), (u16*)((u8*)entry + 0xF2));
}

// NONMATCHING (98.84%): loop i/offset register swap only
void mapDispMapObj_bbox(s32 camId, MapObject* obj) {
	Mtx invMtx;
	Mtx mtx;
	s32 i;
	GXColor color;

	sysWaitDrawSync();
	GXClearBoundingBox();
	PSMTXConcat(camGetPtr(camId)->view, obj->modelWorldMtx, mtx);
	GXLoadPosMtxImm(mtx, GX_PNMTX0);
	PSMTXInvXpose(mtx, invMtx);
	GXLoadNrmMtxImm(invMtx, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
	GXSetCullMode(GX_CULL_ALL);
	GXSetNumChans(1);
	color = unk_80422f84;
	GXSetChanMatColor(GX_COLOR0A0, color);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(0);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	for (i = 0; i < obj->joints->partCount; i++) {
		mapSetPolygon(obj, obj->joints->parts[i].mesh);
	}
	sysWaitDrawSync();
	GXReadBoundingBox((u16*)((u8*)obj + 0xEC), (u16*)((u8*)obj + 0xEE), (u16*)((u8*)obj + 0xF0), (u16*)((u8*)obj + 0xF2));
}

void mapObjGetPos(const char* name, Vec* pos) {
	Vec pos2;
	s32 scale;
	MapObject* obj;

	scale = 0;
	pos2 = vec3_802c18f4[0];
	obj = mapGetMapObjSub(name);

	if (obj == NULL) {
		*pos = vec3_802c1900[0];
	}
	else {
		mapObjGetPosSub(obj, &pos2, &scale, NULL);
		PSVECScale(&pos2, pos, float_1_80419860 / (f32)scale);
	}
}

void _mapDispMapObj_NoMaterial(s32 camId, MapObject* obj) {
	Mtx mtx;
	Mtx invMtx;
	GXColor color;
	s32 i;

	PSMTXConcat(camGetPtr(camId)->view, obj->modelWorldMtx, mtx);
	GXLoadPosMtxImm(mtx, GX_PNMTX0);
	PSMTXInvXpose(mtx, invMtx);
	GXLoadNrmMtxImm(invMtx, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
	GXSetCullMode(GX_CULL_ALL);
	GXSetNumChans(1);
	color = unk_80422f84;
	GXSetChanMatColor(GX_COLOR0A0, color);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
	GXSetNumTexGens(0);
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	for (i = 0; i < obj->joints->partCount; i++) {
		mapSetPolygon(obj, obj->joints->parts[i].mesh);
	}
}

static MapObject* mapGetMapObjSub(const char* name) {
	MapEntry* entry;
	MapWork *wp;
	int j;
	int i;
	MapObject *object;

	wp = &mapWork[activeGroup];
	if (!name) {
		return 0;
	}
	entry = &wp->entries[0];
	for (i = 0; i < wp->entries[0].count; entry++, i++) {
		object = entry->objects;
		for (j = 0; j < entry->numJoints; j++, object++) {
			if (!strcmp(object->joints->name, name)) {
				return object;
			}
		}
	}
	return 0;
}

// Keep mapGrpSetColor out of line: the original calls it rather than inlining.
#pragma dont_inline on
// NONMATCHING (94.47%): *color load is scheduled after the inlined mapGetMapObjSub search
void mapGrpSetColor(const char* name, GXColor* color) {
	GXColor c = *color;

	setColor(mapGetMapObjSub(name), &c, 0);
}
#pragma dont_inline reset

void mapObjSetColor(const char* name, GXColor* color) {
	MapObject* obj = mapGetMapObjSub(name);

	if (obj) {
		*(u32*)((u8*)obj + 0xC) = *(u32*)color;
		*(u32*)obj->field_0x0 |= 0x40;
	}
}

void mapGrpClearOffScreen(const char* name) {
	_setOffScrnId(mapGetMapObjSub(name), -1, 0);
}

void mapObjClearOffScreen(const char* name) {
	MapObject* obj = mapGetMapObjSub(name);

	*(s16*)obj->field_0xDC = -1;
	*(u32*)obj->field_0x0 &= ~0x80;
}

// NONMATCHING (97.86%): offscreenNameToId result goes r3->r4 directly; target uses r0
void mapGrpSetOffScreen(const char* name, const char* offName) {
	_setOffScrnId(mapGetMapObjSub(name), offscreenNameToId(offName), 0);
}

void mapObjSetOffScreen(const char* name, const char* offName) {
	MapObject* obj = mapGetMapObjSub(name);

	*(s16*)obj->field_0xDC = offscreenNameToId(offName);
	*(u32*)obj->field_0x0 |= 0x80;
}

void mapObjScale(const char* name, f32 x, f32 y, f32 z) {
	MapObject* obj = mapGetMapObjSub(name);

	if (obj) {
		Mtx m;
		u32* flags = (u32*)obj->field_0x0;

		if (!(*flags & 0x200)) {
			PSMTXIdentity(obj->unkAC);
		}
		PSMTXScale(m, x, y, z);
		PSMTXConcat(obj->unkAC, m, obj->unkAC);
		*flags |= 0x200000;
		*flags |= 0x200;
	}
}

void mapObjTranslate(const char* name, f32 x, f32 y, f32 z) {
	MapObject* obj = mapGetMapObjSub(name);

	if (obj) {
		Mtx m;
		u32* flags = (u32*)obj->field_0x0;

		if (!(*flags & 0x200)) {
			PSMTXIdentity(obj->unkAC);
		}
		PSMTXTrans(m, x / float_10_80419864, y / float_10_80419864, z / float_10_80419864);
		PSMTXConcat(obj->unkAC, m, obj->unkAC);
		*flags |= 0x200000;
		*flags |= 0x200;
	}
}

void mapObjRotate(const char* name, f32 x, f32 y, f32 z) {
	MapObject* obj = mapGetMapObjSub(name);

	if (obj) {
		Mtx mx;
		Mtx my;
		Mtx mz;
		u32* flags = (u32*)obj->field_0x0;

		if (!(*flags & 0x200)) {
			PSMTXIdentity(obj->unkAC);
		}
		PSMTXRotRad(mx, 'x', float_deg2rad_80419868 * x);
		PSMTXRotRad(my, 'y', float_deg2rad_80419868 * y);
		PSMTXRotRad(mz, 'z', float_deg2rad_80419868 * z);
		PSMTXConcat(obj->unkAC, mz, obj->unkAC);
		PSMTXConcat(obj->unkAC, my, obj->unkAC);
		PSMTXConcat(obj->unkAC, mx, obj->unkAC);
		*flags |= 0x200000;
		*flags |= 0x200;
	}
}

void mapObjGetFlushColor(const char* name, u8* r, u8* g, u8* b, u8* a) {
	MapObject* obj = mapGetMapObjSub(name);

	if (!obj) {
		mapErrorEntry(0, name);
		return;
	}
	*r = *(u8*)((u8*)obj + 0xC);
	*g = *(u8*)((u8*)obj + 0xD);
	*b = *(u8*)((u8*)obj + 0xE);
	*a = *(u8*)((u8*)obj + 0xF);
}

void test_kururing_mapdisp(CameraId camId) {
	MapWork* wp = &mapWork[activeGroup];
	MapEntry* entry = &wp->entries[0];
	u32 flags;
	s32 j;
	s32 i;
	MapObject* obj;

	for (i = 0; i < wp->entries[0].count; entry++, i++) {
		obj = entry->objects;
		for (j = 0; j < entry->numJoints; j++, obj++) {
			flags = *(u32*)obj->field_0x0;
			if (flags & 0x400000) {
				*(u32*)obj->field_0x0 = flags & ~0x2;
				_mapDispMapObj(camId, obj);
				*(u32*)obj->field_0x0 = flags;
			}
		}
	}
}

void mapPauseAnimationAll(void) {
	MapWork* wp = &mapWork[activeGroup];
	MapEntry* entry = &wp->entries[0];
	s32 j;
	s32 i;

	for (i = 0; i < wp->entries[0].count; entry++, i++) {
		MapEntryAnimData* anim = entry->animData;
		for (j = 0; j < *(s32*)entry->field_0x160; j++, anim++) {
			*(u16*)anim |= 0x10;
		}
	}
}

void mapReStartAnimationAll(void) {
	MapWork* wp = &mapWork[activeGroup];
	MapEntry* entry = &wp->entries[0];
	s32 j;
	s32 i;

	for (i = 0; i < wp->entries[0].count; entry++, i++) {
		MapEntryAnimData* anim = entry->animData;
		for (j = 0; j < *(s32*)entry->field_0x160; j++, anim++) {
			*(u16*)anim |= 0x20;
		}
	}
}

// NONMATCHING (99.13%): inlined mapSearchAnmObj swaps wp/entry (target has wp in the higher register)
void mapPauseAnimation(const char* name) {
	MapEntryAnimData* found = mapSearchAnmObj(name);

	if (found) {
		*(u16*)found |= 0x10;
	}
}

// NONMATCHING (99.13%): inlined mapSearchAnmObj swaps wp/entry (target has wp in the higher register)
void mapReStartAnimation(const char* name) {
	MapEntryAnimData* found = mapSearchAnmObj(name);

	if (found) {
		*(u16*)found |= 0x20;
	}
}

// NONMATCHING (99.15%): inlined mapSearchAnmObj swaps wp/entry (target has wp in the higher register)
void mapSetPlayRate(const char* name, f32 rate) {
	MapEntryAnimData* found = mapSearchAnmObj(name);

	if (found) {
		*(f32*)((u8*)found + 0x14) = rate;
	}
}

//local prototypes
//anim functions?

void mapErrorEntry(s32 arg0, const char* arg1) { //1:1
	// Indexing error_data directly (rather than a saved base pointer) is what makes MWCC's inlined copies
	// of this function allocate the index to r0; the saved-pointer form does not inline to the target code.
	error_data[error_count].field_0x20 = arg0;
	strncpy(error_data[error_count].field_0x0, arg1, sizeof(error_data[0].field_0x0) - 1);
	error_flag = TRUE;
	error_count++;
	error_count %= 32;
}

const char* getMapDataDvdRoot(void) {
	return ".";
}

void N_mapDispOff(void) {
	unk_80418458 = TRUE;
}

void N_mapDispOn(void) {
	unk_80418458 = FALSE;
}

MapWork* mapGetWork(void) {
	return &mapWork[activeGroup];
}

s32 mapGetActiveGroup(void) {
	return activeGroup;
}

void mapResetPaperAmbColor(void) {
	paper_ambient = dat_8041982c;
}

void mapSetPaperAmbColor(GXColor* color) {
	paper_ambient = *color;
}

void mapSetTevCallback(s32 idx, void* cb) {
	u8* base = (u8*)mapWork + activeGroup * 0x2F4 + 0x110;

	*(void**)(base + idx * 4) = cb;
}

void mapSetProjMtx(Mtx mtx) {
	PSMTXCopy(mtx, projMtx);
}

void mapSetProjTexObj(GXTexObj* obj, void* imgRef) {
	projTexObj = *obj;
	projTexImgRef = imgRef;
}

void mapBlendOff2(void) {
	*(u16*)((u8*)mapWork + activeGroup * 0x2F4 + 0x4) &= ~0x4;
}

void mapBlendOff(void) {
	*(u16*)((u8*)mapWork + activeGroup * 0x2F4 + 0x4) &= ~0x2;
}

void mapGetBlend2(GXColor* out) {
	MapEntry* entry = &mapWork[activeGroup].entries[0];

	if (entry->flags & 0x4) {
		*out = *(GXColor*)((u8*)entry + 0x1A);
	}
	else {
		*out = dat_80419844;
	}
}

void mapGetBlend(GXColor* out) {
	MapEntry* entry = &mapWork[activeGroup].entries[0];

	if (entry->flags & 0x2) {
		*out = *(GXColor*)((u8*)entry + 0x16);
	}
	else {
		*out = dat_80419840;
	}
}

void mapSetBlend2(GXColor* color) {
	MapEntry* entry = &mapWork[activeGroup].entries[0];

	*(GXColor*)((u8*)entry + 0x1A) = *color;
	entry->flags |= 0x4;
}

void mapSetBlend(GXColor* color) {
	MapEntry* entry = &mapWork[activeGroup].entries[0];

	*(GXColor*)((u8*)entry + 0x16) = *color;
	entry->flags |= 0x2;
}

void mapFogOff(void) {
	*(u16*)((u8*)mapWork + activeGroup * 0x2F4 + 0x4) &= ~0x1;
}

void mapFogOn(void) {
	*(u16*)((u8*)mapWork + activeGroup * 0x2F4 + 0x4) |= 0x1;
}

void mapGetFog(u32* c, f32* near, f32* far, u32* c2) {
	MapEntry* entry = &mapWork[activeGroup].entries[0];

	*c = *(u32*)((u8*)entry + 0x28);
	*near = *(f32*)((u8*)entry + 0x2C);
	*far = *(f32*)((u8*)entry + 0x30);
	*c2 = *(u32*)((u8*)entry + 0x34);
}

// NONMATCHING (97.00%): register allocation of the inlined mapWork/activeGroup address computation
void mapSetFog(u32 color, f32 near, f32 far, u32* color2) {
	MapWork* wp = mapWork + activeGroup;
	MapEntry* entry = &wp->entries[0];
	u32 c2;

	c2 = *color2;
	*(u32*)((u8*)entry + 0x28) = color;
	*(f32*)((u8*)entry + 0x2C) = near;
	*(f32*)((u8*)entry + 0x30) = far;
	*(u32*)((u8*)entry + 0x34) = c2;
}

void mapGrpFlushOff(const char* name) {
	mapGrpFlagOff(name, 0x100000);
}

void mapGrpFlushOn(const char* name) {
	mapGrpFlagOn(name, 0x100000);
}

// Keep mapGrpFlagOff, mapGrpFlagOn out of line: the original calls them rather than inlining.
#pragma dont_inline on
void mapGrpFlagOff(const char* name, u32 flag) {
	MapObject* obj = mapGetMapObjSub(name);

	if (obj == NULL) {
		mapErrorEntry(0, name);
	}
	else {
		mapFlgOff(obj, flag, 0);
	}
}

void mapGrpFlagOn(const char* name, u32 flag) {
	MapObject* obj = mapGetMapObjSub(name);

	if (obj == NULL) {
		mapErrorEntry(0, name);
	}
	else {
		mapFlgOn(obj, flag, 0);
	}
}
#pragma dont_inline reset

void mapObjFlagOff(const char* name, u32 flag) {
	MapObject* obj = mapGetMapObjSub(name);

	if (obj == NULL) {
		mapErrorEntry(0, name);
	}
	else {
		*(u32*)obj->field_0x0 &= ~flag;
	}
}

void mapObjFlagOn(const char* name, u32 flag) {
	MapObject* obj = mapGetMapObjSub(name);

	if (obj == NULL) {
		mapErrorEntry(0, name);
	}
	else if (!(flag & 0x100) || *(void**)((u8*)obj + 0x114)) {
		*(u32*)obj->field_0x0 |= flag;
	}
}

void mapObjFlushOff(const char* name) {
	MapObject* obj = mapGetMapObjSub(name);

	if (obj == NULL) {
		mapErrorEntry(0, name);
	}
	else {
		*(u32*)obj->field_0x0 &= ~0x100000;
	}
}

void mapObjFlushOn(const char* name) {
	MapObject* obj = mapGetMapObjSub(name);

	if (obj == NULL) {
		mapErrorEntry(0, name);
	}
	else {
		*(u32*)obj->field_0x0 |= 0x100000;
	}
}

void mapDispMapGrp(MapObject* obj, void* a2) {
	_mapDispMapGrp(obj, a2);
}

void mapDispMapObj(CameraId camId, MapObject* obj) {
	_mapDispMapObj(camId, obj);
}

void mapDispMapGrp_off(MapObject* obj, void* a2) {
	u16 v0;
	u16 v1;
	u16 v2;
	u16 v3;

	sysWaitDrawSync();
	GXClearBoundingBox();
	_mapDispMapGrp(obj, a2);
	sysWaitDrawSync();
	GXReadBoundingBox(&v0, &v1, &v2, &v3);
	offscreenAddBoundingBox(*(s16*)((u8*)a2 + 0xDC), v0, v1, v2, v3);
}

void mapDispMapObj_off(CameraId camId, MapObject* obj) {
	u16 v0;
	u16 v1;
	u16 v2;
	u16 v3;

	sysWaitDrawSync();
	GXClearBoundingBox();
	_mapDispMapObj(camId, obj);
	sysWaitDrawSync();
	GXReadBoundingBox(&v0, &v1, &v2, &v3);
	offscreenAddBoundingBox(*(s16*)obj->field_0xDC, v0, v1, v2, v3);
}

BOOL mapTestXLU(u32 flags, GXColor* a, GXColor* b) {
	if ((flags & 0x40) && a->a != 0xFF) {
		return TRUE;
	}
	if (b->a != 0xFF) {
		return TRUE;
	}
	if (!(flags & 0x800) && (mapWork[activeGroup].entries[0].flags & 0x2) && mapWork[activeGroup].entries[0].field_0x16[3] != 0xFF) {
		return TRUE;
	}
	if (!(flags & 0x1000) && (mapWork[activeGroup].entries[0].flags & 0x4) && mapWork[activeGroup].entries[0].field_0x16[7] != 0xFF) {
		return TRUE;
	}
	return FALSE;
}

// NONMATCHING (93.88%): the spline arithmetic and control flow match instruction-for-instruction; only
// MWCC's register coloring differs (target keeps u/dt in f1/f2 and uses r4/r6/r8 as point/tangent/knot pointers).
void spline_getvalue(f32* out, s32 type, s32 a, s32 b, s32 c, f32 t) {
	f32* points = (f32*)a;
	f32* knots = (f32*)b;
	f32* tangents = (f32*)c;
	s32 lo;
	s32 hi;
	s32 mid;
	f32 f0, f1, f2, f3, f4, f5, f6, f7, f8;

	if (type == 2) {
		out[0] = t * (points[3] - points[0]) + points[0];
		out[1] = t * (points[4] - points[1]) + points[1];
		out[2] = t * (points[5] - points[2]) + points[2];
		return;
	}

	lo = 0;
	hi = type - 1;
	while (lo < hi) {
		mid = (lo + hi) / 2;
		if (knots[mid] < t) {
			lo = mid + 1;
		}
		else {
			hi = mid;
		}
	}
	if (lo > 0) {
		lo--;
	}

	f2 = knots[lo];
	f6 = 2.0f;
	f7 = tangents[lo * 3 + 0];
	f4 = tangents[lo * 3 + 3];
	f1 = t - f2;
	f0 = knots[lo + 1];
	f5 = f4 - f7;
	f8 = points[lo * 3 + 0];
	f3 = points[lo * 3 + 3];
	f2 = f0 - f2;
	f4 = f6 * f7 + f4;
	f0 = 3.0f;
	f5 = f1 * f5;
	f3 = f3 - f8;
	f5 = f5 / f2;
	f3 = f3 / f2;
	f5 = f0 * f7 + f5;
	f3 = f3 - f2 * f4;
	f3 = f1 * f5 + f3;
	f3 = f1 * f3 + f8;
	out[0] = f3;

	f7 = tangents[lo * 3 + 1];
	f4 = tangents[lo * 3 + 4];
	f5 = f4 - f7;
	f8 = points[lo * 3 + 1];
	f3 = points[lo * 3 + 4];
	f4 = f6 * f7 + f4;
	f5 = f1 * f5;
	f3 = f3 - f8;
	f5 = f5 / f2;
	f3 = f3 / f2;
	f5 = f0 * f7 + f5;
	f3 = f3 - f2 * f4;
	f3 = f1 * f5 + f3;
	f3 = f1 * f3 + f8;
	out[1] = f3;

	f7 = tangents[lo * 3 + 2];
	f4 = tangents[lo * 3 + 5];
	f5 = f4 - f7;
	f8 = points[lo * 3 + 2];
	f3 = points[lo * 3 + 5];
	f4 = f6 * f7 + f4;
	f5 = f1 * f5;
	f3 = f3 - f8;
	f5 = f5 / f2;
	f3 = f3 / f2;
	f5 = f0 * f7 + f5;
	f3 = f3 - f2 * f4;
	f3 = f1 * f5 + f3;
	f3 = f1 * f3 + f8;
	out[2] = f3;
}

//heavily inlined
void mapGetJointsSub(MapFileJoint* joint, s32* numJoints) { //1:1
	(*numJoints)++;
	if (joint->child) {
		mapGetJointsSub(joint->child, numJoints);
	}
	if (joint->next) {
		mapGetJointsSub(joint->next, numJoints);
	}
}

s32 mapGetJoints(MapFileJoint* joint) { //1:1
	s32 numJoints;

	numJoints = 0;
	if (joint) {
		numJoints = 1;
		if (joint->child) {
			mapGetJointsSub(joint->child, &numJoints);
		}
	}
	return numJoints;
}

// NONMATCHING (89.41%): local .rodata constants land at different offsets than vec3_802c16d8/e4
void mapGetBoundingBox(Vec min, Vec max) {
	Vec vec;
	MapFileJoint* joint;
	MapObject* obj;
	MapWork* wp;
	int i;

	wp = &mapWork[activeGroup];
	min = (Vec){0.0f, 0.0f, 0.0f};
	max = (Vec){0.0f, 0.0f, 0.0f};
	obj = wp->entries[0].objects;
	for (i = 0; i < wp->entries[0].numJoints; i++, obj++) {
		joint = obj->joints;
		if (joint && joint->partCount) {
			MTXMultVec(obj->modelWorldMtx, &joint->bboxMin, &vec);
			if (min.x > vec.x) {
				min.x = vec.x;
			}
			if (min.y > vec.y) {
				min.y = vec.y;
			}
			if (min.z > vec.z) {
				min.z = vec.z;
			}
			if (max.x < vec.x) {
				max.x = vec.x;
			}
			if (max.y < vec.y) {
				max.y = vec.y;
			}
			if (max.z < vec.z) {
				max.z = vec.z;
			}
			MTXMultVec(obj->modelWorldMtx, &obj->joints->bboxMax, &vec);
			if (min.x > vec.x) {
				min.x = vec.x;
			}
			if (min.y > vec.y) {
				min.y = vec.y;
			}
			if (min.z > vec.z) {
				min.z = vec.z;
			}
			if (max.x < vec.x) {
				max.x = vec.x;
			}
			if (max.y < vec.y) {
				max.y = vec.y;
			}
			if (max.z < vec.z) {
				max.z = vec.z;
			}
		}
	}
}

void mapInit(void) {
	mapWork = __memAlloc(HEAP_DEFAULT, sizeof(MapWork) * 2);
	memset(mapWork, 0, sizeof(MapWork) * 2);
	activeGroup = 0;
	mapClipOffFlag = FALSE;
}

// NONMATCHING (94.96%): texture-table search register permutation (target walks table in r20, idx in r22)
void mapBuildTexture(MapObject* obj, TEXPalette* texture, void* table) {
	int i;
	int j;

	for (i = 0; i < obj->joints->partCount; i++) {
		u8* material = (u8*)obj->joints->parts[i].material;

		for (j = 0; j < *(u8*)(material + 0xB); j++) {
			u8** ref = *(u8***)(material + 0xC + j * 4);
			const char* name = *(const char**)(*ref);
			TEXDescriptor* desc;
			s32 idx;

			for (idx = 0; idx < *(s32*)table; idx++) {
				if (strcmp(((const char**)table)[idx + 1], name) == 0) {
					break;
				}
			}
			if (idx == *(s32*)table) {
				idx = -1;
			}
			if (idx == -1) {
				idx = 0;
			}
			desc = TEXGet(texture, idx);
			*(u32*)(*ref + 0xC) = (u32)desc->textureHeader->data;
			*(u16*)(*ref + 8) = desc->textureHeader->width;
			*(u16*)(*ref + 0xA) = desc->textureHeader->height;
			switch (desc->textureHeader->format) {
				case 4: (*ref)[4] = 0; break;
				case 5: (*ref)[4] = 1; break;
				case 6: (*ref)[4] = 2; break;
				case 14: (*ref)[4] = 3; break;
				case 0: (*ref)[4] = 4; break;
				case 1: (*ref)[4] = 5; break;
				case 2: (*ref)[4] = 6; break;
				case 3: (*ref)[4] = 7; break;
				case 8: (*ref)[4] = 8; break;
				case 9: (*ref)[4] = 9; break;
				case 10: (*ref)[4] = 10; break;
			}
		}
	}
	if (obj->child) {
		mapBuildTexture(obj->child, texture, table);
	}
	if (obj->next) {
		mapBuildTexture(obj->next, texture, table);
	}
}

// NONMATCHING (0.40%): only a stub is present
MapObject* _mapEnt(MapFileJoint* joint, MapObject* parent, Mtx mtx, s32 index) {
	return NULL;




}

//recursively inlined
// NONMATCHING (70.05%): partial recursive object-tree build; branch/register differences
MapObject* mapEntrySub(MapFileJoint* joint, MapObject* parent, Mtx mtx, BOOL isRoot, s32 index) {
	MapObject* obj = _mapEnt(joint, parent, mtx, index);
	if (joint->child) {
		obj->child = mapEntrySub(joint->child, obj, obj->modelWorldMtx, FALSE, index);
	}
	if (!isRoot) {
		if (joint->next) {
			obj->next = mapEntrySub(joint->next, parent, mtx, FALSE, index);
		}
	}
	return obj;
}

MapObject* mapEntry(MapFileJoint* joint, Mtx mtx, s32 index) {
	MapWork* wp;
	u32 size;

	wp = mapGetWork();

	wp->entries[index].numJoints = mapGetJoints(joint);
	size = OSRoundUp32B(wp->entries[index].numJoints * 0x134);
	wp->entries[index].objects = _mapAlloc(size);
	memset(wp->entries[index].objects, 0, size);
	return mapEntrySub(joint, NULL, mtx, TRUE, index);
}


// NONMATCHING (0.70%): only the opening/loop skeleton is present
void makeDisplayList(BOOL group) {

}

MapEntryAnimData* mapSearchAnmObj(const char* animName) {
	MapEntry* entry;
	MapWork* wp;
	s32 j;
	s32 i;

	wp = mapGetWork();
	entry = &wp->entries[0];

	for (i = 0; i < wp->entries[0].count; entry++, i++) {
		MapEntryAnimData* anim = entry->animData;
		for (j = 0; j < *(s32*)entry->field_0x160; j++, anim++) {
			if ((*(u16*)anim & 0x1) && !strcmp(*(char**)*(void**)((u8*)anim + 0x18), animName)) {
				return anim;
			}
		}
	}
	return NULL;
}

// NONMATCHING (99.94%): only the pool symbol names for the 1.0f/1000.0f literals and the
// double_to_int constant differ (compiler emits local @nnnn entries instead of the named
// .sdata2 symbols); schedule and all other instructions match.
void mapPlayAnimationLv(const char* name, s32 a2, s32 a3) {
	MapWork* wp;
	MapEntry* entry;
	MapEntryAnimData* found = NULL;
	s32 j;
	s32 i;
	BOOL valid = FALSE;
	OSTime time;
	u8* found8;

	wp = &mapWork[activeGroup];
	entry = wp->entries;
	for (i = 0; i < wp->entries[0].count; entry++, i++) {
		found = entry->animData;
		for (j = 0; j < *(s32*)entry->field_0x160; j++, found++) {
			if (!strcmp(*(char**)*(void**)((u8*)found + 0x18), name)) {
				valid = TRUE;
				break;
			}
		}
		if (valid) {
			break;
		}
	}

	if (!valid) {
		return;
	}

	found8 = (u8*)found;

	*(u16*)found = 0;
	switch (a3) {
	case 0:
		break;
	case 1:
		*(u16*)found |= 0x100;
		break;
	case 2:
		*(u16*)found |= 0x200;
		break;
	case 3:
		*(u16*)found |= 0x400;
		break;
	case 4:
		*(u16*)found |= 0x800;
		break;
	}

	if (*(u16*)found & 0x100) {
		time = gp->systemLv0Time;
	}
	else if (*(u16*)found & 0x200) {
		time = gp->systemLv1Time;
	}
	else if (*(u16*)found & 0x400) {
		time = gp->systemLv2Time;
	}
	else if (*(u16*)found & 0x800) {
		time = gp->systemLv3Time;
	}
	else if (gp->inBattle) {
		time = gp->renderTime;
	}
	else {
		time = gp->renderFieldTime;
	}

	*(OSTime*)(found8 + 0x8) = OSTicksToMilliseconds(time);
	*(f32*)(found8 + 0x10) = float_0_80419850;
	*(f32*)(found8 + 0x14) = 1.0f;
	*(u16*)found |= 0x1;
	if (a2 == 1) {
		*(u16*)found |= 0x4;
	}
	if (a2 == 2) {
		*(OSTime*)(found8 + 0x8) -= (u64)(1000.0f * *(f32*)(*(u8**)(found8 + 0x18) + 8) / (f32)gp->framerate);
	}
}

// NONMATCHING (94.21%): search-loop register permutation and the search branch shape
void mapCheckAnimation(const char* animName, BOOL* valid, f32* a3) {
	MapEntryAnimData* found = mapSearchAnmObj(animName);

	*valid = FALSE;
	*a3 = float_0_80419850;
	if (found) {
		if (*(u16*)found & 0x2) {
			*valid = TRUE;
		}
		*a3 = 1000.0f * (*(f32*)(*(u8**)((u8*)found + 0x18) + 8) - *(f32*)((u8*)found + 0x10)) / (f32)gp->framerate;
	}
	else {
		*valid = TRUE;
	}
}

//more animation functions

// NONMATCHING (21.94%): partial map loader; large body still differs
void _mapLoad(MapWork* wp, BOOL group, const char* map) {
	MapHeader* header;
	MapEntry* entry;
	DVDEntry* dvd;
	char str1[128];
	char str2[128];
	char str3[128];
	u32 sizeCheck;
	u8 *data;
	u32 r4, r7, r0;
	int i;

	MapFileInfo* info;

	entry = &wp->entries[group];
	error_flag = FALSE;
	error_count = 0;
	strncpy(entry->name, map, sizeof(entry->name));

	sprintf(str1, "./m/%s/d", map);
	arcOpen(str1, &entry->dat, &entry->datSize);
	if (!entry->dat) {
		sprintf(str2, "%s/%s", ".", str1);
		dvd = DVDMgrOpen(str2, 2, 0);
		entry->datSize = DVDMgrGetLength(dvd);
		entry->dat = _mapAllocTail(OSRoundUp32B(entry->datSize));
		DVDMgrRead(dvd, entry->dat, OSRoundUp32B(entry->datSize), 0);
		DVDMgrClose(dvd);
	}

	sprintf(str1, "./m/%s/t", map);
	entry->tpl = NULL;
	arcOpen(str1, &entry->tpl, &entry->tplSize);
	if (entry->tpl) {
		UnpackTexPalette(entry->tpl);
	}
	else {
		sprintf(str3, "%s/%s", ".", str1);
		dvd = DVDMgrOpen(str3, 2, 0);
		if (dvd) {
			entry->tplSize = DVDMgrGetLength(dvd);
			entry->tpl = _mapAllocTail(OSRoundUp32B(entry->tplSize));
			DVDMgrRead(dvd, entry->tpl, OSRoundUp32B(entry->tplSize), 0);
			DVDMgrClose(dvd);
			UnpackTexPalette(entry->tpl);
		}
	}

	GXInvalidateVtxCache();
	header = &entry->header;
	data = (u8*)entry->dat;
	if (header != NULL) {
		memset(header, 0, sizeof(MapHeader));
		header->field_0x3C |= 1;
		memcpy(header, entry->dat, sizeof(MapFileHeader));
		if (header->file.fileSize == entry->datSize) {
			sizeCheck = sizeof(MapFileHeader);
			if (header->file.dataSize) {
				header->data = &data[sizeCheck];
				sizeCheck = header->file.dataSize + sizeof(MapFileHeader);
			}
			if (header->file.relCount) {
				header->rel = &data[sizeCheck];
				sizeCheck += 4 * header->file.relCount; //TODO: sizeof
			}
			if (header->file.chunkCount) {
				header->chunks = (MapFileChunk*)&data[sizeCheck];
				sizeCheck += sizeof(MapFileChunk) * header->file.chunkCount;
			}
			if (header->file.unused) {
				header->unused = &data[sizeCheck];
				sizeCheck += 8 * header->file.unused;
			}
			if (sizeCheck < header->file.fileSize) {
				header->strings = (const char*)&data[sizeCheck];
			}

			//iunno about this section
			for (i = 0; i < header->file.relCount; i++) {
				r4 = (u32)entry->header.rel;
				r7 = (u32)entry->header.data;
				r4 = *(u32*)(r4 + i);
				r0 = *(u32*)(r7 + r4);
				if (r0 <= 0x80000000) {
					*(u32*)(r7 + r4) = (r0 + r7);
				}
			}
		}
	}
	info = NULL;
	for (i = 0; i < header->file.chunkCount; i++) {
		if (!strcmp(&header->strings[header->chunks[i].string], "information")) {
			info = (MapFileInfo*)&data[header->chunks[i].offset];
			break;
		}
	}


}

// An empty string; symbols.txt names it as a float.
extern const char float_0_804198cc[4];

// NONMATCHING (98.06%): entry vs i register swap (target r29=entry, r30=i); opening load now matches
void mapUnLoad(void) {
	MapEntry* entry;
	s32 i;
	MapWork* wp;

	sysWaitDrawSync();
#ifdef MUST_MATCH
	wp = (MapWork*)mapWork; // redundant cast: makes MWCC load mapWork into wp's home register r31
#else
	wp = mapWork;
#endif
	activeGroup = 0;
	entry = &wp->entries[0];
	for (i = 0; i < wp->entries[0].count; entry++, i++) {
		if (entry->objects) {
			_mapFree(entry->objects);
			entry->numJoints = 0;
			entry->objects = NULL;
		}
		if (entry->animData) {
			_mapFree(entry->animData);
			*(s32*)entry->field_0x160 = 0;
			entry->animData = NULL;
		}
		if (*(void**)((u8*)entry + 0x16C)) {
			_mapFree(*(void**)((u8*)entry + 0x16C));
			*(void**)((u8*)entry + 0x16C) = NULL;
			*(void**)((u8*)entry + 0x168) = NULL;
		}
	}
	unk_80418458 = 0;
	hitReInit();
	lightReInit();
	envReInit();
	camUnLoadRoad(4);
	camSetTypePersp(4);
	entry = &wp->entries[0];
	for (i = 0; i < wp->entries[0].count; entry++, i++) {
		if (entry->dat) {
			if (!arcDataCheck(entry->dat)) {
				_mapFree(entry->dat);
			}
			entry->dat = NULL;
		}
		if (entry->tpl) {
			if (!arcDataCheck(entry->tpl)) {
				_mapFree(entry->tpl);
			}
			entry->tpl = NULL;
		}
		if (*(void**)entry->field_0x8C) {
			_mapFree(*(void**)entry->field_0x8C);
			*(void**)entry->field_0x8C = NULL;
		}
		strncpy(entry->name, float_0_804198cc, 0x10);
	}
	arcDelete(1);
	memset(wp, 0, 0x2F4);
}

BOOL mapPreLoad(void) {
	return TRUE; //do nothing
}

void mapLoad(const char* map) {
	MapWork* wp;

	sysWaitDrawSync();
#ifdef MUST_MATCH
	wp = (MapWork*)mapWork; // redundant cast: makes MWCC load into r31 (wp's home) instead of the memset arg r3
#else
	wp = mapWork;
#endif
	activeGroup = 0;
	memset(wp, 0, sizeof(MapWork));
	_mapLoad(wp, 0, map);
	wp->entries[0].count = 1;
}

// in-file layout for the animation key records mapCalcAnimMatrix walks
// (the joint pointer passed by hitdrv.c points at an array of these)
typedef struct MapAnimChannel {
	f32 value;  // 0x0
	f32 unk4;   // 0x4
	f32 unk8;   // 0x8
	f32 unkC;   // 0xC
	s32 flag;   // 0x10
} MapAnimChannel;

typedef struct MapAnimKey {
	f32 time;                 // 0x0
	MapAnimChannel chan[9];   // 0x4
	u8 unk[0xF0];            // 0xB8
} MapAnimKey;

typedef struct MapAnimData {
	u8 unk0[0x4];   // 0x0
	f32 base[6];    // 0x4
	f32 div[3];     // 0x1C
	u8 unk28[0x30]; // 0x28
	s32 count;      // 0x58
	MapAnimKey key[]; // 0x5C
} MapAnimData;

// NONMATCHING (0%): first pass
void mapCalcAnimMatrix(Mtx outMtx, Mtx inMtx, MapEntryAnimData* anim, void* jointPtr) {
	MapAnimData* joint = (MapAnimData*)jointPtr;
	MapAnimKey* key;
	MapAnimKey* next;
	f32 out[24];
	Mtx trans;
	Mtx scale;
	Mtx rotX;
	Mtx rotY;
	Mtx rotZ;
	Mtx local;
	f32 f31, f30, f29, f28, f27, f23;
	f32 f26, f25, f24;
	s32 count;
	s32 i, k;

	count = joint->count;
	for (i = 1; i < count; i++) {
		f32 t;

		key = &joint->key[i - 1];
		next = &joint->key[i];
		if (*(f32*)((u8*)anim + 0x10) <= next->time) {
			t = *(f32*)((u8*)anim + 0x10);
		}
		else if (i == count - 1) {
			t = next->time;
		}
		else {
			continue;
		}

		for (k = 0; k < 9; k++) {
			MapAnimChannel* ch = &key->chan[k];
			MapAnimChannel* chn = &next->chan[k];

			if (ch->flag != 0) {
				out[k] = ch->value;
			}
			else {
				f32 k0 = key->time;
				f32 k1 = next->time;
				f32 dt = t - k0;
				f32 d = k1 - k0;
				f32 tan1 = chn->unk4;
				f32 value1 = chn->value;
				f32 tan0 = ch->unk8;
				f32 value0 = ch->value;
				f32 den;
				s32 flag;
				f32 u, u2, u3, a, b, c, h10, h00, h01, h11;

				flag = (d != MATCH_F32(float_0_80419850));
				if (flag) {
					den = d;
				}
				else {
					den = float_1Eneg08_804198b8;
				}
				u = dt / den;
				u2 = u * u;
				u3 = u2 * u;
				a = float_3_8041984c * u2;
				b = u3 - float_2_80419858 * u2;
				c = float_2_80419858 * u3 - a;
				h10 = u + b;
				h00 = float_1_80419860 + c;
				h01 = float_neg2_804198a0 * u3 + a;
				h11 = u3 - u2;
				out[k] = tan0 * (d * h10)
				       + value0 * h00
				       + value1 * h01
				       + tan1 * (d * h11);
			}
		}

		f31 = out[0] - joint->base[0];
		f30 = out[1] - joint->base[1];
		f29 = out[2] - joint->base[2];
		f28 = out[3] - joint->base[3];
		f27 = out[4] - joint->base[4];
		f23 = out[5] - joint->base[5];
		f26 = out[6] / joint->div[0];
		f25 = out[7] / joint->div[1];
		f24 = out[8] / joint->div[2];

		if (f31 != float_0_80419850 || f30 != float_0_80419850 || f29 != float_0_80419850) {
			PSMTXTrans(trans, f31, f30, f29);
		}
		if (f28 != float_0_80419850) {
			PSMTXRotRad(rotX, 'x', float_deg2rad_80419868 * f28);
		}
		if (f27 != float_0_80419850) {
			PSMTXRotRad(rotY, 'y', float_deg2rad_80419868 * f27);
		}
		if (f23 != float_0_80419850) {
			PSMTXRotRad(rotZ, 'z', float_deg2rad_80419868 * f23);
		}
		PSMTXScale(scale, f26, f25, f24);
		PSMTXIdentity(local);
		if (f31 != float_0_80419850 || f30 != float_0_80419850 || f29 != float_0_80419850) {
			PSMTXConcat(local, trans, local);
		}
		if (f23 != float_0_80419850) {
			PSMTXConcat(local, rotZ, local);
		}
		if (f27 != float_0_80419850) {
			PSMTXConcat(local, rotY, local);
		}
		if (f28 != float_0_80419850) {
			PSMTXConcat(local, rotX, local);
		}
		if (f26 != float_0_80419850 || f25 != float_0_80419850 || f24 != float_0_80419850) {
			PSMTXConcat(local, scale, local);
		}
		PSMTXConcat(inMtx, local, outMtx);
		break;
	}
}

// NONMATCHING (96.54%): opening constant/pointer setup order; target reuses r28 for activeGroup=0, MWCC uses r0
void bmapLoad(const char* map1, const char* map2) {
	s32 offset;
	u8* p;
	MapEntry* entry;
	s32 i;
	MapWork* wp;

	sysWaitDrawSync();
	offset = 0;
	wp = mapWork;
	activeGroup = 0;
	p = (u8*)wp;
	for (i = 0; i < wp->entries[0].count; i++) {
		entry = (MapEntry*)((u8*)mapWork + activeGroup * 0x2F4 + offset);
		if (*(void**)((u8*)entry + 0x16C)) {
			_mapFree(*(void**)((u8*)entry + 0x16C));
		}
		*(s32*)((u8*)entry + 0x168) = 0;
		*(s32*)((u8*)entry + 0x16C) = 0;
		*(s32*)(p + 0x170) = 0;
		*(s32*)(p + 0x178) = 0;
		*(s32*)(p + 0x174) = 0;
		if (((MapEntry*)p)->dat) {
			if (!arcDataCheck(((MapEntry*)p)->dat)) {
				*(void**)(p + 0x170) = aramMgrToAram(((MapEntry*)p)->dat, ((MapEntry*)p)->datSize);
				*(void**)(p + 0x178) = aramMgrToAram(((MapEntry*)p)->tpl, ((MapEntry*)p)->tplSize);
				_mapFree(((MapEntry*)p)->tpl);
				_mapFree(((MapEntry*)p)->dat);
				((MapEntry*)p)->tpl = NULL;
				((MapEntry*)p)->dat = NULL;
			}
			else {
				*(void**)(p + 0x174) = aramMgrToAram(*(void**)(p + 0x8C), *(u32*)(p + 0x90));
				_mapFree(*(void**)(p + 0x8C));
				*(void**)(p + 0x8C) = NULL;
			}
		}
		offset += 0x178;
		p += 0x178;
	}
	activeGroup = 1;
	entry = (MapEntry*)((u8*)mapWork + 0x2F4);
	memset(entry, 0, 0x2F4);
	_mapLoad((MapWork*)entry, 0, map1);
	entry->count = 1;
	if (map2) {
		_mapLoad((MapWork*)entry, 1, map2);
		entry->count = 2;
	}
}

// NONMATCHING (95.45%): entry/i (r29/r30) register swap in the first two loops and one extra
// activeGroup reload; our build also inlines makeDisplayList, which the original calls.
void bmapUnLoad(void) {
	MapWork* wp;
	MapEntry* entry;
	s32 i;

	sysWaitDrawSync();
	activeGroup = 1;
	wp = &mapWork[1];
	entry = &wp->entries[0];
	for (i = 0; i < wp->entries[0].count; entry++, i++) {
		if (entry->objects) {
			_mapFree(entry->objects);
			entry->numJoints = 0;
			entry->objects = NULL;
		}
		if (entry->animData) {
			_mapFree(entry->animData);
			*(s32*)entry->field_0x160 = 0;
			entry->animData = NULL;
		}
		if (*(void**)((u8*)entry + 0x16C)) {
			_mapFree(*(void**)((u8*)entry + 0x16C));
			*(void**)((u8*)entry + 0x16C) = NULL;
			*(void**)((u8*)entry + 0x168) = NULL;
		}
	}
	unk_80418458 = 0;
	hitReInit();
	lightReInit();
	envReInit();
	entry = &wp->entries[0];
	for (i = 0; i < wp->entries[0].count; entry++, i++) {
		if (entry->dat) {
			if (!arcDataCheck(entry->dat)) {
				_mapFree(entry->dat);
			}
			entry->dat = NULL;
		}
		if (entry->tpl) {
			if (!arcDataCheck(entry->tpl)) {
				_mapFree(entry->tpl);
			}
			entry->tpl = NULL;
		}
		if (*(void**)entry->field_0x8C) {
			_mapFree(*(void**)entry->field_0x8C);
			*(void**)entry->field_0x8C = NULL;
		}
		strncpy(entry->name, float_0_804198cc, 0x10);
	}
	arcDelete(1);
	arcDelete(2);
	memset(&mapWork[1], 0, 0x2F4);

	activeGroup = 0;
	wp = mapWork;
	entry = &wp->entries[0];
	for (i = 0; i < wp->entries[0].count; entry++, i++) {
		if (*(void**)((u8*)entry + 0x170)) {
			entry->dat = _mapAllocTail(entry->datSize);
			entry->tpl = _mapAllocTail(entry->tplSize);
			aramMgrToMram(*(void**)((u8*)entry + 0x170), entry->dat);
			aramMgrToMram(*(void**)((u8*)entry + 0x178), entry->tpl);
			*(void**)((u8*)entry + 0x170) = NULL;
			*(void**)((u8*)entry + 0x178) = NULL;
		}
		else {
			*(void**)entry->field_0x8C = _mapAllocTail(*(u32*)(entry->field_0x8C + 4));
			aramMgrToMram(*(void**)(entry->field_0x168 + 0xC), *(void**)entry->field_0x8C);
			*(void**)(entry->field_0x168 + 0xC) = NULL;
		}
		makeDisplayList(i);
	}
	if (*(void**)wp->entries[0].field_0x8C) {
		arcEntry(1, *(void**)wp->entries[0].field_0x8C, *(u32*)(wp->entries[0].field_0x8C + 4));
	}
}

// NONMATCHING (42.11%): partial TEV setup; large body still differs
void mapSetMaterialTev(s32 texCount, s32 drawMode, s32 flags, Mtx mtx) {
	GXTevStageID tevStage;
	Mtx queen_mtx;
	
	if (texCount > 0) {
		switch (drawMode) {
			case 0:
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
				GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
				GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage = GX_TEVSTAGE1;
				break;

			case 1:
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
				GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_TEXC, GX_CC_TEXA, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
				GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
				GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage = GX_TEVSTAGE3;
				break;

			case 2:
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
				GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXA, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_TEXA, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
				GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage = GX_TEVSTAGE3;
				break;

			case 12:
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
				GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
				GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
				GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage = GX_TEVSTAGE3;
				break;

			case 3:
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
				GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_ZERO, GX_CC_TEXA, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_APREV, GX_CA_ZERO, GX_CA_TEXA, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
				GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage = GX_TEVSTAGE3;
				break;

			case 4:
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
				GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_TEXA, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_TEXC, GX_CC_CPREV, GX_CC_APREV, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
				GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage = GX_TEVSTAGE3;
				break;

			case 5:
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
				GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1);
				GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG1);
				GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
				GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
				GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD2, GX_TEXMAP2, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_C0, GX_CC_C1, GX_CC_TEXA, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_A0, GX_CA_A1, GX_CA_TEXA, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE3, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
				GXSetTevColorOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE3, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE3, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE3, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE3, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage = GX_TEVSTAGE4;
				break;

			case 6:
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
				GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_TEXC, GX_CC_APREV, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_TEXA, GX_CA_APREV, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
				GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage = GX_TEVSTAGE3;
				break;

			case 7:
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
				GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_TEXC, GX_CC_TEXA, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_APREV, GX_CA_ZERO, GX_CA_TEXA, GX_CA_TEXA);
				GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
				GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage = GX_TEVSTAGE3;
				break;

			case 8:
				GXSetTevColor(GX_TEVREG0, (GXColor) {
					0, (u8)((127.0f * sinf((TWO_PI * (f32)(2 * gp->retraceCount)) / 360.0f)) + 127.0f),
					0, 0}); //TODO: check
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_C0);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_A0);
				GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage = GX_TEVSTAGE1;
				break;

			case 9:
				scrl_uv[0] += queen_uv[0];
				if (scrl_uv[0] > 100.0f) {
					scrl_uv[0] -= 100.0f;
				}
				if (scrl_uv[0] < -100.0f) {
					scrl_uv[0] += 100.0f;
				}

				scrl_uv[1] += queen_uv[1];
				if (scrl_uv[1] > 100.0f) {
					scrl_uv[1] -= 100.0f;
				}
				if (scrl_uv[1] < -100.0f) {
					scrl_uv[1] += 100.0f;
				}

				scrl_uv[2] += queen_uv[2];
				if (scrl_uv[2] > 100.0f) {
					scrl_uv[2] -= 100.0f;
				}
				if (scrl_uv[2] < -100.0f) {
					scrl_uv[2] += 100.0f;
				}

				scrl_uv[3] += queen_uv[3];
				if (scrl_uv[3] > 100.0f) {
					scrl_uv[3] -= 100.0f;
				}
				if (scrl_uv[3] < -100.0f) {
					scrl_uv[3] += 100.0f;
				}

				MTXTrans(queen_mtx, scrl_uv[0], scrl_uv[1], 0.0f);
				GXLoadTexMtxImm(queen_mtx, 0x1E, GX_MTX2x4);
				GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
				MTXTrans(queen_mtx, scrl_uv[2], scrl_uv[3], 0.0f);
				GXLoadTexMtxImm(queen_mtx, 0x21, GX_MTX2x4);
				GXSetTexCoordGen2(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_TEX1, GX_TEXMTX1, GX_FALSE, GX_PTIDENTITY);
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
				GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP1, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_TEXC, GX_CC_TEXA, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_APREV, GX_CA_ZERO, GX_CA_TEXA, GX_CA_TEXA);
				GXSetTevSwapMode(GX_TEVSTAGE1, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevOrder(GX_TEVSTAGE2, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
				GXSetTevColorOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE2, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE2, GX_CC_ZERO, GX_CC_CPREV, GX_CC_RASC, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE2, GX_CA_ZERO, GX_CA_APREV, GX_CA_RASA, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE2, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage = GX_TEVSTAGE3;
				break;

			default:
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_ZERO);
				GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage = GX_TEVSTAGE1;
				break;
		}
	}
	else {
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
		GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
		GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
		GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
		tevStage = GX_TEVSTAGE1;
	}

	if (!(flags & 0x10000000)) {
		GXSetTevKColorSel(tevStage, GX_TEV_KCSEL_K0);
		GXSetTevKAlphaSel(tevStage, GX_TEV_KASEL_K0_A);
		GXSetTevOrder(tevStage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
		GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_CPREV, GX_CC_KONST, GX_CC_ZERO);
		GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_APREV, GX_CA_KONST, GX_CA_ZERO);
		GXSetTevSwapMode(tevStage++, GX_TEV_SWAP0, GX_TEV_SWAP0);
	}

}






// NONMATCHING (0.07%): only a stub is present
void mapMain(void) {

}

// NONMATCHING (0.19%): only a stub is present
void mapDisp(void) {

}

MapObject* mapGetMapObj(const char *name) { // 1:1
    MapWork *wp;
    MapEntry* entry;
    int j;
    int i;
    MapObject *object;
    
    wp = &mapWork[activeGroup];
    if (!name) {
        return 0;
    }
    entry = &wp->entries[0];
    for (i = 0; i < wp->entries[0].count; entry++, i++) {
        object = entry->objects;
        for (j = 0; j < entry->numJoints; j++, object++) {
            if (!strcmp(object->joints->name, name)) {
                return object;
            }
        }
    }
    return 0;
}





// NONMATCHING (0.28%): only a stub is present
void mapSetMaterialLight(s32 flags, Vec position) {

}

// NONMATCHING (0.50%): only a stub is present
void mapSetMaterialLastStageBlend(s32 flags, GXColor color1, GXColor color2) {

}

// NONMATCHING (83.18%): logic/structure and call sequence match; MWCC register coloring differs (target
// folds the entries+0x2C base into the displacement and keeps separate sx/sy copies) and float/
// double pool constants use local @nnnn entries. Remaining: addressing/register allocation.
void mapSetTextureMatrix(MapMaterialTex* mat) {
	MapMatTexMtx* entry = &mat->entries[mat->field_0xB - 1];
	s32* mtxTbl = &texmtx_tbl[mat->field_0xB - 1];
	s32 count = mat->field_0xB;
	s32 i = count - 1;
	s32 tc = 0;
	s32 j = 0;
	Mtx mtx, mtx2, mtx3, mtx4, mtx5;

	for (; i >= 0; i--, entry--, mtxTbl--, tc++, j++) {
		f32 f5 = entry->field_0x8;
		f32 f1 = entry->field_0x10;
		f32 f6 = entry->field_0xC;
		f32 sx = f5;
		f32 f0 = entry->field_0x4;
		f32 sy = f6;
		f32 tx = entry->field_0x0;
		f32 rots = -f1;
		f32 ty = -f0;

		if (sx == 1.0f && sy == 1.0f && tx == 0.0f && ty == 0.0f && rots == 0.0f) {
			GXSetTexCoordGen2(tc, 1, count + 3 - j, GX_IDENTITY, 0, 0x7D);
		}
		else {
			if (rots != 0.0f) {
				f32 f25 = 0.5f * entry->field_0x14 * sx;
				f32 f24 = (1.0f - 0.5f * entry->field_0x18) * sy;
				PSMTXTrans(mtx, f25, f24, 0.0f);
				PSMTXRotRad(mtx2, 'z', float_deg2rad_80419868 * rots);
				PSMTXTrans(mtx3, -f25, -f24, 0.0f);
				PSMTXConcat(mtx, mtx2, mtx2);
				PSMTXConcat(mtx2, mtx3, mtx2);
			}
			if (sx != 0.0f || sy != 0.0f) {
				PSMTXScale(mtx4, sx, sy, 1.0f);
			}
			if (tx != 0.0f || ty != 0.0f) {
				PSMTXTrans(mtx5, tx, ty, 0.0f);
			}
			if (rots != 0.0f) {
				if (sx != 0.0f || sy != 0.0f) {
					PSMTXConcat(mtx2, mtx4, mtx);
					if (tx != 0.0f || ty != 0.0f) {
						PSMTXConcat(mtx, mtx5, mtx);
					}
				}
				else {
					if (tx != 0.0f || ty != 0.0f) {
						PSMTXConcat(mtx2, mtx5, mtx);
					}
					else {
						PSMTXCopy(mtx2, mtx);
					}
				}
			}
			else {
				if (sx != 0.0f || sy != 0.0f) {
					if (tx != 0.0f || ty != 0.0f) {
						PSMTXConcat(mtx4, mtx5, mtx);
					}
					else {
						PSMTXCopy(mtx4, mtx);
					}
				}
				else {
					if (tx != 0.0f || ty != 0.0f) {
						PSMTXCopy(mtx5, mtx);
					}
				}
			}
			GXLoadTexMtxImm(mtx, *mtxTbl, 1);
			GXSetTexCoordGen2(tc, 1, count + 3 - j, *mtxTbl, 0, 0x7D);
		}
	}
}

void mapSetMaterialFog(void) {
	CameraEntry* cam = camGetCurPtr();
	MapEntry* entry = &mapWork[activeGroup].entries[0];

	if (entry->flags & 0x1) {
		GXColor color = *(GXColor*)((u8*)entry + 0x34);
		GXSetFog(fog_type[*(s32*)((u8*)entry + 0x28)], *(f32*)((u8*)entry + 0x2C), *(f32*)((u8*)entry + 0x30), *(f32*)((u8*)cam + 0x30), *(f32*)((u8*)cam + 0x34), color);
	}
	else {
		GXSetFog(GX_FOG_NONE, float_0_80419850, float_0_80419850, float_0_80419850, float_0_80419850, dat_80419820);
	}
}

// NONMATCHING (85.28%): GX setup order/constants all match; only MWCC register coloring differs
// (target keeps mesh in r27, obj in r26 and the two array pointers in r29/r30).
void mapSetPolygonVtxDesc(MapObject* obj, MapFileMesh* mesh) {
	MapWork* wp = &mapWork[activeGroup];
	u32* vcd = (u32*)mesh->vcdTableOffset;
	u32 mask = mesh->elementMask;
	void* pos = (void*)(vcd[0] + 4);
	void* nrm = (void*)(vcd[1] + 4);
	s32 i;
	u8* material;

	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
	material = *(u8**)(wp->entries[*(s16*)(obj->field_0xDC + 2)].field_0x8C + 0x18);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, (u8)*(u32*)(material + 0x44));
	GXSetArray(GX_VA_POS, pos, 6);
	GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
	GXSetArray(GX_VA_NRM, nrm, 3);

	for (i = 0; i < 2; i++) {
		if (!(mask & (1 << (i + 2)))) {
			break;
		}
		GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
		GXSetArray(GX_VA_CLR0, (void*)(vcd[3 + i] + 4), 4);
	}

	for (i = 0; i < 8; i++) {
		if (!(mask & (1 << (i + 4)))) {
			break;
		}
		GXSetVtxDesc(GX_VA_TEX0 + i, GX_INDEX16);
		material = *(u8**)(wp->entries[*(s16*)(obj->field_0xDC + 2)].field_0x8C + 0x18);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0 + i, GX_TEX_ST, GX_S16, (u8)*(u32*)(material + 0x48 + i * 4));
		GXSetArray(GX_VA_TEX0 + i, (void*)(vcd[6 + i] + 4), 4);
	}
}

// NONMATCHING (99.21%): obj/o (r30/r31) register swap only
void _mapDispMapGrp(MapObject* obj, void* a2) {
	_mapDispMapObj((s32)obj, (MapObject*)a2);
	if (((MapObject*)a2)->child) _mapDispMapGrp(obj, ((MapObject*)a2)->child);
	if ((*(u32*)((MapObject*)a2)->field_0x0 & 0x20) && ((MapObject*)a2)->next) _mapDispMapGrp(obj, ((MapObject*)a2)->next);
}

// NONMATCHING (98.00%): inlined _mapDispMapObj_NoMaterial keeps an extra camId copy and one extra callee-saved reg
void _mapDispMapGrp_NoMaterial(MapObject* obj, void* a2) {
	MapObject* o = (MapObject*)a2;

	_mapDispMapObj_NoMaterial((s32)obj, o);
	if (o->child) _mapDispMapGrp_NoMaterial(obj, o->child);
	if ((*(u32*)o->field_0x0 & 0x20) && o->next) _mapDispMapGrp_NoMaterial(obj, o->next);
}

void mapFlgOff(MapObject* obj, u32 flag, s32 a3) {
	*(u32*)obj->field_0x0 &= ~flag;
	if (obj->child) mapFlgOff(obj->child, flag, 1);
	if (a3 && obj->next) mapFlgOff(obj->next, flag, 1);
}

void mapFlgOn(MapObject* obj, u32 flag, s32 a3) {
	if (!(flag & 0x100) || *(void**)((u8*)obj + 0x114)) {
		*(u32*)obj->field_0x0 |= flag;
	}
	if (obj->child) mapFlgOn(obj->child, flag, 1);
	if (a3 && obj->next) mapFlgOn(obj->next, flag, 1);
}

void _setOffScrnId(MapObject* obj, s32 id, s32 a3) {
	*(s16*)obj->field_0xDC = id;
	if (id != -1) {
		*(u32*)obj->field_0x0 |= 0x80;
	}
	else {
		*(u32*)obj->field_0x0 &= ~0x80;
	}
	if (obj->child) _setOffScrnId(obj->child, id, 1);
	if (a3 && obj->next) _setOffScrnId(obj->next, id, 1);
}

// NONMATCHING (75.99%): local GXColor copy changes the frame/register layout vs target keeping the color pointer live
void setColor(MapObject* obj, GXColor* color, s32 a3) {
	GXColor c;
	if (obj == 0) return;
	c = *color;
	*(u32*)obj->field_0xC = *(u32*)&c;
	*(u32*)obj->field_0x0 |= 0x40;
	if (obj->child) setColor(obj->child, &c, 1);
	if (a3 && obj->next) setColor(obj->next, &c, 1);
}

// NONMATCHING (48.38%): MWCC inlines one fewer recursion level than the target; all register colors differ
void* mapSearchDmdJointSub(void* a, void* b) {
	MapFileJoint* joint = (MapFileJoint*)a;

	if (!strcmp(joint->name, (const char*)b)) {
		return joint;
	}
	if (joint->child) {
		MapFileJoint* r = mapSearchDmdJointSub(joint->child, b);
		if (r) return r;
	}
	if (joint->next) {
		MapFileJoint* r = mapSearchDmdJointSub(joint->next, b);
		if (r) return r;
	}
	return NULL;
}

#ifdef MUST_MATCH
// The original passes the caller's own Vec where the by-value argument would take a copy.
#define LIGHT_GET_NEAR(p, e, n, f) ((s32 (*)(Vec*, LightEntry**, s32, LightEntryFlags))lightGetNearObj)((p), (e), (n), (f))
#else
#define LIGHT_GET_NEAR(p, e, n, f) lightGetNearObj(*(p), (e), (n), (f))
#endif

// NONMATCHING (99.65%): branch-2 loop only: `e` and the compiler-generated i*4 induction
// accumulate in r30/r31 swapped vs the target. Both pointer walks are otherwise identical.
s32 mapSetLight(MapObject* obj, LightEntry** entries) {
	Vec pos;
	s32 count = 0;
	s32 i;
	LightEntry** e;

	PSMTXMultVec(obj->modelWorldMtx, (Vec*)(obj->field_0xC + 4), &pos);

	if (obj->joints->drawMode->unk2[0] & 0x8) {
		if (*(u32*)(obj->joints->drawMode->unk2 + 2) == 0) {
			count = LIGHT_GET_NEAR(&pos, entries, 8, 0);
		}
		else {
			e = entries;

			for (i = 0; i < 0x20; i++) {
				if (*(u32*)(obj->joints->drawMode->unk2 + 2) & (1 << i)) {
					MapEntry* entry = &mapWork[activeGroup].entries[*(s16*)(obj->field_0xDC + 2)];
					void* list = *(void**)(entry->field_0x8C + 0x0C);
					LightEntry* l = lightNameToPtr(*(const char**)*(void**)((u8*)list + i * 4 + 4));

					if (!(l->flags & LIGHT_AMBIENT)) {
						count++;
						*e = l;
						e++;
						if (count >= 8) break;
					}
				}
			}
			if (count < 8) count += LIGHT_GET_NEAR(&pos, entries + count, 8 - count, 0);
		}
	}
	else {
		if (*(u32*)(obj->joints->drawMode->unk2 + 2) != 0) {
			e = entries;

			for (i = 0; i < 0x20; i++) {
				if (*(u32*)(obj->joints->drawMode->unk2 + 2) & (1 << i)) {
					MapEntry* entry = &mapWork[activeGroup].entries[*(s16*)(obj->field_0xDC + 2)];
					void* list = *(void**)(entry->field_0x8C + 0x0C);
					LightEntry* l = lightNameToPtr(*(const char**)*(void**)((u8*)list + i * 4 + 4));

					if (!(l->flags & LIGHT_AMBIENT)) {
						count++;
						*e = l;
						e++;
						if (count >= 8) break;
					}
				}
			}
		}
		else {
			count = 0;
		}
	}
	return count;
}

void mapObjGetPosSub(MapObject* obj, Vec* pos, s32* count, void* a4) {
	if (obj->child) {
		mapObjGetPosSub(obj->child, pos, count, (void*)1);
	}
	else {
		Vec v;

		PSMTXMultVec(obj->modelWorldMtx, (Vec*)(obj->field_0xC + 4), &v);
		PSVECAdd(pos, &v, pos);
		(*count)++;
	}
	if ((s32)a4 && obj->next) {
		mapObjGetPosSub(obj->next, pos, count, (void*)1);
	}
}

// NONMATCHING (98.53%): a/c callee-saved swap (target a->r29, c->r30; mine reversed) plus an
// extra `mr r0,r3` in the prologue. All strcpy/strcat/strcmp/recursion instructions match.
void* mapSearchDmdJointSub2(void* a, void* b, const char* c) {
	MapFileJoint* joint = (MapFileJoint*)a;
	const char* name = (const char*)b;
	char buf[256];
	void* result;

	strcpy(buf, c);
	strcat(buf, joint->name);
	if (!strcmp(buf, name)) return joint;

	if (joint->child) {
		strcat(buf, str_8041986C);
		result = mapSearchDmdJointSub2(joint->child, b, buf);
		if (result) return result;
	}
	if (joint->next) {
		result = mapSearchDmdJointSub2(joint->next, b, c);
		if (result) return result;
	}
	return NULL;
}
