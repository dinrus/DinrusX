// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __IVOICEPACKETLISTENER_H__
#define __IVOICEPACKETLISTENER_H__

#if !defined(OLD_VOICE_SYSTEM_DEPRECATED)

	#include <DinrusX/DinrusXNetwork/VoicePacket.h>
	#include <DinrusX/DinrusXNetwork/INetwork.h>

struct IVoicePacketListener
{
	virtual ~IVoicePacketListener(){}
	virtual void AddRef() = 0;
	virtual void Release() = 0;

	virtual void OnVoicePacket(SNetObjectID object, const TVoicePacketPtr& pPkt) = 0;
};

typedef _smart_ptr<IVoicePacketListener> IVoicePacketListenerPtr;

#endif
#endif
