// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/CompilerContext.h>

namespace Schematyc
{
class CCompilerTaskList : public ICompilerTaskList
{
public:

	typedef std::vector<const IScriptGraph*> PendingGraphs;

public:

	CCompilerTaskList();

	// ICompilerTaskList
	virtual void CompileGraph(const IScriptGraph& scriptGraph) override;
	// ~ICompilerTaskList

	PendingGraphs& GetPendingGraphs();

private:

	PendingGraphs m_pendingGraphs;
};
} // Schematyc
