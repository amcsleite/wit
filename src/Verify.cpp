#include "Essential.h"
#include "Util.h"
#include "Exception.h"
#include <stdlib.h>

//#define _PARSE_LOG

#undef ilog
#undef log

LogStream g_logStream("wVerify.log");

int main (int argc, char* argv[])
{
    N32 preparations, measurements, states, outcomes;
    N32 rowsPerPage = 5;
    N32 matricesPerRow = 2;
    N32 useLRS = 0;

    Util::readIntParam("db/useLRS", useLRS, 0);

    if (!Util::readIntParam("db/preparations", preparations, 3))
    {
        Util::errorMsg("preparations parameter file not found");
        exit(EXIT_FAILURE);
    }

    if (!Util::readIntParam("db/measurements", measurements, 3))
    {
        Util::errorMsg("measurements file not found");
        exit(EXIT_FAILURE);
    }

    if(!Util::readIntParam("db/states", states, 2))
    {
        Util::errorMsg("dimensions parameter file not found");
        exit(EXIT_FAILURE);
    }

    if(!Util::readIntParam("db/outcomes", outcomes, 2))
    {
        Util::errorMsg("dimensions parameter file not found");
        exit(EXIT_FAILURE);
    }

    N32 vars = outcomes - 1;

    strcpy(Util::s_folderName, ".");

    printf("\nReading the inequations from selected.ieq,  P = %d M = %d D = %d O = %d", preparations, measurements, states, outcomes);

    try
    {
        ExpressionListPtr pVertices = new ExpressionList;
        Util::parsePortaPOI("porta.poi", pVertices);

        ilog("vertices read");
        pVertices->print();
        ilog("------");

        ExpressionListPtr pList = new ExpressionList;

        ilog("\n------------- parsing porta ieq file ----------------------------------------------------------------------------------\n");
        Util::parsePortaIEQ("selected.ieq", pList, preparations, measurements, vars);

        printf("\nReading the vertices,  P = %d M = %d D = %d O = %d", preparations, measurements, states, outcomes);

        ilog("\n------------- original expresssions ----------------------------------------------------------------------------------\n");
        pList->print();

        ilog("\n------------- starting compacting symmetries--------------------------------------------------------------------------\n");

        ExpressionListPtr pRowPermutations = Util::quickPerm(preparations);
        ExpressionListPtr pColPermutations = Util::quickPerm(measurements);
        ExpressionListPtr pVarPermutations = Util::quickPerm(vars);

        ilog("row permutations");
        pRowPermutations->print();

        ilog("col permutations");
        pColPermutations->print();

        ilog("outcome permutations");
        pVarPermutations->print();

        ilog("\n------------- starting verification of the witnesses -----------------------------------------------------------------\n");
        printf("\n");
        pList->verify(pVertices);
        ilog("\n----------------------------------------------------------------------------------------------------------------------\n");
        ilog("");
        printf("\n\n");
    }
    catch (Exception e)
    {
        exit(EXIT_SUCCESS);
    }

    exit(EXIT_SUCCESS);
}

// eof
