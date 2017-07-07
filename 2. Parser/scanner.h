#ifndef SCANNER_H
#define SCANNER_H
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

using namespace std;
/* 
 * The tokens include <identifier>, <number>, <reserved word>, <symbol>, 
 * <string>, and <meta statement>.  They are defined as regular expressions 
 * as follows:
 * <letter>         --> a | b | ... | y | z | A | B | ... | Z | underscore 
 * <identifier>     --> <letter> (<letter> | <digit>)*
 * <digit>          --> 0 | 1 | ... | 9 
 * <number>         --> <digit>+
 * <reserved word>  --> int  | void | if | while | return | read | write | 
 * 		    	print | continue | break | binary | decimal
 * <symbol>         --> left_parenthesis | right_parenthesis | left_brace | 
 * 		      	right_brace | left_bracket | right_bracket | comma | 
 * 	               	semicolon | plus_sign | minus_sign | star_sign | 
 * 	               	forward_slash | == | != | > | >= | < | <= | equal_sign |
 * 	               	double_and_sign | double_or_sign 
 * 		       	(The symbols are: ( ) { } [ ] , ; + - * / == != > >= < 
 * 		       			 <= = && ||)
 * <string>         --> any string between (and including) the closest pair of 
 * 		       	quotation marks. 
 * <meta statement> --> any string begins with '#' or '//' and ends with the 
 * 		       	end of line ('\n'). 
 *
 */
enum TokenTypes {
	TOKEN_IDENTIFIER = 1,
	TOKEN_NUMBER,
	TOKEN_RESERVED_WORD,
	TOKEN_SYMBOL,
	TOKEN_STRING,
	TOKEN_META_STATEMENT,
	TOKEN_SPACE_NEWLINE,
	TOKEN_ERROR,
	TOKEN_EOF
};

enum TokenKind {

	EMPTY = 1,
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
};
/* Class consists of token type and the token content */
class Token {
	enum TokenTypes Type; /* Which type of token */
	public:
		enum TokenKind Kind; /* Which kind of token type */
		string Text; /* Text content of token */
		Token() { /* Constructor */
			Type = TOKEN_ERROR;
			Kind = EMPTY;
			Text = "";
		}
		enum TokenTypes GetTokenType(void) {
			return Type;
		}
		enum TokenKind GetTokenKind(void) {
			return Kind;
		}
		void SetTokenType(enum TokenTypes type) {
			Type = type;
		}
		int SetTokenKind(enum TokenKind kind) {
			Kind = kind;
			return 1;
		}
};

/* Scanner class */
class Scanner {
	ifstream InputFile;	/* FILE pointer to read from input file */
	ofstream OutputFile;	/* FILE pointer to write to output file */
	string InputFileName;	/* Name of input file */
	string OutputFileName;	/* Name of generated output file */
	public:
		/* Constructor */
		Scanner(char *);
		/* Destructor */
		~Scanner();
		void CloseScanner();
		/* Are there any more tokens left in the input file */
		int HasMoreTokens(void);
		/* Return the next token from the input file */
		Token GetNextToken(void);
		//Token GetActualToken(void);
		/* Print normal token to output file */
		void PrintNormal(Token token) {
			//OutputFile << token.Text << "<" << token.Kind << ">";
			OutputFile << token.Text;
			//cout << token.Text;
		}
		/* Print meta statement to output file */
		void PrintMeta(Token token) {
			OutputFile << token.Text << endl;
		}
		/* Print identifier to output file */
		void PrintIdentifier(Token token) {
			OutputFile << "cs512" << token.Text;
		}
		/* Function to delete output file in case of error */
		void DeleteOutputFile() {
			remove(OutputFileName.c_str());
		}
		int Parse();
};
#endif //SCANNER_H
