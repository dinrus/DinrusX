// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSys/StdAfx.h>

#include <DinrusX/DinrusXSys/UserAnalyticsSystem.h>

#if !defined(_RELEASE) && DRX_PLATFORM_WINDOWS

	#include <DinrusX/DinrusXSys/IUserAnalytics.h>
	#include <DinrusX/DinrusXSys/IDrxUserAnalyticsPlugin.h>
	#include <DinrusX/DinrusXSys/DrxPluginUpr.h>

int CUserAnalyticsSystem::m_enableUserAnalyticsCollect = 0;
int CUserAnalyticsSystem::m_enableUserAnalyticsLogging = 0;

///////////////////////////////////////////////////////////////////////////
CUserAnalyticsSystem::CUserAnalyticsSystem()
	: m_pUserAnalyticsPlugin(nullptr)
	, m_pUserAnalytics(nullptr)
{
}

///////////////////////////////////////////////////////////////////////////
void CUserAnalyticsSystem::RegisterCVars()
{
	REGISTER_CVAR2("sys_UserAnalyticsCollect", &m_enableUserAnalyticsCollect, 1, VF_NULL,
	               "Collect User Analytics Events and push them to DRXENGINE server\n"
	               " 0 - disabled\n"
	               " 1 - enabled, recommended");

	REGISTER_CVAR2("sys_UserAnalyticsLogging", &m_enableUserAnalyticsLogging, 0, VF_NULL,
	               "Enable User Analytics Logging\n"
	               " 0 - disabled\n"
	               " 1 - enabled");
}

///////////////////////////////////////////////////////////////////////////
CUserAnalyticsSystem::~CUserAnalyticsSystem()
{
	if (Drx::IPluginUpr* pPluginUpr = GetISystem()->GetIPluginUpr())
	{
		pPluginUpr->RemoveEventListener<IDrxUserAnalyticsPlugin>(this);
	}

	if (IConsole* const pConsole = gEnv->pConsole)
	{
		pConsole->UnregisterVariable("sys_UserAnalyticsCollect");
		pConsole->UnregisterVariable("sys_UserAnalyticsLogging");
	}
}

///////////////////////////////////////////////////////////////////////////
void CUserAnalyticsSystem::Initialize()
{
	m_pUserAnalyticsPlugin = GetISystem()->GetIPluginUpr()->QueryPlugin<IDrxUserAnalyticsPlugin>();

	if (m_pUserAnalyticsPlugin != nullptr)
	{
		GetISystem()->GetIPluginUpr()->RegisterEventListener<IDrxUserAnalyticsPlugin>(this);

		m_pUserAnalytics = m_pUserAnalyticsPlugin->GetIUserAnalytics();
	}
}

///////////////////////////////////////////////////////////////////////////
void CUserAnalyticsSystem::OnPluginEvent(const DrxClassID& pluginClassId, Drx::IPluginUpr::IEventListener::EEvent event)
{
	if (event == Drx::IPluginUpr::IEventListener::EEvent::Unloaded)
	{
		m_pUserAnalyticsPlugin = nullptr;
		m_pUserAnalytics = nullptr;
	}
}

///////////////////////////////////////////////////////////////////////////
void CUserAnalyticsSystem::TriggerEvent(const char* szEventName, UserAnalytics::Attributes* attributes)
{
	if (m_enableUserAnalyticsCollect == 0)
	{
		return;
	}

	if (m_pUserAnalytics)
	{
		if (m_enableUserAnalyticsLogging > 0)
		{
			DrxLogAlways("[User Analytics] Queuing Event: %s", szEventName);
		}

		m_pUserAnalytics->TriggerEvent(szEventName, attributes);
	}
}
#endif
