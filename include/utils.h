#ifndef __UTILS_C
#define __UTILS_C

/**
 * \file utils
 * \brief Program contains different important define and variable necessary to syntax.y 
 * \author DIVRIOTIS C. FLINT C. JUNG L. VETRIVEL G. 
 * \version 0.1
 * \date Janvier 2018
 *
 */

/*
list of operators :
< > : binary
i s : <= (i) and >= (s)
e E : binary == (e) and != (E)
+ - * / : binary
| : bitwise or
& : bitwise and
^ : xor
O : logical or
A : logical and


m : unary -
p : unary +
! : logical not
~ : bitwise not
*/

#define C2MP_NUM_TYPE_FLOAT 0
#define C2MP_NUM_TYPE_INTEGER 1

#define C2MP_OPERATOR_BINARY_PLUS       '+'
#define C2MP_OPERATOR_BINARY_MINUS      '-'
#define C2MP_OPERATOR_BINARY_DOT        '*'
#define C2MP_OPERATOR_BINARY_DIVIDE     '/'

#define C2MP_OPERATOR_UNARY_PLUS        'p'
#define C2MP_OPERATOR_UNARY_MINUS       'm'

#define C2MP_OPERATOR_LOWER_THAN        '<'
#define C2MP_OPERATOR_GREATER_THAN      '>'
#define C2MP_OPERATOR_LOWER_OR_EQUAL    'i'
#define C2MP_OPERATOR_GREATER_OR_EQUAL  's'
#define C2MP_OPERATOR_EQUAL             'e'
#define C2MP_OPERATOR_NOT_EQUAL         'E'

#define C2MP_OPERATOR_LOGICAL_AND       'A'
#define C2MP_OPERATOR_LOGICAL_OR        'O'
#define C2MP_OPERATOR_LOGICAL_NOT       '!'
#define C2MP_OPERATOR_BITWISE_AND       '&'
#define C2MP_OPERATOR_BITWISE_OR        '|'
#define C2MP_OPERATOR_BITWISE_XOR       '^'
#define C2MP_OPERATOR_BITWISE_NOT       '~'

#define C2MP_CHARACTER_INTEGER          'n'
#define C2MP_CHARACTER_FLOAT            'f'
#define C2MP_CHARACTER_VARIABLE         'v'
#define C2MP_CHARACTER_STRING           'S'



#define C2MP_QUAD_ASSIGNMENT            '='
#define C2MP_QUAD_IF                    'I'
#define C2MP_QUAD_ELSE                  'z'
#define C2MP_QUAD_ENDIF                 '}'
#define C2MP_QUAD_WHILE                 'W'
#define C2MP_QUAD_ENDWHILE              'w'
#define C2MP_QUAD_DOWHILE               'D'
#define C2MP_QUAD_ENDDOWHILE            'd'
#define C2MP_QUAD_NO_ASSIGNMENT         ' '

// Known functions
#define C2MP_FUNCTION_POW               1
#define C2MP_FUNCTION_SQRT              2
//#define C2MP_FUNCTION_ABS               3
#define C2MP_FUNCTION_EXP               4
#define C2MP_FUNCTION_LOG               5
#define C2MP_FUNCTION_LOG10             6
#define C2MP_FUNCTION_COS               7
#define C2MP_FUNCTION_SIN               8
#define C2MP_FUNCTION_COSH              9
#define C2MP_FUNCTION_SINH              10
#define C2MP_FUNCTION_UNKNOWN           11
#define C2MP_FUNCTION_SQR               12

// max args for a function
#define MAX_FCT_ARGS 10

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define ERROR      -1


int pragmaOn;
int pragmaBlocOn;
int pragmaBlocIndex;
int optiFlag;
int optionPrintQuads;
int optionPrintSemiquads;
int optionVerbose;
int pragmaMet;
FILE * output;
FILE * finput;

/**
 * \fn void panic(char *, char * , char *)
 * \brief Function Error Output.
 * \param File
 * \param Function 
 * \param Error Message
 */
void panic(char *, char * , char *);

/**
 * \fn int checkExtension(char *)
 * \brief Verify if the pragma exists in params.
 * \return Return PRECISION if the extension is precision, 
 * return ROUNDING if extension is rounding, ERROR (-1) if error.
 */
int checkExtension(char *);

/**
 * \fn int parseFct(char *)
 * \brief Verifiy if the symbol in argument exists and if we can translate to MPC.
 * \return math function match
 */
int parseFct(char *);

/**
 * \fn int open_file(char * name)
 * \brief Open file output.
 * \return 0 if SUCCESS
 */
int open_file(char * name);

/**
 * \fn int close_file(void)
 * \brief Close file output.
 * \return 0 if SUCCESS
 */
int close_file(void);

/**
 * \fn int write_file(const char * expr)
 * \brief Write the expression in argument in file output.
 * \return 0 if SUCCESS
 */
int write_file(const char * expr);

#endif // UTILS_C