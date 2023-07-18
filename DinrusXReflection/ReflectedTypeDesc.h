// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXReflection/IReflection.h>
#include <DinrusX/DinrusXReflection/ITypeDesc.h>
#include <DinrusX/DinrusXReflection/Function.h>
#include <DinrusX/CoreX/Extension/DrxGUID.h>
#include <DinrusX/CoreX/String/DrxString.h>
#include <DinrusX/DinrusXSys/ISystem.h>

#include <unordered_map>
#include <vector>

#include "DescExtension.h"

namespace Drx {
namespace Reflection {

class CReflectedFunctionDesc;
class CReflectedVariableDesc;

class CEnumValueDesc : public IEnumValueDesc
{
public:
	CEnumValueDesc()
		: m_value(0)
	{}

	CEnumValueDesc(const char* szLabel, size_t value, const char* szDescription)
		: m_label(szLabel)
		, m_description(szDescription)
		, m_value(value)
	{}

	// IEnumValueDesc
	const char* GetLabel() const       { return m_label.c_str(); }
	const char* GetDescription() const { return m_description.c_str(); }
	size_t      GetValue() const       { return m_value; }
	// ~IEnumValueDesc

	bool operator==(const CEnumValueDesc& rhs) const
	{
		return m_label == rhs.m_label && m_value == rhs.m_value;
	}

	bool operator!=(const CEnumValueDesc& rhs) const
	{
		return m_label != rhs.m_label || m_value != rhs.m_value;
	}

private:
	string m_label;
	string m_description;
	size_t m_value;
};

class CReflectedTypeDesc : public CExtensibleDesc<ITypeDesc>
{
public:
	CReflectedTypeDesc();
	CReflectedTypeDesc(const DrxGUID& guid, const DrxTypeDesc& typeDesc);
	CReflectedTypeDesc(CReflectedTypeDesc&& rhs);

	virtual ~CReflectedTypeDesc() {}

	virtual DrxTypeId              GetTypeId() const override                         { return m_typeDesc.GetTypeId(); }
	virtual const DrxTypeDesc&     GetTypeDesc() const override                       { return m_typeDesc; }

	virtual bool                   IsAbstract() const override                        { return m_typeDesc.IsAbstract(); }
	virtual bool                   IsArray() const override                           { return m_typeDesc.IsArray(); }
	virtual bool                   IsClass() const override                           { return m_typeDesc.IsClass(); }
	virtual bool                   IsEnum() const override                            { return m_typeDesc.IsEnum(); }
	virtual bool                   IsFundamental() const override                     { return m_typeDesc.IsFundamental(); }
	virtual bool                   IsUnion() const override                           { return m_typeDesc.IsUnion(); }

	virtual const DrxGUID&         GetGuid() const override                           { return m_guid; }
	virtual ETypeCategory          GetCategory() const override                       { return m_category; }
	virtual TypeIndex              GetIndex() const override                          { return m_index; }

	virtual const char*            GetRawName() const override                        { return m_typeDesc.GetRawName(); }
	virtual void                   SetLabel(const char* szLabel)    override          { m_label = szLabel; }
	virtual const char*            GetLabel() const override                          { return m_label; }

	virtual void                   SetDescription(const char* szDescription) override { m_description = szDescription; }
	virtual const char*            GetDescription() const override                    { return m_description; }

	virtual uint16                 GetSize() const override                           { return m_typeDesc.GetSize(); }
	virtual uint16                 GetAlignment() const override                      { return m_typeDesc.GetAlignment(); }

	virtual const SSourceFileInfo& GetSourceInfo() const override                     { return m_sourcePos; }

	// Class Members
	virtual bool                 AddBaseType(DrxTypeId typeId, const SSourceFileInfo& srcInfo) override;
	virtual size_t               GetBaseTypeCount() const override { return IsClass() ? m_classMembers.baseTypesByIndex.size() : 0; }
	virtual const ITypeDesc*     GetBaseTypeByIndex(size_t index) const override;

	virtual IFunctionDesc*       AddFunction(const CMemberFunction& function, const char* szLabel, const DrxGUID& guid, const SSourceFileInfo& srcInfo) override;
	virtual size_t               GetFunctionCount() const override { return IsClass() ? m_classMembers.functionsByIndex.size() : 0; }
	virtual const IFunctionDesc* GetFunctionByIndex(size_t index) const override;

	virtual IVariableDesc*       AddVariable(DrxTypeId typeId, ptrdiff_t offset, const char* szLabel, const DrxGUID& guid, const SSourceFileInfo& srcInfo) override;
	virtual size_t               GetVariableCount() const override { return IsClass() ? m_classMembers.variablesByIndex.size() : 0; }
	virtual const IVariableDesc* GetVariableByIndex(size_t index) const override;

	// Enum Members
	virtual IEnumValueDesc*       AddEnumValue(const char* szName, size_t value, const char* szDescription) override;
	virtual size_t                GetEnumValueCount() const override { return IsEnum() ? m_enumMembers.valuesByIndex.size() : 0; }
	virtual const IEnumValueDesc* GetEnumValueByIndex(size_t index) const override;

	// Generic Members
	virtual Type::CDefaultConstructor GetDefaultConstructor() const override { return m_typeDesc.GetDefaultConstructor(); }
	virtual Type::CCopyConstructor    GetCopyConstructor() const override    { return m_typeDesc.GetCopyConstructor(); }
	virtual Type::CMoveConstructor    GetMoveConstructor() const override    { return m_typeDesc.GetMoveConstructor(); }

	virtual Type::CDestructor         GetDestructor() const override         { return m_typeDesc.GetDestructor(); }

	virtual Type::CCopyAssignOperator GetCopyAssignOperator() const override { return m_typeDesc.GetCopyAssignOperator(); }
	virtual Type::CEqualOperator      GetEqualOperator() const override      { return m_typeDesc.GetEqualOperator(); }

	virtual bool                      AddConversionOperator(DrxTypeId typeId, Type::CConversionOperator conversionOperator) override;
	virtual Type::CConversionOperator GetConversionOperator(DrxTypeId typeId) override;

	// Operators
	bool operator==(const CReflectedTypeDesc& rhs) const
	{
		return m_guid == rhs.GetGuid();
	}

	bool operator!=(const CReflectedTypeDesc& rhs) const
	{
		return m_guid != rhs.GetGuid();
	}

private:
	friend class CReflection;

private:
	DrxTypeDesc     m_typeDesc;

	DrxGUID         m_guid;

	string          m_label;
	string          m_description;

	SSourceFileInfo m_sourcePos;
	ETypeCategory   m_category;
	TypeIndex       m_index;

	std::unordered_map<DrxTypeId::ValueType, Type::CConversionOperator> m_conversionsByTypeId;

	union
	{
		struct
		{
			std::vector<DrxTypeId>               baseTypesByIndex;
			std::vector<CReflectedFunctionDesc*> functionsByIndex;
			std::vector<CReflectedVariableDesc*> variablesByIndex;
		} m_classMembers;

		struct
		{
			std::vector<CEnumValueDesc*> valuesByIndex;
		} m_enumMembers;
	};
};

} // ~Reflection namespace
} // ~Drx namespace
