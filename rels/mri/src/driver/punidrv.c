#include "punidrv.h"
#include "evt/evt_cmd.h"
#include "forcedata.h"

#pragma pool_data off

#pragma force_active on
static void compare(void);  // TODO: real signature
static void camGetRy(void);  // TODO: real signature

// clang-format off
extend_data(72,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(73,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
static u32 sintbl[450] = {
    0x00000000, 0x3C8EF77F, 0x3D0EF241, 0x3D565E46,
    0x3D8EDC3C, 0x3DB27ED8, 0x3DD612C7, 0x3DF99674,
    0x3E0E835E, 0x3E20303C, 0x3E31D0C8, 0x3E43636F,
    0x3E54E6E2, 0x3E66598E, 0x3E77BA67, 0x3E8483ED,
    0x3E8D204B, 0x3E95B1C8, 0x3E9E377A, 0x3EA6B0D9,
    0x3EAF1D3F, 0x3EB77C03, 0x3EBFCC7D, 0x3EC80DE5,
    0x3ED03FD5, 0x3ED86163, 0x3EE0722A, 0x3EE87161,
    0x3EF05E81, 0x3EF83904, 0x3F000000, 0x3F03D988,
    0x3F07A8C6, 0x3F0B6D76, 0x3F0F2746, 0x3F12D5E0,
    0x3F167914, 0x3F1A108C, 0x3F1D9BF6, 0x3F211B1E,
    0x3F248DC1, 0x3F27F37C, 0x3F2B4C2B, 0x3F2E976C,
    0x3F31D51B, 0x3F3504F7, 0x3F3826AB, 0x3F3B3A04,
    0x3F3E3EC0, 0x3F41349C, 0x3F441B76, 0x3F46F30A,
    0x3F49BB17, 0x3F4C7358, 0x3F4F1BBD, 0x3F51B3F2,
    0x3F543BC5, 0x3F56B314, 0x3F5919AC, 0x3F5B6F4C,
    0x3F5DB3D0, 0x3F5FE719, 0x3F6208D1, 0x3F6418F8,
    0x3F66175D, 0x3F6803CD, 0x3F69DE16, 0x3F6BA637,
    0x3F6D5BEE, 0x3F6EFF19, 0x3F708FB8, 0x3F720D78,
    0x3F737868, 0x3F74D068, 0x3F761544, 0x3F7746ED,
    0x3F786552, 0x3F797050, 0x3F7A67E8, 0x3F7B4BE8,
    0x3F7C1C61, 0x3F7CD91F, 0x3F7D8234, 0x3F7E177F,
    0x3F7E98FE, 0x3F7F06A3, 0x3F7F605B, 0x3F7FA637,
    0x3F7FD817, 0x3F7FF60A, 0x3F800000, 0x3F7FF60A,
    0x3F7FD817, 0x3F7FA637, 0x3F7F605B, 0x3F7F06A3,
    0x3F7E98FE, 0x3F7E177F, 0x3F7D8234, 0x3F7CD91F,
    0x3F7C1C61, 0x3F7B4BE8, 0x3F7A67E8, 0x3F797050,
    0x3F786552, 0x3F7746ED, 0x3F761544, 0x3F74D068,
    0x3F737879, 0x3F720D88, 0x3F708FB8, 0x3F6EFF2A,
    0x3F6D5BEE, 0x3F6BA637, 0x3F69DE27, 0x3F6803CD,
    0x3F66175D, 0x3F641909, 0x3F6208E1, 0x3F5FE719,
    0x3F5DB3E1, 0x3F5B6F5D, 0x3F5919AC, 0x3F56B325,
    0x3F543BD6, 0x3F51B3F2, 0x3F4F1BBD, 0x3F4C7369,
    0x3F49BB17, 0x3F46F30A, 0x3F441B86, 0x3F4134AD,
    0x3F3E3EC0, 0x3F3B3A04, 0x3F3826AB, 0x3F3504F7,
    0x3F31D52C, 0x3F2E977D, 0x3F2B4C2B, 0x3F27F37C,
    0x3F248DC1, 0x3F211B2E, 0x3F1D9C06, 0x3F1A108C,
    0x3F167925, 0x3F12D5F1, 0x3F0F2746, 0x3F0B6D76,
    0x3F07A8D6, 0x3F03D999, 0x3F000000, 0x3EF83904,
    0x3EF05EA2, 0x3EE87182, 0x3EE0724B, 0x3ED86184,
    0x3ED03FD5, 0x3EC80E06, 0x3EBFCC7D, 0x3EB77C24,
    0x3EAF1D60, 0x3EA6B0FB, 0x3E9E377A, 0x3E95B1C8,
    0x3E8D206D, 0x3E84840E, 0x3E77BAAA, 0x3E6659D1,
    0x3E54E6E2, 0x3E43636F, 0x3E31D10B, 0x3E20307F,
    0x3E0E83A1, 0x3DF996FB, 0x3DD6134D, 0x3DB27ED8,
    0x3D8EDCC2, 0x3D565F52, 0x3D0EF34D, 0x3C8EF998,
    0x358637BD, 0xBC8EF77F, 0xBD0EF241, 0xBD565D3A,
    0xBD8EDC3C, 0xBDB27E52, 0xBDD612C7, 0xBDF99674,
    0xBE0E835E, 0xBE20303C, 0xBE31D0C8, 0xBE43632C,
    0xBE54E69F, 0xBE66594B, 0xBE77BA23, 0xBE8483ED,
    0xBE8D204B, 0xBE95B1A7, 0xBE9E3758, 0xBEA6B0D9,
    0xBEAF1D1D, 0xBEB77BE1, 0xBEBFCC5C, 0xBEC80DC3,
    0xBED03FB3, 0xBED86163, 0xBEE0722A, 0xBEE87161,
    0xBEF05E81, 0xBEF838E3, 0xBEFFFFDE, 0xBF03D977,
    0xBF07A8C6, 0xBF0B6D76, 0xBF0F2735, 0xBF12D5E0,
    0xBF167914, 0xBF1A107B, 0xBF1D9BF6, 0xBF211B1E,
    0xBF248DB0, 0xBF27F36B, 0xBF2B4C1B, 0xBF2E976C,
    0xBF31D51B, 0xBF3504E6, 0xBF38269A, 0xBF3B39F3,
    0xBF3E3EAF, 0xBF41349C, 0xBF441B76, 0xBF46F2FA,
    0xBF49BB06, 0xBF4C7358, 0xBF4F1BAC, 0xBF51B3F2,
    0xBF543BC5, 0xBF56B314, 0xBF5919AC, 0xBF5B6F4C,
    0xBF5DB3D0, 0xBF5FE708, 0xBF6208D1, 0xBF6418F8,
    0xBF66175D, 0xBF6803BC, 0xBF69DE16, 0xBF6BA626,
    0xBF6D5BDD, 0xBF6EFF19, 0xBF708FA8, 0xBF720D78,
    0xBF737868, 0xBF74D057, 0xBF761533, 0xBF7746ED,
    0xBF786541, 0xBF797050, 0xBF7A67D7, 0xBF7B4BE8,
    0xBF7C1C61, 0xBF7CD91F, 0xBF7D8234, 0xBF7E177F,
    0xBF7E98FE, 0xBF7F06A3, 0xBF7F605B, 0xBF7FA626,
    0xBF7FD817, 0xBF7FF60A, 0xBF800000, 0xBF7FF60A,
    0xBF7FD817, 0xBF7FA637, 0xBF7F605B, 0xBF7F06A3,
    0xBF7E98FE, 0xBF7E177F, 0xBF7D8234, 0xBF7CD91F,
    0xBF7C1C61, 0xBF7B4BE8, 0xBF7A67E8, 0xBF797050,
    0xBF786552, 0xBF7746ED, 0xBF761544, 0xBF74D068,
    0xBF737879, 0xBF720D88, 0xBF708FB8, 0xBF6EFF2A,
    0xBF6D5BEE, 0xBF6BA637, 0xBF69DE27, 0xBF6803CD,
    0xBF66176E, 0xBF641909, 0xBF6208E1, 0xBF5FE719,
    0xBF5DB3E1, 0xBF5B6F5D, 0xBF5919BD, 0xBF56B325,
    0xBF543BD6, 0xBF51B403, 0xBF4F1BCE, 0xBF4C7369,
    0xBF49BB17, 0xBF46F31B, 0xBF441B86, 0xBF4134AD,
    0xBF3E3ED1, 0xBF3B3A04, 0xBF3826BB, 0xBF350508,
    0xBF31D52C, 0xBF2E977D, 0xBF2B4C3C, 0xBF27F38C,
    0xBF248DC1, 0xBF211B2E, 0xBF1D9C06, 0xBF1A109D,
    0xBF167925, 0xBF12D5F1, 0xBF0F2757, 0xBF0B6D87,
    0xBF07A8D6, 0xBF03D999, 0xBF000011, 0xBEF83926,
    0xBEF05EC4, 0xBEE87182, 0xBEE0724B, 0xBED86184,
    0xBED03FD5, 0xBEC80E06, 0xBEBFCC9F, 0xBEB77C24,
    0xBEAF1D82, 0xBEA6B0FB, 0xBE9E379B, 0xBE95B1EA,
    0xBE8D206D, 0xBE84840E, 0xBE77BAAA, 0xBE6659D1,
    0xBE54E725, 0xBE4363B2, 0xBE31D10B, 0xBE20307F,
    0xBE0E83E4, 0xBDF99781, 0xBDD613D3, 0xBDB27F5F,
    0xBD8EDD48, 0xBD565F52, 0xBD0EF45A, 0xBC8EFBB1,
    0xB58637BD, 0x3C8EF566, 0x3D0EF241, 0x3D565D3A,
    0x3D8EDBB6, 0x3DB27DCC, 0x3DD61240, 0x3DF995EE,
    0x3E0E831B, 0x3E202FF9, 0x3E31D085, 0x3E43632C,
    0x3E54E65C, 0x3E66594B, 0x3E77BA23, 0x3E8483CB,
    0x3E8D2029, 0x3E95B1A7, 0x3E9E3737, 0x3EA6B0B8,
    0x3EAF1D1D, 0x3EB77BE1, 0x3EBFCC3A, 0x3EC80DC3,
    0x3ED03FB3, 0x3ED86141, 0x3EE07208, 0x3EE87161,
    0x3EF05E81, 0x3EF838E3, 0x3EFFFFDE, 0x3F03D977,
    0x3F07A8B5, 0x3F0B6D65, 0x3F0F2724, 0x3F12D5D0,
    0x3F167903, 0x3F1A107B, 0x3F1D9BF6, 0x3F211B0D,
    0x3F248DB0, 0x3F27F36B, 0x3F2B4C1B, 0x3F2E975B,
    0x3F31D50B, 0x3F3504D5, 0x3F38269A, 0x3F3B39F3,
    0x3F3E3EAF, 0x3F41349C, 0x3F441B76, 0x3F46F2FA,
    0x3F49BB06, 0x3F4C7358, 0x3F4F1BAC, 0x3F51B3E1,
    0x3F543BC5, 0x3F56B314, 0x3F59199C, 0x3F5B6F3B,
    0x3F5DB3D0, 0x3F5FE708, 0x3F6208D1, 0x3F6418F8,
    0x3F66174C, 0x3F6803BC, 0x3F69DE16, 0x3F6BA626,
    0x3F6D5BDD, 0x3F6EFF19, 0x3F708FA8, 0x3F720D78,
    0x3F737868, 0x3F74D057, 0x3F761533, 0x3F7746DC,
    0x3F786541, 0x3F797050, 0x3F7A67D7, 0x3F7B4BE8,
    0x3F7C1C61, 0x3F7CD91F, 0x3F7D8234, 0x3F7E177F,
    0x3F7E98FE, 0x3F7F06A3, 0x3F7F605B, 0x3F7FA626,
    0x3F7FD817, 0x3F7FF60A,
};

// TODO: Normals$480 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u32 Normals$480[6] = {
//     0x00000000, 0x00000000, 0x3F800000, 0x00000000,
//     0x00000000, 0xBF800000,
// };

// TODO: Colors$481 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u32 Colors$481[1] = {
//     0xFFFFFFFF,
// };

// TODO: UVs$482 is a static local of a function: move it inside that function as
// `static` with this data (it's here at its place in .data)
// u32 UVs$482[11] = {
//     0x00000000, 0x00000000, 0x3F800000, 0x00000000,
//     0x3F800000, 0x3F800000, 0x00000000, 0x3F800000,
//     0x00000000, 0x00000000, 0x00000000,
// };

// TODO: type
static u32 shadow_dl[8] = {
    0xA0001200, 0x01020304, 0x05060708, 0x090A0B0C,
    0x0D0E0F10, 0x01000000, 0x00000000, 0x00000000,
};

// TODO: type
static u32 shadow_vtx[56] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x3F800000, 0x3F3504F7, 0x00000000,
    0x3F3504F7, 0x3F800000, 0x00000000, 0x00000000,
    0x3F3504F7, 0x00000000, 0xBF3504E6, 0x358637BD,
    0x00000000, 0xBF800000, 0xBF3504E6, 0x00000000,
    0xBF3504F7, 0xBF800000, 0x00000000, 0xB58637BD,
    0xBF350508, 0x00000000, 0x3F3504E6, 0xB58637BD,
    0x00000000, 0x3F800000, 0x3F3504D5, 0x00000000,
    0x3F350508, 0x3F800000, 0x00000000, 0x360637BD,
    0x3F350508, 0x00000000, 0xBF3504D5, 0x360637BD,
    0x00000000, 0xBF800000, 0xBF3504D5, 0x00000000,
    0xBF350518, 0xBF800000, 0x00000000, 0xB60637BD,
    0xBF350508, 0x00000000, 0x3F3504D5, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 shadow_col[8] = {
    0x00000080, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
static u32 work[1] = {
    0x00000000,
};

// TODO: write puniSinCosRet (global, 0x44 bytes) here:
// its asm is in build/G8MJ01/mri/asm/driver/punidrv.s; this is where the original has it, just before the first script that uses it

// TODO: write puniInit (global, 0x40 bytes) here:
// its asm is in build/G8MJ01/mri/asm/driver/punidrv.s; this is where the original has it, just before the first script that uses it

// TODO: write puniEntry (global, 0x1cc bytes) here:
// its asm is in build/G8MJ01/mri/asm/driver/punidrv.s; this is where the original has it, just before the first script that uses it

// TODO: write puniMakeTexture (global, 0x30c bytes) here:
// its asm is in build/G8MJ01/mri/asm/driver/punidrv.s; this is where the original has it, just before the first script that uses it

// TODO: write puniReset (global, 0xd0 bytes) here:
// its asm is in build/G8MJ01/mri/asm/driver/punidrv.s; this is where the original has it, just before the first script that uses it

// TODO: write puniFadeOff (global, 0x14 bytes) here:
// its asm is in build/G8MJ01/mri/asm/driver/punidrv.s; this is where the original has it, just before the first script that uses it

// TODO: write puniMain (global, 0x318 bytes) here:
// its asm is in build/G8MJ01/mri/asm/driver/punidrv.s; this is where the original has it, just before the first script that uses it

// TODO: write compare (static, 0x2c bytes) here:
// its asm is in build/G8MJ01/mri/asm/driver/punidrv.s; this is where the original has it, just before the first script that uses it

// TODO: write puniGetPosePtr (global, 0x14 bytes) here:
// its asm is in build/G8MJ01/mri/asm/driver/punidrv.s; this is where the original has it, just before the first script that uses it

// TODO: write puniGetPoseNum (global, 0x14 bytes) here:
// its asm is in build/G8MJ01/mri/asm/driver/punidrv.s; this is where the original has it, just before the first script that uses it

// TODO: write puniPoseDraw (global, 0xc4 bytes) here:
// its asm is in build/G8MJ01/mri/asm/driver/punidrv.s; this is where the original has it, just before the first script that uses it

// TODO: write puniDraw (global, 0x16c bytes) here:
// its asm is in build/G8MJ01/mri/asm/driver/punidrv.s; this is where the original has it, just before the first script that uses it

// TODO: write puniLoadRenderMode (global, 0x60 bytes) here:
// its asm is in build/G8MJ01/mri/asm/driver/punidrv.s; this is where the original has it, just before the first script that uses it

// TODO: write puniLoadVertex (global, 0xa0 bytes) here:
// its asm is in build/G8MJ01/mri/asm/driver/punidrv.s; this is where the original has it, just before the first script that uses it

// TODO: write puniLoadTexture (global, 0xfc bytes) here:
// its asm is in build/G8MJ01/mri/asm/driver/punidrv.s; this is where the original has it, just before the first script that uses it

// TODO: write puniGetUsedTmemSize (global, 0x14 bytes) here:
// its asm is in build/G8MJ01/mri/asm/driver/punidrv.s; this is where the original has it, just before the first script that uses it

// TODO: write puniLoadTextureExit (global, 0x20 bytes) here:
// its asm is in build/G8MJ01/mri/asm/driver/punidrv.s; this is where the original has it, just before the first script that uses it

// TODO: write puniLoadTev (global, 0x1bc bytes) here:
// its asm is in build/G8MJ01/mri/asm/driver/punidrv.s; this is where the original has it, just before the first script that uses it

// TODO: write camGetRy (static, 0x90 bytes) here:
// its asm is in build/G8MJ01/mri/asm/driver/punidrv.s; this is where the original has it, just before the first script that uses it

// TODO: write puniLoadShadowVertex (global, 0x60 bytes) here:
// its asm is in build/G8MJ01/mri/asm/driver/punidrv.s; this is where the original has it, just before the first script that uses it

// TODO: write puniLoadShadowTev (global, 0xec bytes) here:
// its asm is in build/G8MJ01/mri/asm/driver/punidrv.s; this is where the original has it, just before the first script that uses it

// TODO: write puniLoadShadowRenderMode (global, 0x60 bytes) here:
// its asm is in build/G8MJ01/mri/asm/driver/punidrv.s; this is where the original has it, just before the first script that uses it

// TODO: write puniLoadShadowMtx (global, 0x30 bytes) here:
// its asm is in build/G8MJ01/mri/asm/driver/punidrv.s; this is where the original has it, just before the first script that uses it

// TODO: write puniDrawShadow (global, 0xf8 bytes) here:
// its asm is in build/G8MJ01/mri/asm/driver/punidrv.s; this is where the original has it, just before the first script that uses it
// clang-format on
