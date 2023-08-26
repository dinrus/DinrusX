// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXAct/StdAfx.h>
#include <DinrusX/DinrusXAct/MFXContainer.h>

#include <DinrusX/DinrusXAct/MFXRandomEffect.h>
#include <DinrusX/DinrusXAct/MFXParticleEffect.h>
#include <DinrusX/DinrusXAct/MFXAudioEffect.h>
#include <DinrusX/DinrusXAct/MFXDecalEffect.h>
#include <DinrusX/DinrusXAct/MFXFlowGraphEffect.h>
#include <DinrusX/DinrusXAct/MFXForceFeedbackFX.h>

#include <DinrusX/DinrusXAct/GameXmlParamReader.h>

namespace MaterialEffectsUtils
{
CMFXEffectBase* CreateEffectByName(const char* effectType)
{
	if (!stricmp("RandEffect", effectType))
	{
		return new CMFXRandomEffect();
	}
	else if (!stricmp("Particle", effectType))
	{
		return new CMFXParticleEffect();
	}
	else if (!stricmp("Audio", effectType))
	{
		return new CMFXAudioEffect();
	}
	else if (!stricmp("Decal", effectType))
	{
		return new CMFXDecalEffect();
	}
	else if (!stricmp("FlowGraph", effectType))
	{
		return new CMFXFlowGraphEffect();
	}
	else if (!stricmp("ForceFeedback", effectType))
	{
		return new CMFXForceFeedbackEffect();
	}

	return NULL;
}
}

CMFXContainer::~CMFXContainer()
{
}

void CMFXContainer::BuildFromXML(const XmlNodeRef& paramsNode)
{
	DRX_ASSERT(paramsNode != NULL);

	LoadParamsFromXml(paramsNode);

	BuildChildEffects(paramsNode);
}

void CMFXContainer::Execute(const SMFXRunTimeEffectParams& params)
{
	TMFXEffects::iterator it = m_effects.begin();
	TMFXEffects::iterator itEnd = m_effects.end();

	while (it != itEnd)
	{
		TMFXEffectBasePtr& pEffect = *it;
		if (pEffect->CanExecute(params))
		{
			pEffect->Execute(params);
		}

		++it;
	}
}

void CMFXContainer::SetCustomParameter(const char* customParameter, const SMFXCustomParamValue& customParameterValue)
{
	DRX_PROFILE_FUNCTION(PROFILE_ACTION);

	TMFXEffects::iterator it = m_effects.begin();
	TMFXEffects::iterator itEnd = m_effects.end();
	while (it != itEnd)
	{
		TMFXEffectBasePtr& pEffect = *it;
		pEffect->SetCustomParameter(customParameter, customParameterValue);

		++it;
	}
}

void CMFXContainer::GetResources(SMFXResourceList& resourceList) const
{
	TMFXEffects::const_iterator it = m_effects.begin();
	TMFXEffects::const_iterator itEnd = m_effects.end();
	while (it != itEnd)
	{
		TMFXEffectBasePtr pEffect = *it;
		pEffect->GetResources(resourceList);

		++it;
	}
}

void CMFXContainer::PreLoadAssets()
{
	TMFXEffects::const_iterator it = m_effects.begin();
	TMFXEffects::const_iterator itEnd = m_effects.end();
	while (it != itEnd)
	{
		TMFXEffectBasePtr pEffect = *it;
		pEffect->PreLoadAssets();

		++it;
	}
}

void CMFXContainer::ReleasePreLoadAssets()
{
	TMFXEffects::const_iterator it = m_effects.begin();
	TMFXEffects::const_iterator itEnd = m_effects.end();
	while (it != itEnd)
	{
		TMFXEffectBasePtr pEffect = *it;
		pEffect->ReleasePreLoadAssets();

		++it;
	}
}

void CMFXContainer::GetMemoryUsage(IDrxSizer* pSizer) const
{
	pSizer->AddObject(m_params.name);
	pSizer->AddObject(m_params.libraryName);
	pSizer->AddObject(m_effects);
}

void CMFXContainer::BuildChildEffects(const XmlNodeRef& paramsNode)
{
	const CGameXmlParamReader reader(paramsNode);
	const int totalChildCount = reader.GetUnfilteredChildCount();
	const int filteredChildCount = reader.GetFilteredChildCount();

	m_effects.reserve(filteredChildCount);

	for (int i = 0; i < totalChildCount; i++)
	{
		XmlNodeRef currentEffectNode = reader.GetFilteredChildAt(i);
		if (currentEffectNode == NULL)
			continue;

		const char* nodeName = currentEffectNode->getTag();
		if (nodeName == 0 || *nodeName == 0)
			continue;

		TMFXEffectBasePtr pEffect = MaterialEffectsUtils::CreateEffectByName(nodeName);
		if (pEffect != NULL)
		{
			pEffect->LoadParamsFromXml(currentEffectNode);
			m_effects.push_back(pEffect);
		}
	}
}

void CMFXContainer::LoadParamsFromXml(const XmlNodeRef& paramsNode)
{
	m_params.name = paramsNode->getAttr("name");

	paramsNode->getAttr("delay", m_params.delay);
}

//////////////////////////////////////////////////////////////////////////

void CMFXDummyContainer::BuildChildEffects(const XmlNodeRef& paramsNode)
{

}
