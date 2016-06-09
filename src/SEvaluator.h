#pragma once
#include "Essential.h"
#include "Expression.h"
#include "Evaluator.h"

class SEvaluator : public Evaluator
{
public:
	virtual N32 evaluate2(Expression* p);
};

// eof
