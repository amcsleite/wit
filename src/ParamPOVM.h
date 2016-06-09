#pragma once
#include "Essential.h"
#include "M1.h"

class ParamPOVM : public Counted
{
	N32 _stateParameters;
	N32 _operatorParameters;
	N32 _totalOptimizationVars;
	N32 _curP;
	N32 _preparations;
	N32 _measurements;
	N32 _states;
	N32 _outcomes;
	N32 _vars;

	double _cp;

public:

	ParamPOVM(N32 preparations, N32 measurements, N32 states, N32 outcomes);

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

	void prepareLambda(Matrix& l);
	void mkMultipleUnitaries(Matrix (& u)[10], N32 wanted);
};

typedef SmartPtr<SearchParamPOVM> SearchPtr;

// eof
