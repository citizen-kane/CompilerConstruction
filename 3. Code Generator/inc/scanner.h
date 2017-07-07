/***********************************************************************************************
 *
 *  File Name	: scanner.h
 *
 *  Description	: Header file for scanner.cc
 *
 ***********************************************************************************************/

#ifndef __SCANNER_H__
#define __SCANNER_H__

#include <string>
#include <fstream>
#include <iostream>
#include <codegen.h>

using namespace std;

/* Uncomment the below line to start debug prints, and 
 * Comment the below line to stop debug prints */
//#define DEBUG
#ifdef DEBUG
#define	debug(str, args...)	printf(str, ##args)
#else
#define debug(str, args...)	do{} while(0)
#endif

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
	TOKEN_EOF,
	TOKEN_ERROR
};

/* Class consists of token type and the token content */
class Token {
	public:
		enum TokenTypes Type; /* Which type of token */
		string Text; /* Text content of token */
		Token() { /* Constructor */
			Type = TOKEN_ERROR;
			Text = "";
		}
		enum TokenTypes GetTokenType(void) {
			return Type;
		}
		void SetTokenType(enum TokenTypes type) {
			Type = type;
		}
};

/* Scanner class */
class Scanner {
	ifstream InputFile;	/* FILE pointer to read from input file */
	ofstream OutputFile;
	ofstream OutputFile2;	/* FILE pointer to write to output file */
	string InputFileName;	/* Name of input file */
	string OutputFileName;	/* Name of generated output file */
	string OutputFileName2;	/* Name of 2nd generated output file */
	CodeGen *c;		/* Code Gen pointer */
	/*To print meta statement and global part once*/

	public:

	int metaCodeOnce = 1;

	int labelIndex = 0;

	string paramArray[50];
	int    paraIndex;

		/* Constructor */
		Scanner(char * file, CodeGen& c);
		/* Destructor */
		~Scanner();
		/* Are there any more tokens left in the input file */
		int HasMoreTokens(void);
		void createCode(void);
		/* Return the next token from the input file */
		Token GetNextToken(void);
		/* Return the token that can be used for parsing.
		 * Exclude tokens like meta statements */
		Token GetParsingToken(void);
		/* Print meta output */
		void PrintMeta(string str) {
			OutputFile << str + "\n";
			OutputFile2 << str + "\n";
		}
		/* Dump this string into the output file */
		void PrintOutput(void);
		/* Function to delete output file in case of error */
		void DeleteOutputFile() {
			remove(OutputFileName.c_str());
			//remove(OutputFileName2.c_str());
		}
		/*To print meta statement and global part once*/
		void appendMetaCode(void) {
			if (metaCodeOnce == 1) {
				OutputFile2 << "\n#define N 2000\n\n#define top mem[0]\n#define base mem[1]\n#define jumpReg mem[2]\n#define membase 3\nint mem[N];\n\nint main() {  // the only function we will use\n\n// prologue before calling main\n";
				metaCodeOnce = 0;
			}
		}

		void addMainPrelogue(void)
		{
			string buffer;
			int mainBaseSize;
		
			buffer = "top = membase; \nmem[top] = 0;    // exit address\nbase = top + 1;  // base of main\ntop = base + ";
		
			for (int i = 1; i <= c->funcIndex; i++)
			{
				if (c->funcStore[i].funcName == "main"){
					mainBaseSize = c->funcStore[i].varNum;
				}
			}
			buffer += to_string(mainBaseSize)+";  // top of main\n\nmainFunc:\n";
			OutputFile2 << buffer;
		}

		void addMainBody(void)
		{
			int i = 1;
			for ( ; i <= c->funcIndex; i++)
			{
				if (c->funcStore[i].funcName == "main")
				{
					break;
				}
			}

			string body = c->funcStore[i].body;
			size_t pos;
			
			while ((pos = body.find("prelogue"))!=string::npos){
				size_t posnl = body.find("\n", pos);
				int fSize = (posnl-pos-8);
				string fName = body.substr(pos+8, fSize);

				int j = 1;
				for ( ; j <= c->funcStore[i].fCallCount; j++)
				{
					if (c->funcStore[i].fCallName[j] == fName)
					{
						break;
					}
				}
				int fPCount = c->funcStore[i].fParamCount[j];
				string buffer = "//prologue to "+fName+":\n";
				int prVar = 0;
				for (int k = 1; k <= fPCount ; k++)
				{
					buffer += "mem[top+"+to_string(prVar)+"] = "+ c->funcStore[i].fCallParam[j][k]+";\n";
					prVar++;
				}
				buffer += "mem[top+" + to_string(prVar) + "] = base;\n";
				prVar++;

				buffer += "mem[top+" + to_string(prVar) + "] = top;\n";
				prVar++;
				
					int retStore = prVar;
					size_t posRet = body.find("returnCode");
					if (posRet!=string::npos){
						body = body.substr(0, posRet) + "mem[top+" + to_string(retStore) + "];" + body.substr(posRet+10, (body.length()-posRet-10));
					}

				prVar++;
				labelIndex++;
				buffer += "mem[top+" + to_string(prVar) + "] = "+to_string(labelIndex)+";\n";

					size_t posLabel = body.find("labelCode");
					body = body.substr(0, posLabel) + "label_" + to_string(labelIndex) + ":;" + body.substr(posLabel+9, (body.length()-posLabel-9));

				prVar++;
				buffer += "base = top + " + to_string(prVar) + ";\n";
				int varCnt = 0;

				for (int k = 1; k < c->funcIndex ; k++)
				{
					if (c->funcStore[k].funcName == fName)
					{
						varCnt = c->funcStore[k].varNum;
						break;
					}					
				}
				prVar += varCnt;
				buffer += "top = base + " + to_string(varCnt) + ";\n";

				body = body.substr(0, pos) + buffer + body.substr(posnl, (body.length()-posnl));
				//OutputFile2 << body;
			}
			c->funcStore[i].body = body;
			OutputFile2<<c->funcStore[i].body;

		}

		void addMainEpilogue(void)
		{
			string buffer;
		
			buffer = "//epilogue of Main\njumpReg = mem[base-1];\t// Goto(mem[base-1])\ngoto jumpTable;\n\n";
			OutputFile2 << buffer;
		}

		void addFuncBody(void)
		{

			for ( int i = 1; i <= c->funcIndex; i++)
			{
				if (c->funcStore[i].funcName == "main")
				{
					continue;
				}
				else
				{
					string buffer2, buffer3;
					string body = c->funcStore[i].body;
					size_t pos;

					body = c->funcStore[i].funcName + "Func: ;\n" + body;

					while ((pos = body.find("prelogue"))!=string::npos){
						size_t posnl = body.find("\n", pos);
						int fSize = (posnl-pos-8);
						string fName = body.substr(pos+8, fSize);

						int j = 1;
						for ( ; j <= c->funcStore[i].fCallCount; j++)
						{
							if (c->funcStore[i].fCallName[j] == fName)
							{
								break;
							}
						}
						int fPCount = c->funcStore[i].fParamCount[j];
						string buffer = "//prologue to "+fName+":\n";
						int prVar = 0;
						for (int k = 1; k <= fPCount ; k++)
						{
							buffer += "mem[top+"+to_string(prVar)+"] = "+ c->funcStore[i].fCallParam[j][k]+";\n";
							prVar++;
						}
						buffer += "mem[top+" + to_string(prVar) + "] = base;\n";
						prVar++;

						buffer += "mem[top+" + to_string(prVar) + "] = top;\n";
						prVar++;
						
							int retStore = prVar;
							size_t posRet = body.find("returnCode");
							if (posRet!=string::npos) {
								body = body.substr(0, posRet) + "mem[top+" + to_string(retStore) + "];" + body.substr(posRet+10, (body.length()-posRet-10));	
							}
							

						prVar++;
						labelIndex++;
						buffer += "mem[top+" + to_string(prVar) + "] = "+to_string(labelIndex)+";\n";

							size_t posLabel = body.find("labelCode");
							body = body.substr(0, posLabel) + "label_" + to_string(labelIndex) + ":;" + body.substr(posLabel+9, (body.length()-posLabel-9));

						prVar++;
						buffer += "base = top + " + to_string(prVar) + ";\n";
						int varCnt = 0;

						for (int k = 1; k < c->funcIndex ; k++)
						{
							if (c->funcStore[k].funcName == fName)
							{
								varCnt = c->funcStore[k].varNum;
								break;
							}					
						}
						prVar += varCnt;
						buffer += "top = base + " + to_string(varCnt) + ";\n";

						body = body.substr(0, pos) + buffer + body.substr(posnl, (body.length()-posnl));
						//OutputFile2 << body;
					}

					//epilogue
					size_t posReturn;
					if ((posReturn = body.find("returnwithcode"))!=string::npos) 
					{	
						buffer2 = "// epilogue of "+c->funcStore[i].funcName+"\n"+"top = mem[base-3];\n";
						body = body.substr(0, posReturn) + buffer2 + "mem[base-2]" + body.substr(posReturn+14, (body.length()-posReturn-14));
						size_t posReturn2;
						posReturn2 = body.find("// return value\n");
						buffer3 = "// return value\njumpReg = mem[base-1];\nbase = mem[base-4];\t// base of the caller function\ngoto jumpTable;\n";
						body = body.substr(0, posReturn2) + buffer3 + body.substr(posReturn2+16, (body.length()-posReturn2-16));
					}
					else if ((posReturn = body.find("returnwithoutcode"))!=string::npos)
					{
						buffer2 = "// epilogue of "+c->funcStore[i].funcName+"\n"+"top = mem[base-3];\n";
						buffer3 = "jumpReg = mem[base-1];\nbase = mem[base-4];\t// base of the caller function\ngoto jumpTable;\n";
						body = body.substr(0, posReturn) + buffer2 + buffer3 + body.substr(posReturn+17, (body.length()-posReturn-17));
					}
					else
					{
						buffer2 = "// epilogue of "+c->funcStore[i].funcName+"\n"+"top = mem[base-3];\n";
						buffer3 = "jumpReg = mem[base-1];\nbase = mem[base-4];\t// base of the caller function\ngoto jumpTable;\n";
						body = body + buffer2 +buffer3;
					}
					c->funcStore[i].body = body;
					OutputFile2<<c->funcStore[i].body;
				}
			}

		}

		void addJumpCode(void)
		{
			string buffer;
			buffer = "\n// Jump table\njumpTable:\n switch(jumpReg) {\n case 0: exit(0);\n";
			for (int i = 1; i <= labelIndex; i++)
			{
				buffer += " case " + to_string(i) + ": \n";
				buffer += "   goto label_" + to_string(i) + ";\n";
			}
			buffer += " default: assert(0);\n";
			buffer += " }\n}";

			OutputFile2<<buffer;

		}

};

#endif
