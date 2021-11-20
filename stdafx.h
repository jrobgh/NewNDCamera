#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <crtdbg.h>
#include <stdint.h>
#include <string>
#include <locale>         // std::locale, std::ctype, std::use_facet
#include <wtypes.h>
#include <cyioctl.h>
#pragma warning(push, 0)
#include <CyApi.h>
#pragma warning(pop)
#include <dbt.h>