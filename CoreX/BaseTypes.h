// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

// SWIG не может обработать static_assert, поэтому он скрыт под макросом.
#if SWIG
#define static_assert(...)
#endif

static_assert(sizeof(char) == 1, "Неверный размер типа!");
static_assert(sizeof(float) == 4, "Неверный размер типа!");
static_assert(sizeof(int) >= 4, "Неверный размер типа!");

typedef unsigned char  uchar;
typedef signed char    schar;

typedef unsigned short ushort;
typedef signed short   sshort;

#if !defined(CLANG_FIX_UINT_REDEF)
typedef unsigned int       uint;
#endif
typedef signed int         sint;

typedef unsigned long      ulong;
typedef signed long        slong;

typedef unsigned long long ulonglong;
typedef signed long long   slonglong;

static_assert(sizeof(uchar) == sizeof(schar), "Неверный размер типа!");
static_assert(sizeof(ushort) == sizeof(sshort), "Неверный размер типа!");
static_assert(sizeof(uint) == sizeof(sint), "Неверный размер типа!");
static_assert(sizeof(ulong) == sizeof(slong), "Неверный размер типа!");
static_assert(sizeof(ulonglong) == sizeof(slonglong), "Неверный размер типа!");

static_assert(sizeof(uchar) <= sizeof(ushort), "Неверный размер типа!");
static_assert(sizeof(ushort) <= sizeof(uint), "Неверный размер типа!");
static_assert(sizeof(uint) <= sizeof(ulong), "Неверный размер типа!");
static_assert(sizeof(ulong) <= sizeof(ulonglong), "Неверный размер типа!");

typedef schar int8;
typedef schar sint8;
typedef uchar uint8;
static_assert(sizeof(uint8) == 1, "Неверный размер типа!");
static_assert(sizeof(sint8) == 1, "Неверный размер типа!");

typedef sshort int16;
typedef sshort sint16;
typedef ushort uint16;
static_assert(sizeof(uint16) == 2, "Неверный размер типа!");
static_assert(sizeof(sint16) == 2, "Неверный размер типа!");

typedef sint int32;
typedef sint sint32;
typedef uint uint32;
static_assert(sizeof(uint32) == 4, "Неверный размер типа!");
static_assert(sizeof(sint32) == 4, "Неверный размер типа!");

typedef slonglong int64;
typedef slonglong sint64;
typedef ulonglong uint64;
static_assert(sizeof(uint64) == 8, "Неверный размер типа!");
static_assert(sizeof(sint64) == 8, "Неверный размер типа!");

typedef float  f32;
typedef double f64;
static_assert(sizeof(f32) == 4, "Неверный размер типа!");
static_assert(sizeof(f64) == 8, "Неверный размер типа!");
