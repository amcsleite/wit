#include "Essential.h"
#include "Expression.h"
#include "Exception.h"
#include "Evaluator.h"
#include "ZerosEvaluator.h"

N32 ZerosEvaluator::evaluate2(Expression* p)
{
    bool zeroRows[MAX_rows];
    bool zeroCols[MAX_cols];
    bool zeroVars[MAX_vars];

    for (N32 r = 0; r < p->getRows(); ++r)
        zeroRows[r] = true;

    for (N32 c = 0; c < p->getCols(); ++c)
        zeroCols[c] = true;

    for (N32 v = 0; v < p->getVs(); ++v)
        zeroVars[v] = true;

    for (N32 r = 0; r < p->getRows(); ++r)
    {
        bool zeroCol = true;

        for (N32 c = 0; c < p->getCols(); ++c)
        {
            bool zeroVar = true;

            for (N32 v = 0; v < p->getVs(); ++v)
            {
                N32 v1;

                bool b1 = p->vget(r, c, v, v1);

                if (b1 && v1 != 0 )
                {
                    zeroRows[r] = false;
                    zeroCols[c] = false;
                    zeroVars[v] = false;
                }
            }
        }
    }

    for (N32 r = 0; r < p->getRows(); ++r)
    {
        if( zeroRows[r])
            p->tMark() += _direction * (r + 1) * 10000;
    }

    for (N32 c = 0; c < p->getCols(); ++c)
    {
        if( zeroCols[c])
            p->tMark() += _direction * (c + 1) * 1000;
    }

    for (N32 v = 0; v < p->getVs(); ++v)
    {
        if( zeroVars[v])
            p->tMark() += _direction * (v + 1) * 100000;
    }

    return p->tMark();
}

//eof
