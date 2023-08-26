// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/ScriptGraphGetObjectIdNode.h>

#include <DinrusX/DinrusXSchema/IObject.h>
#include <DinrusX/DinrusXSchema/IGraphNodeCompiler.h>

#include <DinrusX/DinrusXSchema/IScriptVariable.h>

#include <DinrusX/DinrusXSchema/RuntimeClass.h>
#include <DinrusX/DinrusXSchema/ScriptView.h>
#include <DinrusX/DinrusXSchema/ScriptGraphNode.h>
#include <DinrusX/DinrusXSchema/ScriptGraphNodeFactory.h>
#include <DinrusX/DinrusXSchema/SerializationContext.h>

namespace Schematyc
{
CScriptGraphGetObjectIdNode::CScriptGraphGetObjectIdNode() {}

DrxGUID CScriptGraphGetObjectIdNode::GetTypeGUID() const
{
	return ms_typeGUID;
}

void CScriptGraphGetObjectIdNode::CreateLayout(CScriptGraphNodeLayout& layout)
{
	layout.SetName("GetObjectId");
	layout.SetStyleId("Core::Data");
	
	layout.AddOutputWithData("ObjectId", GetTypeDesc<ObjectId>().GetGUID(), { EScriptGraphPortFlags::Data, EScriptGraphPortFlags::MultiLink, EScriptGraphPortFlags::Pull }, ObjectId());
}

void CScriptGraphGetObjectIdNode::Compile(SCompilerContext& context, IGraphNodeCompiler& compiler) const
{
	compiler.BindCallback(&Execute);
}

void CScriptGraphGetObjectIdNode::Register(CScriptGraphNodeFactory& factory)
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
				return "GetObjectId";
			}

			virtual const char* GetSubject() const override
			{
				return nullptr;
			}

			virtual const char* GetDescription() const override
			{
				return "Get id of this object";
			}

			virtual const char* GetStyleId() const override
			{
				return "Core::Data";
			}

			virtual IScriptGraphNodePtr Execute(const Vec2& pos) override
			{
				return std::make_shared<CScriptGraphNode>(gEnv->pSchematyc->CreateGUID(), stl::make_unique<CScriptGraphGetObjectIdNode>(), pos);
			}

			// ~IScriptGraphNodeCreationCommand
		};

	public:

		// IScriptGraphNodeCreator

		virtual DrxGUID GetTypeGUID() const override
		{
			return CScriptGraphGetObjectIdNode::ms_typeGUID;
		}

		virtual IScriptGraphNodePtr CreateNode(const DrxGUID& guid) override
		{
			return std::make_shared<CScriptGraphNode>(guid, stl::make_unique<CScriptGraphGetObjectIdNode>());
		}

		virtual void PopulateNodeCreationMenu(IScriptGraphNodeCreationMenu& nodeCreationMenu, const IScriptView& scriptView, const IScriptGraph& graph) override
		{
			if (scriptView.GetScriptClass())
			{
				nodeCreationMenu.AddCommand(std::make_shared<CCreationCommand>());
			}
		}

		// ~IScriptGraphNodeCreator
	};

	factory.RegisterCreator(std::make_shared<CCreator>());
}

SRuntimeResult CScriptGraphGetObjectIdNode::Execute(SRuntimeContext& context, const SRuntimeActivationParams& activationParams)
{
	DynamicCast<ObjectId>(*context.node.GetOutputData(EOutputIdx::Value)) = static_cast<IObject*>(context.pObject)->GetId();

	return SRuntimeResult(ERuntimeStatus::Continue);
}

const DrxGUID CScriptGraphGetObjectIdNode::ms_typeGUID = "6b0c3534-1117-4a57-bb15-e43c51aff2e0"_drx_guid;
} // Schematyc

SCHEMATYC_REGISTER_SCRIPT_GRAPH_NODE(Schematyc::CScriptGraphGetObjectIdNode::Register)
