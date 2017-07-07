#include "scanner.h"

//token is made global so that can be used by all the functions without passing.
Token token;

//count variables are made global so that can be used by all the functions without passing.
int _num_variables = 0;
int _num_functions = 0;
int _num_statements = 0;


Token GetActualToken(Scanner &scanner);

//Declarations for respective function of all the Rules.
bool program(Scanner &scanner);
bool type_name(Scanner &scanner);
bool h(Scanner &scanner);
bool funclist(Scanner &scanner);
bool func(Scanner &scanner);
bool g(Scanner &scanner);
bool funcdecl(Scanner &scanner);
bool parameterlist(Scanner &scanner);
bool nonemptylist(Scanner &scanner);
bool nonemptylistprime(Scanner &scanner);
bool datadecls(Scanner &scanner);
bool idlist(Scanner &scanner);
bool idlistprime(Scanner &scanner);
bool id(Scanner &scanner);
bool f(Scanner &scanner);
bool blockstatements(Scanner &scanner);
bool statements(Scanner &scanner);
bool statement(Scanner &scanner);
bool j(Scanner &scanner);
bool assignment(Scanner &scanner);
bool nonemptyexprlistprime(Scanner &scanner);
bool nonemptyexprlist(Scanner &scanner);
bool exprlist(Scanner &scanner);
bool funccall(Scanner &scanner);
bool i(Scanner &scanner);
bool expression(Scanner &scanner);
bool ifstatement(Scanner &scanner);
bool whilestatement(Scanner &scanner);
bool returnstatement(Scanner &scanner);
bool breakstatement(Scanner &scanner);
bool continuestatement(Scanner &scanner);
bool condition(Scanner &scanner);
bool b(Scanner &scanner);
bool conditionop(Scanner &scanner);
bool comparisonop(Scanner &scanner);
bool e(Scanner &scanner);
bool term(Scanner &scanner);
bool addop(Scanner &scanner);
bool term(Scanner &scanner);
bool factor(Scanner &scanner);
bool mulop(Scanner &scanner);
bool termprime(Scanner &scanner);
bool a(Scanner &scanner);
bool conditionexpression(Scanner &scanner);
bool expressionprime(Scanner &scanner);
bool parse(Scanner &scanner);




//This function gets the token
Token GetActualToken(Scanner &scanner) {
	if (scanner.HasMoreTokens()) {
		token = scanner.GetNextToken();
		cout << token.Text << "~~";
		return token;
	} else {
		token.SetTokenType(TOKEN_EOF);
		token.SetTokenKind(EOF1);
		token.Text = "$";
		return token;
	}

}

//Function that initiates the parser ~ Goal
bool parse(Scanner &scanner){
	cout << "Parse!!" << endl;
	token = GetActualToken(scanner);

	if (program(scanner)) {
		return true;
	} else return false;

}


//Definition for respective function of all the Rules.

bool program(Scanner &scanner) {
	cout << "program!!" << endl;
	if (type_name(scanner)) {
		token = GetActualToken(scanner);
		if (token.GetTokenKind() == ID) {
			token = GetActualToken(scanner);
			if (h(scanner)) {
				return true;
			} else return false;

		} else return false;
	} else if (token.GetTokenKind() == EOF1) {
		return true;
	} else return false;

}


bool type_name(Scanner &scanner) {
	cout << "typename!!" << endl;	
	//if ((token.GetTokenKind() == INT) || (token.GetTokenKind() == VOID) || (token.GetTokenKind() == BINARY) || (token.GetTokenKind() == DECIMAL)) {
	//	return true;
	//} else return false;
	if (token.GetTokenKind() == INT) {
		return true;
	} else if (token.GetTokenKind() == VOID) {
		return true;
	} else if (token.GetTokenKind() == BINARY) {
		return true;
	} else if (token.GetTokenKind() == DECIMAL) {
		return true;
	} else return false;
}

bool h(Scanner &scanner) {
	cout << "h!!" << endl;

	if (f(scanner)) {
		token = GetActualToken(scanner);
		if (idlistprime(scanner)) {
			token = GetActualToken(scanner);
			if (token.GetTokenKind() == SEMICOLON) {
				token = GetActualToken(scanner);
				if (datadecls(scanner)) {
					token = GetActualToken(scanner);
					if (i(scanner)) {
						return true;
					} else return false;
				} else return false;
			} else return false;
		} else return false;
	} else if (token.GetTokenKind() == LEFT_PARENTHESIS) {
		token = GetActualToken(scanner);
		if (parameterlist(scanner)) {
			token = GetActualToken(scanner);
			if (token.GetTokenKind() == RIGHT_PARENTHESIS) {
				token = GetActualToken(scanner);
				if (g(scanner)) {
					token = GetActualToken(scanner);
					if (funclist(scanner)) {
						return true;
					} else return false;
				} else return false;
			} else return false;
		} else return false;
	} else return false;
	cout << "h - exit!!" << endl;

}

bool i(Scanner &scanner) {
	cout << "i!!" << endl;
	if (funclist(scanner)) {
		return true;
	} else if (token.GetTokenKind() == EOF1) {
		return true;
	} else return false;
}

bool funclist(Scanner &scanner) {
	cout << "funclist!!" << endl;
	if (func(scanner)) {
		token = GetActualToken(scanner);
		if (funclist(scanner)) {
			return true;
		} else return false;
	} else if (token.GetTokenKind() == EOF1) {
		return true;
	} else return false;
}

bool func(Scanner &scanner) {
	cout << "func" << endl;
	if (funcdecl(scanner)) {
		token = GetActualToken(scanner);
		if (g(scanner)) {
			return true;
		} else return false;
	} else return false;
}

bool g(Scanner &scanner) {
	cout << "g!!" << endl;
	if (token.GetTokenKind() == SEMICOLON) {
		return true;
	} else if (token.GetTokenKind() == LEFT_BRACE) {
		token = GetActualToken(scanner);
		if (datadecls(scanner)) {
			token = GetActualToken(scanner);
			if (statements(scanner)) {
				token = GetActualToken(scanner);
				if (token.GetTokenKind() == RIGHT_BRACE) {
					_num_functions++;  //add no of functions which have been defined, not only declared
					return true;
				} else return false;
			} else return false;
		} else return false;
	} else return false;
}

bool funcdecl(Scanner &scanner) {
	cout << "funcdecl!!" << endl;
	if (type_name(scanner)) {
		token = GetActualToken(scanner);
		if (token.GetTokenKind() == ID) {
			token = GetActualToken(scanner);
			if (token.GetTokenKind() == LEFT_PARENTHESIS) {
				token = GetActualToken(scanner);
				if (parameterlist(scanner)) {
					token = GetActualToken(scanner);
					if (token.GetTokenKind() == RIGHT_PARENTHESIS) {
						return true;
					} else return false;
				} else return false;
			} else return false;
		} else return false;
	} else return false;
}

bool parameterlist(Scanner &scanner) {
	cout << "parametrlist!!" << endl;
	if (token.GetTokenKind() == VOID) {
		return true;
	} else if (nonemptylist(scanner)) {
		return true;
	} else if (token.GetTokenKind() == RIGHT_PARENTHESIS) {
		return true;
	} else return false;
}

bool nonemptylist(Scanner &scanner) {
	cout << "nonemptylist!!" << endl;
	if (type_name(scanner)) {
		token = GetActualToken(scanner);
		if (token.GetTokenKind() == ID) {
			token = GetActualToken(scanner);
			if (nonemptylistprime(scanner)) {
				return true;
			} else return false;
		} else return false;
	} else return false;
}

bool nonemptylistprime(Scanner &scanner) {
	cout << "nonemptylistprime!!" << endl;
	if (token.GetTokenKind() == COMMA) {
		token = GetActualToken(scanner);
		if (type_name(scanner)) {
			token = GetActualToken(scanner);
			if (token.GetTokenKind() == ID) {
				token = GetActualToken(scanner);
				if (nonemptylistprime(scanner)) {
					return true;
				} else return false;
			} else return false;
		} else return false; 
	} else if (token.GetTokenKind() == RIGHT_PARENTHESIS) {
		return true;
	} else return false;
}

bool datadecls(Scanner &scanner) {
		cout << "datadecls!!" << endl;
	if (type_name(scanner)) {
		token = GetActualToken(scanner);
		if (idlist(scanner)) {
			token = GetActualToken(scanner);
			if (token.GetTokenKind() == SEMICOLON) {
				token = GetActualToken(scanner);
				if (datadecls(scanner)){
					return true;
				} else return false;
			} else return false;
		} else return false;
	} else if (token.GetTokenKind() ==  INT || token.GetTokenKind() ==  VOID || token.GetTokenKind() ==  BINARY || token.GetTokenKind() ==  DECIMAL || token.GetTokenKind() ==  EOF1 || token.GetTokenKind() ==  ID || token.GetTokenKind() ==  IF || token.GetTokenKind() ==  WHILE || token.GetTokenKind() ==  RETURN || token.GetTokenKind() ==  BREAK || token.GetTokenKind() ==  CONTINUE || token.GetTokenKind() ==  READ || token.GetTokenKind() ==  WRITE || token.GetTokenKind() ==  PRINT || token.GetTokenKind() == RIGHT_BRACE) {
		return true;
	} else return false;
}

bool idlist(Scanner &scanner) {
		cout << "idlist!!" << endl;

	if (id(scanner)) {
		token = GetActualToken(scanner);
		if (idlistprime(scanner)){
			return true;
		} else return false;
	} else return false;
}

bool idlistprime(Scanner &scanner) {
	cout << "idlistprime!!" << endl;
	if (token.GetTokenKind() == COMMA) {
		token = GetActualToken(scanner);
		if (token.GetTokenKind() == ID) {
			token = GetActualToken(scanner);
			return idlistprime(scanner);
		} else return false;
	} else if (token.GetTokenKind() == SEMICOLON) {
		return true;
	} else return false;
}

bool id(Scanner &scanner) {
	cout << "id!!" << endl;
	if (token.GetTokenKind() == ID) {
		token = GetActualToken(scanner);
		if (f(scanner)) {
			_num_variables++; //increase count of variable declarations.
			return true;
		} else return false;
	} else return false;
}

bool f(Scanner &scanner) {
	cout << "f!!" << endl;
	if (token.GetTokenKind() == LEFT_BRACKET) {
		token = GetActualToken(scanner);
		if (expression(scanner)) {
			token = GetActualToken(scanner);
			if (token.GetTokenKind() == RIGHT_BRACKET) {
				return true;
			} else return false;
		} else return false;
	} else if (token.GetTokenKind() == COMMA || token.GetTokenKind() == SEMICOLON || token.GetTokenKind() == EQUAL_SIGN) {
		return true;
	} else return false;
}

bool blockstatements(Scanner &scanner) {
	cout << "block!!" << endl;
	if (token.GetTokenKind() == LEFT_BRACE) {
		token = GetActualToken(scanner);
		if (statements(scanner)) {
			token = GetActualToken(scanner);
			if (token.GetTokenKind() == RIGHT_BRACE) {
				return true;
			} else return false;
		} else return false;
	} else return false;
}

bool statements(Scanner &scanner) {
	cout << "statements!!" << endl;
	if (statement(scanner)) {
		token = GetActualToken(scanner);
		if (statements(scanner)) {
			return true;
		} else return false;
	} else if (token.GetTokenKind() == RIGHT_BRACE) {
		return true;
	} else return false;
}

bool statement(Scanner &scanner) {
	cout << "statement!!" << endl;
	if (token.GetTokenKind() == ID) {
		token = GetActualToken(scanner);
		if (j(scanner)) {
			_num_statements++;		//increase count of statements
			return true;
		} else return false;
	} else if (ifstatement(scanner)) {
		_num_statements++;		//increase count of statements
		return true;
	} else if (whilestatement(scanner)) {
		_num_statements++;		//increase count of statements
		return true;
	} else if (returnstatement(scanner)) {
		_num_statements++;		//increase count of statements
		return true;
	} else if (breakstatement(scanner)) {
		_num_statements++;		//increase count of statements
		return true;
	} else if (continuestatement(scanner)) {
		_num_statements++;		//increase count of statements
		return true;
	} else if (token.GetTokenKind() == READ) {
		token = GetActualToken(scanner);
		if (token.GetTokenKind() == LEFT_PARENTHESIS) {
			token = GetActualToken(scanner);
			if (token.GetTokenKind() == ID) {
				token = GetActualToken(scanner);
				if (token.GetTokenKind() == RIGHT_PARENTHESIS) {
					token = GetActualToken(scanner);
					if (token.GetTokenKind() == SEMICOLON) {
						_num_statements++;		//increase count of statements
						return true;
					} else return false;
				} else return false;
			} else return false;
		} else return false;
	} else if (token.GetTokenKind() == WRITE) {
		token = GetActualToken(scanner);
		if (token.GetTokenKind() == LEFT_PARENTHESIS) {
			token = GetActualToken(scanner);
			if (expression(scanner)) {
				token = GetActualToken(scanner);
				if (token.GetTokenKind() == RIGHT_PARENTHESIS) {
					token = GetActualToken(scanner);
					if (token.GetTokenKind() == SEMICOLON) {
						_num_statements++;		//increase count of statements
						return true;
					} else return false;
				} else return false;
			} else return false;
		} else return false;
	} else if (token.GetTokenKind() == PRINT) {
		token = GetActualToken(scanner);
		if (token.GetTokenKind() == LEFT_PARENTHESIS) {
			token = GetActualToken(scanner);
			if (token.GetTokenKind() == STRING) {
				token = GetActualToken(scanner);
				if (token.GetTokenKind() == RIGHT_PARENTHESIS) {
					token = GetActualToken(scanner);
					if (token.GetTokenKind() == SEMICOLON) {
						_num_statements++;		//increase count of statements
						return true;
					} else return false;
				} else return false;
			} else return false;
		} else return false;
	} else return false;
}


bool j(Scanner &scanner) {
	cout << "j!!" << endl;
	if (f(scanner)) {
		token = GetActualToken(scanner);
		if (token.GetTokenKind() == EQUAL_SIGN) {
			token = GetActualToken(scanner);
			if (expression(scanner)) {
				token = GetActualToken(scanner);
				if (token.GetTokenKind() == SEMICOLON) {
					return true;
				} else return false;
			} else return false;
		} else return false;
	} else if (token.GetTokenKind() == LEFT_PARENTHESIS) {
		token = GetActualToken(scanner);
		if (exprlist(scanner)) {
			token = GetActualToken(scanner);
			if (token.GetTokenKind() == RIGHT_PARENTHESIS) {
				token = GetActualToken(scanner);
				if (token.GetTokenKind() == SEMICOLON) {
					return true;
				} else return false;
			} else return false;
		} else return false;
	} else return false;
}


bool assignment(Scanner &scanner) {
	cout << "assignment!!" << endl;
	if (id(scanner)) {
		token = GetActualToken(scanner);
		if (token.GetTokenKind() == EQUAL_SIGN) {
			token = GetActualToken(scanner);
			if (expression(scanner)) {
				token = GetActualToken(scanner);
				if (token.GetTokenKind() == SEMICOLON) {
					return true;
				} else return false;
			} else return false;
		} else return false;
	} else return false;
}

bool funccall(Scanner &scanner) {
	cout << "funccall!!" << endl;
	if (token.GetTokenKind() == ID) {
		token = GetActualToken(scanner);
		if (token.GetTokenKind() == LEFT_PARENTHESIS) {
			token = GetActualToken(scanner);
			if (exprlist(scanner)) {
				token = GetActualToken(scanner);
				if (token.GetTokenKind() == RIGHT_PARENTHESIS) {
					token = GetActualToken(scanner);
					if (token.GetTokenKind() == SEMICOLON) {
						return true;
					} else return false;
				} else return false;
			} else return false;
		} else return false;
	} else return false;
}

bool exprlist(Scanner &scanner) {
	cout << "exprlist!!" << endl;
	if (nonemptyexprlist(scanner)) {
		return true;
	} else if (token.GetTokenKind() == RIGHT_PARENTHESIS) {
		return true;
	} else return false;
}

bool nonemptyexprlist(Scanner &scanner) {
	cout << "nonemptylistprime!!" << endl;
	if (expression(scanner)) {
		token = GetActualToken(scanner);
		if (nonemptyexprlistprime(scanner)) {
			return true;
		} else return false;
	} else return false;
}

bool nonemptyexprlistprime(Scanner &scanner) {
	cout << "nonemptyexprlistprime!!" << endl;
	if (token.GetTokenKind() == COMMA) {
		token = GetActualToken(scanner);
		if (expression(scanner)) {
			token = GetActualToken(scanner);
			if (nonemptyexprlistprime(scanner)) {
				return true;
			} else return false;
		} else return false;
	} else if (token.GetTokenKind() == RIGHT_PARENTHESIS) {
		return true;
	} else return false;
}

bool ifstatement(Scanner &scanner) {
	cout << "if!!" << endl;
	if (token.GetTokenKind() == IF) {
		token = GetActualToken(scanner);
		if (token.GetTokenKind() == LEFT_PARENTHESIS) {
			token = GetActualToken(scanner);
			if (conditionexpression(scanner)) {
				token = GetActualToken(scanner);
				if (token.GetTokenKind() == RIGHT_PARENTHESIS) {
					token = GetActualToken(scanner);
					if (blockstatements(scanner)) {
						return true;
					} else return false;
				} else return false;
			} else return false;
		} else return false;
	} else return false;
}

bool conditionexpression(Scanner &scanner) {
	cout << "conditionexpression!!" << endl;
	if (condition(scanner)) {
		token = GetActualToken(scanner);
		if (b(scanner)) {
			return true;
		} else return false;
	} else return false;
}

bool b(Scanner &scanner) {
	cout << "b!!" << endl;
	if (conditionop(scanner)) {
		token = GetActualToken(scanner);
		if (condition(scanner)) {
			return true;
		} else return false;
	} else if (token.GetTokenKind() == RIGHT_PARENTHESIS) {
		return true;
	} else return false;
}

bool conditionop(Scanner &scanner) {
	cout << "conditionop!!" << endl;
	if (token.GetTokenKind() == DOUBLE_AND) {
		return true;
	} else if (token.GetTokenKind() == DOUBLE_OR) {
		return true;
	} else return false;
}

bool condition(Scanner &scanner) {
	cout << "condition!!" << endl;
	if (expression(scanner)) {
		token = GetActualToken(scanner);
		if (comparisonop(scanner)) {
			token = GetActualToken(scanner);
			if (expression(scanner)) {
				return true;
			} else return false;
		} else return false;
	} else return false;
}

bool comparisonop(Scanner &scanner) {
	//	DOUBLE_EQUAL, NOT_EQUAL, GREATER, GREATER_EQUAL, LESS, LESS_EQUAL,
cout << "comparisonop!!" << endl;
	if (token.GetTokenKind() == DOUBLE_EQUAL) {
		return true;
	} else if (token.GetTokenKind() == NOT_EQUAL) {
		return true;
	} else if (token.GetTokenKind() == GREATER) {
		return true;
	} else if (token.GetTokenKind() == GREATER_EQUAL) {
		return true;
	} else if (token.GetTokenKind() == LESS) {
		return true;
	} else if (token.GetTokenKind() == LESS_EQUAL) {
		return true;
	} else return false;
}

bool whilestatement(Scanner &scanner) {
	cout << "while!!" << endl;
	if (token.GetTokenKind() == WHILE) {
		token = GetActualToken(scanner);
		if (token.GetTokenKind() == LEFT_PARENTHESIS) {
			token = GetActualToken(scanner);
			if (conditionexpression(scanner)) {
				token = GetActualToken(scanner);
				if (token.GetTokenKind() == RIGHT_PARENTHESIS) {
					token = GetActualToken(scanner);
					if (blockstatements(scanner)) {
						return true;
					} else return false;
				} else return false;
			} else return false;
		} else return false;
	} else return false;
}

bool returnstatement(Scanner &scanner) {
	cout << "return!!" << endl;
	if (token.GetTokenKind() == RETURN) {
		token = GetActualToken(scanner);
		if (e(scanner)) {
			return true;
		} else return false;
	} else return false;
}

bool e(Scanner &scanner) {
	cout << "e!!" << endl;
	if (expression(scanner)) {
		token = GetActualToken(scanner);
		if (token.GetTokenKind() == SEMICOLON) {
			return true;
		} else return false;
	} else if (token.GetTokenKind() == SEMICOLON) {
		return true;
	} else return false;
}

bool breakstatement(Scanner &scanner) {
	cout << "break!!" << endl;
	if (token.GetTokenKind() == BREAK) {
		token = GetActualToken(scanner);
		if (token.GetTokenKind() == SEMICOLON) {
			return true;
		} else return false;
	} else return false;
}

bool continuestatement(Scanner &scanner) {
	cout << "continue!!" << endl;
	if (token.GetTokenKind() == CONTINUE) {
		token = GetActualToken(scanner);
		if (token.GetTokenKind() == SEMICOLON) {
			return true;
		} else return false;
	} else return false;
}

bool expression(Scanner &scanner) {
	cout << "expression!!" << endl;
	if (term(scanner)) {
		token = GetActualToken(scanner);
		if (expressionprime(scanner)) {
			return true;
		} else return false;
	} else return false;
}

bool expressionprime(Scanner &scanner) {
	cout << "expressionprime!!" << endl;
	if (addop(scanner)) {
		token = GetActualToken(scanner);
		if (term(scanner)) {
			token = GetActualToken(scanner);
			if (expressionprime(scanner)) {
				return true;
			} else return false;
		} else return false;
	} else if (token.GetTokenKind() == COMMA || token.GetTokenKind() == SEMICOLON || token.GetTokenKind() == RIGHT_PARENTHESIS  || token.GetTokenKind() == RIGHT_BRACKET  || token.GetTokenKind() == DOUBLE_EQUAL  || token.GetTokenKind() == NOT_EQUAL  || token.GetTokenKind() == GREATER  || token.GetTokenKind() == GREATER_EQUAL  || token.GetTokenKind() == LESS  || token.GetTokenKind() == LESS_EQUAL  || token.GetTokenKind() == DOUBLE_AND  || token.GetTokenKind() == DOUBLE_OR ) {
		return true;
	} else return false;
}

bool addop(Scanner &scanner) {
	cout << "addop!!" << endl;
	if (token.GetTokenKind() == PLUS) {
		return true;
	} else if (token.GetTokenKind() == MINUS) {
		return true;
	} else return false;
}

bool term(Scanner &scanner) {
	cout << "term!!" << endl;
	if (factor(scanner)) {
		token = GetActualToken(scanner);
		if (termprime(scanner)) {
			return true;
		} else return false;
	} else return false;
}

bool termprime(Scanner &scanner) {
	cout << "termprime!!" << endl;
	if (mulop(scanner)) {
		token = GetActualToken(scanner);
		if (factor(scanner)) {
			token = GetActualToken(scanner);
			if (termprime(scanner)) {
				return true;
			} else return false;
		} else return false;
	} else if (token.GetTokenKind() == COMMA || token.GetTokenKind() == SEMICOLON || token.GetTokenKind() == RIGHT_PARENTHESIS  || token.GetTokenKind() == RIGHT_BRACKET  || token.GetTokenKind() == DOUBLE_EQUAL  || token.GetTokenKind() == NOT_EQUAL  || token.GetTokenKind() == GREATER  || token.GetTokenKind() == GREATER_EQUAL  || token.GetTokenKind() == LESS  || token.GetTokenKind() == LESS_EQUAL  || token.GetTokenKind() == PLUS  || token.GetTokenKind() == MINUS || token.GetTokenKind() == DOUBLE_AND  || token.GetTokenKind() == DOUBLE_OR ) {
		return true;
	} else return false;
}

bool mulop(Scanner &scanner) {
	cout << "mulop!!" << endl;
	if (token.GetTokenKind() == STAR) {
		return true;
	} else if (token.GetTokenKind() == SLASH) {
		return true;
	} else return false;
}

bool factor(Scanner &scanner) {
	cout << "factor!!" << endl;
	if (token.GetTokenKind() == ID) {
			token = GetActualToken(scanner);
			if (a(scanner)) {
				return true;
			} else	return false;
	} else if (token.GetTokenKind() == NUMBER) {
		return true;
	} else if (token.GetTokenKind() == MINUS) {
		token = GetActualToken(scanner);
		if (token.GetTokenKind() == NUMBER) {
			return true;
		} else return false;
	} else if (token.GetTokenKind() == LEFT_PARENTHESIS) {
		token = GetActualToken(scanner);
		if (expression(scanner)) {
			token = GetActualToken(scanner);
			if (token.GetTokenKind() == RIGHT_PARENTHESIS) {
				return true;
			} else	return false;
		} else return false;
	} else return false;
}

bool a(Scanner &scanner){
	cout << "a!!" << endl;
	if (token.GetTokenKind() == LEFT_BRACKET) {
		token = GetActualToken(scanner);
		if (expression(scanner)) {
			token = GetActualToken(scanner);
			if (token.GetTokenKind() == RIGHT_BRACKET) {
				return true;
			} else	return false;
		} else return false;
	} else if (token.GetTokenKind() == LEFT_PARENTHESIS) {
		token = GetActualToken(scanner);
		if (exprlist(scanner)) {
			token = GetActualToken(scanner);
			if (token.GetTokenKind() == RIGHT_PARENTHESIS) {
				return true;
			} else	return false;
		} else return false;
	} else if (token.GetTokenKind() == COMMA || token.GetTokenKind() == SEMICOLON || token.GetTokenKind() == RIGHT_PARENTHESIS  || token.GetTokenKind() == RIGHT_BRACKET  || token.GetTokenKind() == DOUBLE_EQUAL  || token.GetTokenKind() == NOT_EQUAL  || token.GetTokenKind() == GREATER  || token.GetTokenKind() == GREATER_EQUAL  || token.GetTokenKind() == LESS  || token.GetTokenKind() == LESS_EQUAL  || token.GetTokenKind() == PLUS  || token.GetTokenKind() == MINUS || token.GetTokenKind() == STAR  || token.GetTokenKind() == SLASH || token.GetTokenKind() == DOUBLE_AND  || token.GetTokenKind() == DOUBLE_OR ) {
		return true;
	} else	return false;
}

int main(int argc, char **argv)
{
	//Token token;
	if (argc != 2) {
		cerr << "Invalid number of inputs." << endl;
		cerr << "Use below syntax:" << endl;
		cerr << "./" << argv[0] << " <file-name>" << endl;
		return -1;
	}
	int scan_flag = 1;
	int parse_flag = 1;

	Scanner scanner(argv[1]);

	/* This is the scanner part: Check for all tokens in the input file */
	while (scanner.HasMoreTokens()) {
		token = scanner.GetNextToken();
		switch(token.GetTokenType()) {
			case TOKEN_NUMBER:
			case TOKEN_RESERVED_WORD:
			case TOKEN_SYMBOL:
			case TOKEN_STRING:
			case TOKEN_SPACE_NEWLINE:
			case TOKEN_IDENTIFIER:
				scanner.PrintNormal(token);
				break;
			case TOKEN_META_STATEMENT:
				scanner.PrintMeta(token);
				break;
			case TOKEN_EOF:
				scanner.PrintIdentifier(token);
				break;
			case TOKEN_ERROR:
				scan_flag = 0;
				cerr << "Input file error!!!" << endl;
				/* In case of error, only message is displayed.
				 * So remove the output file */
				scanner.DeleteOutputFile();
				return -1;
		}
	}
	cout << "Input file success!!!" << endl;
	scanner.CloseScanner();

	if (scan_flag == 1) 	/* Once Scanner succeeds start with Parser*/
	{	
		Scanner scanner1(argv[1]); //new object takes tha name of the input file and calls the parse method.
		if (parse(scanner1)) {
			cout << "Pass " << "variable " <<  _num_variables << " function " <<  _num_functions  << " statement " << _num_statements << endl;
		} else cout << "Fail" << endl;
	}
	
	return 0;
}
