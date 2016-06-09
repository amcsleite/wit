#pragma once
#include "Essential.h"
#include "Context.h"
#include "Strategy.h"

class MeasurementGenerator : public Generator
{
        
    int _measures;
    int _bits;
        
    public:

     MeasurementGenerator(int nM, int bits )
     {
           _measures = nM;
           _bits = bits;
           _dimensions = _measures;
     }

     virtual void generate(Context* pCtx, Strategy* pWhere);
};

// eof
