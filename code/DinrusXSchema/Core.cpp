// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXSchema/StdAfx.h>
#include <DinrusX/DinrusXSchema/Core.h>

#include <DinrusX/CoreX/Platform/platform_impl.inl>
#include <DinrusX/DinrusXSys/IDrxPluginUpr.h>
#include <DinrusX/CoreX/Game/IGameFramework.h>

#include <DinrusX/DinrusXSchema/EnvPackage.h>
#include <DinrusX/DinrusXSchema/EnvDataType.h>
#include <DinrusX/DinrusXSchema/SerializationEnums.inl>
#include <DinrusX/DinrusXSchema/SerializationUtils.h>
#include <DinrusX/DinrusXSchema/StackString.h>

#include <DinrusX/DinrusXSchema/CVars.h>
#include <DinrusX/DinrusXSchema/ObjectPool.h>
#include <DinrusX/DinrusXSchema/Compiler.h>
#include <DinrusX/DinrusXSchema/CoreEnv.h>
#include <DinrusX/DinrusXSchema/EnvRegistry.h>
#include <DinrusX/DinrusXSchema/RuntimeRegistry.h>
#include <DinrusX/DinrusXSchema/ScriptRegistry.h>
#include <DinrusX/DinrusXSchema/ScriptView.h>
#include <DinrusX/DinrusXSchema/SerializationContext.h>
#include <DinrusX/DinrusXSchema/ValidatorArchive.h>
#include <DinrusX/DinrusXSchema/Log.h>
#include <DinrusX/DinrusXSchema/LogRecorder.h>
#include <DinrusX/DinrusXSchema/SettingsUpr.h>
#include <DinrusX/DinrusXSchema/TimerSystem.h>
#include <DinrusX/DinrusXSchema/UpdateScheduler.h>
#include <DinrusX/DinrusXSchema/UnitTestRegistrar.h>

namespace Schematyc
{
namespace
{

static void OnLogFileStreamsChange(ICVar* pCVar)
{
	CCore::GetInstance().RefreshLogFileStreams();
}

static void OnLogFileMessageTypesChange(ICVar* pCVar)
{
	CCore::GetInstance().RefreshLogFileMessageTypes();
}

inline bool WantPrePhysicsUpdate()
{
	return !gEnv->pGameFramework->IsGamePaused() && (gEnv->pSystem->GetSystemGlobalState() == ESYSTEM_GLOBAL_STATE_RUNNING) && !gEnv->IsEditing();
}

inline bool WantUpdate()
{
	return !gEnv->pGameFramework->IsGamePaused() && (gEnv->pSystem->GetSystemGlobalState() == ESYSTEM_GLOBAL_STATE_RUNNING);
}

} // Anonymous

static const char* g_szSettingsFolder = "DrxSchematycSettings";

CCore::CCore()
	: m_pEnvRegistry(new CEnvRegistry())
	, m_pScriptRegistry(new CScriptRegistry())
	, m_pRuntimeRegistry(new CRuntimeRegistry())
	, m_pObjectPool(new CObjectPool())
	, m_pCompiler(new CCompiler())
	, m_pTimerSystem(new CTimerSystem())
	, m_pLog(new CLog())
	, m_pLogRecorder(new CLogRecorder())
	, m_pSettingsUpr(new CSettingsUpr())
	, m_pUpdateScheduler(new CUpdateScheduler())
{
	gEnv->pSchematyc = this;
}

CCore::~CCore()
{
	// TODO: This should be moved to a proper shutdown function later.
	m_pEnvRegistry->DeregisterPackage(g_coreEnvPackageGuid);
	// ~TODO

	gEnv->pSystem->GetISystemEventDispatcher()->RemoveListener(this);

	m_pLog->Shutdown();

	Schematyc::CVars::Unregister();

	s_pInstance = nullptr;
	gEnv->pSchematyc = nullptr;
}

const char* CCore::GetName() const
{
	return "DrxSchematycCore";
}

const char* CCore::GetCategory() const
{
	return "Plugin";
}

bool CCore::Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams)
{
	SCHEMATYC_CORE_ASSERT(!s_pInstance);

	s_pInstance = this;
	env.pSchematyc = this;

	Schematyc::CVars::Register();

	m_pLog->Init();
	if (CVars::sc_LogToFile)
	{
		CStackString logFileName;
		const int applicationInstance = gEnv->pSystem->GetApplicationInstance();
		if (applicationInstance)
		{
			logFileName.Format("DrxSchematyc(%d).log", applicationInstance);
		}
		else
		{
			logFileName = "DrxSchematyc.log";
		}
		m_pLogFileOutput = m_pLog->CreateFileOutput(logFileName.c_str());
		SCHEMATYC_CORE_ASSERT(m_pLogFileOutput);
		RefreshLogFileStreams();
		CVars::sc_LogFileStreams->SetOnChangeCallback(OnLogFileStreamsChange);
		RefreshLogFileMessageTypes();
		CVars::sc_LogFileMessageTypes->SetOnChangeCallback(OnLogFileMessageTypesChange);
	}

	if (CVars::sc_RunUnitTests)
	{
		CUnitTestRegistrar::RunUnitTests();
	}

	gEnv->pSystem->GetISystemEventDispatcher()->RegisterListener(this, "CCore");

	env.pSchematyc = this;

	if (!m_pEnvRegistry->RegisterPackage(SCHEMATYC_MAKE_ENV_PACKAGE(g_coreEnvPackageGuid, "CoreEnv", g_szDinrus, "Core Schematyc environment", SCHEMATYC_DELEGATE(&RegisterCoreEnvPackage))))
	{
		env.pSchematyc = nullptr;
		return false;
	}

	return true;
}

void CCore::RefreshEnv()
{
	// #SchematycTODO : Ensure that no objects exist?
	m_pRuntimeRegistry->Reset();
	m_pEnvRegistry->Refresh();
	// #SchematycTODO : Re-load and re-compile scripts?
}

void CCore::SetGUIDGenerator(const GUIDGenerator& guidGenerator)
{
	m_guidGenerator = guidGenerator;
}

DrxGUID CCore::CreateGUID() const
{
	SCHEMATYC_CORE_ASSERT(m_guidGenerator);
	return m_guidGenerator ? m_guidGenerator() : DrxGUID();
}

const char* CCore::GetRootFolder() const
{
	return CVars::sc_RootFolder->GetString();
}

const char* CCore::GetScriptsFolder() const
{
	m_scriptsFolder = PathUtil::GetGameFolder();
	return m_scriptsFolder.c_str();
}

const char* CCore::GetSettingsFolder() const
{
	m_settingsFolder = PathUtil::GetGameFolder();
	m_settingsFolder.append("/");
	m_settingsFolder.append(g_szSettingsFolder);
	return m_settingsFolder.c_str();
}

bool CCore::IsExperimentalFeatureEnabled(const char* szFeatureName) const
{
	return DrxStringUtils::stristr(CVars::sc_ExperimentalFeatures->GetString(), szFeatureName) != nullptr;
}

IEnvRegistry& CCore::GetEnvRegistry()
{
	SCHEMATYC_CORE_ASSERT(m_pEnvRegistry);
	return *m_pEnvRegistry;
}

IScriptRegistry& CCore::GetScriptRegistry()
{
	SCHEMATYC_CORE_ASSERT(m_pScriptRegistry);
	return *m_pScriptRegistry;
}

IRuntimeRegistry& CCore::GetRuntimeRegistry()
{
	SCHEMATYC_CORE_ASSERT(m_pRuntimeRegistry);
	return *m_pRuntimeRegistry;
}

ICompiler& CCore::GetCompiler()
{
	SCHEMATYC_CORE_ASSERT(m_pCompiler);
	return *m_pCompiler;
}

ILog& CCore::GetLog()
{
	return *m_pLog;
}

ILogRecorder& CCore::GetLogRecorder()
{
	return *m_pLogRecorder;
}

ISettingsUpr& CCore::GetSettingsUpr()
{
	return *m_pSettingsUpr;
}

IUpdateScheduler& CCore::GetUpdateScheduler()
{
	return *m_pUpdateScheduler;
}

ITimerSystem& CCore::GetTimerSystem()
{
	return *m_pTimerSystem;
}

IValidatorArchivePtr CCore::CreateValidatorArchive(const SValidatorArchiveParams& params) const
{
	return std::make_shared<CValidatorArchive>(params);
}

ISerializationContextPtr CCore::CreateSerializationContext(const SSerializationContextParams& params) const
{
	return std::make_shared<CSerializationContext>(params);
}

IScriptViewPtr CCore::CreateScriptView(const DrxGUID& scopeGUID) const
{
	return std::make_shared<CScriptView>(scopeGUID);
}

bool CCore::CreateObject(const Schematyc::SObjectParams& params, IObject*& pObjectOut)
{
	return m_pObjectPool->CreateObject(params, pObjectOut);
}

IObject* CCore::GetObject(ObjectId objectId)
{
	return m_pObjectPool->GetObject(objectId);
}

void CCore::DestroyObject(ObjectId objectId)
{
	m_pObjectPool->DestroyObject(objectId);
}

void CCore::SendSignal(ObjectId objectId, const SObjectSignal& signal)
{
	m_pObjectPool->SendSignal(objectId, signal);
}

void CCore::BroadcastSignal(const SObjectSignal& signal)
{
	m_pObjectPool->BroadcastSignal(signal);
}

void CCore::PrePhysicsUpdate()
{
	if (WantPrePhysicsUpdate())
	{
		m_pUpdateScheduler->BeginFrame(gEnv->pTimer->GetFrameTime());
		m_pUpdateScheduler->Update(EUpdateStage::PrePhysics | EUpdateDistribution::Earliest, EUpdateStage::PrePhysics | EUpdateDistribution::End);
	}
}

void CCore::Update()
{
	if (WantUpdate())
	{
		if (!m_pUpdateScheduler->InFrame())
		{
			m_pUpdateScheduler->BeginFrame(gEnv->pTimer->GetFrameTime());
		}

		if (gEnv->IsEditing())
		{
			m_pUpdateScheduler->Update(EUpdateStage::Editing | EUpdateDistribution::Earliest, EUpdateStage::Editing | EUpdateDistribution::End);
			m_pUpdateScheduler->EndFrame();
		}
		else
		{
			m_pTimerSystem->Update();

			m_pUpdateScheduler->Update(EUpdateStage::Default | EUpdateDistribution::Earliest, EUpdateStage::Post | EUpdateDistribution::End);
			m_pUpdateScheduler->EndFrame();
		}

		m_pLog->Update();
	}
}

void CCore::LoadProjectFiles()
{
	GetLogRecorder().Begin();

	DrxLogAlways("[Schematyc]: Loading...");
	DrxLogAlways("[Schematyc]: Loading settings");
	GetSettingsUpr().LoadAllSettings();
	DrxLogAlways("[Schematyc]: Loading scripts");
	GetScriptRegistry().Load();
	DrxLogAlways("[Schematyc]: Compiling scripts");
	GetCompiler().CompileAll();
	DrxLogAlways("[Schematyc]: Loading complete");

	RefreshLogFileSettings();
	if (gEnv->IsEditor())
	{
		GetLogRecorder().End();
	}
}

void CCore::RefreshLogFileSettings()
{
	RefreshLogFileStreams();
	RefreshLogFileMessageTypes();
}

void CCore::RefreshLogFileStreams()
{
	if (m_pLogFileOutput)
	{
		m_pLogFileOutput->DisableAllStreams();
		CStackString logFileStreams = CVars::sc_LogFileStreams->GetString();
		const uint32 length = logFileStreams.length();
		int pos = 0;
		do
		{
			CStackString token = logFileStreams.Tokenize(" ", pos);
			const LogStreamId logStreamId = gEnv->pSchematyc->GetLog().GetStreamId(token.c_str());
			if (logStreamId != LogStreamId::Invalid)
			{
				m_pLogFileOutput->EnableStream(logStreamId);
			}
		}
		while (pos < length);
	}
}

void CCore::RefreshLogFileMessageTypes()
{
	if (m_pLogFileOutput)
	{
		m_pLogFileOutput->DisableAllMessageTypes();
		CStackString logFileMessageTypes = CVars::sc_LogFileMessageTypes->GetString();
		const uint32 length = logFileMessageTypes.length();
		int pos = 0;
		do
		{
			CStackString token = logFileMessageTypes.Tokenize(" ", pos);
			const ELogMessageType logMessageType = gEnv->pSchematyc->GetLog().GetMessageType(token.c_str());
			if (logMessageType != ELogMessageType::Invalid)
			{
				m_pLogFileOutput->EnableMessageType(logMessageType);
			}
		}
		while (pos < length);
	}
}

CRuntimeRegistry& CCore::GetRuntimeRegistryImpl()
{
	SCHEMATYC_CORE_ASSERT(m_pRuntimeRegistry);
	return *m_pRuntimeRegistry;
}

CCompiler& CCore::GetCompilerImpl()
{
	SCHEMATYC_CORE_ASSERT(m_pCompiler);
	return *m_pCompiler;
}

CCore& CCore::GetInstance()
{
	SCHEMATYC_CORE_ASSERT(s_pInstance);
	return *s_pInstance;
}

void CCore::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
{
	if (event == ESYSTEM_EVENT_GAME_FRAMEWORK_INIT_DONE)
	{
		LoadProjectFiles();
	}
}

CCore* CCore::s_pInstance = nullptr;

DRXREGISTER_SINGLETON_CLASS(CCore)

} // Schematyc
