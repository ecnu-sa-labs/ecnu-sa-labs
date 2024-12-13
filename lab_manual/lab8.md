<!-- TOC --><a name="dynamic-symbolic-execution"></a>
# Dynamic Symbolic Execution

<!-- TOC start -->

- [Dynamic Symbolic Execution](#dynamic-symbolic-execution)
   * [1. Objectives](#1-objectives)
   * [2. Getting Stared](#2-getting-stared)
      + [2.1 Motivating Example](#21-motivating-example)
      + [2.2 Limitation](#22-limitation)
      + [2.3 Setup](#23-setup)
    * [3. Understanding the Workflow of `miniklee`](#3-understanding-the-workflow-of-miniklee)
   * [4. Task 1 Implementing the Process of Symbolization](#4-task-1-implementing-the-process-of-symbolization)
      + [4.1 Functions You Need to Know](#41-functions-you-need-to-know)
      + [4.2 Tips](#42-tips)
      + [4.3 Proof of Concept (POC)](#43-proof-of-concept-poc)
   * [5. Task 2 Interpreting the semantics of `Add` and `Sub`](#5-task-2-interpreting-the-semantics-of-add-and-sub)
      + [5.1 Functions You Need to Know](#51-functions-you-need-to-know)
      + [5.2 Tips](#52-tips)
      + [5.3 Proof of Concept (POC)](#53-proof-of-concept-poc)
   * [6. Task 3 Interpreting the semantics of `Br`](#6-task-3-interpreting-the-semantics-of-br)
      + [6.1 Functions You Need to Know](#61-functions-you-need-to-know)
      + [6.2 Tips](#62-tips)
      + [6.3 Proof of Concept (POC)](#63-proof-of-concept-poc)
   * [7. Submission](#7-submission)


<a name="0-attention"></a>
## 0. Attention

> Due to the different virtualization mechanisms of Windows and our older Docker image, please note the following adjustments

**Change to use WSL2 if you are using Windows.**

Open PowerShell, download WSL2, and execute the following command:

``` bash
wsl --install -d Ubuntu
```
(Optional) In PowerShell, set Ubuntu as the default OS:

``` bash
wsl --set-default Ubuntu
```
Open WSL2, download your repository, and open it with VSCode (using the Windows version of VSCode, no need to download it separately):

``` bash
lab8: code .
```
Make sure VSCode has the Docker and Dev Container extensions installed, then simply reopen the container.

Additionally, a potential issue is that the LLVM version in our current container is outdated and not suitable for running MiniKLEE. To address this, I added a hot patch to download the appropriate LLVM version **automically** from the network when the container starts. However, for some students with **poor network**, you'd better switch to a domestic mirror, such as Aliyun or Tsinghua mirrors. After switching the mirror, re-download the corresponding LLVM version. You can either restart the container to download it **automatically** or **download it manually**:

``` bash
lab8$ rm llvm.sh                        # Remove the current llvm.sh
lab8$ wget https://apt.llvm.org/llvm.sh # Download llvm.sh from the network
lab8$ chmod +x llvm.sh                  # Grant execute permission
lab8$ ./llvm.sh 14 all                  # Execute to download LLVM version 14
```

> I apologize for the rushed lab preparation and any inconvenience it may have caused.


<a name="1-objectives"></a>
## 1. Objectives

In this lab, you’ll implement a dynamic symbolic execution (DSE) engine `miniklee`, including:

1. Implementing the process of symbolization
2. Implementing an execution-generated testing (EGT) symbolic execution framework.
	1. Interpreting the semantics of `Add` and `Sub`
	2. Interpreting the semantics of `Br`

<!-- TOC --><a name="2-getting-stared"></a>
## 2. Getting Stared

<!-- TOC --><a name="21-motivating-example"></a>
### 2.1 Motivating Example

We've provided the executable `refminiklee` (in `./`) for a quick look at symbolic execution.

Our motivating example is as follow. Let the `__builtin_trap()` (line 9) be the assumed bug. The program starts by making the variable `a` symbolic. It, then encounters an `if` branch (`if (a + 2 == 100 - 10)`). When the value of `a` is set to 88, the assumed bug can be exploited.

``` c
   1 #include "Symbolic.h"
   2 
   3 int main() {
   4     int a;
   5     make_symbolic(&a, sizeof(a), "a");
   6 
   7     if (a + 2 == 100 - 10) {
   8         // Trap, a must be 88
   9         __builtin_trap();
  10     } else {
  11         // Should reach, a can be assigned all values that are not 88
  12     }
  13 
  14     return 0;
  15 }
```

The `refminiklee` works as follows:
1. It updates (line 5) the variable `a` as symbolic in virtual machine memory
2. Next,it checks the if branch (line 7) and encodes the branch statement into constraint (`a + 2 == 100 - 10`), then sends it with previous path constraints  (none in this example) to a constraint solver
3. Next, the solver determines the feasibility of the constraint(s) to `true branch and false branch are both ok`
4. `miniklee` then forks states, and updates the new state to state pool
5. For the state pass true branch, `miniklee` generates one test case (with `error` as a suffix) with `a  =  88`, as for false branch, it generates one test case (`normal` as a suffix) with random value that are not 88.

**The following shows how to symbolic explore the example above using `refminiklee`**:

1. Compile the program under test to LLVM IR **without any optimization**

``` bash
$ clang -emit-llvm -g -O0 -S ./test/example.c -o ./test/example.ll -I./include
```

2. Use `refminiklee` to do symbolic execution on the generated LLVM IR (`./test/example.ll`)

``` bash
$ ./refminiklee ./test/example.ll
LLVM IR file loaded successfully
State 1: Alloca
State 1: Alloca
State 1 Store
State 1 Mk Sym
State 1 Load
State 1 Add
State 1 ICMP_EQ comparison
State 1 Br
Assigning 88 
Test case written successfully (result_1/test_case_1.error)
State 2 Br
State 2 Ret
Assigning -1634890980 
Test case written successfully (result_1/test_case_2.normal)
```

3. Examine the generated test cases

``` bash
$ cat result_1/test_case_1.error
a, 88
$ cat result_1/test_case_2.normal
a, -1634890980
```

The value of `test_case_2.normal` is randomly generated as long as it is not 88.

<!-- TOC --><a name="22-limitation"></a>
### 2.2 Limitation

To simpilfy, input programs in this lab are assumed to have **only sub-features of the C language** as follows:
- All values are signed integers of 32 bits (i.e., no floating points, pointers, structures, enums, arrays, etc)
- Programs under test contain only  main function, which means no function calls
- Condition involving symbolic variables are either equal or not equal
- Operations only involved:
	- [Alloca](https://llvm.org/doxygen/classllvm_1_1AllocaInst.html): An instruction to allocate memory on the stack
	- [Load](https://llvm.org/doxygen/classllvm_1_1LoadInst.html): An instruction for reading from memory
	- [Store](https://llvm.org/doxygen/classllvm_1_1StoreInst.html): An instruction for storing to memory
	- [Ret](https://llvm.org/doxygen/classllvm_1_1ReturnInst.html): An instruction for returning a value and transferring control flow
	- [Eq](https://llvm.org/doxygen/classllvm_1_1ICmpInst.html): An instruction for comparing its operands (if equal) according to the predicate
	- [SLT](https://llvm.org/doxygen/classllvm_1_1ICmpInst.html): An instruction for comparing its operands (if less) according to the predicate
	- [Br](https://llvm.org/doxygen/classllvm_1_1BranchInst.html): An conditional or unconditional branch instruction
	- [Call](https://llvm.org/doxygen/classllvm_1_1CallInst.html): An instruction for abstracting a target machine's calling convention
	- [Add](https://codebrowser.dev/llvm/llvm/include/llvm/IR/Instruction.def.html#147): An instruction for performing an integer addition of two operands and producing their sum
	- [Sub](https://codebrowser.dev/llvm/llvm/include/llvm/IR/Instruction.def.html#149): An instruction for performing an integer subtraction, subtracting the second operand from the first, and produces the result

<!-- TOC --><a name="23-setup"></a>
### 2.3 Setup

To build your own `miniklee`, simply type `make` in `lab8` directory:

``` bash
$ make
```

Then you get your current version `miniklee` in current directory,  which can only now test naive test cases (e.g., `test/sequential.c`):

``` bash
$ clang -emit-llvm -g -O0 -S -I./include test/sequential.c  -o test/sequential.ll
$ ./miniklee test/sequential.ll
LLVM IR file loaded successfully
State 1: Alloca
State 1: Alloca
State 1: Alloca
State 1 Store
State 1 Store
State 1 Store
Assigning -2082133583 
Test case written successfully (result_1/test_case_1.error)
```

You can observe a test case being generated (e.g., `-2082133583`, a randomly generated value) in the terminal logs. However, there is no assigned value in `result_1/test_case_1.error` because there are no symbolic variables in `sequential.c`. In other words, `sequential.c` doesn't receive any input. 

Your first task is to make variables symbolic (in Section 4).

<!-- TOC --><a name="3-understanding-the-workflow-of-miniklee"></a>

## 3. Understanding the Workflow of `miniklee`

- `src/main.cpp`

The main function receives an LLVM IR file and parses it, then, it transfers control flow to symbolic executor. 

- `src/Executor.cpp:runFunctionAsMain`

After preparations, the `miniklee` performs symbolic execution in the function `runFunctionAsMain`:

1. It begins with an initial state and uses a state pool (`states`) for storage.
2. In the main loop, if the state pool is not empty, it continues:
   1. Select a state for execution.
   2. Fetch the next instruction to execute.
   3. Execute the instruction and perform analysis.
   4. Update the state by adding new states and removing dead ones.
3. When a state reaches termination, it generates a test case for that state.

- `src/Executor.cpp:executeInstruction`

Focusing on instruction execution, this part primarily involves a large `switch-case` statement. It identifies and interprets the fetched instruction. For example:  
- Interpreting a `Ret` instruction terminates the current state.  
	- `src/Executor.cpp:terminateState`
		- Generate test case for current state
		- Remove the state that is about to end
- Interpreting a `Br` instruction transfers control to the next basic block.  
	- `src/Executor.cpp:fork`
		- Invoke a solver to determine the feasibility of  current branch condition
		- Fork states according to the result of solver
- Interpreting a `Add` instruction performs integer addition of two operands.  


<!-- TOC --><a name="4-task-1-implementing-the-process-of-symbolization"></a>
## 4. Task 1 Implementing the Process of Symbolization

As explained in the Section 3, the `miniklee` interprets each instruction according to the corresponding semantics. With interpretation in hand, we can design how we interpret the semantics of the symbolization and implement it.

**Implement the `src/Executor.cpp:executeMakeSymbolic` function. We’ve provided code skeletons marked with “Task 1: Your Code Here”**

<!-- TOC --><a name="41-functions-you-need-to-know"></a>
### 4.1 Functions You Need to Know

- `include/Symbolic.h:make_symbolic`

We have already provided the function declaration for symbolization in `include/Symbolic.h`:

```c++
void make_symbolic(int32_t *sym, size_t nbytes, const char *name);
```

The `make_symbolic` function receives three arguments:
1) Address of variable to be symbolized
2) Number of bytes of the variable
3) Name given to the variable.

We can use it in our programs under test like the following code snippet. Actually, the KLEE symbolic executor also use this method to mark a variable as symbolic (KLEE official documentation [Marking input as symbolic](https://klee-se.org/tutorials/testing-function/)).

``` c++
1: int a;
2: make_symbolic(&a, sizeof(a), "a"); // Variable a now should be symbolized
3: if (a + 2 == 100 - 10) { ...  }
```

- `src/Executor.cpp:executeInstruction`

Please focuse on the `Call` instruction case: when `miniklee` executes the `make_symbolic` statement (line 2 in Snippet 1), it identifies and processes symbolization.

- `src/Executor.cpp:needsSymbolization`

Identify whether the current call is `make_symbolic` function.

- `src/Executor.cpp:processMakeSymbolic`

Parse the arguments of `make_symbolic` and proceed to make them symbolic.

<!-- TOC --><a name="42-tips"></a>
### 4.2 Tips

**Tip 1 Read the friendly code**
- May helpful to read the code interpretation of `Load` and `Store` instructions (`src/Executor.cpp:executeInstruction`) to understand the storage system of `miniklee` (**Especially the function `include/Executor.h:executeMemoryOperation`**)
- May helpful to read the code interpretation of modeling instructions (`include/Expr.h`) to understand the representation of each data in symbolic execution.

 **Tip 2 Deep understanding of the representation of programs**

 We represent programs from different perspectives, for example:
 - **Source code**
   This is concrete syntax of programs, whose strength is easy to read, however, it's difficult to transform, analyze and optimize. Below is an example for calculating the factorial of the number stored in `value` (in this case, `8`).
 ``` c++
 int value = 8;
 int result = 1;
 for (int i = 1; i <= value; ++i) {
     result *= i;
 }
 std::cout << result << std::endl;
 ```

 - **Abstract Syntax Tree**
 ```
       program
             |
         block
      /      |     \
  alloc load store ...
 ```

   The [abstract syntax tree](https://en.wikipedia.org/wiki/Abstract_syntax_tree) (AST) representation is beneficial for compilers as it simplifies parsing and analysis. For interpreters, it is particularly useful—processing a tree recursively allows easy evaluation of blocks by interpreting each statement while updating the program's environment.   The main drawback of ASTs is the diverse behavior of their node types. Writing compiler analyses requires constantly handling the semantic differences between these node types, which can be cumbersome. Alternative representations exist that are more suitable for implementing complex compiler optimizations. These approaches make the representation more regular, simplifying the process.

 - **Instruction** (Especially the LLVM IR)
   The LLVM IR is of form static [single-assignment](https://en.wikipedia.org/wiki/Static_single-assignment_form) (SSA), which is already explained in the [LLVM IR Lecture](https://tingsu.github.io/files/courses/slides/lec-2-llvm-framework-primer.pdf).  We like the instruction representation for its regularity. To do anything useful with it, however, we need to extract higher-level representations (**`miniklee` is what goes through at the basic block and instruction level**):
   - [Control flow graph](https://en.wikipedia.org/wiki/Control-flow_graph) (CFG).
   - [Basic blocks](https://en.wikipedia.org/wiki/Basic_block).
   - Terminators (jmp and br, here).
   - Derive the algorithm for forming basic blocks.
   - Successors & predecessors.
   - Derive the algorithm for forming a CFG of basic blocks.

   We appreciate LLVM IR, particularly for its SSA (Static Single Assignment) property: each variable has a single static assignment globally. However, this does not imply dynamic single assignment, as the same statement can execute multiple times.  To summarize, in LLVM IR code:
   - definitions == variables
   - instructions == values
   - arguments == data flow graph edges

**Tip 3 "Too long didn't read; I just want to lie flat."**

It’s okay to feel that way. Take it step by step—you’re doing great! 💪. 

Let's break the steps of symbolization into the following:
1. Represent the symblic value using the provided type `SymbolicExpr`
2. Store the created symblic value into symblic memory, using `executeMemoryOperation`

The answer is below::

``` c++
void Executor::executeMakeSymbolic(ExecutionState& state,
                                   Instruction *symAddress,
                                   std::string name) {
    // Register the variable (Instruction type) to be symbolic
    executeMemoryOperation(state, true, symAddress, 
                           SymbolicExpr::create(name), 0);
}
```


<!-- TOC --><a name="43-proof-of-concept-poc"></a>
### 4.3 Proof of Concept (POC)

After implementing task 1, run the test case `test/symbolic.c` to verify  your changes. This test introduces symbolic variables, allowing you to observe how the symbolic execution engine processes them. Check the generated test input to ensure the symbolic inputs are recognized and correctly handled.

Expected results:

``` bash
$ make clean
$ make
... compiling logs omitted ...
$ clang -emit-llvm -g -O0 -S -I./include test/symbolic.c  -o test/symbolic.ll
$ ./miniklee test/symbolic.ll
LLVM IR file loaded successfully
State 1: Alloca
State 1: Alloca
State 1 Store
State 1 Mk Sym
Assigning -399613364
Test case written successfully (result_1/test_case_1.error)
```
Now you can see there is an assigned value (`-399613364` is randomly generated here) in `result_1/test_case_1.error` for the symbolic value `a` (assumed as the program input).

<!-- TOC --><a name="5-task-2-interpreting-the-semantics-of-add-and-sub"></a>
## 5. Task 2 Interpreting the semantics of `Add` and `Sub`

As explained in the Section 3, the `miniklee` interprets each instruction according to the corresponding semantics. With interpretation in hand, we can design how we interpret the semantics of the `Add` and `Sub` instructions and implement them.

**Implement the semantic interpretation for `Add` and `Sub` (`src/Executor.cpp:executeInstruction`). We’ve provided code skeletons marked with “Task 2: Your Code Here.”**

<!-- TOC --><a name="51-functions-you-need-to-know"></a>
### 5.1 Functions You Need to Know

- `src/Executor.cpp:executeInstruction`

Please focus on the `Add` and `Sub` instruction cases, when `miniklee` executes the `Add`  or `Sub` statement, it identifies and processes the instruction.

<!-- TOC --><a name="52-tips"></a>
### 5.2 Tips

**Tip 1: "Too long didn't read; I just want to lie flat."**

Read the friendly source code of KLEE to get inspired.
- [KLEE code dealing with `Add`](https://github.com/klee/klee/blob/master/lib/Core/Executor.cpp#L2582)
- [KLEE code dealing with `Sub`](https://github.com/klee/klee/blob/master/lib/Core/Executor.cpp#L2589)

<!-- TOC --><a name="53-proof-of-concept-poc"></a>
### 5.3 Proof of Concept (POC)

After implementing task 2, run the test case `test/example.c` to verify your changes. This test introduces `add` operation, `sub` operation and  branch condition, allowing you to observe how the `miniklee` processes them.

The content of `example.c`

``` c++
#include "Symbolic.h"
int main() {
    int a;
    make_symbolic(&a, sizeof(a), "a");

    if (a + 2 == 100 - 10) {
        // Trap, a must be 88
        __builtin_trap();
    } else {
        // Should reach, a can be assigned all values that are not 88
    }
    
    return 0;
}
```

Run the test

``` bash
$ make clean
$ make
... compiling logs omitted ...
$  clang -emit-llvm -g -O0 -S ./test/example.c -o ./test/example.ll -I./include
$ ./miniklee test/example.ll
LLVM IR file loaded successfully
State 1: Alloca
State 1: Alloca
State 1 Store
State 1 Mk Sym
State 1 Load
State 1 Add
State 1 ICMP_EQ comparison
State 1 Br
Assigning -732525800 
Test case written successfully (result_1/test_case_1.error)
```

It looks good. But there is something wrong: the `miniklee` explores the true branch and assigns a value (`-732525800 ` is randomly generated here) to the symbolic variable `a` that does not trigger the crash (the value should be `88`). 

**Why? This happens because the branch instruction is not handled properly.** In current version, when `miniklee` encoutering a branch instruction, it simply beleves the condition is true and explores the true branch (read the source code of switch-case dealing with `Br` instruction in `src/Executor/cpp:executeInstruction`).

Your task 3 is to handle it properly.

<!-- TOC --><a name="6-task-3-interpreting-the-semantics-of-br"></a>
## 6. Task 3 Interpreting the semantics of `Br`

**Implement the semantic interpretation for `Br` (`src/Executor.cpp:executeInstruction` and `src/Executor.cpp:fork`). We’ve provided code skeletons marked with “Task 3: Your Code Here”.**

Branch processing is central to the Execution-Generated Testing (EGT) symbolic executor. Before proceeding, let's review [its background](https://dl.acm.org/doi/10.1145/2408776.2408795).

Currently there are two types of symbolic execution:

- **Concolic Testing**: Starting execution with random inputs, and after execution, constructing a new path condition pc₁ using the path condition pc₀ of the current path (by negating the last condition) and solving pc₁ to get a new input I₁ to explore the new path, and repeating the process. As in [CREST](https://www.burn.im/crest/) and [DART](https://dl.acm.org/doi/10.1145/1064978.1065036)
_Our previous experiment falls into this type, as does [MIT 6.858: Computer System Security Lab 3: Symbolic Execution](https://css.csail.mit.edu/6.858/2022/labs/lab3.html)._

- **Execution-Generated Testing (EGT)**: Fork symbolic execution at each conditional branch (if both directions are feasible), maintain multiple partial paths, and coordinate their execution simultaneously. As in [EXE](https://dl.acm.org/doi/10.1145/1455518.1455522), [SPF](https://dl.acm.org/doi/10.1145/1858996.1859035), and [KLEE](https://dl.acm.org/doi/10.5555/1855741.1855756).
_This is the type of `miniklee` implementation._

As explained above, EGT executors fork their executions at each conditional branch if both directions are feasible. You task is to implement the `fork` function used for interpreting `Br` instruction.

<!-- TOC --><a name="61-functions-you-need-to-know"></a>
### 6.1 Functions You Need to Know

- `src/Executor.cpp:executeInstruction`

- `src/Executor.cpp:transferToBasicBlock`

Update the program counter to transfer the execution state to the start of the specified basic block.  

- `src/Executor.cpp:getInstructionValue`

Get the symbolic value of the definition of coresponding instruction.

<!-- TOC --><a name="62-tips"></a>
### 6.2 Tips

 **Tip 1: Understanding the workflow of executing `Br` in `miniklee`**
 
 When the executor processes the `Br` instruction, it first casts it to the corresponding `Br` type ([Lecture The LLVM Framework, p.37](https://tingsu.github.io/files/courses/slides/lec-2-llvm-framework-primer.pdf)). It then uses the API ([isUnconditional of BranchInstr](https://llvm.org/doxygen/classllvm_1_1BranchInst.html#a7e4be8b16fbd68c9045a388904044e01)) to determine if the branch is unconditional or conditional and handles each case as follows:
 
 - **Unconditional**: Transfers the execution state to the next basic block.
 - **Conditional**: Extracts the condition and sends it to the solver to check feasibility:
   - **True branch feasible**: Transfers to the basic block for the true branch.
   - **False branch feasible**: Transfers to the basic block for the false branch.
   - **Both branches feasible**: Forks new states for each branch, transfers control to their respective basic blocks, and adds the forked states to the state pool.  
 
 
 **Tip 2: Understanding the workflow of the `fork` function**
 
 To make the lab-doing life less painful, we have already provided a skeleton for the `fork` function (`src/Executor.cpp:fork`), the `fork` works as follow:
 
 - **Invoke solver to determine the feasibility of branch**: It first invokes the solver to check if there is a valid assignment for the given query (true and false branches under the path constraints) where the condition evaluates to true. To be clear, if the true branch is feasible, we say the `trueBranch` is set to true; otherwise, false. Similarly, if the false branch is feasible, `falseBranch` is set to true; otherwise, false. You can implement this part as follows:

``` c++
    // Invoke solver to determinie the feasibility of the condition
    bool trueBranch = solver->evaluate(
        Query(current.constraints, condition)
    );
    bool falseBranch = solver->evaluate(
        Query(current.constraints, NotExpr::create(condition))
    );
```

 - **Perform actions based on the solver's results.**
   - **`trueBranch` is considered true, `falseBranch` is considered false**
     - Just return current state as the first position of `StatePair`
   - **`falseBranch` is considered true, `trueBranch` is considered false**
     - Just return current state as the second position of `StatePair`
   - **Both `trueBranch` and `falseBranch` are considered true**
     - Clone the current state (_may helpful to read the API provided by `ExecutionState` in `src/ExecutionState.cpp`_)
     - Add the new cloned state to the state pool (_pushing back to the global variable `addedStates` is fine; do not directly manipulate the state pool `states`._)  
     - Add current condition to current state (state passing true branch), and the negated condition to cloned state (state passing false branch)
     - Return the two states within a `StatePair`

**Tip 3: "Too long didn't read; I just want to lie flat."**

Read the friendly source code of KLEE to get inspired.
- [KLEE code dealing with `Br`](https://github.com/klee/klee/blob/master/lib/Core/Executor.cpp#L2218)
- [KLEE code fork function](https://github.com/klee/klee/blob/master/lib/Core/Executor.cpp#L1039)

<!-- TOC --><a name="63-proof-of-concept-poc"></a>
### 6.3 Proof of Concept (POC)
After implementing task 3, run the test case `test/example.c` to verify your changes. You should now see the same results of running `refminiklee` (except for the randomly generated value). Please self-check according to the [2.1 Motivating Example](#21-motivating-example).

<!-- TOC --><a name="7-submission"></a>
## 7. Submission

Once you are done with the lab, submit your code by commiting and pushing the changes under lab8/. Specifically, you need to submit the changes to `src/Executor.cpp`

``` bash
   lab8$ git add src/Executor.cpp
   lab8$ git commit -m "your commit message here"
   lab8$ git push
```
