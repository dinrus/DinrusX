// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"
#include "CentralEventUpr.h"


CDrxSignal<void(const CCentralEventUpr::SQueryBlueprintRuntimeParamsChangedEventArgs&)> CCentralEventUpr::QueryBlueprintRuntimeParamsChanged;
CDrxSignal<void(const CCentralEventUpr::SStartOrStopQuerySimulatorEventArgs&)> CCentralEventUpr::StartOrStopQuerySimulator;
CDrxSignal<void(const CCentralEventUpr::SSingleStepQuerySimulatorOnceEventArgs&)> CCentralEventUpr::SingleStepQuerySimulatorOnce;
CDrxSignal<void(const CCentralEventUpr::SQuerySimulatorRunningStateChangedEventArgs&)> CCentralEventUpr::QuerySimulatorRunningStateChanged;
