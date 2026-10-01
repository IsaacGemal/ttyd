#include "evt/evt_img.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr.h"
#include "manager/evtmgr_cmd.h"
#include "driver/imgdrv.h"
#include "driver/animdrv.h"
#include "driver/offscreendrv.h"
#include "mario/mariost.h"
#include "sdk/DEMOInit.h"
#include <dolphin/types.h>
#include <dolphin/gx.h>
#include <dolphin/mtx.h>

// .sbss2: the default .sbss2 is only used for compiler-generated pool
// templates, so pair it with a small-data section and place it by hand.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") GXColor unk_80422fd0;

extern const Vec vec3_802c47f8;
extern const f32 float_1_8041a270;

// Inlined into their callers; `inline` keeps the compiler from emitting a
// standalone copy (the original object has no such symbols).
static inline GXColor makeColor(u8 r, u8 g, u8 b, u8 a) {
	GXColor color = unk_80422fd0;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	return color;
}

static inline Vec makeVec(f32 x, f32 y, f32 z) {
	Vec point = vec3_802c47f8;
	point.x = x;
	point.y = y;
	point.z = z;
	return point;
}

USER_FUNC(evt_img_entry) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);

	imgEntry((const char*)name, gp->inBattle != 0);
	return EVT_RETURN_DONE;
}
USER_FUNC(evt_img_set_position) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);
	f32 x = evtGetFloat(event, args[1]);
	f32 y = evtGetFloat(event, args[2]);
	ImageEntry* entry = imgNameToPtr((const char*)name, gp->inBattle != 0);

	entry->field_0xE8 = x;
	entry->field_0xEC = y;
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_img_set_paper) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);
	s32 anim = evtGetValue(event, args[1]);
	s32 type = gp->inBattle != 0;
	ImageEntry* entry = imgNameToPtr((const char*)name, type);

	entry->poseId = animPaperPoseGetId((const char*)anim, type);
	*(OSTime*)((u8*)entry + 0x110) = animTimeGetTime(type);
	animPoseSetMaterialFlagOn(entry->poseId, 0x1800);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_img_set_paper_anim) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);
	s32 anim = evtGetValue(event, args[1]);
	s32 type = gp->inBattle != 0;
	ImageEntry* entry = imgNameToPtr((const char*)name, type);

	entry->animName = (const char*)anim;
	*(OSTime*)((u8*)entry + 0x110) = animTimeGetTime(type);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_img_alloc_capture) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);
	s32 shadowIndex = evtGetValue(event, args[1]);
	const char* offscreen = (const char*)evtGetValue(event, args[2]);
	s32 offscreenId;
	s32 flag = evtGetValue(event, args[3]);
	s32 left = evtGetValue(event, args[4]);
	s32 top = evtGetValue(event, args[5]);
	s32 width = evtGetValue(event, args[6]);
	s32 height = evtGetValue(event, args[7]);
	ImageEntry* entry = imgNameToPtr((const char*)name, gp->inBattle != 0);
	ImageShadow* shadow = &entry->shadows[shadowIndex];
	s32 i;

	if (offscreen != NULL) {
		offscreenId = offscreenNameToId(offscreen);
	}
	else {
		offscreenId = -1;
	}

	if (shadow->flags & 4) {
		return EVT_RETURN_DONE;
	}

	left -= left & 1;
	top -= top & 1;
	width += width & 1;
	height += height & 1;

	for (i = 0; i < 3; i++) {
		if (i != shadowIndex && (entry->shadows[i].flags & 4)) {
			if (entry->field_0xE4 != (u16)width || entry->field_0xE6 != (u16)height) {
				return EVT_RETURN_DONE;
			}
		}
	}

	if (left == 0 && top == 0 && width == 0 && height == 0 && offscreenId != -1) {
		entry->field_0xE4 = 0;
		entry->field_0xE6 = 0;
		shadow->field_0x4 = 0;
		shadow->field_0x6 = 0;
		if (shadowIndex == 2) {
			shadow->field_0x14 = 0x27;
			shadow->field_0x18 = 1;
		}
		else {
			shadow->field_0x14 = 6;
			shadow->field_0x18 = 6;
		}
		entry->flags |= 4;
	}
	else {
		if (left < 0 || top < 0 || width <= 0 || height <= 0
			|| left + width > DEMOGetRenderModeObj()->fbWidth
			|| top + height > DEMOGetRenderModeObj()->efbHeight) {
			return EVT_RETURN_DONE;
		}
		entry->field_0xE4 = width;
		entry->field_0xE6 = height;
		shadow->field_0x4 = left;
		shadow->field_0x6 = top;
		if (shadowIndex == 2) {
			shadow->field_0x14 = 0x27;
			shadow->field_0x18 = 1;
		}
		else {
			shadow->field_0x14 = 6;
			shadow->field_0x18 = 6;
		}
	}

	shadow->field_0x10 = offscreenId;
	shadow->flags = 0x14;
	shadow->field_0x40 = flag;
	if (flag != 0) {
		shadow->flags |= 0x100;
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_img_free_capture) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);
	s32 shadowId = evtGetValue(event, args[1]);
	ImageEntry* entry = imgNameToPtr((const char*)name, gp->inBattle != 0);

	imgFreeCapture(entry, shadowId);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_img_onoff) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);
	s32 on = evtGetValue(event, args[1]);
	ImageEntry* entry = imgNameToPtr((const char*)name, gp->inBattle != 0);

	if (on) {
		entry->flags &= ~2;
	}
	else {
		entry->flags |= 2;
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_img_set_virtual_point) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);
	f32 x = evtGetFloat(event, args[1]);
	f32 y = evtGetFloat(event, args[2]);
	f32 z = evtGetFloat(event, args[3]);
	ImageEntry* entry = imgNameToPtr((const char*)name, gp->inBattle != 0);
	Vec point = makeVec(x, y, z);

	imgSetVirtualPoint(entry, &point);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_img_clear_virtual_point) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);
	ImageEntry* entry = imgNameToPtr((const char*)name, gp->inBattle != 0);

	imgClearVirtualPoint(entry);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_img_release) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);
	ImageEntry* entry = imgNameToPtr((const char*)name, gp->inBattle != 0);

	imgRelease(entry, gp->inBattle != 0);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_img_wait_animend) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);
	ImageEntry* entry = imgNameToPtr((const char*)name, gp->inBattle != 0);

	if (isFirstCall) {
		return EVT_RETURN_BLOCK;
	}
	if (entry->field_0x118 < float_1_8041a270) {
		return EVT_RETURN_BLOCK;
	}
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_img_set_shadow) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);
	s32 shadowId = evtGetValue(event, args[1]);
	ImageEntry* entry = imgNameToPtr((const char*)name, gp->inBattle != 0);

	imgSetShadow(entry, shadowId);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_img_set_paper_timerate) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);
	f32 rate = evtGetFloat(event, args[1]);
	ImageEntry* entry = imgNameToPtr((const char*)name, gp->inBattle != 0);

	animPoseSetLocalTimeRate(entry->poseId, rate);
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_img_set_color) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);
	u8 r = evtGetValue(event, args[1]);
	u8 g = evtGetValue(event, args[2]);
	u8 b = evtGetValue(event, args[3]);
	u8 a = evtGetValue(event, args[4]);
	ImageEntry* entry = imgNameToPtr((const char*)name, gp->inBattle != 0);

	animPoseSetMaterialFlagOn(entry->poseId, 0x40);
	animPoseSetMaterialEvtColor(entry->poseId, makeColor(r, g, b, a));
	return EVT_RETURN_DONE;
}

USER_FUNC(N_evt_img_set_z) {
	s32* args = event->args;
	s32 name = evtGetValue(event, args[0]);
	f32 z = evtGetFloat(event, args[1]);

	imgNameToPtr((const char*)name, gp->inBattle != 0)->order = z;
	return EVT_RETURN_DONE;
}


// .rodata
const Vec vec3_802c47f8 = {0.0f, 0.0f, 0.0f};

// .sdata2
const f32 float_1_8041a270 = 1.0f;
