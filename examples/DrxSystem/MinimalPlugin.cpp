#include <DinrusX/DinrusXSys/IDrxPlugin.h>

// Minimal plug-in implementation example, initializing and receiving per-frame Update callbacks
class CMyPlugin final : public Drx::IEnginePlugin
{
	// Start declaring the inheritance hierarchy for this plug-in
	// This is followed by any number of DRXINTERFACE_ADD, passing an interface implementing IDrxUnknown that has declared its own GUID using DRXINTERFACE_DECLARE_GUID
	DRXINTERFACE_BEGIN()
	// Indicate that we implement Drx::IEnginePlugin, this is mandatory in order for the plug-in instance to be detected after the plug-in has been loaded
	DRXINTERFACE_ADD(Drx::IEnginePlugin)
	// End the declaration of inheritance hierarchy
	DRXINTERFACE_END()

	// Set the GUID for our plug-in, this should be unique across all used plug-ins
	// Can be generated in Visual Studio under Tools -> Create GUID
	DRXGENERATE_SINGLETONCLASS_GUID(CMyPlugin, "MyPluginName", "{0C7B8742-5FBF-4C48-AE7C-6E70308538EC}"_drx_guid)

	// Called shortly after loading the plug-in from disk
	// This is usually where you would initialize any third-party APIs and custom code
	virtual bool Initialize(SSystemGlobalEnvironment& env, const SSystemInitParams& initParams) override
	{
		/* Initialize plug-in here */

		// Make sure that we receive per-frame call to MainUpdate
		EnableUpdate(IEnginePlugin::EUpdateStep::MainUpdate, true);

		return true;
	}

	virtual void MainUpdate(float frameTime) override
	{
		/* Perform update here */
	}
};

// Register the factory that can create this plug-in instance
// Note that this has to be done in a source file that is not included anywhere else.
DRXREGISTER_SINGLETON_CLASS(CMyPlugin)