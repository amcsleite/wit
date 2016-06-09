#include "Essential.h"
#include "DIGenerator.h"
#include "Expression.h"
#include "ExpressionList.h"
#include "EHash.h"

DIGenerator::DIGenerator(N64 preparations, N64 measurements, N64 classicStates, N64 outcomes)
{
    _preparations = preparations;
    _measurements = measurements;
    _classicStates = classicStates;
    _outcomes = outcomes;
    _outVars = outcomes - 1;
    _useExpectedValue = (outcomes == 2);
}

N32 DIGenerator::output(Expression& codingChoice, Expression& interpretationChoice, N64 preparation, N64 measurement, N64 outVar)
{
    // determine the output for a device that uses choice codingChoice and interpretationChoice

    N64 code = codingChoice.refA(preparation);
	N64 interpretationPosition = code * _measurements + measurement;
	
    N32 value = (outVar == interpretationChoice.refA(interpretationPosition)) ? 1 : 0;
    return value;
}

void DIGenerator::generate(ExpressionList* pWhere)
{
	EHashPtr pH = new EHash;
    printf("\n\n");

    XK(pown(2, 4) == 16);
    XK(pown(3, 2) == 9);
    XK(pown(5, 3) == 125);

    N64 preparationCases = pown(_classicStates, _preparations);
    
    N64 measurementCases = (N64) pown(_outcomes, _classicStates * _measurements);

    printf("\npC: %lld m mC: %lld pC x mC = %lld \n", preparationCases, measurementCases, preparationCases * measurementCases);

    Expression codingChoice;

    for (N64 preparationCase = 0; preparationCase < preparationCases; ++preparationCase)
    {
        Expression interpretationChoice;

        for (N64 measurementCase = 0; measurementCase < measurementCases; ++measurementCase)
        {
            ExpressionPtr pE = new Expression();

            for (N64 preparation = 0; preparation < _preparations; ++preparation)
            {
                for (N64 measurement = 0; measurement < _measurements; ++measurement)
                {
                    for (N64 outVar = 0; outVar < _outVars; ++outVar)
                    {
                        N32 out = output(codingChoice, interpretationChoice, preparation, measurement, outVar);

                        pE->set(preparation, measurement, outVar, out);
                    }
                }
            }

			 #ifdef _DEBUG
			 ilog("-------------------------------------------");
			 codingChoice.printInLine();
			 interpretationChoice.printInLine();
			 pE->printInLine();
			 ilog("-------------------------------------------");
			 #endif

			if (!pH->hLookup(pE))
            {
				pH->setAt(pE);
				XK(pH->hLookup(pE));
				pWhere->pushBack(pE);
			}

            static N64 n;
            ++n;
			
            interpretationChoice.inc(_outcomes);

            if ((n % 1000) == 0)
            {
                printf("\r%lld", n);
                fflush(stdout);
            }
        }

        codingChoice.inc(_classicStates);
    }
}

// eof
