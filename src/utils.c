#include "utils.h"

/* sortie d'erreur */
void panic(char * file, char * function, char * error)
{
    fprintf(stderr, "File %s: Unexpected Error Occurred - function '%s'\n", file, function);
    fprintf(stderr, "\t%s\n", error);
    
    exit(EXIT_FAILURE);
}

int checkExtension(char * s)
{
    if (strncmp(s, "precision", 9) == 0)
        return 1;
    if (strncmp(s, "rounding", 8) == 0)
        return 2;
    return ERROR;
}

int parseFct(char *symbol)
{
    /** 
     * This could be buggy: for example a function called "sqrthello" would
     * match with this implementation...
     */
    if (strncmp(symbol, "sqrt", 5) == 0)
        return C2MP_FUNCTION_SQRT;
    if (strncmp(symbol, "pow", 4) == 0)
        return C2MP_FUNCTION_POW;
    /*if (strncmp(symbol, "fabs", 5) == 0)
        return C2MP_FUNCTION_ABS;*/
    if (strncmp(symbol, "exp", 4) == 0)
        return C2MP_FUNCTION_EXP;
    if (strncmp(symbol, "log", 4) == 0)
        return C2MP_FUNCTION_LOG;
    if (strncmp(symbol, "log10", 6) == 0)
        return C2MP_FUNCTION_LOG10;
    if (strncmp(symbol, "cos", 4) == 0)
        return C2MP_FUNCTION_COS;
    if (strncmp(symbol, "sin", 4) == 0)
        return C2MP_FUNCTION_SIN;
    if (strncmp(symbol, "cosh", 5) == 0)
        return C2MP_FUNCTION_COSH;
    if (strncmp(symbol, "sinh", 5) == 0)
        return C2MP_FUNCTION_SINH;
    if (strncmp(symbol, "sqr", 4) == 0)
        return C2MP_FUNCTION_SQR; 
    return C2MP_FUNCTION_UNKNOWN;
}

int open_file(char * name)
{
    output = fopen(name, "w+");
	if(output == NULL)
	    panic("utils.c", "open_file", "Error open file\n");
    return 0;
}

int close_file(void)
{
    if ( fclose(output) != 0)
        panic("utils.c", "close_file", "Error Close File\n");
    return 0;                             
}


int write_file(const char * expr)
{
    fprintf(output, "%s", expr);
    return 0;
}



