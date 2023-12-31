// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// ----------------------------------------------------------------------------------------
//  File name:   MFXEffectBase.h
//  Описание: Basic and partial implemenation of IMFXEffect which serves as a base for concrete implementations
//
////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _MFX_EFFECT_BASE_H_
#define _MFX_EFFECT_BASE_H_

#pragma once

#include <DinrusX/CoreX/DrxFlags.h>
#include "IMFXEffect.h"

class CMFXEffectBase : public IMFXEffect
{
public:

	CMFXEffectBase(const uint16 _typeFilterFlag);

	//IMFXEffect (partial implementation)
	virtual void SetCustomParameter(const char* customParameter, const SMFXCustomParamValue& customParameterValue) override;
	virtual void PreLoadAssets() override;
	virtual void ReleasePreLoadAssets() override;
	//~IMFXEffect

	bool CanExecute(const SMFXRunTimeEffectParams& params) const;

private:
	CDrxFlags<uint16> m_runtimeExecutionFilter;
};

typedef _smart_ptr<CMFXEffectBase> TMFXEffectBasePtr;

#endif // _MFX_EFFECT_BASE_H_
