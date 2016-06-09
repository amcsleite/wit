#include "Essential.h"
#include "Expression.h"
#include "Symmetries.h"
#include "IntList.h"

IntListPtr Symmetries::permute(Expression* pRowPerm, Expression* pColPerm, Expression** pColVar)
{
    IntListPtr pL = new IntList;

    N32 originalRow = 0;

    for (N32 rp = 0; rp < pRowPerm->getItems(); ++ rp)
    {
        N32 r = pRowPerm->refA(rp) - 1;

        N32 originalCol = 0;

        for (N32 cp = 0; cp < pColPerm->getItems(); ++cp)
        {
            N32 c = pColPerm->refA(cp) - 1;

            N32 originalVar = 0;

            for (N32 vp = 0; vp < pColVar[c]->getItems(); ++vp)
            {
                N32 v = pColVar[c]->refA(vp) - 1;

				IntListPtr p = new IntList;
				p->pushBack(new IntList(r + 1));
				p->pushBack(new IntList(c + 1));
				p->pushBack(new IntList(v + 1));
				
				pL->pushBack(p);

			    ++originalVar;
            }

            ++originalCol;
        }

        ++originalRow;
    }

    return pL;
}

IntListPtr Symmetries::generate(ExpressionList* pRowPermutations, ExpressionList* pColPermutations, ExpressionList* pVarPermutations)
{
    IntListPtr pSet = new IntList;

    for (Expression* pPermutationRow = pRowPermutations->head(); pPermutationRow; pPermutationRow = pPermutationRow->next())
    {
        for (Expression* pPermutationCol = pColPermutations->head(); pPermutationCol; pPermutationCol = pPermutationCol->next())
        {
            Expression* pCV[MAX_cols];
            startColVar(pCV, MAX_cols, pColPermutations->head()->getItems(), pVarPermutations);

            while (true)
            {
                IntListPtr pL = permute(pPermutationRow, pPermutationCol, pCV);

#ifdef _DEBUG_COLVAR
                ilog("--------------");
                ilog("before:");
                printMatrix();
                pPermutationRow->printInLine();
                pPermutationCol->printInLine();

                ilog("--- start colVar ---");

                for (N32 c = 0; c < pColPermutations->head()->getItems(); ++c)
                    pCV[c]->printInLine();

                ilog("--- end colVar ----");

                ilog("after:");
                pL->printMatrix();
                ilog("\n\n\n");
#endif

                pSet->pushBack(pL);

                if(!nextColVar(pCV, pColPermutations->head()->getItems(), pVarPermutations))
                    break;
            }
        }
    }

    return pSet;
}

// static
bool Symmetries::startColVar(Expression** pCV, N32 maxCols, N32 cols,  ExpressionList* pVarPermutations)
{
    if (!pVarPermutations->head() || cols >= maxCols)
    {
		Util::errorMsg("max columns exceeded");
		exit(EXIT_FAILURE);
        return false;
    }

    memset(pCV, 0, maxCols * sizeof(Expression*));

    for (N32 c = 0; c < cols; ++ c)
        pCV[c] = pVarPermutations->head();

    return true;
}

// static
bool Symmetries::nextColVar(Expression** pCV, N32 cols, ExpressionList* pVarPermutations)
{
    if (!pVarPermutations->head())
        return false;

    N32 n = 0;

    while(true)
    {
        if (n >= cols)
            return false;

        pCV[n] = pCV[n]->next();

        if (pCV[n] != NULL)
            return true;

        pCV[n] =  pVarPermutations->head();
        ++n;
    }

    return true;
}

//eof
