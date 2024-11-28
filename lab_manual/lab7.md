## Dynamic Taint Analysis

Writing a dynamic taint analysis tool for C/C++ programs as an LLVM pass to detect `ControlFlowHijack` and `InjectionAttack` problems in the programs.

### Objective
In this lab, you will build a dynamic taint analysis tool on the IR intermediate representation. By implementing taint sources, taint propagation strategies, and taint sinks, you will be able to trace the propagation of taints within the program, thereby detecting potential security issues.

### Setup
The code for Lab7 is located under `/lab7/`.

- Open the lab7 folder in VS Code, using the 'Open Folder' option in VS Code.
- Make sure the Docker is running on your machine.
- Open the VS Code Command Palette by pressing F1; search and select Reopen in Container.
- This will set up the development environment for this lab in VS Code.
- Inside the development environment the skeleton code for Lab 6_2 will be located under /lab7.
- Afterwards, if VS Code prompts you to select a kit for the lab then pick Clang 8.

#### lab7's project structure:

```
- lib
  |
  -- runtime.cpp: Runtime functions to handle runtime taint propagation, such as `StoreInstProcess`, etc., that you will inject using your pass.

- src
  |
  -- DynTaintAnalysisPass.cpp: Contains the overall instrumentation logic for functions and instructions, calling different instrumentation functions for different types of instructions/functions.
  |
  -- Instrument.cpp: Instrumentation functions for each type of instruction or function, which insert calls to the runtime functions at the current instruction location.
  |
  -- Utils.cpp: Some helper functions, such as `getOperandsString`, etc.
```

#### Step 1
The following commands set up the lab, using the CMake/Makefile pattern.
```
/lab7$ mkdir -p build && cd build
/lab7/build$ cmake ..
/lab7/build$ make
```

You should see several files created in the lab7/build directory. A LLVM pass named `DynTaintAnalysisPass.so` will be generated as the result of linking `DynTaintAnalysisPass.cpp` and `Instrument.cpp` under `lab7/src`, and a runtime library called `libruntime.so`, corresponding to `lab7/lib/runtime.cpp`. These are all source files that you will modify later. If you recall the project build steps of lab2, the steps here are almost identical to the part where it uses a dynamic analysis pass.

#### Step 2
Generate LLVM IR as you did in previous lab.
```
/lab7$ cd test
/lab7/test$ clang -emit-llvm -S -fno-discard-value-names -c -o InjectionAttack.ll InjectionAttack.cpp -g
/lab7/test$ clang -emit-llvm -S -fno-discard-value-names -c -o ControlFlowHijack.ll ControlFlowHijack.cpp -g
```

#### Step 3
Use opt to run the provided DynTaintAnalysisPass pass on the compiled C++ program. This step generates an instrumented program with runtime function calls.
```
/lab7/test$ opt -load ../build/DynTaintAnalysisPass.so -DynTaintAnalysisPass -S InjectionAttack.ll -o InjectionAttack.dynamic.ll
/lab7/test$ opt -load ../build/DynTaintAnalysisPass.so -DynTaintAnalysisPass -S ControlFlowHijack.ll -o ControlFlowHijack.dynamic.ll
```

#### Step 4
Next, compile the instrumented program and link it with the runtime library to produce a standalone executable:
```
/lab7/test$ clang -o InjectionAttack -L../build -lruntime InjectionAttack.dynamic.ll
/lab7/test$ clang -o ControlFlowHijack -L../build -lruntime ControlFlowHijack.dynamic.ll
```

#### Step 5
Finally run the executable. When you complete all the source files, they should work like this:
```
/lab7/test$ ./InjectionAttack
Filename:example.txt; ls -al
tainted var address: 0x7ffc34796880
That's the address in:%arraydecay
Taint propagated from 0x7ffc34796880 to 0x7ffc34796880
From :%filename to :%arraydecay3 
Taint propagated from 0x7ffc34796880 to 0x7ffc34796c80
From :%arraydecay3 to :%arraydecay2 
Taint propagated from 0x7ffc34796c80 to 0x7ffc34796c80
From :%cmd to :%arraydecay5 
Taint detected in sensitive position: 0x7ffc34796c80!
Aborted!
That's the address in:%arraydecay5
```

```
/lab7/test$ ./ControlFlowHijack
input:AAAAAAAAA
tainted var : %call1
Taint propagated from %call1 to 0x7ffca89e706c
From :%call1 to :%ch 
Taint propagated from 0x7ffca89e706c to %7
From :%ch to :%7 
Taint propagated from %7 to %conv
From :%7 to :%conv 
Taint propagated from %conv to 0x7ffca89e7080
From :%conv to :%9 
tainted var : %call1

...

Taint propagated from %call1 to 0x7ffca89e706c
From :%call1 to :%ch 
Taint propagated from 0x7ffca89e7088 to %16
From :%data to :%16 
Taint propagated from %16 , 32 to %add
From :%16 , 32 to :%add 
Taint propagated from %add to 0x7ffca89e707c
From :%add to :%secret_value 
Taint propagated from 0x7ffca89e707c to %19
From :%secret_value to :%19 
Taint detected in sensitive position: %19!
Aborted!
```

### Lab Instructions
#### Program being analyzed
We provide two programs to be analyzed:`InjectionAttack.cpp` and `ControlFlowHijack.cpp`.

In `InjectionAttack.cpp`, when the user enters the required filename argument to `/bin/cat`, additional commands can be run unchecked if some additional content is added. For example, if a user inputs `example.txt ; ls -al`, the command string becomes: `/bin/cat example.txt ; ls -al`. The first command (`/bin/cat example.txt`) is executed to display the contents of `example.txt`, and then the second command (`ls -al`) is executed without any restrictions. This allows an attacker to execute arbitrary commands on the system, leading to potential unauthorized access or manipulation of files.
```
char cmd[2048] = "/bin/cat ";
char filename[1024];
printf("Filename:");
scanf (" %1023[^\n]", filename); // The attacker can inject a shell escape here
strcat(cmd, filename);
system(cmd); // Warning: Untrusted data is passed to a system call
```
Example Attack:
```
/lab7/test$ ./InjectionAttack
Filename:example.txt ; ls -al
This is an example txt file.total 32
drwxrwxrwx 1 root root   512 Nov 26 09:22 .
drwxrwxrwx 1 root root   512 Nov 26 08:45 ..
-rwxr-xr-x 1 root root  8416 Nov 26 09:21 ControlFlowHijack
-rw-r--r-- 1 root root   728 Nov 26 08:48 ControlFlowHijack.cpp
-rwxr-xr-x 1 root root 12584 Nov 26 09:21 InjectionAttack
-rw-r--r-- 1 root root   420 Nov 25 16:22 InjectionAttack.cpp
-rwxrwxrwx 1 root root   346 Nov 26 08:47 Makefile
-rw-r--r-- 1 root root    28 Nov 26 09:22 example.txt
-rw-r--r-- 1 root root     0 Nov 26 09:22 otherfile.secret
```

In `ControlFlowHijack.cpp`, when user/hacker write to `mem.buffer` without checking the buffer size, it overwrites what follows; in this case, to successfully hijack the control flow, the user must input exactly 9 characters, with the 9th character being 'A'. This overwrites mem.data with the value 65, resulting in secret_value being calculated as 97. So the user's input can **accidentally** affect the control flow of the program, causing control flow Hijacking(Normally, `secret_value` will not be equal to 97).
```
struct Memory{
        char buffer[8]; 
        int data = 0; 
};

bool check_secret(int secret){
        return secret == 97;
}

int main() {
    Memory mem;
    int secret_value=0;
    
    //To simulate unsafe gets
    char* ptr = mem.buffer;
    int ch;
    while ((ch = getchar()) != '\n') {
        *ptr++ = ch; 
    }
    *ptr = '\0';
    
    secret_value=mem.data+32;
    // check secret_value
    if (check_secret(secret_value)) {
        printf("You've discovered the secret value!\n");
    } else {
        printf("Secret value: %d\n", secret_value);
    }
}
```
Example Attack:
```
/lab7/test$ ./ControlFlowHijack
input:AAAAAAAAA
You've discovered the secret value!
```


#### Dynamic Taint Analysis
Taint analysis consists of three components: `taint source`/`taint propagation strategy`/`taint sink`

- taintsource

    Taint sources are those input points in a program that can introduce untrusted or insecure data. These input points could be user input, file reading, network data, etc. 

    Tips: In our two examples, only user input was used as the taint source, but in real applications, file reading and network data transferred are more common.

- taint propagation strategy

    This part can be simply summarized as: If the source operand is tainted, then the taint should be passed to the target operand.
    
    For example, `%b = load i32, ptr %a, align 4` loads a data of type i32 from an address in `%a` to `%b`, the source operand is `%a`, the destination operand is `%b`, then when %a is tainted, %b needs to be tainted.

    In addition to normal instructions, some function calls also do taint propagation, in our case `strcat` concatenates two strings, if one of the strings is tainted, then taint also needs to be propagated to the concatenated result.

- taint sink

    When a sensitive program location/sensitive program behavior is reached, a taint sink is added to check whether a particular variable is tainted.
    
    In our two examples, the argument variables of system/check_secret need to be checked to see if they are tainted before the system/check_secret is called.



#### Features of our tool
Different taint analysis tools have different features of data structures and taint processing methods.Here, we declare some features of our tool:

- Taint granularity

    The taint granularity of this tool is a mixture of variables and bytes: for non-pointer variables, we track them at the granularity of variables, and for pointer variables, track them at the granularity of bytes.

- Taint color

    In taint tracking, taint color is an attribute used to identify and distinguish different taints. From the color of the taint, one can infer its source, type, or state. However, in our implementation, we do not track the source or state of the taints; we simply differentiate between two states: tainted or not (i.e., only black and white).

- Taint Data Structure

    We use sets to store taint information. Combining the two features mentioned above, in `runtime.cpp`, you will find two sets `taintedPtrVars` and `taintedVars`. For a none-pointer type variable, if its name is in `taintedVars`, the variable is considered tainted; For a pointer type variable, if its runtime-address is in `taintedPtrVars`, it indicates that the variable is tainted.
    
    A more sophisticated tool might use data structures such as shadow memory, which is simplified here.

- Supported Instructions

    This tool does not support all instruction types, but only a subset of them, including TruncInst, GEPInst, StoreInst, LoadInst, BinaryOperator. To create a more comprehensive and universal tool, it will be necessary to accommodate all instruction types.

- Different treatment for pointer and non-pointer types

    The **necessity** of this differentiation: **At the IR level**, we **can't** get the location of a non-pointer variable in memory, while at the binary (assembly) level, we can tell by instruction which register/memory the value is in.

    Before distinguishing between pointer and non-pointer types, we need to know the pointer/non-pointer type of each instruction operand. This is the type of operand for each instruction:

    |Instruction|Format|DestType|SrcType|
    |:-:|:-:|:-:|:-:|
    |TruncInst|`%dest` = trunc **i32** `%src` to **i8**|int|int|
    |GetElementPtrInst|`%dest` = getelementptr **inbounds i8**, **ptr** `%src`, i32 1|ptr|ptr|
    |StoreInst|store **ptr/i8** `%src`, **ptr** `%dest`, align 8|ptr|ptr/int|
    |LoadInst|`%dest` = load **ptr/i8**, **ptr** `%src`, align 8|ptr/int|ptr|
    |BinaryOperator|`%dest` = add nsw **i32** `%src1`, **i32** `%src2`|int|int|
    
    Therefore, there are two versions of the function that handle the taint propagation of the StoreInst: `StoreInstProcess` and `StoreInstProcessPtr`.Similarly, when setting the taint source (taint sink), there will also be two versions: `TaintVal` (`CheckVal`) and `TaintPtrVal` (`CheckPtrVal`).

    For LoadInst, since its source operand must be a pointer, the address of the source operand can determine whether pollution is needed, so there is only one Ptr version.

#### TODO List:
In terms of code/technical implementation, dynamic taint analysis requires the following three steps:   
`1.`Develop the instrumentation logic and package it as an LLVM pass;  
`2.`Use the pass to instrument the IR file of the target program, inserting calls to runtime functions;  
`3.`Compile the modified IR file into an executable and run it.

So in this lab, we need to complete the instrumentation logic as well as  the run time function being inserted, you will have the following TODO list:

- Add corresponding instrumentation function calls for various instructions and taint source-related functions (scanf, getchar) in the main run function `runOnFunction` of `DynTaintAnalysisPass.cpp`. These instrumentation functions are used to insert runtime functions at specific locations.
- Complete the instrumentation functions for `Trunc` and `Load` instructions in `Instrument.cpp`.
- Complete the runtime analysis functions for `Store`and `BinaryOperator` instructions in `runtime.cpp`.

#### Relating to instrumentation
The instrumentation method in this lab is similar to the dynamic analysis pass of lab2. if you forgot some of the details, review [Instrumentation Pass](https://github.com/ecnu-sa-labs/ecnu-sa-labs/blob/ff8658063073a4aa46afa6552bd18c281b477baf/lab_manual/lab2.md#instrumentation-pass) and [Inserting Instructions into LLVM code](https://github.com/ecnu-sa-labs/ecnu-sa-labs/blob/ff8658063073a4aa46afa6552bd18c281b477baf/lab_manual/lab2.md#inserting-instructions-into-llvm-code) in **lab2's tutorial**.

### Submission
Once you are done with the lab, submit your code by commiting and pushing the changes under lab7/. Specifically, you need to submit the changes to `src/DynTaintAnalysisPass.cpp`, `src/Instrument.cpp` and `lib/runtime.cpp`
```
   lab7$ git add src/DynTaintAnalysisPass.cpp src/Instrument.cpp lib/runtime.cpp
   lab7$ git commit -m "your commit message here"
   lab7$ git push
```
