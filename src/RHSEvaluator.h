#pragma once
#include "Essential.h"
#include "Expression.h"
#include "Evaluator.h"

class RHSEvaluator : public Evaluator
{
	N32 _direction;
	
public:
	RHSEvaluator(N32 direction)
	{
		_direction = direction;
	}
	
	virtual N32 evaluate2(Expression* p);
};

// eof
