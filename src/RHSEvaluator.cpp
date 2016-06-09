#include "Essential.h"
#include "Expression.h"
#include "Exception.h"
#include "Evaluator.h"
#include "RHSEvaluator.h"

N32 RHSEvaluator::evaluate2(Expression* p)
{
	p->tMark() += _direction * p->getRHS();
    return p->tMark();
}

//eof
