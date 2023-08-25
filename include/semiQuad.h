#ifndef SEMIQUAD_H
#define SEMIQUAD_H

/**
 * \file semiQuad
 * \brief SemiQuad Program - create / print / free SemiQuad
 * \author DIVRIOTIS C. FLINT C. JUNG L. VETRIVEL G. 
 * \version 0.1
 * \date Janvier 2018
 *
 */

#include <string.h>

#include "utils.h"
#include "../build/y.tab.h"
#include "ast.h"
#include "symbol.h"

/**
 * \struct semiquad
 * \brief struct semiQuad - direct generation of the grammar
 */
typedef struct semiQuad semiQuad;

/**
 * \fn semiQuad *createSemiQuad(char operator, int assignment, expressionAST *expression)
 * \brief Creates a single semiQuad (list of one element)
 * \param operator operation done by the semiQuad
 * \param assignment variable to which the operation will be affected
 * \param expression expression affected to the variable
 * \return corresponding single semiQuad
 */
semiQuad *createSemiQuad(char operator, int assignment, expressionAST *expression);

/**
 * \fn semiQuad *concatSemiQuad(semiQuad *q1, semiQuad *q2)
 * \brief Concatenates two lists of semiQuads
 * (careful ! The two lists are modified by this operation)
 * \param q1 first list
 * \param q2 second list
 * \return concatenated lists
 */
semiQuad *concatSemiQuad(semiQuad *q1, semiQuad *q2);

/**
 * \fn void freeSemiQuad(semiQuad *sq)
 * \brief Frees the memory of the specified list of semi quads
 * \param sq list of semiQuads, which need to be freed
 */
void freeSemiQuad(semiQuad *sq);

/**
 * \fn void printSemiQuads(semiQuad *q)
 * \brief Prints semiQuads to the standard output
 * \param q list of semiQuads, which need to be printed
 */
void printSemiQuads(semiQuad *q);

#endif // SEMIQUAD_H