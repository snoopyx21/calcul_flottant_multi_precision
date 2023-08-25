#ifndef SYMBOL_H
#define SYMBOL_H

/**
 * \file symbol
 * \brief handle symbol and create newTemp
 * \author DIVRIOTIS C. FLINT C. JUNG L. VETRIVEL G. 
 * \version 0.1
 * \date Janvier 2018
 *
 */

#include <stdbool.h>
#include "utils.h"
#include <string.h>

#define MAX 1024*8

/**
 * \enum symbolType
 * \brief Type of the symbol - presence in struct symbol
 */
typedef enum 
{
    FLOAT_NUMBER,
    INTEGER_NUMBER,
    MPC_T
} symbolType;

/**
 * \struct symbol
 * \brief struct symbol - symbol met in syntax.y
 */
typedef struct symbol_s
{
    int reference;
    char *name;
    bool isConstant;
    bool isTemp;
    bool isBlockCondition;
    symbolType type_symbol;
} symbol;

/**
 * \brief Creates a new symbol and adds it to the symbol table.
 * \param name The name of the symbol
 * \param isTemp Specify if the symbol created is a temporary variable
 * \return Returns the instance of the new symbol created.
 */
symbol newSymbol(const char name[], symbolType type, bool isTemp, bool isBlockCondition);

/**
 * \brief This function return the reference of a symbol in the symbol table
 * \param name The name of the symbol
 * \return Returns the index of the symbol in the table, or -1 if not found.
 */
int getSymbolReference(const char name[]);

/**
 * \brief Returns the index in the symbol table of a given symbol. This function
 * creates the symbol if it doesn't exist.
 * \param The name of the symbol
 * \return The index in the symbol table
 */
int getReferenceFromName(const char name[]);

/**
 * \brief Returns the name of a symbol with a given reference
 * \param reference The index in the symbol table
 * \return The name of the symbol
 */
const char *getNameFromReference(int reference);

/**
 * \brief Generate a new temporary variable
 * \return The symbol associated with the variable generated
 */
symbol newTemp(symbolType type, bool isBlockCondition);

/**
 * \brief This function returns the symbol from the symbol table at the index
 * given as parameter.
 * \param ref Index in the symbol table
 * \return The symbol at the index specified as a parameter
 */
symbol getSymbolFromReference(int ref);

int getSymbolNum(void);
void setSymbolToBlockCondition(int ref);

#endif // SYMBOL_H

