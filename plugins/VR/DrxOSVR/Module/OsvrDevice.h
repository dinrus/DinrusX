// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

<DinrusX/DinrusXSys/VR/IHMDDevice.h>
<DinrusX/../Interface/IHmdOSVRDevice.h>

<DinrusX/osvr/ClientKit/ContextC.h>
<DinrusX/osvr/ClientKit/DisplayC.h>
<DinrusX/osvr/ClientKit/InterfaceStateC.h>
<DinrusX/osvr/RenderKit/RenderUprC.h>
#if DRX_PLATFORM_WINDOWS
<DinrusX/osvr/RenderKit/RenderUprD3D11C.h>
#else
<DinrusX/osvr/RenderKit/RenderUprOpenGLC.h>
#endif
<DinrusX/CoreX/Renderer/Tarray.h>

<DinrusX/DinrusXSys/VR/IHMDUpr.h>

struct OSVR_PoseReport;
struct OSVR_ImagingReport;
struct OSVR_TimeValue;

namespace DrxVR
{
namespace Osvr
{
class Device : public IOsvrDevice, public ISystemEventListener, public IHmdEventListener
{
public:
	// IHMDDevice interface
	virtual void                    AddRef() override;
	virtual void                    Release() override;
	int                             GetRefCount() const       { return m_refCount; }

	virtual EHmdClass               GetClass() const override { return eHmdClass_Osvr; }
	virtual void                    GetDeviceInfo(HmdDeviceInfo& info) const override;

	virtual void                    GetCameraSetupInfo(float& fov, float& aspectRatioFactor) const override;
	virtual HMDCameraSetup GetHMDCameraSetup(int nEye, float ratio, float fnear) const override;

	virtual void                    DisableHMDTracking(bool disable) override;
	virtual void                    UpdateInternal(EInternalUpdate) override {};
	virtual void                    RecenterPose() override;
	virtual void                    UpdateTrackingState(EVRComponent, std::uint64_t frameId) override;
	virtual const HmdTrackingState& GetNativeTrackingState() const override;
	virtual const HmdTrackingState& GetLocalTrackingState() const override;
	virtual Quad GetPlayArea() const override { return Quad(ZERO); }
	virtual Vec2 GetPlayAreaSize() const override { return Vec2(ZERO); }

	virtual stl::optional<Matrix34> RequestAsyncCameraUpdate(uint64_t frameId, const Quat& q, const Vec3 &p) override { return stl::nullopt; }

	virtual const IHmdController*   GetController() const override;
	virtual void                    GetPreferredRenderResolution(unsigned int& width, unsigned int& height);
	virtual int                     GetControllerCount() const override { return 2; } // DARIO_SKYHARBOR: HACK. NOT RELEVANT FOR SKYHARBOR ANYWAY

	static Device*                  CreateAndInitializeDevice();

	//IOsvrDevice
	virtual bool InitializeRenderer(void* d3dDevice, void* d3dContext);
	virtual bool RegisterTextureSwapSet(TextureSwapSet* swapSet);
	virtual bool PresentTextureSet(int textureSetIndex);
	virtual void ShutdownRenderer();

	//ISystemEventListener
	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam);

	// IHEventListener
	virtual void OnRecentered();
	// ~IHmdEventListener

private:

	enum Eye
	{
		Left  = 0,
		Right = 1
	};

	struct EyeSetup
	{
		EyeSetup()
			: fov(1.853f),
			aspectRatio(2.f),
			asymV(0),
			asymH(0),
			eyeDist(0.064f)
		{}
		float fov;
		float aspectRatio;
		float asymV;
		float asymH;
		float eyeDist;
	};

	EyeSetup                               m_eyes[2];
	HmdTrackingState                       m_localTrackingState;
	HmdTrackingState                       m_nativeTrackingState;
	HmdTrackingState                       m_disabledTrackingState;
	HmdDeviceInfo                          m_deviceInfo;

	std::string                            m_model;
	std::string                            m_vendor;

	OSVR_ClientContext                     m_context;
	OSVR_ClientInterface                   m_headInterface;
	OSVR_DisplayConfig                     m_displayConfig;
	OSVR_RenderUpr                     m_renderUpr;
	OSVR_RenderUprD3D11                m_renderUprD3D11;
	Quat                                   m_recenterQuat;
	OSVR_RenderParams                      m_renderParams;
	TArray<OSVR_RenderInfoD3D11>           m_latestRenderInfo;
	TArray<TArray<OSVR_RenderBufferD3D11>> m_renderBufferSets;
	float                                  m_maxFovY;
	float                                  m_maxAspect;
	volatile int                           m_refCount;
	int m_lastFrameTrackingStateUpdated;
	bool                                   m_disableTracking;
	bool                                   m_loadingScreenActive;

	Device();
	virtual ~Device();

	void Shutdown();

	bool InitializeDevice();
	void CalculateSymmetricalFovsFromDisplayConfig(float& fovH, float& fovV);
	bool UpdateEyeSetups();
	void UpdatePoseFromOsvrPose(const OSVR_PoseState& newPose);
	void UpdatePoseFromRenderInfo();
	void UpdatePose(const Quat& orientation, const Vec3& translation);
	void UpdateRenderInfo();

	ICVar*                m_pHmdSocialScreenKeepAspectCVar;
	ICVar*                m_pHmdSocialScreenCVar;
};
}

}