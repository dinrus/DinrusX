// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXReflection/IReflection.h>
#include <DinrusX/CoreX/Extension/DrxGUID.h>

#include <unordered_map>
#include <vector>

#include <DinrusX/DinrusXReflection/ReflectedTypeDesc.h>

namespace Drx {
namespace Reflection {

class CReflection : public IReflection
{
	DRXINTERFACE_BEGIN()
	DRXINTERFACE_ADD(Drx::IDefaultModule)
	DRXINTERFACE_ADD(Drx::Reflection::IReflection)
	DRXINTERFACE_END()

	DRXGENERATE_SINGLETONCLASS_GUID(CReflection, "EngineModule_Reflection", "{4E615AA3-5E2D-4E34-883F-4119C9286FB5}"_drx_guid);

public:
	virtual ~CReflection();

	static CReflection& GetInstance();

	// Drx::IDefaultModule
	virtual const char* GetName() const override     { return "DrxReflection"; }
	virtual const char* GetCategory() const override { return "Plugin"; }
	virtual bool        Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override;
	// ~Drx::IDefaultModule

	// IReflectionRegistry
	virtual CReflectedTypeDesc*  Register(const DrxTypeDesc& typeDesc, const DrxGUID& guid, ReflectTypeFunction pReflectFunc, const SSourceFileInfo& srcPos) override;

	virtual TypeIndex::ValueType GetTypeCount() const override;
	virtual const ITypeDesc*     FindTypeByIndex(TypeIndex index) const override;
	virtual const ITypeDesc*     FindTypeByGuid(DrxGUID guid) const override;
	virtual const ITypeDesc*     FindTypeById(DrxTypeId typeId) const override;

	virtual size_t               GetSystemRegistriesCount() const override;
	virtual ISystemTypeRegistry* FindSystemRegistryByIndex(size_t index) const override;
	virtual ISystemTypeRegistry* FindSystemRegistryByGuid(const DrxGUID& guid) const override;
	virtual ISystemTypeRegistry* FindSystemRegistryById(DrxTypeId typeId) const override;

	virtual void                 RegisterSystemRegistry(ISystemTypeRegistry* pSystemRegistry) override;
	// ~IReflectionRegistry

private:
	CReflection();
	static CReflection* s_pInstance;

private:
	std::vector<CReflectedTypeDesc>                     m_typesByIndex;
	std::unordered_map<DrxGUID, TypeIndex>              m_typeIndicesByGuid;
	std::unordered_map<DrxTypeId::ValueType, TypeIndex> m_typeIndicesByTypeId;

	std::vector<ISystemTypeRegistry*>                   m_customRegistriesByIndex;
};

template<typename TYPE>
struct ReflectedTypeDesc
{
	static inline CReflectedTypeDesc* Of()
	{
		return gEnv->pReflection->FindTypeById(TypeIdOf<TYPE>());
	}
};

} // ~Drx namespace
} // ~Reflection namespace

template<typename TYPE>
using DrxReflectedTypeDesc = Drx::Reflection::ReflectedTypeDesc<TYPE>;
