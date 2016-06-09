#pragma once
#include "Essential.h"
#include "Expression.h"
#include "Evaluator.h"

class ZerosEvaluator : public Evaluator
{
	N32 _direction;
	
public:
	ZerosEvaluator()
	{
		_direction = -1;
	}
	
	virtual N32 evaluate2(Expression* p);
};

// eof
