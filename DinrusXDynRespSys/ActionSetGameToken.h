// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/String/HashedString.h>
#include <DinrusX/DinrusXDynRespSys/IDynamicResponseAction.h>

namespace DrxDRS
{
class CActionSetGameToken final : public DRS::IResponseAction
{
public:
	enum class EValueToSet
	{
		StringValue = 0,
		BoolValue = 1,
		IntValue = 2,
		FloatValue = 3,
		IntVariableValue = 4,
		FloatVariableValue = 5,
		BoolVariableValue = 6,
		CurrentActorName = 7
	};

	CActionSetGameToken() = default;
	//CActionSetGameToken(const string& tokenName, const string& stringValue, bool bCreate) : m_tokenName(tokenName), m_bCreateTokenIfNotExisting(bCreate) {}

	//////////////////////////////////////////////////////////
	// IResponseAction implementation
	virtual DRS::IResponseActionInstanceUniquePtr Execute(DRS::IResponseInstance* pResponseInstance) override;
	virtual string                                GetVerboseInfo() const override;
	virtual void                                  Serialize(Serialization::IArchive& ar) override;
	virtual const char*                           GetType() const override        { return "Set GameToken Value"; }
	//////////////////////////////////////////////////////////

private:
	string m_tokenName;
	bool   m_bCreateTokenIfNotExisting = false;

	EValueToSet m_typeToSet = EValueToSet::StringValue;

	string m_stringValueToSet;
	CHashedString m_collectionName;
	CHashedString m_variableName;
	union
	{
		bool m_bBooleanValueToSet;
		int m_intValueToSet;
		float m_floatValueToSet;
	};
};
}  //namespace DrxDRS
