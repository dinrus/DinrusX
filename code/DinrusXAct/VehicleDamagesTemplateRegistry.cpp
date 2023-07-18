// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Описание: Implements a registry for vehicle damages templates

   -------------------------------------------------------------------------
   History:
   - 18:07:2006: Created by Mathieu Pinard

*************************************************************************/
#include <DinrusX/DinrusXAct/StdAfx.h>
#include <DinrusX/DinrusXAct/DinrusAction.h>
#include <DinrusX/DinrusXAct/XMLScriptLoader.h>
#include <DinrusX/DinrusXAct/IVehicleSystem.h>
#include <DinrusX/DinrusXAct/Vehicle.h>
#include <DinrusX/DinrusXAct/VehicleDamagesGroup.h>
#include <DinrusX/DinrusXAct/VehicleDamagesTemplateRegistry.h>

//------------------------------------------------------------------------
bool CVehicleDamagesTemplateRegistry::Init(const string& damagesTemplatesPath)
{
	if (damagesTemplatesPath.empty())
		return false;

	m_templates.clear();

	IDrxPak* pDrxPak = gEnv->pDrxPak;

	_finddata_t fd;
	int ret;
	intptr_t handle;

	if ((handle = pDrxPak->FindFirst(damagesTemplatesPath + string("*") + ".xml", &fd)) != -1)
	{
		do
		{
			string name(fd.name);

			if (name.substr(0, 4) != "def_")
			{
				string filename = damagesTemplatesPath + name;

				if (!RegisterTemplates(filename))
					DrxLog("VehicleDamagesTemplateRegistry: error parsing template file <%s>.", filename.c_str());
			}

			ret = pDrxPak->FindNext(handle, &fd);
		}
		while (ret >= 0);

		pDrxPak->FindClose(handle);
	}

	return true;
}

//------------------------------------------------------------------------
bool CVehicleDamagesTemplateRegistry::RegisterTemplates(const string& filename)
{
	XmlNodeRef table = gEnv->pSystem->LoadXmlFromFile(filename);
	if (!table)
		return false;

	if (XmlNodeRef damagesGroupsTable = table->findChild("DamagesGroups"))
	{
		int i = 0;
		int c = damagesGroupsTable->getChildCount();

		for (; i < c; i++)
		{
			if (XmlNodeRef damagesGroupTable = damagesGroupsTable->getChild(i))
			{
				string name = damagesGroupTable->getAttr("name");
				if (!name.empty())
					m_templates.insert(TTemplateMap::value_type(name, damagesGroupTable));
			}
		}
	}

	return true;
}

//------------------------------------------------------------------------
bool CVehicleDamagesTemplateRegistry::UseTemplate(const string& templateName, IVehicleDamagesGroup* pDamagesGroup)
{
	TTemplateMap::iterator ite = m_templates.find(templateName);
	if (ite != m_templates.end())
	{
		CVehicleModificationParams noModifications;
		CVehicleParams tmpVehicleParams(ite->second, noModifications);
		return pDamagesGroup->ParseDamagesGroup(tmpVehicleParams);
	}

	return false;
}
