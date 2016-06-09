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

LogStream g_logStream("wSelect.log");

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
    
    printf("\nFiltering the inequations in porta.poi.ieq with P = %d M = %d D = %d O = %d", g_preparations, g_measurements, g_states, g_outcomes);

    try
    {
        ExpressionListPtr pWithTrivials = new ExpressionList;
		
		ilog("\n------------- parsing filtered iew file -------------------------------------------------------------------------------\n");
		Util::parsePortaIEQ("filtered.ieq", pWithTrivials, g_preparations, g_measurements, g_vars);
		
        ExpressionListPtr p = pWithTrivials->copyNoTrivials();

		p->resetMarks();

		/// HIGHER PRIORITY FIRST
		//zeros to the bottom
		// most symmetric to the top
		// higher coefficients to the top
		EvaluatorPtr pSort3 = new OrderEvaluator();
        p = p->sort(pSort3);

        EvaluatorPtr pSort2 = new SEvaluator();
        p = p->sort(pSort2);

        EvaluatorPtr pSort1 = new ZerosEvaluator();
		p = p->sort(pSort1);

		// low RHS to the top
        EvaluatorPtr pSort4 = new RHSEvaluator(-1);
        p = p->sort(pSort4);

        ilog("\n------------- compacted symmetries inverses ordered by marks ---------------------------------------------------------\n");
        p->print();

        p->writeIEQPorta("selected.ieq", g_preparations * g_measurements * (g_outcomes - 1), "by wFilter: compacted considering the inverses and sorted");

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
