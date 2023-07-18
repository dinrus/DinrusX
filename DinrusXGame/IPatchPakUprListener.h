// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
	-------------------------------------------------------------------------
	$Id$
	$DateTime$
	Description: 
		Interface for PatchPakUpr events

	-------------------------------------------------------------------------
	History:
	- 25:10:2012  : Created by Jim Bamford

*************************************************************************/

#ifndef _IPATCHPAKMANAGERLISTENER_H_
#define _IPATCHPAKMANAGERLISTENER_H_ 

#if _MSC_VER > 1000
# pragma once
#endif

class IPatchPakUprListener
{
public:
	virtual ~IPatchPakUprListener() {}

	virtual void UpdatedPermissionsNowAvailable() = 0;
};

#endif // _IPATCHPAKMANAGERLISTENER_H_  

