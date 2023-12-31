// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/ScriptGraphNodeFactory.h>

#include <DinrusX/DinrusXSys/ITimer.h>

namespace Schematyc
{
bool CScriptGraphNodeFactory::RegisterCreator(const IScriptGraphNodeCreatorPtr& pCreator)
{
	if (!pCreator)
	{
		return false;
	}
	const DrxGUID& typeGUID = pCreator->GetTypeGUID();
	if (GetCreator(typeGUID))
	{
		return false;
	}
	m_creators.insert(Creators::value_type(typeGUID, pCreator));
	return true;
}

bool CScriptGraphNodeFactory::CanCreateNode(const DrxGUID& typeGUID, const IScriptView& scriptView, const IScriptGraph& graph) const
{
	// TODO: Replace this with a proper function on the node creator.
	struct SDummyMenu : public IScriptGraphNodeCreationMenu
	{
		virtual bool AddCommand(const IScriptGraphNodeCreationCommandPtr& pCommand)
		{
			canBeCreated = true;
			return true;
		}

		bool canBeCreated = false;
	} dummyMenu;
	// ~TODO

	auto result = m_creators.find(typeGUID);
	if (result != m_creators.end())
	{
		result->second->PopulateNodeCreationMenu(dummyMenu, scriptView, graph);
		return dummyMenu.canBeCreated;
	}

	return false;
}

IScriptGraphNodeCreator* CScriptGraphNodeFactory::GetCreator(const DrxGUID& typeGUID)
{
	Creators::iterator itCreator = m_creators.find(typeGUID);
	return itCreator != m_creators.end() ? itCreator->second.get() : nullptr;
}

IScriptGraphNodePtr CScriptGraphNodeFactory::CreateNode(const DrxGUID& typeGUID, const DrxGUID& guid)
{
	IScriptGraphNodeCreator* pCreator = GetCreator(typeGUID);
	return pCreator ? pCreator->CreateNode(guid) : IScriptGraphNodePtr();
}

void CScriptGraphNodeFactory::PopulateNodeCreationMenu(IScriptGraphNodeCreationMenu& nodeCreationMenu, const IScriptView& scriptView, const IScriptGraph& graph)
{
	const int64 startTime = DrxGetTicks();

	for (Creators::value_type& creator : m_creators)
	{
		creator.second->PopulateNodeCreationMenu(nodeCreationMenu, scriptView, graph);
	}

	const float time = gEnv->pTimer->TicksToSeconds(DrxGetTicks() - startTime);
	SCHEMATYC_CORE_COMMENT("Populating node creatiion menu : time = %f(s)", time);
}
}
