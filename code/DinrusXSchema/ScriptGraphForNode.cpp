// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/ScriptGraphForNode.h>

#include <DinrusX/DinrusXSchema/IGraphNodeCompiler.h>

#include <DinrusX/DinrusXSchema/ScriptGraphNode.h>
#include <DinrusX/DinrusXSchema/ScriptGraphNodeFactory.h>

namespace Schematyc
{

CScriptGraphForNode::SRuntimeData::SRuntimeData()
	: pos(InvalidIdx)
	, bBreak(false)
{}

CScriptGraphForNode::SRuntimeData::SRuntimeData(const SRuntimeData& rhs)
	: pos(rhs.pos)
	, bBreak(rhs.bBreak)
{}

void CScriptGraphForNode::SRuntimeData::ReflectType(CTypeDesc<SRuntimeData>& desc)
{
	desc.SetGUID("2cf6b75a-caed-4d69-914a-08ca7b0e5a67"_drx_guid);
}

CScriptGraphForNode::CScriptGraphForNode() {}

DrxGUID CScriptGraphForNode::GetTypeGUID() const
{
	return ms_typeGUID;
}

void CScriptGraphForNode::CreateLayout(CScriptGraphNodeLayout& layout)
{
	layout.SetName("For");
	layout.SetStyleId("Core::FlowControl");

	layout.AddInput("In", DrxGUID(), { EScriptGraphPortFlags::Flow, EScriptGraphPortFlags::MultiLink });
	layout.AddInputWithData("Begin", GetTypeDesc<int32>().GetGUID(), { EScriptGraphPortFlags::Data, EScriptGraphPortFlags::Persistent, EScriptGraphPortFlags::Editable }, int32(0));
	layout.AddInputWithData("End", GetTypeDesc<int32>().GetGUID(), { EScriptGraphPortFlags::Data, EScriptGraphPortFlags::Persistent, EScriptGraphPortFlags::Editable }, int32(0));
	layout.AddInput("Break", DrxGUID(), { EScriptGraphPortFlags::Flow, EScriptGraphPortFlags::MultiLink, EScriptGraphPortFlags::End });

	layout.AddOutput("Out", DrxGUID(), EScriptGraphPortFlags::Flow);
	layout.AddOutput("Loop", DrxGUID(), EScriptGraphPortFlags::Flow);
	layout.AddOutputWithData("Pos", GetTypeDesc<int32>().GetGUID(), { EScriptGraphPortFlags::Data, EScriptGraphPortFlags::MultiLink }, int32(0));
}

void CScriptGraphForNode::Compile(SCompilerContext& context, IGraphNodeCompiler& compiler) const
{
	compiler.BindCallback(&Execute);
	compiler.BindData(SRuntimeData());
}

void CScriptGraphForNode::Register(CScriptGraphNodeFactory& factory)
{
	class CCreator : public IScriptGraphNodeCreator
	{
	private:

		class CCreationCommand : public IScriptGraphNodeCreationCommand
		{
		public:

			// IScriptGraphNodeCreationCommand

			virtual const char* GetBehavior() const override
			{
				return "For";
			}

			virtual const char* GetSubject() const override
			{
				return nullptr;
			}

			virtual const char* GetDescription() const override
			{
				return "For loop";
			}

			virtual const char* GetStyleId() const override
			{
				return "Core::FlowControl";
			}

			virtual IScriptGraphNodePtr Execute(const Vec2& pos) override
			{
				return std::make_shared<CScriptGraphNode>(gEnv->pSchematyc->CreateGUID(), stl::make_unique<CScriptGraphForNode>(), pos);
			}

			// ~IScriptGraphNodeCreationCommand
		};

	public:

		// IScriptGraphNodeCreator

		virtual DrxGUID GetTypeGUID() const override
		{
			return CScriptGraphForNode::ms_typeGUID;
		}

		virtual IScriptGraphNodePtr CreateNode(const DrxGUID& guid) override
		{
			return std::make_shared<CScriptGraphNode>(guid, stl::make_unique<CScriptGraphForNode>());
		}

		virtual void PopulateNodeCreationMenu(IScriptGraphNodeCreationMenu& nodeCreationMenu, const IScriptView& scriptView, const IScriptGraph& graph) override
		{
			nodeCreationMenu.AddCommand(std::make_shared<CCreationCommand>());
		}

		// ~IScriptGraphNodeCreator
	};

	factory.RegisterCreator(std::make_shared<CCreator>());
}

SRuntimeResult CScriptGraphForNode::Execute(SRuntimeContext& context, const SRuntimeActivationParams& activationParams)
{
	SRuntimeData& data = DynamicCast<SRuntimeData>(*context.node.GetData());

	if (activationParams.mode == EActivationMode::Input)
	{
		switch (activationParams.portIdx)
		{
		case EInputIdx::In:
			{
				data.pos = DynamicCast<int32>(*context.node.GetInputData(EInputIdx::Begin));
				break;
			}
		case EInputIdx::Break:
			{
				data.bBreak;
				return SRuntimeResult(ERuntimeStatus::Break);
			}
		}
	}

	if (!data.bBreak)
	{
		DynamicCast<int32>(*context.node.GetOutputData(EOutputIdx::Pos)) = data.pos;

		const int32 end = DynamicCast<int32>(*context.node.GetInputData(EInputIdx::End));
		if (data.pos < end)
		{
			++data.pos;
			return SRuntimeResult(ERuntimeStatus::ContinueAndRepeat, EOutputIdx::Loop);
		}
	}

	return SRuntimeResult(ERuntimeStatus::Continue, EOutputIdx::Out);
}

const DrxGUID CScriptGraphForNode::ms_typeGUID = "a902d2a5-cc66-49e0-8c2e-e52b48cc7159"_drx_guid;

} // Schematyc

SCHEMATYC_REGISTER_SCRIPT_GRAPH_NODE(Schematyc::CScriptGraphForNode::Register)
