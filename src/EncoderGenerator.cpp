#include "Essential.h"
#include "Context.h"
#include "Generator.h"
#include "Strategy.h"
#include "EncoderGenerator.h"

// Describes the efect of the preparations on one of the output bits
// Examples:
// device 000 encodes 0 always
// device 001 encodes 1 if P3 pressed
// There is a total of 8 such devices

// Measurements:
// If the classic bit is zero we can have 2 ^ M possible kinds of interprerers
// for each one of these interpreters has 2 ^ M ways of interpreting the one
// that makes 2^M x 2^M = 2^2M different interpreters

void EncoderGenerator::generate(Context* pCtx, Strategy* pWhere)
{
     Generator* pNext = next();

     int p2 = pow2(_preparations);
     
     for (int preparation = 0; preparation < p2; ++preparation)
     {
        StrategyPtr pPreparation = new Strategy(pCtx, this, preparation, _preparations);
        pWhere->pushBack(pPreparation);

        
        if (pNext)
            pNext->generate(pCtx, pPreparation);  

     }
}

// eof
