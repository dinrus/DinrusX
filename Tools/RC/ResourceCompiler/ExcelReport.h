// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.
//
//  DinrusX Source File.
//  Copyright (C), Dinrus.
// -------------------------------------------------------------------------
//  File name:   ExcelReport.h
//  Created:     19/03/2008 by Timur.
//  Описание: ExcelReporter
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __ExcelReport_h__
#define __ExcelReport_h__
#pragma once

#include <DinrusX/DinrusXSys/ExcelExport.h>
#include "ConvertContext.h"

// Base class for custom DinrusX excel exporters
class CExcelReport : public CExcelExportBase
{
public:
	bool Export(IResourceCompiler* pRC, const char* filename, std::vector<CAssetFileInfo*>& files);

	void ExportSummary(const std::vector<CAssetFileInfo*>& files);
	void ExportTextures(const std::vector<CAssetFileInfo*>& files);
	void ExportCGF(const std::vector<CAssetFileInfo*>& files);
	void ExportCHR(const std::vector<CAssetFileInfo*>& files);
	void ExportCAF(const std::vector<CAssetFileInfo*>& files);

protected:
	IResourceCompiler *m_pRC;
};

#endif //__ExcelReport_h__