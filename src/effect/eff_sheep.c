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

extern const f32 float_0_80421948;
extern const f32 float_10_8042194c;
extern const f32 float_500_80421954;
extern const f32 float_neg1_80421958;
extern const f32 float_0p3_8042195c;
extern const f32 float_100_80421960;
extern const f32 float_1_8042196c;
extern const f32 float_neg500_80421970;
extern const f32 float_0p2_80421974;
extern const f32 float_0p5_80421978;

const Vec vec3_802ff9e8 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802ff9f4 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802ffa00 = {0.0f, 0.0f, 0.0f};
const Vec vec3_802ffa0c = {0.0f, 0.0f, 0.0f};
const char str_802FFA18[] = "SFX_ITEM_SLEEP1";
const char str_802FFA28[] = "SFX_ITEM_SLEEP2";
const char str_802FFA38[] = "EFF_hituji";
f64 double_to_int_802ffa48 = 4503601774854144.0;
const char str_80421950[] = "A_1";
const char str_80421964[] = "sheep";

// NONMATCHING (98.21%): only the loop preheader's `li r30,1` (i) and `addi r31,r31,0x34` (p) are
// scheduled in the opposite order; every other instruction matches.
void effSheepDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	EffSheepWork* data = effect->userdata;
	Mtx mtxLocal;
	Mtx mtxScale;
	Mtx mtxBase;
	EffSheepWork* p;
	s32 i;

	PSMTXTrans(mtxBase, data->x, data->y, data->z);
	i = 1;
	p = &data[1];
	while (i < effect->count) {
		if (p->poseId != -1) {
			PSMTXTrans(mtxLocal, p->x, p->y, p->z);
			PSMTXScale(mtxScale, p->scale, p->scale, p->scale);
			PSMTXConcat(mtxLocal, mtxScale, mtxLocal);
			PSMTXConcat(mtxBase, mtxLocal, mtxLocal);
			animPoseDrawMtx(p->poseId, mtxLocal, 1, float_0_80421948, float_10_8042194c);
			animPoseDrawMtx(p->poseId, mtxLocal, 2, float_0_80421948, float_10_8042194c);
			animPoseDrawMtx(p->poseId, mtxLocal, 3, float_0_80421948, float_10_8042194c);
		}
		i++;
		p++;
	}
}

// NONMATCHING (95.74%): the whole state machine matches; the remaining diffs are register numbers
// (p/sfx1/sfx2 are r30/r28/r29 instead of r28/r30/r29) and a few float temporaries in the type==1 block.
void effSheepMain(EffectEntry* effect) {
	EffSheepWork* p;
	const char* sfx2;
	const char* sfx1;
	Vec pos2;
	Vec pos;
	Vec soundPos1;
	Vec soundPos2;
	Vec soundPos3;
	s32 i;
	s32 living;
	s32 inBattle;

	pos = vec3_802ff9e8;
	pos.x = ((EffSheepWork*)effect->userdata)->x;
	pos.y = ((EffSheepWork*)effect->userdata)->y;
	pos.z = ((EffSheepWork*)effect->userdata)->z;
	pos2 = pos;
	inBattle = gp->inBattle != 0;
	if (effect->flags & 4) {
		effect->flags &= ~4;
		living = 1;
		p = &((EffSheepWork*)effect->userdata)[1];
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

	sfx1 = str_802FFA18;
	sfx2 = str_802FFA28;
	p = &((EffSheepWork*)effect->userdata)[1];
	for (i = 1; i < effect->count; i++, p++) {
		switch (p->state) {
		case 0:
			if (animGroupBaseAsync(str_802FFA38, inBattle, NULL)) {
				p->poseId = animPoseEntry(str_802FFA38, inBattle);
				animPoseSetAnim(p->poseId, str_80421950, 1);
				p->state++;
			}
			break;
		case 1:
			if (--p->timer < 0) {
				p->timer = 0;
				if (i == 1) {
					soundPos1 = vec3_802ff9f4;
					soundPos1.x = pos2.x + p->x;
					soundPos1.y = pos2.y + p->y;
					soundPos1.z = pos2.z + p->z;
					p->sfxId = psndSFXOn_3D(sfx1, &soundPos1);
				}
				else if (i == effect->count - 1) {
					soundPos2 = vec3_802ffa00;
					soundPos2.x = pos2.x + p->x;
					soundPos2.y = pos2.y + p->y;
					soundPos2.z = pos2.z + p->z;
					p->sfxId = psndSFXOn_3D(sfx2, &soundPos2);
				}
				p->state++;
			}
			break;
		case 2:
			if (i == 1 || i == effect->count - 1) {
				soundPos3 = vec3_802ffa0c;
				soundPos3.x = pos2.x + p->x;
				soundPos3.y = pos2.y + p->y;
				soundPos3.z = pos2.z + p->z;
				psndSFX_pos(p->sfxId, &soundPos3);
			}
			if (p->x < float_500_80421954) {
				p->x += p->field_0x2C;
				if (p->type == 1) {
					p->y += p->field_0x18;
					p->field_0x18 -= p->field_0x1C;
					if (p->y < float_0_80421948) {
						p->y = float_0_80421948;
						p->field_0x18 *= float_neg1_80421958;
					}
				}
			}
			else {
				p->state++;
			}
			if ((gp->retraceCount + i) % 10 == 0) {
				effKemuTestEntry(0, pos2.x + p->x, pos2.y + p->y + float_10_8042194c,
						pos2.z + p->z, float_0p3_8042195c + (f32)irand(30) / float_100_80421960);
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
			living++;
			break;
		}
		if (p->poseId != -1) {
			animPoseMain(p->poseId);
		}
	}

	if (living >= effect->count - 1) {
		effDelete(effect);
	}
	else {
		dispEntry(CAMERA_3D, dispCalcZ(&pos2), 1, effSheepDisp, effect);
	}
}

// NONMATCHING (99.49%): only the float-register allocation in the `type != 0` block differs
// (target divisor f2 / 0.2 f1 / 0.5 f0, here f1 / f0 / f2).
EffectEntry* effSheepEntry(s32 type, f32 x, f32 y, f32 z) {
	EffectEntry* entry;
	EffSheepWork* data;
	EffSheepWork* p;
	s32 i;

	entry = effEntry();
	if (gp->inBattle != 0) {
		y = BattleGetFloorHeight(_battleWorkPointer, x, y, z);
	}
	entry->type = str_80421964;
	entry->count = 0x33;
	data = __memAlloc(HEAP_EFFECT, entry->count * sizeof(EffSheepWork));
	entry->userdata = data;
	entry->callback = effSheepMain;
	entry->flags |= 2;

	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->scale = float_1_8042196c;
	data->state = 0;

	p = &data[1];
	for (i = 1; i < entry->count; i++, p++) {
		p->type = irand(100) < 20;
		p->x = float_neg500_80421970;
		p->y = float_0_80421948;
		p->z = float_0_80421948;
		p->poseId = -1;
		p->state = 0;
		p->scale = (f32)(irand(5) + 8) / float_10_8042194c;
		p->field_0x2C = (f32)(irand(20) + 20) / float_10_8042194c;
		p->timer = i + irand(30);
		p->z = (f32)(100 - irand(200));

		if (p->type != 0) {
			p->field_0x14 = (s32)((f32)(irand(50) + 50) / p->field_0x2C);
			p->field_0x1C = float_0p2_80421974;
			p->field_0x18 = p->field_0x1C * p->field_0x14 * float_0p5_80421978;
		}
		if (entry->count - 1 == i) {
			p->z = float_0_80421948;
			p->type = 0;
			p->scale = float_1_8042196c;
			p->timer = entry->count + 200;
		}
	}
	return entry;
}
