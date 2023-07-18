// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXFlowGraph/StdAfx.h>

#include <DinrusX/DinrusXFlowGraph/IFlowBaseNode.h>

class CMovieUpr;

class CMovieInstance
{
public:
	void AddRef() { ++m_nRefs; }
	void Release();

private:
	int            m_nRefs;
	CMovieUpr* m_pUpr;
};

class CMovieUpr
{
public:
	void ReleaseInstance(CMovieInstance* pInstance)
	{
	}
};

void CMovieInstance::Release()
{
	if (0 == --m_nRefs)
	{
		m_pUpr->ReleaseInstance(this);
	}
}

class CFlowNode_Movie : public CFlowBaseNode<eNCT_Singleton>
{
public:
	CFlowNode_Movie(SActivationInfo* pActInfo)
	{
	}

	enum EInputPorts
	{
		eIP_TimeOfDay = 0,
	};

	enum EOutputPorts
	{
		eOP_SunDirection = 0,
	};

	void GetConfiguration(SFlowNodeConfig& config)
	{
	}

	void ProcessEvent(EFlowEvent event, SActivationInfo* pActInfo)
	{
		switch (event)
		{
		case eFE_Activate:
			break;
		}
	}

	virtual void GetMemoryUsage(IDrxSizer* s) const
	{
		s->Add(*this);
	}

private:
	void Reload(const char* filename);

};

// REGISTER_FLOW_NODE("Environment:Movie", CFlowNode_Movie);
