#ifndef _DOLPHIN_OSFASTCAST_H_
#define _DOLPHIN_OSFASTCAST_H_

#ifdef __cplusplus
extern "C" {
#endif

#define OS_GQR_F32 0x0000
#define OS_GQR_U8  0x0004
#define OS_GQR_U16 0x0005
#define OS_GQR_S8  0x0006
#define OS_GQR_S16 0x0007

#define OS_FASTCAST_U8  2
#define OS_FASTCAST_U16 3
#define OS_FASTCAST_S8  4
#define OS_FASTCAST_S16 5
// clang-format off
static inline void OSInitFastCast(void) {
#ifdef __MWERKS__
  asm
  {
        li      r3, OS_GQR_U8
        oris    r3, r3, OS_GQR_U8
        mtspr   GQR2, r3

        li      r3, OS_GQR_U16
        oris    r3, r3, OS_GQR_U16
        mtspr   GQR3, r3

        li      r3, OS_GQR_S8
        oris    r3, r3, OS_GQR_S8
        mtspr   GQR4, r3

        li      r3, OS_GQR_S16
        oris    r3, r3, OS_GQR_S16
        mtspr   GQR5, r3
  }
#else

#endif
}

static inline s16 __OSf32tos16(register f32 in) {
  s16 tmp;
  register s16* ptr = &tmp;
#ifdef __MWERKS__
  asm { psq_st in, 0(ptr), 1, OS_FASTCAST_S16 }
#endif
  return tmp;
}

static inline f32 __OSs16tof32(register const s16* arg) {
  register f32 ret;
#ifdef __MWERKS__
  asm { psq_l ret, 0(arg), 1, OS_FASTCAST_S16 }
#endif
  return ret;
}

static inline void OSf32tos16(const f32* in, s16* out) { *out = __OSf32tos16(*in); }

static inline void OSs16tof32(const s16* in, f32* out) { *out = __OSs16tof32(in); }
// clang-format off

#ifdef __cplusplus
}
#endif

#endif // _DOLPHIN_OSFASTCAST