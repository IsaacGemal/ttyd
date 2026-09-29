#include <dolphin/types.h>
#pragma force_active on


extern const char str_803019E0[24];
extern const char str_803019F8[24];
extern const char str_80301A10[28];
extern const char str_80301A2C[28];
extern const char str_80301A48[36];
extern const char str_80301A6C[32];
extern const char str_80301A8C[32];
extern const char str_80301AAC[32];
extern const char str_80301ACC[40];
extern const char str_80301AF4[28];
extern const char str_80301B10[96];
extern const f64 double_to_int_mask_80301b70[1];
extern const u32 dat_80422600[1];
extern const char str_80422604[4];
extern const f32 float_0_80422608[1];
extern const f32 float_608_8042260c[1];
extern const f32 float_480_80422610[1];
extern const f32 float_1_80422614[1];
extern const f32 float_0p5_80422618[1];
static const u32 pad_sdata2_19[1];

const char str_803019E0[24] = "---- OS_ERROR_DSI ----\n";

const char str_803019F8[24] = "---- OS_ERROR_ISI ----\n";

const char str_80301A10[28] = "---- OS_ERROR_PROGRAM ----\n";

const char str_80301A2C[28] = "---- Context 0x%08x ----\n\000\000";

const char str_80301A48[36] = "r%-2d  = 0x%08x r%-2d  = 0x%08x\n\000\000\000";

const char str_80301A6C[32] = "LR   = 0x%08x CR   = 0x%08x\n\000\000\000";

const char str_80301A8C[32] = "SRR0 = 0x%08x SRR1 = 0x%08x\n\000\000\000";

const char str_80301AAC[32] = "DSISR= 0x%08x DAR  = 0x%08x\n\000\000\000";

const char str_80301ACC[40] = "Address:      Back Chain    LR Save\n\000\000\000";

const char str_80301AF4[28] = "0x%08x:   0x%08x    0x%08x\n";

const char str_80301B10[96] = "Instruction at 0x%x\n(read from SRR0) attempted to access\ninvalid address 0x%x (read from DAR)\n\000";

const f64 double_to_int_mask_80301b70[1] = {
    4503599627370496
};

const u32 dat_80422600[1] = {
    0x000000FF
};

const char str_80422604[4] = "\n\000\000";

const f32 float_0_80422608[1] = {
    0.0
};

const f32 float_608_8042260c[1] = {
    608.0
};

const f32 float_480_80422610[1] = {
    480.0
};

const f32 float_1_80422614[1] = {
    1.0
};

const f32 float_0p5_80422618[1] = {
    0.5
};

static const u32 pad_sdata2_19[1] = {
    0x00000000
};
#pragma force_active reset
