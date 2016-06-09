#pragma once
#include "Essential.h"
#include "Lists.h"
#include "IntList.h"

class Evaluator;
class ExpressionList : public XList3<ExpressionList, ExpressionList, Expression>
{
	N32 _preparations;
	N32 _measurements;
	N32 _states;
	N32 _outcomes;
	N32 _vars;
	N32 _varsInititialized;
	N32 _nSDPRepeats;
	
public:

    ExpressionList();
    ExpressionList(const ExpressionList& t) { }
    ~ExpressionList()  {  }

	N32 getPreparations()
	{
		return _preparations;
	}

	N32 getMeasurements()
	{
		return _measurements;
	}

	N32 getStates()
	{
		return _states;
	}

	N32 getOutcomes()
	{
		return _outcomes;
	}

	N32 getVars()
	{
		return _vars;
	}

	N32 getSDPRepeats()
	{
		return _nSDPRepeats;
	}

	void readListVars();

    ExpressionListPtr rCopy();

	void initVars();

	void resetMarks();

	ExpressionListPtr copyNoTrivials();

	ExpressionListPtr compactSymmetricSlow(ExpressionList* pRowPermutations, ExpressionList* pColPermutations, ExpressionList* pOutcomePermutions, bool fEV);

    void print();
	void writePanda(const char* pName, bool useExpectedValue, IntList* pPermutations);
	void writePorta(const char* pName, bool useExpectedValue, N32 d);
	void writeLRS(const char* pName, bool useExpectedValue, N32 d, N32 preparations, N32  measurements, N32  channelDimensions, N32 outcomes);

	bool verify(ExpressionList* p);
	
	void printTeXPage(FILE* pF, Expression*& pExp);

	void printTeX(const char* pName, N32 preparations, N32 measurements, N32 dimensions, N32 outcomes, bool mkDoc, N32 total);
	void prepareAndPrintTeX(const char* pName, N32 preparations, N32 measurements, N32 dimensions, N32 outcomes, bool mkDoc);
	void writeIEQPorta(const char* pName, N32 d, const char* pComment);
	void writePortaSplit(const char* pName1, const char* pName2, N32 start, N32 count);

	void scan(Evaluator* pEvaluator);
	Expression* findPos(Expression* pNew);
	ExpressionListPtr sort(Evaluator* pEvaluator);

	ExpressionListPtr selectTheBestPermutationOfEachExpression(ExpressionList* pRowPermutations, ExpressionList* pColPermutations, ExpressionList* pVarPermutations, Evaluator* pEv, bool bEv);
	void prepareMat(FILE* pF);
};

// eof
