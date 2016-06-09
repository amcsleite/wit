#pragma once
#include "Essential.h"
#include "Context.h"
#include "Generator.h"

class EncoderGenerator : public Generator
{
   // Describes how the device encodes the nPreparations-
   // Example (for the 1 bit case): 001 means the resulting bit
   // will be 0 for P0, 0 for P1, 1 for P2
   // Others devices like 010 do it differently
   // There is a total of 8 cases
        
   int _preparations;
   int _bits;
        
public:

    EncoderGenerator(int nP, int nBits)
    {
        _preparations = nP;
        _bits = nBits;
        _dimensions = _preparations;
        
    }

   virtual void generate(Context* pCtx, Strategy* pWhere);
};

// eof
