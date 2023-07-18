// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/ILog.h>
#include <DinrusX/DinrusXSchema/Delegate.h>

namespace Schematyc
{
typedef std::function<EVisitStatus (const SLogMessageData&)> LogMessageVisitor;

struct ILogRecorder
{
	virtual ~ILogRecorder() {}

	virtual void Begin() = 0;
	virtual void End() = 0;
	virtual void VisitMessages(const LogMessageVisitor& visitor) = 0;
	virtual void Clear() = 0;
};
} // Schematyc
