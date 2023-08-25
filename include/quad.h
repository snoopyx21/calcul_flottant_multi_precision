#ifndef QUAD_H
#define QUAD_H

/**
 * \file quad
 * \brief Quad Program - create / read / / print / free Quad
 * \author DIVRIOTIS C. FLINT C. JUNG L. VETRIVEL G. 
 * \version 0.1
 * \date Janvier 2018
 *
 */

#include "utils.h"
#include <string.h>
#include "../build/y.tab.h"
#include "semiQuad.h"
#include "symbol.h"
#include "ast.h"

#define C2MP_QUAD_OPERAND_VARIABLE 0
#define C2MP_QUAD_OPERAND_INTEGER 1
#define C2MP_QUAD_OPERAND_FLOAT 2
#define C2MP_QUAD_OPERAND_STRING 3

#define C2MP_QUAD_BINARY 2
#define C2MP_QUAD_UNARY  1

/**
 * \struct quadOperand
 * \brief struct quadOperand - operand in Quad
 */
typedef struct quadOperand quadOperand;

/**
 * \struct quad
 * \brief struct quad generate
 */
typedef struct quad quad;

/**
 * \fn quadOperand createVariableOperand(int reference)
 * \brief Creates an quad operand related to a variable
 * \param reference reference of the variable
 * \return The created operand
 */
quadOperand createVariableOperand(int reference);

/**
 * \fn quadOperand createIntegerOperand(int value)
 * \brief Creates an quad operand related to an integer constant
 * \param value value of the constant
 * \return The created operand
 */
quadOperand createIntegerOperand(int value);

/**
 * \fn quadOperand createFloatOperand(double value)
 * \brief Creates an quad operand related to an float constant
 * \param value value of the constant
 * \return The created operand
 */
quadOperand createFloatOperand(double value);

/**
 * \fn quadOperand createVoidOperand(void)
 * \brief Creates an quad operand related to nothing (example : operand2 of sqrt)
 * \return The created operand
 */
quadOperand createVoidOperand(void);

/**
 * \fn quadOperand createStringOperand(char *string)
 * \brief Creates a string operand for a quad. Used by custom functions.
 * \param string the string (with quotes)
 * \return the created operand
 */
quadOperand createStringOperand(char *string);

/**
 * \fn symbolType getSymbolTypeFromOperand(quadOperand ope)
 * \brief Returns the type of an operand.
 * An operand can be a constant, in this case it is the type of the constant (int/float)
 * or a variable, in this case it returns the type of the variable
 * \param ope the operand from which we want to know the type
 * \return The type of the symbol (cf. symbol.h)
 */
symbolType getSymbolTypeFromOperand(quadOperand ope);

/**
 * \fn quad *createQuad(int assignment, char operator, char * name, int operandsNum, ...)
 * \brief Creates a quad structure
 * \param assignement The symbol (index in the table) to which assign the quad
 * \param operator the operator type
 * \param name The name of the function associated with the quad (optional)
 * \param operandsNum The number of operands used in the quad
 * \return The generated quad
 */
quad *createQuad(int assignment, char operator, char * name, int operandsNum, ...);

/**
 * \fn quad *createQuadFromOperandList(int assignment, char operator, char *name, int argsNum, quadOperand *list)
 * \brief Creates a quad from an operand list
 * \param assignement The assignement to which belongs the resulting quad
 * \param operator The type of the quad
 * \param name The name of the function (optional), if it is one
 * \param argsNum The number of quad operands in the tab
 * \param list The list of the operands (a tab)
 * \return The created quad 
 */
quad *createQuadFromOperandList(int assignment, char operator, char *name, int argsNum, quadOperand *list);


/**
 * \fn void freeQuads(quad *q)
 * \brief Frees the memory from quads
 * \param q a chained list of quads to be freed
 */
void freeQuads(quad *q);


/**
 * \fn quad *copySemiQuad(semiQuad *sq)
 * \brief Gets a quad list from a single semiQuad.
 * Example : the semiQuad a=x+b could be converted to the following quads :
 * t1=x, t2=b, a=t1+t2
 * \param sq semiQuad to be translated into quads
 * \return The created quad list
 */
quad *copySemiQuad(semiQuad *sq);

/**
 * \fn quad *generateQuadsFromAST(expressionAST *expr)
 * \brief Generates a list of quads matching the given AST
 * \param expr expressionAST to be read
 * \return The created quad list
 */
quad *generateQuadsFromAST(expressionAST *expr);

/**
 * \fn quad *getQuadsFromSemiQuads(semiQuad *sq)
 * \brief Generates a list of quads matching a list of semiQuads
 * \param sq semiQuad list
 * \return The created quad list
 */
quad *getQuadsFromSemiQuads(semiQuad *sq);


/**
 * \fn quad *concatQuads(quad *q1, quad *q2)
 * \brief Concatenates two lists of suads
 * (careful ! The two lists are modified by this operation)
 * \param q1 first list
 * \param q2 second list
 * \return concatenated lists
 */
quad *concatQuads(quad *q1, quad *q2);

/**
 * \fn void printOperand(quadOperand operand)
 * \brief Prints an operand
 * \param operand operand
 */
void printOperand(quadOperand operand);

/**
 * \fn void outputOperand(quadOperand operand)
 * \brief Outputs an operand to a file
 * \param operand operand
 */
void outputOperand(quadOperand operand);

/**
 * \fn void printQuads(quad* q)
 * \brief Prints a list of quads
 * \param g quads
 */
void printQuads(quad* q);



#endif // QUAD_H