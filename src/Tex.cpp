#include <stdlib.h>
#include "Essential.h"
#include "Util.h"
#include "Exception.h"
#include "Evaluator.h"
#include "SEvaluator.h"
#include "ZerosEvaluator.h"
#include "OrderEvaluator.h"
#include "RHSEvaluator.h"
#include "WriteVar.h"

#undef ilog
#undef log

LogStream g_logStream("wTex.log");

N32 g_preparations, g_measurements, g_states, g_outcomes, g_vars;

static char szDocHeader[] = "\
\n\\documentclass[fleqn]{article} \
\n\\input{\\string~/wTools/tex/al.tex} \
\n\\usepackage{array} \
\n\\newcolumntype{C}{>{\\centering\\arraybackslash}p{5mm}} \
\n\\newcolumntype{D}{>{\\centering\\arraybackslash}p{5mm}} \
\n\\begin{document} \
\n\\large \
";

static char szDocFooter[] = "\\end{document}";

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

    try
    {
        ExpressionListPtr pList = new ExpressionList;

        ilog("\n------------- parsing selected.ieq file------------------------------------------------------------------------------------\n");
        Util::parsePortaIEQ("selected.ieq", pList, g_preparations, g_measurements, g_vars);

        ilog("\n------------- selected expressions from file -------------------------------------------------------------------------\n");
        pList->print();

        // page
        pList->prepareAndPrintTeX("page.tex", g_preparations, g_measurements, g_states, g_outcomes, false);

        // full doc
        pList->prepareAndPrintTeX("doc.tex", g_preparations, g_measurements, g_states, g_outcomes, true);

        system("/usr/local/texlive/2014/bin/x86_64-linux/pdflatex -interaction=nonstopmode \"\\\\input\" doc.tex");

        char sz[128];
        sprintf(sz, "cp page.tex ../../wPages/%d-%d-%d-%d.tex", g_preparations, g_measurements, g_states, g_outcomes);
        system(sz);

        sprintf(sz, "cp doc.pdf ../../wPDF/%d-%d-%d-%d.pdf", g_preparations, g_measurements, g_states, g_outcomes);
        system(sz);

        ilog("");
        ilog("");

        /// separated
        char* pArg = pArg = (argc == 2) ? argv[1] : NULL;

        for (Expression* pExp = pList->head(); pExp;  pExp = pExp->next())
        {
            char szPath[1024];
            pExp->pasteExpPath(0, szPath);

            WriteVar w(szPath, "wit.tex");

            fprintf(w.getFile(), szDocHeader);
            pExp->printTeX(w.getFile(), pList);
            fprintf(w.getFile(), szDocFooter);
            w.close();
            char cmd[1024];
            sprintf(cmd, "/usr/local/texlive/2014/bin/x86_64-linux/pdflatex  -output-directory=%s -interaction=nonstopmode \"\\\\input\" %s", w.getFolderPath(), w.getPathAndName());
            system(cmd);
        }
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

