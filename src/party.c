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
#include "party/party_bomhei.h"
#include "party/party_motion.h"
#include "math_ext.h"
#include <dolphin/mtx.h>
#include "driver/dispdrv.h"
#include "driver/shadowdrv.h"
#include "party/party_hit.h"

// Raw field accessors for fields missing from the shared PartyEntry header.
#define P8(p, off) (*(u8*)((u8*)(p) + (off)))
#define PS8(p, off) (*(s8*)((u8*)(p) + (off)))
#define P16(p, off) (*(u16*)((u8*)(p) + (off)))
#define P32(p, off) (*(u32*)((u8*)(p) + (off)))
#define PF32(p, off) (*(f32*)((u8*)(p) + (off)))



//.data

extern char* partyPoseTbl[20][15];
extern char* partyDotPoseTbl[8][15];
extern char* yoshiDotPoseTbl[7][15];
extern PartyData partyDataTbl[];
extern Vec vec3_802ccdcc;
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
const f32 vec3_802ccd30[3] = { 0.0f, 0.0f, 0.0f };
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

// NONMATCHING (96.60%): 1 missing, 1 extra, plus register differences.
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

void partyKeyOn(void) {}

void partyKeyOff(void) {}

void partyShadowOff(PartyEntry* party) {
    if (party) {
        party->flags2 |= 0x100000;
    }
}

void partyInit(void) {
    memset(partyPtrTbl, 0, sizeof(partyPtrTbl));
}

void partyStop(PartyEntry* party) {
    partyChgRunMode(party, 0);
}

void partyRun(PartyEntry* party) {
    if (party->field_34 == 0) {
        partyChgRunMode(party, 1);
    }
}

f32 partyGetHeight(PartyEntry* party) {
    if (party->currentMemberId == PARTY_MEMBER_YOSHI && (party->flags & PARTY_FLAG_IS_BEING_USED)) {
        return 18.5f + PF32(party, 0xF0);
    }
    return PF32(party, 0xF0);
}

void partyInitCamId(PartyEntry* party) {
    party->camId = marioGetCamId();
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

s32 partyGoodbye(s32 index) {
    s32 ret = 0;

    if (index >= 0) {
        partyChgRunMode(partyGetPtr(index), 8);
        ret = 1;
    }
    return ret;
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

// NONMATCHING (77.37%): 2 instructions differ, 6 missing, 6 extra, plus register differences.
void partyMain(void) {
    s32 num;
    s32 i;

    marioGetPtr();
    num = partyGetNum();
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

// NONMATCHING (98.00%): Vec copy scheduling of 0x70 (0x8 load hoisted) and the
// low-HP `extsb r3,r0` vs our `extsb r0,r0`.
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

// The original emits two copies of the common body, one per arm. The natural shared-body form is
// under #else; the #ifdef form duplicates it so the two copies are emitted.
#ifdef MUST_MATCH
#define PARTY_FAMICOM_BODY()                                                                                           \
    do {                                                                                                               \
        const char* animName;                                                                                                \
        if (party->currentMemberId != PARTY_MEMBER_YOSHI) {                                                            \
            if (!(party->flags2 & 0x8000)) {                                                                           \
                animName = party_dp->name;                                                                             \
            } else {                                                                                                   \
                animName = str_802CCEEC;                                                                                  \
            }                                                                                                          \
        } else {                                                                                                       \
            animName = yoshigroup[pouchGetPartyColor(PARTNER_YOSHI)];                                                   \
            if (party->flags2 & 0x8000) {                                                                              \
                animName = str_802CCEEC;                                                                                  \
            }                                                                                                          \
        }                                                                                                              \
        if (party->paperAnimGroup >= 0) {                                                                              \
            animPoseRelease(party->paperAnimGroup);                                                                    \
        }                                                                                                              \
        party->paperAnimGroup = animPoseEntry(animName, 2);                                                            \
        {                                                                                                              \
            char* pose;                                                                                                \
            if (party->flags2 & 2) {                                                                                   \
                pose = party->field_18;                                                                                \
            } else {                                                                                                   \
                s32 index = 1;                                                                                         \
                s32 lowHp = 0;                                                                                         \
                if (party->currentMemberId < 8 && partyGetHp(party->currentMemberId) <= 5) {                            \
                    lowHp = 1;                                                                                         \
                }                                                                                                      \
                if (lowHp == 1) {                                                                                      \
                    index = 12;                                                                                        \
                }                                                                                                      \
                if (!(party->flags2 & 0x8000)) {                                                                       \
                    pose = partyPoseTbl[party->currentMemberId][index];                                                \
                } else if (party->currentMemberId != PARTY_MEMBER_YOSHI) {                                             \
                    pose = partyDotPoseTbl[party->currentMemberId][index];                                             \
                } else {                                                                                               \
                    pose = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][index];                                  \
                }                                                                                                      \
                if (!(party->field_18 && !strcmp(party->field_18, pose))) {                                            \
                    party->field_18 = pose;                                                                            \
                    party->field_20 = 0;                                                                               \
                    party->field_8 |= 0x2000;                                                                          \
                }                                                                                                      \
            }                                                                                                          \
            partyChgPose(party, pose);                                                                                 \
        }                                                                                                              \
    } while (0)
#endif

// NONMATCHING (95.53%): the two `str_802CCEEC` address materialisations use `addi r0; mr r30,r0`
// instead of a direct `addi r30`, and r28/r31 allocation differs in the pose block.
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
        PARTY_FAMICOM_BODY();
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

// NONMATCHING (97.23%): animName/pose land in r31 instead of r28, and one beq/bne block layout swap.
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
    animPosePeraOff(party->paperAnimGroup);
    {
        MarioWork* mario = party->playerPtr;

        animPoseSetMaterialFlagOn(party->paperAnimGroup,
                                  animPoseGetMaterialFlag(mario->paperAnimGroupId[marioAnimeId()]));
        animPoseSetMaterialLightFlagOn(party->paperAnimGroup,
                                       animPoseGetMaterialLightFlag(mario->paperAnimGroupId[marioAnimeId()]));
    }
#ifdef MUST_MATCH
    {
        char* pose;

        if (party->flags2 & 2) {
            pose = party->field_18;
        } else {
            s32 index = 1;
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
        partyChgPose(party, pose);
    }
#else
    partyChgPose(party, partyChgPoseId(party, 1));
#endif
    party->field_1C = 0;
    ((void (*)(PartyEntry*))party_dp->init_func)(party);
    return slotId;
}

// NONMATCHING (94.17%): 4 saved registers instead of the original 5 (no `stmw`/`lmw`), so every
// register number is shifted; vec3_802ccdd8 is accessed via `@sda21` bases instead of one base+offset.
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
    if (party->paperAnimGroup >= 0) {
        animPoseRelease(party->paperAnimGroup);
    }
    party->paperAnimGroup = animPoseEntry(animName, 2);
    animPosePeraOff(party->paperAnimGroup);
    {
        MarioWork* mario = party->playerPtr;

        animPoseSetMaterialFlagOn(party->paperAnimGroup,
                                  animPoseGetMaterialFlag(mario->paperAnimGroupId[marioAnimeId()]));
        animPoseSetMaterialLightFlagOn(party->paperAnimGroup,
                                       animPoseGetMaterialLightFlag(mario->paperAnimGroupId[marioAnimeId()]));
    }
#ifdef MUST_MATCH
    {
        char* pose;

        if (party->flags2 & 2) {
            pose = party->field_18;
        } else {
            s32 index = 1;
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
        partyChgPose(party, pose);
    }
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

// NONMATCHING (92.97%): 4 saved registers instead of the original `stmw r27`/`lmw`, so every register
// number is shifted; one branch layout swap.
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
    if (party->paperAnimGroup >= 0) {
        animPoseRelease(party->paperAnimGroup);
    }
    party->paperAnimGroup = animPoseEntry(animName, 2);
    animPosePeraOff(party->paperAnimGroup);
    {
        MarioWork* mario = party->playerPtr;

        animPoseSetMaterialFlagOn(party->paperAnimGroup,
                                  animPoseGetMaterialFlag(mario->paperAnimGroupId[marioAnimeId()]));
        animPoseSetMaterialLightFlagOn(party->paperAnimGroup,
                                       animPoseGetMaterialLightFlag(mario->paperAnimGroupId[marioAnimeId()]));
    }
#ifdef MUST_MATCH
    {
        char* pose;

        if (party->flags2 & 2) {
            pose = party->field_18;
        } else {
            s32 index = 1;
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
        partyChgPose(party, pose);
    }
#else
    partyChgPose(party, partyChgPoseId(party, 1));
#endif
    party->field_1C = 0;
    ((void (*)(PartyEntry*))party_dp->init_func)(party);
    partyChgRunMode(party, 4);
    return slotId;
}

// NONMATCHING (95.49%): memberId in r27 vs r28, table base r29 vs r27, and vec3_802ccdcc accessed via
// `@sda21` per component instead of one base+offset.
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
    if (party->paperAnimGroup >= 0) {
        animPoseRelease(party->paperAnimGroup);
    }
    party->paperAnimGroup = animPoseEntry(animName, 2);
    animPosePeraOff(party->paperAnimGroup);
    {
        MarioWork* mario = party->playerPtr;

        animPoseSetMaterialFlagOn(party->paperAnimGroup,
                                  animPoseGetMaterialFlag(mario->paperAnimGroupId[marioAnimeId()]));
        animPoseSetMaterialLightFlagOn(party->paperAnimGroup,
                                       animPoseGetMaterialLightFlag(mario->paperAnimGroupId[marioAnimeId()]));
    }
#ifdef MUST_MATCH
    {
        char* pose;

        if (party->flags2 & 2) {
            pose = party->field_18;
        } else {
            s32 index = 1;
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
        partyChgPose(party, pose);
    }
#else
    partyChgPose(party, partyChgPoseId(party, 1));
#endif
    party->field_1C = 0;
    ((void (*)(PartyEntry*))party_dp->init_func)(party);
    party->position = player->position;
    party->field_B8 = party->position;
    {
        const Vec* v = &vec3_802ccdcc;

        party->field_70 = *v;
    }
    party->field_34 = 0;
    return slotId;
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

// NONMATCHING (99.38%): first draft of partyMakeDispDir.
void partyMakeDispDir(PartyEntry* party) {
    f32 angle;
    f32 target;

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
    if (party->field_10C <= float_180_8041b0fc) {
        if (float_0_8041b100 == party->field_110) {
            f32 t = (party->field_8 & 0x1000) ? float_20_8041b140 : float_neg40_8041b144;
            f32 s;

            s = float_0p125_8041b13c * (t - party->field_10C);
            party->field_10C = party->field_10C + s;
            if (!(party->field_8 & 0x1000)) {
                if (party->field_10C < float_neg30_8041b148) {
                    party->field_8 |= 0x1000;
                }
            } else if (party->field_10C >= float_0_8041b100) {
                party->field_10C = float_0_8041b100;
                party->field_8 &= ~0x1000;
            }
        } else if (float_180_8041b0fc == party->field_110) {
            f32 t = (party->field_8 & 0x1000) ? float_170_8041b138 : float_220_8041b14c;
            f32 s;

            s = float_0p125_8041b13c * (t - party->field_10C);
            party->field_10C = party->field_10C + s;
            if (!(party->field_8 & 0x1000)) {
                if (party->field_10C > float_210_8041b150) {
                    party->field_8 |= 0x1000;
                }
            } else if (party->field_10C <= float_180_8041b0fc) {
                party->field_10C = float_180_8041b0fc;
                party->field_8 &= ~0x1000;
            }
        }
    } else if (party->field_10C > float_180_8041b0fc) {
        if (float_0_8041b100 == party->field_110) {
            f32 t = (party->field_8 & 0x1000) ? float_20_8041b140 : float_neg40_8041b144;
            f32 s;

            s = float_0p125_8041b13c * (t - party->field_10C);
            party->field_10C = party->field_10C + s;
            if (!(party->field_8 & 0x1000)) {
                if (party->field_10C < float_neg30_8041b148) {
                    party->field_8 |= 0x1000;
                }
            } else if (party->field_10C >= float_0_8041b100) {
                party->field_10C = float_0_8041b100;
                party->field_8 &= ~0x1000;
            }
        } else if (float_180_8041b0fc == party->field_110) {
            f32 t = (party->field_8 & 0x1000) ? float_170_8041b138 : float_220_8041b14c;
            f32 s;

            s = float_0p125_8041b13c * (t - party->field_10C);
            party->field_10C = party->field_10C + s;
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

// NONMATCHING (92.21%): the compiler's magic unsigned->double constant is `@911` instead of
// `double_to_int_mask_802ccef8`; float register allocation in the order calculation and the
// partyHitCheck argument build scheduling differ.
void partyPreDisp(PartyEntry* party) {
    Vec dispPos;
    Vec hitCheckPos;
    Vec dir;
    Vec hitPos;
    Vec hitNormal;
    f32 dist;
    f32 order;
    s16 field2A;
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
        field2A = party->field_2A;
        if (field2A > 0) {
            if (!(field2A & 2)) {
                dispEntry((CameraId)party->camId, order, 1, partyDisp, party);
            }
        } else {
            dispEntry((CameraId)party->camId, order, 1, partyDisp, party);
        }
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
        dir = vec3_802ccee0;
        hitCheckPos = party->position;
        dist = float_1000_8041b120;
        hitCheckPos.y += float_10_8041b124;
        if (partyHitCheck(party, &hitCheckPos, &dir, &hitPos, &hitNormal, &dist)) {
            party->field_B8.y = hitPos.y;
        } else {
            party->field_B8.y = party->position.y;
        }
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

// NONMATCHING (94.42%): the leading partyGetNum()/partyGetPtr() inline keeps the
// partyPtrTbl element load after the index test; the original hoists the load above it
// and tests the element before the index (the same compiler difference as partyMain).
void partyDisp(CameraId cameraId, void* param) {
    PartyEntry* party = param;
    Mtx scaleMtx;
    Mtx transMtx;
    Mtx rotMtx;
    f32 angle;
    f32 scale;
    f32 yOffset;

    if (partyGetNum() != 0) {
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
