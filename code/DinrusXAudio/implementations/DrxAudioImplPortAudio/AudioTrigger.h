// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/DinrusXAudio/ATLEntityData.h>
#include <DinrusX/3rdParty/portaudio/portaudio.h>

namespace DrxAudio
{
namespace Impl
{
namespace PortAudio
{
enum class EEventType : EnumFlagsType
{
	None,
	Start,
	Stop,
};

class CTrigger final : public ITrigger
{
public:

	explicit CTrigger(
	  uint32 const pathId_,
	  int const numLoops_,
	  double const sampleRate_,
	  EEventType const eventType_,
	  DrxFixedStringT<MaxFilePathLength> const& filePath_,
	  PaStreamParameters const& streamParameters_)
		: pathId(pathId_)
		, numLoops(numLoops_)
		, sampleRate(sampleRate_)
		, eventType(eventType_)
		, filePath(filePath_)
		, streamParameters(streamParameters_)
	{}

	virtual ~CTrigger() override = default;

	CTrigger(CTrigger const&) = delete;
	CTrigger& operator=(CTrigger const&) = delete;

	// DrxAudio::Impl::ITrigger
	virtual ERequestStatus Load() const override                             { return ERequestStatus::Success; }
	virtual ERequestStatus Unload() const override                           { return ERequestStatus::Success; }
	virtual ERequestStatus LoadAsync(IEvent* const pIEvent) const override   { return ERequestStatus::Success; }
	virtual ERequestStatus UnloadAsync(IEvent* const pIEvent) const override { return ERequestStatus::Success; }
	// ~DrxAudio::Impl::ITrigger

	uint32 const                             pathId;
	int const                                numLoops;
	double const                             sampleRate;
	EEventType const                         eventType;
	DrxFixedStringT<MaxFilePathLength> const filePath;
	PaStreamParameters const                 streamParameters;
};
} // namespace PortAudio
} // namespace Impl
} // namespace DrxAudio
