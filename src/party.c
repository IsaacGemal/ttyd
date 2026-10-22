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
#define PS8(p, off) (*(s8*)((u8*)(p) + (off)))
#define P16(p, off) (*(u16*)((u8*)(p) + (off)))



//.data

extern char* partyPoseTbl[20][15];
extern char* partyDotPoseTbl[8][15];
extern char* yoshiDotPoseTbl[7][15];
extern PartyData partyDataTbl[];

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
PartyEntry* partyPtrTbl[2];
PartyData* party_dp;
s32 partyCtrlNo;


// Prototypes for the forward references the original function order needs.
static inline void partyHelloEnd(PartyEntry* party);
static inline void partyRideEnd(PartyEntry* party);

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

BOOL partyPaperOn(PartyEntry* party, const char* animGroup) {
    BOOL flag;
    BOOL lightOff;

    partyPaperRelease(party);
    party->flags2 |= 0x4000000;
    party->paperPoseId = animPaperPoseEntry(animGroup, 2);
    flag = !strcmp(animGroup, "p_roll") || !strcmp(animGroup, "p_plane") || !strcmp(animGroup, "p_dokan_x") ||
           !strcmp(animGroup, "p_dokan_y");
    lightOff = flag ? FALSE : TRUE;
    if (!strcmp(animGroup, "p_slit")) {
        lightOff = party->currentMemberId == PARTY_MEMBER_FLURRIE;
    }
    animPoseSetPaperAnimGroup(party->paperAnimGroup, animGroup, lightOff);
    return TRUE;
}

void partyPaperOff(PartyEntry* party) {
    s32 paperPoseId; // r3

    if ((party->flags2 & 0x4000000) && party->paper) {
        animPoseSetPaperAnimGroup(party->paperAnimGroup, NULL, FALSE);
        party->paper = NULL;
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

void setAnimPoseMaterialFlag(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;

    animPoseSetMaterialFlagOn(party->paperAnimGroup, animPoseGetMaterialFlag(mario->paperAnimGroupId[marioAnimeId()]));
    animPoseSetMaterialLightFlagOn(party->paperAnimGroup,
                                   animPoseGetMaterialLightFlag(mario->paperAnimGroupId[marioAnimeId()]));
}

void partyPaperRelease(PartyEntry* party) {
    if (party->paperPoseId >= 0) {
        animPaperPoseRelease(party->paperPoseId);
        party->paperPoseId = -1;
    }
}

char* partyChgPoseId(PartyEntry* party, s32 poseId) {
    BOOL v5;             // r30
    char* v6;            // r30
    s32 index = poseId;

    if ((party->flags2 & 2) != 0) {
        return party->pose;
    }
    v5 = partyChkHerohero(party);
    if (v5 == TRUE) {
        if (poseId == 1) {
            index = 12;
        } else if (poseId == 2) {
            index = 13;
        } else if (poseId == 3) {
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
    if (party->pose && !strcmp(party->pose, v6)) {
        return v6;
    }
    party->pose = v6;
    party->field_20 = 0;
    party->field_8 |= 0x2000u;
    return v6;
}

void partyChgPose(PartyEntry* party, char* pose) {
    if (party->flags2 & 2) {
        return;
    }
    if (party->pose && strcmp(party->pose, pose) == 0) {
        return;
    }
    if (party->flags2 & 0x8000) {
        return;
    }
    party->pose = pose;
    party->field_20 = 0;
    party->field_8 |= 0x2000;
}

void partyChgPaper(PartyEntry* party, char* paper) {
    if (party->flags2 & 2) {
        return;
    }
    if (party->paper && strcmp(party->paper, paper) == 0) {
        return;
    }
    party->paper = paper;
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
    if (party->runMode == 0) {
        partyChgRunMode(party, 1);
    }
}

void partyStop(PartyEntry* party) {
    partyChgRunMode(party, 0);
}

void partySetForceMove(PartyEntry* party, s32 unk, f32 x, f32 z) {
    party->dir = x;
    party->field_28 = unk;
    party->speed = z;
    partyChgRunMode(party, 9);
    partyChgMot(party, 1);
}

s32 getPartyEntryId(void) {
    s32 i;
    PartyEntry* party;

    for (i = 0; i < 2; i++) {
        if (partyPtrTbl[i] == NULL) {
            party = __memAlloc(HEAP_DEFAULT, sizeof(PartyEntry));
            partyPtrTbl[i] = party;
            memset(party, 0, sizeof(PartyEntry));
            return i;
        }
    }
    return -1;
}

s32 partyEntryHello(s32 memberId) {
    s32 ret = partyEntryMain(memberId);

    partyChgRunMode(partyGetPtr(ret), 4);
    return ret;
}

s32 partyEntry(s32 memberId) {
    PartyEntry* party;
    s32 index;
    char* pose;
    s32 ret;
    s32 lowHp;

    ret = partyEntryMain(memberId);
    party = partyGetPtr(ret);
    party->scale = (Vec){1.0f, 1.0f, 1.0f};
    party->motionStep = 0xa;
    {
        MarioWork* mario = party->playerPtr;
        party->field_110 = mario->dispDir;
        party->dispDir = mario->dispDir;
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
        if (!(party->pose && !strcmp(party->pose, pose))) {
            party->pose = pose;
            party->field_20 = 0;
            party->field_8 |= 0x2000;
        }
    }
    party->runMode = 1;
    return ret;
}

s32 partyEntryPos(s32 memberId, f32 x, f32 y, f32 z) {
    PartyEntry* party;
    s32 index;
    s32 ret;
    s32 lowHp;
    char* pose;
    char* prev;
    Vec pos;

    ret = partyEntryMain(memberId);
    party = partyGetPtr(ret);
    index = 1;
    party->scale = (Vec){1.0f, 1.0f, 1.0f};
    pos = (Vec){0.0f, 0.0f, 0.0f};
    pos.x = x;
    pos.y = y;
    pos.z = z;
    party->position = pos;
    {
        MarioWork* mario = party->playerPtr;
        party->scale = (Vec){1.0f, 1.0f, 1.0f};
        party->field_110 = mario->dispDir;
        party->dispDir = mario->dispDir;
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
        prev = party->pose;
        if (!(prev && !strcmp(prev, pose))) {
            party->pose = pose;
            party->field_20 = 0;
            party->field_8 |= 0x2000;
        }
    }
    party->runMode = 0;
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
    if (party_dp->exit_func && party->runMode != 0xb) {
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
    if (party_dp->exit_func && party->runMode != 0xb) {
        ((PartyFunc)party_dp->exit_func)(party);
    }
    player->prevPartyMemberId[index] = 0;
    player->currPartySlotId[index] = -1;
    partyPtrTbl[index] = NULL;
    animPoseRelease(party->paperAnimGroup);
    __memFree(HEAP_DEFAULT, party);
    return 0;
}

char* yoshigroup[7] = {
    "c_babyyoshi",   "c_babyyoshi2", "c_babyyoshi3", "c_babyyoshi4",
    "c_babyyoshi5",  "c_babyyoshi6", "c_babyyoshi7",
};

void partyPoseEntry(PartyEntry* party) {
    const char* animName;

    if (party->currentMemberId != PARTY_MEMBER_YOSHI) {
        if (!(party->flags2 & 0x8000)) {
            animName = party_dp->name;
        } else {
            animName = "d_mario";
        }
    } else {
        animName = yoshigroup[pouchGetPartyColor(PARTNER_YOSHI)];
        if (party->flags2 & 0x8000) {
            animName = "d_mario";
        }
    }
    if (party->paperAnimGroup >= 0) {
        animPoseRelease(party->paperAnimGroup);
    }
    party->paperAnimGroup = animPoseEntry(animName, 2);
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
    pouchSetPartyColor(PARTNER_YOSHI, color);
    return color;
}

f32 partyGetHeight(PartyEntry* party) {
    if (party->currentMemberId == PARTY_MEMBER_YOSHI && (party->flags & PARTY_FLAG_IS_BEING_USED)) {
        return 18.5f + party->height;
    }
    return party->height;
}

s32 partyEntryMain(s32 memberId) {
    MarioWork* player = marioGetPtr();
    s32 slotId;
    PartyEntry* party;

    slotId = getPartyEntryId();
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
        party->flags |= PARTY_FLAG_IS_FOLLOWER;
    }
    party_dp = &partyDataTbl[memberId];
    if (player->flags & 0x80000000) {
        party->flags2 |= 0x8000;
    }
    partyPoseEntry(party);
    animPosePeraOff(party->paperAnimGroup);
    setAnimPoseMaterialFlag(party);
    partyChgPose(party, partyChgPoseId(party, 1));
    party->paper = NULL;
    party_dp->init_func(party);
    return slotId;
}

s32 partyEntry2(s32 memberId) {
    MarioWork* player = marioGetPtr();
    s32 slotId;
    PartyEntry* party;
    const char* animName;

    if (partyPtrTbl[1] == NULL) {
        partyPtrTbl[1] = __memAlloc(HEAP_DEFAULT, sizeof(PartyEntry));
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
        party->flags |= PARTY_FLAG_IS_FOLLOWER;
    }
    party_dp = &partyDataTbl[memberId];
    partyPoseEntry(party);
    animPosePeraOff(party->paperAnimGroup);
    setAnimPoseMaterialFlag(party);
    partyChgPose(party, partyChgPoseId(party, 1));
    party->paper = NULL;
    party_dp->init_func(party);
    party->position = player->position;
    party->field_B8 = party->position;
    {
        party->scale = (Vec){1.0f, 1.0f, 1.0f};
    }
    party->runMode = 0;
    return slotId;
}

s32 partyEntry2Hello(s32 memberId) {
    MarioWork* player = marioGetPtr();
    s32 slotId;
    PartyEntry* party;
    const char* animName;

    if (partyPtrTbl[1] == NULL) {
        partyPtrTbl[1] = __memAlloc(HEAP_DEFAULT, sizeof(PartyEntry));
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
        party->flags |= PARTY_FLAG_IS_FOLLOWER;
    }
    party_dp = &partyDataTbl[memberId];
    partyPoseEntry(party);
    animPosePeraOff(party->paperAnimGroup);
    setAnimPoseMaterialFlag(party);
    partyChgPose(party, partyChgPoseId(party, 1));
    party->paper = NULL;
    party_dp->init_func(party);
    partyChgRunMode(party, 4);
    return slotId;
}

s32 partyEntry2Pos(s32 memberId, f32 x, f32 y, f32 z) {
    MarioWork* player = marioGetPtr();
    s32 slotId;
    PartyEntry* party;
    const char* animName;

    if (partyPtrTbl[1] == NULL) {
        partyPtrTbl[1] = __memAlloc(HEAP_DEFAULT, sizeof(PartyEntry));
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
        party->flags |= PARTY_FLAG_IS_FOLLOWER;
    }
    party_dp = &partyDataTbl[memberId];
    partyPoseEntry(party);
    animPosePeraOff(party->paperAnimGroup);
    setAnimPoseMaterialFlag(party);
    partyChgPose(party, partyChgPoseId(party, 1));
    party->paper = NULL;
    party_dp->init_func(party);
    partyChgRunMode(party, 2);
    {
        Vec pos = {0.0f, 0.0f, 0.0f};

        pos.x = x;
        pos.y = y;
        pos.z = z;
        party->position = pos;
        party->field_B8 = party->position;
        party->scale.x = 1.0f;
        party->scale.y = 1.0f;
        party->scale.z = 1.0f;
    }
    return slotId;
}

void partyInit(void) {
    memset(partyPtrTbl, 0, sizeof(partyPtrTbl));
}

void partyReInit(void) {
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
        party->field_37 = 0;
        if (party->paper) {
            animPoseSetPaperAnimGroup(party->paperAnimGroup, NULL, 0);
            party->paper = NULL;
            party->flags2 &= ~0x04000000;
            if (party->paperPoseId >= 0) {
                animPaperPoseRelease(party->paperPoseId);
                party->paperPoseId = -1;
            }
        }
        party->frontHit = NULL;
        party->field_12C = NULL;
        party->field_0x148 = 0;
        party->groundHit = NULL;
        party->field_0x13C = NULL;
        party->field_134 = NULL;
        party->headHit = NULL;
        party->field_0x144 = NULL;
        partyChgPose(party, partyChgPoseId(party, 1));
        partyChgRunMode(party, 2);
        partyChgMot(party, 0);
        party->yoshiPsndSFXId = -1;
        party->scale = (Vec){1.0f, 1.0f, 1.0f};
    }

    party = partyGetPtr(marioGetPartyId());
    if (party) {
        s32 paperAnimGroup;
        s32 playSound = party->flags2 & 0x8000;

        party->flags = 1;
        party->flags2 = 0;
        party->field_37 = 0;
        party->camId = marioGetCamId();
        if (party) {
            paperAnimGroup = party->paperAnimGroup;
            if (paperAnimGroup >= 0) {
                animPoseSetMaterialFlagOff(paperAnimGroup, -1);
                animPoseSetMaterialLightFlagOn(paperAnimGroup, 2);
            }
            if (party->paper) {
                animPoseSetPaperAnimGroup(party->paperAnimGroup, NULL, 0);
                party->paper = NULL;
                party->flags2 &= ~0x04000000;
                if (party->paperPoseId >= 0) {
                    animPaperPoseRelease(party->paperPoseId);
                    party->paperPoseId = -1;
                }
            }
            party->frontHit = NULL;
            party->field_12C = NULL;
            party->field_0x148 = 0;
            party->groundHit = NULL;
            party->field_0x13C = NULL;
            party->field_134 = NULL;
            party->headHit = NULL;
            party->field_0x144 = NULL;
            party->scale = (Vec){1.0f, 1.0f, 1.0f};
        }
        partyClearFootmark();
        if (playSound) {
            partyPoseEntry(party);
        }
        if (!(party->currentMemberId == PARTY_MEMBER_YOSHI && party->runMode == 3)) {
            MarioWork* mario = marioGetPtr();

            if (!(mario->flags & 0x400000)) {
                partyChgPose(party, partyChgPoseId(party, 1));
                partyChgRunMode(party, 2);
                partyChgMot(party, 0);
            } else {
                partyChgPose(party, partyChgPoseId(party, 1));
                partyChgRunMode(party, 2);
                party->moveMode = 6;
                party->motionStep = 0;
                party->flags |= 0x8000;
            }
            if (party->currentMemberId == PARTY_MEMBER_MS_MOWZ) {
                chuchu_reinit(party);
            }
            partyResetTouchWallCt();
        }
    }
}

void partyUsePost(PartyEntry* party) {
    typedef void (*PartyFunc)(PartyEntry*);

    if (party && party->runMode == 3) {
        PartyFunc post = (PartyFunc)partyDataTbl[party->currentMemberId].post_func;

        if (post) {
            post(party);
        }
    }
}

void N__partyChgRunMode(PartyEntry* party, s32 oldMode, s32 newMode) {
    typedef void (*PartyFunc)(PartyEntry*);
    s32 index;
    char* pose;

    switch (oldMode) {
        case 11:
            if (newMode == 8) {
                break;
            }
            partyRideEnd(party);
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
                if (!(party->pose && !strcmp(party->pose, pose))) {
                    party->pose = pose;
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
            partyHelloEnd(party);
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
                if (!(party->pose && !strcmp(party->pose, pose))) {
                    party->pose = pose;
                    party->field_20 = 0;
                    party->field_8 |= 0x2000;
                }
            }
            party->flags2 &= ~0x10;
            party->runMode = 1;
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
    if (party->runMode != 9 && party->runMode != 3) {
        party->field_35 = party->runMode;
    }
    N__partyChgRunMode(party, party->runMode, mode);
    party->runMode = mode;
    party->field_8 |= 8;
}

// Keep N_partyForceChgRunMode out of line: the original calls it rather than inlining.
void N_partyForceChgRunMode(PartyEntry* party, s32 mode, s32 unk) {
    if (party->runMode != 9 && party->runMode != 3) {
        party->field_35 = party->runMode;
    }
    N__partyChgRunMode(party, party->runMode, mode);
    party->runMode = mode;
    party->field_8 &= ~8;
    party->motionStep = unk;
}
#pragma dont_inline reset

void unk_800ba2dc(PartyEntry* party) {
    typedef void (*PartyFunc)(PartyEntry*);
    s32 index;
    char* pose;

    if (party->runMode == party->field_35) {
        return;
    }
    if (party->runMode == 4) {
            partyHelloEnd(party);
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
                if (!(party->pose && !strcmp(party->pose, pose))) {
                    party->pose = pose;
                    party->field_20 = 0;
                    party->field_8 |= 0x2000;
                }
            }
            party->flags2 &= ~0x10;
            party->runMode = 1;
            if (party && party->runMode == 3) {
                if (partyDataTbl[party->currentMemberId].post_func) {
                    ((PartyFunc)partyDataTbl[party->currentMemberId].post_func)(party);
                }
            }
            partyChgMot(party, 0);
    } else if (party->runMode == 11) {
            partyRideEnd(party);
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
                if (!(party->pose && !strcmp(party->pose, pose))) {
                    party->pose = pose;
                    party->field_20 = 0;
                    party->field_8 |= 0x2000;
                }
            }
            party->flags2 &= ~0x10;
            party->flags2 &= ~1;
            if (party) {
                party->flags2 &= ~0x100000;
            }
            if (party && party->runMode == 3) {
                if (partyDataTbl[party->currentMemberId].post_func) {
                    ((PartyFunc)partyDataTbl[party->currentMemberId].post_func)(party);
                }
            }
            partyChgMot(party, 0);
    } else {
            if (party->runMode == 12) {
                if (party) {
                    party->flags2 &= ~0x100000;
                }
            }
            if (party && party->runMode == 3) {
                if (partyDataTbl[party->currentMemberId].post_func) {
                    ((PartyFunc)partyDataTbl[party->currentMemberId].post_func)(party);
                }
            }
            party->runMode = party->field_35;
            party->field_8 |= 8;
            partyChgMot(party, 0);
    }
}

s32 partyGetNum(void) {
    s32 num = 0;
    s32 i;

    for (i = 0; i < 2; i++) {
        PartyEntry* party = partyGetPtr(i);

        if (party && (party->flags & PARTY_FLAG_IS_ACTIVE)) {
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

            if (party && (party->flags & PARTY_FLAG_IS_ACTIVE) && marioBgmodeChk() != 1) {
                driveParty(party);
            }
        }
    }
}

// The original reads these rodata Vecs/strings as offsets from the unit's first rodata object.


void driveParty(PartyEntry* party) {
    MarioWork* player = party->playerPtr;

    party->playerTargetYaw = player->targetYaw;
    if (party->mutekiTime > 0) {
        party->mutekiTime--;
    }
    switch (party->runMode) {
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 0xe:
            break;
        default:
            if (party->flags & PARTY_FLAG_CTRL_OFF) {
                if (vivianGetStatus() != 2) {
                    vivianShadow(party);
                }
                partyPreDisp(party);
                return;
            }
            break;
    }
    if (player->flags & 0x02000000) {
        party->flags |= PARTY_FLAG_BG_MODE;
    } else {
        party->flags &= ~PARTY_FLAG_BG_MODE;
    }
    party->groundHit = NULL;
    party->field_12C = NULL;
    party->headHit = NULL;
    party->field_134 = NULL;
    party->frontHit = NULL;
    switch (party->runMode) {
        case 0:
            if (!(party->flags & PARTY_FLAG_CTRL_OFF) || (party->flags & PARTY_FLAG_RUN_OFF)) {
                if (party->field_8 & 8) {
                    party->field_8 &= ~8;
                    partyChgPoseId(party, 1);
                    party->speed = 0.0f;
                    party->flags &= ~PARTY_FLAG_IS_BEING_USED;
                    partyChgMot(party, 0);
                }
                party_dp = &partyDataTbl[party->currentMemberId];
                party_dp->move_func(party);
            }
            break;
        case 1:
            if (!(party->flags & PARTY_FLAG_CTRL_OFF) || (party->flags & PARTY_FLAG_RUN_OFF)) {
                partyIdleMain(party);
            }
            break;
        case 2:
            if (!(party->flags & PARTY_FLAG_CTRL_OFF) || (party->flags & PARTY_FLAG_RUN_OFF)) {
                party_dp = &partyDataTbl[party->currentMemberId];
                party_dp->move_func(party);
            }
            break;
        case 9:
            if (!(party->flags & PARTY_FLAG_CTRL_OFF) || (party->flags & PARTY_FLAG_RUN_OFF)) {
                partyForceWalk(party);
            }
            break;
        case 3:
            if (!(party->flags & PARTY_FLAG_CTRL_OFF) || (party->flags & PARTY_FLAG_RUN_OFF)) {
                void (*fn)(PartyEntry*);

                party_dp = &partyDataTbl[party->currentMemberId];
                fn = party_dp->use_func;
                party->flags |= PARTY_FLAG_IS_BEING_USED;
                fn(party);
            }
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            partyHello(party);
            break;
        case 8:
            party_dp = &partyDataTbl[party->currentMemberId];
            party_dp->bye_func(party);
            break;
        case 10:
            if (party->field_8 & 8) {
                party->field_8 &= ~8;
                partyDokanInit(party);
                party->flags &= ~PARTY_FLAG_IS_BEING_USED;
            }
            partyDokanMain(party);
            break;
        case 11:
            if (!(party->flags & PARTY_FLAG_CTRL_OFF) || (party->flags & PARTY_FLAG_RUN_OFF)) {
                if (party->field_8 & 8) {
                    party->field_8 &= ~8;
                    party->motionStep = 0;
                    party->flags &= ~PARTY_FLAG_IS_BEING_USED;
                    L_partyForceSlitOff(party);
                }
                partyRideMain(party);
            }
            break;
        case 12:
            if (!(party->flags & PARTY_FLAG_CTRL_OFF) || (party->flags & PARTY_FLAG_RUN_OFF)) {
                if (party->field_8 & 8) {
                    party->field_8 &= ~8;
                    party->motionStep = 0;
                    party->flags &= ~PARTY_FLAG_IS_BEING_USED;
                }
                partyRideYoshiMain(party);
            }
            break;
        case 13:
            if (!(party->flags & PARTY_FLAG_CTRL_OFF) || (party->flags & PARTY_FLAG_RUN_OFF)) {
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
        f32 scrX;
        f32 scrY;
        f32 scrZ;

        marioGetScreenPos(party->position, &scrX, &scrY, &scrZ);
        if (marioChkInScreen((s32)scrX, (s32)scrY)) {
            party->flags2 &= ~0x00800000;
        } else {
            party->flags2 |= 0x00800000;
        }
    }
    party->moveDelta = (Vec){0.0f, 0.0f, 0.0f};
    partyPreDisp(party);
}

s32 partyDoWork(void) {
    PartyEntry* party = partyGetPtr(marioGetPtr()->currPartySlotId[0]);
    f32 speed;

    if (!party) {
        return 0;
    }
    if (party->runMode != 0 && party->runMode != 1 && party->runMode != 2) {
        return 0;
    }
    speed = 0.0f;
    marioGetPtr()->baseSpeed = speed;
    partyChgRunMode(party, 3);
    return 1;
}

void partyHello(PartyEntry* party) {
    if (party->field_8 & 8) {
        party->field_8 &= ~8;
        partyHelloInit(party);
        party->flags &= ~PARTY_FLAG_IS_BEING_USED;
    }
    partyHelloMain(party);
}

void partyHelloInit(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;

    if (mario->currMotionId == 0x16 || mario->currMotionId == 0x18 || mario->currMotionId == 0x19) {
        party->motionStep = 0x1e;
    } else if (party->runMode == 4 || party->runMode == 5) {
        party->flags2 |= 0x10;
        party->scale = (Vec){0.0f, 0.0f, 0.0f};
        party->motionStep = 0;
        party->field_110 = mario->dispDir;
        party->dispDir = mario->dispDir;
        party->position = mario->position;
        party->field_B8 = party->position;
    } else if (party->runMode == 7) {
        party->scale = (Vec){1.0f, 1.0f, 1.0f};
        party->motionStep = 0x14;
        party->field_110 = mario->dispDir;
        party->dispDir = mario->dispDir;
    } else {
        party->scale = (Vec){1.0f, 1.0f, 1.0f};
        party->motionStep = 0xa;
        party->field_110 = mario->dispDir;
        party->dispDir = mario->dispDir;
        party->position = mario->position;
        party->field_B8 = party->position;
    }
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

    switch (party->motionStep) {
        case 0:
            x = player->position.x;
            y = player->position.y;
            z = player->position.z;
            party->position.x = x;
            party->position.y = 0.5f * player->wPlayerCollisionBox.y + y;
            party->position.z = z;
            partyGetAppearPos(party, &party->targetPos);
            party->timer = 16;
            party->targetDist = distABf(x, z, party->targetPos.x, party->targetPos.z);
            party->dir = angleABf(x, z, party->targetPos.x, party->targetPos.z);
            party->speed = party->targetDist / (f32)party->timer;
            {
                f32 half = 0.5f * (f32)party->timer;
                f32 dy = party->targetPos.y - party->position.y;
                f32 q;

                if (__fabsf(dy) < 1.0f) {
                    dy = 10.0f;
                } else if (dy >= 0.0f) {
                    dy += 0.5f * party->height;
                } else {
                    dy = 10.0f;
                }
                q = (dy + dy) / (half * half - half);
                party->jumpPara1 = q * half;
                party->jumpPara2 = -q;
            }
            party->flags |= PARTY_FLAG_IS_JUMPING;
            party->motionStep = 1;
            break;
        case 1:
            partyMoveNoHosei(party);
            if (party->jumpPara1 <= 0.0f) {
                party->flags &= ~PARTY_FLAG_IS_JUMPING;
                party->jumpPara2 = -1.6f;
            }
            if (party->position.y < -2000.0f) {
                party->timer = 0;
                party->motionStep = 2;
                break;
            }
            if (party->groundHit == NULL) {
                f32 t = (f32)(0x10 - party->timer) / 16.0f;
                Vec v = {0.0f, 0.0f, 0.0f};

                v.x = t;
                v.y = t;
                v.z = t;
                party->scale = v;
                if (--party->timer <= 0) {
                    party->timer = 0;
                }
                break;
            }
            party->motionStep = 2;
        case 2:
        {
            f32 t = (f32)(0x10 - party->timer) / 16.0f;
            Vec v = {0.0f, 0.0f, 0.0f};

            v.x = t;
            v.y = t;
            v.z = t;
            party->scale = v;
            if (--party->timer > 0) {
                break;
            }
            party->timer = 0;
            if (party->field_110 >= 90.0f && party->field_110 <= 270.0f) {
                party->field_FC = partyToMovedir(party, 180.0f);
                party->dir = party->field_FC;
                party->dispDir = 180.0f;
                party->field_110 = 180.0f;
            } else {
                party->field_FC = partyToMovedir(party, 0.0f);
                party->dir = party->field_FC;
                party->dispDir = 0.0f;
                party->field_110 = 0.0f;
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
                if (!(party->pose && !strcmp(party->pose, pose))) {
                    party->pose = pose;
                    party->field_20 = 0;
                    party->field_8 |= 0x2000;
                }
            }
            party->flags2 &= ~0x10;
            party->runMode = 1;
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
                if (!(party->pose && !strcmp(party->pose, (char*)index))) {
                    party->pose = (char*)index;
                    party->field_20 = 0;
                    party->field_8 |= 0x2000;
                }
            }
            party->runMode = 1;
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
                if (!(party->pose && !strcmp(party->pose, (char*)index))) {
                    party->pose = (char*)index;
                    party->field_20 = 0;
                    party->field_8 |= 0x2000;
                }
            }
            party->runMode = 0;
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
    MarioMotion v;

    party->motionStep = 10;
    party_dp = &partyDataTbl[party->currentMemberId];
    if (party_dp->init_func) {
        party_dp->init_func(party);
    }
    if (party->flags & PARTY_FLAG_IS_BEING_USED) {
        if (party->currentMemberId == PARTY_MEMBER_VIVIAN) {
            vivianUnhold(party);
            party->motionStep = 0;
        }
        if (party->currentMemberId == PARTY_MEMBER_YOSHI) {
            marioGetoffYoshi();
            party->motionStep = 0;
        }
    }
    v = party->playerPtr->currMotionId;
    if (v == MARIO_MOTION_ROLL || v == MARIO_MOTION_PLANE || v == MARIO_MOTION_SHIP) {
        party->motionStep = 0x14;
    }
    return FALSE;
}

BOOL partyGoodbyeMain(PartyEntry* party) {
    MarioWork* mario = marioGetPtr();
    char* pose;
    s32 index;

    switch (party->motionStep) {
        case 0:
            party_dp = &partyDataTbl[party->currentMemberId];
            party_dp->use_func(party);
            if (party->currentMemberId == PARTY_MEMBER_VIVIAN) {
                if (vivianGetStatus() == 0) {
                    party->motionStep = 10;
                    party->runMode = 8;
                }
                break;
            }
            if (party->currentMemberId == PARTY_MEMBER_YOSHI) {
                if (yoshiGetStatus() == 0) {
                    party->motionStep = 10;
                    party->runMode = 8;
                }
                break;
            }
            party->motionStep = 10;
            party->runMode = 8;
            break;
        case 10: {
            f32 my;
            f32 mx;
            f32 mz;
            f32 px;
            f32 py;
            f32 pz;

            psndSFXOn_3D((const char*)0x2091F, &mario->position);
            mx = mario->position.x;
            party->targetPos.x = mario->position.x;
            my = 0.5f * mario->wPlayerCollisionBox.y + mario->position.y;
            party->targetPos.y = my;
            mz = mario->position.z;
            party->targetPos.z = mario->position.z;
            px = party->position.x;
            pz = party->position.z;
            py = party->position.y;
            party->speed = 4.0f;
            party->jumpPara3 = 1.6f;
            party->targetDist = distABf(px, pz, mx, mz);
            party->dir = angleABf(px, pz, mx, mz);
            party->timer = 15;
            py = my - py; // now the drop to the target height
            party->speed = party->targetDist / (f32)party->timer;
            party->jumpPara1 = (0.5f * (party->jumpPara3 * (f32)party->timer * (f32)party->timer) + py) /
                               (f32)party->timer;
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
                if (!(party->pose && !strcmp(party->pose, pose))) {
                    party->pose = pose;
                    party->field_20 = 0;
                    party->field_8 |= 0x2000;
                }
            }
            party->motionStep = 11;
            break;
        }
        case 11: {
            f32 t;

            party->jumpPara1 -= party->jumpPara3;
            party->position.y += party->jumpPara1;
            movePos(&party->position.x, &party->position.z, party->speed, party->dir);
            party->dispDir = revise360(50.0f + party->dispDir);
            t = (f32)party->timer;
            if (t > 10.0f) {
                t = 10.0f;
            }
            party->scale.x = t / 10.0f;
            party->scale.y = party->scale.x;
            party->scale.z = party->scale.x;
            if (--party->timer < 0) {
                party->motionStep = 12;
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
                if (!(party->pose && !strcmp(party->pose, pose))) {
                    party->pose = pose;
                    party->field_20 = 0;
                    party->field_8 |= 0x2000;
                }
            }
            party->jumpPara1 = 0.0f;
            party->position.y = party->targetPos.y;
            partyChgRunMode(party, 0xe);
            return TRUE;
        case 20:
            partyChgRunMode(party, 0xe);
            return TRUE;
    }
    return FALSE;
}

static inline void partyHelloEnd(PartyEntry* party) {
    MarioWork* player;

    player = party->playerPtr;
    party->scale = (Vec){1.0f, 1.0f, 1.0f};
    party->flags2 &= ~0x10;
    party->field_FC = player->moveDir;
    party->dir = player->directionView;
    party->field_110 = player->dispReqDir;
    party->dispDir = player->dispDir;
    partyClearJumpPara(party);
}


s32 partyDokanInit(PartyEntry* party) {
    MarioWork* player = party->playerPtr;
    char* pose;

    party->flags2 |= 0x10;
    switch (player->field_0x3E) {
        case 2:
        case 3:
            partyPaperOn(party, "p_dokan_y");
            party->field_0x124 = 30.0f;
            party->motionStep = 0;
            break;
        case 0:
            party->scale = (Vec){0.0f, 0.0f, 0.0f};
            // fallthrough
        case 1:
            partyPaperOn(party, "p_dokan_y");
            party->field_0x124 = 30.0f;
            party->motionStep = 0;
            break;
        case 5:
            partyPaperOn(party, "p_dokan_x");
            party->field_0x124 = 26.0f;
            party->motionStep = 0xa;
            party->spinPivot.y = 10.0f;
            break;
        case 4:
            partyPaperOn(party, "p_dokan_x");
            party->field_0x124 = -26.0f;
            party->motionStep = 0xa;
            party->spinPivot.y = 10.0f;
            party->dispRot.x = revise360(180.0f + player->dispRot.x);
            break;
        default:
            break;
    }
    partyChgPaper(party, "PM_D_1A");
    if (!(party->flags2 & 2)) {
#ifdef MUST_MATCH
        // No-op cast: the value is already s8, but the cast changes the object.
        s32 memberId = (s8)party->currentMemberId;
#else
        s32 memberId = party->currentMemberId;
#endif

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
        if (!(party->pose && !strcmp(party->pose, pose))) {
            party->pose = pose;
            party->field_20 = 0;
            party->field_8 |= 0x2000;
        }
    }
    party->flags2 |= 8;
    party->flags2 |= 0x10;
    animPoseSetMaterialLightFlagOff(party->paperAnimGroup, 1);
    party->field_110 = revise360(180.0f + player->dispDir);
    party->dispDir = player->dispDir;
    party->field_FC = player->moveDir;
    party->position = player->position;
    L_partyVivianTailStop(party);
    return 0;
}

s32 partyDokanMain(PartyEntry* party) {
    MarioWork* player = party->playerPtr;
    s32 count;

    switch (party->motionStep) {
        case 0:
            party->position = player->position;
            party->dispDir = revise360(party->dispDir + party->field_0x124);
            party->scale.x += 0.05f;
            if (party->scale.x >= 1.0f) {
                party->scale.x = 1.0f;
            }
            party->scale.y = party->scale.x;
            party->scale.z = party->scale.x;
            break;
        case 10:
            party->dispRot.x += party->field_0x124;
            break;
        case 100:
            switch ((s8)party->field_2E) {
                case 0:
                case 1:
                    partyChgPaper(party, "PM_D_1B");
                    party->timer = 0x12;
                    party->motionStep = 0x65;
                    break;
                case 4:
                    partyChgPaper(party, "PM_D_1B");
                    party->timer = 0x12;
                    party->motionStep = 0x65;
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
            if (--party->timer <= 0) {
                party->motionStep = 0x66;
            }
            break;
        case 102:
            party->scale = (Vec){1.0f, 1.0f, 1.0f};
            party->dispRot = (Vec){0.0f, 0.0f, 0.0f};
            party->spinPivot = (Vec){0.0f, 0.0f, 0.0f};
            party->dispOffset = (Vec){0.0f, 0.0f, 0.0f};
            party->field_110 = player->dispReqDir;
            party->dispDir = party->field_110;
            party->dir = player->directionView;
            party->field_FC = player->directionView;
            partyPaperOff(party);
            animPoseSetMaterialLightFlagOn(party->paperAnimGroup, 1);
            party->flags2 &= ~8;
            party->flags2 &= ~0x10;
            party->flags &= ~PARTY_FLAG_DISABLE_FLOORS;
            party->flags &= ~PARTY_FLAG_FORCE_FALL;
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
                } else if (!(party->flags & PARTY_FLAG_IS_PARTNER)) {
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
                } else if (!(party->flags & PARTY_FLAG_IS_PARTNER)) {
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

    if (party && party->runMode == 0xa) {
        party->motionStep = 0x64;
        party->field_2E = unk;
        party->flags2 &= ~8;
        party->flags2 &= ~0x10;
        party->flags2 &= ~0xC0;
        party->flags &= ~PARTY_FLAG_DISABLE_FLOORS;
        party->flags &= ~PARTY_FLAG_FORCE_FALL;
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
        party->flags |= PARTY_FLAG_IS_RIDING_PLANE;
        partyChgRunMode(party, 0xb);
    }
    party = partyPtrTbl[1];
    if (party) {
        party->flags |= PARTY_FLAG_IS_RIDING_PLANE;
        partyChgRunMode(party, 0xb);
    }
}

void allPartyRideShip(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (party) {
        bomhei_use_cancel();
        party->flags |= PARTY_FLAG_IS_RIDING_SHIP;
        partyChgRunMode(party, 0xb);
    }
    party = partyPtrTbl[1];
    if (party) {
        party->flags |= PARTY_FLAG_IS_RIDING_SHIP;
        partyChgRunMode(party, 0xb);
    }
}

void getRidePos(PartyEntry* party, Vec* pos) {
    MarioWork* mario = party->playerPtr;
    f32 angle;
    f32 sn;
    f32 cs;
    MarioMotion id = mario->currMotionId;

    if (id == MARIO_MOTION_YOSHI) {
        angle = 3.1415927f * toMovedir(mario->dispReqDir) / 180.0f;
        if (yoshiGetStatus() != 2) {
            pos->x = mario->position.x - 10.0f * (f32)sin(angle);
            pos->z = mario->position.z - 10.0f * (f32)cos(angle);
            pos->y = mario->position.y + 20.0f;
        } else {
            pos->x = mario->position.x - 20.0f * (f32)sin(angle);
            pos->z = mario->position.z - 20.0f * (f32)cos(angle);
            pos->y = mario->position.y + 16.0f;
        }
        if (party->currentSlotId != 0) {
            PartyEntry* other = partyGetPtr(marioGetPartyId());
            f32 a = toMovedir2(mario->dispReqDir, 10.0f);

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
        pos->x = mario->position.x;
        pos->z = mario->position.z;
        pos->y = mario->position.y + 40.0f;
    } else if (id == 0x19) {
        f32 a;

        if (N_marioShipGetDir() != 0) {
            a = 0.0f;
        } else {
            a = 180.0f;
        }
        a = toMovedirSimple(a);
        sincosf(&sn, &cs, a);
        pos->x = mario->position.x + 22.0f * sn;
        pos->z = mario->position.z + 22.0f * cs;
        pos->y = mario->position.y;
    } else {
        pos->x = mario->position.x;
        pos->z = mario->position.z;
        pos->y = mario->position.y + 20.0f;
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

    if (party && party->runMode == 0xb) {
        party->motionStep = 0x64;
    }
    party = partyGetPtr(marioGetExtraPartyId());
    if (party && party->runMode == 0xb) {
        party->motionStep = 0x64;
    }
}

void allPartyRideOff2(void) {
    PartyEntry* party = partyGetPtr(marioGetPartyId());

    if (party && party->runMode == 0xb) {
        party->motionStep = 0x65;
    }
    party = partyGetPtr(marioGetExtraPartyId());
    if (party && party->runMode == 0xb) {
        party->motionStep = 0x65;
    }
}

s32 partyRideChk(PartyEntry* party) {
    s32 result = 0;

    if (party == NULL) {
        return 0;
    }
    if (party->runMode == 11) {
        result = 1;
        if (party->motionStep >= 2) {
            result = 2;
        }
    }
    return result;
}

// Raw offsets used: 0x38 ride mode, 0x80 scale, 0x94 ride target Vec, 0x128 distance, 0xF0 vertical assist.
// The `case 22:` leaf (state 22, set by case 21) is needed to balance the switch tree even though
// it shares the default body.
void partyRideMain(PartyEntry* party) {
    MarioWork* player = party->playerPtr;
    Vec ridePos;
    f32 dy;
    f32 tmp;
    f32 t;
    f32 scale;
    f32 px;
    f32 pz;

    switch (party->motionStep) {
        case 0:
            getRidePos(party, &ridePos);
            party->targetPos = ridePos;
            party->targetDist =
                distABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z);
            party->dir =
                angleABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z);
            party->timer = 30;
            party->speed = party->targetDist / (f32)party->timer;
            dy = party->targetPos.y - party->position.y;
            party->jumpPara3 = 0.4f;
            party->jumpPara1 =
                (0.5f *
                     ((f32)party->timer * (party->jumpPara3 * (f32)party->timer)) +
                 dy) /
                (f32)party->timer;
            party->field_0x124 = 1.0f;
            party->flags2 |= 0x10;
            partyChgPoseId(party, 4);
            party->motionStep = 1;
            break;
        case 1:
            party->jumpPara1 = party->jumpPara1 - party->jumpPara3;
            party->position.y = party->position.y + party->jumpPara1;
            movePos(&party->position.x, &party->position.z, party->speed, party->dir);
            party->dispDir = revise360(party->dispDir + party->field_0x124);
            party->field_0x124 += 2.0f;
            {
                f32 cap = 30.0f;
                if (party->field_0x124 >= cap) {
                    party->field_0x124 = cap;
                }
            }
            scale = (f32)party->timer;
            if (scale > 10.0f) {
                scale = 10.0f;
            }
            party->scale.x = scale / 10.0f;
            party->scale.y = party->scale.x;
            party->scale.z = party->scale.x;
            if ((party->timer = party->timer - 1) < 0) {
                party->motionStep = 2;
            }
            break;
        case 2:
            partyChgPoseId(party, 1);
            party->jumpPara1 = 0.0f;
            party->position.y = party->targetPos.y;
            party->scale = (Vec){0.0f,0.0f,0.0f};
            party->flags2 |= 1;
            if (party != NULL) {
                party->flags2 |= 0x100000;
            }
            party->motionStep = 10;
            break;
        case 10:
            party->position = player->position;
            party->field_110 = player->dispReqDir;
            party->field_FC = toMovedir(party->field_110);
            break;
        case 11:
            partyChgPoseId(party, 1);
            party->flags2 |= 1;
            if (party != NULL) {
                party->flags2 |= 0x100000;
            }
            getRidePos(party, &ridePos);
            party->scale = (Vec){0.0f,0.0f,0.0f};
            party->position = ridePos;
            party->jumpPara1 = 0.0f;
            party->field_110 = party->playerPtr->dispReqDir;
            party->field_FC = toMovedir(party->field_110);
            break;
        case 20:
            party->motionStep = 21;
            party->timer = 16;
            party->jumpPara1 = 16.0f;
            party->tail.jump.jumpAccel = 0.5f;
            /* fallthrough */
        case 21:
            party->position.y = party->position.y + party->jumpPara1;
            party->jumpPara1 = party->jumpPara1 + party->tail.jump.jumpAccel;
            if ((party->timer = party->timer - 1) <= 0) {
                party->motionStep = 22;
                party->flags |= PARTY_FLAG_FORCE_FALL;
            }
            break;
        case 200:
            party->flags &= ~0x70000;
            party->dispOffset.y = 0.0f;
            party->scale.x = 1.0f;
            party->scale.y = party->scale.x;
            party->scale.z = party->scale.x;
            if (party != NULL) {
                party->flags2 &= ~0x100000;
            }
            partyChgRunMode(party, 1);
            break;
        case 22:
            break;
        case 100:
        case 101:
            party->flags2 &= ~1;
            px = player->position.x;
            dy = player->position.y;
            pz = player->position.z;
            party->position.x = px;
            party->position.y = 0.5f * player->wPlayerCollisionBox.y + dy;
            party->position.z = pz;
            if (party->motionStep == 100) {
                partyGetAppearPos(party, &party->targetPos);
            } else {
                partyGetAppearPos2(party, &party->targetPos);
            }
            party->field_110 = player->dispReqDir;
            party->timer = 16;
            party->targetDist = distABf(px, pz, party->targetPos.x, party->targetPos.z);
            party->dir = angleABf(px, pz, party->targetPos.x, party->targetPos.z);
            party->speed = party->targetDist / (f32)party->timer;
            dy = party->targetPos.y - party->position.y;
            tmp = 0.5f * (f32)party->timer;
            if (__fabsf(dy) < 1.0f) {
                dy = 10.0f;
            } else if (dy >= 0.0f) {
                dy += 0.5f * party->height;
            } else {
                dy = 10.0f;
            }
            t = (dy + dy) / ((tmp * tmp) - tmp);
            party->jumpPara1 = t * tmp;
            party->jumpPara2 = -t;
            party->flags |= PARTY_FLAG_IS_JUMPING;
            party->flags2 &= ~1;
            if (party != NULL) {
                party->flags2 &= ~0x100000;
            }
            party->motionStep = 110;
            break;
        case 110:
            partyMoveNoHosei(party);
            if (party->jumpPara1 <= 0.0f) {
                party->flags &= ~PARTY_FLAG_IS_JUMPING;
                party->jumpPara2 = -1.6f;
            }
            if (party->position.y < -2000.0f) {
                party->timer = 0;
                party->motionStep = 111;
                break;
            } else if (party->groundHit == 0) {
                Vec vec;
                t = (f32)(16 - party->timer) / 16.0f;
                vec = (Vec){0.0f,0.0f,0.0f};
                vec.x = t;
                vec.y = t;
                vec.z = t;
                party->scale = vec;
                if ((party->timer = party->timer - 1) <= 0) {
                    party->timer = 0;
                }
                break;
            }
            party->motionStep = 111;
            /* fallthrough */
        case 111:
        {
            Vec vec;
            t = (f32)(16 - party->timer) / 16.0f;
            vec = (Vec){0.0f,0.0f,0.0f};
            vec.x = t;
            vec.y = t;
            vec.z = t;
            party->scale = vec;
            if ((party->timer = party->timer - 1) <= 0) {
                party->timer = 0;
                party->field_110 = player->dispReqDir;
                party->dispDir = player->dispReqDir;
                party->field_FC = toMovedirSimple(party->dispDir);
                party->dir = party->field_FC;
                partyClearJumpPara(party);
                partyChgPoseId(party, 1);
                party->flags2 &= ~0x10;
                party->flags &= ~0x70000;
                party->dispOffset.y = 0.0f;
                party->scale.x = 1.0f;
                party->scale.y = party->scale.x;
                party->scale.z = party->scale.x;
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

static inline void partyRideEnd(PartyEntry* party) {
    MarioWork* player;

    player = party->playerPtr;
    {
        Vec* dst = &party->scale;

        dst[0] = (Vec){1.0f, 1.0f, 1.0f};
        dst[1] = (Vec){0.0f, 0.0f, 0.0f};
    }
    party->field_FC = player->moveDir;
    party->dir = player->directionView;
    party->field_110 = player->dispReqDir;
    party->flags &= ~0x70000;
    partyClearJumpPara(party);
}

void party_force_ride_yoshi(PartyEntry* party) {
    Vec pos;

    N_partyForceChgRunMode(party, 0xc, 0xa);
    party->flags &= ~PARTY_FLAG_IS_BEING_USED;
    getRidePos(party, &pos);
    party->position = pos;
    partyChgPoseId(party, 1);
    party->jumpPara1 = 0.0f;
    partyShadowOff(party);
    party->field_110 = party->playerPtr->dispReqDir;
    party->field_FC = toMovedir(party->field_110);
}

void party_ride_yoshi_force_move(void) {
    PartyEntry* party = partyGetPtr(marioGetExtraPartyId());
    Vec pos;

    if (party) {
        getRidePos(party, &pos);
        party->position = pos;
        party->field_110 = party->playerPtr->dispReqDir;
        party->field_FC = toMovedir(party->field_110);
    }
}

// Register-allocation notes:
//  * the constant-index pose reads use byte arithmetic over the flattened table
//    (`partyPoseTbl[id][4]` == `*(char**)((char*)partyPoseTbl + id*0x3c + 0x10)`); the natural
//    `[id][4]` form puts the base in r3 and the index in r0, while this puts them in r0/r3-r4.
// The `player` local must also stay the last callee-saved declaration for r29/r30 to line up.
s32 partyRideYoshiMain(PartyEntry* party) {
    char* pose;
    s32 index;
    s32 lowHp;
    MarioWork* player = party->playerPtr;
    Vec pos;
    Vec appearPos;

    switch (party->motionStep) {
        case 0:
            party->jumpPara3 = 1.6f;
            getRidePos(party, &pos);
            party->targetPos = pos;
            party->targetDist = distABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z);
            party->dir = angleABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z);
            party->timer = 15;
            party->speed = party->targetDist / (f32)party->timer;
            party->jumpPara1 = (0.5f * (party->jumpPara3 * (f32)party->timer * (f32)party->timer) +
                                (party->targetPos.y - party->position.y)) /
                               (f32)party->timer;
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
                    pose = yoshiDotPoseTbl[pouchGetPartyColor(PARTNER_YOSHI)][4];
                }
                if (!(party->pose && !strcmp(party->pose, pose))) {
                    party->pose = pose;
                    party->field_20 = 0;
                    party->field_8 |= 0x2000;
                }
            }
            party->motionStep = 1;
            break;
        case 1:
            getRidePos(party, &pos);
            party->targetPos = pos;
            party->targetDist = distABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z);
            party->dir = angleABf(party->position.x, party->position.z, party->targetPos.x, party->targetPos.z);
            party->speed = party->targetDist / (f32)party->timer;
            party->jumpPara1 = party->jumpPara1 - party->jumpPara3;
            party->position.y = party->position.y + party->jumpPara1;
            movePos(&party->position.x, &party->position.z, party->speed, party->dir);
            if (--party->timer <= 0) {
                party->motionStep = 2;
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
                if (!(party->pose && !strcmp(party->pose, pose))) {
                    party->pose = pose;
                    party->field_20 = 0;
                    party->field_8 |= 0x2000;
                }
            }
            party->jumpPara1 = 0.0f;
            party->position.y = party->targetPos.y;
            party->motionStep = 10;
            partyShadowOff(party);
        }
            break;
        case 10:
            getRidePos(party, &pos);
            party->position = pos;
            party->field_110 = player->dispReqDir;
            party->field_FC = toMovedir(party->field_110);
            if (player->currMotionId != 0x1A) {
                partyGetAppearPos(party, &appearPos);
                party->position.x = appearPos.x;
                party->position.z = appearPos.z;
                partyChgRunMode(party, 2);
                if (party) {
                    party->flags2 &= ~0x100000;
                }
                party->dispOffset = (Vec){0.0f, 0.0f, 0.0f};
            }
            break;
    }
    return 0;
}

void party_damage_return(PartyEntry* party) {
    MarioWork* mario = party->playerPtr;
    f32 partyX;
    f32 partyY;
    f32 partyZ;
    f32 marioX;
    f32 marioY;
    f32 marioZ;

    if (party->field_8 & 8) {
        party->field_8 &= ~8;
        party->flags &= ~PARTY_FLAG_IS_BEING_USED;
        if (!(party->field_8 & 0x1000000)) {
            party->motionStep = 10;
        } else {
            party->motionStep = 0;
            party->timer = 0x64;
        }
    }
    switch (party->motionStep) {
        case 0:
            if (--party->timer <= 0) {
                party->motionStep = 10;
            } else {
                break;
            }
            // fall through
        case 10:
            partyChgPoseId(party, 8);
            party->timer = sysMsec2Frame(0xc8);
            party->motionStep = 11;
            /* fall through */
        case 11:
            if (--party->timer <= 0) {
                party->jumpPara1 = 15.0f;
                party->jumpPara2 = 0.0f;
                party->motionStep = 12;
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
                party->timer = 0x20;
                party->motionStep = 0x14;
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
            party->motionStep = 0x15;
            // fall through
        }
        case 21: {
            Vec pos;

            if (--party->timer <= 0) {
                partyGetAppearPos(party, &pos);
                party->position.x = pos.x;
                party->position.z = pos.z;
                party->jumpPara1 = 0.0f;
                party->jumpPara2 = -1.0f;
                party->speed = 0.0f;
                party->flags |= PARTY_FLAG_IS_FALLING;
                party->flags |= PARTY_FLAG_FORCE_FALL;
                party->motionStep = 0x16;
            }
            break;
        }
        case 22: {
            marioGetScreenPos(party->position, &partyX, &partyY, &partyZ);
            if (partyY > 240.0f) {
                party->flags &= ~PARTY_FLAG_FORCE_FALL;
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
            if (party->groundHit != NULL) {
                if ((hitGetAttr(party->groundHit) >> 9 & 1) == 0) {
                    partyChgPoseId(party, 9);
                    party->motionStep = 0x17;
                    party->timer = 0x1e;
                }
            }
            break;
        }
        case 23:
            if (--party->timer <= 0) {
                partyChgRunMode(party, 2);
                party->moveMode = 0;
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
                partyPtrTbl[1] = __memAlloc(HEAP_DEFAULT, sizeof(PartyEntry));
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
        entry->flags |= PARTY_FLAG_IS_FOLLOWER;
        party_dp = &partyDataTbl[memberId];
        if (entry->currentMemberId == PARTY_YOSHI) {
                animName = yoshigroup[pouchGetPartyColor(PARTNER_YOSHI)];
                if ((entry->flags2 & 0x8000)) {
                        animName = "d_mario";
                }
        } else if ((entry->flags2 & 0x8000)) {
                animName = "d_mario";
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

// A named `want` cache orders the 0.0 compare first (the plain field_110 read form scores 99.38%).
// Caching dispDir in `angle` (reused after the facing test; nothing writes dispDir in between) gives the
// original arithmetic and clamp FPR allocation (dispDir in f2, field_110 in f3, the clamp reloads in f1);
// a separate local for it puts dispDir in f3, and without the cache CSE raises the arithmetic to f3.
void partyMakeDispDir(PartyEntry* party) {
    f32 angle;
    f32 target;
    f32 want;

    if (party->flags2 & 0x100) {
        return;
    }
    if (!(party->flags2 & 0x10)) {
        angle = revise360(party->field_FC - party->playerTargetYaw);
        target = party->field_110;
        if (!(party->flags2 & 0x100)) {
            if (290.0f == target || 250.0f == target) {
                target = 180.0f;
                if (angle >= target) {
                    target = 0.0f;
                }
            }
        }
        if (party->speed) {
            if (angle >= 190.0f && angle <= 350.0f) {
                target = 0.0f;
            } else if (angle >= 10.0f && angle <= 170.0f) {
                target = 180.0f;
            }
        }
        party->field_110 = target;
        if (party->flags2 & 0x20) {
            party->dispDir = party->field_110;
        }
        if (party->dispDir == party->field_110) {
            return;
        }
    }
    angle = party->dispDir;
    if (angle <= 180.0f) {
        if (0.0f == (want = party->field_110)) {
            f32 t = (party->field_8 & 0x1000) ? 20.0f : -40.0f;
            f32 s;

            s = 0.125f * (t - angle);
            party->dispDir = angle + s;
            if (!(party->field_8 & 0x1000)) {
                if (party->dispDir < -30.0f) {
                    party->field_8 |= 0x1000;
                }
            } else if (party->dispDir >= 0.0f) {
                party->dispDir = 0.0f;
                party->field_8 &= ~0x1000;
            }
        } else if (180.0f == want) {
            f32 t = (party->field_8 & 0x1000) ? 170.0f : 220.0f;
            f32 s;

            s = 0.125f * (t - angle);
            party->dispDir = angle + s;
            if (!(party->field_8 & 0x1000)) {
                if (party->dispDir > 210.0f) {
                    party->field_8 |= 0x1000;
                }
            } else if (party->dispDir <= 180.0f) {
                party->dispDir = 180.0f;
                party->field_8 &= ~0x1000;
            }
        }
    } else if (angle > 180.0f) {
        if (0.0f == (want = party->field_110)) {
            f32 t = (party->field_8 & 0x1000) ? 20.0f : -40.0f;
            f32 s;

            s = 0.125f * (t - angle);
            party->dispDir = angle + s;
            if (!(party->field_8 & 0x1000)) {
                if (party->dispDir < -30.0f) {
                    party->field_8 |= 0x1000;
                }
            } else if (party->dispDir >= 0.0f) {
                party->dispDir = 0.0f;
                party->field_8 &= ~0x1000;
            }
        } else if (180.0f == want) {
            f32 t = (party->field_8 & 0x1000) ? 170.0f : 220.0f;
            f32 s;

            s = 0.125f * (t - angle);
            party->dispDir = angle + s;
            if (!(party->field_8 & 0x1000)) {
                if (party->dispDir > 210.0f) {
                    party->field_8 |= 0x1000;
                } else if (party->dispDir <= 180.0f) {
                    party->dispDir = 180.0f;
                    party->field_8 &= ~0x1000;
                }
            }
        }
    }
}

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
        animPoseSetAnim(party->paperAnimGroup, party->pose, 1);
        if (party->paper != NULL) {
            animPoseSetPaperAnim(party->paperAnimGroup, party->paper, 1);
        }
        animPoseSetLocalTime(party->paperAnimGroup, (f32)party->field_20);
    }

    animPoseMain(party->paperAnimGroup);

    // The original wraps the whole tail (disp entry, shadow setup, vivian, shadow entry) in this arm:
    // the target's `bne` at `flags2 & 1` jumps straight to the epilogue.
    if (!(party->flags2 & 1)) {
        dispPos = party->position;
        PSMTX44MultVec(camGetPtr(CAMERA_3D)->projection, &dispPos, &dispPos);
        order = 5000.0f * dispPos.z + 5000.0f;
        if (order < 0.0f) {
            order = 0.0f;
        } else if (order > 10000.0f) {
            order = 10000.0f;
        }
        order *= -1.0f;
        if (party->mutekiTime > 0) {
            if (!(party->mutekiTime & 2)) {
                dispEntry((CameraId)party->camId, order, 1, partyDisp, party);
            }
        } else {
            dispEntry((CameraId)party->camId, order, 1, partyDisp, party);
        }

        shadowMode = 2;
        size = 13.333333f;
        if (party->currentMemberId != PARTY_MEMBER_VIVIAN) {
            party->field_B8 = party->position;
        } else {
            if (party->runMode == 4 || (party->runMode == 8 && !(party->flags & PARTY_FLAG_IS_BEING_USED))) {
                party->field_B8 = party->position;
            }
            if (party->flags & PARTY_FLAG_IS_BEING_USED) {
                size = party->tail.vivian.charge;
                if (party->flags & PARTY_FLAG_IS_EVT_USE) {
                    shadowMode = 1;
                }
            }
        }

        if (N_vivianDisplayTailCheck(party)) {
            dist = 1000.0f;
            hitCheckPos = party->position;
            hitCheckPos.y += 10.0f;
            dir = (Vec){0.0f, -1.0f, 0.0f};
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
            party->spinPivot.x = -6.8f;
            party->spinPivot.y = 6.8f;
        }

        scale = (party->flags & PARTY_FLAG_BG_MODE) ? 1.2f : 2.0f;
        PSMTXScale(scaleMtx, party->scale.x * scale, party->scale.y * scale, party->scale.z * scale);

        if (!(party->flags2 & 8)) {
            angle = revise360(party->dispDir);
            PSMTXScale(rotMtx, 1.0f, 1.0f,
                       (angle >= 90.0f && angle <= 270.0f) ? -1.0f
                                                                                  : 1.0f);
            PSMTXConcat(rotMtx, scaleMtx, scaleMtx);
        }

        PSMTXTrans(transMtx, -party->spinPivot.x, -party->spinPivot.y, -party->spinPivot.z);
        PSMTXConcat(transMtx, scaleMtx, scaleMtx);

        PSMTXRotRad(transMtx, 'z', 0.017453292f * party->dispRot.z);
        PSMTXConcat(transMtx, scaleMtx, scaleMtx);

        PSMTXRotRad(transMtx, 'x', 0.017453292f * party->dispRot.x);
        PSMTXConcat(transMtx, scaleMtx, scaleMtx);

        angle = revise360(party->dispDir - party->playerTargetYaw);
        PSMTXRotRad(transMtx, 'y', 0.017453292f * angle);
        PSMTXConcat(transMtx, scaleMtx, scaleMtx);

        if (party->flags2 & 0xC0) {
            angle = revise360(party->dispDir);
            if (angle >= 90.0f && angle <= 270.0f) {
                PSMTXRotRad(transMtx, 'y',
                            0.017453292f * ((party->flags2 & 0x40) ? 180.0f : 0.0f));
                PSMTXConcat(transMtx, scaleMtx, scaleMtx);
            } else {
                PSMTXRotRad(transMtx, 'y',
                            0.017453292f * ((party->flags2 & 0x40) ? 0.0f : 180.0f));
                PSMTXConcat(transMtx, scaleMtx, scaleMtx);
            }
        }

        PSMTXTrans(transMtx, party->spinPivot.x, party->spinPivot.y, party->spinPivot.z);
        PSMTXConcat(transMtx, scaleMtx, scaleMtx);

        yOffset = 0.0f;
        if (party->paper != NULL) {
            if (party->currentMemberId == PARTY_MEMBER_YOSHI) {
                yOffset = 1.0f;
            } else if (party->currentMemberId == PARTY_MEMBER_BOBBERY) {
                yOffset = 2.0f;
            } else if (party->currentMemberId == PARTY_MEMBER_MS_MOWZ) {
                yOffset = 2.0f;
            } else if (party->currentMemberId == PARTY_MEMBER_GOOMBELLA) {
                yOffset = 2.0f;
            } else if (party->currentMemberId == PARTY_MEMBER_FRANKLY) {
                yOffset = 3.0f;
            } else if (party->currentMemberId == PARTY_MEMBER_EGG) {
                yOffset = 3.5f;
            } else if (party->currentMemberId == PARTY_MEMBER_PUNIO) {
                yOffset = 4.0f;
            } else if (party->currentMemberId == PARTY_MEMBER_FLAVIO) {
                yOffset = -1.0f;
            }
        }

        PSMTXTrans(transMtx, party->position.x + party->dispOffset.x,
                   party->position.y + party->dispOffset.y + yOffset, party->position.z + party->dispOffset.z);
        PSMTXConcat(transMtx, scaleMtx, scaleMtx);

        animPoseDrawMtx(party->paperAnimGroup, scaleMtx, 2, 0.0f, 1.0f);
        animPoseDrawMtx(party->paperAnimGroup, scaleMtx, 1, 0.0f, 1.0f);
        animPoseSetLocalTimeRate(party->paperAnimGroup,
                                 (party->flags & PARTY_FLAG_IN_SHALLOW_WATER) ? 0.75f : 1.0f);
    }
}

void partyKeyOn(void) {}

void partyKeyOff(void) {}

void L_partyClearCont(PartyEntry* party) {
    party->stickX = 0;
    party->stickY = 0;
    party->substickX = 0;
    party->substickY = 0;
    party->triggerLeft = 0;
    party->triggerRight = 0;
    party->field_0x15E = 0;
    party->field_0x15F = 0;
    party->button = 0;
    party->buttonNew = 0;
    party->buttonRepeat = 0;
    party->buttonUp = 0;
}

void partyUpdateKeyData(PartyEntry* party) {
    s32 member = party->field_30;

    if (party->flags & 0x60000000) {
        party->stickX = 0;
        party->stickY = 0;
        party->substickX = 0;
        party->substickY = 0;
        party->triggerLeft = 0;
        party->triggerRight = 0;
        party->field_0x15E = 0;
        party->field_0x15F = 0;
        party->button = 0;
        party->buttonNew = 0;
        party->buttonRepeat = 0;
        party->buttonUp = 0;
        return;
    }
    party->stickX = gp->stickX[member];
    party->stickY = -gp->stickY[member];
    party->substickX = gp->substickX[member];
    party->substickY = -gp->substickY[member];
    party->triggerLeft = gp->triggerLeft[member];
    party->triggerRight = gp->triggerRight[member];
    party->field_0x15E = 0;
    party->field_0x15F = 0;
    party->button = gp->button[member];
    party->buttonNew = gp->buttonNew[member];
    party->buttonRepeat = gp->buttonRepeat[member];
    party->buttonUp = gp->buttonUp[member];
}

void partyShadowOff(PartyEntry* party) {
    if (party) {
        party->flags2 |= 0x100000;
    }
}

BOOL partyChkHerohero(PartyEntry* party) {
    BOOL ret = FALSE;

    if (party->currentMemberId < 8) {
        if (partyGetHp(party->currentMemberId) <= 5) {
            ret = TRUE;
        }
    }
    return ret;
}

void partySetFamicomMode(PartyEntry* party, s32 mode) {
    if (mode == 1) {
        if (party->flags2 & 0x8000) {
            return;
        }
        L_partyForceSlitOff(party);
        party->flags2 |= 0x8000;
        partyPoseEntry(party);
        partyChgPose(party, partyChgPoseId(party, 1));
    } else {
        if (!(party->flags2 & 0x8000)) {
            return;
        }
        party->flags2 &= ~0x8000;
        partyPoseEntry(party);
        partyChgPose(party, partyChgPoseId(party, 1));
    }
}

// 4-byte zero word that ends the unit's .sdata: all-zero objects need explicit_zero_data to be emitted and
// force_active to survive the linker.
#pragma explicit_zero_data on
#pragma force_active on
__declspec(section ".sdata") u32 gap_07_8041061C_sdata = 0;
#pragma force_active reset
#pragma explicit_zero_data reset
