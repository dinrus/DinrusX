// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/ScriptGraphGetEntityIdNode.h>

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
CScriptGraphGetEntityIdNode::CScriptGraphGetEntityIdNode() {}

DrxGUID CScriptGraphGetEntityIdNode::GetTypeGUID() const
{
	return ms_typeGUID;
}

void CScriptGraphGetEntityIdNode::CreateLayout(CScriptGraphNodeLayout& layout)
{
	layout.SetName("GetEntityId");
	layout.SetStyleId("Core::Data");
	
	layout.AddOutputWithData("Entity", GetTypeDesc<ExplicitEntityId>().GetGUID(), { EScriptGraphPortFlags::Data, EScriptGraphPortFlags::MultiLink, EScriptGraphPortFlags::Pull }, ExplicitEntityId(INVALID_ENTITYID));
}

void CScriptGraphGetEntityIdNode::Compile(SCompilerContext& context, IGraphNodeCompiler& compiler) const
{
	compiler.BindCallback(&Execute);
}

void CScriptGraphGetEntityIdNode::Register(CScriptGraphNodeFactory& factory)
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
				return "GetEntity";
			}

			virtual const char* GetSubject() const override
			{
				return nullptr;
			}

			virtual const char* GetDescription() const override
			{
				return "Gets the Entity we are attached to";
			}

			virtual const char* GetStyleId() const override
			{
				return "Core::Data";
			}

			virtual IScriptGraphNodePtr Execute(const Vec2& pos) override
			{
				return std::make_shared<CScriptGraphNode>(gEnv->pSchematyc->CreateGUID(), stl::make_unique<CScriptGraphGetEntityIdNode>(), pos);
			}

			// ~IScriptGraphNodeCreationCommand
		};

	public:

		// IScriptGraphNodeCreator

		virtual DrxGUID GetTypeGUID() const override
		{
			return CScriptGraphGetEntityIdNode::ms_typeGUID;
		}

		virtual IScriptGraphNodePtr CreateNode(const DrxGUID& guid) override
		{
			return std::make_shared<CScriptGraphNode>(guid, stl::make_unique<CScriptGraphGetEntityIdNode>());
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

SRuntimeResult CScriptGraphGetEntityIdNode::Execute(SRuntimeContext& context, const SRuntimeActivationParams& activationParams)
{
	IEntity *pEntity = static_cast<IObject*>(context.pObject)->GetEntity();
	if (pEntity)
	{
		DynamicCast<ExplicitEntityId>(*context.node.GetOutputData(EOutputIdx::Value)) = static_cast<ExplicitEntityId>(pEntity->GetId());
	}
	else
	{
		DynamicCast<ExplicitEntityId>(*context.node.GetOutputData(EOutputIdx::Value)) = static_cast<ExplicitEntityId>(INVALID_ENTITYID);
	}

	return SRuntimeResult(ERuntimeStatus::Continue);
}

const DrxGUID CScriptGraphGetEntityIdNode::ms_typeGUID = "29C4EC3A-8E27-4D95-9C6D-8B3A19865FC2"_drx_guid;
} // Schematyc

SCHEMATYC_REGISTER_SCRIPT_GRAPH_NODE(Schematyc::CScriptGraphGetEntityIdNode::Register)
