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

#include "LogStream.h"

int mkpath(const char *path, mode_t mode);

class Util
{
public:
    static char szCheck[];
    static char s_folderName[1024];
    static inline bool fexists(const char* p);
    static inline bool ftouch(const char* p);
    static void errorMsg(const char* p);
    static ExpressionListPtr quickPerm(N32 n);
	static ExpressionListPtr genRotations(N32 nb);

    static bool mkFolder(int preparations, int measurements, int bits, int outputs);

    static bool writeIntParam(const char* name, N32 v);
    static bool writeDoubleParam(const char* name, double v);

    static bool readIntParam(const char* name, N32& v, N32 d);
    static bool readDoubleParam(const char* name, double& v, double d);

    static bool parsePortaPOI(const char* pName, ExpressionList* pList);
    static bool parseLRS(ExpressionList* pList, N32 preparations, N32 measurements, N32 vars);
    static bool parsePortaIEQ(const char* pName, ExpressionList* pList, N32 preparations, N32 measurements, N32 vars);
	static bool parsePandaIEQ(const char* pName, ExpressionList* pList, N32 preparations, N32 measurements, N32 vars);

	static double rDouble();

    static char* getFolderName()
    {
        return s_folderName;
    }
};

class Indent
{
public:
    static const N32 IndentChars = 4;

    static N32 s_indent;

    Indent()
    {
        ++s_indent;
    }
    ~Indent()
    {
        --s_indent;
    }
};

class Level
{
public:

    static N32 s_level;

    Level()
    {
        ++s_level;
    }

    ~Level()
    {
        --s_level;
    }

    operator int()
    {
        return s_level;
    }
};

bool readNumber(char*& p, int& value);
bool readNumber2(char*& p, int& value);
bool findChar ( char*& p, int c);
bool readChar ( char*& p, int c);
bool readBlks ( char*& p);
bool readSign (char*& p, int& sign);
bool readIne (char*& p, int& ine);
void errorExit(const char* p);

// eof
