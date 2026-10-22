#include "eff/eff_fukidashi.h"
#include "driver/dispdrv.h"
#include "driver/icondrv.h"
#include "driver/mapdrv.h"
#include "manager/fontmgr.h"
#include "mario/mario.h"
#include "mario/mario_party.h"
#include "mario/mariost.h"
#include "memory.h"
#include "party.h"
#include "pmario_sound.h"
#include "system.h"

#include <dolphin/gx.h>
#include <dolphin/mtx.h>
#include "math_ext.h"

// pool_data: the original reaches this unit's .rodata (the Vec templates and strings) through one base
// register seeded from the first Vec template, so let MWCC pool it instead of rematerialising lis/addi.
#pragma pool_data on

// .sbss2: the 4-byte section-tail gap after effFukidashiDisp's colour templates.
#pragma section ".sdata" ".sbss2"
__declspec(section ".sdata") u8 gap_10_8042300C_sbss2[4];
#pragma section ".sdata" ".sbss"

__declspec(section ".data") u8 lbl_8036BEA0[8] ATTRIBUTE_ALIGN(16) = {
	0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
};

__declspec(section ".data") static u8 negone_one_374[8] = {
	0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
};

__declspec(section ".data") u8 lbl_8036BEB0[8] = {
	0x3F, 0x80, 0x00, 0x00, 0xBF, 0x80, 0x00, 0x00,
};

__declspec(section ".data") static u8 one_negone_373[8] = {
	0x3F, 0x80, 0x00, 0x00, 0xBF, 0x80, 0x00, 0x00,
};

__declspec(section ".data") u8 lbl_8036BEC0[8] = {
	0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
};

__declspec(section ".data") static u8 negone_one_360[8] = {
	0xBF, 0x80, 0x00, 0x00, 0x3F, 0x80, 0x00, 0x00,
};

__declspec(section ".data") u8 lbl_8036BED0[8] = {
	0x3F, 0x80, 0x00, 0x00, 0xBF, 0x80, 0x00, 0x00,
};

__declspec(section ".data") static u8 one_negone_359[8] = {
	0x3F, 0x80, 0x00, 0x00, 0xBF, 0x80, 0x00, 0x00,
};

u8 anim_data_tenten[] = {
	0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02,
	0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0xFF, 0x00,
};
u32 anim_data_bikkuri[] = {
	0x00000102, 0x03050405, 0x05050505, 0x05050505, 0x05050505, 0x05050505, 0x05050505,
	0x05050505, 0x05050505, 0x0505FF00,
};

u32 anim_data_hatena[] = {
	0x00000201, 0x03020303, 0x03030303, 0x03030303, 0x03030303, 0x03030303, 0x03030303,
	0x03030303, 0x03030303, 0x03FF0000,
};

u32 anim_data_loop[] = { 0x00F60000 };

u32 anim_data_moja[] = { 0x00000101, 0x02020303, 0xF6000000 };


static void* anim_data_table[] = {
	anim_data_bikkuri, anim_data_hatena, anim_data_bikkuri, anim_data_loop,
	anim_data_tenten, anim_data_moja, anim_data_loop,
};

u32 anim_data_bikkuri_2[] = { 0x00000102, 0x03050405, 0xEFFF0000 };

u32 anim_data_hatena_2[] = { 0x00000201, 0x030203F0, 0xFF000000 };

u32 anim_data_tenten_2[] = {
	0x00000101, 0x01010101, 0x01010101, 0x02020202, 0x02020202, 0x02020303, 0x03030303,
	0x03030303, 0xE0FF0000,
};

void* anim_data_table_2[] = {
	anim_data_bikkuri_2, anim_data_hatena_2, anim_data_bikkuri_2, anim_data_loop,
	anim_data_tenten_2, anim_data_moja, anim_data_loop,
	NULL, NULL, NULL,
};

__declspec(section ".sdata") u32 moja_model_color0_tbl[8] = {
	0x224EB7FF, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000,
};

__declspec(section ".sdata") u8 moja_model_dl_0_1[32] = {
	0x98, 0x00, 0x05, 0x1B, 0x0D, 0x00, 0x1A, 0x0D,
	0x00, 0x18, 0x0C, 0x00, 0x19, 0x0C, 0x00, 0x12,
	0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

__declspec(section ".sdata") u8 moja_model_dl_0_2[32] = {
	0x98, 0x00, 0x04, 0x1C, 0x0E, 0x00, 0x1A, 0x0D,
	0x00, 0x20, 0x0E, 0x00, 0x1B, 0x0D, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

__declspec(section ".sdata") u8 moja_model_dl_0_3[32] = {
	0x98, 0x00, 0x04, 0x1D, 0x0F, 0x00, 0x1C, 0x0E,
	0x00, 0x21, 0x0F, 0x00, 0x20, 0x0E, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

__declspec(section ".sdata") u8 moja_model_dl_0_4[32] = {
	0x98, 0x00, 0x04, 0x1E, 0x10, 0x00, 0x1D, 0x0F,
	0x00, 0x22, 0x10, 0x00, 0x21, 0x0F, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

__declspec(section ".sdata") u8 moja_model_dl_0_5[32] = {
	0x98, 0x00, 0x04, 0x1F, 0x06, 0x00, 0x1E, 0x10,
	0x00, 0x23, 0x06, 0x00, 0x22, 0x10, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

__declspec(section ".sdata") u8 moja_model_dl_0_6[32] = {
	0x98, 0x00, 0x04, 0x24, 0x11, 0x00, 0x1F, 0x06,
	0x00, 0x29, 0x11, 0x00, 0x23, 0x06, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

__declspec(section ".sdata") u8 moja_model_dl_0_7[32] = {
	0x98, 0x00, 0x04, 0x25, 0x12, 0x00, 0x24, 0x11,
	0x00, 0x2A, 0x12, 0x00, 0x29, 0x11, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

__declspec(section ".sdata") u8 moja_model_dl_0_8[32] = {
	0x98, 0x00, 0x04, 0x26, 0x13, 0x00, 0x25, 0x12,
	0x00, 0x2B, 0x13, 0x00, 0x2A, 0x12, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

__declspec(section ".sdata") u8 moja_model_dl_0_9[32] = {
	0x98, 0x00, 0x04, 0x27, 0x14, 0x00, 0x26, 0x13,
	0x00, 0x2C, 0x14, 0x00, 0x2B, 0x13, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

__declspec(section ".sdata") u8 moja_model_dl_0_10[32] = {
	0x98, 0x00, 0x04, 0x28, 0x15, 0x00, 0x27, 0x14,
	0x00, 0x2D, 0x15, 0x00, 0x2C, 0x14, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

__declspec(section ".sdata") u8 moja_model_dl_0_11[32] = {
	0x98, 0x00, 0x04, 0x2E, 0x0D, 0x00, 0x28, 0x15,
	0x00, 0x33, 0x0D, 0x00, 0x2D, 0x15, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

__declspec(section ".sdata") u8 moja_model_dl_0_12[32] = {
	0x98, 0x00, 0x04, 0x2F, 0x0E, 0x00, 0x2E, 0x0D,
	0x00, 0x34, 0x0E, 0x00, 0x33, 0x0D, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

__declspec(section ".sdata") u8 moja_model_dl_0_13[32] = {
	0x98, 0x00, 0x04, 0x30, 0x0F, 0x00, 0x2F, 0x0E,
	0x00, 0x35, 0x0F, 0x00, 0x34, 0x0E, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

__declspec(section ".sdata") u8 moja_model_dl_0_14[32] = {
	0x98, 0x00, 0x04, 0x31, 0x10, 0x00, 0x30, 0x0F,
	0x00, 0x36, 0x10, 0x00, 0x35, 0x0F, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

__declspec(section ".sdata") u8 moja_model_dl_0_15[32] = {
	0x98, 0x00, 0x04, 0x32, 0x06, 0x00, 0x31, 0x10,
	0x00, 0x37, 0x06, 0x00, 0x36, 0x10, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

__declspec(section ".sdata") u8 moja_model_dl_0_16[32] = {
	0x98, 0x00, 0x04, 0x38, 0x11, 0x00, 0x32, 0x06,
	0x00, 0x3D, 0x11, 0x00, 0x37, 0x06, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

__declspec(section ".sdata") u8 moja_model_dl_0_17[32] = {
	0x98, 0x00, 0x04, 0x39, 0x12, 0x00, 0x38, 0x11,
	0x00, 0x3E, 0x12, 0x00, 0x3D, 0x11, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

__declspec(section ".sdata") u8 moja_model_dl_0_18[32] = {
	0x98, 0x00, 0x04, 0x3A, 0x13, 0x00, 0x39, 0x12,
	0x00, 0x3F, 0x13, 0x00, 0x3E, 0x12, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

__declspec(section ".sdata") u8 moja_model_dl_0_19[32] = {
	0x98, 0x00, 0x04, 0x3B, 0x14, 0x00, 0x3A, 0x13,
	0x00, 0x40, 0x14, 0x00, 0x3F, 0x13, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

__declspec(section ".sdata") u8 moja_model_dl_0_20[32] = {
	0x98, 0x00, 0x04, 0x3C, 0x16, 0x00, 0x3B, 0x14,
	0x00, 0x41, 0x16, 0x00, 0x40, 0x14, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

__declspec(section ".sdata") u8 moja_model_dl_0_size_tbl[24] = {
	0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
};
__declspec(section ".data") u32 moja_model_vertex_tbl[104] = {
	0xF6010000, 0x0000F757, 0x030004FF, 0xFB000600, 0x08A90000, 0x09000A00,
	0x05000C00, 0x08A908A9, 0x0F000500, 0x0A001200, 0x0000F601, 0x0E000000,
	0xF7571100, 0x04FFFB00, 0x140008A9, 0x00001700, 0x0A000500, 0x19FF08A9,
	0x08A91D00, 0x05000A00, 0x20000000, 0x07FD1500, 0xF8810280, 0x1800F303,
	0xFB011B00, 0xF101F381, 0x1E00F303, 0xEE032100, 0xF88107FD, 0x2300F881,
	0x02802600, 0xF303FB01, 0x2900F101, 0xF3812C00, 0xF303EE03, 0x2F00F881,
	0xEC013200, 0x0000EC01, 0x24000000, 0xEFD22700, 0x0BC1EFD2, 0x35000BC1,
	0xF9D22A00, 0x1305062E, 0x2D001305, 0x102E3000, 0x0BC11400, 0x33000000,
	0xF9D23800, 0x1305062E, 0x3B001305, 0x102E3E00, 0x0BC11400, 0x41000000,
	0x0F393600, 0xF14F02B9, 0x3900E83A, 0xF3473C00, 0xE83AE6C7, 0x3F00F14F,
	0xE2004200, 0x00000F39, 0x4400F14F, 0x02B94700, 0xE83AF347, 0x49FFE83A,
	0xE6C74CFF, 0xF14FE200, 0x50000000, 0xE7BB44FF, 0x11A2F6BB, 0x47FF1C88,
	0x09454B00, 0x1C881845, 0x4E0011A2, 0x1E0050FF, 0x0000E7BB, 0x52FF11A2,
	0xF6BB55FF, 0x1C880945, 0x58FF1C88, 0x18455BFF, 0x11A21E00, 0x5EFF0000,
	0x175053FF, 0xEB6E05D0, 0x56FFDEB7, 0xF03059FF, 0xDEB7DEB0, 0x5CFFEB6E,
	0xD8005FFF, 0x00001750, 0x61FFEB6E, 0x05D064FF, 0xDEB7F030, 0x67FFDEB7,
	0xDEB06AFF, 0xEB6ED800, 0x6DFF0000, 0x00000000, 0x00000000, 0x00000000,
	0x00000000, 0x00000000,
};

__declspec(section ".data") u32 moja_model_normal_tbl[24] = {
	0xC30010C9, 0x0020E000, 0x37000040, 0x20003737, 0x00204000, 0x003700E0,
	0x2000C900, 0x00C0E000, 0xC9C900E0, 0xC10001CD, 0x0025ED00, 0x3C13003C,
	0x33002533, 0x00DB1300, 0xC4ED00C4, 0xCD00DBC0, 0x0000C400, 0xED000000,
	0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

__declspec(section ".data") u32 moja_model_dl_0_0[24] = {
	0x98001918, 0x0C00120B, 0x00170B00, 0x110A0016, 0x0A001009, 0x00150900,
	0x0F080014, 0x08000E07, 0x00130700, 0x0606000D, 0x06000505, 0x000C0500,
	0x0404000B, 0x04000303, 0x000A0300, 0x02020009, 0x02000101, 0x00080100,
	0x00000007, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
};

__declspec(section ".data") void* moja_model_dl_0_tbl[21] = {
	moja_model_dl_0_0,
	moja_model_dl_0_1,
	moja_model_dl_0_2,
	moja_model_dl_0_3,
	moja_model_dl_0_4,
	moja_model_dl_0_5,
	moja_model_dl_0_6,
	moja_model_dl_0_7,
	moja_model_dl_0_8,
	moja_model_dl_0_9,
	moja_model_dl_0_10,
	moja_model_dl_0_11,
	moja_model_dl_0_12,
	moja_model_dl_0_13,
	moja_model_dl_0_14,
	moja_model_dl_0_15,
	moja_model_dl_0_16,
	moja_model_dl_0_17,
	moja_model_dl_0_18,
	moja_model_dl_0_19,
	moja_model_dl_0_20
};

__declspec(section ".data") u32 chuchu_dt[45] = {
	0x00000000, 0x3F800000, 0x3F800000, 0xBE2D5555, 0x3F58EEEF, 0x3F493334,
	0xBEBC71C7, 0x3F2D27D3, 0x3F0BBBBC, 0xBF020000, 0x3F0ACCCD, 0x3EB73333,
	0xBF000000, 0x3F000000, 0x3E99999A, 0xBE8FD0AB, 0x3F143ACD, 0x3ED40949,
	0x3DAC0831, 0x3F3D07C8, 0x3F247454, 0x3EFA89E6, 0x3F6DA2E2, 0x3F69E1B1,
	0x3F546C51, 0x3F8CA406, 0x3F940949, 0x3F800000, 0x3F99999A, 0x3FA66666,
	0x3F746509, 0x3F9CA062, 0x3FAA6A96, 0x3F439581, 0x3F99F9B8, 0x3FA6293C,
	0x3EFDF3B6, 0x3F93404F, 0x3F9C0EBF, 0x3E5FEA28, 0x3F8A0ED6, 0x3F8E8788,
	0x00000000, 0x3F800000, 0x3F800000,
};





void calc_pos(effFukidashiData* data, s32 index) {
	CameraEntry* cam;
	Vec axis;
	Vec temp;
	Mtx mtx;
	MarioWork* mario;
	PartyEntry* party;

	cam = camGetPtr(CAMERA_3D);
	temp = (Vec){ sin(MTXDegToRad(cam->viewYaw)), 0.0f, -(f32)cos(MTXDegToRad(cam->viewYaw)) };
	axis = temp;

	PSMTXRotAxisRad(mtx, &axis, MTXDegToRad(-(data->angleBase - 20.0f + (f32)(index * 20))));

	if ((s32)data->target == -1) {
		mario = marioGetPtr();
		data->position.x = mario->position.x + data->offset.x + mtx[0][1] * (16.0f + data->height);
		data->position.y = mario->position.y + data->offset.y + mtx[1][1] * (16.0f + data->height);
		data->position.z = mario->position.z + data->offset.z + mtx[2][1] * (16.0f + data->height);
	} else if ((s32)data->target == -2) {
		party = partyGetPtr(marioGetPartyId());
		if (party != NULL) {
			data->position.x = party->position.x + data->offset.x + mtx[0][1] * (16.0f + data->height);
			data->position.y = party->position.y + data->offset.y + mtx[1][1] * (16.0f + data->height);
			data->position.z = party->position.z + data->offset.z + mtx[2][1] * (16.0f + data->height);
		}
	} else if ((s32)data->target == -3) {
		party = partyGetPtr(marioGetExtraPartyId());
		if (party != NULL) {
			data->position.x = party->position.x + data->offset.x + mtx[0][1] * (16.0f + data->height);
			data->position.y = party->position.y + data->offset.y + mtx[1][1] * (16.0f + data->height);
			data->position.z = party->position.z + data->offset.z + mtx[2][1] * (16.0f + data->height);
		}
	} else if (data->target) {
		mario = (MarioWork*)data->target;
		data->position.x = mario->position.x + data->offset.x + mtx[0][1] * (16.0f + data->height);
		data->position.y = mario->position.y + data->offset.y + mtx[1][1] * (16.0f + data->height);
		data->position.z = mario->position.z + data->offset.z + mtx[2][1] * (16.0f + data->height);
	} else {
		data->position.x = data->offset.x + mtx[0][1] * (16.0f + data->height);
		data->position.y = data->offset.y + mtx[1][1] * (16.0f + data->height);
		data->position.z = data->offset.z + mtx[2][1] * (16.0f + data->height);
	}

	data->tiltZ = data->angleBase - 20.0f + (f32)(index * 20);
	data->field_0x2C = 0.5f * (data->height / 12.0f) + 0.5f;
}

// This unit is built with pool_data on: MWCC reaches all of its .rodata through one base register
// (calc_pos's Vec template). The Vec literals' templates sit in use order: calc_pos's, the four sfx
// positions of effFukidashiEntry, then initializeVec's (defined just before its user,
// effFukidashiMain, so its template follows the others) and effFukidashiDisp's.
//
// The original compares the c==-2/-3 fallthrough as unsigned (cmplwi); `c != 0` emits cmpwi.
#ifdef MUST_MATCH
#define FUKI_C_NONZERO(x) ((u32)(x) != 0)
#else
#define FUKI_C_NONZERO(x) ((x) != 0)
#endif

EffectEntry* effFukidashiEntry(s32 a, s32 b, s32 c, s32 d, f32 e, f32 f, f32 g, f32 h, f32 i) {
	EffectEntry* entry;
	effFukidashiData* data;

	entry = effEntry();
	entry->type = "Fukidashi";
	entry->count = 1;
	data = __memAlloc(HEAP_EFFECT, sizeof(effFukidashiData));
	entry->userdata = data;
	entry->callback = effFukidashiMain;

	data->type = a;
	data->number = b;
	if (d < 0) {
		d = 10000;
	}
	data->duration = d;
	data->timer = d;
	data->animFrame = 0;
	data->target = (void*)c;
	data->spinY = 0.0f;
	data->alpha = 0xFF;
	data->camId = CAMERA_3D;
	data->offset.x = e;
	data->offset.y = 3.0f + f;
	data->offset.z = g;
	data->angleBase = i;
	data->height = h;
	data->phase = 0;

	calc_pos(data, 1);

	if (a == 6) {
		data->spinY = -450.0f;
	}

	if ((s32)data->target == -1) {
		MarioWork* mario = marioGetPtr();
		e += mario->position.x;
		f += mario->position.y;
		g += mario->position.z;
	} else if ((s32)data->target == -2) {
		PartyEntry* party = partyGetPtr(marioGetPartyId());
		if (party != NULL) {
			e += party->position.x;
			f += party->position.y;
			g += party->position.z;
		}
	} else if ((s32)data->target == -3) {
		PartyEntry* party = partyGetPtr(marioGetExtraPartyId());
		if (party != NULL) {
			e += party->position.x;
			f += party->position.y;
			g += party->position.z;
		}
	} else if (data->target != NULL) {
		MarioWork* mario = (MarioWork*)data->target;
		e = mario->position.x;
		f = mario->position.y;
		g = mario->position.z;
	}

	if (c == -2 || c == -3) {
		switch (a) {
		case 0: {
			psndSFXOn_3D((const char*)0x5D, &(Vec){ e, f, g });
			break;
		}
		case 1: {
			psndSFXOn_3D((const char*)0x5E, &(Vec){ e, f, g });
			break;
		}
		}
	} else if (FUKI_C_NONZERO(c) && c != -1) {
		switch (a) {
		case 0: {
			psndSFXOn_3D("SFX_EVT_NPC_BIKKURI_MARK1", &(Vec){ e, f, g });
			break;
		}
		case 1: {
			psndSFXOn_3D("SFX_EVT_NPC_QUESTION_MARK1", &(Vec){ e, f, g });
			break;
		}
		}
	}

	return entry;
}

// Local copy of system.c's inline helper so it inlines here (as in eff_queen.c).
inline Vec initializeVec(Vec* input) {
	Vec temp = (Vec){ 0.0f, 0.0f, 0.0f };
	temp.x = input->x;
	temp.y = input->y;
	temp.z = input->z;
	return temp;
}

void effFukidashiMain(EffectEntry* effect) {
	effFukidashiData* data = effect->userdata;
	Vec disp;
	s32 type;
	s32 anim;

	disp = initializeVec(&data->position);
	type = data->type;

	if (data->duration == 0) {
		anim = ((s8*)anim_data_table[type])[data->animFrame / 2];
	} else {
		anim = ((s8*)anim_data_table_2[type])[data->animFrame / 2];
	}
	data->animFrame++;
	data->animCell = anim;

	if (anim < 0) {
		if (anim == -1) {
			effDelete(effect);
			return;
		}
		data->animFrame = (-10 - anim) * 2;
		if (data->duration == 0) {
			anim = ((s8*)anim_data_table[type])[data->animFrame / 2];
		} else {
			anim = ((s8*)anim_data_table_2[type])[data->animFrame / 2];
		}
		data->animFrame++;
		data->animCell = anim;
	}

	if (type == 6) {
		switch (data->phase) {
			case 0:
				data->spinY += 22.5f;
				if (data->spinY >= 0.0f) {
					data->spinY = 0.0f;
					data->phase++;
				}
				break;
			case 0x78:
				data->spinY -= 9.0f;
				if (data->spinY <= -90.0f) {
					effDelete(effect);
					return;
				}
				break;
			default:
				data->phase++;
				break;
		}
	} else {
		if (data->duration > 0) {
			if (data->timer < 10000) {
				data->timer--;
			}
			if (data->timer < 0) {
				effDelete(effect);
				return;
			}
		}
	}

	if (data->target) {
		calc_pos(data, 1);
	}

	if (data->type == 4) {
		dispEntry(data->camId, dispCalcZ(&disp), 2, effFukidashiDisp, effect);
	} else {
		dispEntry(data->camId, dispCalcZ(&disp), 8, effFukidashiDisp, effect);
	}
}

// The frame (0x390), prologue (stmw r19), the single .data base r29 = lbl_8036BEA0 and every GPR/FPR
// match the original. The register/stack allocation is pinned by these source facts:
//  - the unit's .data (the 8 compiler literal pairs, anim_data_tenten, moja_model_* and chuchu_dt) is
//    now defined here in the original address order, so vertex/normal/dl_0_tbl/chuchu_dt are reached
//    through r29 instead of lis/addi pairs;
//  - the locals alphaA/alphaB, data, alpha, i/j, count are declared in that order (MWCC gives a
//    later-declared callee-saved variable the lower register), putting count/i in r23/r24, alpha r25,
//    data r26, alphaB r27, alphaA r28 as retail does;
//  - the Mtx locals are declared in the order m5, m3, m0, m6, m1, m4, m2, m8, m9, then GXLightObj
//    light, then m7. MWCC gives the first-declared Mtx the highest stack slot (0x2c8) and steps down
//    0x30, so this order makes the first loop's m0=0x268, m1=0x208, m2=0x1a8, m3=0x298, m4=0x1d8,
//    m5=0x2c8, m6=0x238, m8=0x178, m9=0x148, light=0x108 and m7=0xd8, matching retail; the case-2
//    scratch uses m7 and the case-0/1 `PSMTXConcat(m3, m0, m0)` writes back to m0, as retail does;
//  - the 15 colours are compound literals written directly in each GXSet* argument (no separate
//    `color` local), so they land as the original's interleaved 8-byte [template copy, argument copy]
//    pairs instead of two 4-byte banks.
//  - loading `cam = camGetPtr(camId)` before `alpha = data->alpha` makes the compiler hoist the
//    .data base `lis r5, lbl_8036BEA0@ha` ahead of the userdata load (`lwz r26, 0xc(r4)`), as retail;
//  - computing `count = 1` before the `mapSetMaterialFog()` call matches retail's `li r23, 0x1`;
//  - the display-list loop lives in its own `static inline` (MWCC colours a loop's induction variables
//    differently when it is a named call): this puts the table pointer in r19, `m` in r20 and the size
//    pointer in r21 -- retail's colours -- and the chuchu_dt base falls into r19 as well;
//  - `-width / 2.0f`, not `* 0.5f`, is what lands the last FPR in f1: the division folds to the same
//    `fmuls` but negotiates the negation/constant operands the other way round.
static inline void fukiDrawDl(void) {
	u32 m;
	for (m = 0; m < 0x15; m++) {
		GXCallDisplayList((void*)moja_model_dl_0_tbl[m], moja_model_dl_0_size_tbl[m] << 5);
	}
}

void effFukidashiDisp(CameraId camId, void* param) {
	EffectEntry* effect = param;
	s32 alphaA, alphaB;
	effFukidashiData* data = effect->userdata;
	s32 alpha;
	s32 i, j;
	s32 count;
	CameraEntry* cam;
	GXTexObj tex;
	Mtx m5, m3, m0, m6, m1, m4, m2, m8, m9;
	GXLightObj light;
	Mtx m7;
	Vec vec;
	Vec v2;
	u32 n;
	s32 lang;
	f32 width;
	char buf[0x20];

	cam = camGetPtr(camId);
	alpha = data->alpha;
	count = 1;
	mapSetMaterialFog();

	if (data->spinY != 0.0f) {
		count = 2;
	}
	if (data->type == 2) {
		count = 2;
	}

	alphaA = (alpha * 0xB0) / 0xFF;
	alphaB = (alpha * 0xFF) / 0xFF;

	for (i = 0; i < count; i++) {
		PSMTXTrans(m0, data->position.x, data->position.y, data->position.z);
		PSMTXRotRad(m1, 'y', MTXDegToRad(-camGetPtr(camId)->viewYaw));
		PSMTXConcat(m0, m1, m2);
		PSMTXScale(m3, 0.8f, 0.8f, 0.8f);
		PSMTXConcat(m2, m3, m2);
		PSMTXScale(m3, 1.0f, 0.8f, 1.0f);
		PSMTXRotRad(m4, 'z', MTXDegToRad(data->tiltZ));
		PSMTXConcat(m2, m3, m3);
		PSMTXConcat(m3, m4, m5);
		PSMTXConcat(cam->view, m5, m5);
		GXSetCurrentMtx(GX_PNMTX0);
		if (i == 0) {
			GXSetCullMode(GX_CULL_BACK);
		} else {
			GXSetCullMode(GX_CULL_FRONT);
		}

		PSMTXIdentity(m0);
		GXLoadTexMtxImm(m0, GX_TEXMTX0, GX_MTX2x4);
		GXSetNumTevStages(1);
		GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
		GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
		GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
		GXSetZCompLoc(GX_TRUE);
		GXSetAlphaCompare(GX_ALWAYS, 0, GX_AOP_AND, GX_ALWAYS, 0);
		GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
		GXClearVtxDesc();
		GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
		GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
		GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
		GXSetNumChans(1);
		GXSetChanCtrl(GX_COLOR0A0, GX_FALSE, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_CLAMP, GX_AF_NONE);
		GXSetNumTexGens(1);
		GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0, GX_FALSE, GX_PTIDENTITY);

		for (j = 0; j < 2; j++) {
			if (j == 0) {
				effGetTexObj(5, &tex);
				GXLoadTexObj(&tex, GX_TEXMAP0);
				GXSetChanMatColor(GX_COLOR0A0, (GXColor){ 0, 0, 0, alpha });
				PSMTXTrans(m0, 1.3f, -1.3f, 0.0f);
				PSMTXConcat(m5, m0, m0);
				PSMTXRotRad(m6, 'y', MTXDegToRad(data->spinY));
				PSMTXConcat(m0, m6, m0);
				GXLoadPosMtxImm(m0, GX_PNMTX0);
				GXSetCurrentMtx(GX_PNMTX0);
			} else {
				effGetTexObj(4, &tex);
				GXLoadTexObj(&tex, GX_TEXMAP0);
				if (i == 0) {
					GXSetChanMatColor(GX_COLOR0A0, (GXColor){ 0xFF, 0xFF, 0xFF, alpha });
				} else {
					GXSetChanMatColor(GX_COLOR0A0, (GXColor){ 0xA0, 0xA0, 0xA0, alpha });
				}
				PSMTXTrans(m0, 0.0f, 0.0f, 0.0f);
				PSMTXConcat(m5, m0, m0);
				PSMTXRotRad(m6, 'y', MTXDegToRad(data->spinY));
				PSMTXConcat(m0, m6, m0);
				GXLoadPosMtxImm(m0, GX_PNMTX0);
				GXSetCurrentMtx(GX_PNMTX0);
			}

			GXBegin(GX_QUADS, GX_VTXFMT0, 4);
			GXPosition3f32(-16.0f, 13.0f, 0.0f);
			GXTexCoord2f32(0.0f, 0.0f);
			GXPosition3f32(16.0f, 13.0f, 0.0f);
			GXTexCoord2f32(2.0f, 0.0f);
			GXPosition3f32(16.0f, -19.0f, 0.0f);
			GXTexCoord2f32(2.0f, 1.0f);
			GXPosition3f32(-16.0f, -19.0f, 0.0f);
			GXTexCoord2f32(0.0f, 1.0f);
		}

		PSMTXScale(m8, 1.3f, 1.3f, 1.3f);
		PSMTXConcat(m5, m8, m5);

		switch (data->type) {
			case 0:
			case 1:
				GXSetNumTevStages(1);
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_C0);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
				switch (data->type) {
					case 0:
						effGetTexObj(6, &tex);
						GXLoadTexObj(&tex, GX_TEXMAP0);
						PSMTXScale(m3, 0.16666667f, 1.0f, 1.0f);
						PSMTXTrans(m0, (f32)data->animCell, 0.0f, 0.0f);
						PSMTXConcat(m3, m0, m0);
						GXLoadTexMtxImm(m0, GX_TEXMTX0, GX_MTX2x4);
						break;
					case 1:
						effGetTexObj(7, &tex);
						GXLoadTexObj(&tex, GX_TEXMAP0);
						PSMTXScale(m3, 0.25f, 1.0f, 1.0f);
						PSMTXTrans(m0, (f32)data->animCell, 0.0f, 0.0f);
						PSMTXConcat(m3, m0, m0);
						GXLoadTexMtxImm(m0, GX_TEXMTX0, GX_MTX2x4);
						break;
				}
				GXSetTevColor(GX_TEVREG0, (GXColor){ 0, 0, 0, alphaA });
				PSMTXTransApply(m5, m8, 1.0f, -1.0f, 0.0f);
				GXLoadPosMtxImm(m8, GX_PNMTX0);
				if (data->type == 0) {
					GXBegin(GX_QUADS, GX_VTXFMT0, 4);
					GXPosition3f32(-4.0f, 8.0f, 0.0f);
					GXTexCoord2f32(0.0f, 0.0f);
					GXPosition3f32(4.0f, 8.0f, 0.0f);
					GXTexCoord2f32(1.0f, 0.0f);
					GXPosition3f32(4.0f, -8.0f, 0.0f);
					GXTexCoord2f32(1.0f, 1.0f);
					GXPosition3f32(-4.0f, -8.0f, 0.0f);
					GXTexCoord2f32(0.0f, 1.0f);
				} else {
					GXBegin(GX_QUADS, GX_VTXFMT0, 4);
					GXPosition3f32(-8.0f, 16.0f, 0.0f);
					GXTexCoord2f32(0.0f, 0.0f);
					GXPosition3f32(8.0f, 16.0f, 0.0f);
					GXTexCoord2f32(1.0f, 0.0f);
					GXPosition3f32(8.0f, -8.0f, 0.0f);
					GXTexCoord2f32(1.0f, 1.0f);
					GXPosition3f32(-8.0f, -8.0f, 0.0f);
					GXTexCoord2f32(0.0f, 1.0f);
				}
				if (i == 0) {
					GXSetTevColor(GX_TEVREG0, (GXColor){ 0xFF, 0x00, 0x00, alphaB });
				} else {
					GXSetTevColor(GX_TEVREG0, (GXColor){ 0xA0, 0x00, 0x00, alphaB });
				}
				GXLoadPosMtxImm(m5, GX_PNMTX0);
				if (data->type == 0) {
					GXBegin(GX_QUADS, GX_VTXFMT0, 4);
					GXPosition3f32(-4.0f, 8.0f, 0.0f);
					GXTexCoord2f32(0.0f, 0.0f);
					GXPosition3f32(4.0f, 8.0f, 0.0f);
					GXTexCoord2f32(1.0f, 0.0f);
					GXPosition3f32(4.0f, -8.0f, 0.0f);
					GXTexCoord2f32(1.0f, 1.0f);
					GXPosition3f32(-4.0f, -8.0f, 0.0f);
					GXTexCoord2f32(0.0f, 1.0f);
				} else {
					GXBegin(GX_QUADS, GX_VTXFMT0, 4);
					GXPosition3f32(-8.0f, 16.0f, 0.0f);
					GXTexCoord2f32(0.0f, 0.0f);
					GXPosition3f32(8.0f, 16.0f, 0.0f);
					GXTexCoord2f32(1.0f, 0.0f);
					GXPosition3f32(8.0f, -8.0f, 0.0f);
					GXTexCoord2f32(1.0f, 1.0f);
					GXPosition3f32(-8.0f, -8.0f, 0.0f);
					GXTexCoord2f32(0.0f, 1.0f);
				}
				break;

			case 2:
				GXSetNumTevStages(1);
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL, GX_COLOR0A0);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_RASC);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_ZERO, GX_CA_ZERO, GX_CA_RASA);
				GXSetNumChans(1);
				GXSetChanCtrl(GX_COLOR0A0, GX_TRUE, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT0, GX_DF_CLAMP, GX_AF_NONE);
				GXSetChanAmbColor(GX_COLOR0A0, (GXColor){ 0xE0, 0xE0, 0xE0, alphaB });
				v2 = (Vec){ 0.0f, 0.0f, 0.0f };
				v2.x = data->position.x + 10.0f * (f32)cos((f32)gp->retraceCount / 10.0f);
				v2.y = data->position.y;
				v2.z = data->position.z - 10.0f * (f32)sin((f32)gp->retraceCount / 10.0f);
				vec = v2;
				PSMTXMultVec(camGetPtr(camId)->view, &vec, &vec);
				GXInitLightPos(&light, vec.x, vec.y, vec.z);
				GXInitLightColor(&light, (GXColor){ 0xFF, 0xFF, 0xFF, alphaB });
				GXInitLightAttn(&light, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
				GXLoadLightObjImm(&light, GX_LIGHT0);
				PSMTXTrans(m7, 5.0f, -8.0f, 0.0f);
				PSMTXConcat(m5, m7, m8);
				PSMTXRotRad(m7, 'z', 0.43633232f);
				PSMTXConcat(m8, m7, m8);
				PSMTXRotRad(m7, 'y', MTXDegToRad((f32)(gp->retraceCount * 10)));
				PSMTXConcat(m8, m7, m8);
				PSMTXScale(m7, 0.15f, 0.15f, 0.15f);
				PSMTXConcat(m8, m7, m8);
				GXLoadPosMtxImm(m8, GX_PNMTX0);
				PSMTXInvXpose(m8, m9);
				GXLoadNrmMtxImm(m9, GX_PNMTX0);
				GXSetZMode(GX_FALSE, GX_LEQUAL, GX_FALSE);
				GXClearVtxDesc();
				GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
				GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 8);
				GXSetArray(GX_VA_POS, moja_model_vertex_tbl, 6);
				GXSetVtxDesc(GX_VA_NRM, GX_INDEX8);
				GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_NRM, GX_NRM_XYZ, GX_S8, 6);
				GXSetArray(GX_VA_NRM, moja_model_normal_tbl, 3);
				GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);
				GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
				GXSetArray(GX_VA_CLR0, moja_model_color0_tbl, 4);
				fukiDrawDl();
				break;

			case 3:
				if (data->number >= 0 && data->number <= 0x63) {
					sprintf(buf, "%d", data->number);
					width = 10.0f + (f32)FontGetMessageWidth(buf);
					PSMTXRotRad(m6, 'y', MTXDegToRad(data->spinY));
					PSMTXScale(m3, 0.5f, 0.5f, 0.5f);
					PSMTXTrans(m0, -width / 2.0f, -10.0f, 0.0f);
					PSMTXConcat(m2, m6, m8);
					PSMTXConcat(m8, m3, m8);
					PSMTXConcat(m8, m0, m8);
					if (i == 0) {
						iconDispGxCol(m8, 0x400, 0x18B, (GXColor){ 0xFF, 0xFF, 0xFF, alpha });
					} else {
						iconDispGxCol(m8, 0x800, 0x18B, (GXColor){ 0xA0, 0xA0, 0xA0, alpha });
					}
					PSMTXTrans(m0, 10.0f, 22.0f, 0.0f);
					PSMTXConcat(m8, m0, m8);
					FontDrawStart_alpha(alpha);
					GXSetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
					if (i == 0) {
						GXSetCullMode(GX_CULL_BACK);
					} else {
						GXSetCullMode(GX_CULL_FRONT);
					}
					FontDrawStringMtx(m8, buf);
				}
				break;

			case 4:
				PSMTXIdentity(m0);
				GXLoadTexMtxImm(m0, GX_TEXMTX0, GX_MTX2x4);
				GXSetNumTevStages(1);
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
				GXSetTevColor(GX_TEVREG0, (GXColor){ 0, 0, 0, alphaB });
				effGetTexObj(8, &tex);
				GXLoadTexObj(&tex, GX_TEXMAP0);
				for (j = 0; j < data->animCell; j++) {
					PSMTXTransApply(m5, m8, -5.0f + 5.0f * (f32)j, 0.0f, 0.0f);
					GXLoadPosMtxImm(m8, GX_PNMTX0);
					GXBegin(GX_QUADS, GX_VTXFMT0, 4);
					GXPosition3f32(-3.0f, 3.0f, 0.0f);
					GXTexCoord2f32(0.0f, 0.0f);
					GXPosition3f32(3.0f, 3.0f, 0.0f);
					GXTexCoord2f32(1.0f, 0.0f);
					GXPosition3f32(3.0f, -3.0f, 0.0f);
					GXTexCoord2f32(1.0f, 1.0f);
					GXPosition3f32(-3.0f, -3.0f, 0.0f);
					GXTexCoord2f32(0.0f, 1.0f);
				}
				break;

			case 5:
				GXSetNumTevStages(1);
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
				effGetTexObj(9, &tex);
				GXLoadTexObj(&tex, GX_TEXMAP0);
				PSMTXScale(m3, 0.25f, 1.0f, 1.0f);
				PSMTXTrans(m0, (f32)data->animCell, 0.0f, 0.0f);
				PSMTXConcat(m3, m0, m3);
				GXLoadTexMtxImm(m3, GX_TEXMTX0, GX_MTX2x4);
				GXSetTevColor(GX_TEVREG0, (GXColor){ 0, 0, 0, alphaB });
				GXLoadPosMtxImm(m5, GX_PNMTX0);
				GXBegin(GX_QUADS, GX_VTXFMT0, 4);
				GXPosition3f32(-7.0f, 7.0f, 0.0f);
				GXTexCoord2f32(0.0f, 0.0f);
				GXPosition3f32(7.0f, 7.0f, 0.0f);
				GXTexCoord2f32(1.0f, 0.0f);
				GXPosition3f32(7.0f, -7.0f, 0.0f);
				GXTexCoord2f32(1.0f, 1.0f);
				GXPosition3f32(-7.0f, -7.0f, 0.0f);
				GXTexCoord2f32(0.0f, 1.0f);
				break;

			case 6:
				lang = gp->retraceCount % 15;
				PSMTXIdentity(m0);
				GXLoadTexMtxImm(m0, GX_TEXMTX0, GX_MTX2x4);
				PSMTXTrans(m0, 0.0f, ((f32*)chuchu_dt)[lang * 3 + 0], 0.0f);
				PSMTXScale(m3, 0.7f * ((f32*)chuchu_dt)[lang * 3 + 1], 0.7f * ((f32*)chuchu_dt)[lang * 3 + 2], 0.7f);
				PSMTXRotRad(m6, 'y', MTXDegToRad(data->spinY));
				PSMTXConcat(m0, m6, m0);
				PSMTXConcat(m0, m3, m0);
				GXSetNumTevStages(1);
				GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR_NULL);
				GXSetTevColorOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevAlphaOp(GX_TEVSTAGE0, GX_TEV_ADD, GX_TB_ZERO, GX_CS_SCALE_1, GX_TRUE, GX_TEVPREV);
				GXSetTevColorIn(GX_TEVSTAGE0, GX_CC_ZERO, GX_CC_C0, GX_CC_TEXC, GX_CC_ZERO);
				GXSetTevAlphaIn(GX_TEVSTAGE0, GX_CA_ZERO, GX_CA_A0, GX_CA_TEXA, GX_CA_ZERO);
				effGetTexObj(0xA, &tex);
				GXLoadTexObj(&tex, GX_TEXMAP0);
				if (i == 0) {
					GXSetTevColor(GX_TEVREG0, (GXColor){ 0, 0, 0, alphaA });
					PSMTXTransApply(m5, m8, 1.0f, -1.0f, 0.0f);
					PSMTXConcat(m8, m0, m8);
					GXLoadPosMtxImm(m8, GX_PNMTX0);
					GXBegin(GX_QUADS, GX_VTXFMT0, 4);
					GXPosition3f32(-8.0f, 16.0f, 0.0f);
					GXTexCoord2f32(0.0f, 0.0f);
					GXPosition3f32(8.0f, 16.0f, 0.0f);
					GXTexCoord2f32(2.0f, 0.0f);
					GXPosition3f32(8.0f, -8.0f, 0.0f);
					GXTexCoord2f32(2.0f, 2.0f);
					GXPosition3f32(-8.0f, -8.0f, 0.0f);
					GXTexCoord2f32(0.0f, 2.0f);
					if (i == 0) {
						GXSetTevColor(GX_TEVREG0, (GXColor){ 0xFF, 0xCD, 0x28, alphaB });
					} else {
						GXSetTevColor(GX_TEVREG0, (GXColor){ 0x7F, 0x66, 0x14, alphaB });
					}
					PSMTXConcat(m5, m0, m8);
					GXLoadPosMtxImm(m8, GX_PNMTX0);
					GXBegin(GX_QUADS, GX_VTXFMT0, 4);
					GXPosition3f32(-8.0f, 16.0f, 0.0f);
					GXTexCoord2f32(0.0f, 0.0f);
					GXPosition3f32(8.0f, 16.0f, 0.0f);
					GXTexCoord2f32(2.0f, 0.0f);
					GXPosition3f32(8.0f, -8.0f, 0.0f);
					GXTexCoord2f32(2.0f, 2.0f);
					GXPosition3f32(-8.0f, -8.0f, 0.0f);
					GXTexCoord2f32(0.0f, 2.0f);
				}
				break;
		}
	}
}

// .rodata tail gap: the original's last 4-byte zero word, after the three strings.
__declspec(section ".rodata") const u32 gap_04_802F30F4_rodata = 0;
