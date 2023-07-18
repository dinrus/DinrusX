// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#if !defined(AFX_STDAFX_H__4AA14050_1A79_4A11_9G24_4E229BF87E2C__INCLUDED_)
#define AFX_STDAFX_H__4AA14050_1A79_4A11_9G24_4E229BF87E2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif                                                         // _MSC_VER > 1000

#include <DinrusX/CoreX/Project/DrxModuleDefs.h>
#define eDrxModule eDrxM_Physics
#define PHYSICS_EXPORTS
#include <DinrusX/CoreX/Platform/platform.h>

#if DRX_COMPILER_MSVC
#pragma warning (disable : 4554 4305 4244 4996)
#pragma warning (disable : 6326)                               //Potential comparison of a constant with another constant
#elif DRX_COMPILER_CLANG
#pragma clang diagnostic ignored "-Wdeprecated-declarations"   // MSVC equivalent C4996
#elif DRX_COMPILER_GCC
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"     // MSVC equivalent C4996
#endif

// C6246: Local declaration of <variable> hides declaration of same name in outer scope.
#define LOCAL_NAME_OVERRIDE_OK PREFAST_SUPPRESS_WARNING(6246)

#include <vector>
#include <map>
#include <algorithm>
#include <float.h>

#include <DinrusX/CoreX/Math/Drx_Math.h>

#define NO_DRX_STREAM

#ifndef NO_DRX_STREAM
#include "Stream.h"
#else
#include <DinrusX/DinrusXSys/ISystem.h>
#include <DinrusX/DinrusXSys/ILog.h>

class CStream
{
public:
	bool           WriteBits(unsigned char* pBits, uint32 nSize)   { return true; }
	bool           ReadBits(unsigned char* pBits, uint32 nSize)    { return true; }
	bool           Write(bool b)                                   { return true; }
	bool           Write(char c)                                   { return true; }
	bool           Write(unsigned char uc)                         { return true; }
	bool           Write(float f)                                  { return true; }
	bool           Write(unsigned short us)                        { return true; }
	bool           Write(short s)                                  { return true; }
	bool           Write(int i)                                    { return true; }
	bool           Write(unsigned int ui)                          { return true; }
	bool           Write(const Vec3& v)                            { return true; }
	bool           Write(const Ang3& v)                            { return true; }
	bool           Read(bool& b)                                   { return true; }
	bool           Read(char& c)                                   { return true; }
	bool           Read(unsigned char& uc)                         { return true; }
	bool           Read(unsigned short& us)                        { return true; }
	bool           Read(short& s)                                  { return true; }
	bool           Read(int& i)                                    { return true; }
	bool           Read(unsigned int& ui)                          { return true; }
	bool           Read(float& f)                                  { return true; }
	bool           Read(Vec3& v)                                   { return true; }
	bool           Read(Ang3& v)                                   { return true; }
	bool           WriteNumberInBits(int n, size_t nSize)          { return true; }
	bool           WriteNumberInBits(unsigned int n, size_t nSize) { return true; }
	bool           ReadNumberInBits(int& n, size_t nSize)          { return true; }
	bool           ReadNumberInBits(unsigned int& n, size_t nSize) { return true; }
	bool           Seek(size_t dwPos = 0)                          { return true; }
	size_t         GetReadPos()                                    { return 0; }
	unsigned char* GetPtr() const                                  { return 0; };
	size_t         GetSize() const                                 { return 0; }
	bool           SetSize(size_t indwBitSize)                     { return true; }
};
#endif

// debug physx / print every function-name with return value
#include <boost/current_function.hpp>
// disable/enable the logging
#define DRX_PHYSX_LOG_FUNCTION DrxLogAlways("DRXPHYSX EMPTY FUNCTION CALL: %s\n", BOOST_CURRENT_FUNCTION);
//#define DRX_PHYSX_LOG_FUNCTION {}
#define  _RETURN_FLOAT_DUMMY_ {return 0.0f;}
#define  _RETURN_PTR_DUMMY_ {return nullptr;}
#define  _RETURN_INT_DUMMY_ {return 0;}

// TODO: reference additional headers your program requires here
#include <DinrusX/CoreX/Memory/DrxSizer.h>
#include <DinrusX/DinrusXPhys/primitives.h>
#include <DinrusX/DinrusXPhys/utils.h>
#include <DinrusX/DinrusXPhys/physinterface.h>
#include "DrxPhysX.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__4AA14050_1B79_4A11_9D24_4E209BF87E2C__INCLUDED_)
