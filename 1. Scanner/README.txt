This directory contains two files - README.txt and main.c for the purpose of the assignment in CSC 512 namely Project 1: Scanner.

---------
Execution
---------

a.The files main.c and readme.txt should be copied to the "CSC 512 ubuntu 14.04 image" image in the NCSU VCL. Please place the files in a folder (say A).
b.Add test programs/files to the same folder.
c.OPEN Terminal and move the current cursor to Directory A.
d.Run the following command to execute the program:

gcc -o hello main.c

./hello <inputfile>

-------------------
Warnings and Errors
-------------------

The program main.c when compiled receives a warning:

main.c: In function ‘Identifier’:
main.c:38:5: warning: incompatible implicit declaration of built-in function ‘strcat’ [enabled by default]
     strcat(chkstr, line);

This command was used to convert the line array into a string for further string operations.
Please ignore this warning.

----------------
Assumptions:
----------------
a. Added ’main’ as a reserved word as the 'cs512' appending was not to be performed on the same.

b. Formatting kept as close to the input file as possible.

c. In case of a quote (' or "), the entire rest of the file is swept to find a pair (not only until \n - even beyond this}

d. The contents of the meta statements have not been scanned for token identification as the line was to printed as it is on the file.

e. File is not further scanned on the occasion of an error.

----------------
Design Decisions
----------------
a. The main() function starts with opening the input and output files to read and write respectively with the output file name as required by the rule.

b. The basic design of the main() function is to get a character from the input file and then branch out depending on the value. Eg. If the character is a '#', the rest of the line should be printed as it is until a '\n' is encountered. Hence function 'Meta' would be called to handle this.

c. The one character is checked for a DIGIT, LETTER, SYMBOL, WHITE-SPACE, QUOTE, and METASTATEMENT INITIATOR and respective functions are called out depending on the same:

d. Identifier:
This function is called once a letter/underscore is encountered in main(). This function checks for successive letters/digits/underscores until an EOF or a different lexical is encountered.
This "word" is then compared to the list of "reserved words" provided and is printed out in the file as it is in the comparison is confirmed. In case it falls of, a string 'cs512’ is prefixed to the identifier and displayed on file as required by the assignment.
The "other lexical" is then sent back to main() where this lexical is looped in for a check as (c). This is done using the reset flag (explained ahead).

d. Number:
Starting with the digit encountered in main(), it is further checked if the successive characters are digits. This goes on until an EOF or a character other than a digit is encountered providing the NUMBER string. The "other character" is then sent back to main() where this lexical is looped in for a check similar to (c). This is done using the reset flag (explained ahead).

e. Operator:
This function checks if the character from main() is equivalent to the allowed symbols and prints them in file if they are. In case the character is not similar, it means an incorrect input is encountered. The success is tracked through the 'flag' variable and the program terminates displaying the error message in case flag = 0;.

f. Space:
This functions reads the white-spaces as encountered in the input and displays the same to the output file.

6. Quote:
This function checks if the str encountered from main() has a match in the successive characters in file and reports error in case it does not find a match.

7. Meta:
This function checks the meta statements that start with '#' or '//'. Such statements are copied as is to the output until an EOF or a '\n' is encountered.

Important flag used:
reset in main():
This flag when equal to 1 allows main() to read one character from file on every iteration. In case this is equal to 0, and the getc is skipped fir that particular iteration.
This is required in cases where a previous call to Number or Identifier was made. As they shoot ahead by one character to find the end of the lexical pattern, the extra character needs to be checked for another pattern. Another case where this is required is in case when a 'Meta’ call is made on basis of ch1='/' but the next character encountered in 'Meta' is not an '/'. In this case the new character is sent back to main with reset = 0 so that this can be checked for a further pattern.

