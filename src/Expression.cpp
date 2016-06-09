#include <linux/limits.h>
#include "Essential.h"
#include "Expression.h"
#include "Exception.h"
#include "WriteVar.h"
#include "ReadVar.h"

void Expression::writeWitMat(ExpressionList* pL, FILE* pF, N32 wit)
{
    pL->readListVars();

    for (N32 v = 0; v < getVs(); ++v)
    {
        fprintf(pF, "\n a{%d}(:, :, %d) = [", wit, v + 1);

        for (N32 r = 0; r < getRows(); ++r)
        {
            for (N32 c = 0; c < getCols(); ++c)
            {
                N32 value = get(r, c, v);

                fprintf(pF, "%2d", value);

                if(c != getCols() - 1)
                    fprintf(pF, ",");
            }

            if(r != getRows() - 1)
                fprintf(pF, "; ");
        }

        fprintf(pF, "]; ");
    }

    fprintf(pF, "\n c{%d} = %d;\n", wit, getRHS());
}

void Expression::pasteExpPath(N32 pos, char* path)
{
    pos += sprintf(path + pos, "wit/%d", _id);
}

bool Expression::readInt(const char* p, N32& v, N32 d)
{
    char szPath[1024];
    pasteExpPath(0, szPath);
    ReadVar r(szPath, p);

    return r.readInt(v, d);
}

bool Expression::readDouble(const char* p, double& v, N32 d)
{
    char szPath[1024];
    pasteExpPath(0, szPath);

    ReadVar r(szPath, p);

    return r.readDouble(v, d);
}

void Expression::writeInt(const char* p, N32 n)
{
    char szPath[1024];
    pasteExpPath(0, szPath);

    WriteVar w(szPath, p);

    w.writeInt(n);
}

void Expression::writeDouble(const char* p, double d)
{
    char szPath[1024];
    pasteExpPath(0, szPath);

    WriteVar w(szPath, p);

    w.writeDouble(d);
}

bool Expression::isTrivial()
{
    N32 nonZeroTerms = 0;

    for (N32 r = 0; r < _rows; ++r)
    {
        for (N32 c = 0; c < _cols; ++c)
        {
            for (N32 v = 0; v < _vars; ++v)
            {
                if (0 != value(r, c, v))
                    ++nonZeroTerms;

                if (nonZeroTerms > 2)
                    return false;
            }
        }
    }

    XK(nonZeroTerms <= 2);
    return true;
}

double Expression::getRandomNoise()
{
    N32 n = 0;
    double total = 0.;

    for (N32 r = 0; r < _rows; ++r)
    {
        for (N32 c = 0; c < _cols; ++c)
        {
            for (N32 v = 0; v < _vars; ++v)
            {
                total += value(r, c, v);
                ++n;
            }
        }
    }

    total /= n;
    return total;
}

void Expression::cp(Expression& e)
{
    reset();

    _id = e._id;
    _RHS = e._RHS;
    _ine = e._ine;
    _LHSModulo = e._LHSModulo;
    _cMark = e._cMark;
    _tMark = e._tMark;

    setPortaExp(e._pPortaExp);
    setOriginalExp(e._pOriginal);

    for (N32 r = 0; r < e._rows; ++r)
    {
        for (N32 c = 0; c < e._cols; ++c)
        {
            for (N32 v = 0; v < e._vars; ++v)
            {
                set(r, c, v, e.get(r, c, v));
            }
        }
    }
}

void Expression::inc(int base)
{
    N32 n = 0;

    while(true)
    {
        N32& v = refA(n);

        ++v;

        if (v < base)
            break;

        v = 0;
        ++n;
    }
}

bool Expression::isLessOrEqual(Expression* pExp)
{
    for (N32 r = 0; r < _rows; ++r)
    {
        for (N32 c = 0; c < _cols; ++c)
        {
            for (N32 v = 0; v < _vars; ++v)
            {
                N32 vThis = get(r, c, v);
                N32 vOther = pExp->get(r, c, v);


                if (vOther < vThis)
                    return true;

                if (vOther > vThis)
                    return false;
            }
        }
    }

    return true;
}

bool Expression::isBetter(Expression* pExp)
{
    for (N32 r = _rows - 1  ; r >= 0; --r)
    {
        for (N32 c = _cols - 1; c >= 0 ; --c)
        {
            for (N32 v = _vars - 1; v >= 0; --v)
            {
                N32 vThis = get(r, c, v);
                N32 vOther = pExp->get(r, c, v);

                if (vOther != vThis)
                {
                    if (vOther == 0)
                        return true;

                    if (vThis == 0)
                        return false;

                    if (vOther < vThis)
                        return true;

                    if (vOther > vThis)
                        return false;
                }
            }
        }
    }

    return true;
}

bool Expression::isGreaterOrEqual(Expression* pExp)
{
    for (N32 r = 0; r < _rows; ++r)
    {
        for (N32 c = 0; c < _cols; ++c)
        {
            for (N32 v = 0; v < _vars; ++v)
            {
                N32 vThis = get(r, c, v);
                N32 vOther = pExp->get(r, c, v);

                if (vThis > vOther)
                    return true;

                if (vThis < vOther)
                    return false;
            }
        }
    }

    return true;
}

size_t Expression::getHashValue()
{
    size_t h = getIne() * 10000 + getRHS() * 11000;

    for (N32 r = 0; r < _rows; ++r)
    {
        for (N32 c = 0; c < _cols; ++c)
        {
            for (N32 v = 0; v < _vars; ++v)
            {
                N32 val = get(r, c, v);

                size_t ki = val * (1000 * r + 100 * c + v);
                size_t highorder = h & 0xf8000000 ;

                h = h << 5;
                h = h ^ (highorder >> 27);
                h = h ^ ki;
            }
        }
    }

    return h;
}

bool Expression::isLHSEqual(Expression* pExp)
{
    for (N32 r = 0; r < _rows; ++r)
    {
        for (N32 c = 0; c < _cols; ++c)
        {
            for (N32 v = 0; v < _vars; ++v)
            {
                N32 vThis = get(r, c, v);
                N32 vOther = pExp->get(r, c, v);

                if (vThis != vOther)
                    return false;
            }
        }
    }

    return true;
}

bool Expression::isEqual(Expression* pOther)
{
    if (_rows != pOther->_rows || _cols != pOther->_cols || _vars != pOther->_vars)
        return false;

    if (getIne() != pOther->getIne())
        return false;

    if (getRHS() != pOther->getRHS())
        return false;

    if (!isLHSEqual(pOther))
        return false;

    return true;
}

const char* Expression::ineText()
{
    if (_ine == -2)
        return "<";
    else if (_ine == -1)
        return "<=";
    else if (_ine == 0)
        return "=";
    else if (_ine == 1)
        return ">=";
    else if (_ine == 2)
        return ">";
    else
        return "";
}

void Expression::printLHS()
{
    printInLine();
}

void Expression::getTermsText(char* sz, N32& pos)
{
    lineText(sz, pos);
    pos += sprintf(sz + pos, "%s %2s %2d [%d]", _LHSModulo ? "|" : " ", ineText(), getRHS(), cMark());
}

// static
void Expression::tests()
{
    Expression b1, b2, b3;

    for (int n = 0; n < 16; ++n)
    {
        b3.printLHS();
        b3.inc(2);
    }

    b1.inc(2);
    b1.inc(2);

    b2.inc(2);
    b2.inc(2);

    printf("-----\n");
    XK(b1.isEqual(&b2));
    XK(b2.isEqual(&b1));

    b1.inc(2);

    b1.printLHS();
    b2.printLHS();

    XK(b1.isGreaterOrEqual(&b2));
    XK(!b2.isGreaterOrEqual(&b1));
}

void Expression::set(N32 row, N32 col, N32 var, N32 v)
{
    if( row < 0 || col < 0 || var < 0 || row >= MAX_rows || col >= MAX_cols || var >= MAX_vars)
    {
        XK(false);
        throw Exception("maximum dimension of 32 exceeded");
    }

    if (row >= _rows)
        _rows = row + 1;

    if (col >= _cols)
        _cols = col + 1;

    if (var >= _vars)
        _vars = var + 1;

    _pMatrix[ row * MAX_cols * MAX_vars + col * MAX_vars + var] = v;
}

N32 Expression::get(N32 row, N32 col, N32 var)
{
    XK(row < MAX_rows);
    XK(col < MAX_cols);
    XK(var < MAX_vars);

    return _pMatrix[ row * MAX_cols * MAX_vars + col * MAX_vars + var];
}

bool Expression::vget(N32 row, N32 col, N32 var, N32& value)
{
    if (row < _rows && col < _cols && var < _vars)
    {
        value = _pMatrix[ row * MAX_cols * MAX_vars + col * MAX_vars + var];
        return true;
    }

    value = 0;

    return false;
}

N32& Expression::ref(N32 row, N32 col, N32 var)
{
    XK(row < MAX_rows);
    XK(col < MAX_cols);
    XK(var < MAX_vars);

    if (row >= _rows)
        _rows = row + 1;

    if (col >= _cols)
        _cols = col + 1;

    if (var >= _vars)
        _vars = var + 1;

    return _pMatrix[ row * MAX_cols * MAX_vars + col * MAX_vars + var];
}

N32& Expression::refA(N32 pos)
{
    XK(_rows == 0 &&  _cols == 0);
    XK(pos < MAX_rows * MAX_cols * MAX_vars);

    if (pos >= _vars)
        _vars = pos + 1;

    return _pMatrix[pos];
}

N32 Expression::value(N32 row, N32 col, N32 var)
{
    return get(row, col, var);
}

void Expression::printMatrix()
{
    ilog("rows: %d cols: %d vars: %d %s %d", _rows, _cols, _vars, ineText(), getRHS());

    for (N32 r = 0; r < _rows; ++r)
    {
        ilog("");

        for (N32 c = 0; c < _cols; ++c)
        {
            for (N32 v = 0; v < _vars; ++v)
            {
                log("%3d ", value(r, c, v));
            }
        }
    }
}

void Expression::printInLine()
{
    ilog("");

    if (_rows == 0 && _cols == 0)
    {
        for (N32 v = 0; v < _vars; ++v)
        {
            log("%3d ", refA(v));
        }
    }
    else
        for (N32 r = 0; r < _rows; ++r)
        {
            for (N32 c = 0; c < _cols; ++c)
            {
                for (N32 v = 0; v < _vars; ++v)
                {
                    log("%3d ", value(r, c, v));
                }
            }

            log(" ");
        }
}

void Expression::printExp()
{
    char sz[2048];
    int pos = 0;
    sz[pos] = 0;

    getTermsText(sz, pos);

    ilog(sz);
}

void Expression::lineText(char* sz, N32& pos)
{
    if (_rows == 0 && _cols == 0)
    {
        for (N32 v = 0; v < _vars; ++v)
        {
            int w = sprintf(sz + pos, "%3d ", refA(v));

            XK(w > 0);
            pos += w;
        }
    }
    else
    {
        pos += sprintf(sz + pos, "   ");

        for (N32 r = 0; r < _rows; ++r)
        {
            for (N32 c = 0; c < _cols; ++c)
            {
                int w = 0;

                for (N32 v = 0; v < _vars; ++v)
                {
                    w = sprintf(sz + pos, "%3d ", get(r, c, v));

                    XK(w > 0);
                    pos += w;
                }
            }

            pos += sprintf(sz + pos, "   ");
        }
    }
}

ExpressionPtr Expression::transposeEV(Expression* pRowPerm, Expression* pColPerm, Expression** pColVarPerm)
{
    ExpressionPtr pE = new Expression;

    N32 originalRow = 0;

    pE->setRHS(getRHS());
    pE->setIne(getIne());
    pE->setLHSModulo(getLHSModulo());

    for (N32 rp = 0; rp < pRowPerm->getItems(); ++ rp)
    {
        N32 r = pRowPerm->refA(rp) - 1;

        N32 originalCol = 0;

        for (N32 cp = 0; cp < pColPerm->getItems(); ++cp)
        {
            N32 c = pColPerm->refA(cp) - 1;

            N32 v = pColVarPerm[0]->refA(0) - 1;  // <----using  COL VAR zero only N32 v = pColVarPerm[c]->refA(0) - 1;

            N32 value = get(originalRow, originalCol, 0);

            pE->set(r, c, 0, (v == 0) ? value : -value);

            ++originalCol;
        }

        ++originalRow;
    }

    return pE;
}

ExpressionPtr Expression::transpose(Expression* pRowPerm, Expression* pColPerm, Expression** pColVarPerm)
{
    ExpressionPtr pE = new Expression;

    N32 originalRow = 0;

    pE->setRHS(getRHS());
    pE->setIne(getIne());
    pE->setLHSModulo(getLHSModulo());

    for (N32 rp = 0; rp < pRowPerm->getItems(); ++ rp)
    {
        N32 r = pRowPerm->refA(rp) - 1;

        N32 originalCol = 0;

        for (N32 cp = 0; cp < pColPerm->getItems(); ++cp)
        {
            N32 c = pColPerm->refA(cp) - 1;

            N32 originalVar = 0;

            N32 last = pColVarPerm[c]->getItems() - 1;
            N32 theOneMovingOut =  - 1;

            // find which one is going out
            for (N32 vp = 0; vp < pColVarPerm[c]->getItems(); ++vp)
            {
                N32 v = pColVarPerm[c]->refA(vp) - 1;

                if (v == last)
                {
                    theOneMovingOut = vp;
                    break;
                }
            }

            XK(theOneMovingOut >= 0 && theOneMovingOut <= last);

            N32 coefOftheOneMovingOut = (theOneMovingOut == last) ? 0 : get(originalRow, originalCol, theOneMovingOut);

            for (N32 vp = 0; vp < pColVarPerm[c]->getItems(); ++vp)
            {
                N32 v = pColVarPerm[c]->refA(vp) - 1;

                N32 value = (originalVar < last) ? get(originalRow, originalCol, originalVar)  : 0;
                value -= coefOftheOneMovingOut;

                if (v != last)
                    pE->set(r, c, v, value);

                ++originalVar;
            }

            pE->setRHS(pE->getRHS() - coefOftheOneMovingOut);

            ++originalCol;
        }

        ++originalRow;
    }

    return pE;
}

ExpressionListPtr Expression::generateEquivalentSymmetricExpressions(ExpressionList* pRowPermutations, ExpressionList* pColPermutations, ExpressionList* pVarPermutations, bool fEV)
{
    ExpressionListPtr pSet = new ExpressionList;

    for (Expression* pPermutationRow = pRowPermutations->head(); pPermutationRow; pPermutationRow = pPermutationRow->next())
    {
        for (Expression* pPermutationCol = pColPermutations->head(); pPermutationCol; pPermutationCol = pPermutationCol->next())
        {
            Expression* pCV[MAX_cols];
            startColVar(pCV, MAX_cols, pColPermutations->head()->getItems(), pVarPermutations);

            while (true)
            {
                ExpressionPtr pL;

                if (fEV)
                    pL = transposeEV(pPermutationRow, pPermutationCol, pCV);
                else
                    pL = transpose(pPermutationRow, pPermutationCol, pCV);

#ifdef _DEBUG_COLVAR
                ilog("");
                ilog("--------------------------------");
                ilog("matrix before:");
                printMatrix();
                ilog("row permutations:");
                pPermutationRow->printInLine();
                ilog("col permutations:");
                pPermutationCol->printInLine();

                ilog("--- listing colVars ---");

                for (N32 c = 0; c < pColPermutations->head()->getVars(); ++c)
                    pCV[c]->printInLine();

                ilog("--- end colVar ----");

                ilog("matrix after:");
                pL->printMatrix();
                ilog("--------------------------------");
                ilog("");
#endif

                pSet->pushBack(pL);

                if(!nextColVar(pCV, pColPermutations->head()->getItems(), pVarPermutations))
                    break;
            }
        }
    }

    return pSet;
}

// static
bool Expression::startColVar(Expression** pCV, N32 maxCols, N32 cols,  ExpressionList* pVarPermutations)
{
    if (!pVarPermutations->head() || cols >= maxCols)
    {
        XK(false);
        return false;
    }

    memset(pCV, 0, maxCols * sizeof(Expression*));

    for (N32 c = 0; c < cols; ++ c)
        pCV[c] = pVarPermutations->head();

    return true;
}

// static
bool Expression::nextColVar(Expression** pCV, N32 cols, ExpressionList* pVarPermutations)
{
    if (!pVarPermutations->head())
        return false;

    N32 n = 0;

    while(true)
    {
        if (n >= cols)
            return false;

        pCV[n] = pCV[n]->next();

        if (pCV[n] != NULL)
            return true;

        pCV[n] =  pVarPermutations->head();
        ++n;
    }

    return true;
}

ExpressionPtr Expression::findSpecialEquivalent(ExpressionList* pRowPermutations, ExpressionList* pColPermutations, ExpressionList* pVarPermutations)
{
    ExpressionListPtr pList = generateEquivalentSymmetricExpressions(pRowPermutations, pColPermutations, pVarPermutations, pVarPermutations->count() == 2);

#ifdef _DEBUG
    ilog("equivalents");
    pList->print();
#endif

    ExpressionPtr p = pList->head();

    ExpressionPtr pMax = p;

    if (!p)
        return pMax;

    p = p->next();

    for (; p; p = p->next())
    {
        if (pMax->isBetter(p))
pMax = p->rCopy();
}

return pMax;
}

N64 Expression::verifyVertex(Expression* pV)
{
    N64 total = 0;

    for (N32 r = 0; r < _rows; ++r)
    {
        for (N32 c = 0; c < _cols; ++c)
        {
            for (N32 v = 0; v < _vars; ++v)
            {
                total += value(r, c, v) * pV->get(r * _cols * _vars + c * _vars + v);
            }
        }
    }

    return total;
}

void Expression::printEq()
{
    N32 i = 1;

    for (N32 r = 0; r < getRows(); ++r)
    {
        for (N32 c = 0; c < getCols(); ++c)
        {
            for (N32 v = 0; v < getVs(); ++v)
            {
                N32 s = get(r, c, v);

                if (s == 0)
                    log("      ");
                else if (s == 1)
                    log(" +x%d", i);
                else if (s == -1)
                    log(" -x%d", i);
                else
                    log(" %+2dx%d", s, i);

                ++i;
            }
        }
    }

    log("%s %d\n", ineText(), getRHS());
}

void Expression::readExpressionVars()
{
    readInt("POVMStatus", _POVMStatus, -1);
    readInt("ProjectiveStatus", _ProjectiveStatus, -1);
    readInt("RealProjectiveStatus", _RealProjectiveStatus, -1);
    readInt("RealPOVMStatus", _RealPOVMStatus, -1);

    readDouble("ProjectiveValue", _projectiveValue, 0.);
    readDouble("ProjectiveRobustness", _projectiveRobustness, 0.);

    readDouble("POVMValue", _POVMValue, 0.);
    readDouble("POVMRobustness", _POVMRobustness, 0.);


    readDouble("RealProjectiveValue", _RealProjectiveValue, 0.);
    readDouble("RealProjectiveRobustness", _RealProjectiveRobustness, 0.);

    readDouble("RealPOVMValue", _RealPOVMValue, 0.);
    readDouble("RealPOVMRobustness", _RealPOVMRobustness, 0.);

    _varsInitialized = true;
}


void texItem(FILE* pF, const char* pFormat, ...)
{
    char sz[1024];
    sz[0] = 0;

    va_list arglist;
    va_start(arglist, pFormat);

    int nTotal = vsnprintf(sz, sizeof(sz), pFormat, arglist);

    if (nTotal == -1)
    {
        XK(false); // not enough space in buffer
    }

    N32 fontSize = 8;
    //    fprintf(pF, "\n\\fontsize{%d}{7.2}\\selectfont %s\\\\", fontSize, sz);
    fprintf(pF, "\n%s\\\\", sz);
    fprintf(pF, "\n\\hline ");
}

void Expression::printTeX(FILE* pF, ExpressionList* pL)
{
    pL->readListVars();
    readExpressionVars();

    fprintf(pF, "\n\\begin{minipage}[c]{0.50\\textwidth}");

    fprintf(pF, "\n\\begin{tabular}{|l|l|}");
    fprintf(pF, "\n\\hline ");

    texItem(pF, "Witness & %d@%d-%d-%d-%d", getID(), pL->getPreparations(), pL->getMeasurements(), pL->getStates(), pL->getOutcomes());
    texItem(pF, "Classic bound & %d", getRHS());

    if (_ProjectiveStatus == 1)
    {
        texItem(pF, "Projective value & %f", _projectiveValue);
        texItem(pF, "Projective robustness & %f", _projectiveRobustness);
    }

    if (_POVMStatus == 1)
    {
        texItem(pF, "POVM value & %f", _POVMValue);
        texItem(pF, "POVM robustness & %f", _POVMRobustness);
    }

    if (_RealProjectiveStatus == 1)
    {
        texItem(pF, "Real projective value & %f", _RealProjectiveValue);
        texItem(pF, "Real projective robustness & %f", _RealProjectiveRobustness);
    }

    if (_RealPOVMStatus == 1)
    {
        texItem(pF, "Real POVM value & %f", _RealPOVMValue);
        texItem(pF, "Real POVM robustness & %f", _RealPOVMRobustness);
    }

    fprintf(pF, "\n\\end{tabular}");

    fprintf(pF, "\n\\end{minipage}");

    fprintf(pF, "\n\\begin{minipage}[c]{0.50\\textwidth}");

    fprintf(pF, "\n\\begin{tabular}{l}");
    fprintf(pF, "\n\\begin{tabular}{lc}");

    if (getLHSModulo())
        fprintf(pF, "\n\\begin{tabular}{||");
    else
        fprintf(pF, "\n\\begin{tabular}{|");

    for (N32 c = 0; c < _cols; ++c)
    {
        for (N32 v = 0; v < _vars; ++v)
        {
            fprintf(pF, "C");

            if (v == _vars - 1 && c != _cols - 1)
                fprintf(pF, "!{\\vrule width 3pt}");
            else
                fprintf(pF, "|");
        }
    }

    if (getLHSModulo())
        fprintf(pF, "|}\n\\hline");
    else
        fprintf(pF, "}\n\\hline");

    for (N32 r = 0; r < _rows;  ++r)
    {
        fprintf(pF, "\n");

        for (N32 c = 0; c < _cols; ++c)
        {
            for (N32 v = 0; v < _vars; ++v)
            {
                fprintf(pF, "%d", get(r, c, v));

                if (c != _cols - 1 || v != _vars - 1)
                    fprintf(pF, "&");
            }
        }

        fprintf(pF, "\\\\");
        fprintf(pF, " \\hline");
    }

    fprintf(pF, "\n\\end{tabular} &");
    fprintf(pF, "\n%s %d ", ineText(), getRHS());

    fprintf(pF, "\n\\end{tabular} \\\\");


    fprintf(pF, "\\renewcommand{\\arraystretch}{1}");

    fprintf(pF, "\n\\begin{tabular}{|c|c|c|}");
    fprintf(pF, "\n\\hline ");

    fprintf(pF, "\n\\hline");
    fprintf(pF, "\n\\end{tabular}");
    fprintf(pF, "\n\\end{tabular}");


    fprintf(pF, "\n\\end{minipage}");

}

//eof

