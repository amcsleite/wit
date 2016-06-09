#pragma once
#include "Essential.h"
#include "Context.h"
#include "Strategy.h"

class Encode : public Strategy
{
   // Describes how the device encodes the nPreparations-
   // Example (for the 1 bit case): 001 means the resulting bit
   // will be 0 for P0, 0 for P1, 1 for P2
   // Others devices like 010 do it differently
   // There is a total of 8 cases
        
   int _preparations;
   int _bits;
        
public:

    Encode(int nP, int nBits) : Module(p)
    {
        _preparations = nP;
        _bits = nBits;
    }

    static U32 pow2(U32 n)
    {
        return 2 << n;
    }
        
    void run()
    {
        for (int n = 0; n < pow2(_preparations); ++n)
        {
           for (int bit = 0; bit < pow2(_bits); ++bit)
           {
                   
               StrategyPtr pS = new Strategy(n);
               _pCtx->appendStrategy(pS);
           }
        }
   }
};

// eof
