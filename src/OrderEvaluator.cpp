#include "Essential.h"
#include "Expression.h"
#include "Exception.h"
#include "Evaluator.h"
#include "OrderEvaluator.h"

N32 OrderEvaluator::evaluate2(Expression* p)
{
    for (N32 r = 0; r < p->getRows(); ++r)
    {
        for (N32 c = 0; c < p->getCols(); ++c)
        {
            for (N32 v = 0; v < p->getVs(); ++v)
            {
                N32 v1 = 0;

                bool b1 = p->vget(r, c, v, v1);

				// max d is at 0, 0
				N32 d = 10*(p->getRows() - r) + 10*(p->getCols() - c) + p->getVs() - v;

				if (v1 != 0)
				{
					p->tMark() += _direction * v1 * d; // highs to 0,0
				}
				else 
					p->tMark() += -10000 * d; // a zero at 0,0 is very penalized
            }
        }
    }
    return p->tMark();
}

//eof
