// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/ScriptConstructor.h>

#include <DinrusX/DinrusXSchema/ISerializationContext.h>
#include <DinrusX/DinrusXSchema/IGUIDRemapper.h>

#include <DinrusX/DinrusXSchema/ScriptGraph.h>
#include <DinrusX/DinrusXSchema/ScriptGraphNode.h>
#include <DinrusX/DinrusXSchema/ScriptGraphBeginNode.h>

namespace Schematyc
{
CScriptConstructor::CScriptConstructor()
	: CScriptElementBase({ EScriptElementFlags::CanOwnScript, EScriptElementFlags::FixedName })
{
	CreateGraph();
}

CScriptConstructor::CScriptConstructor(const DrxGUID& guid, const char* szName)
	: CScriptElementBase(guid, szName, { EScriptElementFlags::CanOwnScript, EScriptElementFlags::FixedName })
{
	CreateGraph();
}

void CScriptConstructor::EnumerateDependencies(const ScriptDependencyEnumerator& enumerator, EScriptDependencyType type) const
{
	CScriptElementBase::EnumerateDependencies(enumerator, type);
}

void CScriptConstructor::RemapDependencies(IGUIDRemapper& guidRemapper) {}

void CScriptConstructor::ProcessEvent(const SScriptEvent& event)
{
	CScriptElementBase::ProcessEvent(event);

	switch (event.id)
	{
	case EScriptEventId::EditorAdd:
		{
			// TODO: This should happen in editor!
			IScriptGraph* pGraph = static_cast<IScriptGraph*>(CScriptElementBase::GetExtensions().QueryExtension(EScriptExtensionType::Graph));
			SCHEMATYC_CORE_ASSERT(pGraph);
			if (pGraph)
			{
				// TODO : Shouldn't we be using CScriptGraphNodeFactory::CreateNode() instead of instantiating the node directly?!?
				pGraph->AddNode(std::make_shared<CScriptGraphNode>(gEnv->pSchematyc->CreateGUID(), stl::make_unique<CScriptGraphBeginNode>()));
				// ~TODO
			}

			m_userDocumentation.SetCurrentUserAsAuthor();
			// ~TODO

			break;
		}
	case EScriptEventId::EditorPaste:
		{
			m_userDocumentation.SetCurrentUserAsAuthor();
			break;
		}
	}
}

void CScriptConstructor::Serialize(Serialization::IArchive& archive)
{
	CScriptElementBase::Serialize(archive);

	switch (SerializationContext::GetPass(archive))
	{
	case ESerializationPass::LoadDependencies:
	case ESerializationPass::Save:
		{
			archive(m_userDocumentation, "userDocumentation");
			break;
		}
	case ESerializationPass::Edit:
		{
			archive(m_userDocumentation, "userDocumentation", "Documentation");
			break;
		}
	}

	CScriptElementBase::SerializeExtensions(archive);
}

void CScriptConstructor::CreateGraph()
{
	CScriptElementBase::AddExtension(std::make_shared<CScriptGraph>(*this, EScriptGraphType::Construction));
}
} // Schematyc
