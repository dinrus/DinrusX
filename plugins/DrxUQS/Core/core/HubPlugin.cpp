// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include "StdAfx.h"

// *INDENT-OFF* - <hard to read code and declarations due to inconsistent indentation>

// - needs to be included once per DLL module
// - otherwise, you'd get linker errors regarding DrxAssert, memory manager, etc.
// - not required when building a .lib
<DinrusX/CoreX/Platform/platform_impl.inl>


namespace UQS
{
	namespace Core
	{
		//===================================================================================
		//
		// CHubPlugin
		//
		//===================================================================================

		class CHubPlugin : public IHubPlugin
		{
			DRXINTERFACE_BEGIN()
			DRXINTERFACE_ADD(IHubPlugin)
			DRXINTERFACE_ADD(Drx::IEnginePlugin)
			DRXINTERFACE_END()

			DRXGENERATE_SINGLETONCLASS_GUID(CHubPlugin, "Plugin_UQS", "2a2f00e0-f068-4baf-b31b-b3c8f78b3477"_drx_guid)

			CHubPlugin();
			virtual ~CHubPlugin() = default;

		private:
			// Drx::IEnginePlugin
			virtual const char*                  GetName() const override;
			virtual const char*                  GetCategory() const override;
			virtual bool                         Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override;
			virtual void                         MainUpdate(float frameTime) override;
			// ~Drx::IEnginePlugin

			// IHubPlugin
			virtual void                         RegisterHubPluginEventListener(IHubPluginEventListener* pListenerToRegister) override;
			virtual void                         UnregisterHubPluginEventListener(IHubPluginEventListener* pListenerToUnregister) override;
			virtual IHub&                        GetHubImplementation() override;
			virtual void                         TearDownHubImplementation() override;
			// ~IHubPlugin

		private:
			std::unique_ptr<CHub>                m_pHub;
			std::list<IHubPluginEventListener*>  m_hubPluginEventListeners;
		};

		DRXREGISTER_SINGLETON_CLASS(CHubPlugin)

		// the ctor and dtor were declared by some code-weaver macros (specifically by _ENFORCE_DRXFACTORY_USAGE() which is used by DRXGENERATE_SINGLETONCLASS())

		CHubPlugin::CHubPlugin()
		{
			m_pHub = stl::make_unique<CHub>();
		}

		const char* CHubPlugin::GetName() const
		{
			return "UQS";
		}

		const char* CHubPlugin::GetCategory() const
		{
			return "Plugin";
		}

		bool CHubPlugin::Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams)
		{
			// Notice: we currently do *not* yet instantiate the UQS Hub here, because some of its sub-systems rely on the presence of sub-systems of ISystem, like IInput, which
			// are still NULL at this point in time. So instead, we instantiate the UQS Hub upon ESYSTEM_EVENT_DRXSYSTEM_INIT_DONE (see OnSystemEvent()).
			EnableUpdate(EUpdateStep::MainUpdate, true);
			return true;
		}

		void CHubPlugin::MainUpdate(float frameTime)
		{
			if (gEnv->IsEditing())
				return;	// leave it to the UQS editor-plugins to update the IHub

			if (!m_pHub.get())
				return;	// IHub got torn down already

			if (!(m_pHub->GetOverrideFlags() & EHubOverrideFlags::CallUpdate))
			{
				//
				// update the IHub to process all running queries and to do some basic on-screen 2D debug rendering
				//

				m_pHub->AutomaticUpdateBegin();
				m_pHub->Update();
				m_pHub->AutomaticUpdateEnd();
			}

			if (!(m_pHub->GetOverrideFlags() & EHubOverrideFlags::CallUpdateDebugRendering3D))
			{
				//
				// update the 3D debug rendering of all items of all queries
				//

				CQueryHistoryUpr& queryHistoryUpr = m_pHub->GetQueryHistoryUpr();
				queryHistoryUpr.AutomaticUpdateDebugRendering3DBegin();

				const CCamera& sysCamera = gEnv->pSystem->GetViewCamera();
				SDebugCameraView debugCameraView;
				debugCameraView.pos = sysCamera.GetPosition();
				debugCameraView.dir = sysCamera.GetViewdir();
				queryHistoryUpr.UpdateDebugRendering3D(&debugCameraView, IQueryHistoryUpr::SEvaluatorDrawMasks::CreateAllBitsSet());

				queryHistoryUpr.AutomaticUpdateDebugRendering3DEnd();
			}
		}

		void CHubPlugin::RegisterHubPluginEventListener(IHubPluginEventListener* pListenerToRegister)
		{
			stl::push_back_unique(m_hubPluginEventListeners, pListenerToRegister);
		}

		void CHubPlugin::UnregisterHubPluginEventListener(IHubPluginEventListener* pListenerToUnregister)
		{
			m_hubPluginEventListeners.remove(pListenerToUnregister);
		}

		IHub& CHubPlugin::GetHubImplementation()
		{
			// if this assert fails, then some client code tried to access the one and only UQS Hub instance before ESYSTEM_EVENT_DRXSYSTEM_INIT_DONE (where it gets instantiated).
			assert(m_pHub.get());
			return *m_pHub.get();
		}

		void CHubPlugin::TearDownHubImplementation()
		{
			if (m_pHub.get())
			{
				//
				// notify all listeners
				// (notice that since we're using a std::list, the currently being called listener is even allowed to unregister himself
				//  during the callback without affecting the remaining iterators)
				//

				for (auto it = m_hubPluginEventListeners.begin(); it != m_hubPluginEventListeners.end(); )
				{
					IHubPluginEventListener* pListener = *it++;
					pListener->OnHubPluginEvent(IHubPluginEventListener::HubIsAboutToGetTornDown);
				}

				//
				// prematurely cancel all running queries
				//

				m_pHub->GetQueryUpr().CancelAllRunningQueriesDueToUpcomingTearDownOfHub();

				//
				// destroy the one and only Hub instance
				//

				m_pHub.reset();
			}
		}
	}
}
