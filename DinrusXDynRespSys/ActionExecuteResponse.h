// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/String/HashedString.h>
#include <DinrusX/DinrusXDynRespSys/IDynamicResponseAction.h>
#include <DinrusX/DinrusXDynRespSys/IDynamicResponseSystem.h>

namespace DrxDRS
{
	class CResponseInstance;

class CActionExecuteResponse final : public DRS::IResponseAction
{
public:
	//////////////////////////////////////////////////////////
	// IResponseAction implementation
	virtual DRS::IResponseActionInstanceUniquePtr Execute(DRS::IResponseInstance* pResponseInstance) override;
	virtual string                                GetVerboseInfo() const override { return " '" + m_responseID.GetText() + "'"; }
	virtual void                                  Serialize(Serialization::IArchive& ar) override;
	virtual const char*                           GetType() const override        { return "Execute Response"; }
	//////////////////////////////////////////////////////////

private:
	CHashedString m_responseID;
};

//////////////////////////////////////////////////////////

class CActionExecuteResponseInstance final : public DRS::IResponseActionInstance, public DRS::IResponseUpr::IListener
{
public:
	CActionExecuteResponseInstance() : m_state(CS_RUNNING), m_pStartedResponse(nullptr) {}
	virtual ~CActionExecuteResponseInstance() override;

	//////////////////////////////////////////////////////////
	// IResponseActionInstance implementation
	virtual eCurrentState Update() override;
	virtual void          Cancel() override;
	//////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////
	// DRS::IResponseUpr::IListener implementation
	virtual void OnSignalProcessingStarted(SSignalInfos& signal, DRS::IResponseInstance* pStartedResponse) override;
	virtual void OnSignalProcessingFinished(SSignalInfos& signal, DRS::IResponseInstance* pFinishedResponse, eProcessingResult outcome) override;
	//////////////////////////////////////////////////////////

private:
	CResponseInstance* m_pStartedResponse;
	eCurrentState m_state;
};
}  //namespace DrxDRS
