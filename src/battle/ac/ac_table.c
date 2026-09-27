#include "battle/ac/ac_table.h"
#include "battle/ac/ac_timing_a.h"
#include "battle/ac/ac_button_down.h"
#include "battle/ac/ac_air_gauge.h"
#include "battle/ac/ac_timing_wide.h"
#include "battle/ac/ac_power_gauge_lv.h"
#include "battle/ac/ac_stick_keep_left.h"
#include "battle/ac/ac_lst_rc_keep.h"
#include "battle/ac/ac_repeatedly.h"
#include "battle/ac/ac_repeatedly_lv.h"
#include "battle/ac/ac_shot.h"
#include "battle/ac/ac_shot_target.h"
#include "battle/ac/ac_stick_rotate.h"
#include "battle/ac/ac_pendulum_crane_timing.h"
#include "battle/ac/ac_crane_timing.h"
#include "battle/ac/ac_power_gauge.h"
#include "battle/ac/ac_power_gauge_lv2.h"
#include "battle/ac/ac_signal_timing.h"
#include "battle/ac/ac_gauge_timing.h"
#include "battle/ac/ac_gauge_timing2.h"
#include "battle/ac/ac_monosiri_target.h"

// Prototypes for the action-command files that have no header yet; the signatures match
// ActionCommandEntry's callback fields.



















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
