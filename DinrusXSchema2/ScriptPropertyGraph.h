// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema2/ScriptGraphBase.h>

namespace Schematyc2
{
	class CScriptPropertyGraph : public CScriptGraphBase
	{
	public:

		CScriptPropertyGraph(IScriptElement& element);

		// IScriptExtension
		virtual void Refresh_New(const SScriptRefreshParams& params) override;
		virtual void Serialize_New(Serialization::IArchive& archive) override;
		// ~IScriptExtension

		// IScriptGraph
		virtual EScriptGraphType GetType() const override;
		// ~IScriptGraph

	private:

		bool BeginNodeExists() const;
	};
}
