#include "Essential.h"
#include "DIGenerator.h"
#include "Expression.h"
#include "ExpressionList.h"
#include "Util.h"
#include "Exception.h"
#include "Symmetries.h"

extern "C"
{
    int mainLib( int argc, const char* argv[] );
}

int runTests (int argc, char* argv[]);

void filter(N32 nP, N32 nM, ExpressionListPtr& pPorta, ExpressionListPtr& pSorted, ExpressionListPtr& pCompacted, ExpressionListPtr& pCompactedInverses);

void printUsageMsg()
{
    printf("\n--------------------------------------------------------\n");
    printf("Witness generator usage:\n");
    printf("  wit P M b <enter> \n");
    printf("  P number of preparations\n");
    printf("  M number of measurements\n");
    printf("  d dimension of the classical channel same as number of classic states \n");
    printf("  b number of outputs \n");
    printf("\n--------------------------------------------------------\n");
}

LogStream g_logStream("wPoly.log");

int main (int argc, char* argv[])
{
    try
    {
        if (argc != 5)
        {
            Util::errorMsg("This program takes exactly 4 arguments.");
            printUsageMsg();
            exit(EXIT_FAILURE);
        }

        int preparations = atol(argv[1]);
        int measurements = atol(argv[2]);
        int channelDimensions = atol(argv[3]);
        int outcomes = atol(argv[4]);

        // args sanity check

        if (preparations <= 0 || preparations > 100)
        {
            Util::errorMsg("Invalid number of preparations");
            exit(EXIT_FAILURE);
        }

        if (measurements <= 0 || measurements > 100)
        {
            Util::errorMsg("Invalid number of measurements");
            exit(EXIT_FAILURE);
        }

        if (channelDimensions < 2 || channelDimensions > 100)
        {
            Util::errorMsg("Invalid number of dimensions. (dimension must be >= 2) ");
            exit(EXIT_FAILURE);
        }

        if (outcomes < 2 || outcomes > 100)
        {
            Util::errorMsg("Invalid number of outcomes. (must be >= 2) ");
            exit(EXIT_FAILURE);
        }

        if(!Util::mkFolder(preparations, measurements, channelDimensions, outcomes))
        {
            Util::errorMsg("couldn't create device folder");
            exit(EXIT_FAILURE);
        }

        if(chdir(Util::getFolderName()) != 0)
        {
            Util::errorMsg("couldn't cd to device folder");
            exit(EXIT_FAILURE);
        }

        Util::writeIntParam("db/preparations", preparations);
        Util::writeIntParam("db/measurements", measurements);
        Util::writeIntParam("db/states", channelDimensions);
        Util::writeIntParam("db/outcomes", outcomes);

        unlink("complementary.poi");
        unlink("log.log");
        unlink("porta.log");
        unlink("porta.poi");
        unlink("lrs.vf");
        unlink("porta.poi.ieq");
        unlink("porta.stdout");
        unlink("witnesses.tex");
        unlink("witnesses.pdf");

        time_t t = time(NULL);
        ilog("This run is starting at: %s ", ctime(&t));

#ifdef _DEBUG
        ilog("_DEBUG is defined");
#else
        ilog("_DEBUG is not defined");
#endif
        ilog("");

        ilog("- Models classic devices with:");
        ilog("%d preparations, %d measurements %d dimensions %d outputs -", preparations, measurements, channelDimensions, outcomes);

        ilog("");

        ExpressionListPtr pStrategyList = new ExpressionList();

        DIGeneratorPtr pG = new DIGenerator(preparations, measurements, channelDimensions, outcomes);

        printf("\n-----------------------------------------------------------------------------");
        printf("\n Results and intermediate files are in the folder: %s", Util::getFolderName());
        printf("\n-----------------------------------------------------------------------------");

        printf("\nPolytope prepared for porta with d = %lld  (c.f. porta.poi)", pG->getDimensionsTotal());

        pG->generate(pStrategyList);

        if (outcomes == 2)
        {
            pStrategyList->writePorta("portaEV.poi", true, pG->getDimensionsTotal());
            pStrategyList->writeLRS("lrsEV.vf", true, pG->getDimensionsTotal(), preparations, measurements, channelDimensions, outcomes);
        }

        pStrategyList->writePorta("portaP.poi", false, pG->getDimensionsTotal());
        pStrategyList->writeLRS("lrsP.vf", false, pG->getDimensionsTotal(), preparations, measurements, channelDimensions, outcomes);

        pStrategyList->print();

        // panda
        ExpressionListPtr pRowPermutations = Util::quickPerm(preparations);
        ExpressionListPtr pColPermutations = Util::quickPerm(measurements);
        ExpressionListPtr pVarPermutations = Util::quickPerm(outcomes - 1);

        SymmetriesPtr pS = new Symmetries;
        IntListPtr pP = pS->generate(pRowPermutations, pColPermutations, pVarPermutations);

        ilog(" - dumping all symmetries -");

        pP->print();

        if (outcomes == 2)
            pStrategyList->writePanda("pandaEV.poi", true, pP);

        pStrategyList->writePanda("pandaP.poi", false, pP);

        printf("\n\n");

        exit(EXIT_SUCCESS);
    }
    catch (Exception e)
    {
        printf("\n\nException\n\n");
        exit(EXIT_FAILURE);
    }
}

// eof

