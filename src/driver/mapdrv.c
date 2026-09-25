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

extern f32 queen_uv[4]; //gxsub.c
extern GlobalWork* gp;
extern int sprintf(char* str, const char* format, ...);

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
extern f32 float_0_80419850;
extern f32 float_1000_80419874;
extern f32 float_10_80419864;
extern f32 float_deg2rad_80419868;
extern const char str_8041986C[];

//.sdata
f32 scrl_uv[4] = {0.0f, 0.0f, 0.0f, 0.0f};
s32 fog_type[5] = {2, 4, 5, 6, 7};

//forward decls for functions defined later in this file
void mapGrpFlagOff(const char* name, u32 flag);
void mapGrpFlagOn(const char* name, u32 flag);
void mapFlgOff(MapObject* obj, u32 flag, s32 a3);
void mapFlgOn(MapObject* obj, u32 flag, s32 a3);
void setColor(MapObject* obj, GXColor* color, s32 a3);
void mapSetPolygon(MapObject* obj, s32 a2);
void _mapDispMapGrp(MapObject* obj, void* a2);
void _mapDispMapObj(MapObject* obj, void* a2);
void _mapDispMapGrp_NoMaterial(MapObject* obj, void* a2);
void _mapDispMapObj_NoMaterial(MapObject* obj, void* a2);
void _setOffScrnId(MapObject* obj, s32 id, s32 a3);
s32 offscreenNameToId(const char* name);
void mapSearchDmdJointSub(void* a, void* b);
void mapSearchDmdJointSub2(void* a, void* b, const char* c);
void mapGrpSetColor(const char* name, GXColor* color);

void mapGrpSetFlushColor(const char* name, u8 r, u8 g, u8 b, u8 a) {
	GXColor color;

	color = *(GXColor*)&unk_80422f90;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	mapGrpSetColor(name, &color);
	mapGrpFlagOff(name, 0x40);
}

void mapSearchDmdJoint(void* a, char* b) {
	if (*(u8*)b == 0x7C) {
		mapSearchDmdJointSub2(a, b, str_8041986C);
	}
	else {
		mapSearchDmdJointSub(a, b);
	}
}

void mapDispMapGrp_bbox(MapObject* obj, MapEntry* entry) {
	sysWaitDrawSync();
	GXClearBoundingBox();
	_mapDispMapGrp_NoMaterial(obj, entry);
	sysWaitDrawSync();
	GXReadBoundingBox((u16*)((u8*)entry + 0xEC), (u16*)((u8*)entry + 0xEE), (u16*)((u8*)entry + 0xF0), (u16*)((u8*)entry + 0xF2));
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

void mapGrpSetColor(const char* name, GXColor* color) {
	GXColor c = *color;

	setColor(mapGetMapObjSub(name), &c, 0);
}

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

	*(s16*)((u8*)obj + 0xDC) = -1;
	*(u32*)obj->field_0x0 &= ~0x80;
}

void mapGrpSetOffScreen(const char* name, const char* offName) {
	_setOffScrnId(mapGetMapObjSub(name), offscreenNameToId(offName), 0);
}

void mapObjSetOffScreen(const char* name, const char* offName) {
	MapObject* obj = mapGetMapObjSub(name);

	*(s16*)((u8*)obj + 0xDC) = offscreenNameToId(offName);
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

void test_kururing_mapdisp(MapObject* arg) {
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
				_mapDispMapObj(arg, obj);
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

void mapPauseAnimation(const char* name) {
	MapEntryAnimData* found;
	s32 i;
	s32 j;
	MapWork* wp;
	MapEntry* entry;

	wp = &mapWork[activeGroup];
	entry = &wp->entries[0];
	for (i = 0; i < wp->entries[0].count; entry++, i++) {
		found = entry->animData;
		for (j = 0; j < *(s32*)entry->field_0x160; j++, found++) {
			if (!(*(u16*)found & 0x1)) {
				continue;
			}
			if (strcmp(*(char**)*(void**)((u8*)found + 0x18), name)) {
				continue;
			}
			goto found;
		}
	}
	found = NULL;
found:
	if (found) {
		*(u16*)found |= 0x10;
	}
}

void mapReStartAnimation(const char* name) {
	MapEntryAnimData* found;
	s32 i;
	s32 j;
	MapWork* wp;
	MapEntry* entry;

	wp = &mapWork[activeGroup];
	entry = &wp->entries[0];
	for (i = 0; i < wp->entries[0].count; entry++, i++) {
		found = entry->animData;
		for (j = 0; j < *(s32*)entry->field_0x160; j++, found++) {
			if (!(*(u16*)found & 0x1)) {
				continue;
			}
			if (strcmp(*(char**)*(void**)((u8*)found + 0x18), name)) {
				continue;
			}
			goto found;
		}
	}
	found = NULL;
found:
	if (found) {
		*(u16*)found |= 0x20;
	}
}

void mapSetPlayRate(const char* name, f32 rate) {
	MapEntryAnimData* found;
	s32 i;
	s32 j;
	MapWork* wp;
	MapEntry* entry;

	wp = &mapWork[activeGroup];
	entry = &wp->entries[0];
	for (i = 0; i < wp->entries[0].count; entry++, i++) {
		found = entry->animData;
		for (j = 0; j < *(s32*)entry->field_0x160; j++, found++) {
			if (!(*(u16*)found & 0x1)) {
				continue;
			}
			if (strcmp(*(char**)*(void**)((u8*)found + 0x18), name)) {
				continue;
			}
			goto found;
		}
	}
	found = NULL;
found:
	if (found) {
		*(f32*)((u8*)found + 0x14) = rate;
	}
}

//local prototypes
void mapGetJointsSub(MapFileJoint* joint, s32* numJoints);
void mapGetBoundingBox(Vec min, Vec max);
void mapBuildTexture(MapObject* obj, TEXPalette* texture, void* table);
MapObject* _mapEnt(MapFileJoint* joint, MapObject* parent, Mtx mtx, s32 index);
MapObject* mapEntrySub(MapFileJoint* joint, MapObject* parent, Mtx mtx, BOOL isRoot, s32 index);
MapObject* mapEntry(MapFileJoint* joint, Mtx mtx, s32 index);
void makeDisplayList(BOOL group);
//anim functions?
void _mapLoad(MapWork* wp, BOOL group, const char* map);

void mapErrorEntry(s32 arg0, const char* arg1) { //1:1
	MapError* error = error_data;

	error += error_count;
	error->field_0x20 = arg0;
	strncpy(error->field_0x0, arg1, sizeof(error->field_0x0) - 1);
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

void mapSetFog(u32 color, f32 near, f32 far, u32* color2) {
	MapEntry* entry = &mapWork[activeGroup].entries[0];
	u32 c2 = *color2;

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

void mapDispMapGrp(MapObject* obj, void* a2) {
	_mapDispMapGrp(obj, a2);
}

void mapDispMapObj(MapObject* obj, void* a2) {
	_mapDispMapObj(obj, a2);
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

void mapDispMapObj_off(MapObject* obj, void* a2) {
	u16 v0;
	u16 v1;
	u16 v2;
	u16 v3;

	sysWaitDrawSync();
	GXClearBoundingBox();
	_mapDispMapObj(obj, a2);
	sysWaitDrawSync();
	GXReadBoundingBox(&v0, &v1, &v2, &v3);
	offscreenAddBoundingBox(*(s16*)((u8*)a2 + 0xDC), v0, v1, v2, v3);
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

void mapBuildTexture(MapObject* obj, TEXPalette* texture, void* table) {
	int i;
	MapFileJointPart* part;

	for (i = 0; i < obj->joints->partCount; i++) {
		part = &obj->joints->parts[i];
	}
	if (obj->child) {
		mapBuildTexture(obj->child, texture, table);
	}
	if (obj->next) {
		mapBuildTexture(obj->next, texture, table);
	}
}

MapObject* _mapEnt(MapFileJoint* joint, MapObject* parent, Mtx mtx, s32 index) {
	return NULL;




}

//recursively inlined
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

	wp = &mapWork[activeGroup];

	wp->entries[index].numJoints = mapGetJoints(joint);
	size = OSRoundUp32B(wp->entries[index].numJoints * 0x134);
	wp->entries[index].objects = _mapAlloc(size);
	memset(wp->entries[index].objects, 0, size);
	return mapEntrySub(joint, NULL, mtx, TRUE, index);
}

void makeDisplayList(BOOL group) {

}

MapEntryAnimData* mapSearchAnmObj(const char* animName) {
	MapWork* wp = &mapWork[activeGroup];
	MapEntry* entry = &wp->entries[0];
	s32 j;
	s32 i;

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

void mapCheckAnimation(const char* animName, BOOL* valid, f32* a3) {
	MapWork* wp = &mapWork[activeGroup];
	MapEntry* entry = &wp->entries[0];
	MapEntryAnimData* found;
	s32 j;
	s32 i;

	for (i = 0; i < wp->entries[0].count; entry++, i++) {
		found = entry->animData;
		for (j = 0; j < *(s32*)entry->field_0x160; j++, found++) {
			if ((*(u16*)found & 0x1) && !strcmp(*(char**)*(void**)((u8*)found + 0x18), animName)) {
				goto found;
			}
		}
	}
	found = NULL;
found:
	*valid = FALSE;
	*a3 = float_0_80419850;
	if (found) {
		if (*(u16*)found & 0x2) {
			*valid = TRUE;
		}
		*a3 = 1000.0f * (*(f32*)(*(u8**)((u8*)found + 0x18) + 8) - *(f32*)((u8*)found + 0x10)) / (f32)gp->retraceCount;
	}
	else {
		*valid = TRUE;
	}
}

//more animation functions

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

extern void _mapFree(void* alloc);
extern void hitReInit(void);
extern void lightReInit(void);
extern void envReInit(void);
extern void camUnLoadRoad(CameraId cameraId);
extern void camSetTypePersp(CameraId cameraId);
extern BOOL arcDataCheck(void* data);
extern void arcDelete(s32 id);
extern f32 float_0_804198cc;

void mapUnLoad(void) {
	MapEntry* entry;
	s32 i;
	MapWork* wp;

	sysWaitDrawSync();
	wp = mapWork;
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
			*(s32*)((u8*)entry + 0x160) = 0;
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
		if (*(void**)((u8*)entry + 0x8C)) {
			_mapFree(*(void**)((u8*)entry + 0x8C));
			*(void**)((u8*)entry + 0x8C) = NULL;
		}
		strncpy(entry->name, (char*)&float_0_804198cc, 0x10);
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
	wp = mapWork;
	activeGroup = 0;
	memset(wp, 0, sizeof(MapWork));
	_mapLoad(wp, 0, map);
	wp->entries[0].count = 1;
}






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






void mapMain(void) {

}

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





void mapSetMaterialLight(s32 flags, Vec position) {

}

void mapSetMaterialLastStageBlend(s32 flags, GXColor color1, GXColor color2) {

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
