// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$

   -------------------------------------------------------------------------
   History:
   - 2:8:2004   15:20 : Created by Márcio Martins

*************************************************************************/
#include <DinrusX/DinrusXSys/StdAfx.h>
#include <DinrusX/DinrusXSys/CmdLineArg.h>

CCmdLineArg::CCmdLineArg(const char* name, const char* value, ECmdLineArgType type)
{
	m_name = name;
	m_value = value;
	m_type = type;
}

CCmdLineArg::~CCmdLineArg()
{
}

const char* CCmdLineArg::GetName() const
{
	return m_name.c_str();
}
const char* CCmdLineArg::GetValue() const
{
	return m_value.c_str();
}
const ECmdLineArgType CCmdLineArg::GetType() const
{
	return m_type;
}
const float CCmdLineArg::GetFValue() const
{
	return (float)atof(m_value.c_str());
}
const int CCmdLineArg::GetIValue() const
{
	return atoi(m_value.c_str());
}
