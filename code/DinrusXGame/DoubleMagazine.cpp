// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXGame/StdAfx.h>
#include <DinrusX/DinrusXGame/DoubleMagazine.h>
#include <DinrusX/DinrusXGame/IDrxMannequin.h>



CDoubleMagazine::CDoubleMagazine()
	:	m_reloadFaster(true)
{
}



void CDoubleMagazine::OnAttach(bool attach)
{
	m_reloadFaster = true;
}



void CDoubleMagazine::OnParentReloaded()
{
	m_reloadFaster = !m_reloadFaster;
}



void CDoubleMagazine::SetAccessoryReloadTags(CTagState& fragTags)
{
	if (m_reloadFaster)
	{
		TagID clipRemaining = fragTags.GetDef().Find(CItem::sFragmentTagCRCs.doubleclip_fast);
		if(clipRemaining != TAG_ID_INVALID)
			fragTags.Set(clipRemaining, true);
	}
}
