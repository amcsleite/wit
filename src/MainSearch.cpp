#include <iostream>
#include <armadillo>
#include "Util.h"
#include "Search.h"
#include "UtilMath.h"

#define Mat mat

LogStream g_logStream("search.log", false);

void compute(ExpressionList* pL, bool fProjective, bool fComplex, Expression* pExp, N32 maxIterations)
{
    SearchPtr pR = new Search(pExp, fProjective, fComplex, pL->getPreparations(), pL->getMeasurements(), pL->getStates(), pL->getOutcomes());

    N32 repetitions = 5;

    double robustness = 0.;

    bool valid = false;
    double bestQuantumValue = pR->repeat(repetitions, maxIterations, valid);

    const char* pStatus, *pValue, *pRobustness, *pMat;

    if (fComplex)
    {
        if (fProjective)
        {
            pStatus = "ProjectiveStatus";
            pValue = "ProjectiveValue";
            pRobustness = "ProjectiveRobustness";
            pMat = "matProjective.m";
        }
        else
        {
            pStatus = "POVMStatus";
            pValue = "POVMValue";
            pRobustness = "POVMRobustness";
            pMat = "matPOVM.m";
        }
    }
    else
    {
        if (fProjective)
        {
            pStatus = "RealProjectiveStatus";
            pValue = "RealProjectiveValue";
            pRobustness = "RealProjectiveRobustness";
            pMat = "RealMatProjective.m";
        }
        else
        {
            pStatus = "RealPOVMStatus";
            pValue = "RealPOVMValue";
            pRobustness = "RealPOVMRobustness";
            pMat = "RealMatPOVM.m";
        }
    }

    if (valid)
    {
        pExp->writeInt(pStatus, 1);
        pExp->writeDouble(pValue, bestQuantumValue);

		double N = pExp->getRandomNoise();
		
		if (pL->getOutcomes() == 2)
			robustness = pExp->getRHS()/ bestQuantumValue;
		else
			robustness = (pExp->getRHS() - N) / (bestQuantumValue  - N);

			
        pExp->writeDouble(pRobustness, robustness);
        pR->saveMat(pMat, pExp);
    }
    else
    {
        pExp->writeInt(pStatus, -1);
        pExp->writeDouble(pValue, 0.);
        pExp->writeDouble(pRobustness, 0);
    }
}

int main(int argc, char** argv)
{
    initStackTrace();

    bool doProjective = false;
    bool doPOVM = false;
    bool doComplex = false;
    bool doReal = false;

    strcpy(Util::s_folderName, ".");

    for (N32 n = 1; n < argc; ++n)
    {
        if(argv[n][0] == 'a')
        {
            doProjective = true;
            doPOVM = true;
            doComplex = true;
            doReal = true;
        }

        if(argv[n][0] == 'p')
        {
            doProjective = true;
        }

        if(argv[n][0] == 'x')
        {
            doComplex = true;
        }

        if(argv[n][0] == 'r')
        {
            doReal = true;
        }

        if(argv[n][0] == 'v')
        {
            doPOVM = true;
        }

        if (argv[n][0] == 't')
        {
            Search::tests();
            exit(0);
        }

        if (argv[n][0] == 'g')
        {
            UtilMath::testGellM();
            exit(0);
        }

        if (argv[n][0] == 'c')
        {
            g_logStream.setUseCout();
        }

        if (argv[n][0] == 'z')
        {
            if(chdir("/home/al/wTools/witnesses/4-2-2-3") != 0)
                exit(EXIT_FAILURE);
        }

        if (argv[n][0] == 'w')
        {
            // do Gallego witness
            if(chdir("/home/al/wTools/witnesses/3-2-2-2") != 0)
                exit(EXIT_FAILURE);
        }
    }

    N32 preparations, measurements, states, outcomes;

    if (!Util::readIntParam("db/preparations", preparations, 3))
    {
        Util::errorMsg("preparations parameter file not found");
        exit(EXIT_FAILURE);
    }

    if (!Util::readIntParam("db/measurements", measurements, 2))
    {
        Util::errorMsg(" measurements file not found");
        exit(EXIT_FAILURE);
    }

    if(!Util::readIntParam("db/states", states, 2))
    {
        Util::errorMsg("states parameter file not found");
        exit(EXIT_FAILURE);
    }

    if(!Util::readIntParam("db/outcomes", outcomes, 2))
    {
        Util::errorMsg("outcomes parameter file not found");
        exit(EXIT_FAILURE);
    }

    N32 maxIterations = 1000;

    if(!Util::readIntParam("db/maxIterations", maxIterations, 1000))
    {
        Util::errorMsg("maximum iterations parameter not set so I will be using the default: 2000");
    }

    N32 vars = outcomes - 1;

    ExpressionListPtr pList = new ExpressionList;
    Util::parsePortaIEQ("selected.ieq", pList, preparations, measurements, vars);

    pList->readListVars();

    for (Expression* p = pList->head(); p; p = p->next())
    {
        p->readExpressionVars();

        if (doComplex)
        {
            if (doProjective)
                compute(pList, true, true, p, maxIterations);

            if (doPOVM)
                compute(pList, false, true, p, maxIterations);
        }

        if (doReal)
        {
            if (doProjective)
                compute(pList, true, false, p, maxIterations);

            if (doPOVM)
                compute(pList, false, false, p, maxIterations);
        }
    }

    /*for (Expression* p = pList->head(); p; p = p->next())
    {
        N32 inconsistent = 0;

        double povm, proj, sdp;
        p->readDouble("SearchValue", povm, 0.);
        p->readDouble("ProjectiveValue", proj, 0.);
        p->readDouble("quantumValue", sdp, 0.);

        if(!EQ4(povm, sdp))
            inconsistent = 1;

        p->writeInt("inconsistent", inconsistent);
        }*/

    return 0;
}

//eof
