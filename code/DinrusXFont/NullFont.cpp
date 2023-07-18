// Copyright 2018-2023 DinrusPro / Dinrus Group. РНЦП Динрус.

/*************************************************************************
   -------------------------------------------------------------------------
   $Id$
   $DateTime$
   Описание:	Dummy font implementation (dedicated server)
   -------------------------------------------------------------------------
   History:
   - Jun 30,2006:	Created by Sascha Demetrio

*************************************************************************/

#include <DinrusX/DinrusXFont/StdAfx.h>

#if defined(USE_NULLFONT)

	#include<DinrusX/DinrusXFont/NullFont.h>

CNullFont CDrxNullFont::ms_nullFont;

#endif // USE_NULLFONT
