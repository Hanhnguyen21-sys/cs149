s/\*\*

- Description: A minimal shell program
- Author names: Nguyen Nguyen and Duc Nguyen
- Author emails: nguyen.n.nguyen@sjsu.edu
- Last modified date: 02/23/2026
- Creation date: 02/22/2026
  \*\*/

## Project Description

The assignment extends a minimal shell program to spawn multiple child processes in parallel for a single command when the command is `./countnames names1.txt names2.txt ...`.

- The shell reads command line
- The command is `./countnames` followed by N filenames, the shell forks N children
- Each child executes `./countnames` on exactly one file.
- Each `countnames` child writes results to:
  - `PID.out` (name counts)
  - `PID.err` (warnings for empty lines)

## Files Included

- `shell.c` : prompt-based shell, forks one child/filename
- `countnames.c`: read names and counts occurrences; write output to `PID.out` and warnings to `PID.err`

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

- PID.out:
  Jenn Xu: 2
  Tom Wu: 1
- PID. err is empty because there is no empty line in names2.txt

Test case 2 - Two files in parallel
Command (inside shell)
%./countnames test/names2.txt test/names.txt

Expected output files:

- PID1.out:
  Jenn Xu: 2
  Tom Wu: 1
- PID1. err is empty because there is no empty line in names2.txt

- PID2.out:
  Nicky: 1
  Dave Joe: 2
  Yuan Cheng Chang: 3
  John Smith: 1
- PID2 err:
  Warning - file test/names.txt line 2 is empty.
  Warning - file test/names.txt line 5 is empty.

Test case 3 - Many files
Command (inside shell)
%./countnames test/names2.txt test/names.txt test/names_long_redundant.txt names1.txt namesB.txt

Expected output files:
5 .err files and 5 .out files

Test case 4 - Invalid file
Command (inside shell)
%./countnames invalid.txt

Expected output files:

- PID.out is empty
- PID. err:
  error: cannot open file not_exist.txt

Test case 4 - Empty file
Command (inside shell)
%./countnames test/subdir/empty_lines.txt

Expected output files:

- PID.out and PID.err are empty

## Reflection

From this assignment, we gained a deeper understanding of
how processes work and how multiple processes can run at the
same time. We also developed a clearer understanding of how
processes are executed in parallel by the operating system. While
implementing the assignment, we learned how to use several
process-related APIs such as fork(), exec(), and wait(). To prevent
bottlenecks when many child processes are created, the parent process
must properly wait for them to finish using wait() or waitpid().
Because the assignment required writing results to PID.out and warnings
to PID.err, we also learned how to redirect program output to per-process
log files by opening and writing to files. In addition, we improved our
understanding of exit codes and how they are used to indicate the termination
status of a process.

## References

Slides in classes: Class 5 Slides - Process API, CS149 s

Author
Duc Nguyen
Nguyen Nguyen
