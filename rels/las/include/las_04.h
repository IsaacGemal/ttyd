#pragma once

#include <dolphin/mtx.h>
#include <dolphin/types.h>

#include "evt/evt_cmd.h"

typedef struct FirebarEntry FirebarEntry;

extern int __float_nan[];

extern const Vec vec3_las_00011ed4;
extern const Vec vec3_las_00011ee0;
extern const f32 float_0p8_las_00011f54;
extern const f32 float_1_las_00011f58;
extern const f32 float_550_las_00011f5c;
extern const f32 float_70_las_00011f60;
extern const f64 double_to_int_las_00011f68;
extern const f32 zero_las_00011f70;
extern const f32 float_2_las_00011f74;
extern const f32 float_6p2832_las_00011f78;
extern const f32 float_360_las_00011f7c;
extern const f32 float_0p625_las_00011f80;
extern const f32 float_0p375_las_00011f84;
extern const f32 float_0p4_las_00011f88;
extern const f32 float_0p2_las_00011f8c;
extern const f32 float_17_las_00011f90;
extern const f32 float_neg1_las_00011f94;
extern const char str_00000668[];
extern const char str_00000674[];
extern const f32 float_10_las_00011fac;
extern const f32 float_120_las_00011fb0;
extern const f32 float_6_las_00011fb4;
extern const f32 float_0p05_las_00011fb8;
extern const f32 float_20_las_00011fbc;
extern const f32 float_0p5_las_00011fc0;
extern const f32 float_15_las_00011fc4;
extern const f32 float_5_las_00011fc8;
extern const f64 double_0p5_las_00011fd0;
extern const f64 double_3_las_00011fd8;
extern const f64 zero_las_00011fe0;

typedef struct FirebarSegment {
    s32 poseId;   // 0x00
    Vec position; // 0x04
} FirebarSegment; // 0x10

typedef struct FirebarDesc {
    u8 type;     // 0x00
    u8 count;    // 0x01
    u16 pad_02;  // 0x02
    Vec position; // 0x04
    f32 angleSpeed; // 0x10
    f32 field_0x14; // 0x14
    f32 speed;      // 0x18
    void (*callback)(FirebarEntry*, s32); // 0x1C
    f32 field_0x20; // 0x20
    f32 baseRadius; // 0x24
} FirebarDesc; // 0x28

struct FirebarEntry {
    u8 type;              // 0x00
    u8 count;             // 0x01
    u16 flags;            // 0x02
    Vec position;         // 0x04
    f32 prevAngleDiff;    // 0x10
    f32 angle;            // 0x14
    f32 speed;            // 0x18
    f32 angleSpeed;       // 0x1C
    f32 field_0x20;       // 0x20
    f32 scale;            // 0x24
    f32 targetX;          // 0x28
    f32 baseRadius;       // 0x2C
    void (*callback)(FirebarEntry*, s32); // 0x30
    s32 counter;          // 0x34
    s32 sfxId;            // 0x38
    s32 state;            // 0x3C
    f32 pulsePhase;       // 0x40
    FirebarSegment* segments; // 0x44
    FirebarDesc* desc;    // 0x48
}; // 0x4C

void firebar_cb(FirebarEntry* entry, s32 kind);
USER_FUNC(coin_entry);
void firebar_disp(CameraId camera, void* param);
USER_FUNC(firebar_disp_evt);
USER_FUNC(firebar_entry);
void _firebar_trans(FirebarEntry* entry);
void _firebar_rot(FirebarEntry* entry);
s32 _firebar_hitCheck2(FirebarEntry* entry);
s32 _firebar_hitCheck(FirebarEntry* entry, FirebarSegment* seg);
EVT_DEF(firebar_coin);
EVT_DEF(firebar_evt);
EVT_DEF(las_04_init_evt);
