// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSys/TimeValue.h>

#include <DinrusX/DinrusXSchema/FundamentalTypes.h>
#include <DinrusX/DinrusXSchema/Delegate.h>
#include <DinrusX/DinrusXSchema/GUID.h>

namespace Schematyc
{
struct IScriptElement;

typedef std::function<EVisitStatus (IScriptElement&)> ScriptElementVisitor;

struct IScript
{
	virtual ~IScript() {}

	virtual DrxGUID             GetGUID() const = 0;

	virtual const char*       SetFilePath(const char* szFilePath) = 0;
	virtual const char*       GetFilePath() const = 0;

	virtual const CTimeValue& GetTimeStamp() const = 0;

	virtual void              SetRoot(IScriptElement* pRoot) = 0; // #SchematycTODO : Does this really need to be part of the public interface?
	virtual IScriptElement*   GetRoot() = 0;

	virtual EVisitStatus      VisitElements(const ScriptElementVisitor& visitor) = 0;

};
} // Schematyc
