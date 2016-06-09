#include <stdlib.h>
#include "Essential.h"
#include "Util.h"
#include "Exception.h"
#include "Evaluator.h"
#include "SEvaluator.h"
#include "ZerosEvaluator.h"
#include "OrderEvaluator.h"
#include "RHSEvaluator.h"

//#define _PARSE_LOG

#undef ilog
#undef log

LogStream g_logStream("wFilter.log");

N32 g_preparations, g_measurements, g_states, g_outcomes, g_vars, g_useEV;

int main (int argc, char* argv[])
{

    N32 rowsPerPage = 5;
    N32 matricesPerRow = 2;
    N32 useLRS = 0;
    N32 usePanda = 1;
    N32 usePorta = 0;

    strcpy(Util::s_folderName, ".");

    Util::readIntParam("db/useLRS", useLRS, 0);
    Util::readIntParam("db/usePanda", usePanda, 1);
    Util::readIntParam("db/usePorta", usePorta, 0);

    if (!Util::readIntParam("db/preparations", g_preparations, 3))
    {
        Util::errorMsg("preparations parameter file not found");
        exit(EXIT_FAILURE);
    }

    if (!Util::readIntParam("db/measurements", g_measurements, 3))
    {
        Util::errorMsg(" measurements file not found");
        exit(EXIT_FAILURE);
    }

    if(!Util::readIntParam("db/states", g_states, 2))
    {
        Util::errorMsg("db/dimensions parameter file not found");
        exit(EXIT_FAILURE);
    }

    if(!Util::readIntParam("db/outcomes", g_outcomes, 2))
    {
        Util::errorMsg("outcomes parameter file not found");
        exit(EXIT_FAILURE);
    }

    Util::readIntParam("db/useEV", g_useEV, 0);

    g_vars = g_outcomes - 1;


    try
    {
        ExpressionListPtr pList = new ExpressionList;

        if(useLRS)
        {
            ilog("\n------------- parsing lrs.hf  -----------------------------------------------------------------------------------------\n");
            Util::parseLRS(pList, g_preparations, g_measurements, g_vars);
        }
        else if(usePanda)
        {
            ilog("\n------------- parsing panda.ieq  -----------------------------------------------------------------------------------------\n");
            Util::parsePandaIEQ("panda.ieq", pList, g_preparations, g_measurements, g_vars);
        }
        else if (usePorta)
        {
            ilog("\n------------- parsing porta ieq file ----------------------------------------------------------------------------------\n");
            Util::parsePortaIEQ("porta.poi.ieq", pList, g_preparations, g_measurements, g_vars);
        }

        ilog("\n------------- original expressions ----------------------------------------------------------------------------------\n");
        pList->print();
        pList->writeIEQPorta("original.ieq", g_preparations * g_measurements * (g_outcomes - 1), "by wFilter: original");

        ilog("\n------------- starting compacting symmetries--------------------------------------------------------------------------\n");

        printf("\nloaded ieq P = %d M = %d D = %d O = %d", g_preparations, g_measurements, g_states, g_outcomes);
        fflush(stdout);

        printf("\ncomputing permutations...");
        fflush(stdout);
        ExpressionListPtr pRowPermutations = Util::quickPerm(g_preparations);
        ExpressionListPtr pColPermutations = Util::quickPerm(g_measurements);
        ExpressionListPtr pVarPermutations = Util::quickPerm(g_outcomes);

        ilog("row permutations");
        pRowPermutations->print();

        ilog("col permutations");
        pColPermutations->print();

        ilog("outcome permutations");
        pVarPermutations->print();

        printf("\nfiltering ...");
        fflush(stdout);

        N32 sortPerm = 1;
        Util::readIntParam("db/sortPerm", sortPerm, 1);

        ExpressionListPtr p = pList->compactSymmetricSlow(pRowPermutations, pColPermutations, pVarPermutations, g_useEV);

        if (sortPerm)
        {
            printf("\nbest permutations for each expression...");
            fflush(stdout);
            p = p->selectTheBestPermutationOfEachExpression(pRowPermutations, pColPermutations, pVarPermutations, new SEvaluator, g_useEV);
            p = p->selectTheBestPermutationOfEachExpression(pRowPermutations, pColPermutations, pVarPermutations, new ZerosEvaluator(), g_useEV);
            p = p->selectTheBestPermutationOfEachExpression(pRowPermutations, pColPermutations, pVarPermutations, new OrderEvaluator(), g_useEV);
        }
        else
        {
            printf("\nsortPerm == 0 so I skipped best permutations for each expression.");
        }

        p->writeIEQPorta("filtered.ieq", g_preparations * g_measurements * (g_outcomes - 1), "by wFilter: compacted considering the inverses and sorted");
        printf("\nDone");
        fflush(stdout);
        ilog("");
    }
    catch (Exception e)
    {
        exit(EXIT_SUCCESS);
    }

    printf("\n-----------------------------------------------------------------------------");
    printf("\n Results and intermediate files are in the folder: %s", Util::getFolderName());
    printf("\n-----------------------------------------------------------------------------");
    printf("\n\n");

    exit(EXIT_SUCCESS);
}

// eof

