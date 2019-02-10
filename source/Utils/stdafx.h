#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif // _CRT_SECURE_NO_WARNINGS


#include <string>
#include <list>
#include "StrSafe.h"
#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <limits.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers


#include <tlhelp32.h>