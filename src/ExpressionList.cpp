#include <stdint.h>
#include "Essential.h"
#include "ExpressionList.h"
#include "Expression.h"
#include "EHash.h"
#include "Evaluator.h"
#include "SEvaluator.h"
#include "WriteVar.h"

#define min(a,b) ((a) < (b) ? (a) : (b))

ExpressionList::ExpressionList()
{
    _preparations = 0;
    _measurements = 0;
    _states = 0;
    _outcomes = 0;
    _vars = 0;
    _varsInititialized = false;
	_nSDPRepeats = 5;
}

ExpressionListPtr ExpressionList::rCopy()
{
    ExpressionListPtr pCopy = new ExpressionList();

    for (Expression* p = head(); p; p = p->next())
    {
        pCopy->pushBack(p->rCopy());
    }

    return pCopy;
}

void ExpressionList::print()
{
    int order = 1;

    for (Expression* pExp = head(); pExp; pExp = pExp->next())
    {
        ilog("");

        char sz[2048];
        int pos = 0;
        sz[pos] = 0;

        pos += sprintf(sz + pos, "%3d :", order);

        pExp->getTermsText(sz, pos);

        if (pExp->getOriginalExp())
        {
            pos += sprintf(sz + pos, "      ");
            pExp->getOriginalExp()->getTermsText(sz, pos);
            log("%80s    %s", sz,  pExp->getPortaExp() ? pExp->getPortaExp() : "");
        }
        else
            log("%40s    %s", sz,  pExp->getPortaExp() ? pExp->getPortaExp() : "");

        ++order;
    }
}

void ExpressionList::writePanda(const char* pName, bool useExpectedValue, IntList* pPermutations)
{
    FILE* pF = fopen(pName, "w");

    if (!pF)
    {
        Util::errorMsg("couldn't openfile");
        exit(EXIT_FAILURE);
    }

    N32 nVertex = 1;

    Expression* p = head();

    fprintf(pF, "\nNames");

    fprintf(pF, "\n");

    for (N32 r = 0; r < p->getRows(); ++r)
    {
        for (N32 c = 0; c < p->getCols(); ++c)
        {
            if (useExpectedValue)
                fprintf(pF, "E%d%d ", r + 1, c + 1);
            else
            {
                for (N32 v = 0; v < p->getVs(); ++v)
                {
                    fprintf(pF, "P%d%d%d ", r + 1, c + 1, v + 1);
                }
            }
        }
    }

    fprintf(pF, "\n\nMaps\n");

    for (IntList* pP = pPermutations->head(); pP; pP = pP->next())
    {
        for (IntList* pEl = pP->head(); pEl; pEl = pEl->next())
        {
            IntList* pR = pEl->head();

            if (!pR)
            {
                XK(false);
                break;
            }

            IntList* pC = pR->next();

            if (!pC)
            {
                XK(false);
                break;
            }

            IntList* pV = pC->next();

            if (!pV)
            {
                XK(false);
                break;
            }

            N32 r = pR->get();
            N32 c = pC->get();
            N32 v = pV->get();

            if (useExpectedValue)
                fprintf(pF, "E%d%d ", r, c);
            else
                fprintf(pF, "P%d%d%d ", r, c, v);
        }

        fprintf(pF, "\n");
    }

    fprintf(pF, "\n\nVertices\n");

    for (Expression* p = head(); p; p = p->next())
    {
        for (N32 r = 0; r < p->getRows(); ++r)
        {
            for (N32 c = 0; c < p->getCols(); ++c)
            {
                for (N32 v = 0; v < p->getVs(); ++v)
                {
                    if (useExpectedValue)
                        fprintf(pF, " %2d ", p->get(r, c, v) ? 1 : -1);
                    else
                        fprintf(pF, " %2d ", p->get(r, c, v));
                }
            }
        }

        fprintf(pF, "\n");
        nVertex++;
    }

    fclose(pF);
}

void ExpressionList::writePorta(const char* pName, bool useExpectedValue, N32 d)
{
    FILE* pF = fopen(pName, "w");

    if (!pF)
    {
        Util::errorMsg("couldn't openfile");
        exit(EXIT_FAILURE);
    }

    N32 nVertex = 1;

    fprintf(pF, "DIM=%d\n", d);
    fprintf(pF, "COMMENT\n");
    fprintf(pF, "CONV_SECTION\n");

    for (Expression* p = head(); p; p = p->next())
    {
        fprintf(pF, "( %5d ) ", nVertex);

        for (N32 r = 0; r < p->getRows(); ++r)
        {
            for (N32 c = 0; c < p->getCols(); ++c)
            {
                for (N32 v = 0; v < p->getVs(); ++v)
                {
                    if (useExpectedValue)
                        fprintf(pF, " %2d ", p->get(r, c, v) ? 1 : -1);
                    else
                        fprintf(pF, " %2d ", p->get(r, c, v));
                }
            }
        }

        fprintf(pF, "\n");
        nVertex++;
    }

    fprintf(pF, "END\n");
    fclose(pF);
}

void ExpressionList::writePortaSplit(const char* pName1, const char* pName2, N32 start, N32 count)
{
    FILE* pF1 = fopen(pName1, "w");
    FILE* pF2 = fopen(pName2, "w");

    if (!pF1 || !pF2)
    {
        Util::errorMsg("error opening file(s)");
        exit(EXIT_FAILURE);
    }

    N32 nVertex = 1;

	Expression* p = head();

    fprintf(pF1, "DIM=%d\n", count);
    fprintf(pF1, "COMMENT\n");
    fprintf(pF1, "CONV_SECTION\n");

    fprintf(pF2, "DIM=%d\n", p->getItems() - count);
    fprintf(pF2, "COMMENT\n");
    fprintf(pF2, "CONV_SECTION\n");

    for (; p; p = p->next())
    {
        fprintf(pF1, "( %5d ) ", nVertex);
        fprintf(pF2, "( %5d ) ", nVertex);

        for (N32 i = 0; i < p->getItems(); ++i)
        {
			if (i >= start && i < start + count)
			{
				fprintf(pF1, " %2d ", p->get(i));
			}
			else
			{
				fprintf(pF2, " %2d ", p->get(i));
			}
        }

        fprintf(pF1, "\n");
        fprintf(pF2, "\n");
        nVertex++;
    }

    fprintf(pF1, "END\n");
    fprintf(pF2, "END\n");

	fclose(pF1);
    fclose(pF2);
}

void ExpressionList::writeLRS(const char* pName, bool useExpectedValue, N32 d, N32 preparations, N32  measurements, N32  channelDimensions, N32 outcomes)
{
    FILE* pF = fopen(pName, "w");

    if (!pF)
    {
        Util::errorMsg("couldn't openfile");
        exit(EXIT_FAILURE);
    }

    N32 nVertex = 1;

    fprintf(pF, "Device");
    fprintf(pF, "\n* preparations: %d measurements:%d channelDimensions:%d outcomes:%d", preparations, measurements, channelDimensions, outcomes);
    fprintf(pF, "\n* plrs format. See lrs site for more. Interpret inequalities as follows:");
    fprintf(pF, "\n* const <= E11 E12 ...E1m ...  En1...Enm");
    fprintf(pF, "\nV-representation");
    fprintf(pF, "\nbegin");
    fprintf(pF, "\n%d %d rational", count(), d + 1);

    for (Expression* p = head(); p; p = p->next())
    {
        fprintf(pF, "\n1 ");

        for (N32 r = 0; r < p->getRows(); ++r)
        {
            for (N32 c = 0; c < p->getCols(); ++c)
            {
                for (N32 v = 0; v < p->getVs(); ++v)
                {
                    if (useExpectedValue)
                        fprintf(pF, " %2d ", p->get(r, c, v) ? 1 : -1);
                    else
                        fprintf(pF, " %2d ", p->get(r, c, v));
                }
            }
        }

        nVertex++;
    }

    fprintf(pF, "\nend");
    fclose(pF);
}

ExpressionListPtr ExpressionList::compactSymmetricSlow(ExpressionList* pRowPermutations, ExpressionList* pColPermutations, ExpressionList* pVarPermutations, bool bEV)
{
#ifdef _DEBUG
    ilog("-------------------- Starting compacting --------");
#endif

    EHashPtr pH = new EHash;

    ExpressionListPtr pT = new ExpressionList();

    if (!head())
        return pT;

    for (Expression* p = head(); p; p = p->next())
    {
#ifdef _DEBUG
        ilog("checking:");
        p->printExp();
        fflush(stdout);
#endif

        ExpressionListPtr pAllEquivalent = p->generateEquivalentSymmetricExpressions(pRowPermutations, pColPermutations, pVarPermutations, bEV);

#ifdef _DEBUG
        ilog("adding equivalents to above expression");
        pAllEquivalent->print();
        ilog("------------------------");

        //XK(p->getRHS() != 3);
#endif

        bool bIN = false;

        for (Expression* q = pAllEquivalent->head(); q; q = q->next())
        {
            Expression* pIN = pH->hLookup(q);

            if (pIN)
            {
                bIN = true;

                break;
            }
        }

        if (!bIN)
        {
            ExpressionPtr pCopy = p->rCopy();
            pH->setAt(pCopy);

            XK(pH->hLookup(p));
            XK(pH->hLookup(pCopy));

            pT->pushBack(pCopy);
        }

		#ifdef _DEBUG
		p->printExp();

		if (bIN)
			ilog("was added");
		else
			ilog("not added");
			
		#endif
    }

    return pT;
}

char szDocHeader[] = "\
\n\\documentclass[fleqn]{article} \
\n\\input{\\string~/wTools/tex/al.tex} \
\n\\usepackage{array} \
\n\\newcolumntype{C}{>{\\centering\\arraybackslash}p{5mm}} \
\n\\newcolumntype{D}{>{\\centering\\arraybackslash}p{5mm}} \
\n\\begin{document} \
\n\\large \
";

char szDocFooter[] = "\\end{document}";

void ExpressionList::printTeXPage(FILE* pF, Expression*& pExp)
{
    N32 rowsPerPage = 5;
    N32 matricesPerRow = 1;

    Util::readIntParam("db/rowsPerPage", rowsPerPage, 5);
    //Util::readIntParam("db/matricesPerRow", matricesPerRow, 2);

    N32 order = 0;

    N32 rows = pExp->getRows();
    N32 cols = pExp->getCols();
    N32 vars = pExp->getVs();

    bool bComplete = false;

    //fprintf(pF, "\\[");

    fprintf(pF, "\n\\begin{tabular}{");

    for (N32 n = 0; n < matricesPerRow; ++n)
        fprintf(pF, "c");

    fprintf(pF, "}");

    for (; pExp && order / matricesPerRow < rowsPerPage; pExp = pExp->next())
    {
		pExp->printTeX(pF, this);

		if (order % matricesPerRow == matricesPerRow - 1)
			fprintf(pF, "\\\\ \\\\");
		else
			fprintf(pF, "&");

		++order;

    }

    fprintf(pF, "\n\\end{tabular}");

}

void ExpressionList::printTeX(const char* pName, N32 preparations, N32 measurements, N32 dimensions, N32 outcomes,  bool mkDoc, N32 total)
{
    FILE* pF = fopen(pName, "w");

    if (!pF)
        return;

    if (mkDoc)
        fprintf(pF, szDocHeader);

    Expression* pExp = head();

    if (!pExp)
        return;

    N32 maxPages = 5;
    N32 rowsPerPage = 5;
    N32 matricesPerRow = 3;

    Util::readIntParam("db/maxPages", maxPages, 5);
    Util::readIntParam("db/rowsPerPage", rowsPerPage, 5);
    Util::readIntParam("db/matricesPerRow", matricesPerRow, 5);

    N32 useLRS = 0;
    Util::readIntParam("db/useLRS", useLRS, 0);

    N32 usePanda = 0;
    Util::readIntParam("db/usePanda", usePanda, 0);

    N32 finished = 0;
    Util::readIntParam("db/finished", finished, 0);

    N32 maxMatrices = maxPages * rowsPerPage * matricesPerRow;

    fprintf(pF, "\n\\section*{p:%d m:%d d:%d b:%d showing %d of %d.", preparations, measurements, dimensions, outcomes, min(total, maxMatrices), total);

    if (useLRS)
        fprintf(pF, " Program used: plrs - %s }", finished ? " finished" : " not finished");
    else if(usePanda)
        fprintf(pF, " Program used: panda - %s }", finished ? " finished" : " not finished");
    else
        fprintf(pF, " Program used: porta - %s }", finished ? " finished" : " not finished");

    N32 pages = 0;

    while (pExp && pages < maxPages)
    {
        printTeXPage(pF, pExp);
        pages++;

        fprintf(pF, "\n");

        if (pages < maxPages && pExp)
            fprintf(pF, "\\newpage");
    }

    if (mkDoc)
        fprintf(pF, szDocFooter);

    fclose(pF);
}

ExpressionListPtr ExpressionList::copyNoTrivials()
{
    ExpressionListPtr pList = new ExpressionList;

    for (Expression* p = head(); p ; p = p->next())
    {
        if (p->isTrivial())
            continue;

        pList->pushBack(p->rCopy());
    }

    return pList;
}

void ExpressionList::prepareAndPrintTeX(const char* pName, N32 preparations, N32 measurements, N32 dimensions, N32 outcomes, bool mkDoc)
{
    ExpressionListPtr pList = new ExpressionList;

    N32 n = 0;

    for (Expression* p = head(); p ; p = p->next())
    {
        if (p->isTrivial())
            continue;

#ifdef _DEBUG
        ilog("------ max. equivalents of expression: ");
        p->printLHS();
#endif

        ExpressionPtr pExp = p->rCopy();

#ifdef _DEBUG
        ilog("result is:");
        pExp->printLHS();
        ilog("-------");
#endif
        pList->pushBack(pExp);
        n++;
    }

    pList->printTeX(pName, preparations, measurements, dimensions, outcomes, mkDoc, pList->count());
}

bool ExpressionList::verify(ExpressionList* pVList)
{
    int respected = 0;
    int failed = 0;

    for (Expression* pExp = head(); pExp; pExp = pExp->next())
    {
        for (Expression* pVertex = pVList->head(); pVertex; pVertex = pVertex->next())
        {
            N64 total = pExp->verifyVertex(pVertex);

            bool valid = false;

            switch (pExp->getIne())
            {
                case -2:
                    valid = (total < pExp->getRHS());
                    break;

                case -1:
                    valid = (total <= pExp->getRHS());
                    break;

                case 0:
                    valid = (total == pExp->getRHS());
                    break;

                case 1:
                    valid = (total >= pExp->getRHS());
                    break;

                case 2:
                    valid = (total > pExp->getRHS());
                    break;
            }

            if (!valid)
            {
                ilog("The expression:");
                pExp->printEq();

                ilog("failed the vertex:");
                pVertex->printLHS();

                ilog("calculated rhs: %lld ine: %s ", total, pExp->ineText());
                ilog("-----------------------------------------------------------");
                //pExp->verifyVertex(pVertex);
                //return false;
                ++failed;
            }
            else
            {
                /*
                //printf("\nThe vertex:");
                pVertex->printVertex(stdout);

                pExp->printHuman(stdout);

                printf ("\n %lld %d ", total, pExp->getIne());
                fflush(stdout);
                //pExp->verifyVertex(pVertex);
                //return false;*/
                ++respected;
            }
        }
    }

    ilog("Expressions: %d Vertices: %d", count(), pVList->count());
    ilog("respects %d failed %d out of %d", respected, failed, respected + failed);

    printf("\n\n Expressions: %d Vertices: %d", count(), pVList->count());
    printf("\n\n - respected: %d failed: %d out of %d - ", respected, failed, respected + failed);

    return failed == 0;
}

void ExpressionList::writeIEQPorta(const char* pName, N32 d, const char* pComment)
{
    FILE* pF = fopen(pName, "w");

    if (!pF)
    {
        Util::errorMsg("couldn't openfile");
        exit(EXIT_FAILURE);
    }

    N32 nVertex = 1;

    fprintf(pF, "DIM=%d\n", d);
    fprintf(pF, "COMMENT %s\n", pComment);
    fprintf(pF, "INEQUALITIES_SECTION\n");

    for (Expression* p = head(); p; p = p->next())
    {
        fprintf(pF, "( %5d ) ", nVertex);

        N32 i = 1;

        for (N32 r = 0; r < p->getRows(); ++r)
        {
            for (N32 c = 0; c < p->getCols(); ++c)
            {
                for (N32 v = 0; v < p->getVs(); ++v)
                {
                    N32 s = p->get(r, c, v);

                    if (s == 0)
                        fprintf(pF, "     ");
                    else if (s == 1)
                        fprintf(pF, " +x%d ", i);
                    else if (s == -1)
                        fprintf(pF, " -x%d ", i);
                    else
                        fprintf(pF, " %+2dx%d ", s, i);

                    ++i;
                }
            }
        }

        fprintf(pF, "%s %d\n", p->ineText(), p->getRHS());
        nVertex++;
    }

    fprintf(pF, "END\n");
    fclose(pF);
}

void ExpressionList::scan(Evaluator* pEvaluator)
{
    for (Expression* p = head(); p; p = p->next())
    {
        pEvaluator->evaluate(p);
    }
}

Expression* ExpressionList::findPos(Expression* pNew)
{
    for (Expression* p = head(); p; p = p->next())
    {
        if (Expression::gm(pNew, p))
            return p;
    }

    return NULL;
}

ExpressionListPtr ExpressionList::sort(Evaluator* pEvaluator)
{
	// high valued on top
    ExpressionListPtr pSortedList = new ExpressionList();

    for (Expression* pExp = head(); pExp; pExp = pExp->next())
    {
        ExpressionPtr pCopy = pExp->rCopy();

        pEvaluator->evaluate(pCopy);

        Expression* pWhere = pSortedList->findPos(pCopy);

        if (pWhere)
            pSortedList->insertNodeInList(pWhere, pCopy, true);
        else
        {
            pSortedList->pushBack(pCopy);
        }
    }

    // consolidate
    N32 curCMark = INT32_MIN ;
    N32 curTMark = INT32_MIN ;
    N32 m = 0;

    for (Expression* p = pSortedList->head(); p; p = p->next())
    {
        if (p->cMark() != curCMark || p->tMark() != curTMark )
        {
            --m;
            curCMark = p->cMark();
            curTMark = p->tMark();

#ifdef _DEBUG
            ilog("::::::::::::: %d %d", curCMark, curTMark);
#endif
        }

        p->cMark() = m;
        p->tMark() = 0;
    }

    return pSortedList;
}

void ExpressionList::resetMarks()
{
    for (Expression* p = head(); p; p = p->next())
    {
        p->cMark() = 0;
        p->tMark() = 0;
    }
}

void ExpressionList::prepareMat(FILE* pF)
{
    for (Expression* p = head(); p; p = p->next())
    {
		p->writeWitMat(this, pF, p->getID());
    }
}

ExpressionListPtr ExpressionList::selectTheBestPermutationOfEachExpression(ExpressionList* pRowPermutations, ExpressionList* pColPermutations, ExpressionList* pVarPermutations, Evaluator* pEv, bool bEv)
{
    ExpressionListPtr pT = new ExpressionList();

    for (Expression* p = head(); p; p = p->next())
    {
        ExpressionListPtr pAllEquivalent;

        pAllEquivalent = p->generateEquivalentSymmetricExpressions(pRowPermutations, pColPermutations, pVarPermutations, bEv);

        ExpressionListPtr pOrdered = pAllEquivalent->sort(pEv);

        Expression* pBestRepresentative = pOrdered->head();
        ExpressionPtr pCopy = pBestRepresentative->rCopy();
        pT->pushBack(pCopy);
    }

    return pT;
}

void ExpressionList::readListVars()
{
    if(_varsInititialized)
        return;

    if (!Util::readIntParam("db/preparations", _preparations, 3))
    {
        Util::errorMsg("preparations parameter file not found");
        exit(EXIT_FAILURE);
    }

    if (!Util::readIntParam("db/measurements", _measurements, 3))
    {
        Util::errorMsg(" measurements file not found");
        exit(EXIT_FAILURE);
    }

    if(!Util::readIntParam("db/states", _states, 2))
    {
        Util::errorMsg("dimensions parameter file not found");
        exit(EXIT_FAILURE);
    }

    if(!Util::readIntParam("db/outcomes", _outcomes, 2))
    {
        Util::errorMsg("outcomes parameter file not found");
        exit(EXIT_FAILURE);
    }

    Util::readIntParam("db/SDPRepeats", _nSDPRepeats, 5);

    _vars = _outcomes - 1;
    _varsInititialized = true;
}

//eof
