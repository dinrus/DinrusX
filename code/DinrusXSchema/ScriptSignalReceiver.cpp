// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/ScriptSignalReceiver.h>

#include <DinrusX/CoreX/Serialization/Decorators/ActionButton.h>
#include <DinrusX/DinrusXSchema/IEnvRegistry.h>
#include <DinrusX/DinrusXSchema/IEnvSignal.h>
#include <DinrusX/DinrusXSchema/IScriptRegistry.h>
#include <DinrusX/DinrusXSchema/IScriptSignal.h>
#include <DinrusX/DinrusXSchema/IScriptTimer.h>
#include <DinrusX/DinrusXSchema/ISerializationContext.h>
#include <DinrusX/DinrusXSchema/IGUIDRemapper.h>
#include <DinrusX/DinrusXSchema/StackString.h>

#include <DinrusX/DinrusXSchema/ScriptGraph.h>
#include <DinrusX/DinrusXSchema/ScriptGraphNode.h>
#include <DinrusX/DinrusXSchema/ScriptGraphBeginNode.h>

namespace Schematyc
{
CScriptSignalReceiver::CScriptSignalReceiver()
	: CScriptElementBase(EScriptElementFlags::CanOwnScript)
	, m_type(EScriptSignalReceiverType::Unknown)
{
	CreateGraph();
}

CScriptSignalReceiver::CScriptSignalReceiver(const DrxGUID& guid, const char* szName, EScriptSignalReceiverType type, const DrxGUID& signalGUID)
	: CScriptElementBase(guid, szName, EScriptElementFlags::CanOwnScript)
	, m_type(type)
	, m_signalGUID(signalGUID)
{
	if (m_type != EScriptSignalReceiverType::Universal)
	{
		ScriptElementFlags elementFlags = CScriptElementBase::GetFlags();
		elementFlags.Add(EScriptElementFlags::FixedName);
		CScriptElementBase::SetFlags(elementFlags);
	}

	CreateGraph();
}

void CScriptSignalReceiver::EnumerateDependencies(const ScriptDependencyEnumerator& enumerator, EScriptDependencyType type) const
{
	SCHEMATYC_CORE_ASSERT(enumerator);
	if (enumerator)
	{
		enumerator(m_signalGUID);

		CScriptElementBase::EnumerateDependencies(enumerator, type);
	}
}

void CScriptSignalReceiver::RemapDependencies(IGUIDRemapper& guidRemapper)
{
	CScriptElementBase::RemapDependencies(guidRemapper);
}

void CScriptSignalReceiver::ProcessEvent(const SScriptEvent& event)
{
	CScriptElementBase::ProcessEvent(event);

	switch (m_type)
	{
	case EScriptSignalReceiverType::EnvSignal:
		{
			const IEnvSignal* pEnvSignal = gEnv->pSchematyc->GetEnvRegistry().GetSignal(m_signalGUID);
			if (pEnvSignal)
			{
				CStackString name;
				name = "On";
				name.append(pEnvSignal->GetName());
				CScriptElementBase::SetName(name);
			}
			break;
		}
	case EScriptSignalReceiverType::ScriptSignal:
		{
			const IScriptSignal* pScriptSignal = DynamicCast<IScriptSignal>(gEnv->pSchematyc->GetScriptRegistry().GetElement(m_signalGUID));
			if (pScriptSignal)
			{
				CStackString name;
				name = "On";
				name.append(pScriptSignal->GetName());
				CScriptElementBase::SetName(name);
			}
			break;
		}
	case EScriptSignalReceiverType::ScriptTimer:
		{
			const IScriptTimer* pScriptTimer = DynamicCast<IScriptTimer>(gEnv->pSchematyc->GetScriptRegistry().GetElement(m_signalGUID));
			if (pScriptTimer)
			{
				CStackString name;
				name = "On";
				name.append(pScriptTimer->GetName());
				CScriptElementBase::SetName(name);
			}
			break;
		}
	}

	switch (event.id)
	{
	case EScriptEventId::EditorAdd:
		{
			// TODO: This should happen in editor!
			if (m_type != EScriptSignalReceiverType::Universal)
			{
				IScriptGraph* pGraph = static_cast<IScriptGraph*>(CScriptElementBase::GetExtensions().QueryExtension(EScriptExtensionType::Graph));
				SCHEMATYC_CORE_ASSERT(pGraph);
				if (pGraph)
				{
					// TODO : Shouldn't we be using CScriptGraphNodeFactory::CreateNode() instead of instantiating the node directly?!?
					pGraph->AddNode(std::make_shared<CScriptGraphNode>(gEnv->pSchematyc->CreateGUID(), stl::make_unique<CScriptGraphBeginNode>()));
					// ~TODO
				}
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

void CScriptSignalReceiver::Serialize(Serialization::IArchive& archive)
{
	// #SchematycTODO : Shouldn't this be handled by CScriptElementBase itself?
	CScriptElementBase::Serialize(archive);
	CMultiPassSerializer::Serialize(archive);
	CScriptElementBase::SerializeExtensions(archive);
}

EScriptSignalReceiverType CScriptSignalReceiver::GetSignalReceiverType() const
{
	return m_type;
}

DrxGUID CScriptSignalReceiver::GetSignalGUID() const
{
	return m_signalGUID;
}

void CScriptSignalReceiver::LoadDependencies(Serialization::IArchive& archive, const ISerializationContext& context)
{
	archive(m_signalGUID, "signalGUID");
}

void CScriptSignalReceiver::Load(Serialization::IArchive& archive, const ISerializationContext& context)
{
	archive(m_type, "type");
	archive(m_userDocumentation, "userDocumentation");

	if (m_type != EScriptSignalReceiverType::Universal)
	{
		ScriptElementFlags elementFlags = CScriptElementBase::GetFlags();
		elementFlags.Add(EScriptElementFlags::FixedName);
		CScriptElementBase::SetFlags(elementFlags);
	}
}

void CScriptSignalReceiver::Save(Serialization::IArchive& archive, const ISerializationContext& context)
{
	archive(m_type, "type");
	archive(m_signalGUID, "signalGUID");
	archive(m_userDocumentation, "userDocumentation");
}

void CScriptSignalReceiver::Edit(Serialization::IArchive& archive, const ISerializationContext& context)
{
	archive(m_userDocumentation, "userDocumentation", "Documentation");

	if (archive.isValidation())
	{
		Validate(archive, context);
	}
}

void CScriptSignalReceiver::Validate(Serialization::IArchive& archive, const ISerializationContext& context)
{
	// #SchematycTODO : Use script view to ensure signal is scoped correctly?

	switch (m_type)
	{
	case EScriptSignalReceiverType::EnvSignal:
		{
			const IEnvSignal* pEnvSignal = gEnv->pSchematyc->GetEnvRegistry().GetSignal(m_signalGUID);
			if (!pEnvSignal)
			{
				archive.error(*this, "Failed to retrieve environment signal!");
			}
			break;
		}
	case EScriptSignalReceiverType::ScriptSignal:
		{
			const IScriptSignal* pScriptSignal = DynamicCast<IScriptSignal>(gEnv->pSchematyc->GetScriptRegistry().GetElement(m_signalGUID));
			if (!pScriptSignal)
			{
				archive.error(*this, "Failed to retrieve script signal!");
			}
			break;
		}
	case EScriptSignalReceiverType::ScriptTimer:
		{
			const IScriptTimer* pScriptTimer = DynamicCast<IScriptTimer>(gEnv->pSchematyc->GetScriptRegistry().GetElement(m_signalGUID));
			if (!pScriptTimer)
			{
				archive.error(*this, "Failed to retrieve script timer!");
			}
			break;
		}
	}
}

void CScriptSignalReceiver::CreateGraph()
{
	CScriptElementBase::AddExtension(std::make_shared<CScriptGraph>(*this, EScriptGraphType::Signal));
}

void CScriptSignalReceiver::GoToSignal()
{
	DrxLinkUtils::ExecuteCommand(DrxLinkUtils::ECommand::Show, m_signalGUID);
}
} // Schematyc
