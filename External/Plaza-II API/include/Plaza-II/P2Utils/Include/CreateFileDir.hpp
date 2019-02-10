// CreateFileDir.hpp: implementation of CreateFileDir.
//
//////////////////////////////////////////////////////////////////////
#ifndef _P2UTILS_CREATEFILEDIR_HPP__INCLUDED_
#define _P2UTILS_CREATEFILEDIR_HPP__INCLUDED_


#if defined(__linux__) || defined(__MACH__)
//#include <sys/io.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#define MAX_PATH PATH_MAX
#else
#include "../../P2Common/Include/P2Windows.h"
#include <direct.h>
#include <io.h>
#include <errno.h>
#endif

#include <stdio.h>
#include <string>

#if defined(__linux__) || defined(__MACH__)

static bool CreateFileDir(const char* path, bool isDir = false) 
{

	if(!path) 
		return false;

    char  thePath[PATH_MAX]; 
	char* pChar;
	int   err;

	strncpy(thePath, path, PATH_MAX);
	thePath[PATH_MAX -1] = 0;

	// search first non existence dir
	pChar = thePath;
	err = 0;
	while(*pChar != 0 && err != -1)
	{ 
		do{ ++pChar; } while(*pChar != 0 && *pChar != '/');

		if(*pChar == '/')
		{ 
			*pChar = 0; 
			err = access(thePath, F_OK);
			*pChar = '/'; 
		} 
	}

	if(err == -1 && errno != ENOENT)
		return false;

	// create all needed dir 
	err = 0;
	while (*pChar != 0 && err == 0)
	{ 
		*pChar = 0; 
		err = mkdir(thePath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if (err != 0 && errno == EEXIST) err = 0; 
		*pChar = '/'; 
		do{ ++pChar; } while(*pChar != 0 && *pChar != '/'); 
	} 
	return err == 0; 
}
#else
static bool CreateFileDir(const char* path, bool isDir = false)
{
    _ASSERT(path);

    // Make full path for outside call
    char fullPath[_MAX_PATH];
    if (!isDir)
    {
        if (_fullpath(fullPath, path, sizeof(fullPath)) == 0)
            return false;
        path = fullPath;
    }

    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];
    _splitpath(path, drive, dir, fname, ext);


	std::string dir_path(drive);
    size_t dir_len = strlen(dir);
    if (dir_len > 2)    // Directory part not empty
    {
        // Make directory name without last slash
        dir_path.append(dir, dir_len - 1);

        // Check directory existence
        if (_access(dir_path.c_str(), 0) != 0)
        {
            if (errno == EACCES)
                return false;  // Access denied

            // Create new dir
            if (!CreateFileDir(dir_path.c_str(), true))
                return false;
        }
    } 
	if (isDir && strspn(dir, "\\/") < dir_len)
	{
		int err = _mkdir(path);
		if (!err || errno == EEXIST) return true; 
		return false; 
	}
	return true; 
}
#endif // defined(__linux__) || defined(__MACH__)

#endif // _P2UTILS_CREATEFILEDIR_HPP__INCLUDED_
