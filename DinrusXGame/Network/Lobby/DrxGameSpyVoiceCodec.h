// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#ifndef __DRXGAMESPYVOICECODEC_H__
#define __DRXGAMESPYVOICECODEC_H__

#include <DinrusX/CoreX/Lobby/IDrxLobby.h>

#if USE_DRXLOBBY_GAMESPY && USE_DRXLOBBY_GAMESPY_VOIP

extern "C" 
{
#include <DinrusX/DinrusXGame/Gamespy2.11.00/Source/Voice2/gvCodec.h>
};

#define GAMESPY_USE_SPEEX_CODEC				1

#if GAMESPY_USE_SPEEX_CODEC

#include <DinrusX/DinrusXGame/speex-1.0.5/include/speex.h>

#endif

// windef.h #defines this as GetUserNameA or GetUserNameW but it's a method of ISystem
#undef GetUserName

class CDrxGameSpyVoiceCodec
{
public:
	static void*			Initialise(void);
	static void				Terminate(void);

protected:
	static SDrxGameSpyVoiceCodecInfo m_info;

#if GAMESPY_USE_SPEEX_CODEC
	static	GVBool		SpeexNewDecoderCallback(GVDecoderData* pData);
	static	void			SpeexFreeDecoderCallback(GVDecoderData data);
	static	void			SpeexEncodeCallback(GVByte* pOut, const GVSample* pIn);
	static	void			SpeexDecodeAddCallback(GVSample* pOut, const GVByte* pIn, GVDecoderData data);
	static	void			SpeexDecodeSetCallback(GVSample* pOut, const GVByte* pIn, GVDecoderData data);

protected:

	static GVCustomCodecInfo m_speexCodecInfo;

	static SpeexBits	m_speexBits;
	static void*			m_pspeexEncoderState;
	static int16*			m_pspeexBuffer;
	static int				m_speexSamplesPerFrame;
	static int				m_speexEncodedFrameSize;

#endif // GAMESPY_USE_SPEEX_CODEC
};

#endif // USE_DRXLOBBY_GAMESPY && USE_DRXLOBBY_GAMESPY_VOIP
#endif // __DRXGAMESPYVOICECODEC_H__
// [EOF]
