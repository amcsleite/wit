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

N32 g_preparations, g_measurements, g_states, g_outcomes, g_vars;

int main (int argc, char* argv[])
{
    N32 rowsPerPage = 5;
    N32 matricesPerRow = 2;
    N32 useLRS = 0;
    N32 usePanda = 0;

	strcpy(Util::s_folderName, ".");

    Util::readIntParam("db/useLRS", useLRS, 0);
    Util::readIntParam("db/usePanda", usePanda, 0);

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

    g_vars = g_outcomes - 1;

	if (argc != 3)
	{
		printf("\n dimensions: %d\n", g_preparations * g_measurements * g_vars);
		Util::errorMsg("Usage: wCut start and count (indexed at 0)");
		exit(EXIT_SUCCESS);
	}

    N32 start = atol(argv[1]);
    N32 count = atol(argv[2]);

    try
    {
		ExpressionListPtr pVertices = new ExpressionList;
        Util::parsePortaPOI("porta.poi", pVertices);

		pVertices->writePortaSplit("cut1.poi", "cut2.poi", start, count);

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
