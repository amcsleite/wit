#pragma once
#include "Essential.h"
#include "Context.h"
#include "Generator.h"

class EndGenerator : public Generator
{
   // used to mark the leaves
        
public:

    EndGenerator(int nP, int nBits)
    {
    }

   virtual void generate(Context* pCtx, Strategy* pWhere);
   virtual void print()
   {
           printf("\r\n");
   }

};

// eof
