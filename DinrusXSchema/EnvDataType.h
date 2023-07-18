// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXSchema/FundamentalTypes.h>
#include <DinrusX/DinrusXSchema/EnvElementBase.h>
#include <DinrusX/DinrusXSchema/IEnvDataType.h>
#include <DinrusX/DinrusXSchema/TypeDesc.h>
#include <DinrusX/DinrusXSchema/Any.h>
#include <DinrusX/DinrusXSchema/TypeName.h>

#define SCHEMATYC_MAKE_ENV_DATA_TYPE(type) Schematyc::EnvDataType::MakeShared<type>(SCHEMATYC_SOURCE_FILE_INFO)

namespace Schematyc
{

template<typename TYPE> class CEnvDataType : public CEnvElementBase<IEnvDataType>
{
public:

	inline CEnvDataType(const SSourceFileInfo& sourceFileInfo)
		: CEnvElementBase(sourceFileInfo)
	{
		const CCommonTypeDesc& typeDesc = Schematyc::GetTypeDesc<TYPE>();
		CEnvElementBase::SetGUID(typeDesc.GetGUID());
		CEnvElementBase::SetName(typeDesc.GetLabel());
		CEnvElementBase::SetDescription(typeDesc.GetDescription());
	}

	// IEnvElement

	virtual bool IsValidScope(IEnvElement& scope) const override
	{
		const EEnvElementType elementType = scope.GetType();
		switch (elementType)
		{
		case EEnvElementType::Root:
		case EEnvElementType::Module:
		case EEnvElementType::Class:
		case EEnvElementType::Component:
		case EEnvElementType::Action:
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

	// IEnvType

	virtual const CCommonTypeDesc& GetDesc() const override
	{
		return Schematyc::GetTypeDesc<TYPE>();
	}

	// IEnvType
};

namespace EnvDataType
{

template<typename TYPE> inline std::shared_ptr<CEnvDataType<TYPE>> MakeShared(const SSourceFileInfo& sourceFileInfo)
{
	static_assert(std::is_default_constructible<TYPE>::value, "Type must be default constructible!");
	SCHEMATYC_VERIFY_TYPE_IS_REFLECTED(TYPE);

	return std::make_shared<CEnvDataType<TYPE>>(sourceFileInfo);
}

} // EnvDataType
} // Schematyc
