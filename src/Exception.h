#pragma once
#include "Essential.h"

class Exception : public Counted
{

public:
   
   Exception(const char* p)
   {
    printf("Error found: \n");
    printf("%s", p);
    printf("\n");
   }
};

//eof
