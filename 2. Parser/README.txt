This directory contains 6 files - README.txt scanner.h, scanner.cpp, test.cpp, and the grammar definition file - LL(1) grammar for Parser.pdf for the purpose of the assignment Project 2: Parser in CSC 512.

---------
Execution
---------

a.The files README.txt, scanner.h, scanner.cpp, test.cpp should be copied to the "CSC 512 ubuntu 14.04 image" image in the NCSU VCL. Please place the files in a folder (say A).
b.Add test programs/files to the same folder.
c.OPEN Terminal and move the current cursor to Directory A.
d.Run the following command to execute the program:

g++ scanner.cpp test.cpp -o parser
./parser <inputfile>

-------------------
Warnings and Errors
-------------------

The program is not succcessfully parsing at this juncture.
I think the reason is that I have tried to handle the ambiguity present in RULE <program> to avoid backtracking as this remains the purpose of an LL1 parser and in the process have made some mistake in the grammer. I am as of now unable to find the exact point.

Please check the file "LL(1) grammar for Parser.pdf" for the same.

I would here be expecting some consideration as I have created the grammer and coded it (please check the same at test.cpp) but am suffering with accuracy.

--------------
REFERENCE
--------------
Mr. Vivek Kumar(C++ solution) provided by Kaustubh Khasnis.

----------------
Assumptions:
----------------
a. in scanner.cpp in the GetNextToken method definition, I am ignoring the TOKEN if it is found to be a kind of a SPACE or a META Statement as these do not affect my parser grammer.

b. File is not further scanned/parsed on the occasion of an error.

----------------
Design Decisions
----------------
a. On top of Mr. Vivek Kumar's scanner model, I have made the following updates:

scanner.h
	- Removed the cs512 prefix addition on an IDENTIFIER.
	- Added and enum TokenKind with the following list of values to handle the Terminals in the parser.

	EMPTY,
	META,
	SEMICOLON, COMMA, EQUAL_SIGN,
	LEFT_PARENTHESIS, RIGHT_PARENTHESIS,
	LEFT_BRACE, RIGHT_BRACE,
	DOUBLE_AND, DOUBLE_OR, 
	DOUBLE_EQUAL, NOT_EQUAL, GREATER, GREATER_EQUAL, LESS, LESS_EQUAL,
	PLUS, MINUS, STAR, SLASH,
	LEFT_BRACKET, RIGHT_BRACKET,
	ID,	
	INT, VOID, IF, WHILE, RETURN, READ, WRITE, PRINT, CONTINUE, BREAK, BINARY, DECIMAL,
	STRING,
	NUMBER,
	OTHER, SPACE,
	ERROR,
	EOF1

	- To use the enum, a GetTokenKind and a SetTokenKind methods are used as getter and setter and public instance variable "enum TokenKind Kind" is used for capturing the value.

scanner.cpp 
	- As mentioned, in the GetNextToken method definition, I am ignoring the TOKEN if it is found to be a kind of a SPACE or a META Statement as these do not affect my parser grammer.
	- Wherever a TokenType is encountered, I am updating the related value for TokenKind modifying the GetNextToken accordingly.

test.cpp
	- All the grammer rules are added as functions.
	- the global variables are added for counting variables declarations, function definitions, and statements.
	- global Token variable is used to switch between Rule functions.
	- the main() function first scans the file and then once it is passed moves on to parse the same.

