// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.


#pragma once
#include <DinrusX/DinrusXRend/D3DRender/Vulkan/VKBase.hpp>

namespace NDrxVulkan
{
	class CShader : public CDeviceObject
	{
	public:
		CShader(CDevice* pDevice);
		virtual ~CShader();

		VkResult Init(const void* pShaderCode, size_t shaderCodeSize);
		VkShaderModule GetVulkanShader() const { return m_shaderHandle; }
		void DebugSetName(const char* szName) { /*TODO*/ }

	private:
		VkShaderModule m_shaderHandle;
	};

}
