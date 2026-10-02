#include "motion/mot_plane.h"
#include "mot/mot_plane.h"
#include "evt/evt_cmd.h"
#include "evt/evt_snd.h"
#include "mario/mario.h"
#include "mario/mario_cam.h"
#include "mario/mario_hit.h"
#include "mario/mario_sbr.h"
#include "party.h"
#include "pmario_sound.h"
#include "system.h"
#include "memory.h"
#include <dolphin/mtx.h>
#include <string.h>
#include "matching.h"

// Raw MotionPlaneData accessors for fields the header does not name yet.
#define MF32(d, off) (*(f32*)((u8*)(d) + (off)))
#define MFVEC(d, off) (*(Vec*)((u8*)(d) + (off)))

// Raw MarioWork accessor for the unnamed Vec fields the original writes.
#define MWVEC(m, off) (*(Vec*)((u8*)(m) + (off)))
// MotionPlaneData fields the header does not name yet (macro expands to raw offsets).
#define DATA ((MotionPlaneData*)mario->motStruct)

// .rodata zero vectors used by the plane motion (original vec3_802c5c9c / vec3_802c5cd8...).
// They are defined at the source positions that reproduce the original .rodata order.

EVT_BEGIN(sound_evt)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_AIRPLANE1"), LW(0), LW(1), LW(2), 0)
WAIT_FRAMES(12)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_AIRPLANE1"), LW(0), LW(1), LW(2), 0)
WAIT_FRAMES(12)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_AIRPLANE1"), LW(0), LW(1), LW(2), 0)
WAIT_FRAMES(12)
CALL(evt_snd_sfxon_3d, STRING("SFX_MARIO_AIRPLANE1"), LW(0), LW(1), LW(2), 0)
RETURN
EVT_END()

// .rodata:0x14, after the SFX string emitted by sound_evt above.
const Vec vec3_802c5c9c = { 0.0f, 40.0f, 0.0f };

s32 marioPlaneGetFlyingDir(void) {
	MarioWork* mario = marioGetPtr();

	if (mario->currMotionId != MARIO_MOTION_PLANE) {
		return 0;
	}
	if (((MotionPlaneData*)mario->motStruct)->flags & 1) {
		return 1;
	}
	return -1;
}

s32 marioGetPlaneStatus(void) {
	MarioWork* mario = marioGetPtr();

	if (mario->currMotionId != MARIO_MOTION_PLANE) {
		return 0;
	}
#ifdef MUST_MATCH
	// The original compares currSubMotionId with cmpwi (signed); the header types it u32.
	if ((s32)mario->currSubMotionId >= 0xC && (s32)mario->currSubMotionId < 0x14) {
#else
	if (mario->currSubMotionId >= 0xC && mario->currSubMotionId < 0x14) {
#endif
		return 2;
	}
	return 1;
}

s32 mario_plane_cancel(void) {
	MarioWork* mario = marioGetPtr();

	if (mario->currMotionId != MARIO_MOTION_PLANE) {
		return 0;
	}
	if ((s32)mario->currSubMotionId >= 0xC && (s32)mario->currSubMotionId < 0x14) {
		mario->currSubMotionId = 0x32;
		*(f32*)mario->field_0x2AC = mario->position.y;
		return 1;
	}
	return 0;
}

// Paper animation names indexed by mario's current color (paper_plane@sda21 in the asm).
const char* paper_plane[4] = {
    "p_hikohki",
    "p_hikohki_g",
    "p_hikohki_y",
    "p_hikohki_v",
};

// .rodata:0x50, after the four paper-plane names emitted by paper_plane above.
const Vec vec3_802c5cd8 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5ce4 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5cf0 = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5cfc = { 0.0f, 0.0f, 0.0f };
const Vec vec3_802c5d08 = { 0.0f, 0.0f, 0.0f };

// .sdata2 constant pool in the original's address order (build/G8MJ01/asm/motion/mot_plane.s:2739),
// so every `@sda21` offset resolves the same. Read through MATCH_F32 so MWCC does not pool a copy.
const f32 float_4_8041ac00 = 4.0f;
const f32 float_90_8041ac04 = 90.0f;
const f32 float_10_8041ac08 = 10.0f;
const f32 float_0_8041ac0c = 0.0f;
const f32 float_10p1_8041ac10 = 10.1f;
const f32 float_neg1_8041ac14 = -1.0f;
const f32 float_2_8041ac18 = 2.0f;
const f32 float_1_8041ac1c = 1.0f;
const f32 float_270_8041ac20 = 270.0f;
const f32 float_20_8041ac24 = 20.0f;
const f32 float_37_8041ac28 = 37.0f;
const f32 float_60_8041ac2c = 60.0f;
const f32 float_0p1_8041ac30 = 0.1f;
const f32 float_150_8041ac34 = 150.0f;
const f32 float_330_8041ac38 = 330.0f;
const f32 float_210_8041ac3c = 210.0f;
const f32 float_3p1416_8041ac40 = 3.1415927f;
const f32 float_180_8041ac44 = 180.0f;
const f32 float_neg0p001_8041ac48 = -0.001f;
const f32 float_0p5_8041ac4c = 0.5f;
const f32 float_6_8041ac50 = 6.0f;
const f32 float_neg80_8041ac54 = -80.0f;
const f32 float_neg22_8041ac58 = -22.0f;
const f32 float_1000_8041ac5c = 1000.0f;
const f32 float_0p4_8041ac60 = 0.4f;
const f32 float_0p01_8041ac64 = 0.01f;
const f32 float_0p125_8041ac68 = 0.125f;
const f32 float_30_8041ac6c = 30.0f;
const f32 float_630_8041ac70 = 630.0f;
const f32 float_neg630_8041ac74 = -630.0f;
const char str_8041AC78[] = "M_Z_1";
const f32 float_1024_8041ac80 = 1024.0f;
const f32 float_1260_8041ac84 = 1260.0f;
const f32 float_neg1080_8041ac88 = -1080.0f;
const f32 float_12p333_8041ac8c = 12.333333f;
const f32 float_0p02_8041ac90 = 0.02f;
const f32 float_0p004_8041ac94 = 0.004f;
const f32 float_70_8041ac98 = 70.0f;
const f32 float_neg70_8041ac9c = -70.0f;
const f32 float_100_8041aca0 = 100.0f;
const f32 float_0p3_8041aca4 = 0.3f;
const f32 float_28_8041aca8 = 28.0f;
const f32 float_neg20_8041acac = -20.0f;
const f32 float_neg2_8041acb0 = -2.0f;

void marioForcePlaneAnime(void) {
    MarioWork* mario = marioGetPtr();

    marioPaperOff();
    marioChgPaper(NULL);
    marioPaperOn(paper_plane[marioGetColor()]);
    marioChgPaper("PM_P_1B");
    marioChgPose(str_8041AC78);
    mario->dispFlags |= 0x8;
}

// NONMATCHING (3.48%): 6 instructions differ, 891 missing, plus register differences.
void mot_plane(void) {
	MarioWork* mario;
	MotionPlaneData* data;

	mario = marioGetPtr();
	if (mario->trigFlags & 1) {
		mario->trigFlags &= ~1;
		mario->motStruct = __memAlloc(HEAP_DEFAULT, sizeof(MotionPlaneData));
		memset(mario->motStruct, 0, sizeof(MotionPlaneData));
		data = marioGetPtr()->motStruct;
		data->field_0x28 = -1;
		mario->currSubMotionId = 0;
	}
	switch (mario->currSubMotionId) {
		case 0:
			//inline start?
			mario = marioGetPtr();

			//inline end?
			//marioPreJump();
			break;
	}
}

// NONMATCHING (2.15%): 4 instructions differ, 844 missing, plus register differences.
static void flyMain(void) {
	MarioWork* mario;

	mario = marioGetPtr();
	switch (mario->currSubMotionId) {
		case 0xC:
			if (mario->wCamVal1 >= mario->position.y) {
				mario->currSubMotionId = 14;
			}
			//inlined?
			mario = marioGetPtr();
	}
}

// Matched at 100%. The MATCH_F32() and volatile accesses below reproduce the original's scheduling:
// the y store must stay volatile so the volatile Vec copies do not hoist above it, and the Vec
// copies are volatile so each loads/stores as an unscheduled pair.
void mot_plane_post(void) {
	MarioWork* mario = marioGetPtr();
	MarioWork* m;

	camFollowYOn();
	m = marioGetPtr();
	if (m->motStruct != NULL) {
		if (*(u32*)&((MotionPlaneData*)m->motStruct)->field_0x28 != 0xFFFFFFFFu) {
			psndSFXOff(((MotionPlaneData*)m->motStruct)->field_0x28);
			((MotionPlaneData*)m->motStruct)->field_0x28 = -1;
		}
	}
	m = marioGetPtr();
#ifdef MUST_MATCH
	// Volatile store keeps the following volatile Vec loads below it, as the original schedules them.
	m->wPlayerCollisionBox.x = MATCH_F32(float_20_8041ac24);
	*(volatile f32*)&m->wPlayerCollisionBox.y = MATCH_F32(float_37_8041ac28);
#else
	m->wPlayerCollisionBox.x = MATCH_F32(float_20_8041ac24);
	m->wPlayerCollisionBox.y = MATCH_F32(float_37_8041ac28);
#endif

#ifdef MUST_MATCH
	// The original copies each Vec with an unscheduled per-component load/store pair.
	*(volatile Vec*)((u8*)m + 0xBC) = *(volatile Vec*)&vec3_802c5cd8;
	*(volatile Vec*)((u8*)m + 0xB0) = *(volatile Vec*)&vec3_802c5ce4;
	*(volatile Vec*)((u8*)m + 0x98) = *(volatile Vec*)&vec3_802c5cf0;
#else
	MWVEC(m, 0xBC) = vec3_802c5cd8;
	MWVEC(m, 0xB0) = vec3_802c5ce4;
	MWVEC(m, 0x98) = vec3_802c5cf0;
#endif
	marioPaperOff();
	marioChgPaper(NULL);
	m->dispFlags &= ~0x8;
	m->flags |= 0x80;
	marioAdjustMoveDir();
	m->dispFlags &= ~0x100;
#ifdef MUST_MATCH
	{
		f32 zero = MATCH_F32(float_0_8041ac0c);
		mario->unk138 = zero;
		mario->unk140 = zero;
	}
#else
	mario->unk138 = 0.0f;
	mario->unk140 = 0.0f;
#endif
	if (mario->motStruct != NULL) {
		__memFree(HEAP_DEFAULT, mario->motStruct);
		mario->motStruct = NULL;
	}
	allPartyRideOff2();
}

// v3C and v30 blocks plus the five marioHitCheck calls now match except for a few FP/address
// registers (v30.y result f30 vs f1, planeData/vec3 r3/r4) and the call-1 load schedule.
// NONMATCHING (97.18%): every constant, call argument and data reference is correct.
s32 chkCollision(f32* out) {
	s32 flag = 0;
	MarioWork* mario = marioGetPtr();
	Vec dir;
	Vec normal;
	Vec v3C;
	Vec v30;
	f32 sinC, cosC;
	f32 sinA, cosA;
	f32 sinR, cosR;
	f32 ten;
	f32 hx, hy, hz;
	f32 dist;
	f32 nx, ny, nz;
	HitEntry* hit;

	sincosf(&sinA, &cosA, MATCH_F32(float_90_8041ac04) + MF32(DATA, 0x4));
	sincosf(&sinC, &cosC, MF32(DATA, 0x8));

	v3C = vec3_802c5cfc;
	{
		f32 t0,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15;
		t1 = MF32(mario, 0x98);
		t3 = MF32(mario, 0x9C);
		t0 = mario->position.x;
		t2 = mario->position.y;
		t4 = MF32(mario, 0xB0);
		t9 = -sinA;
		t10 = sinC * cosA;
		t11 = 4.0f;
		t12 = cosC * cosA;
		t5 = t0 + t1;
		t6 = t2 + t3;
		t4 = t4 + t5;
		t8 = MF32(mario, 0xB4) + t6;
		t4 = t11 * t10 + t4;
		t8 = t9 * t11 + t8;
		t14 = mario->position.z;
		t13 = MF32(mario, 0xA0);
		t15 = t14 + t13;
		t13 = MF32(mario, 0xB8);
		t14 = t13 + t15;
		t14 = t11 * t12 + t14;
		v3C.x = t4;
		v3C.y = t8;
		v3C.z = t14;
	}
	MFVEC(DATA, 0x3C) = v3C;

	sincosf(&sinR, &cosR, MF32(DATA, 0xC));
	sincosf(&sinA, &cosA, MF32(DATA, 0x4));
	*(HitEntry**)((u8*)mario + 0x1E0) = NULL;
	{
		f32 vx, vy, vz;
		f32 pd40 = planeData[40];
		sincosf(&sinC, &cosC, MF32(DATA, 0x8));
		vx = MF32(mario, 0xB0) + (mario->position.x + MF32(mario, 0x98));
		vy = MF32(mario, 0xB4) + (mario->position.y + MF32(mario, 0x9C));
		vz = MF32(mario, 0xB8) + (mario->position.z + MF32(mario, 0xA0));
		vx = pd40 * (sinC * -cosA) + vx;
		vz = pd40 * (cosC * -cosA) + vz;
		vy = sinA * pd40 + vy;
		sincosf(&sinA, &cosA, MF32(DATA, 0x4) - MATCH_F32(float_90_8041ac04));
		v30 = vec3_802c5d08;
		vx = vx + (planeData[39] * (cosA * sinC) - planeData[39] * (sinR * cosC));
		v30.x = vx;
		v30.y = vy + planeData[39] * (sinA * cosR);
		vz = vz + (planeData[39] * (cosA * cosC) - planeData[39] * (sinR * sinC));
		v30.z = vz;
		MFVEC(DATA, 0x30) = v30;
	}

	ten = 10.0f;
	{
		MotionPlaneData* d = DATA;
		f32 a0 = MF32(d, 0x30) - ten * sinC;
		f32 a1 = MF32(d, 0x34);
		dist = ten + mario->baseSpeed;
		hit = marioHitCheck(&hx, &hy, &hz, &dist, &nx, &ny, &nz,
		                    a0, a1, MF32(d, 0x38) - ten * cosC, sinC, 0.0f, cosC);
	}
	if (hit != NULL) {
		*(HitEntry**)((u8*)mario + 0x1E0) = hit;
		flag = 1;
		*out = hy;
	}

	{
		f32 a0 = MF32(DATA, 0x3C);
		f32 a1 = MF32(DATA, 0x40);
		f32 a2 = MF32(DATA, 0x44);
		dist = mario->baseSpeed;
		hit = marioHitCheck(&hx, &hy, &hz, &dist, &nx, &ny, &nz,
		                    a0, a1, a2, sinC, 0.0f, cosC);
	}
	if (hit != NULL) {
		*(HitEntry**)((u8*)mario + 0x1E0) = hit;
		flag = 1;
		*out = hy;
	}

	{
		f32 a0 = MF32(DATA, 0x30);
		f32 a1 = MF32(DATA, 0x34);
		f32 a2 = MF32(DATA, 0x38);
		mario->hitobjStandOn = NULL;
		dist = MATCH_F32(float_10p1_8041ac10);
		hit = marioHitCheck(&hx, &hy, &hz, &dist, &nx, &ny, &nz,
		                    a0, a1 + ten, a2, 0.0f, MATCH_F32(float_neg1_8041ac14), 0.0f);
	}
	if (hit != NULL) {
		mario->hitobjStandOn = hit;
		flag = 1;
		*out = hy;
	}

	if (flag == 0) {
		{
			f32 a0 = MF32(DATA, 0x3C);
			f32 a1 = MF32(DATA, 0x40);
			f32 a2 = MF32(DATA, 0x44);
			mario->hitobjStandOn = NULL;
			dist = MATCH_F32(float_10p1_8041ac10);
			hit = marioHitCheck(&hx, &hy, &hz, &dist, &nx, &ny, &nz,
			                    a0, a1 + ten, a2, 0.0f, MATCH_F32(float_neg1_8041ac14), 0.0f);
		}
		if (hit != NULL) {
			mario->hitobjStandOn = hit;
			flag = 1;
			*out = hy;
		}
	}

	if (flag == 0) {
		PSVECSubtract(&MFVEC(DATA, 0x3C), &MFVEC(DATA, 0x30), &dir);
		dist = PSVECMag(&dir);
		PSVECNormalize(&dir, &dir);
		hit = marioHitCheckVec(&MFVEC(DATA, 0x30), &dir, &normal, NULL, &dist);
		if (hit != NULL) {
			mario->hitobjStandOn = hit;
			flag = 1;
			*out = normal.y;
		}
	}

	*(HitEntry**)((u8*)mario + 0x1F4) = NULL;
	if (mario->hitobjStandOn == NULL) {
		{
			f32 a1 = MF32(DATA, 0x34);
			f32 a2 = MF32(DATA, 0x38);
			f32 a0 = MF32(DATA, 0x30);
			dist = MATCH_F32(float_2_8041ac18);
			hit = marioHitCheck(&hx, &hy, &hz, &dist, &nx, &ny, &nz,
			                    a0, a1, a2, 0.0f, MATCH_F32(float_1_8041ac1c), 0.0f);
		}
		if (hit != NULL) {
			*(HitEntry**)((u8*)mario + 0x1F4) = hit;
			flag = 1;
			*out = hy;
		}
	}
	return flag;
}
