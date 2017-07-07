REFERENCE:
================================================
I am building up the "Function code generation" project on the "Intra Function Code Generation" solution.

HOW TO COMPILE THE CODE
================================================
Use below command to compile the code (it creates an executable "codegen"):
make

HOW TO RUN
================================================
Use below command to run the executable 'codegen':
./codegen <file-name>

This would result in an output file
Eg. for an input file - test.c
an output file - test_gen.c would be generated

ISSUES:
================================================
1. Updated the "int const N=2000;" required in the output file to "#define N 2000"
On using "int const N=2000;" the output file received Error "error: variably modified 'mem' at file scope " on compiling.

This line is present at line 131 at file scanner.h in case this needs to be changed back to "int const N=2000;".

2. Out of the list of test files:
automaton.c, binrep.c, fibonacci.c, formulae.c, funcall.c, funcall2.c, illegal.c, mandel.c, parse.c, parse2.c, sort.c, square.c, tax.c, times8.c, polymorphism.c, recursion.c, trivial.c, loop.c, max.c, parameter.c, branch.c, errorString.c, expression.c

the files:
mandel.c, max.c, recursion.c, fibonacci.c

have multiple "return" statements in a given function.

As the project description did not mention the epilogue handling of such a scenario, I could not take a decision on how to implement this.
Hence, though an output file successfully gets generated, they are not successfully compiled as required by the program description.

3. My code has issues where an array reference is found in the test file.
Due to this the files:

illegal.c, fibonacci.c, sort.c

get into error while executing the code for the above said files.

Error:
libc++abi.dylib: terminating with uncaught exception of type std::invalid_argument: stoi: no conversion
Abort trap: 6

FUTURE WORK:
================================================
1. As per point 2 in the Issues section, I would like to get more clarity on how to handle a scenario where multiple return commands occur in a function and then implement the concerned solution.

2. As per point 3 in the Issues section, I need to resolve the issue for test files having array references.

POINT TO NOTE
================================================
This project is using the cpp solution provided for project 3.

The example input program in project 4 has the following output with the project 3 solution without any modification whatsoever:

#include <stdio.h>
#define read(x) scanf("%d",&x)
#define write(x) printf("%d\n",x)

int add(int a)
{
int local[4];
read(local[0]);
local[1] = a;
local[2] = local[1] + local[0];
local[3] = local[2];
return local[3];
}

int main()
{
int local[3];
read(local[0]);
local[2] = add(local[0]);
write(local[2]);
}

It can be seen that in LINE 72 and 73 here, local array reference moves from 0 to 2 skipping 1 somehow. However, I can assure that the implication of the code does not change in any which way.
I write this to point out that the output of this project might be a bit different from the exact output expected due to this reason. However, I re-assure that the way the code should work is intact and this does not cause any compilation or similar issues at all. Neither does is change the defnition of the code in any way at all.

IMPLIMENTATION
================================================
1. The Project 3 solution has been used where while the code id being parsed to generate an Intra Function Code, I have used the same opprtunity to distribute the input code into respective functions.
2. For this, I have implemented a class functionStore which has been encapsulated in the Intra Code Generation class 'CodeGen'. CodeGen encapsulates an array of functionStore objects which in turn store the different functions as they are encountered while CodeGen parses the input file. 
3. The functionStore class saves a lot of information with respect to the function, viz. name, number of variables, number of parameters, IDs of parameters, number of function calls within the function, the IDs of the function calls which occur in the given function. Along with this information, it also houses respective Index variables to access each of the properties mentioned aboev.
3. As the parsing is completed for Intra Function Code, a block of methods defined in the Scanner class are called to generate the Function Code. These methods have been defined in the Scanner class as this class is responsible for the File reading and writing functionality.
4. The above mentioned functions are in the format - appendMetaCode, addMainPrelogue, addMainBody, addMainEpilogue, addFuncBody, addJumpCode. All the functions are self explanatory, except addFuncBody. addFuncBody function collectively does for all the functions in the input file with the exception of 'main' what addMainPrelogue, addMainBody, addMainEpilogue do for main individually.
These functions create relevant Function Code and adds them to the output file.