// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <stdlib.h>
#include <string>
#include <exception>
#include <vector>
#include <memory>
#include <unordered_map>
#include <deque>
#include <numeric>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Plaza2 API Headers
#include "P2Windows.h"
#include "P2Sys.h"
#include "P2SysIni.h"
#include "P2Errors.h"
#include "P2SysLog.h"
#include "p2ini.h"
#include "p2log.h"
#include "p2time.h"
