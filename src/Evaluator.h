#pragma once
#include "Essential.h"
#include "Expression.h"

class Evaluator : public Counted
{
public:

	Evaluator()
	{
	}

	N32 evaluate(Expression* p)
	{
		return evaluate2(p);
	}

	virtual N32 evaluate2(Expression* p)
	{
		XK(false);
		return 0;
	}
};

typedef SmartPtr<Evaluator> EvaluatorPtr;

// eof
