// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/ScriptComponentInstance.h>

#include <DinrusX/CoreX/Serialization/BlackBox.h>
#include <DinrusX/DinrusXSys/ArchiveHost.h>
#include <DinrusX/CoreX/Serialization/STL.h>
#include <DinrusX/DinrusXSchema/IEnvRegistry.h>
#include <DinrusX/DinrusXSchema/IEnvComponent.h>
#include <DinrusX/DinrusXSchema/IEnvInterface.h>
#include <DinrusX/DinrusXSchema/ISerializationContext.h>
#include <DinrusX/DinrusXSchema/SerializationUtils.h>
#include <DinrusX/DinrusXSchema/Any.h>
#include <DinrusX/DinrusXSchema/Assert.h>
#include <DinrusX/DinrusXSchema/IGUIDRemapper.h>

#include <DinrusX/DinrusXSchema/CVars.h>
#include <DinrusX/DinrusXSchema/ScriptGraph.h>
#include <DinrusX/DinrusXSchema/ScriptGraphNode.h>
#include <DinrusX/DinrusXSchema/ScriptGraphBeginNode.h>
#include <DinrusX/DinrusXSchema/SerializationContext.h>

SERIALIZATION_ENUM_BEGIN_NESTED(Schematyc, EScriptComponentInstanceFlags, "DrxSchematyc Script Component Instance Flags")
SERIALIZATION_ENUM(Schematyc::EScriptComponentInstanceFlags::EnvClass, "envClass", "Environment Class")
SERIALIZATION_ENUM_END()

namespace Schematyc
{

CScriptComponentInstance::CScriptComponentInstance()
	: CScriptElementBase(EScriptElementFlags::None)
{}

CScriptComponentInstance::CScriptComponentInstance(const DrxGUID& guid, const char* szName, const DrxGUID& typeGUID)
	: CScriptElementBase(guid, szName, EScriptElementFlags::None)
	, m_typeGUID(typeGUID)
{
	ApplyComponent(); // #SchematycTODO : Do this on EScriptEventId::Add?
}

EScriptElementAccessor CScriptComponentInstance::GetAccessor() const
{
	return m_accessor;
}

void CScriptComponentInstance::EnumerateDependencies(const ScriptDependencyEnumerator& enumerator, EScriptDependencyType type) const {}

void CScriptComponentInstance::RemapDependencies(IGUIDRemapper& guidRemapper)
{
	CScriptElementBase::RemapDependencies(guidRemapper);
}

void CScriptComponentInstance::ProcessEvent(const SScriptEvent& event)
{
	CScriptElementBase::ProcessEvent(event);
}

void CScriptComponentInstance::Serialize(Serialization::IArchive& archive)
{
	// #SchematycTODO : Shouldn't this be handled by CScriptElementBase itself?
	CScriptElementBase::Serialize(archive);
	CMultiPassSerializer::Serialize(archive);
	CScriptElementBase::SerializeExtensions(archive);
}

DrxGUID CScriptComponentInstance::GetTypeGUID() const
{
	return m_typeGUID;
}

ScriptComponentInstanceFlags CScriptComponentInstance::GetComponentInstanceFlags() const
{
	return m_flags;
}

bool CScriptComponentInstance::HasTransform() const
{
	return m_bHasTransform;
}

void CScriptComponentInstance::SetTransform(const CTransformPtr& transform)
{
	m_pTransform = transform;
}

const CTransformPtr& CScriptComponentInstance::GetTransform() const
{
	return m_pTransform;
}

const CClassProperties& CScriptComponentInstance::GetProperties() const
{
	return m_properties;
}

void CScriptComponentInstance::LoadDependencies(Serialization::IArchive& archive, const ISerializationContext& context)
{
	archive(m_typeGUID, "typeGUID");
}

void CScriptComponentInstance::Load(Serialization::IArchive& archive, const ISerializationContext& context)
{
	ApplyComponent();
	archive(m_accessor, "accessor");
	if (m_bHasTransform && m_pTransform)
	{
		archive(*m_pTransform, "transform");
	}
	archive(m_properties, "properties", "Properties");
}

void CScriptComponentInstance::Save(Serialization::IArchive& archive, const ISerializationContext& context)
{
	archive(m_accessor, "accessor");
	archive(m_typeGUID, "typeGUID");
	if (m_bHasTransform && m_pTransform)
	{
		archive(*m_pTransform, "transform");
	}
	archive(m_properties, "properties", "Properties");
}

void CScriptComponentInstance::Edit(Serialization::IArchive& archive, const ISerializationContext& context)
{
	if (!m_properties.IsEmpty())
	{
		bool bPublic = m_accessor == EScriptElementAccessor::Public;
		archive(bPublic, "bPublic", "Public");
		if (archive.isInput())
		{
			m_accessor = bPublic ? EScriptElementAccessor::Public : EScriptElementAccessor::Private;
		}
	}
	if (m_bHasTransform && m_pTransform)
	{
		archive(*m_pTransform, "transform", "Transform");
	}
	if(!m_properties.IsEmpty())
	{
		archive(m_properties, "properties", "Properties");
	}
}

void CScriptComponentInstance::Validate(Serialization::IArchive& archive, const ISerializationContext& context)
{
	const IEnvComponent* pEnvComponent = gEnv->pSchematyc->GetEnvRegistry().GetComponent(m_typeGUID);
	if (!pEnvComponent)
	{
		CStackString guid;
		GUID::ToString(guid, m_typeGUID);

		CStackString message;
		message.Format("Unable to retrieve environment component: guid = %s", guid.c_str());

		archive.error(*this, message.c_str());
	}
}

void CScriptComponentInstance::ApplyComponent()
{
	const IEnvComponent* pEnvComponent = gEnv->pSchematyc->GetEnvRegistry().GetComponent(m_typeGUID);
	if (pEnvComponent)
	{
		const CEntityComponentClassDesc& componentDesc = pEnvComponent->GetDesc();
		m_bHasTransform = componentDesc.GetComponentFlags().Check(IEntityComponent::EFlags::Transform);
		if (m_bHasTransform && !m_pTransform)
		{
			m_pTransform = std::make_shared<CTransform>();
		}
		m_properties.Set(componentDesc);
	}
}

} // Schematyc
