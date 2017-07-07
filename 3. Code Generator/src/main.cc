/***********************************************************************************************
 *
 *  File Name	: main.cc
 *
 *  Description	: Main function is defined in this file. This program accepts a file name as 
 *  		  parameter. Here's the syntax:
 *  		  ./parser <file-name>
 *
 * 		  It scans the input file and parses it to find if it is gramatically correct.
 * 		  The output is in the format:
 * 		  variable _num_variables  function  _num_functions statement _num_statements.
 *
 * 		  For example, for a correct program with 4 variables, 2 functions and 6 statements, 
 * 		  your compiler should output at least the following:
 *  		  Pass variable 4 function 2 statement 6
 *
 ***********************************************************************************************/

#include <string>
#include <fstream>
#include <iostream>
#include <scanner.h>
#include <parser.h>

using namespace std;

int main(int argc, char **argv)
{
	/* Check for valid input to program */
	if (argc != 2) {
		cout << "Invalid number of inputs." << endl;
		cout << "Use below syntax:" << endl;
		cout << "./" << argv[0] << " <file-name>" << endl;
		return -1;
	}

	/* code generation object */
	CodeGen c;

	/* Create a scanner object */
	Scanner scanner(argv[1], c);

	/* Create a parser object */
	Parser parser(scanner, c);

	/* Get first token and start parsing */
	parser.token = scanner.GetParsingToken();
	if (!parser.parseProgram()) {
		cout << "Error" << endl;
		scanner.DeleteOutputFile();
	}

	return 0;
}

