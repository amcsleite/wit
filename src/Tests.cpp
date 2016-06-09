#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "Essential.h"
#include "DIGenerator.h"
#include "ExpressionList.h"
#include "Expression.h"

void runTests10 (int argc, char* argv[])
{
    ExpressionListPtr pL = Util::genRotations(5);
    ilog("rotations");
    pL->print();
}

void runTests9 (int argc, char* argv[])
{
    ExpressionListPtr pRowPermutations = Util::quickPerm(5);
    ExpressionListPtr pColPermutations = Util::quickPerm(2);
    ExpressionListPtr pVarPermutations = Util::quickPerm(1);

    ilog("row permutations");
    pRowPermutations->print();

    ilog("col permutations");
    pColPermutations->print();

    ilog("outcome permutations");
    pVarPermutations->print();

	ilog("---- p: ----");
	ExpressionPtr p = new Expression;
	p->set(0, 0, 0, 1);
	p->set(0, 1, 0, 1);
	p->set(1, 0, 0, 1);
	p->set(1, 1, 0, -1);
	p->set(2, 0, 0, 0);
	p->set(2, 1, 0, 0);
	p->set(3, 0, 0, -1);
	p->set(3, 1, 0, 1);
	p->set(4, 0, 0, -1);
	p->set(4, 1, 0, -1);
	p->printMatrix();


//-1  -1      1   1     -1   1      1  -1      0   0 
	ExpressionPtr p2 = new Expression;
	p2->set(0, 0, 0, -1);
	p2->set(0, 1, 0, 1);
	p2->set(1, 0, 0, 1);
	p2->set(1, 1, 0, 1);
	p2->set(2, 0, 0, -1);
	p2->set(2, 1, 0, 1);
	p2->set(3, 0, 0, 1);
	p2->set(3, 1, 0, -1);
	p2->set(4, 0, 0, 0);
	p2->set(4, 1, 0, 0);
	
	ilog("---- p2: ----");
	p2->printMatrix();

	p2->isBetter(p);
	
    ExpressionPtr pExp = p->findSpecialEquivalent(pRowPermutations, pColPermutations, pVarPermutations);

	ilog("---- result: ----");
	pExp->printMatrix();
	ilog("--------");
}

void runTests8 (int argc, char* argv[])
{
    ilog("2 -------------");
    ExpressionListPtr pL0 = Util::quickPerm(2);

    pL0->print();

    ilog("3 -------------");
    ExpressionListPtr pL = Util::quickPerm(3);
    pL->print();

    ilog("4 -------------");
    ExpressionListPtr pL1 = Util::quickPerm(4);
    pL1->print();

    ilog("5 -------------");
    ExpressionListPtr pL2 = Util::quickPerm(5);
    pL2->print();

}

/*
int runTests7 (int argc, char* argv[])
{
    ilog("*** Testing sort stuff ***");

    ExpressionPtr pE = new Expression();
    {
        {
            TermPtr pT = new Term();
            pT->setRow(0);
            pT->setCol(0);

            pT->setState(1);
            pE->pushBack(pT);
        }
        {
            TermPtr pT = new Term();
            pT->setRow(0);
            pT->setCol(1);

            pT->setState(1);
            pE->pushBack(pT);
        }

        ////
        {
            TermPtr pT = new Term();
            pT->setRow(1);
            pT->setCol(0);

            pT->setState(1);
            pE->pushBack(pT);
        }
        {
            TermPtr pT = new Term();
            pT->setRow(1);
            pT->setCol(1);

            pT->setState(0);
            pE->pushBack(pT);
        }

        ///
        {
            TermPtr pT = new Term();
            pT->setRow(2);
            pT->setCol(0);

            pT->setState(-1);
            pE->pushBack(pT);
        }
        {
            TermPtr pT = new Term();
            pT->setRow(2);
            pT->setCol(1);

            pT->setState(-1);
            pE->pushBack(pT);
        }

        ilog("*** original ***");
        pE->printWithRowCol();

        ilog("*** sorted linear ***");
        ExpressionPtr pSorted = pE->sortExpression();
        pSorted->printWithRowCol();

        ilog("*** sorted qs ***");
        ExpressionPtr pSorted1 = pE->qsExp();
        pSorted1->printWithRowCol();

        ilog("*** Testing init ***");
        ExpressionPtr pInit = new Expression(pSorted1);
        pInit->printMatrix();

    }

    ExpressionPtr pS = new Expression();
    //pS->sortPM(pE);
    pS->printMatrix();

    return 0;
}
*/

int runTests4 (int argc, char* argv[])
{
    for (int base = 0; base < 8; ++base)
    {
        ilog("------ base %d ------", base);

        for (int s = 0; s < base; ++s)
        {
            /*Indent indent;
            TermPtr p = new Term;

            p->setState(s);

            ilog("%2d ---> %2d", s, p->symState(base));*/
        }
    }

    ilog("");
    return 0;
}
/*first:
rows: 4 cols: 3
  0   0   0
  0   0   0
  0   0  -1
  0   0   0
rotated:
rows: 4 cols: 3
  0   0   0
  0   0   0
  0   0  -1
  0   0   0
rotated:
rows: 4 cols: 3
  0   0   0
  0  -1   0
  0   0   0
  0   0   0
rotated:
rows: 4 cols: 3
  0   0   0
  0  -1   0
  0   0   0
  -   -   -

*/



LogStream g_logStream(NULL);

int main (int argc, char* argv[])
{
	ilog("%f", Util::rDouble());
	ilog("%f", Util::rDouble());
	ilog("%f", Util::rDouble());
	ilog("%f", Util::rDouble());
    runTests10(argc, argv);
    return 0;
}


// eof

