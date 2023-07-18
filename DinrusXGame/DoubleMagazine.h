// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#ifndef _DOUBLE_MAGAZINE_H_
#define _DOUBLE_MAGAZINE_H_


#include <DinrusX/DinrusXGame/Accessory.h>


class CDoubleMagazine : public CAccessory
{
public:
	CDoubleMagazine();

	virtual void OnAttach(bool attach);
	virtual void OnParentReloaded();
	virtual void SetAccessoryReloadTags(CTagState& fragTags);

private:
	bool m_reloadFaster;
};



#endif
