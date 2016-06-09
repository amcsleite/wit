#include <iostream>
#include <armadillo>
#include "Util.h"

#define Mat mat
LogStream g_logStream("wMat.log");

N32 preparations, measurements, states, outcomes, vars, maxIterations;

void tests(void);

void writeAllWits()
{
    char name[1024];
    char fileName[1024];
    sprintf(name, "W%d%d%d%d", preparations, measurements, states, outcomes);
    sprintf(fileName, "W%d%d%d%d.m", preparations, measurements, states, outcomes);

    FILE* pF = fopen(fileName, "w");

    if (!pF)
    {
        Util::errorMsg("couldn't open matlab file");
        return;
    }

    fprintf(pF, "\nfunction [ a, c ] = %s()", name);

    ExpressionListPtr pList = new ExpressionList;
    Util::parsePortaIEQ("selected.ieq", pList, preparations, measurements, vars);

    pList->prepareMat(pF);

    fprintf(pF, "\nend\n");
    fclose(pF);
}

void readVars()
{
    if (!Util::readIntParam("db/preparations", preparations, 3))
    {
        Util::errorMsg("preparations parameter file not found");
        exit(EXIT_FAILURE);
    }

    if (!Util::readIntParam("db/measurements", measurements, 2))
    {
        Util::errorMsg(" measurements file not found");
        exit(EXIT_FAILURE);
    }

    if(!Util::readIntParam("db/states", states, 2))
    {
        Util::errorMsg("states parameter file not found");
        exit(EXIT_FAILURE);
    }

    if(!Util::readIntParam("db/outcomes", outcomes, 2))
    {
        Util::errorMsg("outcomes parameter file not found");
        exit(EXIT_FAILURE);
    }

    vars = outcomes - 1;
}

int main(int argc, char** argv)
{
    strcpy(Util::s_folderName, ".");

    readVars();
    writeAllWits();
    exit(EXIT_SUCCESS);
}

//eof

