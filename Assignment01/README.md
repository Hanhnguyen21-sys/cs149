/\*\*

- Description: This program counts all the names in files how many times each name appears.
- Author names: Nguyen Nguyen and Duc Nguyen
- Author emails: nguyen.n.nguyen@sjsu.edu
- Last modified date: 02/12/2026
- Creation date: 02/08/2026
  \*\*/

PROJECT DESCRIPTION
This program implements a utility called countnames, which reads a text file containing one name per line and counts how many times each name appears. It also detects and reports empty lines in the input.

COMPILATION INSTRUCTIONS
To compile the program, use the following command:
gcc countnames.c -o countnames

This creates an executable file named: countnames

RUN THE PROGRAM

Requirements
GCC (GNU Compiler Collection) installed on your system.

After compiling, you can run the program with specific test file:
./countnames <input filename>
EX: ./countnames names.txt

You can run using standard input:
cat <input filename> | ./countnames

If you want to show warning/error messages only:
./countnames <input filename> > /dev/null

TEST CASES
All added test cases are located in the directory:
test/subdir/

Test 1 - Normal Name Counting Without Repetition
Input file: test/subdir/test1.txt

Command:
./countnames test/subdir/test1.txt

Expected Output:
CATHERINE CAMPBELL: 1
FRANCES PARKER: 1
ANN EVANS: 1
JOYCE EDWARDS: 1

Test 2 - Normal Name Counting With Repetition and Empty Lines
Input file: test/subdir/test2.txt

Command:
./countnames test/subdir/test2.txt

Expected Output:
Warning - Line 2 is empty.
Warning - Line 5 is empty.
Warning - Line 8 is empty.
STEPHANIE MITCHELL: 3
BARBARA JONES: 2
ANN EVANS: 1

Test 3 - Long names (more than 30 characters)
Input file: test/subdir/test3.txt

Command:
./countnames test/subdir/test3.txt

Expected Output:
(Name is truncated to the first 30 characters)
abcdedsfnasdgjiergoireuqt94309: 1

Test 4 - Last line is an empty line
Input file: test/subdir/test4.txt

Command:
./countnames test/subdir/test4.txt

Expected Output:
Warning - Line 3 is empty.
Warning - Line 7 is empty.
STEPHANIE MITCHELL: 3
CAROLYN PEREZ: 2

Test 5 - Case sensitivity
Input file: test/subdir/test5.txt

Command:
./countnames test/subdir/test5.txt

Expected Output:
Warning - Line 2 is empty.
Warning - Line 4 is empty.
Warning - Line 8 is empty.
MARIA MILLER: 2
mARIA MILLER: 1
MArIA MILlER: 2
MARiA MIlLER: 1

REFLECTION
We have two people in our team, and this is the first time for both of us coding in C. Through this assignment, we learned new concepts such as character encodings, string handling, and memory usage in C. We also learned how to compile a C program using gcc and how to run programs in a Linux environment.

From the instructions provided, we became more familiar with Linux commands such as cat for concatenating files and redirecting output using > /dev/null to display only error or warning messages.

At the beginning, our code did not work correctly for input files that contained blank lines between names. The program counted newline or whitespace characters as a name instead of reporting an empty line warning. We used an AI tool to help us identify the issue. After understanding the problem, we modified our code by adding logic to trim whitespace and properly detect empty lines. This allowed the program to behave as expected.

Author
Duc Nguyen
Nguyen Nguyen
