// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXNetwork/StdAfx.h>
#include <DinrusX/DinrusXNetwork/NetHelpers.h>
#include  <DinrusX/DinrusXNetwork/CET_Server.h>
#include  <DinrusX/DinrusXNetwork/NetChannel.h>
#include  <DinrusX/DinrusXNetwork/ContextView.h>
#include  <DinrusX/DinrusXNetwork/ClientContextView.h>

class CCET_PostSpawnObjects : public CCET_Base
{
public:
	CCET_PostSpawnObjects() { ++g_objcnt.postSpawnObjects; }
	~CCET_PostSpawnObjects() { --g_objcnt.postSpawnObjects; }

	const char* GetName()
	{
		return "PostInitObjects";
	}

	EContextEstablishTaskResult OnStep(SContextEstablishState& state)
	{
		CNetChannel* pChannel = (CNetChannel*) state.pSender;
		if (pChannel)
		{
			// TODO: tidy up
			SCOPED_GLOBAL_LOCK;
			pChannel->GetContextView()->ContextState()->GC_SendPostSpawnEntities(pChannel->GetContextView());
			return eCETR_Ok;
		}
		return eCETR_Failed;
	}
};

void AddPostSpawnObjects(IContextEstablisher* pEst, EContextViewState state)
{
	pEst->AddTask(state, new CCET_PostSpawnObjects);
}
