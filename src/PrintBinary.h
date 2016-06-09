#include <sdtio.h>
#include <stdlib.h>
#include "Module.h"

#define U32 unsigned int

class PrintBinary : public Module
{
   Strategy *pS;
public:

    PrintBinary(Context* p, Strategy *pS) : Module(p)
    {
    }
        
    void run()
    {
            p->getBit(
       while (u)
       {
           bool b = u & 1;

           if (b)
           {
              printf(pCtx->getZeroTxt());
           }
           else
           {
              printf(pCtx->getOneTxt());
           }

           printf(" ");

           u >>= 1;
       }
    }
}

// eof
