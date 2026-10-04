#include "driver/effdrv.h"
#include "driver/arcdrv.h"
#include "eff/eff_n64.h"
#include "manager/dvdmgr.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"
#include <string.h>
#include <stdio.h>

// Data objects defined later in this file or in other units, declared before their first use.
extern const f32 float_1000_8041a0a8;
extern const f32 float_0_8041a0ac;
extern const f32 dat_8041a0b0;
extern const f32 float_3_8041a0b4;
extern const f32 float_neg2_8041a0b8;
extern const f32 float_1_8041a0bc;
extern const f32 float_2_8041a0c0;


extern GlobalWork* gp;

//.data
EffectSet eff_set_table[] = {
	{0, "kemuri"},
	{1, "confetti"},
	{2, "fukidashi"},
	{3, "butterfly"},
	{4, "hit"},
	{5, "damage_star"},
	{6, "small_star"},
	{7, "mario_balloon"},
	{8, "mugi"},
	{9, "stardust"},
	{10, "recovery"},
	{11, "miss_star"},
	{12, "nice"},
	{13, "breath_fire"},
	{14, "torch"},
	{15, "confusion"},
	{16, "spark"},
	{17, "updown"},
	{18, "charge"},
	{19, "toge_flush"},
	{20, "ice"},
	{21, "fire"},
	{22, "bomb"},
	{23, "ripple"},
	{24, "coin_fukidashi"},
	{25, "star_point"},
	{26, "puni_balloon"},
	{27, "p_fukidashi"},
	{28, "kemuri_test"},
	{29, "starstone"},
	{30, "itemget"},
	{31, "status"},
	{32, "pointget"},
	{33, "levelup"},
	{34, "stageclear"},
	{35, "fall"},
	{36, "fpdamage"},
	{37, "break"},
	{38, "mobj_broken"},
	{39, "mahojin"},
	{40, "nozle"},
	{41, "mizutama"},
	{42, "minigame"},
	{43, "splash"},
	{44, "treasure_map"},
	{45, "kiss"},
	{46, "funemizu"},
	{47, "mist"},
	{48, "teresa"},
	{49, "batten"},
	{50, "sheep"},
	{51, "naniga"},
	{52, "sandars"},
	{53, "boomerang"},
	{54, "irekae"},
	{55, "rankup"},
	{56, "scanning"},
	{57, "toiki"},
	{58, "mist2"},
	{59, "particle"},
	{60, "ibuki"},
	{61, "mahorn"},
	{62, "spirit"},
	{63, "indirect"},
	{64, "syuryou"},
	{65, "uranoko"},
	{66, "hibashira"},
	{67, "number"},
	{68, "laser"},
	{69, "machingegun"},
	{70, "las_mon"},
	{71, "sleep"},
	{72, "energy"},
	{73, "biribirikinoko"},
	{74, "nokotarou"},
	{75, "queen"},
	{76, "queen2"},
	{77, "gonbaba_breath"},
	{78, "majinai"},
	{79, "mahorn2"},
	{80, "ultra_hammer"},
	{-1, NULL}
};

//.sbss
static EffectWork work;

//.sdata
static EffectWork* wp = &work;
const char* prefix_tbl[2] = {"jp", "us"};

//local prototypes
static void _callback_tpl(s32 error, DVDFileInfo* info);

static void _callback_tpl(s32 error, DVDFileInfo* info) {
	DVDEntry* entry = info->cb.userData;
	UnpackTexPalette(wp->texture);
	DVDMgrClose(entry);
	wp->texLoaded = TRUE;
}

void effInit(void) {
	wp->count = 0x100;
	wp->entries = __memAlloc(HEAP_DEFAULT, wp->count * sizeof(EffectEntry));
	memset(wp->entries, 0, wp->count * sizeof(EffectEntry));
	wp->handle = NULL;
	wp->language = -1;
	wp->texture = NULL;
	wp->texLoaded = FALSE;
	effInit64();
}

void effTexSetup(void) {
	DVDEntry* entry;
	char path[128];
	u32 size;

	wp->texture = NULL;
	wp->texLoaded = FALSE;
	wp->texture = arcOpen("effect.tpl", 0, 0);
	if (wp->texture == NULL) {
#ifdef MUST_MATCH
		// retail's pooled format string carries a second terminator (a 0x14-byte object)
		sprintf(path, "%s/e/jp/effect.tpl\0", getMarioStDvdRoot());
#else
		sprintf(path, "%s/e/jp/effect.tpl", getMarioStDvdRoot());
#endif
		entry = DVDMgrOpen(path, 2, 0);
		size = OSRoundUp32B(DVDMgrGetLength(entry));
		wp->texture = __memAlloc(HEAP_DEFAULT, size);
		entry->info.cb.userData = entry;
		DVDMgrReadAsync(entry, wp->texture, size, 0, _callback_tpl);
	}
	else {
		UnpackTexPalette(wp->texture);
		wp->texLoaded = TRUE;
	}
	effTexSetupN64();
}

// The original keeps these zero-initialised function-local dummies in .sdata, 32 bytes apart; without the
// pragma they land in .sbss. The original compares id as a signed value (cmpwi), not unsigned (cmplwi).
void effGetTexObj(u32 id, GXTexObj* obj) {
#ifdef __MWERKS__
#pragma explicit_zero_data on
	if ((s32)id < 146) { //number of images in effect.tpl
		if (!wp->texLoaded) {
			static char dummy ATTRIBUTE_ALIGN(32) = '\0';
			GXInitTexObj(obj, &dummy, 1, 1, GX_TF_I4, GX_CLAMP, GX_CLAMP, GX_FALSE);
		}
		else {
			TEXGetGXTexObjFromPalette(wp->texture, obj, id);
		}
	}
	else {
		static char dummy ATTRIBUTE_ALIGN(32) = '\0';
		if (!wp->handle) {
			GXInitTexObj(obj, &dummy, 1, 1, GX_TF_I4, GX_CLAMP, GX_CLAMP, GX_FALSE);
		}
		else {
			TEXGetGXTexObjFromPalette(*wp->handle->data, obj, id - 146);
		}
	}
#pragma explicit_zero_data reset
#endif
}

void effAutoRelease(BOOL inBattle) {
	int i;
	int count;
	EffectEntry* entry;

	count = wp->count;
	entry = wp->entries;

	for (i = 0; i < count; i++, entry++) {
		if (entry->flags & 1 && (s32)entry->inBattle == inBattle && entry && entry->flags) {
			__memFree(HEAP_EFFECT, entry->userdata);
			entry->flags = 0;
		}
	}
}

EffectEntry* effEntry(void) {
	EffectEntry* entry = wp->entries;
	int i;

	for (i = 0; i < wp->count; i++, entry++) {
		if (!entry->flags) {
			break;
		}
	}
	entry->flags = 1;
	entry->inBattle = gp->inBattle != 0;
	entry->type = 0;
	entry->count = 0;
	entry->userdata = NULL;
	entry->callback = NULL;
	entry->name[0] = '\0';
	return entry;
}

void effSetName(EffectEntry* effect, const char* name) {
	int i;
	int count;
	EffectEntry* entry;

	count = wp->count;
	entry = wp->entries;

	for (i = 0; i < count; i++, entry++) {
		if (entry->flags && !strcmp(entry->name, name)) {
			break;
		}
	}
	strcpy(effect->name, name);
}

void effMain(void) {
	const char *dvdroot, *prefix;
	int i;
	int count;
	EffectEntry* entry;

	count = wp->count;
	entry = wp->entries;

	for (i = 0; i < count; i++, entry++) {
		if (gp->inBattle) {
			if ((s32)entry->inBattle != 1) {
				continue;
			}
		}
		else if ((s32)entry->inBattle != 0) {
			continue;
		}
		if (entry->flags & 1) {
			if (entry->callback) {
				entry->callback(entry);
			}
		}
	}
	if ((u32)wp->language != (u32)gp->language) {
		if (wp->handle) {
			fileFree(wp->handle);
			wp->handle = NULL;
		}
		prefix = prefix_tbl[gp->language];
		dvdroot = getMarioStDvdRoot();
		if ((u32)fileAsyncf(4, 0, "%s/e/%s/effect_%s.tpl", dvdroot, prefix, prefix) != 0) {
			prefix = prefix_tbl[gp->language];
			wp->handle = fileAllocf(4, "%s/e/%s/effect_%s.tpl", getMarioStDvdRoot(), prefix, prefix);
			wp->language = gp->language;
		}
 	}
}

void effDelete(EffectEntry* effect) {
	if (effect && effect->flags) {
		__memFree(HEAP_EFFECT, effect->userdata);
		effect->flags = 0;
	}
}

void effSoftDelete(EffectEntry* effect) {
	if (effect && effect->flags) {
		if (effect->flags & 2) {
			effect->flags |= 4; // "deleted"
		}
		else {
			effDelete(effect); //inlined
		}
	}
}

EffectEntry* effNameToPtr(const char* name) {
	int i;
	int count;
	EffectEntry* entry;

	count = wp->count;
	entry = wp->entries;

	for (i = 0; i < count; i++, entry++) {
		if (entry->flags && !strcmp(entry->name, name)) {
			break;
		}
	}
	if (i >= count) { //no match found
		return NULL;
	}
	return entry;
}

EffectSet* effGetSet(const char* name) {
	int i = 0;

	while (eff_set_table[i].id != -1) { //run until end of table
		if (!strcmp(eff_set_table[i].name, name)) {
			return &eff_set_table[i];
		}
		i++;
	}
	return NULL;
}

typedef struct EffectMayaAnimData {
	u32 field_0x0; // 0x0
	f32 field_0x4; // 0x4
	s32* count;    // 0x8
} EffectMayaAnimData;

typedef struct EffectMayaAnim {
	u16 flags;                      // 0x0
	u8 field_0x2[0x2];              // 0x2
	f32 field_0x4;                  // 0x4
	OSTime startTime;               // 0x8
	EffectMayaAnimData* animData;   // 0x10
	EffectMayaAnimEntry entries[];  // 0x14
} EffectMayaAnim;

typedef struct EffectMayaVtx {
	s16 x; // 0x0
	s16 y; // 0x2
	s16 z; // 0x4
	s16 s; // 0x6
	s16 t; // 0x8
} EffectMayaVtx;

typedef struct EffectMayaPoly {
	s16 count;           // 0x0
	EffectMayaVtx vtx[]; // 0x2
} EffectMayaPoly;

typedef struct EffectMayaPolyList {
	u32 field_0x0;          // 0x0
	u32 field_0x4;          // 0x4
	s32 count;              // 0x8
	EffectMayaPoly** polys; // 0xC
} EffectMayaPolyList;

void* effMayaAnimAlloc(void* animData) {
	u32 size = *((EffectMayaAnimData*)animData)->count * 0x34 + 0x18;
	EffectMayaAnim* anim = __memAlloc(HEAP_EFFECT, size);

	memset(anim, 0, size);
	anim->animData = animData;
	return anim;
}

void effDeleteMayaAnim(void* anim) {
	__memFree(HEAP_EFFECT, anim);
}

void effPlayMayaAnim(void* animPtr) {
	EffectMayaAnim* anim = (EffectMayaAnim*)animPtr;

	anim->startTime = OSTicksToMilliseconds(gp->inBattle ? gp->renderTime : gp->renderFieldTime);
	anim->field_0x4 = float_0_8041a0ac;
	anim->flags |= 1;
}

void effCalcMayaAnim(void* animPtr) {
	EffectMayaAnim* anim = (EffectMayaAnim*)animPtr;
	s32 i;

	if (!(anim->flags & 1)) {
		return;
	}
	if (anim->flags & 2) {
		return;
	}

	anim->field_0x4 = (f32)(u64)(OSTicksToMilliseconds(gp->inBattle ? gp->renderTime : gp->renderFieldTime) - anim->startTime) * gp->framerate / float_1000_8041a0a8;

	if (anim->field_0x4 >= anim->animData->field_0x4) {
		anim->field_0x4 = anim->animData->field_0x4;
		anim->flags |= 2;
	}

	for (i = 0; i < anim->animData->count[0]; i++) {
		effCalcMayaAnimMatrix(anim, (u8*)anim->animData->count[1] + i * 0x10, anim->entries[i].mtx, &anim->entries[i].field_0x30);
	}
}

void effDrawMayaPoly(void* data_) {
	EffectMayaPolyList* data = (EffectMayaPolyList*)data_;
	s32 i;
	s32 j;

	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 9);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_S16, 9);
	GXSetCullMode(GX_CULL_NONE);

	for (i = 0; i < data->count; i++) {
		EffectMayaPoly* poly = data->polys[i];
		u16 count = poly->count;
		EffectMayaVtx* vtx = poly->vtx;

		GXBegin(GX_TRIANGLEFAN, GX_VTXFMT0, count);
		for (j = 0; j < count; j++) {
			GXPosition3s16(vtx[j].x, vtx[j].y, vtx[j].z);
			GXTexCoord2s16(vtx[j].s, vtx[j].t);
		}
	}
}

// ---- .sdata2 ----
// All the float words the unit reads by name, in original address order. They are defined
// after the code so the compiler emits them as objects instead of folding their values.
#pragma force_active on
#pragma explicit_zero_data on
__declspec(section ".sdata2") const f32 float_1000_8041a0a8 = 1000.0f;
__declspec(section ".sdata2") const f32 float_0_8041a0ac = 0.0f;
__declspec(section ".sdata2") const f32 dat_8041a0b0 = 0.00000001f;
__declspec(section ".sdata2") const f32 float_3_8041a0b4 = 3.0f;
__declspec(section ".sdata2") const f32 float_neg2_8041a0b8 = -2.0f;
__declspec(section ".sdata2") const f32 float_1_8041a0bc = 1.0f;
__declspec(section ".sdata2") const f32 float_2_8041a0c0 = 2.0f;
__declspec(section ".sdata2") u32 gap_09_8041A0C4_sdata2[1] = { 0 };
#pragma explicit_zero_data reset
#pragma force_active reset

// trailing alignment bytes of the unit's .sdata (the original keeps them after dummy$824)
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_80410261_sdata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset
