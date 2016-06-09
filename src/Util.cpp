#include "Essential.h"
#include "Util.h"
#include "Exception.h"
#include <random>
#include <chrono>

char Util::szCheck[] = "KEEP-this-file-please";

char Util::s_folderName[];

N32 Indent::s_indent;

N32 Level::s_level;


inline bool Util::fexists(const char* p)
{
    FILE* pFile = fopen(p, "r");

    if (pFile)
    {
        fclose(pFile);
        return true;
    }
    else
    {
        return false;
    }
}

inline bool Util::ftouch(const char* p)
{
    FILE* pFile = fopen(p, "w");

    if (pFile)
    {
        fclose(pFile);
        return true;
    }
    else
    {
        return false;
    }
}

void Util::errorMsg(const char* p)
{
    printf("\n\n--- Error found: ---\n\n");
    printf("%s", p);
    printf("\n\n");
	fflush(stdout);
}

bool Util::mkFolder(int preparations, int measurements, int states, int outputs)
{
    int nMax = sizeof(s_folderName);

    char path[1024];
    char* home = getenv ("HOME");

    if (home == NULL)
    {
        printf("couldn't determine HOME directory");
        exit(EXIT_FAILURE);
    }

    snprintf(path, sizeof(path), "%s/wit/scenarios", home);

    int version = 0;

    if(-1 == chdir(path))
    {
        printf("cannot cd into witnesses folder - ~/wit/scenarios must exist");
        exit(EXIT_FAILURE);
    }

    if (preparations != 0)
        sprintf(s_folderName, "%s/%d-%d-%d-%d", path, preparations, measurements, states, outputs);
    else
        sprintf(s_folderName, "%s/out", path);

    int e1 = mkdir(s_folderName, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    if(e1 == -1 && errno != EEXIST)
    {
        printf("error creating witness folder");
        exit(EXIT_FAILURE);
    }

    char szParamsPath[1024];
    sprintf(szParamsPath, "%s/db", s_folderName);

    int e2 = mkdir(szParamsPath, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    if(e2 == -1 && errno != EEXIST)
    {
        printf("error creating database folder");
        exit(EXIT_FAILURE);
    }

    return true;
}

bool isBlk(int c)
{
    return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

bool readNumber(char*& p, int& value)
{
    char szNumber[64];
    int n = 0;

    for (; *p && isBlk(*p); ++p);

    for (; *p && isdigit(*p); ++p)
    {
        szNumber[n] = *p;

        if (n >= sizeof(szNumber))
            return false;

        ++n;
    }

    for (; *p && isBlk(*p); ++p);

    if (n)
    {
        szNumber[n] = 0;
        value = atol(szNumber);
    }

    return true;
}

bool readNumber2(char*& p, int& value)
{
    char szNumber[64];
    szNumber[0] = 0;

    int n = 0;

    for (; *p && (isdigit(*p) || *p == '+' || *p == '-'); ++p)
    {
        szNumber[n] = *p;

        if (n >= sizeof(szNumber))
            return false;

        ++n;
    }

    szNumber[n] = 0;

    value = atol(szNumber);
    return true;
}

bool findChar ( char*& p, int c)
{
    for (; *p && *p != c; p++)
    {
    }

    bool b = *p == c;
    p++;
    return b;
}

bool readChar ( char*& p, int c)
{
    for (; *p && isBlk(*p); p++)
    {
    }

    bool b = *p == c;
    ++p;
    return b;
}

bool readEOrP ( char*& p, int& c)
{
    for (; *p && isBlk(*p); p++)
    {
    }

	c = *p;
	
    bool b = (c == 'E' || c == 'P');
	
    ++p;
    return b;
}

bool readBlks ( char*& p)
{
    for (; *p && isBlk(*p); p++)
    {
    }

    return *p != 0;
}

bool readSign (char*& p, int& sign)
{
    for (; *p && isBlk(*p) ; ++p)
    {
    }

    sign = *p;
    p++;
    return sign == '+' || sign == '-';
}

bool readSign2 (char*& p, int& sign)
{
    for (; *p && isBlk(*p) ; ++p)
    {
    }

    if (*p == '+' || *p == '-')
    {
        sign = *p;
        p++;
    }
    else
        sign = '+';

    return true;
}

bool readIne (char*& p, int& ine)
{
    if (*p == '>' || *p == '<')
    {
        ine = (*p == '>') ? 2 : -2;

        ++p;

        if (*p == '=')
        {
            ine /= 2;
            ++p;
        }

        return true;
    }

    return false;
}

void errorExit(const char* p)
{
    printf("------ %s", p);
    XK(false)
    throw new Exception(p);
}

static void mkPermutationExpression(ExpressionList* pL, U32* a, int n)
{
#ifdef _DEBUGO
    ilog("");

    for(U32 x = 0; x < n; x++)
        log("%d ", a[x]);

#endif

    ExpressionPtr pE = new Expression;

    for(U32 i = 0; i < n; i++)
    {
        pE->refA(i) = a[i];
    }

    pL->pushBack(pE);
}

ExpressionListPtr Util::quickPerm(N32 n)
{
    ExpressionListPtr pL = new ExpressionList;

    U32 a[n];

    XK(n >= 1);

    if(n < 2)
    {
        a[0] = 1;
        mkPermutationExpression(pL, a, 1);
        return pL;
    }

    U32 p[n + 1];

    U32 i, j, tmp; // Upper Index i; Lower Index j

    for(i = 0; i < n; i++)   // initialize arrays; a[N] can be any type
    {
        a[i] = i + 1;
        p[i] = i;
    }

    p[n] = n; // p[N] > 0 controls iteration and the index boundary for i

    mkPermutationExpression(pL, a, n);    // remove comment to display array a[]

    i = 1;                  // setup first swap points to be 1 and 0 respectively (i & j)

    while(i < n)
    {
        p[i]--;             // decrease index "weight" for i by one

        j = i % 2 * p[i];   // IF i is odd then j = p[i] otherwise j = 0
        tmp = a[j];         // swap(a[j], a[i])
        a[j] = a[i];
        a[i] = tmp;

        mkPermutationExpression(pL, a, n);   // remove comment to display target array a[]

        i = 1;              // reset index i to 1 (assumed)

        while (!p[i])       // while (p[i] == 0)
        {
            p[i] = i;        // reset p[i] zero value
            i++;             // set new index value for i (increase by one)
        }
    }

    return pL;
}

ExpressionListPtr Util::genRotations(N32 nb)
{
	ExpressionListPtr pL = new ExpressionList;

    for(U32 r = 0; r < nb; r++)
	{
	   ExpressionPtr pE = new Expression;

	   for(U32 i = 0; i < nb; i++)
	   {
		   pE->refA( (i + r) % nb) = i + 1;
	   }

	   pL->pushBack(pE);
	}
	
	return pL;
}

bool Util::writeIntParam(const char* name, N32 v)
{
    FILE* pF = fopen(name, "w");

    if (!pF)
        return false;

    fprintf(pF, "%d", v);

    fclose(pF);

    return true;
}

bool Util::writeDoubleParam(const char* name, double v)
{
    FILE* pF = fopen(name, "w");

    if (!pF)
        return false;

    fprintf(pF, "%f", v);

    fclose(pF);

    return true;
}

bool Util::readIntParam(const char* name, N32& v, N32 d)
{
    v = d;
	
    FILE* pF = fopen(name, "r");

    if (!pF)
    {
        ilog("variable: %s not found", name);
        writeIntParam(name, d);
        return false;
    }

    char str[0x100];

    str[0] = 0;

    fgets(str, sizeof(str), pF);


    fclose(pF);
    v = atol(str);

    return true;
}

bool Util::readDoubleParam(const char* name, double& v, double d)
{
    FILE* pF = fopen(name, "r");

    if (!pF)
    {
        ilog("variable: %s not found", name);
        v = d;
        writeIntParam(name, d);
        return false;
    }

    char str[0x100];

    str[0] = 0;

    fgets(str, sizeof(str), pF);


    fclose(pF);
    v = atof(str);

    return true;
}

bool Util::parsePortaPOI(const char* pName, ExpressionList* pList)
{
    char str[0x10000];
    FILE* pF = fopen(pName, "r");

    if (!pF)
    {
        errorExit("couldn't open file");
    }

    int dim = 0;

    while(fgets(str, sizeof(str), pF))
    {
#ifdef _PARSE_LOG
        ilog("");
#endif
        int nLen = strlen(str);
        char* p = str;

        XK(str[nLen - 1] == '\n');
        str[nLen - 1] = 0;

        char* pDim = strstr(p, "DIM");

        if (pDim)
        {
            char* pEq = strstr(p, "=");

            if (!pEq)
                errorExit(" DIM ? missing =");

            pEq++;
            p = pEq  + 1;

            readNumber(pEq, dim);

#ifdef _PARSE_LOG
            log("Dimension is %d:", dim);
#endif
            continue;
        }

        if (!readChar(p, '('))
            continue;

        int nVertex;

        if (!readNumber(p, nVertex))
            errorExit("couldn't find a vertex number");

        if (!readChar(p, ')'))
            errorExit("couldn't find a )");

#ifdef _PARSE_LOG
        log("%d:", nVertex);
#endif

        ExpressionPtr pMatrix = new Expression();
        pList->pushBack(pMatrix);

        int varIndexMax = -1;

        for (N32 n = 0; n < dim; ++n)
        {
            if(!readBlks(p))
                errorExit("incomplet");

            int value = 0;

            readNumber2(p, value);

            pMatrix->set(n, value);
        }

    }

    fclose(pF);


    return true;
}

// static
bool Util::parsePortaIEQ(const char* pName, ExpressionList* pList, N32 preparations, N32 measurements, N32 vars)
{
    char str[0x10000];
    FILE* pF = fopen(pName, "r");

    if (!pF)
    {
        errorExit("couldn't read ieq file");
    }

    int dim = 0;

    while(fgets(str, sizeof(str), pF))
    {
#ifdef _PARSE_LOG
        ilog("");
#endif
        int nLen = strlen(str);
        char* p = str;

        XK(str[nLen - 1] == '\n');
        str[nLen - 1] = 0;

        char* pDim = strstr(p, "DIM");

        if (pDim)
        {
            char* pEq = strstr(p, "=");

            if (!pEq)
                errorExit(" DIM ? missing =");

            pEq += 1;
            readNumber(pEq, dim);

#ifdef _PARSE_LOG
            log("Dimension is %d:", dim);
#endif
            continue;
        }

        if (!readChar(p, '('))
            continue;

        int nVertex;

        if (!readNumber(p, nVertex))
            errorExit("couldn't find a vertex number");

        if (!readChar(p, ')'))
            continue;

#ifdef _PARSE_LOG
        log("%d:", nVertex);
#endif

        N32 a[256];

        for (int n = 0; n < 256; ++n)
            a[n] = 0;

        ExpressionPtr pIneq = new Expression();

		pIneq->setID(nVertex);
		
        pList->pushBack(pIneq);

        int varIndexMax = -1;

        while(true)
        {
            int sign;
            int ine;
            int rhs;

            if(!readBlks(p))
                errorExit("incomplet");

            if (readIne(p, ine))
            {
                if (!readNumber(p, rhs))
                    errorExit("couldn't find lrs");

#ifdef _PARSE_LOG
                log(" rhs: %d .... %s", rhs, str);
#endif

                pIneq->setRHS(rhs);
                pIneq->setIne(ine);
                pIneq->setPortaExp(str);
                break;
            }

            if (!readSign(p, sign))
                errorExit("couldn't find a sign (+/-)");

            int factor = 1;

            readNumber(p, factor);

            if (!readChar(p, 'x'))
                errorExit("couldn't find a x");

            int varIndex;

            if (!readNumber(p, varIndex))
                errorExit("couldn't find a >");

            if (varIndex > varIndexMax)
                varIndexMax = varIndex;

            N32 termState = (sign == '-') ? -factor : factor;
            //fprintf(pLog, " %dx%d", termState, varIndex);

            //TermPtr p = new Term();
            //p->setState(termState);

            //pIneq->pushBack(p);
            a[varIndex - 1] = termState;
        }

        for (int n = 0; n < dim; ++n)
        {
            pIneq->set(n / (measurements * vars),  (n % (measurements * vars)) / vars, (n % vars), a[n]);

#ifdef _PARSE_LOG
            log(" %dx%d", a[n], n);
#endif
        }
    }

    fclose(pF);


    return true;
}

// static
bool Util::parseLRS(ExpressionList* pList, N32 preparations, N32 measurements, N32 vars)
{
    static char szRepresentation[] = "H-representation";

    bool bFoundRepresentation = false;
    bool bBegin = false;

    char str[0x10000];
    FILE* pF = fopen("lrs.hf", "r");

    if (!pF)
    {
        errorExit("This program uses always uses lrs.hf as input which I couldn't find");
    }

    while(fgets(str, sizeof(str), pF))
    {
#ifdef _PARSE_LOG
        ilog("");
#endif
        int nLen = strlen(str);
        char* p = str;

        if(str[nLen - 1] == '\n')
            str[nLen - 1] = 0;

        if (*p == '#' || *p == '*')
            continue;

        if (!bFoundRepresentation)
        {
            char* pPresentation = strstr(p, szRepresentation);

            if (pPresentation)
                bFoundRepresentation = true;

            continue;
        }

        if (!bBegin)
        {
            char* pBegin = strstr(p, "begin");

            if (pBegin)
                bBegin = true;

            continue;
        }

        char* pEnd = strstr(p, "end");

        if (pEnd)
            break;

#ifdef _PARSE_LOG
        ilog("");
#endif

        N32 a[256];

        for (int n = 0; n < 256; ++n)
            a[n] = 0;

        ExpressionPtr pIneq = new Expression();
        pList->pushBack(pIneq);

        int dPlusOne = 0;

        while(true)
        {
            int ine;
            int rhs;

            N32 v = 0;

            if(!readBlks(p))
                break;

            if (*p == 0)
                break;

            if (!readNumber2(p, v))
                errorExit("error");

            a[dPlusOne] = v;

            ++dPlusOne;
        }

        // in lrs  the const is in LHS we need to change it to RHS

        pIneq->setRHS(a[0]);
        pIneq->setIne(-1);

        for (int i = 1; i < dPlusOne; ++i)
        {
            int n = i - 1;

            //          pIneq->set( n / measurements, n % measurements, p);
            pIneq->set(n / (measurements * vars),  (n % (measurements * vars)) / vars, (n % vars), -a[i]);


#ifdef _PARSE_LOG
            log(" %dx%d", -a[n], n);
#endif
        }
    }

    fclose(pF);


    return true;
}

// static
bool Util::parsePandaIEQ(const char* pName, ExpressionList* pList, N32 preparations, N32 measurements, N32 vars)
{
    char str[0x10000];
    FILE* pF = fopen(pName, "r");

    if (!pF)
    {
        errorExit("couldn't read ieq file");
    }

    while(fgets(str, sizeof(str), pF))
    {
#ifdef _PARSE_LOG
        ilog("");
#endif
        int nLen = strlen(str);
        char* p = str;

        XK(str[nLen - 1] == '\n');
        str[nLen - 1] = 0;

        char* pReduced = strstr(p, "Reduced");

        if (pReduced)
        {
            continue;
        }

        N32 a[256];

        for (int n = 0; n < 256; ++n)
            a[n] = 0;

        if(!readBlks(p))
            break;

        ExpressionPtr pIneq = new Expression();
        pList->pushBack(pIneq);

        int varIndexMax = -1;

        while(true)
        {
            int sign;
            int ine;
            int rhs;

            if(!readBlks(p))
                errorExit("incomplet");

            if (readIne(p, ine))
            {
                if (!readNumber(p, rhs))
                    errorExit("couldn't find lrs");

#ifdef _PARSE_LOG
                log(" rhs: %d .... %s", rhs, str);
#endif

                pIneq->setRHS(rhs);
                pIneq->setIne(ine);
                pIneq->setPortaExp(str);
                break;
            }

            if (!readSign2(p, sign))
                errorExit("couldn't find a sign (+/-)");

            int factor = 1;

            readNumber(p, factor);

			int c;
			
            if (!readEOrP(p, c))
                errorExit("couldn't find a E");

            int nn;

            if (!readNumber(p, nn))
                errorExit("couldn't find a number");

			N32 prep, mes, var, varIndex;
			
			if (c == 'E')
			{
				prep = nn / 10;
				mes = nn % 10;
				var = 0;
				
				varIndex = (prep - 1) * measurements + mes - 1;
			}
			else
			{
				prep = nn / 100;
				N32 mesAndVar = nn % 100;

				mes = mesAndVar / 10;
				var = mesAndVar % 10;

				varIndex = (prep - 1) * measurements*vars + (mes - 1)*vars + (var - 1);
			}

            if (varIndex > varIndexMax)
                varIndexMax = varIndex;

            N32 termState = (sign == '-') ? -factor : factor;
            //fprintf(pLog, " %dx%d", termState, varIndex);

            //TermPtr p = new Term();
            //p->setState(termState);

            //pIneq->pushBack(p);
            a[varIndex] = termState;
        }

        for (int n = 0; n <= varIndexMax; ++n)
        {
            pIneq->set(n / (measurements * vars),  (n % (measurements * vars)) / vars, (n % vars), a[n]);

#ifdef _PARSE_LOG
            log(" %dx%d", a[n], n + 1);
#endif
        }
    }

    fclose(pF);


    return true;
}

std::mt19937_64 _mtRrandom(std::chrono::system_clock::now().time_since_epoch().count());
	
double Util::rDouble()
{
	// mersenne twister
	return (double) (_mtRrandom() - UINT64_MAX / 2)/ (double) UINT64_MAX;
}
	

// eof
