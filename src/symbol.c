#include "symbol.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

symbol variables[MAX];
int variablesSize = 0;

symbol newSymbol(const char name[], symbolType type, bool isTemp, bool isBlockCondition)
{
    variables[variablesSize].name = strdup(name);
    variables[variablesSize].reference = variablesSize;
    variables[variablesSize].isConstant = 0;
    variables[variablesSize].isTemp = isTemp;
    variables[variablesSize].isBlockCondition = isBlockCondition;
    
    variables[variablesSize].type_symbol = type;
    
    
    /*if (isTemp)
    {
        variables[variablesSize].type_symbol = MPC_T;
    }
    else
    {
        // default value
        variables[variablesSize].type_symbol = FLOAT_NUMBER;
    }*/

    return variables[variablesSize++];
}

// if the variable is found, returns a reference, else -1
int getSymbolReference(const char name[])
{
    for(int i=0;i<variablesSize;++i)
    {
        if(strcmp(name, variables[i].name) == 0)
        {
            return i;
        }
    }
    return -1;
}

symbol getSymbolFromReference(int ref)
{
    return variables[ref];
}

// if the variable is found, returns a reference, else creates one and returns it
int getReferenceFromName(const char name[])
{
    int reference = getSymbolReference(name);
    if(reference == -1)
    {
        return newSymbol(name, FLOAT_NUMBER, false, false).reference;
    }

    return reference;
}

// returns the name of the variable
const char *getNameFromReference(int reference)
{
    return variables[reference].name;
}

symbol newTemp(symbolType type, bool isBlockCondition)
{
    char buffer[1024];
    static int tempCount = 0;
    snprintf(buffer, 1024, "C2MP___temp_%d",tempCount);
    
    ++tempCount;

    return newSymbol(buffer, type, true, isBlockCondition);
}

int getSymbolNum(void)
{
    return variablesSize;
}

void setSymbolToBlockCondition(int ref)
{
    variables[ref].isBlockCondition = true;
}
