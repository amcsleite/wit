#pragma once
#include "Essential.h"
#include "Lists.h"

class DIGenerator : public Counted
{
protected:
        N64 _dimensions;

        N64 _preparations;
        N64 _measurements;   
        N64 _classicStates;                   
		N64 _outcomes;
		N64 _outVars;
		N64 _useExpectedValue;
	
public:

    DIGenerator(N64 preparations, N64 measurements, N64 channelDimensions, N64 outcomes);
 
    N64 getDimensions()
    {
         return _dimensions;
    }

	N32 output(Expression& codingChoice, Expression& interpretationChoice, N64 preparation, N64 measurement, N64 outVar);

    N64 getDimensionsTotal()
    {
       return _preparations * _measurements * _outVars;
    }

    virtual void generate(ExpressionList* pWhere);

    virtual void print()
    {}
        
};

typedef SmartPtr<DIGenerator> DIGeneratorPtr;

// eof
