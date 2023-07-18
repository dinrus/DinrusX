// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXRend/StdAfx.h>
#include <DinrusX/DinrusXRend/OpticsFactory.h>

#include <DinrusX/DinrusXRend/RendElements/RootOpticsElement.h>
#include <DinrusX/DinrusXRend/RendElements/OpticsElement.h>
#include <DinrusX/DinrusXRend/Ghost.h>
#include <DinrusX/DinrusXRend/Glow.h>
#include <DinrusX/DinrusXRend/ChromaticRing.h>
#include <DinrusX/DinrusXRend/RendElements/CameraOrbs.h>
#include <DinrusX/DinrusXRend/IrisShafts.h>
#include <DinrusX/DinrusXRend/Streaks.h>
#include <DinrusX/DinrusXRend/ImageSpaceShafts.h>
#include <DinrusX/DinrusXRend/OpticsReference.h>
#include <DinrusX/DinrusXRend/OpticsProxy.h>
#include <DinrusX/DinrusXRend/OpticsPredef.hpp>

IOpticsElementBase* COpticsFactory::Create(EFlareType type) const
{
	switch (type)
	{
	case eFT_Root:
		return new RootOpticsElement;
	case eFT_Group:
		return new COpticsGroup("[Group]");
	case eFT_Ghost:
		return new CLensGhost("Ghost");
	case eFT_MultiGhosts:
		return new CMultipleGhost("Multi Ghost");
	case eFT_Glow:
		return new Glow("Glow");
	case eFT_IrisShafts:
		return new IrisShafts("Iris Shafts");
	case eFT_ChromaticRing:
		return new ChromaticRing("Chromatic Ring");
	case eFT_CameraOrbs:
		return new CameraOrbs("Orbs");
	case eFT_ImageSpaceShafts:
		return new ImageSpaceShafts("Vol Shafts");
	case eFT_Streaks:
		return new Streaks("Streaks");
	case eFT_Reference:
		return new COpticsReference("Reference");
	case eFT_Proxy:
		return new COpticsProxy("Proxy");
	default:
		return NULL;
	}
}
