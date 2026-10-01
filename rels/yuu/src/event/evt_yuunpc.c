#include "evt_yuunpc.h"
#include "evt/evt_cmd.h"
#include "evt/evt_ext.h"
#include "evt/evt_npc.h"
#include "evt/evt_seq.h"
#include "forcedata.h"
#include "driver/animdrv.h"
#include "driver/camdrv.h"
#include "driver/dispdrv.h"
#include "driver/extdrv.h"
#include "manager/evtmgr_cmd.h"
#include "mario/mariost.h"
#include "memory.h"
#include "system.h"
#include <string.h>
#include <stdlib.h>

void* yuunpc_yuuminigame4_ptr;
static YuunpcWp* yuunpc_wp;

#pragma pool_data off

#pragma force_active on

// clang-format off
extend_data(36,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

extend_data(37,
     u64, negone_one, 0xBF8000003F800000,
     u32, neg_one, 0xBF800000,
     f32, __local_PI, 3.1415926535897932384626433832795f,
     f32, __local_3PIO2, 3.0f * 1.57079632679489661923132169163975f,
     u64, one_negone, 0x3F800000BF800000,
     f32, __local_PIO2, 1.57079632679489661923132169163975f,
     f32, one, 1.0f)

// TODO: type
u32 yuusintbl[450] = {
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

// TODO: type
u8 hitdata[1156] = {
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x36, 0x31, 0x37, 0x31, 0x38,
    0x31, 0x39, 0x31, 0x31, 0x31, 0x32, 0x31, 0x33, 0x31, 0x34, 0x31, 0x35, 0x31, 0x31, 0x32, 0x32,
    0x32, 0x33, 0x31, 0x31, 0x30, 0x32, 0x30, 0x33, 0x30, 0x34, 0x30, 0x35, 0x30, 0x36, 0x30, 0x36,
    0x30, 0x37, 0x30, 0x38, 0x30, 0x39, 0x31, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x32, 0x34, 0x32, 0x35, 0x32, 0x36,
    0x32, 0x37, 0x32, 0x38, 0x32, 0x38, 0x32, 0x38, 0x32, 0x38, 0x32, 0x38, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x37, 0x34, 0x37, 0x34, 0x37, 0x34, 0x37, 0x34,
    0x37, 0x34, 0x37, 0x34, 0x37, 0x34, 0x34, 0x39, 0x34, 0x39, 0x32, 0x34, 0x32, 0x34, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA1, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA1, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA1, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA1, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA1, 0x81, 0xA1,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA1, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA1, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA1, 0x81, 0xA1,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA1, 0x81, 0xA1, 0x81, 0xA1, 0x81, 0xA1, 0x81, 0xA1, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA1, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA1, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA1, 0x81, 0xA1,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA1, 0x81, 0xA1, 0x81, 0xA1, 0x81, 0xA1, 0x81, 0xA1, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA1, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA1, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA1, 0x81, 0xA1,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA1, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA1, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0, 0x81, 0xA0,
    0x00, 0x00, 0x00, 0x00,
};

char* ext_yuunpc_minigame1_data[18] = {
    "c_kinopiko", "KNF_S_1", NULL, "c_roten", "S_1", NULL, "c_jugemu", "JGM_S_1", NULL, "c_kinopio",
    "KNP_S_1", NULL, NULL, NULL, NULL, NULL, NULL, NULL,
};

char* ext_yuunpc_minigame2_data[18] = {
    "c_kinopiko", "KNF_S_1", NULL, "c_roten", "S_1", NULL, "c_jugemu", "JGM_S_1", NULL, "c_kinopio",
    "KNP_S_1", NULL, NULL, NULL, NULL, NULL, NULL, NULL,
};

char* ext_yuunpc_minigame3_data[18] = {
    "c_kinopiko", "KNF_S_1", NULL, "c_roten", "S_1", NULL, "c_jugemu", "JGM_S_1", NULL, "c_kinopio",
    "KNP_S_1", NULL, NULL, NULL, NULL, NULL, NULL, NULL,
};

// TODO: type
u32 ext_yuunpc_minigame4_data[234] = {
    (u32)"c_kinopiko", (u32)"KNF_S_1", 0x00000000, (u32)"c_roten",
    (u32)"S_1", 0x00000000, (u32)"c_jugemu", (u32)"JGM_S_1",
    0x00000000, (u32)"c_kinopio", (u32)"KNP_S_1", 0x00000000,
    (u32)"c_kinopio", (u32)"KNP_R_1", 0x00000000, (u32)"c_kinopio",
    (u32)"KNP_R_1", 0x3F800000, (u32)"c_kinopio", (u32)"KNP_R_1",
    0x40000000, (u32)"c_kinopio", (u32)"KNP_R_1", 0x40400000,
    (u32)"c_kinopio", (u32)"KNP_R_1", 0x40800000, (u32)"c_kinopio",
    (u32)"KNP_R_1", 0x40A00000, (u32)"c_kinopio", (u32)"KNP_R_1",
    0x40C00000, (u32)"c_kinopio", (u32)"KNP_R_1", 0x40E00000,
    (u32)"c_kinopio", (u32)"KNP_R_1", 0x41000000, (u32)"c_kinopio",
    (u32)"KNP_R_1", 0x41100000, (u32)"c_kinopio", (u32)"KNP_R_1",
    0x41200000, (u32)"c_kinopio", (u32)"KNP_R_1", 0x41300000,
    (u32)"c_kinopio", (u32)"KNP_R_1", 0x41400000, (u32)"c_kinopio",
    (u32)"KNP_R_1", 0x41500000, (u32)"c_kinopio", (u32)"KNP_R_1",
    0x41600000, (u32)"c_kinopio", (u32)"KNP_R_1", 0x41700000,
    (u32)"c_kinopio", (u32)"KNP_R_1", 0x41800000, (u32)"c_kinopio",
    (u32)"KNP_R_1", 0x41880000, (u32)"c_kinopio", (u32)"KNP_R_1",
    0x41900000, (u32)"c_kinopio", (u32)"KNP_R_1", 0x41980000,
    (u32)"c_kinopio", (u32)"KNP_R_1", 0x41A00000, (u32)"c_kinopio",
    (u32)"KNP_R_1", 0x41A80000, (u32)"c_kinopio", (u32)"KNP_R_1",
    0x41B00000, (u32)"c_kinopio", (u32)"KNP_R_1", 0x41B80000,
    (u32)"c_bom_n", (u32)"S_1", 0x00000000, (u32)"c_bom_n",
    (u32)"R_1", 0x00000000, (u32)"c_bom_n", (u32)"R_1",
    0x3F800000, (u32)"c_bom_n", (u32)"R_1", 0x40000000,
    (u32)"c_bom_n", (u32)"R_1", 0x40400000, (u32)"c_bom_n",
    (u32)"R_1", 0x40800000, (u32)"c_bom_n", (u32)"R_1",
    0x40A00000, (u32)"c_bom_n", (u32)"R_1", 0x40C00000,
    (u32)"c_bom_n", (u32)"R_1", 0x40E00000, (u32)"c_bom_n",
    (u32)"R_1", 0x41000000, (u32)"c_bom_n", (u32)"R_1",
    0x41100000, (u32)"c_bom_n", (u32)"R_1", 0x41200000,
    (u32)"c_bom_n", (u32)"R_1", 0x41300000, (u32)"c_bom_n",
    (u32)"R_1", 0x41400000, (u32)"c_bom_n", (u32)"R_1",
    0x41500000, (u32)"c_bom_n", (u32)"R_1", 0x41600000,
    (u32)"c_bom_n", (u32)"R_1", 0x41700000, (u32)"c_bom_n",
    (u32)"R_1", 0x41800000, (u32)"c_bom_n", (u32)"R_1",
    0x41880000, (u32)"c_bom_n", (u32)"R_1", 0x41900000,
    (u32)"c_bom_n", (u32)"R_1", 0x41980000, (u32)"c_tyusan",
    (u32)"CHU_S_1", 0x00000000, (u32)"c_tyusan", (u32)"CHU_S_1",
    0x43030000, (u32)"c_tyusan", (u32)"CHU_S_1", 0x43040000,
    (u32)"c_tyusan", (u32)"CHU_S_1", 0x43050000, (u32)"c_tyusan",
    (u32)"CHU_R_1", 0x00000000, (u32)"c_tyusan", (u32)"CHU_R_1",
    0x3F800000, (u32)"c_tyusan", (u32)"CHU_R_1", 0x40000000,
    (u32)"c_tyusan", (u32)"CHU_R_1", 0x40400000, (u32)"c_tyusan",
    (u32)"CHU_R_1", 0x40800000, (u32)"c_tyusan", (u32)"CHU_R_1",
    0x40A00000, (u32)"c_tyusan", (u32)"CHU_R_1", 0x40C00000,
    (u32)"c_tyusan", (u32)"CHU_R_1", 0x40E00000, (u32)"c_tyusan",
    (u32)"CHU_R_1", 0x41000000, (u32)"c_tyusan", (u32)"CHU_R_1",
    0x41100000, (u32)"c_tyusan", (u32)"CHU_R_1", 0x41200000,
    (u32)"c_tyusan", (u32)"CHU_R_1", 0x41300000, (u32)"c_tyusan",
    (u32)"CHU_R_1", 0x41400000, (u32)"c_tyusan", (u32)"CHU_R_1",
    0x41500000, 0x00000000, 0x00000000, 0x00000000,
    (u32)"c_luigi", (u32)"S_1", 0x42200000, (u32)"c_luigi",
    (u32)"W_1", 0x42200000, (u32)"c_luigi", (u32)"R_1",
    0x41A00000, (u32)"c_nokorin", (u32)"S_1", 0x42200000,
    (u32)"c_nokorin", (u32)"W_1", 0x42200000, (u32)"c_nokorin",
    (u32)"R_1", 0x41A00000, (u32)"c_joho_a", (u32)"S_1",
    0x42700000, (u32)"c_joho_a", (u32)"W_1", 0x42400000,
    (u32)"c_joho_a", (u32)"R_1", 0x41C00000, 0x00000000,
    0x00000000, 0x00000000,
};

// TODO: type
u32 chusan_mov_tbl[28] = {
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000001, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00000001, 0x00000001,
    0x00000000, 0x00000000, 0x00000001, 0x00000001,
    0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

// TODO: type
u32 chusan_act_tbl[8] = {
    0x00000001, 0x00000002, 0x00000003, 0x00000003,
    0x00000003, 0x00000002, 0x00000001, 0x00000000,
};

// MWCC inlined yuunpc_pose_change into the three minigame*_init bodies in the original; this
// static copy (MUST_MATCH only) reproduces that. The natural form calls yuunpc_pose_change.
#ifdef MUST_MATCH
static inline void yuunpc_pose_change_sub(YuunpcEntry* entry, s16 animIndex, f32 a, f32 b) {
    YuunpcPoseEntry* cur = &yuunpc_wp->poseEntries[entry->field_54];
    YuunpcPoseEntry* pe = &yuunpc_wp->poseEntries[animIndex];

    if (a <= 0.0f) {
        entry->flags |= 4;
        entry->field_54 = animIndex;
        entry->field_64 = 1.0f;
        {
            u32 ticks = (*(u32*)0x800000F8 >> 2) / 1000;
            f32 t = 1000.0f * (b * (pe->field_1C * entry->field_64)) / 60.0f;
            entry->field_68 = (u64)((f32)ticks * t);
        }
    } else {
        u16 oldflags = entry->flags;
        entry->flags &= ~4;
        if (entry->field_54 == animIndex) {
            b = entry->field_64;
            if (b == a) {
                return;
            }
            {
                if (oldflags & 4) {
                    u32 ticks = (*(u32*)0x800000F8 >> 2) / 1000;
                    b = 60.0f * (f32)(entry->field_68 / ticks) / 1000.0f / (pe->field_1C * b);
                } else {
                    u32 ticks = (*(u32*)0x800000F8 >> 2) / 1000;
                    b = 60.0f * (f32)((gp->renderTime - entry->field_68) / ticks) / 1000.0f / (cur->field_1C * b);
                }
            }
        } else {
            entry->field_54 = animIndex;
        }
        entry->field_64 = a;
        {
            u32 ticks = (*(u32*)0x800000F8 >> 2) / 1000;
            f32 t = 1000.0f * (b * (pe->field_1C * a)) / 60.0f;
            entry->field_68 = (u64)((f32)gp->renderTime - (f32)ticks * t);
        }
    }
    switch (entry->flags & 0x300) {
        case 0x100:
        case 0x200:
        case 0x300:
            animPoseSetAnim(entry->npc->poseId, pe->anim, 1);
            animPoseMain(entry->npc->poseId);
            break;
    }
}
#endif
void ext_yuunpc_minigame1_main(void) {
    yuunpc_pose_minigame1_main();
}

void ext_yuunpc_dispent(void) {
    dispEntry(4, 0.0f, 1, yuunpc_disp, NULL);
}


EVT_BEGIN(evt_yuunpc_minigame1)
    CALL(evt_npc_entry, STRING("yuunpc0"), STRING("c_kinopiko"))
    CALL(evt_npc_entry, STRING("yuunpc1"), STRING("c_roten"))
    CALL(evt_npc_entry, STRING("yuunpc2"), STRING("c_jugemu"))
    CALL(evt_npc_entry, STRING("yuunpc3"), STRING("c_kinopio"))
    CALL(evt_npc_set_position, STRING("yuunpc0"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("yuunpc1"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("yuunpc2"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("yuunpc3"), 0, -1000, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("yuunpc0"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("yuunpc1"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("yuunpc2"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("yuunpc3"), 32)
    CALL(evt_ext_entry, 300, PTR(ext_yuunpc_minigame1_data), PTR(ext_yuunpc_minigame1_init), PTR(ext_yuunpc_minigame1_main), PTR(ext_yuunpc_dispent))
    RETURN
EVT_END()
void ext_yuunpc_minigame2_main(void) {
    yuunpc_pose_minigame2_main();
}

// NONMATCHING (97.93%): logic complete; the inlined pose_change now uses the static copy below,
// so only register allocation differs (the original keeps f23 and r21-r24 live, ours f24-r24).
// The dat_yuu_00014efc/dat_yuu_00014f0c block copy is byte-identical.
void ext_yuunpc_minigame2_init(void) {
    s32 i;
    f32 arr1[4] = { 170.0f, 130.0f, 90.0f, 50.0f };
    f32 arr2[8] = { -7.0f, -1.0f, 5.0f, -5.0f, 1.0f, 7.0f, -3.0f, 3.0f };
    ExtPose* pose;
    YuunpcEntry* entry;

    yuunpc_wp = _mapAlloc(0xA94);
    memset(yuunpc_wp, 0, 0xA94);
    yuunpc_wp->count = extGetPoseNum();
    yuunpc_wp->entries = _mapAlloc(yuunpc_wp->count * 0x70);
    memset(yuunpc_wp->entries, 0, yuunpc_wp->count * 0x70);
    yuunpc_pose_init((YuunpcPoseArg*)ext_yuunpc_minigame2_data);

    pose = extGetPosePtr();
    entry = yuunpc_wp->entries;
    for (i = 0; i < yuunpc_wp->count; i++, pose++, entry++) {
        s32 q = i / 4;
        f32 t;
        f32 y;
        s32 idx;

        entry->field_54 = yuunpc_wp->field_1C;
        entry->field_0 = i;
        entry->flags |= 1;
        pose->flags |= 7;
        PSMTXIdentity((MtxPtr)pose->matrix);

        t = 60.0f * (f32)(i % 4) - 655.0f;
        y = 30.0f + t;
        ((MtxPtr)pose->matrix)[0][3] = 13.333333f * (f32)q - 500.0f;
        ((MtxPtr)pose->matrix)[1][3] = arr1[(s32)((y - -655.0f) / 60.0f)];
        ((MtxPtr)pose->matrix)[2][3] = y + arr2[q % 8];

        pose->field_0x4 = (f32)(irand(2) * 180 + 90);

        switch (irand(1000) % 4) {
            case 0:
                idx = yuunpc_pose_check("c_kinopio", "KNP_S_1");
                entry->field_5C = idx;
                entry->field_5A = idx;
                entry->field_58 = idx;
                entry->field_56 = idx;
                break;
            case 1:
                idx = yuunpc_pose_check("c_kinopiko", "KNF_S_1");
                entry->field_5C = idx;
                entry->field_5A = idx;
                entry->field_58 = idx;
                entry->field_56 = idx;
                break;
            case 2:
                idx = yuunpc_pose_check("c_roten", "S_1");
                entry->field_5C = idx;
                entry->field_5A = idx;
                entry->field_58 = idx;
                entry->field_56 = idx;
                break;
            case 3:
                idx = yuunpc_pose_check("c_jugemu", "JGM_S_1");
                entry->field_5C = idx;
                entry->field_5A = idx;
                entry->field_58 = idx;
                entry->field_56 = idx;
                break;
        }

#ifdef MUST_MATCH
        yuunpc_pose_change_sub(entry, entry->field_56, 1.0f, (f32)rand() / 32767.0f);
#else
        yuunpc_pose_change(entry, entry->field_56, 1.0f, (f32)rand() / 32767.0f);
#endif
    }
}

EVT_BEGIN(evt_yuunpc_minigame2)
    CALL(evt_npc_entry, STRING("yuunpc0"), STRING("c_kinopiko"))
    CALL(evt_npc_entry, STRING("yuunpc1"), STRING("c_roten"))
    CALL(evt_npc_entry, STRING("yuunpc2"), STRING("c_jugemu"))
    CALL(evt_npc_entry, STRING("yuunpc3"), STRING("c_kinopio"))
    CALL(evt_npc_set_position, STRING("yuunpc0"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("yuunpc1"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("yuunpc2"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("yuunpc3"), 0, -1000, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("yuunpc0"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("yuunpc1"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("yuunpc2"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("yuunpc3"), 32)
    CALL(evt_ext_entry, 300, PTR(ext_yuunpc_minigame2_data), PTR(ext_yuunpc_minigame2_init), PTR(ext_yuunpc_minigame2_main), PTR(ext_yuunpc_dispent))
    RETURN
EVT_END()
void ext_yuunpc_minigame3_main(void) {
    yuunpc_pose_minigame3_main();
}

// NONMATCHING (97.93%): logic complete; the inlined pose_change now uses the static copy below,
// so only register allocation differs (the original keeps f23 and r21-r24 live, ours f24-r24).
// The dat_yuu_00014f2c/dat_yuu_00014f3c block copy is byte-identical.
void ext_yuunpc_minigame3_init(void) {
    s32 i;
    f32 arr1[4] = { 200.0f, 160.0f, 120.0f, 80.0f };
    f32 arr2[8] = { -7.0f, -1.0f, 5.0f, -5.0f, 1.0f, 7.0f, -3.0f, 3.0f };
    ExtPose* pose;
    YuunpcEntry* entry;

    yuunpc_wp = _mapAlloc(0xA94);
    memset(yuunpc_wp, 0, 0xA94);
    yuunpc_wp->count = extGetPoseNum();
    yuunpc_wp->entries = _mapAlloc(yuunpc_wp->count * 0x70);
    memset(yuunpc_wp->entries, 0, yuunpc_wp->count * 0x70);
    yuunpc_pose_init((YuunpcPoseArg*)ext_yuunpc_minigame3_data);

    pose = extGetPosePtr();
    entry = yuunpc_wp->entries;
    for (i = 0; i < yuunpc_wp->count; i++, pose++, entry++) {
        s32 q = i / 4;
        f32 t;
        f32 y;
        s32 idx;

        entry->field_54 = yuunpc_wp->field_1C;
        entry->field_0 = i;
        entry->flags |= 1;
        pose->flags |= 7;
        PSMTXIdentity((MtxPtr)pose->matrix);

        t = 60.0f * (f32)(i % 4) - 750.0f;
        y = 30.0f + t;
        ((MtxPtr)pose->matrix)[0][3] = 13.333333f * (f32)q - 600.0f;
        ((MtxPtr)pose->matrix)[1][3] = arr1[(s32)((y - -750.0f) / 60.0f)];
        ((MtxPtr)pose->matrix)[2][3] = y + arr2[q % 8];

        pose->field_0x4 = (f32)(irand(2) * 180 + 90);

        switch (irand(1000) % 4) {
            case 0:
                idx = yuunpc_pose_check("c_kinopio", "KNP_S_1");
                entry->field_5C = idx;
                entry->field_5A = idx;
                entry->field_58 = idx;
                entry->field_56 = idx;
                break;
            case 1:
                idx = yuunpc_pose_check("c_kinopiko", "KNF_S_1");
                entry->field_5C = idx;
                entry->field_5A = idx;
                entry->field_58 = idx;
                entry->field_56 = idx;
                break;
            case 2:
                idx = yuunpc_pose_check("c_roten", "S_1");
                entry->field_5C = idx;
                entry->field_5A = idx;
                entry->field_58 = idx;
                entry->field_56 = idx;
                break;
            case 3:
                idx = yuunpc_pose_check("c_jugemu", "JGM_S_1");
                entry->field_5C = idx;
                entry->field_5A = idx;
                entry->field_58 = idx;
                entry->field_56 = idx;
                break;
        }

#ifdef MUST_MATCH
        yuunpc_pose_change_sub(entry, entry->field_56, 1.0f, (f32)rand() / 32767.0f);
#else
        yuunpc_pose_change(entry, entry->field_56, 1.0f, (f32)rand() / 32767.0f);
#endif
    }
}

EVT_BEGIN(evt_yuunpc_minigame3)
    CALL(evt_npc_entry, STRING("yuunpc0"), STRING("c_kinopiko"))
    CALL(evt_npc_entry, STRING("yuunpc1"), STRING("c_roten"))
    CALL(evt_npc_entry, STRING("yuunpc2"), STRING("c_jugemu"))
    CALL(evt_npc_entry, STRING("yuunpc3"), STRING("c_kinopio"))
    CALL(evt_npc_set_position, STRING("yuunpc0"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("yuunpc1"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("yuunpc2"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("yuunpc3"), 0, -1000, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("yuunpc0"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("yuunpc1"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("yuunpc2"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("yuunpc3"), 32)
    CALL(evt_ext_entry, 360, PTR(ext_yuunpc_minigame3_data), PTR(ext_yuunpc_minigame3_init), PTR(ext_yuunpc_minigame3_main), PTR(ext_yuunpc_dispent))
    RETURN
EVT_END()
void ext_yuunpc_minigame4_main(void) {
    yuunpc_pose_minigame4_main();
}

void ext_yuunpc_minigame4_init(void) {
    yuunpc_wp = _mapAlloc(0xA94);
    memset(yuunpc_wp, 0, 0xA94);
    yuunpc_wp->count = extGetPoseNum();
    yuunpc_wp->entries = _mapAlloc(yuunpc_wp->count * 0x70);
    memset(yuunpc_wp->entries, 0, yuunpc_wp->count * 0x70);
    ext_yuunpc_minigame4_init_sub();
}

EVT_BEGIN(evt_yuunpc_minigame4)
    CALL(evt_npc_entry, STRING("yuunpc0"), STRING("c_kinopiko"))
    CALL(evt_npc_entry, STRING("yuunpc1"), STRING("c_roten"))
    CALL(evt_npc_entry, STRING("yuunpc2"), STRING("c_jugemu"))
    CALL(evt_npc_entry, STRING("yuunpc3"), STRING("c_kinopio"))
    CALL(evt_npc_entry, STRING("yuunpc4"), STRING("c_bom_n"))
    CALL(evt_npc_entry, STRING("yuunpc5"), STRING("c_tyusan"))
    CALL(evt_npc_entry, STRING("yuusp0"), STRING("c_luigi"))
    CALL(evt_npc_entry, STRING("yuusp1"), STRING("c_nokorin"))
    CALL(evt_npc_entry, STRING("yuusp2"), STRING("c_joho_a"))
    CALL(evt_npc_set_position, STRING("yuunpc0"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("yuunpc1"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("yuunpc2"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("yuunpc3"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("yuunpc4"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("yuunpc5"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("yuusp0"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("yuusp1"), 0, -1000, 0)
    CALL(evt_npc_set_position, STRING("yuusp2"), 0, -1000, 0)
    CALL(evt_npc_flag_onoff, 1, STRING("yuunpc0"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("yuunpc1"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("yuunpc2"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("yuunpc3"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("yuunpc4"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("yuunpc5"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("yuusp0"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("yuusp1"), 32)
    CALL(evt_npc_flag_onoff, 1, STRING("yuusp2"), 32)
    CALL(evt_ext_entry, 900, PTR(ext_yuunpc_minigame4_data), PTR(ext_yuunpc_minigame4_init), PTR(ext_yuunpc_minigame4_main), PTR(ext_yuunpc_dispent))
    BEGIN_INLINE_EVENT
        CALL(evt_seq_wait, 2)
        CALL(evt_npc_delete, STRING("yuunpc0"))
        CALL(evt_npc_delete, STRING("yuunpc1"))
        CALL(evt_npc_delete, STRING("yuunpc2"))
        CALL(evt_npc_delete, STRING("yuunpc3"))
        CALL(evt_npc_delete, STRING("yuunpc4"))
        CALL(evt_npc_delete, STRING("yuunpc5"))
    END_INLINE_EVENT
    RETURN
EVT_END()

// TODO: write yuunpc_pose_minigame4_main (global, 0x37b0 bytes) here:
// its asm is in build/G8MJ01/yuu/asm/event/evt_yuunpc.s; this is where the original has it, just before the first script that uses it

// TODO: type
static u32 jumptable_00005CC4[25] = {
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1770), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1764), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1764), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1758),
};

// TODO: type
static u32 jumptable_00005D28[25] = {
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1724), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1718), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1718), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1778),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1714),
};

// TODO: type
static u32 jumptable_00005D8C[25] = {
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x16a0), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x16a0), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x16a0), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x16a0),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x16a0), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x16a0), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x16a0), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x16a0),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1460), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x16a0), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x16a0), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x16a0),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x16a0), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x16a0), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x16a0), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x16a0),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x15d8), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x16a0), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x16a0), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x16a0),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x16a0), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x16a0), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x16a0), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x16a0),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1518),
};

// TODO: type
static u32 jumptable_00005DF0[8] = {
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1374), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x12ac), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1310), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x12ac),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1310), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x12ac), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1310), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1374),
};

// TODO: type
static u32 jumptable_00005E10[8] = {
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1140), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1078), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x10dc), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1078),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x10dc), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1078), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x10dc), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1140),
};

// TODO: type
static u32 jumptable_00005E30[25] = {
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0xf04), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1438), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1438), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1438),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1438), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1438), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1438), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1438),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1438), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1438), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1438), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1438),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1438), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1438), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1438), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1438),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1438), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1438), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1438), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1438),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1438), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1438), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1438), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1438),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x1208),
};

// TODO: type
static u32 jumptable_00005E94[29] = {
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0xd90), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xdd4), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xd90), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xd90),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0xd90), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xde0), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xde0), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xd90),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0xd90), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xd90), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xdd4), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xea4),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0xd90), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xd90), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xd90), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xea4),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0xd90), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xd90), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xd90), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xea4),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0xd90), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xd90), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xd90), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xea4),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0xd90), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xd90), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xd90), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xde0),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0xde0),
};

// TODO: type
static u32 jumptable_00005F08[29] = {
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0xc68), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xca8), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xc68), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xc68),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0xc68), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xcb4), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xcb4), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xc68),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0xc68), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xc68), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xca8), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xea4),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0xc68), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xc68), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xc68), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xea4),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0xc68), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xc68), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xc68), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xea4),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0xc68), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xc68), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xc68), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xea4),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0xc68), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xc68), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xc68), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xcb4),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0xcb4),
};

// TODO: type
static u32 jumptable_00005F7C[29] = {
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x4b8), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x4b8), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x550), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x550),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x550), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x550), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x550), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x6ec),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x6ec), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x6ec), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x6ec), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x784),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x7f0), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x7f0), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x7f0), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x7f0),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x944), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x944), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x944), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x944),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0xa94), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xa94), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xa94), (u32)((u8*)&yuunpc_pose_minigame4_main + 0xa94),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x548), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x548), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x548), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x548),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x548),
};

// TODO: type
static u32 jumptable_00005FF0[25] = {
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x310), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x310), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x310), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x310),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x310), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x310), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x310), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x310),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x274), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x310), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x310), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x310),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x310), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x310), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x310), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x310),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x28c), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x310), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x310), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x310),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x310), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x310), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x310), (u32)((u8*)&yuunpc_pose_minigame4_main + 0x310),
    (u32)((u8*)&yuunpc_pose_minigame4_main + 0x310),
};

// TODO: write ext_yuunpc_minigame4_init_sub (global, 0x26ac bytes) here:
// its asm is in build/G8MJ01/yuu/asm/event/evt_yuunpc.s; this is where the original has it, just before the first script that uses it

// TODO: type
static u32 jumptable_00006054[8] = {
    (u32)((u8*)&ext_yuunpc_minigame4_init_sub + 0x25b0), (u32)((u8*)&ext_yuunpc_minigame4_init_sub + 0x24e8), (u32)((u8*)&ext_yuunpc_minigame4_init_sub + 0x254c), (u32)((u8*)&ext_yuunpc_minigame4_init_sub + 0x24e8),
    (u32)((u8*)&ext_yuunpc_minigame4_init_sub + 0x254c), (u32)((u8*)&ext_yuunpc_minigame4_init_sub + 0x24e8), (u32)((u8*)&ext_yuunpc_minigame4_init_sub + 0x254c), (u32)((u8*)&ext_yuunpc_minigame4_init_sub + 0x25b0),
};
void errper_set_rtn(ErrPer* p) {
    switch (p->field_4) {
        case -1:
        case 6:
            p->field_8 = 2.0f * (0.25f * ((f32)rand() / 32767.0f)) + (-0.25f);
            break;
        case 0:
        case 2:
        case 4:
            p->field_8 = 2.0f * (0.25f * ((f32)rand() / 32767.0f)) + (-0.25f);
            break;
        case 1:
        case 3:
        case 5:
            p->field_8 = 2.0f * (0.25f * ((f32)rand() / 32767.0f)) + (-0.25f);
            break;
        default:
            p->field_8 = 2.0f * (0.1f * ((f32)rand() / 32767.0f)) + (-0.1f);
            break;
    }
}

void yuunpc_disp(CameraId cameraId, void* param) {
    extLoadRenderMode();
    extLoadVertex();
    extLoadTexture();
    extLoadTev();
    extDraw();
    extLoadTextureExit();
}

s32 yuunpc_juni(Vec* pos) {
    f32 value = pos->x;
    YuunpcEntry* entry = yuunpc_wp->entries;
    s32 count = 0;
    s32 i;
    for (i = yuunpc_wp->count; i > 0; i--) {
        if ((entry->flags & 2) && !(entry->flags & 0x40) && value < entry->field_24) {
            count++;
        }
        entry++;
    }
    return count;
}

u16 yuunpc_wp_flag_check(u16 mask) {
    return yuunpc_wp->flags & mask;
}

void yuunpc_wp_flag_off(u16 mask) {
    yuunpc_wp->flags &= ~mask;
}

void yuunpc_wp_flag_on(u16 mask) {
    yuunpc_wp->flags |= mask;
}

s32 goalincheck(YuunpcMapWork* p) {
    f32 dx = p->field_24.x - p->field_18.x;
    f32 dz = p->field_24.z - p->field_18.z;
    f32 r = p->field_3C;
    if (dx * dx + dz * dz <= r * r) {
        if (p->field_18.x == p->field_0C.x && p->field_18.z == p->field_0C.z) {
            p->field_24.x = p->field_18.x;
            p->field_24.z = p->field_18.z;
            return -1;
        }
        p->field_18 = p->field_0C;
    }
    return 0;
}

s32 fallcheck(YuunpcMapWork* p) {
    BOOL outOfRange;
    s32 height;
    f32 ground;
    p->field_24.y += p->field_40;
    outOfRange = p->field_24.x < -1800.0f || p->field_24.x >= 1800.0f || p->field_24.z < -150.0f || p->field_24.z >= 150.0f;
    if (outOfRange) {
        height = -500;
    } else {
        s32 zidx = (s32)((p->field_24.z - -150.0f) / 50.0f);
        s32 xidx = (s32)((p->field_24.x - -1800.0f) / 50.0f);
        height = yuunpc_wp->map[zidx * 72 + xidx];
    }
    ground = height;
    if (p->field_24.y <= ground) {
        p->field_24.y = ground;
        p->field_40 = 0.0f;
        return -1;
    }
    p->field_40 -= 0.53333336f;
    if (p->field_40 < -16.666666f) {
        p->field_40 = -16.666666f;
    }
    return 0;
}

s32 hitcheck(YuunpcMapWork* p) {
    Vec disp;
    Vec out;
    f32 x;
    f32 z;
    s32 height;
    f32 ground;
    f32 posY;
    BOOL oob;

    if (p->field_3C > 0.0f) {
        PSVECScale(&p->field_30, &disp, p->field_3C);
    } else {
        disp = (Vec){ 0.0f, 0.0f, 0.0f };
    }
    disp.z += p->field_48;
    PSVECAdd(&p->field_24, &disp, &out);

    oob = out.x < -1800.0f || out.x >= 1800.0f || out.z < -150.0f || out.z >= 150.0f;
    if (oob) {
        height = -500;
    } else {
        s32 zidx = (s32)((out.z - -150.0f) / 50.0f);
        s32 xidx = (s32)((out.x - -1800.0f) / 50.0f);
        height = yuunpc_wp->map[zidx * 72 + xidx];
    }
    ground = height;
    posY = p->field_24.y;
    if (posY > ground) {
        p->field_24 = out;
        return -1;
    }
    if (posY < ground) {

    if (disp.x > 0.0f) {
        x = 50.0f * (f32)(s32)((3600.0f + out.x) / 50.0f) - 3600.0f - 0.001f;
        if (p->field_24.x > x) {
            x = p->field_24.x;
        } else {
        oob = x < -1800.0f || x >= 1800.0f || out.z < -150.0f || out.z >= 150.0f;
        if (oob) {
            height = -500;
        } else {
            s32 zidx = (s32)((out.z - -150.0f) / 50.0f);
            s32 xidx = (s32)((x - -1800.0f) / 50.0f);
            height = yuunpc_wp->map[zidx * 72 + xidx];
        }
        if (p->field_24.y == (f32)height) {
            p->field_24.x = x;
            p->field_24.z = out.z;
            return 0;
        }
        }
    } else if (disp.x < 0.0f) {
        x = 50.0f * (f32)((s32)((3600.0f + out.x) / 50.0f) + 1) - 3600.0f;
        if (p->field_24.x < x) {
            x = p->field_24.x;
        } else {
        oob = x < -1800.0f || x >= 1800.0f || out.z < -150.0f || out.z >= 150.0f;
        if (oob) {
            height = -500;
        } else {
            s32 zidx = (s32)((out.z - -150.0f) / 50.0f);
            s32 xidx = (s32)((x - -1800.0f) / 50.0f);
            height = yuunpc_wp->map[zidx * 72 + xidx];
        }
        if (p->field_24.y == (f32)height) {
            p->field_24.x = x;
            p->field_24.z = out.z;
            return 0;
        }
        }
    }

    if (disp.z > 0.0f) {
        z = 50.0f * (f32)(s32)((300.0f + out.z) / 50.0f) - 300.0f - 0.001f;
        if (p->field_24.z > z) {
            z = p->field_24.z;
        } else {
        oob = out.x < -1800.0f || out.x >= 1800.0f || z < -150.0f || z >= 150.0f;
        if (oob) {
            height = -500;
        } else {
            s32 zidx = (s32)((z - -150.0f) / 50.0f);
            s32 xidx = (s32)((out.x - -1800.0f) / 50.0f);
            height = yuunpc_wp->map[zidx * 72 + xidx];
        }
        if (p->field_24.y == (f32)height) {
            p->field_24.x = out.x;
            p->field_24.z = z;
            return 0;
        }
        }
    } else if (disp.z < 0.0f) {
        z = 50.0f * (f32)((s32)((300.0f + out.z) / 50.0f) + 1) - 300.0f;
        if (p->field_24.z < z) {
            z = p->field_24.z;
        } else {
        oob = out.x < -1800.0f || out.x >= 1800.0f || z < -150.0f || z >= 150.0f;
        if (oob) {
            height = -500;
        } else {
            s32 zidx = (s32)((z - -150.0f) / 50.0f);
            s32 xidx = (s32)((out.x - -1800.0f) / 50.0f);
            height = yuunpc_wp->map[zidx * 72 + xidx];
        }
        if (p->field_24.y == (f32)height) {
            p->field_24.x = out.x;
            p->field_24.z = z;
            return 0;
        }
        }
    }

    oob = x < -1800.0f || x >= 1800.0f || z < -150.0f || z >= 150.0f;
    if (oob) {
        height = -500;
    } else {
        s32 zidx = (s32)((z - -150.0f) / 50.0f);
        s32 xidx = (s32)((x - -1800.0f) / 50.0f);
        height = yuunpc_wp->map[zidx * 72 + xidx];
    }
    if (p->field_24.y == (f32)height) {
        p->field_24.x = x;
        p->field_24.z = z;
        return 0;
    }
    } else {
    p->field_24 = out;
    }
    return 0;
}

// NONMATCHING (98.48%): logic complete. Remaining diff: one `lis` register swap, the y-wrap
// arithmetic result register, and the named sdata2 constant addresses.
void yuunpc_pose_minigame3_main(void) {
    s32 i;
    ExtPose* pose;
    MtxPtr mtx;
    YuunpcEntry* entry;
    f32 f31;
    f32 camX;
    f32 f29;
    f32 f28;
    f32 f1;

    camX = camGetPtr(CAMERA_3D)->cameraAt.x;
    pose = extGetPosePtr();
    f31 = 600.0f + camX;
    entry = yuunpc_wp->entries;
    for (i = 0; i < yuunpc_wp->count; i++, pose++, entry++) {
        if (entry->flags & 1) {
            f29 = ((MtxPtr)pose->matrix)[0][3];
            if (f29 > camX) {
                f32 step = 1200.0f;
                for (;;) {
                    if (!(f29 > f31)) break;
                    f29 -= step;
                    ((MtxPtr)pose->matrix)[0][3] = f29;
                }
            } else {
                f32 low = camX - 600.0f;
                f32 step = 1200.0f;
                for (;;) {
                    if (!(f29 < low)) break;
                    f29 += step;
                    ((MtxPtr)pose->matrix)[0][3] = f29;
                }
            }
            mtx = (MtxPtr)pose->matrix;
            f28 = mtx[2][3];
            if (f28 < -570.0f) {
                f1 = mtx[1][3];
                if ((f29 >= -1700.0f && f29 <= -1600.0f) || (f29 >= -50.0f && f29 <= 50.0f) ||
                    (f29 >= 1600.0f && f29 <= 1700.0f)) {
                    if (f1 > -500.0f) {
                        f1 = f1 + -1000.0f;
                        mtx[1][3] = f1;
                    }
                } else if (f1 < -500.0f) {
                    f1 = f1 - -1000.0f;
                    mtx[1][3] = f1;
                }
            }
            if (irand(180) == 0) {
                if (irand(101) <= 80) {
                    pose->field_0x4 = angleABf(camX, 0.0f, f29, f28);
                } else {
                    pose->field_0x4 = angleABf(camX, 0.0f, f29 - 2.0f * (f29 - camX), f28);
                }
            }
        }
        yuunpc_pose_sub(pose, entry);
    }
}

// NONMATCHING (98.67%): logic complete. Remaining diff: one `lis` register swap, the y-wrap
// arithmetic result register, and the named sdata2 constant addresses.
void yuunpc_pose_minigame2_main(void) {
    s32 i;
    ExtPose* pose;
    MtxPtr mtx;
    YuunpcEntry* entry;
    f32 f31;
    f32 camX;
    f32 f29;
    f32 f28;
    f32 f1;

    camX = camGetPtr(CAMERA_3D)->cameraAt.x;
    pose = extGetPosePtr();
    f31 = 500.0f + camX;
    entry = yuunpc_wp->entries;
    for (i = 0; i < yuunpc_wp->count; i++, pose++, entry++) {
        if (entry->flags & 1) {
            f29 = ((MtxPtr)pose->matrix)[0][3];
            if (f29 > camX) {
                f32 step = 1000.0f;
                for (;;) {
                    if (!(f29 > f31)) break;
                    f29 -= step;
                    ((MtxPtr)pose->matrix)[0][3] = f29;
                }
            } else {
                f32 low = camX - 500.0f;
                f32 step = 1000.0f;
                for (;;) {
                    if (!(f29 < low)) break;
                    f29 += step;
                    ((MtxPtr)pose->matrix)[0][3] = f29;
                }
            }
            mtx = (MtxPtr)pose->matrix;
            f28 = mtx[2][3];
            if (f28 < -415.0f) {
                f1 = mtx[1][3];
                if ((f29 >= -5000.0f && f29 <= -4000.0f) || (f29 >= 790.0f && f29 <= 5000.0f)) {
                    if (f1 > -500.0f) {
                        mtx[1][3] = f1 + -1000.0f;
                    }
                } else if (f28 < -475.0f) {
                    if (f29 >= -50.0f && f29 <= 50.0f) {
                        if (f1 > -500.0f) {
                            mtx[1][3] = f1 + -1000.0f;
                        }
                    } else if (f1 < -500.0f) {
                        mtx[1][3] = f1 - -1000.0f;
                    }
                } else if (f1 < -500.0f) {
                    mtx[1][3] = f1 - -1000.0f;
                }
            }
            if (irand(150) == 0) {
                if (irand(101) <= 80) {
                    pose->field_0x4 = angleABf(camX, 0.0f, f29, f28);
                } else {
                    pose->field_0x4 = angleABf(camX, 0.0f, f29 - 2.0f * (f29 - camX), f28);
                }
            }
        }
        yuunpc_pose_sub(pose, entry);
    }
}

// NONMATCHING (98.48%): logic complete (camera-follow x-wrap, y-range wrap, random turn). Remaining diff:
// one `lis` register swap, the y-wrap arithmetic result register, and the named sdata2 constant addresses.
void yuunpc_pose_minigame1_main(void) {
    s32 i;
    ExtPose* pose;
    MtxPtr mtx;
    YuunpcEntry* entry;
    f32 f31;
    f32 camX;
    f32 f29;
    f32 f28;
    f32 f1;

    camX = camGetPtr(CAMERA_3D)->cameraAt.x;
    pose = extGetPosePtr();
    f31 = 500.0f + camX;
    entry = yuunpc_wp->entries;
    for (i = 0; i < yuunpc_wp->count; i++, pose++, entry++) {
        if (entry->flags & 1) {
            f29 = ((MtxPtr)pose->matrix)[0][3];
            if (f29 > camX) {
                f32 step = 1000.0f;
                for (;;) {
                    if (!(f29 > f31)) break;
                    f29 -= step;
                    ((MtxPtr)pose->matrix)[0][3] = f29;
                }
            } else {
                f32 low = camX - 500.0f;
                f32 step = 1000.0f;
                for (;;) {
                    if (!(f29 < low)) break;
                    f29 += step;
                    ((MtxPtr)pose->matrix)[0][3] = f29;
                }
            }
            mtx = (MtxPtr)pose->matrix;
            f28 = mtx[2][3];
            if (f28 < -260.0f) {
                f1 = mtx[1][3];
                if ((f29 >= -5000.0f && f29 <= -1500.0f) || (f29 >= 1700.0f && f29 <= 5000.0f) ||
                    (f29 >= 6000.0f && f29 <= 7000.0f)) {
                    if (f1 > -500.0f) {
                        f1 = f1 + -1000.0f;
                        mtx[1][3] = f1;
                    }
                } else if (f1 < -500.0f) {
                    f1 = f1 - -1000.0f;
                    mtx[1][3] = f1;
                }
            }
            if (irand(150) == 0) {
                if (irand(101) <= 80) {
                    pose->field_0x4 = angleABf(camX, 0.0f, f29, f28);
                } else {
                    pose->field_0x4 = angleABf(camX, 0.0f, f29 - 2.0f * (f29 - camX), f28);
                }
            }
        }
        yuunpc_pose_sub(pose, entry);
    }
}

// The `450.0f - pose->field_0x4` register allocation is only reproduced by loading the field
// into angle first and subtracting it from the constant (MWCC puts the field in f2 and the
// constant in f1, as the original does); the natural one-expression form lands in the other
// registers. Everything else (logic, switch, calls) matches the original directly.
f32 yuunpc_pose_sub(ExtPose* pose, YuunpcEntry* entry) {
    f32 f31;
    YuunpcPoseEntry* pe = &yuunpc_wp->poseEntries[entry->field_54];

    if (!(pose->flags & 2)) {
#ifdef MUST_MATCH
        f32 angle = pose->field_0x4;
        angle = 450.0f - angle;
#else
        f32 angle = 450.0f - pose->field_0x4;
#endif
        if (angle >= 360.0f) {
            angle -= 360.0f;
        }
        {
            f32* tbl = (f32*)yuusintbl;
            s32 idx = (s32)angle;
            f32 c = tbl[idx + 90];
            f32 s = tbl[idx];
            ((MtxPtr)pose->matrix)[0][0] = c;
            ((MtxPtr)pose->matrix)[2][2] = c;
            ((MtxPtr)pose->matrix)[0][2] = s;
            ((MtxPtr)pose->matrix)[2][0] = -s;
        }
    }
    if (entry->flags & 4) {
        u32 ticks = (*(u32*)0x800000F8 >> 2) / 1000;
        f31 = 60.0f * (f32)(entry->field_68 / ticks) / 1000.0f;
        f31 = f31 / (pe->field_1C * entry->field_64);
    } else {
        u32 ticks = (*(u32*)0x800000F8 >> 2) / 1000;
        f31 = 0.5f + 60.0f * (f32)((gp->renderTime - entry->field_68) / ticks) / 1000.0f;
        f31 = f31 / (pe->field_1C * entry->field_64);
        if (f31 >= 1.0f) {
            entry->field_68 = gp->renderTime;
            f31 = 0.0f;
        }
    }
    switch (entry->flags & 0x300) {
        case 0x100:
        case 0x200:
        case 0x300:
            animPoseSetLocalTime(entry->npc->poseId, (f32)(s32)(pe->field_1C * f31));
            {
                f32 t = 3600.0f + (90.0f - pose->field_0x4);
                entry->npc->rotation.y = t - (f32)((s32)(t / 360.0f) * 360);
                entry->npc->position = *(Vec*)&entry->field_24;
            }
            break;
        case 0:
            if (entry->field_54 != yuunpc_wp->field_20) {
                pose->field_0x10 = pe->field_24 + (s32)((f32)pe->field_28 * f31);
            } else {
                s32 j = (s32)(224.0f * f31);
                s32 v = (s32)chusan_mov_tbl[j >> 3] != 0 ? chusan_act_tbl[j & 7] : 0;
                pose->field_0x10 = pe->field_24 + v;
            }
            break;
    }
    return f31;
}

void yuunpc_set_jump_pose(const char* name, const char* anim, f32 value) {
    s32 idx = yuunpc_pose_check(name, anim);
    if (idx >= 0) {
        yuunpc_wp->poseEntries[idx].field_20 = value;
    }
}

// NONMATCHING (92.24%): logic complete; remaining diff is instruction scheduling around
// the oldflags&4 e-computation and the sdata2 constant-pool labels.
void yuunpc_pose_change2(YuunpcEntry* entry, const char* name, const char* anim, f32 a) {
    f32 b = (f32)rand() / 32767.0f;
    s16 idx = (s16)yuunpc_pose_check(name, anim);
    YuunpcPoseEntry* cur = &yuunpc_wp->poseEntries[entry->field_54];
    YuunpcPoseEntry* pe = &yuunpc_wp->poseEntries[idx];

    if (a <= 0.0f) {
        entry->flags |= 4;
        entry->field_54 = idx;
        entry->field_64 = 1.0f;
        {
            u32 ticks = (*(u32*)0x800000F8 >> 2) / 1000;
            f32 t = 1000.0f * (b * (pe->field_1C * entry->field_64)) / 60.0f;
            entry->field_68 = (u64)((f32)ticks * t);
        }
    } else {
        u16 oldflags = entry->flags;
        entry->flags &= ~4;
        if (entry->field_54 == idx) {
            b = entry->field_64;
            if (b == a) {
                return;
            }
            {
                if (oldflags & 4) {
                    u32 ticks = (*(u32*)0x800000F8 >> 2) / 1000;
                    b = 60.0f * (f32)(entry->field_68 / ticks) / 1000.0f / (pe->field_1C * b);
                } else {
                    u32 ticks = (*(u32*)0x800000F8 >> 2) / 1000;
                    b = 60.0f * (f32)((gp->renderTime - entry->field_68) / ticks) / 1000.0f / (cur->field_1C * b);
                }
            }
        } else {
            entry->field_54 = idx;
        }
        entry->field_64 = a;
        {
            u32 ticks = (*(u32*)0x800000F8 >> 2) / 1000;
            f32 t = 1000.0f * (b * (pe->field_1C * a)) / 60.0f;
            entry->field_68 = (u64)((f32)gp->renderTime - (f32)ticks * t);
        }
    }
    switch (entry->flags & 0x300) {
        case 0x100:
        case 0x200:
        case 0x300:
            animPoseSetAnim(entry->npc->poseId, pe->anim, 1);
            animPoseMain(entry->npc->poseId);
            break;
    }
}

// NONMATCHING (90.33%): logic complete; remaining diff is instruction scheduling around
// the oldflags&4 e-computation and the sdata2 constant-pool labels.
void yuunpc_pose_change(YuunpcEntry* entry, s16 animIndex, f32 a, f32 b) {
    YuunpcPoseEntry* cur = &yuunpc_wp->poseEntries[entry->field_54];
    YuunpcPoseEntry* pe = &yuunpc_wp->poseEntries[animIndex];

    if (a <= 0.0f) {
        entry->flags |= 4;
        entry->field_54 = animIndex;
        entry->field_64 = 1.0f;
        {
            u32 ticks = (*(u32*)0x800000F8 >> 2) / 1000;
            f32 t = 1000.0f * (b * (pe->field_1C * entry->field_64)) / 60.0f;
            entry->field_68 = (u64)((f32)ticks * t);
        }
    } else {
        u16 oldflags = entry->flags;
        entry->flags &= ~4;
        if (entry->field_54 == animIndex) {
            b = entry->field_64;
            if (b == a) {
                return;
            }
            {
                if (oldflags & 4) {
                    u32 ticks = (*(u32*)0x800000F8 >> 2) / 1000;
                    b = 60.0f * (f32)(entry->field_68 / ticks) / 1000.0f / (pe->field_1C * b);
                } else {
                    u32 ticks = (*(u32*)0x800000F8 >> 2) / 1000;
                    b = 60.0f * (f32)((gp->renderTime - entry->field_68) / ticks) / 1000.0f / (cur->field_1C * b);
                }
            }
        } else {
            entry->field_54 = animIndex;
        }
        entry->field_64 = a;
        {
            u32 ticks = (*(u32*)0x800000F8 >> 2) / 1000;
            f32 t = 1000.0f * (b * (pe->field_1C * a)) / 60.0f;
            entry->field_68 = (u64)((f32)gp->renderTime - (f32)ticks * t);
        }
    }
    switch (entry->flags & 0x300) {
        case 0x100:
        case 0x200:
        case 0x300:
            animPoseSetAnim(entry->npc->poseId, pe->anim, 1);
            animPoseMain(entry->npc->poseId);
            break;
    }
}


// NONMATCHING (97.91%): logic complete; the inlined pose_change now uses the static copy below,
// so only register allocation differs (the original keeps f23 and r21-r24 live, ours f24-r24).
// The dat_yuu_00014ec8/dat_yuu_00014edc block copy is byte-identical.
void ext_yuunpc_minigame1_init(void) {
    s32 i;
    f32 arr1[5] = { 260.0f, 220.0f, 180.0f, 140.0f, 100.0f };
    f32 arr2[8] = { -7.0f, -1.0f, 5.0f, -5.0f, 1.0f, 7.0f, -3.0f, 3.0f };
    ExtPose* pose;
    YuunpcEntry* entry;

    yuunpc_wp = _mapAlloc(0xA94);
    memset(yuunpc_wp, 0, 0xA94);
    yuunpc_wp->count = extGetPoseNum();
    yuunpc_wp->entries = _mapAlloc(yuunpc_wp->count * 0x70);
    memset(yuunpc_wp->entries, 0, yuunpc_wp->count * 0x70);
    yuunpc_pose_init((YuunpcPoseArg*)ext_yuunpc_minigame1_data);

    pose = extGetPosePtr();
    entry = yuunpc_wp->entries;
    for (i = 0; i < yuunpc_wp->count; i++, pose++, entry++) {
        s32 q = i / 5;
        f32 t;
        f32 y;
        s32 idx;

        entry->field_54 = yuunpc_wp->field_1C;
        entry->field_0 = i;
        entry->flags |= 1;
        pose->flags |= 7;
        PSMTXIdentity((MtxPtr)pose->matrix);

        t = 60.0f * (f32)(i % 5) - 560.0f;
        y = 30.0f + t;
        ((MtxPtr)pose->matrix)[0][3] = 16.666666f * (f32)q - 500.0f;
        ((MtxPtr)pose->matrix)[1][3] = arr1[(s32)((y - -560.0f) / 60.0f)];
        ((MtxPtr)pose->matrix)[2][3] = y + arr2[q % 8];

        pose->field_0x4 = (f32)(irand(2) * 180 + 90);

        switch (irand(1000) % 4) {
            case 0:
                idx = yuunpc_pose_check("c_kinopio", "KNP_S_1");
                entry->field_5C = idx;
                entry->field_5A = idx;
                entry->field_58 = idx;
                entry->field_56 = idx;
                break;
            case 1:
                idx = yuunpc_pose_check("c_kinopiko", "KNF_S_1");
                entry->field_5C = idx;
                entry->field_5A = idx;
                entry->field_58 = idx;
                entry->field_56 = idx;
                break;
            case 2:
                idx = yuunpc_pose_check("c_roten", "S_1");
                entry->field_5C = idx;
                entry->field_5A = idx;
                entry->field_58 = idx;
                entry->field_56 = idx;
                break;
            case 3:
                idx = yuunpc_pose_check("c_jugemu", "JGM_S_1");
                entry->field_5C = idx;
                entry->field_5A = idx;
                entry->field_58 = idx;
                entry->field_56 = idx;
                break;
        }

#ifdef MUST_MATCH
        yuunpc_pose_change_sub(entry, entry->field_56, 1.0f, (f32)rand() / 32767.0f);
#else
        yuunpc_pose_change(entry, entry->field_56, 1.0f, (f32)rand() / 32767.0f);
#endif
    }
}

s32 yuunpc_pose_check(const char* name, const char* anim) {
    YuunpcPoseEntry* entry = yuunpc_wp->poseEntries;
    s32 i;
    for (i = 0; i < yuunpc_wp->field_1C; entry++) {
        if (strcmp(entry->name, name) == 0 && strcmp(entry->anim, anim) == 0) {
            break;
        }
        i++;
    }
    if (i >= yuunpc_wp->field_1C) {
        return -1;
    }
    return i;
}

// The first while is a do-while so the loop's `arg++`/`arg->name` test folds into the original's
// `lwzu`; the final scan uses a fresh counter variable so the allocator picks the original's registers.
// The chusan index store is `= j` (the matched entry's index), which is what the original stores.
void yuunpc_pose_init(YuunpcPoseArg* arg) {
    char name[0x20];
    char anim[0x20];
    s32 i = 0;
    s32 idx;
    s32 j;

    strcpy(name, "");
    strcpy(anim, "");
    yuunpc_wp->field_1C = -1;
    do {
        if (strcmp(arg->name, name) != 0) {
            strcpy(name, arg->name);
            strcpy(anim, "");
        }
        if (strcmp(arg->anim, anim) != 0) {
            strcpy(anim, arg->anim);
            idx = i;
            yuunpc_wp->field_1C += 1;
        }
        strcpy(yuunpc_wp->poseEntries[yuunpc_wp->field_1C].name, name);
        strcpy(yuunpc_wp->poseEntries[yuunpc_wp->field_1C].anim, anim);
        yuunpc_wp->poseEntries[yuunpc_wp->field_1C].field_24 = idx;
        yuunpc_wp->poseEntries[yuunpc_wp->field_1C].field_28 = (i - idx) + 1;
        yuunpc_wp->poseEntries[yuunpc_wp->field_1C].field_1C += 1.0f;
        yuunpc_wp->poseEntries[yuunpc_wp->field_1C].field_18 = 1;
        yuunpc_wp->poseEntries[yuunpc_wp->field_1C].field_20 = 0.0f;
        yuunpc_wp->poseEntries[yuunpc_wp->field_1C].field_30 = 2.0f;
        i++;
        arg++;
    } while (arg->name != NULL);
    yuunpc_wp->field_1C += 1;
    yuunpc_wp->field_18 = yuunpc_wp->field_1C;
    arg++;
    while (arg->name != NULL) {
        strcpy(yuunpc_wp->poseEntries[yuunpc_wp->field_1C].name, arg->name);
        strcpy(yuunpc_wp->poseEntries[yuunpc_wp->field_1C].anim, arg->anim);
        yuunpc_wp->poseEntries[yuunpc_wp->field_1C].field_24 = idx;
        yuunpc_wp->poseEntries[yuunpc_wp->field_1C].field_28 = 1;
        yuunpc_wp->poseEntries[yuunpc_wp->field_1C].field_1C = arg->field_8;
        yuunpc_wp->poseEntries[yuunpc_wp->field_1C].field_18 = 1;
        yuunpc_wp->poseEntries[yuunpc_wp->field_1C].field_20 = 0.0f;
        yuunpc_wp->poseEntries[yuunpc_wp->field_1C].field_30 = 1.0f;
        yuunpc_wp->poseEntries[yuunpc_wp->field_1C].flags |= 1;
        yuunpc_wp->field_1C += 1;
        arg++;
    }
    for (j = 0; j < yuunpc_wp->field_18; j++) {
        if (strcmp(yuunpc_wp->poseEntries[j].name, "c_tyusan") == 0 && strcmp(yuunpc_wp->poseEntries[j].anim, "CHU_S_1") == 0) {
            yuunpc_wp->field_20 = j;
            yuunpc_wp->poseEntries[j].field_1C = 224.0f;
        }
    }
}

// NONMATCHING (86.55%): register allocation differs and the str_00000EBC rodata address is off because
// ext_yuunpc_minigame4_init_sub (the other user of that literal) is not implemented yet; logic is right.
void yuunpc_maphit_init(void) {
    s8 str[3] = "\x81\xA1";
    s8* hp = (s8*)hitdata;
    s32 k;
    s32 i;
    s32 j;

    for (i = 0; i < 72; i++) {
        yuunpc_wp->map[6 * 72 + i] = (hp[i * 2] - '0') * 10 + (hp[i * 2 + 1] - '0');
    }
    for (i = 0; i < 72; i++) {
        s16 value = -((hp[i + 0x90] - '0') * 10 + (hp[i + 0x91] - '0'));
        yuunpc_wp->map[5 * 72 + i] = value;
        yuunpc_wp->map[4 * 72 + i] = value;
        yuunpc_wp->map[3 * 72 + i] = value;
        yuunpc_wp->map[2 * 72 + i] = value;
        yuunpc_wp->map[1 * 72 + i] = value;
        yuunpc_wp->map[0 * 72 + i] = value;
    }
    for (i = 0; i < 6; i++) {
        s8* row = hp + i * 0x90;
        for (j = 0; j < 72; j++) {
            if (row[j * 2 + 0x120] == str[0] && row[j * 2 + 0x121] == str[1]) {
                yuunpc_wp->map[i * 72 + j] = -500;
            }
        }
    }
}
// clang-format on
