// Разработка 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

#pragma once

// *INDENT-OFF* - <hard to read code and declarations due to inconsistent indentation>

namespace UQS
{
	namespace Core
	{

		//===================================================================================
		//
		// CLoggerIndentation
		//
		//===================================================================================

		class CLoggerIndentation
		{
		public:
			explicit                   CLoggerIndentation();
			                           ~CLoggerIndentation();
			static int                 GetCurrentIndentLevel();

		private:
			                           UQS_NON_COPYABLE(CLoggerIndentation);

		private:
			static int                 s_indentLevel;
		};

		//===================================================================================
		//
		// CLogger
		//
		//===================================================================================

		class CLogger
		{
		public:
			void                       Printf(const char* szFormat, ...) PRINTF_PARAMS(2, 3);
		};

	}
}
