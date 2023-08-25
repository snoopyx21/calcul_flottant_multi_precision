#include "optimization.h"

refRow refTable[MAX];
int refSize = 0;
int refCount = 0;

exprHashRow exprHashTable[MAX];
int exprHashSize = 0;

constRow constTable[MAX];
int constSize = 0;

referenceList *addReference(referenceList *list, int ref)
{
    referenceList *element = malloc(sizeof(referenceList));
    element->next = list;
    element->ref = ref;
    return element;
}

referenceList *concatReferenceList(referenceList *list1, referenceList *list2)
{
    if(list1 == NULL)
    {
        return list2;
    }
    
    referenceList *element = list1;
    while(element->next != NULL)
    {
        element = element->next;
    }
    element->next = list2;
    return list1;
}

quad* optimizeQuads(quad* quads)
{
    for(int i=0;i<1000;++i)
    {
        quads = removeAllCommonSubExpressions(quads);
        quads = removeUselessTemp(quads);
        quads = removeLoopsInvariants(quads);
        quads = reuseTemporaries(quads);
    }
    
    return quads;
}

bool referenceIsIn(int reference, referenceList *list)
{
    if(list == NULL)
    {
        return false;
    }
    
    referenceList *current = list;
    
    do
    {
        if(reference == current->ref)
        {
            return true;
        }
        current = current->next;
    }while(current != NULL);
    
    return false;
}

/*
    for (int i = 0; i < n; i++) {
        x = y + z;
        a[i] = 6 * i + x * x;
    }
    ->
    x = y + z;
    temp1 = x * x;
    for (int i = 0; i < n; i++) {
        a[i] = 6 * i + temp1;
    }
*/
quad* reuseTemporaries(quad* quads)
{
    quad *firstQuad = quads;
    for(int i=0;i<getSymbolNum();++i)
    {
        if(!getSymbolFromReference(i).isTemp)
        {
            continue;
        }
        if(getSymbolFromReference(i).isBlockCondition)
        {
            continue;
        }
        
        quad *nextFreeQuad = getSafeReuseQuad(quads, i);
        int replaced = 0;
        
        while(nextFreeQuad != firstQuad)
        {
            switch(nextFreeQuad->operator)
            {
                case C2MP_QUAD_ASSIGNMENT:
                case C2MP_OPERATOR_BINARY_PLUS:
                case C2MP_OPERATOR_BINARY_DOT:
                case C2MP_OPERATOR_EQUAL:
                case C2MP_OPERATOR_NOT_EQUAL:
                case C2MP_OPERATOR_BITWISE_AND:
                case C2MP_OPERATOR_BITWISE_OR:
                case C2MP_OPERATOR_BITWISE_XOR:
                case C2MP_OPERATOR_LOGICAL_AND:
                case C2MP_OPERATOR_LOGICAL_OR:
                case C2MP_OPERATOR_BINARY_MINUS:
                case C2MP_OPERATOR_BINARY_DIVIDE:
                case C2MP_OPERATOR_LOWER_THAN:
                case C2MP_OPERATOR_GREATER_THAN:
                case C2MP_OPERATOR_LOWER_OR_EQUAL:
                case C2MP_OPERATOR_GREATER_OR_EQUAL:
                case C2MP_FUNCTION_POW:
                case C2MP_OPERATOR_UNARY_MINUS:
                case C2MP_OPERATOR_UNARY_PLUS:
                case C2MP_OPERATOR_LOGICAL_NOT:
                case C2MP_OPERATOR_BITWISE_NOT:
                case C2MP_FUNCTION_SQRT:
                //case C2MP_FUNCTION_ABS:
                case C2MP_FUNCTION_EXP:
                case C2MP_FUNCTION_LOG:
                case C2MP_FUNCTION_LOG10:
                case C2MP_FUNCTION_COS:
                case C2MP_FUNCTION_SIN:
                case C2MP_FUNCTION_COSH:
                case C2MP_FUNCTION_SINH:
                case C2MP_FUNCTION_SQR:
                case C2MP_FUNCTION_UNKNOWN:
                    if(getSymbolFromReference(nextFreeQuad->assignment).isTemp &&
                    getSymbolFromReference(nextFreeQuad->assignment).type_symbol == getSymbolFromReference(i).type_symbol &&
                    !getSymbolFromReference(nextFreeQuad->assignment).isBlockCondition)
                    {
                        nextFreeQuad = replaceUntil(nextFreeQuad, firstQuad, nextFreeQuad->assignment, i);
                        replaced = 1;
                    }
                    break;
                default:
                    break;
            }
            
            if(replaced)
            {
                break;
            }
            
            nextFreeQuad = nextFreeQuad->next;
        }
    }
    
    return quads;
}

quad* replaceUntil(quad* begin, quad*end, int oldRef, int newRef)
{
    quad *q = begin;
    while(q != end)
    {
        switch(q->operator)
        {
            case C2MP_QUAD_ASSIGNMENT:
            case C2MP_OPERATOR_UNARY_MINUS:
            case C2MP_OPERATOR_UNARY_PLUS:
            case C2MP_OPERATOR_LOGICAL_NOT:
            case C2MP_OPERATOR_BITWISE_NOT:
            case C2MP_FUNCTION_SQRT:
            //case C2MP_FUNCTION_ABS:
            case C2MP_FUNCTION_EXP:
            case C2MP_FUNCTION_LOG:
            case C2MP_FUNCTION_LOG10:
            case C2MP_FUNCTION_COS:
            case C2MP_FUNCTION_SIN:
            case C2MP_FUNCTION_COSH:
            case C2MP_FUNCTION_SINH:
            case C2MP_FUNCTION_SQR:
                if(q->assignment == oldRef)
                {
                    q->assignment = newRef;
                }
                if(q->operands[0].type == C2MP_QUAD_OPERAND_VARIABLE && q->operands[0].reference == oldRef)
                {
                    q->operands[0].reference = newRef;
                }
                break;
            case C2MP_OPERATOR_BINARY_PLUS:
            case C2MP_OPERATOR_BINARY_DOT:
            case C2MP_OPERATOR_EQUAL:
            case C2MP_OPERATOR_NOT_EQUAL:
            case C2MP_OPERATOR_BITWISE_AND:
            case C2MP_OPERATOR_BITWISE_OR:
            case C2MP_OPERATOR_BITWISE_XOR:
            case C2MP_OPERATOR_LOGICAL_AND:
            case C2MP_OPERATOR_LOGICAL_OR:
            case C2MP_OPERATOR_BINARY_MINUS:
            case C2MP_OPERATOR_BINARY_DIVIDE:
            case C2MP_OPERATOR_LOWER_THAN:
            case C2MP_OPERATOR_GREATER_THAN:
            case C2MP_OPERATOR_LOWER_OR_EQUAL:
            case C2MP_OPERATOR_GREATER_OR_EQUAL:
            case C2MP_FUNCTION_POW:
                if(q->assignment == oldRef)
                {
                    q->assignment = newRef;
                }
                if(q->operands[0].type == C2MP_QUAD_OPERAND_VARIABLE && q->operands[0].reference == oldRef)
                {
                    q->operands[0].reference = newRef;
                }
                if(q->operands[1].type == C2MP_QUAD_OPERAND_VARIABLE && q->operands[1].reference == oldRef)
                {
                    q->operands[1].reference = newRef;
                }
                break;
            case C2MP_FUNCTION_UNKNOWN:
                if(q->assignment == oldRef)
                {
                    q->assignment = newRef;
                }
                for(int i = 0; i < q->operandsNum; i++)
                {
                    if(q->operands[i].type == C2MP_QUAD_OPERAND_VARIABLE && q->operands[i].reference == oldRef)
                    {
                        q->operands[i].reference = newRef;
                    }
                }
                break;
            case C2MP_QUAD_IF:
            case C2MP_QUAD_WHILE:
            case C2MP_QUAD_DOWHILE:
            case C2MP_QUAD_ENDIF:
            case C2MP_QUAD_ENDWHILE:
            case C2MP_QUAD_ENDDOWHILE:
                if(q->assignment == oldRef)
                {
                    q->assignment = newRef;
                }
                break;
            case C2MP_QUAD_NO_ASSIGNMENT:
                for(int i = 0; i < q->operandsNum; i++)
                {
                    if(q->operands[i].type == C2MP_QUAD_OPERAND_VARIABLE && q->operands[i].reference == oldRef)
                    {
                        q->operands[i].reference = newRef;
                    }
                }
                break;
            case C2MP_QUAD_ELSE:
                // nothing
                break;
            default:
                panic("optimization.c","getSafeReuseQuad","Not recognized operator\n");
        }
        
        q = q->next;
    }
    return begin;
}

quad* getSafeReuseQuad(quad* quads, int reference)
{
    quad *q = quads;
    quad *firstQuad = quads;
    quad *lastFreeQuad = quads;
    
    int blocDepth = 0;
    int inIf = 0;
    referenceList *modifiedVariables = NULL;
    referenceList *usedVariables = NULL;
    
    do
    {
        switch(q->operator)
        {
            case C2MP_QUAD_IF:
                if(blocDepth == 0)
                {
                    inIf = 1;
                }
                break;
            case C2MP_QUAD_WHILE:
            case C2MP_QUAD_DOWHILE:
                ++blocDepth;
                if(blocDepth == 1 && !inIf)
                {
                    modifiedVariables = getModifiedVariablesInBloc(q);
                    usedVariables = getUsedVariablesInBloc(q);
                }
                break;
            case C2MP_QUAD_NO_ASSIGNMENT:
            case C2MP_QUAD_ELSE:
                // nothing
                break;
            case C2MP_QUAD_ENDIF:
                if(blocDepth == 0)
                {
                    inIf = 0;
                }
                break;
            case C2MP_QUAD_ENDWHILE:
            case C2MP_QUAD_ENDDOWHILE:
            	--blocDepth;
            default:
                break;
        }
        if(blocDepth == 0 && !inIf)
        {
            int usedOrModified;
            switch(q->operator)
            {
                case C2MP_QUAD_ASSIGNMENT:
                case C2MP_OPERATOR_BINARY_PLUS:
                case C2MP_OPERATOR_BINARY_DOT:
                case C2MP_OPERATOR_EQUAL:
                case C2MP_OPERATOR_NOT_EQUAL:
                case C2MP_OPERATOR_BITWISE_AND:
                case C2MP_OPERATOR_BITWISE_OR:
                case C2MP_OPERATOR_BITWISE_XOR:
                case C2MP_OPERATOR_LOGICAL_AND:
                case C2MP_OPERATOR_LOGICAL_OR:
                case C2MP_OPERATOR_BINARY_MINUS:
                case C2MP_OPERATOR_BINARY_DIVIDE:
                case C2MP_OPERATOR_LOWER_THAN:
                case C2MP_OPERATOR_GREATER_THAN:
                case C2MP_OPERATOR_LOWER_OR_EQUAL:
                case C2MP_OPERATOR_GREATER_OR_EQUAL:
                case C2MP_FUNCTION_POW:
                case C2MP_OPERATOR_UNARY_MINUS:
                case C2MP_OPERATOR_UNARY_PLUS:
                case C2MP_OPERATOR_LOGICAL_NOT:
                case C2MP_OPERATOR_BITWISE_NOT:
                case C2MP_FUNCTION_SQRT:
                //case C2MP_FUNCTION_ABS:
                case C2MP_FUNCTION_EXP:
                case C2MP_FUNCTION_LOG:
                case C2MP_FUNCTION_LOG10:
                case C2MP_FUNCTION_COS:
                case C2MP_FUNCTION_SIN:
                case C2MP_FUNCTION_COSH:
                case C2MP_FUNCTION_SINH:
                case C2MP_FUNCTION_SQR:
                    if(q->assignment == reference || q->operands[0].reference == reference || q->operands[1].reference == reference)
                    {
                        lastFreeQuad = q->next;
                    }
                    break;
                case C2MP_FUNCTION_UNKNOWN:
                    usedOrModified = q->assignment == reference;
                    for(int i = 0; i < q->operandsNum; i++)
                    {
                        usedOrModified = usedOrModified||(q->operands[i].reference == reference);
                    }
                    if(usedOrModified)
                    {
                        lastFreeQuad = q->next;
                    }
                    break;
                case C2MP_QUAD_ENDWHILE:
                case C2MP_QUAD_ENDDOWHILE:
                    if(referenceIsIn(reference, modifiedVariables) || referenceIsIn(reference, usedVariables))
                    { // if the reference has been used in a loop, only the quad next to the end is free to use
                        lastFreeQuad = q->next;
                    }
                    modifiedVariables = NULL;
                    usedVariables = NULL;
                    break;
                case C2MP_QUAD_IF:
                case C2MP_QUAD_WHILE:
                case C2MP_QUAD_DOWHILE:
                case C2MP_QUAD_NO_ASSIGNMENT:
                case C2MP_QUAD_ELSE:
                case C2MP_QUAD_ENDIF:
                    // nothing
                    break;
                default:
                    panic("optimization.c","getSafeReuseQuad","Not recognized operator\n");
            }
        }
        q=q->next;
    }while(q != firstQuad);
    
    return lastFreeQuad;
}


quad* removeLoopsInvariants(quad* quads)
{
    quad *firstQuad = quads;
    quad* q = quads;

    do
    {
        switch(q->operator)
        {
            case C2MP_QUAD_WHILE:
            case C2MP_QUAD_DOWHILE:
                if(q == firstQuad)
                {
                    firstQuad = removeLoopInvariants(q);
                    q = firstQuad;
                }
                else
                {
                    q = removeLoopInvariants(q);
                }
                break;
            default:
                break;
        }
        q=q->next;
    }while(q != firstQuad);
    
    return firstQuad;
}

quad* removeLoopInvariants(quad* quads)
{
    quad *firstQuadInBloc = quads;
    quad *firstQuadReturn = quads->previous;
    quad *q = quads;
    
    referenceList *modifiedVariables = getModifiedVariablesInBloc(firstQuadInBloc);
    
    int blocDepth = 0;
	
	do
	{
	    bool isInvariant;
		switch(q->operator)
		{
			case C2MP_QUAD_ASSIGNMENT:
            case C2MP_OPERATOR_BINARY_PLUS:
            case C2MP_OPERATOR_BINARY_DOT:
            case C2MP_OPERATOR_EQUAL:
            case C2MP_OPERATOR_NOT_EQUAL:
            case C2MP_OPERATOR_BITWISE_AND:
            case C2MP_OPERATOR_BITWISE_OR:
            case C2MP_OPERATOR_BITWISE_XOR:
            case C2MP_OPERATOR_LOGICAL_AND:
            case C2MP_OPERATOR_LOGICAL_OR:
            case C2MP_OPERATOR_BINARY_MINUS:
            case C2MP_OPERATOR_BINARY_DIVIDE:
            case C2MP_OPERATOR_LOWER_THAN:
            case C2MP_OPERATOR_GREATER_THAN:
            case C2MP_OPERATOR_LOWER_OR_EQUAL:
            case C2MP_OPERATOR_GREATER_OR_EQUAL:
            case C2MP_FUNCTION_POW:
            case C2MP_OPERATOR_UNARY_MINUS:
            case C2MP_OPERATOR_UNARY_PLUS:
            case C2MP_OPERATOR_LOGICAL_NOT:
            case C2MP_OPERATOR_BITWISE_NOT:
            case C2MP_FUNCTION_SQRT:
            //case C2MP_FUNCTION_ABS:
            case C2MP_FUNCTION_EXP:
            case C2MP_FUNCTION_LOG:
            case C2MP_FUNCTION_LOG10:
            case C2MP_FUNCTION_COS:
            case C2MP_FUNCTION_SIN:
            case C2MP_FUNCTION_COSH:
            case C2MP_FUNCTION_SINH:
            case C2MP_FUNCTION_SQR:
            case C2MP_FUNCTION_UNKNOWN:
                if(blocDepth == 1)
                {
                    isInvariant = true;
                    for(int i=0;i<q->operandsNum;++i)
                    {
                        if(q->operands[i].type == C2MP_QUAD_OPERAND_INTEGER || q->operands[i].type == C2MP_QUAD_OPERAND_FLOAT)
                        {
                            continue;
                        }
                        if(q->operands[i].type == C2MP_QUAD_OPERAND_VARIABLE)
                        {
                            if(referenceIsIn(q->operands[i].reference, modifiedVariables))
                            {
                                isInvariant = false;
                            }
                        }
                        else
                        { // not integer or float or variable
                            isInvariant = false;
                        }
                    }
                    if(isInvariant)
                    {
                        
                        quad *quadToInsert = q;
                        q=q->previous;
                        q->next = quadToInsert->next;
                        q->next->previous = q;
                        
                        quadToInsert->previous = firstQuadInBloc->previous;
                        quadToInsert->next = firstQuadInBloc;
                        firstQuadInBloc->previous->next = quadToInsert;
                        firstQuadInBloc->previous = quadToInsert;
                    }
                }
                break;
            case C2MP_QUAD_IF:
            case C2MP_QUAD_WHILE:
            case C2MP_QUAD_DOWHILE:
                ++blocDepth;
                break;
            case C2MP_QUAD_NO_ASSIGNMENT:
            case C2MP_QUAD_ELSE:
                // nothing
                break;
            case C2MP_QUAD_ENDIF:
            case C2MP_QUAD_ENDWHILE:
            case C2MP_QUAD_ENDDOWHILE:
            	--blocDepth;
                break;

            default:
            	panic("optimization.c","getModifiedVariablesInBloc","Not recognized operator\n");
            	break;
		}
		q = q->next;
	} while(blocDepth > 0);
    
    return firstQuadReturn->next;
}

referenceList *getModifiedVariablesInBloc(quad* quads)
{
    if(quads == NULL)
    {
        printf("no quads for optimization\n");
    }
    
    referenceList *modifiedVariables = NULL;
	
	quad* q = quads;
	quad *firstQuad = quads;
	int blocDepth = 0;
	
	do
	{
		switch(q->operator)
		{
			case C2MP_QUAD_ASSIGNMENT:
            case C2MP_OPERATOR_BINARY_PLUS:
            case C2MP_OPERATOR_BINARY_DOT:
            case C2MP_OPERATOR_EQUAL:
            case C2MP_OPERATOR_NOT_EQUAL:
            case C2MP_OPERATOR_BITWISE_AND:
            case C2MP_OPERATOR_BITWISE_OR:
            case C2MP_OPERATOR_BITWISE_XOR:
            case C2MP_OPERATOR_LOGICAL_AND:
            case C2MP_OPERATOR_LOGICAL_OR:
            case C2MP_OPERATOR_BINARY_MINUS:
            case C2MP_OPERATOR_BINARY_DIVIDE:
            case C2MP_OPERATOR_LOWER_THAN:
            case C2MP_OPERATOR_GREATER_THAN:
            case C2MP_OPERATOR_LOWER_OR_EQUAL:
            case C2MP_OPERATOR_GREATER_OR_EQUAL:
            case C2MP_FUNCTION_POW:
            case C2MP_OPERATOR_UNARY_MINUS:
            case C2MP_OPERATOR_UNARY_PLUS:
            case C2MP_OPERATOR_LOGICAL_NOT:
            case C2MP_OPERATOR_BITWISE_NOT:
            case C2MP_FUNCTION_SQRT:
            //case C2MP_FUNCTION_ABS:
            case C2MP_FUNCTION_EXP:
            case C2MP_FUNCTION_LOG:
            case C2MP_FUNCTION_LOG10:
            case C2MP_FUNCTION_COS:
            case C2MP_FUNCTION_SIN:
            case C2MP_FUNCTION_COSH:
            case C2MP_FUNCTION_SINH:
            case C2MP_FUNCTION_SQR:
            case C2MP_FUNCTION_UNKNOWN:
                modifiedVariables = addReference(modifiedVariables, q->assignment);
                break;
            case C2MP_QUAD_IF:
            case C2MP_QUAD_WHILE:
            case C2MP_QUAD_DOWHILE:
                ++blocDepth;
                break;
            case C2MP_QUAD_NO_ASSIGNMENT:
            case C2MP_QUAD_ELSE:
                // nothing
                break;
            case C2MP_QUAD_ENDIF:
            case C2MP_QUAD_ENDWHILE:
            case C2MP_QUAD_ENDDOWHILE:
            	--blocDepth;
                break; 
            default:
            	panic("optimization.c","getModifiedVariablesInBloc","Not recognized operator\n");
            	break;
		}
		q = q->next;
	} while(q != firstQuad && blocDepth > 0);
	
	return modifiedVariables;
}

referenceList *getUsedVariablesInBloc(quad* quads)
{
    if(quads == NULL)
    {
        printf("no quads for optimization\n");
    }
    
    referenceList *usedVariables = NULL;
	
	quad* q = quads;
	quad *firstQuad = quads;
	int blocDepth = 0;
	
	do
	{
		switch(q->operator)
		{
			case C2MP_QUAD_ASSIGNMENT:
            case C2MP_OPERATOR_UNARY_MINUS:
            case C2MP_OPERATOR_UNARY_PLUS:
            case C2MP_OPERATOR_LOGICAL_NOT:
            case C2MP_OPERATOR_BITWISE_NOT:
            case C2MP_FUNCTION_SQRT:
            //case C2MP_FUNCTION_ABS:
            case C2MP_FUNCTION_EXP:
            case C2MP_FUNCTION_LOG:
            case C2MP_FUNCTION_LOG10:
            case C2MP_FUNCTION_COS:
            case C2MP_FUNCTION_SIN:
            case C2MP_FUNCTION_COSH:
            case C2MP_FUNCTION_SINH:
            case C2MP_FUNCTION_SQR:
                usedVariables = addReference(usedVariables, q->operands[0].reference);
                break;
            case C2MP_OPERATOR_BINARY_PLUS:
            case C2MP_OPERATOR_BINARY_DOT:
            case C2MP_OPERATOR_EQUAL:
            case C2MP_OPERATOR_NOT_EQUAL:
            case C2MP_OPERATOR_BITWISE_AND:
            case C2MP_OPERATOR_BITWISE_OR:
            case C2MP_OPERATOR_BITWISE_XOR:
            case C2MP_OPERATOR_LOGICAL_AND:
            case C2MP_OPERATOR_LOGICAL_OR:
            case C2MP_OPERATOR_BINARY_MINUS:
            case C2MP_OPERATOR_BINARY_DIVIDE:
            case C2MP_OPERATOR_LOWER_THAN:
            case C2MP_OPERATOR_GREATER_THAN:
            case C2MP_OPERATOR_LOWER_OR_EQUAL:
            case C2MP_OPERATOR_GREATER_OR_EQUAL:
            case C2MP_FUNCTION_POW:
                usedVariables = addReference(usedVariables, q->operands[0].reference);
                usedVariables = addReference(usedVariables, q->operands[1].reference);
                break;
            case C2MP_FUNCTION_UNKNOWN:
                for(int i = 0; i < q->operandsNum; i++)
                {
                    usedVariables = addReference(usedVariables, q->operands[i].reference);
                }
                break;
            case C2MP_QUAD_IF:
            case C2MP_QUAD_WHILE:
            case C2MP_QUAD_DOWHILE:
                ++blocDepth;
                break;
            case C2MP_QUAD_NO_ASSIGNMENT:
            case C2MP_QUAD_ELSE:
                // nothing
                break;
            case C2MP_QUAD_ENDIF:
            case C2MP_QUAD_ENDWHILE:
            case C2MP_QUAD_ENDDOWHILE:
            	--blocDepth;
                break; 
            default:
            	panic("optimization.c","getUsedVariablesInBloc","Not recognized operator\n");
            	break;
		}
		q = q->next;
	} while(q != firstQuad && blocDepth > 0);
	
	return usedVariables;
}

quad* removeUselessTempFromBloc(quad* quads, quad* firstQuad, int assignment, int * usedInBloc)
{  
    quad * q = quads;
    int assignmentUsed = *usedInBloc;
    int i;

    do
    {
        switch(q->operator)
        {
            case C2MP_QUAD_ASSIGNMENT:
                if(assignmentUsed)
                    break;
                if(q->assignment == assignment && q->operands[0].reference != assignment)  
                    assignmentUsed = -1;
                else if(q->operands[0].reference == assignment)
                    assignmentUsed = 1;
                break;
            case C2MP_OPERATOR_BINARY_PLUS:
            case C2MP_OPERATOR_BINARY_DOT:
            case C2MP_OPERATOR_EQUAL:
            case C2MP_OPERATOR_NOT_EQUAL:
            case C2MP_OPERATOR_BITWISE_AND:
            case C2MP_OPERATOR_BITWISE_OR:
            case C2MP_OPERATOR_BITWISE_XOR:
            case C2MP_OPERATOR_LOGICAL_AND:
            case C2MP_OPERATOR_LOGICAL_OR:
            case C2MP_FUNCTION_SQRT:
            case C2MP_FUNCTION_POW:
            case C2MP_FUNCTION_SQR:
            case C2MP_FUNCTION_EXP:
            case C2MP_FUNCTION_LOG:
            case C2MP_FUNCTION_LOG10:
            case C2MP_FUNCTION_COS:
            case C2MP_FUNCTION_SIN:
            case C2MP_FUNCTION_COSH:
            case C2MP_FUNCTION_SINH:
            case C2MP_OPERATOR_BINARY_MINUS:
            case C2MP_OPERATOR_BINARY_DIVIDE:
            case C2MP_OPERATOR_LOWER_THAN:
            case C2MP_OPERATOR_GREATER_THAN:
            case C2MP_OPERATOR_LOWER_OR_EQUAL:
            case C2MP_OPERATOR_GREATER_OR_EQUAL:
            case C2MP_OPERATOR_UNARY_MINUS:
            case C2MP_OPERATOR_UNARY_PLUS:
            case C2MP_OPERATOR_LOGICAL_NOT:
            case C2MP_OPERATOR_BITWISE_NOT:
            case C2MP_FUNCTION_UNKNOWN:
                if(assignmentUsed)
                    break;
                for(i = 0; i < q->operandsNum; i++)
                {
                    if(q->operands[i].reference == assignment)
                    {
                        assignmentUsed = 1;
                        break;
                    }
                }
                break;
            case C2MP_QUAD_IF:
            case C2MP_QUAD_WHILE:
            case C2MP_QUAD_DOWHILE:
                if(q->assignment == assignment || q->operands[0].reference == assignment)
                    assignmentUsed = 1;
                q = removeUselessTempFromBloc(q->next, firstQuad,assignment,usedInBloc)->previous;
                break;
            case C2MP_QUAD_ENDIF:
            case C2MP_QUAD_ENDWHILE:
            case C2MP_QUAD_ENDDOWHILE:
                if(q->assignment == assignment)
                    *usedInBloc = *usedInBloc || 1;
                return q->next;
                break; 

            default:
                break;
        } 
        if(assignmentUsed == -1)
        {
            *usedInBloc = *usedInBloc || 0;
        }
        if(assignmentUsed)
        {
            *usedInBloc = *usedInBloc || 1;
        }
        q = q->next;
    }while(q != firstQuad);

    return firstQuad;
       
}


quad* removeUselessTemp(quad* quads)
{
    quad *firstQuad = quads;
    quad* q = quads;
    quad* quadPtr;
    int refOccurence;
    int assignment;
    int i;
    int caseToOptimize;
    int usedInBloc;
    do
    {
        switch(q->operator)
        {
            case C2MP_QUAD_ASSIGNMENT:
            case C2MP_OPERATOR_BINARY_PLUS:
            case C2MP_OPERATOR_BINARY_DOT:
            case C2MP_OPERATOR_EQUAL:
            case C2MP_OPERATOR_NOT_EQUAL:
            case C2MP_OPERATOR_BITWISE_AND:
            case C2MP_OPERATOR_BITWISE_OR:
            case C2MP_OPERATOR_BITWISE_XOR:
            case C2MP_OPERATOR_LOGICAL_AND:
            case C2MP_OPERATOR_LOGICAL_OR:
            case C2MP_FUNCTION_SQRT:
            case C2MP_FUNCTION_POW:
            case C2MP_FUNCTION_SQR:
            case C2MP_FUNCTION_EXP:
            case C2MP_FUNCTION_LOG:
            case C2MP_FUNCTION_LOG10:
            case C2MP_FUNCTION_COS:
            case C2MP_FUNCTION_SIN:
            case C2MP_FUNCTION_COSH:
            case C2MP_FUNCTION_SINH:
            case C2MP_OPERATOR_BINARY_MINUS:
            case C2MP_OPERATOR_BINARY_DIVIDE:
            case C2MP_OPERATOR_LOWER_THAN:
            case C2MP_OPERATOR_GREATER_THAN:
            case C2MP_OPERATOR_LOWER_OR_EQUAL:
            case C2MP_OPERATOR_GREATER_OR_EQUAL:
            case C2MP_OPERATOR_UNARY_MINUS:
            case C2MP_OPERATOR_UNARY_PLUS:
            case C2MP_OPERATOR_LOGICAL_NOT:
            case C2MP_OPERATOR_BITWISE_NOT:
            case C2MP_FUNCTION_UNKNOWN:
                caseToOptimize = 1;
                break;     
            default:
                caseToOptimize = 0;
                break;
        }

        if(caseToOptimize)
        {
            if(getSymbolFromReference(q->assignment).isTemp)
            {
                usedInBloc = 0;
                refOccurence = 0;
                quadPtr = q->next;
                assignment = q->assignment;
                if(getSymbolFromReference(q->assignment).isBlockCondition)
                {
                    refOccurence = 1;
                    quadPtr = firstQuad;
                }
                if(q->operands[0].reference == assignment)
                {
                    refOccurence = 0;
                    quadPtr = firstQuad;
                }
 
                 while (quadPtr != firstQuad)
                {
                    switch(quadPtr->operator)
                    {
                        case C2MP_QUAD_ASSIGNMENT:
                            if(quadPtr->assignment == assignment && q->operands[0].reference != assignment)
                                refOccurence = -1;
                            if(quadPtr->operands[0].reference == assignment)
                               refOccurence = 1;
                            break;
                        case C2MP_QUAD_IF:
                        case C2MP_QUAD_WHILE:
                        case C2MP_QUAD_DOWHILE:
                            if(quadPtr->assignment == assignment || quadPtr->operands[0].reference == assignment)
                            {
                                refOccurence = 1;
                                break;
                            }
                            quadPtr = removeUselessTempFromBloc(quadPtr->next, firstQuad, assignment, &usedInBloc)->previous;
                            break;
                        case C2MP_QUAD_ENDIF:
                        case C2MP_QUAD_ENDWHILE:
                        case C2MP_QUAD_ENDDOWHILE:
                            if(quadPtr->assignment == assignment)
                            {
                                refOccurence = 1;
                                break;
                            }
                        default:
                            break;
                    }
                    if(refOccurence == -1)
                    {
                        refOccurence = 0;
                        break;
                    }
                    for(i = 0; i < quadPtr->operandsNum; i++)
                    {
                        if(quadPtr->operands[i].reference == q->assignment)
                        {
                            refOccurence = 1;
                            break;
                        }
                    }
                    if(refOccurence)
                        break;

                    quadPtr = quadPtr->next;
                }
                if(!refOccurence && !usedInBloc)
                {
                    /*We can delete this temp TODO firstc*/
                    quad * toFree = q;
                    q->next->previous = q->previous;
                    q->previous->next = q->next;
                    q = q->previous;
                    if(toFree == firstQuad)
                        firstQuad = toFree->next;
                    free(toFree);
                }
            }
        }
        q = q->next;
    }while(q != firstQuad);

    return firstQuad;
}

quad* removeAllCommonSubExpressions(quad* quads)
{
    quad *firstQuad = quads;
    quad* q = quads;

    q = removeCommonSubExpression(q, firstQuad);

    do
    {
        switch(q->operator)
        {
            case C2MP_QUAD_IF:
            case C2MP_QUAD_WHILE:
            case C2MP_QUAD_DOWHILE:
                q = removeCommonSubExpression(q->next, firstQuad)->previous; // not having ->previous makes it loop infinitely on test_invariant.c
                break;
            default:
                break;
        }
        q=q->next;
    }while(q != firstQuad);

    return firstQuad;
}


quad* removeCommonSubExpression(quad* quads, quad* firstQuad)
{
	if(quads == NULL)
    {
        printf("no quads for optimization\n");
    }
	
    resetTables();
	quad* q = quads;
	int optimizationRefOp1;
	int optimizationRefOp2;
	int isCommutative;
	int refTableIndex;
	int operandOptimizationRef;
	
	do
	{
		/*ignoring quad which aren't expr or assignment,
		 which means with voidOperand*/
		optimizationRefOp1 = -1;
		optimizationRefOp2 = -1;
		isCommutative = 0;
		switch(q->operator)
		{
			case C2MP_QUAD_ASSIGNMENT:
				operandOptimizationRef = getOperandOptimizationRef(q->operands[0].reference);

				
				if((refTableIndex = findRefTable(q->assignment)) < 0)
				{
					refTableIndex=createRefTableRow(q->assignment);
				}

				refTable[refTableIndex].optimizationRef = 
					operandOptimizationRef;
				break;

            case C2MP_OPERATOR_BINARY_PLUS:
            case C2MP_OPERATOR_BINARY_DOT:
            case C2MP_OPERATOR_EQUAL:
            case C2MP_OPERATOR_NOT_EQUAL:
            case C2MP_OPERATOR_BITWISE_AND:
            case C2MP_OPERATOR_BITWISE_OR:
            case C2MP_OPERATOR_BITWISE_XOR:
            case C2MP_OPERATOR_LOGICAL_AND:
            case C2MP_OPERATOR_LOGICAL_OR:
            
            	isCommutative = 1;
                __attribute__ ((fallthrough));
                /*Remove GCC fallthrough warning)*/
            case C2MP_OPERATOR_BINARY_MINUS:
            case C2MP_OPERATOR_BINARY_DIVIDE:
            case C2MP_OPERATOR_LOWER_THAN:
            case C2MP_OPERATOR_GREATER_THAN:
            case C2MP_OPERATOR_LOWER_OR_EQUAL:
            case C2MP_OPERATOR_GREATER_OR_EQUAL:
            case C2MP_FUNCTION_POW:

				optimizationRefOp2 = getOperandOptimizationRef(q->operands[1].reference);
                __attribute__ ((fallthrough));

            case C2MP_OPERATOR_UNARY_MINUS:
            case C2MP_OPERATOR_UNARY_PLUS:
            case C2MP_OPERATOR_LOGICAL_NOT:
            case C2MP_OPERATOR_BITWISE_NOT:
            case C2MP_FUNCTION_SQRT:
            //case C2MP_FUNCTION_ABS:
            case C2MP_FUNCTION_EXP:
            case C2MP_FUNCTION_LOG:
            case C2MP_FUNCTION_LOG10:
            case C2MP_FUNCTION_COS:
            case C2MP_FUNCTION_SIN:
            case C2MP_FUNCTION_COSH:
            case C2MP_FUNCTION_SINH:
            case C2MP_FUNCTION_SQR:

				optimizationRefOp1 = getOperandOptimizationRef(q->operands[0].reference);
				
				if((refTableIndex = findRefTable(q->assignment)) < 0)
				{
					refTableIndex=createRefTableRow(q->assignment);
				}

				/*handle commutativity*/
				if(isCommutative)
				{
					if(optimizationRefOp2 < optimizationRefOp1)
					{//todo pour les commutative op only
						int temp = optimizationRefOp1;
						optimizationRefOp1 = optimizationRefOp2;
						optimizationRefOp2 = temp;
					}
				}

				int exprHashTableIndex = findExprHashTable(q->operator,
											optimizationRefOp1,
											optimizationRefOp2);
				if(exprHashTableIndex < 0)
				{
					exprHashTableIndex = createExprHash(q->operator,
											optimizationRefOp1,
											optimizationRefOp2);
				}

				setOptimizationRef(refTableIndex,exprHashTableIndex);

				int optimizationRefUsed;
				if((optimizationRefUsed = 
						findOptimizationRefTable(refTable[refTableIndex].optimizationRef)) >= 0)
				{
					if(optimizationRefUsed != refTableIndex)
					{
						optimizeExprToAssignment(q,refTable[optimizationRefUsed].reference);
					}
				}	
                break;
            case C2MP_QUAD_NO_ASSIGNMENT:
            case C2MP_FUNCTION_UNKNOWN:
                break;
            case C2MP_QUAD_IF:
            case C2MP_QUAD_WHILE:
            case C2MP_QUAD_DOWHILE:
                q = ignoreBlocForCommonSubExpression(q->next, firstQuad);
                break;
            case C2MP_QUAD_ELSE:
            case C2MP_QUAD_ENDIF:
            case C2MP_QUAD_ENDWHILE:
            case C2MP_QUAD_ENDDOWHILE:
            	return quads;
                break; 
            default:
            	panic("optimization.c","removeCommonSubExpressions","Not recognized operator\n");
            	break;
		}
		q = q->next;
	} while(q != firstQuad);

	return quads;
}

quad* ignoreBlocForCommonSubExpression(quad* quads, quad* firstQuad)
{
    if(quads == NULL)
    {
        printf("no quads for optimization\n");
    }

    quad* q = quads;
    
    do
    {
        switch(q->operator)
        {
            case C2MP_QUAD_ASSIGNMENT:
            case C2MP_OPERATOR_BINARY_PLUS:
            case C2MP_OPERATOR_BINARY_DOT:
            case C2MP_OPERATOR_EQUAL:
            case C2MP_OPERATOR_NOT_EQUAL:
            case C2MP_OPERATOR_BITWISE_AND:
            case C2MP_OPERATOR_BITWISE_OR:
            case C2MP_OPERATOR_BITWISE_XOR:
            case C2MP_OPERATOR_LOGICAL_AND:
            case C2MP_OPERATOR_LOGICAL_OR:
            case C2MP_FUNCTION_SQRT:
            case C2MP_FUNCTION_POW:
            //case C2MP_FUNCTION_ABS:
            case C2MP_FUNCTION_EXP:
            case C2MP_FUNCTION_LOG:
            case C2MP_FUNCTION_LOG10:
            case C2MP_FUNCTION_COS:
            case C2MP_FUNCTION_SIN:
            case C2MP_FUNCTION_COSH:
            case C2MP_FUNCTION_SINH:
            case C2MP_FUNCTION_SQR:
            case C2MP_OPERATOR_BINARY_MINUS:
            case C2MP_OPERATOR_BINARY_DIVIDE:
            case C2MP_OPERATOR_LOWER_THAN:
            case C2MP_OPERATOR_GREATER_THAN:
            case C2MP_OPERATOR_LOWER_OR_EQUAL:
            case C2MP_OPERATOR_GREATER_OR_EQUAL:
            case C2MP_OPERATOR_UNARY_MINUS:
            case C2MP_OPERATOR_UNARY_PLUS:
            case C2MP_OPERATOR_LOGICAL_NOT:
            case C2MP_OPERATOR_BITWISE_NOT:
                // printf("%s\n",getSymbolFromReference(q->assignment).name);
                assignNewOptimizationRef(q->assignment);
                break;
            case C2MP_FUNCTION_UNKNOWN:
                break;
            case C2MP_QUAD_IF:
            case C2MP_QUAD_WHILE:
            case C2MP_QUAD_DOWHILE:
                q = ignoreBlocForCommonSubExpression(q->next, firstQuad);
                break;
            case C2MP_QUAD_NO_ASSIGNMENT:
            case C2MP_QUAD_ELSE:
                // ignore
                break;
            case C2MP_QUAD_ENDIF:
            case C2MP_QUAD_ENDWHILE:
            case C2MP_QUAD_ENDDOWHILE:
                return q;
                break; 
            default:
                panic("optimization.c","ignoreBlocForCommonSubExpression","Not recognized operator\n");
                break;
        }
        q = q->next;
    } while(q != firstQuad);

    return q;
}

void optimizeExprToAssignment(quad* q, int reference)
{
	q->operator = C2MP_QUAD_ASSIGNMENT;
	q->operands[0] = createVariableOperand(reference);
	q->operands[1] = createVoidOperand();
    q->operands[1].reference = -1;
	q->operandsNum = C2MP_QUAD_UNARY;
}

void assignNewOptimizationRef(int reference)
{
    int refOp = findRefTable(reference);
    if((refOp) < 0)
    {
        refOp=createRefTableRow(reference);
    }
    refTable[refOp].optimizationRef = refCount;
    refCount++;
}

void setOptimizationRef(int reference, int hashReference)
{
	refTable[reference].optimizationRef = 
				exprHashTable[hashReference].optimizationRef;	
}

int getOperandOptimizationRef(int reference)
{
	int refOp = findRefTable(reference);
	if((refOp) < 0)
	{
		refOp=createRefTableRow(reference);
		refTable[refOp].optimizationRef = refCount;
		refCount++;
	}	
	return refTable[refOp].optimizationRef;
}

int createExprHash(char op, int operand1, int operand2)
{
	exprHashTable[exprHashSize].operator = op;
	exprHashTable[exprHashSize].operand1 = operand1;
	exprHashTable[exprHashSize].operand2 = operand2;
	exprHashTable[exprHashSize].optimizationRef = refCount;
	int exprHashTableIndex = exprHashSize;
	refCount++;
	exprHashSize++;
	return exprHashTableIndex;
}

int createRefTableRow(int reference)
{
	refTable[refSize].reference = reference;
	int refTableIndex = refSize;
	refSize++;
	return refTableIndex;
}

int findOptimizationRefTable(int reference)
{
	int i = 0;
	for(i = 0; i < refSize;i++)
	{
		if(refTable[i].optimizationRef == reference)
		{
			return i;
		}
	}
	return -1;
}

int findRefTable(int reference)
{
	int i = 0;
	for(i = 0; i < refSize;i++)
	{
		if(refTable[i].reference == reference)
		{
			return i;
		}
	}
	return -1;
}

int findExprHashTable(char op, int operand1, int operand2)
{
	int i = 0;
	for(i = 0; i < exprHashSize;++i)
	{
		if(   exprHashTable[i].operator == op 
		   && exprHashTable[i].operand1 == operand1
		   && exprHashTable[i].operand2 == operand2
		  )
		{
			return i;
		}
	}
	return -1;
}

int findConstantTable(int reference)
{
	int i = 0;
	for(i = 0; i < constSize;++i)
	{
		if(constTable[i].reference == reference)
		{
			return i;
		}
	}
	return -1;
}

void resetTables(void)
{
	refSize = 0;
	refCount = 0;
	exprHashSize = 0;
	constSize = 0;
}