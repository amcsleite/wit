#pragma once
#include "Essential.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include "Util.h"

class WriteVar
{
	FILE* _pF;
    char _szFolderPath[1024];
    char _szPathAndName[1024];
	
public:

    WriteVar(const char* pPath, const char* pName)
    {
        strcpy(_szFolderPath, Util::getFolderName());
        strcat(_szFolderPath, "/");
        strcat(_szFolderPath, pPath);

        mkpath(_szFolderPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

		strcpy(_szPathAndName, _szFolderPath);
        strcat(_szPathAndName, "/");
        strcat(_szPathAndName, pName);

        _pF = fopen(_szPathAndName, "w");
    }

    ~WriteVar()
    {
        if (_pF)
            fclose(_pF);
    }
	
    void close()
    {
        if (_pF)
        {
			fclose(_pF);
			_pF = NULL;
		}
    }

	FILE* getFile()
	{
		return _pF;
	}

	const char* getFolderPath()
	{
		return _szFolderPath;
	}

	const char* getPathAndName()
	{
		return _szPathAndName;
	}

    void writeInt(N32 v)
    {
        fprintf(_pF, "%d", v);
        char str[0x100];

        str[0] = 0;

        fgets(str, sizeof(str), _pF);
    }

    void writeDouble(double v)
    {
        fprintf(_pF, "%f", v);
    }

    void ws( const char* pFormat, ...)
    {
        char sz[1024];
        sz[0] = 0;

        va_list arglist;
        va_start(arglist, pFormat);

        int nTotal = vsnprintf(sz, sizeof(sz), pFormat, arglist);

        if (nTotal == -1)
        {
            XK(false); // not enough space in buffer
        }

        fputs(sz, _pF);
    }

};

// eof
