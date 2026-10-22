#pragma once

#include "evt/evt_cmd.h"
#include "battle/battle_database_common.h"
extern BattleWeapon marioWeapon_KaitenHammerInvolved;
extern BattleWeapon marioWeapon_UltraHammerInvolved;
extern BattleWeapon marioDefaultWeapon_Jump;
extern BattleWeapon marioWeapon_KururinJump;
extern BattleWeapon marioWeapon_JyabaraJump;
extern BattleWeapon marioDefaultWeapon_Hammer;
extern BattleWeapon marioWeapon_KaitenHammer;
extern BattleWeapon marioWeapon_UltraHammer;
extern BattleWeapon* superActionTable[18];
extern BattleWeapon ItemWeaponData_CookingItem;
extern s32 marioAttackEvent_MajinaiDefenceUp[];

USER_FUNC(_whirlwind_effect);
USER_FUNC(_jump_star_effect);
USER_FUNC(_bgset_iron_frame_check);
USER_FUNC(_wait_jyabara_hit_iron_frame);
USER_FUNC(_tatsumaki_effect);
USER_FUNC(_record_renzoku_count);
USER_FUNC(mario_get_renzoku_count_max);
USER_FUNC(_hammer_star_effect);
USER_FUNC(_kaiten_hammer_acrobat_rotate);
USER_FUNC(_mario_fire_ball_controll);
USER_FUNC(_fire_wave);
USER_FUNC(_majinai_powerup_check);
USER_FUNC(_battle_majinai_effect);
USER_FUNC(_paper_light_off);
extern BattleWeapon badgeWeapon_2KaiItem;
extern BattleWeapon badgeWeapon_3KaiItem;
extern BattleWeapon badgeWeapon_Charge;
extern BattleWeapon badgeWeapon_ChargeP;
extern BattleWeapon badgeWeapon_SuperCharge;
extern BattleWeapon badgeWeapon_SuperChargeP;
