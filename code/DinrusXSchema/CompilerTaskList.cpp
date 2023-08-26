// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/CompilerTaskList.h>

namespace Schematyc
{
CCompilerTaskList::CCompilerTaskList()
{
	m_pendingGraphs.reserve(20);
}

void CCompilerTaskList::CompileGraph(const IScriptGraph& scriptGraph)
{
	m_pendingGraphs.push_back(&scriptGraph);
}

CCompilerTaskList::PendingGraphs& CCompilerTaskList::GetPendingGraphs()
{
	return m_pendingGraphs;
}
} // Schematyc
