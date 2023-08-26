// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// -------------------------------------------------------------------------
//  File name:   CDrxDXGLBlob.hpp
//  Version:     v1.00
//  Created:     20/02/2013 by Valerio Guagliumi.
//  Описание: Declaration of the DXGL wrapper for ID3D10Blob
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __DRXDXGLBLOB__
#define __DRXDXGLBLOB__

#include <DinrusX/DinrusXRend/CDrxDXGLBase.hpp>

#if defined(DXGL_BLOB_INTEROPERABILITY) && !DXGL_FULL_EMULATION
class CDrxDXGLBlob : public ID3D10Blob
#else
class CDrxDXGLBlob : public CDrxDXGLBase
#endif
{
public:
	DXGL_IMPLEMENT_INTERFACE(CDrxDXGLBlob, D3D10Blob)

	CDrxDXGLBlob(size_t uBufferSize);
	virtual ~CDrxDXGLBlob();

#if defined(DXGL_BLOB_INTEROPERABILITY)
	//IUnknown implementation
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);
	ULONG STDMETHODCALLTYPE   AddRef();
	ULONG STDMETHODCALLTYPE   Release();
#endif //defined(DXGL_BLOB_INTEROPERABILITY)

	// ID3D10Blob implementation
	LPVOID STDMETHODCALLTYPE GetBufferPointer();
	SIZE_T STDMETHODCALLTYPE GetBufferSize();
protected:
#if defined(DXGL_BLOB_INTEROPERABILITY)
	uint32 m_uRefCount;
#endif //defined(DXGL_BLOB_INTEROPERABILITY)

	uint8 * m_pBuffer;
	size_t m_uBufferSize;
};

#endif //__DRXDXGLBLOB__
