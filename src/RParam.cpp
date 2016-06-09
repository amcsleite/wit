#include "Essential.h"
#include <iostream>
#include <string>
#include <iomanip>   // for std::setw
#include "Util.h"
#include "XList3.h"

typedef std::string String;

class StringMatrix
{
    static const N32 rowMax = 10;
    static const N32 colMax = 10;

    String _s[rowMax][colMax];

    N32 _rows;
    N32 _cols;

public:

    StringMatrix()
    {
        _rows = 0;
        _cols = 0;
    }

    StringMatrix(const StringMatrix& s)
    {
        _rows = 0;
        _cols = 0;

        for(N32 r = 0; r < s._rows; ++r)
        {
            for(N32 c = 0; c < s._cols; ++c)
            {
                at(r, c) = s.atc(r, c);
            }
        }
    }

    String& at(N32 row, N32 col)
    {

        XK(row >= 0 && row < rowMax);
        XK(col >= 0 && col < colMax);

        if (row >= _rows)
            _rows = row + 1;

        if (col >= _cols)
            _cols = col + 1;

        return _s[row][col];
    }

    const String& atc(N32 row, N32 col) const
    {

        XK(row >= 0 && row < rowMax);
        XK(col >= 0 && col < colMax);

        return _s[row][col];
    }

    void print(std::ostream& stream)
    {
        for(N32 r = 0; r < _rows; ++r)
        {
            for(N32 c = 0; c < _cols; ++c)
            {
				stream << "/*  " + std::to_string(r) + ", " + std::to_string(c) + " */     ";
                stream << at(r, c);
                stream << ";\n";
            }
            stream << "\n";
        }
    }

    static StringMatrix generateUnitary(N32 n);
    static StringMatrix mul(StringMatrix& m1, StringMatrix& m2);
};

String p(N32 n, N32 i)
{
	String s;

	if (i == 0)
		return "Cos(p" + std::to_string(n) + "_" + std::to_string(i) + ")";
	
	s = "Sin(p" + std::to_string(n) + "_" + std::to_string(i) + ")";

	s += "*" + p(n, i - 1);

	if (i >= 2)
		s += "*e"+ std::to_string(n) + ")";
	
	return s;
}

String wA(N32 n, N32 i)
{
	if (n == 1 && i == 0)
		return "1";
	
    return "gA(" + std::to_string(n) + ", " + std::to_string(i) + ")";
}

String wAC(N32 n, N32 i)
{
	if (n == 1 && i == 0)
		return "1";
	
    return "gAC(" + std::to_string(n) + ", " + std::to_string(i) + ")";
}

String wS(N32 n)
{
    return "gS(" + std::to_string(n) + ")";
}

String wC(N32 n)
{
    return "gC(" + std::to_string(n) + ")";
}

StringMatrix StringMatrix::generateUnitary(N32 n)
{
    StringMatrix m;

    if (n == 1)
    {
        m.at(0, 0) = "1";
        return m;
    }

    for (N32 i = 0; i < n - 1; ++i)
    {
        m.at(i, i) = "(1 - (1 - " + wC(n) + ")";

		if (wA(n - 1, i) != "1")
			m.at(i, i) += "*" + wA(n - 1, i);

		if (wAC(n - 1, i) != "1")
			m.at(i, i) +=  "*" + wAC(n - 1, i);

		m.at(i, i) += ")";
    }

    for (N32 r = 0; r < n - 1; ++r)
    {
		m.at(r, n - 1) =  wS(n);

		if (wA(n - 1, r) != "1")
			wA(n - 1, r) += "*" + wA(n - 1, r);
	}

    for (N32 c = 0; c < n - 1; ++c)
	{
        m.at(n - 1, c) = "-" + wS(n) ;

		if (wAC(n - 1, c) != "1")
			m.at(n - 1, c) +=  " * " + wAC(n - 1, c);
	}

    m.at(n - 1, n - 1)  = wC(n);

    StringMatrix v = generateUnitary(n - 1);

    v.at(n - 1, n - 1) = "1";

    return StringMatrix::mul(m, v);
}

StringMatrix genB(N32 n)
{
    StringMatrix m;
	
	for (N32 i = 0; i < n; ++i)
		m.at(i, i) = "eB" + std::to_string(i);
	
	return m;
}

StringMatrix genA(N32 n)
{
    StringMatrix m;
	m.at(0, 0) = "1";
	
	for (N32 i = 1; i < n; ++i)
		m.at(i, i) = "eA" + std::to_string(i);
	
	return m;
}

bool writeUnitaryFunction(std::ostream& stream, N32 dim)
{
    stream << "#include <armadillo>\n";

    StringMatrix sm = StringMatrix::generateUnitary(dim);

	StringMatrix eA = genA(dim);
	StringMatrix eB = genB(dim);

	StringMatrix m1 = StringMatrix::mul(sm, eB);
	StringMatrix m2 = StringMatrix::mul(eA, m1);
		
    stream << "\nvoid unitary()";
    stream << "\n{\n";

    m2.print(stream);

    stream << "\n}\n\n";

    return true;
}

StringMatrix StringMatrix::mul(StringMatrix& m1, StringMatrix& m2)
{
    StringMatrix result;

    if (m1._cols != m2._rows)
    {
        std::cout << m1._cols;
        std::cout << m2._rows;
        std::cout << "---------------\n";
        m2.print(std::cout);
    }

    XK(m1._cols == m2._rows);

    for (N32 r = 0; r < m1._rows; ++r)
    {
        for (N32 c = 0; c < m2._cols; ++c)
        {
            for (N32 j = 0; j < m1._cols; ++j)
            {
                String s;

                if (m1.at(r, j).empty() || m2.at(j, c).empty())
                {
                    s = "";
                }
                else if (m1.at(r, j) == String("1"))
                {
                    s = m2.at(j, c);
                }
                else if (m2.at(j, c) == String("1"))
                {
                    s = m1.at(r, j);
                }
                else
                    s = m1.at(r, j) + "*" + m2.at(j, c);

                if (!s.empty())
				{
					if (result.at(r, c).empty())
						result.at(r, c) = s;
					else
						result.at(r, c) += " + " + s;
				}
            }

        }
    }

    return result;
}

int main(int argc, char** argv)
{

    std::cout << "--------------------------------------------------------------------------------------------------------------------\n";
	std::cout << "\n" << pCos(2, 2);
	std::cout << "\n" << pSin(2, 2);
    std::cout << "--------------------------------------------------------------------------------------------------------------------\n";

	std::cout << "\n" << pCos(2, 3);
	std::cout << "\n" << pSin(2, 3);
    std::cout << "--------------------------------------------------------------------------------------------------------------------\n";

    StringMatrix a;
    a.at(0, 0) = "a";
    a.at(0, 1) = "b";
    a.at(1, 0) = "c";
    a.at(1, 1) = "d";

    StringMatrix b;
    b.at(0, 0) = "e";
    b.at(0, 1) = "f";
    b.at(1, 0) = "g";
    b.at(1, 1) = "g";

    a.print(std::cout);
    std::cout << "--------------------------------------------------------------------------------------------------------------------\n";
    b.print(std::cout);
    std::cout << "--------------------------------------------------------------------------------------------------------------------\n";

    StringMatrix c = StringMatrix::mul(a, b);
    c.print(std::cout);


    //stream.open("_unitary.cpp", std::ios_base::app);

    writeUnitaryFunction(std::cout, 1);
    std::cout << "--------------------------------------------------------------------------------------------------------------------\n";

    writeUnitaryFunction(std::cout, 2);
    std::cout << "--------------------------------------------------------------------------------------------------------------------\n";

    writeUnitaryFunction(std::cout, 3);
    std::cout << "--------------------------------------------------------------------------------------------------------------------\n";

    writeUnitaryFunction(std::cout, 4);
    std::cout << "--------------------------------------------------------------------------------------------------------------------\n";

    writeUnitaryFunction(std::cout, 5);
    std::cout << "--------------------------------------------------------------------------------------------------------------------\n";

    exit(EXIT_SUCCESS);
}

//eof
