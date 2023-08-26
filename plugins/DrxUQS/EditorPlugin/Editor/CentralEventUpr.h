// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

<DinrusX/CoreX/Sandbox/DrxSignal.h>


namespace UQSEditor
{
	class CQueryBlueprint;
};
class CSimulatedRuntimeParam;


class CCentralEventUpr
{
public:

	struct SQueryBlueprintRuntimeParamsChangedEventArgs
	{
		explicit SQueryBlueprintRuntimeParamsChangedEventArgs(UQSEditor::CQueryBlueprint& _affectedQueryBlueprint)
			: affectedQueryBlueprint(_affectedQueryBlueprint)
		{}

		UQSEditor::CQueryBlueprint& affectedQueryBlueprint;
	};

	struct SStartOrStopQuerySimulatorEventArgs
	{
		explicit SStartOrStopQuerySimulatorEventArgs(const char* _szQueryBlueprintName, const std::vector<CSimulatedRuntimeParam>& _simulatedParams, bool _bRunInfinitely)
			: queryBlueprintName(_szQueryBlueprintName)
			, simulatedParams(_simulatedParams)
			, bRunInfinitely(_bRunInfinitely)
		{}

		string queryBlueprintName;
		const std::vector<CSimulatedRuntimeParam>& simulatedParams;
		bool bRunInfinitely;  // whether the query should be run infinitely each time it finishes or only once
	};

	struct SSingleStepQuerySimulatorOnceEventArgs
	{
		// FIXME: queryBlueprintName is only needed for when actually initiating the single-step mode, but no longer afterwards
		explicit SSingleStepQuerySimulatorOnceEventArgs(const char* _szQueryBlueprintName, const std::vector<CSimulatedRuntimeParam>& _simulatedParams, bool _bRunInfinitely)
			: queryBlueprintName(_szQueryBlueprintName)
			, simulatedParams(_simulatedParams)
			, bRunInfinitely(_bRunInfinitely)
		{}

		string queryBlueprintName;
		const std::vector<CSimulatedRuntimeParam>& simulatedParams;
		bool bRunInfinitely;
	};

	struct SQuerySimulatorRunningStateChangedEventArgs
	{
		explicit SQuerySimulatorRunningStateChangedEventArgs(bool _bIsRunningNow)
			: bIsRunningNow(_bIsRunningNow)
		{}

		bool bIsRunningNow;
	};

public:

	static CDrxSignal<void(const SQueryBlueprintRuntimeParamsChangedEventArgs&)> QueryBlueprintRuntimeParamsChanged;
	static CDrxSignal<void(const SStartOrStopQuerySimulatorEventArgs&)> StartOrStopQuerySimulator;
	static CDrxSignal<void(const SSingleStepQuerySimulatorOnceEventArgs&)> SingleStepQuerySimulatorOnce;
	static CDrxSignal<void(const SQuerySimulatorRunningStateChangedEventArgs&)> QuerySimulatorRunningStateChanged;

};
