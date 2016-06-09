#pragma once
#include "Essential.h"
#include "Lists.h"
#include "XHash.h"

//class EHash : public  XHash<Expression, Expression,  67867979>
//class EHash : public  XHash<Expression, Expression,  32452867>
//class EHash : public  XHash<Expression, Expression,  10000019>
//class EHash : public  XHash<Expression, Expression, 353868013>


class EHash : public  XHash<Expression, Expression, 353868013>
{
public:
    void setAt(Expression* p)
    {
        Expression** pPos = hAt(p);

        if (*pPos != NULL)
            return;

        *pPos = p;

#ifdef _DEBUGO
        Expression* pL = hLookup(p);

        XK(p && pL && p->isEqual(pL));

        ilog(".....................>");

        if (p)
            pL->printMatrix();

        if (pL)
            pL->printMatrix();

        ilog("<......................");

#endif
    }
};

typedef SmartPtr<EHash> EHashPtr;

// eof
