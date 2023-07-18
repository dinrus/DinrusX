// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IScriptElement.h>

namespace Schematyc
{
// Forward declare structures.


enum class EScriptSignalReceiverType
{
	Unknown = 0,
	EnvSignal,
	ScriptSignal,
	ScriptTimer,
	Universal // #SchematycTODO : All signal receivers should be universal.
};

struct IScriptSignalReceiver : public IScriptElementBase<EScriptElementType::SignalReceiver>
{
	virtual ~IScriptSignalReceiver() {}

	virtual EScriptSignalReceiverType GetSignalReceiverType() const = 0;
	virtual DrxGUID                     GetSignalGUID() const = 0;
};
} // Schematyc
