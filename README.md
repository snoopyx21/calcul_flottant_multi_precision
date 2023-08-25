# C2MP

Floating Point Operation Compiler

## Authors

DIVRIOTIS Constantin 

FLINT Clément

JUNG Lionel   
      
VETRIVEL Govindaraj


## Link

https://github.com/GovinV/C2MP


## Synopsis

C2MP (from "C to MPC") is a source to source compiler that translate arithmetic
operations and functions written in C syntax, into functions from the Multi Point 
Precision (MPC) library. The compiler translates the C code present in a pragma macro.
In the following example, the code between the brackets is compiled:
```

#PRAGMA MPC precision(128) rounding(MPC_RNDZZ)
{
    a = sqrt(pow(b, 5));
    if (a > 10)
    {
        c = 1;
    } else {
        c = 0;
    }
}

```
When compiling a C source file with C2MP, the code outside the pragma is just rewritten in
the resulting source file.

You can visit the MPC project website here: http://www.multiprecision.org/mpc

## Installation

Run the following commands :

```

make

./C2MP <file> [-O] [-o <output>] [-v] [-q] [-s]

```
**Options**
* `-O` : add optimization to the generate code
* `-o <output>` : file of the output file
* `-q` : display generated quads
* `-s` : display generated Abstract Syntax Tree
* `-v` : display AST, quads and others informations

**Repository**
* Repository doc        - Doxygen Documentation
* Repository src        - Source File
* Repository include    - Header File
* Repository tests      - Test File


To generate a doxygen documentation, run:

```

make doc

```


To run premade tests from the test folder, run:

```

make run

```

## Features

**Capabilities**

Our compiler handles the following instructions:
* Arithmetic operators (+, -, /, *)
* Math functions: pow, sqrt, exp, log, log10, cos, sin, sinh, cosh, sinh and sqr
* Assignments to variables with variables, functions or expressions as rvalues
* Conditions: single if, if binded with else, nested ifs are supported too
* Loops: while, do while and for (note: incrementation is not supported, must use "i=i+1")
* Loops and conditions without brackets (one instuction) are also handled
* Logical operators "and" (&&), "or" (||)
* Comparisons operators: "<", ">", "<=", ">=", "==", "!=" (for mpc_t types comparisons)
* Parenthezing of expressions is supported
* Unknown functions are recopied in out, arguments are translated: 
if they are expressions (ex: function(a, c+d, pow(x,y)) => function(a, temp1, temp2) where 
temp1 and temp2 are mpc_t variables that contains the result of there respective expression)
* Printf() in the pragma are supported (note: maximum arguments number is 10 for a function). 
Also, C2MP is compiling as many pragmas as found in one source file.

**Optimizations**

Besides, C2MP does the following optimization on the code:
* Removing common subexpressions
* Removing useless temporary variables
* Removing loop invariants
* Reusing temporary variables (if not used anymore)

## Test
To run all tests in repository tests/, run `make run` after `make`.

Each test will go through 8 steps:
* Step 1: Precompile with C2MP `./C2MP prgm.c`
* Step 2: Compile with gcc `gcc result.c -lmpc -o result`
* Step 3: Execute the generated file `./result`
* Step 4: Test on the output, all output should be this way `xxxx GeneratedResult ExepectedResult` xxxx can be replaced by the variable name that contain the printed result for example, any others print should be precede by `#`
* Step 5: Precompile with C2MP with optimization `./C2MP prgm.c -O -o opti_result.c` 
* Step 6: Compile with gcc `gcc opti_result.c -lmpc -o opti_result`
* Step 7: Execute the generated file `./opti_result`
* Step 8: Test if output of optimized version is the same as the not optimized one
* Step 9: Success

**Specify a behavior for a test**

If you write a test that fails one of the test's step, you have to add in file `tests/expected_behavior` the test filename and the step on which it is expected to fail. 

Examples, if you want the testfile test_x.c to fail at the precompilation, you have to had on a new line:
`test_x.c 1`
## Examples

Check the tests/ repository for examples.


## Documentation

Run `make doc | firefox doc/html/index.html"`

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
