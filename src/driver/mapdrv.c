#include "driver/mapdrv.h"
#include "driver/arcdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
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
#include <dolphin/os.h>
#include "driver/shadowdrv.h"

extern s32 texmtx_tbl[];

extern f32 queen_uv[4]; //gxsub.c
extern GlobalWork* gp;

#ifdef MUST_MATCH
// The original passes the caller's own Vec where the by-value argument would take a copy.
#define LIGHT_GET_NEAR(p, e, n, f) ((s32 (*)(Vec*, LightEntry**, s32, LightEntryFlags))lightGetNearObj)((p), (e), (n), (f))
#else
#define LIGHT_GET_NEAR(p, e, n, f) lightGetNearObj(*(p), (e), (n), (f))
#endif

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
extern GXColor dat_80419824;
extern GXColor dat_80419828;
extern GXColor dat_80419830;
extern GXColor dat_80419840;
extern GXColor dat_80419844;
extern GXColor dat_80419820;
extern GXColor unk_80422f8c;
extern GXColor unk_80422f90;
extern GXColor unk_80422f84;
extern GXColor unk_80422f88;
const f32 float_0_80419850 = 0.0f;
extern f32 float_0p5_80419854;
extern f32 float_1000_80419874;
extern f32 float_10_80419864;
extern f32 float_deg2rad_80419868;
const f32 float_1_80419860 = 1.0f;
extern f32 float_2_80419858;
extern f32 float_3_8041984c;
extern f32 float_neg2_804198a0;
extern f32 float_neg1_804198a8;
extern f32 float_5000_804198ac;
extern f32 float_10000_804198b0;
extern f32 float_20000_804198d8;
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

extern f32 float_neg1p0486E06_80419870;
extern f32 float_0p01_804198b4;

// material rodata blob: the original anchors this whole .rodata region at vec3_802c16d8
// and reaches the other tables below with constant offsets (0xEC texwrap_tbl, 0x100 lightid_tbl,
// 0x120..0x150 direction vectors, 0x2C8 int->double mask).
extern u8 vec3_802c16d8[];

// the TEV-stage callback slots at mapWork[activeGroup]+0x110 (16 entries)

// GXColor blobs the shadow TEV sets: the original is not linked while this function is
extern GXColor dat_80419800;
extern GXColor dat_80419804;
extern GXColor dat_80419808;
extern GXColor dat_8041980c;
extern GXColor dat_80419810;
extern GXColor dat_80419814;
extern GXColor dat_80419818;
extern u8 texture_0[];

#ifdef MUST_MATCH
// The original reaches texmtx_tbl through the shared .rodata base r28 = vec3_802c16d8 + 0x98.
#define TEXMTX(i) (((s32*)(void*)(base + 0x98))[i])
#else
#define TEXMTX(i) texmtx_tbl[i]
#endif

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

void mapGrpSetOffScreen(const char* name, const char* offName) {
	MapObject* obj = mapGetMapObjSub(name);
	s32 id = offscreenNameToId(offName);

	_setOffScrnId(obj, id, 0);
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

#ifdef MUST_MATCH
// Inline copy of mapSearchAnmObj with `entry` declared before `wp`: the inliner allocates the helper's
// locals in declaration order, and that order puts wp in the higher register (r28) as in the target.
// Natural form: `found = mapSearchAnmObj(name);`.
static inline MapEntryAnimData* mapFindAnm(const char* animName) {
	MapEntry* entry;
	MapWork* wp;
	s32 j;
	s32 i;

	wp = &mapWork[activeGroup];
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
#endif

void mapPauseAnimation(const char* name) {
#ifdef MUST_MATCH
	MapEntryAnimData* found = mapFindAnm(name);
#else
	MapEntryAnimData* found = mapSearchAnmObj(name);
#endif

	if (found) {
		*(u16*)found |= 0x10;
	}
}

// Matches via mapFindAnm: the inline copy's `entry`-before-`wp` declaration order puts wp in r28
// (the higher register) in the target; inlining mapSearchAnmObj directly colors them the other way.
void mapReStartAnimation(const char* name) {
#ifdef MUST_MATCH
	MapEntryAnimData* found = mapFindAnm(name);
#else
	MapEntryAnimData* found = mapSearchAnmObj(name);
#endif

	if (found) {
		*(u16*)found |= 0x20;
	}
}

void mapSetPlayRate(const char* name, f32 rate) {
#ifdef MUST_MATCH
	MapEntryAnimData* found = mapFindAnm(name);
#else
	MapEntryAnimData* found = mapSearchAnmObj(name);
#endif

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

void mapDispMapGrp(CameraId camId, MapObject* obj) {
	_mapDispMapGrp(camId, obj);
}

void mapDispMapObj(CameraId camId, MapObject* obj) {
	_mapDispMapObj(camId, obj);
}

void mapDispMapGrp_off(CameraId camId, MapObject* obj) {
	u16 v0;
	u16 v1;
	u16 v2;
	u16 v3;

	sysWaitDrawSync();
	GXClearBoundingBox();
	_mapDispMapGrp(camId, obj);
	sysWaitDrawSync();
	GXReadBoundingBox(&v0, &v1, &v2, &v3);
	offscreenAddBoundingBox(*(s16*)((u8*)obj + 0xDC), v0, v1, v2, v3);
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

extern int __float_nan[];
#define SPLINE_NAN (*(f32*)__float_nan)

static inline s32 spline_fpclassifyf(f32 x) {
	switch (*(s32*)&x & 0x7F800000) {
		case 0x7F800000:
			if (*(s32*)&x & 0x007FFFFF) {
				return FP_NAN;
			} else {
				return FP_INFINITE;
			}
			break;
		case 0:
			if (*(s32*)&x & 0x007FFFFF) {
				return FP_SUBNORMAL;
			} else {
				return FP_ZERO;
			}
			break;
		default:
			return FP_NORMAL;
	}
}

static inline f32 spline_sqrtf(f32 x, const f64* c) {
	if (x > float_0_80419850) {
		f64 guess = __frsqrte(x);
		f64 half = c[0];
		f64 three = c[1];

		guess = half * guess * (three - guess * guess * x);
		guess = half * guess * (three - guess * guess * x);
		guess = half * guess * (three - guess * guess * x);
		return x * guess;
	} else if (x < c[2]) {
		return SPLINE_NAN;
	} else if (spline_fpclassifyf(x) == FP_NAN) {
		return SPLINE_NAN;
	}
	return x;
}

// NONMATCHING (90.75%): only the tangents[count-2] block differs, in register/scheduling details: ours
// reads both tangents through the one tg+count base kept live from the init, while the target reads
// tangents[count-1] through the init's base and [count-2] through a recomputed one. Everything else matches.
void spline_maketable(s32 count, s32 a, s32* b, s32* c) {
#ifdef MUST_MATCH
	// The target keeps the incoming pointers in r27/r28/r29 in argument order. Naming the parameters
	// through macros (instead of copying them to locals) is what reproduces that colouring; with locals
	// MWCC colours tangents/dt first and shifts every callee-saved GPR one register low.
#define points ((Vec*)a)
#define knots ((f32*)b)
#define tangents ((Vec*)c)
	Vec* tg = (Vec*)c; // separate base for the two count-1/count-2 blocks, so they stay base+offset
	Vec* tend; // match-only: kept live from the tangents init to the count-2 block, like the target's r9
#else
	Vec* points = (Vec*)a;
	f32* knots = (f32*)b;
	Vec* tangents = (Vec*)c;
	Vec* tg = tangents;
#endif
	const f64* cf = (const f64*)(vec3_802c16d8 + 0x2A8);
	f32* dt = (f32*)_mapAlloc(count * 4);
	Vec* mat = (Vec*)_mapAlloc(count * 0xC);
	s32 i;

	knots[0] = 0.0f;
	for (i = 1; i < count; i++) {
		f32 dx = points[i].x - points[i - 1].x;
		f32 dy = points[i].y - points[i - 1].y;
		f32 dz = points[i].z - points[i - 1].z;

		f32 sq = dx * dx + dy * dy + dz * dz;
#ifdef MUST_MATCH
		// match-only: writing the Newton result back into sq reuses x's FPR (the target overwrites f5);
		// calling spline_sqrtf forces the f32 return register f1 and leaves an extra `fmr f1,f5`
		if (sq > float_0_80419850) {
			f64 guess = __frsqrte(sq);
			f64 half = cf[0];
			f64 three = cf[1];
			guess = half * guess * (three - guess * guess * sq);
			guess = half * guess * (three - guess * guess * sq);
			guess = half * guess * (three - guess * guess * sq);
			sq = sq * guess;
		} else if (sq < cf[2]) {
			sq = SPLINE_NAN;
		} else if (spline_fpclassifyf(sq) == FP_NAN) {
			sq = SPLINE_NAN;
		}
#else
		sq = spline_sqrtf(sq, cf);
#endif
		knots[i] = knots[i - 1] + sq;
	}

	for (i = 1; i < count; i++) {
		knots[i] = knots[i] / knots[count - 1];
	}

	tg[0].x = 0.0f;
	tg[0].y = 0.0f;
	tg[0].z = 0.0f;
	tg[count - 1].x = 0.0f;
	tg[count - 1].y = 0.0f;
	tg[count - 1].z = 0.0f;
#ifdef MUST_MATCH
	tend = tg + count; // evaluated here (not in the block) so MWCC keeps count*12 in a register across the loops
#endif
	for (i = 0; i < count - 1; i++) {
		dt[i] = knots[i + 1] - knots[i];
		if (dt[i] == 0.0f) {
			dt[i] = float_0p0001_8041985c;
		}
		mat[i + 1].x = (points[i + 1].x - points[i].x) / dt[i];
		mat[i + 1].y = (points[i + 1].y - points[i].y) / dt[i];
		mat[i + 1].z = (points[i + 1].z - points[i].z) / dt[i];
	}

	tangents[1].x = mat[2].x - mat[1].x;
	tangents[1].y = mat[2].y - mat[1].y;
	tangents[1].z = mat[2].z - mat[1].z;
	mat[1].x = 2.0f * (knots[2] - knots[0]);
	mat[1].y = 2.0f * (knots[2] - knots[0]);
	mat[1].z = 2.0f * (knots[2] - knots[0]);

	for (i = 1; i < count - 2; i++) {
		f32 wx = dt[i] / mat[i].x;
		f32 wy = dt[i] / mat[i].y;
		f32 wz = dt[i] / mat[i].z;

		tangents[i + 1].x = (mat[i + 2].x - mat[i + 1].x) - wx * tangents[i].x;
		tangents[i + 1].y = (mat[i + 2].y - mat[i + 1].y) - wy * tangents[i].y;
		tangents[i + 1].z = (mat[i + 2].z - mat[i + 1].z) - wz * tangents[i].z;
		mat[i + 1].x = 2.0f * (knots[i + 2] - knots[i]) - wx * dt[i];
		mat[i + 1].y = 2.0f * (knots[i + 2] - knots[i]) - wy * dt[i];
		mat[i + 1].z = 2.0f * (knots[i + 2] - knots[i]) - wz * dt[i];
	}

#ifdef MUST_MATCH
	// match-only: the target reads tangents[count-1] through a tg+count base and dt[count-2] through
	// dt+(count-2) (negative offsets), so name those bases instead of indexing both through tg
	{
		f32* dprev = dt + (count - 2);
		tend[-2].x = tend[-2].x - dprev[0] * tend[-1].x;
		tend[-2].y = tend[-2].y - dprev[0] * tend[-1].y;
		tend[-2].z = tend[-2].z - dprev[0] * tend[-1].z;
	}
#else
	tg[count - 2].x = tg[count - 2].x - dt[count - 2] * tg[count - 1].x;
	tg[count - 2].y = tg[count - 2].y - dt[count - 2] * tg[count - 1].y;
	tg[count - 2].z = tg[count - 2].z - dt[count - 2] * tg[count - 1].z;
#endif

	for (i = count - 2; i > 0; i--) {
		tangents[i].x = (tangents[i].x - dt[i] * tangents[i + 1].x) / mat[i].x;
		tangents[i].y = (tangents[i].y - dt[i] * tangents[i + 1].y) / mat[i].y;
		tangents[i].z = (tangents[i].z - dt[i] * tangents[i + 1].z) / mat[i].z;
	}

	_mapFree(mat);
	_mapFree(dt);
#ifdef MUST_MATCH
#undef points
#undef knots
#undef tangents
#endif
}

// NONMATCHING (94.57%): arithmetic and control flow match instruction for instruction; only MWCC's FP
// colouring differs now (knot[lo] loads into f2/f3, t-knot[lo] into f8, 2.0 into f0 vs the target's
// f2/f3 for constants, f1 for t-knot[lo], f6 for 2.0). Uses the extern float_2/float_3 constants.
void spline_getvalue(f32* out, s32 type, s32 a, s32 b, s32 c, f32 t) {
	f32* points = (f32*)a;
	f32* knots = (f32*)b;
	f32* tangents = (f32*)c;
	s32 lo;
	s32 hi;
	s32 mid;
	f32 f2, f3, f4, f0, f6, f5, f8, f7;

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
	f6 = float_2_80419858;
	f7 = tangents[lo * 3 + 0];
	f4 = tangents[lo * 3 + 3];
	t = t - f2;
	f0 = knots[lo + 1];
	f5 = f4 - f7;
	f8 = points[lo * 3 + 0];
	f3 = points[lo * 3 + 3];
	f2 = f0 - f2;
	f4 = f6 * f7 + f4;
	f0 = float_3_8041984c;
	f5 = t * f5;
	f3 = f3 - f8;
	f5 = f5 / f2;
	f3 = f3 / f2;
	f5 = f0 * f7 + f5;
	f3 = f3 - f2 * f4;
	f3 = t * f5 + f3;
	f3 = t * f3 + f8;
	out[0] = f3;

	f7 = tangents[lo * 3 + 1];
	f4 = tangents[lo * 3 + 4];
	f5 = f4 - f7;
	f8 = points[lo * 3 + 1];
	f3 = points[lo * 3 + 4];
	f4 = f6 * f7 + f4;
	f5 = t * f5;
	f3 = f3 - f8;
	f5 = f5 / f2;
	f3 = f3 / f2;
	f5 = f0 * f7 + f5;
	f3 = f3 - f2 * f4;
	f3 = t * f5 + f3;
	f3 = t * f3 + f8;
	out[1] = f3;

	f7 = tangents[lo * 3 + 2];
	f4 = tangents[lo * 3 + 5];
	f5 = f4 - f7;
	f8 = points[lo * 3 + 2];
	f3 = points[lo * 3 + 5];
	f4 = f6 * f7 + f4;
	f5 = t * f5;
	f3 = f3 - f8;
	f5 = f5 / f2;
	f3 = f3 / f2;
	f5 = f0 * f7 + f5;
	f3 = f3 - f2 * f4;
	f3 = t * f5 + f3;
	f3 = t * f3 + f8;
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

// MATCH. The original signature is by pointer: the caller passes two stack addresses in r3/r4 and the target
// writes through them word-by-word with no local buffer. A by-value Vec parameter makes MWCC keep the struct
// in registers and buffer the copy (90% match); `Vec*` plus this declaration order (wp first) gives 100%.
void mapGetBoundingBox(Vec* min, Vec* max) {
	MapWork* wp;
	MapObject* obj;
	MapFileJoint* joint;
	Vec vec;
	int i;

	wp = &mapWork[activeGroup];
	*min = *(Vec*)vec3_802c16d8;
	*max = *(Vec*)vec3_802c16e4;
	obj = wp->entries[0].objects;
	for (i = 0; i < wp->entries[0].numJoints; i++, obj++) {
		joint = obj->joints;
		if (joint && joint->partCount) {
			MTXMultVec(obj->modelWorldMtx, &joint->bboxMin, &vec);
			if (min->x > vec.x) {
				min->x = vec.x;
			}
			if (min->y > vec.y) {
				min->y = vec.y;
			}
			if (min->z > vec.z) {
				min->z = vec.z;
			}
			if (max->x < vec.x) {
				max->x = vec.x;
			}
			if (max->y < vec.y) {
				max->y = vec.y;
			}
			if (max->z < vec.z) {
				max->z = vec.z;
			}
			MTXMultVec(obj->modelWorldMtx, &obj->joints->bboxMax, &vec);
			if (min->x > vec.x) {
				min->x = vec.x;
			}
			if (min->y > vec.y) {
				min->y = vec.y;
			}
			if (min->z > vec.z) {
				min->z = vec.z;
			}
			if (max->x < vec.x) {
				max->x = vec.x;
			}
			if (max->y < vec.y) {
				max->y = vec.y;
			}
			if (max->z < vec.z) {
				max->z = vec.z;
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

// NONMATCHING (99.17%): the found/not-found split (goto found, idx = -1 on fall-through, then
// use = idx; if (use == -1) use = 0) matches instruction for instruction; only the strcmp test's
// branch direction differs (target bne increment + b found, ours beq found with increment falling through).
void mapBuildTexture(MapObject* obj, TEXPalette* texture, void* table) {
	int i;
	int j;

	for (i = 0; i < obj->joints->partCount; i++) {
		u8* material = (u8*)obj->joints->parts[i].material;

		for (j = 0; j < *(u8*)(material + 0xB); j++) {
			u8** ref = *(u8***)(material + 0xC + j * 4);
			s32 idx;
			s32 use;
			const char* name = *(const char**)(*ref);
			TEXDescriptor* desc;
			const char** p;

			idx = 0;
			p = (const char**)table;
			while (idx < *(s32*)table) {
				if (strcmp(p[1], name) == 0) {
					goto found;
				}
				p++;
				idx++;
			}
			idx = -1;
		found:
			use = idx;
			if (use == -1) {
				use = 0;
			}
			desc = TEXGet(texture, use);
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

MapObject* _mapEnt(MapFileJoint* joint, MapObject* parent, Mtx mtx, s32 index) {
	MapWork* wp;
	MapObject* obj;
	Mtx transMtx;
	Mtx scaleMtx;
	Mtx rotMtxX;
	Mtx rotMtxY;
	Mtx rotMtxZ;
	Vec vecA;
	Vec vecB;
	void* ref;
	s32 mode;
	s32 i;
	s32 j;
	s32 count2;
	s32 count1;

	wp = &mapWork[activeGroup];
	obj = wp->entries[index].objects;
	for (i = 0; i < wp->entries[index].numJoints; i++) {
		if (obj->joints == NULL) break;
		obj++;
	}

	*(u32*)obj->field_0x0 = 0;
	obj->joints = joint;
	*(s16*)(obj->field_0xDC + 2) = index;

	PSMTXTrans(transMtx, joint->translation.x, joint->translation.y, joint->translation.z);
	PSMTXScale(scaleMtx, joint->scale.x, joint->scale.y, joint->scale.z);
	PSMTXRotRad(rotMtxX, 'x', float_deg2rad_80419868 * joint->rotation.x);
	PSMTXRotRad(rotMtxY, 'y', float_deg2rad_80419868 * joint->rotation.y);
	PSMTXRotRad(rotMtxZ, 'z', float_deg2rad_80419868 * joint->rotation.z);
	PSMTXConcat(transMtx, rotMtxZ, transMtx);
	PSMTXConcat(transMtx, rotMtxY, transMtx);
	PSMTXConcat(transMtx, rotMtxX, transMtx);
	PSMTXConcat(transMtx, scaleMtx, (f32(*)[4])obj->field_0x4C);
	PSMTXConcat(mtx, (f32(*)[4])obj->field_0x4C, obj->modelWorldMtx);
	PSMTXIdentity(obj->unkAC);

	*(u32*)obj->field_0x0 |= 0x200;
	*(u8*)(obj->field_0x0 + 5) = 4;
	*(s16*)(obj->field_0xDC) = -1;
	obj->parent = parent;
	obj->next = NULL;
	obj->child = NULL;

	if (joint->drawMode != NULL) {
	if (joint->drawMode->unk2[0] & 1) {
		*(u32*)obj->field_0x0 |= 0x8;
	}
	switch (joint->drawMode->unk0[0]) {
	case 0:
		count2 = 0;
		count1 = 0;
		for (i = 0; i < obj->joints->partCount; i++) {
			if (*(u8*)((u8*)obj->joints->parts[i].material + 0xA) == 2) count2++;
			else if (*(u8*)((u8*)obj->joints->parts[i].material + 0xA) == 1) count1++;
			for (j = 0; j < 8; j++) {
				ref = *(void**)((u8*)obj->joints->parts[i].material + 0xC + j * 4);
				if (ref == NULL) break;
				if (*(u8*)((u8*)ref + 0xA) == 2) count2++;
				else if (*(u8*)((u8*)ref + 0xA) == 1) count1++;
			}
		}
		if (count2) *(u8*)(obj->field_0x0 + 4) = 2;
		else if (count1) *(u8*)(obj->field_0x0 + 4) = 1;
		else *(u8*)(obj->field_0x0 + 4) = 0;
		break;
	case 1:
		mode = 0;
		if (*(u32*)obj->field_0x0 & 0x8) mode = 3;
		*(u8*)(obj->field_0x0 + 4) = mode;
		break;
	case 2:
		mode = 1;
		if (*(u32*)obj->field_0x0 & 0x8) mode = 4;
		*(u8*)(obj->field_0x0 + 4) = mode;
		break;
	case 3:
		mode = 2;
		if (*(u32*)obj->field_0x0 & 0x8) mode = 5;
		*(u8*)(obj->field_0x0 + 4) = mode;
		break;
	}

	if (joint->drawMode->unk2[0] & 0x2) {
		*(u32*)obj->field_0x0 |= 0x10;
		if (joint->drawMode->unk2[0] & 0x10) {
			*(u32*)obj->field_0x0 |= 0x8000;
		}
		switch (joint->drawMode->unk0[0]) {
		case 0: *(u8*)(obj->field_0x0 + 4) = 2; break;
		case 1: *(u8*)(obj->field_0x0 + 4) = 0; break;
		case 2: *(u8*)(obj->field_0x0 + 4) = 1; break;
		case 3:
		default: *(u8*)(obj->field_0x0 + 4) = 2; break;
		}
	}

	if (*(u32*)obj->field_0x0 & 0x10) {
		if (parent != NULL && ((*(u32*)parent->field_0x0 & 0x10) || (*(u32*)parent->field_0x0 & 0x20))) {
			*(u32*)obj->field_0x0 &= ~0x10;
		}
	}
	else if (parent != NULL && ((*(u32*)parent->field_0x0 & 0x10) || (*(u32*)parent->field_0x0 & 0x20))) {
		*(u32*)obj->field_0x0 |= 0x20;
		if (*(u32*)parent->field_0x0 & 0x8000) {
			*(u32*)obj->field_0x0 |= 0x8000;
		}
	}

	if (joint->drawMode->unk2[0] & 0x4) {
		*(u32*)obj->field_0x0 |= 0x4;
	}
	}
	else {
		OSPanic("mapdrv.c", 741, "\203W\203\207\203C\203\223\203g\202\314\203f\201[\203^\202\314\216w\222\350\202\311\202\250\202\251\202\265\202\310\202\340\202\314\202\252\202\240\202\351\n");
	}

	if (joint->partCount) {
		PSVECAdd(&joint->bboxMin, &joint->bboxMax, (Vec*)(obj->field_0xC + 4));
		PSVECScale((Vec*)(obj->field_0xC + 4), (Vec*)(obj->field_0xC + 4), 0.5f);
	}
	else {
		*(Vec*)(obj->field_0xC + 4) = joint->translation;
	}

	if (joint->partCount) {
		PSMTXMultVec(obj->modelWorldMtx, (Vec*)(obj->field_0xC + 4), &vecA);
		PSMTXMultVec(obj->modelWorldMtx, &joint->bboxMax, &vecB);
		if (PSVECSquareDistance(&vecA, &vecB) < float_20000_804198d8) {
			*(u32*)obj->field_0x0 |= 0x2;
		}
	}
	return obj;
}

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

// MWCC register allocation of the long-lived locals differs: the target keeps wp in r31 and the
// group*0x178 offset in r27, ours puts wp in r27 and the offset/slot/j8 temporaries in r29/r30/r31.
// The declaration order below is match-only: it makes MWCC's declared-locals block start at r28 and
// reproduce every other register.
// NONMATCHING (97.11%): only wp/offset/slot/j8 registers differ.
void makeDisplayList(BOOL group) {
	MapEntry* entry;
	MapWork* wp;
	void* buf;
	s32 i;
	s32 j;
	u8* p;
	s32 pass;
	s32 total;
	u8* dest;
	MapObject* obj;

	wp = &mapWork[activeGroup];
	buf = _mapAlloc(0x10000);
	entry = &wp->entries[group];
	__mapdrv_make_dl = TRUE;
	total = 0;
	for (pass = 0; pass < 2; pass++) {
		obj = entry->objects;
		for (i = 0; i < 8; i++) {
			*(void**)(obj->field_0xEC + 8 + i * 4) = NULL;
			*(u32*)(obj->field_0xEC + 0x28 + i * 4) = 0;
		}
		for (i = 0; i < entry->numJoints; i++, obj++) {
			if (obj->joints->partCount < 1 || obj->joints->partCount > 8) {
				continue;
			}
			if (obj->joints->drawMode->unk2[0] & 0x8) {
				continue;
			}
			if (*(u32*)(obj->joints->drawMode->unk2 + 2) != 0) {
				continue;
			}
			if (shadowGetCharShadowProjection() != 0) {
				if (*(u32*)obj->field_0x0 & 0x4) {
					continue;
				}
			}
			if (!(entry->flags & 0x8000)) {
				continue;
			}
			for (j = 0; j < obj->joints->partCount; j++) {
				MapFileMesh* mesh = obj->joints->parts[j].mesh;

				memset(buf, 0, 0x10000);
				DCInvalidateRange(buf, 0x10000);
				GXBeginDisplayList(buf, 0x10000);
				GXResetWriteGatherPipe();
				mapSetMaterial(obj, mesh);
				GXSetCullMode(culling[obj->joints->drawMode->cullMode]);
				*(u32*)(obj->field_0xEC + 0x28 + j * 4) = GXEndDisplayList();
				if (pass == 0) {
					total += *(u32*)(obj->field_0xEC + 0x28 + j * 4);
				}
				else {
					*(void**)(obj->field_0xEC + 8 + j * 4) = dest;
					dest += *(u32*)(obj->field_0xEC + 0x28 + j * 4);
					memcpy(*(void**)(obj->field_0xEC + 8 + j * 4), buf, *(u32*)(obj->field_0xEC + 0x28 + j * 4));
					DCFlushRange(*(void**)(obj->field_0xEC + 8 + j * 4), *(u32*)(obj->field_0xEC + 0x28 + j * 4));
				}
			}
		}
		if (pass == 0) {
			*(u32*)(entry->field_0x168) = total;
			if (*(u32*)(entry->field_0x168) != 0) {
				*(void**)(entry->field_0x168 + 4) = _mapAllocTail(*(u32*)(entry->field_0x168));
				dest = *(u8**)(entry->field_0x168 + 4);
			}
			else {
#ifdef MUST_MATCH
				// MUST_MATCH: naming the group offset here is what makes MWCC keep the offset live and
				// emit the target's `add r3,wp,off; stw 0x16c(r3)` instead of an indexed store.
				*(void**)(((u8*)wp + group * 0x178) + 0x16C) = NULL;
#else
				*(void**)(entry->field_0x168 + 4) = NULL;
#endif
				break;
			}
		}
	}
	_mapFree(buf);
	__mapdrv_make_dl = FALSE;
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

// the 1.0f/1000.0f literals must be the named .sdata2 externs, and float_0/float_1 must be
// declared `const` (they are read-only in the original) so the compiler hoists both loads ahead of
// the stores and assigns f1=float_0, f0=float_1. objdiff still prints an arg_mismatch for the
// compiler-generated int->float `double_to_int` constant (local @nnnn vs the named .rodata symbol);
// it is a relocation-only difference and does not affect the instruction score.
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
	*(f32*)(found8 + 0x14) = float_1_80419860;
	*(u16*)found |= 0x1;
	if (a2 == 1) {
		*(u16*)found |= 0x4;
	}
	if (a2 == 2) {
		*(OSTime*)(found8 + 0x8) -= (u64)(float_1000_80419874 * *(f32*)(*(u8**)(found8 + 0x18) + 8) / (f32)gp->framerate);
	}
}

// The search loop is inlined here instead of calling mapSearchAnmObj: MWCC's deferred inliner colors
// the inlined wp/entry pair the other way when it inlines that function itself (99.47%). This copy,
// with the loop pointer declared at function scope and j declared before i, reproduces the target's
// registers. Natural form: `found = mapSearchAnmObj(animName);`.
#ifdef MUST_MATCH
static inline MapEntryAnimData* mapCheckAnimationSearch(const char* animName) {
	MapEntry* entry;
	MapWork* wp;
	MapEntryAnimData* anim;
	s32 j;
	s32 i;

	wp = &mapWork[activeGroup];
	entry = &wp->entries[0];
	for (i = 0; i < wp->entries[0].count; entry++, i++) {
		anim = entry->animData;
		for (j = 0; j < *(s32*)entry->field_0x160; j++, anim++) {
			if ((*(u16*)anim & 0x1) && !strcmp(*(char**)*(void**)((u8*)anim + 0x18), animName)) {
				return anim;
			}
		}
	}
	return NULL;
}
#endif

void mapCheckAnimation(const char* animName, BOOL* valid, f32* a3) {
#ifdef MUST_MATCH
	MapEntryAnimData* found = mapCheckAnimationSearch(animName);
#else
	MapEntryAnimData* found = mapSearchAnmObj(animName);
#endif

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
extern GXColor dat_8041981c;

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

// NONMATCHING (98.51%): spline math, constants and compare directions match. What differs is MWCC
// register colouring of the inner loop's three pointers (chan/out/chan2) and the key-select value (f4 vs
// f13); the d==0 boolean is `clrlwi.` vs the original's `cntlzw/srwi.` (same result, different normalisation.)
void mapCalcAnimMatrix(Mtx outMtx, Mtx inMtx, MapEntryAnimData* anim, void* jointPtr) {
	MapAnimData* joint = (MapAnimData*)jointPtr;
	MapAnimKey* key;
	s32 count;
	s32 i, k;
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
	f32 zero;

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
				f32 two, neg2;

				flag = (d != MATCH_F32(float_0_80419850));
				if ((flag & 1) == 0) {
					den = MATCH_F32(float_1Eneg08_804198b8);
				}
				else {
					den = d;
				}
				u = dt / den;
				two = MATCH_F32(float_2_80419858);
				u2 = u * u;
				u3 = u2 * u;
				a = MATCH_F32(float_3_8041984c) * u2;
				b = u3 - two * u2;
				c = two * u3 - a;
				h10 = u + b;
				h00 = MATCH_F32(float_1_80419860) + c;
				neg2 = MATCH_F32(float_neg2_804198a0);
				h01 = neg2 * u3 + a;


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

		zero = float_0_80419850;
		if (f31 != zero || f30 != zero || f29 != zero) {
			PSMTXTrans(trans, f31, f30, f29);
		}
		if (f28 != zero) {
			PSMTXRotRad(rotX, 'x', float_deg2rad_80419868 * f28);
		}
		if (f27 != zero) {
			PSMTXRotRad(rotY, 'y', float_deg2rad_80419868 * f27);
		}
		if (f23 != zero) {
			PSMTXRotRad(rotZ, 'z', float_deg2rad_80419868 * f23);
		}
		PSMTXScale(scale, f26, f25, f24);
		PSMTXIdentity(local);
		zero = float_0_80419850;
		if (f31 != zero || f30 != zero || f29 != zero) {
			PSMTXConcat(local, trans, local);
		}
		if (f23 != zero) {
			PSMTXConcat(local, rotZ, local);
		}
		if (f27 != zero) {
			PSMTXConcat(local, rotY, local);
		}
		if (f28 != zero) {
			PSMTXConcat(local, rotX, local);
		}
		if (f26 != zero || f25 != zero || f24 != zero) {
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

// NONMATCHING (96.81%): entry/i (r29/r30) register swap in the first two loops and one extra
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

// NONMATCHING (94.58%): register colouring/scheduling only -- texCoord/texMap coalesce into one call-clobbered register, the int->float magic double is a fresh copy not base+0x2C8, the texCount test folds into `mr.`, and case 9 uses indexed scrl_uv updates (see REPORT.md).
void mapSetMaterialTev(s32 texCount, s32 drawMode, s32 flags, Mtx mtx) {
	u8* base;
	s32 texMap;
	s32 texCoord;
	GXTevStageID tevStage;
	s32 numIndStages;
	Mtx queen_mtx;

	base = vec3_802c16d8;
	texMap = texCount;
	texCoord = texCount;
	numIndStages = 0;
	
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

			case 8: {
				GXColor color = *(GXColor*)&unk_80422f88;

				color.g = (u8)((127.0f * (f32)sin((TWO_PI * (f32)(2 * gp->retraceCount)) / 360.0f)) + 127.0f);
				GXSetTevColor(GX_TEVREG0, color);
			}
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
		GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
		tevStage++;
	}

	if (flags & 0x4) {
		if (shadowGetCharShadowProjection() != 0) {
			if (shadowGetCharShadowTexObj() != NULL) {
				Mtx mtx2;
				GXColor color = shadowGetCharShadowColor();

				PSMTXConcat(shadowGetCharShadowProjTexMtx(), mtx, mtx2);
				GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, TEXMTX(texMap), GX_FALSE, GX_PTIDENTITY);
				GXLoadTexMtxImm(mtx2, TEXMTX(texMap), GX_MTX3x4);
				GXLoadTexObj(shadowGetCharShadowTexObj(), texMap);
				GXSetTevOrder(tevStage, texCoord, texMap, GX_COLOR_NULL);
				GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(tevStage, GX_CC_CPREV, GX_CC_KONST, GX_CC_TEXC, GX_CC_ZERO);
				GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
				GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevKColor(GX_KCOLOR1, color);
				GXSetTevKColorSel(tevStage, GX_TEV_KCSEL_K1);
				tevStage++;
				texMap++;
				texCoord++;
			}
		}
	}

	if (flags & 0x400) {
		Mtx mtx2;
		GXTevStageID stage;
		GXColor color;

		GXSetTevColorOp(tevStage - 1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
		GXSetTevAlphaOp(tevStage - 1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
		PSMTXConcat(projMtx, mtx, mtx2);
		GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, TEXMTX(texMap), GX_FALSE, GX_PTIDENTITY);
		GXLoadTexMtxImm(mtx2, TEXMTX(texMap), GX_MTX3x4);
		GXLoadTexObj(smartTexObj(&projTexObj, (void**)projTexImgRef), texMap);
		GXSetTevOrder(tevStage, texCoord, texMap, GX_COLOR_NULL);
		GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C1);
		GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
		GXSetTevSwapMode(tevStage, GX_TEV_SWAP3, GX_TEV_SWAP3);
		GXSetTevSwapModeTable(GX_TEV_SWAP3, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_RED);
		color = *(GXColor*)&dat_80419800;
		GXSetTevColor(GX_TEVREG1, color);
		stage = tevStage + 1;
		texMap++;
		texCoord++;
		GXSetTevOrder(stage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
		GXSetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(stage, GX_CC_C0, GX_CC_CPREV, GX_CC_APREV, GX_CC_ZERO);
		GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
		GXSetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP0);
		tevStage += 2;
	}

	if (flags & 0x10000) {
		if (shadowGetDepthShadowTexObj() != NULL) {
			GXTexObj* depthTex = shadowGetDepthShadowTexObj();
			void* depthMtx = shadowGetDepthShadowDepthMtx();
			void* depthProjMtx = shadowGetDepthShadowProjTexMtx();
			GXColor color = shadowGetDepthShadowColor();
			void* config = shadowGetDepthShadowConfig();
			Mtx mtx2;
			GXTevStageID stage;
			s32 coord;

			switch (*(s32*)((u8*)config + 0x3C)) {
			case 0:
				GXSetTevColorOp(tevStage - 1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
				GXSetTevAlphaOp(tevStage - 1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
				GXSetTevOrder(tevStage, texCoord, texMap, GX_COLOR_NULL);
				GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
				GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
				GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, TEXMTX(texMap), GX_FALSE, GX_PTIDENTITY);
				GXLoadTexObj(shadowGetRamp8TexObj(), texMap);
				PSMTXConcat(depthMtx, mtx, mtx2);
				GXLoadTexMtxImm(mtx2, TEXMTX(texMap), GX_MTX3x4);
				stage = tevStage + 1;
				coord = texCoord + 1;
				texMap++;
				GXSetTevOrder(stage, coord, texMap, GX_COLOR_NULL);
				GXSetTevColorIn(stage, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ONE, GX_CC_ZERO);
				GXSetTevColorOp(stage, GX_TEV_COMP_R8_GT, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
				GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
				GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTexCoordGen2(coord, GX_TG_MTX3x4, GX_TG_POS, TEXMTX(texMap), GX_FALSE, GX_PTIDENTITY);
				GXLoadTexObj(depthTex, texMap);
				PSMTXConcat(depthProjMtx, mtx, mtx2);
				GXLoadTexMtxImm(mtx2, TEXMTX(texMap), GX_MTX3x4);
				texMap++;
				texCoord += 2;
				stage = tevStage + 2;
				GXSetTevOrder(stage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
				GXSetTevColorIn(stage, GX_CC_ZERO, GX_CC_KONST, GX_CC_CPREV, GX_CC_C0);
				GXSetTevColorOp(stage, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevKColor(GX_KCOLOR1, color);
				GXSetTevKColorSel(stage, GX_TEV_KCSEL_K1);
				GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
				GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage += 3;
				break;
			case 1:
				GXSetTevColor(GX_TEVREG1, *(GXColor*)&dat_80419804);
				GXSetTevOrder(tevStage, texCoord, texMap, GX_COLOR_NULL);
				GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C1, GX_CC_ZERO);
				GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
				GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, TEXMTX(texMap), GX_FALSE, GX_PTIDENTITY);
				GXLoadTexObj(shadowGetRamp8TexObj(), texMap);
				PSMTXConcat(depthMtx, mtx, mtx2);
				GXLoadTexMtxImm(mtx2, TEXMTX(texMap), GX_MTX3x4);
				tevStage++;
				texMap++;
				texCoord++;
				break;
			case 2:
				GXSetTevColor(GX_TEVREG1, *(GXColor*)&dat_80419808);
				GXSetTevOrder(tevStage, texCoord, texMap, GX_COLOR_NULL);
				GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C1, GX_CC_ZERO);
				GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
				GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, TEXMTX(texMap), GX_FALSE, GX_PTIDENTITY);
				GXLoadTexObj(depthTex, texMap);
				PSMTXConcat(depthProjMtx, mtx, mtx2);
				GXLoadTexMtxImm(mtx2, TEXMTX(texMap), GX_MTX3x4);
				tevStage++;
				texMap++;
				texCoord++;
				break;
			case 3:
				GXSetTevColorOp(tevStage - 1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
				GXSetTevAlphaOp(tevStage - 1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
				GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_ALPHA, GX_CH_BLUE, GX_CH_ALPHA);
				GXSetTevSwapModeTable(GX_TEV_SWAP2, GX_CH_ALPHA, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
				GXSetTevOrder(tevStage, texCoord, texMap, GX_COLOR_NULL);
				GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
				GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
				GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP1);
				GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, TEXMTX(texMap), GX_FALSE, GX_PTIDENTITY);
				GXLoadTexObj(shadowGetRamp16TexObj(), texMap);
				PSMTXConcat(depthMtx, mtx, mtx2);
				GXLoadTexMtxImm(mtx2, TEXMTX(texMap), GX_MTX3x4);
				stage = tevStage + 1;
				coord = texCoord + 1;
				texMap++;
				GXSetTevOrder(stage, coord, texMap, GX_COLOR_NULL);
				GXSetTevColorIn(stage, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ONE, GX_CC_ZERO);
				GXSetTevColorOp(stage, GX_TEV_COMP_GR16_GT, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
				GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
				GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP2);
				GXSetTexCoordGen2(coord, GX_TG_MTX3x4, GX_TG_POS, TEXMTX(texMap), GX_FALSE, GX_PTIDENTITY);
				GXLoadTexObj(depthTex, texMap);
				PSMTXConcat(depthProjMtx, mtx, mtx2);
				GXLoadTexMtxImm(mtx2, TEXMTX(texMap), GX_MTX3x4);
				texMap++;
				texCoord += 2;
				stage = tevStage + 2;
				GXSetTevOrder(stage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
				GXSetTevColorIn(stage, GX_CC_ZERO, GX_CC_KONST, GX_CC_CPREV, GX_CC_C0);
				GXSetTevColorOp(stage, GX_TEV_SUB, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevKColor(GX_KCOLOR1, color);
				GXSetTevKColorSel(stage, GX_TEV_KCSEL_K1);
				GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
				GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage += 3;
				break;
			case 4:
				GXSetTevColor(GX_TEVREG1, *(GXColor*)&dat_8041980c);
				GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_ALPHA, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
				GXSetTevOrder(tevStage, texCoord, texMap, GX_COLOR_NULL);
				GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C1, GX_CC_ZERO);
				GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
				GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP1);
				GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, TEXMTX(texMap), GX_FALSE, GX_PTIDENTITY);
				GXLoadTexObj(shadowGetRamp16TexObj(), texMap);
				PSMTXConcat(depthMtx, mtx, mtx2);
				GXLoadTexMtxImm(mtx2, TEXMTX(texMap), GX_MTX3x4);
				tevStage++;
				texMap++;
				texCoord++;
				break;
			case 5:
				GXSetTevColor(GX_TEVREG1, *(GXColor*)&dat_80419810);
				GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_ALPHA, GX_CH_BLUE, GX_CH_ALPHA);
				GXSetTevOrder(tevStage, texCoord, texMap, GX_COLOR_NULL);
				GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C1, GX_CC_ZERO);
				GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
				GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP1);
				GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, TEXMTX(texMap), GX_FALSE, GX_PTIDENTITY);
				GXLoadTexObj(depthTex, texMap);
				PSMTXConcat(depthProjMtx, mtx, mtx2);
				GXLoadTexMtxImm(mtx2, TEXMTX(texMap), GX_MTX3x4);
				tevStage++;
				texMap++;
				texCoord++;
				break;
			}
		}
	}

	if (flags & 0x20000) {
		if (shadowGetProjShadowTexObj() != NULL) {
			GXTexObj* projTex = shadowGetProjShadowTexObj();
			void* projShadowMtx = shadowGetProjShadowProjTexMtx();
			GXColor color = shadowGetProjShadowColor();
			Mtx mtx2;

			shadowGetProjShadowConfig();
			GXSetTevOrder(tevStage, texCoord, texMap, GX_COLOR_NULL);
			GXSetTevColorIn(tevStage, GX_CC_CPREV, GX_CC_KONST, GX_CC_TEXA, GX_CC_ZERO);
			GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_KONST, GX_CA_TEXA, GX_CA_APREV);
			GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
			GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
			GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, TEXMTX(texMap), GX_FALSE, GX_PTIDENTITY);
			GXLoadTexObj(projTex, texMap);
			PSMTXConcat(projShadowMtx, mtx, mtx2);
			GXLoadTexMtxImm(mtx2, TEXMTX(texMap), GX_MTX3x4);
			GXSetTevKColor(GX_KCOLOR1, color);
			GXSetTevKColorSel(tevStage, GX_TEV_KCSEL_K1);
			GXSetTevKAlphaSel(tevStage, GX_TEV_KASEL_K1_A);
			tevStage++;
			texMap++;
			texCoord++;
		}
	}

	if (flags & 0x40000) {
		if (shadowGetDepthShadowTexObj() != NULL) {
			GXTexObj* depthTex = shadowGetDepthShadowTexObj();
			void* depthMtx = shadowGetDepthShadowDepthMtx();
			void* depthProjMtx = shadowGetDepthShadowProjTexMtx();
			GXColor color = shadowGetDepthShadowColor();
			void* config = shadowGetDepthShadowConfig();
			Mtx mtx2;
			GXTevStageID stage;
			s32 coord;

			switch (*(s32*)((u8*)config + 0x3C)) {
			case 0:
				GXSetTevColorOp(tevStage - 1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
				GXSetTevAlphaOp(tevStage - 1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
				GXSetTevOrder(tevStage, texCoord, texMap, GX_COLOR_NULL);
				GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
				GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
				GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, TEXMTX(texMap), GX_FALSE, GX_PTIDENTITY);
				GXLoadTexObj(shadowGetRamp8TexObj(), texMap);
				PSMTXConcat(depthMtx, mtx, mtx2);
				GXLoadTexMtxImm(mtx2, TEXMTX(texMap), GX_MTX3x4);
				stage = tevStage + 1;
				coord = texCoord + 1;
				texMap++;
				GXSetTevOrder(stage, coord, texMap, GX_COLOR_NULL);
				GXSetTevColorIn(stage, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ONE, GX_CC_ZERO);
				GXSetTevColorOp(stage, GX_TEV_COMP_R8_GT, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
				GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
				GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTexCoordGen2(coord, GX_TG_MTX3x4, GX_TG_POS, TEXMTX(texMap), GX_FALSE, GX_PTIDENTITY);
				GXLoadTexObj(depthTex, texMap);
				PSMTXConcat(depthProjMtx, mtx, mtx2);
				GXLoadTexMtxImm(mtx2, TEXMTX(texMap), GX_MTX3x4);
				texMap++;
				texCoord += 2;
				stage = tevStage + 2;
				GXSetTevOrder(stage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
				GXSetTevColorIn(stage, GX_CC_C0, GX_CC_KONST, GX_CC_CPREV, GX_CC_ZERO);
				GXSetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevKColor(GX_KCOLOR1, color);
				GXSetTevKColorSel(stage, GX_TEV_KCSEL_K1);
				GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ONE);
				GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevKAlphaSel(stage, GX_TEV_KASEL_K1_A);
				tevStage += 3;
				break;
			case 3:
				GXSetTevColorOp(tevStage - 1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
				GXSetTevAlphaOp(tevStage - 1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
				GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_ALPHA, GX_CH_BLUE, GX_CH_ALPHA);
				GXSetTevSwapModeTable(GX_TEV_SWAP2, GX_CH_ALPHA, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
				GXSetTevOrder(tevStage, texCoord, texMap, GX_COLOR_NULL);
				GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
				GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
				GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP1);
				GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, TEXMTX(texMap), GX_FALSE, GX_PTIDENTITY);
				GXLoadTexObj(shadowGetRamp16TexObj(), texMap);
				PSMTXConcat(depthMtx, mtx, mtx2);
				GXLoadTexMtxImm(mtx2, TEXMTX(texMap), GX_MTX3x4);
				stage = tevStage + 1;
				coord = texCoord + 1;
				texMap++;
				GXSetTevOrder(stage, coord, texMap, GX_COLOR_NULL);
				GXSetTevColorIn(stage, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ONE, GX_CC_ZERO);
				GXSetTevColorOp(stage, GX_TEV_COMP_GR16_GT, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
				GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
				GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP2);
				GXSetTexCoordGen2(coord, GX_TG_MTX3x4, GX_TG_POS, TEXMTX(texMap), GX_FALSE, GX_PTIDENTITY);
				GXLoadTexObj(depthTex, texMap);
				PSMTXConcat(depthProjMtx, mtx, mtx2);
				GXLoadTexMtxImm(mtx2, TEXMTX(texMap), GX_MTX3x4);
				texMap++;
				texCoord += 2;
				stage = tevStage + 2;
				GXSetTevOrder(stage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
				GXSetTevColorIn(stage, GX_CC_C0, GX_CC_KONST, GX_CC_CPREV, GX_CC_ZERO);
				GXSetTevColorOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ONE);
				GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTevKColor(GX_KCOLOR1, color);
				GXSetTevKColorSel(stage, GX_TEV_KCSEL_K1);
				GXSetTevKAlphaSel(stage, GX_TEV_KASEL_K1_A);
				tevStage += 3;
				break;
			}
		}
	}

	if (flags & 0x80100000) {
		s32 r19;
		s32 r21;
		s32 r22;
		s32 r20;

		if (flags & 0x100000) {
			r19 = current_mp->field_0xC[0];
			r21 = current_mp->field_0xC[1];
			r22 = current_mp->field_0xC[2];
			r20 = current_mp->field_0xC[3];
		}
		else {
			r19 = 0xF0;
			r21 = 0xF0;
			r22 = 0x64;
			r20 = (u8)(64.0f * (1.0f - (f32)cos(1.5708f * (f32)gp->retraceCount / 20.0f)) * 0.5f + 64.0f);
		}
		GXSetTevOrder(tevStage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
		GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(tevStage, GX_CC_CPREV, GX_CC_ZERO, GX_CC_ZERO, GX_CC_KONST);
		GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
		GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
		{
			GXColor color = *(GXColor*)&dat_80419814;
			color.r = r19 * r20 / 255;
			color.g = r21 * r20 / 255;
			color.b = r22 * r20 / 255;
			GXSetTevKColor(GX_KCOLOR1, color);
		}
		GXSetTevKColorSel(tevStage, GX_TEV_KCSEL_K1);
		tevStage++;
	}

	if (flags & 0x800000) {
		GXTexObj texObj;
		Mtx orthoMtx;
		Mtx lookAtMtx;
		Vec camPos;
		Vec up;
		Vec target;
		s32 v;

		camPos = *(Vec*)(base + 0x120);
		up = *(Vec*)(base + 0x12c);
		target = *(Vec*)(base + 0x138);
		v = (s32)(127.0f * (1.0f - (f32)cos(1.5708f * (f32)gp->retraceCount / 30.0f)) * 0.5f + 128.0f);
		GXInitTexObj(&texObj, texture_0, 4, 0x40, GX_TF_CMPR, GX_CLAMP, GX_REPEAT, GX_FALSE);
		GXLoadTexObj(&texObj, texMap);
		C_MTXLightOrtho(orthoMtx, -2.0f, 2.0f, -2.0f, 2.0f, 0.5f, 0.5f, 0.5f, (f32)gp->retraceCount / 50.0f);
		C_MTXLookAt(lookAtMtx, &camPos, &up, &target);
		PSMTXConcat(orthoMtx, lookAtMtx, orthoMtx);
		GXLoadTexMtxImm(orthoMtx, TEXMTX(texMap), GX_MTX3x4);
		GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, TEXMTX(texMap), GX_FALSE, GX_PTIDENTITY);
		GXSetTevOrder(tevStage, texCoord, texMap, GX_COLOR_NULL);
		GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(tevStage, GX_CC_TEXC, GX_CC_CPREV, GX_CC_KONST, GX_CC_ZERO);
		GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
		GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
		{
			GXColor color = *(GXColor*)&dat_80419818;
			color.r = v;
			color.g = v;
			color.b = v;
			GXSetTevKColor(GX_KCOLOR1, color);
		}
		GXSetTevKColorSel(tevStage, GX_TEV_KCSEL_K1);
		tevStage++;
		texMap++;
		texCoord++;
	}

	if (flags & 0xF000000) {
		MapTevCallback work;
		void (*callback)(MapTevCallback*);

		work.obj = current_mp;
		work.mtx = mtx;
		work.drawMode = drawMode;
		work.tevStage = tevStage;
		work.texMap = texMap;
		work.texCoord = texCoord;
		work.numIndStage = 0;
		callback = *(void**)((u8*)&mapWork[activeGroup].entries[0] + 0x110 + ((flags & 0xF000000) >> 22));
		if (callback != NULL) {
			callback(&work);
		}
		tevStage = work.tevStage;
		texCoord = work.texCoord;
		numIndStages = work.numIndStage;
	}

	GXSetNumTexGens((u8)texCoord);
	GXSetNumTevStages((u8)tevStage);
	GXSetNumIndStages((u8)numIndStages);
}

static BOOL mapReCalcMatrixCheck(BOOL force, u32 flags) {
	BOOL recalc;

	recalc = FALSE;
	if (flags & 0x200) {
		recalc = TRUE;
	}
	if (force) {
		recalc = TRUE;
	}
	return recalc;
}

void mapReCalcMatrix(MapObject* obj, Mtx mtx, BOOL force) {
	u32* flags = (u32*)obj->field_0x0;
	MapObject* child;
	MapObject* next;
	BOOL recalc;

	recalc = FALSE;
	if ((*flags) & 0x200) {
		recalc = TRUE;
	}
	if (force) {
		recalc = TRUE;
	}

	if (recalc) {
		if ((*flags) & 0x2000) {
			PSMTXConcat(mtx, (MtxPtr)((u8*)obj + 0x7C), obj->modelWorldMtx);
		}
		else {
			PSMTXConcat(mtx, (MtxPtr)((u8*)obj + 0x4C), obj->modelWorldMtx);
		}
		if ((*flags) & 0x200000) {
			PSMTXConcat(obj->modelWorldMtx, obj->unkAC, obj->modelWorldMtx);
		}
		(*flags) &= ~0x200;

		child = obj->child;
		if (child != NULL) {
			u32* cflags = (u32*)child->field_0x0;

			if ((*cflags) & 0x2000) {
				PSMTXConcat(obj->modelWorldMtx, (MtxPtr)((u8*)child + 0x7C), child->modelWorldMtx);
			}
			else {
				PSMTXConcat(obj->modelWorldMtx, (MtxPtr)((u8*)child + 0x4C), child->modelWorldMtx);
			}
			if ((*cflags) & 0x200000) {
				PSMTXConcat(child->modelWorldMtx, child->unkAC, child->modelWorldMtx);
			}
			(*cflags) &= ~0x200;

			if (child->child != NULL) {
				mapReCalcMatrix(child->child, child->modelWorldMtx, 1);
			}
			if (child->next != NULL) {
				mapReCalcMatrix(child->next, obj->modelWorldMtx, 1);
			}
		}

		next = obj->next;
		if (next != NULL) {
			u32* nflags = (u32*)next->field_0x0;

			if (mapReCalcMatrixCheck(force, *nflags)) {
				if ((*nflags) & 0x2000) {
					PSMTXConcat(mtx, (MtxPtr)((u8*)next + 0x7C), next->modelWorldMtx);
				}
				else {
					PSMTXConcat(mtx, (MtxPtr)((u8*)next + 0x4C), next->modelWorldMtx);
				}
				if ((*nflags) & 0x200000) {
					PSMTXConcat(next->modelWorldMtx, next->unkAC, next->modelWorldMtx);
				}
				(*nflags) &= ~0x200;

				if (next->child != NULL) {
					mapReCalcMatrix(next->child, next->modelWorldMtx, 1);
				}
				if (next->next != NULL) {
					mapReCalcMatrix(next->next, mtx, force);
				}
			}
			else {
				if (next->child != NULL) {
					mapReCalcMatrix(next->child, next->modelWorldMtx, force);
				}
				if (next->next != NULL) {
					mapReCalcMatrix(next->next, mtx, force);
				}
			}
		}
	}
	else {
		child = obj->child;
		if (child != NULL) {
			u32* cflags = (u32*)child->field_0x0;

			if (mapReCalcMatrixCheck(force, *cflags)) {
				if ((*cflags) & 0x2000) {
					PSMTXConcat(obj->modelWorldMtx, (MtxPtr)((u8*)child + 0x7C), child->modelWorldMtx);
				}
				else {
					PSMTXConcat(obj->modelWorldMtx, (MtxPtr)((u8*)child + 0x4C), child->modelWorldMtx);
				}
				if ((*cflags) & 0x200000) {
					PSMTXConcat(child->modelWorldMtx, child->unkAC, child->modelWorldMtx);
				}
				(*cflags) &= ~0x200;

				if (child->child != NULL) {
					mapReCalcMatrix(child->child, child->modelWorldMtx, 1);
				}
				if (child->next != NULL) {
					mapReCalcMatrix(child->next, obj->modelWorldMtx, force);
				}
			}
			else {
				if (child->child != NULL) {
					mapReCalcMatrix(child->child, child->modelWorldMtx, force);
				}
				if (child->next != NULL) {
					mapReCalcMatrix(child->next, obj->modelWorldMtx, force);
				}
			}
		}

		next = obj->next;
		if (next != NULL) {
			u32* nflags = (u32*)next->field_0x0;

			if (mapReCalcMatrixCheck(force, *nflags)) {
				if ((*nflags) & 0x2000) {
					PSMTXConcat(mtx, (MtxPtr)((u8*)next + 0x7C), next->modelWorldMtx);
				}
				else {
					PSMTXConcat(mtx, (MtxPtr)((u8*)next + 0x4C), next->modelWorldMtx);
				}
				if ((*nflags) & 0x200000) {
					PSMTXConcat(next->modelWorldMtx, next->unkAC, next->modelWorldMtx);
				}
				(*nflags) &= ~0x200;

				if (next->child != NULL) {
					mapReCalcMatrix(next->child, next->modelWorldMtx, 1);
				}
				if (next->next != NULL) {
					mapReCalcMatrix(next->next, mtx, force);
				}
			}
			else {
				if (next->child != NULL) {
					mapReCalcMatrix(next->child, next->modelWorldMtx, force);
				}
				if (next->next != NULL) {
					mapReCalcMatrix(next->next, mtx, force);
				}
			}
		}
	}
}

static inline u8* mapSearchAnimObj(const char* name) {
	MapEntry* entry;
	MapWork* wp;
	s32 i;
	s32 j;
	u8* p;
	u8* table;

	wp = &mapWork[activeGroup];
	entry = &wp->entries[0];
	for (i = 0; i < wp->entries[0].count; entry++, i++) {
		table = *(u8**)((u8*)entry + 0xA0);
		if (table == NULL) {
			return NULL;
		}
		for (j = 0; j < *(s32*)table; j++) {
			u8* pair = table + 4 + j * 8;
			if (!strcmp(*(char**)pair, name)) {
				return *(u8**)(pair + 4);
			}
		}
	}
	return NULL;
}

// Complete and logically correct; remaining diffs are register allocation/scheduling: the outer entry
// loop spills i, the five animation-list pointers and the OSTime land in different callee-saved
// registers, the inlined Hermite interpolation uses f29-f31 (frame 0x310 vs 0x2e0), and the
// list17/18/16 key loops pick an equivalent but differently laid-out compare/branch.
// NONMATCHING (80.60%): see above.
void mapMain(void) {
	MapWork* wp0;
	MapEntry* entry;
	u8* anim;
	u8* list19;
	u8* list18;
	u8* list17;
	u8* list16;
	u8* list15;
	OSTime time;
	BOOL updated;
	s32 i;
	s32 j;
	u8* p;
	char namebuf[0x100];
	char namebuf2[0x100];

	wp0 = &mapWork[activeGroup];
	entry = &wp0->entries[0];
	for (i = 0; i < wp0->entries[0].count; entry++, i++) {
		anim = (u8*)entry->animData;
		updated = FALSE;
		for (j = 0; j < *(s32*)entry->field_0x160; j++, anim += 0x20) {
			list19 = *(u8**)((*(u8**)(anim + 0x18)) + 0xC);
			list18 = *(u8**)((*(u8**)(anim + 0x18)) + 0x10);
			list17 = *(u8**)((*(u8**)(anim + 0x18)) + 0x14);
			list16 = *(u8**)((*(u8**)(anim + 0x18)) + 0x18);
			list15 = *(u8**)((*(u8**)(anim + 0x18)) + 0x1C);
			if (!((*(u16*)anim) & 0x1)) {
				continue;
			}
			if ((*(u16*)anim) & 0x2) {
				continue;
			}
			if ((*(u16*)anim) & 0x100) {
				time = gp->systemLv0Time;
			}
			else if ((*(u16*)anim) & 0x200) {
				time = gp->systemLv1Time;
			}
			else if ((*(u16*)anim) & 0x400) {
				time = gp->systemLv2Time;
			}
			else if ((*(u16*)anim) & 0x800) {
				time = gp->systemLv3Time;
			}
			else if (gp->inBattle) {
				time = gp->renderTime;
			}
			else {
				time = gp->renderFieldTime;
			}
			if (!((*(u16*)anim) & 0x10)) {
				f32 f = (f32)(u64)(OSTicksToMilliseconds(time) - *(OSTime*)(anim + 0x8)) * gp->framerate / 1000.0f;
				f32 frame = *(f32*)(anim + 0x10);
				if (f < frame) {
					*(f32*)(anim + 0x10) = frame - (frame - f);
				}
				else {
					*(f32*)(anim + 0x10) = *(f32*)(anim + 0x14) * (f - frame) + frame;
				}
				*(OSTime*)(anim + 0x8) = OSTicksToMilliseconds(time);
				*(OSTime*)(anim + 0x8) -= (u64)(1000.0f * *(f32*)(anim + 0x10) / (f32)gp->framerate);
			}
			else if ((*(u16*)anim) & 0x20) {
				*(OSTime*)(anim + 0x8) = OSTicksToMilliseconds(time);
				*(OSTime*)(anim + 0x8) -= (u64)(1000.0f * *(f32*)(anim + 0x10) / (f32)gp->framerate);
				*(u16*)anim &= ~0x20;
				*(u16*)anim &= ~0x10;
			}
			{
				f32 frame = *(f32*)(anim + 0x10);
				f32 length = *(f32*)((*(u8**)(anim + 0x18)) + 0x8);
				if (frame >= length) {
					if ((*(u16*)anim) & 0x4) {
						while (frame > length) {
							frame -= length;
						}
						*(f32*)(anim + 0x10) = frame;
						*(OSTime*)(anim + 0x8) = OSTicksToMilliseconds(time);
						*(OSTime*)(anim + 0x8) -= (u64)(1000.0f * *(f32*)(anim + 0x10) / (f32)gp->framerate);
					}
					else {
						*(f32*)(anim + 0x10) = length;
						*(u16*)anim |= 0x2;
					}
				}
			}
			/* matrix animation */
			if (list19 != NULL) {
				u8* lp = list19;
				s32 li;
				for (li = 0; li < *(s32*)list19; li++, lp += 4) {
					u8* joint = *(u8**)(lp + 4);
					MapObject* obj = mapGetMapObjSub(*(char**)joint);
					if (obj != NULL) {
						mapCalcAnimMatrix((f32(*)[4])((u8*)obj + 0x7C), (f32(*)[4])((u8*)obj + 0x4C), (MapEntryAnimData*)anim, joint);
						*(u32*)obj |= 0x2000;
						*(u32*)obj |= 0x200;
						updated = TRUE;
					}
				}
			}
			/* color animation */
			if (list18 != NULL) {
				u8* lp = list18;
				s32 li;
				for (li = 0; li < *(s32*)list18; li++, lp += 4) {
					u8* item = *(u8**)(lp + 4);
					u8* obj;
					strcpy(namebuf, *(char**)item);
					obj = mapSearchAnimObj(namebuf);
					if (obj != NULL) {
						{
							s32 cnt = *(s32*)(item + 0x10);
							s32 ki;
							if (cnt > 1) {
								for (ki = 1; ki < cnt; ki++) {
									u8* prev = item + 0x14 + (ki - 1) * 0x68;
									u8* next = item + 0x14 + ki * 0x68;
									f32 t = *(f32*)(anim + 0x10);
									s32 k;
									u8* ch;
									u8* chn;
									if (t > *(f32*)next) {
										if (ki == cnt - 1) {
											t = *(f32*)next;
										}
										else {
											continue;
										}
									}
									ch = prev + 4;
									chn = next + 4;
									for (k = 0; k < 5; k++, ch += 0x14, chn += 0x14) {
										if (*(s32*)(ch + 0x10) != 0) {
											*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x2C + k * 4) = *(f32*)ch;
										}
										else {
											f32 k0 = *(f32*)prev;
											f32 k1 = *(f32*)next;
											f32 d = k1 - k0;
											f32 dt = t - k0;
											f32 tan0 = *(f32*)(ch + 8);
											f32 value0 = *(f32*)ch;
											f32 tan1 = *(f32*)(chn + 4);
											f32 value1 = *(f32*)chn;
											f32 den;
											f32 u, u2, a, u3, b, c, h10, h00, h01, h11;
											s32 flag;
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
											*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x2C + k * 4) = tan0 * (d * h10) + value0 * h00 + value1 * h01 + tan1 * (d * h11);
										}
									}
									*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x40) = *(f32*)(item + 8);
									*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x44) = *(f32*)(item + 0xC);
									break;
								}
							}
						}
					}
					strcpy(namebuf, *(char**)item);
					strcat(namebuf, "_v");
					obj = mapSearchAnimObj(namebuf);
					if (obj != NULL) {
						{
							s32 cnt = *(s32*)(item + 0x10);
							s32 ki;
							if (cnt > 1) {
								for (ki = 1; ki < cnt; ki++) {
									u8* prev = item + 0x14 + (ki - 1) * 0x68;
									u8* next = item + 0x14 + ki * 0x68;
									f32 t = *(f32*)(anim + 0x10);
									s32 k;
									u8* ch;
									u8* chn;
									if (t > *(f32*)next) {
										if (ki == cnt - 1) {
											t = *(f32*)next;
										}
										else {
											continue;
										}
									}
									ch = prev + 4;
									chn = next + 4;
									for (k = 0; k < 5; k++, ch += 0x14, chn += 0x14) {
										if (*(s32*)(ch + 0x10) != 0) {
											*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x2C + k * 4) = *(f32*)ch;
										}
										else {
											f32 k0 = *(f32*)prev;
											f32 k1 = *(f32*)next;
											f32 d = k1 - k0;
											f32 dt = t - k0;
											f32 tan0 = *(f32*)(ch + 8);
											f32 value0 = *(f32*)ch;
											f32 tan1 = *(f32*)(chn + 4);
											f32 value1 = *(f32*)chn;
											f32 den;
											f32 u, u2, a, u3, b, c, h10, h00, h01, h11;
											s32 flag;
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
											*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x2C + k * 4) = tan0 * (d * h10) + value0 * h00 + value1 * h01 + tan1 * (d * h11);
										}
									}
									*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x40) = *(f32*)(item + 8);
									*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x44) = *(f32*)(item + 0xC);
									break;
								}
							}
						}
					}
					strcpy(namebuf, *(char**)item);
					strcat(namebuf, "_v_x");
					obj = mapSearchAnimObj(namebuf);
					if (obj != NULL) {
						{
							s32 cnt = *(s32*)(item + 0x10);
							s32 ki;
							if (cnt > 1) {
								for (ki = 1; ki < cnt; ki++) {
									u8* prev = item + 0x14 + (ki - 1) * 0x68;
									u8* next = item + 0x14 + ki * 0x68;
									f32 t = *(f32*)(anim + 0x10);
									s32 k;
									u8* ch;
									u8* chn;
									if (t > *(f32*)next) {
										if (ki == cnt - 1) {
											t = *(f32*)next;
										}
										else {
											continue;
										}
									}
									ch = prev + 4;
									chn = next + 4;
									for (k = 0; k < 5; k++, ch += 0x14, chn += 0x14) {
										if (*(s32*)(ch + 0x10) != 0) {
											*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x2C + k * 4) = *(f32*)ch;
										}
										else {
											f32 k0 = *(f32*)prev;
											f32 k1 = *(f32*)next;
											f32 d = k1 - k0;
											f32 dt = t - k0;
											f32 tan0 = *(f32*)(ch + 8);
											f32 value0 = *(f32*)ch;
											f32 tan1 = *(f32*)(chn + 4);
											f32 value1 = *(f32*)chn;
											f32 den;
											f32 u, u2, a, u3, b, c, h10, h00, h01, h11;
											s32 flag;
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
											*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x2C + k * 4) = tan0 * (d * h10) + value0 * h00 + value1 * h01 + tan1 * (d * h11);
										}
									}
									*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x40) = *(f32*)(item + 8);
									*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x44) = *(f32*)(item + 0xC);
									break;
								}
							}
						}
					}
				}
			}
			/* alpha animation */
			if (list17 != NULL) {
				u8* lp = list17;
				s32 li;
				for (li = 0; li < *(s32*)list17; li++, lp += 4) {
					u8* item = *(u8**)(lp + 4);
					u8* obj;
					strcpy(namebuf2, *(char**)item);
					obj = mapSearchAnimObj(namebuf2);
					if (obj != NULL) {
						{
							s32 cnt = *(s32*)(item + 4);
							s32 ki;
							if (cnt > 1) {
								for (ki = 1; ki < cnt; ki++) {
									u8* prev = item + 8 + (ki - 1) * 0x54;
									u8* next = item + 8 + ki * 0x54;
									f32 t = *(f32*)(anim + 0x10);
									if (t > *(f32*)next) {
										if (ki == cnt - 1) {
											t = *(f32*)next;
										}
										else {
											continue;
										}
									}
									if (*(s32*)(prev + 0x14) != 0) {
										f32 value = *(f32*)(prev + 4);
										*(u8*)(obj + 0x10C) = (s32)(255.0f - 255.0f * value);
										break;
									}
									else {
										f32 k0 = *(f32*)prev;
										f32 k1 = *(f32*)next;
										f32 d = k1 - k0;
										f32 dt = t - k0;
										f32 tan0 = *(f32*)(prev + 0xC);
										f32 value0 = *(f32*)(prev + 4);
										f32 tan1 = *(f32*)(next + 8);
										f32 value1 = *(f32*)(next + 4);
										f32 den;
										f32 u, u2, a, u3, b, c, h10, h00, h01, h11, val;
										s32 flag;
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
										val = tan0 * (d * h10) + value0 * h00 + value1 * h01 + tan1 * (d * h11);
										*(u8*)(obj + 0x10C) = (s32)(255.0f - 255.0f * val);
										break;
									}
								}
							}
						}
					}
					strcpy(namebuf2, *(char**)item);
					strcat(namebuf2, "_v");
					obj = mapSearchAnimObj(namebuf2);
					if (obj != NULL) {
						{
							s32 cnt = *(s32*)(item + 4);
							s32 ki;
							if (cnt > 1) {
								for (ki = 1; ki < cnt; ki++) {
									u8* prev = item + 8 + (ki - 1) * 0x54;
									u8* next = item + 8 + ki * 0x54;
									f32 t = *(f32*)(anim + 0x10);
									if (t > *(f32*)next) {
										if (ki == cnt - 1) {
											t = *(f32*)next;
										}
										else {
											continue;
										}
									}
									if (*(s32*)(prev + 0x14) != 0) {
										f32 value = *(f32*)(prev + 4);
										*(u8*)(obj + 0x10C) = (s32)(255.0f - 255.0f * value);
										break;
									}
									else {
										f32 k0 = *(f32*)prev;
										f32 k1 = *(f32*)next;
										f32 d = k1 - k0;
										f32 dt = t - k0;
										f32 tan0 = *(f32*)(prev + 0xC);
										f32 value0 = *(f32*)(prev + 4);
										f32 tan1 = *(f32*)(next + 8);
										f32 value1 = *(f32*)(next + 4);
										f32 den;
										f32 u, u2, a, u3, b, c, h10, h00, h01, h11, val;
										s32 flag;
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
										val = tan0 * (d * h10) + value0 * h00 + value1 * h01 + tan1 * (d * h11);
										*(u8*)(obj + 0x10C) = (s32)(255.0f - 255.0f * val);
										break;
									}
								}
							}
						}
					}
					strcpy(namebuf2, *(char**)item);
					strcat(namebuf2, "_v_x");
					obj = mapSearchAnimObj(namebuf2);
					if (obj != NULL) {
						{
							s32 cnt = *(s32*)(item + 4);
							s32 ki;
							if (cnt > 1) {
								for (ki = 1; ki < cnt; ki++) {
									u8* prev = item + 8 + (ki - 1) * 0x54;
									u8* next = item + 8 + ki * 0x54;
									f32 t = *(f32*)(anim + 0x10);
									if (t > *(f32*)next) {
										if (ki == cnt - 1) {
											t = *(f32*)next;
										}
										else {
											continue;
										}
									}
									if (*(s32*)(prev + 0x14) != 0) {
										f32 value = *(f32*)(prev + 4);
										*(u8*)(obj + 0x10C) = (s32)(255.0f - 255.0f * value);
										break;
									}
									else {
										f32 k0 = *(f32*)prev;
										f32 k1 = *(f32*)next;
										f32 d = k1 - k0;
										f32 dt = t - k0;
										f32 tan0 = *(f32*)(prev + 0xC);
										f32 value0 = *(f32*)(prev + 4);
										f32 tan1 = *(f32*)(next + 8);
										f32 value1 = *(f32*)(next + 4);
										f32 den;
										f32 u, u2, a, u3, b, c, h10, h00, h01, h11, val;
										s32 flag;
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
										val = tan0 * (d * h10) + value0 * h00 + value1 * h01 + tan1 * (d * h11);
										*(u8*)(obj + 0x10C) = (s32)(255.0f - 255.0f * val);
										break;
									}
								}
							}
						}
					}
				}
			}
			/* light animation (position/rotation) */
			if (list16 != NULL) {
				u8* lp = list16;
				s32 li;
				for (li = 0; li < *(s32*)list16; li++, lp += 4) {
					u8* item = *(u8**)(lp + 4);
					LightEntry* light = lightNameToPtr(*(char**)item);
					if (light != NULL) {
						s32 cnt = *(s32*)(item + 4);
						s32 ki;
						f32 out[9];
						if (cnt > 1) {
							for (ki = 1; ki < cnt; ki++) {
								u8* prev = item + 8 + (ki - 1) * 0xB8;
								u8* next = item + 8 + ki * 0xB8;
								f32 t = *(f32*)(anim + 0x10);
								s32 k;
								u8* ch;
								u8* chn;
								if (t > *(f32*)next) {
									if (ki == cnt - 1) {
										t = *(f32*)next;
									}
									else {
										continue;
									}
								}
								ch = prev + 4;
								chn = next + 4;
								for (k = 0; k < 9; k++, ch += 0x14, chn += 0x14) {
									if (*(s32*)(ch + 0x10) != 0) {
										out[k] = *(f32*)ch;
									}
									else {
										f32 k0 = *(f32*)prev;
										f32 k1 = *(f32*)next;
										f32 d = k1 - k0;
										f32 dt = t - k0;
										f32 tan0 = *(f32*)(ch + 8);
										f32 value0 = *(f32*)ch;
										f32 tan1 = *(f32*)(chn + 4);
										f32 value1 = *(f32*)chn;
										f32 den;
										f32 u, u2, a, u3, b, c, h10, h00, h01, h11;
										s32 flag;
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
										out[k] = tan0 * (d * h10) + value0 * h00 + value1 * h01 + tan1 * (d * h11);
									}
								}
								light->position.x = float_10_80419864 * out[0];
								light->position.y = float_10_80419864 * out[1];
								light->position.z = float_10_80419864 * out[2];
								light->rotation.x = out[3];
								light->rotation.y = out[4];
								light->rotation.z = out[5];
								break;
							}
						}
					}
				}
			}
			/* light animation (color/angle) */
			if (list15 != NULL) {
				u8* lp = list15;
				s32 li;
				for (li = 0; li < *(s32*)list15; li++, lp += 4) {
					u8* item = *(u8**)(lp + 4);
					LightEntry* light = lightNameToPtr(*(char**)item);
					if (light != NULL) {
						s32 cnt = *(s32*)(item + 4);
						s32 ki;
						f32 out[5];
						if (cnt > 1) {
							for (ki = 1; ki < cnt; ki++) {
								u8* prev = item + 8 + (ki - 1) * 0x68;
								u8* next = item + 8 + ki * 0x68;
								f32 t = *(f32*)(anim + 0x10);
								s32 k;
								u8* ch;
								u8* chn;
								if (t > *(f32*)next) {
									if (ki == cnt - 1) {
										t = *(f32*)next;
									}
									else {
										continue;
									}
								}
								ch = prev + 4;
								chn = next + 4;
								for (k = 0; k < 5; k++, ch += 0x14, chn += 0x14) {
									if (*(s32*)(ch + 0x10) != 0) {
										out[k] = *(f32*)ch;
									}
									else {
										f32 k0 = *(f32*)prev;
										f32 k1 = *(f32*)next;
										f32 d = k1 - k0;
										f32 dt = t - k0;
										f32 tan0 = *(f32*)(ch + 8);
										f32 value0 = *(f32*)ch;
										f32 tan1 = *(f32*)(chn + 4);
										f32 value1 = *(f32*)chn;
										f32 den;
										f32 u, u2, a, u3, b, c, h10, h00, h01, h11;
										s32 flag;
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
										out[k] = tan0 * (d * h10) + value0 * h00 + value1 * h01 + tan1 * (d * h11);
									}
								}
								light->color.r = (s32)(255.0f * out[0]);
								light->color.g = (s32)(255.0f * out[1]);
								light->color.b = (s32)(255.0f * out[2]);
								light->color.a = 0xFF;
								light->spotAngle = out[4];
								light->attenAngle = out[3];
								break;
							}
						}
					}
				}
			}
		}
		if (updated) {
			Mtx mtx;
			PSMTXScale(mtx, float_10_80419864, float_10_80419864, float_10_80419864);
			mapReCalcMatrix(entry->rootMapObj, mtx, 0);
		}
	}
}

// 1:1
void mapDisp(void) {
	MapWork* wp;
	MapEntry* entry;
	MapObject* obj;
	CameraEntry* cam;
	MapFileJoint* joint;
	Vec clipPos;
	Vec dispPos;
	Mtx scale;
	Mtx mtx;
	s32 j;
	s32 i;
	s32 id;
	BOOL clip;
	GXColor color;
	GXColor matColor;
	f32 order;
	f32 clipV;

	wp = &mapWork[activeGroup];
	cam = camGetPtr(CAMERA_3D);
#ifdef MUST_MATCH
	/* the original emits cmpwi, i.e. it treats unk_80418458 as signed */
	if ((s32)unk_80418458 != 0) {
#else
	if (unk_80418458 != 0) {
#endif
		return;
	}
	entry = &wp->entries[0];
	for (i = 0; i < wp->entries[0].count; entry++, i++) {
		if (entry->rootMapObj != NULL) {
			PSMTXScale(scale, float_10_80419864, float_10_80419864, float_10_80419864);
			mapReCalcMatrix(entry->rootMapObj, scale, 0);
		}
		obj = entry->objects;
		for (j = 0; j < entry->numJoints; j++, obj++) {
#define OBJ_FLAGS (*(u32*)obj->field_0x0)
			if (OBJ_FLAGS & 0x4000) continue;
			if ((OBJ_FLAGS & 0x1) && !(OBJ_FLAGS & 0x80)) continue;
			if (OBJ_FLAGS & 0x40) {
				if (obj->field_0xC[3] == 0) continue;
			}
			if (OBJ_FLAGS & 0x20000000) {
				*(u16*)(obj->field_0xEC + 4) = 0;
				*(u16*)(obj->field_0xEC + 6) = 0;
				*(u16*)(obj->field_0xEC + 0) = 0;
				*(u16*)(obj->field_0xEC + 2) = 0;
			}
			if (OBJ_FLAGS & 0x20) continue;
			if (!(OBJ_FLAGS & 0x10)) {
				if (obj->joints->partCount < 1) continue;
			}
			if (!(OBJ_FLAGS & 0x800)) {
				if ((entry->flags & 2) && entry->field_0x16[3] == 0) continue;
			}
			if (!(OBJ_FLAGS & 0x1000)) {
				if ((entry->flags & 4) && entry->field_0x16[7] == 0) continue;
			}
			PSMTXConcat(cam->view, obj->modelWorldMtx, mtx);
			if ((OBJ_FLAGS & 0x2) && !mapClipOffFlag) {
				joint = obj->joints;
#define CLIP_TEST(cx, cy, cz) \
	clipPos.x = (cx); \
	clipPos.y = (cy); \
	clipPos.z = (cz); \
	PSMTXMultVec(mtx, &clipPos, &clipPos); \
	PSMTX44MultVec(cam->projection, &clipPos, &clipPos); \
	clipV = clipPos.z; \
	if (clipV > float_neg1_804198a8 && clipV < float_0_80419850) { \
		clipV = clipPos.x; \
		if (clipV >= float_neg1_804198a8 && clipV <= float_1_80419860) { \
			clipV = clipPos.y; \
			if (clipV >= float_neg1_804198a8 && clipV <= float_1_80419860) goto visible; \
		} \
	}
				clip = FALSE;
				CLIP_TEST(joint->bboxMax.x, joint->bboxMax.y, joint->bboxMax.z)
				CLIP_TEST(joint->bboxMin.x, joint->bboxMin.y, joint->bboxMin.z)
				CLIP_TEST(joint->bboxMin.x, joint->bboxMax.y, joint->bboxMax.z)
				CLIP_TEST(joint->bboxMax.x, joint->bboxMin.y, joint->bboxMax.z)
				/* the original transforms this corner through the model matrix twice (a bug kept as is) */
				clipPos.x = joint->bboxMax.x;
				clipPos.y = joint->bboxMax.y;
				clipPos.z = joint->bboxMin.z;
				PSMTXMultVec(mtx, &clipPos, &clipPos);
				PSMTXMultVec(mtx, &clipPos, &clipPos);
				PSMTX44MultVec(cam->projection, &clipPos, &clipPos);
				clipV = clipPos.z;
				if (clipV > float_neg1_804198a8 && clipV < float_0_80419850) {
					clipV = clipPos.x;
					if (clipV >= float_neg1_804198a8 && clipV <= float_1_80419860) {
						clipV = clipPos.y;
						if (clipV >= float_neg1_804198a8 && clipV <= float_1_80419860) goto visible;
					}
				}
				CLIP_TEST(joint->bboxMin.x, joint->bboxMin.y, joint->bboxMax.z)
				CLIP_TEST(joint->bboxMax.x, joint->bboxMin.y, joint->bboxMin.z)
				CLIP_TEST(joint->bboxMin.x, joint->bboxMax.y, joint->bboxMin.z)
#undef CLIP_TEST
				clip = TRUE;
			visible:
				if (clip) continue;
			}
#undef OBJ_FLAGS
			PSMTXMultVec(mtx, (Vec*)(obj->field_0xC + 4), &dispPos);
			PSMTX44MultVec(cam->projection, &dispPos, &dispPos);
			order = 5000.0f * dispPos.z + 5000.0f;
			if (order < float_0_80419850) {
				order = float_0_80419850;
			}
			else if (order > float_10000_804198b0) {
				order = float_10000_804198b0;
			}
			order *= float_neg1_804198a8;
			id = obj->field_0x0[4];
			if (obj->joints->partCount > 0) {
				color = *(GXColor*)obj->field_0xC;
				matColor = *(GXColor*)((u8*)obj->joints->parts[0].material + 0x10C);
				if (mapTestXLU(*(u32*)obj->field_0x0, &color, &matColor)) {
					id = 2;
				}
			}
			if (*(u32*)obj->field_0x0 & 0x20000000) {
				dispEntry(CAMERA_OFFSCREEN2, float_5000_804198ac, 3, !(*(u32*)obj->field_0x0 & 0x10) ? (DispCallback)mapDispMapObj_bbox : (DispCallback)mapDispMapGrp_bbox, obj);
			}
			if (*(u32*)obj->field_0x0 & 0x80) {
				dispEntry(CAMERA_OFFSCREEN2, order, id, !(*(u32*)obj->field_0x0 & 0x10) ? (DispCallback)mapDispMapObj_off : (DispCallback)mapDispMapGrp_off, obj);
			}
			if (!(*(u32*)obj->field_0x0 & 0x1)) {
				dispEntry(obj->field_0x0[5], order, id, !(*(u32*)obj->field_0x0 & 0x10) ? (DispCallback)mapDispMapObj : (DispCallback)mapDispMapGrp, obj);
			}
		}
	}
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

// material layout (MapMaterialTex only covers 0xB/0x2C onwards)


// 1:1
void mapSetMaterialLight(s32 flags, Vec position) {
	LightEntry* lights[8];
	LightEntry* light;
	LightEntry* ambient;
	CameraEntry* cam;
	GXLightObj lightObj;
	GXColor fogColor;
	GXColor ambColor;
	GXColor chanAmbColor;
	Vec pos;
	Vec dir;
	Mtx rotX, rotY, rotZ;
#ifdef MUST_MATCH
	f32 spotAngle; /* keeps the 0.5f constant in f0 and the field in f1, as the target does */
#endif
	s32 count = 0;
	u32 lightMask;
	s32 attn = 2;
	s32 i;

	fogColor = dat_80419834;
	ambColor = dat_80419838;

	if ((u32)flags & 1) {
		lights[0] = lightGetPaper();
		count = 1;
	}
	if ((u32)flags & 8) {
		lights[count++] = lightGetPaperCraft();
	}
	if ((u32)flags & 0x10) {
		lights[count++] = lightGetPaper();
	}
	if ((u32)flags & 2) {
		if (lightCheckCharaLight() && count < 8) {
			count += LIGHT_GET_NEAR(&position, &lights[count], 8 - count, LIGHT_CHARA);
		}
	}
	if ((u32)flags & 0x19) {
		if (count == 1) {
			if ((u32)flags & 1) {
				ambColor = paper_ambient;
			}
			else if ((u32)flags & 8) {
				ambColor = paperCraft_ambient;
			}
			else {
				ambColor = dat_8041983c;
			}
		}
		else {
			count = LIGHT_GET_NEAR(&position, lights, 8, LIGHT_CHARA);
		}
	}

	lightMask = 0;
	for (i = 0; i < count; i++) {
		/* The target keeps lights[i] in a callee-saved register for the flags test, the point
		   position and the first spot rotation, but re-materializes it from the array for the
		   remaining fields.  The light/lights[i] mix below reproduces that allocation; both
		   forms are semantically identical. */
		dir = vec3_802c1828[0];
		light = lights[i];

		if (light->flags & LIGHT_POINT) {
			cam = camGetCurPtr();
			PSMTXMultVec(cam->view, &light->position, &pos);
			GXInitLightPos(&lightObj, pos.x, pos.y, pos.z);
			GXInitLightColor(&lightObj, lights[i]->color);
			if (lights[i]->flags & LIGHT_NO_DIST_ATTEN) {
				GXInitLightAttn(&lightObj, lights[i]->attenAngle, float_0_80419850, float_0_80419850,
				                float_1_80419860, float_0_80419850, float_0_80419850);
			}
			else {
				GXInitLightAttn(&lightObj, lights[i]->attenAngle, float_0_80419850, float_0_80419850,
				                lights[i]->attenDistCoef.x, lights[i]->attenDistCoef.y, lights[i]->attenDistCoef.z);
				attn = 1;
			}
			GXLoadLightObjImm(&lightObj, lightid_tbl[i]);
		}
		else if (light->flags & LIGHT_SPOT) {
			PSMTXRotRad(rotX, 'x', float_deg2rad_80419868 * light->rotation.x);
			PSMTXRotRad(rotY, 'y', float_deg2rad_80419868 * lights[i]->rotation.y);
			PSMTXRotRad(rotZ, 'z', float_deg2rad_80419868 * lights[i]->rotation.z);
			PSMTXConcat(rotZ, rotY, rotY);
			PSMTXConcat(rotY, rotX, rotX);
			PSMTXMultVec(rotX, &dir, &dir);
			PSVECScale(&dir, &dir, float_neg1p0486E06_80419870);
			PSVECAdd(&lights[i]->position, &dir, &pos);
			cam = camGetCurPtr();
			PSMTXMultVec(cam->view, &pos, &pos);
			GXInitLightPos(&lightObj, pos.x, pos.y, pos.z);
			GXInitLightColor(&lightObj, lights[i]->color);
			GXLoadLightObjImm(&lightObj, lightid_tbl[i]);
		}
		else if (lights[i]->flags & LIGHT_DIRECTIONAL) {
			cam = camGetCurPtr();
			PSMTXMultVec(cam->view, &light->position, &pos);
			PSMTXRotRad(rotX, 'x', float_deg2rad_80419868 * lights[i]->rotation.x);
			PSMTXRotRad(rotY, 'y', float_deg2rad_80419868 * lights[i]->rotation.y);
			PSMTXRotRad(rotZ, 'z', float_deg2rad_80419868 * lights[i]->rotation.z);
			PSMTXConcat(rotZ, rotY, rotY);
			PSMTXConcat(rotY, rotX, rotX);
			PSMTXMultVec(rotX, &dir, &dir);
			cam = camGetCurPtr();
			PSMTXMultVecSR(cam->view, &dir, &dir);
			GXInitLightPos(&lightObj, pos.x, pos.y, pos.z);
			GXInitLightDir(&lightObj, dir.x, dir.y, dir.z);
#ifdef MUST_MATCH
			/* split out so the field lands in f1 and 0.5f in f0, as the target does */
			spotAngle = lights[i]->spotAngle;
			GXInitLightSpot(&lightObj, spotAngle * float_0p5_80419854, 4);
#else
			GXInitLightSpot(&lightObj, lights[i]->spotAngle * float_0p5_80419854, 4);
#endif
			GXInitLightDistAttn(&lightObj, float_1000_80419874, float_0p5_80419854, 0);
			GXInitLightColor(&lightObj, lights[i]->color);
			GXLoadLightObjImm(&lightObj, lightid_tbl[i]);
			attn = 1;
		}
		lightMask |= lightid_tbl[i];
	}

	ambient = lightGetCharaAmbient();
	if (ambient != NULL) {
		chanAmbColor = ambient->color;
	}
	else {
		chanAmbColor = ambColor;
	}

	GXSetFog(GX_FOG_NONE, float_0_80419850, float_0_80419850, float_0_80419850, float_0_80419850, fogColor);
	GXSetNumChans(1);
	GXSetChanAmbColor(GX_COLOR0A0, chanAmbColor);

	if ((u32)flags & 4) {
		GXSetChanMatColor(GX_COLOR0A0, fogColor);
		GXSetChanCtrl(GX_COLOR0, count > 0, GX_SRC_REG, GX_SRC_REG, lightMask, GX_DF_CLAMP, attn);
		GXSetChanCtrl(GX_ALPHA0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_CLAMP, GX_AF_NONE);
	}
	else {
		GXSetChanCtrl(GX_COLOR0, count > 0, GX_SRC_REG, GX_SRC_VTX, lightMask, GX_DF_CLAMP, attn);
		GXSetChanCtrl(GX_ALPHA0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, 0, GX_DF_CLAMP, GX_AF_NONE);
	}
}

void mapSetMaterialLastStageBlend(s32 flags, GXColor color1, GXColor color2) {
	MapWork* wp = &mapWork[activeGroup];
	GXColor color = dat_8041981c;

	if (flags & 0x40) {
		color.r = color.r * color1.r / 255;
		color.g = color.g * color1.g / 255;
		color.b = color.b * color1.b / 255;
		color.a = color.a * color1.a / 255;
	}
	if (color2.r != 0xFF) {
		color.a = color.a * color2.r / 255;
	}
	if ((wp->entries[0].flags & 0x2) && !(flags & 0x800)) {
		color.r = color.r * wp->entries[0].field_0x16[0] / 255;
		color.g = color.g * wp->entries[0].field_0x16[1] / 255;
		color.b = color.b * wp->entries[0].field_0x16[2] / 255;
		color.a = color.a * wp->entries[0].field_0x16[3] / 255;
	}
	if ((wp->entries[0].flags & 0x4) && !(flags & 0x1000)) {
		color.r = color.r * wp->entries[0].field_0x16[4] / 255;
		color.g = color.g * wp->entries[0].field_0x16[5] / 255;
		color.b = color.b * wp->entries[0].field_0x16[6] / 255;
		color.a = color.a * wp->entries[0].field_0x16[7] / 255;
	}
	if ((wp->entries[0].flags & 0x8) && !(flags & 0x800)) {
		color.r = color.r * wp->entries[0].field_0x16[8] / 255;
		color.g = color.g * wp->entries[0].field_0x16[9] / 255;
		color.b = color.b * wp->entries[0].field_0x16[10] / 255;
		color.a = color.a * wp->entries[0].field_0x16[11] / 255;
	}
	GXSetTevKColor(GX_KCOLOR0, color);
}

void mapSetTextureMatrix(MapMaterialTex* mat) {
	s32 tc = 0;
	s32 j = 0;
	s32 i = mat->field_0xB - 1;
	Mtx mtx, mtx3, mtx5, mtx2, mtx4;

	for (; i >= 0; tc++, j++, i--) {
		f32 f1 = mat->entries[i].field_0x10;
		f32 rots = -f1;
		f32 f5 = mat->entries[i].field_0x8;
		f32 sx = f5;
		f32 f6 = mat->entries[i].field_0xC;
		f32 sy = f6;
		f32 tx = mat->entries[i].field_0x0;
		f32 f0 = mat->entries[i].field_0x4;
		f32 zero = 0.0f;
		f32 ty = -f0;

		if (mat->entries[i].field_0x8 != 1.0f || mat->entries[i].field_0xC != 1.0f || tx != 0.0f || ty != 0.0f || rots != 0.0f) {
			if (rots != zero) {
				f32 f25 = 0.5f * mat->entries[i].field_0x14 * mat->entries[i].field_0x8;
				f32 f24 = (1.0f - 0.5f * mat->entries[i].field_0x18) * mat->entries[i].field_0xC;
				PSMTXTrans(mtx, f25, f24, 0.0f);
				PSMTXRotRad(mtx2, 'z', float_deg2rad_80419868 * rots);
				PSMTXTrans(mtx3, -f25, -f24, 0.0f);
				PSMTXConcat(mtx, mtx2, mtx2);
				PSMTXConcat(mtx2, mtx3, mtx2);
			}
			if (sx != zero || sy != zero) {
				PSMTXScale(mtx4, sx, sy, 1.0f);
			}
			if (tx != zero || ty != zero) {
				PSMTXTrans(mtx5, tx, ty, 0.0f);
			}
			if (rots != zero) {
				if (sx != zero || sy != zero) {
					PSMTXConcat(mtx2, mtx4, mtx);
					if (tx != zero || ty != zero) {
						PSMTXConcat(mtx, mtx5, mtx);
					}
				}
				else {
					if (tx != zero || ty != zero) {
						PSMTXConcat(mtx2, mtx5, mtx);
					}
					else {
						PSMTXCopy(mtx2, mtx);
					}
				}
			}
			else {
				if (sx != zero || sy != zero) {
					if (tx != zero || ty != zero) {
						PSMTXConcat(mtx4, mtx5, mtx);
					}
					else {
						PSMTXCopy(mtx4, mtx);
					}
				}
				else {
					if (tx != zero || ty != zero) {
						PSMTXCopy(mtx5, mtx);
					}
				}
			}
			GXLoadTexMtxImm(mtx, texmtx_tbl[i], 1);
			GXSetTexCoordGen2(tc, 1, mat->field_0xB + 3 - j, texmtx_tbl[i], 0, 0x7D);
		}
		else {
			GXSetTexCoordGen2(tc, 1, mat->field_0xB + 3 - j, GX_IDENTITY, 0, 0x7D);
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

// vertex layout of an unpacked display list (stride 0x18)
typedef struct MapVtx {
	u32 field_0x0;   // 0x0
	s16 x;           // 0x4
	s16 y;           // 0x6
	s16 z;           // 0x8
	s16 field_0xA;   // 0xA
	s16 tex[8];      // 0xC
} MapVtx;

// per-mesh vtx-format block (the value at entry->field_0x8C + 0x18)
typedef struct MapVtxFmt {
	u8 field_0x0[0x44]; // 0x0
	u32 field_0x44;     // 0x44
	u32 field_0x48[8];  // 0x48
} MapVtxFmt;

// NONMATCHING (94.99%): register colouring only; the address computations and immediates match, but wp/pos/nrm
// land in r29/r30/r31 vs the target r31/r30/r29, and the vertex-loop pointers/indices still permute.
void mapSetPolygon(MapObject* obj, MapFileMesh* mesh) {
	MapWork* wp = &mapWork[activeGroup];
	void* nrm = (void*)(((u32*)mesh->vcdTableOffset)[1] + 4);
	void* pos = (void*)(((u32*)mesh->vcdTableOffset)[0] + 4);
	s32 texCount = ((u32*)mesh->vcdTableOffset)[5];
	s32 i;
	s32 j;
	u8* p;

	GXSetCullMode(culling[obj->joints->drawMode->cullMode]);

	if (!(wp->entries[*(s16*)(obj->field_0xDC + 2)].flags & 0x8000)) {
		GXClearVtxDesc();
		GXSetVtxAttrFmtv(GX_VTXFMT0, (const GXVtxAttrFmtList*)attr_fmt);
		GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
		GXSetArray(GX_VA_POS, pos, 0xc);
		GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
		GXSetArray(GX_VA_NRM, nrm, 0xc);
		GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
		GXSetArray(GX_VA_CLR0, (void*)(((u32*)mesh->vcdTableOffset)[3] + 4), 4);

		for (i = 0; i < texCount; i++) {
			GXSetVtxDesc(GX_VA_TEX0 + i, GX_INDEX16);
			GXSetArray(GX_VA_TEX0 + i, (void*)(((u32*)mesh->vcdTableOffset)[6 + i] + 4), 8);
		}

		{
			u8* dp = (u8*)mesh;

			for (i = 0; i < mesh->displayListCount; i++) {
				MapVtx* v = (MapVtx*)*(u32*)(dp + 0x10);
				MapVtx* vp;

				GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, (u16)v->field_0x0);
				vp = v;
				for (j = 0; j < (s32)v->field_0x0; j++) {
					s32 k;

					GXWGFifo.s16 = vp->x;
					GXWGFifo.s16 = vp->y;
					GXWGFifo.s16 = vp->z;
					for (k = 0; k < texCount; k++) {
						s16 t = vp->tex[k];
						GXWGFifo.u16 = (t != -1) ? (u16)t : 0;
					}
					vp = (MapVtx*)((u8*)vp + 0x18);
				}
				dp += 4;
			}
		}
	}
	else {
		u32 mask = mesh->elementMask;
		MapVtxFmt* fmt;

		GXClearVtxDesc();
		GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
		fmt = (MapVtxFmt*)*(void**)(wp->entries[*(s16*)(obj->field_0xDC + 2)].field_0x8C + 0x18);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, (u8)fmt->field_0x44);
		GXSetArray(GX_VA_POS, pos, 6);
		GXSetVtxDesc(GX_VA_NRM, GX_INDEX16);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
		GXSetArray(GX_VA_NRM, nrm, 3);

		for (i = 0; i < 2; i++) {
			if (!(mask & (1 << (i + 2)))) break;
			GXSetVtxDesc(GX_VA_CLR0, GX_INDEX16);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
			GXSetArray(GX_VA_CLR0, (void*)(((u32*)mesh->vcdTableOffset)[3 + i] + 4), 4);
		}

		for (i = 0; i < 8; i++) {
			if (!(mask & (1 << (i + 4)))) break;
			GXSetVtxDesc(GX_VA_TEX0 + i, GX_INDEX16);
			fmt = (MapVtxFmt*)*(void**)(wp->entries[*(s16*)(obj->field_0xDC + 2)].field_0x8C + 0x18);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0 + i, GX_TEX_ST, GX_S16, (u8)fmt->field_0x48[i]);
			GXSetArray(GX_VA_TEX0 + i, (void*)(((u32*)mesh->vcdTableOffset)[6 + i] + 4), 4);
		}

		{
			u8* dp = (u8*)mesh;

			for (i = 0; i < mesh->displayListCount; i++) {
				GXCallDisplayList((void*)*(u32*)(dp + 0x10), *(u32*)(dp + 0x14));
				dp += 8;
			}
		}
	}
}


// NONMATCHING (95.93%): GX setup order/constants and addressing match; only MWCC register coloring differs
// (target keeps wp in r31, pos in r29, nrm in r30; ours puts wp in r29, pos in r25, nrm in r30, and the
// first material load folds to lwzx instead of lwz with 0xA4 displacement).
void mapSetPolygonVtxDesc(MapObject* obj, MapFileMesh* mesh) {
	MapWork* wp = &mapWork[activeGroup];
	u32 mask = mesh->elementMask;
	void* pos = (void*)(((u32*)mesh->vcdTableOffset)[0] + 4);
	void* nrm = (void*)(((u32*)mesh->vcdTableOffset)[1] + 4);
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
		GXSetArray(GX_VA_CLR0, (void*)(((u32*)mesh->vcdTableOffset)[3 + i] + 4), 4);
	}

	for (i = 0; i < 8; i++) {
		if (!(mask & (1 << (i + 4)))) {
			break;
		}
		GXSetVtxDesc(GX_VA_TEX0 + i, GX_INDEX16);
		material = *(u8**)(wp->entries[*(s16*)(obj->field_0xDC + 2)].field_0x8C + 0x18);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0 + i, GX_TEX_ST, GX_S16, (u8)*(u32*)(material + 0x48 + i * 4));
		GXSetArray(GX_VA_TEX0 + i, (void*)(((u32*)mesh->vcdTableOffset)[6 + i] + 4), 4);
	}
}

// Routing the inlined mapSetMaterialLastStageBlend through this wrapper reproduces the
// original's register allocation (flags in r3, activeGroup in r4) and blend temp layout.
static inline void mapBlendSet(s32 flags, MapObject* obj, MapMaterialTex* material) {
	mapSetMaterialLastStageBlend(flags, *(GXColor*)obj->field_0xC, *(GXColor*)((u8*)material + 0x10C));
}

// NONMATCHING (99.98%): instruction-for-instruction except the by-value GXColor call temps.
// The target's slots (fog 0x18/1c, loop2 0x20/24, loop1 0x28/2c) need the blend expanded as a
// macro, which swaps flags/activeGroup; the inline wrapper fixes the registers but rotates them.
void _mapDispMapObj(CameraId camId, MapObject* obj) {
	CameraEntry* cam;
	Mtx mtx;
	Mtx invMtx;
	GXCullMode cull;
	s32 i;
	s32 j;
	u8* p;
	s32 kind;

	cam = camGetPtr(camId);
	current_mp = obj;

	if (*(u32*)obj->field_0x0 & 0x20) {
		if (*(u32*)obj->field_0x0 & 0x8000) {
			f32 proj[7];
			MapFileJoint* root;
			s32 index;
			f32 t;
			f32 fNear;
			f32 fFar;
			f32 f0;
			f32 f1;
			f32 f2;
			f32 f3;
			f32 f4;

			root = obj->joints;
			index = 0;
			while (root->prev) {
				root = root->prev;
			}
			{
				MapFileJoint* p = root;

				while (p != obj->joints) {
					p = p->next;
					index++;
				}
			}
			t = float_0p01_804198b4 * (f32)index;
			GXGetProjectionv(proj);
			f0 = float_neg2_804198a0;
			f3 = float_1_80419860;
			{
				f32 nn = cam->near;
				f32 sum = cam->far + nn;
				f32 t2 = f0 * cam->far;
				f32 pr = t2 * nn;

				proj[5] = proj[5] * (f3 + t * pr / (f3 * sum * (f3 + t)));
			}
			GXSetProjectionv(proj);
		}

		kind = obj->field_0x0[4];
		if (obj->joints->partCount > 0) {
			GXColor color = *(GXColor*)obj->field_0xC;
			GXColor matColor = *(GXColor*)((u8*)obj->joints->parts[0].material + 0x10C);

			if (mapTestXLU(*(u32*)obj->field_0x0, &color, &matColor)) {
				kind = 2;
			}
		}
		switch (kind) {
		case 0:
			GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_OR);
			GXSetZCompLoc(GX_TRUE);
			GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
			GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
			break;
		case 3:
			GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
			GXSetZCompLoc(GX_TRUE);
			GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
			GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
			break;
		case 1:
			GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
			GXSetZCompLoc(GX_FALSE);
			GXSetAlphaCompare(GX_GEQUAL, 0x80, GX_AOP_OR, GX_NEVER, 0);
			GXSetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
			break;
		case 4:
			GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_CLEAR);
			GXSetZCompLoc(GX_FALSE);
			GXSetAlphaCompare(GX_GEQUAL, 0x80, GX_AOP_OR, GX_NEVER, 0);
			GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
			break;
		case 5:
		default:
			GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
			GXSetZCompLoc(GX_TRUE);
			GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
			GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
			break;
		}
	}

	if (camId == 1) {
		GXSetZCompLoc(GX_FALSE);
	}

	PSMTXConcat(cam->view, obj->modelWorldMtx, mtx);
	GXLoadPosMtxImm(mtx, GX_PNMTX0);
	if (!(*(u32*)obj->field_0x0 & 0x100)) {
		PSMTXInvXpose(mtx, invMtx);
		GXLoadNrmMtxImm(invMtx, GX_PNMTX0);
	}
	GXSetCurrentMtx(GX_PNMTX0);
	GXGetCullMode(&cull);
	if (cull != culling[obj->joints->drawMode->cullMode]) {
		GXSetCullMode(culling[obj->joints->drawMode->cullMode]);
	}
	{
		CameraEntry* cur = camGetCurPtr();
		MapEntry* entry = &mapWork[activeGroup].entries[0];

		if ((entry->flags & 0x1) && !(*(u32*)obj->field_0x0 & 0x80000)) {
			GXSetFog(fog_type[*(s32*)((u8*)entry + 0x28)], *(f32*)((u8*)entry + 0x2C), *(f32*)((u8*)entry + 0x30), cur->near, cur->far, *(GXColor*)((u8*)entry + 0x34));
		}
		else {
			GXSetFog(GX_FOG_NONE, float_0_80419850, float_0_80419850, float_0_80419850, float_0_80419850, dat_80419824);
		}
	}

	if (!(*(u32*)obj->field_0x0 & 0x20) || !((*(u32*)obj->field_0x0 & 0x4000) || ((*(u32*)obj->field_0x0 & 0x1) && !(*(u32*)obj->field_0x0 & 0x80)) || ((*(u32*)obj->field_0x0 & 0x40) && obj->field_0xC[3] == 0))) {
		if (*(u32*)obj->field_0x0 & 0x100) {
			for (i = 0; i < obj->joints->partCount; i++) {
				MapMaterialTex* material = obj->joints->parts[i].material;
				MapFileMesh* mesh = obj->joints->parts[i].mesh;

				mapSetTextureMatrix(material);
				mapBlendSet(*(u32*)obj->field_0x0, obj, material);
				mapSetPolygonVtxDesc(obj, mesh);
				GXCallDisplayList(*(void**)(obj->field_0xEC + 8 + i * 4), *(u32*)(obj->field_0xEC + 0x28 + i * 4));
				{
					p = (u8*)mesh;
					for (j = 0; j < mesh->displayListCount; j++) {
						GXCallDisplayList(*(void**)(p + 0x10), *(u32*)(p + 0x14));
						p += 8;
					}
				}
			}
		}
		else {
			for (i = 0; i < obj->joints->partCount; i++) {
				MapMaterialTex* material = obj->joints->parts[i].material;
				MapFileMesh* mesh = obj->joints->parts[i].mesh;

				mapSetTextureMatrix(material);
				mapBlendSet(*(u32*)obj->field_0x0, obj, material);
				mapSetMaterial(obj, material);
				mapSetPolygon(obj, mesh);
			}
		}
	}

	if (*(u32*)obj->field_0x0 & 0x8000) {
		GXSetProjection(cam->projection, cam->projType);
	}
	if (*(u32*)obj->field_0x0 & 0x0F000000) {
		GXSetTevDirect(GX_TEVSTAGE0);
		GXSetTevDirect(GX_TEVSTAGE1);
		GXSetTevDirect(GX_TEVSTAGE2);
		GXSetTevDirect(GX_TEVSTAGE3);
		GXSetTevDirect(GX_TEVSTAGE4);
		GXSetNumIndStages(0);
	}
}
void _mapDispMapGrp(CameraId camId, MapObject* obj) {
	_mapDispMapObj(camId, obj);
	if (obj->child) _mapDispMapGrp(camId, obj->child);
	if ((*(u32*)obj->field_0x0 & 0x20) && obj->next) _mapDispMapGrp(camId, obj->next);
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

// NONMATCHING (90.01%): field write must be a real GXColor struct copy and each `a3 && next` level needs its own
// fresh GXColor copy (loaded from *color) while `child` passes the parent's &c. Remaining diff is scheduling:
// target stores each level's copy just before the out-of-line bl, we store early (r0/r3 swap).
void setColor(MapObject* obj, GXColor* color, s32 a3) {
	GXColor c;
	if (obj == 0) return;
	c = *color;
	*(GXColor*)&obj->field_0xC = c;
	*(u32*)obj->field_0x0 |= 0x40;
	if (obj->child) setColor(obj->child, &c, 1);
	if (a3 && obj->next) { GXColor d = *color; setColor(obj->next, &d, 1); }
}

void* mapSearchDmdJointSub(void* a, void* b) {
	MapFileJoint* joint = (MapFileJoint*)a;
	void* result;

	if (!strcmp(joint->name, (const char*)b)) return joint;
	if (joint->child && (result = mapSearchDmdJointSub(joint->child, b))) return result;
	if (joint->next && (result = mapSearchDmdJointSub(joint->next, b))) return result;
	return NULL;
}

s32 mapSetLight(MapObject* obj, LightEntry** entries) {
	Vec pos;
	s32 count = 0;
	s32 i;

	PSMTXMultVec(obj->modelWorldMtx, (Vec*)(obj->field_0xC + 4), &pos);

	if (obj->joints->drawMode->unk2[0] & 0x8) {
		if (*(u32*)(obj->joints->drawMode->unk2 + 2) == 0) {
			count = LIGHT_GET_NEAR(&pos, entries, 8, 0);
		}
		else {
			for (i = 0; i < 0x20; i++) {
				if (*(u32*)(obj->joints->drawMode->unk2 + 2) & (1 << i)) {
					MapEntry* entry = &mapWork[activeGroup].entries[*(s16*)(obj->field_0xDC + 2)];
					void* list = *(void**)(entry->field_0x8C + 0x0C);
					LightEntry* l = lightNameToPtr(*(const char**)*(void**)((u8*)list + i * 4 + 4));

					if (!(l->flags & LIGHT_AMBIENT)) {
						entries[count] = l;
						count++;
						if (count >= 8) break;
					}
				}
			}
			if (count < 8) count += LIGHT_GET_NEAR(&pos, entries + count, 8 - count, 0);
		}
	}
	else {
		if (*(u32*)(obj->joints->drawMode->unk2 + 2) != 0) {
			for (i = 0; i < 0x20; i++) {
				if (*(u32*)(obj->joints->drawMode->unk2 + 2) & (1 << i)) {
					MapEntry* entry = &mapWork[activeGroup].entries[*(s16*)(obj->field_0xDC + 2)];
					void* list = *(void**)(entry->field_0x8C + 0x0C);
					LightEntry* l = lightNameToPtr(*(const char**)*(void**)((u8*)list + i * 4 + 4));

					if (!(l->flags & LIGHT_AMBIENT)) {
						entries[count] = l;
						count++;
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
