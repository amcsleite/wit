#pragma once
#include "Essential.h"
#include "M1.h"

class Search : public Counted
{
	friend double fun(const gsl_vector* pV, void* params);

    ExpressionPtr _pExp;
	N32 _preparations;
	N32 _measurements;
	N32 _states;
	N32 _outcomes;
	N32 _vars;
	bool _fProjective;
    N32 _fComplex;

	bool _bUseExpectedValue;
	int _totalOptimizationVars;
	N32 _iter;
	
	N32 _stateParameters;
	N32 _operatorParameters;
	
	const gsl_vector* _pV;

	std::vector<Matrix> _measurementOperators;
	std::vector<Matrix> _quantumStates;

	std::vector<Matrix> _bestMeasurementOperators;
	std::vector<Matrix> _bestQuantumStates;

	gsl_multimin_fminimizer* _pMinimizer;

	N32 _curP;
	
	double _cp;
	double _bestQuantumValue;

	ExpressionListPtr _pOutPermutations;
    Expression* _pCV[MAX_cols];

public:

	Search(Expression* pE, bool fProjective, bool fReal, N32 preparations, N32 measurements, N32 states, N32 outcomes);

	double evaluateSimple();
	double evaluatePermuting();

	N32 getPermutedValue(N32 m, N32 o)
	{
		return _pCV[m]->get(o) - 1;
	}
	
	void saveMat(const char* pName, Expression* p);

	void showResult(gsl_multimin_fminimizer* pMinimizer);
	void showIter(gsl_multimin_fminimizer* pMinimizer);

	void printStatus();
	void printPOVM();
	
	bool run(N32 maxIterations, bool fDump, double& result);
	double repeat(N32 repetitions, N32 maxIterations, bool& valid);

	Matrix mulgm(N32 k);

	void initP()
	{
		_curP = -1;
	}
	
	void nextP()
	{
		_curP++;
		XK(_curP < _totalOptimizationVars);
		_cp = 2. * M_PI * gsl_vector_get(_pV, _curP);
	}

	Matrix& state(N32 p)
	{
		XK( p < _preparations);
		return _quantumStates[p];
	}

	Matrix& oper(N32 measurement, N32 outcome)
	{
		XK(measurement < _measurements);
		XK(outcome < _outcomes);
		return _measurementOperators[measurement * _outcomes + outcome];
	}

	Matrix& bestState(N32 p)
	{
		XK( p < _preparations);
		return _bestQuantumStates[p];
	}

	Matrix& bestOper(N32 measurement, N32 outcome)
	{
		XK(measurement < _measurements);
		XK(outcome < _outcomes);
		return _bestMeasurementOperators[measurement * _outcomes + outcome];
	}

	Expression* getExp()
	{
		return _pExp;
	}

	double fun(const gsl_vector* v);
	static double staticFun(const gsl_vector* pV, void* params);

	void mkStates();
	void mkStatesGM();

	void mkGeneralSearch();
	void mkProjectiveSearch();
	void prepareLambda(Matrix& l);

	void mkProjective();
	bool isProjective(N32 m);
	bool isProjectiveDump(N32 m);
	bool isComplete(N32 m);
	bool isPositive(const Matrix& m);

	void mkMultipleUnitaries(Matrix (& u)[10], N32 wanted);

	bool getUseExpectedValue()
	{
		return _bUseExpectedValue;
	}
	
	N32 getPreparations()
	{
		return _preparations;
	}

	N32 getMeasurements()
	{
		return _measurements;
	}

	N32 getVars()
	{
		XK(_vars == _outcomes - 1);
		return _vars;
	}

	N32 getStates()
	{
		return _states;
	}

	N32 getOutcomes()
	{
		return _outcomes;
	}
	
	static void tests();
};

typedef SmartPtr<Search> SearchPtr;

// eof
