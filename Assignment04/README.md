- Description: The assignment describes how processes can communicate through shared memory created by using mmap().

- Author names: Nguyen Nguyen and Duc Nguyen
- Author emails: nguyen.n.nguyen@sjsu.edu
- Last modified date: 04/06/2026
- Creation date: 03/30/2026

## Project Description

The assignment extends a minimal shell so that each child processes can send data by writing it into a shared global memory. Then, the parent process will combine the results stored in this memory.
We choose the easier option to implement the assignment by

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

Through this assignment, we learned several important concepts related to process communication and memory management. First, we gained a deeper understanding of how a parent process can create a large shared memory region and divide it into multiple sections. Each child process is assigned a specific region, which ensures that processes do not interfere with each other while writing their results. This helped us understand how memory can be safely shared and organized among multiple processes.

In addition, we learned how to use mmap() to map files into memory and use them as a communication mechanism. Compared to pipes, this approach allows data to be written and accessed more directly, without needing to send messages through a stream. This made the design more structured, since each child writes its results in a fixed location, and the parent can later read from those locations at its own pace.

Another key takeaway is the difference between process memory spaces and shared memory. Normally, after a fork() and especially after execvp(), processes do not share memory. This assignment showed us how memory-mapped files can be used to overcome that limitation and allow data sharing across separate processes.

## References

Author
Duc Nguyen
Nguyen Nguyen
