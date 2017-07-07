/***********************************************************************************************
 *
 *  File Name	: codegen.h
 *
 *  Description	: Header file for codegen.cc
 *
 ***********************************************************************************************/

#ifndef __CODEGEN_H__
#define __CODEGEN_H__

#include <string>
#include <iostream>

using namespace std;

/* Stack size = 4KB.
 * This is the maximum limit for thenumber of symbols allowed in global/local scope */
#define MAX_SYMBOLS		4096

/* Different types of nodes in abstract syntax tree i.e. AST */
enum nodeTypes {
	NUMBER_NODE = 1, /* AST node is a number */
	IDENTIFIER_NODE, /* AST node is a variable */
	ARRAY_NODE,      /* AST node is an array */
	OPERATION_NODE,  /* AST node is an operator */
	FUNCTION_NODE    /* AST node is a function call */
};

/* Symbol table entry */
typedef struct symbolTableEntry {
	string name; /* name of the variable */
	int start_idx; /* Its index in local[] or global[] array */
} stEntry;

/* Function store */
typedef class functionStore {
	public:
		string funcName; /* name of the function */
		string body; /* body of the function */
		
		string bodyArr[50];
		int bodyIndex[50] = {0};
		int bodyType[50] = {0};

		int varNum = 0;

		//string funcCalls[MAX_SYMBOLS];
		//fCalls fCallStore[50];
		string fCallName[50];
		string fCallParam[50][50];

		int fCallCount=0;
		int fParamCount[50] = {0};

		string paramArray[50];
		int    paraIndex;
} fStore;

/* Class representing any node in an AST */
typedef class expTree {
	//private:
	public:
		string value; /* Value of the node */
		enum nodeTypes type; /* Node type, one of enum variables */
		class expTree *left; /* Pointer to left child */
		class expTree *right; /* Pointer to right child */
		void setValue(string v);
		string getValue(void);
		void setIndex(int index);
		int getIndex(void);
		void setLeftChild(class expTree *e);
		class expTree* getLeftChild();
		void setRightChild(class expTree *e);
		class expTree* getRightChild();
} eTree;


/* Storing modified generated code global/local */
class CodeGen {
	/* Number of global variables */
	int globalArraySize;
	/* Number of local variables */
	int localArraySize;
	/* Current index in the global symbol table */
	int globalCurrIndex;
	/* Current index in the local symbol table */
	int localCurrIndex;
	/* variable type */
	string globalVarType;
	string localVarType;
	/* variable declaration */
	string varCode;
	/* Function prototype */
	string funcCode;
	/* the next, brak and continue labels */
	int nextLabel;
	int breakLabel;
	int contLabel;
	/* Are we parsing currently in global/local scope? */
	bool global;
	/* Symbol table information. */
	stEntry globalSymbol[MAX_SYMBOLS];
	stEntry localSymbol[MAX_SYMBOLS];
	
	public:

	fStore funcStore[100];
	int funcIndex;
	string currFuncName;
	int insideFunc;
	int fCallIndex;
	int fParamIndex;

	int special;


		/* Constructor */
		CodeGen();
		/* Destructor */
		~CodeGen();
		/* Get variable type */
		string getVarType(void) {
			if (global)
				return globalVarType;
			else
				return localVarType;
		}
		/* Set variable type */
		void setVarType(string v) {
			if (global)
				globalVarType = v;
			else
				localVarType = v;
		}
		/* Get variable code */
		string getVarCode(void) {
			return varCode;
		}
		/* Get function code */
		string getFuncCode(void) {
			return funcCode;
		}
		/* Reset function code */
		void resetFuncCode() {
			funcCode = "";
		}
		/* Reset variable code */
		void resetVarCode() {
			varCode = "";
		}
		/* Append this string to function code */
		void appendFuncCode(string str) {
			funcCode += str;
			if (insideFunc == 1) {
				appendFuncBody(str);
			}
		}
		void appendSpecial1FuncCode(string str, string id) {
			funcCode += str;
			if (insideFunc == 1) {
				appendFuncBody("prelogue"+ id +"\n");
				appendFuncBody("goto "+id+"Func;\n");
				appendFuncBody("labelCode\n");

			}
		}
		void appendSpecial2FuncCode(string str, string id1, string id2) {
			funcCode += str;
			if (insideFunc == 1) {
				appendFuncBody("prelogue\n");
				appendFuncBody("goto "+id1+"Func;\n");
				appendFuncBody("labelCode\n");
				appendFuncBody(id2+" = returnCode\n");

			}
		}
		void appendTreeFuncCode(string str, string str2, string str3) {
			funcCode += str;
			if (insideFunc == 1) {

				size_t pos = str3.find("(");
				string id = str3.substr (0, pos);

				appendFuncBody("prelogue"+id+"\n");
				appendFuncBody("goto "+id+"Func;\n");
				appendFuncBody("labelCode\n");
				appendFuncBody(str2+" = returnCode\n");
			}
		}
		void appendReturnFuncCode(string str, string str2) {
			funcCode += str;
			if (insideFunc == 1) {
				appendFuncBody("returnwithcode = " + str2 + ";\t// return value\n");
			}
		}
		void appendReturn1FuncCode(string str) {
			funcCode += str;
			if (insideFunc == 1) {
				appendFuncBody("returnwithoutcode\n");
			}
		}
		/* Append this string to the variable declaration code */
		void appendVarCode(string str) {
			varCode = varCode + str;
		}
		void inFunc(void) {
			insideFunc = 1;
		}
		void outFunc(void) {
			insideFunc = 0;
		}
		/* Generate var code */
		void genVarCode(void);
		/* Get next label */
		string getNextlabel(void) {
			nextLabel++;
			return "c" + to_string((unsigned long long)nextLabel);
		}
		/* Set break label */
		void setBreakLabel(void) {
			breakLabel = nextLabel;
		}
		/* Get break label */
		string getBreakLabel(void) {
			return "c" + to_string((unsigned long long)breakLabel);
		}
		/* Set continue label */
		void setContLabel(void) {
			contLabel = nextLabel;
		}
		/* Get cont label */
		string getContLabel(void) {
			return "c" + to_string((unsigned long long)contLabel);
		}
		/* Reset block label */
		void resetBlockLabel(void) {
			breakLabel = 0;
			contLabel = 0;
		}
		/* Save symbol information */
		void saveSymbol(string id, int size);
		/* Save file information */
		void saveFuncName(string str, int paraIndex, string paraArray[50]);
		void appendFuncBody(string str);
		void appendFuncBody2(string str);
		void saveFuncVarNum(void);
		void saveFuncCallName(string str);
		void saveFuncCallParam(string str);
		int getFuncCallCount(void);

		/* Create local temporary symbol */
		string createLocalSymbol(void);
		/* get Symbol variable to replace with */
		string getSymbol(string id, int index);
		/* Get scope of this array variable */
		bool getScope(string id);
		/* Get base address of this array symbol */
		int getBaseAddress(string id);
		/* Reset local */
		void resetLocal(void);
		/* Parse expression tree */
		void parseTree(eTree *t);
		/* Parse condition expression tree */
		void parseCondTree(eTree *t, eTree *root);
};

#endif
