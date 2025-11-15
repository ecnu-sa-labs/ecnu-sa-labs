## Understanding Static and Dynamic Analysis

Understanding the basic concepts of software analysis and metrics used to estimate the effectiveness of analysis.

### Objective

The objective of this lab is to use standard analysis tools for static and
dynamic analysis on C programs to discover divide-by-zero errors and interpreting
their results to better understand various trade-offs between the techniques.
Specifically, we will use <a href="https://github.com/google/AFL">AFL</a> (American Fuzzy Lop), a dynamic analyzer (fuzzer) and <a href="https://clang-analyzer.llvm.org/">CSA</a> (Clang Static Analyzer), a static analyzer.

### Pre-Requisites

The lectures introduce various terminology used throughout this lab such as:
static and dynamic analysis, soundness, completeness, precision, recall, and more.

### Setup

##### Step 1.
The skeleton code for Lab 1 is located under `/lab1`.
We will refer to this top-level directory for Lab 1 simply as `lab1`
when describing file locations for the lab.


##### Step 2.
Throughout the labs, we will use `CMake`, a modern tool for
managing the build process.
If you’re unfamiliar with `CMake` we recommend reading the
[CMake tutorial][cmake-tutorial]
(especially pay attention to Step 1 and Step 2 in the tutorial).
Running `cmake` produces a `Makefile` that you might be more familiar with.
If you are not familiar with `Make`, read either the
[Makefile tutorial][makefile-tutorial]
or [Learn Make in Y minutes][learn-make-in-y-minutes] first,
and then peruse file `lab1/Makefile`.
Ensure that you are comfortable with using `Makefile` in this lab.

##### Step 3.

Inspect the Makefile to see the commands used to run <a href="https://github.com/google/AFL">AFL</a> and <a href="https://clang-analyzer.llvm.org/">CSA</a>.

```sh
# Compile the program with AFL
AFL_DONT_OPTIMIZE=1 afl-gcc c_programs/test1.c -o test1
# Set kernel to dump crashes into "core" files for AFL
echo core >/proc/sys/kernel/core_pattern
# Run AFL for 30s on test1
timeout 30s afl-fuzz -i afl_input -o afl_output -- ./test1

# Run CSA on test1.c
clang -v --analyze c_programs/test1.c
```

### Lab Instructions

In this lab, you will run AFL and CSA on a suite of C programs,
study these two tools’ results, and report your findings.

##### Step 1.

Run the provided analysis tools, AFL and CSA, on all C programs
located under the `lab1/c_programs` directory.
To do so, simply run the following command,
which first runs AFL with a timeout of 30 seconds per program,
and then runs CSA.

```sh
/lab1$ make check_versions && make all
```

On running the above command, you will see the output that looks
something like this:

```
AFL_DONT_OPTIMIZE=1 afl-gcc c_programs/test1.c -o ...
timeout 30s afl-fuzz -i afl_input -o ...
Makefile:8: recipe for target 'results/afl_logs/test1/out.txt' failed
make: [results/afl_logs/test1/out.txt] Error 124 (ignored)
clang -v --analyze c_programs/test1.c ...
...
```

Ignore the error reported by Make above; it is normal because
AFL keeps running until it is forcibly terminated by the timeout command.
Feel free to experiment by changing the timeout which is set to 30 seconds.
We do not expect everyone to report the same solutions since
AFL is non-deterministic anyway.

On running the make command, the following files and directories should be generated
under the `lab1/results` directory:

```
   ├── afl_logs/
   │   ├── test0/
   │   │   ├── out.txt
   │   │   ├── afl_output/
   │   │   └── test0
   │   ├── ... // similar for test1
   │   ...
   │
   └── csa_logs/
       ├── test0_out.txt
       ├── ... // similar for test1
       ...
```

##### Step 2.

Determine the ground truth (`right` vs. `wrong`) of the C programs with respect to
division-by-zero errors.
In particular, for each division instruction in each program, determine by
inspecting the program whether it can result in a division-by-zero error on
some program inputs.
Write your answers in file `lab1/answers.txt` in the “ground truth” column
of the table for each test.

##### Step 3.

Study the output of AFL and CSA and determine if they accept or reject each program.
Fill in your answers in file `lab1/answers.txt` in the corresponding columns of
the table for each test program.

The crashing inputs discovered by AFL are stored in separate files under
directories `lab1/results/afl_logs/<test-name>/afl_output/crashes/`.
The files have idiosyncratic names of the form
`id:000000,sig:08,src:000000,op:arith8,pos:2,val:-8`.[^1].
It is the contents of these files that AFL used as the input
to the test program when it encountered a crash.

Upon examining CSA's outputs, if a `core.DivideZero` warning emerges, it indicates that CSA has detected a division by zero error. 

For example:

```c
c_programs/test9.c:10:17: warning: Division by zero [core.DivideZero]
   10 |   int avg = sum / len;
      |             ~~~~^~~~~
```

##### Step 4.

Using your entries from Steps 2 and 3, calculate the
Precision, Recall, and F1 Score of each column.
Enter them in the corresponding rows in `lab1/answers.txt`.

##### Step 5.

Answer the questions in `answers.txt` with the help of the table you filled in.

### Submission

Once you are done with the lab, submit your results by commiting and pushing the changes under `lab1/`. 
Note that please submit the results on your *local machine* rather than on the remote machine in Docker.
Specifically, you need to submit the results generated by CSA and AFL in `lab1/results` with your answer sheet `answers.txt`.

```
   lab1$ git add results/ answers.txt
   lab1$ git commit -m "your commit message here"
   lab1$ git push
```

[^1]: The file name encodes various things such as the ID of the crashing input, the crashing signal, the non-crashing seed input from which this crashing input was produced -- which in our case is always the file lab1/afl_input/seed.txt, and the operations by which the non-crashing seed input was transformed into this crashing input.

[cmake-tutorial]: https://cmake.org/cmake/help/latest/guide/tutorial/index.html
[makefile-tutorial]: https://www.gnu.org/software/make/manual/html_node/Simple-Makefile.html
[learn-make-in-y-minutes]: https://learnxinyminutes.com/docs/make
