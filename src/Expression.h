#pragma once
#include "Essential.h"
#include "Lists.h"

#define MAX_rows 9
#define MAX_cols 4
#define MAX_vars 4

class Expression : public XList3<Expression, ExpressionList, Expression>
{
    char* _pPortaExp;
    ExpressionPtr _pOriginal;

    N32  _pMatrix[MAX_rows * MAX_cols * MAX_vars];

    N32 _RHS;
    N8  _ine;
    N8  _LHSModulo;
    N8 _rows;
    N8 _cols;
    N8 _vars;

    N32 _cMark;
    N32 _tMark;

    double _POVMValue;
    double _POVMRobustness;

    double _projectiveValue;
    double _projectiveRobustness;

    double _RealPOVMValue;
    double _RealPOVMRobustness;

    double _RealProjectiveValue;
    double _RealProjectiveRobustness;

    N32 _ProjectiveStatus;
    N32 _POVMStatus;
    N32 _RealProjectiveStatus;
    N32 _RealPOVMStatus;

    bool _varsInitialized;
    N32 _id;
    ExpressionListPtr _pOutPermutions;

public:

    void setID(N32 n)
    {
        _id = n;
    }

    N32 getID()
    {
        return _id;
    }
	void writeWitMat(ExpressionList* pL, FILE* pF, N32 wit);

    void pasteExpPath(N32 pos, char* path);

    void writeInt(const char* p, N32 n);
    void writeDouble(const char* p, double d);

    bool readInt(const char* name, N32& v, N32 d);
    bool readDouble(const char* name, double& v, N32 d);

    void readExpressionVars();

    bool getVarsInitialized()
    {
        return _varsInitialized;
    }

    N32& cMark()
    {
        return _cMark;
    }

    N32& tMark()
    {
        return _tMark;
    }

    static bool gm(Expression* p1, Expression* p2)
    {
        if (p1-> _cMark > p2->_cMark)
            return true;

        if (p1-> _cMark < p2->_cMark)
            return false;

        return p1->_tMark > p2->_tMark;
    }

    void reset()
    {

        _POVMValue = 0.;
        _POVMRobustness = 0.;

        _projectiveValue = 0.;
        _projectiveRobustness = 0.;

        _RealPOVMValue = 0.;
        _RealPOVMRobustness = 0.;

        _RealProjectiveValue = 0.;
        _RealProjectiveRobustness = 0.;

        _ProjectiveStatus = -1;
        _POVMStatus = -1;
        _RealProjectiveStatus = -1;
        _RealPOVMStatus = -1;


        _cMark = 0;
        _tMark = 0;

        _rows = 0;
        _cols = 0;
        _vars = 0;
        _RHS = 0;
        _ine = 0;

        _pPortaExp = NULL;
        _LHSModulo = 0;

        memset(_pMatrix, 0, MAX_rows * MAX_cols * MAX_vars * sizeof(N32));
    }

    Expression()
    {
        reset();
    }

    Expression(Expression& e)
    {
        reset();
        cp(e);
    }

    Expression(Expression* p)
    {
        reset();
        cp(*p);
    }

    void setLHSModulo(bool b)
    {
        _LHSModulo = b ? 1 : 0;
    }

    bool getLHSModulo()
    {
        return _LHSModulo != 0;
    }

    size_t getHashValue();

	double getRandomNoise();
	
    bool isTheSame(Expression* pX)
    {
        return isEqual(pX);
    }

    bool isTrivial();

    void lineText(char* sz, N32& pos);
    void printMatrix();
    void printInLine();
    void printExp();

    void set(N32 row, N32 col, N32 var, N32);

    N32 get(N32 row, N32 col, N32 var);
    bool vget(N32 row, N32 col, N32 var, N32& value);

    N32& ref(N32 row, N32 col, N32 var);

    N32& refA(N32 pos);

    N32 get(N32 pos)
    {
        return refA(pos);
    }

    void set(N32 pos, N32 v)
    {
        refA(pos) = v;
    }

    N32 value(N32 row, N32 col, N32 var);

    N32 getCols()
    {
        return _cols;
    }

    N32 getRows()
    {
        return _rows;
    }

    N32 getVs()
    {
        return _vars;
    }

    N32 getItems()
    {
        return _vars;
    }

    bool isEquivalent(ExpressionList* pPremutations, Expression* q);

    void operator =( Expression& e)
    {
        cp(e);
    }

    void cp(Expression& e);

    ~Expression()
    {
        if (_pPortaExp)
            free(_pPortaExp);
    }

    ExpressionPtr rCopy()
    {
        ExpressionPtr p = new Expression;
        p->cp(*this);
        return p;
    }

    ExpressionPtr mkInverse(bool bPhysical);

    N64 verifyVertex(Expression* pV);

    bool isEqual(Expression* pOther);
    bool isLHSEqual(Expression* pOther);

    bool isLessOrEqual(Expression* pOther);
    bool isBetter(Expression* pOther);
    bool isGreaterOrEqual(Expression* pOther);

    void cpBack(Expression* p);

    ExpressionPtr sortExpression();
    ExpressionPtr qsExp();

    void sortInPlace();

    Term* findTermInsertionPointInExpression(Term* pNew);

    void printEq();

    N32 getRHS() const
    {
        return _RHS;
    }

    void setRHS(N32 rhs)
    {
        _RHS = rhs;
    }

    N32 getIne() const
    {
        return _ine;
    }

    void setIne(N32 ine)
    {
        _ine = ine;
    }

    const char* ineText();

    void setPortaExp(const char* p)
    {
        if(_pPortaExp)
        {
            free(_pPortaExp);
            _pPortaExp = NULL;
        }

        if (p)
        {
            XK(sizeof(char) == 1);

            int len = strlen (p);

            _pPortaExp = (char*) malloc(len + 1);

            strcpy(_pPortaExp, p);
        }
        else
            _pPortaExp = NULL;
    }

    const char* getPortaExp()
    {
        return _pPortaExp;
    }

    Expression* getOriginalExp()
    {
        return _pOriginal;
    }

    void setOriginalExp(Expression* p)
    {
        _pOriginal = p;
    }

    void getTermsText(char* sz, N32& pos);

    void printLHS();
    void printWithRowCol();

    void incRecursive(int base);
    void inc(int base);

    Term* setBit(int nBit, bool b);

    ExpressionListPtr generateEquivalentSymmetricExpressions(ExpressionList* pRowPermutations, ExpressionList* pColPermutations, ExpressionList* pOutcomePermutions, bool fEV);
    ExpressionPtr transposeEV(Expression* pRowPerm, Expression* pColPerm, Expression** pColVar);
    ExpressionPtr transpose(Expression* pRowPerm, Expression* pColPerm, Expression** pColVar);
    ExpressionPtr findSpecialEquivalent(ExpressionList* pRowPermutations, ExpressionList* pColPermutations,  ExpressionList* pOutcomePermutations);

    static bool startColVar(Expression** pCV, N32 maxCols, N32 cols,  ExpressionList* pVarPermutations);
    static bool nextColVar(Expression** pCV, N32 maxCols, ExpressionList* pVarPermutations);

    static void tests();
    void printTeX(FILE* pF, ExpressionList* pL);
};


// eof
