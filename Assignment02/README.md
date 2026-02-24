/\*\*

/\*\*

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

Slides in classes: Class 5 Slides - Process API, CS149 C Introduction 3

Author
Duc Nguyen
Nguyen Nguyen
