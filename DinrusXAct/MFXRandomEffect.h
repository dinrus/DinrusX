// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   MFXRandomEffect.h
//  Version:     v1.00
//  Created:     28/11/2006 by JohnN/AlexL
//  Компиляторы:   mingw-w64-clang-x86_64
//  Описание: Random effect (randomly plays one of its child effects)
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#ifndef __MFXRANDOMEFFECT_H__
#define __MFXRANDOMEFFECT_H__

#pragma once

#include "MFXEffectBase.h"
#include "MFXContainer.h"

class CMFXRandomizerContainer
	: public CMFXContainer
{
public:
	void ExecuteRandomizedEffects(const SMFXRunTimeEffectParams& params);

private:
	TMFXEffectBasePtr ChooseCandidate(const SMFXRunTimeEffectParams& params) const;
};

class CMFXRandomEffect :
	public CMFXEffectBase
{

public:
	CMFXRandomEffect();
	virtual ~CMFXRandomEffect();

	//IMFXEffect
	virtual void Execute(const SMFXRunTimeEffectParams& params) override;
	virtual void LoadParamsFromXml(const XmlNodeRef& paramsNode) override;
	virtual void SetCustomParameter(const char* customParameter, const SMFXCustomParamValue& customParameterValue) override;
	virtual void GetResources(SMFXResourceList& resourceList) const override;
	virtual void PreLoadAssets() override;
	virtual void ReleasePreLoadAssets() override;
	virtual void GetMemoryUsage(IDrxSizer* pSizer) const override;
	//~IMFXEffect

private:
	CMFXRandomizerContainer m_container;
};

#endif
