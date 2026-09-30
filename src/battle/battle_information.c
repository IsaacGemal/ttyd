#include "battle/battle_information.h"

#include "battle/battle.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mario_pouch.h"
#include "system.h"

void BattleInformationSetMode(FieldBattleInfo* info, u32 mode) {
	info->mode = mode;
}

void BattleInformationSetParty(FieldBattleInfo* info, BattleUnitType party) {
	info->party = party;
}

void BattleInformationSetFirstAttack(FieldBattleInfo* info, u32 partyId) {
	info->partyId = partyId;
}

// NONMATCHING (3.57%): empty placeholder, not decompiled yet.
void BattleInformationInit(FieldBattleInfo* info) {

}

void BattleInfomationSetBattleSetupInfo(FieldBattleInfo* info, NpcBattleInfo* setup) {
	info->setup = setup;
}

void BattleInformationSetResult(FieldBattleInfo* info, u32 result) {
	info->result = result;
}

u32 BattleInformationGetResult(FieldBattleInfo* info) {
	return info->result;
}

// One entry of a heart/flower drop parameter table (battle_heart_drop_param_default etc.).
typedef struct DropMaterialParam {
	s32 field_0x0; // 0x0
	s32 field_0x4; // 0x4
	s32 field_0x8; // 0x8
	s32 field_0xC; // 0xC
} DropMaterialParam;

// The 20 candidate items the Hog badge rolls between.
typedef struct HogTable {
	u32 items[20];
} HogTable;

// Items the Hog badge can drop when the player has no held items.
HogTable N_item_hog_table = {
	{
		0x00000098, 0x00000098, 0x00000098, 0x000000A2,
		0x00000094, 0x0000009A, 0x00000099, 0x00000084,
		0x00000087, 0x0000008B, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
		0x00000000, 0x00000000, 0x00000000, 0x00000000,
	}
};

// NONMATCHING (98.05%): the original hoists each drop-count load (lwz r26, 0x8(rp)) above the chance
// comparison; MWCC 2.6 leaves it after the branch. No source form or pragma tried moves it.
void BattleInformationSetDropMaterial(FieldBattleInfo* info) {
	NpcBattleInfo* setup = info->setup;
	BattleGroupSetup* group = setup->groupSetup;
	DropMaterialParam* heartParams = (DropMaterialParam*)group->heartDropParams;
	DropMaterialParam* flowerParams = (DropMaterialParam*)group->flowerDropParams;
	BattleUnit* mario = BattleGetMarioPtr(_battleWorkPointer);
	s32 hpPercent;
	s32 fpPercent;
	s32 roll;
	s32 i;
	s32 threshold;
	s32 j;

	hpPercent = mario->currentHp * 100 / mario->maxHp;
	if (hpPercent > 100) {
		hpPercent = 100;
	}

	setup->heartsDropped = 0;
	while ((threshold = heartParams->field_0x0) != 0) {
		if (hpPercent <= threshold) {
			if (irand(100) < heartParams->field_0x4) {
				i = heartParams->field_0x8;
				while (i != 0) {
					if (irand(100) < heartParams->field_0xC) {
						setup->heartsDropped++;
					}
					i--;
				}
			}
			break;
		}
		heartParams++;
	}

	fpPercent = mario->currentFp * 100 / mario->maxFp;
	if (fpPercent > 100) {
		fpPercent = 100;
	}

	setup->flowersDropped = 0;
	while ((threshold = flowerParams->field_0x0) != 0) {
		if (fpPercent <= threshold) {
			if (irand(100) < flowerParams->field_0x4) {
				j = flowerParams->field_0x8;
				while (j != 0) {
					if (irand(100) < flowerParams->field_0xC) {
						setup->flowersDropped++;
					}
					j--;
				}
			}
			break;
		}
		flowerParams++;
	}

	{
		s32 fieldA = group->field_0x8;
		s32 fieldB = group->field_0xC;
		s32 fieldC = group->field_0x10;
		s32 band = 0;
		s32 badge;

		if (evtGetValue(0, 0xF5DE0180) <= 0x13) {
			fieldB = 0;
			fieldA = 0;
			fieldC = 100;
		}
		badge = pouchEquipCheckBadge(0x13a);
		if (badge >= 1) {
			fieldA *= badge + 1;
			fieldB *= badge + 1;
		}
		roll = irand(fieldB + fieldA + fieldC);

		{
			s32 diff = roll - fieldB;

			if (diff < 0) {
				band = 1;
			} else if (diff - fieldA < 0) {
				band = 2;
			}
		}

		setup->itemDropped = 0;

		switch (band) {
			case 0:
				break;
			case 1: {
				ItemDropData* p = group->enemyData->itemDropTable;
				ItemDropData* q;
				s32 total;

				q = p;
				if (p == NULL) {
					break;
				}
				total = 0;
				while (q->dropWeight > 0 || q->type != 0) {
					total += q->dropWeight;
					q++;
				}
				if (total > 0) {
					roll = irand(total);
					while (1) {
						roll -= p->dropWeight;
						if (roll < 0) {
							setup->itemDropped = p->type;
							break;
						}
						p++;
					}
				}
				break;
			}
			case 2: {
				s32 count = 0;

				for (i = 0; i < 8; i++) {
					if (setup->heldItems[i] != 0) {
						count++;
					}
				}
				if (count > 0) {
					s32 r = irand(count);

					for (i = 0; i < 8; i++) {
						ItemType item = setup->heldItems[i];

						if (item != 0) {
							if (--r < 0) {
								setup->itemDropped = item;
								break;
							}
						}
					}
					if (i >= 8) {
						setup->itemDropped = 0;
					}
				} else if (pouchEquipCheckBadge(0x13a) >= 1) {
					HogTable itemTable = N_item_hog_table;

					setup->itemDropped = itemTable.items[irand(20)];
				}
				break;
			}
		}
	}
}
