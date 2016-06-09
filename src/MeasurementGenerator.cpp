#include "Essential.h"
#include "Context.h"
#include "Generator.h"
#include "Strategy.h"
#include "MeasurementGenerator.h"

// identifies devices by the way they interpret the bit as a function of the selected Ms
// there are thus 2 ^ M different ways of interpreting
// which correpond to 2 ^ M alternative devices

void MeasurementGenerator::generate(Context* pCtx, Strategy* pWhere)
{
    Generator* pNext = next();


    N32 value = pWhere->getValue();
    N32 _preparations = pWhere->getPreparations();


    N32 jTotal = pow2(_bits * measures);

    for (N32 bit = 0; bit < _bits; ++_bit)
     {
        for (N32 m = 0; m < p2Measures; ++m)
        {
            for (N32 c = 0; c < prepCases; ++c)
            {

    for (N32 j = 0; j < jTotal; ++j)
    {
        N32 i = interpretation(j, bit, measure, preparations, _measures);
            
        StrategyPtr pM = new Strategy(pCtx, this, m, _measures);

        if (!bit(value, c))
        {
        }
        else
        {
        }

        pWhere->pushBack(pM);

        if (pNext)
          pNext->generate(pCtx, pM);  
     }
    
    /*for (int bit = 0; bit < _bits; ++_bit)
     {
       for (int m = 0; m < p2Measures; ++m)
        {

                
            StrategyPtr pM = new Strategy(pCtx, this, m, _measures);
            pWhere->pushBack(pM);

            if (pNext)
                pNext->generate(pCtx, pM);  
        }
        }*/
}

// eof
