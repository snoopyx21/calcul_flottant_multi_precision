#ifndef OPTI_H
#define OPTI_H

/**
 * \file optimization
 * \brief Optimize Quad or Operation to reduce the generate code
 * \author DIVRIOTIS C. FLINT C. JUNG L. VETRIVEL G. 
 * \version 0.1
 * \date Janvier 2018
 *
 */
#include "utils.h"
#include <string.h>
#include "quad.h"
#include "symbol.h"
#include "mpc.h"



// Number table
// Expression Hash table
// Name Table (Constant Table)

typedef struct referenceRow
{
	int reference;
	int optimizationRef;
} refRow;

typedef struct expressionHashRow
{
	char operator;
	int operand1;
	int operand2;
	int optimizationRef;
} exprHashRow;

typedef struct constantRow
{
	int reference;
	mpc_t constant;
	int flag;
} constRow;

typedef struct referenceList
{
	int ref;
	struct referenceList *next;
} referenceList;

/**
 * \fn referenceList *addReference(referenceList *list, int ref)
 * \brief Adds a reference to a referenceList
 * \param list list to which ref will be added
 * \param ref reference of the variable
 */
referenceList *addReference(referenceList *list, int ref);
/**
 * \fn concatReferenceList(referenceList *list1, referenceList *list2)
 * \brief Concatenates two reference lists
 * \param list1 list1
 * \param list2 list2
 */
referenceList *concatReferenceList(referenceList *list1, referenceList *list2);
/**
 * \fn bool referenceIsIn(int reference, referenceList *list)
 * \brief Checks if referenceList contains a particular reference
 * \param reference reference to be ckeched
 * \param list list
 */
bool referenceIsIn(int reference, referenceList *list);


/**
 * \fn quad* optimizeQuads(quad* quads)
 * \brief Applies various optiimizations to a quad list
 * \param quads list of quads
 * \return List of optimized quads
 */
quad* optimizeQuads(quad* quads);
/**
 * \fn quad* removeLoopsInvariants(quad* quads)
 * \brief Removes loop invariants in every loop in the quadlist
 * \param quads list of quads
 * \return New list of quads
 */
quad* removeLoopsInvariants(quad* quads);
/**
 * \fn quad* removeLoopInvariants(quad* quads)
 * \brief Removes loop invariants in ONE loop
 * \param quads first quad of the loop
 * \return The last quad of the bloc
 */
quad* removeLoopInvariants(quad* quads);
/**
 * \fn referenceList *getModifiedVariablesInBloc(quad* quads)
 * \brief Returns variables which are modified in this bloc
 * \param quads first quad of the block
 * \return List of references which are modified
 */
referenceList *getModifiedVariablesInBloc(quad* quads);
/**
 * \fn referenceList *getUsedVariablesInBloc(quad* quads)
 * \brief Returns variables which are used in this bloc
 * \param quads first quad of the block
 * \return List of references which are used
 */
referenceList *getUsedVariablesInBloc(quad* quads);
/**
 * \fn quad* reuseTemporaries(quad* quads)
 * \brief Reuse temporaries of a quadlist
 * \param quads quad list
 * \return New quadlist
 */
quad* reuseTemporaries(quad* quads);
/**
 * \fn quad* replaceUntil(quad* begin, quad*end, int oldRef, int newRef)
 * \brief Replaces every occurence of a variable from begin quad to end quad
 * \param begin first quad to apply this operation
 * \param end last quad to apply this operation (excluded)
 * \param oldRef reference to be replaced
 * \param newRef reference which will replace the old
 * \return New quadlist
 */
quad* replaceUntil(quad* begin, quad*end, int oldRef, int newRef);
/**
 * \fn quad* getSafeReuseQuad(quad* quads, int reference)
 * \brief Gets the quad to which we are sure a particular variable will never be called again
 * \param quads quadlist
 * \param reference reference we want to know when we can reuse it
 * \return New quadlist
 */
quad* getSafeReuseQuad(quad* quads, int reference);
/**
 * \fn quad* removeUselessTemp(quad* quads)
 * \brief Removes temporary whose value is never used
 * \param quads quadlist
 * \return New quadlist
 */
quad* removeUselessTemp(quad* quads);
/**
 * \fn quad* removeAllCommonSubExpressions(quad* quads)
 * \brief If a sub expression is affected twice to two different variables, affects only one time to the first
 * \param quads quadlist
 * \return New quadlist
 */
quad* removeAllCommonSubExpressions(quad* quads);
/**
 * \fn quad* removeCommonSubExpression(quad* quads, quad* firstQuad)
 * \brief Removes subexpression in ONE bloc
 * \param quads quadlist
 * \param firstQuad first quad of the list
 * \return New quadlist
 */
quad* removeCommonSubExpression(quad* quads, quad* firstQuad);
/**
 * \fn quad* ignoreBlocForCommonSubExpression(quad* quads, quad* firstQuad)
 * \brief Properly ignores a bloc for the  subexpression matching
 * \param quads quadlist
 * \param firstQuad first quad of the list
 * \return Last quad of the bloc
 */
quad* ignoreBlocForCommonSubExpression(quad* quads, quad* firstQuad);
/**
 * \fn void resetTables(void)
 * \brief Reset tables used by the subexpression matching functions
 */
void resetTables(void);

void optimizeExprToAssignment(quad* q, int reference);
void assignNewOptimizationRef(int reference);
void setOptimizationRef(int reference, int hashReference);
int getOperandOptimizationRef(int reference);
int createExprHash(char op, int operand1, int operand2);
int createRefTableRow(int reference);
int findOptimizationRefTable(int reference);
int findRefTable(int reference);
int findExprHashTable(char op, int operand1, int operand2);
int findConstantTable(int reference);


#endif //OPTIMIZATION_H