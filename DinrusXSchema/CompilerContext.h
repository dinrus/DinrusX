// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IInterfaceMap.h>

namespace Schematyc
{
// Forward declare interfaces.
struct IScriptGraph;

struct ICompilerTaskList
{
	virtual ~ICompilerTaskList() {}

	virtual void CompileGraph(const IScriptGraph& scriptGraph) = 0;
};

typedef IInterfaceMap<void> ICompilerInterfaceMap;

struct SCompilerContext
{
	inline SCompilerContext(ICompilerTaskList& _tasks, ICompilerInterfaceMap& _interfaces)
		: tasks(_tasks)
		, interfaces(_interfaces)
	{}

	ICompilerTaskList&     tasks;
	ICompilerInterfaceMap& interfaces;
};
} // Schematyc
