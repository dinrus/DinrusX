// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#include <DinrusX/DinrusXRend/StdAfx.h>
#include <DinrusX/DinrusXRend/CDrxVKGIAdapter.hpp>

#include <DinrusX/DinrusXRend/CDrxVKGIFactory.hpp>
#include <DinrusX/DinrusXRend/CDrxVKGIOutput.hpp>

CDrxVKGIAdapter* CDrxVKGIAdapter::Create(CDrxVKGIFactory* pFactory, UINT Adapter)
{
	return new CDrxVKGIAdapter(pFactory, Adapter);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CDrxVKGIAdapter::CDrxVKGIAdapter(CDrxVKGIFactory* pFactory, UINT index)
	: m_pFactory(pFactory),
	m_deviceIndex(index)
{
	VK_FUNC_LOG();
}

CDrxVKGIAdapter::~CDrxVKGIAdapter()
{
	VK_FUNC_LOG();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE CDrxVKGIAdapter::EnumOutputs(UINT Output, _COM_Outptr_ IDXGIOutput** ppOutput)
{
	VK_FUNC_LOG();
	* ppOutput = CDrxVKGIOutput::Create(this, Output);
	return *ppOutput ? S_OK : E_FAIL;
}
