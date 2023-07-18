// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DrxDynamicResponseSystem/IDynamicResponseCondition.h>
#include <DrxDynamicResponseSystem/IDynamicResponseSystem.h>

namespace DRS
{
	struct IResponseActor;
	struct IVariableCollection;
}

class CConditionDistanceToEntity final : public DRS::IResponseCondition
{
public:
	CConditionDistanceToEntity();
	CConditionDistanceToEntity(const string& actorName);
	virtual ~CConditionDistanceToEntity();

	//////////////////////////////////////////////////////////
	// IResponseCondition implementation
	virtual bool IsMet(DRS::IResponseInstance* pResponseInstance) override;
	virtual void Serialize(Serialization::IArchive& ar) override;
	virtual string GetVerboseInfo() const override;
	virtual const char* GetType() const override { return "Distance to Entity"; }
	//////////////////////////////////////////////////////////

private:
	float m_squaredDistance;
	string m_entityName;
};
