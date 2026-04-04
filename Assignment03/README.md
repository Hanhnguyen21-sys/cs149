s/\*\*

- Description: Project describes how processes communicate through pipes or mmap
- Author names: Nguyen Nguyen and Duc Nguyen
- Author emails: nguyen.n.nguyen@sjsu.edu
- Last modified date: 03/15/2026
- Creation date: 03/13/2026
  \*\*/

## Project Description

The assignment extends a minimal shell so that each child processes can send back information to its parent through pipe. After each child executes `./countnames` on exactly one file, it writes MessengerHeader and NameCountData to its parent. The parent process get the result from each child, sum all counts and print the final result after processes have finished.

## Files Included

- `shell.c` : prompt-based shell, forks one child/filename
- `countnames.c`: read names and counts occurrences
- `structs.h`
- `test/` directory containing provided test files

---

## Compilation Instruction

To compile `countnames`:
gcc countnames.c -o countnames

To compile `shell`:
gcc shell.c -o shell

## Run the program

After compiling, you can run the program with specific test file:
./shell

You will see the prompt
%

Enter commands like:
%./countnames test/names.txt test/names_long.txt

## Test cases

Test case 1 - Single input file
Command (inside shell)
%./countnames test/names2.txt

Expected output files:

- Shell:
  Result:
  Jenn Xu: 2
  Tom Wu: 1
  Total name counted: 2
- PID out:
  Jenn Xu: 2
  Tom Wu: 1
- PID. err is empty because there is no empty line in names2.txt

Test case 2 - Two files in parallel
Command (inside shell)
%./countnames test/names2.txt test/names1.txt

Expected output files:

- Shell:
  Result:
  Jenn Xu: 2
  Tom Wu: 4
  Total name counted: 2
- PID2.err:
  Warning - file test/names1.txt line 3 is empty.
- Outputs are printed in .out files

Test case 3 - Many files
Command (inside shell)
%./countnames test/names2.txt test/names.txt test/names1.txt

- Shell:
  Result:
  Jenn Xu: 2
  Tom Wu: 4
  Nicky: 1
  Dave Joe: 2
  Yuan Cheng Chang: 3
  John Smith: 1
  Total name counted: 6

- 5 .err files and 5.out files

Test case 4 - Invalid file
Command (inside shell)
%./countnames test/not_exist.txt

Expected output:

- Shell:
  Invalid file
  Result:
  Total name counted: 0

- PID. err:
  error: cannot open file test/not_exist.txt
- PID.out is empty

Test case 5 - Empty file
Command (inside shell)
%./countnames test/empty.txt

Expected output files:

- Shell:
  Result:
  Total name counted: 0
- PID.err and PID.out are empty

## Reflection

This assignment helped me gain a better understanding of how two processes communicate with each other using pipe(). At the beginning, it was confusing, especially when trying to distinguish which end of the pipe the parent should use to read information and which end the child should use to send messages.

I also added two helper functions that ChatGPT suggested. One of them is add_to_totals(), which is used to calculate the total number of names counted from all child processes. The other one is read_all(), which ensures that all bytes are read from the pipe. In the read_from_pipe() function (provided by the professor), we need to read all the data sent from the child process. If we only use read(), it may not guarantee that all bytes are returned in one call. Therefore, read_all() is used to repeatedly read from the pipe until all expected bytes have been received.

Overall, this assignment helped me better understand interprocess communication and how parent and child processes coordinate through pipes.

## References

Slides in classes: Class 8 Slides - Inter-Process Communication (IPC): Pipes and Memory mapped files (mmap)

Author
Duc Nguyen
Nguyen Nguyen
