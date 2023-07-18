// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   FlowWeaponCustomizationNodes.h
//  Version:     v1.00
//  Created:     03/05/2012 by Michiel Meesters.
//  Description: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////
#ifndef __ModelToHudFlowNodes_H__
#define __ModelToHudFlowNodes_H__
#include <DinrusX/DinrusXGame/Nodes/G2FlowBaseNode.h>
#include <DinrusX/DinrusXGame/UI/Menu3dModels/MenuRender3DModelMgr.h>

class CFlowSetupModelPostRender: public CFlowBaseNode<eNCT_Instanced>
{
public:
	enum EInputs
	{
		IN_START,
		IN_SHUTDOWN,
		IN_MC,
		IN_AMBIENTLIGHTCOLOR,
		IN_AMBIENTLIGHTSTRENGTH,
		IN_LIGHTCOLOR1,
		IN_LIGHTCOLOR2,
		IN_LIGHTCOLOR3,
		IN_DEBUGSCALE,
	};

	CFlowSetupModelPostRender( SActivationInfo * pActInfo );
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo);
	virtual void GetConfiguration(SFlowNodeConfig& config);
	virtual void ProcessEvent( EFlowEvent event, SActivationInfo *pActInfo );
	virtual void GetMemoryUsage(IDrxSizer * s) const;

private:
	CMenuRender3DModelMgr::TAddedModelIndex characterModelIndex;
	char* playerModelName;
	string sUIElement;
	string sMovieClipName;
};


class CFlowAddModelToPostRender: public CFlowBaseNode<eNCT_Instanced>
{
public:
	enum EInputs
	{
		IN_ADD,
		IN_MODEL,
		IN_SCALE,
		IN_ANIM,
		IN_ANIM_SPEED,
		IN_ENTITYPOS,
		IN_ENTITYROT,
		IN_ENTITYCONTROT,
		IN_SCREENUV,
		IN_SCREENU2V2,
	};

	CFlowAddModelToPostRender( SActivationInfo * pActInfo );
	virtual IFlowNodePtr Clone(SActivationInfo *pActInfo);
	virtual void GetConfiguration(SFlowNodeConfig& config);
	virtual void ProcessEvent( EFlowEvent event, SActivationInfo *pActInfo );
	virtual void GetMemoryUsage(IDrxSizer * s) const;

private:
	CMenuRender3DModelMgr::TAddedModelIndex characterModelIndex;
	char* playerModelName;
	string sUIElement;
	string sMovieClipName;
};

#endif