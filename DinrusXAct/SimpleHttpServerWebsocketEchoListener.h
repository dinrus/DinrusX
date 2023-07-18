// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __SIMPLEHTTPSERVERWEBSOCKETECHOLISTENER_H__
#define __SIMPLEHTTPSERVERWEBSOCKETECHOLISTENER_H__

#pragma once

#if defined(HTTP_WEBSOCKETS)

class CSimpleHttpServerWebsocketEchoListener : public IHttpWebsocketProtocol
{
public:

	static CSimpleHttpServerWebsocketEchoListener& GetSingleton();

private:

	CSimpleHttpServerWebsocketEchoListener() {};
	~CSimpleHttpServerWebsocketEchoListener() {};

	void OnUpgrade(int connectionID);
	void OnReceive(int connectionID, SMessageData& data);
	void OnClosed(int connectionID, bool bGraceful);

	static CSimpleHttpServerWebsocketEchoListener s_singleton;
};

#endif // HTTP_WEBSOCKETS

#endif // __SIMPLEHTTPSERVERWEBSOCKETECHOLISTENER_H__
