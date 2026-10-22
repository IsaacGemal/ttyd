#include "eff/eff_sheep.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "memory.h"
#include "pmario_sound.h"
#include "mario/mariost.h"
#include "battle/battle.h"
#include "eff/eff_kemutest.h"
#include "system.h"
#include <dolphin/mtx.h>

// effSheepMain's `pos` zero vector must come from an inline that builds it from the
// literal `(Vec){ 0.0f, 0.0f, 0.0f }`, not from a named `vec3_...`: the scheduler then keeps the
// `effect->userdata` load ahead of the copy (from the named vec it hoists the copy first). The
// helper must sit above the unit's other rodata so the anonymous literal lands at the start of
// .rodata (offset 0), which is where the original's `vec3_802ff9e8` sat, and the named
// `vec3_802ff9e8` definition is dropped (the literal replaces it byte for byte).
static inline void sheepMakeVec(Vec* v, f32 x, f32 y, f32 z) {
	*v = (Vec){ 0.0f, 0.0f, 0.0f };
	v->y = y;
	v->x = x;
	v->z = z;
}



EffectEntry* effSheepEntry(s32 type, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	EffSheepWork* data;
	EffSheepWork* p;
	s32 i;

	entry = effEntry();
	if (gp->inBattle != 0) {
		y = BattleGetFloorHeight(_battleWorkPointer, x, y, z);
	}
	entry->type = "sheep";
	entry->count = 0x33;
	data = __memAlloc(HEAP_EFFECT, entry->count * sizeof(EffSheepWork));
	entry->userdata = data;
	entry->callback = effSheepMain;
	entry->flags |= 2;

	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = 1.0f;
	data->state = 0;

	p = &data[1];
	for (i = 1; i < entry->count; i++, p++) {
		p->type = irand(100) < 20;
		p->x = -500.f;
		p->y = 0.0f;
		p->z = 0.0f;
		p->poseId = -1;
		p->state = 0;
		p->scale = (f32)(irand(5) + 8) / 10.f;
		p->field_0x2C = (f32)(irand(20) + 20) / 10.f;
		p->timer = i + irand(30);
		p->z = (f32)(100 - irand(200));

		if (p->type != 0) {
			p->field_0x14 = (s32)((f32)(irand(50) + 50) / p->field_0x2C);
			p->field_0x1C = 0.2f;
			p->field_0x18 = p->field_0x1C * p->field_0x14 / 2.0f;
		}
		if (entry->count - 1 == i) {
			p->z = 0.0f;
			p->type = 0;
			p->scale = 1.0f;
			p->timer = entry->count + 200;
		}
	}
	return entry;
}

void effSheepMain(EffectEntry* effect) {
	EffSheepWork* p;
	const char* sfx[2];
	Vec pos2;
	Vec pos;
	Vec soundPos1;
	Vec soundPos2;
	Vec soundPos3;
	s32 i;
	s32 living;
	s32 alive = 0;
	s32 inBattle;
	EffSheepWork* data = effect->userdata;

	sheepMakeVec(&pos, data->x, data->y, data->z);
	pos2 = pos;
	inBattle = gp->inBattle != 0;
	if (effect->flags & 4) {
		effect->flags &= ~4;
		living = 1;
		// The separate increment makes MWCC coalesce the userdata load's destination with the
		// pointer register (the target loads it straight into r25, not r3 then r25).
		p = effect->userdata;
		p++;
		while (living < effect->count) {
			if (p->poseId != -1) {
				animPoseRelease(p->poseId);
			}
			living++;
			p++;
		}
		psndSFXOff(((EffSheepWork*)effect->userdata)[1].sfxId);
		psndSFXOff(((EffSheepWork*)effect->userdata)[effect->count - 1].sfxId);
		effDelete(effect);
		return;
	}

	sfx[0] = "SFX_ITEM_SLEEP1";
	sfx[1] = "SFX_ITEM_SLEEP2";
	p = &data[1];
	for (i = 1; i < effect->count; i++, p++) {
		switch (p->state) {
		case 0:
			if (animGroupBaseAsync("EFF_hituji", inBattle, NULL)) {
				p->poseId = animPoseEntry("EFF_hituji", inBattle);
				animPoseSetAnim(p->poseId, "A_1", 1);
				p->state++;
			}
			break;
		case 1:
			if (--p->timer < 0) {
				p->timer = 0;
				if (i == 1) {
					soundPos1 = (Vec){ 0.0f, 0.0f, 0.0f };
					soundPos1.x = pos2.x + p->x;
					soundPos1.y = pos2.y + p->y;
					soundPos1.z = pos2.z + p->z;
					p->sfxId = psndSFXOn_3D(sfx[0], &soundPos1);
				}
				else if (i == effect->count - 1) {
					soundPos2 = (Vec){ 0.0f, 0.0f, 0.0f };
					soundPos2.x = pos2.x + p->x;
					soundPos2.y = pos2.y + p->y;
					soundPos2.z = pos2.z + p->z;
					p->sfxId = psndSFXOn_3D(sfx[1], &soundPos2);
				}
				p->state++;
			}
			break;
		case 2:
			if (i == 1 || i == effect->count - 1) {
				soundPos3 = (Vec){ 0.0f, 0.0f, 0.0f };
				soundPos3.x = pos2.x + p->x;
				soundPos3.y = pos2.y + p->y;
				soundPos3.z = pos2.z + p->z;
				psndSFX_pos(p->sfxId, &soundPos3);
			}
			if (p->x < 500.f) {
				p->x += p->field_0x2C;
				if (p->type == 1) {
					f32 zero = 0.0f;
					p->y += p->field_0x18;
					p->field_0x18 -= p->field_0x1C;
					if (p->y < zero) {
						p->y = zero;
						p->field_0x18 *= -1.0f;
					}
				}
			}
			else {
				p->state++;
			}
			if ((gp->retraceCount + i) % 10 == 0) {
				effKemuTestEntry(0, pos2.x + p->x, pos2.y + p->y + 10.f,
						pos2.z + p->z, 0.3f + (f32)irand(30) / 100.f);
			}
			break;
		case 3:
			if (i == effect->count - 2) {
				psndSFXOff(((EffSheepWork*)effect->userdata)[1].sfxId);
			}
			if (i == effect->count - 1) {
				psndSFXOff(p->sfxId);
			}
			animPoseRelease(p->poseId);
			p->poseId = -1;
			p->state++;
			break;
		case 4:
			alive++;
			break;
		}
		if (p->poseId != -1) {
			animPoseMain(p->poseId);
		}
	}

	if (alive >= effect->count - 1) {
		effDelete(effect);
	}
	else {
		dispEntry(CAMERA_3D, dispCalcZ(&pos2), 1, effSheepDisp, effect);
	}
}

// `data++; for (i = 1; i < effect->count; i++, data++)` (the effStarPointDisp shape)
// makes MWCC schedule the preheader as `li r30,1` then `addi r31,r31,0x34`, matching the original;
// with a separate pointer (`p = &data[1]; while (...)`) the scheduler emits the addi first.
void effSheepDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	EffSheepWork* data = effect->userdata;
	Mtx mtxLocal;
	Mtx mtxScale;
	Mtx mtxBase;
	s32 i;

	PSMTXTrans(mtxBase, data->x, data->y, data->z);
	data++;
	for (i = 1; i < effect->count; i++, data++) {
		if (data->poseId != -1) {
			PSMTXTrans(mtxLocal, data->x, data->y, data->z);
			PSMTXScale(mtxScale, data->scale, data->scale, data->scale);
			PSMTXConcat(mtxLocal, mtxScale, mtxLocal);
			PSMTXConcat(mtxBase, mtxLocal, mtxLocal);
			animPoseDrawMtx(data->poseId, mtxLocal, 1, 0.0f, 10.f);
			animPoseDrawMtx(data->poseId, mtxLocal, 2, 0.0f, 10.f);
			animPoseDrawMtx(data->poseId, mtxLocal, 3, 0.0f, 10.f);
		}
	}
}

// The pool constants the code above loads by name, defined after it (defined first, the compiler folds them).
