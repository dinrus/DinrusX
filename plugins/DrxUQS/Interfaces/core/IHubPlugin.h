// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

<DinrusX/DinrusXSys/IDrxPlugin.h>
<DinrusX/DinrusXSys/IDrxPluginUpr.h>

// *INDENT-OFF* - <hard to read code and declarations due to inconsistent indentation>

namespace UQS
{
	namespace Core
	{

		//===================================================================================
		//
		// IHubPlugin
		//
		//===================================================================================

		class IHubPlugin : public Drx::IEnginePlugin
		{
			DRXINTERFACE_DECLARE_GUID(IHubPlugin, "de73f1db-0c37-498f-8396-d76e43988c60"_drx_guid)

			// virtual ~IHubPlugin() {} is already provided by the DRXINTERFACE_DECLARE class-weaver macro

		protected:

			//===================================================================================
			//
			// IHubPluginEventListener
			//
			//===================================================================================

			struct IHubPluginEventListener
			{
				//===================================================================================
				//
				// EEvent
				//
				//===================================================================================

				enum EEvent
				{
					HubIsAboutToGetTornDown
				};

				virtual           ~IHubPluginEventListener() {}
				virtual void      OnHubPluginEvent(EEvent ev) = 0;
			};


		private:

			//===================================================================================
			//
			// CCachedHubPtr
			//
			// - caches a pointer to the one and only global UQS Hub
			// - resets that pointer when the UQS plugin gets unloaded and retrieves the pointer again on first access
			//
			//===================================================================================

			class CCachedHubPtr : public Drx::IPluginUpr::IEventListener, public IHubPluginEventListener
			{
			public:
				explicit          CCachedHubPtr();
								  ~CCachedHubPtr();
				IHub*             GetHubPtr();

			private:
				// Drx::IPluginUpr::IEventListener
				virtual void      OnPluginEvent(const DrxClassID& pluginClassId, Drx::IPluginUpr::IEventListener::EEvent event) override;
				// ~Drx::IPluginUpr::IEventListener

				// IHubPluginEventListener
				virtual void      OnHubPluginEvent(IHubPluginEventListener::EEvent ev) override;
				// ~IHubPluginEventListener

			private:
				IHub*             m_pHub;
			};

		public:

			// these are meant to be used by game code and other client code to get access to the potentially instantiated global UQS Hub instance
			static IHub*          GetHubPtr();
			static IHub&          GetHub();

			// - this function allows to prematurely destroy the potentially existing one and only Hub instance before the Plugin system does it implicitly by its own shutdown
			// - can be useful to prevent crashes if some client code has registered item factories, and is about to get shut down *before* the UQS gets shut down
			// - such crashes can occur, for examples, if there there is a query blueprint that has as constant input-parameters (i. e. CVariantDict instances) when going out of scope
			static void           TearDownHub();

		private:
			virtual void          RegisterHubPluginEventListener(IHubPluginEventListener* pListenerToRegister) = 0;
			virtual void          UnregisterHubPluginEventListener(IHubPluginEventListener* pListenerToUnregister) = 0;
			virtual IHub&         GetHubImplementation() = 0;
			virtual void          TearDownHubImplementation() = 0;
		};

		// This can be called after the plugin system has loaded and instantiated the UQS to obtain a pointer to the one and only IHub instance.
		// Returns nullptr if the UQS plugin is not loaded or has already been unloaded.
		// Checking for nullptr can be used to see if the UQS is actually installed and available for use.
		inline IHub* IHubPlugin::GetHubPtr()
		{
			static CCachedHubPtr cachedHubPtr;
			return cachedHubPtr.GetHubPtr();  // might still be a nullptr
		}

		// Same functionality as GetHubPtr() except that it triggers a fatal error if no IHub has been installed yet.
		inline IHub& IHubPlugin::GetHub()
		{
			IHub* pHub = GetHubPtr();
			if (!pHub)
			{
				DrxFatalError("UQS hub not yet installed! (or got un-installed already)");
			}
			return *pHub;
		}

		inline void IHubPlugin::TearDownHub()
		{
			if (IHubPlugin* pHubPlugin = gEnv->pSystem->GetIPluginUpr()->QueryPlugin<IHubPlugin>())
			{
				pHubPlugin->TearDownHubImplementation();
			}
		}

		inline IHubPlugin::CCachedHubPtr::CCachedHubPtr()
			: m_pHub(nullptr)
		{
			gEnv->pSystem->GetIPluginUpr()->RegisterEventListener<IHubPlugin>(this);
		}

		inline IHubPlugin::CCachedHubPtr::~CCachedHubPtr()
		{
			if (gEnv && gEnv->pSystem && gEnv->pSystem->GetIPluginUpr())
			{
				gEnv->pSystem->GetIPluginUpr()->RemoveEventListener<IHubPlugin>(this);
			}
		}

		inline IHub* IHubPlugin::CCachedHubPtr::GetHubPtr()
		{
			if (!m_pHub)
			{
				if (IHubPlugin* pHubPlugin = gEnv->pSystem->GetIPluginUpr()->QueryPlugin<IHubPlugin>())
				{
					m_pHub = &pHubPlugin->GetHubImplementation();
					pHubPlugin->RegisterHubPluginEventListener(this);  // get notified when someone is tearing down the one and only IHub instance so that we can reset m_pHub (otherwise it will start do dangle)
				}
			}
			return m_pHub;  // might still be a nullptr
		}

		inline void IHubPlugin::CCachedHubPtr::OnPluginEvent(const DrxClassID& pluginClassId, Drx::IPluginUpr::IEventListener::EEvent event)
		{
			if (pluginClassId == drxiidof<IHubPlugin>())
			{
				// reset the cached Hub pointer when the plugin gets unloaded
				if (event == Drx::IPluginUpr::IEventListener::EEvent::Unloaded)
				{
					m_pHub = nullptr;
				}
			}
		}

		inline void IHubPlugin::CCachedHubPtr::OnHubPluginEvent(IHubPluginEventListener::EEvent ev)
		{
			// reset the cached Hub pointer when the Hub itself is just about to get torn down by the plugin
			if (ev == IHubPluginEventListener::EEvent::HubIsAboutToGetTornDown)
			{
				m_pHub = nullptr;

				IHubPlugin* pHubPlugin = gEnv->pSystem->GetIPluginUpr()->QueryPlugin<IHubPlugin>();
				assert(pHubPlugin);
				pHubPlugin->UnregisterHubPluginEventListener(this);
			}
		}

	}
}
