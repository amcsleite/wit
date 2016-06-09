#pragma once

#define N32 int
#define U32 unsigned int

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <signal.h>

#define BYTE unsigned char
#define WORD unsigned short

#define U8 unsigned char
#define N8 char

#define N16 short
#define U16 unsigned short

#define N32 int
#define U32 unsigned int

#define N64 long long
#define U64 unsigned long long

#define NADA XK(true)


const U64 InvalidPosSpec = 0LL;

const U32 InvalidStringBufferID = 0;

const U32 INFINITE = 0xFFFFFFFF;

#define Min(X,Y) ( ((X) < (Y)) ?  (X) : (Y) )
#define Max(X,Y) ( ((X) > (Y)) ?  (X) : (Y) )

#define N32_MIN ( (N32) -0x7FFFFFFF)
#define N32_MAX ( (N32)  0x7FFFFFFF)

#define N64_MIN ( (N64) -0X7FFFFFFFFFFFFFFFLL)
#define N64_MAX ( (N64)  0X7FFFFFFFFFFFFFFFLL)

// below must have something after the asm or it won't work !!

void initStackTrace();

#ifdef _DEBUG

#define XK(bExpression)\
 if (!(bExpression)) \
 {\
     printf("\n - XK failed: %s -\n", #bExpression);\
     printf("\n"); \
	raise(SIGTRAP); \
 }

#else

#define XK(e)

#endif

#define EQ(a, b) ( fabs( (a) - (b) ) < 1.e-7)
#define EQ3(a, b) ( fabs( (a) - (b) ) < 1.e-3)
#define EQ4(a, b) ( fabs( (a) - (b) ) < 1.e-4)

#ifdef _DEBUG
//#pragma message(" _DEBUG  defined ")
#endif

#ifdef _DEBUG
#define DBG(expr)  \
    do             \
    {              \
       (expr);     \
    }              \
    while (0)
#else
#define DBG(e)
#endif

#define _read read
#define _write write
#define _open open

#define _O_CREAT O_CREAT
#define _S_IREAD  S_IREAD
#define _S_IWRITE S_IWRITE


#include "Counted.h"
#include "SmartPtr.h"
#include "XList3.h"

inline U32 pow2(U32 n)
{
    return 1 << n;
}

inline U64 pown(U64 u, U64 e)
{
	U64 total = 1;
	
	for (U64 n = 0; n < e; ++n)
		total *= u;
	
    return total;
}

inline bool bit(U32 u, N32 n)
{
    u >>= n;
    return  (u & 1) != 0;
}

#include "Util.h"
//eof
