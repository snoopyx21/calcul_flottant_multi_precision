#include "ast.h"

expressionAST *createExpressionAST(char operator, expressionAST *expr1, expressionAST *expr2)
{
    expressionAST *expr = malloc(sizeof(expressionAST));
    expr->operator = operator;
    expr->expression.e1 = expr1;
    expr->expression.e2 = expr2;
    return expr;
}

expressionAST *createStringAST(const char *string)
{
    expressionAST *expr = malloc(sizeof(expressionAST));
    expr->operator = C2MP_CHARACTER_STRING;
    expr->valueString = strdup(string);
    return expr;
}

expressionAST *createIntAST(int integer)
{
    expressionAST *expr = malloc(sizeof(expressionAST));
    expr->operator = C2MP_CHARACTER_INTEGER;
    expr->valueInt = integer;
    return expr;
}

expressionAST *createFloatAST(double number)
{
    expressionAST *expr = malloc(sizeof(expressionAST));
    expr->operator = C2MP_CHARACTER_FLOAT;
    expr->valueFloat = number;
    return expr;
}

expressionAST *createVariableAST(int variable)
{
    expressionAST *expr = malloc(sizeof(expressionAST));
    expr->operator = C2MP_CHARACTER_VARIABLE;
    expr->valueVariable = variable;
    return expr;
}

expressionAST *createCustomFunctionAST(char *name, int argNum, struct expressionAST **list)
{
    expressionAST *expr = malloc(sizeof(expressionAST));
    expr->operator = C2MP_FUNCTION_UNKNOWN;

    expr->customFunction.name = strdup(name);
    expr->customFunction.argnum = argNum;

    // the arguments are stored in inverted order, we have to permut them
    for (int i = argNum-1; i >= 0; i--)
    {
        expr->customFunction.args[argNum-i-1] = list[i];
    }

    return expr;
}

expressionAST *copyExpressionAST(expressionAST *expressionAST)
{
    if(expressionAST == NULL)
    {
        return NULL;
    }

    struct expressionAST *currentAST = malloc(sizeof(struct expressionAST));
    currentAST->operator = expressionAST->operator;

    switch(expressionAST->operator)
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
        case C2MP_FUNCTION_POW:
            currentAST->expression.e1 = copyExpressionAST(expressionAST->expression.e1);
            currentAST->expression.e2 = copyExpressionAST(expressionAST->expression.e2);
            break;
        case C2MP_OPERATOR_UNARY_MINUS:
        case C2MP_OPERATOR_UNARY_PLUS:
        case C2MP_OPERATOR_LOGICAL_NOT:
        case C2MP_OPERATOR_BITWISE_NOT:
        case C2MP_FUNCTION_SQRT:
        case C2MP_FUNCTION_EXP:
        case C2MP_FUNCTION_LOG:
        case C2MP_FUNCTION_LOG10:
        case C2MP_FUNCTION_COS:
        case C2MP_FUNCTION_SIN:
        case C2MP_FUNCTION_COSH:
        case C2MP_FUNCTION_SINH:
        case C2MP_FUNCTION_SQR:
            currentAST->expression.e1 = copyExpressionAST(expressionAST->expression.e1);
            break;
        
        case C2MP_FUNCTION_UNKNOWN:
            for (int i = 0; i < expressionAST->customFunction.argnum; i++)
            {
                currentAST->customFunction.args[i] = copyExpressionAST(expressionAST->customFunction.args[i]);
            }
            break;

        case C2MP_CHARACTER_INTEGER: // number
            currentAST->valueInt = expressionAST->valueInt;
            break;
        case C2MP_CHARACTER_FLOAT: // float
            currentAST->valueFloat = expressionAST->valueFloat;
            break;
        case C2MP_CHARACTER_VARIABLE: // variable
            currentAST->valueVariable = expressionAST->valueVariable;
            break;
        case C2MP_CHARACTER_STRING: // string
            currentAST->valueString = strdup(expressionAST->valueString);
            break;
        default:
            fprintf(stderr, "Warning, unknown expression operation : %c (%d)\n", expressionAST->operator, expressionAST->operator);
    }

    return currentAST;
}


void freeExpressionAST(expressionAST *expr)
{
    if(expr == NULL)
    {
        fprintf(stderr, "Warning, tried to free a NULL expression\n");
        return;
    }

    switch(expr->operator)
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
            freeExpressionAST(expr->expression.e1);
            freeExpressionAST(expr->expression.e2);
            free(expr);
            break;
        case C2MP_OPERATOR_UNARY_MINUS: // unary minus
        case C2MP_OPERATOR_UNARY_PLUS: // unary plus
        case C2MP_OPERATOR_LOGICAL_NOT:
        case C2MP_OPERATOR_BITWISE_NOT:
            freeExpressionAST(expr->expression.e1);
            free(expr);
            break;
        case C2MP_CHARACTER_INTEGER: // number
        case C2MP_CHARACTER_FLOAT: // float
        case C2MP_CHARACTER_VARIABLE: // variable
        case C2MP_CHARACTER_STRING: // string
            free(expr);
            break;
        case C2MP_FUNCTION_POW:
            freeExpressionAST(expr->expression.e1);
            freeExpressionAST(expr->expression.e2);
            free(expr);
            break;
        case C2MP_FUNCTION_SQRT:
        //case C2MP_FUNCTION_ABS:
        case C2MP_FUNCTION_EXP:
        case C2MP_FUNCTION_LOG:
        case C2MP_FUNCTION_LOG10:
        case C2MP_FUNCTION_COS:
        case C2MP_FUNCTION_SIN:
        case C2MP_FUNCTION_COSH:
        case C2MP_FUNCTION_SINH:
            freeExpressionAST(expr->expression.e1);
            free(expr);
            break;
        case C2MP_FUNCTION_UNKNOWN:
            for (int i = 0; i < expr->customFunction.argnum; i++)
            {
                freeExpressionAST(expr->customFunction.args[i]);
            }
            free(expr->customFunction.name);
            free(expr);
            break;
        default:
            fprintf(stderr, "Warning, unknown expression operation : %c\n", expr->operator);
    }
}

void printExpressionAST(expressionAST *expr)
{
    if(expr == NULL)
    {
        fprintf(stderr, "Warning, tried to print a NULL expression\n");
        return;
    }
    switch(expr->operator)
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
            printf("(");
            printExpressionAST(expr->expression.e1);
            printf("%c", expr->operator);
            printExpressionAST(expr->expression.e2);
            printf(")");
            break;
        case C2MP_OPERATOR_UNARY_MINUS:
        case C2MP_OPERATOR_UNARY_PLUS:
        case C2MP_OPERATOR_LOGICAL_NOT:
        case C2MP_OPERATOR_BITWISE_NOT:
            printf("(");
            printf("%c", expr->operator);
            printExpressionAST(expr->expression.e1);
            printf(")");
            break;
        case C2MP_CHARACTER_INTEGER: // number
            printf("%d", expr->valueInt);
            break;
        case C2MP_CHARACTER_FLOAT: // float
            printf("%f", expr->valueFloat);
            break;
        case C2MP_CHARACTER_VARIABLE: // variable
            printf("(%s)", getNameFromReference(expr->valueVariable));
            break;
        case C2MP_CHARACTER_STRING: // string
            printf("%s", expr->valueString);
            break;
        case C2MP_FUNCTION_POW:
            printf("pow("); 
            printExpressionAST(expr->expression.e1);
            printf(", ");
            printExpressionAST(expr->expression.e2);
            printf(")");
            break;
        case C2MP_FUNCTION_SQRT:
            printf("sqrt(");
            printExpressionAST(expr->expression.e1);
            printf(")");
            break;
        //case C2MP_FUNCTION_ABS:
            printf("abs(");
            printExpressionAST(expr->expression.e1);
            printf(")");
            break;
        case C2MP_FUNCTION_EXP:
            printf("exp(");
            printExpressionAST(expr->expression.e1);
            printf(")");
            break;
        case C2MP_FUNCTION_LOG:
            printf("log(");
            printExpressionAST(expr->expression.e1);
            printf(")");
            break;
        case C2MP_FUNCTION_LOG10:
            printf("log10(");
            printExpressionAST(expr->expression.e1);
            printf(")");
            break;
        case C2MP_FUNCTION_COS:
            printf("cos(");
            printExpressionAST(expr->expression.e1);
            printf(")");
            break;
        case C2MP_FUNCTION_COSH:
            printf("cosh(");
            printExpressionAST(expr->expression.e1);
            printf(")");
            break;
        case C2MP_FUNCTION_SIN:
            printf("sin(");
            printExpressionAST(expr->expression.e1);
            printf(")");
            break;
        case C2MP_FUNCTION_SINH:
            printf("sinh(");
            printExpressionAST(expr->expression.e1);
            printf(")");
            break;
        case C2MP_FUNCTION_UNKNOWN:
            printf("%s(", expr->customFunction.name);
            printExpressionAST(expr->customFunction.args[0]);
            for (int i = 1; i < expr->customFunction.argnum; i++)
            {
                printf(", ");
                printExpressionAST(expr->customFunction.args[i]);
            }
            printf(")");
            break;
        default:
            fprintf(stderr, "Warning, unknown expression operation : %c\n", expr->operator);
    }
}
