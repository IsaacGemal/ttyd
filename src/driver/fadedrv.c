#include "driver/fadedrv.h"
#include "driver/animdrv.h"
#include "driver/arcdrv.h"
#include "driver/camdrv.h"
#include "driver/imgdrv.h"
#include "driver/dispdrv.h"
#include "driver/offscreendrv.h"
#include "evt/evt_img.h"
#include "manager/evtmgr.h"
#include "pmario_sound.h"
#include "manager/dvdmgr.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"
#include <dolphin/dvd.h>
#include "math_ext.h"
#include <string.h>
#include <stdio.h>

static f32 z(void);  // local to this file in the original (fadedrv.s: .fn z, local)


u8 i4tex = 0xFF;
#ifdef MUST_MATCH
// Preserve the original 32-byte texture storage without aligning a separate byte array to four.
#pragma explicit_zero_data on // Keep the texture padding in initialized .sdata.
#pragma force_active on // The original padding is retained despite having no references.
static u8 i4tex_pad_byte = 0;
static u16 i4tex_pad_half = 0;
static u32 i4tex_pad_words[7] = {0};
#pragma force_active reset
#pragma explicit_zero_data reset
#endif

//.data
static s32 depth_image[32] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

#ifdef MUST_MATCH
#pragma force_active on // Retain the original .sbss padding after wp (reverse declaration order).
static u32 gap_08_80418404_sbss;
#pragma force_active reset
#endif
static FadeWork* wp;

#ifdef MUST_MATCH
#pragma section ".sdata" ".sbss2" // Reproduce the original read-only zero storage.
#pragma force_active on // Retain the gap after the compiler-generated zero GXColor template.
static u32 gap_10_80422F7C_sbss2;
#pragma force_active reset
#pragma section ".sdata" ".sbss" // Restore the default uninitialized small-data section.
#endif

//local prototypes
static void zFill(void);
static void _callback(s32 error, DVDFileInfo* info) {
	void* userData = info->cb.userData;

	UnpackTexPalette(wp->texture);
	DVDMgrClose(userData);
	wp->loaded = TRUE;
}
void fadeInit(void) {
	int i;

	wp = __memAlloc(HEAP_DEFAULT, sizeof(FadeWork));
	memset(wp, 0, sizeof(FadeWork));

	for (i = 0; i < 5; i++) {
		FadeEntry* entry = &wp->entries[i];

		entry->pose = -1;
		entry->paperPose = -1;

		entry->fadeSpotX = 0.0f;
		entry->fadeSpotY = 0.0f;

		entry->animOffsetX = 0.0f;
		entry->animOffsetY = 0.0f;

		entry->scale = 1.0f;

		entry->delayFrames = 0;
	}

	wp->activeEntry = -1;
	wp->texture = NULL;
	wp->loaded = FALSE;
}
void fadeTexSetup(void) {
	DVDEntry* dvd;
	u32 size;
	char path[128];

	wp->texture = NULL;
	wp->texture = arcOpen("fade.tpl", NULL, NULL);
	if (wp->texture == NULL) {
		sprintf(path, "%s/fade.tpl", getMarioStDvdRoot());
		dvd = DVDMgrOpen(path, 2, 0);
		size = OSRoundUp32B(DVDMgrGetLength(dvd));
		wp->texture = __memAlloc(HEAP_DEFAULT, size);
		dvd->info.cb.userData = dvd;
		DVDMgrReadAsync(dvd, wp->texture, size, 0, _callback);
	}
	else {
		UnpackTexPalette(wp->texture);
		wp->loaded = TRUE;
	}
}

//TODO: probably lots of switch statements
// Frees the resources held by an entry and re-initialises it.
static inline void fadeEntryReset(FadeEntry* entry) {
	if (entry->pose >= 0) {
		animPoseRelease(entry->pose);
	}
	if (entry->paperPose >= 0) {
		animPaperPoseRelease(entry->paperPose);
	}
	if (entry->image) {
		imgRelease(entry->image, entry->imageType);
	}
	memset(entry, 0, sizeof(FadeEntry));
	entry->pose = -1;
	entry->paperPose = -1;
	entry->fadeSpotX = 0.0f;
	entry->fadeSpotY = 0.0f;
	entry->animOffsetX = 0.0f;
	entry->animOffsetY = 0.0f;
	entry->scale = 1.0f;
}

void fadeEntry(s32 type, s32 duration, GXColor color) {
	FadeEntry* entry;
	s32 index;

	if (!type) {
		return;
	}
	if (type == 16) { //type 16 is type 9 but white
		type = FADE_IN_BLACK;
		color = (GXColor){0xF0, 0xF0, 0xF0, 0xFF};
	}
	if (type == FADE_OUT_WHITE) {
		type = FADE_OUT_BLACK;
		color = (GXColor){0xF0, 0xF0, 0xF0, 0xFF};
	}
	if (type == FADE_IN_WHITE2) {
		type = 11;
		color = (GXColor){0xF0, 0xF0, 0xF0, 0xFF};
	}
	if (type == 19) {
		type = 12;
		color = (GXColor){0xF0, 0xF0, 0xF0, 0xFF};
	}
	if (type == 20) {
		type = 14;
		color = (GXColor){0xF0, 0xF0, 0xF0, 0xFF};
	}
	if (type == 21) {
		type = 15;
		color = (GXColor){0xF0, 0xF0, 0xF0, 0xFF};
	}
	if ((type == 1 || type == 2) && duration < 1000) {
		duration = 1000;
	}
	if (type == 7) {
		type = 3;
	}
	else if (type == 8) {
		type = 4;
	}
	else if (((u32)(type - 3) <= 2 || type == 6) && duration < 1000) {
		duration = 1000;
	}

	switch (type) {
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			entry = &wp->entries[0];
			index = 0;
			fadeEntryReset(&wp->entries[2]);
			entry->delayFrames = 0;
			switch (type) {
				case 1:
				case 3:
				case 5:
				case 9:
				case 11:
				case 14:
					if (duration != 0) {
						entry->delayFrames = 2;
					}
					break;
			}
			break;
		case 22:
		case 23:
		case 24:
		case 25:
		case 26:
		case 27:
		case 28:
		case 29:
		case 30:
		case 31:
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
		case 38:
		case 39:
		case 40:
		case 51:
			entry = &wp->entries[1];
			index = 1;
			break;
		case 41:
		case 42:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
		case 48:
			entry = &wp->entries[2];
			index = 2;
			fadeEntryReset(&wp->entries[0]);
			break;
		case 49:
		case 50:
		case 52:
		case 53:
		case 54:
		case 55:
		case 56:
		case 57:
		case 58:
		case 59:
		case 60:
		case 61:
		case 62:
			entry = &wp->entries[3];
			index = 3;
			switch (type) {
				case 50:
				case 53:
				case 54:
				case 56:
				case 58:
					fadeEntryReset(&wp->entries[0]);
					break;
			}
			break;
		case 63:
		case 64:
			entry = &wp->entries[4];
			index = 4;
			break;
		default:
			break;
	}

	entry->flags = 0;
	entry->flags |= 1;
	entry->type = type;
	entry->duration = duration;
	entry->color = color;
	entry->alpha = 0;
	entry->angle = 0;
	entry->elapsed = 0;
	entry->step = 0;
	entry->animOffsetX = 0.0f;
	entry->animOffsetY = 0.0f;
	entry->scale = 1.0f;
	if (type != 50 && type != 53) {
		entry->callback = NULL;
	}
	wp->activeEntry = index;
	if (type == 34 || type == 40) {
		entry->scale = 3.0f;
	}

	switch (type) {
		case 43:
			entry->groupName = "OFF_d_dokan_down";
			entry->animName = "A_1";
			entry->imageType = 2;
			break;
		case 44:
			entry->groupName = "OFF_d_dokan_down";
			entry->animName = "A_2";
			entry->imageType = 2;
			break;
		case 41:
			entry->groupName = "OFF_d_dokan_up";
			entry->animName = "A_1";
			entry->imageType = 2;
			break;
		case 42:
			entry->groupName = "OFF_d_dokan_up";
			entry->animName = "A_2";
			entry->imageType = 2;
			break;
		case 45:
			entry->groupName = "OFF_d_dokan_left";
			entry->animName = "A_1";
			entry->imageType = 2;
			break;
		case 46:
			entry->groupName = "OFF_d_dokan_left";
			entry->animName = "A_2";
			entry->imageType = 2;
			break;
		case 47:
			entry->groupName = "OFF_d_dokan_right";
			entry->animName = "A_1";
			entry->imageType = 2;
			break;
		case 48:
			entry->groupName = "OFF_d_dokan_right";
			entry->animName = "A_2";
			entry->imageType = 2;
			break;
		case 22:
			entry->groupName = "OFF_d_maku_up_down";
			entry->animName = "A_1";
			entry->imageType = 2;
			break;
		case 23:
			entry->groupName = "OFF_d_maku_up_down";
			entry->animName = "A_2";
			entry->imageType = 2;
			break;
		case 24:
		case 27:
		case 63:
			entry->groupName = "OFF_d_maku_left_right";
			entry->animName = "A_1";
			entry->imageType = 2;
			break;
		case 26:
			entry->groupName = "OFF_d_maku_left_right";
			entry->animName = "Z_1";
			entry->imageType = 2;
			break;
		case 25:
		case 28:
		case 64:
			entry->groupName = "OFF_d_maku_left_right";
			entry->animName = "A_2";
			entry->imageType = 2;
			break;
		case 29:
		case 35:
			entry->groupName = "OFF_d_maku_center";
			entry->animName = "A_1";
			entry->imageType = 2;
			break;
		case 30:
		case 36:
			entry->groupName = "OFF_d_maku_center";
			entry->animName = "A_2";
			entry->imageType = 2;
			break;
		case 31:
		case 34:
		case 37:
		case 40:
			entry->groupName = "OFF_d_maku_center";
			entry->animName = "Z_2";
			entry->imageType = 2;
			break;
		case 32:
		case 38:
			entry->groupName = "OFF_d_maku_center";
			entry->animName = "Z_1";
			entry->imageType = 2;
			break;
		case 49:
		case 52:
			entry->groupName = "OFF_d_meku";
			entry->animName = "Z_1";
			entry->imageType = 2;
			break;
		case 50:
		case 53:
			entry->groupName = "OFF_d_meku";
			entry->animName = "A_1";
			entry->imageType = 2;
			break;
		case 51:
			entry->groupName = "OFF_d_four_meku_5sec";
			entry->animName = "A_1";
			entry->imageType = 2;
			break;
		case 54:
			entry->groupName = "OFF_f_in_out_mario";
			entry->animName = "A_3";
			entry->imageType = 2;
			break;
		case 55:
			entry->groupName = "OFF_f_in_out_mario";
			entry->animName = "A_1";
			entry->imageType = 2;
			break;
		case 56:
			entry->groupName = "OFF_f_in_out_peach";
			entry->animName = "A_3";
			entry->imageType = 2;
			break;
		case 57:
			entry->groupName = "OFF_f_in_out_peach";
			entry->animName = "A_1";
			entry->imageType = 2;
			break;
		case 58:
			entry->groupName = "OFF_f_in_out_kuppa";
			entry->animName = "A_3";
			entry->imageType = 2;
			break;
		case 59:
			entry->groupName = "OFF_f_in_out_kuppa";
			entry->animName = "A_1";
			entry->imageType = 2;
			break;
		case 60:
			entry->groupName = "OFF_f_in_out_mario";
			entry->animName = "A_2";
			entry->imageType = 2;
			break;
		case 61:
			entry->groupName = "OFF_f_in_out_peach";
			entry->animName = "A_2";
			entry->imageType = 2;
			break;
		case 62:
			entry->groupName = "OFF_f_in_out_kuppa";
			entry->animName = "A_2";
			entry->imageType = 2;
			break;
		default:
			break;
	}
}
void fadeTecOn(void) {
	wp->flags |= 0x1000;
}
void fadeTecOff(void) {
	wp->flags &= ~0x1000;
}
void fadeTecSoftOn(void) {
	wp->flags |= 0x1010;
	wp->softAmount = 0.0f;
}
void fadeTecSoftOff(void) {
	wp->flags |= 0x1020;
	wp->softAmount = 1.0f;
}
void fadeSoftFocusOn(void) {
	wp->flags |= 0x2000;
}
void fadeSoftFocusOff(void) {
	wp->flags &= ~0x2000;
}
void fadeWhiteOutOn(void) {
	wp->flags |= 0x4000;
}
void fadeWhiteOutOff(void) {
	wp->flags &= ~0x4000;
}
void fadeSetSpotPos(f32 x, f32 y) {
	FadeEntry* entry = &wp->entries[0];

	entry->fadeSpotX = x;
	entry->fadeSpotY = y;
}
void fadeSetAnimOfsPos(f32 x, f32 y) {
	FadeEntry* entry = &wp->entries[1];

	entry->animOffsetX = x;
	entry->animOffsetY = y;
}
void fadeSetAnimVirtualPos(f32 x, f32 y, f32 z) {
	FadeEntry* entry = &wp->entries[1];
	entry->VirtualPos = (Vec){x, y, z};
	entry->flags |= 4;
}
void fadeSetOffscreenCallback(OffscreenCallback callback, void* param) {
	FadeEntry* entry = &wp->entries[3];

	entry->callback = callback;
	entry->offparam = param;
}

void fadeMain(void) {
	FadeEntry* entry;
	s32 i;

	for (i = 0, entry = wp->entries; i < 5; i++, entry++) {
		if (!(entry->flags & 1)) {
			continue;
		}

		switch (entry->type) {
			case 1:
			case 3:
			case 5:
			case 9:
			case 11:
			case 14:
				switch (entry->step) {
					case 0:
						if (entry->duration != 0) {
							if (entry->delayFrames != 0) {
								entry->delayFrames--;
							}
							else {
								entry->elapsed += 16.666666f;
							}
							entry->alpha = entry->elapsed / (f32)entry->duration;
							entry->alpha = 1.0f - entry->alpha;
							entry->angle += 10.0f;
							if (entry->alpha < 0.0f) {
								entry->alpha = 0.0f;
								entry->flags |= 2;
								entry->step++;
							}
						}
						else {
							entry->alpha = 0.0f;
							entry->flags |= 2;
							entry->step++;
						}
						break;
				}
				break;
			case 2:
			case 4:
			case 6:
			case 10:
			case 12:
			case 13:
			case 15:
				switch (entry->step) {
					case 0:
						if (entry->duration != 0) {
							entry->elapsed += 16.666666f;
							entry->alpha = entry->elapsed / (f32)entry->duration;
							entry->angle -= 10.0f;
							if (entry->alpha > 1.0f) {
								entry->alpha = 1.0f;
								entry->flags |= 2;
								entry->step++;
							}
						}
						else {
							entry->alpha = 1.0f;
							entry->flags |= 2;
							entry->step++;
						}
						break;
				}
				break;
			case 41:
			case 42:
			case 43:
			case 44:
			case 45:
			case 46:
			case 47:
			case 48:
				switch (entry->step) {
					case 0: {
						s32 args[10];
						EventEntry event;

						if (!animGroupBaseAsync(entry->groupName, entry->imageType, NULL)) {
							if (entry->paperPose < 0 && (entry->type == 42 || entry->type == 44 ||
							                              entry->type == 46 || entry->type == 48)) {
								wp->flags |= 0x8000;
							}
							break;
						}
						wp->flags &= ~0x8000;
						if (entry->paperPose >= 0) {
							imgRelease(entry->image, entry->imageType);
							animPaperPoseRelease(entry->paperPose);
							entry->paperPose = -1;
							entry->image = NULL;
						}
						entry->paperPose = animPaperPoseEntry(entry->groupName, entry->imageType);
						imgEntry("fImg_dokan", entry->imageType);
						entry->image = imgNameToPtr("fImg_dokan", entry->imageType);
						entry->image->poseId = animPaperPoseGetId(entry->groupName, entry->imageType);
						entry->image->startTime = animTimeGetTime(entry->imageType);
						entry->image->animName = entry->animName;
						imgClearVirtualPoint(entry->image);
						event.args = args;
						args[0] = (s32)"fImg_dokan";
						args[1] = 0;
						args[2] = 0;
						args[3] = 0;
						args[4] = 0;
						args[5] = 0;
						args[6] = 608;
						args[7] = 480;
						evt_img_alloc_capture(&event, TRUE);
						switch (entry->type) {
							case 45:
								entry->sfxHandle = psndSFXOn((const char*)0x403AB);
								break;
							case 47:
								entry->sfxHandle = psndSFXOn((const char*)0x403AC);
								break;
							case 41:
								entry->sfxHandle = psndSFXOn((const char*)0x403AD);
								break;
							case 43:
								entry->sfxHandle = psndSFXOn((const char*)0x403AE);
								break;
							case 46:
								entry->sfxHandle = psndSFXOn((const char*)0x403AF);
								break;
							case 48:
								entry->sfxHandle = psndSFXOn((const char*)0x403B0);
								break;
							case 42:
								entry->sfxHandle = psndSFXOn((const char*)0x403B1);
								break;
							case 44:
								entry->sfxHandle = psndSFXOn((const char*)0x403B2);
								break;
						}
						entry->step++;
						break;
					}
					case 1:
						if (entry->image->loopTimes >= 1.0f) {
							entry->flags |= 2;
							switch (entry->type) {
								case 42:
								case 44:
								case 46:
								case 48:
									imgRelease(entry->image, entry->imageType);
									animPaperPoseRelease(entry->paperPose);
									entry->paperPose = -1;
									entry->image = NULL;
									break;
							}
							entry->step++;
						}
						break;
				}
				break;
			case 49:
			case 52:
			case 55:
			case 57:
			case 59:
			case 60:
			case 61:
			case 62:
				switch (entry->step) {
					case 0:
						if (!animGroupBaseAsync(entry->groupName, entry->imageType, NULL)) {
							break;
						}
						if (entry->paperPose >= 0) {
							imgRelease(entry->image, entry->imageType);
							animPaperPoseRelease(entry->paperPose);
							entry->paperPose = -1;
							entry->image = NULL;
						}
						entry->paperPose = animPaperPoseEntry(entry->groupName, entry->imageType);
						animPoseSetMaterialFlagOn(entry->paperPose, 0x1800);
						imgEntry("fImg_peron", entry->imageType);
						entry->image = imgNameToPtr("fImg_peron", entry->imageType);
						entry->image->poseId = animPaperPoseGetId(entry->groupName, entry->imageType);
						entry->image->startTime = animTimeGetTime(entry->imageType);
						entry->image->animName = entry->animName;
						imgClearVirtualPoint(entry->image);
						switch (entry->type) {
							case 52: {
								s32 args[10];
								EventEntry event;

								offscreenEntry("fOff_peron");
								event.args = args;
								args[0] = (s32)"fImg_peron";
								args[1] = 0;
								args[2] = (s32)"fOff_peron";
								args[3] = 0;
								args[4] = 0;
								args[5] = 0;
								args[6] = 608;
								args[7] = 480;
								evt_img_alloc_capture(&event, TRUE);
								break;
							}
							case 55:
							case 57:
							case 59: {
								s32 args[10];
								EventEntry event;

								event.args = args;
								args[0] = (s32)"fImg_peron";
								args[1] = 0;
								args[2] = 0;
								args[3] = 2;
								args[4] = 0;
								args[5] = 0;
								args[6] = 608;
								args[7] = 480;
								evt_img_alloc_capture(&event, TRUE);
								entry->image->camId = CAMERA_FADE2;
								entry->image->captureCamId = 10;
								psndSFXOn((const char*)0x403B4);
								break;
							}
							case 60:
							case 61:
							case 62: {
								s32 args[10];
								EventEntry event;

								event.args = args;
								args[0] = (s32)"fImg_peron";
								args[1] = 0;
								args[2] = 0;
								args[3] = 2;
								args[4] = 0;
								args[5] = 0;
								args[6] = 608;
								args[7] = 480;
								evt_img_alloc_capture(&event, TRUE);
								entry->image->camId = CAMERA_FADE;
								entry->image->captureCamId = 9;
								break;
							}
							default: {
								s32 args[10];
								EventEntry event;

								event.args = args;
								args[0] = (s32)"fImg_peron";
								args[1] = 0;
								args[2] = 0;
								args[3] = 2;
								args[4] = 0;
								args[5] = 0;
								args[6] = 608;
								args[7] = 480;
								evt_img_alloc_capture(&event, TRUE);
								break;
							}
						}
						entry->step++;
						break;
					case 1:
						if (entry->image->loopTimes >= 1.0f) {
							switch (entry->type) {
								case 55:
								case 57:
								case 59:
								case 60:
								case 61:
								case 62:
									entry->image->startTime = animTimeGetTime(entry->imageType);
									entry->image->animName = "A_2";
									break;
								default:
									entry->flags |= 2;
									break;
							}
							entry->step++;
						}
						break;
					case 2:
						if (entry->image->loopTimes >= 1.0f) {
							entry->flags |= 2;
							entry->step++;
						}
						break;
				}
				break;
			case 50:
			case 53:
			case 54:
			case 56:
			case 58:
				switch (entry->step) {
					case 0:
						entry->image = imgNameToPtr("fImg_peron", entry->imageType);
						entry->image->poseId = animPaperPoseGetId(entry->groupName, entry->imageType);
						entry->image->startTime = animTimeGetTime(entry->imageType);
						entry->image->animName = entry->animName;
						imgClearVirtualPoint(entry->image);
						switch (entry->type) {
							case 54:
							case 56:
							case 58:
								psndSFXOn((const char*)0x403B5);
								break;
							case 50:
							case 53:
								psndSFXOn((const char*)0x40365);
								break;
						}
						entry->step++;
						break;
					case 1:
						if (entry->image->loopTimes >= 1.0f) {
							entry->flags |= 2;
							imgRelease(entry->image, entry->imageType);
							animPaperPoseRelease(entry->paperPose);
							entry->paperPose = -1;
							entry->image = NULL;
							if (entry->type == 53) {
								offscreenDelete("fOff_peron");
							}
							entry->step++;
						}
						break;
				}
				break;
			case 22:
				switch (entry->step) {
					case 0:
						if (!animGroupBaseAsync(entry->groupName, entry->imageType, NULL)) {
							break;
						}
						if (entry->pose >= 0) {
							animPoseRelease(entry->pose);
						}
						entry->pose = animPoseEntry(entry->groupName, entry->imageType);
						animPoseSetMaterialFlagOn(entry->pose, 0x1800);
						animPoseSetAnim(entry->pose, entry->animName, TRUE);
						entry->poseColor = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
						animPoseSetMaterialFlagOff(entry->pose, 0x40);
						entry->step++;
					case 1:
						animPoseMain(entry->pose);
						if (animPoseGetLoopTimes(entry->pose) >= 1.0f) {
							entry->flags |= 2;
							entry->step++;
						}
						break;
					case 2:
						animPoseMain(entry->pose);
						break;
				}
				break;
			case 23:
				switch (entry->step) {
					case 0:
						if (!animGroupBaseAsync(entry->groupName, entry->imageType, NULL)) {
							break;
						}
						if (entry->pose >= 0) {
							animPoseRelease(entry->pose);
						}
						entry->pose = animPoseEntry(entry->groupName, entry->imageType);
						animPoseSetMaterialFlagOn(entry->pose, 0x1800);
						animPoseSetAnim(entry->pose, entry->animName, TRUE);
						entry->poseColor = (GXColor){0xFF, 0xFF, 0xFF, 0xFF};
						animPoseSetMaterialFlagOff(entry->pose, 0x40);
						entry->step++;
					case 1:
						if (animPoseGetLoopTimes(entry->pose) >= 0.5f) {
							entry->poseColor.r *= 0.98f;
							entry->poseColor.g *= 0.98f;
							entry->poseColor.b *= 0.98f;
							animPoseSetMaterialEvtColor(entry->pose, entry->poseColor);
							animPoseSetMaterialFlagOn(entry->pose, 0x40);
						}
						animPoseMain(entry->pose);
						if (animPoseGetLoopTimes(entry->pose) >= 1.0f) {
							entry->flags |= 2;
							entry->step++;
							animPoseRelease(entry->pose);
							entry->pose = -1;
						}
						break;
				}
				break;
			case 24:
			case 25:
			case 26:
			case 27:
			case 28:
			case 29:
			case 30:
			case 31:
			case 32:
			case 34:
			case 35:
			case 36:
			case 37:
			case 38:
			case 40:
			case 51:
			case 63:
			case 64:
				switch (entry->step) {
					case 0:
						if (!animGroupBaseAsync(entry->groupName, entry->imageType, NULL)) {
							break;
						}
						switch (entry->type) {
							case 24:
							case 27:
							case 63:
								entry->sfxHandle = psndSFXOn((const char*)0x40325);
								break;
							case 25:
							case 28:
							case 64:
								entry->sfxHandle = psndSFXOn((const char*)0x40326);
								break;
							case 29:
							case 35:
								entry->sfxHandle = psndSFXOn((const char*)0x40321);
								break;
							case 30:
							case 36:
								entry->sfxHandle = psndSFXOn((const char*)0x40322);
								break;
						}
						if (entry->pose >= 0) {
							animPoseRelease(entry->pose);
						}
						entry->pose = animPoseEntry(entry->groupName, entry->imageType);
						animPoseSetMaterialFlagOn(entry->pose, 0x1800);
						animPoseSetAnim(entry->pose, entry->animName, TRUE);
						entry->step++;
						if (entry->type == 34 || entry->type == 40) {
							entry->scale = 3.0f;
							entry->frameCount = 0;
						}
					case 1:
						animPoseMain(entry->pose);
						if (entry->type == 34 || entry->type == 40) {
							if (!(entry->flags & 8)) {
								if (entry->frameCount < 180) {
									entry->scale = intplGetValue(12, ++entry->frameCount, 180, 3.0f, 1.0f);
								}
							}
							else {
								if (entry->frameCount < 90) {
									entry->scale = intplGetValue(12, ++entry->frameCount, 90, 1.5f, 1.0f);
								}
							}
						}
						if (animPoseGetLoopTimes(entry->pose) >= 1.0f) {
							if (entry->type == 34 || entry->type == 40) {
								if (!(entry->flags & 8)) {
									if (entry->frameCount == 180) {
										entry->flags |= 2;
										entry->step++;
									}
								}
								else {
									if (entry->frameCount == 90) {
										entry->flags |= 2;
										entry->step++;
									}
								}
							}
							else {
								entry->flags |= 2;
								entry->step++;
								psndSFXOff(entry->sfxHandle);
								if (entry->type == 25 || entry->type == 28 || entry->type == 64) {
									animPoseRelease(entry->pose);
									entry->pose = -1;
								}
							}
						}
						break;
					case 2:
						if (entry->pose >= 0) {
							animPoseMain(entry->pose);
						}
						break;
				}
				break;
			case 33:
			case 39:
				switch (entry->step) {
					case 0:
						entry->scale = 1.0f;
						entry->frameCount = 0;
						entry->step++;
					case 1:
						animPoseMain(entry->pose);
						if (entry->frameCount < 120) {
							entry->scale = intplGetValue(12, ++entry->frameCount, 120, 1.0f, 3.0f);
						}
						else {
							entry->flags |= 2;
							entry->step++;
							animPoseRelease(entry->pose);
							entry->pose = -1;
						}
						break;
				}
				break;
		}

		switch (entry->type) {
			case 22:
			case 23:
			case 35:
			case 36:
			case 37:
			case 38:
			case 39:
			case 40:
				dispEntry(CAMERA_FADE, 0.0f, 0, disp_maku, entry);
				break;
			case 27:
			case 28:
			case 63:
			case 64:
				dispEntry(CAMERA_FADE2, 0.0f, 0, disp_maku, entry);
				break;
			case 24:
			case 25:
			case 26:
			case 29:
			case 30:
			case 31:
			case 32:
			case 33:
			case 34:
			case 51:
				dispEntry(CAMERA_3D_IMAGE, 0.0f, 0, disp_maku, entry);
				break;
			case 49:
			case 50:
			case 52:
			case 53:
			case 54:
			case 55:
			case 56:
			case 57:
			case 58:
			case 59:
			case 60:
			case 61:
			case 62:
				if (entry->paperPose >= 0 && entry->callback != NULL) {
					dispEntry(CAMERA_OFFSCREEN2, 0.0f, 0, entry->callback, entry->offparam);
				}
				break;
			case 11:
			case 12:
			case 13:
				dispEntry(CAMERA_2D, 1000.0f, 0, disp_texture, wp);
				break;
			case 5:
			case 6:
			case 14:
			case 15:
				dispEntry(CAMERA_3D, 0.0f, 7, disp_texture, wp);
				break;
			case 41:
			case 42:
			case 43:
			case 44:
			case 45:
			case 46:
			case 47:
			case 48:
				break;
			default:
				dispEntry(CAMERA_FADE, 0.0f, 0, disp_texture, wp);
				break;
		}
	}

	if (wp->flags & 0x3000) {
		dispEntry(CAMERA_2D, 999.0f, 0, disp_tec, wp);
	}
	if (wp->flags & 0x8000) {
		dispEntry(CAMERA_FADE, -100.0f, 0, disp_dummy_black, wp);
	}
	if (wp->flags & 0x4000) {
		dispEntry(CAMERA_3D, 1000.0f, 7, disp_dummy_black, wp);
	}
}

void disp_texture(CameraId camId, void* param) {
	CameraEntry* camera;
	FadeEntry* entry;
	GXTexObj texObj;
	f32 projv[7];
	Mtx mtx1;
	Mtx mtx2;
	Mtx mtx3;
	Mtx mtx4;
	f32 f4;
	f32 f31;

	camera = camGetPtr(CAMERA_2D);
	entry = &wp->entries[0];

	switch (entry->type) {
		case 1:
		case 3:
		case 5:
		case 9:
		case 11:
		case 14:
			if (entry->flags & 2) {
				return;
			}
			break;
	}

	GXGetProjectionv(projv);
	GXSetProjection(camera->projection, camera->projType);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_OR);
	GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
	GXSetNumChans(0);
	GXSetNumTevStages(1);
	GXSetNumTexGens(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
	GXSetCullMode(GX_CULL_BACK);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);

	switch (entry->type) {
		case 1:
		case 2:
			GXLoadPosMtxImm(camera->view, GX_PNMTX0);
			GXSetCurrentMtx(GX_PNMTX0);
			TEXGetGXTexObjFromPalette(wp->texture, &texObj, 0);
			GXLoadTexObj(&texObj, GX_TEXMAP0);
			if (!(entry->flags & 2)) {
				f4 = entry->alpha;
				f31 = 100.0f * f4 * f4 * f4 * f4 * f4;
				PSMTXTrans(mtx1, 0.5f, 0.5f, 0.0f);
				PSMTXScale(mtx3, f31, f31, 1.0f);
				PSMTXTrans(mtx2, -0.5f, -0.5f, 0.0f);
				PSMTXConcat(mtx1, mtx3, mtx3);
				PSMTXConcat(mtx3, mtx2, entry->texMtx);
			}
			else if (entry->type == 2) {
				PSMTXTrans(entry->texMtx, 1.0f, 1.0f, 0.0f);
			}
			break;
		case 3:
		case 4:
		case 5:
		case 6:
			GXLoadPosMtxImm(camera->view, GX_PNMTX0);
			GXSetCurrentMtx(GX_PNMTX0);
			TEXGetGXTexObjFromPalette(wp->texture, &texObj, 1);
			GXLoadTexObj(&texObj, GX_TEXMAP0);
			if (!(entry->flags & 2)) {
				f4 = entry->alpha;
				f31 = 100.0f * f4 * f4 * f4 * f4 * f4;
				PSMTXTrans(mtx1, 0.5f, 0.5f, 0.0f);
				PSMTXScale(mtx3, f31, f31, 1.0f);
				PSMTXRotRad(mtx4, 'z', 0.017453292f * entry->angle);
				PSMTXTrans(mtx2, -0.5f - entry->fadeSpotX / (f32)gp->fbWidth,
				           -0.5f + entry->fadeSpotY / (f32)gp->efbHeight, 0.0f);
				PSMTXConcat(mtx1, mtx3, mtx3);
				PSMTXConcat(mtx3, mtx4, mtx3);
				PSMTXConcat(mtx3, mtx2, entry->texMtx);
			}
			else if (entry->type == 4 || entry->type == 6) {
				PSMTXTrans(entry->texMtx, 1.0f, 1.0f, 0.0f);
			}
			break;
		case 7:
		case 8:
			break;
		case 9:
		case 10:
		case 11:
		case 12:
		case 14:
		case 15:
			GXLoadPosMtxImm(camera->view, GX_PNMTX0);
			GXSetCurrentMtx(GX_PNMTX0);
			GXInitTexObj(&texObj, &i4tex, 1, 1, GX_TF_I4, GX_CLAMP, GX_CLAMP, GX_FALSE);
			GXLoadTexObj(&texObj, GX_TEXMAP0);
			PSMTXIdentity(entry->texMtx);
			entry->color.a = (u8)(255.0f * entry->alpha);
			break;
		case 13:
			GXLoadPosMtxImm(camera->view, GX_PNMTX0);
			GXSetCurrentMtx(GX_PNMTX0);
			GXInitTexObj(&texObj, &i4tex, 1, 1, GX_TF_I4, GX_CLAMP, GX_CLAMP, GX_FALSE);
			GXLoadTexObj(&texObj, GX_TEXMAP0);
			PSMTXIdentity(entry->texMtx);
			break;
	}

	GXLoadTexMtxImm(entry->texMtx, GX_TEXMTX0, GX_MTX2x4);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);
	GXSetTevColor(GX_TEVREG0, entry->color);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	f4 = 304.0f;
	GXPosition3f32(-f4, f4, 0.0f);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3f32(f4, f4, 0.0f);
	GXTexCoord2f32(1.0f, 0.0f);
	GXPosition3f32(f4, -f4, 0.0f);
	GXTexCoord2f32(1.0f, 1.0f);
	GXPosition3f32(-f4, -f4, 0.0f);
	GXTexCoord2f32(0.0f, 1.0f);
	GXSetProjectionv(projv);
}

static f32 z(void) {
	FadeEntry* entry = &wp->entries[1];
	CameraEntry* cam;
	Vec pos;
	f32 one = 1.0f;
	f32 denom;
	f32 zpos;

	if (camGetPtr(CAMERA_3D)->projType == 0) {
		return one;
	}

	cam = camGetPtr(CAMERA_FADE);
	PSMTXMultVec(cam->view, &entry->VirtualPos, &pos);
	zpos = pos.z;
	denom = (cam->viewport[5] - cam->viewport[4])
	      * ((one / -zpos) * (cam->projection[2][2] * zpos + cam->projection[2][3]))
	      + cam->viewport[5];

	cam = camGetPtr(CAMERA_3D);
	PSMTXMultVec(cam->view, &entry->VirtualPos, &pos);
	return ((cam->viewport[5] - cam->viewport[4])
	      * (one * (cam->projection[2][2] * pos.z + cam->projection[2][3]))
	      + cam->viewport[5]) / denom;
}
static void zFill(void) {
	CameraEntry* camera;
	f32 y0, y1, x0, x1;
	f32 zpos;
	f32 nf, half;
	GXTexObj obj;
	Mtx mtx;

	camera = camGetCurPtr();
	nf = camera->near + camera->far;
	half = 0.5f;
	y0 = nf * half;
	y0 = y0 * (f32)tan((half * ((6.2831855f * camera->fovY) / 360.0f)));
	x1 = (y0 * camera->aspect);
	y1 = -y0;
	x0 = -x1;

	GXInitTexObj(&obj, depth_image, 4, 4, GX_TF_Z8, GX_REPEAT, GX_REPEAT, GX_FALSE);
	GXLoadTexObj(&obj, GX_TEXMAP0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);
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
	PSMTXIdentity(mtx);
	GXLoadPosMtxImm(camera->view, 0);
	GXSetCurrentMtx(0);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	zpos = -10000.0f;
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



void disp_maku(CameraId camId, void* param) {
	FadeEntry* entry = param;
	CameraEntry* camera;
	Mtx mtx1;
	Mtx mtx2;
	Vec pos;
	Vec normal;
	f32 f30;
	f32 f31;
	f32 tx;
	f32 ty;
#ifdef MUST_MATCH
	// See the note on the `+ zero` below.
	f32 zero = 0.0f;
#endif

	camGetPtr(camId);

	switch (entry->type) {
		case 23:
		case 25:
		case 28:
		case 64:
			if (entry->flags & 2) {
				return;
			}
			break;
	}

	switch (entry->type) {
		case 22:
		case 23:
		case 24:
		case 25:
		case 26:
		case 27:
		case 28:
		case 29:
		case 30:
		case 31:
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
		case 38:
		case 39:
		case 40:
		case 51:
		case 63:
		case 64:
			break;
		default:
			return;
	}

	if (entry->pose < 0) {
		return;
	}
	if (entry->flags & 4) {
		f32 mag;

		camera = camGetPtr(CAMERA_3D);
		f30 = camera->fovY;
		f30 = f30 / 2.0f / 180.0f;
		f30 = 3.141592f * f30;
		f30 = 24.0f / (f32)tan(f30);
		camera = camGetPtr(CAMERA_3D);
		PSVECSubtract(&camera->cameraPos, &entry->VirtualPos, &pos);
		camera = camGetPtr(CAMERA_3D);
		normal = *(Vec*)&camera->view[2];
		PSVECNormalize(&normal, &normal);
		pos.x *= normal.x;
		pos.y *= normal.y;
		pos.z *= normal.z;
		mag = PSVECMag(&pos);
		f31 = mag / f30;
		f30 = f30 - mag;
		animPoseSetGXFunc(entry->pose, 0, 0);
		GXSetZScaleOffset(z(), 0.0f);
	}
	else {
		f31 = entry->scale;
		f30 = 0.0f;
		zFill();
	}

#ifdef MUST_MATCH
	tx = entry->animOffsetX * f31 + zero;
	ty = entry->animOffsetY * f31 + zero;
#else
	tx = entry->animOffsetX * f31;
	ty = entry->animOffsetY * f31;
#endif

	PSMTXTrans(mtx1, 64.0f * (tx / (f32)gp->fbWidth),
	           48.0f * (ty / (f32)gp->efbHeight), f30);
	PSMTXTrans(mtx2, -32.0f, 24.0f, 0.0f);
	PSMTXConcat(mtx1, mtx2, mtx1);
	PSMTXScale(mtx2, 608.0f / (f32)gp->fbWidth, 480.0f / (f32)gp->efbHeight, 608.0f / (f32)gp->fbWidth);
	PSMTXConcat(mtx1, mtx2, mtx1);
	PSMTXTrans(mtx2, 32.0f, -24.0f, 0.0f);
	PSMTXConcat(mtx1, mtx2, mtx1);
	animPoseDrawMtx(entry->pose, mtx1, 1, 0.0f, f31);
	animPoseDrawMtx(entry->pose, mtx1, 2, 0.0f, f31);
	animPoseDrawMtx(entry->pose, mtx1, 3, 0.0f, f31);
	sysWaitDrawSync();
}

#ifdef MUST_MATCH
// Emit pi immediately before disp_maku's other literals; the unused body is stripped at link time.
static f32 fadedrvPoolPi(void) {
	return 3.141592f;
}
#endif

// Building the colour in an inline that returns it keeps the by-value argument at sp+0xC and the colour at
// sp+0x8, as in the original; assigning the fields of the local directly swaps those slots.
static inline GXColor makeFadeTevColor(u8 a) {
	GXColor c = (GXColor){0, 0, 0, 0};
	c.a = a;
	return c;
}

void disp_tec(CameraId camId, void* param) {
	CameraEntry* camera;
	GXTexObj texObj;
	GXColor color;

	camera = camGetPtr(camId);
	GXSetZCompLoc(GX_TRUE);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_OR);
	GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
	GXSetNumChans(0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY, GX_FALSE, GX_PTIDENTITY);

	if (wp->flags & 0x1000) {
		TEXGetGXTexObjFromPalette(wp->texture, &texObj, 2);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
	}
	else {
		TEXGetGXTexObjFromPalette(wp->texture, &texObj, 3);
		GXLoadTexObj(&texObj, GX_TEXMAP0);
	}
	GXSetNumTevStages(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
	GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_A0, GX_CA_ZERO, GX_CA_TEXA, GX_CA_ZERO);

	if (wp->flags & 0x1000) {
		GXSetTevColor(GX_TEVREG0, (GXColor){0x19, 0x23, 0x2a, 0xff});
	}
	else {
		GXSetTevColor(GX_TEVREG0, (GXColor){0xe6, 0xe6, 0xe6, 0xff});
	}

	if (wp->flags & 0x30) {
		if (wp->flags & 0x10) {
			wp->softAmount += 0.04f;
			if (wp->softAmount > 1.0f) {
				wp->softAmount = 1.0f;
				wp->flags &= ~0x10;
			}
		}
		else {
			wp->softAmount -= 0.04f;
			if (wp->softAmount < 0.0f) {
				wp->softAmount = 0.0f;
				wp->flags &= ~0x20;
				wp->flags &= ~0x3000;
			}
		}
		color = makeFadeTevColor((u8)(255.0f * wp->softAmount));
		GXSetTevColor(GX_TEVREG1, color);
		GXSetNumTevStages(2);
		GXSetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR_NULL);
		GXSetTevColorOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevAlphaOp(GX_TEVSTAGE1, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
		GXSetTevColorIn(GX_TEVSTAGE1, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_CPREV);
		GXSetTevAlphaIn(GX_TEVSTAGE1, GX_CA_ZERO, GX_CA_APREV, GX_CA_A1, GX_CA_ZERO);
	}

	GXSetCullMode(GX_CULL_BACK);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
	GXLoadPosMtxImm(camera->view, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3s16(-308, 244, 0);
	GXTexCoord2f32(0.0f, 0.0f);
	GXPosition3s16(308, 244, 0);
	GXTexCoord2f32(2.0f, 0.0f);
	GXPosition3s16(308, -244, 0);
	GXTexCoord2f32(2.0f, 2.0f);
	GXPosition3s16(-308, -244, 0);
	GXTexCoord2f32(0.0f, 2.0f);
}
void disp_dummy_black(CameraId camId, void* param) {
	CameraEntry* cam;
	f32 projv[7];

	cam = camGetPtr(CAMERA_FADE);
	GXGetProjectionv(projv);
	GXSetProjection(cam->projection, cam->projType);
	GXSetZCompLoc(GX_ZC_LINEAR);
	GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
	GXSetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_OR);
	GXSetZMode(GX_FALSE, GX_ALWAYS, GX_FALSE);
	GXSetNumChans(1);
	GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, 0, GX_DF_NONE, GX_AF_NONE);
	if (wp->flags & 0x8000) {
		GXSetChanMatColor(GX_COLOR0A0, (GXColor){0, 0, 0, 255});
	}
	else {
		GXSetChanMatColor(GX_COLOR0A0, (GXColor){255, 255, 255, 255});
	}
	GXSetNumTevStages(1);
	GXSetNumTexGens(1);
	GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
	GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
	GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
	GXLoadPosMtxImm(cam->view, GX_PNMTX0);
	GXSetCurrentMtx(GX_PNMTX0);
	GXSetCullMode(GX_CULL_BACK);
	GXClearVtxDesc();
	GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
	GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
	GXBegin(GX_QUADS, GX_VTXFMT0, 4);
	GXPosition3s16(-32, 24, 0);
	GXPosition3s16(32, 24, 0);
	GXPosition3s16(32, -24, 0);
	GXPosition3s16(-32, -24, 0);
	GXSetProjectionv(projv);
}
BOOL fadeIsFinish(void) {
	FadeEntry* entry;

	if (wp->activeEntry == -1) {
		return TRUE;
	}

	entry = &wp->entries[wp->activeEntry];
	return (entry->flags >> 1) & 1;
}
void fadeReset(s32 id) { //TODO: better param name?
	FadeEntry* entry;

	entry = &wp->entries[id];
	if (entry->pose >= 0) {
		animPoseRelease(entry->pose);
	}
	if (entry->paperPose >= 0) {
		animPaperPoseRelease(entry->paperPose);
	}
	if (entry->image) {
		imgRelease(entry->image, entry->imageType);
	}
	memset(entry, 0, sizeof(FadeEntry));
	entry->pose = -1;
	entry->paperPose = -1;
	entry->fadeSpotX = 0.0f;
	entry->fadeSpotY = 0.0f;
	entry->animOffsetX = 0.0f;
	entry->animOffsetY = 0.0f;
	entry->scale = 1.0f;
}
TEXPalette* fadeGetTpl(void) {
	return wp->texture;
}
void fadeSetNarrowFast(void) {
	wp->entries[1].flags |= 8;
}

#ifdef MUST_MATCH
// Generating this unused conversion helper flushes the deferred functions before force_active is enabled.
// This keeps the pool helpers' bodies out of the linked text and places the data tail after the pools.
#pragma defer_codegen off // Generate the flushing helper immediately; its conversion double is already pooled.
static f32 fadedrvFlushPools(u32 value) {
	return (f32)value;
}
#pragma explicit_zero_data on // This zero word belongs to an initialized section.
#pragma force_active on // Retain the original section tail.
__declspec(section ".data") static u32 fadedrv_data_pad = 0;
#pragma force_active reset
#pragma explicit_zero_data reset
#pragma defer_codegen reset // Restore the default function emission mode.
#endif
