// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef _GFXVIDEOSOUNDDRXSOUND_H_
#define _GFXVIDEOSOUNDDRXSOUND_H_

#pragma once

#ifdef INCLUDE_SCALEFORM_SDK

namespace DrxVideoSoundSystem
{
	struct IChannelDelegate
	{
		// <interfuscator:shuffle>
		virtual void Release() = 0;

		virtual bool Stop()                = 0;
		virtual bool SetPaused(bool pause) = 0;

		virtual bool SetVolume(float volume) = 0;
		virtual bool Mute(bool mute)         = 0;

		virtual bool SetBytePosition(unsigned int bytePos)  = 0;
		virtual bool GetBytePosition(unsigned int& bytePos) = 0;

		virtual bool SetSpeakerMix(float fl, float fr, float c, float lfe, float bl, float br, float sl, float sr) = 0;
		// </interfuscator:shuffle>

	protected:
		virtual ~IChannelDelegate() {}
	};

	struct ISoundDelegate
	{
		struct LockRange
		{
			void* p0;
			void* p1;
			unsigned int length0;
			unsigned int length1;
		};

		// <interfuscator:shuffle>
		virtual void Release() = 0;

		virtual IChannelDelegate* Play() = 0;

		virtual bool Lock(unsigned int offset, unsigned int length, LockRange& lr) = 0;
		virtual bool Unlock(const LockRange& lr) = 0;
		// </interfuscator:shuffle>

	protected:
		virtual ~ISoundDelegate() {}
	};

	struct IPlayerDelegate
	{
		// <interfuscator:shuffle>
		virtual void Release() = 0;

		virtual ISoundDelegate* CreateSound(unsigned int numChannels, unsigned int sampleRate, unsigned int lengthInBytes) = 0;

		virtual bool MuteMainTrack() const = 0;
		// </interfuscator:shuffle>

	protected:
		virtual ~IPlayerDelegate() {}
	};

	struct IAllocatorDelegate
	{
		// <interfuscator:shuffle>
		virtual void* Allocate(size_t size) = 0;
		virtual void  Free(void* p)         = 0;
		// </interfuscator:shuffle>

	protected:
		virtual ~IAllocatorDelegate() {}
	};
}                                        // namespace DrxVideoSoundSystem

#endif                                   //#ifdef INCLUDE_SCALEFORM_SDK

#ifdef INCLUDE_SCALEFORM_SDK

#include <DinrusX/DinrusXSys/ConfigScaleform.h>

#if defined(USE_GFX_VIDEO)

#pragma warning(push)
#pragma warning(disable : 6326)          // Potential comparison of a constant with another constant
#pragma warning(disable : 6011)          // Dereferencing NULL pointer
#include <DinrusX/CoreX/Platform/DrxWindows.h>
#include <GFxSystemSoundInterface.h>     // includes <windows.h>
#pragma warning(pop)

class GFxVideoDrxSoundSystemImpl;

class GFxVideoDrxSoundSystem:public GFxVideoSoundSystem
{
public:
	virtual GFxVideoSound* Create(GFxVideoPlayer::SoundTrack type);

public:
	GFxVideoDrxSoundSystem(GMemoryHeap* pHeap);
	virtual ~GFxVideoDrxSoundSystem();

public:
	static void InitCVars();

private:
	GFxVideoDrxSoundSystemImpl* m_pImpl;
};

#else

class GFxVideoDrxSoundSystem
{
public:
	static void InitCVars();

};

#endif   // #if defined(USE_GFX_VIDEO)

#endif   // #ifdef INCLUDE_SCALEFORM_SDK

#endif   // #ifndef _GFXVIDEOSOUNDDRXSOUND_H_
