#pragma once
#include "Essential.h"
#include "DIGenerator.h"
#include "Expression.h"
#include "ExpressionList.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>

class LogStream
{
	bool _open;
	bool _useCout;
	
public:
    const char* _pName;
	bool _fAppend;
    std::ofstream _stream;

    LogStream(const char* pName, bool bAppend = true);

    template<typename T>
	const LogStream& operator<<(const T& v) const
    {
        _stream << v;
        return *this;
    }
	
	void openStream();
	void ws( const char* pFormat, ...);

	void setUseCout()
	{
		_useCout = true;
	}

	void setTruncate()
	{
		_fAppend = false;
	}

	void write(const char *p)
	{
		openStream();

		if(_useCout)
			std::cout << p;

		_stream << p;
	}

	std::ostream& getStream()
	{
		if(_useCout)
			return std::cout;
		else
			return _stream;
	}
};

extern LogStream g_logStream;

void log(const char* pFormat, ...);
void ilog(const char* pFormat, ...);


// eof
