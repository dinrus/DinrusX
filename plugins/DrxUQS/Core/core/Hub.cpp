// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"
#include "Hub.h"
<DinrusX/DrxUQS/DataSource_XML/DataSource_XML_Includes.h>
<DinrusX/DrxUQS/Client/ClientIncludes.h>
<DinrusX/DrxUQS/StdLib/StdLibRegistration.h>

// *INDENT-OFF* - <hard to read code and declarations due to inconsistent indentation>

namespace UQS
{
	namespace Core
	{

		//===================================================================================
		//
		// Hub_HaveConsistencyChecksBeenDoneAlready()
		//
		//===================================================================================

		bool Hub_HaveConsistencyChecksBeenDoneAlready()
		{
			assert(g_pHub);
			return g_pHub->HaveConsistencyChecksBeenDoneAlready();
		}

		//===================================================================================
		//
		// CHub
		//
		//===================================================================================

		CHub* g_pHub;

		CHub::CHub()
			: m_bConsistencyChecksDoneAlready(false)
			, m_bAutomaticUpdateInProgress(false)
			, m_queryHistoryInGameGUI(m_queryHistoryUpr)
			, m_queryUpr(m_queryHistoryUpr)
			, m_pEditorLibraryProvider(nullptr)
		{
			assert(!g_pHub);
			g_pHub = this;
			GetISystem()->GetISystemEventDispatcher()->RegisterListener(this,"CHub");
			m_utils.SubscribeToStuffInHub(*this);

			CQueryFactoryBase::InstantiateFactories();
			CQueryFactoryBase::RegisterAllInstancesInFactoryDatabase(m_queryFactoryDatabase);

			CScoreTransformFactory::InstantiateFactories();
			CScoreTransformFactory::RegisterAllInstancesInFactoryDatabase(m_scoreTransformFactoryDatabase);

			SCvars::Register();
			REGISTER_COMMAND("UQS_ListFactoryDatabases", CmdListFactoryDatabases, 0, "Prints all registered factories for creating items, functions, generators and evaluators to the console.");
			REGISTER_COMMAND("UQS_ListQueryBlueprintLibrary", CmdListQueryBlueprintLibrary, 0, "Prints all query-blueprints in the library to the console.");
			REGISTER_COMMAND("UQS_ListRunningQueries", CmdListRunningQueries, 0, "Prints all currently running queries to the console.");
			REGISTER_COMMAND("UQS_DumpQueryHistory", CmdDumpQueryHistory, 0, "Dumps all queries that were executed so far to an XML file for de-serialization at a later time.");
			REGISTER_COMMAND("UQS_LoadQueryHistory", CmdLoadQueryHistory, 0, "Loads a history of queries from an XML for debug-rendering and inspection in the 3D world.");
			REGISTER_COMMAND("UQS_ClearLiveQueryHistory", CmdClearLiveQueryHistory, 0, "Clears the history of currently ongoing queries in memory.");
			REGISTER_COMMAND("UQS_ClearDeserialzedQueryHistory", CmdClearDeserializedQueryHistory, 0, "Clears the history of queries previously loaded from disk into memory.");
		}

		CHub::~CHub()
		{
#if UQS_SCHEMATYC_SUPPORT
			if (gEnv->pSchematyc != nullptr)
			{
				gEnv->pSchematyc->GetEnvRegistry().DeregisterPackage(GetSchematycPackageGUID());
			}
#endif

			g_pHub = nullptr;
			GetISystem()->GetISystemEventDispatcher()->RemoveListener(this);
			m_utils.UnsubscribeFromStuffInHub(*this);
			SCvars::Unregister();
		}

		void CHub::RegisterHubEventListener(IHubEventListener* pListener)
		{
			stl::push_back_unique(m_eventListeners, pListener);
		}

		void CHub::Update()
		{
			DRX_PROFILE_FUNCTION(UQS_PROFILED_SUBSYSTEM_TO_USE);

			// - if this assert fails, then the game code tries to do the update when it hasn't declared to do so
			// - this check is done to prevent updating from more than one place
			assert(gEnv->IsEditing() || (m_bAutomaticUpdateInProgress == !m_overrideFlags.Check(EHubOverrideFlags::CallUpdate)));

			//
			// query manager
			//

			m_queryUpr.Update();

			//
			// history drawing (only 2D texts; the 3D debug primitives of each historic query are done via IQueryHistoryUpr::UpdateDebugRendering3D())
			//

			if (SCvars::debugDraw)
			{
				//
				// ongoing queries as 2D text
				//

				m_queryUpr.DebugDrawRunningQueriesStatistics2D();

				//
				// draw the list of historic queries along with detailed information about the one that is currently focused by the camera
				//

				m_queryHistoryInGameGUI.Draw();
			}
		}

		CEnumFlags<EHubOverrideFlags>& CHub::GetOverrideFlags()
		{
			return m_overrideFlags;
		}

		QueryFactoryDatabase& CHub::GetQueryFactoryDatabase()
		{
			return m_queryFactoryDatabase;
		}

		ItemFactoryDatabase& CHub::GetItemFactoryDatabase()
		{
			return m_itemFactoryDatabase;
		}

		FunctionFactoryDatabase& CHub::GetFunctionFactoryDatabase()
		{
			return m_functionFactoryDatabase;
		}

		GeneratorFactoryDatabase& CHub::GetGeneratorFactoryDatabase()
		{
			return m_generatorFactoryDatabase;
		}

		InstantEvaluatorFactoryDatabase& CHub::GetInstantEvaluatorFactoryDatabase()
		{
			return m_instantEvaluatorFactoryDatabase;
		}

		DeferredEvaluatorFactoryDatabase& CHub::GetDeferredEvaluatorFactoryDatabase()
		{
			return m_deferredEvaluatorFactoryDatabase;
		}

		ScoreTransformFactoryDatabase& CHub::GetScoreTransformFactoryDatabase()
		{
			return m_scoreTransformFactoryDatabase;
		}

		CQueryBlueprintLibrary& CHub::GetQueryBlueprintLibrary()
		{
			return m_queryBlueprintLibrary;
		}

		CQueryUpr& CHub::GetQueryUpr()
		{
			return m_queryUpr;
		}

		CQueryHistoryUpr& CHub::GetQueryHistoryUpr()
		{
			return m_queryHistoryUpr;
		}

		CUtils& CHub::GetUtils()
		{
			return m_utils;
		}

		CEditorService& CHub::GetEditorService()
		{
			return m_editorService;
		}

		CItemSerializationSupport& CHub::GetItemSerializationSupport()
		{
			return m_itemSerializationSupport;
		}

		CSettingsUpr& CHub::GetSettingsUpr()
		{
			return m_settingsUpr;
		}

		DataSource::IEditorLibraryProvider* CHub::GetEditorLibraryProvider()
		{
			return m_pEditorLibraryProvider;
		}

		void CHub::SetEditorLibraryProvider(DataSource::IEditorLibraryProvider* pProvider)
		{
			m_pEditorLibraryProvider = pProvider;
		}

		bool CHub::HaveConsistencyChecksBeenDoneAlready() const
		{
			return m_bConsistencyChecksDoneAlready;
		}

		void CHub::AutomaticUpdateBegin()
		{
			assert(!m_bAutomaticUpdateInProgress);
			m_bAutomaticUpdateInProgress = true;
		}

		void CHub::AutomaticUpdateEnd()
		{
			assert(m_bAutomaticUpdateInProgress);
			m_bAutomaticUpdateInProgress = false;
		}

		void CHub::OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam)
		{
			if (event == ESYSTEM_EVENT_GAME_POST_INIT_DONE)
			{
				//
				// - ask all sub-systems to register their factories (before doing the consistency checks below!!!)
				// - the sub-systems should have used ESYSTEM_EVENT_GAME_POST_INIT (*not* the _DONE event) to subscribe to the IHub for receiving events
				//

				SendHubEventToAllListeners(UQS::Core::EHubEvent::RegisterYourFactoriesNow);

				//
				// instantiate all factories from the StdLib
				//

				if (!(m_overrideFlags & EHubOverrideFlags::InstantiateStdLibFactories))
				{
					StdLib::CStdLibRegistration::InstantiateAllFactoriesForRegistration();
				}

				//
				// register all factories from the StdLib (this happens implicitly)
				// (this is also necessary for monolithic build where the game code, for example, does *not* call Client::CFactoryRegistrationHelper::RegisterAllFactoryInstancesInHub)
				//

				Client::CFactoryRegistrationHelper::RegisterAllFactoryInstancesInHub(*this);

				//
				// check for consistency errors (this needs to be done *after* all subsystems registered their item types, functions, generators, evaluators)
				//

				CStartupConsistencyChecker startupConsistencyChecker;
				startupConsistencyChecker.CheckForConsistencyErrors();
				if (const size_t numErrors = startupConsistencyChecker.GetErrorCount())
				{
					for (size_t i = 0; i < numErrors; ++i)
					{
						if (gEnv->IsEditor())
						{
							// prepend "!" to the error message to make the error report pop up in Sandbox
							gEnv->pSystem->Warning(VALIDATOR_MODULE_UNKNOWN, VALIDATOR_ERROR, 0, 0, "!UQS consistency check: %s", startupConsistencyChecker.GetError(i));
						}
						else
						{
							gEnv->pLog->LogError("UQS consistency check: %s", startupConsistencyChecker.GetError(i));
						}
					}
				}

				// from now on, don't allow any further factory registrations (UQS::Core::CFactoryDatabase<>::RegisterFactory() will assert for it)
				m_bConsistencyChecksDoneAlready = true;

#if UQS_SCHEMATYC_SUPPORT
				static_assert((int)ESYSTEM_EVENT_REGISTER_SCHEMATYC_ENV == (int)ESYSTEM_EVENT_GAME_POST_INIT_DONE, "");

				//
				// register some stuff in schematyc
				//

				{
					const char* szName = "UniversalQuerySystem";
					const char* szDescription = "Universal Query System";
					Schematyc::EnvPackageCallback callback = SCHEMATYC_DELEGATE(&CHub::OnRegisterSchematycEnvPackage);
					gEnv->pSchematyc->GetEnvRegistry().RegisterPackage(SCHEMATYC_MAKE_ENV_PACKAGE(GetSchematycPackageGUID(), szName, Schematyc::g_szDinrus, szDescription, callback));
				}
#endif

				//
				// tell the game (or whoever "owns" the UQS instance) to load the query blueprints
				//

				if (m_overrideFlags & EHubOverrideFlags::InstallDatasourceAndLoadLibrary)
				{
					SendHubEventToAllListeners(EHubEvent::LoadQueryBlueprintLibrary);
				}
				else
				{
					m_pXmlDatasource.reset(new DataSource_XML::CXMLDatasource);
					m_pXmlDatasource->SetupAndInstallInHub(*this, "libs/ai/uqs");
				}
			}
#if UQS_SCHEMATYC_SUPPORT
			if (gEnv->pSchematyc)
			{
				if (event == ESYSTEM_EVENT_FULL_SHUTDOWN || event == ESYSTEM_EVENT_FAST_SHUTDOWN)
				{
					if (!(m_overrideFlags & EHubOverrideFlags::InstantiateStdLibFactories))
					{
						gEnv->pSchematyc->GetEnvRegistry().DeregisterPackage(GetSchematycPackageGUID());
					}
				}
			}
#endif 
		}

		void CHub::SendHubEventToAllListeners(EHubEvent ev)
		{
			for (std::list<IHubEventListener*>::const_iterator it = m_eventListeners.cbegin(); it != m_eventListeners.cend(); )
			{
				IHubEventListener* pListener = *it++;
				pListener->OnUQSHubEvent(ev);
			}
		}

#if UQS_SCHEMATYC_SUPPORT
		void CHub::OnRegisterSchematycEnvPackage(Schematyc::IEnvRegistrar& registrar)
		{
			CSchematycUqsComponent::Register(registrar);
		}
#endif

		void CHub::CmdListFactoryDatabases(IConsoleCmdArgs* pArgs)
		{
			if (g_pHub)
			{
				CLogger logger;
				g_pHub->m_queryFactoryDatabase.PrintToConsole(logger, "Query");
				g_pHub->m_itemFactoryDatabase.PrintToConsole(logger, "Item");
				g_pHub->m_functionFactoryDatabase.PrintToConsole(logger, "Function");
				g_pHub->m_generatorFactoryDatabase.PrintToConsole(logger, "Generator");
				g_pHub->m_instantEvaluatorFactoryDatabase.PrintToConsole(logger, "InstantEvaluator");
				g_pHub->m_deferredEvaluatorFactoryDatabase.PrintToConsole(logger, "DeferredEvaluator");
				g_pHub->m_scoreTransformFactoryDatabase.PrintToConsole(logger, "ScoreTransform");
			}
		}

		void CHub::CmdListQueryBlueprintLibrary(IConsoleCmdArgs* pArgs)
		{
			if (g_pHub)
			{
				CLogger logger;
				g_pHub->m_queryBlueprintLibrary.PrintToConsole(logger);
			}
		}

		void CHub::CmdListRunningQueries(IConsoleCmdArgs* pArgs)
		{
			if (g_pHub)
			{
				CLogger logger;
				g_pHub->m_queryUpr.PrintRunningQueriesToConsole(logger);
			}
		}

		void CHub::CmdDumpQueryHistory(IConsoleCmdArgs* pArgs)
		{
			if (g_pHub)
			{
				//
				// create an XML filename with a unique counter as part of it
				//

				stack_string unadjustedFilePath;

				for (int uniqueCounter = 0; uniqueCounter < 9999; ++uniqueCounter)
				{
					unadjustedFilePath.Format("%%USER%%/UQS_Logs/QueryHistory_%04i.xml", uniqueCounter);
					if (!gEnv->pDrxPak->IsFileExist(unadjustedFilePath))	// no need to call gEnv->pDrxPak->AdjustFileName() beforehand
						break;
				}

				char adjustedFilePath[IDrxPak::g_nMaxPath] = "";

				if (!gEnv->pDrxPak->AdjustFileName(unadjustedFilePath.c_str(), adjustedFilePath, IDrxPak::FLAGS_FOR_WRITING))
				{
					DrxWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "%s: Could not adjust the desired file path '%s' for writing", pArgs->GetArg(0), unadjustedFilePath.c_str());
					return;
				}

				Shared::CUqsString error;
				if (!g_pHub->m_queryHistoryUpr.SerializeLiveQueryHistory(adjustedFilePath, error))
				{
					DrxWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "%s: Serializing the live query to '%s' failed: %s", pArgs->GetArg(0), unadjustedFilePath.c_str(), error.c_str());
					return;
				}

				DrxLogAlways("Successfully dumped query history to '%s'", adjustedFilePath);
			}
		}

		void CHub::CmdLoadQueryHistory(IConsoleCmdArgs* pArgs)
		{
			if (g_pHub)
			{
				if (pArgs->GetArgCount() < 2)
				{
					DrxWarning(VALIDATOR_MODULE_GAME, VALIDATOR_WARNING, "usage: %s <xml query history file>", pArgs->GetArg(0));
					return;
				}

				const char* szXmlQueryHistoryFilePath = pArgs->GetArg(1);

				// check if the desired XML file exists at all (just for giving a more precise warning)
				if (!gEnv->pDrxPak->IsFileExist(szXmlQueryHistoryFilePath))	// no need to call gEnv->pDrxPak->AdjustFileName() beforehand
				{
					DrxWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "%s: File not found: '%s'", pArgs->GetArg(0), szXmlQueryHistoryFilePath);
					return;
				}

				Shared::CUqsString error;
				if (!g_pHub->m_queryHistoryUpr.DeserializeQueryHistory(szXmlQueryHistoryFilePath, error))
				{
					DrxWarning(VALIDATOR_MODULE_GAME, VALIDATOR_ERROR, "%s: Could not de-serialize the query history: ", pArgs->GetArg(0), szXmlQueryHistoryFilePath, error.c_str());
					return;
				}

				DrxLogAlways("Successfully de-serialized '%s'", szXmlQueryHistoryFilePath);
			}
		}

		void CHub::CmdClearLiveQueryHistory(IConsoleCmdArgs* pArgs)
		{
			if (g_pHub)
			{
				g_pHub->m_queryHistoryUpr.ClearQueryHistory(IQueryHistoryUpr::EHistoryOrigin::Live);
			}
		}

		void CHub::CmdClearDeserializedQueryHistory(IConsoleCmdArgs* pArgs)
		{
			if (g_pHub)
			{
				g_pHub->m_queryHistoryUpr.ClearQueryHistory(IQueryHistoryUpr::EHistoryOrigin::Deserialized);
			}
		}

	}
}
