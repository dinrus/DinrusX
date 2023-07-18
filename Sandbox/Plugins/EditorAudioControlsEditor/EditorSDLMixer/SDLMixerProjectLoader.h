// Copyright 2001-2016 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include <DinrusX/DinrusXSys/XML/IXml.h>
#include "ACETypes.h"

namespace ACE
{
class CAudioSystemEditor_sdlmixer;

class CSDLMixerProjectLoader
{
public:
	CSDLMixerProjectLoader(const string& sAssetsPath, CAudioSystemEditor_sdlmixer* pAudioSystemImpl);
	CAudioSystemEditor_sdlmixer* m_pAudioSystemImpl;
};
}
