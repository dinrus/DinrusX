// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXNetwork/INetwork.h>

class CGameClientChannel;
class CGameContext;
struct IGameFramework;

class CGameClientNub final :
	public IGameClientNub
{
public:
	CGameClientNub(IGameFramework* pFramework) : m_pGameContext(0), m_pClientChannel(0), m_pFramework(pFramework), m_fInactivityTimeout(300.0f) {};
	virtual ~CGameClientNub();

	// IGameNub
	virtual void                 Release();
	virtual SCreateChannelResult CreateChannel(INetChannel* pChannel, const char* pRequest);
	virtual void                 FailedActiveConnect(EDisconnectionCause cause, const char* description);
	// ~IGameNub

	// IGameClientNub
	virtual INetChannel* GetNetChannel();
	// ~IGameClientNub

	void                Disconnect(EDisconnectionCause cause, const char* msg);
	void                SetGameContext(CGameContext* pGameContext) { m_pGameContext = pGameContext; };
	CGameClientChannel* GetGameClientChannel() const               { return m_pClientChannel; }

	void                ClientChannelClosed();

	void                GetMemoryUsage(IDrxSizer* s) const;

private:
	CGameContext*       m_pGameContext;
	CGameClientChannel* m_pClientChannel;
	IGameFramework*     m_pFramework;
	float               m_fInactivityTimeout;
};