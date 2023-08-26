// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Описание:  maintains a globally unique id for game sessions
   -------------------------------------------------------------------------
   History:
   - 27/02/2006   12:34 : Created by Craig Tiller
*************************************************************************/

#ifndef __SESSIONID_H__
#define __SESSIONID_H__

#pragma once

#include <DinrusX/DinrusXNetwork/Whirlpool.h>

class CSessionID
{
public:
	CSessionID();
	CSessionID(const char* hostname, int counter, time_t start);

	string GetHumanReadable() const
	{
		return m_hash.GetHumanReadable();
	}

	void SerializeWith(TSerialize ser)
	{
		m_hash.SerializeWith(ser);
	}

private:
	CWhirlpoolHash m_hash;
};

#endif
