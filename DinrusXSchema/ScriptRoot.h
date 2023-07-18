// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/IScriptRoot.h>
#include <DinrusX/DinrusXSchema/MultiPassSerializer.h>

#include <DinrusX/DinrusXSchema/ScriptElementBase.h>

namespace Schematyc
{
class CScriptRoot : public CScriptElementBase<IScriptRoot>, public CMultiPassSerializer
{
public:

	CScriptRoot();

	// IScriptElement
	virtual void EnumerateDependencies(const ScriptDependencyEnumerator& enumerator, EScriptDependencyType type) const override;
	virtual void RemapDependencies(IGUIDRemapper& guidRemapper) override;
	virtual void ProcessEvent(const SScriptEvent& event) override;
	virtual void Serialize(Serialization::IArchive& archive) override;
	// ~IScriptElement
};
} // Schematyc
