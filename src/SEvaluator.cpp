#include "Essential.h"
#include "Expression.h"
#include "Exception.h"
#include "Evaluator.h"
#include "SEvaluator.h"

N32 SEvaluator::evaluate2(Expression* p)
{
	for (N32 r = 0; r < p->getRows(); ++r)
    {
        for (N32 c = 0; c < p->getCols(); ++c)
        {
            for (N32 v = 0; v < p->getVs(); ++v)
            {
				N32 v1, v2;

				bool b1 = p->vget(r, c, v, v1);
				bool b2 = p->vget(c, r, v, v2);

				if ( ( b1 && b2 && v1 == v2 ) || !b1 || !b2 )
				{
					p->tMark() ++;
				}
            }
        }
    }
	
	return p->tMark();
}

//eof
