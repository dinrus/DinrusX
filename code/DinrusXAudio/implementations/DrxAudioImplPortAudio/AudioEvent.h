// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXAudio/ATLEntityData.h>
#include <DinrusX/3rdParty/portaudio/portaudio.h>
#include <atomic>
#include <DinrusX/DinrusXAudio/PoolObject.h>

// Forward declare C struct
struct sf_private_tag;
using SNDFILE = struct sf_private_tag;

namespace DrxAudio
{
namespace Impl
{
namespace PortAudio
{
class CObject;

class CEvent final : public IEvent, public CPoolObject<CEvent, stl::PSyncNone>
{
public:

	explicit CEvent(CATLEvent& event_);
	virtual ~CEvent() override;

	CEvent(CEvent const&) = delete;
	CEvent(CEvent&&) = delete;
	CEvent& operator=(CEvent const&) = delete;
	CEvent& operator=(CEvent&&) = delete;

	bool    Execute(
	  int const numLoops,
	  double const sampleRate,
	  DrxFixedStringT<MaxFilePathLength> const& filePath,
	  PaStreamParameters const& streamParameters);
	void Update();

	// DrxAudio::Impl::IEvent
	virtual ERequestStatus Stop() override;
	// ~DrxAudio::Impl::IEvent

	SNDFILE*          pSndFile;
	PaStream*         pStream;
	void*             pData;
	CObject*          pObject;
	int               numChannels;
	int               remainingLoops;
	CATLEvent&        event;
	uint32            pathId;
	PaSampleFormat    sampleFormat;
	std::atomic<bool> bDone;
};
} // namespace PortAudio
} // namespace Impl
} // namespace DrxAudio
