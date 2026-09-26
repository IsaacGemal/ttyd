#include "battle/ac/ac_table.h"
#include "battle/ac/ac_timing_a.h"
#include "battle/ac/ac_button_down.h"
#include "battle/ac/ac_air_gauge.h"

// Prototypes for the action-command files that have no header yet; the signatures match
// ActionCommandEntry's callback fields.
s32 battleAcMain_TimingWide(BattleWork* work);
s32 battleAcResult_TimingWide(BattleWork* work);
void battleAcDisp_TimingWide(CameraId camId, void* param);
void battleAcDelete_TimingWide(BattleWork* work);

s32 battleAcMain_StickKeepLeft(BattleWork* work);
s32 battleAcResult_StickKeepLeft(BattleWork* work);
void battleAcDisp_StickKeepLeft(CameraId camId, void* param);
void battleAcDelete_StickKeepLeft(BattleWork* work);

s32 battleAcMain_LstRcKeep(BattleWork* work);
s32 battleAcResult_LstRcKeep(BattleWork* work);
void battleAcDisp_LstRcKeep(CameraId camId, void* param);
void battleAcDelete_LstRcKeep(BattleWork* work);

s32 battleAcMain_Repeatedly(BattleWork* work);
s32 battleAcResult_Repeatedly(BattleWork* work);
void battleAcDisp_Repeatedly(CameraId camId, void* param);
void battleAcDelete_Repeatedly(BattleWork* work);

s32 battleAcMain_RepeatedlyLv(BattleWork* work);
s32 battleAcResult_RepeatedlyLv(BattleWork* work);
void battleAcDisp_RepeatedlyLv(CameraId camId, void* param);
void battleAcDelete_RepeatedlyLv(BattleWork* work);

s32 battleAcMain_Shot(BattleWork* work);
s32 battleAcResult_Shot(BattleWork* work);
void battleAcDisp_Shot(CameraId camId, void* param);
void battleAcDelete_Shot(BattleWork* work);

s32 battleAcMain_ShotTarget(BattleWork* work);
s32 battleAcResult_ShotTarget(BattleWork* work);
void battleAcDisp_ShotTarget(CameraId camId, void* param);
void battleAcDelete_ShotTarget(BattleWork* work);

s32 battleAcMain_StickRotate(BattleWork* work);
s32 battleAcResult_StickRotate(BattleWork* work);
void battleAcDisp_StickRotate(CameraId camId, void* param);
void battleAcDelete_StickRotate(BattleWork* work);


s32 battleAcMain_PendulumCraneTiming(BattleWork* work);
s32 battleAcResult_PendulumCraneTiming(BattleWork* work);
void battleAcDisp_PendulumCraneTiming(CameraId camId, void* param);
void battleAcDelete_PendulumCraneTiming(BattleWork* work);

s32 battleAcMain_CraneTiming(BattleWork* work);
s32 battleAcResult_CraneTiming(BattleWork* work);
void battleAcDisp_CraneTiming(CameraId camId, void* param);
void battleAcDelete_CraneTiming(BattleWork* work);


s32 battleAcMain_PowerGauge(BattleWork* work);
s32 battleAcResult_PowerGauge(BattleWork* work);
void battleAcDisp_PowerGauge(CameraId camId, void* param);
void battleAcDelete_PowerGauge(BattleWork* work);

s32 battleAcMain_PowerGaugeLv(BattleWork* work);
s32 battleAcResult_PowerGaugeLv(BattleWork* work);
void battleAcDisp_PowerGaugeLv(CameraId camId, void* param);
void battleAcDelete_PowerGaugeLv(BattleWork* work);

s32 battleAcMain_PowerGaugeLv2(BattleWork* work);
s32 battleAcResult_PowerGaugeLv2(BattleWork* work);
void battleAcDisp_PowerGaugeLv2(CameraId camId, void* param);
void battleAcDelete_PowerGaugeLv2(BattleWork* work);

s32 battleAcMain_SignalTiming(BattleWork* work);
s32 battleAcResult_SignalTiming(BattleWork* work);
void battleAcDisp_SignalTiming(CameraId camId, void* param);
void battleAcDelete_SignalTiming(BattleWork* work);

s32 battleAcMain_GaugeTiming(BattleWork* work);
s32 battleAcResult_GaugeTiming(BattleWork* work);
void battleAcDisp_GaugeTiming(CameraId camId, void* param);
void battleAcDelete_GaugeTiming(BattleWork* work);

s32 battleAcMain_GaugeTiming2(BattleWork* work);
s32 battleAcResult_GaugeTiming2(BattleWork* work);
void battleAcDisp_GaugeTiming2(CameraId camId, void* param);
void battleAcDelete_GaugeTiming2(BattleWork* work);

s32 battleAcMain_MonosiriTarget(BattleWork* work);
s32 battleAcResult_MonosiriTarget(BattleWork* work);
void battleAcDisp_MonosiriTarget(CameraId camId, void* param);
void battleAcDelete_MonosiriTarget(BattleWork* work);

ActionCommandEntry ActionCommandList[] = {
	{1, battleAcMain_TimingA, battleAcResult_TimingA, battleAcDisp_TimingA, battleAcDelete_TimingA},
	{11, battleAcMain_TimingWide, battleAcResult_TimingWide, battleAcDisp_TimingWide, battleAcDelete_TimingWide},
	{3, battleAcMain_StickKeepLeft, battleAcResult_StickKeepLeft, battleAcDisp_StickKeepLeft, battleAcDelete_StickKeepLeft},
	{4, battleAcMain_LstRcKeep, battleAcResult_LstRcKeep, battleAcDisp_LstRcKeep, battleAcDelete_LstRcKeep},
	{5, battleAcMain_Repeatedly, battleAcResult_Repeatedly, battleAcDisp_Repeatedly, battleAcDelete_Repeatedly},
	{6, battleAcMain_RepeatedlyLv, battleAcResult_RepeatedlyLv, battleAcDisp_RepeatedlyLv, battleAcDelete_RepeatedlyLv},
	{7, battleAcMain_Shot, battleAcResult_Shot, battleAcDisp_Shot, battleAcDelete_Shot},
	{8, battleAcMain_ShotTarget, battleAcResult_ShotTarget, battleAcDisp_ShotTarget, battleAcDelete_ShotTarget},
	{9, battleAcMain_StickRotate, battleAcResult_StickRotate, battleAcDisp_StickRotate, battleAcDelete_StickRotate},
	{10, battleAcMain_ButtonDown, battleAcResult_ButtonDown, battleAcDisp_ButtonDown, battleAcDelete_ButtonDown},
	{12, battleAcMain_PendulumCraneTiming, battleAcResult_PendulumCraneTiming, battleAcDisp_PendulumCraneTiming, battleAcDelete_PendulumCraneTiming},
	{13, battleAcMain_CraneTiming, battleAcResult_CraneTiming, battleAcDisp_CraneTiming, battleAcDelete_CraneTiming},
	{14, battleAcMain_AirGauge, battleAcResult_AirGauge, battleAcDisp_AirGauge, battleAcDelete_AirGauge},
	{15, battleAcMain_PowerGauge, battleAcResult_PowerGauge, battleAcDisp_PowerGauge, battleAcDelete_PowerGauge},
	{16, battleAcMain_PowerGaugeLv, battleAcResult_PowerGaugeLv, battleAcDisp_PowerGaugeLv, battleAcDelete_PowerGaugeLv},
	{17, battleAcMain_PowerGaugeLv2, battleAcResult_PowerGaugeLv2, battleAcDisp_PowerGaugeLv2, battleAcDelete_PowerGaugeLv2},
	{18, battleAcMain_SignalTiming, battleAcResult_SignalTiming, battleAcDisp_SignalTiming, battleAcDelete_SignalTiming},
	{19, battleAcMain_GaugeTiming, battleAcResult_GaugeTiming, battleAcDisp_GaugeTiming, battleAcDelete_GaugeTiming},
	{20, battleAcMain_GaugeTiming2, battleAcResult_GaugeTiming2, battleAcDisp_GaugeTiming2, battleAcDelete_GaugeTiming2},
	{21, battleAcMain_MonosiriTarget, battleAcResult_MonosiriTarget, battleAcDisp_MonosiriTarget, battleAcDelete_MonosiriTarget},
	{0, NULL, NULL, NULL, NULL}
};
