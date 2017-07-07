//
//  main.c
//  Program
//
//  Created by Krishna Agarwala on 07/09/15.
//  Copyright (c) 2015 ghosty. All rights reserved.
//

#include <stdio.h>
FILE *ifp, *ofp;                                    //Global File Pointers for input and output respectively.


//Identifier - function for scanning Identifiers and Reserved Keywords
char Identifier (char str){
    char ch;
    char line[100];
    int  i = 0;
    char chkstr[100] = "";
    char chkstr2[100] = "cs512";                    //to append to a non-reserved word identifier.

    line[i] = str;
    i++;
    
    while (!feof(ifp)) {
        while ((ch = getc(ifp)) != EOF) {
            if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch == '_') || (ch >= '0' && ch <= '9')) {
                line[i] = ch;
                i++;
            }
            else {
                break;
            }
        }
        break;
    }
    
    line[i] = '\0';
    strcat(chkstr, line);

    if ((!strcmp(chkstr, "int")) || (!strcmp(chkstr, "void")) || (!strcmp(chkstr, "if")) || (!strcmp(chkstr, "while")) || (!strcmp(chkstr, "return")) || (!strcmp(chkstr, "read")) || (!strcmp(chkstr, "write")) || (!strcmp(chkstr, "print")) || (!strcmp(chkstr, "continue")) || (!strcmp(chkstr, "break")) || (!strcmp(chkstr, "binary")) || (!strcmp(chkstr, "decimal")) || (!strcmp(chkstr, "main"))) {
        
        fprintf(ofp, "%s", chkstr);                         //print to file in case its a reserved word
    }
    else {
        strcat(chkstr2, chkstr);
        fprintf(ofp, "%s", chkstr2);                        //append cs512 in case it is an identifier and then print to file
    }
    
    return ch;
}

//Number - function for scanning Number


char Number (char str){
    char ch;
    char line[100];
    int  i = 0;
    line[0] = str;
    i++;
    
    while (!feof(ifp)) {
        while ((ch = getc(ifp)) != EOF) {
            if (ch >= '0' && ch <= '9'){
                line[i] = ch;
                i++;
            }
            else {
                break;
            }
        }
        break;
    }
    
    line[i] = '\0';
    fprintf(ofp, "%s", line);
    
    return ch;                                          //ch carries the next read value to main for further analysis.
    
}

//Operator - function for scanning Symbols

int Operator (char str){
    
    char ch;
    int flag = 0;
    char line[3];
    
    if ((str == '(') || (str == ')') || (str == '{') || (str == '}') || (str == '[') || (str == ']') || (str == ',') || (str == ';') || (str == '+') || (str == '-') || (str == '/') || (str == '*') || (str == '=') || (str == '>') || (str == '<')) {
        flag = 1;
        fprintf(ofp, "%c", str);                        //cases where symbol stand alone successfully
    }
    else if ((str == '&') || (str == '|')) {            //cases where && and || symbol stand stand together
        ch = getc(ifp);
        if ((ch != EOF) && (ch == str)){
            flag = 1;
            line[0] = str;
            line[1] = ch;
            line[2] = '\0';
            fprintf(ofp, "%s", line);
        }
    }
    else if (str == '!') {                              //case for != symbol stand stand together
        ch = getc(ifp);
        if ((ch != EOF) && (ch == '=')){
            flag = 1;
            line[0] = str;
            line[1] = ch;
            line[2] = '\0';
            fprintf(ofp, "%s", line);
        }
    }
    
    return flag;                                        //flag = 1 implies success
}

void Space (char str){
    fprintf(ofp, "%c", str);
}

//Quote - function for scanning quotes
int Quote (char str){
    char ch;                                //variable scaning further elements from file
    char line[10000];
    int i = 0;
    int flag = 0;
    line[0] = str;
    i++;
    
    while (!feof(ifp)) {
        while ((ch = getc(ifp)) != EOF) {   //checks until EOF for the quote pair
            if (ch != str){
                line[i] = ch;
                i++;
            }
            else {
                break;
            }
        }
        break;
    }
    if (ch == str){                         //implies success
        line[i] = ch;
        line[i+1] = '\0';
        flag = 1;
        
        fprintf(ofp, "%s", line);
    }
    
    return flag;                            //flag shows success(=1) or error(!= 1)
    
}

//Meta - function for scanning metastatements
char Meta (char str){
    char ch;                                //variable scaning further elements from file
    char ret;                               //return variable in case line does not start with // but only /
    char line[100];
    int i = 0;
    int flag = 0;
    
    if (str == 47){                         //47 - ASCII for /
        ch = getc(ifp);
        if (ch == 47){                      //success if two consecutive '/' encountered
            flag = 1;                       //flag = 1 implies success
            line[i] = str;
            i++;
            line[i] = ch;
            i++;
        }
    }
    else if (str == '#'){
        flag = 1;                           //flag = 1 implies success
        line[i] = str;
        i++;
    }
    
    if (flag == 1){                         //if success - scan entire line until \n
        while (!feof(ifp)) {
            while ((ch = getc(ifp)) != EOF) {
                if (ch != '\n') {
                    line[i] = ch;
                    i++;
                }
                else {
                    break;
                }
            }
            if (ch == '\n'){
                line[i] = '\n';
                line[i+1] = '\0';
                break;
            }
            else{
                line[i] = '\0';             //in case meta statement concludes with EOF
                break;
            }
        }
        fprintf(ofp, "%s", line);
        ret = '/';                          //ret = '/' implies successfull scanning
    }
    else if (flag == 0){
        fprintf(ofp, "%c", str);            //SYMBOL / Tagging requiried. //Need to check the ch left now.
        ret = ch;                           //in case of unsuccessful scanning, next scanned char is sent back to main.
    }                                       //scanning unsuccessful when line starts from / and not //
    
    return ret;
}

int main(int argc, char *argv[]) {
    
    //input file filename defined
    char *inpfname;
    if (argc == 2){
        inpfname = argv[1];
    }
    
    //output file name being derived
    int i=0, j = 0;
    char oupfname[100];
    char append[6] = "_gen.";
    
    while (argv[1][i] != '.') {
        oupfname[i] = argv[1][i];
        i++;
    }
    for (j=i; (j<=i+4);j++){
        oupfname[j] = append[j-i];
    }
    i++;
    while (argv[1][i] != '\0') {
        oupfname[i+4] = argv[1][i];
        i++;
    }
    oupfname[i+4]= '\0';
    
    //input and output file being opened
    
    ifp = fopen(inpfname, "r");
    if (ifp == NULL || feof(ifp)) {
        printf("No or Empty Input File\n");
        return 0;
    }
    
    ofp = fopen(oupfname, "w");
    if (ofp == NULL) {
        printf("Can't open output file!\n");
        return 0;
    }
    
    //main implementation begins
    int quoteflag, opflag, metaflag, numflag, idflag; //flags used for respective cases
    int error_flag = 0;                               //identify error cases
    int reset = 1;
    char ch1;                                         //file read character
    
    
    while (!feof(ifp)) {
        
        if (reset == 1){
            ch1 = getc(ifp);
        }
        else {
            reset = 1;
        }
                
        if (ch1 != EOF){
            
            char v_case = 'X'; //Acts as default - illegal characters.
            
            if (ch1 >= '0' && ch1 <= '9') {
                v_case = 'N'; //NUMBER
            }
            else if ((ch1 >= 'a' && ch1 <= 'z')||(ch1 >= 'A' && ch1 <= 'Z')||(ch1 == '_')){
                v_case = 'I'; //IDENTIFIER
            }
            else if ((ch1 == 32)||(ch1 == 9)||(ch1 == 10)||(ch1 == 11)||(ch1 == 13)){
                v_case = 'S'; //SPACE - checks for blankspace, h tab, line feed, v tab, carriage return
            }
            else if ((ch1 == 39)||(ch1 == 34)){
                v_case = 'Q'; //QUOTE - handles both ' and "
            }
            else if ((ch1 == '#')||(ch1 == 47)){
                v_case = 'M'; //META - handles cases that start with # and /
            }
            else if ( (ch1 == '(') || (ch1 == ')') || (ch1 == '{') || (ch1 == '}') || (ch1 == '[') || (ch1 == ']') || (ch1 == ',') || (ch1 == ';') || (ch1 == '+') || (ch1 == '-') || (ch1 == '/') || (ch1 == '*') || (ch1 == '=') || (ch1 == '>') || (ch1 == '<') || (ch1 == '&') || (ch1 == '|') || (ch1 == '!') ){
                v_case = 'O'; //OPERATOR - SYMBOL
            }
            
            switch(v_case)
            {
                case 'M' :
                    metaflag = Meta(ch1);
                    if (metaflag != '/'){       // metaflag = '/' is a sign of success i.e Meta statement started with //
                        ch1 = metaflag;
                        reset = 0;
                    }
                    break;
                case 'N' :
                    numflag = Number(ch1);      //numflag holds character that was accessed after a Number was recognized
                    ch1 = numflag;
                    reset = 0;
                    break;
                case 'I' :
                    idflag = Identifier(ch1);   //idflag holds character that was accessed after an identifier was recognised
                    ch1 = idflag;
                    reset = 0;
                    break;
                case 'S' :                      //formatting provided as per input file
                    Space (ch1);
                    break;
                case 'Q' :
                    quoteflag = Quote (ch1);    //quoteflag = 1 is a sign of success i.e pair of quote literal present
                    if (quoteflag == 1) {
                        break;
                    }
                    else {
                        fprintf(ofp, "Error - Illegal input wrt quotes");
                        error_flag = 1;
                        break;
                    }
                case 'O' :
                    opflag = Operator (ch1);    //opflag = 1 is a sign of success i.e successful operator encountered
                    if (opflag == 1) {
                        break;
                    }
                    else {
                        fprintf(ofp, "Error - Illegal input wrt operators");
                        error_flag = 1;
                        break;
                    }
                case 'X' :                      //none of the recognised characters - hence illegeal input.
                    fprintf(ofp, "Error - Illegal input - Unidentified.");
                    error_flag = 1;
                    break;
            }
            
            if (error_flag == 1){
                break;
            }
        }
    }
    
    
    //closing both input and output files
    
    fclose(ifp);
    fclose(ofp);

    return 0;
}
