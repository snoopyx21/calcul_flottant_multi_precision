#include "generate.h"
#include "quad.h"
#include "symbol.h"

void generateCode(quad *q, char *rounding, int precision)
{
    symbol ass;
    quad *firstQuad = q;
    quad *currentQuad = q;
    int indent = 0;
    bool *listTemps;

    if (q == NULL)
    {
        printf("no quads\n");
    }

    // generation of the initialisations of the temporary variables
    listTemps = generateInitCode(q, precision);

    do
    {
        for (int i = 0; i < indent + 1; ++i)
        { 
            // indent quads
            fprintf(output, "    ");
        }

        switch (currentQuad->operator)
        {
        case C2MP_OPERATOR_BINARY_PLUS:
            fprintf(output, "mpc_add(%s, ", getNameFromReference(currentQuad->assignment));
            outputOperand(currentQuad->operands[0]);
            fprintf(output, ", ");
            outputOperand(currentQuad->operands[1]);
            fprintf(output, ", %s);", rounding);
            break;

        case C2MP_OPERATOR_BINARY_MINUS:
            fprintf(output, "mpc_sub(%s, ", getNameFromReference(currentQuad->assignment));
            outputOperand(currentQuad->operands[0]);
            fprintf(output, ", ");
            outputOperand(currentQuad->operands[1]);
            fprintf(output, ", %s);", rounding);
            break;

        case C2MP_OPERATOR_BINARY_DOT:
            fprintf(output, "mpc_mul(%s, ", getNameFromReference(currentQuad->assignment));
            outputOperand(currentQuad->operands[0]);
            fprintf(output, ", ");
            outputOperand(currentQuad->operands[1]);
            fprintf(output, ", %s);", rounding);
            break;

        case C2MP_OPERATOR_BINARY_DIVIDE:
            fprintf(output, "mpc_div(%s, ", getNameFromReference(currentQuad->assignment));
            outputOperand(currentQuad->operands[0]);
            fprintf(output, ", ");
            outputOperand(currentQuad->operands[1]);
            fprintf(output, ", %s);", rounding);
            break;

        // result of mpc_cmp must be > 0
        case C2MP_OPERATOR_LOWER_THAN:
            fprintf(output, "%s = ", getNameFromReference(currentQuad->assignment));
            fprintf(output, "mpc_cmp(");
            outputOperand(currentQuad->operands[0]);
            fprintf(output, ", ");
            outputOperand(currentQuad->operands[1]);
            fprintf(output, ") == 2;");
            break;

        // result of mpc_cmp must be < 0
        case C2MP_OPERATOR_GREATER_THAN:
            fprintf(output, "%s = ", getNameFromReference(currentQuad->assignment));
            fprintf(output, "mpc_cmp(");
            outputOperand(currentQuad->operands[0]);
            fprintf(output, ", ");
            outputOperand(currentQuad->operands[1]);
            fprintf(output, ") == 1;");
            break;

        // result of mpc_cmp must be >= 0
        case C2MP_OPERATOR_LOWER_OR_EQUAL:
            fprintf(output, "%s = ", getNameFromReference(currentQuad->assignment));
            fprintf(output, "mpc_cmp(");
            outputOperand(currentQuad->operands[0]);
            fprintf(output, ", ");
            outputOperand(currentQuad->operands[1]);
            fprintf(output, ") != 1;");
            break;

        // result of mpc_cmp must be <= 0
        case C2MP_OPERATOR_GREATER_OR_EQUAL:
            fprintf(output, "%s = ", getNameFromReference(currentQuad->assignment));
            fprintf(output, "mpc_cmp(");
            outputOperand(currentQuad->operands[0]);
            fprintf(output, ", ");
            outputOperand(currentQuad->operands[1]);
            fprintf(output, ") != 2;");
            break;

        // result of mpc_cmp must be == 0
        case C2MP_OPERATOR_EQUAL:
            fprintf(output, "%s = ", getNameFromReference(currentQuad->assignment));
            fprintf(output, "mpc_cmp(");
            outputOperand(currentQuad->operands[0]);
            fprintf(output, ", ");
            outputOperand(currentQuad->operands[1]);
            fprintf(output, ") == 0;");
            break;

        // result must be different from 0
        case C2MP_OPERATOR_NOT_EQUAL:
            fprintf(output, "%s = ", getNameFromReference(currentQuad->assignment));
            fprintf(output, "mpc_cmp(");
            outputOperand(currentQuad->operands[0]);
            fprintf(output, ", ");
            outputOperand(currentQuad->operands[1]);
            fprintf(output, ") != 0;");
            break;
        
        case C2MP_QUAD_NO_ASSIGNMENT:
            // can only be custom functions!
            fprintf(output, "%s(", currentQuad->fctName);
            for (int i = 0; i < currentQuad->operandsNum; i++)
            {
                if (i != 0) 
                    fprintf(output, ", ");
                outputOperand(currentQuad->operands[i]);
            }
            fprintf(output, ");");
            break;

        case C2MP_QUAD_ASSIGNMENT:
            ass = getSymbolFromReference(currentQuad->assignment);
            switch (ass.type_symbol)
            {
                case FLOAT_NUMBER:
                case INTEGER_NUMBER:
                    fprintf(output, "%s = ", getNameFromReference(currentQuad->assignment));
                    switch(getSymbolTypeFromOperand(currentQuad->operands[0]))
                    {
                        case INTEGER_NUMBER:
                        case FLOAT_NUMBER:
                            fprintf(output, "%s;", getNameFromReference(currentQuad->operands[0].reference));
                            break;
                        case MPC_T:
                            fprintf(output, "mpc_get_ldc(");
                            outputOperand(currentQuad->operands[0]);
                            fprintf(output, ", %s);", rounding);
                            break;
                    }
                    break;
                
                case MPC_T:
                    // switching the operand 1 => the rvalue of the assignment
                    switch (getSymbolTypeFromOperand(currentQuad->operands[0]))
                    {
                        case FLOAT_NUMBER:
                            fprintf(output, "mpc_set_d(");
                            break;

                        case INTEGER_NUMBER:
                            fprintf(output, "mpc_set_si(");
                            break;

                        case MPC_T:
                            fprintf(output, "mpc_set(");
                            break;                           
                    
                        default:
                            panic("generate.c", "generateCode", "Unknown operand type");
                    }
                    // printed every time
                    fprintf(output, "%s, ", getNameFromReference(currentQuad->assignment));
                    outputOperand(currentQuad->operands[0]);
                    fprintf(output, ", %s);", rounding);
                    break;

                default:
                    panic("generate.c", "generateCode", "Unknown operand type");
            }
            break;

        case C2MP_QUAD_IF:
            ++indent;
            fprintf(output, "\n%*sif (",indent*4," ");
            outputOperand(currentQuad->operands[0]);
            fprintf(output, ")\n%*s{",indent*4," ");
            break;

        case C2MP_QUAD_ELSE:
            fprintf(output, "} else {");
            break;

        case C2MP_QUAD_ENDIF:
            --indent;
            fseeko(output,-4,SEEK_END);
            fprintf(output, "}\n");
            break;

        case C2MP_QUAD_WHILE:
            ++indent;
            fprintf(output, "\n%*swhile (%s)\n%*s{",indent*4," ", getNameFromReference(currentQuad->assignment),indent*4," ");
            break;

        case C2MP_QUAD_DOWHILE:
            ++indent;
            fprintf(output, "\n%*sdo\n%*s{",indent*4," ",indent*4," ");
            break;

        case C2MP_QUAD_ENDWHILE:
            --indent;
            fseeko(output,-4,SEEK_END);
            fprintf(output, "}   // %s\n", getNameFromReference(currentQuad->assignment));
            break;

        case C2MP_QUAD_ENDDOWHILE:
            --indent;
            fseeko(output,-4,SEEK_END);            
            fprintf(output, "} while (%s);\n", getNameFromReference(currentQuad->assignment));
            break;

        case C2MP_FUNCTION_POW:
            fprintf(output, "mpc_pow(%s, ", getNameFromReference(currentQuad->assignment));
            outputOperand(currentQuad->operands[0]);
            fprintf(output, ", ");
            outputOperand(currentQuad->operands[1]);
            fprintf(output, ", %s);", rounding);
            break;
        
        case C2MP_FUNCTION_SQRT:
            fprintf(output, "mpc_sqrt(%s, ", getNameFromReference(currentQuad->assignment));
            outputOperand(currentQuad->operands[0]);
            fprintf(output, ", %s);", rounding);
            break;
        /**
         * Not supporting ABS because MPC is 
        case C2MP_FUNCTION_ABS:
            fprintf(output, "mpc_abs(%s, ", getNameFromReference(currentQuad->assignment));
            outputOperand(currentQuad->operands[0]);
            fprintf(output, ", %s);", rounding);
            break;
        */

        case C2MP_FUNCTION_EXP:
            fprintf(output, "mpc_exp(%s, ", getNameFromReference(currentQuad->assignment));
            outputOperand(currentQuad->operands[0]);
            fprintf(output, ", %s);", rounding);
            break;
        
        case C2MP_FUNCTION_LOG:
            fprintf(output, "mpc_log(%s, ", getNameFromReference(currentQuad->assignment));
            outputOperand(currentQuad->operands[0]);
            fprintf(output, ", %s);", rounding);
            break;
        
        case C2MP_FUNCTION_LOG10:
            fprintf(output, "mpc_log10(%s, ", getNameFromReference(currentQuad->assignment));
            outputOperand(currentQuad->operands[0]);
            fprintf(output, ", %s);", rounding);
            break;
        
        case C2MP_FUNCTION_COS:
            fprintf(output, "mpc_cos(%s, ", getNameFromReference(currentQuad->assignment));
            outputOperand(currentQuad->operands[0]);
            fprintf(output, ", %s);", rounding);
            break;

        case C2MP_FUNCTION_SIN:
            fprintf(output, "mpc_sin(%s, ", getNameFromReference(currentQuad->assignment));
            outputOperand(currentQuad->operands[0]);
            fprintf(output, ", %s);", rounding);
            break;
        
        case C2MP_FUNCTION_COSH:
            fprintf(output, "mpc_cosh(%s, ", getNameFromReference(currentQuad->assignment));
            outputOperand(currentQuad->operands[0]);
            fprintf(output, ", %s);", rounding);
            break;

        case C2MP_FUNCTION_SINH:
            fprintf(output, "mpc_sinh(%s, ", getNameFromReference(currentQuad->assignment));
            outputOperand(currentQuad->operands[0]);
            fprintf(output, ", %s);", rounding);
            break;
        
        case C2MP_FUNCTION_SQR:
            fprintf(output, "mpc_sqr(%s, ", getNameFromReference(currentQuad->assignment));
            outputOperand(currentQuad->operands[0]);
            fprintf(output, ", %s);", rounding);
            break;
        
        // custom function (not converted into a MPC function)
        case C2MP_FUNCTION_UNKNOWN:
            if (currentQuad->assignment != C2MP_QUAD_NO_ASSIGNMENT)
            {
                // we have to assign the function a lvalue !
                fprintf(output, "%s = ", getNameFromReference(currentQuad->assignment));
            }
            fprintf(output, "%s(", currentQuad->fctName);
            for (int i = 0; i < currentQuad->operandsNum; i++)
            {
                if (i != 0) 
                    fprintf(output, ", ");
                outputOperand(currentQuad->operands[i]);
            }
            fprintf(output, ");");
            break;

        case C2MP_OPERATOR_UNARY_MINUS:
            fprintf(output, "mpc_neg(%s, ", getNameFromReference(currentQuad->assignment));
            outputOperand(currentQuad->operands[0]);
            fprintf(output, ", %s);", rounding);
            break;

        // not supported yet
        case C2MP_OPERATOR_BITWISE_AND:
        case C2MP_OPERATOR_BITWISE_OR:
        case C2MP_OPERATOR_BITWISE_NOT:
        case C2MP_OPERATOR_BITWISE_XOR:
        case C2MP_OPERATOR_UNARY_PLUS:
        case C2MP_OPERATOR_LOGICAL_OR:
        case C2MP_OPERATOR_LOGICAL_AND:
        case C2MP_OPERATOR_LOGICAL_NOT:
            break;

        default:
            fprintf(stderr, "Warning, unknown quad operation : %c\n", currentQuad->operator);
        }

        fprintf(output, "\n");

        currentQuad = currentQuad->next;

    } while (currentQuad != firstQuad);
    
    fprintf(output, "\n");
    
    // generate the clear instructions
    generateClearCode(listTemps);

    // The code has been generated, We do not need the quads anymore.
    //freeQuads(q);

}

bool * generateInitCode(quad *q, int precision)
{
    bool *tempList;
    quad *firstQuad = q;
    quad *currentQuad = q;

    if (q == NULL)
    {
        fprintf(stderr, "generateInitCode warning: no quad was generated\n");
        return NULL;
    }
    
    /**
     * List of the temporary variables we found. A variable can be used several
     * times. So we need to check for each variable found if it was already
     * found and put into the list. The list is a tab of booleans, each index
     * represent a temporary variable of the form "C2MP___temp_index". When
     * tempList[index] is true, it means that "C2MP__temp_index" must be init.
     */
    tempList = malloc(MAX * sizeof(bool));
    if (tempList == NULL)
    {
        panic("generate.c", "generateInitCode", "malloc: failed to init tempList");
    }

    for (int i = 0; i < MAX; i++)
    {
        tempList[i] = false;
    }

    // First, we need to find all the different temporary variables
    do
    {
        if (getSymbolFromReference(currentQuad->assignment).isTemp)
        {
            tempList[currentQuad->assignment] = true;
        }
        currentQuad = currentQuad->next;
    } while(currentQuad != firstQuad);

    // Second, we have to generate the code to init the variables
    for (int i = 0; i < MAX; i++)
    {
        if (tempList[i])
        {
            switch(getSymbolFromReference(i).type_symbol)
            {
                case INTEGER_NUMBER:
                    fprintf(output, "    int %s;\n", getSymbolFromReference(i).name);
                    break;
                case FLOAT_NUMBER:
                    fprintf(output, "    double %s;\n", getSymbolFromReference(i).name);
                    break;
                case MPC_T:
                    fprintf(output, "    mpc_t %s;", getSymbolFromReference(i).name);
                    fprintf(output, " mpc_init2(%s, %d);\n", getSymbolFromReference(i).name, precision);
                    break;
                default:
                    panic("generate.c", "generateInitCode", "unknown type");
            }
        }
    }

    fprintf(output, "\n");

    return tempList;

}

void generateClearCode(bool *tempList)
{

    if (tempList == NULL)
    {
        fprintf(stderr, "generateClearCode warning: no templist given");
        return;
    }

    for (int i = 0; i < MAX; i ++)
    {
        // if the field is true, it means the variable i has to be cleared
        if (tempList[i])
        {
            switch(getSymbolFromReference(i).type_symbol)
            {
                case INTEGER_NUMBER:
                    break;
                case FLOAT_NUMBER:
                    break;
                case MPC_T:
                    fprintf(output, "    mpc_clear(%s);\n", getSymbolFromReference(i).name);
                    break;
                default:
                    panic("generate.c", "generateClearCode", "unknown type");
            }
        }
    }

    
    // we don't need to use it again
    free(tempList);

}