// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.
//
//  Dinrus Engine Source File.
//  Разработка (C), Dinrus Studios, 2006.
// -------------------------------------------------------------------------
//  File name:   ColladaCompiler.h
//  Version:     v1.00
//  Created:     3/4/2006 by Michael Smith
//  Компиляторы:   mingw-w64-clang-x86_64
//  Описание: 
// -------------------------------------------------------------------------
//  History:
//
////////////////////////////////////////////////////////////////////////////

#ifndef __COLLADACOMPILER_H__
#define __COLLADACOMPILER_H__

#include "SingleThreadedConverter.h"

class IDrxXML;
class CContentCGF;
class CPhysicsInterface;
struct sMaterialLibrary;
enum ColladaAuthorTool;
struct IPakSystem;
struct ExportFile;

class ColladaCompiler : public CSingleThreadedCompiler
{
public:
	ColladaCompiler(IDrxXML* pDrxXML, IPakSystem* pPakSystem);
	virtual ~ColladaCompiler();

	// IConverter methods.
	virtual const char* GetExt(int index) const;

	// ICompiler methods.
	virtual void BeginProcessing(const IConfig* config) { }
	virtual void EndProcessing() { }
	virtual bool Process();
	string GetOutputFileNameOnly() const;

private:
	void WriteMaterialLibrary(sMaterialLibrary& materialLibrary, const string& matFileName);
	
	void SetExportInfo(CContentCGF* const pCompiledCGF, ExportFile &exportFile);

	bool CompileToCGF(ExportFile &exportFile, const string& exportFileName);
	bool CompileToCHR(ExportFile &exportFile, const string& exportFileName);
	bool CompileToANM(ExportFile &exportFile, const string& exportFileName);
	bool CompileToCAF(ExportFile &exportFile, const string& exportFileName);

	void PrepareCGF(CContentCGF* pCGF);
	void FindUsedMaterialIDs(std::vector<int>& usedMaterialIDs, CContentCGF* pCGF);

private:
	IDrxXML* pDrxXML;
	IPakSystem* pPakSystem;
	CPhysicsInterface* pPhysicsInterface;
};

#endif //__COLLADACOMPILER_H__
