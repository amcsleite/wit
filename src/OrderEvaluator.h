#pragma once
#include "Essential.h"
#include "Expression.h"
#include "Evaluator.h"

class OrderEvaluator : public Evaluator
{
	N32 _direction;
	
public:
	OrderEvaluator(N32 direction = 1)
	{
		_direction = direction;
	}
	
	virtual N32 evaluate2(Expression* p);
};

// eof
