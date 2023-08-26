// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/RuntimeGraph.h>
#include <DinrusX/DinrusXSchema/GUID.h>

#include <DinrusX/DinrusXSchema/ScriptGraphNodeModel.h>

namespace Schematyc
{
class CScriptGraphBranchNode : public CScriptGraphNodeModel
{
private:

	struct EInputIdx
	{
		enum : uint32
		{
			In = 0,
			Value
		};
	};

	struct EOutputIdx
	{
		enum : uint32
		{
			True = 0,
			False
		};
	};

public:

	CScriptGraphBranchNode();

	// CScriptGraphNodeModel
	virtual DrxGUID GetTypeGUID() const override;
	virtual void  CreateLayout(CScriptGraphNodeLayout& layout) override;
	virtual void  Compile(SCompilerContext& context, IGraphNodeCompiler& compiler) const override;
	// ~CScriptGraphNodeModel

	static void Register(CScriptGraphNodeFactory& factory);

private:

	static SRuntimeResult Execute(SRuntimeContext& context, const SRuntimeActivationParams& activationParams);

public:

	static const DrxGUID ms_typeGUID;
};
}
