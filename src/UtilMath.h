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
#include "Essential.h"
#include "M1.h"

extern std::complex<double> ii;

class UtilMath
{
	static const N32 MaxMatrices = 30;
	
	static M1 s_GellM[];
	static N32 s_nMatrices;
	
public:
	static Matrix sigma(N32 n);
	static Matrix Identity2();

	static bool isNull(const Matrix& m);
	static bool notNegative(const Matrix& m);

	static Matrix identity(N32 d)
	{
		Matrix m;
		return m.eye(d, d);
	}
	static void initGellM(N32 d);
	static void testGellM();
	static N32 nGellM()
	{
		return s_nMatrices;
	}

	static Matrix gellM(N32 n)
	{
		XK(n < s_nMatrices);
		return s_GellM[n];
	}
	
	static void printGellM();
};

// eof
