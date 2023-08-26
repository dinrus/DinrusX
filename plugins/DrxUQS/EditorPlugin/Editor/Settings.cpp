// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"
#include "Settings.h"

bool SDocumentSettings::operator==(const SDocumentSettings& other) const
{
	return ::memcmp(this, &other, sizeof(SDocumentSettings)) == 0;
}

bool SDocumentSettings::operator!=(const SDocumentSettings& other) const
{
	return !(*this == other);
}
