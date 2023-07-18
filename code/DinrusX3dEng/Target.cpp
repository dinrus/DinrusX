// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusX3dEng/StdAfx.h>
#include <DinrusX/DinrusX3dEng/Target.h>
#include <DinrusX/CoreX/Serialization/Math.h>

DRX_PFX2_DBG

namespace pfx2
{

CTargetSource::CTargetSource(ETargetSource source)
	: m_offset(ZERO)
	, m_source(source)
{
}

void CTargetSource::Serialize(Serialization::IArchive& ar)
{
	ar(m_source, "Source", "^");
	ar(m_offset, "Offset", "Offset");
}

}
