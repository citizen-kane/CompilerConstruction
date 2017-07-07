/***********************************************************************************************
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
 ***********************************************************************************************/

#include <string>
#include <fstream>
#include <iostream>
#include <scanner.h>

using namespace std;

/* @func: Scanner() - constructor
 * @args: input file name
 * @ret : no return for constructor
 * @desc: Opens the given input file. It also opens an output file whose
 * 	  name is same as input file name but has "_gen.c" appended to it.
 */
Scanner::Scanner(char * file, CodeGen& codegen)
{
	/* Save code gen */
	c = &codegen;
	/* If the input filename is abc.c, output filename will be abc_gen.c */
	InputFileName = file;
	OutputFileName = InputFileName.substr(0, InputFileName.find("."));
	OutputFileName += "_gen2.c";

	OutputFileName2 = InputFileName.substr(0, InputFileName.find("."));
	OutputFileName2 += "_gen.c";


	/* Open both input and output files */
	InputFile.open(InputFileName.c_str());
	OutputFile.open(OutputFileName.c_str());
	OutputFile2.open(OutputFileName2.c_str());

	OutputFile2 << "#include <assert.h>\n#include <stdlib.h>\n";

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
	OutputFile2.close();
	DeleteOutputFile();
}

/* print the output to the output file */
void Scanner::PrintOutput(void)
{
	c->genVarCode();
	OutputFile << c->getVarCode();
	OutputFile << c->getFuncCode();
}

void Scanner::createCode(void)
{
	addMainPrelogue();
	addMainBody();
	addMainEpilogue();
	addFuncBody();
	addJumpCode();

	/*for (int i = 1; i <= c->funcIndex; ++i)
	{
		OutputFile2 << c->funcStore[i].funcName << "\n";
		OutputFile2 << c->funcStore[i].body << "\n";
	}*/
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
Token Scanner::GetNextToken(void)
{
	char c;
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
		if (/*(token.Text.compare("main") == 0) ||*/
				(token.Text.compare("int") == 0) ||
				(token.Text.compare("void") == 0) ||
				(token.Text.compare("if") == 0) ||
				(token.Text.compare("while") == 0) ||
				(token.Text.compare("return") == 0) ||
				(token.Text.compare("read") == 0) ||
				(token.Text.compare("write") == 0) ||
				(token.Text.compare("print") == 0) ||
				(token.Text.compare("continue") == 0) ||
				(token.Text.compare("break") == 0) ||
				(token.Text.compare("binary") == 0) ||
				(token.Text.compare("decimal") == 0)) {
			token.SetTokenType(TOKEN_RESERVED_WORD); /* reserved word */
		} else {
			token.SetTokenType(TOKEN_IDENTIFIER); /* identifier */
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
		} else { /* Error */
			token.SetTokenType(TOKEN_ERROR);
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
		}
		/* Others including symbols and errors */
	} else {
		switch (c) {
			case '(': //symbol (
			case ')': //symbol )
			case '{': //symbol {
			case '}': //symbol }
			case '[': //symbol [
			case ']': //symbol ]
			case ',': //symbol ,
			case ';': //symbol ;
			case '+': //symbol +
			case '-': //symbol -
			case '*': //symbol *
			/* case '/': //symbol / --> This case is covered in meta-statement */
				token.Text.append(1, c);
				token.SetTokenType(TOKEN_SYMBOL);
				break;
			case '=': //symbol = and ==
			case '!': //symbol !=
			case '>': //symbol > and >=
			case '<': //symbol < and <=
				token.Text.append(1, c);
				InputFile.get(c);
				if (c == '=') {
					token.Text.append(1, c);
					token.SetTokenType(TOKEN_SYMBOL);
				} else {
					token.SetTokenType(TOKEN_SYMBOL);
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
				} else {
					token.SetTokenType(TOKEN_SYMBOL);
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
				} else {
					token.SetTokenType(TOKEN_SYMBOL);
					/* Finally position the file pointer one character back,
					 * because we have read an extra character */
					if (InputFile.tellg() > 0)
						InputFile.seekg((long)InputFile.tellg() - 1);
				}
				break;
			case 0 : // null: There is a null character before EOF
				token.SetTokenType(TOKEN_EOF);
				break;
			case ' ' : //space
			case '\t': //tab space
				/* Keep the same format i.e keep spaces and new line in output file */
				token.Text.append(1, c);
				token.SetTokenType(TOKEN_SPACE_NEWLINE);
				break;
			case '\n': //new line
				token.Text.append(1, c);
				token.SetTokenType(TOKEN_SPACE_NEWLINE);
				break;
			default:  //error cases
				token.SetTokenType(TOKEN_ERROR);
				break;
		}
	}

	/* Check for EOF here */
	if (InputFile.tellg() == -1)
		token.SetTokenType(TOKEN_EOF);
	return token;
}

/* @func: GetParsingToken()
 * @args: no argument
 * @ret : Token
 * @desc: Returns a valid parsing token or an error token
 */
Token Scanner::GetParsingToken(void)
{
	int found;
	Token token;

	/* Initialize this token as last token */
	token.Type = TOKEN_EOF;

	/* Check for all tokens in the input file */
	while (HasMoreTokens()) {
		found = 0;
		token = GetNextToken();
		switch(token.GetTokenType()) {
			case TOKEN_NUMBER:
			case TOKEN_SYMBOL:
			case TOKEN_STRING:
			case TOKEN_IDENTIFIER:
			case TOKEN_RESERVED_WORD:
			case TOKEN_EOF:
			case TOKEN_ERROR:
				/* These are valid token cases or error */
				found = 1;
				break;
			case TOKEN_SPACE_NEWLINE:
				break;
			case TOKEN_META_STATEMENT:
				PrintMeta(token.Text);
				break;
		}

		/* Break from the loop if the token is found */
		if (found)
			break;
	}

	/* Return this valid parsing token */
	debug("Parsed token = %s\n", token.Text.c_str());
	return token;
}

