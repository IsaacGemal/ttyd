#include "eff/eff_fall.h"
#include "driver/animdrv.h"
#include "driver/dispdrv.h"
#include "mario/mariost.h"
#include "memory.h"
#include "pmario_sound.h"
#include "system.h"
#include "eff/n64/eff_kemuri1_n64.h"
#include "eff/n64/eff_water_damage_n64.h"
#include "manager/evtmgr.h"
#include "evt/evt_cam.h"
#include <dolphin/os/OSCache.h>
#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "math_ext.h" // external MSL math ABI; <math.h> pulls in the inline sqrtf/sqrt statics

// First object of this unit's .data (the float pair that used to end the previous unit's .data).
__declspec(section ".data") u8 lbl_803ADD20[8] = { 0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00 };

// Float-pair anchors at .data+0x8..0x40. The original names the local ones `negone_one$374` etc.;
// no code references them, so `force_active` keeps them. The following shibuki tables are reached
// from effFallDispWater through lbl_803ADD20 as the common base register.
#pragma force_active on
__declspec(section ".data") static u32 negone_one_374[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") u32 lbl_803ADD30[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") static u32 one_negone_373[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") u32 lbl_803ADD40[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") static u32 negone_one_360[2] = { 0xBF800000, 0x3F800000 };
__declspec(section ".data") u32 lbl_803ADD50[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") static u32 one_negone_359[2] = { 0x3F800000, 0xBF800000 };
__declspec(section ".data") u32 shibuki_vertex_tbl[64] = {
	0xC94403B2, 0xF600E345, 0x03B2F600, 0xFD4703B2, 0xF6001747, 0x03B2F600,
	0x314903B2, 0xF600BC47, 0x1CF9F600, 0xE03E29DD, 0xF600FD47, 0x2FEFF600,
	0x1A4E29DD, 0xF6003E46, 0x1CF9F600, 0xA5C72D0F, 0xF600D2C7, 0x48A2F600,
	0xFD474FCF, 0xF6002A46, 0x48A2F600, 0x54C62D0F, 0xF600C647, 0x0F30F600,
	0xE2590F52, 0xF600FD47, 0x0F52F600, 0x18330F52, 0xF60033CE, 0x0F52F600,
	0xB4A4226F, 0xF600D9AE, 0x38DCF600, 0xFD473F71, 0xF600227F, 0x39A5F600,
	0x45E8226F, 0xF600CF07, 0x0000F240, 0x01060000, 0xF2403306, 0x0000F240,
	0xCF0706A6, 0xF2400106, 0x0FA6F240, 0x330606A6, 0xF240CF07, 0x134CF240,
	0x01061F4C, 0xF2403306, 0x134CF240, 0x2925174C, 0xF24028C7, 0x0AA6F240,
	0x28E40000, 0xF240D8CB, 0x174CF240, 0xD8DB0AA6, 0xF240D8EA, 0x0000F240,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
__declspec(section ".data") u32 shibuki_texcoord0_tbl[40] = {
	0x00002EF2, 0x100031CD, 0x10003999, 0x00003999, 0x200032DF, 0x20003999,
	0x300031CD, 0x30003999, 0x40002EF2, 0x40003999, 0x00000666, 0x10000666,
	0x10001385, 0x00001750, 0x20000666, 0x2000138B, 0x30000666, 0x300012DE,
	0x40000666, 0x40001750, 0x00002000, 0x10002000, 0x20002000, 0x30002000,
	0x40002000, 0xF36C2000, 0xC0002000, 0xC0004000, 0xF3584000, 0x32E22000,
	0x40004000, 0x33084000, 0xF3800000, 0xC0000000, 0x335B0000, 0x40000000,
	0x00000000, 0x00004000, 0x00000000, 0x00000000,
};
__declspec(section ".data") u32 shibuki_dl_0_0[16] = {
	0x98000A0A, 0x00000A0B, 0x00000B14, 0x00000D15, 0x00020C05, 0x00001406,
	0x0003150F, 0x00000010, 0x00010100, 0x00000301, 0x00000200, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
__declspec(section ".data") u32 shibuki_dl_0_1[16] = {
	0x98000A0B, 0x00000B0C, 0x00000E15, 0x00020C16, 0x00020F06, 0x00031507,
	0x00031610, 0x00010111, 0x00010401, 0x00000202, 0x00000500, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
__declspec(section ".data") u32 shibuki_dl_0_2[16] = {
	0x98000A0C, 0x00000E0D, 0x00001016, 0x00020F17, 0x00021107, 0x00031608,
	0x00031711, 0x00010412, 0x00010602, 0x00000503, 0x00000700, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
__declspec(section ".data") u32 shibuki_dl_0_3[16] = {
	0x98000A0D, 0x0000100E, 0x00001217, 0x00021118, 0x00001308, 0x00031709,
	0x00001812, 0x00010613, 0x00000803, 0x00000704, 0x00000900, 0x00000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000,
};
#pragma force_active reset

extern const f32 float_deg2rad_80420f70;
extern const f32 float_6p2832_80420fa4;
extern const f32 float_360_80420fa8;
extern const f32 float_2p05_80420fac;
extern const f32 float_2p1_80420fb4;
extern const f32 float_10_80420fb0;
extern const f32 float_2_80420f88;
extern const f32 float_0_80420f84;
extern const GXColor dat_80420f5c;
extern const GXColor dat_80420f60;
extern const Vec vec3_802fe46c;
extern const Vec vec3_802fe418;
extern const f32 float_3p1416_8042103c;
extern const f32 float_180_80420fc0;
extern const f32 float_300_80421060;
extern const f32 float_0p2_80421014;
extern const f32 float_0p5_80420f7c;
extern const f32 float_1p2_80421064;
extern const f32 float_1p6_80421068;
extern const f32 float_0p1_8042106c;
extern const f32 float_200_80420fc8;
extern const f32 float_0p6_80421070;
extern const f32 float_1p3_80421074;
extern const f32 float_400_80421078;
extern const f32 float_0p25_80420fa0;
extern const f32 float_30_80420fe0;
extern const f32 float_1_80420f80;
extern const f32 float_64_80420f74;
extern const f32 float_704_80420f78;
extern const f32 float_0p56_80420f8c;
extern const f32 float_0p4_80420f90;
extern const f32 float_0p8_80420f94;
extern const f32 float_0p125_80420f98;
extern const f32 float_0p7_80420f9c;
extern const GXColor dat_80420f64;
extern const GXColor dat_80420f68;
extern const GXColor dat_80420f6c;
extern const GXColor dat_80420f48;
extern const GXColor dat_80420f4c;
extern const GXColor dat_80420f50;
extern const GXColor dat_80420f54;
extern const GXColor dat_80420f58;
extern const f32 float_42p667_80420fb8;
extern const f32 float_0p33333_80420fbc;
extern const f32 float_neg200_80420fc4;
extern const f32 float_neg82p655_80420fcc;
extern const f32 float_88p8_80420fd0;
extern const f32 float_1p03_80420fd4;
extern const f32 float_82p655_80420fd8;
extern const f32 float_neg88p8_80420fdc;
extern const Vec vec3_802fe460;
// effFallMain constants and effect-local rodata (addresses from the target symbol table).
extern const Vec vec3_802fe424;
extern const Vec dat_802fe430[];
extern const f32 float_neg0p3_80420fe4;
extern const f32 float_35_80420fe8;
extern const f32 float_62p832_80420fec;
extern const f32 float_60_80420ff0;
extern const f32 float_16_80420ff4;
extern const f32 float_neg16_80420ff8;
extern const f32 float_128_80420ffc;
extern const f32 float_8p5333_80421000;
extern const f32 float_0p066667_80421004;
extern const f32 float_600_80421008;
extern const f32 float_0p9_8042100c;
extern const f32 float_0p01_80421010;
extern const f32 float_neg0p5_8042101c;
extern const f32 float_430_80421020;
extern const f32 float_neg27_80421024;
extern const f32 float_80_80421028;
extern const f32 float_5_80421030;
extern const f32 float_0p00761_80421034;
extern const f32 float_0p16605_80421038;
extern const f32 float_1p5708_80421040;
extern const f32 float_4p7124_80421044;
extern const f32 float_0p03705_80421048;
extern const f32 float_0p4967_8042104c;
extern const f32 float_40_80421050;
__declspec(section ".sdata2") extern const char str_80421018[];
__declspec(section ".sdata2") extern const char str_8042102C[];
__declspec(section ".sdata2") extern const char str_80421054[];
__declspec(section ".sdata2") extern const char str_80421058[];
extern const char str_802FE488[];
extern const char str_802FE4A0[];
extern const char str_802FE4B8[];
extern const char str_802FE4D4[];
extern const char str_802FE4F0[];
extern const char str_802FE50C[];
extern const char str_802FE52C[];
extern const char str_802FE548[];
extern const char str_802FE554[];
extern const char str_802FE560[];
extern const char str_802FE56C[];
extern const char str_802FE578[];
// .sdata tables and the shake event script. `shibuki_normal_tbl`/`shibuki_color0_tbl` are the
// GXSetArray sources; `evt_shake` is the local event script (the original names it local).
__declspec(section ".sdata") u32 shibuki_normal_tbl[8] = {
	0x00004000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000,
};
__declspec(section ".sdata") u32 shibuki_color0_tbl[8] = {
	0xFFFFFF00, 0xFFFFFF7F, 0xFFFFFF19, 0xFFFFFF4C, 0xB6CCCB33, 0xB6CCCB4C,
	0xB6CCCB99, 0xB6CCCB00,
};
__declspec(section ".sdata") u32 shibuki_dl_1_0[8] = {
	0x9800061F, 0x00072425, 0x0004201C, 0x00071426, 0x00041919, 0x00072527,
	0x00061C00, 0x00000000,
};
__declspec(section ".sdata") u32 shibuki_dl_1_1[8] = {
	0x98000620, 0x00042422, 0x0004221D, 0x00051423, 0x00041D1A, 0x00062524,
	0x00061F00, 0x00000000,
};
__declspec(section ".sdata") u32 shibuki_dl_1_2[8] = {
	0x98000622, 0x00042221, 0x00072323, 0x00041D1E, 0x00071824, 0x00061F1B,
	0x00071E00, 0x00000000,
};
__declspec(section ".sdata") u32 shibuki_dl_1_3[8] = {
	0x98000625, 0x00042020, 0x00042126, 0x0004191D, 0x00051A27, 0x00061C1A,
	0x00061B00, 0x00000000,
};
__declspec(section ".sdata") static u32 evt_shake[8] = {
	0x0005005B, (u32)evt_cam_shake, 0x00000004, 0xF24A7A8A,
	0xF24A7A8A, 0x0000012C, 0x00000002, 0x00000001,
};
const Vec vec3_802fe418 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802fe424 = { 0.0f, 0.0f, 0.0f };
const Vec dat_802fe430[4] = {
	{ 140.0f, 40.0f, -10.0f },
	{ 0.0f, 40.0f, -40.0f },
	{ -140.0f, 40.0f, -10.0f },
	{ -400.0f, 40.0f, -40.0f },
};
const Vec vec3_802fe460 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802fe46c = { 0.0f, 0.0f, 0.0f };
const f64 double_to_int_mask_802fe478 = 4503599627370496.0;
const f64 double_to_int_802fe480 = 4503601774854144.0;
const char str_802FE488[] = "SFX_BTL_STAGE_FALL_BUG1";
const char str_802FE4A0[] = "SFX_BTL_STAGE_FALL_BUG2";
const char str_802FE4B8[] = "SFX_BTL_STAGE_DAMAGE_FORK1";
const char str_802FE4D4[] = "SFX_BTL_STAGE_FALL_KOOPA1";
const char str_802FE4F0[] = "SFX_BTL_STAGE_FALL_METEOR1";
const char str_802FE50C[] = "SFX_BTL_STAGE_DAMAGE_METEOR2";
const char str_802FE52C[] = "SFX_BTL_STAGE_DAMAGE_LIGHT1";
const char str_802FE548[] = "EFF_Baketu";
const char str_802FE554[] = "EFF_Tarai";
const char str_802FE560[] = "EFF_koopa";
const char str_802FE56C[] = "EFF_inseki";
const char str_802FE578[] = "EFF_light";
const char str_802FE584[] = "SFX_BTL_STAGE_FALL_WATER1";


// Full element-0 init, particle loop, and the type 0-8 switch bodies are reconstructed. The ternary is the
// `irand(100) > 50 ? 0 : 180` form (writing `<= 50 ? 180 : 0` compiles to a different select idiom), the vec
// copy stores the zero vector before the x/y/z overwrite, and the sound-name address is hoisted into a
// callee-saved register: the declaration order (cInt, sfx, entry, elem) is what puts entry in r28 and cInt in
// r30 as the original does. `sfx` exists only to force that hoist.
// NONMATCHING (99.22%): the count/type `switch` below (with the redundant `case 0`) reproduces the original's two
// dead `b`s; the one remaining diff is the sound-name address (single `lis r5; addi r29, r5` in the original,
// scratch `addi`+copy or an extra `addi` here).
EffectEntry* effFallEntry(s32 type, f32 x, f32 y, s32 a, f32 z, s32 b, f32 c, f32 d, f32 e) {
	s32 cInt;
#ifdef MUST_MATCH
	// Hoists the sound-name address to a callee-saved register like the original (rule 7):
	const char* sfx;
#endif
	EffectEntry* entry;
	FallWork* elem;
	FallWork* data;
	Vec pos;
	s32 count;
	s32 i;
	f32 angle;
	f32 scale;

	entry = effEntry();
	count = a + 1;
	switch (type) {
	case 0:
		count = a + 1;
		break;
	case 1: case 2: case 3: case 4:
	case 5: case 6: case 7: case 8:
		a = 1;
		count = 2;
		break;
	}
	entry->type = str_80421058;
	entry->count = count;
	data = __memAlloc(HEAP_EFFECT, sizeof(FallWork) * entry->count);
	entry->userdata = data;
	entry->callback = effFallMain;
#ifdef MUST_MATCH
	sfx = (const char*)&vec3_802fe418 + 0x16C;
#endif
	entry->flags |= 2;

	data->type = type;
	data->x = x;
	data->y = y;
	data->z = z;
	data->unk10 = c;
	data->unk14 = d;
	data->unk18 = e;
	data->unk1C = a;
	data->unk24 = 0;
	data->unk28 = 0;
	data->unk20 = b;
	data->unk5C = -1;
	data->unk60 = -1;

	cInt = (s32)c;
	elem = &data[1];
	for (i = 1; i < entry->count; i++, elem++) {
		angle = (f32)irand(360) * float_3p1416_8042103c / float_180_80420fc0;
		scale = (f32)irand(cInt);
		elem->x = scale * (f32)sin(angle);
		elem->y = float_300_80421060 + (f32)irand(entry->count);
		elem->z = scale * (f32)cos(angle);
		elem->unk24 = 0;
		elem->unk28 = 0;
		elem->unk3C = 0xFF;
		elem->unk48 = float_0_80420f84;
		elem->unk18 = (f32)((irand(100) > 50) ? 0 : 180);
		elem->unk38 = 0;
		elem->unk5C = -1;
		elem->unk60 = -1;
		switch (type) {
		case 0:
		case 1:
			elem->unk34 = float_0_80420f84;
			elem->unk30 = float_0p2_80421014;
			if (i == 1) {
				elem->x = float_0_80420f84;
				elem->z = float_0_80420f84;
				elem->unk24 = 0;
			} else {
				elem->unk24 = irand(entry->count) + 0x3c;
			}
			elem->unk2C = (type == 0) ? float_0p5_80420f7c : float_1p2_80421064;
			break;
		case 2:
			elem->unk34 = float_0_80420f84;
			elem->unk30 = float_1p6_80421068;
			elem->x = float_0_80420f84;
			elem->z = float_0_80420f84;
			elem->unk24 = 0;
			elem->unk44 = float_0_80420f84;
			elem->unk2C = float_0p5_80420f7c;
			break;
		case 3:
			elem->unk34 = float_0_80420f84;
			elem->unk30 = float_0p1_8042106c;
			elem->x = float_0_80420f84;
			elem->y = float_200_80420fc8;
			elem->z = float_0_80420f84;
			elem->unk50 = float_0_80420f84;
			elem->unk54 = float_0_80420f84;
			elem->unk58 = float_0_80420f84;
			elem->unk2C = float_1_80420f80;
			pos = vec3_802fe418;
			pos.x = x;
			pos.y = y;
			pos.z = z;
			elem->unk64 = psndSFXOn_3D(
#ifdef MUST_MATCH
			    sfx
#else
			    str_802FE584
#endif
			    , &pos);
			break;
		case 4:
		case 5:
			elem->unk34 = float_0_80420f84;
			elem->unk30 = float_0p6_80421070;
			elem->x = float_0_80420f84;
			elem->z = float_0_80420f84;
			elem->unk24 = 0;
			elem->unk2C = float_1_80420f80;
			break;
		case 6:
			elem->x = float_0_80420f84;
			elem->y = float_200_80420fc8;
			elem->z = float_0_80420f84;
			elem->unk50 = -elem->x / float_30_80420fe0;
			elem->unk54 = -elem->y / float_30_80420fe0;
			elem->unk58 = -elem->z / float_30_80420fe0;
			elem->unk2C = float_1p3_80421074;
			elem->unk48 = float_30_80420fe0;
			elem->unk4C = float_0_80420f84;
			break;
		case 7:
			elem->x = float_400_80421078;
			elem->y = float_200_80420fc8;
			elem->z = float_0_80420f84;
			elem->unk2C = float_0p5_80420f7c;
			break;
		case 8:
			elem->unk34 = float_0_80420f84;
			elem->unk30 = float_0p25_80420fa0;
			elem->x = float_0_80420f84;
			elem->y = float_400_80421078;
			elem->z = float_0_80420f84;
			elem->unk2C = float_1_80420f80;
			break;
		}
	}
	return entry;
}

// Full reconstruction of the element-0 init, the per-element state machines for types 0-8, the type 2
// procedural vertex build, the type 7 hand-rolled sin/cos approximation, and the display registration.
// The switch bodies, call order, type 7 sin/cos polynomial and type 2 vertex build are byte-identical.
#ifdef MUST_MATCH
#define FB(off, sym) ((const char*)&vec3_802fe418 + (off))
#define FV(off, sym) (*(const Vec*)((const u8*)&vec3_802fe418 + (off)))
#else
#define FB(off, sym) (sym)
#define FV(off, sym) (sym)
#endif

// Builds the effect's position vector through a temporary so the zero-vector copy and the x/y/z overwrite
// schedule (and spill) as the original does; plain assignment under #else.
static inline Vec effFallMakePos(f32 x, f32 y, f32 z) {
#ifdef MUST_MATCH
	Vec temp = *(const Vec*)((const u8*)&vec3_802fe418 + 0xC);
#else
	Vec temp = vec3_802fe424;
#endif
	temp.x = x;
	temp.y = y;
	temp.z = z;
	return temp;
}

// Register allocation and the int-to-float pool remain on effFallMain:
//  - ours saves r14 where the target's lowest callee-saved is r15. The cause is the compiler's `(f32)`
//    magic doubles: it pools anonymous `@` copies at .rodata+0x188/0x190 and addresses them through the
//    section base (...rodata.0, r30), while the named `double_to_int_*` objects sit at r31+0x60/0x68. That
//    second base register (r30) is live across the main loop and pushes the peak to 18, so the allocator
//    needs r14; the target reuses the named objects and fits in 17. This is a project-wide MWCC limitation
//    (win_badge/win_item/win_log/animdrv note the same); defining the objects does not stop the duplicate.
//  - the type-2 x-update loop body is byte-identical (do-while with an explicit `--n` countdown and a
//    4-vertex body, strength-reduced to one pointer stepped by 0x50), except that ours lowers the
//    countdown to `subic.; bne` where the target uses `mtctr`/`bdnz`.
//  - the remaining string addresses (0x130/0x13c/0x148/0x154/0x160) are base-relative but via the section
//    base r30, not r31, for the same reason; 0x13c/0x148/0x160 are the named `str_802FE5*` symbols.
// `#include "math_ext.h"` above (external MSL math ABI, no inline `sqrtf`/`sqrt` bodies) instead of
// `<math.h>` removes the four weak-local sqrt statics from .rodata and makes the unit's .rodata layout
// match the target exactly (vec3s, doubles at 0x60/0x68, strings at 0x70+). With the old `<math.h>`
// include the statics shifted everything by 0x30 and capped the function at 96.37%.
// The sfx88/sfx_d8 locals and the five named str_802FE5* references (0x130/0x13c/0x148/0x154/0x160)
// are the best of a 3^9 search over each string's
// FB/local/named mode; the declaration order is MWCC's default. Earlier fixes: block-scoping the type-7
// sine/cosine locals (one per branch; p00761/p16605 and p03705/p4967 in their own blocks), the effFallMakePos
// temp, and the do-while type-2 countdown (94.29% -> 96.37%).
// NONMATCHING (98.19%): every call/constant/switch body is byte-identical; the rest is register
// colouring (ours saves r14, target starts at r15, driven by the duplicate i2f pool base), the
// `subic.`/`bdnz` countdown and a handful of schedule slots.
void effFallMain(EffectEntry* effect) {
	FallWork* data = effect->userdata;
	FallWork* elem;
	Vec pos;
	s32 inBattle;
	s32 type;
	s32 unk20;
	s32 i;
	s32 flag = 0;
	s32 done;
	f32 y;
	f32 f30;
	f32 f29;
	f32 unk10;
	f32 floor;
	f32 f26;
	f32 unk18;

	pos = effFallMakePos(data->x, data->y, data->z);

	type = data->type;
	unk10 = data->unk10;
	y = data->y;
	floor = y + data->unk14;
	unk18 = data->unk18;
	inBattle = gp->inBattle != 0;
	unk20 = data->unk20;

	if (effect->flags & 4) {
		effect->flags &= ~4;
		for (i = 0, elem = effect->userdata; i < effect->count; i++, elem++) {
			if (elem->unk5C != -1) {
				animPoseRelease(elem->unk5C);
			}
			if (elem->unk60 != -1) {
				animPoseRelease(elem->unk60);
			}
			psndSFXOff(elem->unk64);
		}
		effDelete(effect);
		return;
	}

	f30 = float_6p2832_80420fa4 * unk18;
	f29 = float_30_80420fe0 + y;

	{
	const char* sfx88 = FB(0x88, str_802FE4A0);
	const char* sfxD8 = FB(0xD8, str_802FE4F0);
	done = 0;
	for (i = 1, elem = &data[1]; i < effect->count; i++, elem++) {
		switch (type) {
		case 0:
		case 1:
			switch (elem->unk38) {
			case 0:
				if (elem->unk24 != 0) {
					elem->unk24--;
				}
				if (elem->unk24 == 0) {
					elem->unk38++;
					if (type == 1) {
						psndSFXOn_3D(FB(0x70, str_802FE488), &pos);
					} else if (i == 1) {
						psndSFXOn_3D(sfx88, &pos);
					}
				}
				break;
			case 1:
				if (type == 0 && i > 1 && flag == 0 && gp->retraceCount % 6 == 0) {
					psndSFXOn_3D(sfx88, &pos);
					flag = 1;
				}
				elem->y += elem->unk34;
				elem->unk34 -= elem->unk30;
				if (distABf(elem->x, elem->z, float_0_80420f84, float_0_80420f84) < unk10 * float_0p5_80420f7c) {
					if (elem->y < floor) {
						elem->y = floor;
						elem->unk34 *= float_neg0p3_80420fe4;
						elem->unk38++;
					}
				} else {
					if (elem->y < y) {
						elem->y = y;
						elem->unk34 *= float_neg0p3_80420fe4;
						elem->unk38++;
					}
				}
				break;
			case 2:
				elem->y += elem->unk34;
				elem->unk34 -= elem->unk30;
				f26 = float_6p2832_80420fa4 * angleABf(float_0_80420f84, float_0_80420f84, elem->x, elem->z) / float_360_80420fa8;
				if (i == 1) {
					f26 = f30 / float_360_80420fa8;
				}
				elem->x += float_0p5_80420f7c * (f32)sin(f26);
				elem->z += float_0p5_80420f7c * -(f32)cos(f26);
				if (elem->y < y) {
					elem->y = y;
					elem->unk24 = 0;
					elem->unk38++;
				}
				break;
			case 3:
				elem->unk24++;
				if (elem->unk24 > 30) {
					elem->unk38++;
				}
				break;
			case 4:
				elem->unk3C -= 10;
				if (elem->unk3C < 0) {
					elem->unk3C = 0;
					elem->unk38++;
				}
				break;
			case 5:
				done++;
				break;
			}
			if ((gp->retraceCount >> 2) & 1) {
				elem->unk28++;
			}
			if (elem->unk28 >= 3) {
				elem->unk28 = 0;
			}
			elem->unk48 = float_35_80420fe8 * (f32)sin(float_0p5_80420f7c * (f32)gp->retraceCount + (f32)i);
			break;
		case 2:
			switch (elem->unk38) {
			case 0:
				elem->y += elem->unk34;
				elem->unk34 -= elem->unk30;
				if (elem->y < y) {
					elem->y = y;
					elem->unk24 = 0;
					elem->unk38++;
					psndSFXOn_3D(FB(0xA0, str_802FE4B8), &pos);
				}
				break;
			case 1:
				elem->unk44 = (float_60_80420ff0 - (f32)elem->unk24) *
				                  (f32)sin(float_62p832_80420fec * (f32)elem->unk24 / float_60_80420ff0) /
				                  float_60_80420ff0;
				elem->unk24++;
				if (elem->unk24 > 60) {
					elem->unk38++;
				}
				break;
			case 2:
				elem->unk24++;
				if (elem->unk24 > 30) {
					elem->unk38++;
				}
				break;
			case 3:
				elem->unk3C -= 10;
				if (elem->unk3C < 0) {
					elem->unk3C = 0;
					elem->unk38++;
				}
				break;
			case 4:
				done++;
				break;
			}
			{
				f32* v;
				s32 j;
				s32 n;
				*(smartEntry**)((u8*)elem + 0x40) = smartAlloc(0x280, 3);
				v = (*(smartEntry**)((u8*)elem + 0x40))->address;
				for (j = 0; j < 0x20; j++) {
					v[j * 5 + 0] = (j & 1) ? float_16_80420ff4 : float_neg16_80420ff8;
					v[j * 5 + 1] = float_128_80420ffc - float_8p5333_80421000 * (f32)(j / 2);
					v[j * 5 + 2] = float_0_80420f84;
					v[j * 5 + 3] = (j & 1) ? float_2_80420f88 : float_0_80420f84;
					v[j * 5 + 4] = float_0p066667_80421004 * (f32)(j / 2);
				}
				v = (*(smartEntry**)((u8*)elem + 0x40))->address;
				i = 0;
				n = 8;
				do {
					v[i * 5 + 0] += elem->unk44 * ((v[i * 5 + 1] * v[i * 5 + 1]) / float_600_80421008);
					v[i * 5 + 5] +=
					    elem->unk44 * ((v[i * 5 + 6] * v[i * 5 + 6]) / float_600_80421008);
					v[i * 5 + 10] +=
					    elem->unk44 * ((v[i * 5 + 11] * v[i * 5 + 11]) / float_600_80421008);
					v[i * 5 + 15] +=
					    elem->unk44 * ((v[i * 5 + 16] * v[i * 5 + 16]) / float_600_80421008);
					i += 4;
				} while (--n != 0);
				DCFlushRange((*(smartEntry**)((u8*)elem + 0x40))->address, 0x280);
			}
			break;
		case 3:
			switch (elem->unk38) {
			case 0:
				elem->y += elem->unk34;
				elem->unk34 -= elem->unk30;
				if (elem->y < y) {
					elem->y = y;
					elem->unk24 = 0;
					elem->unk38++;
				}
				break;
			case 1:
				if (elem->unk24 % 10 == 0) {
					effWaterDamageN64Entry(4, 20, pos.x, pos.y, pos.z, float_1_80420f80);
				}
				elem->unk24++;
				if (elem->unk24 > unk20) {
					elem->unk38++;
				}
				break;
			case 2:
				elem->unk3C = (s32)((f32)elem->unk3C * float_0p9_8042100c);
				elem->unk2C *= float_0p9_8042100c;
				if (elem->unk2C < float_0p01_80421010) {
					psndSFXOff(elem->unk64);
					effDelete(effect);
					return;
				}
				break;
			}
			elem->unk54 -= float_0p2_80421014;
			break;
		case 4:
		case 5:
			switch (elem->unk38) {
			case 0:
				if (type == 5) {
					if (!animGroupBaseAsync(str_802FE548, inBattle, NULL)) {
						break;
					}
					if (elem->unk5C == -1) {
						elem->unk5C = animPoseEntry(str_802FE548, inBattle);
						animPoseSetAnim(elem->unk5C, str_80421018, 1);
					}
				} else {
					if (!animGroupBaseAsync(str_802FE554, inBattle, NULL)) {
						break;
					}
					if (elem->unk5C == -1) {
						elem->unk5C = animPoseEntry(str_802FE554, inBattle);
						animPoseSetAnim(elem->unk5C, str_80421018, 1);
					}
				}
				if (elem->unk24 != 0) {
					elem->unk24--;
				}
				if (elem->unk24 == 0) {
					elem->unk38++;
				}
				break;
			case 1:
				elem->y += elem->unk34;
				elem->unk34 -= elem->unk30;
				if (distABf(elem->x, elem->z, float_0_80420f84, float_0_80420f84) < unk10 * float_0p5_80420f7c) {
					if (elem->y < floor) {
						elem->y = floor;
						elem->unk34 *= float_neg0p3_80420fe4;
						elem->unk38++;
					}
				} else {
					if (elem->y < y) {
						elem->y = y;
						elem->unk34 *= float_neg0p3_80420fe4;
						elem->unk38++;
					}
				}
				break;
			case 2:
				elem->y += elem->unk34;
				elem->unk34 -= elem->unk30;
				f26 = float_6p2832_80420fa4 * angleABf(float_0_80420f84, float_0_80420f84, elem->x, elem->z) / float_360_80420fa8;
				if (i == 1) {
					f26 = f30 / float_360_80420fa8;
				}
				elem->x += float_2_80420f88 * (f32)sin(f26);
				elem->z += float_2_80420f88 * -(f32)cos(f26);
				if (elem->y < y) {
					elem->y = y;
					elem->unk34 = float_neg0p5_8042101c * elem->unk34;
					elem->y = y;
					elem->unk24 = 0;
					if (elem->unk34 < float_1_80420f80) {
						elem->unk38++;
					}
				}
				break;
			case 3:
				elem->unk24++;
				if (elem->unk24 > 30) {
					elem->unk38++;
				}
				break;
			case 4:
				elem->unk3C -= 10;
				if (elem->unk3C < 0) {
					elem->unk3C = 0;
					elem->unk38++;
					animPoseRelease(elem->unk5C);
					elem->unk5C = -1;
				}
				break;
			case 5:
				done++;
				break;
			}
			switch (type) {
			case 4:
				if (elem->unk38 > 1) {
					elem->unk48 += (float_360_80420fa8 - elem->unk48) / float_10_80420fb0;
				}
				break;
			case 5:
				if (elem->unk38 > 1) {
					elem->unk48 += (float_430_80421020 - elem->unk48) / float_10_80420fb0;
				}
				break;
			}
			if (elem->unk5C != -1) {
				animPoseMain(elem->unk5C);
			}
			break;
		case 6:
			switch (elem->unk38) {
			case 0:
				if (!animGroupBaseAsync(str_802FE560, inBattle, NULL)) {
					break;
				}
				if (elem->unk5C == -1) {
					elem->unk5C = animPoseEntry(str_802FE560, inBattle);
					animPoseSetAnim(elem->unk5C, str_80421018, 1);
					psndSFXOn_3D(FB(0xBC, str_802FE4D4), &pos);
				}
				elem->x += elem->unk50;
				elem->y += elem->unk54;
				elem->z += elem->unk58;
				elem->unk24++;
				if (elem->unk24 > 30) {
					elem->unk24 = 0;
					elem->unk38++;
					effKemuri1N64Entry(4, pos.x + elem->x, pos.y + elem->y, pos.z + elem->z, float_0_80420f84);
					evtEntry(evt_shake, 0, 0);
				}
				break;
			case 1:
				elem->unk48 = intplGetValue(0xb, elem->unk24, 0xb4, float_30_80420fe0, float_neg27_80421024);
				if (elem->unk48 < float_0_80420f84) {
					elem->unk4C = elem->unk48;
					elem->unk48 = float_0_80420f84;
				}
				elem->unk24++;
				if (elem->unk24 > 0xb4) {
					elem->unk24 = 0;
					elem->unk38++;
				}
				if (elem->unk24 == 0x64) {
					psndSFXOn_3D(FB(0xBC, str_802FE4D4), &pos);
				}
				break;
			case 2:
				elem->unk48 = intplGetValue(0xb, elem->unk24, 0xc8, float_neg27_80421024, float_80_80421028);
				if (elem->unk48 < float_0_80420f84) {
					elem->unk4C = elem->unk48;
					elem->unk48 = float_0_80420f84;
				}
				elem->unk24++;
				if (elem->unk24 > 0xc8) {
					animPoseRelease(elem->unk5C);
					elem->unk5C = -1;
					done++;
				}
				if (elem->unk24 == 0x46) {
					psndSFXOn_3D(FB(0xBC, str_802FE4D4), &pos);
				}
				break;
			}
			if (elem->unk5C != -1) {
				animPoseMain(elem->unk5C);
			}
			break;
		case 7:
			switch (elem->unk38) {
			case 0:
				if (!animGroupBaseAsync(str_802FE56C, inBattle, NULL)) {
					break;
				}
				psndSFXOn_3D(sfxD8, &pos);
				if (elem->unk5C == -1) {
					elem->unk5C = animPoseEntry(str_802FE56C, inBattle);
					animPoseSetAnim(elem->unk5C, str_80421018, 1);
				}
				if (elem->unk60 == -1) {
					elem->unk60 = animPoseEntry(str_802FE56C, inBattle);
					animPoseSetAnim(elem->unk60, str_8042102C, 1);
				}
				/* fallthrough */
			case 2:
			case 4:
			case 6: {
				Vec tbl[4];
#ifdef MUST_MATCH
				tbl[0] = *(const Vec*)((const u8*)&vec3_802fe418 + 0x18);
				tbl[1] = *(const Vec*)((const u8*)&vec3_802fe418 + 0x24);
				tbl[2] = *(const Vec*)((const u8*)&vec3_802fe418 + 0x30);
				tbl[3] = *(const Vec*)((const u8*)&vec3_802fe418 + 0x3C);
#else
				tbl[0] = dat_802fe430[0];
				tbl[1] = dat_802fe430[1];
				tbl[2] = dat_802fe430[2];
				tbl[3] = dat_802fe430[3];
#endif
				elem->unk50 = tbl[elem->unk38 / 2].x;
				elem->unk54 = tbl[elem->unk38 / 2].y;
				elem->unk58 = tbl[elem->unk38 / 2].z;
				elem->unk30 = float_0p4_80420f90;
				elem->unk24 = (s32)(distABf(elem->unk50, elem->unk58, elem->x, elem->z) / float_5_80421030);
				elem->unk34 = (elem->unk54 - elem->y) / (f32)elem->unk24 +
				              float_0p5_80420f7c * (elem->unk30 * (f32)elem->unk24);
				elem->unk18 = angleABf(elem->x, elem->z, elem->unk50, elem->unk58);
				elem->unk48 = float_0_80420f84;
				elem->unk38++;
				break;
			}
			case 1:
			case 3:
			case 5:
			case 7: {
				f32 t, s, c, t2, nc;
				elem->y += elem->unk34;
				elem->unk34 -= elem->unk30;
				{
				f32 p00761 = float_0p00761_80421034, p16605 = float_0p16605_80421038;
				f32 one = float_1_80420f80;
				t = float_6p2832_80420fa4 * elem->unk18 / float_360_80420fa8;
				if (t <= float_3p1416_8042103c) {
					if (t <= float_1p5708_80421040) {
						t2 = t * t;
						s = ((p00761 * t2 - p16605) * t2 +
						     one) *
						    t;
					} else {
						t = float_1p5708_80421040 - (t - float_1p5708_80421040);
						t2 = t * t;
						s = ((p00761 * t2 - p16605) * t2 +
						     one) *
						    t;
					}
				} else {
					if (t < float_4p7124_80421044) {
						t = t - float_3p1416_8042103c;
						t2 = t * t;
						s = -(((p00761 * t2 - p16605) * t2 +
						       one) *
						      t);
					} else {
						t = float_1p5708_80421040 - (t - float_4p7124_80421044);
						t2 = t * t;
						s = -(((p00761 * t2 - p16605) * t2 +
						       one) *
						      t);
					}
				}
				}
				elem->x += float_5_80421030 * s;
				{
				f32 p03705 = float_0p03705_80421048, p4967 = float_0p4967_8042104c;
				f32 one = float_1_80420f80;
				t = elem->unk18 * float_6p2832_80420fa4 / float_360_80420fa8;
				if (t <= float_3p1416_8042103c) {
					if (t <= float_1p5708_80421040) {
						t2 = t * t;
						c = (p03705 * t2 - p4967) * t2 +
						    one;
					} else {
						t = float_1p5708_80421040 - (t - float_1p5708_80421040);
						t2 = t * t;
						nc = (p03705 * t2 - p4967) * t2 + one;
						c = -nc;
					}
				} else {
					if (t < float_4p7124_80421044) {
						t = t - float_3p1416_8042103c;
						t2 = t * t;
						nc = (p03705 * t2 - p4967) * t2 + one;
						c = -nc;
					} else {
						t = float_1p5708_80421040 - (t - float_4p7124_80421044);
						t2 = t * t;
						c = (p03705 * t2 - p4967) * t2 +
						    one;
					}
				}
				}
				elem->z += float_5_80421030 * -c;
				if (--elem->unk24 < 0) {
					elem->x = elem->unk50;
					elem->y = elem->unk54;
					elem->z = elem->unk58;
					elem->unk38++;
					if (elem->unk38 == 2) {
						elem->unk64 = psndSFXOn_3D(FB(0xF4, str_802FE50C), &pos);
					}
					if (elem->unk38 != 8) {
						effKemuri1N64Entry(4, pos.x + elem->x, pos.y + elem->y - float_40_80421050,
						                   pos.z + elem->z, float_0_80420f84);
						evtEntry(evt_shake, 0, 0);
					}
				}
				elem->unk48 += (float_360_80420fa8 - elem->unk48) / float_30_80420fe0;
				break;
			}
			case 8:
				psndSFXOff(elem->unk64);
				animPoseRelease(elem->unk5C);
				animPoseRelease(elem->unk60);
				elem->unk5C = -1;
				elem->unk60 = -1;
				elem->unk38++;
				break;
			case 9:
				done++;
				break;
			}
			if (elem->unk5C != -1) {
				animPoseMain(elem->unk5C);
			}
			if (elem->unk60 != -1) {
				animPoseMain(elem->unk60);
			}
			break;
		case 8:
			switch (elem->unk38) {
			case 0:
				if (!animGroupBaseAsync(str_802FE578, inBattle, NULL)) {
					break;
				}
				elem->unk5C = animPoseEntry(str_802FE578, inBattle);
				animPoseSetAnim(elem->unk5C, str_80421018, 1);
				elem->unk38++;
				break;
			case 1:
			case 2:
				elem->y += elem->unk34;
				elem->unk34 -= elem->unk30;
				if (elem->y < y) {
					elem->y = y;
					elem->unk24 = 0;
					elem->unk38++;
				}
				if (elem->unk38 == 1 && elem->y < f29) {
					effKemuri1N64Entry(4, pos.x + elem->x, pos.y + y, pos.z + elem->z, float_0_80420f84);
					evtEntry(evt_shake, 0, 0);
					animPoseSetAnim(elem->unk5C, str_80421054, 1);
					elem->unk38++;
					psndSFXOn_3D(FB(0x114, str_802FE52C), &pos);
				}
				break;
			case 3:
				if (animPoseGetLoopTimes(elem->unk5C) < float_1_80420f80) {
					break;
				}
				elem->unk24++;
				if (elem->unk24 > elem->unk20) {
					elem->unk38++;
				}
				break;
			case 4:
				elem->unk3C -= 10;
				if (elem->unk3C < 0) {
					elem->unk3C = 0;
					elem->unk38++;
				}
				break;
			case 5:
				animPoseRelease(elem->unk5C);
				elem->unk5C = -1;
				elem->unk38++;
				break;
			case 6:
				done++;
				break;
			}
			if (elem->unk5C != -1) {
				animPoseMain(elem->unk5C);
			}
			break;
		}
	}

	if (done >= effect->count - 1) {
		effDelete(effect);
		return;
	}

	switch (type) {
	case 3:
		dispEntry(CAMERA_3D, dispCalcZ(&pos), 2, effFallDispWater, effect);
		break;
	case 4:
	case 5:
		dispEntry(CAMERA_3D, dispCalcZ(&pos), 0, effFallDisp, effect);
		break;
	case 7:
		dispEntry(CAMERA_3D, dispCalcZ(&pos), 2, effFallDispInseki, effect);
		break;
	default:
		dispEntry(CAMERA_3D, dispCalcZ(&pos), 2, effFallDisp, effect);
		break;
	}
	}
}

// Builds the type-8 light colour and the case 0/1 / case 2 material colours. Returning the colour by
// value (rather than assigning a base and then `color.a`) is what puts the colour temp and its
// outgoing argument copy in the target's stack slots; `effFallColA`'s (alpha, base) parameter order is
// what lands the base word / alpha byte in the target's r5 / r0. MUST_MATCH builds them, otherwise the
// natural assignment at each site is used.
#ifdef MUST_MATCH
static inline GXColor effFallCol8(u8 a) { GXColor c = dat_80420f58; c.a = a; return c; }
static inline GXColor effFallColA(u8 a, GXColor c) { c.a = a; return c; }
#endif

// GX display for the default/koopa/light types: a per-type textured quad, the procedural type-2 strip,
// the type 4/5 animation halo, the type 6 koopa body and the type 8 light pose.
// The viewYaw product, the case-0/1 half-angle products and the two GXChanMat colours go through
// explicit locals under MUST_MATCH: `rad`, `w` and `neg29`/`pos29` put the compiler's temporaries in the
// target's FPRs, and the colour helpers put them in its GPRs/stack slots. The natural expressions sit
// under each #else.
void effFallDisp(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	FallWork* elem = effect->userdata;
	CameraEntry* cam;
	CameraEntry* camView;
	GXTexObj texObj;
	Mtx mtxPos, mtxScale, mtxRot, mtxView;
	s32 i;
	s32 type;
	f32 f31;
	f32 f30;
	f32 f29;
	f32 f28;
#ifdef MUST_MATCH
	f32 rad;
#endif

	camView = camGetPtr(cameraId);
	type = elem->type;
	f30 = elem->unk18;
	PSMTXTrans(mtxPos, elem->x, elem->y, elem->z);
	cam = camGetPtr(cameraId);
#ifdef MUST_MATCH
	rad = -cam->viewYaw;
	rad = float_deg2rad_80420f70 * rad;
	PSMTXRotRad(mtxRot, 'y', rad);
#else
	PSMTXRotRad(mtxRot, 'y', -cam->viewYaw * float_deg2rad_80420f70);
#endif
	PSMTXConcat(mtxPos, mtxRot, mtxView);

	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
	GXSetNumTevStages(1);
	GXSetTevOrder(0, 0, 0, 4);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevOp(0, 0);
	GXSetCullMode(0);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xd, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xd, 1, 4, 0);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);

	switch (type) {
	case 0:
	case 1:
		effGetTexObj(0x32, &texObj);
		GXLoadTexObj(&texObj, 0);
		break;
	case 2:
		effGetTexObj(0x33, &texObj);
		GXLoadTexObj(&texObj, 0);
		break;
	}

	f31 = float_6p2832_80420fa4 * f30;
	for (i = 1, elem = &elem[1]; i < effect->count; i++, elem++) {
		switch (type) {
		case 0:
		case 1: {
#ifndef MUST_MATCH
			GXColor color;
#endif
#ifdef MUST_MATCH
			f32 w;
			f32 neg29;
			f32 pos29;
#endif
			f29 = float_64_80420f74;
			f28 = float_42p667_80420fb8;
			f29 = f29 * float_0p5_80420f7c;
			f28 = f28 * float_0p5_80420f7c;
			PSMTXTrans(mtxPos, elem->x, elem->y, elem->z);
			PSMTXRotRad(mtxRot, 'y', float_deg2rad_80420f70 * elem->unk18);
			PSMTXConcat(mtxPos, mtxRot, mtxPos);
#ifdef MUST_MATCH
			w = f28 * float_0p5_80420f7c;
			PSMTXTrans(mtxRot, float_0_80420f84, w * elem->unk2C, float_0_80420f84);
#else
			PSMTXTrans(mtxRot, float_0_80420f84, f28 * 0.5f * elem->unk2C, float_0_80420f84);
#endif
			PSMTXConcat(mtxPos, mtxRot, mtxPos);
			PSMTXRotRad(mtxRot, 'z', float_deg2rad_80420f70 * elem->unk48);
			PSMTXConcat(mtxPos, mtxRot, mtxPos);
#ifdef MUST_MATCH
			w = -f28 * float_0p5_80420f7c;
			PSMTXTrans(mtxRot, float_0_80420f84, w * elem->unk2C, float_0_80420f84);
#else
			PSMTXTrans(mtxRot, float_0_80420f84, -f28 * 0.5f * elem->unk2C, float_0_80420f84);
#endif
			PSMTXConcat(mtxPos, mtxRot, mtxPos);
			PSMTXScale(mtxScale, elem->unk2C, elem->unk2C, elem->unk2C);
			PSMTXConcat(mtxPos, mtxScale, mtxPos);
			PSMTXConcat(mtxView, mtxPos, mtxPos);
			PSMTXConcat(camView->view, mtxPos, mtxPos);
			GXLoadPosMtxImm(mtxPos, 0);
			GXSetCurrentMtx(0);
#ifdef MUST_MATCH
			GXSetChanMatColor(4, effFallColA(elem->unk3C, dat_80420f48));
#else
			color = dat_80420f48;
			color.a = elem->unk3C;
			GXSetChanMatColor(4, color);
#endif
			PSMTXScale(mtxScale, float_1_80420f80, float_0p33333_80420fbc, float_1_80420f80);
			PSMTXTrans(mtxPos, float_0_80420f84, (f32)elem->unk28, float_0_80420f84);
			PSMTXConcat(mtxScale, mtxPos, mtxPos);
			GXLoadTexMtxImm(mtxPos, 0x1e, 1);
			GXBegin(0x80, 0, 4);
#ifdef MUST_MATCH
			neg29 = -f29;
			neg29 = neg29 * float_0p5_80420f7c;
			pos29 = f29 * float_0p5_80420f7c;
			GXPosition3f32(neg29, f28, 0.0f);
#else
			GXPosition3f32(-f29 * 0.5f, f28, 0.0f);
#endif
			GXTexCoord2f32(0.0f, 0.0f);
#ifdef MUST_MATCH
			GXPosition3f32(pos29, f28, 0.0f);
#else
			GXPosition3f32(f29 * 0.5f, f28, 0.0f);
#endif
			GXTexCoord2f32(1.0f, 0.0f);
#ifdef MUST_MATCH
			GXPosition3f32(pos29, 0.0f, 0.0f);
#else
			GXPosition3f32(f29 * 0.5f, 0.0f, 0.0f);
#endif
			GXTexCoord2f32(1.0f, 1.0f);
#ifdef MUST_MATCH
			GXPosition3f32(neg29, 0.0f, 0.0f);
#else
			GXPosition3f32(-f29 * 0.5f, 0.0f, 0.0f);
#endif
			GXTexCoord2f32(0.0f, 1.0f);
			break;
		}
		case 2: {
#ifndef MUST_MATCH
			GXColor color;
#endif
			s32 j;
			f32* v;
			PSMTXTrans(mtxPos, elem->x, elem->y, elem->z);
			PSMTXScale(mtxScale, elem->unk2C, elem->unk2C, elem->unk2C);
			PSMTXConcat(mtxPos, mtxScale, mtxPos);
			PSMTXConcat(mtxView, mtxPos, mtxPos);
			PSMTXConcat(camView->view, mtxPos, mtxPos);
			GXLoadPosMtxImm(mtxPos, 0);
			GXSetCurrentMtx(0);
#ifdef MUST_MATCH
			GXSetChanMatColor(4, effFallColA(elem->unk3C, dat_80420f4c));
#else
			color = dat_80420f4c;
			color.a = elem->unk3C;
			GXSetChanMatColor(4, color);
#endif
			PSMTXIdentity(mtxPos);
			GXLoadTexMtxImm(mtxPos, 0x1e, 1);
			for (j = 0; j < 0x1e; j += 2) {
				v = (f32*)(*(smartEntry**)((u8*)elem + 0x40))->address + j * 5;
				GXBegin(0x80, 0, 4);
				GXPosition3f32(v[0], v[1], v[2]);
				GXTexCoord2f32(v[3], v[4]);
				GXPosition3f32(v[10], v[11], v[12]);
				GXTexCoord2f32(v[13], v[14]);
				GXPosition3f32(v[15], v[16], v[17]);
				GXTexCoord2f32(v[18], v[19]);
				GXPosition3f32(v[5], v[6], v[7]);
				GXTexCoord2f32(v[8], v[9]);
			}
			break;
		}
		case 3:
		case 7:
			break;
		case 4:
		case 5: {
			Vec axis;
			Vec tmpAxis;
			f28 = f31 / float_360_80420fa8;
			tmpAxis = vec3_802fe460;
			tmpAxis.x = (f32)cos(f28);
			tmpAxis.z = (f32)sin(f28);
			axis = tmpAxis;
			PSMTXTrans(mtxPos, elem->x, elem->y, elem->z);
			PSMTXScale(mtxScale, elem->unk2C, elem->unk2C, elem->unk2C);
			PSMTXConcat(mtxPos, mtxScale, mtxPos);
			PSMTXTrans(mtxScale, 0.0f, (f32)(type == 4 ? 8 : 16), 0.0f);
			PSMTXConcat(mtxPos, mtxScale, mtxPos);
			PSMTXRotAxisRad(mtxRot, &axis, float_deg2rad_80420f70 * -elem->unk48);
			PSMTXConcat(mtxPos, mtxRot, mtxPos);
			PSMTXTrans(mtxScale, 0.0f, (f32)-(type == 4 ? 8 : 16), 0.0f);
			PSMTXConcat(mtxPos, mtxScale, mtxPos);
			PSMTXConcat(mtxView, mtxPos, mtxPos);
			if (elem->unk5C != -1) {
				animPoseDrawMtx(elem->unk5C, mtxPos, 1, float_0_80420f84, float_10_80420fb0);
				animPoseDrawMtx(elem->unk5C, mtxPos, 2, float_0_80420f84, float_10_80420fb0);
				animPoseDrawMtx(elem->unk5C, mtxPos, 3, float_0_80420f84, float_10_80420fb0);
			}
			break;
		}
		case 6: {
			GXColor color;
			if (elem->unk5C != -1) {
				PSMTXTrans(mtxPos, elem->x, elem->y, elem->z);
				if (f30 < float_180_80420fc0) {
					PSMTXScale(mtxScale, elem->unk2C, elem->unk2C, elem->unk2C);
				} else {
					PSMTXScale(mtxScale, -elem->unk2C, elem->unk2C, elem->unk2C);
				}
				PSMTXConcat(mtxPos, mtxScale, mtxPos);
				if (elem->unk48 != float_0_80420f84) {
					PSMTXRotRad(mtxRot, 'z', float_deg2rad_80420f70 * -elem->unk48);
					PSMTXConcat(mtxPos, mtxRot, mtxPos);
				} else {
					PSMTXTrans(mtxScale, float_neg200_80420fc4, float_0_80420f84, float_0_80420f84);
					PSMTXConcat(mtxPos, mtxScale, mtxPos);
					PSMTXRotRad(mtxRot, 'z', float_deg2rad_80420f70 * -elem->unk4C);
					PSMTXConcat(mtxPos, mtxRot, mtxPos);
					PSMTXTrans(mtxScale, float_200_80420fc8, float_0_80420f84, float_0_80420f84);
					PSMTXConcat(mtxPos, mtxScale, mtxPos);
				}
				PSMTXConcat(mtxView, mtxPos, mtxPos);
				PSMTXTrans(mtxScale, float_neg82p655_80420fcc, float_88p8_80420fd0, float_0_80420f84);
				PSMTXConcat(mtxPos, mtxScale, mtxScale);
				PSMTXScale(mtxRot, float_1p03_80420fd4, float_1p03_80420fd4, float_1p03_80420fd4);
				PSMTXConcat(mtxScale, mtxRot, mtxRot);
				PSMTXTrans(mtxScale, float_82p655_80420fd8, float_neg88p8_80420fdc, float_0_80420f84);
				PSMTXConcat(mtxRot, mtxScale, mtxScale);
				animPoseSetMaterialFlagOn(elem->unk5C, 0x40);
				color = dat_80420f50;
				animPoseSetMaterialEvtColor(elem->unk5C, color);
				animPoseDrawMtx(elem->unk5C, mtxScale, 1, float_0_80420f84, float_10_80420fb0);
				animPoseDrawMtx(elem->unk5C, mtxScale, 2, float_0_80420f84, float_10_80420fb0);
				animPoseDrawMtx(elem->unk5C, mtxScale, 3, float_0_80420f84, float_10_80420fb0);
				animPoseSetMaterialFlagOn(elem->unk5C, 0x40);
				color = dat_80420f54;
				animPoseSetMaterialEvtColor(elem->unk5C, color);
				animPoseDrawMtx(elem->unk5C, mtxPos, 1, float_0_80420f84, float_10_80420fb0);
				animPoseDrawMtx(elem->unk5C, mtxPos, 2, float_0_80420f84, float_10_80420fb0);
				animPoseDrawMtx(elem->unk5C, mtxPos, 3, float_0_80420f84, float_10_80420fb0);
			}
			break;
		}
		case 8: {
			GXColor color;
			if (elem->unk5C != -1) {
				PSMTXTrans(mtxPos, elem->x, elem->y, elem->z);
				PSMTXScale(mtxScale, elem->unk2C, elem->unk2C, elem->unk2C);
				PSMTXConcat(mtxPos, mtxScale, mtxPos);
				PSMTXConcat(mtxView, mtxPos, mtxPos);
				animPoseSetMaterialFlagOn(elem->unk5C, 0x40);
#ifdef MUST_MATCH
				color = effFallCol8(elem->unk3C);
#else
				color = dat_80420f58;
				color.a = elem->unk3C;
#endif
				animPoseSetMaterialEvtColor(elem->unk5C, color);
				animPoseDrawMtx(elem->unk5C, mtxPos, 1, float_0_80420f84, float_10_80420fb0);
				animPoseDrawMtx(elem->unk5C, mtxPos, 2, float_0_80420f84, float_10_80420fb0);
				animPoseDrawMtx(elem->unk5C, mtxPos, 3, float_0_80420f84, float_10_80420fb0);
			}
			break;
		}
		}
	}
}

void effFallDispInseki(CameraId cameraId, void* param) {
#ifdef MUST_MATCH
	f32 rad;
#endif
	EffectEntry* effect = param;
	FallWork* work = effect->userdata;
	CameraEntry* cam;
	Mtx mtxA, mtxD, mtxB, mtxC;
	Vec axis;
	GXColor color;
	s32 i;

	camGetPtr(cameraId);
	PSMTXTrans(mtxA, work->x, work->y, work->z);
	cam = camGetPtr(cameraId);
#ifdef MUST_MATCH
	rad = -cam->viewYaw;
	rad = float_deg2rad_80420f70 * rad;
	PSMTXRotRad(mtxB, 'y', rad);
#else
	PSMTXRotRad(mtxB, 'y', -cam->viewYaw * float_deg2rad_80420f70);
#endif
	PSMTXConcat(mtxA, mtxB, mtxC);

	if (work[1].unk5C != -1 && work[1].unk60 != -1) {
		Vec tmpAxis;
		tmpAxis = vec3_802fe46c;
		tmpAxis.x = (f32)cos(float_6p2832_80420fa4 * work[1].unk18 / float_360_80420fa8);
		tmpAxis.z = (f32)sin(float_6p2832_80420fa4 * work[1].unk18 / float_360_80420fa8);
		axis = tmpAxis;

		for (i = 0; i < 2; i++) {
			if (i == 0) {
				PSMTXTrans(mtxA, work[1].x, work[1].y, work[1].z);
				PSMTXRotAxisRad(mtxB, &axis, float_deg2rad_80420f70 * -work[1].unk48);
				PSMTXScale(mtxD, float_2p05_80420fac * work[1].unk2C, float_2p05_80420fac * work[1].unk2C,
				           float_2p05_80420fac * work[1].unk2C);
				PSMTXConcat(mtxC, mtxA, mtxA);
				PSMTXConcat(mtxA, mtxB, mtxA);
				PSMTXConcat(mtxA, mtxD, mtxA);
				animPoseSetMaterialFlagOn(work[1].unk5C, 0x40);
				color = dat_80420f5c;
				animPoseSetMaterialEvtColor(work[1].unk5C, color);
			} else {
				PSMTXTrans(mtxA, work[1].x, work[1].y, work[1].z);
				PSMTXRotAxisRad(mtxB, &axis, float_deg2rad_80420f70 * -work[1].unk48);
				PSMTXScale(mtxD, float_2_80420f88 * work[1].unk2C, float_2_80420f88 * work[1].unk2C,
				           float_2_80420f88 * work[1].unk2C);
				PSMTXConcat(mtxC, mtxA, mtxA);
				PSMTXConcat(mtxA, mtxB, mtxA);
				PSMTXConcat(mtxA, mtxD, mtxA);
				animPoseSetMaterialFlagOn(work[1].unk5C, 0x40);
				color = dat_80420f60;
				animPoseSetMaterialEvtColor(work[1].unk5C, color);
			}
			animPoseDrawMtx(work[1].unk5C, mtxA, 1, float_0_80420f84, float_10_80420fb0);
			animPoseDrawMtx(work[1].unk5C, mtxA, 2, float_0_80420f84, float_10_80420fb0);
			animPoseDrawMtx(work[1].unk5C, mtxA, 3, float_0_80420f84, float_10_80420fb0);
		}

		PSMTXTrans(mtxA, work[1].x, work[1].y, work[1].z);
		PSMTXScale(mtxD, float_2p1_80420fb4 * work[1].unk2C, float_2p1_80420fb4 * work[1].unk2C,
		           float_2p1_80420fb4 * work[1].unk2C);
		PSMTXConcat(mtxC, mtxA, mtxA);
		PSMTXConcat(mtxA, mtxD, mtxA);
		animPoseDrawMtx(work[1].unk60, mtxA, 1, float_0_80420f84, float_10_80420fb0);
		animPoseDrawMtx(work[1].unk60, mtxA, 2, float_0_80420f84, float_10_80420fb0);
		animPoseDrawMtx(work[1].unk60, mtxA, 3, float_0_80420f84, float_10_80420fb0);
	}
}

// Full GX state, quad display list and both shibuki splash passes. The quad uses float literals and the
// splash pointer is re-read from effect->userdata. `rad`/`nf31`/`half` and the effFallCol68 inline are
// match-only spellings for the register-allocation clusters. `half` is deliberately non-const: a
// literal folds and the compiler commutes `x * 0.5f` to `0.5f * x`, breaking the quad's operand order.
#ifdef MUST_MATCH
static inline GXColor effFallCol68(GXColor c, u8 a) { c.a = a; return c; }
#endif

void effFallDispWater(CameraId cameraId, void* param) {
	EffectEntry* effect = param;
	FallWork* work = effect->userdata;
	CameraEntry* cam;
	CameraEntry* camView;
	GXColor color;
	GXTexObj texObj;
	Mtx mtxPos, mtxScale, mtxRot, mtxView;
	u8* base;
	u16 height;
	f32 f31;
	f32 f30;
#ifdef MUST_MATCH
	f32 rad;
	f32 nf31;
	f32 half;
#endif
	base = lbl_803ADD20;
	camView = camGetPtr(cameraId);
	PSMTXTrans(mtxPos, work->x, work->y, work->z);
	cam = camGetPtr(cameraId);
#ifdef MUST_MATCH
	rad = -cam->viewYaw;
	rad = float_deg2rad_80420f70 * rad;
	PSMTXRotRad(mtxRot, 'y', rad);
#else
	PSMTXRotRad(mtxRot, 'y', -cam->viewYaw * float_deg2rad_80420f70);
#endif
	PSMTXConcat(mtxPos, mtxRot, mtxView);
	PSMTXConcat(camView->view, mtxView, mtxView);

	GXSetNumTevStages(2);
	GXSetTevOrder(0, 0, 0, 4);
	GXSetTevColorOp(0, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
	GXSetTevOp(0, 0);
	GXSetTevOrder(1, 0xff, 0xff, 0xff);
	GXSetTevColorOp(1, 0, 0, 0, 1, 0);
	GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
	GXSetTevColorIn(1, 0xf, 2, 0, 0xf);
	GXSetTevAlphaIn(1, 7, 1, 0, 7);
	GXSetNumTexGens(1);
	GXSetTexCoordGen2(0, 1, 4, 0x1e, 0, 0x7d);
	GXSetNumChans(1);
	GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
	GXSetCullMode(2);
	GXClearVtxDesc();
	GXSetVtxDesc(9, 1);
	GXSetVtxDesc(0xb, 1);
	GXSetVtxDesc(0xd, 1);
	GXSetVtxAttrFmt(0, 9, 1, 4, 0);
	GXSetVtxAttrFmt(0, 0xb, 1, 5, 0);
	GXSetVtxAttrFmt(0, 0xd, 1, 4, 0);
	effGetTexObj(0x35, &texObj);
	GXLoadTexObj(&texObj, 0);

	f31 = float_64_80420f74;
	f30 = float_704_80420f78;
	f31 = f31 * float_0p5_80420f7c;
	f30 = f30 * float_0p5_80420f7c;
	PSMTXTrans(mtxPos, work[1].unk50, work[1].unk54, work[1].unk58);
#ifdef MUST_MATCH
	PSMTXScale(mtxScale, float_1_80420f80, f30 / (f32)GXGetTexObjHeight(&texObj), float_1_80420f80);
#else
	height = GXGetTexObjHeight(&texObj);
	PSMTXScale(mtxScale, float_1_80420f80, f30 / (f32)height, float_1_80420f80);
#endif
	PSMTXConcat(mtxPos, mtxScale, mtxPos);
	GXLoadTexMtxImm(mtxPos, 0x1e, 1);
	PSMTXTrans(mtxPos, work[1].x, work[1].y, work[1].z);
	PSMTXScale(mtxScale, work[1].unk2C, float_1_80420f80, float_1_80420f80);
	PSMTXConcat(mtxPos, mtxScale, mtxPos);
	PSMTXConcat(mtxView, mtxPos, mtxPos);
	GXLoadPosMtxImm(mtxPos, 0);
	GXSetTevColor(1, dat_80420f64);
#ifdef MUST_MATCH
	nf31 = -f31;
	half = 0.5f;
#endif
	GXBegin(0x80, 0, 4);
#ifdef MUST_MATCH
	GXPosition3f32(nf31 * half, f30, 0.0f);
#else
	GXPosition3f32(0.5f * -f31, f30, 0.0f);
#endif
	GXColor4u8(0xff, 0xff, 0xff, 0xff);
	GXTexCoord2f32(0.0f, 0.0f);
#ifdef MUST_MATCH
	GXPosition3f32(f31 * half, f30, 0.0f);
#else
	GXPosition3f32(f31 * 0.5f, f30, 0.0f);
#endif
	GXColor4u8(0xff, 0xff, 0xff, 0xff);
	GXTexCoord2f32(2.0f, 0.0f);
#ifdef MUST_MATCH
	GXPosition3f32(f31 * half, 0.0f, 0.0f);
#else
	GXPosition3f32(f31 * 0.5f, 0.0f, 0.0f);
#endif
	GXColor4u8(0xff, 0xff, 0xff, 0x00);
	GXTexCoord2f32(2.0f, 1.0f);
	GXPosition3f32(0.5f * -f31, 0.0f, 0.0f);
	GXColor4u8(0xff, 0xff, 0xff, 0x00);
	GXTexCoord2f32(0.0f, 1.0f);

	work = effect->userdata;
	if (work[1].unk38 > 0) {
		PSMTXTrans(mtxPos, work[1].x, work[1].y, work[1].z);
		PSMTXScale(mtxScale, float_0p56_80420f8c, float_0p4_80420f90, float_0p8_80420f94);
		PSMTXConcat(mtxView, mtxPos, mtxPos);
		PSMTXConcat(mtxPos, mtxScale, mtxPos);
		GXLoadPosMtxImm(mtxPos, 0);
		PSMTXTrans(mtxPos, work[1].unk54 * float_0p125_80420f98, float_0_80420f84, float_0_80420f84);
		GXLoadTexMtxImm(mtxPos, 0x1e, 1);
#ifdef MUST_MATCH
		GXSetTevColor(1, effFallCol68(dat_80420f68, work[1].unk3C));
#else
		color = dat_80420f68;
		color.a = work[1].unk3C;
		GXSetTevColor(1, color);
#endif
		effGetTexObj(0x37, &texObj);
		GXLoadTexObj(&texObj, 0);
		GXClearVtxDesc();
		GXSetVtxDesc(9, 2);
		GXSetVtxAttrFmt(0, 9, 1, 3, 8);
		GXSetArray(9, base + 0x40, 6);
		GXSetVtxDesc(0xa, 2);
		GXSetVtxAttrFmt(0, 0xa, 0, 1, 6);
		GXSetArray(0xa, shibuki_normal_tbl, 3);
		GXSetVtxDesc(0xb, 2);
		GXSetVtxAttrFmt(0, 0xb, 1, 5, 0);
		GXSetArray(0xb, shibuki_color0_tbl, 4);
		GXSetVtxDesc(0xd, 2);
		GXSetVtxAttrFmt(0, 0xd, 1, 3, 0xe);
		GXSetArray(0xd, base + 0x140, 4);
		GXCallDisplayList(shibuki_dl_1_0, 0x20);
		GXCallDisplayList(shibuki_dl_1_1, 0x20);
		GXCallDisplayList(shibuki_dl_1_2, 0x20);
		GXCallDisplayList(shibuki_dl_1_3, 0x20);
		PSMTXTrans(mtxPos, work[1].x, work[1].y, work[1].z);
		PSMTXScale(mtxScale, float_0p7_80420f9c * (work[1].unk2C * float_0p8_80420f94), float_0p5_80420f7c * (work[1].unk2C * float_0p8_80420f94), work[1].unk2C * float_0p8_80420f94);
		PSMTXConcat(mtxView, mtxPos, mtxPos);
		PSMTXConcat(mtxPos, mtxScale, mtxPos);
		GXLoadPosMtxImm(mtxPos, 0);
		PSMTXTrans(mtxPos, float_0_80420f84, -work[1].unk54 * float_0p25_80420fa0, float_0_80420f84);
		GXLoadTexMtxImm(mtxPos, 0x1e, 1);
		GXSetTevColor(1, dat_80420f6c);
		effGetTexObj(0x36, &texObj);
		GXLoadTexObj(&texObj, 0);
		GXClearVtxDesc();
		GXSetVtxDesc(9, 2);
		GXSetVtxAttrFmt(0, 9, 1, 3, 8);
		GXSetArray(9, base + 0x40, 6);
		GXSetVtxDesc(0xa, 2);
		GXSetVtxAttrFmt(0, 0xa, 0, 1, 6);
		GXSetArray(0xa, shibuki_normal_tbl, 3);
		GXSetVtxDesc(0xb, 2);
		GXSetVtxAttrFmt(0, 0xb, 1, 5, 0);
		GXSetArray(0xb, shibuki_color0_tbl, 4);
		GXSetVtxDesc(0xd, 2);
		GXSetVtxAttrFmt(0, 0xd, 1, 3, 0xe);
		GXSetArray(0xd, base + 0x140, 4);
		GXCallDisplayList(base + 0x1e0, 0x40);
		GXCallDisplayList(base + 0x220, 0x40);
		GXCallDisplayList(base + 0x260, 0x40);
		GXCallDisplayList(base + 0x2a0, 0x40);
	}
}

s32 effFallCheckHit(EffectEntry* effect) {
	FallWork* work = effect->userdata;

	switch (work->type) {
	case 0:
	case 1:
	case 4:
	case 5:
		if (work[1].unk38 >= 2) {
			return 1;
		}
		break;
	case 2:
	case 3:
		if (work[1].unk38 >= 1) {
			return 1;
		}
		break;
	case 6:
		if (work[1].unk38 < 2) {
			if (work[1].unk38 != 1) {
				break;
			}
			if (work[1].unk24 <= 90) {
				break;
			}
		}
		return 1;
	case 7:
		if (work[1].unk38 >= 4) {
			return 1;
		}
		break;
	case 8:
		if (work[1].unk38 >= 3) {
			return 1;
		}
		break;
	}
	return 0;
}

// 4-byte zero word that ends the unit's .data (after the compiler's switch jump tables): an all-zero
// object needs explicit_zero_data to stay in .data and force_active to survive the linker.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".data") u32 gap_05_803AE094_data[1] = { 0 };
#pragma force_active reset
#pragma explicit_zero_data reset

// .sdata2 objects, in the original address order. Defined after the code so the compiler does not
// fold their values into the functions; the code loads each by name.
const GXColor dat_80420f48 = { 0xFF, 0xFF, 0xFF, 0x00 };
const GXColor dat_80420f4c = { 0xFF, 0xFF, 0xFF, 0x00 };
const GXColor dat_80420f50 = { 0x00, 0x00, 0x00, 0xFF };
const GXColor dat_80420f54 = { 0xFF, 0xFF, 0xFF, 0xFF };
const GXColor dat_80420f58 = { 0xFF, 0xFF, 0xFF, 0x00 };
const GXColor dat_80420f5c = { 0x00, 0x00, 0x00, 0xFF };
const GXColor dat_80420f60 = { 0xFF, 0xFF, 0xFF, 0xFF };
const GXColor dat_80420f64 = { 0xFF, 0xFF, 0xFF, 0xFF };
const GXColor dat_80420f68 = { 0xFF, 0xFF, 0xFF, 0x00 };
const GXColor dat_80420f6c = { 0xFF, 0xFF, 0xFF, 0xFF };
const f32 float_deg2rad_80420f70 = 0.017453292f;
const f32 float_64_80420f74 = 64.0f;
const f32 float_704_80420f78 = 704.0f;
const f32 float_0p5_80420f7c = 0.5f;
const f32 float_1_80420f80 = 1.0f;
const f32 float_0_80420f84 = 0.0f;
const f32 float_2_80420f88 = 2.0f;
const f32 float_0p56_80420f8c = 0.56f;
const f32 float_0p4_80420f90 = 0.4f;
const f32 float_0p8_80420f94 = 0.8f;
const f32 float_0p125_80420f98 = 0.125f;
const f32 float_0p7_80420f9c = 0.7f;
const f32 float_0p25_80420fa0 = 0.25f;
const f32 float_6p2832_80420fa4 = 6.2831855f;
const f32 float_360_80420fa8 = 360.0f;
const f32 float_2p05_80420fac = 2.05f;
const f32 float_10_80420fb0 = 10.0f;
const f32 float_2p1_80420fb4 = 2.1f;
const f32 float_42p667_80420fb8 = 42.666668f;
const f32 float_0p33333_80420fbc = 0.33333334f;
const f32 float_180_80420fc0 = 180.0f;
const f32 float_neg200_80420fc4 = -200.0f;
const f32 float_200_80420fc8 = 200.0f;
const f32 float_neg82p655_80420fcc = -82.655f;
const f32 float_88p8_80420fd0 = 88.8f;
const f32 float_1p03_80420fd4 = 1.03f;
const f32 float_82p655_80420fd8 = 82.655f;
const f32 float_neg88p8_80420fdc = -88.8f;
const f32 float_30_80420fe0 = 30.0f;
const f32 float_neg0p3_80420fe4 = -0.3f;
const f32 float_35_80420fe8 = 35.0f;
const f32 float_62p832_80420fec = 62.831856f;
const f32 float_60_80420ff0 = 60.0f;
const f32 float_16_80420ff4 = 16.0f;
const f32 float_neg16_80420ff8 = -16.0f;
const f32 float_128_80420ffc = 128.0f;
const f32 float_8p5333_80421000 = 8.533334f;
const f32 float_0p066667_80421004 = 0.06666667f;
const f32 float_600_80421008 = 600.0f;
const f32 float_0p9_8042100c = 0.9f;
const f32 float_0p01_80421010 = 0.01f;
const f32 float_0p2_80421014 = 0.2f;
__declspec(section ".sdata2") const char str_80421018[4] = "Z_1";
const f32 float_neg0p5_8042101c = -0.5f;
const f32 float_430_80421020 = 430.0f;
const f32 float_neg27_80421024 = -27.0f;
const f32 float_80_80421028 = 80.0f;
__declspec(section ".sdata2") const char str_8042102C[4] = "Z_2";
const f32 float_5_80421030 = 5.0f;
const f32 float_0p00761_80421034 = 0.00761f;
const f32 float_0p16605_80421038 = 0.16605f;
const f32 float_3p1416_8042103c = 3.1415927f;
const f32 float_1p5708_80421040 = 1.5707964f;
const f32 float_4p7124_80421044 = 4.712389f;
const f32 float_0p03705_80421048 = 0.03705f;
const f32 float_0p4967_8042104c = 0.4967f;
const f32 float_40_80421050 = 40.0f;
__declspec(section ".sdata2") const char str_80421054[4] = "A_1";
__declspec(section ".sdata2") const char str_80421058[8] = "Fall";
const f32 float_300_80421060 = 300.0f;
const f32 float_1p2_80421064 = 1.2f;
const f32 float_1p6_80421068 = 1.6f;
const f32 float_0p1_8042106c = 0.1f;
const f32 float_0p6_80421070 = 0.6f;
const f32 float_1p3_80421074 = 1.3f;
const f32 float_400_80421078 = 400.0f;
