/************************************************************************************************************
 *
 *  File Name	: parser.cc
 *
 *  Description	: Write the parser, the second phase of your compiler.  The parser 
 *  		  should use your previous scanner (or the one that we will provide) 
 *  		  to read all tokens of an input program and check whether the program 
 *  		  is grammatically correct.  The specific requirements are:
 *  		  1) As in the scanner project, your parser should ignore (but copy) 
 *  		     "meta-statements".
 *  		  2) The grammer definition is shown below in the description. 
 *  		     Regardless the choice of your implementation, you must convert 
 *  		     the grammar into an LL(1) grammar and submit the modified grammar 
 *  		     in writing.
 *  		  3) When submitting your grammar, put down the number of production 
 *  		     rules and the number of distinct non-terminals used in your grammar. 
 *  		     For example, A -> aA | epsilon should be counted as two production 
 *  		     rules and one non-terminal.
 *  		  4) For each input program, your parser should report "pass" or "error" 
 *  		     as the result of grammar analysis.  It suffices to include only one 
 *  		     of these two words in the output (stdout or stderr) of your program.
 *  		  5) Count the number of global and local variables (through <data decls>), 
 *  		     functions (through <func list> but not function declarations), and 
 *  		     statements (through <statements>) in correct programs.  Output the 
 *  		     result in the following format: variable _num_variables  function 
 *  		     _num_functions statement _num_statements.  For example, for a correct 
 *  		     program with 4 variables, 2 functions and 6 statements, your compiler 
 *  		     should output at least the following:
 *  				Pass variable 4 function 2 statement 6
 *
 *************************************************************************************************************************/

#include <string>
#include <fstream>
#include <iostream>
#include <parser.h>
#include <scanner.h>

/* Check for a token error */
#define CHECK_FOR_TOKEN_ERROR() debug("Non-Terminal = %s, Token = %s\n", __func__, token.Text.c_str());\
				if (token.Type == TOKEN_ERROR) return false;

/* Check for Enf of file */
#define CHECK_FOR_TOKEN_EOF() if (token.Type == TOKEN_EOF) { s->createCode(); return true;}

using namespace std;

/* Constructor for parser */
Parser::Parser(Scanner& scanner, CodeGen& codegen)
{
	/* Save a pointer to scanner, and code gen */
	s = &scanner;
	c = &codegen;
	/* Reset all counts */
	//NumberOfVariables = 0;
	//NumberOfFunctions = 0;
	//NumberOfStatements = 0;
}

/* 
 * Parse non-terminal symbol <program>:
 * -----------------------------------
 * <program>	-->	<data decls> <func list> 
 *
 * The above production has been simplified to:
 *
 * <program>	-->	  int ID left_parenthesis <parameter list> right_parenthesis <func prime> <program>
 *			| void ID left_parenthesis <parameter list> right_parenthesis <func prime> <program>
 *			| binary ID left_parenthesis <parameter list> right_parenthesis <func prime> <program>
 *			| decimal ID left_parenthesis <parameter list> right_parenthesis <func prime> <program>
 *
 * 			| int ID left_bracket <expression> right_bracket <id list prime> semicolon <program>
 *			| void ID left_bracket <expression> right_bracket <id list prime> semicolon <program>
 *			| binary ID left_bracket <expression> right_bracket <id list prime> semicolon <program>
 *			| decimal ID left_bracket <expression> right_bracket <id list prime> semicolon <program>
 *
 *			| int ID comma <id> <id list prime> semicolon <program>
 *			| void ID comma <id> <id list prime> semicolon <program>
 *			| binary ID comma <id> <id list prime> semicolon <program>
 *			| decimal ID comma <id> <id list prime> semicolon <program>
 *			
 *			| int ID semicolon <program>
 *			| void ID semicolon <program>
 *			| binary ID semicolon <program>
 *			| decimal ID semicolon <program>
 *			
 *			| empty
 * */
bool Parser::parseProgram(void)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();
	/* Return in case of end-of-file */
	CHECK_FOR_TOKEN_EOF();

	/* Variable type and id name */
	string type, id;

 	if ((token.Text == "int") || (token.Text == "void") 
		|| (token.Text == "binary") || (token.Text == "decimal")) {
		type = token.Text;
		token = s->GetParsingToken();
		if (token.Type == TOKEN_IDENTIFIER) {
			id = token.Text;
			token = s->GetParsingToken();
			 /* <program> -->  int ID left_paranthesis <parameter list> ...
			  * 		 | void ID left_paranthesis <parameter list> ...
			  * 		 | binary ID left_paranthesis <parameter list> ...
			  * 		 | decimal ID left_paranthesis <parameter list> ...
			  */
			if (token.Text == "(") {
				/* Variable declaration in intra-function code generation */
				c->appendFuncCode("\n" + type + " " + id + "(");
				token = s->GetParsingToken();
				s->paraIndex = 0;
				if (parseParameterList()) {
					if (token.Text == ")") {
						c->appendFuncCode(")");
						token = s->GetParsingToken();
						c->currFuncName = id;
						if (parseFuncPrime())
							return parseProgram();
						else
							return false;
					} else {
						return false;
					}
				} else {
					return false;
				}
 			} else if (token.Text == "[") {
				/* <program> -->  int ID left_bracket <expression> ...
				 * 		 | void ID left_bracket <expression> ...
				 * 		 | binary ID left_bracket <expression> ...
				 * 		 | decimal ID left_bracket <expression> ...
				 */
				token = s->GetParsingToken();
				/* Save type */
				c->setVarType(type);
				string expr = "";
				if (parseExpression(expr)) {
					/* Array declaration cannot be of the form 'int arr[x+y]'
					 * Its of the form 'int arr[2]'. Hence expr string here 
					 * can be directly converted to integer */
					c->saveSymbol(id, stoi(expr));
					if (token.Text == "]") {
						token = s->GetParsingToken();
						if (parseIdListPrime()) {
							if (token.Text == ";") {
								token = s->GetParsingToken();
								return parseProgram();
							} else {
								return false;
							}
						} else {
							return false;
						}
					} else {
						return false;
					}
				} else {
					return false;
				}
			} else if (token.Text == ",") {
				 /* <program> -->  int ID comma <id> <id list prime> semicolon <program>
				  *		 | void ID comma <id> <id list prime> semicolon <program>
				  *		 | binary ID comma <id> <id list prime> semicolon <program>
				  *		 | decimal ID comma <id> <id list prime> semicolon <program>
				  */
				//NumberOfVariables++;
				/* Save type */
				c->setVarType(type);
				/* save this variable in symbol table */
				c->saveSymbol(id, 1);
				token = s->GetParsingToken();
				if (parseId(token.Text)) {
					if (parseIdListPrime()) {
						if (token.Text == ";") {
							token = s->GetParsingToken();
							return parseProgram();
						} else {
							return false;
						}
					} else {
						return false;
					}
				} else {
					return false;
				}
 			} else {
				 /* <program> -->   int ID semicolon <program>
				  *		  | void ID semicolon <program>
				  *		  | binary ID semicolon <program>
				  *		  | decimal ID semicolon <program>
				  */
				if (token.Text == ";") {
					//NumberOfVariables++;
					/* Save type */
					c->setVarType(type);
					/* save this variable in symbol table */
					c->saveSymbol(id, 1);
					token = s->GetParsingToken();
					return parseProgram();
				} else {
					return false;
				}
			}
		} else {
			/* Invalid syntax */
			return false;
		}
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* **** THIS IS A REDUNDANT FUNCTION NOW ****
 * ------------------------------------------
 * Parse non-terminal symbol <func list>:
 * ------------------------------------------
 * <func list>	-->	  <func> <func list>
 * 			| empty 
 * 
 * */
bool Parser::parseFuncList(void)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if (parseFunc()) {
		/* <func list>  -->  <func> <func list> */
		return parseFuncList();
	} else {
		/* <func list>  -->  <empty> */
		return true;
	}
}


/* **** THIS IS A REDUNDANT FUNCTION NOW ****
 * ------------------------------------------
 * Parse non-terminal symbol <func>:
 * ------------------------------------------
 * <func>	-->	<func decl> <func prime>
 *
 *  */
bool Parser::parseFunc(void)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if (parseFuncDecl()) {
		/* <func>  -->  <func decl> <func prime> */
		return parseFuncPrime();
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* Parse non-terminal symbol <func prime>:
 * --------------------------------------
 * <func prime>		-->	  left_brace <data decls> <statements> right_brace 
 * 				| semicolon
 *
 *  */
bool Parser::parseFuncPrime(void)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if (token.Text == "{") {
		s->appendMetaCode();
		/* Function code starts here */
		/* Saving function name in the code generator class's funcStore array object reference*/
		c->saveFuncName(c->currFuncName, s->paraIndex, s->paramArray);
		s->paraIndex = 0;

		/* hence all variable are reset below*/
		c->appendFuncCode("\n{\n");
		/* <func prime> --> left_brace <data decls> <statements> right_brace */
		token = s->GetParsingToken();
		/* Dump to currently stored output, which is of previous function or global scope */
		s->PrintOutput();
		/* Reset all local variables */
		c->resetLocal();
		if (parseDataDecls()) {
			c->inFunc();
			if (parseStatements()) {
				c->outFunc();
				c->saveFuncVarNum();
				if (token.Text == "}") {
					/* One more function declration complete */
					//NumberOfFunctions++;
					c->appendFuncCode("}\n");
					/* Dump to output */
					s->PrintOutput();
					/* Reset variables */
					c->resetLocal();
					token = s->GetParsingToken();
					return true;
				} else {
					/* Invalid syntax */
					return false;
				}
			} else {
				/* Invalid syntax */
				return false;
			}
		} else {
			/* Invalid syntax */
			return false;
		}
	} else if (token.Text == ";") {
		/* <func prime> --> semicolon */
		c->appendFuncCode(";\n");
		token = s->GetParsingToken();
		return true;
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* **** THIS IS A REDUNDANT FUNCTION NOW ****
 * ------------------------------------------
 * Parse non-terminal symbol <func decl>:
 * ------------------------------------------
 * <func decl>	-->	<type name> ID left_parenthesis <parameter list> right_parenthesis 
 *
 * */
bool Parser::parseFuncDecl(void)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if (parseTypeName()) {
		if (token.Type == TOKEN_IDENTIFIER) {
			token = s->GetParsingToken();
			if (token.Text == "(") {
				token = s->GetParsingToken();
				if (parseParameterList()) {
					if (token.Text == ")") {
						token = s->GetParsingToken();
						return true;
					} else {
						/* Invalid syntax */
						return false;
					}
				} else {
					/* Invalid syntax */
					return false;
				}
			} else {
				/* Invalid syntax */
				return false;
			}
		} else {
			/* Invalid syntax */
			return false;
		}
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* Parse non-terminal symbol <type name>:
 * -------------------------------------
 * <type name>		-->	  int 
 * 				| void 
 * 				| binary 
 * 				| decimal 
 *
 *  */
bool Parser::parseTypeName(void)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if ((token.Text == "int") || (token.Text == "void") 
		|| (token.Text == "binary") || (token.Text == "decimal")) {
		/* Append variable type name in code generation */
		c->appendFuncCode(token.Text + " ");
		token = s->GetParsingToken();
		return true;
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* Parse non-terminal symbol <parameter list>:
 * ------------------------------------------
 * <parameter list>	-->	  int ID <non-empty list prime>
 * 				| binary ID <non-empty list prime>
 * 				| decimal ID <non-empty list prime> 
 * 				| void <parameter list prime>
 * 				| empty
 * 												
 * */
bool Parser::parseParameterList(void)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if ((token.Text == "int") || (token.Text == "binary") || (token.Text == "decimal")) {
		/* <parameter list> -->   int ID <non-empty list prime>
		 * 			| binary ID <non-empty list prime>
		 * 			| decimal ID <non-empty list prime> */
		/* Append variable type in code generation */
		c->appendFuncCode(token.Text + " ");
		token = s->GetParsingToken();
		if (token.Type == TOKEN_IDENTIFIER) {
			/* Append variable name in code generation */
			c->appendFuncCode(token.Text);
			s->paraIndex++;
			s->paramArray[s->paraIndex] = token.Text;
			token = s->GetParsingToken();
			return parseNonEmptyListPrime();
		} else {
			/* Invalid syntax */
			return false;
		}
	} else if (token.Text == "void") {
		/* Append variable type in code generation */
		c->appendFuncCode(token.Text);
		/* <parameter list> --> void <parameter list prime> */
		token = s->GetParsingToken();
		return parseParameterListPrime();
	} else {
		/* <parameter list> --> empty */
		return true;
	}
}


/* Parse non-terminal symbol <parameter list prime>:
 * ------------------------------------------------
 * <parameter list prime>	-->	  ID <non-empty list prime>
 * 					| empty
 *
 *  */
bool Parser::parseParameterListPrime(void)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if (token.Type == TOKEN_IDENTIFIER) {
		/* <parameter list prime> --> ID <non-empty list prime> */
		/* Append variable name in code generation */
		c->appendFuncCode(" " + token.Text);
		s->paraIndex++;
		s->paramArray[s->paraIndex] = token.Text;
		token = s->GetParsingToken();
		return parseNonEmptyListPrime();
	} else {
		/* <parameter list prime> --> empty */
		return true;
	}
}


/* **** THIS IS A REDUNDANT FUNCTION NOW ****
 * ------------------------------------------
 * Parse non-terminal symbol <non-empty list>:
 * ------------------------------------------
 * <non-empty list>	--> 	  int ID <non-empty list prime>
 * 				| void ID <non-empty list prime>
 * 				| binary ID <non-empty list prime>
 * 				| decimal ID <non-empty list prime>
 *
 * */
bool Parser::parseNonEmptyList(void)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

 	/* <non-empty list> -->   int ID <non-empty list prime>
	 * 			| void ID <non-empty list prime>
	 * 			| binary ID <non-empty list prime>
	 * 			| decimal ID <non-empty list prime> */
	if ((token.Text == "int") || (token.Text == "void") 
		|| (token.Text == "binary") || (token.Text == "decimal")) {
		token = s->GetParsingToken();
		if (token.Type == TOKEN_IDENTIFIER) {
			token = s->GetParsingToken();
			return parseNonEmptyListPrime();
		} else {
			/* Invalid syntax */
			return false;
		}
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* Parse non-terminal symbol <non-empty list prime>:
 * ------------------------------------------------
 * <non-empty list prime>	-->	  comma <type name> ID <no-empty list prime>
 * 					| empty
 *
 *  */
bool Parser::parseNonEmptyListPrime(void)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	/* <non-empty list prime> --> comma <type name> ID <no-empty list prime> */
	if (token.Text == ",") {
		c->appendFuncCode(", ");
		token = s->GetParsingToken();
		if (parseTypeName()) {
			if (token.Type == TOKEN_IDENTIFIER) {
				/* Append variable name in code generation */
				c->appendFuncCode(token.Text);
				s->paraIndex++;
				s->paramArray[s->paraIndex] = token.Text;
				token = s->GetParsingToken();
				return parseNonEmptyListPrime();
			}
			/* Invalid syntax */
			return false;
		} else {
			/* Invalid syntax */
			return false;
		}
	} else {
		/* <non-empty list prime> --> empty */
		return true;
	}
}


/* Parse non-terminal symbol <data decls>:
 * --------------------------------------
 * <data decls>		--> 	  int <id list> semicolon <data decls> 
 * 				| void <id list> semicolon <data decls> 
 * 				| binary <id list> semicolon <data decls> 
 * 				| decimal <id list> semicolon <data decls> 
 * 				| empty
 *
 *  */
bool Parser::parseDataDecls(void)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if ((token.Text == "int") || (token.Text == "void") 
		|| (token.Text == "binary") || (token.Text == "decimal")) {
		/* <data decls>  -->  int <id list> semicolon <data decls> 
		 * 		   | void <id list> semicolon <data decls> 
		 * 		   | binary <id list> semicolon <data decls> 
		 * 	 	   | decimal <id list> semicolon <data decls> */
		/* Set variable type in symbol table */
		c->setVarType(token.Text);
 		token = s->GetParsingToken();
		if (parseIdList(token.Text)) {
			if (token.Text == ";") {
				token = s->GetParsingToken();
				return parseDataDecls();
			} else {
				/* Invalid syntax */
				return false;
			}
		} else {
				/* Invalid syntax */
			return false;
		}
	} else {
		/* <data decls>  -->  empty */
		return true;
	}
}


/* Parse non-terminal symbol <id list>:
 * -----------------------------------
 * <id list>		--> 	  <id> <id list prime>
 *
 *  */
bool Parser::parseIdList(string id)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if (parseId(id)) {
		/* <id list> --> <id> <id list prime> */
		return parseIdListPrime();
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* Parse non-terminal symbol <id list prime>:
 * <id list prime>	-->	  comma <id> <id list prime>
 * 				| empty
 *
 * */
bool Parser::parseIdListPrime(void)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	/* One more variable declared */
	//NumberOfVariables++;

	if (token.Text == ",") {
		/* <id list prime> --> comma <id> <id list prime */
		token = s->GetParsingToken();
		if (parseId(token.Text))
			return parseIdListPrime();
		else
			return false; /* Invalid syntax */
	} else {
		/*  <id list prime> --> empty */
		return true;
	}
}


/* Parse non-terminal symbol <id>:
 * ------------------------------
 * <id>		--> 	  ID <id prime>
 *
 *  */
bool Parser::parseId(string id)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();
	if (token.Type == TOKEN_IDENTIFIER) {
		token = s->GetParsingToken();
		return parseIdPrime(id);
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* Parse non-terminal symbol <id prime>:
 * ------------------------------------
 * <id prime>	-->	  left_bracket <expression> right_bracket
 * 			| empty
 *
 *  */
bool Parser::parseIdPrime(string id)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if (token.Text == "[") {
		/* <id prime>  -->  left_bracket <expression> right_bracket */
		token = s->GetParsingToken();
		/* Array declaration cannot be of the form 'int arr[x+y]'
		 * Its of the form 'int arr[2]'. Hence 'token.Text' string 
		 * here can be directly converted to integer */
		c->saveSymbol(id, stoi(token.Text));
		string expr = "";
		if (parseExpression(expr)) {
			if (token.Text == "]") {
				token = s->GetParsingToken();
				return true;
			} else {
				/* Invalid syntax */
				return false;
			}
		} else {
			/* Invalid syntax */
			return false;
		}
	} else {
		/* <id prime>  -->  empty */
		/* save this variable in symbol table */
		c->saveSymbol(id, 1);
		return true;
	}
}


/* Parse non-terminal symbol <block statements>:
 * --------------------------------------------
 * <block statements>		-->	  left_brace <statements> right_brace
 *
 *  */
bool Parser::parseBlockStatements(void)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if (token.Text == "{") {
		token = s->GetParsingToken();
		if (parseStatements()) {
			if (token.Text == "}") {
				token = s->GetParsingToken();
				return true;
			} else {
				/* Invalid syntax */
				return false;
			}
		} else {
			/* Invalid syntax */
			return false;
		}
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* Parse non-terminal symbol <statements>:
 * --------------------------------------
 * <statements>	 	-->	 <statement> <statements> 
 * 				| empty
 *
 *  */
bool Parser::parseStatements(void)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if (parseStatement()) {
		/* <statements>  -->  <statement> <statements> */
		return parseStatements();
	} else {
		/* Invalid syntax */
		return true;
	}
}


/* Parse non-terminal symbol <statement>:
 * -------------------------------------
 * <statement> 	--> 	  ID <statement prime>
 * 			| return <statement double-prime>
 * 			| if left_parenthesis <condition expression> right_parenthesis <block statements> 
 * 			| while left_parenthesis <condition expression> right_parenthesis <block statements>
 * 			| break semicolon 
 * 			| continue semicolon 
 * 			| read left_parenthesis ID right_parenthesis semicolon 
 * 			| write left_parenthesis <expression>right_parenthesis semicolon 
 * 			| print left_parenthesis  STRING right_parenthesis semicolon
 *
 *  */
bool Parser::parseStatement(void)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if (token.Type == TOKEN_IDENTIFIER) {
	/* <statement>  -->  ID <statement prime> */
		string id = token.Text;
		token = s->GetParsingToken();
		if (parseStatementPrime(id)) {
			/* One more statement */
			//NumberOfStatements++;
			return true;
		} else {
			/* Invalid syntax */
			return false;
		}
	} else if (token.Text == "return") {
	/* <statement>  -->  return <statement double-prime> */
		token = s->GetParsingToken();
		if (parseStatementDoublePrime()) {
			/* One more statement */
			//NumberOfStatements++;
			return true;
		} else {
			/* Invalid syntax */
			return false;
		}
	} else if ((token.Text == "if") || (token.Text == "while")) {
	/* <statement>  --> if left_parenthesis <condition expression> right_parenthesis <block statements> */
	/* <statement>  --> while left_parenthesis <condition expression> right_parenthesis <block statements> */
		string str1 = ""; /* For saving continue label in case of while */
		string str2 = ""; /* For storing if/while goto label */
		string str3 = ""; /* For storing a label that comes immediately after if statment */
		/* Only in case of while */
		if (token.Text == "while") {
			str1 = c->getNextlabel();
			c->setContLabel(); /* sets continue label to str1 */
			c->appendFuncCode(str1 + ":;\n");
		}
		/* for both if and else */
		str2 = c->getNextlabel();
		str3 = c->getNextlabel();
		if (token.Text == "while")
			c->setBreakLabel(); /* sets break label to str3 */

		token = s->GetParsingToken();
		if (token.Text == "(") {
			token = s->GetParsingToken();
			string str = "";
			if (parseConditionExpression(str)) {
				if (token.Text == ")") {
					/* This is the new grammar for code generation */
					c->appendFuncCode("if (" + str + ") ");
					c->appendFuncCode("goto " + str2 + ";\n");
					c->appendFuncCode("goto " + str3 + ";\n");
					c->appendFuncCode(str2 + ":;\n");
					token = s->GetParsingToken();
					if (parseBlockStatements()) {
						if (str1.size()) {
							/* This is the new grammar for code generation */
							c->appendFuncCode("goto " + str1 + ";\n");
							c->resetBlockLabel();
						}
						c->appendFuncCode(str3 + ":;\n");
						/* One more statement */
						//NumberOfStatements++;
						return true;
					} else {
						/* Invalid syntax */
						return false;
					}
				} else {
					/* Invalid syntax */
					return false;
				}
			} else {
				/* Invalid syntax */
				return false;
			}
		} else {
			/* Invalid syntax */
			return false;
		}
	} else if ((token.Text == "break") || (token.Text == "continue" )) {
		/* <statement>  --> break semicolon */
		/* <statement>  --> continue semicolon */

		/* This is the new grammar for code generation */
		if (token.Text == "break") /* break statement */
			c->appendFuncCode("goto " + c->getBreakLabel() + ";\n");
		else /* continue statement */
			c->appendFuncCode("goto " + c->getContLabel() + ";\n");
		token = s->GetParsingToken();
		if (token.Text == ";") {
			token = s->GetParsingToken();
			/* One more statement */
			//NumberOfStatements++;
			return true;
		} else {
			/* Invalid syntax */
			return false;
		}
	} else if (token.Text == "read") {
	/*  <statement>  --> read left_parenthesis ID right_parenthesis semicolon */
		token = s->GetParsingToken();
		if (token.Text == "(") {
			token = s->GetParsingToken();
			if (token.Type == TOKEN_IDENTIFIER) {
				/* Get local[] variable from local symbol table */
				string sym = c->getSymbol(token.Text, 0);
				if (!sym.size()) /* Varibale not found in symbol table */
					c->saveSymbol(token.Text, 1);
				/* Add this variable in symbol table */
				sym = c->getSymbol(token.Text, 0);
				token = s->GetParsingToken();
				if (token.Text == ")") {
					token = s->GetParsingToken();
					if (token.Text == ";") {
						/* This is new grammar for code generation */
						c->appendFuncCode("read(" + sym + ");\n");
						token = s->GetParsingToken();
						/* One more statement */
						//NumberOfStatements++;
						return true;
					} else {
						/* Invalid syntax */
						return false;
					}
				} else {
					/* Invalid syntax */
					return false;
				}
			} else {
				/* Invalid syntax */
				return false;
			}
		} else {
			/* Invalid syntax */
			return false;
		}
	} else if (token.Text == "write") {
	/* <statement>  --> write left_parenthesis <expression> right_parenthesis semicolon */
		token = s->GetParsingToken();
		if (token.Text == "(") {
			token = s->GetParsingToken();
			string expr = "";
			if (parseExpression(expr)) {
				if (token.Text == ")") {
					token = s->GetParsingToken();
					if (token.Text == ";") {
						/* This is new grammar for code generation */
						c->appendFuncCode("write(" + expr + ");\n");
						token = s->GetParsingToken();
						/* One more statement */
						//NumberOfStatements++;
						return true;
					} else {
						/* Invalid syntax */
						return false;
					}
				} else {
					/* Invalid syntax */
					return false;
				}
			} else {
				/* Invalid syntax */
				return false;
			}
		} else {
			/* Invalid syntax */
			return false;
		}
	} else if (token.Text == "print") {
	/*  <statement>  --> print left_parenthesis  STRING right_parenthesis semicolon */
		token = s->GetParsingToken();
		if (token.Text == "(") {
			token = s->GetParsingToken();
			if (token.Type == TOKEN_STRING) {
				string str = token.Text;
				//string str = c->createLocalSymbol();
				//c->appendFuncCode(str + " = " + token.Text + ";\n");
				token = s->GetParsingToken();
				if (token.Text == ")") {
					token = s->GetParsingToken();
					if (token.Text == ";") {
						/* This is new grammar for code generation */
						c->appendFuncCode("print(" + str + ");\n");
						token = s->GetParsingToken();
						/* One more statement */
						//NumberOfStatements++;
						return true;
					} else {
						/* Invalid syntax */
						return false;
					}
				} else {
					/* Invalid syntax */
					return false;
				}
			} else {
				/* Invalid syntax */
				return false;
			}
		} else {
			/* Invalid syntax */
			return false;
		}
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* Parse non-terminal symbol <statement prime>:
 * -------------------------------------------
 * <statement prime>	-->	  equal_sign <expression> semicolon
 * 				| left_bracket <expression> right_bracket equal_sign <expression> semicolon
 * 				| left_parenthesis <expr list> right_parenthesis semicolon
 *
 *  */
bool Parser::parseStatementPrime(string id)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if (token.Text == "=") {
		/* Get local[] variable name from local symbol table */
		string sym = c->getSymbol(id, 0);
		if (!sym.size()) /* variable not found in symbol table */
			c->saveSymbol(id, 1);
		/* Add this variable in symbol table */
		sym = c->getSymbol(id, 0);
	/*  <statement prime> --> equal_sign <expression> semicolon */
		token = s->GetParsingToken();
		string expr;
		if (parseExpression(expr)) {
			if (token.Text == ";") {
				/* A new statement added in code gen */
				c->appendFuncCode(sym + " = " + expr + ";\n");
				token = s->GetParsingToken();
				return true;
			} else {
				/* Invalid syntax */
				return false;
			}
		} else {
			/* Invalid syntax */
			return false;
		}
	} else if (token.Text == "[") {
	/*  <statement prime> --> left_bracket <expression> right_bracket equal_sign <expression> semicolon */
		token = s->GetParsingToken();
		string expr1 = "";
		if (parseExpression(expr1)) {
			if (token.Text == "]") {
				string str;
				int index = c->getBaseAddress(id);
				if (expr1.at(0) == 'l') {/* l for local */
					/* Create a new local variable */
					str = c->createLocalSymbol();
					/* A new statement added in code gen */
					c->appendFuncCode(str + " = " + to_string((unsigned long long)index)
								+ " + " + expr1 + ";\n");
				} else { /* expr is a number */
					str = to_string((unsigned long long)(stoi(expr1) + index));
				}
				/* If global returns true */
				if (c->getScope(id)) /* global varibale */
					str = "global[" + str + "]";
				else /* local variable */
					str = "mem[base+" + str + "]";
				/*  <statement prime> --> equal_sign <expression> semicolon */
				token = s->GetParsingToken();
				if (token.Text == "=") {
					token = s->GetParsingToken();
					string expr2 = "";
					if (parseExpression(expr2)) {
						if (token.Text == ";") {
							/* A new statement added in code gen */
							c->appendFuncCode(str + " = " + expr2 + ";\n");
							token = s->GetParsingToken();
							return true;
						} else {
							/* Invalid syntax */
							return false;
						}
					} else {
						/* Invalid syntax */
						return false;
					}
				} else {
					/* Invalid syntax */
					return false;
				}
			} else {
				/* Invalid syntax */
				return false;
			}
		} else {
			/* Invalid syntax */
			return false;
		}
	} else if (token.Text == "(") {
		c->saveFuncCallName(id);
	/*  <statement prime> --> left_parenthesis <expr list> right_parenthesis semicolon */
		token = s->GetParsingToken();
		string exprList = "";
		if (parseExprList(exprList)) {
			if (token.Text == ")") {
				token = s->GetParsingToken();
				if (token.Text == ";") {
					/* Function call in code gen */
					c->appendSpecial1FuncCode(id + "(" + exprList + ");\n", id);
					token = s->GetParsingToken();
					return true;
				} else {
					/* Invalid syntax */
					return false;
				}
			} else {
				/* Invalid syntax */
				return false;
			}
		} else {
			/* Invalid syntax */
			return false;
		}
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* Parse non-terminal symbol <statement double prime>:
 * --------------------------------------------------
 * <statement double-prime>	-->	  <expression> semicolon
 * 					| semicolon
 *
 * */
bool Parser::parseStatementDoublePrime(void)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	string expr = "";
	if (parseExpression(expr)) {
		/* <statement double-prime> --> <expression> semicolon */
		if (token.Text == ";") {
			string str = c->createLocalSymbol();
			/* Return statement's expression resolved into an assigment */
			c->appendFuncCode(str + " = " + expr + ";\n");
			
			/* Add return statement here in code gen */
			c->appendReturnFuncCode("return " + str + ";\n", str);
			token = s->GetParsingToken();
			return true;
		} else {
			/* Invalid syntax */
			return false;
		}
	} else if(token.Text == ";") {
		/* <statement double-prime> --> semicolon */
		/* Returning without any variable */
		c->appendReturn1FuncCode("return;\n");
		token = s->GetParsingToken();
		return true;
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* **** THIS IS A REDUNDANT FUNCTION NOW ****
 * ------------------------------------------
 * Parse non-terminal symbol <assigment>:
 * ------------------------------------------
 * <assignment> 	--> 	  ID <assignment prime>
 *
 *  */
bool Parser::parseAssignment(void)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if (token.Type == TOKEN_IDENTIFIER) {
		token = s->GetParsingToken();
		return parseAssignmentPrime();
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* **** THIS IS A REDUNDANT FUNCTION NOW ****
 * ---------------------------------------------
 * Parse non-terminal symbol <assignment prime>:
 * ---------------------------------------------
 * <assignment prime>	-->    equal_sign <expression> semicolon
 * 			     | left_bracket <expression> right_bracket equal_sign <expression> semicolon
 *
 * */
bool Parser::parseAssignmentPrime(void)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if (token.Text == "=") {
	/* <assignment prime> --> equal_sign <expression> semicolon */
		token = s->GetParsingToken();
		string expr = "";
		if (parseExpression(expr)) {
			if (token.Text == ";") {
				token = s->GetParsingToken();
				return true;
			} else {
				/* Invalid syntax */
				return false;
			}
		} else {
			/* Invalid syntax */
			return false;
		}
	} else if (token.Text == "[") {
	/* <assignment prime> --> left_bracket <expression> right_bracket equal_sign <expression> semicolon */
		token = s->GetParsingToken();
		string expr = "";
		if (parseExpression(expr)) {
			if (token.Text == "]") {
				token = s->GetParsingToken();
				if (token.Text == "=") {
					token = s->GetParsingToken();
					string expr = "";
					if (parseExpression(expr)) {
						if (token.Text == ";") {
							token = s->GetParsingToken();
							return true;
						} else {
							/* Invalid syntax */
							return false;
						}
					} else {
						/* Invalid syntax */
						return false;
					}
				} else {
					/* Invalid syntax */
					return false;
				}
			} else {
				/* Invalid syntax */
				return false;
			}
		} else {
			/* Invalid syntax */
			return false;
		}
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* **** THIS IS A REDUNDANT FUNCTION NOW ****
 * ------------------------------------------
 * Parse non-terminal symbol <func call>:
 * ------------------------------------------
 * <func call> 		--> 	  ID left_parenthesis <expr list> right_parenthesis semicolon
 *
 *  */
bool Parser::parseFuncCall(void)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();
	if (token.Type == TOKEN_IDENTIFIER) {
		token = s->GetParsingToken();
		if (token.Text == "(") {
			token = s->GetParsingToken();
			string exprList = "";
			if (parseExprList(exprList)) {
				if (token.Text == ")") {
					token = s->GetParsingToken();
					if (token.Text == ";") {
						token = s->GetParsingToken();
						return true;
					} else {
						/* Invalid syntax */
						return false;
					}
				} else {
					/* Invalid syntax */
					return false;
				}
			} else {
				/* Invalid syntax */
				return false;
			}
		} else {
			/* Invalid syntax */
			return false;
		}
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* Parse non-terminal symbol <expr list>:
 * -------------------------------------
 * <expr list> 		--> 	  <non-empty expr list> 
 * 				| empty
 *
 *  */
bool Parser::parseExprList(string& exprList)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	/* Anyway has to return true in both the cases */
	parseNonEmptyExprList(exprList);
	return true;
}


/* Parse non-terminal symbol <non-empty expr list>:
 * -----------------------------------------------
 * <non-empty expr list> 	--> 	  <expression> <non-empty expr list prime>
 *
 *  */
bool Parser::parseNonEmptyExprList(string& exprList)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	string expr = "";
	if (parseExpression(expr)) {
		/* Append this in the expression list passed */
		exprList += expr;
		c->saveFuncCallParam(expr);
		return parseNonEmptyExprListPrime(exprList);
	} else {
		return false; /* Invalid syntax */
	}
}


/* Parse non-terminal symbol <non-empty expr list prime>:
 * -----------------------------------------------------
 * <non-empty expr list prime>	-->	  comma <expression> <non-empty expr list prime>
 * 					| empty
 *
 *  */
bool Parser::parseNonEmptyExprListPrime(string& exprList)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if (token.Text == ",") {
	/* <non-empty expr list prime>  -->  comma <expression> <non-empty expr list prime> */
		token = s->GetParsingToken();
		string expr = "";
		if (parseExpression(expr)) {
			/* Append this in the expression list passed */
			exprList = exprList + ", " + expr;
			c->saveFuncCallParam(expr);
			return parseNonEmptyExprListPrime(exprList);
		} else {
			return false;
		}
	} else {
		/* <non-empty expr list prime>  -->  empty */
		return true;
	}
}


/* **** THIS IS A REDUNDANT FUNCTION NOW ****
 * ------------------------------------------
 * Parse non-terminal symbol <is statement>:
 * ------------------------------------------
 * <if statement> --> if left_parenthesis <condition expression> right_parenthesis <block statements>
 *
 *  */
bool Parser::parseIfStatement(void)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if (token.Text == "if") {
		token = s->GetParsingToken();
		if (token.Text == "(") {
			token = s->GetParsingToken();
			string str = "";
			if (parseConditionExpression(str)) {
				if (token.Text == ")") {
					token = s->GetParsingToken();
					return parseBlockStatements();
				} else {
					/* Invalid syntax */
					return false;
				}
			} else {
				/* Invalid syntax */
				return false;
			}
		} else {
			/* Invalid syntax */
			return false;
		}
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* Parse non-terminal symbol <condition expression>:
 * ------------------------------------------------
 * <condition expression> 	--> 	  <condition> <condition expression prime>
 *
 *  */
bool Parser::parseConditionExpression(string& str)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	/* Condition expression tree */
	eTree *t = NULL;
	if (parseCondition(&t)) {
		bool result = parseConditionExpressionPrime(&t);
		if (!result)
			return result;
		/* Parse the tree */
		c->parseCondTree(t, t);
		/* Final condition expression to be aded insed if/while statement */
		str = t->left->value + " " + t->value + " " + t->right->value;
		return result;
	} else {
		return false; /* Invalid syntax */
	}
}


/* Parse non-terminal symbol <condition expression prime>:
 * ------------------------------------------------------
 * <condition expression prime>	-->	  <condition op> <condition> 
 * 					| empty
 *
 *  */
bool Parser::parseConditionExpressionPrime(eTree **t)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if (parseConditionOp(t)) {
		/* <condition expression prime> --> <condition op> <condition> */
		return parseCondition(&((*t)->right));
	} else {
		/*  <condition expression prime> --> empty */
		return true;
	}
}


/* Parse non-terminal symbol <condition op>:
 * ----------------------------------------
 * <condition op>	--> 	  double_and_sign 
 * 				| double_or_sign
 *
 * */
bool Parser::parseConditionOp(eTree **t)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if ((token.Text == "&&") || (token.Text == "||")) {
		/* Populate a node in AST */
		eTree *n = new eTree();
		/* Populate this node */
		n->value = token.Text;
		n->type = OPERATION_NODE;
		n->left = *t; /* make current node as its left node */
		n->right = NULL;
		*t = n; /* make this node as root */
		token = s->GetParsingToken();
		return true;
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* Parse non-terminal symbol <condition>:
 * -------------------------------------
 *  <condition> 	--> 	  <expression> <comparison op> <expression>
 *
 * */
bool Parser::parseCondition(eTree **t)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	string expr1 = ""; /* expression before comparson operator */
	string expr2 = ""; /* expression after comparison operator */
	if (parseExpression(expr1)) {
		/* Populate a node in AST */
		if (!(*t)) { /* make this a node */
			eTree *n = new eTree();
			/* Populate this node */
			n->value = expr1;
			n->type = ARRAY_NODE;
			n->left = NULL;
			n->right = NULL;
			*t = n;
		}
		if (parseComparisonOp(t)) {
			bool result = parseExpression(expr2);
			if (!result)
				return result;
			/* Populate a node in AST */
			eTree *n = new eTree();
			/* Populate this node */
			n->value = expr2;
			n->type = ARRAY_NODE;
			n->left = NULL;
			n->right = NULL;
			/* This node becomes right child */
			(*t)->right = n;
			return result;
		} else {
			return false;
		}
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* Parse non-terminal symbol <comparison op>:
 * -----------------------------------------
 * <comparison op> 	--> 	  == 
 * 				| != 
 * 				| > 
 * 				| >= 
 * 				| < 
 * 				| <=
 *
 * */
bool Parser::parseComparisonOp(eTree **t)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if ((token.Text == "==") || (token.Text == "!=") || (token.Text == ">")
		|| (token.Text == ">=") || (token.Text == "<") || (token.Text == "<=")) {
		/* Populate a node in AST */
		eTree *n = new eTree();
		/* Populate this node */
		n->value = token.Text;
		n->type = OPERATION_NODE;
		n->left = *t; /* make current node as its left node */
		n->right = NULL;
		*t = n; /* make this node as root */
		token = s->GetParsingToken();
		return true;
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* **** THIS IS A REDUNDANT FUNCTION NOW ****
 * --------------------------------------------
 * Parse non-terminal symbol <while statement>:
 * --------------------------------------------
 * <while statement> --> while left_parenthesis <condition expression> right_parenthesis <block statements>
 *
 * */
bool Parser::parseWhileStatement(void)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if (token.Text == "while") {
		token = s->GetParsingToken();
		if (token.Text == "(") {
			token = s->GetParsingToken();
			string str = "";
			if (parseConditionExpression(str)) {
				if (token.Text == ")") {
					token = s->GetParsingToken();
					return parseBlockStatements();
				} else {
					/* Invalid syntax */
					return false;
				}
			} else {
				/* Invalid syntax */
				return false;
			}
		} else {
			/* Invalid syntax */
			return false;
		}
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* **** THIS IS A REDUNDANT FUNCTION NOW ****
 * --------------------------------------------
 * Parse non-terminal symbol <return statement>:
 * --------------------------------------------
 * <return statement> 	--> 	  return <return statement prime>
 *
 *  */
bool Parser::parseReturnStatement(void)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if (token.Text == "return") {
		token = s->GetParsingToken();
		return parseReturnStatementPrime();
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* **** THIS IS A REDUNDANT FUNCTION NOW ****
 * --------------------------------------------------
 * Parse non-terminal symbol <return statement prime>:
 * --------------------------------------------------
 * <return statement prime>	-->	  <expression> semicolon
 * 					| semicolon
 *
 *  */
bool Parser::parseReturnStatementPrime(void)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	string expr = "";
	if (parseExpression(expr)) {
		/* <return statement prime> --> <expression> semicolon */
		if (token.Text == ";") {
			token = s->GetParsingToken();
			return true;
		} else {
			/* Invalid syntax */
			return false;
		}
	} else if (token.Text == ";") {
		/* <return statement prime> --> semicolon */
		token = s->GetParsingToken();
		return true;
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* **** THIS IS A REDUNDANT FUNCTION NOW ****
 * -------------------------------------------
 * Parse non-terminal symbol <break statement>:
 * -------------------------------------------
 * <break statement> 		--> 	  break semicolon
 *
 * */
bool Parser::parseBreakStatement(void)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if (token.Text == "break") {
		token = s->GetParsingToken();
		if (token.Text == ";") {
			token = s->GetParsingToken();
			return true;
		} else {
			/* Invalid syntax */
			return false;
		}
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* **** THIS IS A REDUNDANT FUNCTION NOW ****
 * ----------------------------------------------
 * Parse non-terminal symbol <continue statement>:
 * ----------------------------------------------
 * <continue statement> 	--> 	  continue semicolon
 *
 *  */
bool Parser::parseContinueStatement(void)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if (token.Text == "continue") {
		token = s->GetParsingToken();
		if (token.Text == ";") {
			token = s->GetParsingToken();
			return true;
		} else {
			/* Invalid syntax */
			return false;
		}
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* Parse non-terminal symbol <expression>:
 * --------------------------------------
 * <expression> 	--> 	  <term> <expression prime>
 *
 *  */
bool Parser::parseExpression(string& str)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();
	/* Root of AST */
	eTree *t = NULL;
	bool result;
	if (parseTerm(&t)) {
		result = parseExpressionPrime(&t);
		if (!result) {
			return result;
		}

		/* Parse the tree */
		c->parseTree(t);
		/* This is the value of the root node of AST */
		str = t->value;
		return true;
	} else {
		return false; /* Invalid syntax */
	}
}


/* Parse non-terminal symbol <expression prime>:
 * --------------------------------------------
 * <expression prime>		-->	  <addop> <term> <expression prime>
 * 					| empty
 *
 *  */
bool Parser::parseExpressionPrime(eTree **t)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if (parseAddop(t)) {
		/* <expression prime> --> <addop> <term> <expression prime> */
		if (parseTerm(&((*t)->right)))
			return parseExpressionPrime(t);
		else
			return false;
	} else {
		/* <expression prime> --> empty */
		return true;
	}
}


/* Parse non-terminal symbol <addop>:
 * ---------------------------------
 * <addop> 	--> 	  plus_sign 
 * 			| minus_sign
 *
 * */
bool Parser::parseAddop(eTree **t)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if ((token.Text == "+") || (token.Text == "-")) {
		/* Create a node with value +/-; make this node as root and t as its left child */
		eTree *n = new eTree();
		/* Populate this node */
		n->type = OPERATION_NODE;
		n->value = token.Text;
		n->left = *t; /* make t as its left child */
		n->right = NULL;
		*t = n; /* make n as root */
		token = s->GetParsingToken();
		return true;
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* Parse non-terminal symbol <term>:
 * --------------------------------
 * <term> 	--> 	  <factor> <term prime>
 *
 *  */
bool Parser::parseTerm(eTree **t)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if (parseFactor(t))
		return parseTermPrime(t);
	else
		return false;
}


/* Parse non-terminal symbol <term prime>:
 * --------------------------------------
 * <term prime>		-->	  <mulop> <factor> <term prime> 
 * 				| empty
 *
 * */
bool Parser::parseTermPrime(eTree **t)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if (parseMulop(t)) {
		/* <term prime> --> <mulop> <factor> <term prime> */
		if (parseFactor(&((*t)->right)))
			return parseTermPrime(t);
		else
			return false;
	} else {
		/* <term prime> --> empty */ 
		return true;
	}
}


/* Parse non-terminal symbol <mulop>:
 * ---------------------------------
 * <mulop> 	--> 	  star_sign 
 * 			| forward_slash
 *
 * */
bool Parser::parseMulop(eTree **t)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if ((token.Text == "*") || (token.Text == "/")) {
		/* Create a node with value  * or /; make this node as root and t as its left child */
		eTree *n = new eTree();
		/* Populate this node */
		n->type = OPERATION_NODE;
		n->value = token.Text;
		n->left = *t; /* make t as its left child */
		n->right = NULL;
		*t = n; /* make n as root */
		token = s->GetParsingToken();
		return true;
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* Parse non-terminal symbol <factor>:
 * ----------------------------------
 *  <factor> 	--> 	  ID <factor prime>
 *  			| NUMBER 
 *  			| minus_sign NUMBER 
 *  			| left_parenthesis <expression> right_parenthesis
 *
 * */
bool Parser::parseFactor(eTree **t)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();

	if (token.Type == TOKEN_IDENTIFIER) {
	/* <factor> --> ID <factpr prime> */
		string id = token.Text;
		token = s->GetParsingToken();
		return parseFactorPrime(t, id);
	} else if (token.Type == TOKEN_NUMBER) {
		/* Populate a node in AST */
		if (!(*t)) { /* make this a node */
			eTree *n = new eTree();
			/* Populate this node */
			n->value = token.Text;
			n->type = NUMBER_NODE;
			n->left = NULL;
			n->right = NULL;
			*t = n;
		}
		/* <factor> --> NUMBER */
		token = s->GetParsingToken();
		return true;
	} else if (token.Text == "-") {
		/* <factor> --> minu_sign NUMBER */
		token = s->GetParsingToken();
		if (token.Type == TOKEN_NUMBER) {
			/* Populate a node in AST */
			if (!*(t)) { /* make this a node */
				eTree *n = new eTree();
				/* Value of a negative number */
				n->value = "-" + token.Text;
				n->type = NUMBER_NODE;
				n->left = NULL;
				n->right = NULL;
				*t = n;
			}
			token = s->GetParsingToken();
			return true;
		} else {
			/* Invalid syntax */
			return false;
		}
	} else if (token.Text == "(") {
	/* <factor> --> left_parenthesis <expression> right_parenthesis */
		token = s->GetParsingToken();
		string expr = "";
		if (parseExpression(expr)) {
			if (token.Text == ")") {
				/* Populate a node in AST */
				if (!(*t)) { /* make this a node */
					eTree *n = new eTree();
					n->value = expr;
					n->type = NUMBER_NODE;
					n->left = NULL;
					n->right = NULL;
					*t = n;
				}
				token = s->GetParsingToken();
				return true;
			} else {
				/* Invalid syntax */
				return false;
			}
		} else {
			/* Invalid syntax */
			return false;
		}
	} else {
		/* Invalid syntax */
		return false;
	}
}


/* Parse non-terminal symbol <factor prime>:
 * ----------------------------------------
 * <factor prime>	-->	  left_bracket <expression> right_bracket
 * 				| left_parenthesis <expr list> right_parenthesis
 * 				| empty
 *
 * */
bool Parser::parseFactorPrime(eTree **t, string id)
{
	/* Return in case of token error */
	CHECK_FOR_TOKEN_ERROR();
	if (token.Text == "[") {
	/* <factor prime> --> left_bracket <expression> right_bracket */
		string expr = "";
		token = s->GetParsingToken();
		if (parseExpression(expr)) {
			if (token.Text == "]") {
				/* Populate a node in AST */
				if (!(*t)) { /* make this a node */
					/* expr is not a number */
					string str;
					int index = c->getBaseAddress(id);
					if (expr.at(0) == 'l') {/* l for local */
						/* Get base address of array id[] */
						str = c->createLocalSymbol();
						c->appendFuncCode(str + " = " + to_string((unsigned long long)index)
									+ " + " + expr + ";\n");
					} else { /* expr is a number */
						str = to_string((unsigned long long)(stoi(expr) + index));
					}
					/* Populate this node */
					eTree *n = new eTree();
					/* If global returns true */
					if (c->getScope(id))
						n->value = "global[" + str + "]";
					else
						n->value = "mem[base+" + str + "]";
					n->type = ARRAY_NODE;
					n->left = NULL;
					n->right = NULL;
					*t = n;
				}
				token = s->GetParsingToken();
				return true;
			} else {
				/* Invalid syntax */
				return false;
			}
		} else {
			/* Invalid syntax */
			return false;
		}
	} else if (token.Text == "(") {
		c->saveFuncCallName(id);
	/* <factor prime> --> left_parenthesis <expr list> right_parenthesis */
		token = s->GetParsingToken();
		string exprList = id + "(";
		if (parseExprList(exprList)) {
			if (token.Text == ")") {
				exprList += ")";
				/* Populate a node in AST */
				if (!(*t)) { /* make this a node */
					eTree *n = new eTree();
					n->value = exprList;
					n->type = FUNCTION_NODE; /* function */
					n->left = NULL;
					n->right = NULL;
					*t = n;
				}
				token = s->GetParsingToken();
				return true;
			} else {
				/* Invalid syntax */
				return false;
			}
		} else {
			/* Invalid syntax */
			return false;
		}
	} else {
		/* <factor prime> --> empty */
		/* Populate a node in AST */
		if (!(*t)) { /* make this a node */
			eTree *n = new eTree();
			/* Populate this node */
			n->value = id;
			n->type = IDENTIFIER_NODE; /* variable */
			n->left = NULL;
			n->right = NULL;
			*t = n;
		}
		return true;
	}
}


