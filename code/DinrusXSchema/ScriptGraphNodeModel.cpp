// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/ScriptGraphNodeModel.h>

#include <DinrusX/DinrusXSchema/Assert.h>

namespace Schematyc
{
void CScriptGraphNodeModel::Init()                                                                                   {}

void CScriptGraphNodeModel::LoadDependencies(Serialization::IArchive& archive, const ISerializationContext& context) {}

void CScriptGraphNodeModel::Load(Serialization::IArchive& archive, const ISerializationContext& context)             {}

void CScriptGraphNodeModel::PostLoad(Serialization::IArchive& archive, const ISerializationContext& context)         {}

void CScriptGraphNodeModel::Save(Serialization::IArchive& archive, const ISerializationContext& context)             {}

void CScriptGraphNodeModel::Edit(Serialization::IArchive& archive, const ISerializationContext& context)             {}

void CScriptGraphNodeModel::Validate(Serialization::IArchive& archive, const ISerializationContext& context)         {}

void CScriptGraphNodeModel::RemapDependencies(IGUIDRemapper& guidRemapper)                                           {}

void CScriptGraphNodeModel::Attach(CScriptGraphNode& node)
{
	m_pNode = &node;
}

CScriptGraphNode& CScriptGraphNodeModel::GetNode()
{
	SCHEMATYC_CORE_ASSERT(m_pNode);
	return *m_pNode;
}

const CScriptGraphNode& CScriptGraphNodeModel::GetNode() const
{
	SCHEMATYC_CORE_ASSERT(m_pNode);
	return *m_pNode;
}

CScriptGraphNodeRegistrar::CScriptGraphNodeRegistrar(ScriptGraphNodeRegistrarFunctionPtr pFunction)
	: m_pFunction(pFunction)
{}

void CScriptGraphNodeRegistrar::Process(CScriptGraphNodeFactory& factory)
{
	for (const CScriptGraphNodeRegistrar* pInstance = GetFirstInstance(); pInstance; pInstance = pInstance->GetNextInstance())
	{
		(*pInstance->m_pFunction)(factory);
	}
}
}
