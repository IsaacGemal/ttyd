#include "evt/evt_eff.h"
#include "driver/effdrv.h"
#include "eff/n64/eff_expbom_n64.h"
#include "eff/eff_n64.h"
#include "evt/evt_cmd.h"
#include <string.h>
#include "manager/evtmgr_cmd.h"
extern const f32 float_0_8041a240;
extern const char str_8041A244[4];
extern const char lbl_8041A248[4];
#include "eff/eff_fukidashi.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "party.h"
#include "driver/npcdrv.h"
#include "eff/eff_bomb.h"
#include "eff/eff_breath_fire.h"
#include "eff/eff_butterfly.h"
#include "eff/eff_charge.h"
#include "eff/eff_confetti.h"
#include "eff/eff_damage_star.h"
#include "eff/eff_fall.h"
#include "eff/eff_fire.h"
#include "eff/eff_gonbaba_breath.h"
#include "eff/eff_hibashira.h"
#include "eff/eff_hit.h"
#include "eff/eff_ibuki.h"
#include "eff/eff_ice.h"
#include "eff/eff_indirect.h"
#include "eff/eff_irekaeru.h"
#include "eff/eff_itemget.h"
#include "eff/eff_kemuri.h"
#include "eff/eff_kemutest.h"
#include "eff/eff_kiss.h"
#include "eff/eff_laser.h"
#include "eff/eff_las_mon.h"
#include "eff/eff_levelup.h"
#include "eff/eff_machinegun.h"
#include "eff/eff_mahojin.h"
#include "eff/eff_mahorn.h"
#include "eff/eff_majinai.h"
#include "eff/eff_mist2.h"
#include "eff/eff_mist.h"
#include "eff/eff_nice.h"
#include "eff/eff_nozle.h"
#include "eff/eff_particle.h"
#include "eff/eff_puniballoon.h"
#include "eff/eff_queen2.h"
#include "eff/eff_queen.h"
#include "eff/eff_rankup.h"
#include "eff/eff_recovery.h"
#include "eff/eff_ripple.h"
#include "eff/eff_sandars.h"
#include "eff/eff_scanning.h"
#include "eff/eff_sheep.h"
#include "eff/eff_small_star.h"
#include "eff/eff_splash.h"
#include "eff/eff_stageclear.h"
#include "eff/eff_stardust.h"
#include "eff/eff_starpoint.h"
#include "eff/eff_starstone.h"
#include "eff/eff_syuryou.h"
#include "eff/eff_toge_flush.h"
#include "eff/eff_toiki.h"
#include "eff/eff_treasure_map.h"
#include "eff/eff_uranoko.h"
#include "eff/n64/eff_akari_charge_n64.h"
#include "eff/n64/eff_akari_pose_n64.h"
#include "eff/n64/eff_akari_sflush_n64.h"
#include "eff/n64/eff_balloon_heiho_n64.h"
#include "eff/n64/eff_balloon_n64.h"
#include "eff/n64/eff_blow_n64.h"
#include "eff/n64/eff_bomb_n64.h"
#include "eff/n64/eff_bomhei_n64.h"
#include "eff/n64/eff_booboo_n64.h"
#include "eff/n64/eff_breath_fire_n64.h"
#include "eff/n64/eff_broken_barrier_n64.h"
#include "eff/n64/eff_bush_n64.h"
#include "eff/n64/eff_butterfly_n64.h"
#include "eff/n64/eff_cloud_n64.h"
#include "eff/n64/eff_coin_n64.h"
#include "eff/n64/eff_confetti_n64.h"
#include "eff/n64/eff_cool_leaf_n64.h"
#include "eff/n64/eff_crystal_n64.h"
#include "eff/n64/eff_dogabon_attack_n64.h"
#include "eff/n64/eff_explosion_n64.h"
#include "eff/n64/eff_fall_leaf_n64.h"
#include "eff/n64/eff_fire_dust_n64.h"
#include "eff/n64/eff_fireflower_n64.h"
#include "eff/n64/eff_fire_ring_n64.h"
#include "eff/n64/eff_fire_spark_n64.h"
#include "eff/n64/eff_fireworks_n64.h"
#include "eff/n64/eff_flame_n64.h"
#include "eff/n64/eff_flower_n64.h"
#include "eff/n64/eff_freeze_n64.h"
#include "eff/n64/eff_gareki_n64.h"
#include "eff/n64/eff_guruguru_n64.h"
#include "eff/n64/eff_hagetaka_flush_n64.h"
#include "eff/n64/eff_hit_n64.h"
#include "eff/n64/eff_hokori_n64.h"
#include "eff/n64/eff_holography_n64.h"
#include "eff/n64/eff_horao_spell_n64.h"
#include "eff/n64/eff_ice_n64.h"
#include "eff/n64/eff_kameki_tornade_n64.h"
#include "eff/n64/eff_kemuri10_n64.h"
#include "eff/n64/eff_kemuri11_n64.h"
#include "eff/n64/eff_kemuri1_n64.h"
#include "eff/n64/eff_kemuri2_n64.h"
#include "eff/n64/eff_kemuri3_n64.h"
#include "eff/n64/eff_kemuri4_n64.h"
#include "eff/n64/eff_kemuri5_n64.h"
#include "eff/n64/eff_kemuri6_n64.h"
#include "eff/n64/eff_kemuri7_n64.h"
#include "eff/n64/eff_kemuri8_n64.h"
#include "eff/n64/eff_kemuri9_n64.h"
#include "eff/n64/eff_kumokumo_chip_n64.h"
#include "eff/n64/eff_kumokumo_n64.h"
#include "eff/n64/eff_lensflare_n64.h"
#include "eff/n64/eff_magic1_n64.h"
#include "eff/n64/eff_magic2_n64.h"
#include "eff/n64/eff_magic3_n64.h"
#include "eff/n64/eff_magic_horao_n64.h"
#include "eff/n64/eff_onpu_n64.h"
#include "eff/n64/eff_opuku_jetw_n64.h"
#include "eff/n64/eff_opuku_jetwrain_n64.h"
#include "eff/n64/eff_opuku_wcurtain_n64.h"
#include "eff/n64/eff_opuku_wgun_n64.h"
#include "eff/n64/eff_opuku_wprotect_n64.h"
#include "eff/n64/eff_packn_bfire_n64.h"
#include "eff/n64/eff_peach_dust_n64.h"
#include "eff/n64/eff_pikkyolo_n64.h"
#include "eff/n64/eff_pokopi_pcharge_n64.h"
#include "eff/n64/eff_pokopi_pipo_n64.h"
#include "eff/n64/eff_pokopi_thunder_n64.h"
#include "eff/n64/eff_pturn_n64.h"
#include "eff/n64/eff_radiation_n64.h"
#include "eff/n64/eff_rappa_n64.h"
#include "eff/n64/eff_recover_heart_n64.h"
#include "eff/n64/eff_release_n64.h"
#include "eff/n64/eff_ripple_n64.h"
#include "eff/n64/eff_rothammer_fail_n64.h"
#include "eff/n64/eff_rothammer_success_n64.h"
#include "eff/n64/eff_rubble_n64.h"
#include "eff/n64/eff_shock_n64.h"
#include "eff/n64/eff_small_star_n64.h"
#include "eff/n64/eff_snow_dust_n64.h"
#include "eff/n64/eff_snowfall_n64.h"
#include "eff/n64/eff_snowman_n64.h"
#include "eff/n64/eff_snow_n64.h"
#include "eff/n64/eff_sstar_n64.h"
#include "eff/n64/eff_stamp_n64.h"
#include "eff/n64/eff_stardust2_n64.h"
#include "eff/n64/eff_stardust_n64.h"
#include "eff/n64/eff_star_power_n64.h"
#include "eff/n64/eff_stone_smoke_n64.h"
#include "eff/n64/eff_sunlight_n64.h"
#include "eff/n64/eff_sweat_n64.h"
#include "eff/n64/eff_thruhammer_n64.h"
#include "eff/n64/eff_thunderflare_n64.h"
#include "eff/n64/eff_thunder_n64.h"
#include "eff/n64/eff_timestamp_n64.h"
#include "eff/n64/eff_vapor_n64.h"
#include "eff/n64/eff_wangflush_n64.h"
#include "eff/n64/eff_water_damage_n64.h"
#include "eff/n64/eff_whirlwind_n64.h"

// NONMATCHING (99.17%): every case matches; MWCC emits the target int base (`addi r25, r27, 8`) but coalesces
// the float base copy (`mr r24, r25`) into it, so we save 11 callee-saved regs (stmw r21) instead of 12 (stmw r20)
// and effSet/int sit one register higher (r24/r23..r21 vs target r23/r22..r20).
// Tried and still coalesced: `fargs = iargs`/parens/casts (arg array, void*, u32/f32 pointer types), union and
// struct-box copies, declaration reorder/one-line, assignment placed after each call, index vs pointer reads,
// explicit increments, register/const args, a third alias pointer; pragmas opt_propagation/opt_common_subs/
// opt_lifetimes/opt_dead_code/opt_strength_reduction/global_optimizer off, optimization_level 2-4, scheduling
// off (all no change or worse); file flags -O2/-O3/-O4,s/-inline auto. Only the source text retail used reproduces it.
// Putting the copy after the `effSet` call (or through an address-taken helper there) does emit `stmw r20` and a
// real copy, but rematerialises the int base as `addi rN, args, 0xc` and recolours the whole function (event r30,
// args r29): 97.90%. The copy must survive at its current position without moving the colouring; permuter work.
USER_FUNC(evt_eff) {
	s32* args = event->args;
	s32* iargs;
	s32* fargs;
	EffectEntry* entry;
	const char* effName;
	const char* effSet;
	s32 int2, int3, int4, int5, int6, int7, int8, int9,
		int10, int11, int12, int13, int14;
	f32 float2, float3, float4, float5, float6, float7, float8,
		float9, float10, float11, float12, float13, float14;

	effName = (const char*)evtGetValue(event, args[0]);
	//these are probably storing different typecasts, s32 and u32
	evtGetValue(event, args[0]);
	evtGetValue(event, args[0]);
	iargs = args + 2;
	fargs = args + 2;
	effSet = (const char*)evtGetValue(event, args[1]);
	//these are probably storing different typecasts, s32 and u32
	evtGetValue(event, args[1]);
	evtGetValue(event, args[1]);
	int2 = evtGetValue(event, *iargs++);
	int3 = evtGetValue(event, *iargs++); //unused
	int4 = evtGetValue(event, *iargs++); //unused
	int5 = evtGetValue(event, *iargs++); //unused
	int6 = evtGetValue(event, *iargs++);
	int7 = evtGetValue(event, *iargs++);
	int8 = evtGetValue(event, *iargs++);
	int9 = evtGetValue(event, *iargs++);
	int10 = evtGetValue(event, *iargs++);
	int11 = evtGetValue(event, *iargs++);
	int12 = evtGetValue(event, *iargs++); //unused
	int13 = evtGetValue(event, *iargs++); //unused
	int14 = evtGetValue(event, *iargs++); //unused

	float2 = evtGetFloat(event, *fargs++); //unused
	float3 = evtGetFloat(event, *fargs++);
	float4 = evtGetFloat(event, *fargs++);
	float5 = evtGetFloat(event, *fargs++);
	float6 = evtGetFloat(event, *fargs++);
	float7 = evtGetFloat(event, *fargs++);
	float8 = evtGetFloat(event, *fargs++);
	float9 = evtGetFloat(event, *fargs++);
	float10 = evtGetFloat(event, *fargs++);
	float11 = evtGetFloat(event, *fargs++);
	float12 = evtGetFloat(event, *fargs++);
	float13 = evtGetFloat(event, *fargs++);
	float14 = evtGetFloat(event, *fargs++);
	entry = NULL;

	switch (effGetSet(effSet)->id) {
		case 0: entry = effKemuriEntry(int2, float3, float4, float5, float6); break;
		case 28: entry = effKemuTestEntry(int2, float3, float4, float5, float6); break;
		case 29: entry = effStarStoneEntry(int2, float3, float4, float5, float6); break;
		case 1: entry = effConfettiEntry(int2, int6, float3, float4, float5); break;
		case 3: entry = effButterflyEntry(int2, float3, float4, float5); break;
		case 10: entry = effRecoveryEntry(int2, int6, float3, float4, float5); break;
		case 13: {
			entry = effBreathFireEntry(int2, float3, float4, int9, float5, int10, float6, int11, float7, float8);
			*(f32*)((u8*)entry->userdata + 0x38) = float12;
			*(f32*)((u8*)entry->userdata + 0x30) = float12;
			*(f32*)((u8*)entry->userdata + 0x34) = float13;
			break;
		}
		case 4: entry = effHitEntry(int2, int7, float3, float4, float5, float6); break;
		case 22: entry = effBombEntry(int2, float3, float4, float5, float6); break;
		case 18: entry = effChargeEntry(int2, int7, float3, float4, float5, float6); break;
		case 19: entry = effTogeFlushEntry(int2, float3, float4, float5, int6); break;
		case 20: entry = effIceEntry(int2, int7, float3, float4, float5, float6); break;
		case 5: entry = effDamageStarEntry(int2, int8, float3, float4, float5, (f32)int6, (f32)int7); break;
		case 25: entry = effStarPointEntry(int2, int6, float3, float4, float5); break;
		case 30: entry = effItemGetEntry(int2, float3, float4, float5); break;
		case 34: entry = effStageClearEntry(int2, float3, float4, float5); break;
		case 33: entry = effLevelupEntry(int2, int6, float3, float4, float5); break;
		case 35: entry = effFallEntry(int2, float3, float4, int9, float5, int10, float6, float7, float8); break;
		case 39: entry = effMahojinEntry(int2, float3, float4, float5); break;
		case 40: entry = effNozleEntry(int2, int8, float3, float4, float5, float6, float7); break;
		case 26: entry = effPuniBalloonEntry(int2, float3, float4, float5, float6); break;
		case 45: entry = effKissEntry(int2, float3, float4, float5, float6); break;
		case 43: entry = effSplashEntry(int2, float3, float4, float5, float6); break;
		case 12: entry = effNiceEntry(int2, float3, float4, float5); break;
		case 47: entry = effMistEntry(int2, int7, float3, float4, float5, float6); break;
		case 58: entry = effMist2Entry(int2, int8, float3, float4, float5, float6, float7); break;
		case 56: entry = effScanningEntry(int2, float3, float4, float5); break;
		case 59: entry = effParticleEntry(int2, int9, int10, float3, float4, float5, float6, float7, float8); break;
		case 9: effStardustEntry(int2, int8, int9, float3, float4, float5, float6, float7); break;
		case 44: entry = effTreasureMapEntry(int2, float3, float4, float5); break;
		case 52: entry = effSandarsEntry(int2, float3, float4, float5, (f32)int6); break;
		case 50: entry = effSheepEntry(int2, float3, float4, float5); break;
		case 55: entry = effRankupEntry(int2, float3, float4, float5); break;
		case 54: entry = effIrekaeruEntry(int2, float3, float4, float5); break;
		case 6: entry = effSmallStarEntry(int2, int9, float3, float4, float5, float6, float7, float8); break;
		case 60: entry = effIbukiEntry(int2, int6, float3, float4, float5); break;
		case 61: entry = effMahornEntry(int2, int7, float3, float4, float5, float6, int8); break;
		case 57: entry = effToikiEntry(int2, int6, float3, float4, float5); break;
		case 65: entry = effUranokoEntry(int2, int6, float3, float4, float5); break;
		case 64: entry = effSyuryouEntry(int2, int6, float3, float4, float5); break;
		case 66: entry = effHibashiraEntry(int2, int9, int11, float3, float4, float5, float6, float7, float8, float10); break;
		case 21: entry = effFireEntry(int2, int7, float3, float4, float5, float6); break;
		case 69: entry = effMachinegunEntry(int2, float3, float4, float5, float6, float7, float8); break;
		case 23: entry = effRippleEntry(int2, float3, float4, float5); break;
		case 70: entry = effLasMonEntry(int2, float3, float4, float5); break;
		case 68: entry = effLaserEntry(int2, float3, float4, float5); break;
		case 63: entry = effIndirectEntry(int2, int6, float3, float4, float5); break;
		case 75: entry = effQueenEntry(int2, int7, float3, float4, float5, float6); break;
		case 76: entry = effQueen2Entry(int2, float3, float4, float5); break;
		case 77: entry = effGonbabaBreathEntry(int2, int7, float3, float4, float5, float6); break;
		case 78: entry = effMajinaiEntry(int2, float3, float4, float5); break;
		default:
			break;
	}

	if (entry && effName) {
		if (strcmp(effName, lbl_8041A248)) {
			effSetName(entry, effName);
		}
	}

	return EVT_RETURN_DONE;
}





#pragma push
#pragma optimization_level 1 // retail's register allocation keeps the three args+2 base pointers apart; higher levels coalesce them
// Returning the stored pointer through a helper forces a real register copy (`mr r28,r29`) instead of
// rematerialising `addi r28,r17,8`; at level 1 the bare `fargs = args + 2` is folded.
static inline s32* effIdent(s32** d, s32* p) { return *d = p; }

// NONMATCHING (99.78%): every case, call and argument matches; the 8 remaining instructions are level-1
// allocator/scheduler choices: `lwz r17` is one slot early, the two pointer copies schedule one slot before
// the call's `mr r3,r30`, and the switch keeps the jump target in r3 not r0.
USER_FUNC(evt_eff64) {
	s32* args = event->args;
	s32* iargs;
	s32* fargs;
	s32* rargs;
	const char* effName;
	const char* effSet;
	s32 int2, int3, int4, int5, int6, int7, int8, int9,
		int10, int11, int12, int13, int14, raw3;
	f32 float2, float3, float4, float5, float6, float7, float8,
		float9, float10, float11, float12, float13, float14;
	EffectEntry* effect;

	effName = (const char*)evtGetValue(event, args[0]);
	evtGetValue(event, args[0]);
	evtGetValue(event, args[0]);
	iargs = args + 2;
	effSet = (const char*)evtGetValue(event, args[1]);
	fargs = effIdent(&fargs, iargs);
	evtGetValue(event, args[1]);
	rargs = effIdent(&rargs, iargs);
	evtGetValue(event, args[1]);

	int2 = evtGetValue(event, iargs[0]);
	int3 = evtGetValue(event, iargs[1]);
	int4 = evtGetValue(event, iargs[2]);
	int5 = evtGetValue(event, iargs[3]);
	int6 = evtGetValue(event, iargs[4]);
	int7 = evtGetValue(event, iargs[5]);
	int8 = evtGetValue(event, iargs[6]);
	int9 = evtGetValue(event, iargs[7]);
	int10 = evtGetValue(event, iargs[8]);
	int11 = evtGetValue(event, iargs[9]);
	int12 = evtGetValue(event, iargs[10]);
	int13 = evtGetValue(event, iargs[11]);
	int14 = evtGetValue(event, iargs[12]);

	float2 = evtGetFloat(event, fargs[0]);
	float3 = evtGetFloat(event, fargs[1]);
	float4 = evtGetFloat(event, fargs[2]);
	float5 = evtGetFloat(event, fargs[3]);
	float6 = evtGetFloat(event, fargs[4]);
	float7 = evtGetFloat(event, fargs[5]);
	float8 = evtGetFloat(event, fargs[6]);
	float9 = evtGetFloat(event, fargs[7]);
	float10 = evtGetFloat(event, fargs[8]);
	float11 = evtGetFloat(event, fargs[9]);
	float12 = evtGetFloat(event, fargs[10]);
	float13 = evtGetFloat(event, fargs[11]);
	float14 = evtGetFloat(event, fargs[12]);
	raw3 = rargs[1];
	effect = NULL;
	switch (effGetSetN64(effSet)->id) {
		case 0: effect = effExpBomN64Entry(float2, float3, float4); break;
		case 1: effect = effKemuri1N64Entry(int2, float3, float4, float5, float6); break;
		case 2: effect = effKemuri2N64Entry(int2, float3, float4, float5, float6, float7); break;
		case 3: effect = effKemuri3N64Entry(float2, float3, float4, float5); break;
		case 4: effect = effKemuri4N64Entry(int2, float3, float4, float5, float6, float7); break;
		case 5: effect = effKemuri5N64Entry(float2, float3, float4, float5); break;
		case 6: effect = effKemuri6N64Entry(int2, float3, float4, float5); break;
		case 7: effect = effKemuri7N64Entry(float2, float3, float4, float5, float6); break;
		case 8: effect = effFlowerN64Entry(int2, int6, float3, float4, float5); break;
		case 9: effect = effSnowN64Entry(int5, float2, float3, float5); break;
		case 10: effect = effSstarN64Entry(int2, float3, float4, float5, float6, float7, float8, (f32)int9); break;
		case 11: effect = effStardustN64Entry(int2, float3, float4, float5, float6); break;
		case 12: effect = effMagic1N64Entry(int2, int9, float3, float4, float5, float6, float7, float8); break;
		case 13: effect = effMagic2N64Entry(int2, int7, float3, float4, float5, float6); break;
		case 14: effect = effFallLeafN64Entry(int2, int6, float3, float4, float5); break;
		case 15: effect = effHokoriN64Entry(int2, int6, float3, float4, float5); break;
		case 16: effect = effGarekiN64Entry(int2, float3, float4, float5, float6); break;
		case 17: effect = effKemuri8N64Entry(int2, float3, float4, float5); break;
		case 18: effect = effSmallStarN64Entry(int2, int9, float3, float4, float5, float6, float7, float8); break;
		case 19: effect = effBombN64Entry(int2, float3, float4, float5); break;
		case 20: effect = effWangFlushN64Entry(int2, int6, float3, float4, float5); break;
		case 21: effect = effMagic3N64Entry(int2, int6, float3, float4, float5); break;
		case 22: effect = effKemuri9N64Entry(int2, int7, int9, float3, float4, float5, float6, float8); break;
		case 23: effect = effKemuri10N64Entry(int2, float3, float4, float5, float6, float7, float8, float9); break;
		case 25: effect = effRappaN64Entry(int2, int8, float3, float4, float5, float6, float7); break;
		case 26: effect = effStardust2N64Entry(int2, int8, int9, float3, float4, float5, float6, float7); break;
		case 27: effect = effRippleN64Entry(int2, float3, float4, float5, float6); break;
		case 28: effect = effExplosionN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 29: effect = effKemuri11N64Entry(int2, float3, float4, float5); break;
		case 30: effect = effOnpuN64Entry(int2, float3, float4, float5); break;
		case 31: effect = effKemuri12N64Entry(int2, int7, float3, float4, float5, float6); break;
		case 32: effect = effSweatN64Entry(int2, float3, float4, float5, float6, float7, int8); break;
		case 33: effect = effBalloonN64Entry(int2, float3, float4, float5, float6, float7); break;
		case 34: effect = effCoolLeafN64Entry(int2, float3, float4, float5); break;
		case 35: effect = effBushN64Entry(int2, float3, float4, float5); break;
		case 36: effect = effRotHammerSuccessN64Entry(int2, int6, float3, float4, float5, int7); break;
		case 37: effect = effRotHammerFailN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 38: effect = effGuruguruN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 39: effect = effIceN64Entry(int2, float3, float4, float5); break;
		case 40: effect = effStampN64Entry(int2, float3, float4, float5); break;
		case 41: effect = effThruHammerN64Entry(int2, float3, float4, float5, float6); break;
		case 42: effect = effRadiationN64Entry(int2, float3, float4, float5, float6); break;
		case 43: effect = effThunderN64Entry(int2, float3, float4, float5, float6, float7); break;
		case 44: effect = effBreathFireN64Entry(int2, float3, float4, int9, float5, int10, float6, int11, float7, float8); break;
		case 45: effect = effBrokenBarrierN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 46: effect = effFireRingN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 47: effect = effFireDustN64Entry(int2, int8, int9, float3, float4, float5, float6, float7); break;
		case 48: effect = effFlameN64Entry(int2, float3, float4, float5, float6); break;
		case 49: effect = effBlowN64Entry(int2, int9, float3, float4, float5, float6, float7, float8); break;
		case 50: effect = effBomheiN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 51: effect = effFireFlowerN64Entry(int2, int6, float3, float4, float5); break;
		case 52: effect = effRecoverHeartN64Entry(int2, int6, float3, float4, float5); break;
		case 53: effect = effPturnN64Entry(int2, int6, float3, float4, float5); break;
		case 54: effect = effHitN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 55: effect = effConfettiN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 56: effect = effSnowfallN64Entry(int2, int3); break;
		case 57: effect = effWhirlwindN64Entry(int2, raw3, int5, float4); break;
		case 58: effect = effThunderflareN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 59: effect = effCoinN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 60: effect = effShockN64Entry(int2, float3, float4, float5, float6, float7); break;
		case 61: effect = effBalloonHeihoN64Entry(int2, int8, float3, float4, float5, float6, float7); break;
		case 62: effect = effRubbleN64Entry(int2, int10, float3, float4, float5, float6, float7, float8, float9); break;
		case 63: effMagicHoraoN64Entry(int2, int7, int9, int11, float3, float4, float5, float6, float8, float10); break;
		case 64: effect = effLensflareN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 65: effect = effCrystalN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 66: effect = effButterflyN64Entry(int2, float3, float4, float5); break;
		case 67: effect = effAkariChargeN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 68: effect = effAkariSflushN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 69: effect = effOpukuWgunN64Entry(int2, int10, float3, float4, float5, float6, float7, float8, float9); break;
		case 70: effect = effOpukuWprotectN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 71: effect = effOpukuWcurtainN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 72: effect = effOpukuJetwN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 73: effect = effOpukuJetwrainN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 74: effect = effPokopiThunderN64Entry(int2, int10, float3, float4, float5, float6, float7, float8, float9); break;
		case 75: effect = effWaterDamageN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 76: effect = effSnowmanN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 77: effect = effHolographyN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 78: effect = effTimestampN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 79: effect = effPacknBfireN64Entry(int2, int10, int11, float3, float4, float5, float6, float7, float8, float9); break;
		case 80: effect = effPokopiPipoN64Entry(int2, int10, float3, float4, float5, float6, float7, float8, float9); break;
		case 81: effect = effCloudN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 82: effect = effDogabonAttackN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 83: effect = effKamekiTornadeN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 84: effect = effStoneSmokeN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 85: effect = effPokopiPchargeN64Entry(float3, float4, float5, float6, int2, int7); break;
		// eff_hagetaka_flush_n64.h declares this with 5 parameters, but the original passes six: the caller's asm
		// loads r4 = int7 (int2 in r3, float3..float6 in f1..f4) before the bl. The callee never reads r4, so the
		// header dropped it. Call through the real signature (the same bridge win_main.c uses for the pouch accessors).
		case 86: effect = ((EffectEntry* (*)(s32, s32, f32, f32, f32, f32))effHagetakaFlushN64Entry)(int2, int7, float3, float4, float5, float6); break;
		case 87: effect = effKumokumoN64Entry(int2, int9, float3, float4, float5, float6, float7, float8); break;
		case 88: effect = effVaporN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 89: effect = effFireSparkN64Entry(int2, int9, int10, float3, float4, float5, float6, float7, float8, float11, float12); break;
		case 90: effect = effHoraoSpellN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 91: effect = effSnowDustN64Entry(int2, float3, float4, float5, int9, int10, float6, float7, float8); break;
		case 92: effect = effAkariPoseN64Entry(int2, int7, int8, float3, float4, float5, float6); break;
		case 93: effect = effKumokumoChipN64Entry(int2, float3, float4, float5, float6, float7, float8, float9, float10); break;
		case 94: effect = effFireworksN64Entry(int2, int10, float3, float4, float5, float6, float7, float8, float9); break;
		case 95: effect = effGlassN64Entry(int2, int7, float3, float4, float5, float6); break;
		// eff_booboo_n64.h declares this (void), but the original passes the same six arguments (its prologue
		// ignores them and calls effEntry first), so call through the real signature.
		case 96: effect = ((EffectEntry* (*)(s32, s32, f32, f32, f32, f32))effBoobooN64Entry)(int2, int7, float3, float4, float5, float6); break;
		case 97: effect = effPikkyoloN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 98: effect = effFreezeN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 99: effect = effSunlightN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 100: effect = effReleaseN64Entry(int2, int7, float3, float4, float5, float6); break;
		case 101: effect = effPeachDustN64Entry(int2, float3, float4, float5, float6, (f32)int7); break;
		case 102: effect = effStarPowerN64Entry(int2, int7, float3, float4, float5, float6); break;
		default:
			break;
	}

	if (effect && effName) {
		if (strcmp(effName, lbl_8041A248)) {
			effSetName(effect, effName);
		}
	}
	return EVT_RETURN_DONE;
}
#pragma pop

USER_FUNC(evt_eff_softdelete) {
	effSoftDelete(effNameToPtr((const char*)evtGetValue(event, event->args[0])));
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_eff_delete) {
	effDelete(effNameToPtr((const char*)evtGetValue(event, event->args[0])));
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_eff_softdelete_ptr) {
	effSoftDelete((EffectEntry*)evtGetValue(event, event->args[0]));
	return EVT_RETURN_DONE;
}

USER_FUNC(evt_eff_delete_ptr) {
	effDelete((EffectEntry*)evtGetValue(event, event->args[0]));
	return EVT_RETURN_DONE;
}

// Matching shape from theKidOfArcrania's decomp.me scratch NwWJP (opt_propagation off, the height kept in f2 and
// 2/3 of it computed into f3 before the call, literal 0.0f arguments).
#pragma opt_propagation off
USER_FUNC(evt_eff_fukidashi) {
	s32* args = event->args;
	s32 type;
	const char* effName;
	const char* npcName;
	s32 int3, int4, int10;
	f32 float5, float6, float7, float8, float9;
	EffectEntry* entry;
	f32 f2, f3;

	type = evtGetValue(event, args[0]);
	effName = (const char*)evtGetValue(event, args[1]);
	npcName = (const char*)evtGetValue(event, args[2]);
	int3 = evtGetValue(event, args[3]);
	int4 = evtGetValue(event, args[4]);
	float5 = evtGetFloat(event, args[5]);
	float6 = evtGetFloat(event, args[6]);
	float7 = evtGetFloat(event, args[7]);
	float8 = evtGetFloat(event, args[8]);
	float9 = evtGetFloat(event, args[9]);
	int10 = evtGetValue(event, args[10]);
	entry = NULL;

	switch (type) {
		case 0: {
			f2 = marioGetPtr()->wPlayerCollisionBox.y;
			f3 = (f2 * 2.0f) / 3.0f;
			entry = effFukidashiEntry(int3, int4, -1, int10, 0.0f, f3, 0.0f, f2 / 3.0f, float9);
			break;
		}
		case 1: {
			NpcEntry* npc;
			f32 height;
			if (strcmp(npcName, str_8041A244) == 0) {
				npc = event->ownerNPC;
			} else {
				npc = npcNameToPtr(npcName);
			}
			f2 = (2.0f * npc->height) / 3.0f;
			entry = effFukidashiEntry(int3, int4, (s32)npc, int10, 0.0f, f2, 0.0f, npc->height / 3.0f, float9);
			break;
		}
		case 2:
			entry = effFukidashiEntry(int3, int4, 0, int10, float5, float6, float7, float8, float9);
			break;
		case 3: {
			PartyEntry* party = partyGetPtr(marioGetPartyId());
			if (party) {
				f2 = (2.0f * *(f32*)((u8*)party + 0xF0)) / 3.0f;
				entry = effFukidashiEntry(int3, int4, -2, int10, 0.0f, f2, 0.0f, *(f32*)((u8*)party + 0xF0) / 3.0f, float9);
			}
			break;
		}
		case 4: {
			PartyEntry* party = partyGetPtr(marioGetExtraPartyId());
			if (party) {
				f2 = (2.0f * *(f32*)((u8*)party + 0xF0)) / 3.0f;
				entry = effFukidashiEntry(int3, int4, -3, int10, 0.0f, f2, 0.0f, *(f32*)((u8*)party + 0xF0) / 3.0f, float9);
			}
			break;
		}
	}

	if (effName && strcmp(effName, lbl_8041A248) && entry) {
		effSetName(entry, effName);
	}
	return EVT_RETURN_DONE;
}
#pragma opt_propagation reset
