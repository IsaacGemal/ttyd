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
#include "math_ext.h"
#include <string.h>
#include "driver/hitdrv.h"
#include "driver/lightdrv.h"
#include "driver/envdrv.h"
#include "manager/arammgr.h"
#include "matching.h"
#include <stdio.h>
#include <dolphin/os.h>
#include "driver/shadowdrv.h"

#pragma pool_data on // the original addresses its .rodata through one base register

// Original reaches queen_uv with absolute @ha/@l addressing, not the .sdata sda21 form.
__declspec(section ".rodata") extern f32 queen_uv[4]; //gxsub.c
extern GlobalWork* gp;

#define TWO_PI 6.28318530717958647692528676655900576839433879875021f

//.bss
MapError error_data[32];

//.sbss (MWCC lays these out in reverse declaration order)
GXColor paper_ambient;
GXColor paperCraft_ambient;
GXTexObj projTexObj;
void* projTexImgRef;
Mtx projMtx;
BOOL __mapdrv_make_dl;
BOOL mapDispDisabled;
s32 error_count;
BOOL error_flag;
MapObject* current_mp;
BOOL mapClipOffFlag;
s32 activeGroup;
MapWork* mapWork; //allocates two entries


//forward decls for functions defined later in this file
static MapObject* mapGetMapObjSub(const char* name);



// the TEV-stage callback slots at mapWork[activeGroup]+0x110 (16 entries)

// The CMPR texture the 0x800000 material pass samples (4x64).
static u8 texture_0[0x100] ATTRIBUTE_ALIGN(32) = {
	0xEB, 0x88, 0xE2, 0x28, 0x55, 0xFF, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xF5, 0x28, 0xEC, 0x08, 0x55, 0xFF, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xFE, 0xE8, 0xF5, 0xA8, 0x55, 0xFF, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xFF, 0x47, 0xD6, 0xE8, 0x00, 0xAA, 0xFF, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xBE, 0xA8, 0x86, 0x0A, 0x00, 0xAA, 0xFF, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x7D, 0xEA, 0x0D, 0x8B, 0x00, 0xAA, 0xFF, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x05, 0x6B, 0x05, 0x4A, 0x00, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x05, 0x6B, 0x05, 0x4E, 0x00, 0x00, 0xAA, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x05, 0xBA, 0x05, 0x70, 0x55, 0xFF, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x05, 0xDE, 0x05, 0xBA, 0x55, 0xFF, 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x05, 0xBD, 0x05, 0x1B, 0x00, 0x00, 0xAA, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x13, 0x76, 0x04, 0x9A, 0x55, 0xFF, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x33, 0x15, 0x42, 0x53, 0x00, 0xAA, 0x55, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x6A, 0x33, 0x3A, 0x53, 0xFF, 0xFF, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xB1, 0xF2, 0x82, 0x12, 0x55, 0xFF, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xE9, 0xF2, 0xC2, 0x12, 0x55, 0xFF, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};


extern int __float_nan[];
#define SPLINE_NAN (*(f32*)__float_nan)
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
typedef struct MapVtx {
	u32 field_0x0;   // 0x0
	s16 x;           // 0x4
	s16 y;           // 0x6
	s16 z;           // 0x8
	s16 field_0xA;   // 0xA
	s16 tex[8];      // 0xC
} MapVtx;
typedef struct MapVtxFmt {
	u8 field_0x0[0x44]; // 0x0
	u32 field_0x44;     // 0x44
	u32 field_0x48[8];  // 0x48
} MapVtxFmt;

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
	mapDispDisabled = TRUE;
}

void N_mapDispOn(void) {
	mapDispDisabled = FALSE;
}

MapWork* mapGetWork(void) {
	return &mapWork[activeGroup];
}

s32 mapGetActiveGroup(void) {
	return activeGroup;
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
	*min = (Vec){0.0f, 0.0f, 0.0f};
	*max = (Vec){0.0f, 0.0f, 0.0f};
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

// The table walk is an inline whose found path is a `return`, which makes the deferred inliner emit the
// `bne increment; b found` split instead of a single `beq found`.
static inline s32 mapSearchTextureIdx(void* table, u8** ref) {
	const char** p;
	const char* name;
	s32 idx;

	name = *(const char**)(*ref);
	idx = 0;
	p = (const char**)table;
	while (idx < *(s32*)table) {
		if (strcmp(p[1], name) == 0) {
			return idx;
		}
		p++;
		idx++;
	}
	return -1;
}
void mapBuildTexture(MapObject* obj, TEXPalette* texture, void* table) {
	int i;
	int j;

	for (i = 0; i < obj->joints->partCount; i++) {
		MapMaterial* material = obj->joints->parts[i].material;

		for (j = 0; j < material->texCount; j++) {
			u8** ref = (u8**)material->texRefs[j];
			s32 idx;
			s32 use;
			TEXDescriptor* desc;

			idx = mapSearchTextureIdx(table, ref);
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
	MapMatTexRef* ref;
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

	obj->flags = 0;
	obj->joints = joint;
	obj->entryIdx = index;

	PSMTXTrans(transMtx, joint->translation.x, joint->translation.y, joint->translation.z);
	PSMTXScale(scaleMtx, joint->scale.x, joint->scale.y, joint->scale.z);
	PSMTXRotRad(rotMtxX, 'x', 0.017453292f * joint->rotation.x);
	PSMTXRotRad(rotMtxY, 'y', 0.017453292f * joint->rotation.y);
	PSMTXRotRad(rotMtxZ, 'z', 0.017453292f * joint->rotation.z);
	PSMTXConcat(transMtx, rotMtxZ, transMtx);
	PSMTXConcat(transMtx, rotMtxY, transMtx);
	PSMTXConcat(transMtx, rotMtxX, transMtx);
	PSMTXConcat(transMtx, scaleMtx, obj->localMtx);
	PSMTXConcat(mtx, obj->localMtx, obj->modelWorldMtx);
	PSMTXIdentity(obj->unkAC);

	obj->flags |= MAPOBJ_FLAG_DIRTY;
	obj->cameraId = 4;
	obj->offScreenId = -1;
	obj->parent = parent;
	obj->next = NULL;
	obj->child = NULL;

	if (joint->drawMode != NULL) {
	if (joint->drawMode->unk2 & 1) {
		obj->flags |= 0x8;
	}
	switch (joint->drawMode->unk0) {
	case 0:
		count2 = 0;
		count1 = 0;
		for (i = 0; i < obj->joints->partCount; i++) {
			if (obj->joints->parts[i].material->field_0xA == 2) count2++;
			else if (obj->joints->parts[i].material->field_0xA == 1) count1++;
			for (j = 0; j < 8; j++) {
				ref = obj->joints->parts[i].material->texRefs[j];
				if (ref == NULL) break;
				if (ref->field_0xA == 2) count2++;
				else if (ref->field_0xA == 1) count1++;
			}
		}
		if (count2) obj->renderMode = 2;
		else if (count1) obj->renderMode = 1;
		else obj->renderMode = 0;
		break;
	case 1:
		mode = 0;
		if (obj->flags & 0x8) mode = 3;
		obj->renderMode = mode;
		break;
	case 2:
		mode = 1;
		if (obj->flags & 0x8) mode = 4;
		obj->renderMode = mode;
		break;
	case 3:
		mode = 2;
		if (obj->flags & 0x8) mode = 5;
		obj->renderMode = mode;
		break;
	}

	if (joint->drawMode->unk2 & 0x2) {
		obj->flags |= MAPOBJ_FLAG_GROUP;
		if (joint->drawMode->unk2 & 0x10) {
			obj->flags |= 0x8000;
		}
		switch (joint->drawMode->unk0) {
		case 0: obj->renderMode = 2; break;
		case 1: obj->renderMode = 0; break;
		case 2: obj->renderMode = 1; break;
		case 3:
		default: obj->renderMode = 2; break;
		}
	}

	if (obj->flags & MAPOBJ_FLAG_GROUP) {
		if (parent != NULL && ((parent->flags & MAPOBJ_FLAG_GROUP) || (parent->flags & MAPOBJ_FLAG_IN_GROUP))) {
			obj->flags &= ~MAPOBJ_FLAG_GROUP;
		}
	}
	else if (parent != NULL && ((parent->flags & MAPOBJ_FLAG_GROUP) || (parent->flags & MAPOBJ_FLAG_IN_GROUP))) {
		obj->flags |= MAPOBJ_FLAG_IN_GROUP;
		if (parent->flags & 0x8000) {
			obj->flags |= 0x8000;
		}
	}

	if (joint->drawMode->unk2 & 0x4) {
		obj->flags |= 0x4;
	}
	}
	else {
		OSPanic("mapdrv.c", 741, "\203W\203\207\203C\203\223\203g\202\314\203f\201[\203^\202\314\216w\222\350\202\311\202\250\202\251\202\265\202\310\202\340\202\314\202\252\202\240\202\351\n");
	}

	if (joint->partCount) {
		PSVECAdd(&joint->bboxMin, &joint->bboxMax, &obj->center);
		PSVECScale(&obj->center, &obj->center, 0.5f);
	}
	else {
		obj->center = joint->translation;
	}

	if (joint->partCount) {
		PSMTXMultVec(obj->modelWorldMtx, &obj->center, &vecA);
		PSMTXMultVec(obj->modelWorldMtx, &joint->bboxMax, &vecB);
		if (PSVECSquareDistance(&vecA, &vecB) < 20000.0f) {
			obj->flags |= MAPOBJ_FLAG_CLIP;
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

// `wp = mapGetWork()` (same value as &mapWork[activeGroup]) moves wp into r31 and lets the cached
// entry pointer be dropped.
void makeDisplayList(BOOL group) {
	static const s32 culling[] = {GX_CULL_FRONT, GX_CULL_BACK, GX_CULL_ALL, GX_CULL_NONE};
	MapWork* wp;
	void* buf;
	s32 i;
	s32 j;
	u8* p;
	s32 pass;
	s32 total;
	u8* dest;
	MapObject* obj;

	wp = mapGetWork();
	buf = _mapAlloc(0x10000);
	__mapdrv_make_dl = TRUE;
	total = 0;
	for (pass = 0; pass < 2; pass++) {
		obj = wp->entries[group].objects;
		for (i = 0; i < 8; i++) {
			obj->dlBuf[i] = NULL;
			obj->dlSize[i] = 0;
		}
		for (i = 0; i < wp->entries[group].numJoints; i++, obj++) {
			if (obj->joints->partCount < 1 || obj->joints->partCount > 8) {
				continue;
			}
			if (obj->joints->drawMode->unk2 & 0x8) {
				continue;
			}
			if (obj->joints->drawMode->unk4 != 0) {
				continue;
			}
			if (shadowGetCharShadowProjection() != 0) {
				if (obj->flags & 0x4) {
					continue;
				}
			}
			if (!(wp->entries[group].flags & 0x8000)) {
				continue;
			}
			for (j = 0; j < obj->joints->partCount; j++) {
				MapMaterial* material = obj->joints->parts[j].material;

				memset(buf, 0, 0x10000);
				DCInvalidateRange(buf, 0x10000);
				GXBeginDisplayList(buf, 0x10000);
				GXResetWriteGatherPipe();
				mapSetMaterial(obj, material);
				GXSetCullMode(culling[obj->joints->drawMode->cullMode]);
				obj->dlSize[j] = GXEndDisplayList();
				if (pass == 0) {
					total += obj->dlSize[j];
				}
				else {
					obj->dlBuf[j] = dest;
					dest += obj->dlSize[j];
					memcpy(obj->dlBuf[j], buf, obj->dlSize[j]);
					DCFlushRange(obj->dlBuf[j], obj->dlSize[j]);
				}
			}
		}
		if (pass == 0) {
			wp->entries[group].dlTotal = total;
			if (wp->entries[group].dlTotal != 0) {
				wp->entries[group].dlData = _mapAllocTail(wp->entries[group].dlTotal);
				dest = wp->entries[group].dlData;
			}
			else {
				wp->entries[group].dlData = NULL;
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
		for (j = 0; j < entry->animCount; j++, anim++) {
			if ((anim->flags & 0x1) && !strcmp(anim->data->name, animName)) {
				return anim;
			}
		}
	}
	return NULL;
}

// Local copy of mapSearchAnmObj for the animation callers, with `entry` declared before `wp`: the inliner
// allocates the helper's locals in declaration order, which puts wp in the higher register (r28).
static inline MapEntryAnimData* mapFindAnm(const char* animName) {
	MapEntry* entry;
	MapWork* wp;
	s32 j;
	s32 i;

	wp = &mapWork[activeGroup];
	entry = &wp->entries[0];
	for (i = 0; i < wp->entries[0].count; entry++, i++) {
		MapEntryAnimData* anim = entry->animData;
		for (j = 0; j < entry->animCount; j++, anim++) {
			if ((anim->flags & 0x1) && !strcmp(anim->data->name, animName)) {
				return anim;
			}
		}
	}
	return NULL;
}

void mapCheckAnimation(const char* animName, BOOL* valid, f32* a3) {
	MapEntryAnimData* found = mapFindAnm(animName);

	*valid = FALSE;
	*a3 = 0.0f;
	if (found) {
		if (found->flags & 0x2) {
			*valid = TRUE;
		}
		*a3 = 1000.0f * (found->data->field_0x8 - found->frame) / (f32)gp->framerate;
	}
	else {
		*valid = TRUE;
	}
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
		for (j = 0; j < entry->animCount; j++, found++) {
			if (!strcmp(found->data->name, name)) {
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

	found->flags = 0;
	switch (a3) {
	case 0:
		break;
	case 1:
		found->flags |= 0x100;
		break;
	case 2:
		found->flags |= 0x200;
		break;
	case 3:
		found->flags |= 0x400;
		break;
	case 4:
		found->flags |= 0x800;
		break;
	}

	if (found->flags & 0x100) {
		time = gp->systemLv0Time;
	}
	else if (found->flags & 0x200) {
		time = gp->systemLv1Time;
	}
	else if (found->flags & 0x400) {
		time = gp->systemLv2Time;
	}
	else if (found->flags & 0x800) {
		time = gp->systemLv3Time;
	}
	else if (gp->inBattle) {
		time = gp->renderTime;
	}
	else {
		time = gp->renderFieldTime;
	}

	*(OSTime*)(found8 + 0x8) = OSTicksToMilliseconds(time);
	*(f32*)(found8 + 0x10) = 0.0f;
	*(f32*)(found8 + 0x14) = 1.0f;
	found->flags |= 0x1;
	if (a2 == 1) {
		found->flags |= 0x4;
	}
	if (a2 == 2) {
		*(OSTime*)(found8 + 0x8) -= (u64)(1000.0f * *(f32*)(*(u8**)(found8 + 0x18) + 8) / (f32)gp->framerate);
	}
}

void mapPauseAnimation(const char* name) {
	MapEntryAnimData* found = mapFindAnm(name);

	if (found) {
		found->flags |= 0x10;
	}
}

void mapReStartAnimation(const char* name) {
	MapEntryAnimData* found = mapFindAnm(name);

	if (found) {
		found->flags |= 0x20;
	}
}

void mapPauseAnimationAll(void) {
	MapWork* wp = &mapWork[activeGroup];
	MapEntry* entry = &wp->entries[0];
	s32 j;
	s32 i;

	for (i = 0; i < wp->entries[0].count; entry++, i++) {
		MapEntryAnimData* anim = entry->animData;
		for (j = 0; j < entry->animCount; j++, anim++) {
			anim->flags |= 0x10;
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
		for (j = 0; j < entry->animCount; j++, anim++) {
			anim->flags |= 0x20;
		}
	}
}

void mapSetPlayRate(const char* name, f32 rate) {
	MapEntryAnimData* found = mapFindAnm(name);

	if (found) {
		found->rate = rate;
	}
}

//more animation functions

// The loader searches the chunk table by name seven times; the search inlines into each caller.
static inline void* mapLoadFindChunk(MapHeader* header, const char* name) {
	u32 i;

	for (i = 0; i < (u32)header->file.chunkCount; i++) {
		if (!strcmp(&header->strings[header->chunks[i].string], name)) {
			return (u8*)header->data + header->chunks[i].offset;
		}
	}
	return NULL;
}

// The loader's own steps (header parse, relocation, animation table, display flags) inline into _mapLoad;
// as helpers their locals rank above _mapLoad's, which is what gives the original's registers. `header` is
// `&wp->entries[group].header`, written out again in the parse call, so the frontend keeps it as a CSE temp.
static inline void mapLoadRelocate(MapHeader* header) {
	u32 i;

	for (i = 0; i < (u32)header->file.relCount; i++) {
		u32 off = ((u32*)header->rel)[i];
		u8* relData = (u8*)header->data + off;
		if (*(u32*)relData <= 0x80000000) {
			*(u32*)relData += (u32)header->data;
		}
	}
}

static inline void mapLoadAnimTable(BOOL group) {
	MapWork* wp = mapGetWork();
	MapEntryAnimData* out;
	void* anim;
	u32 size;
	s32 i;

	anim = mapLoadFindChunk(&wp->entries[group].header, "animation_table");
	if (anim != NULL) {
		wp->entries[group].animCount = *(s32*)anim;
		if (wp->entries[group].animCount != 0) {
			size = (wp->entries[group].animCount * 32 + 0x1F) & ~0x1F;
			wp->entries[group].animData = _mapAlloc(size);
			memset(wp->entries[group].animData, 0, size);
			out = wp->entries[group].animData;
			for (i = 0; i < wp->entries[group].animCount; i++, out++) {
				out->flags = 0;
				out->data = ((void**)anim)[i + 1];
			}
		}
	}
}

static inline void mapLoadHeader(MapWork* wp, BOOL group, MapHeader* header) {
	u32 entryDatSize = wp->entries[group].datSize;
	u8* data = (u8*)wp->entries[group].dat;
	u32 sizeCheck;

	if (header != NULL) {
		memset(header, 0, sizeof(MapHeader));
		wp->entries[group].header.field_0x3C |= 1;
		memcpy(header, data, sizeof(MapFileHeader));
		if (header->file.fileSize == entryDatSize) {
			sizeCheck = sizeof(MapFileHeader);
			if ((u32)wp->entries[group].header.file.dataSize != 0) {
				wp->entries[group].header.data = &data[sizeCheck];
				sizeCheck = wp->entries[group].header.file.dataSize + sizeof(MapFileHeader);
			}
			if ((u32)wp->entries[group].header.file.relCount != 0) {
				wp->entries[group].header.rel = &data[sizeCheck];
				sizeCheck += 4 * wp->entries[group].header.file.relCount; //TODO: sizeof
			}
			if ((u32)wp->entries[group].header.file.chunkCount != 0) {
				wp->entries[group].header.chunks = (MapFileChunk*)&data[sizeCheck];
				sizeCheck += sizeof(MapFileChunk) * wp->entries[group].header.file.chunkCount;
			}
			if ((u32)wp->entries[group].header.file.unused != 0) {
				wp->entries[group].header.unused = &data[sizeCheck];
				sizeCheck += 8 * wp->entries[group].header.file.unused;
			}
			if (sizeCheck < wp->entries[group].header.file.fileSize) {
				wp->entries[group].header.strings = (const char*)&data[sizeCheck];
			}
			wp->entries[group].header.buffer = data;

			mapLoadRelocate(&wp->entries[group].header);
		}
	}
}

static inline void mapLoadDispFlags(BOOL group) {
	MapWork* wp = mapGetWork();
	MapObject* obj;
	s32 i;

	obj = wp->entries[group].objects;
	for (i = 0; i < wp->entries[group].numJoints; i++, obj++) {
		MapFileJoint* joint = obj->joints;

		if (joint->partCount >= 1 && joint->partCount <= 8) {
			MapJointDrawMode* dm = joint->drawMode;

			if (!(dm->unk2 & 8) && dm->unk4 == 0) {
				if (shadowGetCharShadowProjection() == 0 || !(obj->flags & 4)) {
					if (wp->entries[group].flags & 0x8000) {
						obj->flags |= 0x100;
					}
				}
			}
		}
	}
}

void _mapLoad(MapWork* wp, BOOL group, const char* map) {
	MapHeader* header;
	MapEntry* entry;
	MapFileInfo* info;
	MapFileJoint* mapJoint;
	MapFileJoint* hitJoint;
	DVDEntry* dvd;
	char str1[128];
	char str2[128];
	char str3[128];
	Vec pos;
	Vec min;
	Vec max;
	Mtx mtx;
	s32 i;
	MapFileLight* light;

	entry = &wp->entries[group];
	error_flag = FALSE;
	error_count = 0;
	strncpy(entry->name, map, sizeof(entry->name));

	sprintf(str1, "./m/%s/d", map);
	arcOpen(str1, &wp->entries[group].dat, &wp->entries[group].datSize);
	if (!wp->entries[group].dat) {
		sprintf(str2, "%s/%s", ".", str1);
		dvd = DVDMgrOpen(str2, 2, 0);
		wp->entries[group].datSize = DVDMgrGetLength(dvd);
		wp->entries[group].dat = _mapAllocTail(OSRoundUp32B(wp->entries[group].datSize));
		DVDMgrRead(dvd, wp->entries[group].dat, OSRoundUp32B(wp->entries[group].datSize), 0);
		DVDMgrClose(dvd);
	}

	sprintf(str1, "./m/%s/t", map);
	wp->entries[group].tpl = NULL;
	arcOpen(str1, &wp->entries[group].tpl, &wp->entries[group].tplSize);
	if (wp->entries[group].tpl) {
		UnpackTexPalette(wp->entries[group].tpl);
	}
	else {
		sprintf(str3, "%s/%s", ".", str1);
		dvd = DVDMgrOpen(str3, 2, 0);
		if (dvd) {
			wp->entries[group].tplSize = DVDMgrGetLength(dvd);
			wp->entries[group].tpl = _mapAllocTail(OSRoundUp32B(wp->entries[group].tplSize));
			DVDMgrRead(dvd, wp->entries[group].tpl, OSRoundUp32B(wp->entries[group].tplSize), 0);
			DVDMgrClose(dvd);
			UnpackTexPalette(wp->entries[group].tpl);
		}
	}

	GXInvalidateVtxCache();
	header = &wp->entries[group].header;
	mapLoadHeader(wp, group, &wp->entries[group].header);

	info = (MapFileInfo*)mapLoadFindChunk(header, "information");
	wp->entries[group].textureTable = mapLoadFindChunk(header, "texture_table");
	wp->entries[group].lightTable = mapLoadFindChunk(header, "light_table");
	wp->entries[group].fogTable = mapLoadFindChunk(header, "fog_table");
	wp->entries[group].materialNameTable = mapLoadFindChunk(header, "material_name_table");
	wp->entries[group].vcdTable = mapLoadFindChunk(header, "vcd_table");

	mapJoint = mapSearchDmdJoint(info->joint, (s8*)info->mapName);
	hitJoint = mapSearchDmdJoint(info->joint, (s8*)info->hitName);
	if (!strcmp(info->version, "ver1.02")) {
		wp->entries[group].flags |= 0x8000;
	}
	else {
		wp->entries[group].flags &= ~0x8000;
	}

	PSMTXScale(mtx, 10.0f, 10.0f, 10.0f);
	wp->entries[group].rootMapObj = mapEntry(mapJoint, mtx, group);
	wp->entries[group].rootHitObj = hitEntry(hitJoint, mtx, group);

	for (i = 0; i < entry->lightTable->count; i++) {
		s32 type;
		s32 flags;

		light = entry->lightTable->lights[i];
		type = 1;
		flags = 0;
		PSVECScale(&light->position, &pos, 10.0f);
		if (!strcmp(light->type, "ambientLight")) {
			type = 0;
		}
		else if (!strcmp(light->type, "pointLight")) {
			type = 1;
		}
		else if (!strcmp(light->type, "directionalLight")) {
			type = 2;
		}
		else if (!strcmp(light->type, "spotLight")) {
			type = 3;
		}
		if (light->field_0x40 == 0x12D60D8) {
			flags = light->field_0x3C;
		}
		lightEntry(light->name, type, &pos, &light->rotation, light->color, light->spotAngle, light->attenAngle, light->attenType, flags);
	}

	mapResetPaperAmbColor();
	mapResetPaperCraftAmbColor();

	if (wp->entries[group].fogTable != NULL) {
		wp->entries[group].fogEnable = wp->entries[group].fogTable->enable;
		wp->entries[group].fogType = wp->entries[group].fogTable->type;
		wp->entries[group].fogStart = wp->entries[group].fogTable->start;
		wp->entries[group].fogEnd = wp->entries[group].fogTable->end;
		wp->entries[group].fogColor = wp->entries[group].fogTable->color;
		if (wp->entries[group].fogEnable != 0) {
			wp->entries[group].flags |= 1;
		}
	}
	else {
		wp->entries[group].fogEnable = 0;
		wp->entries[group].fogType = 0;
		wp->entries[group].fogStart = 0.0f;
		wp->entries[group].fogEnd = 0.0f;
		wp->entries[group].fogColor = (GXColor){0, 0, 0, 0};
		wp->entries[group].flags &= ~1;
	}
	if (activeGroup != 1) {
		sprintf(str1, "./m/%s/c", map);
		camLoadRoad(CAMERA_3D, str1);
	}
	camSetTypePersp(CAMERA_3D);
	mapBuildTexture(wp->entries[group].rootMapObj, wp->entries[group].tpl, wp->entries[group].textureTable);
	mapGetBoundingBox(&min, &max);

	wp->entries[group].bbox[0] = (Vec){max.x, max.y, max.z};
	wp->entries[group].bbox[1] = (Vec){min.x, max.y, max.z};
	wp->entries[group].bbox[2] = (Vec){max.x, min.y, max.z};
	wp->entries[group].bbox[3] = (Vec){max.x, max.y, min.z};
	wp->entries[group].bbox[4] = (Vec){min.x, min.y, max.z};
	wp->entries[group].bbox[5] = (Vec){max.x, min.y, min.z};
	wp->entries[group].bbox[6] = (Vec){min.x, max.y, min.z};
	wp->entries[group].bbox[7] = (Vec){min.x, min.y, min.z};

	mapLoadAnimTable(group);

	makeDisplayList(group);
	mapLoadDispFlags(group);
	mapDispDisabled = 0;
}

// An empty string; symbols.txt names it as a float.

// `i = 0; activeGroup = i;` plus `opt_propagation off` matched this; the pragma keeps the assignment
// from constant-folding to a fresh r0 so i keeps r30.
#pragma opt_propagation off // keep activeGroup=i from folding to a fresh r0 so i keeps r30

static const s32 culling[] = {GX_CULL_FRONT, GX_CULL_BACK, GX_CULL_ALL, GX_CULL_NONE};
static const s32 texmtx_tbl[] = {
	GX_TEXMTX0, GX_TEXMTX1, GX_TEXMTX2, GX_TEXMTX3, GX_TEXMTX4,
	GX_TEXMTX5, GX_TEXMTX6, GX_TEXMTX7, GX_TEXMTX8, GX_TEXMTX9,
};
static const s32 texfmt_tbl[] = {
	GX_TF_RGB565, GX_TF_RGB5A3, GX_TF_RGBA8, GX_TF_CMPR, GX_TF_I4, GX_TF_I8,
	GX_TF_IA4, GX_TF_IA8, GX_TF_C4, GX_TF_C8, GX_TF_C14X2,
};
static const s32 texwrap_tbl[] = {GX_CLAMP, GX_REPEAT, GX_MIRROR};
static const s32 palfmt_tbl[] = {GX_TL_RGB565, GX_TL_RGB5A3};
static const u32 lightid_tbl[] = {
	GX_LIGHT0, GX_LIGHT1, GX_LIGHT2, GX_LIGHT3, GX_LIGHT4, GX_LIGHT5, GX_LIGHT6, GX_LIGHT7,
};

void mapUnLoad(void) {
	MapEntry* entry;
	s32 i;
	MapWork* wp;

	sysWaitDrawSync();
	wp = mapWork;
	i = 0;
	activeGroup = i;
	entry = &wp->entries[0];
	for (; i < wp->entries[0].count; entry++, i++) {
		if (entry->objects) {
			_mapFree(entry->objects);
			entry->numJoints = 0;
			entry->objects = NULL;
		}
		if (entry->animData) {
			_mapFree(entry->animData);
			entry->animCount = 0;
			entry->animData = NULL;
		}
		if (entry->dlData) {
			_mapFree(entry->dlData);
			entry->dlData = NULL;
			entry->dlTotal = 0;
		}
	}
	mapDispDisabled = 0;
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
		if (entry->chunkBuf) {
			_mapFree(entry->chunkBuf);
			entry->chunkBuf = NULL;
		}
		strncpy(entry->name, "", 0x10);
	}
	arcDelete(1);
	memset(wp, 0, 0x2F4);
}
#pragma opt_propagation reset

BOOL mapPreLoad(const char* map) {
	return TRUE; //do nothing
}

void mapLoad(const char* map) {
	MapWork* wp;

	sysWaitDrawSync();
	wp = (MapWork*)mapWork; // redundant cast: makes MWCC load into r31 (wp's home) instead of the memset arg r3
	activeGroup = 0;
	memset(wp, 0, sizeof(MapWork));
	_mapLoad(wp, 0, map);
	wp->entries[0].count = 1;
}

void bmapUnLoad(void) {
	MapEntry* entry;
	MapWork* wp;
	s32 i;

	sysWaitDrawSync();
	activeGroup = 1;
	i = 0;
	wp = &mapWork[1];
	entry = &wp->entries[0];
	for (; i < wp->entries[0].count; entry++, i++) {
		if (entry->objects) {
			_mapFree(entry->objects);
			entry->numJoints = 0;
			entry->objects = NULL;
		}
		if (entry->animData) {
			_mapFree(entry->animData);
			entry->animCount = 0;
			entry->animData = NULL;
		}
		if (entry->dlData) {
			_mapFree(entry->dlData);
			entry->dlData = NULL;
			entry->dlTotal = 0;
		}
	}
	mapDispDisabled = 0;
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
		if (entry->chunkBuf) {
			_mapFree(entry->chunkBuf);
			entry->chunkBuf = NULL;
		}
		strncpy(entry->name, "", 0x10);
	}
	arcDelete(1);
	arcDelete(2);
	memset(wp, 0, 0x2F4);

	activeGroup = 0;
	entry = (wp = mapWork)->entries;
	for (i = 0; i < wp->entries[0].count; entry++, i++) {
		if (entry->field_0x170) {
			entry->dat = _mapAllocTail(entry->datSize);
			entry->tpl = _mapAllocTail(entry->tplSize);
			aramMgrToMram(entry->field_0x170, entry->dat);
			aramMgrToMram(*(void**)((u8*)entry + 0x178), entry->tpl);
			entry->field_0x170 = NULL;
			*(void**)((u8*)entry + 0x178) = NULL;
		}
		else {
			entry->chunkBuf = _mapAllocTail(entry->chunkBufSize);
			aramMgrToMram(entry->field_0x174, entry->chunkBuf);
			entry->field_0x174 = NULL;
		}
		makeDisplayList(i);
	}
	if (wp->entries[0].chunkBuf) {
		arcEntry(1, wp->entries[0].chunkBuf, wp->entries[0].chunkBufSize);
	}
}

void bmapLoad(const char* map1, const char* map2) {
	MapEntry* entry;
	u8* p;
	s32 i;
	MapWork* wp;

	sysWaitDrawSync();
	activeGroup = 0;
	wp = mapGetWork();
	for (i = 0; i < wp->entries[0].count; i++) {
		entry = &mapGetWork()->entries[i];
		if (entry->dlData) {
			_mapFree(entry->dlData);
		}
		entry->dlTotal = 0;
		entry->dlData = NULL;
		p = (u8*)&wp->entries[i];
		((MapEntry*)p)->field_0x170 = NULL;
		*(s32*)(p + 0x178) = 0;
		((MapEntry*)p)->field_0x174 = NULL;
		if (((MapEntry*)p)->dat) {
			if (!arcDataCheck(((MapEntry*)p)->dat)) {
				((MapEntry*)p)->field_0x170 = aramMgrToAram(((MapEntry*)p)->dat, ((MapEntry*)p)->datSize);
				*(void**)(p + 0x178) = aramMgrToAram(((MapEntry*)p)->tpl, ((MapEntry*)p)->tplSize);
				_mapFree(((MapEntry*)p)->tpl);
				_mapFree(((MapEntry*)p)->dat);
				((MapEntry*)p)->tpl = NULL;
				((MapEntry*)p)->dat = NULL;
			}
			else {
				((MapEntry*)p)->field_0x174 = aramMgrToAram(((MapEntry*)p)->chunkBuf, ((MapEntry*)p)->chunkBufSize);
				_mapFree(((MapEntry*)p)->chunkBuf);
				((MapEntry*)p)->chunkBuf = NULL;
			}
		}
	}
	activeGroup = 1;
	wp = mapGetWork();
	memset(wp, 0, sizeof(MapWork));
	_mapLoad(wp, 0, map1);
	wp->entries[0].count = 1;
	if (map2) {
		_mapLoad(wp, 1, map2);
		wp->entries[0].count = 2;
	}
}

// in-file layout for the animation key records mapCalcAnimMatrix walks
// (the joint pointer passed by hitdrv.c points at an array of these)



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
	f32 tx, ty, rz, tz, rx, u3, ry, dt, u, sx, value0, sy, tan0, den, sz, k0, value1, tan1, u2, d, a, t;
	f32 k1, h00, h10, h01, h11, zero;
	s32 flag;

	count = joint->count;
	for (i = 1; i < count; i++) {
		key = &joint->key[i - 1];
		next = &joint->key[i];
		if (anim->frame <= next->time) {
			t = anim->frame;
		}
		else if (i == count - 1) {
			t = next->time;
		}
		else {
			continue;
		}

		for (k = 0; k < 9; k++) {
			MapAnimChannel* ch = &key->chan[k];
			f32* outp = &out[k]; // declared before chn so MWCC colours out=r4, chn=r5
			MapAnimChannel* chn = &next->chan[k];

			if (ch->flag != 0) {
				*outp = ch->value;
			}
			else {
				k0 = key->time;
				k1 = next->time;
				dt = t - k0;
				d = k1 - k0;

				tan1 = chn->unk4;
				value1 = chn->value;
				tan0 = ch->unk8;
				value0 = ch->value;

				zero = 0.0f;
				flag = (d != zero);
				if (((u32)__cntlzw(flag) >> 5) != 0) {
					den = 0.00000001f;
				}
				else {
					den = d;
				}
				u = dt / den;
				u2 = u * u;
				u3 = u2 * u;
				a = 3.0f * u2;
				h00 = 1.0f + (2.0f * u3 - a);
				h10 = u + (u3 - 2.0f * u2);
				h01 = -2.0f * u3 + a;
				h11 = u3 - u2;
				*outp = value0 * h00
				       + tan0 * (d * h10)
				       + value1 * h01
				       + tan1 * (d * h11);
			}
		}

		tx = out[0] - joint->base[0];
		ty = out[1] - joint->base[1];
		tz = out[2] - joint->base[2];
		rx = out[3] - joint->base[3];
		ry = out[4] - joint->base[4];
		rz = out[5] - joint->base[5];
		sx = out[6] / joint->div[0];
		sy = out[7] / joint->div[1];
		sz = out[8] / joint->div[2];

		zero = 0.0f;
		if (tx != zero || ty != zero || tz != zero) {
			PSMTXTrans(trans, tx, ty, tz);
		}
		if (rx != zero) {
			PSMTXRotRad(rotX, 'x', 0.017453292f * rx);
		}
		if (ry != zero) {
			PSMTXRotRad(rotY, 'y', 0.017453292f * ry);
		}
		if (rz != zero) {
			PSMTXRotRad(rotZ, 'z', 0.017453292f * rz);
		}
		PSMTXScale(scale, sx, sy, sz);
		PSMTXIdentity(local);
		zero = 0.0f;
		if (tx != zero || ty != zero || tz != zero) {
			PSMTXConcat(local, trans, local);
		}
		if (rz != zero) {
			PSMTXConcat(local, rotZ, local);
		}
		if (ry != zero) {
			PSMTXConcat(local, rotY, local);
		}
		if (rx != zero) {
			PSMTXConcat(local, rotX, local);
		}
		if (sx != zero || sy != zero || sz != zero) {
			PSMTXConcat(local, scale, local);
		}
		PSMTXConcat(inMtx, local, outMtx);
		break;
	}
}

static BOOL mapReCalcMatrixCheck(BOOL force, u32 flags) {
	BOOL recalc;

	recalc = FALSE;
	if (flags & MAPOBJ_FLAG_DIRTY) {
		recalc = TRUE;
	}
	if (force) {
		recalc = TRUE;
	}
	return recalc;
}

void mapReCalcMatrix(MapObject* obj, Mtx mtx, BOOL force) {
	u32* flags = &obj->flags;
	MapObject* child;
	MapObject* next;
	BOOL recalc;

	recalc = FALSE;
	if ((*flags) & MAPOBJ_FLAG_DIRTY) {
		recalc = TRUE;
	}
	if (force) {
		recalc = TRUE;
	}

	if (recalc) {
		if ((*flags) & MAPOBJ_FLAG_ANIM_MTX) {
			PSMTXConcat(mtx, obj->animLocalMtx, obj->modelWorldMtx);
		}
		else {
			PSMTXConcat(mtx, obj->localMtx, obj->modelWorldMtx);
		}
		if ((*flags) & MAPOBJ_FLAG_USER_MTX) {
			PSMTXConcat(obj->modelWorldMtx, obj->unkAC, obj->modelWorldMtx);
		}
		(*flags) &= ~MAPOBJ_FLAG_DIRTY;

		child = obj->child;
		if (child != NULL) {
			u32* cflags = &child->flags;

			if ((*cflags) & MAPOBJ_FLAG_ANIM_MTX) {
				PSMTXConcat(obj->modelWorldMtx, child->animLocalMtx, child->modelWorldMtx);
			}
			else {
				PSMTXConcat(obj->modelWorldMtx, child->localMtx, child->modelWorldMtx);
			}
			if ((*cflags) & MAPOBJ_FLAG_USER_MTX) {
				PSMTXConcat(child->modelWorldMtx, child->unkAC, child->modelWorldMtx);
			}
			(*cflags) &= ~MAPOBJ_FLAG_DIRTY;

			if (child->child != NULL) {
				mapReCalcMatrix(child->child, child->modelWorldMtx, 1);
			}
			if (child->next != NULL) {
				mapReCalcMatrix(child->next, obj->modelWorldMtx, 1);
			}
		}

		next = obj->next;
		if (next != NULL) {
			u32* nflags = &next->flags;

			if (mapReCalcMatrixCheck(force, *nflags)) {
				if ((*nflags) & MAPOBJ_FLAG_ANIM_MTX) {
					PSMTXConcat(mtx, next->animLocalMtx, next->modelWorldMtx);
				}
				else {
					PSMTXConcat(mtx, next->localMtx, next->modelWorldMtx);
				}
				if ((*nflags) & MAPOBJ_FLAG_USER_MTX) {
					PSMTXConcat(next->modelWorldMtx, next->unkAC, next->modelWorldMtx);
				}
				(*nflags) &= ~MAPOBJ_FLAG_DIRTY;

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
			u32* cflags = &child->flags;

			if (mapReCalcMatrixCheck(force, *cflags)) {
				if ((*cflags) & MAPOBJ_FLAG_ANIM_MTX) {
					PSMTXConcat(obj->modelWorldMtx, child->animLocalMtx, child->modelWorldMtx);
				}
				else {
					PSMTXConcat(obj->modelWorldMtx, child->localMtx, child->modelWorldMtx);
				}
				if ((*cflags) & MAPOBJ_FLAG_USER_MTX) {
					PSMTXConcat(child->modelWorldMtx, child->unkAC, child->modelWorldMtx);
				}
				(*cflags) &= ~MAPOBJ_FLAG_DIRTY;

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
			u32* nflags = &next->flags;

			if (mapReCalcMatrixCheck(force, *nflags)) {
				if ((*nflags) & MAPOBJ_FLAG_ANIM_MTX) {
					PSMTXConcat(mtx, next->animLocalMtx, next->modelWorldMtx);
				}
				else {
					PSMTXConcat(mtx, next->localMtx, next->modelWorldMtx);
				}
				if ((*nflags) & MAPOBJ_FLAG_USER_MTX) {
					PSMTXConcat(next->modelWorldMtx, next->unkAC, next->modelWorldMtx);
				}
				(*nflags) &= ~MAPOBJ_FLAG_DIRTY;

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

// Inline copy of mapSearchAnmObj for mapMain. The inner-loop bound assigns the table
// (`table = *(u8**)((u8*)entry + 0xA0)`) inside the condition so the body reuses the condition's
// loaded register (retail loads `0xa0(entry)` once per iteration, not twice); the pair address is
// the table plus the byte offset `ofs` plus 4.
static inline u8* mapSearchAnimObj(const char* name) {
	u8* table;
	MapEntry* entry;
	s32 ofs;
	MapWork* wp;
	s32 j;
	s32 i;
	wp = &mapWork[activeGroup];
	entry = &wp->entries[0];
	for (i = 0; i < wp->entries[0].count; entry++, i++) {
		if ((u8*)entry->materialNameTable == NULL) {
			return NULL;
		}
		for (j = 0, ofs = 0; j < *(s32*)(table = (u8*)entry->materialNameTable); ofs += 8, j++) {
			u8* pair = (u8*)((u32)table + ofs + 4);
			if (!strcmp(*(char**)pair, name)) {
				return *(u8**)(pair + 4);
			}
		}
	}
	return NULL;
}

// Source shapes that mattered: (1) the animation `item` pointer must be re-loaded from the list
// inside each `if (obj != NULL)` / `if (light != NULL)` block instead of cached across the block's
// mapSearchAnimObj / lightNameToPtr calls; (2) the flags word `*(u16*)anim` is read once into
// `animFlags` at the top of the j-body -- that keeps it in r4 across the branches instead of letting
// the gp-pointer loads clobber r3 and force a reload; (3) in mapSearchAnimObj the table is read with
// the expression inside the inner-loop bound (assigned to `table` in the bound), so retail's second
// `0xa0(entry)` load disappears; (4) the matrix loop walks the function-level `p`/`k` (the other loops
// keep block locals), and with the top-level declaration order below the callee-saved GPRs match retail
// (lists in r19..r15, the OSTime in r22/r25); (5) the alpha, colour and light steps are inline helpers
// (as blocks of mapMain the clamp diamond keeps an extra `fmr` for t), each declaring its locals in the
// order that gives retail's registers: inside an inlined helper the first-declared local gets the
// lowest virtual register (found with tools/rasim.py on a tools/mwccdbg.py dump); (6) the colour step
// indexes its output with a byte offset `off`, the light steps store through a `dst` pointer, and
// mapSearchAnimObj steps a byte offset `ofs` beside `j`: as named locals declared between the other
// pointers they colour like retail's, where `k * 4` / `j * 8` would become loop temps numbered below
// every local; (7) the function-scope `Mtx` (retail's stack puts it above the light steps' `out`
// arrays); (8) the matrix loop's search is the public mapGetMapObj (its declaration order differs
// from mapGetMapObjSub's).
// The Hermite output sum evaluates `value0 * h00` before `tan0 * (d * h10)`, and the keys load
// `tan1, value1, tan0, value0` with `zero = 0.0f; d != zero` as in mapCalcAnimMatrix. The d==0
// boolean uses the __cntlzw intrinsic to reproduce retail's cntlzw/srwi normalisation. The colour/light
// Hermite loops have no `if (cnt > 1)` wrapper: the bare `for (ki = 1; ...)` guard reproduces retail's
// `subi cnt-1; mtctr` scheduled before the `cmpwi`.
// The light-colour step converts to the u8 colour fields without a cast (an `(s32)` cast schedules
// the first byte's load one slot early).
// The public lookup gets its work pointer through mapGetWork and indexes the outer entry loop;
// its getter result and strength-reduced entry pointer give retail's registers both here and
// in the emitted mapGetMapObj body.
static inline void mapAnimAlphaObj(u8* obj, u8* anim, u8* item) {
	s32 cnt = *(s32*)(item + 4);
	s32 ki;
	for (ki = 1; ki < cnt; ki++) {
		u8* prev = item + 8 + (ki - 1) * 0x54;
		u8* next = item + 8 + ki * 0x54;
		f32 a, d, u2, k0, tan1, value1, tan0, value0, t, dt, u, u3, den, k1, b, c, h10, h00, h01, h11, val, zero;
		t = *(f32*)(anim + 0x10);
		if (t <= *(f32*)next) {
			t = *(f32*)(anim + 0x10);
		}
		else if (ki == cnt - 1) {
			t = *(f32*)next;
		}
		else {
			continue;
		}
		if (*(s32*)(prev + 0x14) != 0) {
			f32 value = *(f32*)(prev + 4);
			*(u8*)(obj + 0x10C) = (s32)(255.0f - 255.0f * value);
			break;
		}
		else {
			s32 flag;
			k0 = *(f32*)prev;
			k1 = *(f32*)next;
			dt = t - k0;
			d = k1 - k0;
			tan1 = *(f32*)(next + 8);
			value1 = *(f32*)(next + 4);
			tan0 = *(f32*)(prev + 0xC);
			value0 = *(f32*)(prev + 4);
			zero = 0.0f;
			flag = (d != zero);
			if (((u32)__cntlzw(flag) >> 5) != 0) {
				den = 0.00000001f;
			}
			else {
				den = d;
			}
			u = dt / den;
			u2 = u * u;
			u3 = u2 * u;
			a = 3.0f * u2;
			b = u3 - 2.0f * u2;
			c = 2.0f * u3 - a;
			h11 = u3 - u2;
			h10 = u + b;
			h00 = 1.0f + c;
			h01 = -2.0f * u3 + a;
			val = value0 * h00 + tan0 * (d * h10) + value1 * h01 + tan1 * (d * h11);
			val = 255.0f - 255.0f * val;
			*(u8*)(obj + 0x10C) = (s32)val;
			break;
		}
	}
}

static inline void mapAnimColorObj(u8* obj, u8* anim, u8* lp) {
	u8* chn;
	s32 cnt;
	s32 off;
	u8* ch;
	u8* item;
	u8* prev;
	u8* next;
	s32 ki;
	s32 flag;
	f32 a, t, k0, d, tan1, dt, value1, tan0, u2, den, value0, u, u3, k1, h11, h00, h01, c, b, h10, zero;

	item = *(u8**)(lp + 4);
	cnt = *(s32*)(item + 0x10);
	for (ki = 1; ki < cnt; ki++) {
		prev = item + 0x14 + (ki - 1) * 0x68;
		next = item + 0x14 + ki * 0x68;
		t = *(f32*)(anim + 0x10);
		if (t <= *(f32*)next) {
			t = *(f32*)(anim + 0x10);
		}
		else if (ki == cnt - 1) {
			t = *(f32*)next;
		}
		else {
			continue;
		}
		ch = prev;
		chn = next;
		for (off = 0; off < 0x14; ch += 0x14, off += 4, chn += 0x14) {
			if (*(s32*)(ch + 0x14) != 0) {
				*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x2C + off) = *(f32*)(ch + 4);
			}
			else {
				k0 = *(f32*)prev;
				k1 = *(f32*)next;
				dt = t - k0;
				d = k1 - k0;
				tan1 = *(f32*)(chn + 8);
				value1 = *(f32*)(chn + 4);
				tan0 = *(f32*)(ch + 0xC);
				value0 = *(f32*)(ch + 4);
				zero = 0.0f;
				flag = (d != zero);
				if (((u32)__cntlzw(flag) >> 5) != 0) {
					den = 0.00000001f;
				}
				else {
					den = d;
				}
				u = dt / den;
				u2 = u * u;
				u3 = u2 * u;
				a = 3.0f * u2;
				b = u3 - 2.0f * u2;
				c = 2.0f * u3 - a;
				h11 = u3 - u2;
				h10 = u + b;
				h00 = 1.0f + c;
				h01 = -2.0f * u3 + a;
				*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x2C + off) = (value0 * h00) + (tan0 * (d * h10)) + value1 * h01 + tan1 * (d * h11);
			}
		}
		*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x40) = *(f32*)(item + 8);
		*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x44) = *(f32*)(item + 0xC);
		break;
	}
}

static inline void mapAnimLightPos(LightEntry* light, u8* anim, u8* item) {
	s32 cnt;
	s32 ki;
	s32 k;
	u8* chn;
	f32* dst;
	u8* ch;
	u8* next;
	u8* prev;
	s32 flag;
	f32 out[9];
	f32 den, u2, a, t, d, tan1, value1, tan0, k0, dt, value0, u, u3, k1, h11, h00, h01, c, b, h10, zero;

	cnt = *(s32*)(item + 4);
	for (ki = 1; ki < cnt; ki++) {
		prev = item + 8 + (ki - 1) * 0xB8;
		next = item + 8 + ki * 0xB8;
		t = *(f32*)(anim + 0x10);
		if (t <= *(f32*)next) {
			t = *(f32*)(anim + 0x10);
		}
		else if (ki == cnt - 1) {
			t = *(f32*)next;
		}
		else {
			continue;
		}
		ch = prev;
		chn = next;
		dst = out;
		for (k = 0; k < 9; ch += 0x14, dst++, chn += 0x14, k++) {
			if (*(s32*)(ch + 0x14) != 0) {
				*dst = *(f32*)(ch + 4);
			}
			else {
				k0 = *(f32*)prev;
				k1 = *(f32*)next;
				dt = t - k0;
				d = k1 - k0;
				tan1 = *(f32*)(chn + 8);
				value1 = *(f32*)(chn + 4);
				tan0 = *(f32*)(ch + 0xC);
				value0 = *(f32*)(ch + 4);
				zero = 0.0f;
				flag = (d != zero);
				if (((u32)__cntlzw(flag) >> 5) != 0) {
					den = 0.00000001f;
				}
				else {
					den = d;
				}
				u = dt / den;
				u2 = u * u;
				u3 = u2 * u;
				a = 3.0f * u2;
				b = u3 - 2.0f * u2;
				c = 2.0f * u3 - a;
				h11 = u3 - u2;
				h10 = u + b;
				h00 = 1.0f + c;
				h01 = -2.0f * u3 + a;
				*dst = (value0 * h00) + (tan0 * (d * h10)) + value1 * h01 + tan1 * (d * h11);
			}
		}
		{
			f32 scale = 10.0f;
			light->position.x = scale * out[0];
			light->position.y = scale * out[1];
			light->position.z = scale * out[2];
		}
		light->rotation.x = out[3];
		light->rotation.y = out[4];
		light->rotation.z = out[5];
		break;
	}
}

static inline void mapAnimLightColor(LightEntry* light, u8* anim, u8* item) {
	s32 cnt;
	s32 ki;
	s32 k;
	u8* chn;
	f32* dst;
	u8* ch;
	u8* next;
	u8* prev;
	s32 flag;
	f32 out[5];
	f32 den, u2, a, t, d, tan1, value1, tan0, k0, dt, value0, u, u3, k1, h11, h00, h01, c, b, h10, zero;

	cnt = *(s32*)(item + 4);
	for (ki = 1; ki < cnt; ki++) {
		prev = item + 8 + (ki - 1) * 0x68;
		next = item + 8 + ki * 0x68;
		t = *(f32*)(anim + 0x10);
		if (t <= *(f32*)next) {
			t = *(f32*)(anim + 0x10);
		}
		else if (ki == cnt - 1) {
			t = *(f32*)next;
		}
		else {
			continue;
		}
		ch = prev;
		chn = next;
		dst = out;
		for (k = 0; k < 5; ch += 0x14, dst++, chn += 0x14, k++) {
			if (*(s32*)(ch + 0x14) != 0) {
				*dst = *(f32*)(ch + 4);
			}
			else {
				k0 = *(f32*)prev;
				k1 = *(f32*)next;
				dt = t - k0;
				d = k1 - k0;
				tan1 = *(f32*)(chn + 8);
				value1 = *(f32*)(chn + 4);
				tan0 = *(f32*)(ch + 0xC);
				value0 = *(f32*)(ch + 4);
				zero = 0.0f;
				flag = (d != zero);
				if (((u32)__cntlzw(flag) >> 5) != 0) {
					den = 0.00000001f;
				}
				else {
					den = d;
				}
				u = dt / den;
				u2 = u * u;
				u3 = u2 * u;
				a = 3.0f * u2;
				b = u3 - 2.0f * u2;
				c = 2.0f * u3 - a;
				h11 = u3 - u2;
				h10 = u + b;
				h00 = 1.0f + c;
				h01 = -2.0f * u3 + a;
				*dst = (value0 * h00) + (tan0 * (d * h10)) + value1 * h01 + tan1 * (d * h11);
			}
		}
		light->color.r = 255.0f * out[0];
		light->color.g = 255.0f * out[1];
		light->color.b = 255.0f * out[2];
		light->color.a = 0xFF;
		light->spotAngle = out[4];
		light->attenAngle = out[3];
		break;
	}
}

#pragma opt_common_subs off // required for retail's register colouring here
void mapMain(void) {
	MapWork* wp0;
	u8* p;
	OSTime time;
	char namebuf[0x100];
	MapEntry* entry;
	s32 j = 0;
	s32 k;
	u8* anim;
	BOOL updated;
	u8* list19;
	u8* list18;
	u8* list17;
	u8* list16;
	u8* list15;
	s32 i = 0;
	char namebuf2[0x100];
	Mtx mtx;

	wp0 = &mapWork[activeGroup];
	for (entry = &wp0->entries[0]; i < wp0->entries[0].count; entry++, i++) {
		anim = (u8*)entry->animData;
		updated = FALSE;
		for (j = 0; j < entry->animCount; j++, anim += 0x20) {
			u16 animFlags = *(u16*)anim;
			list19 = *(u8**)((*(u8**)(anim + 0x18)) + 0xC);
			list18 = *(u8**)((*(u8**)(anim + 0x18)) + 0x10);
			list17 = *(u8**)((*(u8**)(anim + 0x18)) + 0x14);
			list16 = *(u8**)((*(u8**)(anim + 0x18)) + 0x18);
			list15 = *(u8**)((*(u8**)(anim + 0x18)) + 0x1C);
			if (!(animFlags & 0x1)) {
				continue;
			}
			if (animFlags & 0x2) {
				continue;
			}
			if (animFlags & 0x100) {
				time = gp->systemLv0Time;
			}
			else if (animFlags & 0x200) {
				time = gp->systemLv1Time;
			}
			else if (animFlags & 0x400) {
				time = gp->systemLv2Time;
			}
			else if (animFlags & 0x800) {
				time = gp->systemLv3Time;
			}
			else if (gp->inBattle) {
				time = gp->renderTime;
			}
			else {
				time = gp->renderFieldTime;
			}
			if (!(animFlags & 0x10)) {
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
			else if (animFlags & 0x20) {
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
				p = list19;
				for (k = 0; k < *(s32*)list19; p += 4, k++) {
					MapObject* obj = mapGetMapObj(*(char**)*(u8**)(p + 4));
					if (obj != NULL) {
						mapCalcAnimMatrix(obj->animLocalMtx, obj->localMtx, (MapEntryAnimData*)anim, *(u8**)(p + 4));
						obj->flags |= MAPOBJ_FLAG_ANIM_MTX;
						obj->flags |= MAPOBJ_FLAG_DIRTY;
						updated = TRUE;
					}
				}
			}
			/* color animation */
			if (list18 != NULL) {
				u8* lp = list18;
				s32 li;
				for (li = 0; li < *(s32*)list18; lp += 4, li++) {
					u8* obj;
					strcpy(namebuf, *(char**)*(u8**)(lp + 4));
					obj = mapSearchAnimObj(namebuf);
					if (obj != NULL) {
						mapAnimColorObj(obj, anim, lp);
					}
					strcpy(namebuf, *(char**)*(u8**)(lp + 4));
					strcat(namebuf, "_v");
					obj = mapSearchAnimObj(namebuf);
					if (obj != NULL) {
						mapAnimColorObj(obj, anim, lp);
					}
					strcpy(namebuf, *(char**)*(u8**)(lp + 4));
					strcat(namebuf, "_v_x");
					obj = mapSearchAnimObj(namebuf);
					if (obj != NULL) {
						mapAnimColorObj(obj, anim, lp);
					}
				}
			}
			/* alpha animation */
			if (list17 != NULL) {
				u8* lp = list17;
				s32 li;
				for (li = 0; li < *(s32*)list17; lp += 4, li++) {
					u8* obj;
					strcpy(namebuf2, *(char**)*(u8**)(lp + 4));
					obj = mapSearchAnimObj(namebuf2);
					if (obj != NULL) {
						mapAnimAlphaObj(obj, anim, *(u8**)(lp + 4));
					}
					strcpy(namebuf2, *(char**)*(u8**)(lp + 4));
					strcat(namebuf2, "_v");
					obj = mapSearchAnimObj(namebuf2);
					if (obj != NULL) {
						mapAnimAlphaObj(obj, anim, *(u8**)(lp + 4));
					}
					strcpy(namebuf2, *(char**)*(u8**)(lp + 4));
					strcat(namebuf2, "_v_x");
					obj = mapSearchAnimObj(namebuf2);
					if (obj != NULL) {
						mapAnimAlphaObj(obj, anim, *(u8**)(lp + 4));
					}
				}
			}
			/* light animation (position/rotation) */
			if (list16 != NULL) {
				u8* lp = list16;
				s32 li;
				for (li = 0; li < *(s32*)list16; lp += 4, li++) {
					LightEntry* light = lightNameToPtr(*(char**)*(u8**)(lp + 4));
					if (light != NULL) {
						mapAnimLightPos(light, anim, *(u8**)(lp + 4));
					}
				}
			}
			/* light animation (color/angle) */
			if (list15 != NULL) {
				u8* lp = list15;
				s32 li;
				for (li = 0; li < *(s32*)list15; lp += 4, li++) {
					LightEntry* light = lightNameToPtr(*(char**)*(u8**)(lp + 4));
					if (light != NULL) {
						mapAnimLightColor(light, anim, *(u8**)(lp + 4));
					}
				}
			}
		}
		if (updated) {
			PSMTXScale(mtx, 10.0f, 10.0f, 10.0f);
			mapReCalcMatrix(entry->rootMapObj, mtx, 0);
		}
	}
}

#pragma opt_common_subs reset
static void mapDrawObjectFog(MapObject* obj);

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

	if (obj->flags & MAPOBJ_FLAG_IN_GROUP) {
		if (obj->flags & 0x8000) {
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
			t = 0.01f * (f32)index;
			GXGetProjectionv(proj);
			f0 = -2.0f;
			f3 = 1.0f;
			{
				f32 nn = cam->near;
				f32 sum = cam->far + nn;
				f32 t2 = f0 * cam->far;
				f32 pr = t2 * nn;

				proj[5] = proj[5] * (f3 + t * pr / (f3 * sum * (f3 + t)));
			}
			GXSetProjectionv(proj);
		}

		kind = obj->renderMode;
		if (obj->joints->partCount > 0) {
			GXColor color = obj->color;
			GXColor matColor = obj->joints->parts[0].material->field_0x10C;

			if (mapTestXLU(obj->flags, &color, &matColor)) {
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
	if (!(obj->flags & MAPOBJ_FLAG_DISPLAY_LIST)) {
		PSMTXInvXpose(mtx, invMtx);
		GXLoadNrmMtxImm(invMtx, GX_PNMTX0);
	}
	GXSetCurrentMtx(GX_PNMTX0);
	GXGetCullMode(&cull);
	if (cull != culling[obj->joints->drawMode->cullMode]) {
		GXSetCullMode(culling[obj->joints->drawMode->cullMode]);
	}
	mapDrawObjectFog(obj);

	if (!(obj->flags & MAPOBJ_FLAG_IN_GROUP) || !((obj->flags & MAPOBJ_FLAG_NO_DISP) || ((obj->flags & MAPOBJ_FLAG_HIDDEN) && !(obj->flags & MAPOBJ_FLAG_OFFSCREEN)) || ((obj->flags & MAPOBJ_FLAG_COLOR) && obj->color.a == 0))) {
		if (obj->flags & MAPOBJ_FLAG_DISPLAY_LIST) {
			for (i = 0; i < obj->joints->partCount; i++) {
				MapMaterial* material = obj->joints->parts[i].material;
				MapFileMesh* mesh = obj->joints->parts[i].mesh;

				mapSetTextureMatrix(material);
				mapSetMaterialLastStageBlend(obj->flags, obj->color, material->field_0x10C);
				mapSetPolygonVtxDesc(obj, mesh);
				GXCallDisplayList(obj->dlBuf[i], obj->dlSize[i]);
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
				MapMaterial* material = obj->joints->parts[i].material;
				MapFileMesh* mesh = obj->joints->parts[i].mesh;

				mapSetTextureMatrix(material);
				mapSetMaterialLastStageBlend(obj->flags, obj->color, material->field_0x10C);
				mapSetMaterial(obj, material);
				mapSetPolygon(obj, mesh);
			}
		}
	}

	if (obj->flags & 0x8000) {
		GXSetProjection(cam->projection, cam->projType);
	}
	if (obj->flags & MAPOBJ_TEV_CALLBACK_MASK) {
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
	if ((obj->flags & MAPOBJ_FLAG_IN_GROUP) && obj->next) _mapDispMapGrp(camId, obj->next);
}

void mapDispMapObj(CameraId camId, MapObject* obj) {
	_mapDispMapObj(camId, obj);
}

void mapDispMapGrp(CameraId camId, MapObject* obj) {
	_mapDispMapGrp(camId, obj);
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
	offscreenAddBoundingBox(obj->offScreenId, v0, v1, v2, v3);
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
	offscreenAddBoundingBox(obj->offScreenId, v0, v1, v2, v3);
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
			flags = obj->flags;
			if (flags & 0x400000) {
				obj->flags = flags & ~MAPOBJ_FLAG_CLIP;
				_mapDispMapObj(camId, obj);
				obj->flags = flags;
			}
		}
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
	color = (GXColor){0, 0, 0, 0};
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

void _mapDispMapGrp_NoMaterial(MapObject* obj, void* a2) {
	MapObject* o = (MapObject*)a2;

	_mapDispMapObj_NoMaterial((s32)obj, o);
	// the redundant casts colour the camera id to r30 and o to r31
	if (o->child)
		_mapDispMapGrp_NoMaterial((MapObject*)obj, o->child);
	if ((o->flags & MAPOBJ_FLAG_IN_GROUP) && o->next)
		_mapDispMapGrp_NoMaterial((MapObject*)obj, o->next);
}

void mapDispMapObj_bbox(s32 camId, MapObject* obj) {
	sysWaitDrawSync();
	GXClearBoundingBox();
	_mapDispMapObj_NoMaterial(camId, obj);
	sysWaitDrawSync();
	GXReadBoundingBox(&obj->bboxLeft, &obj->bboxTop, &obj->bboxRight, &obj->bboxBottom);
}

void mapDispMapGrp_bbox(MapObject* obj, MapObject* group) {
	sysWaitDrawSync();
	GXClearBoundingBox();
	_mapDispMapGrp_NoMaterial(obj, group);
	sysWaitDrawSync();
	GXReadBoundingBox(&group->bboxLeft, &group->bboxTop, &group->bboxRight, &group->bboxBottom);
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
	if (mapDispDisabled != 0) {
		return;
	}
	entry = &wp->entries[0];
	for (i = 0; i < wp->entries[0].count; entry++, i++) {
		if (entry->rootMapObj != NULL) {
			PSMTXScale(scale, 10.0f, 10.0f, 10.0f);
			mapReCalcMatrix(entry->rootMapObj, scale, 0);
		}
		obj = entry->objects;
		for (j = 0; j < entry->numJoints; j++, obj++) {
#define OBJ_FLAGS (obj->flags)
			if (OBJ_FLAGS & MAPOBJ_FLAG_NO_DISP) continue;
			if ((OBJ_FLAGS & MAPOBJ_FLAG_HIDDEN) && !(OBJ_FLAGS & MAPOBJ_FLAG_OFFSCREEN)) continue;
			if (OBJ_FLAGS & MAPOBJ_FLAG_COLOR) {
				if (obj->color.a == 0) continue;
			}
			if (OBJ_FLAGS & MAPOBJ_FLAG_BBOX) {
				obj->bboxRight = 0;
				obj->bboxBottom = 0;
				obj->bboxLeft = 0;
				obj->bboxTop = 0;
			}
			if (OBJ_FLAGS & MAPOBJ_FLAG_IN_GROUP) continue;
			if (!(OBJ_FLAGS & MAPOBJ_FLAG_GROUP)) {
				if (obj->joints->partCount < 1) continue;
			}
			if (!(OBJ_FLAGS & MAPOBJ_FLAG_NO_BLEND)) {
				if ((entry->flags & 2) && entry->blend[0].a == 0) continue;
			}
			if (!(OBJ_FLAGS & MAPOBJ_FLAG_NO_BLEND1)) {
				if ((entry->flags & 4) && entry->blend[1].a == 0) continue;
			}
			PSMTXConcat(cam->view, obj->modelWorldMtx, mtx);
			if ((OBJ_FLAGS & MAPOBJ_FLAG_CLIP) && !mapClipOffFlag) {
				joint = obj->joints;
				// the first corner inside the view jumps past the rest of the unrolled tests (goto visible)
#define CLIP_TEST(cx, cy, cz) \
	clipPos.x = (cx); \
	clipPos.y = (cy); \
	clipPos.z = (cz); \
	PSMTXMultVec(mtx, &clipPos, &clipPos); \
	PSMTX44MultVec(cam->projection, &clipPos, &clipPos); \
	clipV = clipPos.z; \
	if (clipV > -1.0f && clipV < 0.0f) { \
		clipV = clipPos.x; \
		if (clipV >= -1.0f && clipV <= 1.0f) { \
			clipV = clipPos.y; \
			if (clipV >= -1.0f && clipV <= 1.0f) goto visible; \
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
				if (clipV > -1.0f && clipV < 0.0f) {
					clipV = clipPos.x;
					if (clipV >= -1.0f && clipV <= 1.0f) {
						clipV = clipPos.y;
						if (clipV >= -1.0f && clipV <= 1.0f) goto visible;
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
			PSMTXMultVec(mtx, &obj->center, &dispPos);
			PSMTX44MultVec(cam->projection, &dispPos, &dispPos);
			order = 5000.0f * dispPos.z + 5000.0f;
			if (order < 0.0f) {
				order = 0.0f;
			}
			else if (order > 10000.0f) {
				order = 10000.0f;
			}
			order *= -1.0f;
			id = obj->renderMode;
			if (obj->joints->partCount > 0) {
				color = obj->color;
				matColor = obj->joints->parts[0].material->field_0x10C;
				if (mapTestXLU(obj->flags, &color, &matColor)) {
					id = 2;
				}
			}
			if (obj->flags & MAPOBJ_FLAG_BBOX) {
				dispEntry(CAMERA_OFFSCREEN2, 5000.0f, 3, !(obj->flags & MAPOBJ_FLAG_GROUP) ? (DispCallback)mapDispMapObj_bbox : (DispCallback)mapDispMapGrp_bbox, obj);
			}
			if (obj->flags & MAPOBJ_FLAG_OFFSCREEN) {
				dispEntry(CAMERA_OFFSCREEN2, order, id, !(obj->flags & MAPOBJ_FLAG_GROUP) ? (DispCallback)mapDispMapObj_off : (DispCallback)mapDispMapGrp_off, obj);
			}
			if (!(obj->flags & MAPOBJ_FLAG_HIDDEN)) {
				dispEntry(obj->cameraId, order, id, !(obj->flags & MAPOBJ_FLAG_GROUP) ? (DispCallback)mapDispMapObj : (DispCallback)mapDispMapGrp, obj);
			}
		}
	}
}

s32 mapSetLight(MapObject* obj, LightEntry** entries) {
	Vec pos;
	s32 count = 0;
	s32 i;

	PSMTXMultVec(obj->modelWorldMtx, &obj->center, &pos);

	if (obj->joints->drawMode->unk2 & 0x8) {
		if (obj->joints->drawMode->unk4 == 0) {
			count = lightGetNearObj(&pos, entries, 8, 0);
		}
		else {
			for (i = 0; i < 0x20; i++) {
				if (obj->joints->drawMode->unk4 & (1 << i)) {
					MapEntry* entry = &mapWork[activeGroup].entries[obj->entryIdx];
					MapFileLightTable* list = entry->lightTable;
					LightEntry* l = lightNameToPtr(list->lights[i]->name);

					if (!(l->flags & LIGHT_AMBIENT)) {
						entries[count] = l;
						count++;
						if (count >= 8) break;
					}
				}
			}
			if (count < 8) count += lightGetNearObj(&pos, entries + count, 8 - count, 0);
		}
	}
	else {
		if (obj->joints->drawMode->unk4 != 0) {
			for (i = 0; i < 0x20; i++) {
				if (obj->joints->drawMode->unk4 & (1 << i)) {
					MapEntry* entry = &mapWork[activeGroup].entries[obj->entryIdx];
					MapFileLightTable* list = entry->lightTable;
					LightEntry* l = lightNameToPtr(list->lights[i]->name);

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

void mapSetMaterialTev(s32 texCount, s32 drawMode, s32 flags, Mtx mtx) {
	static f32 scrl_uv[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	GXTexMapID texMap;
	u32 texCoord;
	GXTevStageID tevStage;
	s32 numIndStages;
	Mtx queen_mtx;
#ifdef MUST_MATCH
	f32 wave;
#endif
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

			case 8:
#ifdef MUST_MATCH
				// the sine in its own comma step pools 2pi and 360 ahead of 127, as retail's .sdata2 has them.
				// The plain expression (any operand order, or with the angle in a statement before it) pools 127 first,
				// and `wave = sin(...);` as its own statement moves the GXColor template load after the call.
				GXSetTevColor(GX_TEVREG0, (GXColor){0, (wave = sin((TWO_PI * (f32)(2 * gp->retraceCount)) / 360.0f), (u8)((127.0f * wave) + 127.0f)), 0, 0});
#else
				GXSetTevColor(GX_TEVREG0, (GXColor){0, (u8)((127.0f * (f32)sin((TWO_PI * (f32)(2 * gp->retraceCount)) / 360.0f)) + 127.0f), 0, 0});
#endif
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_RASC, GX_CC_TEXC, GX_CC_C0);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_RASA, GX_CA_TEXA, GX_CA_A0);
				GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
				tevStage = GX_TEVSTAGE1;
				break;

			case 9: {
				f32* p = scrl_uv;
				f32* q = queen_uv;
				s32 i;
				for (i = 0; i < 4; i++) {
					p[i] += q[i];
					if (p[i] > 100.0f) {
						p[i] -= 100.0f;
					}
					if (p[i] < -100.0f) {
						p[i] += 100.0f;
					}
				}
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
				GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, texmtx_tbl[texMap], GX_FALSE, GX_PTIDENTITY);
				GXLoadTexMtxImm(mtx2, texmtx_tbl[texMap], GX_MTX3x4);
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
		GXColor color;

		GXSetTevColorOp(tevStage - 1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
		GXSetTevAlphaOp(tevStage - 1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
		PSMTXConcat(projMtx, mtx, mtx2);
		GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, texmtx_tbl[texMap], GX_FALSE, GX_PTIDENTITY);
		GXLoadTexMtxImm(mtx2, texmtx_tbl[texMap], GX_MTX3x4);
		GXLoadTexObj(smartTexObj(&projTexObj, (void**)projTexImgRef), texMap);
		GXSetTevOrder(tevStage, texCoord, texMap, GX_COLOR_NULL);
		GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_2, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C1);
		GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_TEXA);
		GXSetTevSwapMode(tevStage, GX_TEV_SWAP3, GX_TEV_SWAP3);
		GXSetTevSwapModeTable(GX_TEV_SWAP3, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_RED);
		color = (GXColor){0, 0, 0, 0xFF};
		GXSetTevColor(GX_TEVREG1, color);
		tevStage++;
		texMap++;
		texCoord++;
		GXSetTevOrder(tevStage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
		GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(tevStage, GX_CC_C0, GX_CC_CPREV, GX_CC_APREV, GX_CC_ZERO);
		GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_A0);
		GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
		tevStage++;
	}

	if (flags & 0x10000) {
		if (shadowGetDepthShadowTexObj() != NULL) {
			GXTexObj* depthTex = shadowGetDepthShadowTexObj();
			void* depthMtx = shadowGetDepthShadowDepthMtx();
			void* depthProjMtx = shadowGetDepthShadowProjTexMtx();
			GXColor color = shadowGetDepthShadowColor();
			ShadowConfigFields* config = shadowGetDepthShadowConfig();
			Mtx mtx2;
			GXTevStageID stage;

			switch (config->depthMode) {
			case 0:
				GXSetTevColorOp(tevStage - 1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
				GXSetTevAlphaOp(tevStage - 1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
				GXSetTevOrder(tevStage, texCoord, texMap, GX_COLOR_NULL);
				GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
				GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
				GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, texmtx_tbl[texMap], GX_FALSE, GX_PTIDENTITY);
				GXLoadTexObj(shadowGetRamp8TexObj(), texMap);
				PSMTXConcat(depthMtx, mtx, mtx2);
				GXLoadTexMtxImm(mtx2, texmtx_tbl[texMap], GX_MTX3x4);
				stage = tevStage + 1;
				texMap++;
				{
					GXTexCoordID coord = texCoord + 1;

					GXSetTevOrder(stage, coord, texMap, GX_COLOR_NULL);
					GXSetTevColorIn(stage, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ONE, GX_CC_ZERO);
					GXSetTevColorOp(stage, GX_TEV_COMP_R8_GT, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
					GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
					GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
					GXSetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP0);
					GXSetTexCoordGen2(coord, GX_TG_MTX3x4, GX_TG_POS, texmtx_tbl[texMap], GX_FALSE, GX_PTIDENTITY);
					GXLoadTexObj(depthTex, texMap);
					PSMTXConcat(depthProjMtx, mtx, mtx2);
					GXLoadTexMtxImm(mtx2, texmtx_tbl[texMap], GX_MTX3x4);
				}
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
				GXSetTevColor(GX_TEVREG1, (GXColor){0xFF, 0, 0, 0});
				GXSetTevOrder(tevStage, texCoord, texMap, GX_COLOR_NULL);
				GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C1, GX_CC_ZERO);
				GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
				GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, texmtx_tbl[texMap], GX_FALSE, GX_PTIDENTITY);
				GXLoadTexObj(shadowGetRamp8TexObj(), texMap);
				PSMTXConcat(depthMtx, mtx, mtx2);
				GXLoadTexMtxImm(mtx2, texmtx_tbl[texMap], GX_MTX3x4);
				tevStage++;
				texMap++;
				texCoord++;
				break;
			case 2:
				GXSetTevColor(GX_TEVREG1, (GXColor){0xFF, 0, 0, 0});
				GXSetTevOrder(tevStage, texCoord, texMap, GX_COLOR_NULL);
				GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C1, GX_CC_ZERO);
				GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
				GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, texmtx_tbl[texMap], GX_FALSE, GX_PTIDENTITY);
				GXLoadTexObj(depthTex, texMap);
				PSMTXConcat(depthProjMtx, mtx, mtx2);
				GXLoadTexMtxImm(mtx2, texmtx_tbl[texMap], GX_MTX3x4);
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
				GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, texmtx_tbl[texMap], GX_FALSE, GX_PTIDENTITY);
				GXLoadTexObj(shadowGetRamp16TexObj(), texMap);
				PSMTXConcat(depthMtx, mtx, mtx2);
				GXLoadTexMtxImm(mtx2, texmtx_tbl[texMap], GX_MTX3x4);
				stage = tevStage + 1;
				texMap++;
				{
					GXTexCoordID coord = texCoord + 1;

					GXSetTevOrder(stage, coord, texMap, GX_COLOR_NULL);
					GXSetTevColorIn(stage, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ONE, GX_CC_ZERO);
					GXSetTevColorOp(stage, GX_TEV_COMP_GR16_GT, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
					GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
					GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
					GXSetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP2);
					GXSetTexCoordGen2(coord, GX_TG_MTX3x4, GX_TG_POS, texmtx_tbl[texMap], GX_FALSE, GX_PTIDENTITY);
					GXLoadTexObj(depthTex, texMap);
					PSMTXConcat(depthProjMtx, mtx, mtx2);
					GXLoadTexMtxImm(mtx2, texmtx_tbl[texMap], GX_MTX3x4);
				}
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
				GXSetTevColor(GX_TEVREG1, (GXColor){0xFF, 0xFF, 0, 0});
				GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_ALPHA, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
				GXSetTevOrder(tevStage, texCoord, texMap, GX_COLOR_NULL);
				GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C1, GX_CC_ZERO);
				GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
				GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP1);
				GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, texmtx_tbl[texMap], GX_FALSE, GX_PTIDENTITY);
				GXLoadTexObj(shadowGetRamp16TexObj(), texMap);
				PSMTXConcat(depthMtx, mtx, mtx2);
				GXLoadTexMtxImm(mtx2, texmtx_tbl[texMap], GX_MTX3x4);
				tevStage++;
				texMap++;
				texCoord++;
				break;
			case 5:
				GXSetTevColor(GX_TEVREG1, (GXColor){0xFF, 0xFF, 0, 0});
				GXSetTevSwapModeTable(GX_TEV_SWAP1, GX_CH_RED, GX_CH_ALPHA, GX_CH_BLUE, GX_CH_ALPHA);
				GXSetTevOrder(tevStage, texCoord, texMap, GX_COLOR_NULL);
				GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_TEXC, GX_CC_C1, GX_CC_ZERO);
				GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
				GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP1);
				GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, texmtx_tbl[texMap], GX_FALSE, GX_PTIDENTITY);
				GXLoadTexObj(depthTex, texMap);
				PSMTXConcat(depthProjMtx, mtx, mtx2);
				GXLoadTexMtxImm(mtx2, texmtx_tbl[texMap], GX_MTX3x4);
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
			GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, texmtx_tbl[texMap], GX_FALSE, GX_PTIDENTITY);
			GXLoadTexObj(projTex, texMap);
			PSMTXConcat(projShadowMtx, mtx, mtx2);
			GXLoadTexMtxImm(mtx2, texmtx_tbl[texMap], GX_MTX3x4);
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
			ShadowConfigFields* config = shadowGetDepthShadowConfig();
			Mtx mtx2;
			GXTevStageID stage;

			switch (config->depthMode) {
			case 0:
				GXSetTevColorOp(tevStage - 1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
				GXSetTevAlphaOp(tevStage - 1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVREG0);
				GXSetTevOrder(tevStage, texCoord, texMap, GX_COLOR_NULL);
				GXSetTevColorIn(tevStage, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_TEXC);
				GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
				GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
				GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, texmtx_tbl[texMap], GX_FALSE, GX_PTIDENTITY);
				GXLoadTexObj(shadowGetRamp8TexObj(), texMap);
				PSMTXConcat(depthMtx, mtx, mtx2);
				GXLoadTexMtxImm(mtx2, texmtx_tbl[texMap], GX_MTX3x4);
				stage = tevStage + 1;
				texMap++;
				{
					GXTexCoordID coord = texCoord + 1;

					GXSetTevOrder(stage, coord, texMap, GX_COLOR_NULL);
					GXSetTevColorIn(stage, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ONE, GX_CC_ZERO);
					GXSetTevColorOp(stage, GX_TEV_COMP_R8_GT, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
					GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
					GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
					GXSetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP0);
					GXSetTexCoordGen2(coord, GX_TG_MTX3x4, GX_TG_POS, texmtx_tbl[texMap], GX_FALSE, GX_PTIDENTITY);
					GXLoadTexObj(depthTex, texMap);
					PSMTXConcat(depthProjMtx, mtx, mtx2);
					GXLoadTexMtxImm(mtx2, texmtx_tbl[texMap], GX_MTX3x4);
				}
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
				GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, texmtx_tbl[texMap], GX_FALSE, GX_PTIDENTITY);
				GXLoadTexObj(shadowGetRamp16TexObj(), texMap);
				PSMTXConcat(depthMtx, mtx, mtx2);
				GXLoadTexMtxImm(mtx2, texmtx_tbl[texMap], GX_MTX3x4);
				stage = tevStage + 1;
				texMap++;
				{
					GXTexCoordID coord = texCoord + 1;

					GXSetTevOrder(stage, coord, texMap, GX_COLOR_NULL);
					GXSetTevColorIn(stage, GX_CC_CPREV, GX_CC_TEXC, GX_CC_ONE, GX_CC_ZERO);
					GXSetTevColorOp(stage, GX_TEV_COMP_GR16_GT, GX_TB_ZERO, GX_CS_SCALE_1, GX_FALSE, GX_TEVPREV);
					GXSetTevAlphaIn(stage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO);
					GXSetTevAlphaOp(stage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
					GXSetTevSwapMode(stage, GX_TEV_SWAP0, GX_TEV_SWAP2);
					GXSetTexCoordGen2(coord, GX_TG_MTX3x4, GX_TG_POS, texmtx_tbl[texMap], GX_FALSE, GX_PTIDENTITY);
					GXLoadTexObj(depthTex, texMap);
					PSMTXConcat(depthProjMtx, mtx, mtx2);
					GXLoadTexMtxImm(mtx2, texmtx_tbl[texMap], GX_MTX3x4);
				}
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
		s32 red;
		s32 green;
		s32 blue;
		s32 alpha;
		f32 c;

		if (flags & MAPOBJ_FLAG_FLUSH) {
			red = current_mp->color.r;
			green = current_mp->color.g;
			blue = current_mp->color.b;
			alpha = current_mp->color.a;
		}
		else {
			red = 0xF0;
			green = 0xF0;
			blue = 0x64;
			c = (f32)cos(1.5707964f * (f32)gp->retraceCount / 20.0f);
			alpha = (u8)(64.0f * (1.0f - c) / 2.0f + 64.0f);
		}
		GXSetTevOrder(tevStage, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
		GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(tevStage, GX_CC_CPREV, GX_CC_ZERO, GX_CC_ZERO, GX_CC_KONST);
		GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
		GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
		GXSetTevKColor(GX_KCOLOR1, (GXColor){red * alpha / 255, green * alpha / 255, blue * alpha / 255, 0xFF});
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
		f32 c;

		camPos = (Vec){0.0f, 0.0f, 1.0f};
		up = (Vec){0.0f, 1.0f, 0.0f};
		target = (Vec){0.0f, 0.0f, 0.0f};
		c = (f32)cos(1.5707964f * (f32)gp->retraceCount / 30.0f);
		v = (s32)(127.0f * (1.0f - c) / 2.0f + 128.0f);
		GXInitTexObj(&texObj, texture_0, 4, 0x40, GX_TF_CMPR, GX_CLAMP, GX_REPEAT, GX_FALSE);
		GXLoadTexObj(&texObj, texMap);
		C_MTXLightOrtho(orthoMtx, -2.0f, 2.0f, -2.0f, 2.0f, 0.5f, 0.5f, 0.5f, (f32)gp->retraceCount / 50.0f);
		C_MTXLookAt(lookAtMtx, &camPos, &up, &target);
		PSMTXConcat(orthoMtx, lookAtMtx, orthoMtx);
		GXLoadTexMtxImm(orthoMtx, texmtx_tbl[texMap], GX_MTX3x4);
		GXSetTexCoordGen2(texCoord, GX_TG_MTX3x4, GX_TG_POS, texmtx_tbl[texMap], GX_FALSE, GX_PTIDENTITY);
		GXSetTevOrder(tevStage, texCoord, texMap, GX_COLOR_NULL);
		GXSetTevColorOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(tevStage, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(tevStage, GX_CC_TEXC, GX_CC_CPREV, GX_CC_KONST, GX_CC_ZERO);
		GXSetTevAlphaIn(tevStage, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_APREV);
		GXSetTevSwapMode(tevStage, GX_TEV_SWAP0, GX_TEV_SWAP0);
		GXSetTevKColor(GX_KCOLOR1, (GXColor){v, v, v, 0xFF});
		GXSetTevKColorSel(tevStage, GX_TEV_KCSEL_K1);
		tevStage++;
		texMap++;
		texCoord++;
	}

	if (flags & MAPOBJ_TEV_CALLBACK_MASK) {
		MapTevCallback work;
		MapTevCallback* workPtr = &work;
		void (*callback)(MapTevCallback*);
		u32 cbIndex;
		u8* cbBase;

		workPtr->obj = current_mp;
		workPtr->mtx = mtx;
		workPtr->drawMode = drawMode;
		workPtr->tevStage = tevStage;
		workPtr->texMap = texMap;
		workPtr->texCoord = texCoord;
		workPtr->numIndStage = 0;
		cbIndex = (u32)(flags & MAPOBJ_TEV_CALLBACK_MASK) >> 22 & 0x3FC;
		cbBase = (u8*)mapWork[activeGroup].entries[0].tevCallback;
		callback = *(void**)(cbBase + cbIndex);
		if (callback != NULL) {
			callback(workPtr);
		}
		tevStage = work.tevStage;
		texCoord = work.texCoord;
		numIndStages = work.numIndStage;
	}

	GXSetNumTexGens((u8)texCoord);
	GXSetNumTevStages((u8)tevStage);
	GXSetNumIndStages((u8)numIndStages);
}

void mapSetMaterialLastStageBlend(s32 flags, GXColor color1, GXColor color2) {
	MapWork* wp = mapGetWork();
	GXColor color = {0xFF, 0xFF, 0xFF, 0xFF};

	if (flags & MAPOBJ_FLAG_COLOR) {
		color.r = color.r * color1.r / 255;
		color.g = color.g * color1.g / 255;
		color.b = color.b * color1.b / 255;
		color.a = color.a * color1.a / 255;
	}
	if (color2.r != 0xFF) {
		color.a = color.a * color2.r / 255;
	}
	if ((wp->entries[0].flags & 0x2) && !(flags & MAPOBJ_FLAG_NO_BLEND)) {
		color.r = color.r * wp->entries[0].blend[0].r / 255;
		color.g = color.g * wp->entries[0].blend[0].g / 255;
		color.b = color.b * wp->entries[0].blend[0].b / 255;
		color.a = color.a * wp->entries[0].blend[0].a / 255;
	}
	if ((wp->entries[0].flags & 0x4) && !(flags & MAPOBJ_FLAG_NO_BLEND1)) {
		color.r = color.r * wp->entries[0].blend[1].r / 255;
		color.g = color.g * wp->entries[0].blend[1].g / 255;
		color.b = color.b * wp->entries[0].blend[1].b / 255;
		color.a = color.a * wp->entries[0].blend[1].a / 255;
	}
	if ((wp->entries[0].flags & 0x8) && !(flags & MAPOBJ_FLAG_NO_BLEND)) {
		color.r = color.r * wp->entries[0].blend[2].r / 255;
		color.g = color.g * wp->entries[0].blend[2].g / 255;
		color.b = color.b * wp->entries[0].blend[2].b / 255;
		color.a = color.a * wp->entries[0].blend[2].a / 255;
	}
	GXSetTevKColor(GX_KCOLOR0, color);
}

s32 fog_type[5] = {2, 4, 5, 6, 7};

void mapSetMaterialFog(void) {
	CameraEntry* cam = camGetCurPtr();
	MapEntry* entry = &mapWork[activeGroup].entries[0];

	if (entry->flags & 0x1) {
		GXColor color = entry->fogColor;
		GXSetFog(fog_type[entry->fogType], entry->fogStart, entry->fogEnd, cam->near, cam->far, color);
	}
	else {
		GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
	}
}

// Objects with the 0x80000 flag suppress the map fog.
static inline void mapDrawObjectFog(MapObject* obj) {
	CameraEntry* cur = camGetCurPtr();
	MapEntry* entry = &mapWork[activeGroup].entries[0];

	if ((entry->flags & 0x1) && !(obj->flags & MAPOBJ_FLAG_NO_FOG)) {
		GXSetFog(fog_type[entry->fogType], entry->fogStart, entry->fogEnd, cur->near, cur->far, entry->fogColor);
	}
	else {
		GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, (GXColor){0xFF, 0xFF, 0xFF, 0xFF});
	}
}

void mapSetTextureMatrix(MapMaterial* mat) {
	s32 tc = 0;
	s32 j = 0;
	s32 i = mat->texCount - 1;
	Mtx mtx, mtx3, mtx5, mtx2, mtx4;

	for (; i >= 0; tc++, j++, i--) {
		f32 f1 = mat->entries[i].rotation;
		f32 rots = -f1;
		f32 f5 = mat->entries[i].scaleX;
		f32 sx = f5;
		f32 f6 = mat->entries[i].scaleY;
		f32 sy = f6;
		f32 tx = mat->entries[i].transX;
		f32 f0 = mat->entries[i].transY;
		f32 zero = 0.0f;
		f32 ty = -f0;

		if (mat->entries[i].scaleX != 1.0f || mat->entries[i].scaleY != 1.0f || tx != 0.0f || ty != 0.0f || rots != 0.0f) {
			if (rots != zero) {
				f32 f25 = 0.5f * mat->entries[i].field_0x14 * mat->entries[i].scaleX;
				f32 f24 = (1.0f - 0.5f * mat->entries[i].field_0x18) * mat->entries[i].scaleY;
				PSMTXTrans(mtx, f25, f24, 0.0f);
				PSMTXRotRad(mtx2, 'z', 0.017453292f * rots);
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
			GXSetTexCoordGen2(tc, 1, mat->texCount + 3 - j, texmtx_tbl[i], 0, 0x7D);
		}
		else {
			GXSetTexCoordGen2(tc, 1, mat->texCount + 3 - j, GX_IDENTITY, 0, 0x7D);
		}
	}
}

// The index of the texture called `name` in a map's texture name table (a count, then the names), or -1.
static inline s32 mapMatSearchTex(s32* table, const char* name) {
	s32 i;

	for (i = 0; i < table[0]; i++) {
		if (strcmp(((char**)table)[i + 1], name) == 0) {
			return i;
		}
	}
	return -1;
}

// Loads the map object's lights into GX light slots (point, spot and directional), sets the colour channel from
// the material, loads each of the material's textures by name from the map's TPL, then sets up TEV.
void mapSetMaterial(MapObject* obj, void* materialPtr) {
	LightEntry* light;
	s32 texMap;
	MapWork* wp;
	u32 lightMask;
	s32 attn;
	s32 i;
	MapMaterial* mat;
	GXLightObj lightObj;
	Mtx mtxX;
	Mtx mtxY;
	Mtx mtxZ;
	LightEntry* lights[8];
	GXTexObj texObj;
	Vec pos;
	Vec defaultDir;
	GXColor ambient;
	GXColor defaultAmbient;
	s32 count;

	mat = materialPtr;
	texMap = 0;
	attn = 2;
	defaultAmbient = (GXColor){0, 0, 0, 0xFF};
	count = mapSetLight(obj, lights);
	lightMask = 0;

	for (i = 0; i < count; i++) {

		defaultDir = (Vec){0.0f, 0.0f, -1.0f};
		light = lights[i];
		if (light->flags & LIGHT_POINT) {
			CameraEntry* cam = camGetCurPtr();

			PSMTXMultVec(cam->view, &light->position, &pos);
			GXInitLightPos(&lightObj, pos.x, pos.y, pos.z);
			GXInitLightColor(&lightObj, lights[i]->color);
			GXInitLightAttn(&lightObj, lights[i]->attenAngle, 0.0f, 0.0f,
			                lights[i]->attenDistCoef.x, lights[i]->attenDistCoef.y, lights[i]->attenDistCoef.z);
			GXLoadLightObjImm(&lightObj, lightid_tbl[i]);
			attn = 1;
		}
		else if (light->flags & LIGHT_SPOT) {
			PSMTXRotRad(mtxX, 'x', 0.017453292f * light->rotation.x);
			PSMTXRotRad(mtxY, 'y', 0.017453292f * lights[i]->rotation.y);
			PSMTXRotRad(mtxZ, 'z', 0.017453292f * lights[i]->rotation.z);
			PSMTXConcat(mtxZ, mtxY, mtxY);
			PSMTXConcat(mtxY, mtxX, mtxX);
			PSMTXMultVec(mtxX, &defaultDir, &defaultDir);
			PSVECScale(&defaultDir, &defaultDir, -1048576.0f);
			PSVECAdd(&lights[i]->position, &defaultDir, &pos);
			{
				CameraEntry* cam = camGetCurPtr();

				PSMTXMultVec(cam->view, &pos, &pos);
			}
			GXInitLightPos(&lightObj, pos.x, pos.y, pos.z);
			GXInitLightColor(&lightObj, lights[i]->color);
			GXInitLightAttn(&lightObj, lights[i]->attenAngle, 0.0f, 0.0f,
			                0.0f, 0.0f, 1.0f);
			GXLoadLightObjImm(&lightObj, lightid_tbl[i]);
		}
		else if (light->flags & LIGHT_DIRECTIONAL) {
			CameraEntry* cam = camGetCurPtr();

			PSMTXMultVec(cam->view, &light->position, &pos);
			PSMTXRotRad(mtxX, 'x', 0.017453292f * lights[i]->rotation.x);
			PSMTXRotRad(mtxY, 'y', 0.017453292f * lights[i]->rotation.y);
			PSMTXRotRad(mtxZ, 'z', 0.017453292f * lights[i]->rotation.z);
			PSMTXConcat(mtxZ, mtxY, mtxY);
			PSMTXConcat(mtxY, mtxX, mtxX);
			PSMTXMultVec(mtxX, &defaultDir, &defaultDir);
			cam = camGetCurPtr();
			PSMTXMultVecSR(cam->view, &defaultDir, &defaultDir);
			GXInitLightPos(&lightObj, pos.x, pos.y, pos.z);
			GXInitLightDir(&lightObj, defaultDir.x, defaultDir.y, defaultDir.z);
			GXInitLightSpot(&lightObj, lights[i]->spotAngle / 2.0f, GX_SP_SHARP);
			GXInitLightDistAttn(&lightObj, 1000.0f, 0.5f, GX_DA_OFF);
			GXInitLightColor(&lightObj, lights[i]->color);
			GXLoadLightObjImm(&lightObj, lightid_tbl[i]);
			attn = 1;
		}
		lightMask |= lightid_tbl[i];
	}

	{
		LightEntry* amb = lightGetAmbient();

		if (amb != NULL) {
			ambient = amb->color;
		}
		else {
			ambient = defaultAmbient;
		}
	}
	GXSetNumChans(1);
	GXSetChanAmbColor(GX_COLOR0A0, ambient);
	GXSetChanMatColor(GX_COLOR0A0, mat->color);

	switch (mat->field_0x8) {
	case 0:
		GXSetChanCtrl(GX_COLOR0, count > 0, GX_SRC_REG, GX_SRC_REG, lightMask, GX_DF_CLAMP, attn);
		GXSetChanCtrl(GX_ALPHA0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
		break;
	case 1:
		GXSetChanCtrl(GX_COLOR0, count > 0, GX_SRC_REG, GX_SRC_VTX, lightMask, GX_DF_CLAMP, attn);
		GXSetChanCtrl(GX_ALPHA0, GX_FALSE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
		break;
	}

	for (i = mat->texCount - 1; i >= 0; i--) {
		MapMatTexRef* ref = mat->texRefs[i];
		TEXDescriptor* desc;

		wp = &mapWork[activeGroup];
		desc = TEXGet(wp->entries[obj->entryIdx].tpl,
		              mapMatSearchTex(wp->entries[obj->entryIdx].textureTable,
		                              *(const char**)ref->field_0x0));
		GXInitTexObj(&texObj, desc->textureHeader->data, desc->textureHeader->width, desc->textureHeader->height,
		             desc->textureHeader->format, texwrap_tbl[ref->wrapS], texwrap_tbl[ref->wrapT],
		             desc->textureHeader->minLOD == desc->textureHeader->maxLOD ? GX_FALSE : GX_TRUE);
		GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, (f32)desc->textureHeader->minLOD,
		                (f32)desc->textureHeader->maxLOD, desc->textureHeader->LODBias, GX_DISABLE,
		                desc->textureHeader->edgeLODEnable, GX_ANISO_1);
		GXLoadTexObj(&texObj, texMap);
		texMap++;
	}

	mapSetMaterialTev(mat->texCount, mat->field_0x110 ? mat->field_0x110[0] : 0,
	                  obj->flags, obj->modelWorldMtx);
}

void mapSetPaperAmbColor(GXColor* color) {
	paper_ambient = *color;
}

void mapResetPaperAmbColor(void) {
	paper_ambient = (GXColor){0x70, 0x70, 0x70, 0xFF};
}

void mapResetPaperCraftAmbColor(void) {
	paperCraft_ambient = (GXColor){0x70, 0x70, 0x70, 0xFF};
}


// 1:1
void mapSetMaterialLight(s32 flags, Vec position) {
	LightEntry* lights[8];
	LightEntry* light;
	LightEntry* ambient;
	CameraEntry* cam;
	GXLightObj lightObj;
	GXColor fogColor = {0xFF, 0xFF, 0xFF, 0xFF};
	GXColor ambColor = {0, 0, 0, 0xFF};
	GXColor chanAmbColor;
	Vec pos;
	Vec dir;
	Mtx rotX, rotY, rotZ;
	s32 count = 0;
	u32 lightMask;
	s32 attn = 2;
	s32 i;


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
			count += lightGetNearObj(&position, &lights[count], 8 - count, LIGHT_CHARA);
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
				ambColor = (GXColor){0, 0, 0, 0xFF};
			}
		}
		else {
			count = lightGetNearObj(&position, lights, 8, LIGHT_CHARA);
		}
	}

	lightMask = 0;
	for (i = 0; i < count; i++) {
		/* The target keeps lights[i] in a callee-saved register for the flags test, the point
		   position and the first spot rotation, but re-materializes it from the array for the
		   remaining fields.  The light/lights[i] mix below reproduces that allocation; both
		   forms are semantically identical. */
		dir = (Vec){0.0f, 0.0f, -1.0f};
		light = lights[i];

		if (light->flags & LIGHT_POINT) {
			cam = camGetCurPtr();
			PSMTXMultVec(cam->view, &light->position, &pos);
			GXInitLightPos(&lightObj, pos.x, pos.y, pos.z);
			GXInitLightColor(&lightObj, lights[i]->color);
			if (lights[i]->flags & LIGHT_NO_DIST_ATTEN) {
				GXInitLightAttn(&lightObj, lights[i]->attenAngle, 0.0f, 0.0f,
				                1.0f, 0.0f, 0.0f);
			}
			else {
				GXInitLightAttn(&lightObj, lights[i]->attenAngle, 0.0f, 0.0f,
				                lights[i]->attenDistCoef.x, lights[i]->attenDistCoef.y, lights[i]->attenDistCoef.z);
				attn = 1;
			}
			GXLoadLightObjImm(&lightObj, lightid_tbl[i]);
		}
		else if (light->flags & LIGHT_SPOT) {
			PSMTXRotRad(rotX, 'x', 0.017453292f * light->rotation.x);
			PSMTXRotRad(rotY, 'y', 0.017453292f * lights[i]->rotation.y);
			PSMTXRotRad(rotZ, 'z', 0.017453292f * lights[i]->rotation.z);
			PSMTXConcat(rotZ, rotY, rotY);
			PSMTXConcat(rotY, rotX, rotX);
			PSMTXMultVec(rotX, &dir, &dir);
			PSVECScale(&dir, &dir, -1048576.0f);
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
			PSMTXRotRad(rotX, 'x', 0.017453292f * lights[i]->rotation.x);
			PSMTXRotRad(rotY, 'y', 0.017453292f * lights[i]->rotation.y);
			PSMTXRotRad(rotZ, 'z', 0.017453292f * lights[i]->rotation.z);
			PSMTXConcat(rotZ, rotY, rotY);
			PSMTXConcat(rotY, rotX, rotX);
			PSMTXMultVec(rotX, &dir, &dir);
			cam = camGetCurPtr();
			PSMTXMultVecSR(cam->view, &dir, &dir);
			GXInitLightPos(&lightObj, pos.x, pos.y, pos.z);
			GXInitLightDir(&lightObj, dir.x, dir.y, dir.z);
			GXInitLightSpot(&lightObj, lights[i]->spotAngle / 2.0f, 4);
			GXInitLightDistAttn(&lightObj, 1000.0f, 0.5f, 0);
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

	GXSetFog(GX_FOG_NONE, 0.0f, 0.0f, 0.0f, 0.0f, fogColor);
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

// vertex layout of an unpacked display list (stride 0x18)

// per-mesh vtx-format block (entry->vcdTable)

#pragma opt_common_subs off // stop MWCC CSEing a shared base into the wp/nrm/pos slots
// The vertex-loop `p = &vp->x` scratch is matched by reading z as `p[2]`.
static const GXVtxAttrFmtList attr_fmt[] = {
	{GX_VA_POS, GX_POS_XYZ, GX_F32, 0},
	{GX_VA_NRM, GX_NRM_XYZ, GX_F32, 0},
	{GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0},
	{GX_VA_TEX0, GX_TEX_ST, GX_F32, 0},
	{GX_VA_TEX1, GX_TEX_ST, GX_F32, 0},
	{GX_VA_TEX2, GX_TEX_ST, GX_F32, 0},
	{GX_VA_TEX3, GX_TEX_ST, GX_F32, 0},
	{GX_VA_TEX4, GX_TEX_ST, GX_F32, 0},
	{GX_VA_TEX5, GX_TEX_ST, GX_F32, 0},
	{GX_VA_TEX6, GX_TEX_ST, GX_F32, 0},
	{GX_VA_TEX7, GX_TEX_ST, GX_F32, 0},
	{GX_VA_NULL, GX_POS_XY, GX_U8, 0},
};

void mapSetPolygon(MapObject* obj, MapFileMesh* mesh) {
	MapWork* wp = mapGetWork();
	u32* vcd = (u32*)mesh->vcdTableOffset;
	void* pos = (void*)(vcd[0] + 4);
	void* nrm = (void*)(vcd[1] + 4);
	s32 texCount = vcd[5];
	s32 i;
	u8* p;

	GXSetCullMode(culling[obj->joints->drawMode->cullMode]);

	if (!(wp->entries[obj->entryIdx].flags & 0x8000)) {
		GXClearVtxDesc();
		GXSetVtxAttrFmtv(GX_VTXFMT0, attr_fmt);
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
			MapVtx* v;
			MapVtx* vp;
			s16 t;
			s32 n;
			u8* dp = (u8*)mesh;

			for (i = 0; i < mesh->displayListCount; i++) {
				v = (MapVtx*)*(u32*)(dp + 0x10);

				GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT0, (u16)v->field_0x0);
				vp = v;
				for (n = 0; n < (s32)v->field_0x0; n++) {
					s32 k;
					s16* p = &vp->x;

					GXWGFifo.s16 = vp->x;
					GXWGFifo.s16 = vp->y;
					GXWGFifo.s16 = p[2];
					for (k = 0; k < texCount; k++) {
						t = p[k + 4];
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
		{
			u8* base = (u8*)wp + 0xA4;
			fmt = (MapVtxFmt*)*(void**)(base + obj->entryIdx * 0x178);
		}
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
			fmt = (MapVtxFmt*)wp->entries[obj->entryIdx].vcdTable;
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0 + i, GX_TEX_ST, GX_S16, (u8)fmt->field_0x48[i]);
			GXSetArray(GX_VA_TEX0 + i, (void*)(((u32*)mesh->vcdTableOffset)[6 + i] + 4), 4);
		}

		for (i = 0; i < mesh->displayListCount; i++) {
			GXCallDisplayList((void*)mesh->displayLists.packedLists[i].offset, mesh->displayLists.packedLists[i].nbytes);
		}
	}
}

// `opt_common_subs off` keeps GX_VA_TEX0+i from being CSE-hoisted
// GX setup order/constants and addressing match. The `base = (u8*)wp + 0xA4` local gives the first
// vcd_table load the original's `add r7, wp, idx` operand order (a direct `(u8*)wp->entries + off`
// form emits the index first). `wp = mapGetWork()`, the `vcd` pointer local and the pos/nrm
// declaration order are the rest of the matched source shape.
void mapSetPolygonVtxDesc(MapObject* obj, MapFileMesh* mesh) {
	MapWork* wp;
	void* pos;
	void* nrm;
	u32 mask;
	u32* vcd;
	s32 i;
	u8* vcdFmt;
	wp = mapGetWork();
	vcd = (u32*)mesh->vcdTableOffset;
	pos = (void*)(vcd[0] + 4);
	nrm = (void*)(vcd[1] + 4);
	mask = mesh->elementMask;

	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
	{
		u8* base = (u8*)wp + 0xA4;
		vcdFmt = *(u8**)(base + obj->entryIdx * 0x178);
	}
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, (u8)*(u32*)(vcdFmt + 0x44));
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
		vcdFmt = (u8*)wp->entries[obj->entryIdx].vcdTable;
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0 + i, GX_TEX_ST, GX_S16, (u8)*(u32*)(vcdFmt + 0x48 + i * 4));
		GXSetArray(GX_VA_TEX0 + i, (void*)(((u32*)mesh->vcdTableOffset)[6 + i] + 4), 4);
	}
}
#pragma opt_common_subs reset

void* mapSearchDmdJointSub(void* a, void* b) {
	MapFileJoint* joint = (MapFileJoint*)a;
	void* result;

	if (!strcmp(joint->name, (const char*)b)) return joint;
	if (joint->child && (result = mapSearchDmdJointSub(joint->child, b))) return result;
	if (joint->next && (result = mapSearchDmdJointSub(joint->next, b))) return result;
	return NULL;
}

#pragma opt_lifetimes off // target assigns a->r29/c->r30; default lifetime analysis swaps them
void* mapSearchDmdJointSub2(void* a, void* b, const char* c) {
	// `a` is cast at each use rather than aliased into a `joint` local -- the alias pushes
	// a's live range past the buf-address computation and makes MWCC emit the prologue differently.
	char buf[256];
	const char* name = (const char*)b;
	void* result;

	strcpy(buf, c);
	strcat(buf, ((MapFileJoint*)a)->name);
	if (!strcmp(buf, name)) return a;

	if (((MapFileJoint*)a)->child) {
		strcat(buf, "|");
		result = mapSearchDmdJointSub2(((MapFileJoint*)a)->child, b, buf);
		if (result) return result;
	}
	if (((MapFileJoint*)a)->next) {
		result = mapSearchDmdJointSub2(((MapFileJoint*)a)->next, b, c);
		if (result) return result;
	}
	return NULL;
}
#pragma opt_lifetimes reset

void* mapSearchDmdJoint(void* a, s8* b) {
	if (*b != 0x7C) {
		return mapSearchDmdJointSub(a, b);
	}
	else {
		return mapSearchDmdJointSub2(a, b, "|");
	}
}

MapObject* mapGetMapObj(const char *name) { // 1:1
    MapWork *wp;
    MapEntry* entry;
    int j;
    int i;
    MapObject *object;
    
    wp = mapGetWork();
    if (!name) {
        return 0;
    }

    for (i = 0; i < wp->entries[0].count; i++) {
        entry = &wp->entries[i];
        object = entry->objects;
        for (j = 0; j < entry->numJoints; j++, object++) {
            if (!strcmp(object->joints->name, name)) {
                return object;
            }
        }
    }
    return 0;
}

void mapObjFlagOn(const char* name, u32 flag) {
	MapObject* obj = mapGetMapObjSub(name);

	if (obj == NULL) {
		mapErrorEntry(0, name);
	}
	else if (!(flag & MAPOBJ_FLAG_DISPLAY_LIST) || obj->dlSize[0]) {
		obj->flags |= flag;
	}
}

void mapObjFlagOff(const char* name, u32 flag) {
	MapObject* obj = mapGetMapObjSub(name);

	if (obj == NULL) {
		mapErrorEntry(0, name);
	}
	else {
		obj->flags &= ~flag;
	}
}

void mapFlgOn(MapObject* obj, u32 flag, s32 a3) {
	if (!(flag & MAPOBJ_FLAG_DISPLAY_LIST) || obj->dlSize[0]) {
		obj->flags |= flag;
	}
	if (obj->child) mapFlgOn(obj->child, flag, 1);
	if (a3 && obj->next) mapFlgOn(obj->next, flag, 1);
}

void mapFlgOff(MapObject* obj, u32 flag, s32 a3) {
	obj->flags &= ~flag;
	if (obj->child) mapFlgOff(obj->child, flag, 1);
	if (a3 && obj->next) mapFlgOff(obj->next, flag, 1);
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

void mapGrpFlagOff(const char* name, u32 flag) {
	MapObject* obj = mapGetMapObjSub(name);

	if (obj == NULL) {
		mapErrorEntry(0, name);
	}
	else {
		mapFlgOff(obj, flag, 0);
	}
}

void _setOffScrnId(MapObject* obj, s32 id, s32 a3) {
	obj->offScreenId = id;
	if (id != -1) {
		obj->flags |= MAPOBJ_FLAG_OFFSCREEN;
	}
	else {
		obj->flags &= ~MAPOBJ_FLAG_OFFSCREEN;
	}
	if (obj->child) _setOffScrnId(obj->child, id, 1);
	if (a3 && obj->next) _setOffScrnId(obj->next, id, 1);
}

void mapObjSetOffScreen(const char* name, const char* offName) {
	MapObject* obj = mapGetMapObjSub(name);

	obj->offScreenId = offscreenNameToId(offName);
	obj->flags |= MAPOBJ_FLAG_OFFSCREEN;
}

void mapGrpSetOffScreen(const char* name, const char* offName) {
	MapObject* obj = mapGetMapObjSub(name);
	s32 id = offscreenNameToId(offName);

	_setOffScrnId(obj, id, 0);
}

void mapObjClearOffScreen(const char* name) {
	MapObject* obj = mapGetMapObjSub(name);

	obj->offScreenId = -1;
	obj->flags &= ~MAPOBJ_FLAG_OFFSCREEN;
}

void mapGrpClearOffScreen(const char* name) {
	_setOffScrnId(mapGetMapObjSub(name), -1, 0);
}

void mapObjRotate(const char* name, f32 x, f32 y, f32 z) {
	MapObject* obj = mapGetMapObjSub(name);

	if (obj) {
		Mtx mx;
		Mtx my;
		Mtx mz;
		u32* flags = &obj->flags;

		if (!(*flags & MAPOBJ_FLAG_DIRTY)) {
			PSMTXIdentity(obj->unkAC);
		}
		PSMTXRotRad(mx, 'x', 0.017453292f * x);
		PSMTXRotRad(my, 'y', 0.017453292f * y);
		PSMTXRotRad(mz, 'z', 0.017453292f * z);
		PSMTXConcat(obj->unkAC, mz, obj->unkAC);
		PSMTXConcat(obj->unkAC, my, obj->unkAC);
		PSMTXConcat(obj->unkAC, mx, obj->unkAC);
		*flags |= MAPOBJ_FLAG_USER_MTX;
		*flags |= MAPOBJ_FLAG_DIRTY;
	}
}

void mapObjTranslate(const char* name, f32 x, f32 y, f32 z) {
	MapObject* obj = mapGetMapObjSub(name);

	if (obj) {
		Mtx m;
		u32* flags = &obj->flags;

		if (!(*flags & MAPOBJ_FLAG_DIRTY)) {
			PSMTXIdentity(obj->unkAC);
		}
		PSMTXTrans(m, x / 10.0f, y / 10.0f, z / 10.0f);
		PSMTXConcat(obj->unkAC, m, obj->unkAC);
		*flags |= MAPOBJ_FLAG_USER_MTX;
		*flags |= MAPOBJ_FLAG_DIRTY;
	}
}

void mapObjScale(const char* name, f32 x, f32 y, f32 z) {
	MapObject* obj = mapGetMapObjSub(name);

	if (obj) {
		Mtx m;
		u32* flags = &obj->flags;

		if (!(*flags & MAPOBJ_FLAG_DIRTY)) {
			PSMTXIdentity(obj->unkAC);
		}
		PSMTXScale(m, x, y, z);
		PSMTXConcat(obj->unkAC, m, obj->unkAC);
		*flags |= MAPOBJ_FLAG_USER_MTX;
		*flags |= MAPOBJ_FLAG_DIRTY;
	}
}

void mapObjGetPosSub(MapObject* obj, Vec* pos, s32* count, void* a4) {
	if (obj->child) {
		mapObjGetPosSub(obj->child, pos, count, (void*)1);
	}
	else {
		Vec v;

		PSMTXMultVec(obj->modelWorldMtx, &obj->center, &v);
		PSVECAdd(pos, &v, pos);
		(*count)++;
	}
	if ((s32)a4 && obj->next) {
		mapObjGetPosSub(obj->next, pos, count, (void*)1);
	}
}

void mapObjGetPos(const char* name, Vec* pos) {
	Vec pos2;
	s32 scale;
	MapObject* obj;

	scale = 0;
	pos2 = (Vec){0.0f, 0.0f, 0.0f};
	obj = mapGetMapObjSub(name);

	if (obj == NULL) {
		*pos = (Vec){0.0f, 0.0f, 0.0f};
	}
	else {
		mapObjGetPosSub(obj, &pos2, &scale, NULL);
		PSVECScale(&pos2, pos, 1.0f / (f32)scale);
	}
}

void mapSetFog(u32 color, f32 near, f32 far, GXColor color2) {
	MapEntry* entry = &mapWork[activeGroup].entries[0];

	entry->fogType = color;
	entry->fogStart = near;
	entry->fogEnd = far;
	entry->fogColor = color2;
}

void mapGetFog(u32* c, f32* near, f32* far, u32* c2) {
	MapEntry* entry = &mapWork[activeGroup].entries[0];

	*c = entry->fogType;
	*near = entry->fogStart;
	*far = entry->fogEnd;
	*c2 = *(u32*)&entry->fogColor;
}

void mapFogOn(void) {
	MapEntry* entry = &mapWork[activeGroup].entries[0];

	entry->flags |= 0x1;
}

void mapFogOff(void) {
	MapEntry* entry = &mapWork[activeGroup].entries[0];

	entry->flags &= ~0x1;
}

void mapSetBlend(GXColor* color) {
	MapEntry* entry = &mapWork[activeGroup].entries[0];

	entry->blend[0] = *color;
	entry->flags |= 0x2;
}

void mapSetBlend2(GXColor* color) {
	MapEntry* entry = &mapWork[activeGroup].entries[0];

	entry->blend[1] = *color;
	entry->flags |= 0x4;
}

void mapGetBlend(GXColor* out) {
	MapEntry* entry = &mapWork[activeGroup].entries[0];

	if (entry->flags & 0x2) {
		*out = entry->blend[0];
	}
	else {
		*out = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
	}
}

void mapGetBlend2(GXColor* out) {
	MapEntry* entry = &mapWork[activeGroup].entries[0];

	if (entry->flags & 0x4) {
		*out = entry->blend[1];
	}
	else {
		*out = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
	}
}

void mapBlendOff(void) {
	MapEntry* entry = &mapWork[activeGroup].entries[0];

	entry->flags &= ~0x2;
}

void mapBlendOff2(void) {
	MapEntry* entry = &mapWork[activeGroup].entries[0];

	entry->flags &= ~0x4;
}

void mapObjSetColor(const char* name, GXColor* color) {
	MapObject* obj = mapGetMapObjSub(name);

	if (obj) {
		*(u32*)&obj->color = *(u32*)color;
		obj->flags |= MAPOBJ_FLAG_COLOR;
	}
}

void setColor(MapObject* obj, GXColor color, s32 a3) {
	if (obj == 0) return;
	obj->color = color;
	obj->flags |= MAPOBJ_FLAG_COLOR;
	if (obj->child) {
		setColor(obj->child, color, 1);
	}
	if (a3 && obj->next) {
		setColor(obj->next, color, 1);
	}
}

static MapObject* mapGetMapObjSub(const char* name) {
	MapEntry* entry;
	MapWork *wp;
	int j;
	int i;
	MapObject *object;

	wp = mapGetWork();
	if (!name) {
		return 0;
	}

	for (i = 0; i < wp->entries[0].count; i++) {
		entry = &wp->entries[i];
		object = entry->objects;
		for (j = 0; j < entry->numJoints; j++, object++) {
			if (!strcmp(object->joints->name, name)) {
				return object;
			}
		}
	}
	return 0;
}

void mapGrpSetColor(const char* name, GXColor color) {
	setColor(mapGetMapObjSub(name), color, 0);
}

void mapSetProjTexObj(GXTexObj* obj, void* imgRef) {
	projTexObj = *obj;
	projTexImgRef = imgRef;
}

void mapSetProjMtx(Mtx mtx) {
	PSMTXCopy(mtx, projMtx);
}

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

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

static inline f32 spline_sqrtf(f32 x) {
	if (x > 0.0f) {
		f64 guess = __frsqrte(x);

		guess = 0.5 * guess * (3.0 - guess * guess * x);
		guess = 0.5 * guess * (3.0 - guess * guess * x);
		guess = 0.5 * guess * (3.0 - guess * guess * x);
		return x * guess;
	} else if (x < 0.0) {
		return SPLINE_NAN;
	} else if (spline_fpclassifyf(x) == FP_NAN) {
		return SPLINE_NAN;
	}
	return x;
}

// Resolves the two boundary rows of the tridiagonal system in place, then divides each remaining
// row through by its diagonal during the backward sweep.
static inline void spline_backsub(s32 count, f32* dt, Vec* mat, Vec* tangents) {
	Vec* mp;
	f32* dp;
	Vec* tp;
	Vec* tend;
	s32 i;

	tp = &tangents[count - 2];
	dp = &dt[count - 2];
	mp = &mat[count - 2];
	tend = &tangents[count];
	tend[-2].x = tend[-2].x - dt[count - 2] * tangents[count - 1].x;
	tend[-2].y = tend[-2].y - dt[count - 2] * tangents[count - 1].y;
	tend[-2].z = tend[-2].z - dt[count - 2] * tangents[count - 1].z;
	for (i = count - 2; i > 0; i--) {
		tp->x = (tp->x - *dp * tp[1].x) / mp->x;
		tp->y = (tp->y - *dp * tp[1].y) / mp->y;
		tp->z = (tp->z - *dp * tp[1].z) / mp->z;
		tp--;
		dp--;
		mp--;
	}
}

void spline_maketable(s32 count, s32 a, s32* b, s32* c) {
	Vec* points = (Vec*)a;
	f32* knots = (f32*)b;
	Vec* tangents = (Vec*)c;
	f32* dt = (f32*)_mapAlloc(count * sizeof(f32));
	Vec* mat = (Vec*)_mapAlloc(count * sizeof(Vec));
	s32 i;
	f32 knprev;

	knots[0] = 0.0f;
	for (i = 1; i < count; i++) {
		f32 dx = points[i].x - points[i - 1].x;
		f32 dy = points[i].y - points[i - 1].y;
		f32 dz = points[i].z - points[i - 1].z;

		f32 sq = dx * dx + dy * dy + dz * dz;
#ifdef MUST_MATCH
		// writing the Newton result back into sq reuses x's FPR (the target overwrites f5);
		// calling spline_sqrtf forces the f32 return register f1 and leaves an extra `fmr f1,f5`
		if (sq > 0.0f) {
			f64 guess = __frsqrte(sq);
			guess = 0.5 * guess * (3.0 - guess * guess * sq);
			guess = 0.5 * guess * (3.0 - guess * guess * sq);
			guess = 0.5 * guess * (3.0 - guess * guess * sq);
			sq = sq * guess;
		} else if (sq < 0.0) {
			sq = SPLINE_NAN;
		} else if (spline_fpclassifyf(sq) == FP_NAN) {
			sq = SPLINE_NAN;
		}
#else
		sq = spline_sqrtf(sq);
#endif
		knprev = knots[i - 1];
		knots[i] = knprev + sq;
	}

	for (i = 1; i < count; i++) {
		knots[i] = knots[i] / knots[count - 1];
	}

	tangents[0].x = 0.0f;
	tangents[0].y = 0.0f;
	tangents[0].z = 0.0f;
	tangents[count - 1].x = 0.0f;
	tangents[count - 1].y = 0.0f;
	tangents[count - 1].z = 0.0f;
	for (i = 0; i < count - 1; i++) {
		dt[i] = ((f32*)b)[i + 1] - ((f32*)b)[i];
		if (dt[i] == 0.0f) {
			dt[i] = 0.0001f;
		}
		mat[i + 1].x = (((Vec*)a)[i + 1].x - ((Vec*)a)[i].x) / dt[i];
		mat[i + 1].y = (((Vec*)a)[i + 1].y - ((Vec*)a)[i].y) / dt[i];
		mat[i + 1].z = (((Vec*)a)[i + 1].z - ((Vec*)a)[i].z) / dt[i];
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

	spline_backsub(count, dt, mat, tangents);

	_mapFree(mat);
	_mapFree(dt);
}

#pragma opt_lifetimes off // 98.78% without: t and h keep their own FPRs
void spline_getvalue(f32* out, s32 type, s32 a, s32 b, s32 c, f32 t) {
	f32* points = (f32*)a;
	f32* knots = (f32*)b;
	f32* tangents = (f32*)c;
	s32 lo;
	s32 hi;
	s32 mid;
	f32 h;

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

	h = knots[lo];
	t -= h;
	h = knots[lo + 1] - h;
	out[0] = t * (t * (3.0f * tangents[lo * 3 + 0] + t * (tangents[lo * 3 + 3] - tangents[lo * 3 + 0]) / h) + ((points[lo * 3 + 3] - points[lo * 3 + 0]) / h - h * (2.0f * tangents[lo * 3 + 0] + tangents[lo * 3 + 3]))) + points[lo * 3 + 0];
	out[1] = t * (t * (3.0f * tangents[lo * 3 + 1] + t * (tangents[lo * 3 + 4] - tangents[lo * 3 + 1]) / h) + ((points[lo * 3 + 4] - points[lo * 3 + 1]) / h - h * (2.0f * tangents[lo * 3 + 1] + tangents[lo * 3 + 4]))) + points[lo * 3 + 1];
	out[2] = t * (t * (3.0f * tangents[lo * 3 + 2] + t * (tangents[lo * 3 + 5] - tangents[lo * 3 + 2]) / h) + ((points[lo * 3 + 5] - points[lo * 3 + 2]) / h - h * (2.0f * tangents[lo * 3 + 2] + tangents[lo * 3 + 5]))) + points[lo * 3 + 2];
}
#pragma opt_lifetimes reset

#ifdef MUST_MATCH
// STRIPPED FUNCTION PLACEHOLDER (not original code). Retail had an uncalled function here, between spline_getvalue
// and mapCheckData (the slot where Super Paper Mario's mapdrv has Locate); the linker removed its code, but the
// float constants it used stayed in this file's .sdata2 pool, in the order 3.0, 0.0, 0.5. What it computed is
// unknown (SPM's Locate uses no floats), so this only reproduces those constants, as Melee's sdata2_order and
// Wind Waker's stripped-function dummies do. See docs/CONTEXT.md, "mapdrv links".
static void mapdrv_sdata2_order(void) {
	f32* dummy = NULL;
	*dummy = 3.0f;
	*dummy = 0.0f;
	*dummy = 0.5f;
}
#endif

// Checks a loaded map file before use: a supported converter version, a conversion date, and the animation
// and curve tables. Nothing in the game calls it; the original's .rodata still holds its strings.
BOOL mapCheckData(MapHeader* header, const char* map, const char* date) {
	char path[128];
	MapFileInfo* info;

	sprintf(path, "%s/m/%s/d", getMapDataDvdRoot(), map);
	info = (MapFileInfo*)mapLoadFindChunk(header, "information");
	if (strcmp(info->version, "ver1.01") && strcmp(info->version, "ver1.02")) {
		mapErrorEntry(0, path);
		return FALSE;
	}
	if (!strcmp(date, "--/--/-- --:--:--")) {
		mapErrorEntry(1, "\203R\203\223\203o\201[\203g\202\263\202\352\202\304\202\242\202\334\202\271\202\361");
		return FALSE;
	}
	if (mapLoadFindChunk(header, "animation_table") == NULL || mapLoadFindChunk(header, "curve_table") == NULL) {
		mapErrorEntry(2, path);
		return FALSE;
	}
	return TRUE;
}

BOOL mapTestXLU(u32 flags, GXColor* a, GXColor* b) {
	if ((flags & MAPOBJ_FLAG_COLOR) && a->a != 0xFF) {
		return TRUE;
	}
	if (b->a != 0xFF) {
		return TRUE;
	}
	if (!(flags & MAPOBJ_FLAG_NO_BLEND) && (mapWork[activeGroup].entries[0].flags & 0x2) && mapWork[activeGroup].entries[0].blend[0].a != 0xFF) {
		return TRUE;
	}
	if (!(flags & MAPOBJ_FLAG_NO_BLEND1) && (mapWork[activeGroup].entries[0].flags & 0x4) && mapWork[activeGroup].entries[0].blend[1].a != 0xFF) {
		return TRUE;
	}
	return FALSE;
}

void mapSetTevCallback(s32 idx, void* cb) {
	u8* base = (u8*)mapWork + activeGroup * 0x2F4 + 0x110;

	*(void**)(base + idx * 4) = cb;
}

void mapObjFlushOn(const char* name) {
	MapObject* obj = mapGetMapObjSub(name);

	if (obj == NULL) {
		mapErrorEntry(0, name);
	}
	else {
		obj->flags |= MAPOBJ_FLAG_FLUSH;
	}
}

void mapObjFlushOff(const char* name) {
	MapObject* obj = mapGetMapObjSub(name);

	if (obj == NULL) {
		mapErrorEntry(0, name);
	}
	else {
		obj->flags &= ~MAPOBJ_FLAG_FLUSH;
	}
}

void mapGrpFlushOn(const char* name) {
	mapGrpFlagOn(name, 0x100000);
}

void mapGrpFlushOff(const char* name) {
	mapGrpFlagOff(name, 0x100000);
}

void mapObjSetFlushColor(const char* name, u8 r, u8 g, u8 b, u8 a) {
	GXColor color;
	MapObject* obj;

	color = (GXColor){0, 0, 0, 0};
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	obj = mapGetMapObjSub(name);
	if (obj) {
		obj->color = color;
		obj->flags |= MAPOBJ_FLAG_COLOR;
	}
	mapObjFlagOff(name, 0x40);
}

// Built in an inline that returns the colour: puts the by-value argument copy above the local, as retail has it.
static inline GXColor mapFlushColor(u8 r, u8 g, u8 b, u8 a) {
	GXColor color = (GXColor){0, 0, 0, 0};

	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	return color;
}

void mapGrpSetFlushColor(const char* name, u8 r, u8 g, u8 b, u8 a) {
	mapGrpSetColor(name, mapFlushColor(r, g, b, a));
	mapGrpFlagOff(name, 0x40);
}

void mapObjGetFlushColor(const char* name, u8* r, u8* g, u8* b, u8* a) {
	MapObject* obj = mapGetMapObjSub(name);

	if (!obj) {
		mapErrorEntry(0, name);
		return;
	}
	*r = obj->color.r;
	*g = obj->color.g;
	*b = obj->color.b;
	*a = obj->color.a;
}
