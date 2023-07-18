// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Описание:  Helper classes for authentication
   -------------------------------------------------------------------------
   History:
   - 26/07/2004   : Created by Craig Tiller
*************************************************************************/
#include <DinrusX/DinrusXNetwork/StdAfx.h>
#include  <DinrusX/DinrusXNetwork/Authentication.h>
#include <DinrusX/DinrusXSys/ITimer.h>

ILINE static uint32 GetRandomNumber()
{
	NetWarning("Using low quality random numbers: security compromised");
	return drx_random_uint32();
}

SAuthenticationSalt::SAuthenticationSalt() :
	fTime(gEnv->pTimer->GetCurrTime()),
	nRand(GetRandomNumber())
{
}

void SAuthenticationSalt::SerializeWith(TSerialize ser)
{
	ser.Value("fTime", fTime);
	ser.Value("nRand", nRand);
}

CWhirlpoolHash SAuthenticationSalt::Hash(const string& password) const
{
	char n1[32];
	char n2[32];
	drx_sprintf(n1, "%f", fTime);
	drx_sprintf(n2, "%.8x", nRand);
	string buffer = password + ":" + n1 + ":" + n2;
	return CWhirlpoolHash(buffer);
}
