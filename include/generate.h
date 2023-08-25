#ifndef __GENERATE_H
#define __GENERATE_H

/**
 * \file generate
 * \brief generate code C (code MPC) 
 * \author DIVRIOTIS C. FLINT C. JUNG L. VETRIVEL G. 
 * \version 0.1
 * \date Janvier 2018
 *
 */

#include "utils.h"
#include "semiQuad.h"
#include "symbol.h"
#include "ast.h"

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
 * \fn void generateCode(quad* q, char *rounding, int precision)
 * \brief Generate MPC instructions
 * \param q The list of the quads parsed previously
 * \param rounding The rounding type given in the pragma (or default)
 * \param precision The precision specified int the pragma (or default)
 */
void generateCode(quad* q, char *rounding, int precision);

/**
 * \fn bool * generateInitCode(quad *q, int precision)
 * \brief Generate the code necessary to initialize the temporary variables.
 * \param q This is the list of the quads generated from the parsing
 * \param precision This is the precision specified in the pragma (or default)
 * \return A table of booleans that allows us to know which temp is used.
 */
bool * generateInitCode(quad *q, int precision);

/**
 * \fn void generateClearCode(bool *tempList)
 * \brief Generates the code necessary to clear temporary variables.
 * \param tempList List of the temporary variables generated and used.
 */ 
void generateClearCode(bool *tempList);

#endif // GENERATE_H