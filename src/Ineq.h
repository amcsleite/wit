#pragma once
#include "Essential.h"
#include "Binary.h"

class Term;
class Ineq;

typedef SmartPtr<Term> TermPtr;

const U32 TERM_ineq = 1;
const U32 TERM_list = 2;
const U32 TERM_item = 3;

class Ineq : public XList2<Term>
{
    U32 _type;
    N32 _value;
    N32 _RHS;
    N32 _ine;
    char* _pOriginalExp;

public:

    Term(U32 type = 0)
    {
        _type = type;
        _value = 0;
        _RHS = 0;
        _ine = 0;
        _pOriginalExp = NULL;
        _type = type;
    }

    ~Term()
    {
        if (_pOriginalExp)
            free(_pOriginalExp);
    }

    bool isType(U32 u)
    {
        return u == _type;
    }

    TermPtr copy()
    {
        TermPtr p = new Term();
        p->_type = _type;
        p->_value = _value;
        p->_RHS = _RHS;
        p->_ine = _ine;

        p->setOriginalExp(_pOriginalExp);
        return p;
    }

    TermPtr rCopy()
    {
        TermPtr pCopy = copy();

        for (Term* p = head(); p; p = p->next())
        {
            pCopy->pushBack(p->rCopy());
        }

        return pCopy;
    }

	TermPtr mkInverse();

    bool isEquationEqual(Term* pOther);
	bool isEquationLessOrEqual(Term* pOther)

    void print(FILE* pF);
    void printHuman(FILE* pF);

	TermPtr sortInequation();
	TermPtr sortList();

	TermPtr compact();

    Term* findTermInsertionPointInInequation(Term* pNew);
    Term* findInequationInsertionPointInList(Term* pNew);

    N32 getValue()
    {
        return _value;
    }

    N32 getRHS()
    {
        return _RHS;
    }

    void setValue(N32 value)
    {
        _value = value;
    }

    void setRHS(N32 rhs)
    {
        _RHS = rhs;
    }

    N32 getIne()
    {
        return _ine;
    }

    void setIne(N32 ine)
    {
        _ine = ine;
    }

    const char* ineText();

    void setOriginalExp(const char* p)
    {
        if (p)
        {
            XK(sizeof(char) == 1);

            int len = strlen (p);
            _pOriginalExp = (char*) malloc(len + 1);

            strcpy(_pOriginalExp, p);
        }
        else
            _pOriginalExp = NULL;
    }

    const char* getOriginalExp()
    {
        return _pOriginalExp;
    }
};


// eof
