#include <string.h>
#include "Essential.h"
#include "Exception.h"
#include "Lists.h"
#include "ExpressionList.h"
#include "Expression.h"
#include "Term.h"
#include "Util.h"

//#define _PARSE_LOG

static bool parseLRS(ExpressionList* pList, N32 preparations,  N32 measurements)
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
            TermPtr p = new Term();
            p->setState(a[i]);

			int n = i - 1;
            pIneq->set( n / measurements, n % measurements, p);

			#ifdef _PARSE_LOG
            log(" %dx%d", -a[n], n);
			#endif
        }
    }

    fclose(pF);
	

    return true;
}

#undef ilog
#undef log

LogFile g_logFile("filterLRS.log", false);
	
int main (int argc, char* argv[])
{
	N32 preparations, measurements, dimensions;

	strcpy(Util::s_folderName, ".");

    if (!Util::readIntParam("db/preparations", preparations))
    {
		Util::errorMsg("preparations parameter file not found");
        exit(EXIT_FAILURE);
	}
		
    if (!Util::readIntParam("db/measurements", measurements))
    {
		Util::errorMsg(" measurements file not found");
        exit(EXIT_FAILURE);
	}

	if(!Util::readIntParam("db/dimensions", dimensions))
    {
		Util::errorMsg("dimensions parameter file not found");
        exit(EXIT_FAILURE);
	}

    printf("\nFiltering the inequations with P = %d M = %d", preparations, measurements);

    try
    {
        ExpressionListPtr pLRS = new ExpressionList;

        ilog("\n------------- parsing lrs inequalities file---------------------------------------------------------------------------\n");
        parseLRS(pLRS, preparations, measurements);

        ilog("\n------------- original expresssions ----------------------------------------------------------------------------------\n");
        pLRS->print();

        ilog("\n------------- starting compacting symmetries--------------------------------------------------------------------------\n");

        ExpressionListPtr pRowPermutations = Util::quickPerm(preparations);
        ExpressionListPtr pColPermutations = Util::quickPerm(measurements);

		ilog("row permutations");
		pRowPermutations->print();
		
		ilog("col permutations");
		pColPermutations->print();

        ExpressionListPtr pCompactedPM = pLRS->compactSymmetricSlow(pRowPermutations, pColPermutations);

        ilog("\n------------- compacted symmetries -----------------------------------------------------------------------------------\n");
        pCompactedPM->print();

    }
    catch (Exception e)
    {
        XK(false);
    }

    printf("\n-----------------------------------------------------------------------------");
    printf("\n Results and intermediate files are in the folder: %s", Util::getFolderName());
    printf("\n-----------------------------------------------------------------------------");
    printf("\n\n");

    exit(EXIT_SUCCESS);
}

// eof
