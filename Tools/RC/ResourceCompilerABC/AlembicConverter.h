// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.
//
//  Dinrus Engine Source File.
//  Разработка (C), Dinrus Studios, 2012.
// -------------------------------------------------------------------------
//  File name:   AlembicConverter.h
//  Created:     20/7/2012 by Axel Gneiting
//
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IConverter.h"

class IDrxXML;

class AlembicConverter : public IConverter
{
public:
	AlembicConverter(IDrxXML *pXMLParser, IPakSystem* pPakSystem);
	virtual ~AlembicConverter();

	// IConverter methods.
	virtual void Release();
	virtual void DeInit() {}
	virtual ICompiler *CreateCompiler();
	virtual bool SupportsMultithreading() const;
	virtual const char *GetExt(int index) const;

private:
	int m_refCount;
	IDrxXML *m_pXMLParser;
	IPakSystem *m_pPakSystem;
};