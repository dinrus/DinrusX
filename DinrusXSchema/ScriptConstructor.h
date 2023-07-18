// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IScriptConstructor.h>

#include <DinrusX/DinrusXSchema/ScriptElementBase.h>
#include <DinrusX/DinrusXSchema/ScriptUserDocumentation.h>

namespace Schematyc
{
class CScriptConstructor : public CScriptElementBase<IScriptConstructor>
{
public:

	CScriptConstructor();
	CScriptConstructor(const DrxGUID& guid, const char* szName);

	// IScriptElement
	virtual void EnumerateDependencies(const ScriptDependencyEnumerator& enumerator, EScriptDependencyType type) const override;
	virtual void RemapDependencies(IGUIDRemapper& guidRemapper) override;
	virtual void ProcessEvent(const SScriptEvent& event) override;
	virtual void Serialize(Serialization::IArchive& archive) override;
	// ~IScriptElement

private:

	void CreateGraph();

private:

	SScriptUserDocumentation m_userDocumentation;
};
} // Schematyc
