// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   CDrxDXGLBlob.hpp
//  Version:     v1.00
//  Created:     20/02/2013 by Valerio Guagliumi.
//  Описание: Definition of the DXGL wrapper for ID3D10Blob
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#include <DinrusX/DinrusXRend/StdAfx.h>
#include <DinrusX/DinrusXRend/CDrxDXGLBlob.hpp>

CDrxDXGLBlob::CDrxDXGLBlob(size_t uBufferSize)
	: m_uBufferSize(uBufferSize)
#if defined(DXGL_BLOB_INTEROPERABILITY)
	, m_uRefCount(1)
#endif //defined(DXGL_BLOB_INTEROPERABILITY)
{
	DXGL_INITIALIZE_INTERFACE(D3D10Blob)
	m_pBuffer = new uint8[m_uBufferSize];
}

CDrxDXGLBlob::~CDrxDXGLBlob()
{
	delete[] m_pBuffer;
}

#if defined(DXGL_BLOB_INTEROPERABILITY)

////////////////////////////////////////////////////////////////////////////////
// IUnknown implementation
////////////////////////////////////////////////////////////////////////////////

HRESULT CDrxDXGLBlob::QueryInterface(REFIID riid, void** ppvObject)
{
	return E_NOINTERFACE;
}

ULONG CDrxDXGLBlob::AddRef(void)
{
	return ++m_uRefCount;
}

ULONG CDrxDXGLBlob::Release(void)
{
	--m_uRefCount;
	if (m_uRefCount == 0)
	{
		delete this;
		return 0;
	}
	return m_uRefCount;
}

#endif //defined(DXGL_BLOB_INTEROPERABILITY)

////////////////////////////////////////////////////////////////////////////////
// ID3D10Blob implementation
////////////////////////////////////////////////////////////////////////////////

LPVOID CDrxDXGLBlob::GetBufferPointer()
{
	return m_pBuffer;
}

SIZE_T CDrxDXGLBlob::GetBufferSize()
{
	return m_uBufferSize;
}
