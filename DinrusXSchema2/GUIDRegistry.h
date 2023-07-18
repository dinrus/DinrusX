// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

#include <DinrusX/CoreX/Extension/ClassWeaver.h>
#include <DinrusX/DinrusXSchema2/IFramework.h>

#include <DinrusX/DinrusXSchema2/Compiler.h>
#include <DinrusX/DinrusXSchema2/DocUpr.h>
#include <DinrusX/DinrusXSchema2/EnvRegistry.h>
#include <DinrusX/DinrusXSchema2/LibRegistry.h>
#include <DinrusX/DinrusXSchema2/Log.h>
#include <DinrusX/DinrusXSchema2/ObjectUpr.h>
#include <DinrusX/DinrusXSchema2/TimerSystem.h>
#include <DinrusX/DinrusXSchema2/StringPool.h>
#include <DinrusX/DinrusXSchema2/UpdateScheduler.h>

#define SCHEMATYC_FRAMEWORK_CLASS_NAME "GameExtension_SchematycFramework"

namespace Schematyc
{
	// Framework.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	class CFramework : public IFramework
	{
		DRXINTERFACE_SIMPLE(IFramework)
		DRXGENERATE_CLASS_GUID(CFramework, SCHEMATYC_FRAMEWORK_CLASS_NAME, "96d98d98-35aa-4fb6-830b-53dbfe71908d"_drx_guid)

	public:

		void Init();

		// IFramework
		virtual void SetGUIDGenerator(const TGUIDGenerator& guidGenerator) OVERRIDE;
		virtual SGUID CreateGUID() const OVERRIDE;
		virtual IStringPool& GetStringPool() OVERRIDE;
		virtual IEnvRegistry& GetEnvRegistry() OVERRIDE;
		virtual ILibRegistry& GetLibRegistry() OVERRIDE;
		virtual const char* GetRootFolder() const OVERRIDE;
		virtual const char* GetDocFolder() const OVERRIDE;
		virtual const char* GetDocExtension() const OVERRIDE;
		virtual const char* GetSettingsFolder() const OVERRIDE;
		virtual const char* GetSettingsExtension() const OVERRIDE;
		virtual IDocUpr& GetDocUpr() OVERRIDE;
		virtual ICompiler& GetCompiler() OVERRIDE;
		virtual IObjectUpr& GetObjectUpr() OVERRIDE;
		virtual ILog& GetLog() OVERRIDE;
		virtual IUpdateScheduler& GetUpdateScheduler() OVERRIDE;
		virtual ITimerSystem& GetTimerSystem() OVERRIDE;
		virtual IRefreshEnvSignal& GetRefreshEnvSignal() OVERRIDE;
		virtual void RefreshEnv() OVERRIDE;
		// ~IFramework

	private:

		typedef TemplateUtils::CSignal<void ()> TRefreshEnvSignal;

		TGUIDGenerator		m_guidGenerator;
		CStringPool				m_stringPool;
		CEnvRegistry			m_envRegistry;
		CLibRegistry			m_libRegistry;
		CDocUpr				m_docUpr;
		mutable string		m_docFolder;
		mutable string		m_settingsFolder;
		CCompiler					m_compiler;
		CTimerSystem			m_timerSystem;	// TODO : Shutdown is reliant on order of destruction so we should formalize this rather than relying on placement within CFramework class.
		CObjectUpr		m_objectUpr;
		CLog							m_log;
		CUpdateScheduler	m_updateScheduler;
		TRefreshEnvSignal	m_refreshEnvSignal;
	};
}
