#include "semiQuad.h"

semiQuad *createSemiQuad(char operator, int assignment, expressionAST *expression)
{
    semiQuad *quad = malloc(sizeof(semiQuad));
    quad->operator = operator;
    quad->assignment = assignment;
    quad->expression = expression;
    quad->next = quad;
    quad->previous = quad;

    return quad;
}

// concatenates 2 quads or 2 quadlists
semiQuad *concatSemiQuad(semiQuad *q1, semiQuad *q2)
{
    if(q2 == NULL)
    {
        return q1;
    }
    if(q1 == NULL)
    {
        return q2;
    }

    semiQuad *head1 = q1;
    semiQuad *head2 = q2;
    semiQuad *tail1 = q1->previous;
    semiQuad *tail2 = q2->previous;

    head1->previous = tail2;
    tail1->next = head2;
    head2->previous = tail1;
    tail2->next = head1;

    return q1;
}

void freeSemiQuad(semiQuad *sq)
{
    if(sq == NULL)
    {
        return;
    }
    
    semiQuad *firstSemiQuad = sq, *toFree;
    semiQuad *currentSemiQuad = sq;

    while(currentSemiQuad != firstSemiQuad)
    {
        toFree = currentSemiQuad; 
        currentSemiQuad = currentSemiQuad->next;
        if (toFree->expression != NULL)
            freeExpressionAST(toFree->expression);
        free(toFree);
    }
    if (firstSemiQuad->expression != NULL)
        free(firstSemiQuad->expression);
    free(firstSemiQuad);
}

void printSemiQuads(semiQuad *q)
{
    if(q == NULL)
    {
        printf("no quad\n");
    }

    semiQuad *firstQuad = q;
    semiQuad *currentQuad = q;

    int indent = 0;

    do
    {
        for(int i=0;i<indent;++i)
        { // indent quads
            printf("  ");
        }

        switch(currentQuad->operator)
        {
            case C2MP_QUAD_ASSIGNMENT:
                printf("%s = ", getNameFromReference(currentQuad->assignment));
                printExpressionAST(currentQuad->expression);
                break;
            case C2MP_QUAD_IF:
                ++indent;
                printf("if ");
                printExpressionAST(currentQuad->expression);
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
                printf("while ");
                printExpressionAST(currentQuad->expression);
                break;
            case C2MP_QUAD_DOWHILE:
                printf("do ");
                break;
            case C2MP_QUAD_ENDWHILE:
                --indent;
                printf("\b\bendwhile");
                break;
            case C2MP_QUAD_ENDDOWHILE:
                --indent;
                printf("\b\bwhile");
                printExpressionAST(currentQuad->expression);
                break;
            case C2MP_FUNCTION_POW:
                printf("pow");
                printExpressionAST(currentQuad->expression);
                break;
            case C2MP_FUNCTION_SQRT:
                printf("sqrt");
                printExpressionAST(currentQuad->expression);
                break;
            /**
             * Not supported yet: needs mpfr_t type!
             * case C2MP_FUNCTION_ABS:
                printf("abs");
                printExpressionAST(currentQuad->expression);
                break;*/
            case C2MP_FUNCTION_EXP:
                printf("exp");
                printExpressionAST(currentQuad->expression);
                break;
            case C2MP_FUNCTION_LOG:
                printf("log");
                printExpressionAST(currentQuad->expression);
                break;
            case C2MP_FUNCTION_LOG10:
                printf("log10");
                printExpressionAST(currentQuad->expression);
                break;
            case C2MP_FUNCTION_COS:
                printf("cos");
                printExpressionAST(currentQuad->expression);
                break;
            case C2MP_FUNCTION_SIN:
                printf("sin");
                printExpressionAST(currentQuad->expression);
                break;
            case C2MP_FUNCTION_COSH:
                printf("cosh");
                printExpressionAST(currentQuad->expression);
                break;
            case C2MP_FUNCTION_SINH:
                printf("sinh");
                printExpressionAST(currentQuad->expression);
                break;
            default:
                fprintf(stderr, "Warning, unknown semi quad operation : %d (%c)\n",
                    currentQuad->operator, currentQuad->operator);
        }
        printf("\n");

        currentQuad = currentQuad->next;
    }while(currentQuad != firstQuad);
}
