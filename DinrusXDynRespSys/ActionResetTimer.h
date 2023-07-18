// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/************************************************************************

   this action resets a counter to the current time, so that it can be used in TimeSince Conditions later on

   /************************************************************************/

#pragma once

#include <DinrusX/DinrusXDynRespSys/IDynamicResponseAction.h>
#include "Variable.h"

namespace DrxDRS
{
class CVariableCollection;

class CActionResetTimerVariable final : public IVariableUsingBase, public DRS::IResponseAction
{
public:
	CActionResetTimerVariable() = default;
	CActionResetTimerVariable(const CHashedString& collection, const CHashedString& variableName) {}

	//////////////////////////////////////////////////////////
	// IResponseAction implementation
	virtual DRS::IResponseActionInstanceUniquePtr Execute(DRS::IResponseInstance* pResponseInstance) override;
	virtual string                                GetVerboseInfo() const override;
	virtual void                                  Serialize(Serialization::IArchive& ar) override;
	virtual const char*                           GetType() const override { return "ResetTimer"; }
	//////////////////////////////////////////////////////////

private:
};
} // namespace DrxDRS
