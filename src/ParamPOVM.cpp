#include <iostream>
#include <armadillo>
#include "Util.h"
#include "UtilMath.h"
#include "Search.h"
#include "WriteVar.h"
#include "ReadVar.h"
#include "ParamPOVM.h"

ParamPOVM::ParamPOVM(bool fReal, N32 preparations, N32 measurements, N32 states, N32 outcomes)
{
    _preparations = preparations;
    _measurements = measurements;
    _states = states;
    _outcomes = outcomes;
    _vars = outcomes - 1;

	_stateParameters = (d * d - d);
	_operatorParameters = d * d - d + 1;
	_totalOptimizationVars = _preparations * _stateParameters + _measurements * _vars * _operatorParameters;
}

void ParamPOVM::prepareLambda(Matrix& l)
{
    l.zeros(_states, _states);

    for (N32 r = 0; r < _states; ++r)
    {
        for (N32 c = 0; c < _states; ++c)
        {
            if (c > r)
            {
                nextP();
                l(c, r) = _cp / 4.;
            }
            else if (c < r)
            {
                nextP();
                l(c, r) = _cp;
            }
            else
            {
                /*if ( c == 0)//_states - 1)
                {
                    l(c,r) = _cp;
                    nextP();
                }
                else
                {
                    l(c,r) = 1.;
                    }*/
            }

        }
    }
}

void ParamPOVM::mkMultipleUnitaries(Matrix (& u)[10], N32 wanted)
{
    double a[20];

    N32 parameters = _operatorParameters * wanted;

    for (N32 i = 0; i < parameters; ++i)
    {
        a[i] = 0.;
    }

    for (N32 i = 0; i < parameters; ++i)
    {
        nextP();

        for (N32 j = 0; j < parameters; ++j)
        {
            if (i != j)
                a[j] = (a[j] + _cp);
        }
    }

    N32 q = 0;

    for (N32 n = 0; n < wanted; ++n)
    {
        Matrix l;
        l.zeros(_states, _states);

        for (N32 r = 0; r < _states; ++r)
        {
            for (N32 c = 0; c < _states; ++c)
            {
                if (c > r)
                {
                    l(c, r) = a[q] / 4.;
                    q++;
                }
                else if (c < r)
                {
                    l(c, r) = a[q];
                    q++;
                }
            }
        }

        l /= (double) (parameters - 1);

        double s = Sin(a[q]);
        q++;
        u[n] = s * M1::uc(l);
    }

    XK(q == parameters);
}

//eof

