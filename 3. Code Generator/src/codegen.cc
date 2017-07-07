/***********************************************************************************************
 *
 *  File Name	: codegen.cc
 *
 *  Description	: Modify the parser to generate assembly-like code for each function in an input 
 *  		  program.  The specific requirements are as follows.
 *
 *  		  1) As in previous projects, your parser should ignore (but copy) 
 *  		     "meta-statements" to the output program.
 *  		  2) The output program uses the same grammar as the input program with the 
 *  		     following modifications.  (This is a precise grammar defintion.  But you do 
 *  		     not need to worry about parsing this grammar since you are not going to 
 *  		     compile the output program.)
 *  		  3) Grammar modification for data declaration:
 *  		     - No data declaration is allowed except one array at program level and one 
 *  		       array inside each function.
 *  		  4) Grammar modification for assignment and expression:
 *  		     - <assignment> -->   <id> equal_sign <operand> <operation> <operand> semicolon 
 *  		     			| <id>equal_sign <operand> semicolon 
 *  		     			| <id>equal_sign<func call>semicolon
 *  		     - <operand>    -->   NUMBER 
 *  		     			| minus_sign NUMBER 
 *  		     			| <id>
 *  		     - <operation>  -->   <addop> 
 *  		     			| <mulop> 
 *  		     			| <comparison op> 
 *  		     			| <condition op>
 *  		     - Change all mentioning of <expression> with <operand>, as in <id>, read/write 
 *  		       statement, <func call>, <return>, and <condition>.
 *  		     - Now <id>s are array references.
 *  		   5) Grammar modification for control flow:
 *  		     - <if-statement>	 --> if left_parenthesis <operand> <comparison op> <operand> 
 *  		     			     right_parenthesis goto LABEL semicolon
 *  		     - <goto-statement>  --> goto LABEL semicolon
 *  		     - <label-statement> --> LABEL colon semicolon
 *  		     - LABEL is just another name for ID token
 *  		     - Do not use the previous definition of <if-statement>.  Do not use 
 *  		       <while-statement>, <break>, and <continue>.
 *  		   6) Grammar modification for functions:
 *  		     - Function declaration and definition remain unchanged.
 *  		     - In <func call>, each parameter must be <id> instead of <experssion>.
 *  		     - In <return>, use <id> instead of <expression>.
 *  		   7) The output program should be compilable by a C compiler and have the same 
 *  		      behavior as the input program.
 *
 ***********************************************************************************************/

#include <string>
#include <iostream>
#include <scanner.h>
#include <codegen.h>

using namespace std;

/* @func: CodeGen() - constructor
 * @args: void
 * @ret : no return for constructor
 * @desc: Initializes all the variables and the symbol table
 */
CodeGen::CodeGen()
{
	/* Initlialize the variables */
	globalArraySize = 0;
	localArraySize = 0;
	/* Current index in the symbol table */
	globalCurrIndex = 0;
	localCurrIndex = 0;
	/* variable type */
	globalVarType = "";
	localVarType = "";
	/* variable declaration */
	varCode = "";
	/* Function prototype */
	funcCode = "";
	/* labels */
	nextLabel = 0;
	breakLabel = 0;
	contLabel = 0;
	/* Global or local variable declaration */
	global = true;

	funcIndex = 0;
	insideFunc = 0;

	fCallIndex = 0;
	fParamIndex = 0;

	special = 0;
}

/* @func: ~CodeGen() - destructor
 * @args: 
 * @ret : no return for destructor
 * @desc: Closes the opened input and output files
 */
CodeGen::~CodeGen()
{
}
void CodeGen::saveFuncName(string str, int paraIndex, string paraArray[50])
{
	funcIndex += 1;
	fCallIndex = 0;
	funcStore[funcIndex].funcName = str;

	for (int i = 1; i <= paraIndex; i++)
	{
		funcStore[funcIndex].paramArray[i] = paraArray[i];
	}
	funcStore[funcIndex].paraIndex = paraIndex;

}
void CodeGen::appendFuncBody(string str)
{
	funcStore[funcIndex].body += str;
}
void CodeGen::saveFuncVarNum(void)
{
	funcStore[funcIndex].varNum = localArraySize;
}


void CodeGen::saveFuncCallName(string str)
{
	fCallIndex += 1;
	fParamIndex = 0;
	funcStore[funcIndex].fCallName[fCallIndex] = str;
	funcStore[funcIndex].fCallCount = fCallIndex;
}

int CodeGen::getFuncCallCount(void)
{
	return funcStore[funcIndex].fCallCount;
}

void CodeGen::saveFuncCallParam(string str)
{

	fParamIndex += 1;
	funcStore[funcIndex].fCallParam[fCallIndex][fParamIndex] = str;
	funcStore[funcIndex].fParamCount[fCallIndex] = fParamIndex;
}


/* Generate var code */
void CodeGen::genVarCode(void)
{
	if (global) { /* global variable declaration */
		if (globalArraySize) {
			if (globalVarType == "")
				varCode = "\nint global[" + to_string((long long int)globalArraySize) + "];\n"; 
			else
				varCode = "\n" + globalVarType + " global[" + to_string((long long int)globalArraySize) + "];\n"; 
		}
	} else {/* local variable declaration */
		if (localArraySize) {
			if (localVarType == "")
				varCode = "int local[" + to_string((long long int)localArraySize) + "];\n";
			else
				varCode = localVarType + " local[" + to_string((long long int)localArraySize) + "];\n";
		}
	}

	/* Global variable declared only once */
	if (global)
		global = false;
}

/* Save symbol information */
void CodeGen::saveSymbol(string id, int size)
{
	if (global) { /* save global variables */
		/* Save symbol */
		globalSymbol[globalCurrIndex].name = id;
		/* save start index */
		globalSymbol[globalCurrIndex].start_idx = globalArraySize;
		/* Increment global variable count */
		globalArraySize += size;
		/* Update current index in global symbol table */
		globalCurrIndex++;
	} else { /* save local variables */
		/* Save symbol */
		localSymbol[localCurrIndex].name = id;
		/* save start index */
		localSymbol[localCurrIndex].start_idx = localArraySize;
		/* Increment local variable count */
		localArraySize += size;
		/* Update current index in local symbol table */
		localCurrIndex++;
	}
}

/* Create local temporary symbol */
string CodeGen::createLocalSymbol(void)
{
	string str;
	/* No entry in local symbol table just increament the variable count */
	str = "mem[base+" + to_string((unsigned long long)(localArraySize)) + "]";
	localArraySize++;
	return str;
}

/* get Symbol variable to replace with */
string CodeGen::getSymbol(string id, int index)
{
	/* search in global symbol table */
	for (int i = 0; i < globalCurrIndex; i++) {
		if (!((globalSymbol[i].name).compare(id)))
			return "global[" + to_string((long long int)(globalSymbol[i].start_idx + index)) + "]";
	}

	/* search in local symbol table */
	for (int i = 0; i < localCurrIndex; i++) {
		if (!((localSymbol[i].name).compare(id)))
			return "mem[base+" + to_string((long long int)(localSymbol[i].start_idx + index)) + "]";
	}

	/* Return empty string when not found */
	return "";
}

/* Get base adress of this array symbol */
int CodeGen::getBaseAddress(string id)
{
	/* search for start index to get base address in global symbol table */
	for (int i = 0; i < globalCurrIndex; i++) {
		if (!((globalSymbol[i].name).compare(id)))
			return globalSymbol[i].start_idx;
	}

	/* search for start index to get base address in local symbol table */
	for (int i = 0; i < localCurrIndex; i++) {
		if (!((localSymbol[i].name).compare(id)))
			return localSymbol[i].start_idx;
	}

	/* Return empty string when not found */
	return 0;

}

/* Get scope:
 * if the scope of the variable is global, then return true, else false */
bool CodeGen::getScope(string id)
{
	/* search in global symbol table */
	for (int i = 0; i < globalCurrIndex; i++) {
		if (!((globalSymbol[i].name).compare(id)))
			return true;
	}

	/* search in local symbol table */
	for (int i = 0; i < localCurrIndex; i++) {
		if (!((localSymbol[i].name).compare(id)))
			return false;
	}

	/* Return false when not found */
	return false;

}

/* Reset all the variable which will be reused in a new function */
void CodeGen::resetLocal(void)
{
	/* Label for break point */
	breakLabel = 0;
	/* Label for continue */
	contLabel = 0;
	/* Local variable count */
	localArraySize = 0;
	/* Local variable type */
	localVarType = "";
	/* Reset function code part in a function */
	resetFuncCode();
	/* Reset variable declaration part in a function */
	resetVarCode();
	/* Reset local symbol table */
	for (int i = 0; i < MAX_SYMBOLS; i++)
		localSymbol[i].name = "";
}

/* Parse expression tree */
void CodeGen::parseTree(eTree *t)
{
	if (!t) /* base case */
		return;

	if (t->type == IDENTIFIER_NODE) {
		string buffer;
		int flag = 0;
		string str = getSymbol(t->value, 0);
		if (!str.size()) { /* symbol not found */
			saveSymbol(t->value, 1); /* add one */
			str = getSymbol(t->value, 0); /* get new symbol */
			/* Assignment of new local variable just created */

			for (int i = 1; i <= funcStore[funcIndex].paraIndex; ++i)
			{
				if (funcStore[funcIndex].paramArray[i] == t->value) {
					buffer = "mem[base-4-"+to_string(i)+"]";
					flag = 1;
					break;
				}
			}
			if (flag == 1){
				appendFuncCode(str + " = " + buffer + ";\n");
				flag = 0;
			}
			else {
				appendFuncCode(str + " = " + t->value + ";\n");
			}
		}
		/* update node value with new symbol */
		t->value = str;
	} else if (t->type == ARRAY_NODE) {
		/* Do nothing */
	} else if (t->type == NUMBER_NODE) {
		/* Do nothing */
	} else if (t->type == OPERATION_NODE) {
		parseTree(t->left);
		parseTree(t->right);
		/* populate new node */
		string str = createLocalSymbol();
		/* Assignment of new local variable just created */
		appendFuncCode(str + " = " + t->left->value + " " + t->value + " " + t->right->value + ";\n");
		/* remove left and right nodes */
		delete t->left;
		t->left = NULL;
		delete t->right;
		t->right = NULL;
		/* update node value with new symbol */
		t->value = str;
	} else { /* FUNCTION_NODE */
		/* populate new node */
		string str = createLocalSymbol();
		/* Assignment of new local variable just created */
		appendTreeFuncCode(str + " = " + t->value + ";\n", str, t->value);
		/* update node value with new symbol */
		t->value = str;
	}
}

/* Parse condition expression tree */
void CodeGen::parseCondTree(eTree *t, eTree *root)
{
	if (!t) /* base case */
		return;

	/* Base case for condition expression */
	if ((t == root) && (t->left->type == ARRAY_NODE) && (t->right->type == ARRAY_NODE))
		return;

	if (t->type == ARRAY_NODE) {
		string str = getSymbol(t->value, 0);
		if (!str.size()) { /* symbol not found */
			saveSymbol(t->value, 1); /* add one */
			str = getSymbol(t->value, 0); /* get new symbol */
			/* Assignment of new local variable just created */
			appendFuncCode(str + " = " + t->value + ";\n");
		}
		/* update node value with new symbol */
		t->value = str;
	} else { /* t->type == OPERATION_NODE)*/
		parseCondTree(t->left, root);
		parseCondTree(t->right, root);
		/* Base case for condition expression */
		if ((t == root) && (t->left->type == ARRAY_NODE) && (t->right->type == ARRAY_NODE))
			return;
		/* populate new node */
		string str = createLocalSymbol();
		/* Assignment of new local variable just created */
		appendFuncCode(str + " = " + t->left->value + " " + t->value + " " + t->right->value + ";\n");
		/* remove left and right nodes */
		delete t->left;
		t->left = NULL;
		delete t->right;
		t->right = NULL;
		/* update node value with new symbol */
		t->type = ARRAY_NODE;
		t->value = str;
	}
}
