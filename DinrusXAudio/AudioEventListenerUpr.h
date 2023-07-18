// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXAudio/AudioInternalInterfaces.h>

namespace DrxAudio
{
class CAudioEventListenerUpr final
{
public:

	CAudioEventListenerUpr() = default;
	~CAudioEventListenerUpr();

	CAudioEventListenerUpr(CAudioEventListenerUpr const&) = delete;
	CAudioEventListenerUpr(CAudioEventListenerUpr&&) = delete;
	CAudioEventListenerUpr& operator=(CAudioEventListenerUpr const&) = delete;
	CAudioEventListenerUpr& operator=(CAudioEventListenerUpr&&) = delete;

	ERequestStatus              AddRequestListener(SAudioUprRequestData<EAudioUprRequestType::AddRequestListener> const* const pRequestData);
	ERequestStatus              RemoveRequestListener(void (* func)(SRequestInfo const* const), void const* const pObjectToListenTo);
	void                        NotifyListener(SRequestInfo const* const pRequestInfo);

#if defined(INCLUDE_AUDIO_PRODUCTION_CODE)
	size_t GetNumEventListeners() const { return m_listeners.size(); }
#endif //INCLUDE_AUDIO_PRODUCTION_CODE

private:

	using ListenerArray = std::vector<SAudioEventListener>;
	ListenerArray m_listeners;
};
} // namespace DrxAudio
