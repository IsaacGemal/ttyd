#include "dolphin/types.h"
#include "mario/mario.h"

void marioChgMot(s32 motion);
BOOL marioChkItemMotion(void);
BOOL marioChkJump(void);
BOOL marioChkTransform(void);

void marioChgMoveMotion(void);
void marioChgStayMotion(void);
void marioChgShipMotion(void);
f32 marioMakeJumpPara(void);
void marioChgTalkMotion(void);
void marioClearJumpPara(void);
void marioChgJumpStandMotion(f32 a);
void marioChgRollMotion(void);
void marioChgGetItemMotion(void);
f32 marioGetFallSpd(void);
void marioLandOn(void);
void marioSetFallPara(void);
void marioChgSmallJumpMotion(void);
void marioBoots(void);
void marioMotion(void);
void marioFall(void);
void marioJump(void);
void marioChgMotSub(s32 motion, s32 sub);
void marioChgMot2(s32 motion);
s32 marioRollChgChk(void);
BOOL marioChkTalkable(void);
void marioSetJumpPara(void);
s32 L_marioChkRub(void);
void mot_talk(void);
void mot_getItem(void);
void mot_kaze(void);
void mot_shadow(void);
void mot_partyUse(void);
void mot_dummy(void);
void mot_bottomless(void);  // TODO: real signature
void mot_forceReset(void);
void marioChgMotJump2(void);
BOOL marioChkItemGetMotion(void);
