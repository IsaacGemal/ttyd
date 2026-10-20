#include "driver/lightdrv.h"
#include "driver/camdrv.h"
#include "driver/mapdrv.h"
#include "memory.h"
#include "system.h"
#include <string.h>
#include <stdio.h>


//.sbss
static LightWork work[2];

//.bss (declared in reverse target order: MWCC emits .bss in reverse declaration order)
LightEntry paperLight3D;
LightEntry paperLight3DImg;
LightEntry paperCraft;
LightEntry booLight;

//local prototypes
static s32 _sort(const void* a, const void* b);

#define lightGetWork() (mapGetActiveGroup() == 1 ? &work[1] : &work[0])

// The pooled string addresses land after every numeric literal in .rodata, so they move up once lightMain
// emits its four target vec3 literals. The .bss/.rodata offsets match only with the LightEntry globals
// declared in the reverse of the original's .bss order (MWCC emits .bss in reverse declaration order) and
// decay_tbl defined after lightMain (rodata named objects land at their declaration point).
void lightInit(void) {
	Mtx scale;
#ifdef MUST_MATCH
	LightWork* wp;
#endif

	work[0].count = LIGHT_ENTRY_COUNT;
	work[0].entries = __memAlloc(HEAP_DEFAULT, work[0].count * sizeof(LightEntry));

#ifdef MUST_MATCH
	// MUST_MATCH: without the named pointer MWCC rematerializes &work[1] after the second
	// __memAlloc instead of holding it in a callee-saved register (the target uses r29).
	wp = &work[1];
	wp->count = LIGHT_ENTRY_COUNT;
	wp->entries = __memAlloc(HEAP_DEFAULT, wp->count * sizeof(LightEntry));
#else
	work[1].count = LIGHT_ENTRY_COUNT;
	work[1].entries = __memAlloc(HEAP_DEFAULT, work[1].count * sizeof(LightEntry));
#endif

	paperLight3D.flags = LIGHT_NO_DIST_ATTEN | LIGHT_POINT;
	strcpy(paperLight3D.name, "PAPER");
	paperLight3D.position = (Vec){0.0f, 0.0f, 0.0f};
	paperLight3D.rotation = (Vec){0.0f, 0.0f, 0.0f};
	paperLight3D.color = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
	paperLight3D.spotAngle = 0.0f;
	paperLight3D.attenAngle = 1.0f;

	paperLight3DImg.flags = LIGHT_NO_DIST_ATTEN | LIGHT_POINT;
	strcpy(paperLight3DImg.name, "PAPERI");
	paperLight3DImg.position = (Vec){0.0f, 0.0f, 1048576.0f};
	paperLight3DImg.rotation = (Vec){0.0f, 0.0f, 0.0f};
	paperLight3DImg.color = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
	paperLight3DImg.spotAngle = 0.0f;
	paperLight3DImg.attenAngle = 1.0f;

	paperCraft.flags = LIGHT_NO_DIST_ATTEN | LIGHT_POINT;
	strcpy(paperCraft.name, "PAPERCRAFT");
	paperCraft.position = (Vec){-200.0f, 100.0f, 100.0f};
	paperCraft.rotation = (Vec){0.0f, 0.0f, 0.0f};
	paperCraft.color = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
	paperCraft.spotAngle = 0.0f;
	paperCraft.attenAngle = 1.0f;
	MTXScale(scale, 3.0f, 3.0f, 3.0f);
	MTXMultVec(scale, &paperCraft.position, &paperCraft.position);

	booLight.flags = LIGHT_NO_DIST_ATTEN | LIGHT_POINT;
	strcpy(booLight.name, "BooBoo");
	booLight.position = (Vec){0.0f, 0.0f, 0.0f};
	booLight.rotation = (Vec){0.0f, 0.0f, 0.0f};
	booLight.color = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
	booLight.spotAngle = 0.0f;
	booLight.attenAngle = 10.0f;
}

void lightReInit(void) {
	LightWork* wp = lightGetWork();
	memset(wp->entries, 0, sizeof(LightEntry) * wp->count);
}

// Match-only: returning a Vec by value makes MWCC emit the hidden return-slot copy the
// target has (build a temp, then store it to the named local). The zero vector is passed
// as the base so its literal pools at the call site, in the target's order.
static inline Vec lightMainVec(Vec v, f32 x, f32 y, f32 z);

void lightMain(void) {
	CameraEntry* camera;
	Vec dir = {0.0f, 0.0f, 1.0f};
	Vec v0, v1, v2;
	Vec position;

	camera = camGetPtr(CAMERA_3D);
	v0 = *(Vec*)&camera->view[0];
	v1 = *(Vec*)&camera->view[1];
	v2 = *(Vec*)&camera->view[2];
	position = lightMainVec((Vec){0.0f, 0.0f, 0.0f},
		(v0.x * dir.x) + (v1.x * dir.y) + (v2.x * dir.z),
		(v0.y * dir.x) + (v1.y * dir.y) + (v2.y * dir.z),
		(v0.z * dir.x) + (v1.z * dir.y) + (v2.z * dir.z));
	VECNormalize(&position, &position);
	paperLight3D.position = lightMainVec((Vec){0.0f, 0.0f, 0.0f}, 1048576.0f * position.x,
	                                      1048576.0f * position.y, 1048576.0f * position.z);
	booLight.position = (Vec){0.0f, 1000.0f, 0.0f};
}

static inline Vec lightMainVec(Vec v, f32 x, f32 y, f32 z) {
	v.x = x;
	v.y = y;
	v.z = z;
	return v;
}

//.rodata: declared after lightMain so the named object lands at its declaration point, i.e.
// after the vec literals lightInit/lightMain pool (the target has it at 0x90).
const Vec decay_tbl[] = {
	{1.0f, 0.0f, 0.0f},
	{0.0f, 0.1f, 0.0f},
	{0.0f, 0.0f, 0.01f}, 
	{0.0f, 0.0f, 0.01f}
};

// position/rotation are Vec* but color is a by-value GXColor: the pointer params force the second
// `*position` load for field_0x3C, while the by-value color is non-aliasing so its load hoists into the
// position-setup block (color=r0, pos.x/pos.y=r6/r5, decay offset=r7), exactly as the target schedules it.
s32 lightEntry(const char* name, s32 type, Vec* position, Vec* rotation, GXColor color, f32 spotAngle, f32 attenAngle, s32 attenType, s32 flags) {
	LightWork* wp = lightGetWork();
	s32 i;
	s32 k;
	LightEntry* entry;
	s32 j;
	char v41[256];

	entry = wp->entries;
	for (i = 0; i < wp->count; i++, entry++) {
		if (entry->flags & 1 && !strcmp(entry->name, name)) {
			break;
		}
	}
	//seems like it will break
	if (i < LIGHT_ENTRY_COUNT) {
		for (j = 0;; j++) {
			sprintf(v41, "%s_%04d", name, j);
			entry = wp->entries;
			name = v41; //will keep getting longer
			for (k = 0; k < wp->count; k++, entry++) {
				if (entry->flags & 1 && !strcmp(entry->name, name)) {
					break;
				}
			}
			if (k >= LIGHT_ENTRY_COUNT) {
				break;
			}
		}
	}

	entry = wp->entries;
	for (i = 0; i < wp->count; i++, entry++) {
		if (!(entry->flags & 1)) {
			break;
		}
	}
	memset(entry, 0, sizeof(LightEntry));
	strcpy(entry->name, name);
	entry->position = *position;
	entry->rotation = *rotation;
	entry->field_0x3C = *position;
	entry->color = color;
	entry->spotAngle = spotAngle;
	entry->attenAngle = attenAngle;
	entry->attenDistCoef.x = decay_tbl[attenType].x;
	entry->attenDistCoef.y = decay_tbl[attenType].y;
	entry->attenDistCoef.z = decay_tbl[attenType].z;
	entry->flags |= 1;
	switch (type) {
		case 0:
			entry->flags |= LIGHT_AMBIENT;
			break;
		case 1:
			entry->flags |= LIGHT_POINT;
			break;
		case 2:
			entry->flags |= LIGHT_SPOT;
			break;
		case 3:
			entry->flags |= LIGHT_DIRECTIONAL;
			break;
	}
	if (flags & 2) {
		entry->flags |= LIGHT_CHARA;
	}
	return i;
}

LightEntry* lightNameToPtr(const char* name) {
	LightWork* wp = lightGetWork();
	LightEntry* entry = wp->entries;
	s32 i;

	for (i = 0; i < wp->count; i++, entry++) {
		if (entry->flags & 1 && !strcmp(name, entry->name)) {
			return entry;
		}
	}
	return NULL;
}

typedef struct LightNearObj {
	LightEntry* entry;
	f32 distance;
} LightNearObj;

s32 _sort(const void* a, const void* b) {
#ifdef MUST_MATCH
	f32 value2, value1; // declaration order controls float-register allocation
#else
	f32 value1, value2;
#endif

	value1 = ((const LightNearObj*)a)->distance;
	value2 = ((const LightNearObj*)b)->distance;

	if (value1 > value2) {
		return 1;
	} else if (value1 < value2) {
		return -1;
	} else {
		return 0;
	}
}

s32 lightGetNearObj(Vec position, LightEntry** entries, s32 count, LightEntryFlags flags) {
	LightWork* wp = lightGetWork();
	LightNearObj near[32];
	LightEntry* p;
	u32 num;
	s32 i;
	s32 n;

	if (lightGetEntryNum() > 8) {
		p = wp->entries;
		num = 0;
		for (i = 0; i < wp->count; i++, p++) {
			if (p->flags & LIGHT_ENABLED) {
				near[num].entry = p;
				near[num].distance = PSVECDistance(&position, &p->position);
				num++;
			}
		}
		qqsort(near, num, sizeof(LightNearObj), (int (*)(void*, void*))_sort);
	} else {
		p = wp->entries;
		num = 0;
		for (i = 0; i < wp->count; i++, p++) {
			if (p->flags & LIGHT_ENABLED) {
				near[num].entry = p;
				num++;
			}
		}
	}

	n = 0;
	for (i = 0; i < num; i++) {
		p = near[i].entry;
		if (p->flags & LIGHT_AMBIENT) {
			continue;
		}
		if (flags != 0 && !(flags & p->flags)) {
			continue;
		}
		entries[n] = p;
		n++;
		if (n >= count) {
			break;
		}
	}
	for (i = n; i < count; i++) {
		entries[i] = NULL;
	}
	return n;
}

LightEntry* lightGetAmbient(void) {
	LightWork* wp = lightGetWork();
	LightEntry* entry = wp->entries;
	s32 i;

	for (i = 0; i < wp->count; i++, entry++) {
		if ((entry->flags & LIGHT_ENABLED) && !(entry->flags & LIGHT_CHARA) && (entry->flags & LIGHT_AMBIENT)) {
			return entry;
		}
	}
	return NULL;
}

LightEntry* lightGetCharaAmbient(void) {
	LightWork* wp = lightGetWork();
	LightEntry* entry = wp->entries;
	s32 i;

	for (i = 0; i < wp->count; i++, entry++) {
		if ((entry->flags & LIGHT_ENABLED) && (entry->flags & LIGHT_CHARA) && (entry->flags & LIGHT_AMBIENT)) {
			return entry;
		}
	}
	return NULL;
}

s32 lightCheckCharaLight(void) {
	LightWork* wp = lightGetWork();
	LightEntry* entry = wp->entries;
	s32 i;

	for (i = 0; i < wp->count; i++, entry++) {
		if ((entry->flags & LIGHT_ENABLED) && (entry->flags & LIGHT_CHARA)) {
			return 1;
		}
	}
	return 0;
}

LightEntry* lightGetPaper(void) {
	s32 cam = camGetCurNo();

	if (cam == 4) {
		return &paperLight3D;
	}
	if (cam == 6) {
		return &paperLight3DImg;
	}
	return &paperLight3DImg;
}

LightEntry* lightGetPaperCraft(void) {
	return &paperCraft;
}

s32 lightGetEntryNum(void) {
	LightWork* wp = lightGetWork();
	LightEntry* entry = wp->entries;
	s32 i;
	s32 count = 0;

	for (i = 0; i < wp->count; i++, entry++) {
		if (entry->flags & LIGHT_ENABLED) {
			count++;
		}
	}
	return count;
}

LightEntry* lightNumberToPtr(s32 number) {
	LightWork* wp = lightGetWork();
	LightEntry* entry = &wp->entries[number];

	if (entry->flags & LIGHT_ENABLED) {
		return entry;
	}
	return NULL;
}
