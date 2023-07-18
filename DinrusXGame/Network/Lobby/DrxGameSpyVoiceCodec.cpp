// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXGame/StdAfx.h>

#include <DinrusX/DinrusXGame/DrxGameSpyVoiceCodec.h>

#if USE_DRXLOBBY_GAMESPY && USE_DRXLOBBY_GAMESPY_VOIP

SDrxGameSpyVoiceCodecInfo CDrxGameSpyVoiceCodec::m_info;

#if GAMESPY_USE_SPEEX_CODEC

#ifdef _DEBUG
#pragma comment( lib, "libspeex-1.0.5_d.lib" )
#else
#pragma comment( lib, "libspeex-1.0.5.lib" )
#endif // _DEBUG

#define GAMESPY_SPEEX_SAMPLE_RATE (8000)
#define GAMESPY_SPEEX_QUALITY (4)
#define GAMESPY_SPEEX_PERCEPTUAL_ENHANCEMENT (1)

GVCustomCodecInfo CDrxGameSpyVoiceCodec::m_speexCodecInfo;

SpeexBits	CDrxGameSpyVoiceCodec::m_speexBits;
void*			CDrxGameSpyVoiceCodec::m_pspeexEncoderState = NULL;
int16*		CDrxGameSpyVoiceCodec::m_pspeexBuffer = NULL;
int				CDrxGameSpyVoiceCodec::m_speexSamplesPerFrame = 0;
int				CDrxGameSpyVoiceCodec::m_speexEncodedFrameSize = 0;

#endif// GAMESPY_USE_SPEEX_CODEC


void* CDrxGameSpyVoiceCodec::Initialise(void)
{
	m_info.m_pCustomCodecInfo = NULL;
	m_info.m_pTerminateCallback = CDrxGameSpyVoiceCodec::Terminate;

#if GAMESPY_USE_SPEEX_CODEC
	m_pspeexEncoderState = speex_encoder_init(&speex_nb_mode);
	if (m_pspeexEncoderState != NULL)
	{
		int samplesPerSecond = GAMESPY_SPEEX_SAMPLE_RATE;
		int quality = GAMESPY_SPEEX_QUALITY;

		speex_encoder_ctl(m_pspeexEncoderState, SPEEX_SET_SAMPLING_RATE, &samplesPerSecond);
		speex_encoder_ctl(m_pspeexEncoderState, SPEEX_GET_FRAME_SIZE, &m_speexSamplesPerFrame);
		speex_encoder_ctl(m_pspeexEncoderState, SPEEX_SET_QUALITY, &quality);
		speex_bits_init(&m_speexBits);

		int rate;
		speex_encoder_ctl(m_pspeexEncoderState, SPEEX_GET_BITRATE, &rate);
		int bitsPerFrame = (rate / (GAMESPY_SPEEX_SAMPLE_RATE / m_speexSamplesPerFrame));
		m_speexEncodedFrameSize = (bitsPerFrame / 8);
		if (bitsPerFrame % 8)
		{
			++m_speexEncodedFrameSize;
		}

		m_pspeexBuffer = new int16[m_speexSamplesPerFrame];
		if (m_pspeexBuffer)
		{
			m_speexCodecInfo.m_samplesPerFrame = m_speexSamplesPerFrame;
			m_speexCodecInfo.m_encodedFrameSize = m_speexEncodedFrameSize;
			m_speexCodecInfo.m_newDecoderCallback = CDrxGameSpyVoiceCodec::SpeexNewDecoderCallback;
			m_speexCodecInfo.m_freeDecoderCallback = CDrxGameSpyVoiceCodec::SpeexFreeDecoderCallback;
			m_speexCodecInfo.m_encodeCallback = CDrxGameSpyVoiceCodec::SpeexEncodeCallback;
			m_speexCodecInfo.m_decodeAddCallback = CDrxGameSpyVoiceCodec::SpeexDecodeAddCallback;
			m_speexCodecInfo.m_decodeSetCallback = CDrxGameSpyVoiceCodec::SpeexDecodeSetCallback;

			m_info.m_pCustomCodecInfo = &m_speexCodecInfo;
		}
	}
#endif// GAMESPY_USE_SPEEX_CODEC

	return &m_info;
}

void CDrxGameSpyVoiceCodec::Terminate(void)
{
#if GAMESPY_USE_SPEEX_CODEC
	speex_encoder_destroy(m_pspeexEncoderState);
	m_pspeexEncoderState = NULL;

	speex_bits_destroy(&m_speexBits);

	delete[] m_pspeexBuffer;
	m_pspeexBuffer = NULL;
#endif // GAMESPY_USE_SPEEX_CODEC
}

#if GAMESPY_USE_SPEEX_CODEC
GVBool CDrxGameSpyVoiceCodec::SpeexNewDecoderCallback(GVDecoderData* pData)
{
	GVBool done = GVFalse;

	void* pDecoder = speex_decoder_init(&speex_nb_mode);
	if (pDecoder != NULL)
	{
		int perceptualEnhancement = GAMESPY_SPEEX_PERCEPTUAL_ENHANCEMENT;
		speex_decoder_ctl(pDecoder, SPEEX_SET_ENH, &perceptualEnhancement);
		*pData = pDecoder;
		done = GVTrue;
	}

	return done;
}

void CDrxGameSpyVoiceCodec::SpeexFreeDecoderCallback(GVDecoderData data)
{
	speex_decoder_destroy(data);
}

void CDrxGameSpyVoiceCodec::SpeexEncodeCallback(GVByte* pOut, const GVSample* pIn)
{
	speex_bits_reset(&m_speexBits);

	speex_encode_int(m_pspeexEncoderState, (short*)pIn, &m_speexBits);

	int bytesWritten = speex_bits_write(&m_speexBits, (char*)pOut, m_speexEncodedFrameSize);
	DRX_ASSERT(bytesWritten == m_speexEncodedFrameSize);
}

void CDrxGameSpyVoiceCodec::SpeexDecodeAddCallback(GVSample* pOut, const GVByte* pIn, GVDecoderData data)
{
	SpeexDecodeSetCallback(m_pspeexBuffer, pIn, data);

	for (uint32 index = 0; index < m_speexSamplesPerFrame; ++index)
	{
		pOut[index] += m_pspeexBuffer[index];
	}
}

void CDrxGameSpyVoiceCodec::SpeexDecodeSetCallback(GVSample* pOut, const GVByte* pIn, GVDecoderData data)
{
	speex_bits_read_from(&m_speexBits, (char*)pIn, m_speexEncodedFrameSize);

	int rcode = speex_decode_int(data, &m_speexBits, pOut);
	DRX_ASSERT(rcode == 0);
}
#endif // GAMESPY_USE_SPEEX_CODEC

#endif // USE_DRXLOBBY_GAMESPY && USE_DRXLOBBY_GAMESPY_VOIP