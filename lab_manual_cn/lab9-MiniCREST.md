# 动态符号执行

使用 LLVM 和 Z3 为 C 程序构建动态符号执行引擎。

## 目标

在本实验中，你将实现一个动态符号执行（DSE）引擎，该引擎能自动生成输入，以高效地探索不同的程序路径。
你将使用一个 LLVM pass 将 C 程序编码为我们提供的符号解释 API。
最终的工具将为输入变量找到能导致输入 C 程序崩溃的赋值。

本实验分为三个部分：
1. 在 `src/DSEInstrument.cpp` 中完成插桩函数。
2. 使用 Z3 的 C++ API，在 `src/Runtime.cpp` 中编写动态符号解释的约束逻辑。
3. 在 `src/Strategy.cpp` 中实现一个用于探索新程序路径的回溯搜索算法。

## 环境搭建

实验 7 的骨架代码位于 `/lab7` 目录下。
在描述实验的文件位置时，我们会经常将实验 7 的顶层目录称为 `lab7`。

以下命令用于搭建实验环境：

```sh
/lab7$ mkdir build && cd build
/lab7/build$ cmake ..
/lab7/build$ make
```

现在你应该能在当前目录（lab7/build）下看到 `dse` 和 `InstrumentPass.so`。

`dse` 是一个使用 Z3 对输入程序执行动态符号执行的工具。
你可以使用以下命令运行 dse 程序：

```sh
/lab7$ cd test
/lab7/test$ make simple0
/lab7/test$ ../build/dse ./simple0 N           # 其中 N 是迭代次数
/lab7/test$ timeout 10 ../build/dse ./simple0  # 运行 10 秒
```

初始时，你会看到 `formula.smt2` 未找到的错误，因为你还没有实现插桩部分。

## 输入程序的格式

本实验中的输入程序假定只包含 C 语言的以下子集特性：

-   所有值都是整数（即，没有浮点数、指针、结构体、枚举、数组等）。
    你可以忽略其他类型的值。
-   假定用户输入仅通过 `DSE_Input` 函数引入，并且不存在对其他函数的调用指令。

## 示例输入和输出

你的 DSE 引擎应该能在给定的已插桩程序上运行。
例如，以下命令将在 1 次迭代后找到一个崩溃输入，并将输入存储在 `input.txt` 中：

```sh
/lab7$ cd test
/lab7/test$ make
/lab7/test$ ../build/dse ./simple0 5
Floating point exception
Crashing input found (1 iters)
/lab7/test$ cat input.txt
X0,1024
```

## 实验指导

*动态符号执行*（DSE）结合了随机测试和符号执行的技术，用于搜索程序所有执行路径中的错误。
DSE 同时跟踪运行时值和符号约束，并在程序计算树上的回溯搜索过程中，利用前者来简化后者的求解。

我们提供了使用 [Z3](https://github.com/Z3Prover/z3) 构建的符号解释器框架。
你需要将 C 程序编码到这个符号解释器 API 中，并编写驱动动态符号执行的代码。
在接下来的章节中，我们将提供关于如何执行此操作的详细信息。

本实验假设输入程序只包含整数变量（没有指针或其他类型的变量），并且没有函数（没有 `CallInst`）。

### 理解 Z3

Z3 是微软开发的一个定理证明器。
它是一个庞大而复杂的工具，因此以下内容将作为其功能和能力的简要指南。
考虑一个简单的通用方程组，例如以下，其中 `X` 和 `Y` 是整数：

```
X < Y
X > 2
```

虽然这个例子很简单，但请思考一下如何使用你选择的任何编程语言来解决它。
你可能会求助于使用循环来检查数字，或者寻找一个库来处理矩阵乘法。
这是因为大多数编程语言都是命令式的，这意味着需要一系列命令来解决问题。

另一方面，Z3 具有声明式接口，在这种情况下，这意味着你只需要给它提供约束列表（在本例中为 `X < Y` 和 `X > 2`）。
将以下内容输入[在线 Z3 求解器](https://compsys-tools.ens-lyon.fr/z3/index.php)以查看结果：

```
(declare-const x Int)
(declare-const y Int)
(assert (< x y))
(assert (> x 2))
(check-sat)
(get-model)
```

Z3 可能不会给出所有符合约束的可能结果，但重要的是，它验证了可满足性，这是本 DSE 引擎将利用的关键因素。

如果你对 Z3 感到好奇并想了解更多信息，可以查看以下资源：

-   [https://github.com/Z3Prover/z3/wiki/Documentation](https://github.com/Z3Prover/z3/wiki/Documentation)
-   [https://github.com/Z3Prover/z3/blob/master/examples/c%2B%2B/example.cpp](https://github.com/Z3Prover/z3/blob/master/examples/c%2B%2B/example.cpp)
-   [https://theory.stanford.edu/\~nikolaj/programmingz3.html](https://theory.stanford.edu/~nikolaj/programmingz3.html)

### 第一部分：LLVM 插桩

此动态符号执行实现的第一个组件是对输入程序的插桩，这在 `src/DSEInstrument.cpp` 中完成。
这遵循了先前实验中熟悉的格式和模式，不同之处在于，此 LLVM pass 将注入 `src/Runtime.cpp` 中定义的各种函数，并附带来自每个有效 LLVM 指令的适当元数据。
这将使 DSE 能够在运行时与 Z3 交互。
具体来说，以下是需要插桩的函数（来自 `include/DSEInstrument.h`）：

```cpp
static const char *DSEInitFunctionName = "__DSE_Init__";
static const char *DSEAllocaFunctionName = "__DSE_Alloca__";
static const char *DSEStoreFunctionName = "__DSE_Store__";
static const char *DSELoadFunctionName = "__DSE_Load__";
static const char *DSEConstFunctionName = "__DSE_Const__";
static const char *DSERegisterFunctionName = "__DSE_Register__";
static const char *DSEICmpFunctionName = "__DSE_ICmp__";
static const char *DSEBranchFunctionName = "__DSE_Branch__";
static const char *DSEBinOpFunctionName = "__DSE_BinOp__";
```

#### 符号输入

骨架代码提供了一个名为 `DSE_Input` 的辅助函数，供用户指定符号输入。
在目标程序中，你应该首先包含头文件 `include/Runtime.h` 才能使用该函数。
在以下示例代码中，动态符号执行引擎会将变量 `x` 和 `y` 视为具有符号输入，而 `z` 具有具体值 0：

```cpp
#include "../include/Runtime.h"

int main() {
    int x, y, z;
    DSE_Input(x);
    DSE_Input(y);
    z = 0;
    ...
}
```

请注意，`DSE_Input` 是一个宏，会展开为带有唯一 ID 的形式。
有关详细信息，请参阅 `include/Runtime.h` 和 `src/SymbolicInterpreter.cpp`。

初始时，DSE 引擎会为输入变量分配随机数。
在 DSE 的每次迭代之后，会生成新的输入，并以逗号分隔值（CSV）的形式存储在文件 input.txt 中。
该文件将包含从 ID 到其整数值的映射。
以下是符号映射 `{X0 : 1, X1 : 10}` 的示例：

```
X0,1
X1,10
```

如果存在 `input.txt` 文件，则使用以下方法插桩的目标程序将使用这些整数值作为输入，而不是随机数。

#### 步骤 1：DSE 初始化的插桩

你首先需要插桩输入程序，使其在 main 函数的开头调用 `__DSE_Init__` 函数。
骨架代码在 `src/SymbolicInterpreter.cpp` 中提供了 `__DSE_Init__` 的定义。
该函数在 `input.txt` 存在时初始化输入，并注册一个回调函数 `__DSE_Exit__`，该函数将在目标程序正常终止时被调用。
骨架代码还提供了 `__DSE_Exit__` 的定义，用于存储已覆盖的分支列表（在 `branch.txt` 中）、路径公式（在 `formula.smt2` 中）和日志（在 `log.txt` 中）。
简而言之，你的插桩模块应将左侧的代码转换为右侧的代码：

<table>
<tbody>
<tr valign="top">
<td>
<pre><code>
define dso_local i32 @main() #0 {
entry:
    %retval = alloca i32, align 4
    ...
</pre></code>
</td>
<td>
<pre><code>
define dso_local i32 @main() #0 {
entry:
    call void @__DSE_Init__();
    %retval = alloca i32, align 4
    ...
</pre></code>
</td>
</tr>
</tbody>
</table>

#### 步骤 2：IR 指令的插桩

接下来，你将插桩其余的 IR 指令。
通常，如果指令中的某个操作数会改变符号内存状态，则应对其进行插桩。
常量使用 `__DSE_Const__` 函数进行插桩，寄存器使用 `__DSE_Register__` 函数进行插桩（有关详细信息，请参阅下一节）。
此外，`Alloca` 指令的插桩函数调用必须出现在指令*之后*，而所有其他指令的插桩函数调用必须出现在指令*之前*。
`__DSE_ICmp__` 和 `__DSE_BinOp__` 将左侧寄存器的 ID 作为其第一个参数，将其 LLVM 操作码（分别为 `llvm::CmpInst::Predicate` 和 `llvm::Instruction::BinaryOps`）作为第二个参数。
我们提供了一些示例插桩（为便于阅读，函数调用已简化）：

<table>
<tbody>
<tr valign="top">
<td>
<pre><code>
...
  %x = alloca i32, align 4
...
</pre></code>
</td>
<td>
<pre><code>
...
  %x = alloca i32, align 4 
  __DSE_Alloca__(i32 1, i32* %x)
...
</pre></code>
</td>
</tr>
<tr valign="top">
<td>
<pre><code>
...
  store i32 0, i32* %retval, align 4
...
</pre></code>
</td>
<td>
<pre><code>
...
  __DSE_Const__(i32 0)
  __DSE_Store__(i32* %retval)
  store i32 0, i32* %retval, align 4
...
</pre></code>
</td>
</tr>
<tr valign="top">
<td>
<pre><code>
...
  %cmp = icmp eq i32 %1, 1024
...
</pre></code>
</td>
<td>
<pre><code>
...
  __DSE_Register__(i32 5)
  __DSE_Const__(i32 1024)
  __DSE_ICmp__(i32 6, i32 32)
  %cmp = icmp eq i32 %1, 1024
...
</pre></code>
</td>
</tr>
</tbody>
</table>

### 第二部分：运行时符号解释

本实验的第二个组件涉及在 `src/Runtime.cpp` 中编写运行时符号解释函数。
在之前的实验中，插桩函数已经提供，但这次你将自己实现。
当这些函数在运行时被调用时，它们会改变符号内存状态和路径条件。
在这里，你将使用 Z3 API 为符号解释器类添加约束。

#### LLVM 指令的符号解释

你将定义每个 LLVM 指令的符号操作函数，并插桩输入程序以在运行时调用这些函数。
跟随程序的真实执行，DSE 引擎操作一个符号内存状态。
`include/SymbolicInterpreter.h` 中的 `SymbolicInterpreter` 类维护符号内存，该内存定义为从符号地址到符号表达式的映射。
它还维护一个符号表达式栈。

`Address` 类的实例表示一个符号内存地址。
符号地址可以是内存地址或寄存器，遵循 LLVM IR 的定义。
`Type` 字段表示地址的类型。
对于内存地址（通过 LLVM 的 `AllocaInstruction` 分配），我们将使用它们的物理地址作为符号地址。
对于寄存器，我们将通过 `DSEInstrument.h` 中的 `getRegisterID()` 分配唯一的寄存器 ID。
对于符号表达式，你将重用 Z3 的表达式，即 `z3::expr` 类的实例。

具体执行过程的符号操作使用两个辅助函数 `__DSE_Const__` 和 `__DSE_Register__` 执行，每个函数将具体常量和寄存器编码为它们的符号对应物。
这些函数在 `src/SymbolicInterpreter.cpp` 中定义。
函数 `__DSE_Const__` 接收 LLVM IR 的一个常量整数，为该数字创建一个符号表达式，并将该符号表达式压入栈（类 `SymbolicInterpreter` 中的字段 `Stack`）。
函数 `__DSE_Register__` 接收一个 LLVM 寄存器的 ID，并将其符号对应物压入栈。
栈中的每个元素要么是常量，要么是寄存器。
栈中的符号表达式将用于后续的插桩函数。

你将使用辅助函数定义 LLVM 指令的符号操作函数。
考虑以下 LLVM 代码，它等价于一个简单的 C 程序 `int x = 1; int y = x;`（为简单起见省略了类型）：

| 插桩后的代码            | 具体内存       | 栈            | 符号内存            |
| ---------------------- | --------------- | ------------ | ----------------- |
| `%x = alloca`          | `%x : 0x1000`   |              |                   |
| `__DSE_Alloca__(0,%x)` |                 | `[]`         | `Reg(0) : 0x1000` |
| `%y = alloca`          | `%y : 0x1004`   |              |                   |
| `__DSE_Alloca__(1,%y)` |                 | `[]`         | `Reg(1) : 0x1004` |
| `__DSE_Const__(1)`     |                 | `[Const(1)]` |                   |
| `__DSE_Store__(%x)`    |                 | `[]`         | `0x1000 : 1`      |
| `store 1, %x`          | `0x1000 : 1`    |              |                   |
| `__DSE_Load__(2,%x)`   |                 | `[]`         | `Reg(2) : 1`      |
| `%a = load %x`         | `%a : 1`        |              |                   |
| `__DSE_Register__(2)`  |                 | `[Reg(2)]`   |                   |
| `__DSE_Store__(%y)`    |                 | `[]`         | `0x1004 : 1`      |
| `store %a, %y`         | `0x1004 : 1`    |              |                   |

-   `__DSE_Alloca__` 接收左侧寄存器的 ID 和新分配的物理内存块的地址。
    在上面的例子中，`%x` 的 ID 是 0，物理内存地址是 0x1000。
    第 2 行之后的符号内存将包含条目 `Reg(0) : 0x1000`。
-   `__DSE_Store__` 假定栈顶存在其值操作数（常量或寄存器）的符号表达式。
    它接收一个物理内存地址作为参数，并将该符号表达式存储在该地址。
-   `__DSE_Load__` 接收左侧寄存器的 ID 和物理内存块的地址，该内存块的值将被加载到该寄存器中。

其他符号操作函数的行为以类似方式定义。
`__DSE_ICmp__` 和 `__DSE_BinOp__` 接收左侧寄存器的 ID 及其 LLVM 操作码（分别为 `llvm::CmpInst::Predicate` 和 `llvm::Instruction::BinaryOps`）。
骨架代码在 `SymbolicInterpreter.cpp` 中提供了 `__DSE_Branch__` 的实现作为参考。

#### 使用 Z3 表达式

像 `llvm::Inst::CmpInst` 和 `llvm::BinaryOperator` 这样的指令操作符号，需要在约束中等价地表示。
你将使用 Z3 表达式来表示这些操作。
Z3 API 使用了 C++ 的一个特性，称为[运算符重载](https://en.cppreference.com/w/cpp/language/operators)，允许你对 `z3::expr` 类型的对象使用 C++ 算术和比较运算符。
下面我们展示一些示例，用于表示对 `z3::expr` 对象的算术和比较表达式。
这些示例假设 E1 和 E2 是两个 `z3::expr` 类型的对象，它们的结果存储在另一个 `z3::expr` 类型的对象 `E` 中。

| 操作     | 表示方式       |
| --------- | --------------- |
| 加法      | `E = (E1 + E2)` |
| 小于      | `E = (E1 < E2)` |

### 第三部分：回溯策略

回想一下“示例 1：组合方法”讲座视频中，为了 DSE 分析能够探索输入测试程序的更多路径，是如何处理条件的。
修改 `src/Strategy.cpp` 中的 `searchStrategy()` 函数以执行此回溯行为。
它应该修改当前将提供给 Z3 的路径公式，以便推导出新的输入。

#### 路径公式和搜索策略

在每次执行已插桩的程序后，路径公式将被编码并存储在 `formula.smt2` 中。
所有已执行分支指令的 ID 将按执行顺序存储在 `branch.txt` 中，这可能有助于生成下一个输入。
给定当前可满足的路径公式，函数 `searchStrategy` 将提出一个公式，用于推导出能够探索更多路径的新输入。
`DSE.cpp` 中的 main 函数将迭代地生成新输入，直到找到崩溃输入或发生超时。

## 提交

完成实验后，你可以使用以下命令创建 `submission.zip` 文件：

```sh
lab7$ make submit
...
submission.zip created successfully.
```

然后将提交文件上传到助教的邮箱。