#include "Essential.h"
#include "Context.h"
#include "Generator.h"
#include "Strategy.h"
#include "EndGenerator.h"

void EndGenerator::generate(Context* pCtx, Strategy* pWhere)
{
    Generator* pNext = next();

    StrategyPtr pS = new Strategy(pCtx, this);
    pWhere->pushBack(pS);
    
}

// eof
