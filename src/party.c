#include "party.h"
#include "driver/animdrv.h"
#include "mario/mario_pouch.h"
#include "mario/mariost.h"
#include "memory.h"
#include "mario/mario_cam.h"
#include "system.h"
#include "mario/mario_party.h"
#include "party/party_yoshi.h"
#include "party/party_move.h"
#include "mario/mario_sbr.h"
#include "party/party_vivian.h"
#include "motion/mot_ship.h"
#include "motion/mot_damage.h"
#include "party/party_bomhei.h"
#include "party/party_motion.h"
#include "pmario_sound.h"
#include "party/party_chuchu.h"
#include "math_ext.h"
#include <dolphin/mtx.h>
#include "driver/dispdrv.h"
#include "driver/shadowdrv.h"
#include "party/party_hit.h"
#include "pmario_sound.h"

// Raw field accessors for fields missing from the shared PartyEntry header.
#define P8(p, off) (*(u8*)((u8*)(p) + (off)))
#define PS8(p, off) (*(s8*)((u8*)(p) + (off)))
#define P16(p, off) (*(u16*)((u8*)(p) + (off)))
#define P32(p, off) (*(u32*)((u8*)(p) + (off)))
#define PS32(p, off) (*(s32*)((u8*)(p) + (off)))
#define PF32(p, off) (*(f32*)((u8*)(p) + (off)))



//.data

extern char* partyPoseTbl[20][15];
extern char* partyDotPoseTbl[8][15];
extern char* yoshiDotPoseTbl[7][15];
extern PartyData partyDataTbl[];
extern const f32 float_0_8041b100;
extern const f32 float_neg1_8041b0ec;
extern const f32 float_5000_8041b114;
extern const f32 float_10000_8041b118;
extern const f32 float_13p333_8041b11c;
extern const f32 float_1000_8041b120;
extern const f32 float_10_8041b124;
extern const f32 float_180_8041b0fc;
extern const f32 float_290_8041b128;
extern const f32 float_250_8041b12c;
extern const f32 float_190_8041b130;
extern const f32 float_350_8041b134;
extern const f32 float_170_8041b138;
extern const f32 float_0p125_8041b13c;
extern const f32 float_20_8041b140;
extern const f32 float_neg40_8041b144;
extern const f32 float_neg30_8041b148;
extern const f32 float_220_8041b14c;
extern const f32 float_210_8041b150;
extern const f32 float_30_8041b178;
extern const f32 float_0p4_8041b174;
extern const f32 float_0p5_8041b170;
extern const f32 float_16_8041b17c;
extern const f32 float_neg1p6_8041b180;
extern const f32 float_0p0625_8041b184;
extern const f32 float_neg2000_8041b164;
const f32 vec3_802ccd30[3] = { 1.0f, 1.0f, 1.0f };
const char str_802CCEEC[] = "d_mario";
const Vec vec3_802ccee0 = { 0.0f, -1.0f, 0.0f };
const Vec vec3_802ccdd8 = { 0.0f, 0.0f, 0.0f };

/*
    000b95ec 000020 800bebcc  4 partyGetPtr 	party.o
    000b95c8 000024 800beba8  4 anotherPartyGetPtr 	party.o
    000b94a8 000120 800bea88  4 partyPaperOn 	party.o
    000b942c 00007c 800bea0c  4 partyPaperOff 	party.o
    000b9404 000028 800be9e4  4 partyPaperLightOff 	party.o
    UNUSED   000028 ........ partyPaperLightOn party.o
    UNUSED   000078 ........ setAnimPoseMaterialFlag party.o
    UNUSED   000040 ........ partyPaperRelease party.o
    000b92a0 000164 800be880  4 partyChgPoseId 	party.o
    000b9224 00007c 800be804  4 partyChgPose 	party.o
    000b91b4 000070 800be794  4 partyChgPaper 	party.o
    UNUSED   000078 ........ partyChgPaperTime party.o
    000b917c 000038 800be75c  4 partyCtrlOff 	party.o
    000b9144 000038 800be724  4 partyCtrlOn 	party.o
    000b8f2c 000218 800be50c  4 partyRun 	party.o
    000b8d1c 000210 800be2fc  4 partyStop 	party.o
    000b8af4 000228 800be0d4  4 partySetForceMove 	party.o
    UNUSED   00006c ........ partyGetNum party.o
    UNUSED   00007c ........ getPartyEntryId party.o
    UNUSED   00005c ........ getExtraPartyEntryId party.o
    UNUSED   000008 ........ partyCheck party.o
    UNUSED   000238 ........ partyHello_debug party.o
    000b88bc 000238 800bde9c  4 partyEntryHello 	party.o
    000b8704 0001b8 800bdce4  4 partyEntry 	party.o
    000b84e8 00021c 800bdac8  4 partyEntryPos 	party.o
    000b82b0 000238 800bd890  4 partyGoodbye 	party.o
    000b815c 000154 800bd73c  4 partyKill 	party.o
    000b8060 0000fc 800bd640  4 partyKill2 	party.o
    UNUSED   000080 ........ partyKill_debug party.o
    UNUSED   0000b8 ........ partyPoseEntry party.o
    000b7fc0 0000a0 800bd5a0  4 yoshiSetColor 	party.o
    000b7f90 000030 800bd570  4 partyGetHeight 	party.o
    000b7c0c 000384 800bd1ec  4 partyEntryMain 	party.o
    000b7894 000378 800bce74  4 partyEntry2 	party.o
    000b739c 0004f8 800bc97c  4 partyEntry2Hello 	party.o
    000b6e0c 000590 800bc3ec  4 partyEntry2Pos 	party.o
    000b6de0 00002c 800bc3c0  4 partyInit 	party.o
    000b6214 000bcc 800bb7f4  4 partyReInit 	party.o
    000b61b8 00005c 800bb798  4 partyUsePost 	party.o
    000b5fa0 000218 800bb580  4 partyChgRunMode 	party.o
    UNUSED   000208 ........ partyChgRunMode2 party.o
    000b5f0c 000094 800bb4ec  4 partyPrevRunMode 	party.o
    000b5e28 0000e4 800bb408  4 partyMain 	party.o
    000b5444 0009e4 800baa24  4 driveParty 	party.o
    000b51d0 000274 800ba7b0  4 partyDoWork 	party.o
    UNUSED   0000c4 ........ partyNormalie party.o
    000b503c 000194 800ba61c  4 partyHello 	party.o
    UNUSED   000158 ........ partyHelloInit party.o
    000b4990 0006ac 800b9f70  4 partyHelloMain 	party.o
    UNUSED   000190 ........ party_force_hello party.o
    000b48c4 0000cc 800b9ea4  4 partyGoodbyeInit 	party.o
    000b4014 0008b0 800b95f4  4 partyGoodbyeMain 	party.o
    UNUSED   000054 ........ party_dokan party.o
    000b3a6c 0005a8 800b904c  4 partyDokanInit 	party.o
    000b350c 000560 800b8aec  4 partyDokanMain 	party.o
    000b34a0 00006c 800b8a80  4 partyDokanEnd 	party.o
    UNUSED   000014 ........ partyGetRideScale party.o
    000b3068 000438 800b8648  4 allPartyRidePlane 	party.o
    000b2c30 000438 800b8210  4 allPartyRideShip 	party.o
    000b27f8 000438 800b7dd8  4 allPartyRideJabara 	party.o
    UNUSED   000094 ........ allPartyJabaraEscape party.o
    UNUSED   000358 ........ allPartyRideShip2 party.o
    000b25d8 000220 800b7bb8  4 getRidePos 	party.o
    UNUSED   00018c ........ rideInit party.o
    000b2174 000464 800b7754  4 allPartyForceRideOn 	party.o
    UNUSED   000434 ........ allPartyRideOn party.o
    000b20e0 000094 800b76c0  4 allPartyRideOff 	party.o
    000b204c 000094 800b762c  4 allPartyRideOff2 	party.o
    UNUSED   000018 ........ partyRideOff party.o
    UNUSED   000018 ........ partyRideOff2 party.o
    000b2010 00003c 800b75f0  4 partyRideChk 	party.o
    UNUSED   0000f4 ........ allPartyRideChk party.o
    UNUSED   000048 ........ party_ride party.o
    UNUSED   00000c ........ partyRideInit party.o
    000b111c 000ef4 800b66fc  4 partyRideMain 	party.o
    UNUSED   00003c ........ chkMapAttr party.o
    000b01c8 000f54 800b57a8  4 partyGetAppearPos 	party.o
    000afc44 000584 800b5224  4 partyGetAppearPos2 	party.o
    000af908 00033c 800b4ee8  4 partyGetAppearPos3 	party.o
    000af3e8 000520 800b49c8  4 partyGetAppearPos4 	party.o
    UNUSED   000214 ........ partyGetFrontPos party.o
    000af07c 00036c 800b465c  4 party_force_ride_yoshi 	party.o
    000aeff4 000088 800b45d4  4 party_ride_yoshi_force_move 	party.o
    UNUSED   000048 ........ party_ride_yoshi party.o
    000ae96c 000688 800b3f4c  4 partyRideYoshiMain 	party.o
    UNUSED   000024 ........ chkWater party.o
    000ae390 0005dc 800b3970  4 party_damage_return 	party.o
    000ae360 000030 800b3940  4 partyInitCamId 	party.o
    UNUSED   000008 ........ partyGetCamId party.o
    000ae358 000008 800b3938  4 partySetCamId 	party.o
    UNUSED   0000dc ........ partyGetDispDir party.o
    000ae018 000340 800b35f8  4 partyMakeDispDir 	party.o
    000adcf8 000320 800b32d8  4 partyPreDisp 	party.o
    000ad980 000378 800b2f60  4 partyDisp 	party.o
    000ad97c 000004 800b2f5c  4 partyKeyOn 	party.o
    000ad978 000004 800b2f58  4 partyKeyOff 	party.o
    UNUSED   000008 ........ partySetContNo party.o
    UNUSED   00000c ........ partyGetContNo party.o
    UNUSED   000038 ........ partyClearCont party.o
    000ad870 000108 800b2e50  4 partyUpdateKeyData 	party.o
    UNUSED   000018 ........ partyShadowOn party.o
    000ad858 000018 800b2e38  4 partyShadowOff 	party.o
    UNUSED   00004c ........ partyChkHerohero party.o
    000ad464 0003f4 800b2a44  4 partySetFamicomMode 	party.o
    000ad464 00dd4c 800b2a44  1 .text 	party.o
*/

//.sbss
s32 partyCtrlNo;
PartyData* party_dp;
PartyEntry* partyPtrTbl[2];

//.sdata
char* yoshigroup[7] = {
    "c_babyyoshi",   "c_babyyoshi2", "c_babyyoshi3", "c_babyyoshi4",
    "c_babyyoshi5",  "c_babyyoshi6", "c_babyyoshi7",
};

#ifdef MUST_MATCH
// MUST_MATCH: retail reads these zero vectors through the vec3_802ccd30 base register.
#define PR_FV(off, sym) (*(const Vec*)(base + (off)))
#else
#define PR_FV(off, sym) ((Vec){0.0f, 0.0f, 0.0f})
#endif
#ifdef MUST_MATCH
#define DK_FV(off, sym) (*(const Vec*)(base + (off)))
#define DK_FB(off, sym) ((char*)base + (off))
#else
#define DK_FV(off, sym) (sym)
#define DK_FB(off, sym) (sym)
#endif
#define L_DOKAN_PAPER_ON(party, anim)                        \
    do {                                                     \
        BOOL v;                                              \
                                                             \
        if ((party)->paperPoseId >= 0) {                     \
            animPaperPoseRelease((party)->paperPoseId);      \
            (party)->paperPoseId = -1;                       \
        }                                                    \
        (party)->flags2 |= 0x4000000;                        \
        (party)->paperPoseId = animPaperPoseEntry((anim), 2); \
        v = FALSE;                                           \
        if (!strcmp((anim), DK_FB(0x1ec, "p_roll")) ||       \
            !strcmp((anim), DK_FB(0x1f4, "p_plane")) ||      \
            !strcmp((anim), DK_FB(0x1fc, "p_dokan_x")) ||    \
            !strcmp((anim), DK_FB(0x1e0, "p_dokan_y"))) {    \
            v = TRUE;                                        \
        }                                                    \
        v = !v;                                              \
        if (!strcmp((anim), DK_FB(0x208, "p_slit"))) {       \
            v = (party)->currentMemberId == PARTY_MEMBER_FLURRIE; \
        }                                                    \
        animPoseSetPaperAnimGroup((party)->paperAnimGroup, (anim), v); \
    } while (0)

// Prototypes for the forward references the original function order needs.
static inline const char* L_animName(PartyEntry* party);
static inline char* L_partyChgPoseId1(PartyEntry* party);
static inline void L_setMaterial(PartyEntry* party);
static inline Vec* partyVecAt(PartyEntry* party, u32 off);
static inline const char* L_famicomAnimName(PartyEntry* party);

PartyEntry* partyGetPtr(s32 index) {
    if (index >= 0) {
        return partyPtrTbl[index];
    }
    return NULL;
}

PartyEntry* anotherPartyGetPtr(s32 index) {
    if (index < 0) {
        return NULL;
    }
    return *(partyPtrTbl + ((index + 1) & 1));
}

// NONMATCHING (96.60%): base pointer lands in r31, target has it in r30 (v4/v5 swapped with it).
BOOL partyPaperOn(PartyEntry* party, const char* animGroup) {
    BOOL v4;
    BOOL v5;

    if (party->paperPoseId >= 0) {
        animPaperPoseRelease(party->paperPoseId);
        party->paperPoseId = -1;
    }
    party->flags2 |= 0x4000000;
    party->paperPoseId = animPaperPoseEntry(animGroup, 2);
    v4 = FALSE;
    if (!strcmp(animGroup, "p_roll") || !strcmp(animGroup, "p_plane") || !strcmp(animGroup, "p_dokan_x") ||
        !strcmp(animGroup, "p_dokan_y")) {
        v4 = TRUE;
    }
    v5 = !v4;
    if (!strcmp(animGroup, "p_slit")) {
        v5 = party->currentMemberId == PARTY_MEMBER_FLURRIE;
    }
    animPoseSetPaperAnimGroup(party->paperAnimGroup, animGroup, v5);
    return TRUE;
}

void partyPaperOff(PartyEntry* party) {
    s32 paperPoseId; // r3

    if ((party->flags2 & 0x4000000) && party->field_1C) {
        animPoseSetPaperAnimGroup(party->paperAnimGroup, NULL, FALSE);
        party->field_1C = 0;
        party->flags2 &= ~0x4000000;
        paperPoseId = party->paperPoseId;
        if (paperPoseId >= 0) {
            animPaperPoseRelease(paperPoseId);
            party->paperPoseId = -1;
        }
    }
}

void partyPaperLightOff(PartyEntry* party) {
    animPoseSetMaterialLightFlagOff(party->paperAnimGroup, 1);
}

char* partyChgPoseId(PartyEntry* party, s32 index) {
    BOOL v5;             // r30
    char* v6;            // r30
    char* v8;            // r3

    if ((party->flags2 & 2) != 0) {
        return party->field_18;
    }
    v5 = 0;
    if (party->currentMemberId < 8 && partyGetHp(party->currentMemberId) <= 5) {
        v5 = TRUE;
    }
    if (v5 == TRUE) {
        if (index == 1) {
            index = 12;
        } else if (index == 2) {
            index = 13;
        } else if (index == 3) {
            index = 14;
        }
    }
    if (!(party->flags2 & 0x8000)) {
        v6 = partyPoseTbl[party->currentMemberId][index];
    } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
        v6 = partyDotPoseTbl[party->currentMemberId][index];
    } else {
        v6 = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][index];
    }
    v8 = party->field_18;
    if (v8 && !strcmp(v8, v6)) {
        return v6;
    }
    party->field_18 = v6;
    party->field_20 = 0;
    party->field_8 |= 0x2000u;
    return v6;
}

void partyChgPose(PartyEntry* party, char* pose) {
    if (party->flags2 & 2) {
        return;
    }
    if (party->field_18 && strcmp(party->field_18, pose) == 0) {
        return;
    }
    if (party->flags2 & 0x8000) {
        return;
    }
    party->field_18 = pose;
    party->field_20 = 0;
    party->field_8 |= 0x2000;
}

void partyChgPaper(PartyEntry* party, char* paper) {
    if (party->flags2 & 2) {
        return;
    }
    if (party->field_1C && strcmp((char*)party->field_1C, paper) == 0) {
        return;
    }
    party->field_1C = (u32)paper;
    party->field_20 = 0;
    party->field_8 |= 0x4000;
}

void partyCtrlOff(void) {
    PartyEntry* party = partyPtrTbl[0];

    if (party) {
        party->flags |= PARTY_FLAG_CTRL_OFF;
    }
    party = partyPtrTbl[1];
    if (party) {
        party->flags |= PARTY_FLAG_CTRL_OFF;
    }
}

void partyCtrlOn(void) {
    PartyEntry* party = partyPtrTbl[0];

    if (party) {
        party->flags &= ~PARTY_FLAG_CTRL_OFF;
    }
    party = partyPtrTbl[1];
    if (party) {
        party->flags &= ~PARTY_FLAG_CTRL_OFF;
    }
}

void partyRun(PartyEntry* party) {
    if (party->field_34 == 0) {
        partyChgRunMode(party, 1);
    }
}

void partyStop(PartyEntry* party) {
    partyChgRunMode(party, 0);
}

void partySetForceMove(PartyEntry* party, s32 unk, f32 x, f32 z) {
    PF32(party, 0x100) = x;
    P16(party, 0x28) = unk;
    PF32(party, 0x104) = z;
    partyChgRunMode(party, 9);
    partyChgMot(party, 1);
}

s32 partyEntryHello(s32 memberId) {
    s32 ret = partyEntryMain(memberId);

    partyChgRunMode(partyGetPtr(ret), 4);
    return ret;
}

// NONMATCHING (98.00%): only the `party->field_70 = *(const Vec*)vec3_802ccd30` struct copy still
// differs -- the target stores x/y before loading z, ours hoists the z load before the y store.
s32 partyEntry(s32 memberId) {
    PartyEntry* party;
    s32 index;
    char* pose;
    s32 ret;
    s32 lowHp;

    ret = partyEntryMain(memberId);
    party = partyGetPtr(ret);
    party->field_70 = *(const Vec*)vec3_802ccd30;
    P8(party, 0x38) = 0xa;
    {
        MarioWork* mario = party->playerPtr;
        party->field_110 = mario->unk1A8;
        party->field_10C = mario->unk1A8;
    }
    partyGetAppearPos2(party, &party->position);
    party->field_B8 = party->position;
    index = 1;
    if (!(party->flags2 & 2)) {
        lowHp = 0;
        if (party->currentMemberId < 8 && partyGetHp(party->currentMemberId) <= 5) {
            lowHp = 1;
        }
        if (lowHp == 1) {
            index = 12;
        }
        if (!(party->flags2 & 0x8000)) {
            pose = partyPoseTbl[party->currentMemberId][index];
        } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
            pose = partyDotPoseTbl[party->currentMemberId][index];
        } else {
            pose = yoshiDotPoseTbl[pouchGetPartyColor(PARTY_MEMBER_YOSHI)][index];
        }
        if (!(party->field_18 && !strcmp(party->field_18, pose))) {
            party->field_18 = pose;
            party->field_20 = 0;
            party->field_8 |= 0x2000;
        }
    }
    party->field_34 = 1;
    return ret;
}

s32 partyEntryPos(s32 memberId, f32 x, f32 y, f32 z) {
    const Vec* v;
    PartyEntry* party;
    s32 index;
    s32 ret;
    s32 lowHp;
    char* pose;
    char* prev;
    Vec pos;

    v = (const Vec*)vec3_802ccd30;
    ret = partyEntryMain(memberId);
    party = partyGetPtr(ret);
    index = 1;
    party->field_70 = v[1];
    pos = v[2];
    pos.x = x;
    pos.y = y;
    pos.z = z;
    party->position = pos;
    {
        MarioWork* mario = party->playerPtr;
        party->field_70 = v[3];
        party->field_110 = mario->unk1A8;
        party->field_10C = mario->unk1A8;
    }
    if (!(party->flags2 & 2)) {
        lowHp = 0;
        if (party->currentMemberId < 8 && partyGetHp(party->currentMemberId) <= 5) {
            lowHp = 1;
        }
        if (lowHp == 1) {
            index = 12;
        }
        if (!(party->flags2 & 0x8000)) {
            pose = partyPoseTbl[party->currentMemberId][index];
        } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
            pose = partyDotPoseTbl[party->currentMemberId][index];
        } else {
            pose = yoshiDotPoseTbl[pouchGetPartyColor(PARTY_MEMBER_YOSHI)][index];
        }
        prev = party->field_18;
        if (!(prev && !strcmp(prev, pose))) {
            party->field_18 = pose;
            party->field_20 = 0;
            party->field_8 |= 0x2000;
        }
    }
    party->field_34 = 0;
    return ret;
}

s32 partyGoodbye(s32 index) {
    s32 ret = 0;

    if (index >= 0) {
        partyChgRunMode(partyGetPtr(index), 8);
        ret = 1;
    }
    return ret;
}

s32 partyKill(s32 index) {
    typedef void (*PartyFunc)(PartyEntry*);
    PartyEntry* party = partyGetPtr(index);
    MarioWork* player;

    if (!party) {
        return 0;
    }
    player = party->playerPtr;
    party_dp = &partyDataTbl[partyGetPtr(player->currPartySlotId[index])->currentMemberId];
    if (party_dp->exit_func && party->field_34 != 0xb) {
        ((PartyFunc)party_dp->exit_func)(party);
    }
    if (partyGetPtr(player->currPartySlotId[index])) {
        player->prevPartyMemberId[index] = partyGetPtr(player->currPartySlotId[index])->currentMemberId;
    } else {
        player->prevPartyMemberId[index] = 0;
    }
    player->currPartySlotId[index] = -1;
    partyPtrTbl[index] = NULL;
    animPoseRelease(party->paperAnimGroup);
    __memFree(HEAP_DEFAULT, party);
    return 0;
}

s32 partyKill2(s32 index) {
    typedef void (*PartyFunc)(PartyEntry*);
    PartyEntry* party = partyGetPtr(index);
    MarioWork* player;

    if (!party) {
        return 0;
    }
    player = party->playerPtr;
    party_dp = &partyDataTbl[partyGetPtr(player->currPartySlotId[index])->currentMemberId];
    if (party_dp->exit_func && party->field_34 != 0xb) {
        ((PartyFunc)party_dp->exit_func)(party);
    }
    player->prevPartyMemberId[index] = 0;
    player->currPartySlotId[index] = -1;
    partyPtrTbl[index] = NULL;
    animPoseRelease(party->paperAnimGroup);
    __memFree(HEAP_DEFAULT, party);
    return 0;
}

s32 yoshiSetColor(void) {
    s32 color = irand(1000);

    if (color < 150) {
        color = 0;
    } else if (color < 300) {
        color = 1;
    } else if (color < 450) {
        color = 2;
    } else if (color < 600) {
        color = 3;
    } else if (color < 750) {
        color = 4;
    } else if (color < 900) {
        color = 5;
    } else {
        color = 6;
    }
    ((void (*)(s32, s32))pouchSetPartyColor)(PARTNER_YOSHI, color);
    return color;
}

f32 partyGetHeight(PartyEntry* party) {
    if (party->currentMemberId == PARTY_MEMBER_YOSHI && (party->flags & PARTY_FLAG_IS_BEING_USED)) {
        return 18.5f + PF32(party, 0xF0);
    }
    return PF32(party, 0xF0);
}

// NONMATCHING (99.51%): the material setup uses the already-loaded `player` instead of reloading
// party->playerPtr (r31 vs the target's r28); using the reload shifts the whole animName/pose chain
// to r31, and the shared L_setMaterial helper makes mwcceppc 2.6 loop forever in this function.
s32 partyEntryMain(s32 memberId) {
    MarioWork* player = marioGetPtr();
    s32 slotId;
    PartyEntry* party;
    PartyEntry* entry;
    const char* animName;

    for (slotId = 0; slotId < 2; slotId++) {
        if (partyPtrTbl[slotId] == NULL) {
            entry = (PartyEntry*)__memAlloc(HEAP_DEFAULT, sizeof(PartyEntry));
            partyPtrTbl[slotId] = entry;
            memset(entry, 0, sizeof(PartyEntry));
            goto found;
        }
    }
    slotId = -1;
found:
    if (slotId < 0) {
        return -1;
    }
    party = slotId >= 0 ? partyPtrTbl[slotId] : NULL;
    memset(party, 0, sizeof(PartyEntry));
    party->camId = marioGetCamId();
    party->paperAnimGroup = -1;
    party->field_10 = -1;
    party->paperPoseId = -1;
    party->flags = 1;
    party->currentSlotId = slotId;
    party->playerPtr = player;
    party->currentMemberId = memberId;
    if (player->prevPartyMemberId[0] == 0) {
        player->prevPartyMemberId[0] = party->currentMemberId;
    }
    if (slotId == 1) {
        party->flags |= 4;
    }
    party_dp = &partyDataTbl[memberId];
    if (player->flags & 0x80000000) {
        party->flags2 |= 0x8000;
    }
#ifdef MUST_MATCH
    animName = L_famicomAnimName(party);
#else
    if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
        if (!(party->flags2 & 0x8000)) {
            animName = party_dp->name;
        } else {
            animName = str_802CCEEC;
        }
    } else {
        animName = yoshigroup[pouchGetPartyColor(PARTNER_YOSHI)];
        if (party->flags2 & 0x8000) {
            animName = str_802CCEEC;
        }
    }
#endif
    if (party->paperAnimGroup >= 0) {
        animPoseRelease(party->paperAnimGroup);
    }
    party->paperAnimGroup = animPoseEntry(animName, 2);
    animPosePeraOff(party->paperAnimGroup);
    {
        MarioWork* mario = player;

        animPoseSetMaterialFlagOn(party->paperAnimGroup,
                                  animPoseGetMaterialFlag(mario->paperAnimGroupId[marioAnimeId()]));
        animPoseSetMaterialLightFlagOn(party->paperAnimGroup,
                                       animPoseGetMaterialLightFlag(mario->paperAnimGroupId[marioAnimeId()]));
    }
#ifdef MUST_MATCH
    partyChgPose(party, L_partyChgPoseId1(party));
#else
    partyChgPose(party, partyChgPoseId(party, 1));
#endif
    party->field_1C = 0;
    ((void (*)(PartyEntry*))party_dp->init_func)(party);
    return slotId;
}

// 100%: the pose block is the shared L_partyChgPoseId1 helper and the material setup is the shared
// L_setMaterial helper; vec3_802ccdcc is declared as an array so MWCC uses lis/addi (not @sda21).
s32 partyEntry2(s32 memberId) {
    MarioWork* player = marioGetPtr();
    s32 slotId;
    PartyEntry* party;
    const char* animName;

    if (partyPtrTbl[1] == NULL) {
        partyPtrTbl[1] = (PartyEntry*)__memAlloc(HEAP_DEFAULT, sizeof(PartyEntry));
        memset(partyPtrTbl[1], 0, sizeof(PartyEntry));
        slotId = 1;
    } else {
        slotId = -1;
    }
    if (slotId < 0) {
        return -1;
    }
    party = slotId >= 0 ? partyPtrTbl[slotId] : NULL;
    memset(party, 0, sizeof(PartyEntry));
    party->camId = marioGetCamId();
    party->paperAnimGroup = -1;
    party->field_10 = -1;
    party->paperPoseId = -1;
    party->flags = 9;
    party->currentSlotId = slotId;
    party->playerPtr = player;
    party->currentMemberId = memberId;
    if (slotId == 1) {
        party->flags |= 4;
    }
    party_dp = &partyDataTbl[memberId];
#ifdef MUST_MATCH
    animName = L_famicomAnimName(party);
#else
    {
        PartyData* dp = party_dp;

        if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
            if (!(party->flags2 & 0x8000)) {
                animName = dp->name;
            } else {
                animName = str_802CCEEC;
            }
        } else {
            animName = yoshigroup[pouchGetPartyColor(PARTNER_YOSHI)];
            if (party->flags2 & 0x8000) {
                animName = str_802CCEEC;
            }
        }
    }
#endif
    if (party->paperAnimGroup >= 0) {
        animPoseRelease(party->paperAnimGroup);
    }
    party->paperAnimGroup = animPoseEntry(animName, 2);
    animPosePeraOff(party->paperAnimGroup);
    L_setMaterial(party);
#ifdef MUST_MATCH
    partyChgPose(party, L_partyChgPoseId1(party));
#else
    partyChgPose(party, partyChgPoseId(party, 1));
#endif
    party->field_1C = 0;
    ((void (*)(PartyEntry*))party_dp->init_func)(party);
    party->position = player->position;
    party->field_B8 = party->position;
    {
        const Vec* v = vec3_802ccdcc;

        party->field_70 = *v;
    }
    party->field_34 = 0;
    return slotId;
}

s32 partyEntry2Hello(s32 memberId) {
    MarioWork* player = marioGetPtr();
    s32 slotId;
    PartyEntry* party;
    const char* animName;

    if (partyPtrTbl[1] == NULL) {
        partyPtrTbl[1] = (PartyEntry*)__memAlloc(HEAP_DEFAULT, sizeof(PartyEntry));
        memset(partyPtrTbl[1], 0, sizeof(PartyEntry));
        slotId = 1;
    } else {
        slotId = -1;
    }
    if (slotId < 0) {
        return -1;
    }
    party = slotId >= 0 ? partyPtrTbl[slotId] : NULL;
    memset(party, 0, sizeof(PartyEntry));
    party->camId = marioGetCamId();
    party->paperAnimGroup = -1;
    party->field_10 = -1;
    party->paperPoseId = -1;
    party->flags = 9;
    party->currentSlotId = slotId;
    party->playerPtr = player;
    party->currentMemberId = memberId;
    if (slotId == 1) {
        party->flags |= 4;
    }
    party_dp = &partyDataTbl[memberId];
#ifdef MUST_MATCH
    animName = L_animName(party);
#else
    {
        PartyData* dp = party_dp;

        if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
            if (!(party->flags2 & 0x8000)) {
                animName = dp->name;
            } else {
                animName = str_802CCEEC;
            }
        } else {
            animName = yoshigroup[pouchGetPartyColor(PARTNER_YOSHI)];
            if (party->flags2 & 0x8000) {
                animName = str_802CCEEC;
            }
        }
    }
#endif
    if (party->paperAnimGroup >= 0) {
        animPoseRelease(party->paperAnimGroup);
    }
    party->paperAnimGroup = animPoseEntry(animName, 2);
    animPosePeraOff(party->paperAnimGroup);
#ifdef MUST_MATCH
    L_setMaterial(party);
#else
    {
        MarioWork* mario = party->playerPtr;

        animPoseSetMaterialFlagOn(party->paperAnimGroup,
                                  animPoseGetMaterialFlag(mario->paperAnimGroupId[marioAnimeId()]));
        animPoseSetMaterialLightFlagOn(party->paperAnimGroup,
                                       animPoseGetMaterialLightFlag(mario->paperAnimGroupId[marioAnimeId()]));
    }
#endif
#ifdef MUST_MATCH
    // MUST_MATCH: same folded-index trick as partyEntry2Pos above.
    partyChgPose(party, L_partyChgPoseId1(party));
#else
    partyChgPose(party, partyChgPoseId(party, 1));
#endif


    party->field_1C = 0;
    ((void (*)(PartyEntry*))party_dp->init_func)(party);
    partyChgRunMode(party, 4);
    return slotId;
}

// Inlined anim-name selection behind a helper: inline, the result lands in r28 instead of the
// target's r29 and `str_802CCEEC` materialises via `addi r0,...; mr r28,r0`; a helper returning it
// gives it the target's r29.
static inline const char* L_animName(PartyEntry* party) {
    PartyData* dp = party_dp;

    if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
        if (!(party->flags2 & 0x8000)) {
            return dp->name;
        }
        return str_802CCEEC;
    }
    {
        const char* name = yoshigroup[pouchGetPartyColor(PARTNER_YOSHI)];
        if (party->flags2 & 0x8000) {
            name = str_802CCEEC;
        }
        return name;
    }
}
// Static inline copy of partyChgPoseId specialised for index 1 The original's
// inliner folds the `index == 1` chain for the literal argument; a direct partyChgPoseId(party, 1)
// call leaves the three-compare chain in, so this keeps the same `return`-based control flow the
// target emits with the index pre-folded. `v5` is declared before `index` and as `s32`: that pair
// is what makes the allocator give `index` r29 and `v5` r28, matching the target (any other
// declaration order/type leaves them swapped).
static inline char* L_partyChgPoseId1(PartyEntry* party) {
    s32 v5;
    s32 index = 1;
    char* v6;
    char* v8;

    if ((party->flags2 & 2) != 0) {
        return party->field_18;
    }
    v5 = 0;
    if (party->currentMemberId < 8 && partyGetHp(party->currentMemberId) <= 5) {
        v5 = TRUE;
    }
    if (v5 == TRUE) {
        index = 12;
    }
    if (!(party->flags2 & 0x8000)) {
        v6 = partyPoseTbl[party->currentMemberId][index];
    } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
        v6 = partyDotPoseTbl[party->currentMemberId][index];
    } else {
        v6 = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][index];
    }
    v8 = party->field_18;
    if (v8 && !strcmp(v8, v6)) {
        return v6;
    }
    party->field_18 = v6;
    party->field_20 = 0;
    party->field_8 |= 0x2000;
    return v6;
}

// Inlined material-flag setup behind a helper: inline, `mario` lands in r28 instead of the
// target's r29; a helper gives it a fresh allocation in r29.
static inline void L_setMaterial(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;

    animPoseSetMaterialFlagOn(party->paperAnimGroup,
                              animPoseGetMaterialFlag(mario->paperAnimGroupId[marioAnimeId()]));
    animPoseSetMaterialLightFlagOn(party->paperAnimGroup,
                                   animPoseGetMaterialLightFlag(mario->paperAnimGroupId[marioAnimeId()]));
}

s32 partyEntry2Pos(s32 memberId, f32 x, f32 y, f32 z) {
    MarioWork* player = marioGetPtr();
    s32 slotId;
    PartyEntry* party;
    const char* animName;

    if (partyPtrTbl[1] == NULL) {
        partyPtrTbl[1] = (PartyEntry*)__memAlloc(HEAP_DEFAULT, sizeof(PartyEntry));
        memset(partyPtrTbl[1], 0, sizeof(PartyEntry));
        slotId = 1;
    } else {
        slotId = -1;
    }
    if (slotId < 0) {
        return -1;
    }
    party = slotId >= 0 ? partyPtrTbl[slotId] : NULL;
    memset(party, 0, sizeof(PartyEntry));
    party->camId = marioGetCamId();
    party->paperAnimGroup = -1;
    party->field_10 = -1;
    party->paperPoseId = -1;
    party->flags = 9;
    party->currentSlotId = slotId;
    party->playerPtr = player;
    party->currentMemberId = memberId;
    if (slotId == 1) {
        party->flags |= 4;
    }
    party_dp = &partyDataTbl[memberId];
#ifdef MUST_MATCH
    animName = L_animName(party);
#else
    {
        PartyData* dp = party_dp;

        if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
            if (!(party->flags2 & 0x8000)) {
                animName = dp->name;
            } else {
                animName = str_802CCEEC;
            }
        } else {
            animName = yoshigroup[pouchGetPartyColor(PARTNER_YOSHI)];
            if (party->flags2 & 0x8000) {
                animName = str_802CCEEC;
            }
        }
    }
#endif
    if (party->paperAnimGroup >= 0) {
        animPoseRelease(party->paperAnimGroup);
    }
    party->paperAnimGroup = animPoseEntry(animName, 2);
    animPosePeraOff(party->paperAnimGroup);
#ifdef MUST_MATCH
    L_setMaterial(party);
#else
    {
        MarioWork* mario = party->playerPtr;

        animPoseSetMaterialFlagOn(party->paperAnimGroup,
                                  animPoseGetMaterialFlag(mario->paperAnimGroupId[marioAnimeId()]));
        animPoseSetMaterialLightFlagOn(party->paperAnimGroup,
                                       animPoseGetMaterialLightFlag(mario->paperAnimGroupId[marioAnimeId()]));
    }
#endif
#ifdef MUST_MATCH
    // MUST_MATCH: the original's inliner folds partyChgPoseId's `index == 1` chain for the
    // literal argument; L_partyChgPoseId1 keeps that folded control flow.
    partyChgPose(party, L_partyChgPoseId1(party));
#else
    partyChgPose(party, partyChgPoseId(party, 1));
#endif


    party->field_1C = 0;
    ((void (*)(PartyEntry*))party_dp->init_func)(party);
    partyChgRunMode(party, 2);
    {
        Vec pos = vec3_802ccdd8;

        pos.x = x;
        pos.y = y;
        pos.z = z;
        party->position = pos;
        party->field_B8 = party->position;
        party->field_70.x = 1.0f;
        party->field_70.y = 1.0f;
        party->field_70.z = 1.0f;
    }
    return slotId;
}

void partyInit(void) {
    memset(partyPtrTbl, 0, sizeof(partyPtrTbl));
}

// Folded-index pose selection for the literal index 1: the original's inliner folds the
// `index == 1` chain of partyChgPoseId here (matches L_partyChgPoseId1's shape).
static inline char* L_reinitPose1(PartyEntry* party) {
    s32 v5;
    s32 index = 1;
    char* v6;
    char* v8;

    if ((party->flags2 & 2) != 0) {
        return party->field_18;
    }
    v5 = 0;
    if (party->currentMemberId < 8 && partyGetHp(party->currentMemberId) <= 5) {
        v5 = TRUE;
    }
    if (v5 == TRUE) {
        index = 12;
    }
    if (!(party->flags2 & 0x8000)) {
        v6 = partyPoseTbl[party->currentMemberId][index];
    } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
        v6 = partyDotPoseTbl[party->currentMemberId][index];
    } else {
        v6 = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][index];
    }
    v8 = party->field_18;
    if (v8 && !strcmp(v8, v6)) {
        return v6;
    }
    party->field_18 = v6;
    party->field_20 = 0;
    party->field_8 |= 0x2000;
    return v6;
}

// Same body with the index as a parameter: in the later branch arms that makes the allocator
// keep `index` in r29 like the target, where the no-argument form swaps it with r28.
static inline char* L_reinitPose1b(PartyEntry* party, s32 index) {
    s32 v5;
    char* v6;
    char* v8;

    if ((party->flags2 & 2) != 0) {
        return party->field_18;
    }
    v5 = 0;
    if (party->currentMemberId < 8 && partyGetHp(party->currentMemberId) <= 5) {
        v5 = TRUE;
    }
    if (v5 == TRUE) {
        index = 12;
    }
    if (!(party->flags2 & 0x8000)) {
        v6 = partyPoseTbl[party->currentMemberId][index];
    } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
        v6 = partyDotPoseTbl[party->currentMemberId][index];
    } else {
        v6 = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][index];
    }
    v8 = party->field_18;
    if (v8 && !strcmp(v8, v6)) {
        return v6;
    }
    party->field_18 = v6;
    party->field_20 = 0;
    party->field_8 |= 0x2000;
    return v6;
}

// Same anim-name selection as the second party's block, behind a helper so the result keeps the
// target's r29 (inline it lands in r28).
static inline const char* L_reinitAnimName(PartyEntry* party, const u8* base) {
    if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
        if (!(party->flags2 & 0x8000)) {
            return party_dp->name;
        }
        return (const char*)(base + 0x1bc);
    }
    {
        const char* name = yoshigroup[pouchGetPartyColor(PARTNER_YOSHI)];

        if (party->flags2 & 0x8000) {
            name = (const char*)(base + 0x1bc);
        }
        return name;
    }
}

// NONMATCHING (99.82%): the second block's inlined pose value lands in r28 instead
// of r29, plus one branch-target difference.
void partyReInit(void) {
    const u8* base = (const u8*)vec3_802ccd30;
    PartyEntry* party = partyGetPtr(marioGetExtraPartyId());

    if (party) {
        s32 paperAnimGroup;

        party->flags = 0xd;
        party->flags2 = 0;
        paperAnimGroup = party->paperAnimGroup;
        if (paperAnimGroup >= 0) {
            animPoseSetMaterialFlagOff(paperAnimGroup, -1);
            animPoseSetMaterialLightFlagOn(paperAnimGroup, 2);
        }
        P8(party, 0x37) = 0;
        if (party->field_1C) {
            animPoseSetPaperAnimGroup(party->paperAnimGroup, NULL, 0);
            party->field_1C = 0;
            party->flags2 &= ~0x04000000;
            if (party->paperPoseId >= 0) {
                animPaperPoseRelease(party->paperPoseId);
                party->paperPoseId = -1;
            }
        }
        P32(party, 0x130) = 0;
        P32(party, 0x12C) = 0;
        P32(party, 0x148) = 0;
        P32(party, 0x138) = 0;
        P32(party, 0x13C) = 0;
        P32(party, 0x134) = 0;
        P32(party, 0x140) = 0;
        P32(party, 0x144) = 0;
        partyChgPose(party, L_reinitPose1(party));
        partyChgRunMode(party, 2);
        partyChgMot(party, 0);
        party->yoshiPsndSFXId = -1;
        party->field_70 = *(const Vec*)(base + 0xb4);
    }

    party = partyGetPtr(marioGetPartyId());
    if (party) {
        s32 paperAnimGroup;
        s32 playSound = party->flags2 & 0x8000;

        party->flags = 1;
        party->flags2 = 0;
        P8(party, 0x37) = 0;
        party->camId = marioGetCamId();
        if (party) {
            paperAnimGroup = party->paperAnimGroup;
            if (paperAnimGroup >= 0) {
                animPoseSetMaterialFlagOff(paperAnimGroup, -1);
                animPoseSetMaterialLightFlagOn(paperAnimGroup, 2);
            }
            if (party->field_1C) {
                animPoseSetPaperAnimGroup(party->paperAnimGroup, NULL, 0);
                party->field_1C = 0;
                party->flags2 &= ~0x04000000;
                if (party->paperPoseId >= 0) {
                    animPaperPoseRelease(party->paperPoseId);
                    party->paperPoseId = -1;
                }
            }
            P32(party, 0x130) = 0;
            P32(party, 0x12C) = 0;
            P32(party, 0x148) = 0;
            P32(party, 0x138) = 0;
            P32(party, 0x13C) = 0;
            P32(party, 0x134) = 0;
            P32(party, 0x140) = 0;
            P32(party, 0x144) = 0;
            party->field_70 = *(const Vec*)(base + 0xc0);
        }
        partyClearFootmark();
        if (playSound) {
            const char* animName = L_reinitAnimName(party, base);

            if (party->paperAnimGroup >= 0) {
                animPoseRelease(party->paperAnimGroup);
            }
            party->paperAnimGroup = animPoseEntry(animName, 2);
        }
        if (!(party->currentMemberId == PARTY_MEMBER_YOSHI && P8(party, 0x34) == 3)) {
            MarioWork* mario = marioGetPtr();

            if (!(mario->flags & 0x400000)) {
                partyChgPose(party, L_reinitPose1b(party, 1));
                partyChgRunMode(party, 2);
                partyChgMot(party, 0);
            } else {
                partyChgPose(party, L_reinitPose1b(party, 1));
                partyChgRunMode(party, 2);
                P8(party, 0x3B) = 6;
                P8(party, 0x38) = 0;
                party->flags |= 0x8000;
            }
            if (party->currentMemberId == PARTY_MEMBER_MS_MOWZ) {
                chuchu_reinit(party);
            }
            unk_8014c47c();
        }
    }
}

void partyUsePost(PartyEntry* party) {
    typedef void (*PartyFunc)(PartyEntry*);

    if (party && party->field_34 == 3) {
        PartyFunc post = (PartyFunc)partyDataTbl[party->currentMemberId].post_func;

        if (post) {
            post(party);
        }
    }
}

void N__partyChgRunMode(PartyEntry* party, s32 oldMode, s32 newMode) {
    typedef void (*PartyFunc)(PartyEntry*);
    const Vec* v = (const Vec*)vec3_802ccd30;
    s32 index;
    char* pose;
    MarioWork* player;

    switch (oldMode) {
        case 11:
            if (newMode == 8) {
                break;
            }
            player = party->playerPtr;
            {
                Vec* dst = &party->field_70;

                dst[0] = v[33];
                dst[1] = v[34];
            }
            party->field_FC = PF32(player, 0x19C);
            party->dir = player->directionView;
            party->field_110 = PF32(player, 0x1AC);
            party->flags &= ~0x70000;
            partyClearJumpPara(party);
            index = 1;
            if (!(party->flags2 & 2)) {
                s32 lowHp = 0;
                if (party->currentMemberId < 8 && partyGetHp(party->currentMemberId) <= 5) {
                    lowHp = 1;
                }
                if (lowHp == 1) {
                    index = 12;
                }
                if (!(party->flags2 & 0x8000)) {
                    pose = partyPoseTbl[party->currentMemberId][index];
                } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
                    pose = partyDotPoseTbl[party->currentMemberId][index];
                } else {
                    pose = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][index];
                }
                if (!(party->field_18 && !strcmp(party->field_18, pose))) {
                    party->field_18 = pose;
                    party->field_20 = 0;
                    party->field_8 |= 0x2000;
                }
            }
            party->flags2 &= ~0x10;
            party->flags2 &= ~1;
            if (party) {
                party->flags2 &= ~0x100000;
            }
            break;
        case 4:
            player = party->playerPtr;
            party->field_70 = v[23];
            party->flags2 &= ~0x10;
            party->field_FC = PF32(player, 0x19C);
            party->dir = player->directionView;
            party->field_110 = PF32(player, 0x1AC);
            party->field_10C = player->unk1A8;
            partyClearJumpPara(party);
            index = 1;
            if (!(party->flags2 & 2)) {
                s32 lowHp = 0;
                if (party->currentMemberId < 8 && partyGetHp(party->currentMemberId) <= 5) {
                    lowHp = 1;
                }
                if (lowHp == 1) {
                    index = 12;
                }
                if (!(party->flags2 & 0x8000)) {
                    pose = partyPoseTbl[party->currentMemberId][index];
                } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
                    pose = partyDotPoseTbl[party->currentMemberId][index];
                } else {
                    pose = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][index];
                }
                if (!(party->field_18 && !strcmp(party->field_18, pose))) {
                    party->field_18 = pose;
                    party->field_20 = 0;
                    party->field_8 |= 0x2000;
                }
            }
            party->flags2 &= ~0x10;
            party->field_34 = 1;
            break;
        case 12:
            if (party) {
                party->flags2 &= ~0x100000;
            }
            break;
        case 3:
            if (partyDataTbl[party->currentMemberId].post_func) {
                ((PartyFunc)partyDataTbl[party->currentMemberId].post_func)(party);
            }
            break;
        case 1:
        case 2:
            L_partyForceSlitOff(party);
            break;
        default:
            break;
    }
}

// Keep partyChgRunMode out of line: the original calls it rather than inlining.
#pragma dont_inline on
void partyChgRunMode(PartyEntry* party, s32 mode) {
    if (party->field_34 != 9 && party->field_34 != 3) {
        party->field_35 = party->field_34;
    }
    N__partyChgRunMode(party, party->field_34, mode);
    party->field_34 = mode;
    party->field_8 |= 8;
}
#pragma dont_inline reset

// Keep N_partyForceChgRunMode out of line: the original calls it rather than inlining.
#pragma dont_inline on
void N_partyForceChgRunMode(PartyEntry* party, s32 mode, s32 unk) {
    if (party->field_34 != 9 && party->field_34 != 3) {
        party->field_35 = party->field_34;
    }
    N__partyChgRunMode(party, party->field_34, mode);
    party->field_34 = mode;
    party->field_8 &= ~8;
    P8(party, 0x38) = unk;
}
#pragma dont_inline reset

void unk_800ba2dc(PartyEntry* party) {
    typedef void (*PartyFunc)(PartyEntry*);
    const Vec* v = (const Vec*)vec3_802ccd30;
    s32 index;
    char* pose;
    MarioWork* player;

    if (party->field_34 == party->field_35) {
        return;
    }
    if (party->field_34 == 4) {
            player = party->playerPtr;
            party->field_70 = v[23];
            party->flags2 &= ~0x10;
            party->field_FC = PF32(player, 0x19C);
            party->dir = player->directionView;
            party->field_110 = PF32(player, 0x1AC);
            party->field_10C = player->unk1A8;
            partyClearJumpPara(party);
            index = 1;
            if (!(party->flags2 & 2)) {
                s32 lowHp = 0;
                if (party->currentMemberId < 8 && partyGetHp(party->currentMemberId) <= 5) {
                    lowHp = 1;
                }
                if (lowHp == 1) {
                    index = 12;
                }
                if (!(party->flags2 & 0x8000)) {
                    pose = partyPoseTbl[party->currentMemberId][index];
                } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
                    pose = partyDotPoseTbl[party->currentMemberId][index];
                } else {
                    pose = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][index];
                }
                if (!(party->field_18 && !strcmp(party->field_18, pose))) {
                    party->field_18 = pose;
                    party->field_20 = 0;
                    party->field_8 |= 0x2000;
                }
            }
            party->flags2 &= ~0x10;
            party->field_34 = 1;
            if (party && party->field_34 == 3) {
                if (partyDataTbl[party->currentMemberId].post_func) {
                    ((PartyFunc)partyDataTbl[party->currentMemberId].post_func)(party);
                }
            }
            partyChgMot(party, 0);
    } else if (party->field_34 == 11) {
            player = party->playerPtr;
            {
                Vec* dst = &party->field_70;

                dst[0] = v[33];
                dst[1] = v[34];
            }
            party->field_FC = PF32(player, 0x19C);
            party->dir = player->directionView;
            party->field_110 = PF32(player, 0x1AC);
            party->flags &= ~0x70000;
            partyClearJumpPara(party);
            index = 1;
            if (!(party->flags2 & 2)) {
                s32 lowHp = 0;
                if (party->currentMemberId < 8 && partyGetHp(party->currentMemberId) <= 5) {
                    lowHp = 1;
                }
                if (lowHp == 1) {
                    index = 12;
                }
                if (!(party->flags2 & 0x8000)) {
                    pose = partyPoseTbl[party->currentMemberId][index];
                } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
                    pose = partyDotPoseTbl[party->currentMemberId][index];
                } else {
                    pose = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][index];
                }
                if (!(party->field_18 && !strcmp(party->field_18, pose))) {
                    party->field_18 = pose;
                    party->field_20 = 0;
                    party->field_8 |= 0x2000;
                }
            }
            party->flags2 &= ~0x10;
            party->flags2 &= ~1;
            if (party) {
                party->flags2 &= ~0x100000;
            }
            if (party && party->field_34 == 3) {
                if (partyDataTbl[party->currentMemberId].post_func) {
                    ((PartyFunc)partyDataTbl[party->currentMemberId].post_func)(party);
                }
            }
            partyChgMot(party, 0);
    } else {
            if (party->field_34 == 12) {
                if (party) {
                    party->flags2 &= ~0x100000;
                }
            }
            if (party && party->field_34 == 3) {
                if (partyDataTbl[party->currentMemberId].post_func) {
                    ((PartyFunc)partyDataTbl[party->currentMemberId].post_func)(party);
                }
            }
            party->field_34 = party->field_35;
            party->field_8 |= 8;
            partyChgMot(party, 0);
    }
}

s32 partyGetNum(void) {
    s32 num = 0;
    s32 i;

    for (i = 0; i < 2; i++) {
        PartyEntry* party = partyGetPtr(i);

        if (party && (party->flags & 1)) {
            num++;
        }
    }
    return num;
}

// Original inlined body of partyGetNum (the original never emits that function standalone):
// the table element must be loaded before the `index >= 0` test that partyGetPtr inlines, and
// the element load is shared with the flags dereference. The plain `party = partyGetPtr(i)`
// loop loads the element only on the non-negative path, which is what the old version did.
static inline s32 L_partyNum(void) {
    s32 num = 0;
    s32 i;

    for (i = 0; i < 2; i++) {
        if (partyPtrTbl[i] && (partyGetPtr(i)->flags & 1)) {
            num++;
        }
    }
    return num;
}

void partyMain(void) {
    s32 num;
    s32 i;

    marioGetPtr();
    num = L_partyNum();
    if (num != 0) {
        unk_8013be14();
        for (i = 0; i < 2; i++) {
            PartyEntry* party = partyPtrTbl[i];

            if (party && (party->flags & 1) && marioBgmodeChk() != 1) {
                driveParty(party);
            }
        }
    }
}

// The original reads these rodata Vecs/strings as offsets from the unit's first rodata object.

static inline char* L_partyChgPoseId1(PartyEntry* party);

void driveParty(PartyEntry* party) {
    const u8* base = (const u8*)vec3_802ccd30;
    MarioWork* player = party->playerPtr;

    PF32(party, 0xF8) = player->targetYaw; // 0xF8: inside PartyEntry field_F8
    if (party->field_2A > 0) {
        party->field_2A--;
    }
    switch (P8(party, 0x34)) {
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 0xe:
            break;
        default:
            if (party->flags & 0x20000000) {
                if (vivianGetStatus() != 2) {
                    vivianShadow(party);
                }
                partyPreDisp(party);
                return;
            }
            break;
    }
    if (player->flags & 0x02000000) {
        party->flags |= 0x00800000;
    } else {
        party->flags &= ~0x00800000;
    }
    party->field_138 = NULL;
    party->field_12C = NULL;
    party->field_140 = NULL;
    party->field_134 = NULL;
    P32(party, 0x130) = 0;
    switch (P8(party, 0x34)) {
        case 0:
            if (!(party->flags & 0x20000000) || (party->flags & 0x10000000)) {
                if (party->field_8 & 8) {
                    party->field_8 &= ~8;
                    L_partyChgPoseId1(party);
                    PF32(party, 0x104) = 0.0f;
                    party->flags &= ~0x100;
                    partyChgMot(party, 0);
                }
                party_dp = &partyDataTbl[party->currentMemberId];
                ((void (*)(PartyEntry*))party_dp->move_func)(party);
            }
            break;
        case 1:
            if (!(party->flags & 0x20000000) || (party->flags & 0x10000000)) {
                unk_800c7c98(party);
            }
            break;
        case 2:
            if (!(party->flags & 0x20000000) || (party->flags & 0x10000000)) {
                party_dp = &partyDataTbl[party->currentMemberId];
                ((void (*)(PartyEntry*))party_dp->move_func)(party);
            }
            break;
        case 9:
            if (!(party->flags & 0x20000000) || (party->flags & 0x10000000)) {
                partyForceWalk(party);
            }
            break;
        case 3:
            if (!(party->flags & 0x20000000) || (party->flags & 0x10000000)) {
                void* fn;

                party_dp = &partyDataTbl[party->currentMemberId];
                fn = party_dp->use_func;
                party->flags |= 0x100;
                ((void (*)(PartyEntry*))fn)(party);
            }
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            if (party->field_8 & 8) {
                party->field_8 &= ~8;
                player = party->playerPtr;
                if (P16(player, 0x2E) == 0x16 || P16(player, 0x2E) == 0x18 || P16(player, 0x2E) == 0x19) {
                    P8(party, 0x38) = 0x1e;
                } else if (P8(party, 0x34) == 4 || P8(party, 0x34) == 5) {
                    party->flags2 |= 0x10;
                    party->field_70 = DK_FV(0xd8, vec3_802cce08);
                    P8(party, 0x38) = 0;
                    party->field_110 = player->unk1A8;
                    party->field_10C = player->unk1A8;
                    party->position = player->position;
                    party->field_B8 = party->position;
                } else if (P8(party, 0x34) == 7) {
                    party->field_70 = DK_FV(0xe4, vec3_802cce14);
                    P8(party, 0x38) = 0x14;
                    party->field_110 = player->unk1A8;
                    party->field_10C = player->unk1A8;
                } else {
                    party->field_70 = DK_FV(0xf0, vec3_802cce20);
                    P8(party, 0x38) = 0xa;
                    party->field_110 = player->unk1A8;
                    party->field_10C = player->unk1A8;
                    party->position = player->position;
                    party->field_B8 = party->position;
                }
                party->flags &= ~0x100;
            }
            partyHelloMain(party);
            break;
        case 8:
            party_dp = &partyDataTbl[party->currentMemberId];
            ((void (*)(PartyEntry*))party_dp->bye_func)(party);
            break;
        case 10:
            if (party->field_8 & 8) {
                party->field_8 &= ~8;
                partyDokanInit(party);
                party->flags &= ~0x100;
            }
            partyDokanMain(party);
            break;
        case 11:
            if (!(party->flags & 0x20000000) || (party->flags & 0x10000000)) {
                if (party->field_8 & 8) {
                    party->field_8 &= ~8;
                    P8(party, 0x38) = 0;
                    party->flags &= ~0x100;
                    L_partyForceSlitOff(party);
                }
                partyRideMain(party);
            }
            break;
        case 12:
            if (!(party->flags & 0x20000000) || (party->flags & 0x10000000)) {
                if (party->field_8 & 8) {
                    party->field_8 &= ~8;
                    P8(party, 0x38) = 0;
                    party->flags &= ~0x100;
                }
                partyRideYoshiMain(party);
            }
            break;
        case 13:
            if (!(party->flags & 0x20000000) || (party->flags & 0x10000000)) {
                party_damage_return(party);
            }
            break;
        case 14:
            if (party->field_8 & 8) {
                party->field_8 &= ~8;
            }
            {
                player->prevPartyMemberId[party->currentSlotId] = party->currentMemberId;
                player->currPartySlotId[party->currentSlotId] = -1;
                partyPtrTbl[party->currentSlotId] = NULL;
                animPoseRelease(party->paperAnimGroup);
                __memFree(HEAP_DEFAULT, party);
            }
            return;
        default:
            break;
    }
    party_kemuri(party);
    {
        Vec scr = party->position;
        f32 scrX;
        f32 scrY;
        f32 scrZ;

        ((void (*)(Vec*, f32*, f32*, f32*))marioGetScreenPos)(&scr, &scrX, &scrY, &scrZ);
        if (marioChkInScreen((s32)scrX, (s32)scrY)) {
            party->flags2 &= ~0x00800000;
        } else {
            party->flags2 |= 0x00800000;
        }
    }
    *partyVecAt(party, 0xAC) = DK_FV(0xcc, vec3_802ccdfc);
    partyPreDisp(party);
}

s32 partyDoWork(void) {
    PartyEntry* party = partyGetPtr(marioGetPtr()->currPartySlotId[0]);
    f32 speed;

    if (!party) {
        return 0;
    }
    if (party->field_34 != 0 && party->field_34 != 1 && party->field_34 != 2) {
        return 0;
    }
    speed = 0.0f;
    marioGetPtr()->baseSpeed = speed;
    partyChgRunMode(party, 3);
    return 1;
}

void partyHello(PartyEntry* party) {
    const f32* v = vec3_802ccd30;
    MarioWork* mario;

    if (party->field_8 & 8) {
        party->field_8 &= ~8;
        mario = party->playerPtr;
        if (P16(mario, 0x2E) == 0x16 || P16(mario, 0x2E) == 0x18 || P16(mario, 0x2E) == 0x19) {
            P8(party, 0x38) = 0x1e;
        } else if (party->field_34 == 4 || party->field_34 == 5) {
            party->flags2 |= 0x10;
            party->field_70 = *(Vec*)(v + 54);
            P8(party, 0x38) = 0;
            party->field_110 = mario->unk1A8;
            party->field_10C = mario->unk1A8;
            party->position = mario->position;
            party->field_B8 = party->position;
        } else if (party->field_34 == 7) {
            party->field_70 = *(Vec*)(v + 57);
            P8(party, 0x38) = 0x14;
            party->field_110 = mario->unk1A8;
            party->field_10C = mario->unk1A8;
        } else {
            party->field_70 = *(Vec*)(v + 60);
            P8(party, 0x38) = 0xa;
            party->field_110 = mario->unk1A8;
            party->field_10C = mario->unk1A8;
            party->position = mario->position;
            party->field_B8 = party->position;
        }
        party->flags &= ~0x100;
    }
    partyHelloMain(party);
}

s32 partyHelloMain(PartyEntry* party) {
    char* pose;
    s32 index;
    s32 lowHp;
    f32 x;
    f32 y;
    f32 z;
    s32 ret = 0;
    MarioWork* player = party->playerPtr;

    switch (P8(party, 0x38)) {
        case 0:
            x = player->position.x;
            y = player->position.y;
            z = player->position.z;
            party->position.x = x;
            party->position.y = 0.5f * PF32(player, 0x1B8) + y;
            party->position.z = z;
            partyGetAppearPos(party, partyVecAt(party, 0x94));
            PS32(party, 0x24) = 16;
            PF32(party, 0x128) = distABf(x, z, PF32(party, 0x94), PF32(party, 0x9C));
            party->dir = angleABf(x, z, PF32(party, 0x94), PF32(party, 0x9C));
            party->speed = PF32(party, 0x128) / (f32)PS32(party, 0x24);
            {
                f32 half = 0.5f * (f32)PS32(party, 0x24);
                f32 dy = PF32(party, 0x98) - party->position.y;
                f32 q;

                if (__fabsf(dy) < float_1_8041b0e8) {
                    dy = float_10_8041b124;
                } else if (dy >= float_0_8041b100) {
                    dy += 0.5f * PF32(party, 0xF0);
                } else {
                    dy = float_10_8041b124;
                }
                q = (dy + dy) / (half * half - half);
                party->jumpPara1 = q * half;
                party->jumpPara2 = -q;
            }
            party->flags |= 0x10;
            P8(party, 0x38) = 1;
            break;
        case 1:
            partyMoveNoHosei(party);
            if (party->jumpPara1 <= float_0_8041b100) {
                party->flags &= ~0x10;
                party->jumpPara2 = float_neg1p6_8041b180;
            }
            if (party->position.y < float_neg2000_8041b164) {
                PS32(party, 0x24) = 0;
                P8(party, 0x38) = 2;
                break;
            }
            if (party->field_138 == NULL) {
                f32 t = (f32)(0x10 - PS32(party, 0x24)) * float_0p0625_8041b184;
                Vec v = vec3_802cce2c;

                v.x = t;
                v.y = t;
                v.z = t;
                party->field_70 = v;
                if (--PS32(party, 0x24) <= 0) {
                    PS32(party, 0x24) = 0;
                }
                break;
            }
            P8(party, 0x38) = 2;
        case 2:
        {
            f32 t = (f32)(0x10 - PS32(party, 0x24)) * float_0p0625_8041b184;
            Vec v = vec3_802cce38;

            v.x = t;
            v.y = t;
            v.z = t;
            party->field_70 = v;
            if (--PS32(party, 0x24) > 0) {
                break;
            }
            PS32(party, 0x24) = 0;
            if (PF32(party, 0x110) >= float_90_8041b0f0 && PF32(party, 0x110) <= float_270_8041b0f4) {
                party->field_FC = partyToMovedir(party, float_180_8041b0fc);
                party->dir = party->field_FC;
                party->field_10C = float_180_8041b0fc;
                party->field_110 = float_180_8041b0fc;
            } else {
                party->field_FC = partyToMovedir(party, float_0_8041b100);
                party->dir = party->field_FC;
                party->field_10C = float_0_8041b100;
                party->field_110 = float_0_8041b100;
            }
            partyClearJumpPara(party);
            index = 1;
            if (!(party->flags2 & 2)) {
                lowHp = 0;
                if (party->currentMemberId < 8 && partyGetHp(party->currentMemberId) <= 5) {
                    lowHp = 1;
                }
                if (lowHp == 1) {
                    index = 12;
                }
                if (!(party->flags2 & 0x8000)) {
                    pose = partyPoseTbl[party->currentMemberId][index];
                } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
                    pose = partyDotPoseTbl[party->currentMemberId][index];
                } else {
                    pose = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][index];
                }
                if (!(party->field_18 && !strcmp(party->field_18, pose))) {
                    party->field_18 = pose;
                    party->field_20 = 0;
                    party->field_8 |= 0x2000;
                }
            }
            party->flags2 &= ~0x10;
            P8(party, 0x34) = 1;
            ret = 1;
            psndSFXOn_3D((const char*)0x20920, &player->position);
            break;
        }
        case 10: {
            s32 lowHp;
            s32 index;

            partyGetAppearPos2(party, &party->position);
            party->field_B8 = party->position;
            index = 1;
            if (!(party->flags2 & 2)) {
                lowHp = 0;
                if (party->currentMemberId < 8 && partyGetHp(party->currentMemberId) <= 5) {
                    lowHp = 1;
                }
                if (lowHp == 1) {
                    index = 12;
                }
                if (!(party->flags2 & 0x8000)) {
                    index = (s32)partyPoseTbl[party->currentMemberId][index];
                } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
                    index = (s32)partyDotPoseTbl[party->currentMemberId][index];
                } else {
                    index = (s32)yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][index];
                }
                if (!(party->field_18 && !strcmp(party->field_18, (char*)index))) {
                    party->field_18 = (char*)index;
                    party->field_20 = 0;
                    party->field_8 |= 0x2000;
                }
            }
            P8(party, 0x34) = 1;
            ret = 1;
            break;
        }
        case 20: {
            s32 lowHp;
            s32 index;

            index = 1;
            if (!(party->flags2 & 2)) {
                lowHp = 0;
                if (party->currentMemberId < 8 && partyGetHp(party->currentMemberId) <= 5) {
                    lowHp = 1;
                }
                if (lowHp == 1) {
                    index = 12;
                }
                if (!(party->flags2 & 0x8000)) {
                    index = (s32)partyPoseTbl[party->currentMemberId][index];
                } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
                    index = (s32)partyDotPoseTbl[party->currentMemberId][index];
                } else {
                    index = (s32)yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][index];
                }
                if (!(party->field_18 && !strcmp(party->field_18, (char*)index))) {
                    party->field_18 = (char*)index;
                    party->field_20 = 0;
                    party->field_8 |= 0x2000;
                }
            }
            P8(party, 0x34) = 0;
            ret = 1;
            break;
        }
        case 30:
            allPartyForceRideOn();
            ret = 1;
            break;
    }
    return ret;
}

BOOL partyGoodbyeInit(PartyEntry* party) {
    typedef void (*PartyFunc)(PartyEntry*);
    u16 v;

    P8(party, 0x38) = 10;
    party_dp = &partyDataTbl[party->currentMemberId];
    if (party_dp->init_func) {
        ((PartyFunc)party_dp->init_func)(party);
    }
    if (party->flags & PARTY_FLAG_IS_BEING_USED) {
        if (party->currentMemberId == PARTY_MEMBER_VIVIAN) {
            vivianUnhold(party);
            P8(party, 0x38) = 0;
        }
        if (party->currentMemberId == PARTY_MEMBER_YOSHI) {
            marioGetoffYoshi();
            P8(party, 0x38) = 0;
        }
    }
    v = *(u16*)((u8*)party->playerPtr + 0x2E);
    if (v == 0x16 || v == 0x18 || v == 0x19) {
        P8(party, 0x38) = 0x14;
    }
    return FALSE;
}

#pragma opt_propagation off // without it the case-10 drop/speed arithmetic puts drop in f1 and rotates f1-f5; off reproduces the original's f5/f4/f1/f3/f2 set
BOOL partyGoodbyeMain(PartyEntry* party) {
    typedef void (*PartyFunc)(PartyEntry*);
    MarioWork* mario = marioGetPtr();
    char* pose;
    s32 index;

    switch (P8(party, 0x38)) {
        case 0:
            party_dp = &partyDataTbl[party->currentMemberId];
            ((PartyFunc)party_dp->use_func)(party);
            if (party->currentMemberId == PARTY_MEMBER_VIVIAN) {
                if (vivianGetStatus() == 0) {
                    P8(party, 0x38) = 10;
                    party->field_34 = 8;
                }
                break;
            }
            if (party->currentMemberId == PARTY_MEMBER_YOSHI) {
                if (yoshiGetStatus() == 0) {
                    P8(party, 0x38) = 10;
                    party->field_34 = 8;
                }
                break;
            }
            P8(party, 0x38) = 10;
            party->field_34 = 8;
            break;
        case 10: {
            f32 drop;
            f32 my;
            f32 mx;
            f32 mz;
            f32 px;
            f32 py;
            f32 pz;

            psndSFXOn_3D((const char*)0x2091F, &mario->position);
            mx = mario->position.x;
            PF32(party, 0x94) = mario->position.x;
            my = 0.5f * mario->wPlayerCollisionBox.y + mario->position.y;
            PF32(party, 0x98) = my;
            mz = mario->position.z;
            PF32(party, 0x9C) = mario->position.z;
            px = party->position.x;
            pz = party->position.z;
            py = party->position.y;
            party->speed = 4.0f;
            party->jumpPara3 = 1.6f;
            PF32(party, 0x128) = distABf(px, pz, mx, mz);
            party->dir = angleABf(px, pz, mx, mz);
            PS32(party, 0x24) = 15;
            drop = my - py;
            party->speed = PF32(party, 0x128) / (f32)PS32(party, 0x24);
            party->jumpPara1 = (0.5f * (party->jumpPara3 * (f32)PS32(party, 0x24) * (f32)PS32(party, 0x24)) + drop) /
                               (f32)PS32(party, 0x24);
            if (!(party->flags2 & 2)) {
                if (party->currentMemberId < 8) {
                    partyGetHp(party->currentMemberId);
                }
                if (!(party->flags2 & 0x8000)) {
                    pose = *(char**)((u8*)partyPoseTbl + party->currentMemberId * 0x3c + 16);
                } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
                    pose = *(char**)((u8*)partyDotPoseTbl + party->currentMemberId * 0x3c + 16);
                } else {
                    pose = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][4];
                }
                if (!(party->field_18 && !strcmp(party->field_18, pose))) {
                    party->field_18 = pose;
                    party->field_20 = 0;
                    party->field_8 |= 0x2000;
                }
            }
            P8(party, 0x38) = 11;
            break;
        }
        case 11: {
            f32 t;

            party->jumpPara1 -= party->jumpPara3;
            party->position.y += party->jumpPara1;
            movePos(&party->position.x, &party->position.z, party->speed, party->dir);
            party->field_10C = revise360(50.0f + party->field_10C);
            t = (f32)PS32(party, 0x24);
            if (t > 10.0f) {
                t = 10.0f;
            }
            party->field_70.x = t / 10.0f;
            party->field_70.y = party->field_70.x;
            party->field_70.z = party->field_70.x;
            if (--PS32(party, 0x24) < 0) {
                P8(party, 0x38) = 12;
            }
            break;
        }
        case 12:
            index = 1;
            if (!(party->flags2 & 2)) {
                s32 lowHp;
                lowHp = 0;
                if (party->currentMemberId < 8 && partyGetHp(party->currentMemberId) <= 5) {
                    lowHp = 1;
                }
                if (lowHp == 1) {
                    index = 12;
                }
                if (!(party->flags2 & 0x8000)) {
                    pose = partyPoseTbl[party->currentMemberId][index];
                } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
                    pose = partyDotPoseTbl[party->currentMemberId][index];
                } else {
                    pose = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][index];
                }
                if (!(party->field_18 && !strcmp(party->field_18, pose))) {
                    party->field_18 = pose;
                    party->field_20 = 0;
                    party->field_8 |= 0x2000;
                }
            }
            party->jumpPara1 = 0.0f;
            party->position.y = PF32(party, 0x98);
            partyChgRunMode(party, 0xe);
            return TRUE;
        case 20:
            partyChgRunMode(party, 0xe);
            return TRUE;
    }
    return FALSE;
}
#pragma opt_propagation reset

// Manually inlined partyPaperOn with the comparison strings forced to the unit's original
// rodata offsets (its literals pool at the wrong place with the file's functions out of order).
// A macro, not a function: the original inliner substitutes the anim expression at every use and
// rematerialises `addi rX, r31, off`, where a real inline function caches it in a callee-saved reg.

#pragma opt_common_subs off // rematerialise the constant anim address instead of caching it in a callee-saved register
s32 partyDokanInit(PartyEntry* party) {
    const u8* base = (const u8*)vec3_802ccd30;
    MarioWork* player = party->playerPtr;
    char* pose;

    party->flags2 |= 0x10;
    switch (PS8(player, 0x3E)) { // 0x3E: MarioWork field_0x3E, a signed byte, not named in mario.h
        case 2:
        case 3:
            L_DOKAN_PAPER_ON(party, DK_FB(0x1e0, "p_dokan_y"));
            PF32(party, 0x124) = 30.0f;
            P8(party, 0x38) = 0;
            break;
        case 0:
            party->field_70 = DK_FV(0x120, vec3_802cce50);
            // fallthrough
        case 1:
            L_DOKAN_PAPER_ON(party, DK_FB(0x1e0, "p_dokan_y"));
            PF32(party, 0x124) = 30.0f;
            P8(party, 0x38) = 0;
            break;
        case 5:
            L_DOKAN_PAPER_ON(party, DK_FB(0x1fc, "p_dokan_x"));
            PF32(party, 0x124) = 26.0f;
            P8(party, 0x38) = 0xa;
            PF32(party, 0x8C) = 10.0f;
            break;
        case 4:
            L_DOKAN_PAPER_ON(party, DK_FB(0x1fc, "p_dokan_x"));
            PF32(party, 0x124) = -26.0f;
            P8(party, 0x38) = 0xa;
            PF32(party, 0x8C) = 10.0f;
            PF32(party, 0x64) = revise360(180.0f + PF32(player, 0xBC)); // 0xBC: inside MarioWork field_0x98
            break;
        default:
            break;
    }
    partyChgPaper(party, DK_FB(0x210, "PM_D_1A"));
    if (!(party->flags2 & 2)) {
        s32 memberId = (s8)party->currentMemberId;

        if (memberId < 8) {
            partyGetHp(memberId);
        }
        if (!(party->flags2 & 0x8000)) {
            pose = partyPoseTbl[party->currentMemberId][0];
        } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
            pose = partyDotPoseTbl[party->currentMemberId][0];
        } else {
            pose = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][0];
        }
        if (!(party->field_18 && !strcmp(party->field_18, pose))) {
            party->field_18 = pose;
            party->field_20 = 0;
            party->field_8 |= 0x2000;
        }
    }
    party->flags2 |= 8;
    party->flags2 |= 0x10;
    animPoseSetMaterialLightFlagOff(party->paperAnimGroup, 1);
    party->field_110 = revise360(180.0f + player->unk1A8);
    party->field_10C = player->unk1A8;
    party->field_FC = PF32(player, 0x19C); // 0x19C: inside MarioWork unk19C
    party->position = player->position;
    L_partyVivianTailStop(party);
    return 0;
}
#pragma opt_common_subs reset

s32 partyDokanMain(PartyEntry* party) {
    const u8* base = (const u8*)vec3_802ccd30;
    MarioWork* player = party->playerPtr;
    s32 count;

    switch (P8(party, 0x38)) {
        case 0:
            party->position = player->position;
            party->field_10C = revise360(party->field_10C + PF32(party, 0x124));
            party->field_70.x += 0.05f;
            if (party->field_70.x >= 1.0f) {
                party->field_70.x = 1.0f;
            }
            party->field_70.y = party->field_70.x;
            party->field_70.z = party->field_70.x;
            break;
        case 10:
            PF32(party, 0x64) += PF32(party, 0x124);
            break;
        case 100:
            switch ((s8)party->field_2E) {
                case 0:
                case 1:
                    partyChgPaper(party, DK_FB(0x1d8, str_802CCF08));
                    P32(party, 0x24) = 0x12;
                    P8(party, 0x38) = 0x65;
                    break;
                case 4:
                    partyChgPaper(party, DK_FB(0x1d8, str_802CCF08));
                    P32(party, 0x24) = 0x12;
                    P8(party, 0x38) = 0x65;
                    break;
            }
            break;
        case 101:
            switch ((s8)party->field_2E) {
                case 0:
                case 1:
                    party->position.z += -0.2f;
                    break;
            }
            if (--PS32(party, 0x24) <= 0) {
                P8(party, 0x38) = 0x66;
            }
            break;
        case 102:
            party->field_70 = DK_FV(0x12c, vec3_802cce5c);
            *partyVecAt(party, 0x64) = DK_FV(0x138, vec3_802cce68);
            *partyVecAt(party, 0x88) = DK_FV(0x144, vec3_802cce74);
            *partyVecAt(party, 0x7C) = DK_FV(0x150, vec3_802cce80);
            party->field_110 = PF32(player, 0x1AC);
            party->field_10C = party->field_110;
            party->dir = player->directionView;
            party->field_FC = player->directionView;
            partyPaperOff(party);
            animPoseSetMaterialLightFlagOn(party->paperAnimGroup, 1);
            party->flags2 &= ~8;
            party->flags2 &= ~0x10;
            party->flags &= ~0x4000000;
            party->flags &= ~0x2000000;
            if (party) {
                party->flags2 &= ~0x100000;
            }
            L_partyVivianTailStart(party);
            party->field_B8 = party->position;
            partyChgRunMode(party, 1);
            count = 0;
            if (partyGetPtr(marioGetPartyId())) {
                count = 1;
            }
            if (partyGetPtr(marioGetExtraPartyId())) {
                count += 1;
            }
            if ((s8)party->field_2E == 4) {
                if (count == 1) {
                    party->position.z += -4.0f;
                } else if (!(party->flags & 8)) {
                    party->position.z += -4.0f;
                    party->position.x += 1.5f;
                } else {
                    party->position.z += -3.0f;
                    party->position.x += -1.5f;
                }
            }
            if ((s8)party->field_2E == 1) {
                if (count == 1) {
                    party->position.z += -4.0f;
                } else if (!(party->flags & 8)) {
                    party->position.z += -4.0f;
                    party->position.x += 1.5f;
                } else {
                    party->position.z += -3.0f;
                    party->position.x += -1.5f;
                }
            }
            break;
    }
    return 0;
}

void partyDokanEnd(s32 index, s32 unk) {
    PartyEntry* party = partyGetPtr(index);

    if (party && party->field_34 == 0xa) {
        P8(party, 0x38) = 0x64;
        P8(party, 0x2E) = unk;
        party->flags2 &= ~8;
        party->flags2 &= ~0x10;
        party->flags2 &= ~0xC0;
        party->flags &= ~0x04000000;
        party->flags &= ~0x02000000;
        if (party) {
            party->flags2 &= ~0x100000;
        }
        party->field_B8 = party->position;
        L_partyVivianTailStart(party);
    }
}

void allPartyRidePlane(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (party) {
        bomhei_use_cancel();
        party->flags |= 0x10000;
        partyChgRunMode(party, 0xb);
    }
    party = partyPtrTbl[1];
    if (party) {
        party->flags |= 0x10000;
        partyChgRunMode(party, 0xb);
    }
}

void allPartyRideShip(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (party) {
        bomhei_use_cancel();
        party->flags |= 0x20000;
        partyChgRunMode(party, 0xb);
    }
    party = partyPtrTbl[1];
    if (party) {
        party->flags |= 0x20000;
        partyChgRunMode(party, 0xb);
    }
}

void getRidePos(PartyEntry* party, Vec* pos) {
    MarioWork* mario = party->playerPtr;
    f32 angle;
    f32 sn;
    f32 cs;
    u16 id = P16(mario, 0x2E);

    if (id == 0x1A) {
        angle = 3.1415927f * toMovedir(PF32(mario, 0x1AC)) / 180.0f;
        if (yoshiGetStatus() != 2) {
            pos->x = PF32(mario, 0x8C) - 10.0f * (f32)sin(angle);
            pos->z = PF32(mario, 0x94) - 10.0f * (f32)cos(angle);
            pos->y = PF32(mario, 0x90) + 20.0f;
        } else {
            pos->x = PF32(mario, 0x8C) - 20.0f * (f32)sin(angle);
            pos->z = PF32(mario, 0x94) - 20.0f * (f32)cos(angle);
            pos->y = PF32(mario, 0x90) + 16.0f;
        }
        if (party->currentSlotId != 0) {
            PartyEntry* other = partyGetPtr(marioGetPartyId());
            f32 a = toMovedir2(PF32(mario, 0x1AC), 10.0f);

            sincosf(&sn, &cs, a);
            if (yoshiGetStatus() != 2) {
                pos->x = other->position.x - 12.0f * sn;
                pos->z = other->position.z - 12.0f * cs;
                pos->y = other->position.y + 20.0f;
            } else {
                pos->x = other->position.x - 22.0f * sn;
                pos->z = other->position.z - 22.0f * cs;
                pos->y = other->position.y + 16.0f;
            }
        }
    } else if (id == 0x18) {
        pos->x = PF32(mario, 0x8C);
        pos->z = PF32(mario, 0x94);
        pos->y = PF32(mario, 0x90) + 40.0f;
    } else if (id == 0x19) {
        f32 a;

        if (N_marioShipGetDir() != 0) {
            a = 0.0f;
        } else {
            a = 180.0f;
        }
        a = toMovedirSimple(a);
        sincosf(&sn, &cs, a);
        pos->x = PF32(mario, 0x8C) + 22.0f * sn;
        pos->z = PF32(mario, 0x94) + 22.0f * cs;
        pos->y = PF32(mario, 0x90);
    } else {
        pos->x = PF32(mario, 0x8C);
        pos->z = PF32(mario, 0x94);
        pos->y = PF32(mario, 0x90) + 20.0f;
    }
}

// dont_inline keeps this a real bl from partyHelloMain, as in retail.
#pragma dont_inline on
void allPartyForceRideOn(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (party) {
        N_partyForceChgRunMode(party, 0xb, 0xb);
        party->flags2 |= 1;
        partyShadowOff(party);
    }
    party = partyGetPtr(marioGetExtraPartyId());
    if (party) {
        N_partyForceChgRunMode(party, 0xb, 0xb);
        party->flags2 |= 1;
        partyShadowOff(party);
    }
}
#pragma dont_inline reset

// The header leaves 0x64..0x70 and 0x7C..0xB8 as unnamed padding; these Vecs live there.
static inline Vec* partyVecAt(PartyEntry* party, u32 off) {
    u8* base = (u8*)party;
    return (Vec*)(base + off);
}


void L_allPartyRideOn(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (party) {
        partyChgRunMode(party, 0xb);
    }
    party = partyGetPtr(marioGetExtraPartyId());
    if (party) {
        partyChgRunMode(party, 0xb);
    }
}

void allPartyRideOff(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (party && party->field_34 == 0xb) {
        P8(party, 0x38) = 0x64;
    }
    party = partyGetPtr(marioGetExtraPartyId());
    if (party && party->field_34 == 0xb) {
        P8(party, 0x38) = 0x64;
    }
}

void allPartyRideOff2(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (party && party->field_34 == 0xb) {
        P8(party, 0x38) = 0x65;
    }
    party = partyGetPtr(marioGetExtraPartyId());
    if (party && party->field_34 == 0xb) {
        P8(party, 0x38) = 0x65;
    }
}

s32 partyRideChk(PartyEntry* party) {
    s32 result = 0;

    if (party == NULL) {
        return 0;
    }
    if (party->field_34 == 11) {
        result = 1;
        if (P8(party, 0x38) >= 2) {
            result = 2;
        }
    }
    return result;
}

// Case 0's pose selection: index 4, and the dead `partyGetHp` call the original still makes.
static inline char* L_ridePose0(PartyEntry* party) {
    char* pose;

    if (party->currentMemberId < 8) {
        partyGetHp(party->currentMemberId);
    }
    if (!(party->flags2 & 0x8000)) {
        char** row = partyPoseTbl[party->currentMemberId];
        pose = row[4];
    } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
        char** row = partyDotPoseTbl[party->currentMemberId];
        pose = row[4];
    } else {
        char** row = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)];
        pose = row[4];
    }
    if (!party->field_18 || strcmp(party->field_18, pose) != 0) {
        party->field_18 = pose;
        party->field_20 = 0;
        party->field_8 |= 0x2000;
    }
    return pose;
}
static inline char* L_ridePose1(PartyEntry* party) {
    s32 lowHp;
    s32 index = 1;
    char* pose;
    char* prev;

    if ((party->flags2 & 2) != 0) {
        return party->field_18;
    }
    lowHp = 0;
    if (party->currentMemberId < 8 && partyGetHp(party->currentMemberId) <= 5) {
        lowHp = TRUE;
    }
    if (lowHp == TRUE) {
        index = 12;
    }
    if (!(party->flags2 & 0x8000)) {
        char** row = partyPoseTbl[party->currentMemberId];
        pose = row[index];
    } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
        char** row = partyDotPoseTbl[party->currentMemberId];
        pose = row[index];
    } else {
        char** row = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)];
        pose = row[index];
    }
    prev = party->field_18;
    if (prev && !strcmp(prev, pose)) {
        return pose;
    }
    party->field_18 = pose;
    party->field_20 = 0;
    party->field_8 |= 0x2000;
    return pose;
}
static inline char* L_ridePose1b(PartyEntry* party, s32 index) {
    s32 lowHp;
    char* pose;
    char* prev;

    if ((party->flags2 & 2) != 0) {
        return party->field_18;
    }
    lowHp = 0;
    if (party->currentMemberId < 8 && partyGetHp(party->currentMemberId) <= 5) {
        lowHp = TRUE;
    }
    if (lowHp == TRUE) {
        index = 12;
    }
    if (!(party->flags2 & 0x8000)) {
        char** row = partyPoseTbl[party->currentMemberId];
        pose = row[index];
    } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
        char** row = partyDotPoseTbl[party->currentMemberId];
        pose = row[index];
    } else {
        char** row = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)];
        pose = row[index];
    }
    prev = party->field_18;
    if (prev && !strcmp(prev, pose)) {
        return pose;
    }
    party->field_18 = pose;
    party->field_20 = 0;
    party->field_8 |= 0x2000;
    return pose;
}
// NONMATCHING (97.13%): calls, immediates and code shape all match. What still differs is the
// int->float magic double (the target reads it as `lfd fN, 0x1d0(r31)`; our object pools an
// anonymous `@NNN` copy with `lis`/`addi`, a documented project-wide limitation whose extra
// GPR shifts every i2f site's allocation), some FPR colouring rotations (the case-11/111 pose
// value in r28 instead of r30), and the switch's left subtree (our `case 99:` is an explicit
// leaf, the target's is the 100/101 range bound).
// Raw offsets used: 0x24 frame counter, 0x38 ride mode, 0x80 scale, 0x94 ride target Vec, 0x124
// rotation speed, 0x128 distance, 0x174 jump accel, 0xF0 vertical assist. Player fields
// 0x1AC/0x1B8 are read via the file's PF32 macro.
// NONMATCHING (97.13%): the int->float literal pool and a few FPR colours still differ.
void partyRideMain(PartyEntry* party) {
    MarioWork* player = party->playerPtr;
    const u8* base = (const u8*)vec3_802ccd30;
    Vec ridePos;
    f32 dy;
    f32 tmp;
    f32 t;
    f32 scale;
    f32 px;
    f32 pz;

    switch (P8(party, 0x38)) {
        case 0:
            getRidePos(party, &ridePos);
            *(Vec*)(party->field_7C + (0x94 - 0x7C)) = ridePos;
            PF32(party, 0x128) =
                distABf(party->position.x, party->position.z, PF32(party, 0x94), PF32(party, 0x9C));
            party->dir =
                angleABf(party->position.x, party->position.z, PF32(party, 0x94), PF32(party, 0x9C));
            PS32(party, 0x24) = 30;
            party->speed = PF32(party, 0x128) / (f32)PS32(party, 0x24);
            dy = PF32(party, 0x98) - party->position.y;
            party->jumpPara3 = float_0p4_8041b174;
            party->jumpPara1 =
                (float_0p5_8041b170 *
                     ((f32)PS32(party, 0x24) * (party->jumpPara3 * (f32)PS32(party, 0x24))) +
                 dy) /
                (f32)PS32(party, 0x24);
            PF32(party, 0x124) = float_1_8041b0e8;
            party->flags2 |= 0x10;
            if (!(party->flags2 & 2)) {
                L_ridePose0(party);
            }
            P8(party, 0x38) = 1;
            break;
        case 1:
            party->jumpPara1 = party->jumpPara1 - party->jumpPara3;
            party->position.y = party->position.y + party->jumpPara1;
            movePos(&party->position.x, &party->position.z, party->speed, party->dir);
            party->field_10C = revise360(party->field_10C + PF32(party, 0x124));
            PF32(party, 0x124) = PF32(party, 0x124) + float_2_8041b0e4;
            {
                f32 cap = 30.0f;
                if (PF32(party, 0x124) >= cap) {
                    PF32(party, 0x124) = cap;
                }
            }
            scale = (f32)PS32(party, 0x24);
            if (scale > float_10_8041b124) {
                scale = float_10_8041b124;
            }
            party->field_70.x = scale / float_10_8041b124;
            party->field_70.y = party->field_70.x;
            party->field_70.z = party->field_70.x;
            if ((PS32(party, 0x24) = PS32(party, 0x24) - 1) < 0) {
                P8(party, 0x38) = 2;
            }
            break;
        case 2:
            L_ridePose1(party);
            party->jumpPara1 = float_0_8041b100;
            party->position.y = PF32(party, 0x98);
            party->field_70 = PR_FV(0x15c, vec3_802cce8c);
            party->flags2 |= 1;
            if (party != NULL) {
                party->flags2 |= 0x100000;
            }
            P8(party, 0x38) = 10;
            break;
        case 10:
            party->position = player->position;
            party->field_110 = PF32(player, 0x1AC);
            party->field_FC = toMovedir(party->field_110);
            break;
        case 11:
            L_ridePose1b(party, 1);
            party->flags2 |= 1;
            if (party != NULL) {
                party->flags2 |= 0x100000;
            }
            getRidePos(party, &ridePos);
            party->field_70 = PR_FV(0x168, vec3_802cce98);
            party->position = ridePos;
            party->jumpPara1 = float_0_8041b100;
            party->field_110 = PF32(party->playerPtr, 0x1AC);
            party->field_FC = toMovedir(party->field_110);
            break;
        case 20:
            P8(party, 0x38) = 21;
            PS32(party, 0x24) = 16;
            party->jumpPara1 = float_16_8041b17c;
            PF32(party, 0x174) = float_0p5_8041b170;
            /* fallthrough */
        case 21:
            party->position.y = party->position.y + party->jumpPara1;
            party->jumpPara1 = party->jumpPara1 + PF32(party, 0x174);
            if ((PS32(party, 0x24) = PS32(party, 0x24) - 1) <= 0) {
                P8(party, 0x38) = 22;
                party->flags |= 0x02000000;
            }
            break;
        case 200:
            party->flags &= ~0x70000;
            PF32(party, 0x80) = float_0_8041b100;
            party->field_70.x = float_1_8041b0e8;
            party->field_70.y = party->field_70.x;
            party->field_70.z = party->field_70.x;
            if (party != NULL) {
                party->flags2 &= ~0x100000;
            }
            partyChgRunMode(party, 1);
            break;
        case 100:
        case 101:
            party->flags2 &= ~1;
            px = player->position.x;
            pz = player->position.z;
            party->position.x = px;
            party->position.y = PF32(player, 0x1B8) * float_0p5_8041b170 + player->position.y;
            party->position.z = pz;
            if (P8(party, 0x38) == 100) {
                partyGetAppearPos(party, (Vec*)(party->field_7C + (0x94 - 0x7C)));
            } else {
                partyGetAppearPos2(party, (Vec*)(party->field_7C + (0x94 - 0x7C)));
            }
            party->field_110 = PF32(player, 0x1AC);
            PS32(party, 0x24) = 16;
            PF32(party, 0x128) = distABf(px, pz, PF32(party, 0x94), PF32(party, 0x9C));
            party->dir = angleABf(px, pz, PF32(party, 0x94), PF32(party, 0x9C));
            party->speed = PF32(party, 0x128) / (f32)PS32(party, 0x24);
            dy = PF32(party, 0x98) - party->position.y;
            tmp = float_0p5_8041b170 * (f32)PS32(party, 0x24);
            if (__fabsf(dy) < float_1_8041b0e8) {
                dy = float_10_8041b124;
            } else if (dy >= float_0_8041b100) {
                dy = float_0p5_8041b170 * PF32(party, 0xF0) + dy;
            } else {
                dy = float_10_8041b124;
            }
            t = (dy + dy) / ((tmp * tmp) - tmp);
            party->jumpPara1 = t * tmp;
            party->jumpPara2 = -t;
            party->flags |= 0x10;
            party->flags2 &= ~1;
            if (party != NULL) {
                party->flags2 &= ~0x100000;
            }
            P8(party, 0x38) = 110;
            break;
        case 99:
            break;
        case 110:
            partyMoveNoHosei(party);
            if (party->jumpPara1 <= float_0_8041b100) {
                party->flags &= ~0x10;
                party->jumpPara2 = float_neg1p6_8041b180;
            }
            if (party->position.y < float_neg2000_8041b164) {
                PS32(party, 0x24) = 0;
                P8(party, 0x38) = 111;
                break;
            } else if (party->field_138 == 0) {
                Vec vec;
                t = (f32)(16 - PS32(party, 0x24)) * float_0p0625_8041b184;
                vec = PR_FV(0x174, vec3_802ccea4);
                vec.x = t;
                vec.y = t;
                vec.z = t;
                party->field_70 = vec;
                if ((PS32(party, 0x24) = PS32(party, 0x24) - 1) <= 0) {
                    PS32(party, 0x24) = 0;
                }
                break;
            }
            P8(party, 0x38) = 111;
            /* fallthrough */
        case 111:
        {
            Vec vec;
            t = (f32)(16 - PS32(party, 0x24)) * float_0p0625_8041b184;
            vec = PR_FV(0x180, vec3_802cceb0);
            vec.x = t;
            vec.y = t;
            vec.z = t;
            party->field_70 = vec;
            if ((PS32(party, 0x24) = PS32(party, 0x24) - 1) <= 0) {
                PS32(party, 0x24) = 0;
                party->field_110 = PF32(player, 0x1AC);
                party->field_10C = PF32(player, 0x1AC);
                party->field_FC = toMovedirSimple(party->field_10C);
                party->dir = party->field_FC;
                partyClearJumpPara(party);
                L_ridePose1b(party, 1);
                party->flags2 &= ~0x10;
                party->flags &= ~0x70000;
                PF32(party, 0x80) = float_0_8041b100;
                party->field_70.x = float_1_8041b0e8;
                party->field_70.y = party->field_70.x;
                party->field_70.z = party->field_70.x;
                party->flags2 &= ~1;
                if (party != NULL) {
                    party->flags2 &= ~0x100000;
                }
                partyChgRunMode(party, 1);
            }
            break;
        }
    }
    party->field_B8 = party->position;
}

// MUST_MATCH: the inliner does not fold partyChgPoseId's `index == 1` chain for the
// literal argument, so write the already-folded pose selection the target emits; the
// natural call is under #else.
void party_force_ride_yoshi(PartyEntry* party) {
    Vec pos;
#ifdef MUST_MATCH
    char* index;
    char* pose;
#else
    char* pose;
#endif

    N_partyForceChgRunMode(party, 0xc, 0xa);
    party->flags &= ~0x100;
    getRidePos(party, &pos);
    party->position = pos;
#ifdef MUST_MATCH
    index = (char*)1;
    if (!(party->flags2 & 2)) {
        s32 lowHp = 0;

        if (party->currentMemberId < 8 && partyGetHp(party->currentMemberId) <= 5) {
            lowHp = 1;
        }
        if (lowHp == 1) {
            index = (char*)12;
        }
        if (!(party->flags2 & 0x8000)) {
            pose = partyPoseTbl[party->currentMemberId][(s32)index];
        } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
            pose = partyDotPoseTbl[party->currentMemberId][(s32)index];
        } else {
            pose = yoshiDotPoseTbl[pouchGetPartyColor(PARTY_MEMBER_YOSHI)][(s32)index];
        }
        if (!(party->field_18 && !strcmp(party->field_18, pose))) {
            party->field_18 = pose;
            party->field_20 = 0;
            party->field_8 |= 0x2000;
        }
    }
#else
    partyChgPoseId(party, 1);
#endif
    PF32(party, 0x114) = 0.0f;
    partyShadowOff(party);
    PF32(party, 0x110) = PF32(party->playerPtr, 0x1AC);
    PF32(party, 0xFC) = toMovedir(PF32(party, 0x110));
}

void party_ride_yoshi_force_move(void) {
    PartyEntry* party = partyGetPtr(marioGetExtraPartyId());
    Vec pos;

    if (party) {
        getRidePos(party, &pos);
        party->position = pos;
        PF32(party, 0x110) = PF32(party->playerPtr, 0x1AC);
        PF32(party, 0xFC) = toMovedir(PF32(party, 0x110));
    }
}

// Two shapes below exist only to reproduce MWCC 2.6's register allocation:
//  * the constant-index pose reads use byte arithmetic over the flattened table
//    (`partyPoseTbl[id][4]` == `*(char**)((char*)partyPoseTbl + id*0x3c + 0x10)`); the natural
//    `[id][4]` form puts the base in r3 and the index in r0, while this puts them in r0/r3-r4.
//  * the final `vec3_802cced4` copy is qualified volatile on both sides so the scheduler emits the
//    original's `lwz r3,0; lwz r0,4; stw/stw; lwz r0,8; stw`; without it the first word goes to r0.
// The `player` local must also stay the last callee-saved declaration for r29/r30 to line up.
s32 partyRideYoshiMain(PartyEntry* party) {
    char* pose;
    s32 index;
    s32 lowHp;
    MarioWork* player = party->playerPtr;
    Vec pos;
    Vec appearPos;

    switch (P8(party, 0x38)) {
        case 0:
            party->jumpPara3 = 1.6f;
            getRidePos(party, &pos);
            *partyVecAt(party, 0x94) = pos;
            PF32(party, 0x128) = distABf(party->position.x, party->position.z, PF32(party, 0x94), PF32(party, 0x9C));
            party->dir = angleABf(party->position.x, party->position.z, PF32(party, 0x94), PF32(party, 0x9C));
            PS32(party, 0x24) = 15;
            party->speed = PF32(party, 0x128) / (f32)PS32(party, 0x24);
            party->jumpPara1 = (0.5f * (party->jumpPara3 * (f32)PS32(party, 0x24) * (f32)PS32(party, 0x24)) +
                                (PF32(party, 0x98) - party->position.y)) /
                               (f32)PS32(party, 0x24);
            if (!(party->flags2 & 2)) {
                /* the inlined partyChgPoseId(party, 4): its low-HP call is dead for index 4 */
                if (party->currentMemberId < 8) {
                    partyGetHp(party->currentMemberId);
                }
                if (!(party->flags2 & 0x8000)) {
                    pose = *(char**)((char*)partyPoseTbl + (s32)party->currentMemberId * 0x3c + 0x10);
                } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
                    pose = *(char**)((char*)partyDotPoseTbl + (s32)party->currentMemberId * 0x3c + 0x10);
                } else {
                    pose = *(char**)((char*)yoshiDotPoseTbl + pouchGetPartyColor(PARTNER_YOSHI) * 0x3c + 0x10);
                }
                if (!(party->field_18 && !strcmp(party->field_18, pose))) {
                    party->field_18 = pose;
                    party->field_20 = 0;
                    party->field_8 |= 0x2000;
                }
            }
            P8(party, 0x38) = 1;
            break;
        case 1:
            getRidePos(party, &pos);
            *partyVecAt(party, 0x94) = pos;
            PF32(party, 0x128) = distABf(party->position.x, party->position.z, PF32(party, 0x94), PF32(party, 0x9C));
            party->dir = angleABf(party->position.x, party->position.z, PF32(party, 0x94), PF32(party, 0x9C));
            party->speed = PF32(party, 0x128) / (f32)PS32(party, 0x24);
            party->jumpPara1 = party->jumpPara1 - party->jumpPara3;
            party->position.y = party->position.y + party->jumpPara1;
            movePos(&party->position.x, &party->position.z, party->speed, party->dir);
            if (--PS32(party, 0x24) <= 0) {
                P8(party, 0x38) = 2;
            }
            break;
        case 2:
        {
            index = 1;

            if (!(party->flags2 & 2)) {
                lowHp = 0;
                if (party->currentMemberId < 8 && partyGetHp(party->currentMemberId) <= 5) {
                    lowHp = 1;
                }
                if (lowHp == 1) {
                    index = 12;
                }
                if (!(party->flags2 & 0x8000)) {
                    pose = partyPoseTbl[party->currentMemberId][index];
                } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
                    pose = partyDotPoseTbl[party->currentMemberId][index];
                } else {
                    pose = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][index];
                }
                if (!(party->field_18 && !strcmp(party->field_18, pose))) {
                    party->field_18 = pose;
                    party->field_20 = 0;
                    party->field_8 |= 0x2000;
                }
            }
            party->jumpPara1 = 0.0f;
            party->position.y = PF32(party, 0x98);
            P8(party, 0x38) = 10;
            partyShadowOff(party);
        }
            break;
        case 10:
            getRidePos(party, &pos);
            party->position = pos;
            party->field_110 = PF32(player, 0x1AC);
            party->field_FC = toMovedir(party->field_110);
            if (P16(player, 0x2E) != 0x1A) {
                partyGetAppearPos(party, &appearPos);
                party->position.x = appearPos.x;
                party->position.z = appearPos.z;
                partyChgRunMode(party, 2);
                if (party) {
                    party->flags2 &= ~0x100000;
                }
                *(volatile Vec*)partyVecAt(party, 0x7C) = *(volatile const Vec*)&vec3_802cced4;
            }
            break;
    }
    return 0;
}

#pragma opt_lifetimes off // without it the case-22 pose is live-range-split into r31 (mario's free register); off keeps it in r29 like the original
void party_damage_return(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    char* pose;
    f32 partyX;
    f32 partyY;
    f32 partyZ;
    f32 marioX;
    f32 marioY;
    f32 marioZ;

    if (party->field_8 & 8) {
        party->field_8 &= ~8;
        party->flags &= ~0x100;
        if (!(party->field_8 & 0x1000000)) {
            P8(party, 0x38) = 10;
        } else {
            P8(party, 0x38) = 0;
            PS32(party, 0x24) = 0x64;
        }
    }
    switch (P8(party, 0x38)) {
        case 0:
            if (--PS32(party, 0x24) <= 0) {
                P8(party, 0x38) = 10;
            } else {
                break;
            }
            // fall through
        case 10:
            if (!(party->flags2 & 2)) {
                if (party->currentMemberId < 8) {
                    partyGetHp(party->currentMemberId);
                }
                if (!(party->flags2 & 0x8000)) {
                    pose = *(char**)((u8*)partyPoseTbl + party->currentMemberId * 0x3c + 32);
                } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
                    pose = *(char**)((u8*)partyDotPoseTbl + party->currentMemberId * 0x3c + 32);
                } else {
                    pose = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][8];
                }
                if (!(party->field_18 && !strcmp(party->field_18, pose))) {
                    party->field_18 = pose;
                    party->field_20 = 0;
                    party->field_8 |= 0x2000;
                }
            }
            PS32(party, 0x24) = sysMsec2Frame(0xc8);
            P8(party, 0x38) = 11;
            /* fall through */
        case 11:
            if (--PS32(party, 0x24) <= 0) {
                party->jumpPara1 = 15.0f;
                party->jumpPara2 = 0.0f;
                P8(party, 0x38) = 12;
            } else {
                break;
            }
            // fall through
        case 12: {
            f32 y;

            party->position.y += party->jumpPara1;
            marioGetScreenPos(party->position, &partyX, &partyY, &partyZ);
            marioGetScreenPos(mario->position, &marioX, &marioY, &marioZ);
            y = marioY;
            y = (marioY < -100.0f) ? marioY : -100.0f;
            if (partyY <= y) {
                PS32(party, 0x24) = 0x20;
                P8(party, 0x38) = 0x14;
            }
            break;
        }
        case 20: {
            marioGetScreenPos(party->position, &partyX, &partyY, &partyZ);
            if (partyY <= -100.0f) {
                party->position.y += 1.0f;
            }
            if (mario->currMotionId == MARIO_MOTION_DAMAGE) {
                if (!marioWaitDamageReturnFall()) {
                    break;
                }
            } else if (mario->currMotionId == MARIO_MOTION_DAMAGE_TOGE) {
                if (!marioWaitDamageTogeReturnFall()) {
                    break;
                }
            }
            P8(party, 0x38) = 0x15;
            // fall through
        }
        case 21: {
            Vec pos;

            if (--PS32(party, 0x24) <= 0) {
                partyGetAppearPos(party, &pos);
                party->position.x = pos.x;
                party->position.z = pos.z;
                party->jumpPara1 = 0.0f;
                party->jumpPara2 = -1.0f;
                party->speed = 0.0f;
                party->flags |= 0x20;
                party->flags |= 0x2000000;
                P8(party, 0x38) = 0x16;
            }
            break;
        }
        case 22: {
            marioGetScreenPos(party->position, &partyX, &partyY, &partyZ);
            if (partyY > 240.0f) {
                party->flags &= ~0x2000000;
            }
            partyMove(party);
            if (party->jumpPara1 <= -6.0f) {
                party->jumpPara2 = 0.0f;
                party->jumpPara1 = -6.0f;
            }
            if (party->position.y < -2000.0f) {
                party->position = mario->position;
                party->position.y += 200.0f;
                party->flags &= ~0x70;
            }
            if (party->field_138 != NULL) {
                if ((hitGetAttr((HitEntry*)party->field_138) >> 9 & 1) == 0) {
                    if (!(party->flags2 & 2)) {
                        if (party->currentMemberId < 8) {
                            partyGetHp(party->currentMemberId);
                        }
                        if (!(party->flags2 & 0x8000)) {
                            pose = *(char**)((u8*)partyPoseTbl + party->currentMemberId * 0x3c + 36);
                        } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
                            pose = *(char**)((u8*)partyDotPoseTbl + party->currentMemberId * 0x3c + 36);
                        } else {
                            pose = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][9];
                        }
                        if (!(party->field_18 && !strcmp(party->field_18, pose))) {
                            party->field_18 = pose;
                            party->field_20 = 0;
                            party->field_8 |= 0x2000;
                        }
                    }
                    P8(party, 0x38) = 0x17;
                    PS32(party, 0x24) = 0x1e;
                }
            }
            break;
        }
        case 23:
            if (--PS32(party, 0x24) <= 0) {
                partyChgRunMode(party, 2);
                P8(party, 0x3b) = 0;
                party->flags2 &= ~0x10;
            }
            break;
        default:
            break;
    }
    if (vivianGetStatus() != 2) {
        vivianShadow(party);
    }
}
#pragma opt_lifetimes reset

void partyInitCamId(PartyEntry* party) {
    party->camId = marioGetCamId();
}

/*
//.data
typedef struct PartyData {
        char* name; //0, TODO: modelname? animname?
        void* init_func; //1
        void* move_func; //2
        void* use_func; //3
        void* post_func; //4
        void* bye_func; //5
        void* unk_6_func; //6
        void* unk_7_func; //7
        void* unk_8_func; //8
        void* exit_func; //9
} PartyData;

PartyData partyDataTbl[] = { //change to [20]
        {"c_pkr", kuribo_init, partyMoveWalk, kuribo_use, kuribo_use_post, kuribo_bye, NULL, NULL, NULL, NULL},
        {"c_pnk", nokonoko_init, nokonoko_move, nokonoko_use, nokonoko_use_post, nokonoko_bye, NULL, NULL, NULL, NULL},
        {"c_bomt_n", bomhei_init, partyMoveWalk, bomhei_use, bomhei_use_post, bomhei_bye, NULL, NULL, NULL,
bomhei_exit},
        {"c_babyyoshi", yoshi_init, partyMoveWalk, yoshi_use, NULL, yoshi_bye, NULL, NULL, NULL, yoshi_exit},
        {"c_windy", cloud_init, cloud_move, cloud_use, NULL, kuribo_bye, NULL, NULL, NULL, cloud_exit},
        {"c_vivian", vivian_init, vivian_move, vivian_use, NULL, kuribo_bye, NULL, NULL, NULL, NULL},
        {"c_tyutyu", chuchu_init, chuchu_move, chuchu_use, NULL, kuribo_bye, NULL, NULL, NULL, chuchu_exit},
        {"c_babyyoshi", kuribo_init, partyMoveWalk, kuribo_use, NULL, kuribo_bye, NULL, NULL, NULL, NULL},
        {"c_marco", kuribo_init, partyMoveWalk, kuribo_use, NULL, kuribo_bye, NULL, NULL, NULL, NULL},
        {"c_pguide", kuribo_init, partyMoveWalk, kuribo_use, NULL, kuribo_bye, NULL, NULL, NULL, NULL},
        {"c_kuri_h", kuribo_init, partyMoveWalk, kuribo_use, NULL, kuribo_bye, NULL, NULL, NULL, NULL},
        {"c_monban", kuribo_init, partyMoveWalk, kuribo_use, NULL, kuribo_bye, NULL, NULL, NULL, NULL},
        {"c_pkr", kuribo_init, partyMoveWalk, kuribo_use, NULL, kuribo_bye, NULL, NULL, NULL, NULL},
        {"c_pnk", nokonoko_init, nokonoko_move, nokonoko_use, NULL, nokonoko_bye, NULL, NULL, NULL, NULL},
        {"c_bomt_n", bomhei_init, partyMoveWalk, bomhei_use, bomhei_use_post, bomhei_bye, NULL, NULL, NULL, NULL},
        {"c_babyyoshi", yoshi_init, partyMoveWalk, yoshi_use, NULL, yoshi_bye, NULL, NULL, NULL, NULL},
        {"c_windy", cloud_init, cloud_move, cloud_use, NULL, kuribo_bye, NULL, NULL, NULL, cloud_exit},
        {"c_vivian", vivian_init, vivian_move, vivian_use, NULL, kuribo_bye, NULL, NULL, NULL, NULL},
        {"c_tyutyu", chuchu_init, partyMoveWalk, chuchu_use, NULL, kuribo_bye, NULL, NULL, NULL, NULL}
        {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
};

char* partyPoseTbl[20][15] = {
        {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
        {"PKR_Z_1", "PKR_S_1", "PKR_W_1", "PKR_R_1", "PKR_J_1A", "PKR_J_1B", "PKR_N_1", "PKR_T_1", "PKR_D_1", "PKR_D_4",
"PKR_S_1", "PKR_S_1", "PKR_D_2", "PKR_W_2", "PKR_R_2"},
        {"PNK_Z_1", "PNK_S_1", "PNK_W_1", "PNK_R_1", "PNK_J_1A", "PNK_J_1B", "PNK_N_1", "PNK_T_1", "PNK_D_1", "PNK_D_4",
"PNK_S_1", "PNK_S_1", "PNK_D_2", "PNK_W_2", "PNK_R_2"},
        {"Z_1", "S_1", "W_1", "R_1", "J_1A", "J_1B", "N_1", "T_1", "D_1", "F_1", "S_1", "S_1", "D_2", "W_3", "R_3"},
        {"PYS_Z_1", "PYS_S_1", "PYS_W_1", "PYS_R_1", "PYS_J_1A", "PYS_J_1B", "PYS_N_1", "PYS_T_1", "PYS_D_1", "PYS_D_4",
"PYS_S_1", "PYS_S_1", "PYS_D_2", "PYS_W_3", "PYS_R_3"},
        {"PWD_Z_1", "PWD_S_1", "PWD_W_1", "PWD_R_1", "PWD_S_1", "PWD_S_1", "PWD_N_1", "PWD_T_1", "PWD_D_1", "PWD_F_1",
"PWD_S_1", "PWD_S_1", "PWD_D_2", "PWD_W_2", "PWD_R_2"},
        {"PTR_Z_1", "PTR_S_1", "PTR_W_1", "PTR_R_1", "PTR_S_1", "PTR_S_1", "PTR_N_1", "PTR_T_1", "PTR_D_1", "PTR_F_1",
"PTR_Z_1", "PTR_Z_1", "PTR_D_2", "PTR_W_2", "PTR_R_2"},
        {"PCH_Z_1", "PCH_S_1", "PCH_W_1", "PCH_R_1", "PCH_J_1A", "PCH_J_1B", "PCH_N_1", "PCH_T_1", "PCH_D_1", "PCH_F_1",
"PCH_S_1", "PCH_S_1", "PCH_D_2", "PCH_R_2", "PCH_W_2"},
        {"PYS_E_1", "PYS_E_2", "PYS_E_3", "PYS_E_3", "PYS_E_2", "PYS_E_2", "PYS_E_2", "PYS_E_2", "PYS_E_2", "PYS_E_2",
"PYS_E_2", "PYS_E_2", "PYS_E_2", "PYS_E_3", "PYS_E_3"},
        {"Z_1", "S_1", "W_1", "R_1", "J_1A", "J_1B", "S_1", "T_1", "D_1", "D_1", "Z_1", "Z_1", "S_1", "W_1", "R_1"},
        {"Z_1", "S_1", "W_1", "R_1", "J_1A", "J_1B", "S_1", "T_1", "Z_1", "Z_1", "S_1", "Z_1", "Z_1", "W_1", "R_1"},
        {"Z_1", "S_1", "W_1", "R_1", "W_1", "W_1", "S_1", "T_1", "Z_1", "Z_1", "S_1", "Z_1", "Z_1", "W_1", "R_1"},
        {"Z_1", "S_1", "W_1", "R_1", "S_1", "S_1", "S_1", "T_1", "S_1", "S_1", "S_1", "S_1", "S_1", "W_1", "R_1"},
        {"PKR_Z_1", "PKR_S_1", "PKR_W_1", "PKR_R_1", "PKR_J_1A", "PKR_J_1B", "PKR_N_1", "PKR_T_1", "PKR_D_1", "PKR_D_4",
"PKR_S_1", "PKR_S_1", "PKR_D_2", "PKR_W_2", "PKR_R_2"},
        {"PNK_Z_1", "PNK_S_1", "PNK_W_1", "PNK_R_1", "PNK_J_1A", "PNK_J_1B", "PNK_N_1", "PNK_T_1", "PNK_D_1", "PNK_D_4",
"PNK_S_1", "PNK_S_1", "PNK_D_2", "PNK_W_2", "PNK_R_2"},
        {"Z_1", "S_1", "W_1", "R_1", "J_1A", "J_1B", "N_1", "T_1", "D_1", "F_1", "S_1", "S_1", "D_2", "W_3", "R_3"},
        {"PYS_Z_1", "PYS_S_1", "PYS_W_1", "PYS_R_1", "PYS_J_1A", "PYS_J_1B", "PYS_N_1", "PYS_T_1", "PYS_D_1", "PYS_D_4",
"PYS_S_1", "PYS_S_1", "PYS_D_2", "PYS_W_2", "PYS_R_2"},
        {"PWD_Z_1", "PWD_S_1", "PWD_W_1", "PWD_R_1", "PWD_S_1", "PWD_S_1", "PWD_N_1", "PWD_T_1", "PWD_D_1", "PWD_F_1",
"PWD_S_1", "PWD_S_1", "PWD_D_2", "PWD_W_2", "PWD_R_2"},
        {"PTR_Z_1", "PTR_S_1", "PTR_W_1", "PTR_R_1", "PTR_S_1", "PTR_S_1", "PTR_N_1", "PTR_T_1", "PTR_D_1", "PTR_F_1",
"PTR_Z_1", "PTR_Z_1", "PTR_D_2", "PTR_W_2", "PTR_R_2"},
        {"PCH_Z_1", "PCH_S_1", "PCH_W_1", "PCH_R_1", "PCH_J_1A", "PCH_J_1B", "PCH_N_1", "PCH_T_1", "PCH_D_1", "PCH_F_1",
"PCH_S_1", "PCH_S_1", "PCH_D_2", "PCH_R_2", "PCH_W_2"}
};

char* partyDotPoseTbl[8][15] = {
        {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
        {"Z_2", "Z_2", "W_2", "R_2", "Z_2", "Z_2", "Z_2", "Z_2", "Z_2", "Z_2", "Z_2", "Z_2", "Z_2", "W_2", "R_2"},
        {"Z_3", "Z_3", "W_3", "R_3", "Z_3", "Z_3", "Z_3", "Z_3", "Z_3", "Z_3", "Z_3", "Z_3", "Z_3", "W_3", "R_3"},
        {"Z_13", "Z_13", "W_13", "R_13", "Z_13", "Z_13", "Z_13", "Z_13", "Z_13", "Z_13", "Z_13", "Z_13", "Z_13", "W_13",
"R_13"},
        {"Z_5", "Z_5", "W_5", "R_5", "Z_5", "Z_5", "Z_5", "Z_5", "Z_5", "Z_5", "Z_5", "Z_5", "Z_5", "W_5", "R_5"},
        {"Z_4", "Z_4", "W_4", "R_4", "Z_4", "Z_4", "Z_4", "Z_4", "Z_4", "Z_4", "Z_4", "Z_4", "Z_4", "W_4", "R_4"},
        {"Z_12", "Z_12", "W_12", "R_12", "Z_12", "Z_12", "Z_12", "Z_12", "Z_12", "Z_12", "Z_12", "Z_12", "Z_12", "W_12",
"R_12"},
        {"Z_14", "Z_14", "W_14", "R_14", "Z_14", "Z_14", "Z_14", "Z_14", "Z_14", "Z_14", "Z_14", "Z_14", "Z_14", "W_14",
"R_14"}
};

char* yoshiDotPoseTbl[7][15] = {
        {"Z_5", "Z_5", "W_5", "R_5", "Z_5", "Z_5", "Z_5", "Z_5", "Z_5", "Z_5", "Z_5", "Z_5", "Z_5", "W_5", "R_5"},
        {"Z_6", "Z_6", "W_6", "R_6", "Z_6", "Z_6", "Z_6", "Z_6", "Z_6", "Z_6", "Z_6", "Z_6", "Z_6", "W_6", "R_6"},
        {"Z_7", "Z_7", "W_7", "R_7", "Z_7", "Z_7", "Z_7", "Z_7", "Z_7", "Z_7", "Z_7", "Z_7", "Z_7", "W_7", "R_7"},
        {"Z_8", "Z_8", "W_8", "R_8", "Z_8", "Z_8", "Z_8", "Z_8", "Z_8", "Z_8", "Z_8", "Z_8", "Z_8", "W_8", "R_8"},
        {"Z_9", "Z_9", "W_9", "R_9", "Z_9", "Z_9", "Z_9", "Z_9", "Z_9", "Z_9", "Z_9", "Z_9", "Z_9", "W_9", "R_9"},
        {"Z_10", "Z_10", "W_10", "R_10", "Z_10", "Z_10", "Z_10", "Z_10", "Z_10", "Z_10", "Z_10", "Z_10", "Z_10", "W_10",
"R_10"},
        {"Z_11", "Z_11", "W_11", "R_11", "Z_11", "Z_11", "Z_11", "Z_11", "Z_11", "Z_11", "Z_11", "Z_11", "Z_11", "W_11",
"R_11"}
};

//.sbss
PartyData* party_dp;
PartyEntry* partyPtrTbl[2];

//.sdata
char* yoshigroup[7] = {
        "c_babyyoshi",
        "c_babyyoshi2",
        "c_babyyoshi3",
        "c_babyyoshi4",
        "c_babyyoshi5",
        "c_babyyoshi6",
        "c_babyyoshi7"
};

PartyEntry* partyGetPtr(s32 id) {
        if (id >= 0) {
                return partyPtrTbl[id];
        }
        return NULL;
}

PartyEntry* anotherPartyGetPtr(s32 id) {
        if (id < 0) {
                return NULL;
        }
        else {
                return partyPtrTbl[(id + 1) & 1];
        }
}

BOOL partyPaperOn(PartyEntry* entry, char* anim) {
        BOOL v5, v6;

        if (entry->field_0x14 >= 0) {
                animPaperPoseRelease(entry->field_0x14);
                entry->field_0x14 = -1;
        }
        entry->flags2 |= 0x4000000;
        entry->field_0x14 = animPaperPoseEntry(anim, 2);
        v5 = FALSE;
        if (!strcmp(anim, "p_roll") ||
                !strcmp(anim, "p_plane") ||
                !strcmp(anim, "p_dokan_x") ||
                !strcmp(anim, "p_dokan_y")) {
                v5 = TRUE;
        }
        v6 = !v5;
        if (!strcmp(anim, "p_slit")) {
                v6 = entry->currentMemberId == PARTY_FLURRIE;
        }
        animPoseSetPaperAnimGroup(entry->field_0xC, anim, v6);
        return TRUE;
}







s32 partyEntry2(s32 memberId) {
        PartyEntry* entry;
        MarioWork* player;
        const char* animName;
        s32 slotId;

        player = marioGetPtr();
        if (partyPtrTbl[1]) {
                slotId = -1;
        }
        else {
                partyPtrTbl[1] = (PartyEntry*)__memAlloc(HEAP_DEFAULT, sizeof(PartyEntry));
                memset(partyPtrTbl[1], 0, sizeof(PartyEntry));
                slotId = 1;
        }
        if (slotId < 0) {
                return -1;
        }
        entry = partyPtrTbl[slotId];
        memset(entry, 0, sizeof(PartyEntry));
        entry->camId = marioGetCamId();
        entry->field_0xC = -1;
        entry->field_0x10 = -1;
        entry->field_0x14 = -1;
        entry->flags = 9;
        entry->currentSlotId = (s8)slotId; //TODO: re-type?
        entry->currentMemberId = (PartyMember)memberId;
        entry->flags |= 4;
        party_dp = &partyDataTbl[memberId];
        if (entry->currentMemberId == PARTY_YOSHI) {
                animName = yoshigroup[pouchGetPartyColor(PARTNER_YOSHI)];
                if ((entry->flags2 & 0x8000)) {
                        animName = str_802CCEEC;
                }
        } else if ((entry->flags2 & 0x8000)) {
                animName = str_802CCEEC;
        }
        else {
                animName = partyDataTbl[memberId].name;
        }
        if (entry->field_0xC >= 0) {
                animPoseRelease(entry->field_0xC);
        }
        entry->field_0xC = animPoseEntry(animName, 2);






        return -1;
}
*/

void partySetCamId(PartyEntry* party, s32 cameraId) {
    party->camId = cameraId;
}

// A named `want` cache orders the 0.0 compare first (MUST_MATCH; the plain read form scores 99.38%).
// Caching field_10C in `old` before the outer compare (nothing writes it in between) plus these two
// optimizer pragmas gives the original arithmetic and clamp FPR allocation (field_10C in f2, field_110
// in f3, and the clamp reloads in f1): with defaults the three `else if` clamps load the field and
// bound into f0/f1, and without `old` the common-subexpression pass raises the arithmetic to f3.
#pragma opt_common_subs off // the clamp/comparison FPRs land in f0/f1 instead of f1/f0 with CSE on
#pragma opt_lifetimes off // keep the arithmetic's field_10C in f2 and field_110 in f3
void partyMakeDispDir(PartyEntry* party) {
    f32 angle;
    f32 target;
    f32 old;
#ifdef MUST_MATCH
    f32 want;
#endif

    if (party->flags2 & 0x100) {
        return;
    }
    if (!(party->flags2 & 0x10)) {
        angle = revise360(party->field_FC - PF32(party, 0xF8));
        target = party->field_110;
        if (!(party->flags2 & 0x100)) {
            if (float_290_8041b128 == target || float_250_8041b12c == target) {
                target = float_180_8041b0fc;
                if (angle >= target) {
                    target = float_0_8041b100;
                }
            }
        }
        if (party->speed != float_0_8041b100) {
            if (angle >= float_190_8041b130 && angle <= float_350_8041b134) {
                target = float_0_8041b100;
            } else if (angle >= float_10_8041b124 && angle <= float_170_8041b138) {
                target = float_180_8041b0fc;
            }
        }
        party->field_110 = target;
        if (party->flags2 & 0x20) {
            party->field_10C = party->field_110;
        }
        if (party->field_10C == party->field_110) {
            return;
        }
    }
    old = party->field_10C;
    if (old <= float_180_8041b0fc) {
#ifdef MUST_MATCH
        if (float_0_8041b100 == (want = party->field_110)) {
#else
        if (float_0_8041b100 == party->field_110) {
#endif
            f32 t = (party->field_8 & 0x1000) ? float_20_8041b140 : float_neg40_8041b144;
            f32 s;

            s = float_0p125_8041b13c * (t - old);
            party->field_10C = old + s;
            if (!(party->field_8 & 0x1000)) {
                if (party->field_10C < float_neg30_8041b148) {
                    party->field_8 |= 0x1000;
                }
            } else if (party->field_10C >= float_0_8041b100) {
                party->field_10C = float_0_8041b100;
                party->field_8 &= ~0x1000;
            }
#ifdef MUST_MATCH
        } else if (float_180_8041b0fc == want) {
#else
        } else if (float_180_8041b0fc == party->field_110) {
#endif
            f32 t = (party->field_8 & 0x1000) ? float_170_8041b138 : float_220_8041b14c;
            f32 s;

            s = float_0p125_8041b13c * (t - old);
            party->field_10C = old + s;
            if (!(party->field_8 & 0x1000)) {
                if (party->field_10C > float_210_8041b150) {
                    party->field_8 |= 0x1000;
                }
            } else if (party->field_10C <= float_180_8041b0fc) {
                party->field_10C = float_180_8041b0fc;
                party->field_8 &= ~0x1000;
            }
        }
    } else if (old > float_180_8041b0fc) {
#ifdef MUST_MATCH
        if (float_0_8041b100 == (want = party->field_110)) {
#else
        if (float_0_8041b100 == party->field_110) {
#endif
            f32 t = (party->field_8 & 0x1000) ? float_20_8041b140 : float_neg40_8041b144;
            f32 s;

            s = float_0p125_8041b13c * (t - old);
            party->field_10C = old + s;
            if (!(party->field_8 & 0x1000)) {
                if (party->field_10C < float_neg30_8041b148) {
                    party->field_8 |= 0x1000;
                }
            } else if (party->field_10C >= float_0_8041b100) {
                party->field_10C = float_0_8041b100;
                party->field_8 &= ~0x1000;
            }
#ifdef MUST_MATCH
        } else if (float_180_8041b0fc == want) {
#else
        } else if (float_180_8041b0fc == party->field_110) {
#endif
            f32 t = (party->field_8 & 0x1000) ? float_170_8041b138 : float_220_8041b14c;
            f32 s;

            s = float_0p125_8041b13c * (t - old);
            party->field_10C = old + s;
            if (!(party->field_8 & 0x1000)) {
                if (party->field_10C > float_210_8041b150) {
                    party->field_8 |= 0x1000;
                } else if (party->field_10C <= float_180_8041b0fc) {
                    party->field_10C = float_180_8041b0fc;
                    party->field_8 &= ~0x1000;
                }
            }
        }
    }
}
#pragma opt_common_subs reset
#pragma opt_lifetimes reset

// Keep CSE off across this function: with it on the order multiply's FPRs land in f1/f0/f2
// instead of the target's f2/f1/f0.
#pragma opt_common_subs off
void partyPreDisp(PartyEntry* party) {
    Vec dispPos;
    Vec hitCheckPos;
    Vec dir;
    Vec hitPos;
    Vec hitNormal;
    f32 dist;
    f32 order;
    s32 shadowMode;
    f32 size;
    s32 shadowId;

    if (!(party->flags2 & 0x10) && !(party->flags & 0x200)) {
        partyMakeDispDir(party);
    }

    if (party->field_8 & 0x6000) {
        if (party->field_8 & 0x2000) {
            party->field_8 &= ~0x2000;
        }
        if (party->field_8 & 0x4000) {
            party->field_8 &= ~0x4000;
        }
        animPoseSetAnim(party->paperAnimGroup, party->field_18, 1);
        if (party->field_1C != 0) {
#ifdef MUST_MATCH
            ((void (*)(s32, const char*, BOOL))animPoseSetPaperAnim)(party->paperAnimGroup, (const char*)party->field_1C,
                                                                    1);
#else
            animPoseSetPaperAnim(party->paperAnimGroup, (const char*)party->field_1C);
#endif
        }
        animPoseSetLocalTime(party->paperAnimGroup, (f32)party->field_20);
    }

    animPoseMain(party->paperAnimGroup);

    // The original wraps the whole tail (disp entry, shadow setup, vivian, shadow entry) in this arm:
    // the target's `bne` at `flags2 & 1` jumps straight to the epilogue.
    if (!(party->flags2 & 1)) {
        dispPos = party->position;
        PSMTX44MultVec(camGetPtr(CAMERA_3D)->projection, &dispPos, &dispPos);
        order = float_5000_8041b114 * dispPos.z + float_5000_8041b114;
        if (order < float_0_8041b100) {
            order = float_0_8041b100;
        } else if (order > float_10000_8041b118) {
            order = float_10000_8041b118;
        }
        order = order * float_neg1_8041b0ec;
        if (party->field_2A > 0) {
            if (!(party->field_2A & 2)) {
                dispEntry((CameraId)party->camId, order, 1, partyDisp, party);
            }
        } else {
            dispEntry((CameraId)party->camId, order, 1, partyDisp, party);
        }

        shadowMode = 2;
        size = float_13p333_8041b11c;
        if (party->currentMemberId != PARTY_MEMBER_VIVIAN) {
            party->field_B8 = party->position;
        } else {
            if (party->field_34 == 4 || (party->field_34 == 8 && !(party->flags & 0x100))) {
                party->field_B8 = party->position;
            }
            if (party->flags & 0x100) {
                size = PF32(party, 0x174);
                if (party->flags & 0x80000000) {
                    shadowMode = 1;
                }
            }
        }

        if (N_vivianDisplayTailCheck(party)) {
            dist = float_1000_8041b120;
            hitCheckPos = party->position;
            hitCheckPos.y += float_10_8041b124;
            dir = vec3_802ccee0;
            party->field_B8.y = partyHitCheck(party, &hitCheckPos, &dir, &hitPos, &hitNormal, &dist) ? hitPos.y
                                                                                                     : party->position.y;
            animPoseVivianMain(party->paperAnimGroup, &party->field_B8);
        }

        if (!(party->flags2 & 0x100000)) {
            shadowId = shadowEntryMode(shadowMode, party->field_B8.x, party->field_B8.y, party->field_B8.z, size);
            if (party->camId != 4) {
                shadowSetCamId(shadowId, party->camId);
            }
            if (party->flags2 & 0x200000) {
                shadowFlagOn(shadowId, 4);
            }
        }
    }
}
#pragma opt_common_subs reset

// Matches (100%): the leading count inlines L_partyNum (above partyMain), whose
// `partyPtrTbl[i] && (partyGetPtr(i)->flags & 1)` form makes MWCC hoist the element load
// above the index test the way the original does.
void partyDisp(CameraId cameraId, void* param) {
    PartyEntry* party = param;
    Mtx scaleMtx;
    Mtx transMtx;
    Mtx rotMtx;
    f32 angle;
    f32 scale;
    f32 yOffset;

    if (L_partyNum() != 0) {
        if (party->flags & 0x200) {
            PF32(party, 0x88) = float_neg6p8_8041b0d8;
            PF32(party, 0x8C) = float_6p8_8041b0dc;
        }

        scale = (party->flags & 0x800000) ? float_1p2_8041b0e0 : float_2_8041b0e4;
        PSMTXScale(scaleMtx, party->field_70.x * scale, party->field_70.y * scale, party->field_70.z * scale);

        if (!(party->flags2 & 8)) {
#ifdef MUST_MATCH
            // MUST_MATCH: loading the constant through a volatile pointer keeps the compiler from
            // folding it into the ternary else (which emits `fmr` instead of a second `lfs`).
            f32 one;

            angle = revise360(party->field_10C);
            one = *(volatile f32*)&float_1_8041b0e8;
            PSMTXScale(rotMtx, one, one,
                       (angle >= float_90_8041b0f0 && angle <= float_270_8041b0f4) ? float_neg1_8041b0ec
                                                                                  : float_1_8041b0e8);
#else
            angle = revise360(party->field_10C);
            PSMTXScale(rotMtx, float_1_8041b0e8, float_1_8041b0e8,
                       (angle >= float_90_8041b0f0 && angle <= float_270_8041b0f4) ? float_neg1_8041b0ec
                                                                                  : float_1_8041b0e8);
#endif
            PSMTXConcat(rotMtx, scaleMtx, scaleMtx);
        }

        PSMTXTrans(transMtx, -PF32(party, 0x88), -PF32(party, 0x8C), -PF32(party, 0x90));
        PSMTXConcat(transMtx, scaleMtx, scaleMtx);

        PSMTXRotRad(transMtx, 'z', float_deg2rad_8041b0f8 * PF32(party, 0x6C));
        PSMTXConcat(transMtx, scaleMtx, scaleMtx);

        PSMTXRotRad(transMtx, 'x', float_deg2rad_8041b0f8 * PF32(party, 0x64));
        PSMTXConcat(transMtx, scaleMtx, scaleMtx);

        angle = revise360(party->field_10C - PF32(party, 0xF8));
        PSMTXRotRad(transMtx, 'y', float_deg2rad_8041b0f8 * angle);
        PSMTXConcat(transMtx, scaleMtx, scaleMtx);

        if (party->flags2 & 0xC0) {
            angle = revise360(party->field_10C);
            if (angle >= float_90_8041b0f0 && angle <= float_270_8041b0f4) {
                PSMTXRotRad(transMtx, 'y',
                            float_deg2rad_8041b0f8 * ((party->flags2 & 0x40) ? float_180_8041b0fc : float_0_8041b100));
                PSMTXConcat(transMtx, scaleMtx, scaleMtx);
            } else {
                PSMTXRotRad(transMtx, 'y',
                            float_deg2rad_8041b0f8 * ((party->flags2 & 0x40) ? float_0_8041b100 : float_180_8041b0fc));
                PSMTXConcat(transMtx, scaleMtx, scaleMtx);
            }
        }

        PSMTXTrans(transMtx, PF32(party, 0x88), PF32(party, 0x8C), PF32(party, 0x90));
        PSMTXConcat(transMtx, scaleMtx, scaleMtx);

        yOffset = float_0_8041b100;
        if (party->field_1C != 0) {
            if (party->currentMemberId == PARTY_MEMBER_YOSHI) {
                yOffset = float_1_8041b0e8;
            } else if (party->currentMemberId == PARTY_MEMBER_BOBBERY) {
                yOffset = float_2_8041b0e4;
            } else if (party->currentMemberId == PARTY_MEMBER_MS_MOWZ) {
                yOffset = float_2_8041b0e4;
            } else if (party->currentMemberId == PARTY_MEMBER_GOOMBELLA) {
                yOffset = float_2_8041b0e4;
            } else if (party->currentMemberId == PARTY_MEMBER_FRANKLY) {
                yOffset = float_3_8041b104;
            } else if (party->currentMemberId == PARTY_MEMBER_EGG) {
                yOffset = float_3p5_8041b108;
            } else if (party->currentMemberId == PARTY_MEMBER_PUNIO) {
                yOffset = float_4_8041b10c;
            } else if (party->currentMemberId == PARTY_MEMBER_FLAVIO) {
                yOffset = float_neg1_8041b0ec;
            }
        }

        PSMTXTrans(transMtx, party->position.x + PF32(party, 0x7C),
                   party->position.y + PF32(party, 0x80) + yOffset, party->position.z + PF32(party, 0x84));
        PSMTXConcat(transMtx, scaleMtx, scaleMtx);

        animPoseDrawMtx(party->paperAnimGroup, scaleMtx, 2, float_0_8041b100, float_1_8041b0e8);
        animPoseDrawMtx(party->paperAnimGroup, scaleMtx, 1, float_0_8041b100, float_1_8041b0e8);
        animPoseSetLocalTimeRate(party->paperAnimGroup,
                                 (party->flags & 0x80) ? float_0p75_8041b110 : float_1_8041b0e8);
    }
}

void partyKeyOn(void) {}

void partyKeyOff(void) {}

void L_partyClearCont(PartyEntry* party) {
    P8(party, 0x158) = 0;
    P8(party, 0x159) = 0;
    P8(party, 0x15A) = 0;
    P8(party, 0x15B) = 0;
    P8(party, 0x15C) = 0;
    P8(party, 0x15D) = 0;
    P8(party, 0x15E) = 0;
    P8(party, 0x15F) = 0;
    P16(party, 0x150) = 0;
    P16(party, 0x152) = 0;
    P16(party, 0x154) = 0;
    P16(party, 0x156) = 0;
}

void partyUpdateKeyData(PartyEntry* party) {
    s32 member = PS8(party, 0x30);

    if (party->flags & 0x60000000) {
        P8(party, 0x158) = 0;
        P8(party, 0x159) = 0;
        P8(party, 0x15A) = 0;
        P8(party, 0x15B) = 0;
        P8(party, 0x15C) = 0;
        P8(party, 0x15D) = 0;
        P8(party, 0x15E) = 0;
        P8(party, 0x15F) = 0;
        P16(party, 0x150) = 0;
        P16(party, 0x152) = 0;
        P16(party, 0x154) = 0;
        P16(party, 0x156) = 0;
        return;
    }
    P8(party, 0x158) = gp->stickX[member];
    P8(party, 0x159) = -gp->stickY[member];
    P8(party, 0x15A) = gp->substickX[member];
    P8(party, 0x15B) = -gp->substickY[member];
    P8(party, 0x15C) = gp->triggerLeft[member];
    P8(party, 0x15D) = gp->triggerRight[member];
    P8(party, 0x15E) = 0;
    P8(party, 0x15F) = 0;
    P16(party, 0x150) = gp->button[member];
    P16(party, 0x152) = gp->buttonNew[member];
    P16(party, 0x154) = gp->buttonRepeat[member];
    P16(party, 0x156) = gp->buttonUp[member];
}

void partyShadowOff(PartyEntry* party) {
    if (party) {
        party->flags2 |= 0x100000;
    }
}

// The original emits two copies of the common body, one per arm. The natural shared-body form is
// under #else; the #ifdef form duplicates it so the two copies are emitted.
#ifdef MUST_MATCH
// L_partyChgPoseId1 is defined with the other shared helpers further down the file.
static inline char* L_partyChgPoseId1(PartyEntry* party);

// Variant of L_animName without the `dp` local: the target schedules the party_dp load at the
// use site, not at the top of the branch.
static inline const char* L_famicomAnimName(PartyEntry* party) {
    if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
        if (!(party->flags2 & 0x8000)) {
            return party_dp->name;
        }
        return str_802CCEEC;
    }
    {
        const char* name = yoshigroup[pouchGetPartyColor(PARTNER_YOSHI)];
        if (party->flags2 & 0x8000) {
            name = str_802CCEEC;
        }
        return name;
    }
}

static inline char* L_partyChgPoseId1b(PartyEntry* party) {
    s32 index = 1;
    s32 v5;
    char* v6;
    char* v8;

    if ((party->flags2 & 2) != 0) {
        return party->field_18;
    }
    v5 = 0;
    if (party->currentMemberId < 8 && partyGetHp(party->currentMemberId) <= 5) {
        v5 = TRUE;
    }
    if (v5 == TRUE) {
        index = 12;
    }
    if (!(party->flags2 & 0x8000)) {
        v6 = partyPoseTbl[party->currentMemberId][index];
    } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
        v6 = partyDotPoseTbl[party->currentMemberId][index];
    } else {
        v6 = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][index];
    }
    v8 = party->field_18;
    if (v8 && !strcmp(v8, v6)) {
        return v6;
    }
    party->field_18 = v6;
    party->field_20 = 0;
    party->field_8 |= 0x2000;
    return v6;
}

static inline void L_partySetFamicomBodyA(PartyEntry* party) {
    const char* animName = L_famicomAnimName(party);

    if (party->paperAnimGroup >= 0) {
        animPoseRelease(party->paperAnimGroup);
    }
    party->paperAnimGroup = animPoseEntry(animName, 2);
    partyChgPose(party, L_partyChgPoseId1(party));
}

static inline void L_partySetFamicomBodyB(PartyEntry* party) {
    const char* animName = L_famicomAnimName(party);

    if (party->paperAnimGroup >= 0) {
        animPoseRelease(party->paperAnimGroup);
    }
    party->paperAnimGroup = animPoseEntry(animName, 2);
    partyChgPose(party, L_partyChgPoseId1b(party));
}
#define PARTY_FAMICOM_BODY() L_partySetFamicomBodyA(party)
#endif

// The str_802CCEEC materialisations and the anim-name register now match, and the two body copies
// (one per arm) are emitted like the original.
// NONMATCHING (99.84%): the pose value in the second (mode != 1) inline of the pose selection
// lands in r29 instead of the target's r30; index/lowHp registers already match.
void partySetFamicomMode(PartyEntry* party, s32 mode) {
    if (mode == 1) {
        if (party->flags2 & 0x8000) {
            return;
        }
        L_partyForceSlitOff(party);
        party->flags2 |= 0x8000;
#ifdef MUST_MATCH
        PARTY_FAMICOM_BODY();
#endif
    } else {
        if (!(party->flags2 & 0x8000)) {
            return;
        }
        party->flags2 &= ~0x8000;
#ifdef MUST_MATCH
        L_partySetFamicomBodyB(party);
#endif
    }
#ifndef MUST_MATCH
    {
        const char* animName;

        if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
            if (!(party->flags2 & 0x8000)) {
                animName = party_dp->name;
            } else {
                animName = str_802CCEEC;
            }
        } else {
            animName = yoshigroup[pouchGetPartyColor(PARTNER_YOSHI)];
            if (party->flags2 & 0x8000) {
                animName = str_802CCEEC;
            }
        }
        if (party->paperAnimGroup >= 0) {
            animPoseRelease(party->paperAnimGroup);
        }
        party->paperAnimGroup = animPoseEntry(animName, 2);
        partyChgPose(party, partyChgPoseId(party, 1));
    }
#endif
}
