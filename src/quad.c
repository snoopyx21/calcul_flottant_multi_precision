#include <stdarg.h>
#include "quad.h"

quadOperand createVariableOperand(int reference)
{
    quadOperand operand;
    operand.type = C2MP_QUAD_OPERAND_VARIABLE;
    operand.reference = reference;
    return operand;
}

quadOperand createIntegerOperand(int value)
{
    quadOperand operand;
    operand.type = C2MP_QUAD_OPERAND_INTEGER;
    operand.valueInt = value;
    return operand;
}

quadOperand createFloatOperand(double value)
{
    quadOperand operand;
    operand.type = C2MP_QUAD_OPERAND_FLOAT;
    operand.valueFloat = value;
    return operand;
}

quadOperand createStringOperand(char *string)
{
    quadOperand operand;
    operand.type = C2MP_QUAD_OPERAND_STRING;
    operand.valueString = strdup(string);
    return operand;
}

quadOperand createVoidOperand(void)
{
    quadOperand operand;
    operand.type = -1;
    return operand;
}

quad *createQuad(int assignment, char operator, char * name, int operandsNum, ...)
{
    quad *q = malloc(sizeof(quad));
    q->assignment = assignment;
    q->operator = operator;
    q->operandsNum = operandsNum;

    if (operator == C2MP_FUNCTION_UNKNOWN)
    {
        q->fctName = strdup(name);
    }
    
    va_list operandsList;
    va_start(operandsList, operandsNum);

    for (int i = 0; i < MAX_FCT_ARGS; i++)
    { // fills the quad arguments
        if (i < operandsNum)
        {
            q->operands[i] = va_arg(operandsList, quadOperand);
        }
        else
        {
            q->operands[i].reference = -1;
        }
    }

    va_end(operandsList);

    q->next = q;
    q->previous = q;

    return q;
}

void freeQuads(quad *q)
{
    quad *firstQuad = q;
    quad *currentQuad = q->next, *toFree;

    if (q == NULL)
    {
        return;
    }

    while(currentQuad != firstQuad)
    {
        toFree = currentQuad;
        currentQuad = currentQuad->next;
        if (toFree->operator == C2MP_FUNCTION_UNKNOWN)
        {
            free(toFree->fctName);
        }
        free(toFree);
    }

    free(firstQuad);
}

quad *copySemiQuad(semiQuad *sq)
{
    if(sq == NULL)
    {
        fprintf(stderr, "Warning, tried to copy quads from NULL semiQuad\n");
        return NULL;
    }

    quad *generatedQuads;
    switch(sq->operator)
    {
        case C2MP_QUAD_NO_ASSIGNMENT:
            // Only custom functions here!
            generatedQuads = generateQuadsFromAST(sq->expression);
            generatedQuads->previous->assignment = -1;
            generatedQuads->previous->operator = C2MP_QUAD_NO_ASSIGNMENT;
            return generatedQuads;
            break;

        case C2MP_QUAD_ASSIGNMENT:
            generatedQuads = generateQuadsFromAST(sq->expression);
            if (sq->expression->operator == C2MP_FUNCTION_UNKNOWN)
            {
                generatedQuads->previous->assignment = sq->assignment;
                return generatedQuads;
            }
            return concatQuads(generatedQuads, 
                        createQuad(sq->assignment, sq->operator, NULL, C2MP_QUAD_UNARY, 
                            createVariableOperand(generatedQuads->previous->assignment)));
            break;
    
        case C2MP_QUAD_IF:
            generatedQuads = generateQuadsFromAST(sq->expression);
            setSymbolToBlockCondition(generatedQuads->previous->assignment);
            return concatQuads(generatedQuads, 
                        createQuad(sq->assignment, sq->operator, NULL, C2MP_QUAD_UNARY, 
                            createVariableOperand(generatedQuads->previous->assignment)));
            break;

        case C2MP_QUAD_ELSE:
        case C2MP_QUAD_ENDIF:
            return createQuad(-1, sq->operator, NULL, 0);
            break;

        case C2MP_QUAD_WHILE:
            generatedQuads = generateQuadsFromAST(sq->expression);
            generatedQuads = concatQuads(generatedQuads, 
                                createQuad(sq->assignment,
                                    C2MP_QUAD_ASSIGNMENT,
                                    NULL,
                                    C2MP_QUAD_UNARY,
                                    createVariableOperand(generatedQuads->previous->assignment)));
            generatedQuads = concatQuads(generatedQuads, 
                                    createQuad(sq->assignment, sq->operator, NULL, 0));
            return generatedQuads;
            break;
        case C2MP_QUAD_DOWHILE:
            return createQuad(-1, sq->operator, NULL, 0);
            break;
        case C2MP_QUAD_ENDWHILE:
        case C2MP_QUAD_ENDDOWHILE:
            generatedQuads = generateQuadsFromAST(sq->expression);
            generatedQuads = concatQuads(generatedQuads, 
                                createQuad(sq->assignment,
                                    C2MP_QUAD_ASSIGNMENT,
                                    NULL,
                                    C2MP_QUAD_UNARY,
                                    createVariableOperand(generatedQuads->previous->assignment)));
            generatedQuads = concatQuads(generatedQuads, 
                                createQuad(sq->assignment, sq->operator, NULL, 0));
            return generatedQuads;
            break;
        default:
            fprintf(stderr, "Warning, unknown semi quad operation : %d (%c)\n",
                sq->operator, sq->operator);
    }

    return NULL;
}


// TO BE EFFICIENT : C2MP_OPERATOR_LOGICAL_AND and C2MP_OPERATOR_LOGICAL_OR 
// should not do this big logic with if then else if the remaining tree is small
// TODO
// example 1 : a && (3*x*y*z+3241 > 3*d/g) in this case it is smart to check 
// for the second operand only if a is true
// example 2 : a && b in this case it is better just to do : temporary = a && b
/* read AST : a<b&&b<c become :
 *
 *     &&
 *  <       <
 * a  b   b   c
*/
quad *generateQuadsFromAST(expressionAST *expr)
{
    if(expr == NULL)
    {
        fprintf(stderr, "Warning, tried to generate quads from NULL AST\n");
        return NULL;
    }

    quad *quadExpr1, *quadExpr2, *quadExpr, *finalQuads = NULL;
    quadOperand opeList[MAX_FCT_ARGS];
    expressionAST *opeAST;
    symbolType tempType = MPC_T;
    int reference1, reference2, reference;
    int resultTemporary, resultZero, resultRef1;
    switch(expr->operator)
    {
        case C2MP_OPERATOR_LOGICAL_AND:
            quadExpr1 = generateQuadsFromAST(expr->expression.e1);
            quadExpr2 = generateQuadsFromAST(expr->expression.e2);

            reference1 = quadExpr1->previous->assignment;
            reference2 = quadExpr2->previous->assignment;

            resultRef1 = newTemp(MPC_T, false).reference;
            resultZero = newTemp(MPC_T, false).reference;
            resultTemporary = newTemp(INTEGER_NUMBER, true).reference;


            /* generate these quads :
            a = ...
            t = a!=0
            if(t)
            {
                b = ...
                t = b
            }
            else
            {
                t=0
            }

            a result of expr1
            b result of expr2
            */
            finalQuads = concatQuads(quadExpr1,
                            createQuad(resultRef1,
                                C2MP_QUAD_ASSIGNMENT,
                                NULL,
                                C2MP_QUAD_UNARY,
                               createVariableOperand(reference1))); // if expr1 == 0
            finalQuads = concatQuads(quadExpr1,
                            createQuad(resultZero,
                                C2MP_QUAD_ASSIGNMENT,
                                NULL,
                                C2MP_QUAD_UNARY,
                                createIntegerOperand(0))); // if expr1 == 0
            finalQuads = concatQuads(finalQuads,
                            createQuad(resultTemporary,
                                C2MP_OPERATOR_NOT_EQUAL,
                                NULL,
                                C2MP_QUAD_BINARY,
                                createVariableOperand(resultRef1),
                                createVariableOperand(resultZero))); // if expr1 == 0

            finalQuads = concatQuads(finalQuads,
                            createQuad(-1, C2MP_QUAD_IF, NULL, 
                                C2MP_QUAD_UNARY,
                                createVariableOperand(resultTemporary))); // if resultTemporary != 0
            finalQuads = concatQuads(finalQuads, quadExpr2);
            finalQuads = concatQuads(finalQuads,
                            createQuad(resultTemporary,
                                C2MP_QUAD_ASSIGNMENT,
                                NULL,
                                C2MP_QUAD_UNARY,
                                createVariableOperand(reference2))); // t = expr2
            finalQuads = concatQuads(quadExpr1, 
                                     createQuad(-1, C2MP_QUAD_ELSE, NULL, 0)); // else
            finalQuads = concatQuads(finalQuads,
                            createQuad(resultTemporary,
                                C2MP_QUAD_ASSIGNMENT, NULL, 
                                C2MP_QUAD_UNARY,
                                createIntegerOperand(0))); // t = 0
            finalQuads = concatQuads(quadExpr1,
                            createQuad(-1, C2MP_QUAD_ENDIF, NULL, 0)); // endif
            finalQuads = concatQuads(finalQuads,
                            createQuad(resultTemporary,
                                C2MP_QUAD_ASSIGNMENT, NULL, C2MP_QUAD_UNARY,
                                createVariableOperand(resultTemporary)));
                    // t = t, needed for generation and will be optimized

            return finalQuads;
            break;

        case C2MP_OPERATOR_LOGICAL_OR:
            quadExpr1 = generateQuadsFromAST(expr->expression.e1);
            quadExpr2 = generateQuadsFromAST(expr->expression.e2);

            reference1 = quadExpr1->previous->assignment;
            reference2 = quadExpr2->previous->assignment;

            resultRef1 = newTemp(MPC_T, false).reference;
            resultZero = newTemp(MPC_T, false).reference;
            resultTemporary = newTemp(INTEGER_NUMBER, true).reference;

            /* generate these quads :
            a = ...
            t = a==0
            if(t)
            {
                b = ...
                t = b
            }
            else
            {
                t=1
            }

            a result of expr1
            b result of expr2
            */
            finalQuads = concatQuads(quadExpr1,
                            createQuad(resultRef1,
                                C2MP_QUAD_ASSIGNMENT,
                                NULL,
                                C2MP_QUAD_UNARY,
                               createVariableOperand(reference1))); // if expr1 == 0
            finalQuads = concatQuads(quadExpr1,
                            createQuad(resultZero,
                                C2MP_QUAD_ASSIGNMENT,
                                NULL,
                                C2MP_QUAD_UNARY,
                                createVariableOperand(0))); // if expr1 == 0

            finalQuads = concatQuads(finalQuads,
                            createQuad(resultTemporary,
                                C2MP_OPERATOR_NOT_EQUAL,
                                NULL,
                                C2MP_QUAD_BINARY,
                                createVariableOperand(resultRef1),
                                createVariableOperand(resultZero))); // if expr1 == 0

            finalQuads = concatQuads(finalQuads,
                            createQuad(-1,
                                C2MP_QUAD_IF,
                                NULL,
                                C2MP_QUAD_UNARY,
                                // if resultTemporary == 0
                                createVariableOperand(resultTemporary))); 
            finalQuads = concatQuads(finalQuads, quadExpr2);
            finalQuads = concatQuads(finalQuads,
                            createQuad(resultTemporary,
                                C2MP_QUAD_ASSIGNMENT,
                                NULL,
                                C2MP_QUAD_UNARY,
                                createVariableOperand(reference2))); // t = expr2
            finalQuads = concatQuads(quadExpr1,
                            createQuad(-1, C2MP_QUAD_ELSE, NULL, 0)); // else
            finalQuads = concatQuads(finalQuads,
                            createQuad(resultTemporary,
                                C2MP_QUAD_ASSIGNMENT,
                                NULL,
                                C2MP_QUAD_UNARY,
                                createIntegerOperand(1))); // t = 1
            finalQuads = concatQuads(quadExpr1,
                            createQuad(-1,
                                C2MP_QUAD_ENDIF, NULL, 0)); // endif
            finalQuads = concatQuads(finalQuads,
                            createQuad(resultTemporary,
                                C2MP_QUAD_ASSIGNMENT,
                                NULL,
                                C2MP_QUAD_UNARY,
                                createVariableOperand(resultTemporary)));
                    // t = t, needed for generation and will be optimized

            return finalQuads;
            break;

        case C2MP_OPERATOR_LOWER_THAN:
        case C2MP_OPERATOR_GREATER_THAN:
        case C2MP_OPERATOR_LOWER_OR_EQUAL:
        case C2MP_OPERATOR_GREATER_OR_EQUAL:
        case C2MP_OPERATOR_EQUAL:
        case C2MP_OPERATOR_NOT_EQUAL:
        case C2MP_OPERATOR_BITWISE_AND:
        case C2MP_OPERATOR_BITWISE_OR:
        case C2MP_OPERATOR_BITWISE_XOR:
            tempType = INTEGER_NUMBER;
            __attribute__ ((fallthrough));
        case C2MP_OPERATOR_BINARY_PLUS:
        case C2MP_OPERATOR_BINARY_MINUS:
        case C2MP_OPERATOR_BINARY_DOT:
        case C2MP_OPERATOR_BINARY_DIVIDE:
        /******************** FUNCTIONS ********************/
        case C2MP_FUNCTION_POW:
            quadExpr1 = generateQuadsFromAST(expr->expression.e1);
            quadExpr2 = generateQuadsFromAST(expr->expression.e2);

            // the reference is the assigned variable of the last quad
            reference1 = quadExpr1->previous->assignment;
            reference2 = quadExpr2->previous->assignment;

            finalQuads = concatQuads(quadExpr1, quadExpr2);
            finalQuads = concatQuads(finalQuads, 
                            createQuad(newTemp(tempType, false).reference, expr->operator,
                                NULL,
                                C2MP_QUAD_BINARY,
                                createVariableOperand(reference1), 
                                createVariableOperand(reference2)));

            return finalQuads;
            break;

        case C2MP_OPERATOR_UNARY_MINUS:
        case C2MP_OPERATOR_UNARY_PLUS:
        case C2MP_OPERATOR_LOGICAL_NOT:
        case C2MP_OPERATOR_BITWISE_NOT:
        /******************** FUNCTIONS ********************/
        //case C2MP_FUNCTION_ABS:
        case C2MP_FUNCTION_EXP:
        case C2MP_FUNCTION_COS:
        case C2MP_FUNCTION_COSH:
        case C2MP_FUNCTION_LOG:
        case C2MP_FUNCTION_LOG10:
        case C2MP_FUNCTION_SIN:
        case C2MP_FUNCTION_SINH:
        case C2MP_FUNCTION_SQRT:
        case C2MP_FUNCTION_SQR:

            quadExpr = generateQuadsFromAST(expr->expression.e1);

            // the reference is the assigned variable of the last quad
            reference = quadExpr->previous->assignment;

            finalQuads = concatQuads(quadExpr, 
                            createQuad(newTemp(MPC_T, false).reference, expr->operator,
                                NULL,
                                C2MP_QUAD_UNARY,
                                createVariableOperand(reference)));

            return finalQuads;
            break;

        case C2MP_CHARACTER_INTEGER: // number
            return createQuad(newTemp(MPC_T, false).reference, C2MP_QUAD_ASSIGNMENT,
                        NULL,
                        C2MP_QUAD_UNARY,
                        createIntegerOperand(expr->valueInt));
            break;

        case C2MP_CHARACTER_FLOAT: // float

            return createQuad(newTemp(MPC_T, false).reference, C2MP_QUAD_ASSIGNMENT,
                        NULL,
                        C2MP_QUAD_UNARY,
                        createFloatOperand(expr->valueFloat));
            break;

        case C2MP_CHARACTER_VARIABLE: // variable
            return createQuad(newTemp(MPC_T, false).reference, C2MP_QUAD_ASSIGNMENT,
                        NULL,
                        C2MP_QUAD_UNARY,
                        createVariableOperand(expr->valueVariable));
            break;
        
        /**
         *  NOTE: Strings are not generating any quad, they are just printed
         *  in the resulting code generated by the code generation. For that,
         *  we create a string quadOperand and add it to the operand list of
         *  the quad that needs the string (generally a custom function).
         */

        case C2MP_FUNCTION_UNKNOWN:  // custom function
            // At the moment, we do NOT generate quads for the arguments !
            for (int i = 0; i < expr->customFunction.argnum; i++)
            {                
                opeAST = expr->customFunction.args[i];
                switch (opeAST->operator)
                {
                    case C2MP_CHARACTER_STRING:
                        opeList[i] = createStringOperand(opeAST->valueString);
                        break;
                    case C2MP_CHARACTER_FLOAT:
                        opeList[i] = createFloatOperand(opeAST->valueFloat);
                        break;
                    case C2MP_CHARACTER_INTEGER:
                        opeList[i] = createIntegerOperand(opeAST->valueInt);
                        break;
                    case C2MP_CHARACTER_VARIABLE:
                        opeList[i] = createVariableOperand(opeAST->valueVariable);
                        break;
                    //case C2MP_FUNCTION_ABS:
                    case C2MP_FUNCTION_COS:
                    case C2MP_FUNCTION_COSH:
                    case C2MP_FUNCTION_EXP:
                    case C2MP_FUNCTION_LOG:
                    case C2MP_FUNCTION_LOG10:
                    case C2MP_FUNCTION_POW:
                    case C2MP_FUNCTION_SIN:
                    case C2MP_FUNCTION_SQRT:
                    case C2MP_OPERATOR_BINARY_DIVIDE:
                    case C2MP_OPERATOR_BINARY_PLUS:
                    case C2MP_OPERATOR_BINARY_MINUS:
                    case C2MP_OPERATOR_BINARY_DOT:
                        quadExpr = generateQuadsFromAST(expr->customFunction.args[i]);
                        reference = quadExpr->previous->assignment;
                        opeList[i] = createVariableOperand(reference);
                        finalQuads = concatQuads(finalQuads, quadExpr);
                        finalQuads = concatQuads(finalQuads, 
                                     createQuad(newTemp(MPC_T, false).reference, C2MP_QUAD_ASSIGNMENT,
                                     NULL,
                                     C2MP_QUAD_UNARY,
                                     opeList[i]));
                        break;
                    default:
                        panic("quad.c", "generateQuadsFromAST", "unparsed function argument");
                        break;
                }
            }
            // Default behaviour with assignement to the function !
            // we need to know if there is an assignment or not...
            quadExpr1 = createQuadFromOperandList(newTemp(MPC_T, false).reference,
                                             C2MP_FUNCTION_UNKNOWN, 
                                             expr->customFunction.name,
                                             expr->customFunction.argnum,
                                             opeList);
            if (finalQuads != NULL)                    
                return concatQuads(finalQuads, quadExpr1);
            else
                return quadExpr1;
            break;

        default:
            fprintf(stderr, "Warning, unknown expression operation : %c\n", expr->operator);
    }

    return NULL;
}

quad *createQuadFromOperandList(int assignment, char operator, char *name, int argsNum, quadOperand *list)
{
    quad *q = malloc(sizeof(quad));
    q->operator = operator;
    q->operandsNum = argsNum;
    q->assignment = assignment;
    if (operator == C2MP_FUNCTION_UNKNOWN)
    {
        q->fctName = strdup(name);
        for (int i = 0; i < argsNum; i++)
        {
            q->operands[i] = list[i];
        }
        q->next = q;
        q->previous = q;
        return q;
    }
    else {
        fprintf(stderr, "createQuadFromChainedList:");
        fprintf(stderr, "Warning: not supported operator. Returning NULL quad.");
        free(q);
        return NULL;
    }
}

quad *getQuadsFromSemiQuads(semiQuad *sq)
{
    if(sq == NULL)
    {
        printf("no quad\n");
    }

    semiQuad *firstSemiQuad = sq;
    semiQuad *currentSemiQuad = sq;

    quad *currentQuad = copySemiQuad(firstSemiQuad);

    currentSemiQuad = currentSemiQuad->next;

    // 
    while(currentSemiQuad != firstSemiQuad)
    {
        currentQuad = concatQuads(currentQuad, copySemiQuad(currentSemiQuad));

        currentSemiQuad = currentSemiQuad->next;
    }

    // now we can free the semiQuads !
    freeSemiQuad(sq);

    return currentQuad;
}

quad *concatQuads(quad *q1, quad *q2)
{
    if(q2 == NULL)
    {
        return q1;
    }
    if(q1 == NULL)
    {
        return q2;
    }

    quad *head1 = q1;
    quad *head2 = q2;
    quad *tail1 = q1->previous;
    quad *tail2 = q2->previous;

    head1->previous = tail2;
    tail1->next = head2;
    head2->previous = tail1;
    tail2->next = head1;

    return q1;
}

symbolType getSymbolTypeFromOperand(quadOperand ope)
{
    switch (ope.type)
    {
        case C2MP_QUAD_OPERAND_INTEGER:
            return INTEGER_NUMBER;

        case C2MP_QUAD_OPERAND_FLOAT:
            return FLOAT_NUMBER;

        case C2MP_QUAD_OPERAND_VARIABLE:
            return getSymbolFromReference(ope.reference).type_symbol;

        default:
            panic("quad.c", "getSymbolTypeFromOperan", "Unknown symbol type");
    }
    return ERROR;
}

void printOperand(quadOperand operand)
{
    switch(operand.type)
    {
        case C2MP_QUAD_OPERAND_INTEGER:
            printf("%d", operand.valueInt);
            break;
        case C2MP_QUAD_OPERAND_FLOAT:
            printf("%f", operand.valueFloat);
            break;
        case C2MP_QUAD_OPERAND_VARIABLE:
            printf("%s", getNameFromReference(operand.reference));
            break;
        case C2MP_QUAD_OPERAND_STRING:
            printf("%s", operand.valueString);
            break;
        default:
            printf("Warning, unknown operand type : %d\n", operand.type);
    }
}

void outputOperand(quadOperand operand)
{
    switch(operand.type)
    {
        case C2MP_QUAD_OPERAND_INTEGER:
            fprintf(output, "%d", operand.valueInt);
            break;
        case C2MP_QUAD_OPERAND_FLOAT:
            fprintf(output, "%f", operand.valueFloat);
            break;
        case C2MP_QUAD_OPERAND_VARIABLE:
            fprintf(output, "%s", getNameFromReference(operand.reference));
            break;
        case C2MP_QUAD_OPERAND_STRING:
            fprintf(output, "%s", operand.valueString);
            break;
        default:
            fprintf(stderr, "Warning, unknown operand type : %d\n", operand.type);
    }
}

void printQuads(quad* q)
{
    if(q == NULL)
    {
        printf("No quads to print...\n");
    }

    quad *firstQuad = q;
    quad *currentQuad = q;

    int indent = 0;

    do
    {
        for(int i=0;i<indent;++i)
        { // indent quads
            printf("  ");
        }

        switch(currentQuad->operator)
        {
            case C2MP_OPERATOR_BINARY_PLUS:
            case C2MP_OPERATOR_BINARY_MINUS:
            case C2MP_OPERATOR_BINARY_DOT:
            case C2MP_OPERATOR_BINARY_DIVIDE:
            case C2MP_OPERATOR_LOWER_THAN:
            case C2MP_OPERATOR_GREATER_THAN:
            case C2MP_OPERATOR_LOWER_OR_EQUAL:
            case C2MP_OPERATOR_GREATER_OR_EQUAL:
            case C2MP_OPERATOR_EQUAL:
            case C2MP_OPERATOR_NOT_EQUAL:
            case C2MP_OPERATOR_BITWISE_AND:
            case C2MP_OPERATOR_BITWISE_OR:
            case C2MP_OPERATOR_BITWISE_XOR:
            case C2MP_OPERATOR_LOGICAL_AND:
            case C2MP_OPERATOR_LOGICAL_OR:
                printf("%s = ", getNameFromReference(currentQuad->assignment));
                printOperand(currentQuad->operands[0]);
                printf(" %c ", currentQuad->operator);
                printOperand(currentQuad->operands[1]);
                break;
            case C2MP_OPERATOR_UNARY_MINUS:
            case C2MP_OPERATOR_UNARY_PLUS:
            case C2MP_OPERATOR_LOGICAL_NOT:
            case C2MP_OPERATOR_BITWISE_NOT:
                printf("%s = %c ", 
                getNameFromReference(currentQuad->assignment),
                currentQuad->operator);
                printOperand(currentQuad->operands[0]);
                break;
            case C2MP_QUAD_ASSIGNMENT:
                printf("%s = ", getNameFromReference(currentQuad->assignment));
                printOperand(currentQuad->operands[0]);
                break;
            case C2MP_QUAD_IF:
                ++indent;
                printf("if ");
                printOperand(currentQuad->operands[0]);
                break;
            case C2MP_QUAD_ELSE:
                printf("\b\belse");
                break;
            case C2MP_QUAD_ENDIF:
                --indent;
                printf("\b\bendif");
                break;
            case C2MP_QUAD_WHILE:
                ++indent;
                printf("while %s", getNameFromReference(currentQuad->assignment));
                break;
            case C2MP_QUAD_DOWHILE:
                printf("do ");
                break;
            case C2MP_QUAD_ENDWHILE:
                --indent;
                printf("\b\bendwhile   // %s", getNameFromReference(currentQuad->assignment));
                break;
            case C2MP_QUAD_ENDDOWHILE:
                --indent;
                printf("\b\bwhile");
                break;
            case C2MP_FUNCTION_COS:
                printf("cos");
                break;
            case C2MP_FUNCTION_COSH:
                printf("cosh");
                break;
            case C2MP_FUNCTION_SIN:
                printf("sin");
                break;
            case C2MP_FUNCTION_SINH:
                printf("sinh");
                break;
            case C2MP_FUNCTION_EXP:
                printf("exp");
                break;
            case C2MP_FUNCTION_LOG:
                printf("log");
                break;
            case C2MP_FUNCTION_LOG10:
                printf("log10");
                break;
            case C2MP_FUNCTION_POW:
                printf("pow");
                break;
            case C2MP_FUNCTION_SQRT:
                printf("sqrt");
                break;
            case C2MP_FUNCTION_UNKNOWN:
                printf("%s", currentQuad->fctName);
                break;
            default:
                fprintf(stderr, "Warning, unknown quad operation : %c\n", currentQuad->operator);
        }

        printf("\n");

        currentQuad = currentQuad->next;
    }while(currentQuad != firstQuad);
}
