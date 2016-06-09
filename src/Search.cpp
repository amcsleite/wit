#include <iostream>
#include <armadillo>
#include "Util.h"
#include "UtilMath.h"
#include "Search.h"
#include "WriteVar.h"
#include "ReadVar.h"

Search::Search(Expression* pE, bool fProjective, bool fComplex, N32 preparations, N32 measurements, N32 states, N32 outcomes)
{
    _fComplex = fComplex;
    _preparations = preparations;
    _measurements = measurements;
    _states = states;
    _outcomes = outcomes;
    _vars = outcomes - 1;

    if (!fProjective  || _states != outcomes)
        _pOutPermutations = Util::quickPerm(outcomes);
    else
    {
        _pOutPermutations = new ExpressionList();
        ExpressionPtr p = new Expression;

        for(U32 i = 0; i < _outcomes; i++)
            p->refA(i) = i + 1;

		_pOutPermutations->pushBack(p);
    }


#ifdef _DEBUG
    _pOutPermutations->print();
#endif

    _bestQuantumValue = 0.;
    _cp = 0.;
    _curP = -1;

    _pMinimizer = NULL;
    _pExp = pE;

    _fProjective = fProjective;

    if (fProjective)
    {
        ilog("using projective operators only");
    }
    else
    {
        ilog("using POVMs");
    }

    UtilMath::initGellM(_states);
    UtilMath::printGellM();

    _bUseExpectedValue = outcomes == 2;

    N32 d = states;

    if (fProjective)
    {
        _stateParameters = (d * d - d) + 1;
        _operatorParameters = d * d - d + 1; // used in alpha UN matrix

        if (!fComplex)
        {
            _stateParameters -= (d*d - d) / 2;
            _operatorParameters -= (d*d - d) / 2;
        }

        _totalOptimizationVars = (_preparations - 1) * _stateParameters + _measurements * _operatorParameters;
    }
    else
    {
        _stateParameters = (d * d - d) + 1;
        _operatorParameters = d * d - d + 1 + 1;

        if (!fComplex)
        {
            _stateParameters -= (d ^ 2 - d) / 2;
            _operatorParameters -= (d ^ 2 - d) / 2;
        }

        _totalOptimizationVars = (_preparations - 1) * _stateParameters + _measurements * _vars * _operatorParameters;
    }

    // the total number of matrix in the POVM
    _quantumStates.resize(preparations);
    _measurementOperators.resize(_measurements * outcomes);
    _bestQuantumStates.resize(preparations);
    _bestMeasurementOperators.resize(_measurements * outcomes);
}

Matrix Search::mulgm(N32 k)
{
    XK(k >= 1);

    if (k == 1)
    {
        nextP();
        Matrix m  = Sin(_cp) * UtilMath::gellM(0);
        m += Cos(_cp) * UtilMath::gellM(1);
        return m;
    }
    else
    {
        nextP();
        Matrix m =  Cos(_cp) * UtilMath::gellM(k);
        m += Sin(_cp) * mulgm(k - 1);
        return m;
    }
}

void Search::mkStates()
{
    state(0) = UtilMath::identity(_states) * 1e-15;
    state(0)(0, 0) = 1.;

    for (N32 p = 1; p < _preparations; ++p)
    {
        Matrix l;
        prepareLambda(l);

        state(p) = UtilMath::identity(_states) * 1e-15;
        state(p)(1, 1) = 1.;

        Matrix u = M1::uc(l);
        Matrix ut = u.t();

        state(p) = u * state(p) * ut;

        XK(EQ(trace(state(p)), 1.0));
        XK(EQ(trace(state(p) * state(p)), 1.0));
        XK(isPositive(state(p)));
    }
}

void Search::mkStatesGM()
{
    for (N32 p = 0; p < _preparations; ++p)
    {
        if (_states == 2)
            state(p) = 0.5 * ( UtilMath::identity(_states) + mulgm(UtilMath::nGellM() - 1) );
        else
        {
            state(p) = 1. / _states *
                       (
                           UtilMath::identity(_states)
                           + sqrt( (_states * (_states - 1.)) / 2.)  * mulgm(UtilMath::nGellM() - 1)
                       );
        }

        //state(p).print();
        XK(EQ(trace(state(p)), 1.0));

        //(state(p)*state(p)).print();
        XK(EQ(trace(state(p)*state(p)), 1.0));
        XK(isPositive(state(p)));
    }
}

void Search::prepareLambda(Matrix& l)
{
    l.zeros(_states, _states);

    for (N32 r = 0; r < _states; ++r)
    {
        for (N32 c = 0; c < _states; ++c)
        {
            if (c > r)
            {
                nextP();
                l(r, c) = _cp;
            }
            else if (c < r)
            {
                if (_fComplex)
                {
                    nextP();
                    l(r, c) = _cp;
                }
            }
            else
            {
                if (c == 0)
                {
                    nextP();
                    l(0, 0) = _cp;
                }
            }

        }
    }
}

bool Search::isPositive(const Matrix& m)
{
    vec eigval;
    Matrix eigvec;

    eig_sym(eigval, eigvec, m);

    for (N32 n = 0; n < eigval.n_elem; ++n)
    {
        double v = eigval(n);

        if (v < -1.e-10)
            return false;
    }

    return true;
}

bool Search::isComplete(N32 m)
{
    Matrix sum;

    for (N32 o = 0; o < _outcomes; ++o)
    {
        if (o == 0)
            sum = oper(m, o);
        else
            sum += oper(m, o);
    }

#ifdef _DEBUGO
    printf("\n sum **********************************\n");
    sum.print();
#endif

    XK( UtilMath::isNull(sum - UtilMath::identity(_states)) );

    return UtilMath::isNull(sum - UtilMath::identity(_states));
}

void Search::mkProjectiveSearch()
{
    for (N32 m = 0; m < _measurements; ++m)
    {
        N32 o = 0;

        Matrix rest = UtilMath::identity(_states);

        Matrix l;
        prepareLambda(l);

        Matrix u = M1::uc(l);
        Matrix ut = u.t();

        for (; o < _states - 1 && o < _outcomes - 1 ; ++o)
        {
            Matrix op;
            op.zeros(_states, _states);

            op(o, o) = 1.;

            oper(m, getPermutedValue(m, o)) = u * op * ut;
            rest -= oper(m, getPermutedValue(m, o));
        }

        oper(m, getPermutedValue(m, o)) = rest;

        ++o;

        for (; o < _outcomes; ++o)
            oper(m, getPermutedValue(m, o)).zeros(_states, _states);


#ifdef _DEBUG

        for (N32 o = 0; o < _outcomes; ++o)
        {
            //printPOVM();
            XK(isPositive(oper(m, o)));
        }

        XK(isComplete(m));
#endif
    }
}

void Search::mkMultipleUnitaries(Matrix (& u)[10], N32 wanted)
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
                    l(r, c) = a[q] / 4.;
                    q++;
                }
                else if (c < r)
                {
                    if (_fComplex)
                    {
                        l(r, c) = a[q];
                        q++;
                    }
                }
                else
                {
                    if (c == 0)
                    {
                        l(0, 0) = a[q];
                        q++;
                    }
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

void Search::mkGeneralSearch()
{
    for (N32 m = 0; m < _measurements; ++m)
    {
        Matrix rest = UtilMath::identity(_states);

        Matrix u[10];
        mkMultipleUnitaries(u, _vars);

        N32 o = 0;

        for (; o < _outcomes; ++o)
        {
            if (o == _outcomes - 1)
            {
                oper(m, getPermutedValue(m, o)) = rest;
                o++;
                break;
            }

            Matrix ixi;
            //ixi = UtilMath::identity(_states) *  1e-15;
            ixi.zeros(_states, _states);
            N32 o1 = o % _states;

            ixi(o1, o1) = 1.;

            Matrix& un = u[o];
            Matrix unt = u[o].t();

            Matrix op =  un * ixi * unt;

            Matrix newRest = rest - op;

            if (!isPositive(newRest))
            {
                oper(m, getPermutedValue(m, o)) = rest;
                o++;
                break;
            }

            rest = newRest;
            oper(m, getPermutedValue(m, o)) = op;
        }

        for (; o < _outcomes; ++o)
        {
            oper(m, getPermutedValue(m, o)).zeros(_states, _states);
        }

#ifdef _DEBUG

        for (N32 o = 0; o < _outcomes; ++o)
        {
            XK(isPositive(oper(m, o)));
        }

        XK(isComplete(m));


#endif
    }
}

/*
void Search::mkGeneralSearch()
{
    for (N32 m = 0; m < _measurements; ++m)
    {
        Matrix sum;

        bool setZeros = false;

        bool first = true;

        for (N32 o = 0; o < _outcomes; ++o)
        {
            Matrix l;
            prepareLambda(l);

            Matrix u = M1::uc(l);

            Matrix op;
            op.zeros(_states, _states);
            op(o, o ) = 1.;

            nextP();
            double s = Sin(_cp);
            op  = s * s * u.t() * op * u;

            if (first)
            {
                sum = op;
                first = false;
            }
            else
                sum += op;

            oper(m, getPermutedValue(m, o)) = op;
        }

        Matrix iSum = sum.i();
        XK(isPositive(iSum));

        for (N32 o = 0; o < _outcomes; ++o)
        {
            oper(m, o) *= iSum;
        }

#ifdef _DEBUG

        XK(isComplete(m));

        for (N32 o = 0; o < _outcomes; ++o)
        {
            XK(isPositive(oper(m, o)));
        }

#endif
    }
}
*/
/*
double Search::getPreparationParameter(N32 p, N32 n)
{
    XK( p < getPreparations());
    XK( n < _stateParameters);

    return gsl_vector_get(_pV, p * _stateParameters + n);
}

double Search::getMeasurementParameter(N32 m, N32 o, N32 param)
{
    XK( m < getMeasurements() );
    XK( o < getVars() );
    XK( param < _operatorParameters);

    return
        gsl_vector_get(_pV, getPreparations() * _stateParameters
                       + m * getVars() * _operatorParameters
                       + o * _operatorParameters + param);
}
*/
bool Search::isProjective(N32 m)
{
    for (N32 i = 1; i < _outcomes; ++i)
    {
        for (N32 j = 0; j <= i; ++j)
        {
            const Matrix&  a = oper(m, i);
            const Matrix&  b = oper(m, j);

            if (j != i)
            {
                if (!UtilMath::isNull( a * b ) )
                    return false;

                if (!EQ( trace( a * b ), 0.))
                {
                    return false;
                }
            }
            else
            {
                if (!UtilMath::isNull(  a * b - a  ))
                    return false;
            }
        }
    }

    return true;
}

bool Search::isProjectiveDump(N32 m)
{
    for (N32 o = 1; o < _outcomes; ++o)
    {
        for (N32 j = 0; j < o; ++j)
        {
            if (!EQ( trace(oper(m, o) * oper(m, j)), 0.))
            {
                ilog ("POVM not projective measurement: %d  (%d, %d) = %f", m + 1, o + 1, j + 1, trace(oper(m, o) * oper(m, j)));
                ilog("--------- after -----------------");
                printPOVM();
                return false;
            }
        }
    }

    return true;
}

void Search::mkProjective()
{
#ifdef _DEBUGO
    ilog("--------- original -----------------");
    printPOVM();
#endif

    for (N32 m = 0; m < _measurements; ++m)
    {
        double n0 = norm(oper(m, 0), "fro");
        oper(m, 0) /= n0;

        for (N32 o = 1; o < _outcomes; ++o)
        {
            for (N32 j = 0; j < o; ++j)
            {
                double nm = norm(oper(m, j), "fro");

                if (nm != 0.)
                {
                    Matrix u = oper(m, j) / nm; // fro --> Frobenius norm

                    //subtract components along the ones already in
                    oper(m, o) -= trace(oper(m, j) * oper(m, o)) * u;
                }
            }

            double nt = norm(oper(m, o), "fro");

            if (!EQ(nt, 0.))
                oper(m, o) /= nt;
        }
    }
}

double Search::evaluateSimple()
{
    double result = 0.;

    for (N32 preparation = 0; preparation < getPreparations(); ++preparation)
    {
        for (N32 measurement = 0; measurement < getMeasurements(); ++measurement)
        {
            for (N32 var = 0; var < getVars(); ++var)
            {
                double wi = (double) getExp()->value(preparation, measurement, var);
                result += wi * trace(state(preparation) * oper(measurement, var)).real();
            }
        }
    }

    return result;
}
/*
double Search::evaluatePermuting()
{
    double result = 0.;

    for (N32 preparation = 0; preparation < getPreparations(); ++preparation)
    {
        for (N32 measurement = 0; measurement < getMeasurements(); ++measurement)
        {
            double dBest = 0.;
            bool fFirst = true;

            for (Expression* p = _pOutPermutations->head(); p; p = p->next())
            {
                double d = 0.;

                for (N32 var = 0; var < getVars(); ++var)
                {
                    N32 permutedVar = p->get(var) - 1;
                    //printf ("\n%d ---->%d", var, permutedVar);
                    XK(permutedVar >= 0 && permutedVar < getOutcomes());

                    N32 wi = getExp()->value(preparation, measurement, var);
                    d += wi * trace(state(preparation) * oper(measurement, permutedVar)).real();
                }

                if (fFirst || d > dBest)
                {
                    dBest = d;
                    fFirst = false;
                }
            }

            result += dBest;
        }
    }

    return result;
}
*/

double Search::fun(const gsl_vector* pV)
{
    _pV = pV;

    initP();

    mkStates();
    //mkStatesGM();

    if (_fProjective)
        mkProjectiveSearch();
    else
        mkGeneralSearch();

    XK(_curP + 1 == _totalOptimizationVars); // _curP is the one being used not the count

    //if (_fProjective)
    //    mkProjective();

#ifdef _DEBUG

    if (_fProjective)
    {
        for (N32 m = 0; m < _measurements; ++m)
        {
            XK(isProjective(m));
            XK(isComplete(m));
        }
    }

    for (N32 m = 0; m < _measurements; ++m)
    {
        XK(isComplete(m));
    }

    //printPOVM();

#endif

    double result = 0.;

    if (getUseExpectedValue())
    {
        for (N32 preparation = 0; preparation < getPreparations(); ++preparation)
        {
            for (N32 measurement = 0; measurement < getMeasurements(); ++measurement)
            {
                double t = trace(state(preparation) * oper(measurement, 0)).real();

                result += getExp()->value(preparation, measurement, 0) * (2.*t - 1.);
            }
        }
    }
    else
    {
        result = evaluateSimple();
    }

    return -result;
}

void Search::showResult(gsl_multimin_fminimizer* pMinimizer)
{
    double size = gsl_multimin_fminimizer_size (pMinimizer);

    ilog("iterations: %3d result: %12.10f %5.10f at:", _iter, pMinimizer->fval, size);

#ifdef _DEBUG__SHOW_STATES
    ilog("preparation params ");

    for (N32 n = 0; n < getPreparations(); ++n)
    {
        ilog("P: %d ", n);

        for (N32 p = 0; p < _stateParameters; ++p)
            log(" %12.7f:%12.7f", gsl_vector_get(pMinimizer->x, n * _stateParameters + p) );
    }

    N32 totalPrep = getPreparations() * _stateParameters;

    ilog("measurement params ");

    for (N32 m = 0; m < getMeasurements(); ++m)
    {
        N32 base = totalPrep + m * getVars() * _operatorParameters;

        ilog("M: %d ", m);

        for (N32 v = 0; v < getVars() ; ++v)
        {
            for (N32 p = 0; p < _operatorParameters; ++p)
            {
                N32 pos = totalPrep + m * getVars() * _operatorParameters + p;

                log(" %12.7f:%12.7f", gsl_vector_get(pMinimizer->x, v) );
            }
        }
    }

#endif
}

void Search::printStatus()
{
    double size = gsl_multimin_fminimizer_size (_pMinimizer);

    ilog ("result for:");
    ilog("");
    _pExp->printEq();
    ilog("");

    ilog("iterations: %3d result: %12.10f %5.10f at:", _iter, _pMinimizer->fval, size);

#ifdef _DEBUG__SHOW_STATES

    cout.precision(15);

    for (N32 n = 0; n < getPreparations(); ++n)
    {
        ilog("---- preparation: %d -----\n", n + 1);
        state(n).raw_print(g_logStream.getStream());
    }

    for (N32 m = 0; m < getMeasurements(); ++m)
    {
        ilog("measurement: %d\n", m + 1);

        ilog( isProjective(m) ? "measurement is projective" : "measurement is not projective");

        for (N32 v = 0; v < getOutcomes() ; ++v)
        {
            ilog("---- outcome operator: %d ----\n", v + 1);
            ilog("Fro. norm %f\n", norm(oper(m, v), "fro"));
            oper(m, v).raw_print(g_logStream.getStream());
        }
    }

#endif
}

void Search::printPOVM()
{
    for (N32 m = 0; m < getMeasurements(); ++m)
    {
        ilog("measurement: %d\n", m + 1);

        for (N32 v = 0; v < getOutcomes() ; ++v)
        {
            ilog("---- outcome operator: %d ----\n", v + 1);
            ilog("Fro. norm %f\n", norm(oper(m, v), "fro"));
            oper(m, v).raw_print(g_logStream.getStream());
        }
    }
}

void Search::showIter(gsl_multimin_fminimizer* pMinimizer)
{
    showResult(pMinimizer);
}

double Search::repeat(N32 repetitions, N32 maxIterations, bool& valid)
{
    /*ExpressionPtr pP = new Expression;
    pP->refA(0) = 1;
    pP->refA(1) = 2;
    pP->refA(2) = 3;*/

    _bestQuantumValue = 0.;

    valid = false;

    for (N32 r = 0; r < 5; ++r)
    {
        Expression::startColVar(_pCV, MAX_cols, getMeasurements(), _pOutPermutations);

#ifdef _DEBUG
        N32 testCount = 0;
#endif

        while(true)
        {
            double result;
            bool b = run(maxIterations, false, result);

            if (b && (!valid || result > _bestQuantumValue))
            {
                _bestQuantumValue = result;

                for (N32 p = 0; p < getPreparations(); ++p)
                    bestState(p) = state(p);

                for (N32 m = 0; m < getMeasurements(); ++m)
                    for (N32 o = 0; o < getOutcomes(); ++o)
                        bestOper(m, o) = oper(m, o);

                if (!valid)
                    valid = true;
            }

#ifdef _DEBUG
            ++testCount;
#endif

            if(!Expression::nextColVar(_pCV, getMeasurements(), _pOutPermutations))
                break;
        }

        //XK(testCount == 36); test for case 3  --> 3! ^ 2
    }

    return _bestQuantumValue;
}

bool Search::run(N32 maxIterations, bool fDump, double& result)
{
    bool fReturn = false;

    result = 0.;

    // starting point
    gsl_vector* pX = gsl_vector_alloc(_totalOptimizationVars);

    if (!pX)
    {
        XK(false);
        Util::errorMsg("error allocating starting point");
        exit(EXIT_FAILURE);
        return false;
    }

    gsl_vector_set_all (pX, Util::rDouble());

    // set initial step sizes to 1
    gsl_vector* pStepSizes = gsl_vector_alloc (_totalOptimizationVars);

    if (!pStepSizes)
    {
        XK(false);
        Util::errorMsg("error allocating step sizes");
        exit(EXIT_FAILURE);
        return false;
    }

    gsl_vector_set_all (pStepSizes,  M_PI_4 / 4. );

    // initialize method and iterate
    gsl_multimin_function minex_func;
    minex_func.n = _totalOptimizationVars;
    minex_func.f = staticFun;
    minex_func.params = this;

    _pMinimizer = gsl_multimin_fminimizer_alloc(gsl_multimin_fminimizer_nmsimplex2rand, _totalOptimizationVars);

    if (!_pMinimizer)
    {
        XK(false);
        Util::errorMsg("error allocating minimizer");
        exit(EXIT_FAILURE);
        return false;
    }

    gsl_multimin_fminimizer_set(_pMinimizer, &minex_func, pX, pStepSizes);

    _iter = 0;

    _pExp->printExp();

    while (_iter < maxIterations * 32)
    {
        _iter++;
        int status = gsl_multimin_fminimizer_iterate(_pMinimizer);

        if (status)
        {
            ilog ("iterate failed");
            XK(false);
            break;
        }

        double size = gsl_multimin_fminimizer_size(_pMinimizer);

        status = gsl_multimin_test_size (size, 0.0000000001);

        if (status == GSL_SUCCESS)
        {
            result = -_pMinimizer->fval;

#ifdef _DEBUG
            ilog ("converged OK");
            printStatus();
#endif

            fReturn = true;
            break;
        }

        //showResult(pMinimizer);

        if(status == GSL_CONTINUE)
        {
            if (fDump)
                showIter(_pMinimizer);

            continue;
        }
        else
        {
            XK(false);
            Util::errorMsg("error optimizer");
            break;
        }
    }

    if (!fReturn)
    {
        ilog ("didn't converge");

        if (fDump)
            run(10, true, result);
    }

    gsl_vector_free(pX);
    gsl_vector_free(pStepSizes);
    gsl_multimin_fminimizer_free (_pMinimizer);
    _pMinimizer = NULL;

    return fReturn;
}

// static
double Search::staticFun(const gsl_vector* pV, void* params)
{
    Search* p = (Search*) params;
    return p->fun(pV);
}

void Search::tests(void)
{
    Matrix lambda = UtilMath::sigma(1);

    lambda.print();

    Matrix m = M1::uc(lambda);

    m.print();
}

void Search::saveMat(const char* pName, Expression* p)
{
    char szPath[1024];

    p->pasteExpPath(0, szPath);
    WriteVar w(szPath, pName);

    w.ws("\n%%function [] = wResult()");

    w.ws("\n\n%% --- %d preparations %d measurements %d states %d outcomes ---", getPreparations(), getMeasurements(), getStates(), getOutcomes());

    w.ws("\nresult = %f;", _bestQuantumValue);
    w.ws("\npreparations = %d;", getPreparations());
    w.ws("\nmeasurements = %d;",  getMeasurements());
    w.ws("\nstates = %d;", getStates());
    w.ws("\noutcomes = %d;", getOutcomes());
    w.ws("\nclassicBound = %d;", p->getRHS());

    w.ws("\n");

    w.ws("\n\n%% --- w(p, m, o) ---");

    for (N32 v = 0; v < p->getVs(); ++v)
    {
        w.ws("\nw(:,:,%d) = [", v + 1);

        for (N32 r = 0; r < p->getRows(); ++r)
        {
            for (N32 c = 0; c < p->getCols(); ++c)
            {
                N32 value = p->get(r, c, v);

                w.ws("%2d", value);

                if(c != p->getCols() - 1)
                    w.ws(",");
            }

            if(r != p->getRows() - 1)
                w.ws("; ");
        }

        w.ws("]; ");
    }

    w.ws("\n\n%% --- preparations ---");

    for (N32 p = 0; p < getPreparations(); ++p)
    {
        w.ws("\nP{%d} = [", p + 1);

        const Matrix& s = bestState(p);

        for (N32 r = 0; r < s.n_rows; ++r)
        {
            w.ws("\n ");

            for (N32 c = 0; c < s.n_cols; ++c)
            {
                w.ws("%f%+fi", s(r, c).real(), s(r, c).imag());

                if (c != s.n_cols - 1)
                    w.ws(", ");
            }

            if (r != s.n_rows - 1)
                w.ws("; ");
        }

        w.ws("\n];\n");
    }

    w.ws("\n\n%% --- measurements ---");

    for (N32 m = 0; m < getMeasurements(); ++m)
    {
        for (N32 v = 0; v < getOutcomes() ; ++v)
        {
            w.ws("\nM{%d,%d} = [", m + 1, v + 1);

            const Matrix& op = bestOper(m, v);

            for (N32 r = 0; r < op.n_rows; ++r)
            {
                w.ws("\n ");

                for (N32 c = 0; c < op.n_cols; ++c)
                {
                    w.ws("%f%+fi", op(r, c).real(), op(r, c).imag());

                    if (c != op.n_cols - 1)
                        w.ws(", ");
                }

                if (r != op.n_rows - 1)
                    w.ws("; ");
            }

            w.ws("\n];\n");

            vec eigval;
            Matrix eigvec;

            eig_sym(eigval, eigvec, op);

            w.ws("\neigM{%d,%d} = [", m + 1, v + 1);

            for (N32 j = 0; j < eigval.n_elem; ++j)
            {
                w.ws("%.15f", eigval(j));

                if (j != eigval.n_elem - 1)
                    w.ws(", ");
            }

            w.ws("];\n");
            // trace of square
            w.ws("\ntr2M{%d,%d} = %f;\n", m + 1, v + 1, trace(op * op));
            w.ws("\nrankM{%d,%d} = %d;\n", m + 1, v + 1, arma::rank(op, 1.e-5));
        }
    }

    w.ws("\n%%end");
}

//eof
