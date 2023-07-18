// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXAct/StdAfx.h>
#include <DinrusX/CoreX/Extension/ClassWeaver.h>
#include <DinrusX/DinrusXAct/IDrxMannequin.h>
#include <DinrusX/DinrusXAct/Serialization.h>
#include <DinrusX/DinrusXSys/ArchiveHost.h>

class CProceduralParamsComparerDefault
	: public IProceduralParamsComparer
{
public:
	DRXINTERFACE_BEGIN()
	DRXINTERFACE_ADD(IProceduralParamsComparer)
	DRXINTERFACE_END()

	DRXGENERATE_CLASS_GUID(CProceduralParamsComparerDefault, "ProceduralParamsComparerDefault", "fc53bd92-4853-4faa-ab0f-b42b24e55b3e"_drx_guid)

	virtual ~CProceduralParamsComparerDefault() {}

	virtual bool Equal(const IProceduralParams& lhs, const IProceduralParams& rhs) const override
	{
		return Serialization::CompareBinary(lhs, rhs);
	}
};

DRXREGISTER_CLASS(CProceduralParamsComparerDefault);
