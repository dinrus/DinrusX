// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXReflection/IReflection.h>
#include <DinrusX/DinrusXReflection/IVariableDesc.h>
#include <DinrusX/CoreX/Extension/DrxGUID.h>
#include <DinrusX/CoreX/String/DrxString.h>

#include "DescExtension.h"

namespace Drx {
namespace Reflection {

class CReflectedTypeDesc;

class CReflectedVariableDesc : public CExtensibleDesc<IVariableDesc>
{
public:
	CReflectedVariableDesc(void* pAddress, const char* szLabel, DrxTypeId typeId, const DrxGUID& guid, const SSourceFileInfo& srcInfo);
	CReflectedVariableDesc(ptrdiff_t offset, const char* szLabel, const CReflectedTypeDesc& parentTypeDesc, DrxTypeId typeId, const DrxGUID& guid, const SSourceFileInfo& srcInfo);

	// IVariableDesc
	virtual const DrxGUID&         GetGuid() const                           { return m_guid; }
	virtual DrxTypeId              GetTypeId() const                         { return m_typeId; }

	virtual const char*            GetLabel() const                          { return m_label.c_str(); }

	virtual void                   SetDescription(const char* szDescription) { m_description = szDescription; }
	virtual const char*            GetDescription() const                    { return m_description.c_str(); }

	virtual bool                   IsMemberVariable() const                  { return (m_pParentTypeDesc != nullptr); }
	virtual ptrdiff_t              GetOffset() const                         { return (!IsMemberVariable() ? m_offset : std::numeric_limits<ptrdiff_t>::min()); }
	virtual void*                  GetPointer() const                        { return (IsMemberVariable() ? m_pAddress : nullptr); }

	virtual const SSourceFileInfo& GetSourceInfo() const                     { return m_sourceInfo; }
	// ~IVariableDesc

private:
	const DrxGUID             m_guid;
	const CReflectedTypeDesc* m_pParentTypeDesc;
	SSourceFileInfo           m_sourceInfo;

	string                    m_label;
	string                    m_description;

	union
	{
		ptrdiff_t m_offset;
		void*     m_pAddress;
	};

	DrxTypeId m_typeId;
};

inline CReflectedVariableDesc::CReflectedVariableDesc(void* pAddress, const char* szLabel, DrxTypeId typeId, const DrxGUID& guid, const SSourceFileInfo& srcInfo)
	: m_typeId(typeId)
	, m_pParentTypeDesc(nullptr)
	, m_pAddress(pAddress)
	, m_guid(guid)
	, m_label(szLabel)
	, m_sourceInfo(srcInfo)
{}

inline CReflectedVariableDesc::CReflectedVariableDesc(ptrdiff_t offset, const char* szLabel, const CReflectedTypeDesc& parentTypeDesc, DrxTypeId typeId, const DrxGUID& guid, const SSourceFileInfo& srcInfo)
	: m_typeId(typeId)
	, m_pParentTypeDesc(&parentTypeDesc)
	, m_offset(offset)
	, m_guid(guid)
	, m_label(szLabel)
	, m_sourceInfo(srcInfo)
{}

} // ~Drx namespace
} // ~Reflection namespace
