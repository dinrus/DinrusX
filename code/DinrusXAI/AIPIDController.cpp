// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXAI/StdAfx.h>
#include <DinrusX/DinrusXAI/AIPIDController.h>
void CAIPIDController::Serialize(TSerialize ser)
{
	ser.BeginGroup("AIPIDController");
	ser.Value("CP", CP);
	ser.Value("CI", CI);
	ser.Value("CD", CD);
	ser.Value("runningIntegral", runningIntegral);
	ser.Value("lastError", lastError);
	ser.Value("proportionalPower", proportionalPower);
	ser.Value("integralTimescale", integralTimescale);
	ser.EndGroup();
}
