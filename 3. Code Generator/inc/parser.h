/************************************************************************************************************************************
 *
 *  File Name	: parser.h
 *
 *  Description	: Header file for parser.cc
 *
 **************************************************************************************************************************************/

#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include <fstream>
#include <iostream>
#include <scanner.h>
#include <codegen.h>

using namespace std;

class Parser {
	public:
		/* Pointer to scanner object which is defined in main() */
		Scanner *s;
		/* code gen object */
		CodeGen *c;
		/* Token*/
		Token token;
		/* These below commented variables were useful 
		 * in parser, not anymore in code gen */
		/* Variable count in the program */
		//int NumberOfVariables;
		/* Function count in the program */
		//int NumberOfFunctions;
		/* Statement count in the program */
		//int NumberOfStatements;
		/* Constructor */
		Parser(Scanner& scanner, CodeGen& c);
		/* Parse non-terminal symbol <program> */
		bool parseProgram(void);
	protected:
		/* Parse non-terminal symbol <func list> */
		bool parseFuncList(void);
		/* Parse non-terminal symbol <func> */
		bool parseFunc(void);
		/* Parse non-terminal symbol <func prime> */
		bool parseFuncPrime(void);
		/* Parse non-terminal symbol <func decl> */
		bool parseFuncDecl(void);
		/* Parse non-terminal symbol <type name> */
		bool parseTypeName(void);
		/* Parse non-terminal symbol <parameter list> */
		bool parseParameterList(void);
		/* Parse non-terminal symbol <parameter list prime> */
		bool parseParameterListPrime(void);
		/* Parse non-terminal symbol <non-empty list> */
		bool parseNonEmptyList(void);
		/* Parse non-terminal symbol <non-empty list prime> */
		bool parseNonEmptyListPrime(void);
		/* Parse non-terminal symbol <data decls> */
		bool parseDataDecls(void);
		/* Parse non-terminal symbol <id list> */
		bool parseIdList(string id);
		/* Parse non-terminal symbol <id list prime> */
		bool parseIdListPrime(void);
		/* Parse non-terminal symbol <id> */
		bool parseId(string id);
		/* Parse non-terminal symbol <id prime> */
		bool parseIdPrime(string id);
		/* Parse non-terminal symbol <block statements> */
		bool parseBlockStatements(void);
		/* Parse non-terminal symbol <statements> */
		bool parseStatements(void);
		/* Parse non-terminal symbol <statement> */
		bool parseStatement(void);
		/* Parse non-terminal symbol <statement prime> */
		bool parseStatementPrime(string id);
		/* Parse non-terminal symbol <statement double prime> */
		bool parseStatementDoublePrime(void);
		/* Parse non-terminal symbol <assigment> */
		bool parseAssignment(void);
		/* Parse non-terminal symbol <assignment prime> */
		bool parseAssignmentPrime(void);
		/* Parse non-terminal symbol <func call> */
		bool parseFuncCall(void);
		/* Parse non-terminal symbol <expr list> */
		bool parseExprList(string& exprList);
		/* Parse non-terminal symbol <non-empty expr list> */
		bool parseNonEmptyExprList(string& exprList);
		/* Parse non-terminal symbol <non-empty expr list prime> */
		bool parseNonEmptyExprListPrime(string& exprList);
		/* Parse non-terminal symbol <is statement> */
		bool parseIfStatement(void);
		/* Parse non-terminal symbol <condition expression> */
		bool parseConditionExpression(string& str);
		/* Parse non-terminal symbol <condition expression prime> */
		bool parseConditionExpressionPrime(eTree **t);
		/* Parse non-terminal symbol <condition op> */
		bool parseConditionOp(eTree **t);
		/* Parse non-terminal symbol <condition> */
		bool parseCondition(eTree **t);
		/* Parse non-terminal symbol <comparison op> */
		bool parseComparisonOp(eTree **t);
		/* Parse non-terminal symbol <while statement> */
		bool parseWhileStatement(void);
		/* Parse non-terminal symbol <return statement> */
		bool parseReturnStatement(void);
		/* Parse non-terminal symbol <return statement prime> */
		bool parseReturnStatementPrime(void);
		/* Parse non-terminal symbol <break statement> */
		bool parseBreakStatement(void);
		/* Parse non-terminal symbol <continue statement> */
		bool parseContinueStatement(void);
		/* Parse non-terminal symbol <expression> */
		bool parseExpression(string& expr);
		/* Parse non-terminal symbol <expression prime> */
		bool parseExpressionPrime(eTree **t);
		/* Parse non-terminal symbol <addop> */
		bool parseAddop(eTree **t);
		/* Parse non-terminal symbol <term> */
		bool parseTerm(eTree **t);
		/* Parse non-terminal symbol <term prime> */
		bool parseTermPrime(eTree **t);
		/* Parse non-terminal symbol <mulop> */
		bool parseMulop(eTree **t);
		/* Parse non-terminal symbol <factor> */
		bool parseFactor(eTree **t);
		/* Parse non-terminal symbol <factor prime> */
		bool parseFactorPrime(eTree **t, string id);
};

#endif

