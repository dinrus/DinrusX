// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

// #SchematycTODO : Can we combine cleanup with sort? Move empty connections to end?

#pragma once

#include <DinrusX/DinrusXSchema2/TemplateUtils_Delegate.h>
#include <DinrusX/DinrusXSchema2/TemplateUtils_ScopedConnection.h>
#include <DinrusX/DinrusXSchema2/TemplateUtils_PreprocessorUtils.h>

namespace TemplateUtils
{
	// Signal queue class.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//template <typename SIGNATURE, typename KEY = void, typename VALUE = SSignalParamsv2<SIGNATURE> > class CSignalQueuev2;

	template <typename TYPE> struct SDefaultSignalKeyCompare
	{
		typedef std::less<TYPE> Type;
	};

	template <> struct SDefaultSignalKeyCompare<void>
	{
		typedef void Type;
	};

	// Signal class.
	////////////////////////////////////////////////////////////////////////////////////////////////////
	template <typename SIGNATURE, typename KEY = void, class KEY_COMPARE = typename SDefaultSignalKeyCompare<KEY>::Type > class CSignalv2;
}

// Signal template specializations.
////////////////////////////////////////////////////////////////////////////////////////////////////
#define INCLUDING_FROM_TEMPLATE_UTILS_SIGNAL_HEADER

#define PARAM_COUNT 0
#include <DinrusX/DinrusXSchema2/TemplateUtils_SignalImplv2.h>
#undef PARAM_COUNT

#define PARAM_COUNT 1
#include <DinrusX/DinrusXSchema2/TemplateUtils_SignalImplv2.h>
#undef PARAM_COUNT

#define PARAM_COUNT 2
#include <DinrusX/DinrusXSchema2/TemplateUtils_SignalImplv2.h>
#undef PARAM_COUNT

#define PARAM_COUNT 3
#include <DinrusX/DinrusXSchema2/TemplateUtils_SignalImplv2.h>
#undef PARAM_COUNT

#define PARAM_COUNT 4
#include <DinrusX/DinrusXSchema2/TemplateUtils_SignalImplv2.h>
#undef PARAM_COUNT

#define PARAM_COUNT 5
#include <DinrusX/DinrusXSchema2/TemplateUtils_SignalImplv2.h>
#undef PARAM_COUNT

#define PARAM_COUNT 6
#include <DinrusX/DinrusXSchema2/TemplateUtils_SignalImplv2.h>
#undef PARAM_COUNT

#define PARAM_COUNT 7
#include <DinrusX/DinrusXSchema2/TemplateUtils_SignalImplv2.h>
#undef PARAM_COUNT

#define PARAM_COUNT 8
#include <DinrusX/DinrusXSchema2/TemplateUtils_SignalImplv2.h>
#undef PARAM_COUNT

#undef INCLUDING_FROM_TEMPLATE_UTILS_SIGNAL_HEADER