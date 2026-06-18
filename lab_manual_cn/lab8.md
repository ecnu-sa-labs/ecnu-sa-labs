# 动态污点分析

编写一个针对 C/C++ 程序的动态污点分析工具，作为 LLVM pass 来检测程序中的 `ControlFlowHijack` 和 `InjectionAttack` 问题。

## 实验目标
在本实验中，你将在 IR 中间表示上构建一个动态污点分析工具。通过实现污点源、污点传播策略和污点汇聚点，你将能够追踪污点在程序中的传播，从而检测潜在的安全问题。

## 环境搭建
Lab7 的代码位于 `/lab7/` 目录下。

- 在 VS Code 中使用“打开文件夹”选项打开 lab7 文件夹。
- 确保 Docker 正在你的机器上运行。
- 按 F1 打开 VS Code 命令面板；搜索并选择“在容器中重新打开”。
- 这将在 VS Code 中为本实验设置开发环境。
- 在开发环境中，Lab6_2 的骨架代码将位于 `/lab7` 目录下。
- 之后，如果 VS Code 提示你为实验选择一个工具包，请选择 Clang 8。

### lab7 的项目结构：

```
- lib
  |
  -- runtime.cpp: 处理运行时污点传播的运行时函数，例如 `StoreInstProcess` 等，你将通过你的 pass 注入这些函数。

- src
  |
  -- DynTaintAnalysisPass.cpp: 包含函数和指令的整体插桩逻辑，为不同类型的指令/函数调用不同的插桩函数。
  |
  -- Instrument.cpp: 每种指令或函数的插桩函数，这些函数在当前指令位置插入对运行时函数的调用。
  |
  -- Utils.cpp: 一些辅助函数，例如 `getOperandsString` 等。
```

### 步骤 1
以下命令使用 CMake/Makefile 模式设置实验环境。
```
/lab7$ mkdir -p build && cd build
/lab7/build$ cmake ..
/lab7/build$ make
```

你应该会看到在 lab7/build 目录下创建了几个文件。一个名为 `DynTaintAnalysisPass.so` 的 LLVM pass 将作为链接 `lab7/src` 下 `DynTaintAnalysisPass.cpp` 和 `Instrument.cpp` 的结果生成，以及一个名为 `libruntime.so` 的运行时库，对应于 `lab7/lib/runtime.cpp`。这些都是你稍后将修改的源文件。如果你还记得 lab2 的项目构建步骤，这里的步骤与使用动态分析 pass 的部分几乎相同。

### 步骤 2
像之前的实验一样生成 LLVM IR。
```
/lab7$ cd test
/lab7/test$ clang -emit-llvm -S -fno-discard-value-names -c -o InjectionAttack.ll InjectionAttack.cpp -g
/lab7/test$ clang -emit-llvm -S -fno-discard-value-names -c -o ControlFlowHijack.ll ControlFlowHijack.cpp -g
```

### 步骤 3
使用 opt 在编译后的 C++ 程序上运行提供的 DynTaintAnalysisPass。此步骤生成一个带有运行时函数调用的插桩程序。
```
/lab7/test$ opt -load ../build/DynTaintAnalysisPass.so -DynTaintAnalysisPass -S InjectionAttack.ll -o InjectionAttack.dynamic.ll
/lab7/test$ opt -load ../build/DynTaintAnalysisPass.so -DynTaintAnalysisPass -S ControlFlowHijack.ll -o ControlFlowHijack.dynamic.ll
```

### 步骤 4
接下来，编译插桩后的程序并将其与运行时库链接，以生成一个独立的可执行文件：
```
/lab7/test$ clang -o InjectionAttack -L../build -lruntime InjectionAttack.dynamic.ll
/lab7/test$ clang -o ControlFlowHijack -L../build -lruntime ControlFlowHijack.dynamic.ll
```

### 步骤 5
最后运行可执行文件。当你完成所有源文件后，它们应该像这样工作：
```
/lab7/test$ ./InjectionAttack
Filename:example.txt ; ls -al
tainted var address: 0x7ffc369a6c90
That's the address in:%arraydecay
Taint propagated from 0x7ffc369a6c90 to 0x7ffc369a6c90
From :%filename to :%arraydecay3 
Taint propagated from 0x7ffc369a6c90 to 0x7ffc369a7090
From :%arraydecay3 to :%arraydecay2 
Taint propagated from 0x7ffc369a7090 to 0x7ffc369a7090
From :%cmd to :%arraydecay5 
Taint detected in sensitive position: 0x7ffc369a7090!
That's the address in:%arraydecay5
This is an example txt file!total 264
drwxrwxrwx 1 root root   512 Nov 30 08:02 .
drwxrwxrwx 1 root root   512 Nov 26 12:10 ..
-rwxr-xr-x 1 root root 19616 Nov 30 08:02 ControlFlowHijack
-rwxrwxrwx 1 root root   605 Nov 26 12:08 ControlFlowHijack.cpp
-rw-r--r-- 1 root root 60332 Nov 30 08:02 ControlFlowHijack.dynamic.ll
-rw-r--r-- 1 root root 49436 Nov 30 08:02 ControlFlowHijack.ll
-rwxr-xr-x 1 root root 18968 Nov 30 08:02 InjectionAttack
-rwxrwxrwx 1 root root   420 Nov 26 12:08 InjectionAttack.cpp
-rw-r--r-- 1 root root 53797 Nov 30 08:02 InjectionAttack.dynamic.ll
-rw-r--r-- 1 root root 50311 Nov 30 08:02 InjectionAttack.ll
-rwxrwxrwx 1 root root   346 Nov 26 12:08 Makefile
-rw-r--r-- 1 root root    28 Nov 30 08:00 example.txt
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
You've discovered the secret value!
```

## 实验说明
### 被分析的程序
我们提供了两个待分析的程序：`InjectionAttack.cpp` 和 `ControlFlowHijack.cpp`。

在 `InjectionAttack.cpp` 中，当用户向 `/bin/cat` 输入所需的文件名参数时，如果添加了一些额外内容，则可以不受检查地运行其他命令。例如，如果用户输入 `example.txt ; ls -al`，命令字符串变为：`/bin/cat example.txt ; ls -al`。第一个命令（`/bin/cat example.txt`）被执行以显示 `example.txt` 的内容，然后第二个命令（`ls -al`）不受任何限制地执行。这允许攻击者在系统上执行任意命令，导致潜在的未授权访问或文件篡改。
```
char cmd[2048] = "/bin/cat ";
char filename[1024];
printf("Filename:");
scanf (" %1023[^\n]", filename); // 攻击者可以在此处注入 shell 转义符
strcat(cmd, filename);
system(cmd); // 警告：不可信数据被传递给系统调用
```
攻击示例：
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

在 `ControlFlowHijack.cpp` 中，当用户/黑客向 `mem.buffer` 写入数据而不检查缓冲区大小时，会覆盖其后的内容；在这种情况下，要成功劫持控制流，用户必须恰好输入 9 个字符，且第 9 个字符为 'A'。这会用值 65 覆盖 `mem.data`，导致 `secret_value` 被计算为 97。因此，用户的输入可以**意外地**影响程序的控制流，导致控制流劫持（正常情况下，`secret_value` 不会等于 97）。
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
    
    //模拟不安全的 gets
    char* ptr = mem.buffer;
    int ch;
    while ((ch = getchar()) != '\n') {
        *ptr++ = ch; 
    }
    *ptr = '\0';
    
    secret_value=mem.data+32;
    // 检查 secret_value
    if (check_secret(secret_value)) {
        printf("You've discovered the secret value!\n");
    } else {
        printf("Secret value: %d\n", secret_value);
    }
}
```
攻击示例：
```
/lab7/test$ ./ControlFlowHijack
input:AAAAAAAAA
You've discovered the secret value!
```


### 动态污点分析
污点分析由三个组件组成：`污点源`/`污点传播策略`/`污点汇聚点`

- 污点源

    污点源是程序中可能引入不可信或不安全数据的输入点。这些输入点可能是用户输入、文件读取、网络数据等。

    提示：在我们的两个示例中，仅使用了用户输入作为污点源，但在实际应用中，文件读取和网络数据传输更为常见。

- 污点传播策略

    这部分可以简单概括为：如果源操作数被污染，那么污点应该传递给目标操作数。
    
    例如，`%b = load i32, ptr %a, align 4` 从 `%a` 中的地址加载一个 i32 类型的数据到 `%b`，源操作数是 `%a`，目标操作数是 `%b`，那么当 %a 被污染时，%b 也需要被污染。

    除了普通指令外，一些函数调用也会进行污点传播，在我们的例子中，`strcat` 连接两个字符串，如果其中一个字符串被污染，那么污点也需要传播到连接后的结果。

- 污点汇聚点

    当到达敏感的程序位置/敏感的程序行为时，添加一个污点汇聚点来检查特定变量是否被污染。
    
    在我们的两个示例中，在调用 system/check_secret 之前，需要检查 system/check_secret 的参数变量是否被污染。



### 我们工具的特性
不同的污点分析工具在数据结构和污点处理方法上具有不同的特性。这里，我们声明我们工具的一些特性：

- 污点粒度

    本工具的污点粒度是变量和字节的混合：对于非指针变量，我们以变量为粒度进行追踪；对于指针变量，以字节为粒度进行追踪。

- 污点颜色

    在污点追踪中，污点颜色是一种用于识别和区分不同污点的属性。从污点的颜色可以推断其来源、类型或状态。然而，在我们的实现中，我们不追踪污点的来源或状态；我们只是区分两种状态：被污染或未被污染（即只有黑白两色）。

- 污点数据结构

    我们使用集合来存储污点信息。结合上述两个特性，在 `runtime.cpp` 中，你会找到两个集合 `taintedPtrVars` 和 `taintedVars`。对于一个非指针类型的变量，如果它的名字在 `taintedVars` 中，则该变量被视为被污染；对于一个指针类型的变量，如果它的运行时地址在 `taintedPtrVars` 中，则表示该变量被污染。
    
    一个更复杂的工具可能会使用诸如影子内存之类的数据结构，这里进行了简化。

- 支持的指令

    本工具不支持所有指令类型，仅支持其中的一个子集，包括 TruncInst、GEPInst、StoreInst、LoadInst、BinaryOperator。要创建一个更全面和通用的工具，需要支持所有指令类型。

- 对指针和非指针类型的不同处理

    这种区分的**必要性**：**在 IR 层面**，我们**无法**获取非指针变量在内存中的位置，而在二进制（汇编）层面，我们可以通过指令判断值在哪个寄存器/内存中。

    在区分指针和非指针类型之前，我们需要知道每个指令操作数的指针/非指针类型。这是每条指令的操作数类型：

    |指令|格式|目标类型|源类型|
    |:-:|:-:|:-:|:-:|
    |TruncInst|`%dest` = trunc **i32** `%src` to **i8**|int|int|
    |GetElementPtrInst|`%dest` = getelementptr **inbounds i8**, **ptr** `%src`, i32 1|ptr|ptr|
    |StoreInst|store **ptr/i8** `%src`, **ptr** `%dest`, align 8|ptr|ptr/int|
    |LoadInst|`%dest` = load **ptr/i8**, **ptr** `%src`, align 8|ptr/int|ptr|
    |BinaryOperator|`%dest` = add nsw **i32** `%src1`, **i32** `%src2`|int|int|
    
    因此，处理 StoreInst 的污点传播函数有两个版本：`StoreInstProcess` 和 `StoreInstProcessPtr`。类似地，在设置污点源（污点汇聚点）时，也会有两个版本：`TaintVal` (`CheckVal`) 和 `TaintPtrVal` (`CheckPtrVal`)。

    对于 LoadInst，由于其源操作数必须是指针，因此可以通过源操作数的地址判断是否需要污染，所以只有一个 Ptr 版本。

### TODO 列表：
在代码/技术实现方面，动态污点分析需要以下三个步骤：   
`1.` 开发插桩逻辑并将其打包为 LLVM pass；  
`2.` 使用该 pass 对目标程序的 IR 文件进行插桩，插入对运行时函数的调用；  
`3.` 将修改后的 IR 文件编译为可执行文件并运行。

因此，在本实验中，我们需要完成插桩逻辑以及被插入的运行时函数，你将有以下 TODO 列表：

- 在 `DynTaintAnalysisPass.cpp` 的主运行函数 `runOnFunction` 中，为各种指令和与污点源相关的函数（scanf, getchar）添加相应的插桩函数调用。这些插桩函数用于在特定位置插入运行时函数。
- 在 `Instrument.cpp` 中完成 `Trunc` 和 `Load` 指令的插桩函数。
- 在 `runtime.cpp` 中完成 `Store` 和 `BinaryOperator` 指令的运行时分析函数。

### 关于插桩
本实验中的插桩方法类似于 lab2 的动态分析 pass。如果你忘记了一些细节，请回顾 **lab2 教程**中的 [Instrumentation Pass](https://github.com/ecnu-sa-labs/ecnu-sa-labs/blob/ff8658063073a4aa46afa6552bd18c281b477baf/lab_manual/lab2.md#instrumentation-pass) 和 [Inserting Instructions into LLVM code](https://github.com/ecnu-sa-labs/ecnu-sa-labs/blob/ff8658063073a4aa46afa6552bd18c281b477baf/lab_manual/lab2.md#inserting-instructions-into-llvm-code)。

## 提交
完成实验后，通过提交并推送 lab7/ 下的更改来提交你的代码。具体来说，你需要提交对 `src/DynTaintAnalysisPass.cpp`、`src/Instrument.cpp` 和 `lib/runtime.cpp` 的更改。
```
   lab7$ git add src/DynTaintAnalysisPass.cpp src/Instrument.cpp lib/runtime.cpp
   lab7$ git commit -m "你的提交信息"
   lab7$ git push
```