#include "Essential.h"
#include "Generator.h"
#include "Util.h"
#include "Exception.h"

//#define _PARSE_LOG

bool parsePorta(ExpressionList* pList, N32 preparations,  N32 measurements)
{
    char str[0x10000];
    FILE* pF = fopen("porta.poi.ieq", "r");

    if (!pF)
    {
        errorExit("must have a porta.poi.ieq in the folder");
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
		
		char* pDim = strstr(p, "DIM = ");

		if (pDim)
		{
			pDim += 6;
			readNumber(pDim, dim);

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
            TermPtr p = new Term();
            p->setState(a[n]);
			
            pIneq->set(n / measurements, n % measurements, p);

			#ifdef _PARSE_LOG
            log(" %dx%d", a[n], n);
			#endif
        }
    }

    fclose(pF);


    return true;
}


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

LogFile g_logFile("filter.log", false);

int main (int argc, char* argv[])
{
	N32 preparations, measurements, dimensions, outcomes;
	N32 rowsPerPage = 5;
	N32 matricesPerRow = 2;
	N32 useLRS = false;
	
    Util::readIntParam("db/useLRS", useLRS);

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

	if(!Util::readIntParam("db/outcomes", outcomes))
    {
		Util::errorMsg("oucomes parameter file not found");
        exit(EXIT_FAILURE);
	}

	Util::readIntParam("rowsPerPage", rowsPerPage);
	Util::readIntParam("matricesPerRow", matricesPerRow);

	strcpy(Util::s_folderName, ".");

    printf("\nFiltering the inequations in porta.poi.ieq with P = %d M = %d D = %d O = %d", preparations, measurements, dimensions, outcomes);

    try
    {

		ExpressionListPtr pList = new ExpressionList;

		if(useLRS)
	    {
			parseLRS(pList, preparations, measurements);
			ilog("\n------------- parsing porta ieq file ----------------------------------------------------------------------------------\n");
		}
		else
		{
			parsePorta(pList, preparations, measurements);
			ilog("\n------------- parsing lrs.hf  -----------------------------------------------------------------------------------------\n");
		}

		ilog("\n------------- original expresssions ----------------------------------------------------------------------------------\n");
        pList->print();

		ilog("\n------------- starting compacting symmetries--------------------------------------------------------------------------\n");

		ExpressionListPtr pRowPermutations = Util::quickPerm(preparations);
		ExpressionListPtr pColPermutations = Util::quickPerm(measurements);

		ilog("row permutations");
		pRowPermutations->print();
		
		ilog("col permutations");
		pColPermutations->print();
        ExpressionListPtr pCompactedPM = pList->compactSymmetricSlow(pRowPermutations, pColPermutations);

		ilog("\n------------- compacted symmetries -----------------------------------------------------------------------------------\n");
        pCompactedPM->print();

//		void printTeX(const char* pName, N32 preparations, N32 measurements, N32 dimensions, N32 outcomes, N32 rowsPerPage, N32 matricesPerRow);

        pCompactedPM->printTeX("witnesses.tex", preparations, measurements, dimensions, outcomes, rowsPerPage, matricesPerRow);
		ilog("");
		ilog("");
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
