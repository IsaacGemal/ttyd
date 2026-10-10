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

// Original reaches queen_uv with absolute @ha/@l addressing, not the .sdata sda21 form.
__declspec(section ".rodata") extern f32 queen_uv[4]; //gxsub.c
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

extern int __float_nan[];
#define SPLINE_NAN (*(f32*)__float_nan)
extern const char float_0_804198cc[4];
extern GXColor dat_8041981c;
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
typedef struct MapMatTexRef {
	void* field_0x0;  // 0x0, points at the texture name/info block
	u8 field_0x4[4];  // 0x4
	u8 wrapS;         // 0x8
	u8 wrapT;         // 0x9
} MapMatTexRef;
typedef struct MapMaterial {
	u8 field_0x0[4];              // 0x0
	GXColor color;                // 0x4
	u8 field_0x8;                 // 0x8
	u8 field_0x9[2];              // 0x9
	u8 texCount;                  // 0xB
	MapMatTexRef* texRefs[8];    // 0xC
	u8 field_0x2C[0x110 - 0x2C];  // 0x2C
	u8* field_0x110;              // 0x110
} MapMaterial;
#ifdef MUST_MATCH
#define MAPMAT_DEFAULT_DIR (*(Vec*)(base + 0x144))
#define MAPMAT_TEXWRAP(i) (wrapTbl[i])
#else
extern u32 texwrap_tbl[];
#define MAPMAT_DEFAULT_DIR ((Vec){ 0.0f, 0.0f, -1.0f })
#define MAPMAT_TEXWRAP(i) ((s32)texwrap_tbl[i])
#endif
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

// The table walk below was moved into the inline helper so the deferred inliner emits the original's
// `bne increment; b found` split.
#ifdef MUST_MATCH
// Moving the walk into an inline whose found path is a `return` makes the deferred inliner emit the
// target's `bne increment; b found` split instead of a single `beq found`.
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
#endif
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
#ifdef MUST_MATCH
			idx = mapSearchTextureIdx(table, ref);
#else
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
#endif
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

// `wp = mapGetWork()` (same value as &mapWork[activeGroup]) moves wp into r31 and lets the cached
// entry pointer be dropped; the parenthesised else-store offset reproduces the target's
// `add base,wp,off; stw 0x16c(base)` addressing. The only remaining diff is the operand order of that
// one `add` (target puts wp first).
// NONMATCHING (99.93%): `add r3, r31, r27` (target) vs `add r3, r27, r31` (the single differing insn)
void makeDisplayList(BOOL group) {
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
			*(void**)(obj->field_0xEC + 8 + i * 4) = NULL;
			*(u32*)(obj->field_0xEC + 0x28 + i * 4) = 0;
		}
		for (i = 0; i < wp->entries[group].numJoints; i++, obj++) {
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
			if (!(wp->entries[group].flags & 0x8000)) {
				continue;
			}
			for (j = 0; j < obj->joints->partCount; j++) {
				void* material = obj->joints->parts[j].material;

				memset(buf, 0, 0x10000);
				DCInvalidateRange(buf, 0x10000);
				GXBeginDisplayList(buf, 0x10000);
				GXResetWriteGatherPipe();
				mapSetMaterial(obj, material);
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
			*(u32*)(wp->entries[group].field_0x168) = total;
			if (*(u32*)(wp->entries[group].field_0x168) != 0) {
				*(void**)(wp->entries[group].field_0x168 + 4) = _mapAllocTail(*(u32*)(wp->entries[group].field_0x168));
				dest = *(u8**)(wp->entries[group].field_0x168 + 4);
			}
			else {
#ifdef MUST_MATCH
				// MUST_MATCH: naming the group offset here is what makes MWCC keep the offset live and
				// emit the target's `add r3,wp,off; stw 0x16c(r3)` instead of an indexed store.
				*(void**)((u8*)wp + (group * 0x178 + 0x16C)) = NULL;
#else
				*(void**)(wp->entries[group].field_0x168 + 4) = NULL;
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

//more animation functions

// Shared helpers for _mapLoad: the loader searches the chunk table by name seven times and builds its
// eight bounding-box corners from a zero vector seed; both shapes inline into the caller.
static inline void* mapLoadFindChunk(MapHeader* header, const char* name) {
	u32 i;

	for (i = 0; i < (u32)header->file.chunkCount; i++) {
		if (!strcmp(&header->strings[header->chunks[i].string], name)) {
			return (u8*)header->data + header->chunks[i].offset;
		}
	}
	return NULL;
}
#ifndef MUST_MATCH
static const Vec ml_zero_seed = { 0.0f, 0.0f, 0.0f };
#endif
static inline Vec mapLoadCorner(const Vec* seed, f32 x, f32 y, f32 z) {
	Vec v = *seed;

	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}
// The original reaches _mapLoad's strings as offsets from the unit's .rodata base vec3_802c16d8, keeping
// that base live in r23. The natural build uses the literals.
#ifdef MUST_MATCH
// MUST_MATCH: the code below reads these strings through the vec3_802c16d8 base, so the bytes must still be
// emitted into .rodata for the unit's data to match. force_active keeps them when nothing references them.
#pragma force_active on
__declspec(section ".rodata") static const char ml_str_md_d[] = "./m/%s/d";
__declspec(section ".rodata") static const char ml_str_mt_d[] = "./m/%s/t";
__declspec(section ".rodata") static const char ml_str_info[] = "information";
__declspec(section ".rodata") static const char ml_str_ver[] = "ver1.02";
__declspec(section ".rodata") static const char ml_str_anim[] = "animation_table";
__declspec(section ".rodata") static const char ml_str_tex[] = "texture_table";
__declspec(section ".rodata") static const char ml_str_light[] = "light_table";
__declspec(section ".rodata") static const char ml_str_fog[] = "fog_table";
__declspec(section ".rodata") static const char ml_str_mat[] = "material_name_table";
__declspec(section ".rodata") static const char ml_str_vcd[] = "vcd_table";
__declspec(section ".rodata") static const char ml_str_amb[] = "ambientLight";
__declspec(section ".rodata") static const char ml_str_point[] = "pointLight";
__declspec(section ".rodata") static const char ml_str_dir[] = "directionalLight";
__declspec(section ".rodata") static const char ml_str_spot[] = "spotLight";
__declspec(section ".rodata") static const char ml_str_c_d[] = "./m/%s/c";
#pragma force_active reset
#define ML_STR_MD ((const char*)(base + 0x2D0))
#define ML_STR_MT ((const char*)(base + 0x2DC))
#define ML_STR_INFO ((const char*)(base + 0x240))
#define ML_STR_VER ((const char*)(base + 0x254))
#define ML_STR_ANIM ((const char*)(base + 0x28C))
#define ML_STR_TEX ((const char*)(base + 0x2E8))
#define ML_STR_LIGHT ((const char*)(base + 0x2F8))
#define ML_STR_FOG ((const char*)(base + 0x304))
#define ML_STR_MAT ((const char*)(base + 0x310))
#define ML_STR_VCD ((const char*)(base + 0x324))
#define ML_STR_AMB ((const char*)(base + 0x330))
#define ML_STR_POINT ((const char*)(base + 0x340))
#define ML_STR_DIR ((const char*)(base + 0x34C))
#define ML_STR_SPOT ((const char*)(base + 0x360))
#define ML_STR_C ((const char*)(base + 0x36C))
#else
#define ML_STR_MD "./m/%s/d"
#define ML_STR_MT "./m/%s/t"
#define ML_STR_INFO "information"
#define ML_STR_VER "ver1.02"
#define ML_STR_ANIM "animation_table"
#define ML_STR_TEX "texture_table"
#define ML_STR_LIGHT "light_table"
#define ML_STR_FOG "fog_table"
#define ML_STR_MAT "material_name_table"
#define ML_STR_VCD "vcd_table"
#define ML_STR_AMB "ambientLight"
#define ML_STR_POINT "pointLight"
#define ML_STR_DIR "directionalLight"
#define ML_STR_SPOT "spotLight"
#define ML_STR_C "./m/%s/c"
#endif
// NONMATCHING (95.64%): the base and the lightEntry argument order now match. What is left is register
// numbering/rematerialisation: the target saves r17-r31 and recomputes wp->entries[group] after the
// light loop, ours saves r20-r31 and keeps it live, plus one extra `li 0` and the animation store order.
void _mapLoad(MapWork* wp, BOOL group, const char* map) {
	MapHeader* header;
	MapEntry* entry;
	MapFileInfo* info;
	MapFileJoint* mapJoint;
	MapFileLight* light;
	MapFileJoint* hitJoint;
	MapObject* obj;
	DVDEntry* dvd;
	char str1[128];
	char str2[128];
	char str3[128];
	Vec pos;
	Vec min;
	Vec max;
	Mtx mtx;
	u32 sizeCheck;
	u32 size;
	u8 *data;
	u8* base;
	void* anim;
	s32 rel_i;
	s32 disp_i;
	s32 i;

	entry = &wp->entries[group];
	base = (u8*)((u32)vec3_802c16d8);
	error_flag = FALSE;
	error_count = 0;
	strncpy(entry->name, map, sizeof(entry->name));

	sprintf(str1, ML_STR_MD, map);
	arcOpen(str1, &wp->entries[group].dat, &wp->entries[group].datSize);
	if (!wp->entries[group].dat) {
		sprintf(str2, "%s/%s", ".", str1);
		dvd = DVDMgrOpen(str2, 2, 0);
		wp->entries[group].datSize = DVDMgrGetLength(dvd);
		wp->entries[group].dat = _mapAllocTail(OSRoundUp32B(wp->entries[group].datSize));
		DVDMgrRead(dvd, wp->entries[group].dat, OSRoundUp32B(wp->entries[group].datSize), 0);
		DVDMgrClose(dvd);
	}

	sprintf(str1, ML_STR_MT, map);
	wp->entries[group].tpl = NULL;
	arcOpen(str1, &wp->entries[group].tpl, &wp->entries[group].tplSize);
	if (entry->tpl) {
		UnpackTexPalette(entry->tpl);
	}
	else {
		sprintf(str3, "%s/%s", ".", str1);
		dvd = DVDMgrOpen(str3, 2, 0);
		if (dvd) {
			wp->entries[group].tplSize = DVDMgrGetLength(dvd);
			entry->tpl = _mapAllocTail(OSRoundUp32B(entry->tplSize));
			DVDMgrRead(dvd, wp->entries[group].tpl, OSRoundUp32B(wp->entries[group].tplSize), 0);
			DVDMgrClose(dvd);
			UnpackTexPalette(wp->entries[group].tpl);
		}
	}

	GXInvalidateVtxCache();
	header = &entry->header;
	{
		u32 entryDatSize = wp->entries[group].datSize;
		data = (u8*)entry->dat;
		if (header != NULL) {
			memset(header, 0, sizeof(MapHeader));
			wp->entries[group].header.field_0x3C |= 1;
			memcpy(header, data, sizeof(MapFileHeader));
			if (wp->entries[group].header.file.fileSize == entryDatSize) {
				sizeCheck = sizeof(MapFileHeader);
				if ((u32)wp->entries[group].header.file.dataSize != 0) {
					entry->header.data = &data[sizeCheck];
					sizeCheck = entry->header.file.dataSize + sizeof(MapFileHeader);
				}
				if ((u32)wp->entries[group].header.file.relCount != 0) {
					entry->header.rel = &data[sizeCheck];
					sizeCheck += 4 * entry->header.file.relCount; //TODO: sizeof
				}
				if ((u32)wp->entries[group].header.file.chunkCount != 0) {
					entry->header.chunks = (MapFileChunk*)&data[sizeCheck];
					sizeCheck += sizeof(MapFileChunk) * entry->header.file.chunkCount;
				}
				if ((u32)wp->entries[group].header.file.unused != 0) {
					entry->header.unused = &data[sizeCheck];
					sizeCheck += 8 * entry->header.file.unused;
				}
				if (sizeCheck < entry->header.file.fileSize) {
					wp->entries[group].header.strings = (const char*)&data[sizeCheck];
				}
				*(void**)((u8*)&wp->entries[group] + 0x78) = data;

				//iunno about this section
				for (rel_i = 0; rel_i < (u32)entry->header.file.relCount; rel_i++) {
					u32 off = ((u32*)header->rel)[rel_i];
					u8* relData = (u8*)header->data + off;
					if (*(u32*)relData <= 0x80000000) {
						*(u32*)relData += (u32)header->data;
					}
				}
			}
		}
	}

	info = (MapFileInfo*)mapLoadFindChunk(&entry->header, ML_STR_INFO);
	// MapEntry fields the header does not name yet (offsets from MapEntry): chunk pointers and anim count.
	*(void**)((u8*)&wp->entries[group] + 0x94) = mapLoadFindChunk(&entry->header, ML_STR_TEX);
	*(void**)((u8*)entry + 0x98) = mapLoadFindChunk(&entry->header, ML_STR_LIGHT);
	*(void**)((u8*)&wp->entries[group] + 0x9C) = mapLoadFindChunk(&entry->header, ML_STR_FOG);
	*(void**)((u8*)&wp->entries[group] + 0xA0) = mapLoadFindChunk(&entry->header, ML_STR_MAT);
	*(void**)((u8*)&wp->entries[group] + 0xA4) = mapLoadFindChunk(&entry->header, ML_STR_VCD);

	mapJoint = mapSearchDmdJoint(info->joint, (s8*)info->mapName);
	hitJoint = mapSearchDmdJoint(info->joint, (s8*)info->hitName);
	if (!strcmp(info->version, ML_STR_VER)) {
		wp->entries[group].flags |= 0x8000;
	}
	else {
		wp->entries[group].flags &= ~0x8000;
	}

	PSMTXScale(mtx, 10.0f, 10.0f, 10.0f);
	wp->entries[group].rootMapObj = mapEntry(mapJoint, mtx, group);
	wp->entries[group].rootHitObj = hitEntry(hitJoint, mtx, group);

	for (i = 0; i < ((MapFileLightTable*)*(void**)((u8*)entry + 0x98))->count; i++) {
		s32 type;
		s32 flags;

		light = ((MapFileLightTable*)*(void**)((u8*)entry + 0x98))->lights[i];
		type = 1;
		flags = 0;
		PSVECScale(&light->position, &pos, 10.0f);
		if (!strcmp(light->type, ML_STR_AMB)) {
			type = 0;
		}
		else if (!strcmp(light->type, ML_STR_POINT)) {
			type = 1;
		}
		else if (!strcmp(light->type, ML_STR_DIR)) {
			type = 2;
		}
		else if (!strcmp(light->type, ML_STR_SPOT)) {
			type = 3;
		}
		if (light->field_0x40 == 0x12D60D8) {
			flags = light->field_0x3C;
		}
#ifdef MUST_MATCH
		// MUST_MATCH: the original evaluates the two f32 arguments before attenType/flags, so the
		// floats are declared first in this cast; the header's declaration order differs but the
		// register assignment is identical.
		((s32 (*)(const char*, s32, Vec*, Vec*, GXColor, f32, f32, s32, s32))lightEntry)(light->name, type, &pos, &light->rotation, light->color, light->spotAngle, light->attenAngle, light->attenType, flags);
#else
		lightEntry(light->name, type, &pos, &light->rotation, light->color, light->attenType, flags, light->spotAngle, light->attenAngle);
#endif
	}

	paper_ambient = dat_8041982c;
	paperCraft_ambient = dat_80419830;

	{
		MapFileFog* fog = (MapFileFog*)*(void**)((u8*)entry + 0x9C);
		if (fog != NULL) {
			*(s32*)((u8*)entry + 0x24) = (*(MapFileFog**)((u8*)entry + 0x9C))->field_0x0;
			*(s32*)((u8*)entry + 0x28) = (*(MapFileFog**)((u8*)entry + 0x9C))->field_0x4;
			*(f32*)((u8*)entry + 0x2C) = (*(MapFileFog**)((u8*)entry + 0x9C))->field_0x8;
			*(f32*)((u8*)entry + 0x30) = (*(MapFileFog**)((u8*)entry + 0x9C))->field_0xC;
			*(s32*)((u8*)entry + 0x34) = (*(MapFileFog**)((u8*)entry + 0x9C))->field_0x10;
			if (*(s32*)((u8*)entry + 0x24) != 0) {
				entry->flags |= 1;
			}
		}
		else {
			*(s32*)((u8*)entry + 0x24) = 0;
			*(s32*)((u8*)entry + 0x28) = 0;
			*(f32*)((u8*)entry + 0x2C) = float_0_80419850;
			*(f32*)((u8*)entry + 0x30) = float_0_80419850;
			*(u32*)((u8*)entry + 0x34) = *(u32*)&unk_80422f80;
			entry->flags &= ~1;
		}
	}
	if (activeGroup != 1) {
		sprintf(str1, ML_STR_C, map);
		camLoadRoad(CAMERA_3D, str1);
	}
	camSetTypePersp(CAMERA_3D);
	mapBuildTexture(wp->entries[group].rootMapObj, entry->tpl, *(void**)((u8*)entry + 0x94));
	mapGetBoundingBox(&min, &max);

#ifdef MUST_MATCH
	// MUST_MATCH: retail seeds each corner from the unit's .rodata through the vec3_802c16d8 base; every field is
	// overwritten, so the natural build seeds from zero.
#define ML_SEED(off) ((const Vec*)(base + (off)))
#else
#define ML_SEED(off) (&ml_zero_seed)
#endif
	entry->bbox[0] = mapLoadCorner(ML_SEED(0x28), max.x, max.y, max.z);
	wp->entries[group].bbox[1] = mapLoadCorner(ML_SEED(0x34), min.x, max.y, max.z);
	wp->entries[group].bbox[2] = mapLoadCorner(ML_SEED(0x40), max.x, min.y, max.z);
	wp->entries[group].bbox[3] = mapLoadCorner(ML_SEED(0x4C), max.x, max.y, min.z);
	wp->entries[group].bbox[4] = mapLoadCorner(ML_SEED(0x58), min.x, min.y, max.z);
	wp->entries[group].bbox[5] = mapLoadCorner(ML_SEED(0x64), max.x, min.y, min.z);
	wp->entries[group].bbox[6] = mapLoadCorner(ML_SEED(0x70), min.x, max.y, min.z);
	entry->bbox[7] = mapLoadCorner(ML_SEED(0x7C), min.x, min.y, min.z);
#undef ML_SEED

	{
		MapEntry* entry2 = &mapWork[activeGroup].entries[group];

		anim = mapLoadFindChunk(&entry2->header, ML_STR_ANIM);
		if (anim != NULL) {
			*(s32*)((u8*)entry2 + 0x160) = *(s32*)anim;
			if (*(s32*)((u8*)entry2 + 0x160) != 0) {
				size = (*(s32*)((u8*)entry2 + 0x160) * 32 + 0x1F) & ~0x1F;
				entry2->animData = _mapAlloc(size);
				memset(entry2->animData, 0, size);
				{
					u16* out = (u16*)entry2->animData;
					u32* in = (u32*)anim;

					for (i = 0; i < *(s32*)((u8*)entry2 + 0x160); i++) {
						*out = 0;
						*(u32*)((u8*)out + 0x18) = *++in;
						out += 16;
					}
				}
			}
		}
	}

	makeDisplayList(group);
	{
		MapEntry* entry3 = &mapWork[activeGroup].entries[group];

		obj = entry3->objects;
		for (disp_i = 0; disp_i < entry3->numJoints; disp_i++, obj++) {
			MapFileJoint* joint = obj->joints;

			if (joint->partCount >= 1 && joint->partCount <= 8) {
				MapJointDrawMode* dm = joint->drawMode;

				// MapJointDrawMode has no named field at 0x2/0x4; use raw offsets.
				if (!(*(u8*)((u8*)dm + 2) & 8) && *(u32*)((u8*)dm + 4) == 0) {
					if (shadowGetCharShadowProjection() == 0 || !(*(u32*)((u8*)obj + 0) & 4)) {
						if (entry3->flags & 0x8000) {
							*(u32*)((u8*)obj + 0) |= 0x100;
						}
					}
				}
			}
		}
	}
	unk_80418458 = 0;
}
#undef ML_STR_MD
#undef ML_STR_MT
#undef ML_STR_INFO
#undef ML_STR_VER
#undef ML_STR_ANIM
#undef ML_STR_TEX
#undef ML_STR_LIGHT
#undef ML_STR_FOG
#undef ML_STR_MAT
#undef ML_STR_VCD
#undef ML_STR_AMB
#undef ML_STR_POINT
#undef ML_STR_DIR
#undef ML_STR_SPOT
#undef ML_STR_C

// An empty string; symbols.txt names it as a float.

// `i = 0; activeGroup = i;` plus `opt_propagation off` matched this; the pragma keeps the assignment
// from constant-folding to a fresh r0 so i keeps r30.
#pragma opt_propagation off // MUST_MATCH: keep activeGroup=i from folding to a fresh r0 so i keeps r30
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
#pragma opt_propagation reset

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

// NONMATCHING (99.62%): only the third loop's counter/base registers differ (target i=r31,
// mapWork=r30; ours i=r30, mapWork=r31). The first two loops match after `i = 0;` was hoisted
// before `wp`/`entry`, and `memset(wp, ...)` reuses wp instead of recomputing &mapWork[1].
void bmapUnLoad(void) {
	MapWork* wp;
	MapEntry* entry;
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
	memset(wp, 0, 0x2F4);

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

// All registers, calls and immediates match. Only the opening block's scheduling differs: retail
// keeps `li offset=0` ahead of `lwz wp=mapWork` and the `stw activeGroup` that reuses offset's r28,
// while MWCC hoists the load above the li; no source spelling reaches the scheduler tie.
// NONMATCHING (97.80%): opening load hoisted above the offset li.
void bmapLoad(const char* map1, const char* map2) {
	s32 offset;
	u8* p;
	MapEntry* entry;
	s32 i;
	MapWork* wp;

	sysWaitDrawSync();
	activeGroup = offset = 0;
	wp = mapWork;
	i = 0;
	p = (u8*)wp;
	for (; i < wp->entries[0].count; i++) {
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

// in-file layout for the animation key records mapCalcAnimMatrix walks
// (the joint pointer passed by hitdrv.c points at an array of these)



// NONMATCHING (99.29%): spline math, constants, the d==0 boolean (cntlzw/srwi) and the inner-loop
// GPRs (ch=r3, out=r4, chn=r5) now match. What differs is FPR colouring: the key-select time t is f13
// in retail but f6 here, which cascades k0/dt and the polynomial temps (u, u2, neg2). Permuter.
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
	f32 u, u2, u3;

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
#ifdef MUST_MATCH
			f32* outp = &out[k]; // MUST_MATCH: declared before chn so MWCC colours out=r4, chn=r5 like retail
#endif
			MapAnimChannel* chn = &next->chan[k];

			if (ch->flag != 0) {
#ifdef MUST_MATCH
				*outp = ch->value;
#else
				out[k] = ch->value;
#endif
			}
			else {
				f32 k0 = key->time;
				f32 k1 = next->time;
				f32 tan1 = chn->unk4;
				f32 value1 = chn->value;
				f32 tan0 = ch->unk8;
				f32 value0 = ch->value;
				f32 dt = t - k0;
				f32 d = k1 - k0;
				f32 den;
				s32 flag;
				f32 a, b, c, h10, h00, h01, h11;
				f32 two, neg2;

				flag = (d != MATCH_F32(float_0_80419850));
				if (((u32)__cntlzw(flag) >> 5) != 0) {
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
#ifdef MUST_MATCH
				*outp = tan0 * (d * h10)
#else
				out[k] = tan0 * (d * h10)
#endif
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

// Inline copy of mapSearchAnmObj for mapMain. The inner-loop bound assigns the table
// (`table = *(u8**)((u8*)entry + 0xA0)`) inside the condition so the body reuses the condition's
// loaded register (retail loads `0xa0(entry)` once per iteration, not twice), and the pair address
// is built `+4` then `+= j * 8` to match retail's operand order.
static inline u8* mapSearchAnimObj(const char* name) {
	u8* p;
	u8* table;
	MapEntry* entry;
	MapWork* wp;
	s32 j;
	s32 i;
	wp = &mapWork[activeGroup];
	entry = &wp->entries[0];
	for (i = 0; i < wp->entries[0].count; entry++, i++) {
		if (*(u8**)((u8*)entry + 0xA0) == NULL) {
			return NULL;
		}
		for (j = 0; j < *(s32*)(table = *(u8**)((u8*)entry + 0xA0)); j++) {
			u8* pair = (u8*)((u32)table + j * 8 + 4);
			if (!strcmp(*(char**)pair, name)) {
				return *(u8**)(pair + 4);
			}
		}
	}
	return NULL;
}

// Complete and logically correct; the instruction shape matches retail exactly (verified with
// tools/diff_shape.py: no shape-only diffs). 96.21%.
// Source shapes that mattered: (1) the animation `item` pointer must be re-loaded from the list
// inside each `if (obj != NULL)` / `if (light != NULL)` block instead of cached across the block's
// mapSearchAnimObj / lightNameToPtr calls (and each light block re-loads it after the lookup);
// (2) the flags word `*(u16*)anim` is read once into `animFlags` at the top of the j-body -- that
// keeps it in r4 across the branches instead of letting the gp-pointer loads clobber r3 and force
// a reload; (3) in the mapSearchAnimObj helper the table is read with the expression inside the
// inner-loop bound (assigned to `table` in the bound), so retail's second `0xa0(entry)` load
// disappears; (4) the main-function declaration order moves the callee-saved allocation a lot: a
// greedy swap/insertion hill-climb over the 15 top-level locals went 96.03 ->
// 96.20 by itself, so this list is the optimum found, not an arbitrary order; (5) the four
// orientation-load orders per Hermite block matter for the FPR/score even though the scheduler
// reorders them (colour `tan0, value0, value1, tan1`, light `tan0, value0, value1, tan1`, alpha
// `tan1, tan0, value0, value1` at this decl order); (6) the colour loop increment order
// `k++, chn += 0x14, ch += 0x14` and the light-pos order `ch += 0x14, k++, chn += 0x14` each beat
// the retail-looking left-to-right order; (7) the per-ki declaration order `f32 t; s32 k;` then
// `u8* chn; u8* ch;` and the colour `chn = next; ch = prev;` (reverse of source-reading order) move
// the FPR allocation by ~0.09 together with (4); (8) the helper declaration order
// `p, table, entry, wp, j, i` is the optimum for the caller's colouring.
// What still differs (all arg_mismatch): retail keeps i r14, entry r24, anim r21, updated r20,
// j r23, list19 r19, list18 r18 and list17 r17 (all matched now); ours still has list16 r25,
// list15 r22 and the OSTime in r15/r16 where retail has list16 r16, list15 r15 and the OSTime in
// r22/r25. The inlined Hermite blocks colour t in f5 vs retail's f12 (colour/light) or f2
// (alpha, where only d and the orientation FPRs differ), so the polynomial FPRs rotate; and the
// `if (!(t <= next))` diamond lays out as `beq after` vs retail's `bne body; b after` (8
// op_mismatch). The Hermite output sum evaluates `value0 * h00` before `tan0 * (d * h10)` and
// the light blocks declare `ki` first, which together move the FPR/callee-saved colouring; the
// remaining FPR rotation is allocator colouring that no further C shape found moves (the
// remaining levers -- a two-u32 split of `time`, a real inlined Hermite helper -- either do not
// move the colouring or make MWCC hang).
// The d==0 boolean uses the __cntlzw intrinsic to reproduce retail's cntlzw/srwi normalisation.
// The colour/light Hermite loops have no `if (cnt > 1)` wrapper: the bare `for (ki = 1; ...)`
// guard reproduces retail's `subi cnt-1; mtctr` scheduled before the `cmpwi`.
#pragma opt_loop_invariants off // pins the colour/light channel loads and fixes allocator colouring
#pragma opt_common_subs off // same: required for retail's register colouring here
// NONMATCHING (96.69%): the eight clamp diamonds still emit `beq` where retail has `bne`+`b`;
// the rest is FPR colouring (t f5 vs f12, d f6 vs f11), plus the list16/list15 GPR swap.
void mapMain(void) {
	u8* list19;
	MapEntry* entry;
	s32 i = 0;
	s32 j = 0;
	u8* list18;
	u8* anim;
	BOOL updated;
	OSTime time;
	char namebuf[0x100];
	u8* list16;
	u8* p;
	MapWork* wp0;
	u8* list15;
	u8* list17;
	char namebuf2[0x100];

	wp0 = &mapWork[activeGroup];
	for (entry = &wp0->entries[0]; i < wp0->entries[0].count; entry++, i++) {
		anim = (u8*)entry->animData;
		updated = FALSE;
		for (j = 0; j < *(s32*)entry->field_0x160; j++, anim += 0x20) {
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
				u8* lp = list19;
				s32 li;
				for (li = 0; li < *(s32*)list19; lp += 4, li++) {
					MapObject* obj = mapGetMapObjSub(*(char**)*(u8**)(lp + 4));
					if (obj != NULL) {
					mapCalcAnimMatrix((f32(*)[4])((u8*)obj + 0x7C), (f32(*)[4])((u8*)obj + 0x4C), (MapEntryAnimData*)anim, *(u8**)(lp + 4));
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
				for (li = 0; li < *(s32*)list18; lp += 4, li++) {
					u8* obj;
					strcpy(namebuf, *(char**)*(u8**)(lp + 4));
					obj = mapSearchAnimObj(namebuf);
					if (obj != NULL) {
						u8* item = *(u8**)(lp + 4);
						s32 cnt = *(s32*)(item + 0x10);
						s32 ki;
						for (ki = 1; ki < cnt; ki++) {
							u8* prev = item + 0x14 + (ki - 1) * 0x68;
							u8* next = item + 0x14 + ki * 0x68;
							f32 t = *(f32*)(anim + 0x10);
							s32 k;
							u8* chn;
							u8* ch;
							if (!(t <= *(f32*)next)) {
								if (ki == cnt - 1) {
									t = *(f32*)next;
								}
								else {
									continue;
								}
							}
							chn = next;
							ch = prev;
							for (k = 0; k < 5; k++, chn += 0x14, ch += 0x14) {
								if (*(s32*)(ch + 0x14) != 0) {
									*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x2C + k * 4) = *(f32*)(ch + 4);
								}
								else {
									f32 k0 = *(f32*)prev;
									f32 k1 = *(f32*)next;
									f32 dt = t - k0;
									f32 d = k1 - k0;
									f32 value0 = *(f32*)(ch + 4);
									f32 tan0 = *(f32*)(ch + 0xC);
									f32 value1 = *(f32*)(chn + 4);
									f32 tan1 = *(f32*)(chn + 8);
									f32 den;
									f32 h11, h00, u2, h01, c, u3, b, u, a, h10;
									s32 flag;
									flag = (d != MATCH_F32(float_0_80419850));
									if (((u32)__cntlzw(flag) >> 5) != 0) {
										den = float_1Eneg08_804198b8;
									}
									else {
										den = d;
									}
									u = dt / den;
									u2 = u * u;
									u3 = u2 * u;
									a = float_3_8041984c * u2;
									b = u3 - float_2_80419858 * u2;
									c = float_2_80419858 * u3 - a;
									h11 = u3 - u2;
									h10 = u + b;
									h00 = float_1_80419860 + c;
									h01 = float_neg2_804198a0 * u3 + a;
									*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x2C + k * 4) = (value0 * h00) + (tan0 * (d * h10)) + value1 * h01 + tan1 * (d * h11);
								}
							}
							*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x40) = *(f32*)(item + 8);
							*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x44) = *(f32*)(item + 0xC);
							break;
						}
					}
					strcpy(namebuf, *(char**)*(u8**)(lp + 4));
					strcat(namebuf, "_v");
					obj = mapSearchAnimObj(namebuf);
					if (obj != NULL) {
						u8* item = *(u8**)(lp + 4);
						s32 cnt = *(s32*)(item + 0x10);
						s32 ki;
						for (ki = 1; ki < cnt; ki++) {
							u8* prev = item + 0x14 + (ki - 1) * 0x68;
							u8* next = item + 0x14 + ki * 0x68;
							f32 t = *(f32*)(anim + 0x10);
							s32 k;
							u8* chn;
							u8* ch;
							if (!(t <= *(f32*)next)) {
								if (ki == cnt - 1) {
									t = *(f32*)next;
								}
								else {
									continue;
								}
							}
							chn = next;
							ch = prev;
							for (k = 0; k < 5; k++, chn += 0x14, ch += 0x14) {
								if (*(s32*)(ch + 0x14) != 0) {
									*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x2C + k * 4) = *(f32*)(ch + 4);
								}
								else {
									f32 k0 = *(f32*)prev;
									f32 k1 = *(f32*)next;
									f32 dt = t - k0;
									f32 d = k1 - k0;
									f32 value0 = *(f32*)(ch + 4);
									f32 tan0 = *(f32*)(ch + 0xC);
									f32 value1 = *(f32*)(chn + 4);
									f32 tan1 = *(f32*)(chn + 8);
									f32 den;
									f32 h11, h00, u2, h01, c, u3, b, u, a, h10;
									s32 flag;
									flag = (d != MATCH_F32(float_0_80419850));
									if (((u32)__cntlzw(flag) >> 5) != 0) {
										den = float_1Eneg08_804198b8;
									}
									else {
										den = d;
									}
									u = dt / den;
									u2 = u * u;
									u3 = u2 * u;
									a = float_3_8041984c * u2;
									b = u3 - float_2_80419858 * u2;
									c = float_2_80419858 * u3 - a;
									h11 = u3 - u2;
									h10 = u + b;
									h00 = float_1_80419860 + c;
									h01 = float_neg2_804198a0 * u3 + a;
									*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x2C + k * 4) = (value0 * h00) + (tan0 * (d * h10)) + value1 * h01 + tan1 * (d * h11);
								}
							}
							*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x40) = *(f32*)(item + 8);
							*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x44) = *(f32*)(item + 0xC);
							break;
						}
					}
					strcpy(namebuf, *(char**)*(u8**)(lp + 4));
					strcat(namebuf, "_v_x");
					obj = mapSearchAnimObj(namebuf);
					if (obj != NULL) {
						u8* item = *(u8**)(lp + 4);
						s32 cnt = *(s32*)(item + 0x10);
						s32 ki;
						for (ki = 1; ki < cnt; ki++) {
							u8* prev = item + 0x14 + (ki - 1) * 0x68;
							u8* next = item + 0x14 + ki * 0x68;
							f32 t = *(f32*)(anim + 0x10);
							s32 k;
							u8* chn;
							u8* ch;
							if (!(t <= *(f32*)next)) {
								if (ki == cnt - 1) {
									t = *(f32*)next;
								}
								else {
									continue;
								}
							}
							chn = next;
							ch = prev;
							for (k = 0; k < 5; k++, chn += 0x14, ch += 0x14) {
								if (*(s32*)(ch + 0x14) != 0) {
									*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x2C + k * 4) = *(f32*)(ch + 4);
								}
								else {
									f32 k0 = *(f32*)prev;
									f32 k1 = *(f32*)next;
									f32 dt = t - k0;
									f32 d = k1 - k0;
									f32 value0 = *(f32*)(ch + 4);
									f32 tan0 = *(f32*)(ch + 0xC);
									f32 value1 = *(f32*)(chn + 4);
									f32 tan1 = *(f32*)(chn + 8);
									f32 den;
									f32 h11, h00, u2, h01, c, u3, b, u, a, h10;
									s32 flag;
									flag = (d != MATCH_F32(float_0_80419850));
									if (((u32)__cntlzw(flag) >> 5) != 0) {
										den = float_1Eneg08_804198b8;
									}
									else {
										den = d;
									}
									u = dt / den;
									u2 = u * u;
									u3 = u2 * u;
									a = float_3_8041984c * u2;
									b = u3 - float_2_80419858 * u2;
									c = float_2_80419858 * u3 - a;
									h11 = u3 - u2;
									h10 = u + b;
									h00 = float_1_80419860 + c;
									h01 = float_neg2_804198a0 * u3 + a;
									*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x2C + k * 4) = (value0 * h00) + (tan0 * (d * h10)) + value1 * h01 + tan1 * (d * h11);
								}
							}
							*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x40) = *(f32*)(item + 8);
							*(f32*)(obj + (*(s32*)(item + 4)) * 0x1C + 0x44) = *(f32*)(item + 0xC);
							break;
						}
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
						u8* item = *(u8**)(lp + 4);
						s32 cnt = *(s32*)(item + 4);
						s32 ki;
						for (ki = 1; ki < cnt; ki++) {
							u8* prev = item + 8 + (ki - 1) * 0x54;
							u8* next = item + 8 + ki * 0x54;
							f32 t = *(f32*)(anim + 0x10);
							if (!(t <= *(f32*)next)) {
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
								f32 dt = t - k0;
								f32 d = k1 - k0;
								f32 tan1 = *(f32*)(next + 8);
								f32 tan0 = *(f32*)(prev + 0xC);
								f32 value0 = *(f32*)(prev + 4);
								f32 value1 = *(f32*)(next + 4);
								f32 den;
								f32 u, u2, a, u3, b, c, h10, h00, h01, h11, val;
								s32 flag;
								flag = (d != MATCH_F32(float_0_80419850));
								if (((u32)__cntlzw(flag) >> 5) != 0) {
									den = float_1Eneg08_804198b8;
								}
								else {
									den = d;
								}
								u = dt / den;
								u2 = u * u;
								u3 = u2 * u;
								a = float_3_8041984c * u2;
								b = u3 - float_2_80419858 * u2;
								c = float_2_80419858 * u3 - a;
								h11 = u3 - u2;
								h10 = u + b;
								h00 = float_1_80419860 + c;
								h01 = float_neg2_804198a0 * u3 + a;
								val = tan0 * (d * h10) + value0 * h00 + value1 * h01 + tan1 * (d * h11);
								*(u8*)(obj + 0x10C) = (s32)(255.0f - 255.0f * val);
								break;
							}
						}
					}
					strcpy(namebuf2, *(char**)*(u8**)(lp + 4));
					strcat(namebuf2, "_v");
					obj = mapSearchAnimObj(namebuf2);
					if (obj != NULL) {
						u8* item = *(u8**)(lp + 4);
						s32 cnt = *(s32*)(item + 4);
						s32 ki;
						for (ki = 1; ki < cnt; ki++) {
							u8* prev = item + 8 + (ki - 1) * 0x54;
							u8* next = item + 8 + ki * 0x54;
							f32 t = *(f32*)(anim + 0x10);
							if (!(t <= *(f32*)next)) {
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
								f32 dt = t - k0;
								f32 d = k1 - k0;
								f32 tan1 = *(f32*)(next + 8);
								f32 tan0 = *(f32*)(prev + 0xC);
								f32 value0 = *(f32*)(prev + 4);
								f32 value1 = *(f32*)(next + 4);
								f32 den;
								f32 u, u2, a, u3, b, c, h10, h00, h01, h11, val;
								s32 flag;
								flag = (d != MATCH_F32(float_0_80419850));
								if (((u32)__cntlzw(flag) >> 5) != 0) {
									den = float_1Eneg08_804198b8;
								}
								else {
									den = d;
								}
								u = dt / den;
								u2 = u * u;
								u3 = u2 * u;
								a = float_3_8041984c * u2;
								b = u3 - float_2_80419858 * u2;
								c = float_2_80419858 * u3 - a;
								h11 = u3 - u2;
								h10 = u + b;
								h00 = float_1_80419860 + c;
								h01 = float_neg2_804198a0 * u3 + a;
								val = tan0 * (d * h10) + value0 * h00 + value1 * h01 + tan1 * (d * h11);
								*(u8*)(obj + 0x10C) = (s32)(255.0f - 255.0f * val);
								break;
							}
						}
					}
					strcpy(namebuf2, *(char**)*(u8**)(lp + 4));
					strcat(namebuf2, "_v_x");
					obj = mapSearchAnimObj(namebuf2);
					if (obj != NULL) {
						u8* item = *(u8**)(lp + 4);
						s32 cnt = *(s32*)(item + 4);
						s32 ki;
						for (ki = 1; ki < cnt; ki++) {
							u8* prev = item + 8 + (ki - 1) * 0x54;
							u8* next = item + 8 + ki * 0x54;
							f32 t = *(f32*)(anim + 0x10);
							if (!(t <= *(f32*)next)) {
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
								f32 dt = t - k0;
								f32 d = k1 - k0;
								f32 tan1 = *(f32*)(next + 8);
								f32 tan0 = *(f32*)(prev + 0xC);
								f32 value0 = *(f32*)(prev + 4);
								f32 value1 = *(f32*)(next + 4);
								f32 den;
								f32 u, u2, a, u3, b, c, h10, h00, h01, h11, val;
								s32 flag;
								flag = (d != MATCH_F32(float_0_80419850));
								if (((u32)__cntlzw(flag) >> 5) != 0) {
									den = float_1Eneg08_804198b8;
								}
								else {
									den = d;
								}
								u = dt / den;
								u2 = u * u;
								u3 = u2 * u;
								a = float_3_8041984c * u2;
								b = u3 - float_2_80419858 * u2;
								c = float_2_80419858 * u3 - a;
								h11 = u3 - u2;
								h10 = u + b;
								h00 = float_1_80419860 + c;
								h01 = float_neg2_804198a0 * u3 + a;
								val = tan0 * (d * h10) + value0 * h00 + value1 * h01 + tan1 * (d * h11);
								*(u8*)(obj + 0x10C) = (s32)(255.0f - 255.0f * val);
								break;
							}
						}
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
					s32 ki;
					u8* item = *(u8**)(lp + 4);
					s32 cnt = *(s32*)(item + 4);
					f32 out[9];
					for (ki = 1; ki < cnt; ki++) {
						u8* prev = item + 8 + (ki - 1) * 0xB8;
						u8* next = item + 8 + ki * 0xB8;
						f32 t = *(f32*)(anim + 0x10);
						s32 k;
						u8* chn;
						u8* ch;
						if (!(t <= *(f32*)next)) {
							if (ki == cnt - 1) {
								t = *(f32*)next;
							}
							else {
								continue;
							}
						}
						ch = prev;
						chn = next;
						for (k = 0; k < 9; ch += 0x14, k++, chn += 0x14) {
							if (*(s32*)(ch + 0x14) != 0) {
								out[k] = *(f32*)(ch + 4);
							}
							else {
								f32 k0 = *(f32*)prev;
								f32 k1 = *(f32*)next;
								f32 dt = t - k0;
								f32 d = k1 - k0;
								f32 value0 = *(f32*)(ch + 4);
								f32 tan0 = *(f32*)(ch + 0xC);
								f32 value1 = *(f32*)(chn + 4);
								f32 tan1 = *(f32*)(chn + 8);
								f32 den;
								f32 h11, h00, u2, h01, c, u3, b, u, a, h10;
								s32 flag;
								flag = (d != MATCH_F32(float_0_80419850));
								if (((u32)__cntlzw(flag) >> 5) != 0) {
									den = float_1Eneg08_804198b8;
								}
								else {
									den = d;
								}
								u = dt / den;
								u2 = u * u;
								u3 = u2 * u;
								a = float_3_8041984c * u2;
								b = u3 - float_2_80419858 * u2;
								c = float_2_80419858 * u3 - a;
								h11 = u3 - u2;
								h10 = u + b;
								h00 = float_1_80419860 + c;
								h01 = float_neg2_804198a0 * u3 + a;
								out[k] = (value0 * h00) + (tan0 * (d * h10)) + value1 * h01 + tan1 * (d * h11);
							}
						}
						{
							f32 scale = float_10_80419864;
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
				}
			}
			/* light animation (color/angle) */
			if (list15 != NULL) {
				s32 li;
				u8* lp = list15;
				for (li = 0; li < *(s32*)list15; lp += 4, li++) {
					LightEntry* light = lightNameToPtr(*(char**)*(u8**)(lp + 4));
					if (light != NULL) {
					s32 ki;
					u8* item = *(u8**)(lp + 4);
					s32 cnt = *(s32*)(item + 4);
					f32 out[5];
					for (ki = 1; ki < cnt; ki++) {
						u8* prev = item + 8 + (ki - 1) * 0x68;
						u8* next = item + 8 + ki * 0x68;
						f32 t = *(f32*)(anim + 0x10);
						s32 k;
						u8* chn;
						u8* ch;
						if (!(t <= *(f32*)next)) {
							if (ki == cnt - 1) {
								t = *(f32*)next;
							}
							else {
								continue;
							}
						}
						ch = prev;
						chn = next;
						for (k = 0; k < 5; k++, chn += 0x14, ch += 0x14) {
							if (*(s32*)(ch + 0x14) != 0) {
								out[k] = *(f32*)(ch + 4);
							}
							else {
								f32 k0 = *(f32*)prev;
								f32 k1 = *(f32*)next;
								f32 dt = t - k0;
								f32 d = k1 - k0;
								f32 value0 = *(f32*)(ch + 4);
								f32 tan0 = *(f32*)(ch + 0xC);
								f32 value1 = *(f32*)(chn + 4);
								f32 tan1 = *(f32*)(chn + 8);
								f32 den;
								f32 h11, h00, u2, h01, c, u3, b, u, a, h10;
								s32 flag;
								flag = (d != MATCH_F32(float_0_80419850));
								if (((u32)__cntlzw(flag) >> 5) != 0) {
									den = float_1Eneg08_804198b8;
								}
								else {
									den = d;
								}
								u = dt / den;
								u2 = u * u;
								u3 = u2 * u;
								a = float_3_8041984c * u2;
								b = u3 - float_2_80419858 * u2;
								c = float_2_80419858 * u3 - a;
								h11 = u3 - u2;
								h10 = u + b;
								h00 = float_1_80419860 + c;
								h01 = float_neg2_804198a0 * u3 + a;
								out[k] = (value0 * h00) + (tan0 * (d * h10)) + value1 * h01 + tan1 * (d * h11);
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
		if (updated) {
			Mtx mtx;
			PSMTXScale(mtx, float_10_80419864, float_10_80419864, float_10_80419864);
			mapReCalcMatrix(entry->rootMapObj, mtx, 0);
		}
	}
}
#pragma opt_common_subs reset
#pragma opt_loop_invariants reset

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
	offscreenAddBoundingBox(*(s16*)obj->field_0xDC, v0, v1, v2, v3);
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

void _mapDispMapGrp_NoMaterial(MapObject* obj, void* a2) {
	MapObject* o = (MapObject*)a2;

	_mapDispMapObj_NoMaterial((s32)obj, o);
	if (o->child)
#ifdef MUST_MATCH
		// match-only: the redundant cast is what makes MWCC colour the camera id to r30 and o to r31
		_mapDispMapGrp_NoMaterial((MapObject*)obj, o->child);
#else
		_mapDispMapGrp_NoMaterial(obj, o->child);
#endif
	if ((*(u32*)o->field_0x0 & 0x20) && o->next)
#ifdef MUST_MATCH
		_mapDispMapGrp_NoMaterial((MapObject*)obj, o->next);
#else
		_mapDispMapGrp_NoMaterial(obj, o->next);
#endif
}

// NONMATCHING (99.12%): loop counter i/offset register swap only (target i=r31, off=r30 with an
// `mr r30,r31` init; ours i=r30, off=r31). The `(u32)(off >> 3)` index is match-only.
void mapDispMapObj_bbox(s32 camId, MapObject* obj) {
	Mtx invMtx;
	Mtx mtx;
	s32 i;
	s32 off;
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
#ifdef MUST_MATCH
	// MUST_MATCH: the explicit off induction variable and the (u32) index cast are what put the
	// counter in r31/off in r30 (98.84% -> 99.12%); the natural indexed loop is under #else.
	i = 0;
	off = i;
	for (; i < obj->joints->partCount; i++, off += 8) {
		mapSetPolygon(obj, obj->joints->parts[(u32)(off >> 3)].mesh);
	}
#else
	for (i = 0; i < obj->joints->partCount; i++) {
		mapSetPolygon(obj, obj->joints->parts[i].mesh);
	}
#endif
	sysWaitDrawSync();
	GXReadBoundingBox((u16*)((u8*)obj + 0xEC), (u16*)((u8*)obj + 0xEE), (u16*)((u8*)obj + 0xF0), (u16*)((u8*)obj + 0xF2));
}

void mapDispMapGrp_bbox(MapObject* obj, MapEntry* entry) {
	sysWaitDrawSync();
	GXClearBoundingBox();
	_mapDispMapGrp_NoMaterial(obj, entry);
	sysWaitDrawSync();
	GXReadBoundingBox((u16*)((u8*)entry + 0xEC), (u16*)((u8*)entry + 0xEE), (u16*)((u8*)entry + 0xF0), (u16*)((u8*)entry + 0xF2));
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

#pragma opt_lifetimes off // needed: default lifetime analysis colours 15 more instructions wrong (243 -> 228 diffs)
// Register colouring/scheduling only (228 real instruction diffs, 21 symbol-only). The unit's .rodata
// (vec3_802c16d8, double_to_int_mask_802c19a0, ...) is not defined here yet, so MWCC addresses the pooled
// i2f double absolutely (lis/lfd) instead of `lfd f2, 0x2c8(r28)`, and TEXMTX uses add+lwzu (16 sites).
// NONMATCHING (96.99%): the identity casts on the texMap/texCoord init below removed 14 diffs; the
// else-branch constant order in the flags&0x80100000 block was reordered for the same reason.
void mapSetMaterialTev(s32 texCount, s32 drawMode, s32 flags, Mtx mtx) {
	u8* base;
	s32 texMap;
	s32 texCoord;
	GXTevStageID tevStage;
	s32 numIndStages;
	Mtx queen_mtx;

	base = vec3_802c16d8;
#ifdef MUST_MATCH
	// Identity casts break MWCC's coalescing of texMap with texCount and texCoord with texMap; without
	// them the allocator parks texCount in r21, rematerialises texMap from it at every TEXMTX site, and
	// turns the table loads into add+lwzu (240 -> 228 diffs).
	texMap = (s32)texCount;
	texCoord = (s32)texMap;
#else
	texMap = texCount;
	texCoord = texCount;
#endif
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
			r21 = 0xF0;
			r22 = 0x64;
			r19 = 0xF0;
			r20 = (u8)(64.0f * (1.0f - (f32)cos(1.5707964f * (f32)gp->retraceCount / 20.0f)) * 0.5f + 64.0f);
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
		v = (s32)(127.0f * (1.0f - (f32)cos(1.5707964f * (f32)gp->retraceCount / 30.0f)) * 0.5f + 128.0f);
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
		cbIndex = (u32)(flags & 0xF000000) >> 22 & 0x3FC;
		cbBase = (u8*)&mapWork[activeGroup].entries[0] + 0x110;
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
#pragma opt_lifetimes reset

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

// material layout (MapMaterialTex only covers 0xB/0x2C onwards)

// The original reaches texwrap_tbl, lightid_tbl and the default light direction (vec3_802c181c) through the
// unit's .rodata base vec3_802c16d8.

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

// The calls and branches all match. Three instructions are still added: the light index table is materialised
// as vec3_802c16d8 + 0x100 with a second copy of the base address (+1 lis, +1 addi) where retail reaches it as
// r28 + 0x100, and the u8 -> f32 LOD conversion loads the compiler's pooled int->double mask (+1 lis) where
// retail loads the unit's double_to_int_mask from the .rodata base (+0x2C8). Both need the unit's .rodata
// objects defined so MWCC can fold the offsets; without them the frontend emits a fresh address. The rest is
// register colouring: the light-loop counter lands in r25 instead of r22, the texture-loop ref/wp/desc in
// r23/r24/r23 instead of r22/r29/r28. The final mapSetMaterialTev tail now matches (the argument expression is
// inlined so the drawMode conditional is evaluated first). A rasim search over the named locals'
// declaration order cannot reach retail's colouring (the counter and texMap coalesce), so it is allocator-only.
// Loads the map object's lights into GX light slots (point, spot and directional), sets the colour channel from
// the material, loads each of the material's textures by name from the map's TPL, then sets up TEV.
// NONMATCHING (98.13%): base-relative lightId/i2f loads (3 instructions) and register colouring; see above.
void mapSetMaterial(MapObject* obj, void* materialPtr) {
	s32 i;
	s32 texMap;
	MapMaterial* mat;
	GXLightObj lightObj;
	Mtx mtxX;
	Mtx mtxY;
	Mtx mtxZ;
	LightEntry* lights[8];
	LightEntry* light;
	GXTexObj texObj;
	Vec pos;
	Vec defaultDir;
	GXColor ambient;
	GXColor defaultAmbient;
#ifdef MUST_MATCH
	f32 spotAngle;
#endif
	s32* lightId;
	u32 lightMask;
#ifdef MUST_MATCH
	s32* wrapTbl;
#endif
	s32 attn;
#ifdef MUST_MATCH
	u8* base;
#endif
	s32 count;

	mat = materialPtr;
#ifdef MUST_MATCH
	base = vec3_802c16d8;
	lightId = (s32*)(base + 0x100);
#else
	lightId = (s32*)lightid_tbl;
#endif
	texMap = 0;
	attn = 2;
	defaultAmbient = dat_80419828;
	lightMask = 0;
	count = mapSetLight(obj, lights);

	for (i = 0; i < count; i++) {

		defaultDir = MAPMAT_DEFAULT_DIR;
		light = lights[i];
		if (light->flags & LIGHT_POINT) {
			CameraEntry* cam = camGetCurPtr();

			PSMTXMultVec(cam->view, &light->position, &pos);
			GXInitLightPos(&lightObj, pos.x, pos.y, pos.z);
			GXInitLightColor(&lightObj, lights[i]->color);
			GXInitLightAttn(&lightObj, lights[i]->attenAngle, float_0_80419850, float_0_80419850,
			                lights[i]->attenDistCoef.x, lights[i]->attenDistCoef.y, lights[i]->attenDistCoef.z);
			GXLoadLightObjImm(&lightObj, lightId[i]);
			attn = 1;
		}
		else if (light->flags & LIGHT_SPOT) {
			PSMTXRotRad(mtxX, 'x', float_deg2rad_80419868 * light->rotation.x);
			PSMTXRotRad(mtxY, 'y', float_deg2rad_80419868 * lights[i]->rotation.y);
			PSMTXRotRad(mtxZ, 'z', float_deg2rad_80419868 * lights[i]->rotation.z);
			PSMTXConcat(mtxZ, mtxY, mtxY);
			PSMTXConcat(mtxY, mtxX, mtxX);
			PSMTXMultVec(mtxX, &defaultDir, &defaultDir);
			PSVECScale(&defaultDir, &defaultDir, float_neg1p0486E06_80419870);
			PSVECAdd(&lights[i]->position, &defaultDir, &pos);
			{
				CameraEntry* cam = camGetCurPtr();

				PSMTXMultVec(cam->view, &pos, &pos);
			}
			GXInitLightPos(&lightObj, pos.x, pos.y, pos.z);
			GXInitLightColor(&lightObj, lights[i]->color);
			GXInitLightAttn(&lightObj, lights[i]->attenAngle, float_0_80419850, float_0_80419850,
			                float_0_80419850, float_0_80419850, float_1_80419860);
			GXLoadLightObjImm(&lightObj, lightId[i]);
		}
		else if (light->flags & LIGHT_DIRECTIONAL) {
			CameraEntry* cam = camGetCurPtr();

			PSMTXMultVec(cam->view, &light->position, &pos);
			PSMTXRotRad(mtxX, 'x', float_deg2rad_80419868 * lights[i]->rotation.x);
			PSMTXRotRad(mtxY, 'y', float_deg2rad_80419868 * lights[i]->rotation.y);
			PSMTXRotRad(mtxZ, 'z', float_deg2rad_80419868 * lights[i]->rotation.z);
			PSMTXConcat(mtxZ, mtxY, mtxY);
			PSMTXConcat(mtxY, mtxX, mtxX);
			PSMTXMultVec(mtxX, &defaultDir, &defaultDir);
			cam = camGetCurPtr();
			PSMTXMultVecSR(cam->view, &defaultDir, &defaultDir);
			GXInitLightPos(&lightObj, pos.x, pos.y, pos.z);
			GXInitLightDir(&lightObj, defaultDir.x, defaultDir.y, defaultDir.z);
#ifdef MUST_MATCH
			spotAngle = lights[i]->spotAngle;
			GXInitLightSpot(&lightObj, spotAngle * float_0p5_80419854, GX_SP_SHARP);
#else
			GXInitLightSpot(&lightObj, lights[i]->spotAngle * float_0p5_80419854, GX_SP_SHARP);
#endif
			GXInitLightDistAttn(&lightObj, float_1000_80419874, float_0p5_80419854, GX_DA_OFF);
			GXInitLightColor(&lightObj, lights[i]->color);
			GXLoadLightObjImm(&lightObj, lightId[i]);
			attn = 1;
		}
		lightMask |= lightId[i];
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

	texMap = 0;
#ifdef MUST_MATCH
	wrapTbl = (s32*)(base + 0xEC);
#endif
	for (i = mat->texCount - 1; i >= 0; i--) {
		MapMatTexRef* ref = mat->texRefs[i];
		TEXDescriptor* desc;
		MapWork* wp = &mapWork[activeGroup];

		desc = TEXGet(wp->entries[*(s16*)(obj->field_0xDC + 2)].tpl,
		              mapMatSearchTex(*(s32**)(wp->entries[*(s16*)(obj->field_0xDC + 2)].field_0x8C + 8),
		                              *(const char**)ref->field_0x0));
		GXInitTexObj(&texObj, desc->textureHeader->data, desc->textureHeader->width, desc->textureHeader->height,
		             desc->textureHeader->format, MAPMAT_TEXWRAP(ref->wrapS), MAPMAT_TEXWRAP(ref->wrapT),
		             desc->textureHeader->minLOD == desc->textureHeader->maxLOD ? GX_FALSE : GX_TRUE);
		GXInitTexObjLOD(&texObj, GX_LINEAR, GX_LINEAR, (f32)desc->textureHeader->minLOD,
		                (f32)desc->textureHeader->maxLOD, desc->textureHeader->LODBias, GX_DISABLE,
		                desc->textureHeader->edgeLODEnable, GX_ANISO_1);
		GXLoadTexObj(&texObj, texMap);
		texMap++;
	}

	mapSetMaterialTev(mat->texCount, mat->field_0x110 ? mat->field_0x110[0] : 0,
	                  *(s32*)obj->field_0x0, obj->modelWorldMtx);
}

void mapSetPaperAmbColor(GXColor* color) {
	paper_ambient = *color;
}

void mapResetPaperAmbColor(void) {
	paper_ambient = dat_8041982c;
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

// vertex layout of an unpacked display list (stride 0x18)

// per-mesh vtx-format block (the value at entry->field_0x8C + 0x18)

#pragma opt_common_subs off // MUST_MATCH: stop MWCC CSEing a shared base into the wp/nrm/pos slots
// NONMATCHING (99.93%): 3 insns differ, all one register: the final GXCallDisplayList loop counter lands in
// r25 (the freed colour-loop `i`) instead of the target's r28 (the freed `mask`); only the `li`/`addi`/`cmpw`
// names differ. The vertex-loop `p = &vp->x` scratch is matched by reading z as `p[2]`.
void mapSetPolygon(MapObject* obj, MapFileMesh* mesh) {
	MapWork* wp = mapGetWork();
	u32* vcd = (u32*)mesh->vcdTableOffset;
	void* pos = (void*)(vcd[0] + 4);
	void* nrm = (void*)(vcd[1] + 4);
	s32 texCount = vcd[5];
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
			fmt = (MapVtxFmt*)*(void**)(base + *(s16*)(obj->field_0xDC + 2) * 0x178);
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
			fmt = (MapVtxFmt*)*(void**)(wp->entries[*(s16*)(obj->field_0xDC + 2)].field_0x8C + 0x18);
			GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0 + i, GX_TEX_ST, GX_S16, (u8)fmt->field_0x48[i]);
			GXSetArray(GX_VA_TEX0 + i, (void*)(((u32*)mesh->vcdTableOffset)[6 + i] + 4), 4);
		}

		{
			u8* dp = (u8*)mesh;

			for (j = 0; j < mesh->displayListCount; j++) {
				GXCallDisplayList((void*)*(u32*)(dp + 0x10), *(u32*)(dp + 0x14));
				dp += 8;
			}
		}
	}
}
#pragma opt_common_subs reset

#pragma opt_common_subs off // MUST_MATCH: keep GX_VA_TEX0+i from being CSE-hoisted
// GX setup order/constants and addressing match; the only remaining diff is the operand order of the
// first material's `add r7, wp, idx` (target puts wp first, MWCC puts the index first). `wp=mapGetWork()`,
// the `vcd` pointer local, the pos/nrm declaration order and the opt_common_subs pragma are all
// match-only improvements over the 95.93% baseline.
// NONMATCHING (99.91%): `add r7, r31, r0` (target) vs `add r7, r0, r31` (the single differing insn)
void mapSetPolygonVtxDesc(MapObject* obj, MapFileMesh* mesh) {
	MapWork* wp;
	void* pos;
	void* nrm;
	u32 mask;
	u32* vcd;
	s32 i;
	u8* material;
	wp = mapGetWork();
	vcd = (u32*)mesh->vcdTableOffset;
	pos = (void*)(vcd[0] + 4);
	nrm = (void*)(vcd[1] + 4);
	mask = mesh->elementMask;

	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_INDEX16);
	material = *(u8**)((u8*)wp->entries + (*(s16*)(obj->field_0xDC + 2) * 0x178 + 0xA4));
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
	// match-only: `a` is cast at each use rather than aliased into a `joint` local -- the alias pushes
	// a's live range past the buf-address computation and makes MWCC emit the prologue differently.
	char buf[256];
	const char* name = (const char*)b;
	void* result;

	strcpy(buf, c);
	strcat(buf, ((MapFileJoint*)a)->name);
	if (!strcmp(buf, name)) return a;

	if (((MapFileJoint*)a)->child) {
		strcat(buf, str_8041986C);
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

void mapObjFlagOn(const char* name, u32 flag) {
	MapObject* obj = mapGetMapObjSub(name);

	if (obj == NULL) {
		mapErrorEntry(0, name);
	}
	else if (!(flag & 0x100) || *(void**)((u8*)obj + 0x114)) {
		*(u32*)obj->field_0x0 |= flag;
	}
}

void mapObjFlagOff(const char* name, u32 flag) {
	MapObject* obj = mapGetMapObjSub(name);

	if (obj == NULL) {
		mapErrorEntry(0, name);
	}
	else {
		*(u32*)obj->field_0x0 &= ~flag;
	}
}

void mapFlgOn(MapObject* obj, u32 flag, s32 a3) {
	if (!(flag & 0x100) || *(void**)((u8*)obj + 0x114)) {
		*(u32*)obj->field_0x0 |= flag;
	}
	if (obj->child) mapFlgOn(obj->child, flag, 1);
	if (a3 && obj->next) mapFlgOn(obj->next, flag, 1);
}

void mapFlgOff(MapObject* obj, u32 flag, s32 a3) {
	*(u32*)obj->field_0x0 &= ~flag;
	if (obj->child) mapFlgOff(obj->child, flag, 1);
	if (a3 && obj->next) mapFlgOff(obj->next, flag, 1);
}

#pragma dont_inline on
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
#pragma dont_inline reset

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

void mapObjSetOffScreen(const char* name, const char* offName) {
	MapObject* obj = mapGetMapObjSub(name);

	*(s16*)obj->field_0xDC = offscreenNameToId(offName);
	*(u32*)obj->field_0x0 |= 0x80;
}

void mapGrpSetOffScreen(const char* name, const char* offName) {
	MapObject* obj = mapGetMapObjSub(name);
	s32 id = offscreenNameToId(offName);

	_setOffScrnId(obj, id, 0);
}

void mapObjClearOffScreen(const char* name) {
	MapObject* obj = mapGetMapObjSub(name);

	*(s16*)obj->field_0xDC = -1;
	*(u32*)obj->field_0x0 &= ~0x80;
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

// NONMATCHING (97.00%): register allocation of the inlined mapWork/activeGroup address computation
void mapSetFog(u32 color, f32 near, f32 far, u32* color2) {
	MapEntry* entry = &mapWork[activeGroup].entries[0];
	u32 c2;

	c2 = *color2;
	*(u32*)((u8*)entry + 0x28) = color;
	*(f32*)((u8*)entry + 0x2C) = near;
	*(f32*)((u8*)entry + 0x30) = far;
	*(u32*)((u8*)entry + 0x34) = c2;
}

void mapGetFog(u32* c, f32* near, f32* far, u32* c2) {
	MapEntry* entry = &mapWork[activeGroup].entries[0];

	*c = *(u32*)((u8*)entry + 0x28);
	*near = *(f32*)((u8*)entry + 0x2C);
	*far = *(f32*)((u8*)entry + 0x30);
	*c2 = *(u32*)((u8*)entry + 0x34);
}

void mapFogOn(void) {
	*(u16*)((u8*)mapWork + activeGroup * 0x2F4 + 0x4) |= 0x1;
}

void mapFogOff(void) {
	*(u16*)((u8*)mapWork + activeGroup * 0x2F4 + 0x4) &= ~0x1;
}

void mapSetBlend(GXColor* color) {
	MapEntry* entry = &mapWork[activeGroup].entries[0];

	*(GXColor*)((u8*)entry + 0x16) = *color;
	entry->flags |= 0x2;
}

void mapSetBlend2(GXColor* color) {
	MapEntry* entry = &mapWork[activeGroup].entries[0];

	*(GXColor*)((u8*)entry + 0x1A) = *color;
	entry->flags |= 0x4;
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

void mapGetBlend2(GXColor* out) {
	MapEntry* entry = &mapWork[activeGroup].entries[0];

	if (entry->flags & 0x4) {
		*out = *(GXColor*)((u8*)entry + 0x1A);
	}
	else {
		*out = dat_80419844;
	}
}

void mapBlendOff(void) {
	*(u16*)((u8*)mapWork + activeGroup * 0x2F4 + 0x4) &= ~0x2;
}

void mapBlendOff2(void) {
	*(u16*)((u8*)mapWork + activeGroup * 0x2F4 + 0x4) &= ~0x4;
}

void mapObjSetColor(const char* name, GXColor* color) {
	MapObject* obj = mapGetMapObjSub(name);

	if (obj) {
		*(u32*)((u8*)obj + 0xC) = *(u32*)color;
		*(u32*)obj->field_0x0 |= 0x40;
	}
}

// NONMATCHING (99.60%): the target keeps the first `*color` load in r3 and reuses it for the inlined child
// call (spilling it to 0x4c); ours loads it into r0 for the field write then reloads *color into r3 in the
// child block. A function-scope `c` removes the reload but makes MWCC spill it before the flag (90.01%).
void setColor(MapObject* obj, GXColor* color, s32 a3) {
	if (obj == 0) return;
	*(GXColor*)&obj->field_0xC = *color;
	*(u32*)obj->field_0x0 |= 0x40;
	if (obj->child) {
		GXColor c = *color;
		setColor(obj->child, &c, 1);
	}
	if (a3 && obj->next) {
		GXColor d = *color;
		setColor(obj->next, &d, 1);
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

void mapSetProjTexObj(GXTexObj* obj, void* imgRef) {
	projTexObj = *obj;
	projTexImgRef = imgRef;
}

void mapSetProjMtx(Mtx mtx) {
	PSMTXCopy(mtx, projMtx);
}

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

// NONMATCHING (99.31%): only a rotation of the backward-substitution loop pointers differs (target
// tangents=r6,dt=r7,mat=r8,temp=r9; ours tangents=r7,dt=r9,mat=r8,temp=r6). Pinned with `((Vec*)a)`
// in the dt/mat loop, `((f32*)b)` in the dt loop, `f64 three, half` + `f32 knprev` at fn scope.
void spline_maketable(s32 count, s32 a, s32* b, s32* c) {
	Vec* points = (Vec*)a;
	f32* knots = (f32*)b;
	Vec* tangents = (Vec*)c;
	const f64* cf = (const f64*)(vec3_802c16d8 + 0x2A8);
	f32* dt = (f32*)_mapAlloc(count * sizeof(f32));
	Vec* mat = (Vec*)_mapAlloc(count * sizeof(Vec));
	s32 i;
	f64 three, half;
	f32 knprev;

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
			half = cf[0];
			three = cf[1];
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
			dt[i] = float_0p0001_8041985c;
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

	tangents[count - 2].x = tangents[count - 2].x - dt[count - 2] * tangents[count - 1].x;
	tangents[count - 2].y = tangents[count - 2].y - dt[count - 2] * tangents[count - 1].y;
	tangents[count - 2].z = tangents[count - 2].z - dt[count - 2] * tangents[count - 1].z;

	{
		Vec* mp = &mat[count - 2];
		f32* dp = &dt[count - 2];
		Vec* tp = &tangents[count - 2];
		for (i = count - 2; i > 0; i--) {
			tp->x = (tp->x - *dp * tp[1].x) / mp->x;
			tp->y = (tp->y - *dp * tp[1].y) / mp->y;
			tp->z = (tp->z - *dp * tp[1].z) / mp->z;
			tp--;
			dp--;
			mp--;
		}
	}

	_mapFree(mat);
	_mapFree(dt);
}

#pragma opt_lifetimes off // MUST_MATCH: target FP live ranges need lifetime analysis off
// Arithmetic, control flow and the instruction sequence match; MWCC's FPR colouring differs
// (target keeps dt in f1, 2.0 in f6, 3.0 in f0; ours dt in f8, 2.0 in f0, 3.0 in f1) and the
// r9/r0 tangent-base offset. `#pragma opt_lifetimes off` is required (94.57% without).
// NONMATCHING (97.29%): FP colouring + tangent-base offset only
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
#pragma opt_lifetimes reset

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
		*(u32*)obj->field_0x0 |= 0x100000;
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

void mapGrpFlushOn(const char* name) {
	mapGrpFlagOn(name, 0x100000);
}

void mapGrpFlushOff(const char* name) {
	mapGrpFlagOff(name, 0x100000);
}

// NONMATCHING (98.89%): only the second inlined mapGetMapObjSub search's registers differ
// (target wp=r27/entry=r26/object=r28/i=r29/j=r30); the first search and the error body match.
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
