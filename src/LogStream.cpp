#include "Essential.h"
#include "Util.h"
#include "Exception.h"
#include <random>
#include <chrono>

LogStream::LogStream(const char* pName, bool fAppend)
{
	_open = false;
	_pName = pName;
	_useCout = false;
	_fAppend = fAppend;
}

void LogStream::openStream()
{
	if (_open)
		return;

	if (_pName == NULL)
	{
		_open = true;
	}
	else
	if(Util::getFolderName()[0])
	{
		char sz[1024];
		strcpy(sz, Util::getFolderName());
		strcat(sz, "/");
		strcat(sz, _pName);
		_stream.open(sz, _fAppend ? std::ios_base::app : std::ios_base::trunc);
		_open = true;
	}
}

void LogStream::ws( const char* pFormat, ...)
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

	write("\n");
	write(sz);
}

void ilog( const char* pFormat, ...)
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

    char szIndent[512];
    N32 nChars = Indent::s_indent * Indent::IndentChars;

    N32 n = 0;

    for (; n < nChars; ++n)
        szIndent[n] = ' ';

    szIndent[n] = 0;

	g_logStream.write("\n");
	g_logStream.write(szIndent);
	g_logStream.write(sz);
//	g_logStream << "\n" << szIndent << sz;
}

void log( const char* pFormat, ...)
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

	g_logStream.write(sz);
}

// eof



