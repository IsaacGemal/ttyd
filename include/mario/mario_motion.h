#include "dolphin/types.h"
#include "mario/mario.h"

void marioChgMot(s32 motion);
BOOL marioChkItemMotion(void);
BOOL marioChkJump(void);
BOOL marioChkTransform(void);

void marioChgStayMotion(void);
void marioChgShipMotion(void);
f32 marioMakeJumpPara(void);
void marioChgTalkMotion(void);
void marioClearJumpPara(void);
void marioChgJumpStandMotion(f32 a);
void marioChgGetItemMotion(void);
f32 marioGetFallSpd(void);
void marioLandOn(void);
void marioSetFallPara(void);
void marioBoots(void);
void marioMotion(void);
void marioFall(void);
void marioJump(void);
void marioChgMotSub(s32 motion, s32 sub);
void marioChgMot2(s32 motion);
s32 marioRollChgChk(void);
BOOL marioChkTalkable(void);
void marioSetJumpPara(void);
