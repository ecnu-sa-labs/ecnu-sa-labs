<!-- TOC --><a name="dynamic-symbolic-execution"></a>
# 动态符号执行

<!-- TOC start -->

- [动态符号执行](#dynamic-symbolic-execution)
   * [1. 实验目标](#1-实验目标)
   * [2. 开始之前](#2-开始之前)
      + [2.1 示例驱动](#21-示例驱动)
      + [2.2 局限性说明](#22-局限性说明)
      + [2.3 环境配置](#23-环境配置)
    * [3. 理解 `miniklee` 的工作流程](#3-理解-miniklee-的工作流程)
   * [4. 任务 1：实现符号化过程](#4-任务-1实现符号化过程)
      + [4.1 你需要了解的函数](#41-你需要了解的函数)
      + [4.2 小贴士](#42-小贴士)
      + [4.3 概念验证 (POC)](#43-概念验证-poc)
   * [5. 任务 2：解释 `Add` 和 `Sub` 的语义](#5-任务-2解释-add-和-sub-的语义)
      + [5.1 你需要了解的函数](#51-你需要了解的函数)
      + [5.2 小贴士](#52-小贴士)
      + [5.3 概念验证 (POC)](#53-概念验证-poc)
   * [6. 任务 3：解释 `Br` 的语义](#6-任务-3解释-br-的语义)
      + [6.1 你需要了解的函数](#61-你需要了解的函数)
      + [6.2 小贴士](#62-小贴士)
      + [6.3 概念验证 (POC)](#63-概念验证-poc)
   * [7. 提交要求](#7-提交要求)


<a name="0-attention"></a>
## 0. 注意事项

> 由于 Windows 和我们旧版 Docker 镜像的虚拟化机制不同，请注意以下调整

**如果你使用的是 Windows，请改用 WSL2。**

打开 PowerShell，下载 WSL2，并执行以下命令：

``` bash
wsl --install -d Ubuntu
```
（可选）在 PowerShell 中，将 Ubuntu 设置为默认操作系统：

``` bash
wsl --set-default Ubuntu
```
打开 WSL2，下载你的仓库，并使用 VSCode 打开它（使用 Windows 版本的 VSCode，无需单独下载）：

``` bash
lab8: code .
```
确保 VSCode 已安装 Docker 和 Dev Container 扩展，然后重新打开容器即可。

另外，一个潜在的问题是，我们当前容器中的 LLVM 版本过旧，不适合运行 MiniKLEE。为了解决这个问题，我添加了一个热补丁，在容器启动时**自动**从网络下载合适的 LLVM 版本。但是，对于**网络状况不佳**的同学，你最好切换到国内镜像，例如阿里云或清华镜像。切换镜像后，重新下载相应的 LLVM 版本。你可以选择重启容器让它**自动下载**，或者**手动下载**：

``` bash
lab8$ rm llvm.sh                        # 删除当前的 llvm.sh
lab8$ wget https://apt.llvm.org/llvm.sh # 从网络下载 llvm.sh
lab8$ chmod +x llvm.sh                  # 赋予执行权限
lab8$ ./llvm.sh 14 all                  # 执行以下载 LLVM 14 版本
```

> 对于实验准备的仓促以及可能带来的不便，我深表歉意。


<a name="1-objectives"></a>
## 1. 实验目标

在本实验中，你将实现一个动态符号执行 (DSE) 引擎 `miniklee`，具体包括：

1.  实现符号化过程
2.  实现一个基于执行生成测试 (EGT) 的符号执行框架。
    1.  解释 `Add` 和 `Sub` 指令的语义
    2.  解释 `Br` 指令的语义

<!-- TOC --><a name="2-getting-stared"></a>
## 2. 开始之前

<!-- TOC --><a name="21-motivating-example"></a>
### 2.1 示例驱动

我们提供了可执行文件 `refminiklee`（位于 `./` 目录下），方便你快速了解符号执行。

我们的示例程序如下。假设 `__builtin_trap()`（第 9 行）是程序中的 bug。程序首先将变量 `a` 设为符号值。然后，它遇到一个 `if` 分支（`if (a + 2 == 100 - 10)`）。当变量 `a` 的值被设为 88 时，可以触发这个假设的 bug。

``` c
   1 #include "Symbolic.h"
   2 
   3 int main() {
   4     int a;
   5     make_symbolic(&a, sizeof(a), "a");
   6 
   7     if (a + 2 == 100 - 10) {
   8         // 陷阱，a 必须为 88
   9         __builtin_trap();
  10     } else {
  11         // 应到达此处，a 可以赋值为任何不等于 88 的值
  12     }
  13 
  14     return 0;
  15 }
```

`refminiklee` 的工作流程如下：
1.  它（在第 5 行）将变量 `a` 在虚拟机内存中标记为符号值。
2.  接着，它检查 `if` 分支（第 7 行），将分支语句编码为约束条件（`a + 2 == 100 - 10`），然后将其与之前的路径约束（本例中没有）一起发送给约束求解器。
3.  然后，求解器判断这些约束的可行性，结果为“真分支和假分支都可行”。
4.  `miniklee` 随后分叉状态，并将新状态更新到状态池中。
5.  对于进入真分支的状态，`miniklee` 生成一个测试用例（带有 `error` 后缀），其中 `a = 88`；对于假分支，它生成一个测试用例（带有 `normal` 后缀），其中 `a` 是一个不等于 88 的随机值。

**以下展示了如何使用 `refminiklee` 对上述示例进行符号探索**：

1.  将待测程序编译为 LLVM IR，**不进行任何优化**

``` bash
$ clang -emit-llvm -g -O0 -S ./test/example.c -o ./test/example.ll -I./include
```

2.  使用 `refminiklee` 对生成的 LLVM IR（`./test/example.ll`）执行符号执行

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

3.  检查生成的测试用例

``` bash
$ cat result_1/test_case_1.error
a, 88
$ cat result_1/test_case_2.normal
a, -1634890980
```

`test_case_2.normal` 的值是随机生成的，只要它不等于 88 即可。

<!-- TOC --><a name="22-limitation"></a>
### 2.2 局限性说明

为简化起见，本实验中的输入程序假定**仅包含 C 语言的以下子集特性**：
-   所有值都是 32 位有符号整数（即没有浮点数、指针、结构体、枚举、数组等）
-   被测程序仅包含 `main` 函数，意味着没有函数调用
-   涉及符号变量的条件只能是相等或不相等
-   仅涉及以下操作：
    -   [Alloca](https://llvm.org/doxygen/classllvm_1_1AllocaInst.html)：在栈上分配内存的指令
    -   [Load](https://llvm.org/doxygen/classllvm_1_1LoadInst.html)：从内存读取的指令
    -   [Store](https://llvm.org/doxygen/classllvm_1_1StoreInst.html)：存储到内存的指令
    -   [Ret](https://llvm.org/doxygen/classllvm_1_1ReturnInst.html)：返回值并转移控制流的指令
    -   [Eq](https://llvm.org/doxygen/classllvm_1_1ICmpInst.html)：根据谓词比较操作数是否相等的指令
    -   [SLT](https://llvm.org/doxygen/classllvm_1_1ICmpInst.html)：根据谓词比较操作数是否小于的指令
    -   [Br](https://llvm.org/doxygen/classllvm_1_1BranchInst.html)：条件或无条件分支指令
    -   [Call](https://llvm.org/doxygen/classllvm_1_1CallInst.html)：抽象目标机器调用约定的指令
    -   [Add](https://codebrowser.dev/llvm/llvm/include/llvm/IR/Instruction.def.html#147)：对两个操作数执行整数加法并产生其和的指令
    -   [Sub](https://codebrowser.dev/llvm/llvm/include/llvm/IR/Instruction.def.html#149)：执行整数减法，从第一个操作数中减去第二个操作数，并产生结果的指令

<!-- TOC --><a name="23-setup"></a>
### 2.3 环境配置

要构建你自己的 `miniklee`，只需在 `lab8` 目录下输入 `make`：

``` bash
$ make
```

然后你会在当前目录下得到你当前版本的 `miniklee`，它目前只能测试简单的测试用例（例如 `test/sequential.c`）：

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

你可以在终端日志中看到生成了一个测试用例（例如 `-2082133583`，一个随机生成的值）。但是，`result_1/test_case_1.error` 中没有赋值，因为 `sequential.c` 中没有符号变量。换句话说，`sequential.c` 没有接收任何输入。

你的第一个任务是使变量变为符号变量（在第 4 节中）。

<!-- TOC --><a name="3-understanding-the-workflow-of-miniklee"></a>

## 3. 理解 `miniklee` 的工作流程

-   `src/main.cpp`

`main` 函数接收一个 LLVM IR 文件并解析它，然后将控制流转移给符号执行器。

-   `src/Executor.cpp:runFunctionAsMain`

准备工作完成后，`miniklee` 在 `runFunctionAsMain` 函数中执行符号执行：

1.  它从一个初始状态开始，并使用一个状态池 (`states`) 进行存储。
2.  在主循环中，如果状态池不为空，则继续：
    1.  选择一个状态来执行。
    2.  获取要执行的下一条指令。
    3.  执行该指令并进行分析。
    4.  通过添加新状态和移除已终止状态来更新状态池。
3.  当一个状态到达终止点时，它会为该状态生成一个测试用例。

-   `src/Executor.cpp:executeInstruction`

重点关注指令执行部分，这部分主要是一个大型的 `switch-case` 语句。它识别并解释获取到的指令。例如：
-   解释 `Ret` 指令会终止当前状态。
    -   `src/Executor.cpp:terminateState`
        -   为当前状态生成测试用例
        -   移除即将结束的状态
-   解释 `Br` 指令会将控制权转移到下一个基本块。
    -   `src/Executor.cpp:fork`
        -   调用求解器判断当前分支条件的可行性
        -   根据求解器的结果分叉状态
-   解释 `Add` 指令会对两个操作数执行整数加法。


<!-- TOC --><a name="4-task-1-implementing-the-process-of-symbolization"></a>
## 4. 任务 1：实现符号化过程

如第 3 节所述，`miniklee` 根据相应的语义解释每条指令。有了解释方法，我们就可以设计如何解释符号化的语义并实现它。

**实现 `src/Executor.cpp:executeMakeSymbolic` 函数。我们提供了标有 “Task 1: Your Code Here” 的代码框架。**

<!-- TOC --><a name="41-functions-you-need-to-know"></a>
### 4.1 你需要了解的函数

-   `include/Symbolic.h:make_symbolic`

我们已经在 `include/Symbolic.h` 中提供了符号化的函数声明：

```c++
void make_symbolic(int32_t *sym, size_t nbytes, const char *name);
```

`make_symbolic` 函数接收三个参数：
1)  要符号化的变量的地址
2)  变量的字节数
3)  赋予变量的名称。

我们可以在被测程序中使用它，如下面的代码片段所示。实际上，KLEE 符号执行器也使用这种方法将变量标记为符号变量（KLEE 官方文档 [Marking input as symbolic](https://klee-se.org/tutorials/testing-function/)）。

``` c++
1: int a;
2: make_symbolic(&a, sizeof(a), "a"); // 变量 a 现在应该被符号化
3: if (a + 2 == 100 - 10) { ...  }
```

-   `src/Executor.cpp:executeInstruction`

请关注 `Call` 指令的 case：当 `miniklee` 执行 `make_symbolic` 语句（代码片段 1 中的第 2 行）时，它会识别并处理符号化。

-   `src/Executor.cpp:needsSymbolization`

识别当前调用是否为 `make_symbolic` 函数。

-   `src/Executor.cpp:processMakeSymbolic`

解析 `make_symbolic` 的参数，并继续将它们符号化。

<!-- TOC --><a name="42-tips"></a>
### 4.2 小贴士

**提示 1：阅读友好的代码**
-   阅读 `Load` 和 `Store` 指令的代码解释（`src/Executor.cpp:executeInstruction`）可能有助于理解 `miniklee` 的存储系统（**特别是函数 `include/Executor.h:executeMemoryOperation`**）。
-   阅读建模指令的代码解释（`include/Expr.h`）可能有助于理解符号执行中每个数据的表示方式。

**提示 2：深入理解程序的表示形式**

我们从不同的角度表示程序，例如：
-   **源代码**
    这是程序的具体语法，其优点是易于阅读，但难以转换、分析和优化。下面是一个计算存储在 `value` 中的数字（本例中为 `8`）的阶乘的例子。
 ``` c++
 int value = 8;
 int result = 1;
 for (int i = 1; i <= value; ++i) {
     result *= i;
 }
 std::cout << result << std::endl;
 ```

 -   **抽象语法树 (AST)**
 ```
       program
             |
         block
      /      |     \
  alloc load store ...
 ```

   [抽象语法树](https://en.wikipedia.org/wiki/Abstract_syntax_tree) (AST) 表示有利于编译器，因为它简化了解析和分析。对于解释器来说，它特别有用——递归地处理树，通过解释每条语句并更新程序的环境，可以轻松地评估代码块。AST 的主要缺点是其节点类型的行为多样。编写编译器分析需要不断处理这些节点类型之间的语义差异，这可能很繁琐。存在更适合实现复杂编译器优化的替代表示形式。这些方法使表示更加规则，简化了过程。

 -   **指令**（特别是 LLVM IR）
   LLVM IR 是静态[单赋值](https://en.wikipedia.org/wiki/Static_single_assignment_form) (SSA) 形式，这在 [LLVM IR 讲座](https://tingsu.github.io/files/courses/slides/lec-2-llvm-framework-primer.pdf) 中已经解释过。我们喜欢指令表示，因为它具有规则性。然而，要利用它做任何有用的事情，我们需要提取更高级别的表示（**`miniklee` 正是在基本块和指令级别上工作的**）：
   -   [控制流图](https://en.wikipedia.org/wiki/Control-flow_graph) (CFG)。
   -   [基本块](https://en.wikipedia.org/wiki/Basic_block)。
   -   终结指令（此处为 jmp 和 br）。
   -   推导形成基本块的算法。
   -   后继与前驱。
   -   推导形成基本块 CFG 的算法。

   我们欣赏 LLVM IR，特别是它的 SSA（静态单赋值）特性：每个变量在全局范围内只有一个静态赋值。然而，这并不意味着动态单赋值，因为同一条语句可以执行多次。总结一下，在 LLVM IR 代码中：
   -   定义 == 变量
   -   指令 == 值
   -   参数 == 数据流图边

**提示 3：“太长不看；我只想躺平。”**

有这种感觉很正常。一步一步来，你已经做得很棒了！💪。

让我们将符号化的步骤分解如下：
1.  使用提供的类型 `SymbolicExpr` 表示符号值。
2.  使用 `executeMemoryOperation` 将创建的符号值存储到符号内存中。

答案如下：

``` c++
void Executor::executeMakeSymbolic(ExecutionState& state,
                                   Instruction *symAddress,
                                   std::string name) {
    // 将要符号化的变量（Instruction 类型）注册到符号内存中
    executeMemoryOperation(state, true, symAddress, 
                           SymbolicExpr::create(name), 0);
}
```


<!-- TOC --><a name="43-proof-of-concept-poc"></a>
### 4.3 概念验证 (POC)

实现任务 1 后，运行测试用例 `test/symbolic.c` 来验证你的更改。这个测试引入了符号变量，让你可以观察符号执行引擎如何处理它们。检查生成的测试输入，确保符号输入被识别并正确处理。

预期结果：

``` bash
$ make clean
$ make
... 编译日志省略 ...
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
现在你可以看到 `result_1/test_case_1.error` 中为符号值 `a`（假定为程序输入）分配了一个值（此处 `-399613364` 是随机生成的）。

<!-- TOC --><a name="5-task-2-interpreting-the-semantics-of-add-and-sub"></a>
## 5. 任务 2：解释 `Add` 和 `Sub` 的语义

如第 3 节所述，`miniklee` 根据相应的语义解释每条指令。有了解释方法，我们就可以设计如何解释 `Add` 和 `Sub` 指令的语义并实现它们。

**实现 `Add` 和 `Sub` 的语义解释（`src/Executor.cpp:executeInstruction`）。我们提供了标有 “Task 2: Your Code Here” 的代码框架。**

<!-- TOC --><a name="51-functions-you-need-to-know"></a>
### 5.1 你需要了解的函数

-   `src/Executor.cpp:executeInstruction`

请关注 `Add` 和 `Sub` 指令的 case，当 `miniklee` 执行 `Add` 或 `Sub` 语句时，它会识别并处理该指令。

<!-- TOC --><a name="52-tips"></a>
### 5.2 小贴士

**提示 1：“太长不看；我只想躺平。”**

阅读 KLEE 友好的源代码以获得灵感。
-   [KLEE 处理 `Add` 的代码](https://github.com/klee/klee/blob/master/lib/Core/Executor.cpp#L2582)
-   [KLEE 处理 `Sub` 的代码](https://github.com/klee/klee/blob/master/lib/Core/Executor.cpp#L2589)

<!-- TOC --><a name="53-proof-of-concept-poc"></a>
### 5.3 概念验证 (POC)

实现任务 2 后，运行测试用例 `test/example.c` 来验证你的更改。这个测试引入了 `add` 操作、`sub` 操作和分支条件，让你可以观察 `miniklee` 如何处理它们。

`example.c` 的内容

``` c++
#include "Symbolic.h"
int main() {
    int a;
    make_symbolic(&a, sizeof(a), "a");

    if (a + 2 == 100 - 10) {
        // 陷阱，a 必须为 88
        __builtin_trap();
    } else {
        // 应到达此处，a 可以赋值为任何不等于 88 的值
    }
    
    return 0;
}
```

运行测试

``` bash
$ make clean
$ make
... 编译日志省略 ...
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

看起来不错。但有些问题：`miniklee` 探索了真分支，并为符号变量 `a` 分配了一个不会触发崩溃的值（`-732525800` 是随机生成的，而正确的值应该是 `88`）。

**为什么？这是因为分支指令没有被正确处理。** 在当前版本中，当 `miniklee` 遇到分支指令时，它简单地认为条件为真，并探索真分支（请阅读 `src/Executor/cpp:executeInstruction` 中处理 `Br` 指令的 switch-case 源代码）。

你的任务 3 就是正确处理它。

<!-- TOC --><a name="6-task-3-interpreting-the-semantics-of-br"></a>
## 6. 任务 3：解释 `Br` 的语义

**实现 `Br` 的语义解释（`src/Executor.cpp:executeInstruction` 和 `src/Executor.cpp:fork`）。我们提供了标有 “Task 3: Your Code Here” 的代码框架。**

分支处理是执行生成测试 (EGT) 符号执行器的核心。在继续之前，让我们回顾一下[它的背景](https://dl.acm.org/doi/10.1145/2408776.2408795)。

目前有两种类型的符号执行：

-   **Concolic Testing（混合符号执行）**：从随机输入开始执行，执行后，使用当前路径的路径条件 pc₀（通过否定最后一个条件）构造一个新的路径条件 pc₁，并求解 pc₁ 以获得新的输入 I₁ 来探索新路径，然后重复该过程。如 [CREST](https://www.burn.im/crest/) 和 [DART](https://dl.acm.org/doi/10.1145/1064978.1065036) 中所示。
_我们之前的实验属于这种类型，[MIT 6.858: Computer System Security Lab 3: Symbolic Execution](https://css.csail.mit.edu/6.858/2022/labs/lab3.html) 也是如此。_

-   **Execution-Generated Testing (EGT，执行生成测试)**：在每个条件分支处分叉符号执行（如果两个方向都可行），维护多个部分路径，并协调它们同时执行。如 [EXE](https://dl.acm.org/doi/10.1145/1455518.1455522)、[SPF](https://dl.acm.org/doi/10.1145/1858996.1859035) 和 [KLEE](https://dl.acm.org/doi/10.5555/1855741.1855756) 中所示。
_这是 `miniklee` 实现的类型。_

如上所述，EGT 执行器在每个条件分支处，如果两个方向都可行，则会分叉其执行。你的任务是实现用于解释 `Br` 指令的 `fork` 函数。

<!-- TOC --><a name="61-functions-you-need-to-know"></a>
### 6.1 你需要了解的函数

-   `src/Executor.cpp:executeInstruction`

-   `src/Executor.cpp:transferToBasicBlock`

更新程序计数器，将执行状态转移到指定基本块的起始位置。

-   `src/Executor.cpp:getInstructionValue`

获取相应指令定义的符号值。

<!-- TOC --><a name="62-tips"></a>
### 6.2 小贴士

**提示 1：理解 `miniklee` 中执行 `Br` 的工作流程**

当执行器处理 `Br` 指令时，它首先将其转换为相应的 `Br` 类型（[讲座：LLVM 框架，第 37 页](https://tingsu.github.io/files/courses/slides/lec-2-llvm-framework-primer.pdf)）。然后，它使用 API（[BranchInstr 的 isUnconditional 方法](https://llvm.org/doxygen/classllvm_1_1BranchInst.html#a7e4be8b16fbd68c9045a388904044e01)）来确定分支是无条件还是条件分支，并分别处理每种情况：

-   **无条件**：将执行状态转移到下一个基本块。
-   **条件分支**：提取条件并将其发送给求解器以检查可行性：
    -   **真分支可行**：转移到真分支的基本块。
    -   **假分支可行**：转移到假分支的基本块。
    -   **两个分支都可行**：为每个分支分叉出新状态，将控制权转移到各自的基本块，并将分叉出的状态添加到状态池中。

**提示 2：理解 `fork` 函数的工作流程**

为了减轻实验的痛苦，我们已经为 `fork` 函数（`src/Executor.cpp:fork`）提供了一个框架，`fork` 的工作方式如下：

-   **调用求解器判断分支的可行性**：它首先调用求解器，检查在给定查询（路径约束下的真分支和假分支）下，是否存在使条件为真的有效赋值。明确地说，如果真分支可行，我们说 `trueBranch` 设为真；否则为假。类似地，如果假分支可行，`falseBranch` 设为真；否则为假。你可以按如下方式实现这部分：

``` c++
    // 调用求解器判断条件的可行性
    bool trueBranch = solver->evaluate(
        Query(current.constraints, condition)
    );
    bool falseBranch = solver->evaluate(
        Query(current.constraints, NotExpr::create(condition))
    );
```

 -   **根据求解器的结果执行操作。**
   -   **`trueBranch` 为真，`falseBranch` 为假**
     -   直接返回当前状态作为 `StatePair` 的第一个位置
   -   **`falseBranch` 为真，`trueBranch` 为假**
     -   直接返回当前状态作为 `StatePair` 的第二个位置
   -   **`trueBranch` 和 `falseBranch` 都为真**
     -   克隆当前状态（_阅读 `src/ExecutionState.cpp` 中 `ExecutionState` 提供的 API 可能会有所帮助_）
     -   将新克隆的状态添加到状态池（_将其推送到全局变量 `addedStates` 即可；不要直接操作状态池 `states`_）
     -   将当前条件添加到当前状态（进入真分支的状态），并将取反后的条件添加到克隆状态（进入假分支的状态）
     -   将两个状态作为 `StatePair` 返回

**提示 3：“太长不看；我只想躺平。”**

阅读 KLEE 友好的源代码以获得灵感。
-   [KLEE 处理 `Br` 的代码](https://github.com/klee/klee/blob/master/lib/Core/Executor.cpp#L2218)
-   [KLEE fork 函数代码](https://github.com/klee/klee/blob/master/lib/Core/Executor.cpp#L1039)

<!-- TOC --><a name="63-proof-of-concept-poc"></a>
### 6.3 概念验证 (POC)
实现任务 3 后，运行测试用例 `test/example.c` 来验证你的更改。你现在应该看到与运行 `refminiklee` 相同的结果（除了随机生成的值）。请根据 [2.1 示例驱动](#21-示例驱动) 进行自我检查。

<!-- TOC --><a name="7-submission"></a>
## 7. 提交要求

完成实验后，通过提交并推送 lab8/ 目录下的更改来提交你的代码。具体来说，你需要提交对 `src/Executor.cpp` 的更改。

``` bash
   lab8$ git add src/Executor.cpp
   lab8$ git commit -m "你的提交信息"
   lab8$ git push
```