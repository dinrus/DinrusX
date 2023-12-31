// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Memory/STLPoolAllocator.h>

#include <DinrusX/DinrusXSchema/Action.h>
#include <DinrusX/DinrusXSchema/FundamentalTypes.h>
#include <DinrusX/DinrusXSchema/EnvElementBase.h>
#include <DinrusX/DinrusXSchema/IEnvAction.h>
#include <DinrusX/DinrusXSchema/ActionDesc.h>

#define SCHEMATYC_MAKE_ENV_ACTION(type) Schematyc::EnvAction::MakeShared<type>(SCHEMATYC_SOURCE_FILE_INFO)

namespace Schematyc
{

template<typename TYPE> class CEnvAction : public CEnvElementBase<IEnvAction> // #SchematycTODO : Does this type need to be template or can we simply store a const reference to the type description?
{
public:

	inline CEnvAction(const SSourceFileInfo& sourceFileInfo)
		: CEnvElementBase(sourceFileInfo)
	{
		const CCommonTypeDesc& desc = Schematyc::GetTypeDesc<TYPE>();
		CEnvElementBase::SetGUID(desc.GetGUID());
		CEnvElementBase::SetName(desc.GetLabel());
		CEnvElementBase::SetDescription(desc.GetDescription());
	}

	// IEnvElement

	virtual bool IsValidScope(IEnvElement& scope) const override
	{
		switch (scope.GetType())
		{
		case EEnvElementType::Module:
		case EEnvElementType::Class:
		case EEnvElementType::Component:
			{
				return true;
			}
		default:
			{
				return false;
			}
		}
	}

	// ~IEnvElement

	// IEnvAction

	virtual const CActionDesc& GetDesc() const override
	{
		return Schematyc::GetTypeDesc<TYPE>();
	}

	virtual CActionPtr CreateFromPool() const override
	{
		return std::allocate_shared<TYPE>(m_allocator);
	}

	// ~IEnvAction

private:

	stl::STLPoolAllocator<TYPE> m_allocator;
};

namespace EnvAction
{

template<typename TYPE> inline std::shared_ptr<CEnvAction<TYPE>> MakeShared(const SSourceFileInfo& sourceFileInfo)
{
	static_assert(std::is_base_of<CAction, TYPE>::value, "Type must be derived from Schematyc::CAction!");
	SCHEMATYC_VERIFY_TYPE_IS_REFLECTED(TYPE);

	return std::make_shared<CEnvAction<TYPE>>(sourceFileInfo);
}

} // EnvAction
} // Schematyc
