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

class ReadVar
{
	FILE* _pF;

public:

    ReadVar(const char* pPath, const char* pName)
	{
        char szPath[1024];
        strcpy(szPath, Util::getFolderName());
        strcat(szPath, "/");
        strcat(szPath, pPath);

        mkpath(szPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

        strcat(szPath, "/");
        strcat(szPath, pName);

	    _pF = fopen(szPath, "r");
	}
	
	~ReadVar()
	{
		if (_pF)
			fclose(_pF);
	}

	bool readInt(N32& v, N32 d)
	{
		if (!_pF)
		{
			v = d;
			return false;
		}
		
		char str[0x100];

		str[0] = 0;

		fgets(str, sizeof(str), _pF);
		v = atol(str);
		return true;
	}

	bool readDouble(double& v, double d)
	{
		if (!_pF)
		{
			v = d;
			return false;
		}
		
		char str[0x100];

		str[0] = 0;

		fgets(str, sizeof(str), _pF);
		v = atof(str);
		return true;
	}

	void readStr(char* p, N32 len)
	{
		if (!_pF)
		{
			*p = 0;
		}
		fgets(p, len, _pF);
	}
};

// eof
