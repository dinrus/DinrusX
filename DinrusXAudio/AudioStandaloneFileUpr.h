// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Audio/IAudioInterfacesCommonData.h>

#if defined(INCLUDE_AUDIO_PRODUCTION_CODE)
struct IRenderAuxGeom;
#endif //INCLUDE_AUDIO_PRODUCTION_CODE

namespace DrxAudio
{
namespace Impl
{
struct IImpl;
struct ITrigger;
} // namespace Impl

class CAudioStandaloneFileUpr final
{
public:

	CAudioStandaloneFileUpr() = default;
	~CAudioStandaloneFileUpr();

	CAudioStandaloneFileUpr(CAudioStandaloneFileUpr const&) = delete;
	CAudioStandaloneFileUpr(CAudioStandaloneFileUpr&&) = delete;
	CAudioStandaloneFileUpr& operator=(CAudioStandaloneFileUpr const&) = delete;
	CAudioStandaloneFileUpr& operator=(CAudioStandaloneFileUpr&&) = delete;

	void                         Init(Impl::IImpl* const pImpl);
	void                         Release();

	CATLStandaloneFile*          ConstructStandaloneFile(char const* const szFile, bool const bLocalized, Impl::ITrigger const* const pITrigger = nullptr);
	void                         ReleaseStandaloneFile(CATLStandaloneFile* const pStandaloneFile);

private:

	std::list<CATLStandaloneFile*> m_constructedStandaloneFiles;
	Impl::IImpl*                   m_pIImpl = nullptr;

#if defined(INCLUDE_AUDIO_PRODUCTION_CODE)
public:

	void DrawDebugInfo(IRenderAuxGeom& auxGeom, Vec3 const& listenerPosition, float posX, float posY) const;

#endif // INCLUDE_AUDIO_PRODUCTION_CODE
};
} // namespace DrxAudio
