/****************************************************************************************
 *
 *  File Name	: scanner.cc
 *
 *  Description	: Write the scanner, the first phase of your compiler.  The
 *  		  scanner converts an input program into tokens and rewrites the
 *  		  program with some tokens modified.  If the input program is a
 *  		  correct C program, the output program should also be a correct
 *  		  program that has identical behavior.  The specific requirements 
 *  		  for code generation are:
 *  		  1) The scanner should recognize all the tokens of the input program.
 *  		     The token definitions are shown at the end of this description.
 *  		  2) The scanner to add string "cs512" to the beginning of every
 *  		     <identifier> token except the name of function "main".
 *  		  3) The '#include' statements and C++ style comments are treated as 
 *  		     special tokens.  They are defined as <meta statement> in our token 
 *  		     language.  Your scanner should simply copy them to the generated 
 *  		     program (each in a separate line) without modification.  Your 
 *  		     future compiler will be asked to do the same.
 *  		  4) The scanner takes one arguement, which is the name of the input
 *  		     program, and puts the generated program in a new file named with 
 *  		     an extension "_gen".  So "foo.c" will become "foo_gen.c". The output 
 *  		     program does not need to be formatted in any way.  The scanner can 
 *  		     write all statements in one line, except that the meta-statements 
 *  		     should be put in separate lines.
 *
 *  		  Token definitions:
 *  		  -----------------
 *  		  The tokens include <identifier>, <number>, <reserved word>, <symbol>, 
 *  		  <string>, and <meta statement>.  They are defined as regular expressions 
 *  		  as follows:
 *  		  <letter>         --> a | b | ... | y | z | A | B | ... | Z | underscore 
 *  		  <identifier>     --> <letter> (<letter> | <digit>)*
 *  		  <digit>          --> 0 | 1 | ... | 9 
 *  		  <number>         --> <digit>+
 *  		  <reserved word>  --> int  | void | if | while | return | read | write | 
 *  		  		       print | continue | break | binary | decimal
 *  		  <symbol>         --> left_parenthesis | right_parenthesis | left_brace | 
 *  		  		       right_brace | left_bracket | right_bracket | comma | 
 *  		  	               semicolon | plus_sign | minus_sign | star_sign | 
 *  		  	               forward_slash | == | != | > | >= | < | <= | equal_sign |
 *  		  	               double_and_sign | double_or_sign 
 *  		  		       (The symbols are: ( ) { } [ ] , ; + - * / == != > >= < 
 *  		  		       			 <= = && ||)
 *  		  <string>         --> any string between (and including) the closest pair of 
 *  		  		       quotation marks. 
 *  		  <meta statement> --> any string begins with '#' or '//' and ends with the 
 *  		  		       end of line ('\n'). 
 *
 *
 *  Author	: Vivek Kumar (vkumar10@ncsu.edu)
 *  		  Xipeng Shen (xshen5@ncsu.edu)
 *  		  North Carolina State University (www.ncsu.edu)
 *
 ******************************************************************************/

#include <string>
#include <fstream>
#include <iostream>
#include "scanner.h"
#include <locale>
#include <utility>
#include <algorithm>
using namespace std;
/*
 * @args: input file name
 * @ret : no return for constructor
 * @desc: Opens the given input file. It also opens an output file whose
 * 	  name is same as input file name but has "_gen.c" appended to it.
 */
Scanner::Scanner(char * file)
{
	/* If the input filename is abc.c, output filename will be abc_gen.c */
	InputFileName = file;
	OutputFileName = InputFileName.substr(0, InputFileName.find("."));
	OutputFileName += "_gen.c";

	/* Open both input and output files */
	InputFile.open(InputFileName.c_str());
	OutputFile.open(OutputFileName.c_str());
	//cout << "Input file opened: " << InputFileName << endl;
	//cout << "Output file opened: " << OutputFileName << endl;
}

/* @func: ~Scanner() - destructor
 * @args: no argument
 * @ret : no return for destructor
 * @desc: Closes the opened input and output files
 */
Scanner::~Scanner()
{
	InputFile.close();
	OutputFile.close();
	//cout << "Input file closed: " << InputFileName << endl;
	//cout << "Output file closed: " << OutputFileName << endl;
}
void Scanner::CloseScanner()
{
	InputFile.close();
	OutputFile.close();
}

/* @func: HasMoreTokens()
 * @args: void
 * @ret : 1 if there are more tokens, 0 otherwise
 * @desc: Checks if there are more tokens available
 */
int Scanner::HasMoreTokens(void)
{
	int ret;
	/* If the scanner reaches the EOF, means no tokens left */
	ret = InputFile.eof() ? 0 : 1;
	return ret;
}

/* @func: GetNextToken()
 * @args: void
 * @ret : Token object
 * @desc: Scans the input file and returns the next token
 */

int Scanner::Parse(){

return 0;

}
/*Token Scanner::GetActualToken(void){
	Token token;
	token = scanner.GetNextToken();

	if (token.GetTokenKind == OTHER || token.GetTokenKind == ERROR) {
		cerr << "Parse error!!!" << endl;
		// In case of error, only message is displayed.
		// So remove the output file
		scanner.DeleteOutputFile();
		return -1;
	}

	while (token.GetTokenKind == SPACE) {
		if (token.GetTokenKind == OTHER || token.GetTokenKind == ERROR) {
			cerr << "Parse error!!!" << endl;
			// In case of error, only message is displayed.
			// So remove the output file
			scanner.DeleteOutputFile();
			return -1;
		}
		token = scanner.GetNextToken();
	}

	return token;

}*/


Token Scanner::GetNextToken(void)
{
	char c;
	char copy;
	Token token;
	InputFile.get(c);
	//cout << "Index = " << InputFile.tellg() << ", character = " << c << endl;
	/* Letter */
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c == '_')) {
		/* Note:
		 * 1) Any token that starts with a letter will either be an 
		 *    identifier or a reserved word
		 * 2) Apply greedy approach: Scan till you get character or digit.
		 * 3) Check the longest word scanned and determine if it qualifies 
		 *    as an identifier or a reserved word.
		 */
		/* Scan till you get character or digit */
		while ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
				(c == '_') || (c >= '0' && c <= '9')) {
			//cout << " " << c << endl;
			token.Text.append(1, c);
			InputFile.get(c);
		}
		/* Check if the scanned word is an identifier or a reserved word */
		if (//(token.Text.compare("main") == 0) ||
				(token.Text.compare("int") == 0 && token.SetTokenKind(INT)) ||
				(token.Text.compare("void") == 0 && token.SetTokenKind(VOID)) ||
				(token.Text.compare("if") == 0 && token.SetTokenKind(IF)) ||
				(token.Text.compare("while") == 0 && token.SetTokenKind(WHILE)) ||
				(token.Text.compare("return") == 0 && token.SetTokenKind(RETURN)) ||
				(token.Text.compare("read") == 0 && token.SetTokenKind(READ)) ||
				(token.Text.compare("write") == 0 && token.SetTokenKind(WRITE)) ||
				(token.Text.compare("print") == 0 && token.SetTokenKind(PRINT)) ||
				(token.Text.compare("continue") == 0 && token.SetTokenKind(CONTINUE)) ||
				(token.Text.compare("break") == 0 && token.SetTokenKind(BREAK)) ||
				(token.Text.compare("binary") == 0 && token.SetTokenKind(BINARY)) ||
				(token.Text.compare("decimal") == 0 && token.SetTokenKind(DECIMAL))) {
			token.SetTokenType(TOKEN_RESERVED_WORD); /* reserved word */
			/*switch(token.Text) {
			//switch(std::transform(token.Text.begin(), token.Text.end(), token.Text.begin(), ::tolower)) {
				case "int":
					token.SetTokenKind(INT);
					break;
				case "void":
					token.SetTokenKind(VOID);
					break;
				case "if":
					token.SetTokenKind(IF);
					break;
				case "while":
					token.SetTokenKind(WHILE);
					break;
				case "return":
					token.SetTokenKind(RETURN);
					break;
				case "read":
					token.SetTokenKind(READ);
					break;
				case "write":
					token.SetTokenKind(WRITE);
					break;
				case "print":
					token.SetTokenKind(PRINT);
					break;
				case "continue":
					token.SetTokenKind(CONTINUE);
					break;
				case "break":
					token.SetTokenKind(BREAK);
					break;
				case "binary":
					token.SetTokenKind(BINARY);
					break;
				case "decimal":
					token.SetTokenKind(DECIMAL);
					break;
				default:
					token.SetTokenKind(OTHER);
					break;
			}*/

		} else {
			token.SetTokenType(TOKEN_IDENTIFIER); /* identifier */
			token.SetTokenKind(ID);
		}
		/* Finally position the file pointer one character back,
		 * because we have read an extra character */
		if (InputFile.tellg() > 0)
			InputFile.seekg((long)InputFile.tellg() - 1);
		/* Digit */
	} else if (c >= '0' && c <= '9') {
		/* Note: 
		 * 1) Anything that starts with a digit will always be a number 
		 * 2) Scan till you are getting a digit.
		 */
		/* Scan till you get a digit */
		while (c >= '0' && c <= '9') {
			//cout << " " << c << endl;
			token.Text.append(1, c);
			InputFile.get(c);
		}
		token.SetTokenType(TOKEN_NUMBER);
		token.SetTokenKind(NUMBER);

		/* Finally position the file pointer one character back,
		 * because we have read an extra character */
		if (InputFile.tellg() > 0)
			InputFile.seekg((long)InputFile.tellg() - 1);
		/* String */
	} else if (c == '\"' ) {
		/* Note:
		 * 1) String is any set of characters between opening and closing
		 *    double quotes.
		 * 2) String cannot span multiple lines.
		 * 3) This means, if we get a closing double quote before new line,
		 *    that will be considered a string.
		 */
		/* Scan until you get a closing double quote or newline */
		token.Text.append(1, c);
		InputFile.get(c);
		while (c != '\"' && c != '\n') {
			token.Text.append(1, c);
			//cout << " " << c << endl;
			InputFile.get(c);
		}
		if (c == '\"') { /* String */
			token.Text.append(1, c);
			//cout << " " << c << endl;
			token.SetTokenType(TOKEN_STRING);
			token.SetTokenKind(STRING);
		} else { /* Error */
			token.SetTokenType(TOKEN_ERROR);
			token.SetTokenKind(ERROR);
			/* Finally position the file pointer one character back,
			 * because we have read an extra character */
			if (InputFile.tellg() > 0)
				InputFile.seekg((long)InputFile.tellg() - 1);
		}
		/* # - Meta statement */
	} else if (c == '#') {
		/* Note:
		 * 1) Check if the previous character is a new line,
		 *    except if # is used as first character of file
		 * 2) If the previous character is not a new line,
		 *    it has to be copied in a new line.
		 * 3) Scan until you get a new line character
		 */
		if (InputFile.tellg() == 1) { //If # is the first character
			token.Text.append(1, c);
			/* Scan until you get a new line character */
			InputFile.get(c);
			while(c != '\n') {
				//cout << " " << c << endl;
				token.Text.append(1, c);
				InputFile.get(c);
			}
		} else { //# is not the first character, search for previous '\n'
			/* Check for the previous character \n */
			InputFile.seekg((long)InputFile.tellg() - 2);
			InputFile.get(c);
			if (c != '\n') {
				/* Copy it in a new line */
				token.Text.append(1, '\n');
				InputFile.get(c);
				while(c != '\n') {
					//cout << " " << c << endl;
					token.Text.append(1, c);
					InputFile.get(c);
				}
			} else {
				/* Scan until you get a new line character */
				InputFile.get(c);
				while(c != '\n') {
					//cout << " " << c << endl;
					token.Text.append(1, c);
					InputFile.get(c);
				}
			}
		}
		token.SetTokenType(TOKEN_META_STATEMENT);
		token.SetTokenKind(META);
		/* '//' meta statement or '/' symbol*/
	} else if (c == '/') {
		/* Note:
		 * 1) If the next character is not '/', means this is a symbol.
		 * 2) If the next character is a '/' and the previous character
		 *    is a new line, copy it as a meta statement
		 * 3) If previous character is not new line, copy it as a meta
		 *    in the same line.
		 * 4) Scan until you get a new line character
		 */
		/* Check for the next character */
		InputFile.get(c);
		if (c != '/') { /* definitely '/' symbol */
			token.Text.append(1, '/');
			token.SetTokenType(TOKEN_SYMBOL);
			token.SetTokenKind(SLASH);
			/* Finally position the file pointer one character back,
			 * because we have read an extra character */
			InputFile.seekg((long)InputFile.tellg() - 1);
		} else { /* '//' meta statement */
			token.Text.append(2, '/');
			InputFile.get(c);
			/* Scan until you get a new line character */
			while(c != '\n') {
				//cout << " " << c << endl;
				token.Text.append(1, c);
				InputFile.get(c);
			}
			token.SetTokenType(TOKEN_META_STATEMENT);
			token.SetTokenKind(META);
		}
		/* Others including symbols and errors */
	} else {
		switch (c) {
			case '(': //symbol (
				token.Text.append(1, c);
				token.SetTokenType(TOKEN_SYMBOL);
				token.SetTokenKind(LEFT_PARENTHESIS);				
				break;
			case ')': //symbol )
				token.Text.append(1, c);
				token.SetTokenType(TOKEN_SYMBOL);
				token.SetTokenKind(RIGHT_PARENTHESIS);
				break;
			case '{': //symbol {
				token.Text.append(1, c);
				token.SetTokenType(TOKEN_SYMBOL);
				token.SetTokenKind(LEFT_BRACE);
				break;
			case '}': //symbol }
				token.Text.append(1, c);
				token.SetTokenType(TOKEN_SYMBOL);
				token.SetTokenKind(RIGHT_BRACE);
				break;
			case '[': //symbol [
				token.Text.append(1, c);
				token.SetTokenType(TOKEN_SYMBOL);
				token.SetTokenKind(LEFT_BRACKET);
				break;
			case ']': //symbol ]
				token.Text.append(1, c);
				token.SetTokenType(TOKEN_SYMBOL);
				token.SetTokenKind(RIGHT_BRACKET);
				break;
			case ',': //symbol ,
				token.Text.append(1, c);
				token.SetTokenType(TOKEN_SYMBOL);
				token.SetTokenKind(COMMA);
				break;
			case ';': //symbol ;
				token.Text.append(1, c);
				token.SetTokenType(TOKEN_SYMBOL);
				token.SetTokenKind(SEMICOLON);
				break;
			case '+': //symbol +
				token.Text.append(1, c);
				token.SetTokenType(TOKEN_SYMBOL);
				token.SetTokenKind(PLUS);
				break;
			case '-': //symbol -
				token.Text.append(1, c);
				token.SetTokenType(TOKEN_SYMBOL);
				token.SetTokenKind(MINUS);
				break;
			case '*': //symbol *
				token.Text.append(1, c);
				token.SetTokenType(TOKEN_SYMBOL);
				token.SetTokenKind(STAR);
				break;
			/* case '/': //symbol / --> This case is covered in meta-statement */
			case '=': //symbol = and ==
			case '!': //symbol !=
			case '>': //symbol > and >=
			case '<': //symbol < and <=
				copy = c;
				token.Text.append(1, c);
				InputFile.get(c);
				if (c == '=') {
					token.Text.append(1, c);
					token.SetTokenType(TOKEN_SYMBOL);
					switch (copy) {
						case '=':
							token.SetTokenKind(DOUBLE_EQUAL);
							break;
						case '!':
							token.SetTokenKind(NOT_EQUAL);
							break;
						case '>':
							token.SetTokenKind(GREATER_EQUAL);
							break;
						case '<':
							token.SetTokenKind(LESS_EQUAL);
							break;
					}
				} else {
					token.SetTokenType(TOKEN_SYMBOL);
					switch (copy) {
						case '=':
							token.SetTokenKind(EQUAL_SIGN);
							break;
						case '>':
							token.SetTokenKind(GREATER);
							break;
						case '<':
							token.SetTokenKind(LESS);
							break;
					}
					/* Finally position the file pointer one character back,
					 * because we have read an extra character */
					if (InputFile.tellg() > 0)
						InputFile.seekg((long)InputFile.tellg() - 1);
				}
				break;
			case '&': //symbol &&
				token.Text.append(1, c);
				InputFile.get(c);
				if (c == '&') {
					token.Text.append(1, c);
					token.SetTokenType(TOKEN_SYMBOL);
					token.SetTokenKind(DOUBLE_AND);
				} else {
					token.SetTokenType(TOKEN_SYMBOL);
					token.SetTokenKind(OTHER);
					/* Finally position the file pointer one character back,
					 * because we have read an extra character */
					if (InputFile.tellg() > 0)
						InputFile.seekg((long)InputFile.tellg() - 1);
				}
				break;
			case '|': //symbol ||
				token.Text.append(1, c);
				InputFile.get(c);
				if (c == '|') {
					token.Text.append(1, c);
					token.SetTokenType(TOKEN_SYMBOL);
					token.SetTokenKind(DOUBLE_OR);
				} else {
					token.SetTokenType(TOKEN_SYMBOL);
					token.SetTokenKind(OTHER);
					/* Finally position the file pointer one character back,
					 * because we have read an extra character */
					if (InputFile.tellg() > 0)
						InputFile.seekg((long)InputFile.tellg() - 1);
				}
				break;
			case 0 : // null: There is a null character before EOF
				token.SetTokenType(TOKEN_SPACE_NEWLINE);
				token.SetTokenKind(SPACE);
				break;
			case ' ' : //space
			case '\t': //tab space
				/* Keep the same format i.e keep spaces and new line in output file */
				token.Text.append(1, c);
				token.SetTokenType(TOKEN_SPACE_NEWLINE);
				token.SetTokenKind(SPACE);
				break;
			case '\n': //new line
				token.Text.append(1, c);
				token.SetTokenType(TOKEN_SPACE_NEWLINE);
				token.SetTokenKind(SPACE);
				break;
			default:  //error cases
				token.SetTokenType(TOKEN_ERROR);
				token.SetTokenKind(ERROR);
				break;
		}
	}
	
	if (token.GetTokenKind() == SPACE || token.GetTokenKind() == META) {
		Token token2;
		token2 = GetNextToken();
		return token2;
	}
	return token;
}





