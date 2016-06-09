#pragma once
#include "Essential.h"
#include "Lists.h"
#include "IntList.h"

class Symmetries : public Counted
{

public:

	static IntListPtr generate(ExpressionList* pRowPermutations, ExpressionList* pColPermutations, ExpressionList* pOutcomePermutions);
    static IntListPtr permute(Expression* pRowPerm, Expression* pColPerm, Expression** pColVar);
	static IntListPtr findSpecialEquivalent(ExpressionList* pRowPermutations, ExpressionList* pColPermutations,  ExpressionList* pOutcomePermutations);

	static bool startColVar(Expression** pCV, N32 maxCols, N32 cols,  ExpressionList* pVarPermutations);
	static bool nextColVar(Expression** pCV, N32 maxCols, ExpressionList* pVarPermutations);

    static void tests();
};

typedef SmartPtr<Symmetries> SymmetriesPtr;

// eof
