#include "mario/mario_pouch.h"

#include "driver/itemdrv.h"
#include "driver/icondrv.h"
#include "driver/swdrv.h"
#include "evt/evt_cmd.h"
#include "manager/evtmgr_cmd.h"
#include "manager/winmgr.h"
#include "manager/fontmgr.h"
#include "driver/msgdrv.h"
#include "driver/seqdrv.h"
#include "party/party_vivian.h"
#include "data/item_data.h"
#include "mario/mario.h"
#include "mario/mariost.h"
#include "motion/mot_jabara.h"
#include "driver/hitdrv.h"
#include "math_ext.h"
#include "system.h"
#include "memory.h"
#include <string.h>
#include "driver/npcdrv.h"
#include "party/party_nokonoko.h"
#include <stdio.h>

extern GlobalWork* gp;

extern int __float_nan[];
extern const f32 float_0_8041af9c;
extern const f32 float_1_8041afa4;
extern const f32 float_10_8041afb0;
extern const f32 float_15_8041afb4;
extern const f32 float_0p5_8041afb8;
extern const f32 float_180_8041afbc;
extern const f32 float_100_8041afc0;
#define NAN (*(f32*)__float_nan)

#define FP_NAN 1
#define FP_INFINITE 2
#define FP_ZERO 3
#define FP_NORMAL 4
#define FP_SUBNORMAL 5

static inline s32 __fpclassifyf(f32 x) {
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

#define fpclassify(x) __fpclassifyf(x)
#define isnan(x) (fpclassify(x) == FP_NAN)

static inline f32 sqrtf(f32 x) {
    const f64 _half = .5;
    const f64 _three = 3.0;

    if (x > float_0_8041af9c) {
        f64 guess = __frsqrte(x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        return x * guess;
    } else if (x < 0.0) {
        return NAN;
    } else if (isnan(x)) {
        return NAN;
    }
    return x;
}


extern ItemData itemDataTable[];
extern const char str_802CADE8[];
extern const char str_802CAE00[];
extern const char str_802CAE18[];
extern const char str_802CAE30[];
extern const char str_802CAE48[];
extern u32 dat_8041af90;
extern u32 dat_8041af94;
extern u32 dat_8041af98;
const Vec vec3_802cabe0 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cabec = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cabf8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cac04 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cac10 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802cac1c = {0.0f, 0.0f, 0.0f};
const f64 double_20_802cac28 = 20.0;

//.bss
static ItemWork work[2];

//TODO: inline function? I see nothing in the symbol map
#define itemGetWork() (gp->inBattle ? &work[1] : &work[0])

inline void itemCheckBadge(ItemEntry* entry) {
    if (!gp->inBattle) {
        if (BADGE_MIN <= entry->itemId && entry->itemId < BADGE_MAX) {
            if (entry->flags & 0x100) {
                pouchArriveBadge(entry->itemId);
            }
        }
    }
}

void itemInit(void) { // 1:1
    ItemWork* wp;

    wp = &work[0];
    wp->count = 32;
    wp->entries = __memAlloc(HEAP_DEFAULT, sizeof(ItemEntry) * wp->count);
    memset(wp->entries, 0, sizeof(ItemEntry) * wp->count);
    wp->currId = 0;

    wp = &work[1];
    wp->count = 32;
    wp->entries = __memAlloc(HEAP_DEFAULT, sizeof(ItemEntry) * wp->count);
    memset(wp->entries, 0, sizeof(ItemEntry) * wp->count);
    wp->currId = 0;
}

void itemReInit(void) {
    ItemWork* wp = itemGetWork();
    ItemEntry* entry;
    int i;

    if (!gp->inBattle) {
        for (entry = wp->entries, i = 0; i < wp->count; i++, entry++) {
            if (entry->flags & 1) { //flush all current badges
                itemCheckBadge(entry);
            }
        }
    }

    //clear all existing entries
    for (entry = wp->entries, i = 0; i < wp->count; i++, entry++) {
        iconDelete(entry->name);
        entry->flags &= ~1;
        memset(entry->name, 0, sizeof(entry->name));
    }
}






// NONMATCHING (14.78%): 9 instructions differ, 434 missing, 8 extra, plus register differences.
ItemEntry* itemEntry(const char* name, ItemType type, u16 a3, s32 evtId, s32 a5, f32 a6, f32 a7, f32 a8) {
	ItemWork* wp = itemGetWork();
	ItemEntry* entry;
	s32 evtVar;
	int i;
	char itemName[64];


	evtVar = evtId;
	if (evtId != -1) {
		if (evtId >= GSWF(0) && evtId <= EVTDAT_GSWF_MAX) {
			evtVar = evtId + EVTDAT_GSWF_BASE;
			if (swGet(evtId + EVTDAT_GSWF_BASE)) {
				return NULL;
			}
		}
		else {
			if (evtId >= LSWF(0) && evtId <= EVTDAT_LSWF_MAX) {
				evtVar = evtId + EVTDAT_LSWF_BASE;
				if (_swGet(evtId + EVTDAT_LSWF_BASE)) {
					return NULL;
				}
			}
		}
	}

	if (name == NULL) {
		if (++wp->currId >= 0xFFFFF) { //fix overflow
			wp->currId = 1;
		}
		sprintf(itemName, "_i%05x", wp->currId);
		name = itemName;
	}

	entry = wp->entries;
	for (i = 0; i < wp->count; i++, entry++) {
		if (entry->flags & 1 && !strcmp(entry->name, name)) {
			break;
		}
	}

	entry = wp->entries;
	if (wp->count > 0) { //TODO: redundant check?
		for (i = 0; i < wp->count; i++, entry++) {
			if (!(entry->flags & 1)) {
				break;
			}
		}
	}
}


// NONMATCHING (0.24%): empty placeholder, not decompiled yet.
void itemMain(void) {

}

// item status lives at 0x38 (u32, not yet declared in the header)
#define ITEM_STATUS(item) (*(u32*)((u8*)(item) + 0x38))

void itemPickUp(ItemEntry* item) {
    ITEM_STATUS(item) |= 0x1000;
    item->mode = 8;
    item->field_0x26 = 0;
    *(s32*)((u8*)item + 0x7c) = 0;
    *(s32*)((u8*)item + 0x78) = 0;
    ITEM_STATUS(item) &= ~0x100;
}

s32 itemStatus(ItemEntry* item, u32 mask) {
    return ITEM_STATUS(item) & mask;
}

void itemStatusOn(ItemEntry* item, u32 mask) {
    ITEM_STATUS(item) |= mask;
}

u16 itemFlag(ItemEntry* item, u16 flag) {
    return item->flags & flag;
}

void itemFlagOff(ItemEntry* item, u16 flag) {
    item->flags &= ~flag;
}

void itemFlagOn(ItemEntry* item, u16 flag) {
    item->flags |= flag;
}

void itemModeChange(ItemEntry* item, u16 mode) {
    item->mode = mode;
    item->field_0x26 = 0;
}

void itemSetPosition(ItemEntry* item, f32 x, f32 y, f32 z) {
    item->position.x = x;
    item->position.y = y;
    item->position.z = z;
}

// The zero check emits a compiler-generated local 0.0 constant (`@...`) with the operands
// reversed (`fcmpu dirX, 0`) where the original loads float_0_8041af9c and compares
// `fcmpu 0, dirX`; the push-out scratch floats also land in different registers.
// NONMATCHING (98.07%): compiler 0.0 pooling and push-out float registers differ.
HitEntry* itemHitCheckSide(ItemEntry* item, f32* outX, f32* outZ, s32* angle, f32 dirX, f32 dirZ) {
    f32 hitX;
    f32 hitY;
    f32 hitZ;
    f32 distance;
    f32 hitNX;
    f32 hitNY;
    f32 hitNZ;
    f32 sin;
    f32 cos;
    HitEntry* hit;
    f32 dot;
    f32 nx;
    f32 nz;

    *outX = item->position.x;
    *outZ = item->position.z;

    if (!dirX && !dirZ) {
        return NULL;
    }

    sincosf(&sin, &cos, angleABf(float_0_8041af9c, float_0_8041af9c, dirX, dirZ));

    distance = float_10_8041afb0 + sqrtf(dirX * dirX + dirZ * dirZ);
    hit = hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ,
                         item->position.x, item->position.y + float_15_8041afb4, item->position.z,
                         sin, float_0_8041af9c, cos);
    if (hit != NULL) {
        dot = dirX * hitNX + dirZ * hitNZ;
        nx = float_0p5_8041afb8 * (dirX - dot * hitNX);
        nz = float_0p5_8041afb8 * (dirZ - dot * hitNZ);
        *outX = *outX + nx;
        *outZ = *outZ + nz;
        dirZ = reviseAngle(float_180_8041afbc + (f32)*angle);
        dirX = reviseAngle(angleABf(float_0_8041af9c, float_0_8041af9c, float_100_8041afc0 * hitNX,
                                    float_100_8041afc0 * hitNZ));
        *angle = (s32)reviseAngle(dirX + reviseAngle(dirX - dirZ));
        return hit;
    }
    else {
        distance = float_10_8041afb0 + sqrtf(dirX * dirX + dirZ * dirZ);
        hit = hitCheckFilter(NULL, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ,
                             item->position.x, item->position.y + float_15_8041afb4, item->position.z,
                             sin, float_0_8041af9c, cos);
        if (hit != NULL) {
            dot = dirX * hitNX + dirZ * hitNZ;
            nx = float_0p5_8041afb8 * (dirX - dot * hitNX);
            nz = float_0p5_8041afb8 * (dirZ - dot * hitNZ);
            *outX = *outX + nx;
            *outZ = *outZ + nz;
            dirZ = reviseAngle(float_180_8041afbc + (f32)*angle);
            dirX = reviseAngle(angleABf(float_0_8041af9c, float_0_8041af9c, float_100_8041afc0 * hitNX,
                                        float_100_8041afc0 * hitNZ));
            *angle = (s32)reviseAngle(dirX + reviseAngle(dirX - dirZ));
            return hit;
        }
        else {
            *outX = *outX + dirX;
            *outZ = *outZ + dirZ;
            return NULL;
        }
    }
}

void itemForceGet(ItemEntry* item) {
    *(s32*)((u8*)item + 0x7c) = 0;
    *(s32*)((u8*)item + 0x78) = 0;
    item->mode = 2;
    item->field_0x26 = 0;
    item->flags |= 0x1000;
    ITEM_STATUS(item) &= ~0x10000;
}

void itemNokoForceGet(ItemEntry* item) {
    if (itemGetNokoCheck(item)) {
        *(s32*)((u8*)item + 0x7c) = 0;
        *(s32*)((u8*)item + 0x78) = 0;
        item->mode = 2;
        item->field_0x26 = 0;
        item->flags |= 0x5000;
        ITEM_STATUS(item) &= ~0x10000;
    }
    else {
        *(s32*)((u8*)item + 0x7c) = 0;
        *(s32*)((u8*)item + 0x78) = 0;
        item->mode = 1;
        item->field_0x26 = 0;
        ITEM_STATUS(item) &= ~0x4;
        ITEM_STATUS(item) &= ~0x8;
        ITEM_STATUS(item) &= ~0x4000;
        ITEM_STATUS(item) &= ~0x1000;
    }
}

s32 N_itemPickUpFromFieldCheck(void) {
    ItemWork* wp = itemGetWork();
    ItemEntry* entry;
    int i;

    for (entry = wp->entries, i = 0; i < wp->count; i++, entry++) {
        if (entry->flags & 1 && entry->mode == 2) {
            return 1;
        }
    }
    return 0;
}

ItemEntry* itemNameToPtr(char* name) {
    ItemWork* wp = itemGetWork();
    ItemEntry* entry = wp->entries;
    int i;

    for (i = 0; i < wp->count; i++, entry++) {
        if (!strcmp(entry->name, name)) {
            return entry;
        }
    }
    return NULL;
}

// Float-register allocation in the vertical range test differs (8 instructions): the original
// keeps the offset in f2, the 0.5 constant in f4 and the 10.0 constant in f1 (reused for the
// hit distance); this build picks f1/f2/f5 instead. The MUST_MATCH subtraction swap keeps the
// original's f3 (item Y) live; same value through fabs.
// NONMATCHING (99.72%): only the vertical-test float registers differ.
ItemEntry* itemHitCheck(f32 x, f32 y, f32 z, f32 maxDist) {
    ItemWork* wp = itemGetWork();
    MarioWork* mario;
    ItemEntry* entry;
    int i;
    ItemEntry* result;
    HitEntry* hit;
    f32 dx;
    f32 dy;
    f32 dz;
    f32 horizDist;
    f32 totalDist;
    f32 hitX;
    f32 hitY;
    f32 hitZ;
    f32 distance;
    f32 hitNX;
    f32 hitNY;
    f32 hitNZ;

    mario = marioGetPtr();
    entry = wp->entries;
    result = NULL;
    for (i = 0; i < wp->count; i++, entry++) {
        if (!(entry->flags & 1)) {
            continue;
        }
        if (entry->flags & 2) {
            continue;
        }
        if (itemStatus(entry, 0x1000)) {
            continue;
        }
        if (entry->mode == 2) {
            continue;
        }

        dx = x - entry->position.x;
        dy = y - entry->position.y;
        dz = z - entry->position.z;
        horizDist = sqrtf(dx * dx + dz * dz);
        totalDist = sqrtf(dx * dx + dy * dy + dz * dz);
        if (horizDist < maxDist
            && marioItemGetOk()
            && marioGetJabaraState() != 1
            && marioGetJabaraState() != 4
            && marioGetJabaraState() != 5
#ifdef MUST_MATCH
            // reversed subtraction order: same value (fabs), but it makes the original's f3 (item Y) live.
            && __fabsf(entry->position.y - (float_0p5_8041afb8 * mario->wPlayerCollisionBox.y + y))
                < (float_0p5_8041afb8 * mario->wPlayerCollisionBox.y + float_10_8041afb0)) {
#else
            && __fabsf((float_0p5_8041afb8 * mario->wPlayerCollisionBox.y + y) - entry->position.y)
                < (float_0p5_8041afb8 * mario->wPlayerCollisionBox.y + float_10_8041afb0)) {
#endif
            hit = NULL;
            if (y < entry->position.y) {
                distance = float_10_8041afb0 + (entry->position.y - y);
                if (distance > float_0_8041af9c) {
                    hit = hitCheckAttr(0x80000000, &hitX, &hitY, &hitZ, &distance, &hitNX, &hitNY, &hitNZ,
                                       x, y, z, float_0_8041af9c, float_1_8041afa4, float_0_8041af9c);
                }
            }
            if (hit == NULL) {
                result = entry;
                maxDist = totalDist;
            }
        }
    }
    return result;
}

// NONMATCHING (98.25%): the itemId range check emits `blt` to the else block where the
// original emits `bge` to the then block plus an extra `b`; logic is identical, 2 instr differ.
void itemDelete(char* name) {
    ItemWork* wp = itemGetWork();
    ItemEntry* entry = wp->entries;
    int i;

    for (i = 0; i < wp->count; i++, entry++) {
        if (!strcmp(entry->name, name)) {
            itemCheckBadge(entry);
            if (entry->itemId < 0x79 && entry->itemId >= 0x72) {
                effSoftDelete(entry->effect);
            }
            else {
                iconDelete(entry->name);
            }
            entry->flags &= ~1;
        }
    }
}

void winHelpMain(WinMgrEntry* entry) {
    winMgrHelpMain(entry);
}

void winHelpDisp(WinMgrEntry* entry) {
    ItemEntry* item = entry->param;

    if (!winMgrAction(*(s32*)((u8*)item + 0x2c))) {
        entry->field_0x34 = (s32)msgSearch(itemDataTable[item->itemId].description);
        winMgrHelpDraw(entry);
    }
}

void winFullDisp(WinMgrEntry* entry) {
    ItemEntry* item = entry->param;
    const char* msg;
    u16 width;

    msg = (item->itemId >= 0xf0 && item->itemId < 0x153) ? msgSearch(str_802CADE8) : msgSearch(str_802CAE00);

    width = FontGetMessageWidth(msg);
    if (width + 0x14 > entry->field_0x20) {
        entry->field_0x20 = width + 0x14;
        entry->field_0x18 = -entry->field_0x20 / 2;
    }

    if (!winMgrAction(*(s32*)((u8*)item + 0x30))) {
        GXColor color;

        FontDrawStart();
        FontDrawEdge();
        *(u32*)&color = dat_8041af98;
        FontDrawColor(&color);
        FontDrawMessage(-width / 2, entry->field_0x1C - 0xa, msg);
        FontDrawEdgeOff();
    }
}

void winNameDisp2(WinMgrEntry* entry) {
    ItemEntry* item = entry->param;
    char buf[0x400];
    const char* msg;
    const char* name;
    u16 width;

    name = msgSearch(itemDataTable[item->itemId].name);
    msg = msgSearch(str_802CAE18);
    sprintf(buf, msg, name);

    width = FontGetMessageWidth(buf);
    if (width + 0x14 > entry->field_0x20) {
        entry->field_0x20 = width + 0x14;
        entry->field_0x18 = -entry->field_0x20 / 2;
    }

    if (!winMgrAction(*(s32*)((u8*)item + 0x28))) {
        GXColor color;

        FontDrawStart();
        *(u32*)&color = dat_8041af94;
        FontDrawColor(&color);
        FontDrawMessage(-width / 2, entry->field_0x1C - 8, buf);
    }
}

void winNameDisp(WinMgrEntry* entry) {
    ItemEntry* item = entry->param;
    char buf[0x400];
    const char* msg;
    u16 width;

    msg = msgSearch(itemDataTable[item->itemId].name);
    if (item->itemId >= 0xf0 && item->itemId < 0x153) {
        sprintf(buf, msgSearch(str_802CAE30), msg);
    }
    else {
        sprintf(buf, msgSearch(str_802CAE48), msg);
    }

    width = FontGetMessageWidth(buf);
    if (width + 0x14 > entry->field_0x20) {
        entry->field_0x20 = width + 0x14;
        entry->field_0x18 = -entry->field_0x20 / 2;
    }

    if (!winMgrAction(*(s32*)((u8*)item + 0x28))) {
        GXColor color;

        FontDrawStart();
        *(u32*)&color = dat_8041af90;
        FontDrawColor(&color);
        FontDrawMessage(-width / 2, entry->field_0x1C - 8, buf);
    }
}

void itemCoinDrop(Vec* pos) {
    ItemEntry* item;

    if (pouchGetCoin() > 0) {
        pouchAddCoin(-1);
        item = itemEntry(NULL, 0x79, 8, -1, 0, pos->x, pos->y, pos->z);
        if (item != NULL) {
            ITEM_STATUS(item) |= 0x40;
        }
    }
}

// NONMATCHING (80.38%): logic is complete but the struct-init temporaries / register
// allocation differ from the original (frame 0x80 vs 0xa0); stack-offset diffs cascade.
ItemEntry* itemNearDistCheck(f32 x, f32 y, f32 z, f32 maxDist) {
    ItemEntry* entry;
    ItemEntry* result = NULL;
    ItemWork* wp;
    Vec pos;
    Vec itemPos;
    Vec delta;
    f32 best = maxDist;
    f32 mag;
    int i;

    marioGetPtr();
    wp = itemGetWork();

    pos = vec3_802cabec;
    pos.x = x;
    pos.y = y;
    pos.z = z;

    for (entry = wp->entries, i = 0; i < wp->count; i++, entry++) {
        if ((entry->flags & 1) && !itemStatus(entry, 0x1000) && !itemStatus(entry, 0x100000)
            && entry->mode != 2 && !itemStatus(entry, 0x2000000)) {
            itemPos = vec3_802cabf8;
            itemPos.x = entry->position.x;
            itemPos.y = entry->position.y;
            itemPos.z = entry->position.z;

            PSVECSubtract(&itemPos, &pos, &delta);
            mag = PSVECMag(&delta);
            if (mag < best) {
                result = entry;
                best = mag;
            }
        }
    }
    return result;
}

s32 itemGetNokoCheck(ItemEntry* entry) {
    MarioWork* mario;
    u32 motion;
    ItemWork* wp;
    ItemEntry* e;
    int i;

    mario = marioGetPtr();
    if (!entry) {
        return 0;
    }
    if (itemStatus(entry, 0x100000)) {
        return 0;
    }
    if ((u32)marioStGetSystemLevel() != 0) {
        return 0;
    }
    if (mario->flags & 0xa) {
        return 0;
    }
    if (itemStatus(entry, 0x2000000)) {
        return 0;
    }
    if ((s32)gp->field_0x164 != 0) {
        return 0;
    }
    if (seqGetSeq() != 2) {
        return 0;
    }
    if (mario->currMotionId == 0x19) {
        return 0;
    }
    if (vivianGetStatus() != 0) {
        return 0;
    }
    motion = mario->currMotionId;
    if (motion == 0x1f || motion == 0x20) {
        return 0;
    }
    if ((u16)(motion - 5) <= 3 || motion == 0xf || motion == 0xe) {
        return 0;
    }

    wp = itemGetWork();
    for (e = wp->entries, i = 0; i < wp->count; i++, e++) {
        if ((e->flags & 1) && e->mode == 2) {
            return 0;
        }
    }

    return N_fbatPreventMarioEventChk() == 0;
}

// NONMATCHING (93.34%): switch lowering block layout differs; the mode 1 / mode 3-4 return
// blocks are not tail-merged the way the original has them. Tree split was moved to 5 by
// reordering the cases.
s32 itemGetCheck(ItemEntry* entry) {
    MarioWork* mario;
    ItemWork* wp;
    ItemEntry* e;
    int i;

    mario = marioGetPtr();
    if (!entry) {
        return 0;
    }
    if (itemStatus(entry, 0x100000)) {
        return 0;
    }

    if (mario->characterId == 2) {
        if (!(*(u32*)((u8*)mario + 0x14) & 1)) {
            if ((u32)marioStGetSystemLevel() != 0) {
                return 0;
            }
            if (mario->flags & 0xa) {
                return 0;
            }
        }
    }
    else {
        if ((u32)marioStGetSystemLevel() != 0) {
            return 0;
        }
        if (mario->flags & 0xa) {
            return 0;
        }
    }

    if ((s32)gp->field_0x164 != 0) {
        return 0;
    }
    if (seqGetSeq() != 2) {
        return 0;
    }
    if (mario->currMotionId == 0x19) {
        return 0;
    }
    if (mario->currMotionId == 0x18) {
        return 0;
    }
    if (vivianGetStatus() != 0) {
        return 0;
    }
    if (mario->currMotionId == 0x1f || mario->currMotionId == 0x20) {
        return 0;
    }
    if (nokonoko_holdItem() != 0) {
        return 0;
    }

    wp = itemGetWork();
    for (e = wp->entries, i = 0; i < wp->count; i++, e++) {
        if ((e->flags & 1) && e->mode == 2) {
            return 0;
        }
    }
    if (N_fbatPreventMarioEventChk() != 0) {
        return 0;
    }

    switch (entry->mode) {
    case 2:
        return 0;
    case 5:
    case 6:
        if (entry->field_0x26 != 0 && !itemStatus(entry, 0x4) && !itemStatus(entry, 0x8)) {
            return 1;
        }
        return 0;
    case 3:
    case 4:
        if (itemStatus(entry, 0x8)) {
            return 0;
        }
        return 1;
    case 1:
        return 1;
    case 7:
        return 1;
    default:
        return 0;
    }
}
