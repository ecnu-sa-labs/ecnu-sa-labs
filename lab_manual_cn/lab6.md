# 构建静态分析器（数据流分析）

为 C 语言的子集（包含分支和循环）构建一个"除零"静态分析器。

## 目标

在本实验中，你将构建一个静态分析器，能够在编译时检测 C 程序中潜在的除零错误。

你将通过编写一个 LLVM pass 来实现这一目标。

由于为像 C 这样的完整语言开发静态分析器是一项艰巨的任务，本实验将分为两部分。

### 第一部分

1. 实现 `DivZeroAnalysis::check`，用于检查给定指令是否可能导致错误。
2. 实现 `src/Transfer.cpp` 中的 `DivZeroAnalysis::transfer`。
3. 通过补全提供的函数存根，实现 `src/Transfer.cpp` 中的 `eval` 函数。

### 第二部分

在本实验的第二部分，你将实现 `src/ChaoticIteration` 中的各种函数。

1. 实现 `doAnalysis` 函数，该函数为你的分析执行混沌迭代算法。
2. 实现 `flowIn` 函数，该函数合并所有传入流的 out 内存。
3. 实现 `flowOut` 函数，该函数更新 out 内存，并根据需要将所有传出流加入 `WorkSet`。
4. 实现 `join` 函数，该函数取两个 Memory 对象的并集，并考虑 Domain 值。
5. 实现 `equal` 函数，该函数检查两个 Memory 对象是否相等，并考虑 Domain 值。

## 设置

实验6的骨架代码位于 `lab6/` 目录下。

### 步骤 1

以下命令使用之前见过的 [CMake][Cmake ref]/[Makefile][Make ref] 模式来设置实验。

需要注意的一点是使用了 `-DUSE_REFERENCE=ON` 标志：
本实验包含两部分，这个标志将允许你独立于第二部分，专注于第一部分所需的功能。

```sh
/lab6$ mkdir build && cd build
/lab6/build$ cmake -DUSE_REFERENCE=ON ..
/lab6/build$ make
```

在生成的文件中，你现在应该能在 `lab6/build` 目录下看到 `DivZeroPass.so`。

我们现在准备在一个示例输入 C 程序上运行我们的基础实验。

### 步骤 2

在测试程序上运行 pass 之前，我们需要为其生成 LLVM IR 代码。

`clang` 命令从输入 C 程序 `test04.c` 生成 LLVM IR 程序。

`opt` 命令优化该 LLVM IR 程序，并生成一个等效的 LLVM IR 程序，该程序更易于你将在本实验中构建的分析器处理。
特别是，`-mem2reg` 选项将每个 [AllocaInst][LLVM AllocaInst] 提升为寄存器，使你的分析器在本实验中无需处理指针。

```sh
/lab6/test$ clang-19 -emit-llvm -S -fno-discard-value-names -Xclang -disable-O0-optnone -c -o test04.ll test04.c
/lab6/test$ opt-19 -passes="mem2reg" -S test04.ll -o test04.opt.ll
```

### 步骤 3

与之前的实验类似，你将把分析器实现为一个名为 `DivZeroPass` 的 LLVM pass。

**如果你尚未完成代码，运行以下命令将导致段错误，这是正常的。完成代码后再重新运行测试。**

然后，你将使用 `opt` 命令在优化后的 LLVM IR 程序上运行此 pass，如下所示：

```sh
/lab6/test$ opt-19 -load-pass-plugin ../build/DivZeroPass.so -passes="DivZero" -disable-output test04.opt.ll > test04.out 2> test04.err
```

成功完成本实验后，`test/test04.out` 中的输出应如下所示：

```
Running DivZero on f
Potential Instructions by DivZero:
  %div1 = sdiv i32 %div, %div
```

程序的调试输出（使用 `errs()` 打印）将在 `test/test04.err` 文件中提供。

## 输入程序的格式

为了降低实验复杂度，我们限制了分析器必须处理的指令集。
我们假设本实验的输入程序只能使用 C 语言的以下子集：

* 所有值都是整数（即没有浮点数、指针、结构体、枚举、数组等）。
  你可以忽略其他类型的值。
* 程序可以有赋值、有符号和无符号算术运算（+、-、*、/）以及比较运算（<、<=、>、>=、==、!=）。
  所有其他指令都被视为 nop。
* 程序可以有 if 语句和循环。
* 用户输入仅通过提供的 `isInput` 函数返回 `True` 的那组函数引入。
  你可以忽略对其他函数的其他调用指令。

## 实验说明

一个完整的静态分析器包含三个组件：
1. 一个抽象域
2. 针对单个指令的转移函数，该函数使用抽象域评估指令。
3. 结合单个指令的分析结果，以获得整个函数或程序的分析结果。

在本实验的第一部分，我们将只专注于实现第2项，
并且仅针对上述描述的有限指令子集。

更具体地说，你的任务是实现分析如何在提供的抽象域（定义在 `Domain.h` 中）的抽象值上评估不同的 LLVM IR 指令。

在本实验的第二部分，我们将专注于实现第3项，即结合单个转移函数的结果，以获得一个过程内、流敏感、路径不敏感的除零分析。

我们提供了一个框架来构建你的除零静态分析器。
该框架由 `lab6/src/` 下的 `Domain.cpp`、`Transfer.cpp`、`ChaoticIteration.cpp` 和 `DivZeroAnalysis.cpp` 文件组成。

此外，我们还提供了 `src/Utils.cpp`，其中定义了一些有用的函数：

+ `variable` 接受一个 `Value` 并返回字符串。
    该字符串用作存储在 `InMap` 和 `OutMap` 中的 Memory 映射的键。
+ `getOrExtract` 接受一个 `Memory` 和一个 `Value`，并返回 `Memory` 中与 `Value` 对应的 `Domain`，如果未找到，则尝试从指令本身提取 `Domain`。
+ `printMemory`、`printInstructionTransfer` 和 `printMap` 会将各种调试信息打印到 `stderr`。

### **第一部分：检查函数和转移函数**

#### 步骤 1

通过阅读关于 [程序抽象动物园][Menagerie Link] 的文章，刷新你对程序抽象的理解。

一旦你对抽象域有了良好的理解，请研究 `Domain` 类，以了解我们为本实验定义供你使用的抽象域。
文件 `include/Domain.h` 和 `src/Domain.cpp` 包含了抽象值及其操作。
这些操作将执行抽象评估，**而无需运行程序**。
正如文章中所述，我们为加法、减法、乘法和除法定义了抽象运算符。

此分析的一个重要部分是认识到你实际上从未运行程序。
这意味着当你去评估诸如以下指令时：

```llvm
%cmp = icmp slt i32 %x, %y
```

`%cmp` 的 Domain 不是由 `%x` 和 `%y` 的运行时值决定的，而是由它们各自的 Domain 相对于比较指令的评估结果决定的。
所以，更具体地说，如果 `%x` 的 Domain 是 `Domain::Zero`，而 `%y` 的 Domain 是 `Domain::Zero`，由于小于比较会被认为是 **[相等时为假][LLVM CmpInst]**，结果 Domain 将是 `Domain::Zero`。

#### 步骤 2

检查 `DivZeroAnalysis::runOnFunction` 以了解编译器 pass 在高层面上如何执行分析：
```cpp
bool DivZeroAnalysis::runOnFunction(Function &F) {
  outs() << "Running " << getAnalysisName() << " on " << F.getName() << "\n";

  // 初始化 InMap 和 OutMap。
  for (inst_iterator Iter = inst_begin(F), E = inst_end(F); Iter != E; ++Iter) {
    auto Inst = &(*Iter);
    InMap[Inst] = new Memory;
    OutMap[Inst] = new Memory;
  }

  // 混沌迭代算法在 doAnalysis() 内部实现。
  doAnalysis(F);

  // 检查函数中的错误；
  ...
}
```
`runOnFunction` 过程在编译器执行 pass 期间遇到的输入 C 程序中的每个函数上被调用。
每个指令 `I` 被用作键，在全局的 `InMap` 和 `OutMap` 哈希映射中初始化一个新的 `Memory` 对象。
这些映射在下一步中有更详细的描述，但现在你可以将它们视为存储每个变量在指令之前和之后的抽象值。
例如，抽象状态可能存储诸如"**在指令 i 之前，变量 x 是正数**"这样的事实。
由于 `InMap` 和 `OutMap` 是全局的，请随意在代码中直接使用它们。

一旦 **In** 和 **Out** 映射被初始化，`runOnFunction` 调用 `doAnalysis`：一个你将在第二部分中实现的函数，用于执行混沌迭代算法。
对于第一部分，你可以假设它只是使用适当的 `InMap` 和 `OutMap` 映射调用 `transfer`。

所以，在高层面上，`runOnFunction` 将：
1. 初始化 **In** 和 **Out** 映射。
2. 使用混沌迭代算法填充它们。
3. 通过使用每个除法指令的 `InMap` 条目来检查除数是否可能为零，从而找到潜在的除零错误。

#### 步骤 3

理解提供的框架中的内存抽象。

对于每个 `Instruction`，`DivZeroAnalysis::InMap` 和 `DivZeroAnalysis::OutMap` 分别存储指令之前和之后的**抽象状态**。

抽象状态是从 LLVM 变量到抽象值的映射；具体来说，我们将 `Memory` 定义为 `std::map<std::string, Domain *>`。

由于我们将变量引用为 `std::string`，我们提供了一个名为 `variable` 的辅助函数，它将 LLVM `Value` 编码为我们内部的变量字符串表示形式。

请注意，`Instruction` 也是一个 `Value`。

例如，考虑以下 LLVM 程序。我们展示了每个指令之前和之后的抽象状态，记为 **M**：

|  ID   | 指令                            | 指令之前 | 指令之后       |
| :---: | :------------------------------ | :----------------- | :----------------- |
| `I1`  | `%x = call i32 (...) @input()`  | `{  }`             | `{ %x: T }`        |
| `I2`  | `%y = add i32 %x, 1`            | `{ %x: T }`        | `{ %x: T, %y: T }` |

在第一条指令 `I1` 中，我们将一个输入整数赋值给变量 `%x`。

在抽象状态中，我们使用抽象值 **T**（也称为"top"或 `MaybeZero`），因为该值在编译时是未知的。

指令 `I2` 更新了 `%y` 的抽象值，该值是通过对 `%x` 的抽象值使用抽象加法运算（记为 `+`）计算得出的。

请注意，在 LLVM 框架中，赋值指令（例如，call、binary operator、icmp 等）的对象也代表它定义的变量（即其左侧）。

因此，在你的实现中，你将使用指令 `I1` 和 `I2` 的对象来分别引用变量 `%x` 和 `%y`。

例如，`variable(I1)` 将引用 `%x`。

#### 步骤 4

既然我们理解了 pass 如何执行分析以及我们将如何存储每个抽象状态，我们可以开始实现了。

首先，你将在 `src/Transfer.cpp` 中实现函数 `DivZeroAnalysis::transfer`，以填充每个指令的 `OutMap`。
具体来说，给定一个指令及其传入的抽象状态（`const Memory *In`），`transfer` 应填充派生的传出抽象状态（`Memory *NOut`），该状态来自 `eval` 的适当实现。

`Instruction` 类代表所有类型指令的父类。
`Instruction` 有[许多子类][LLVM Instruction Class]。
为了填充 `OutMap`，每种类型的指令应以不同方式处理。

回想一下，在本实验中，你应该处理：
1. [二元运算符][LLVM BinOps]（add、mul、sub 等）
2. [CastInst][LLVM CastInst]
3. [CmpInst][LLVM CmpInst]（icmp、eq、ne、slt、sgt、sge 等）
4. 通过 `getchar()` 的用户输入 - 回想一下，这是使用 `src/Transfer.cpp` 中的 `isInput()` 处理的。

LLVM 提供了[几个模板函数][LLVM template functions]来检查指令的类型。
我们现在将专注于 `dyn_cast<>`。
在此示例中，我们检查 `Instruction` `I` 是否为 BinaryOperator。

```cpp
if (BinaryOperator *BO = dyn_cast<BinaryOperator>(I)) {
  // I 是一个 BinaryOperator，执行某些操作
}
```
在运行时，如果可能，`dyn_cast` 将返回 `I` *转换后*的 `BinaryOperator`，否则返回 null。

此时，你的 `eval(...)` 实现将接受指令，并确定该指令的 Domain 如何受操作影响。
例如，

```llvm
%add = add nsw i32 %x, %y
```
假设 `%x` 的域为 `Domain::Zero`，`%y` 的域为 `Domain::NonZero`。由于 `%y` 可以取任何非零值（正或负），`%add` 的结果域将由 `Zero` 与 `NonZero` 值的加法决定。
因此，`%add` 的域被确定为 `Domain::NonZero`。
通过这种方式，`DivZeroAnalysis::transfer` 函数为给定 `Instruction` 的相关操作更新 `OutMap`。

`PhiNode` 的 `eval` 函数已经为你实现，并提供了一个如何使用工具函数 `getOrExtract` 以及 `Domain::join` 的示例。

**使用 LLVM PHI 节点。**
出于优化目的，编译器通常以**静态单赋值**（SSA）形式实现其中间表示，LLVM IR 也不例外。
在 SSA 形式中，变量在恰好一个代码点被赋值和更新。
如果源代码中的变量有多个赋值，这些赋值在 LLVM IR 中被拆分为不同的变量，然后**合并**回来。
我们将这个合并点称为 **phi 节点**。

为了说明 phi 节点，请考虑以下代码：

<table>
  <tbody>
    <tr valign="top">
      <td>
        <pre><code>
int f() {
  int y = input();
  int x = 0;
  if (y < 1) {
  # then
    x++;
  } else {
    x--;
  }
  # end
  return x;
}
      </code></pre>
      </td>
      <td>
      <pre><code>
entry:
  %call = call i32 (...) @input()
  %cmp = icmp slt i32 %call, 1
  br i1 %cmp, label %then, label %else
       <br></br>
then:                      ; preds = %entry
  %inc = add nsw i32 0, 1  ; 等同于左侧的 x++
  br label %if.end
       <br></br>
else:                      ; preds = %entry
  %dec = add nsw i32 0, -1 ; 等同于左侧的 x--
  br label %end
       <br></br>
end:                       ; preds = %else, %then
  %x = phi i32 [%inc, %then ], [%dec, %else ]
  ret i32 %x
        </code></pre>
      </td>
    </tr>
  </tbody>
</table>



根据 `y` 的值，我们要么走左分支执行 `x++`，要么走右分支执行 `x--`。
在相应的 LLVM IR 中，对 `x` 的更新被拆分为两个变量 `%inc` 和 `%dec`。
`%x` 在分支执行后通过 `phi` 指令赋值；抽象地说，`phi i32 [ %inc, %then ], [ %dec, %else ]` 表示如果 then 分支被采用，则将 `%inc` 赋给 `%x`；如果 else 分支被采用，则将 `%dec` 赋给 `%x`。

这里有一段示例代码，可以帮助你处理 phi 节点，因为具体细节超出了本课程的范围；不过，如果你对这些编译器细节感兴趣，可以自由地进一步阅读 SSA 的相关内容。

```cpp
Domain *eval(PHINode *Phi, const Memory *InMem) {
  if (auto ConstantVal = Phi->hasConstantValue()) {
    return new Domain(extractFromValue(ConstantVal));
  }

  Domain *Joined = new Domain(Domain::Uninit);

  for (unsigned int i = 0; i < Phi->getNumIncomingValues(); i++) {
    auto Dom = getOrExtract(InMem, Phi->getIncomingValue(i));
    Joined = Domain::join(Joined, Dom);
  }
  return Joined;
}
```

#### 步骤 5

实现 `src/DivZeroAnalysis.cpp` 中的 `DivZeroAnalysis::check` 函数。
此函数检查一个 `Instruction`，以确定除零是否**可能**发生。
任何**有符号**或**无符号**除法指令，如果其除数的 `Domain` 是 `Domain::Zero` 或 `Domain::MaybeZero`，都将被视为潜在的除零错误。
你应该使用 `DivZeroAnalysis::InMap` 来判断是否存在错误。

为了测试你的 `check` 和 `transfer` 函数，我们提供了一个参考的 `doAnalysis` 二进制文件。
在第二部分中，你将需要自己实现 `doAnalysis` 函数，但现在你可以使用我们的二进制解决方案进行测试，以确保你到目前为止实现的函数工作正常。
按照以下步骤使用参考二进制文件进行编译：

```sh
/lab6/build$ rm CMakeCache.txt
/lab6/build$ cmake -DUSE_REFERENCE=ON ..
/lab6/build$ make
```

正如我们在设置部分演示的那样，使用 `opt` 在测试文件上运行你的分析器：

```sh
/lab6/test$ opt-19 -load-pass-plugin ../build/DivZeroPass.so -passes="DivZero" -disable-output test04.opt.ll > test04.out 2> test04.err
```

如果程序中存在除零错误，你的输出应如下所示：

```sh
Running DivZero on f
Instructions that potentially divide by zero:
  %div = sdiv i32 1, 0
```

### 第二部分：整合所有内容 - 数据流分析

现在你已经有了填充 in 和 out 映射的代码，并使用它们来检查除零错误，下一步是实现 `src/ChaoticIteration.cpp` 中 `doAnalysis` 函数的混沌迭代算法。

首先，回顾数据流分析的讲座内容。
特别是，研究到达定值分析和混沌迭代算法。
非正式地说，数据流分析为程序控制流图中的每个节点创建并填充一个 **IN** 集和一个 **OUT** 集。
**flowIn** 和 **flowOut** 操作重复执行，直到算法达到一个不动点。

更正式地说，`doAnalysis` 函数应维护一个 `WorkSet`，其中包含"需要更多工作"的节点。
当 `WorkSet` 为空时，算法已达到不动点。
对于 `WorkSet` 中的每个指令，你的函数应执行以下操作：

1. 通过合并所有传入流的 **OUT** 集来执行 **flowIn** 操作，并将结果保存到当前指令的 **IN** 集中。
在这里，你将使用你在第一部分中填充的 `InMap` 和 `OutMap` 中的条目作为 **IN** 和 **OUT** 集。
2. 应用你在第一部分中实现的 `transfer` 函数来填充当前指令的 **OUT** 集。
3. 通过相应地更新 `WorkSet` 来执行 **flowOut** 操作。
仅当 `transfer` 函数更改了 **OUT** 集时，才应添加当前指令的后继指令。

以下是一个关于如何将指令加载到 `WorkSet` 以及介绍 [llvm::SetVector][LLVM SetVector] 容器的示例，请随意在你的实现中使用此代码：

```cpp
void DivZeroAnalysis::doAnalysis(Function &F) {
  SetVector<Instruction *> WorkSet;
  for(inst_iterator I = inst_begin(F), E = inst_end(F); I != E; ++I) {
    WorkSet.insert(&(*I));
  }
  // ...
}
```

对于本实验，我们不需要维护显式的控制流图；LLVM 已经在内部维护了一个。
为了让你专注于本作业的数据流部分，我们提供了两个辅助函数 `getSuccessors` 和 `getPredecessors`（定义在 `include/DivZeroAnalysis.h` 中），它们查找并返回给定 LLVM `Instruction` 的后继和前驱。

接下来，你将实现混沌迭代算法的各个部分。

#### 步骤 1

在 `flowIn` 中，你将通过取 `I` 的所有前驱的 **OUT** 变量的并集来执行到达定值分析的第一步。
你可能会发现 `src/ChaoticIteration.cpp` 中的 `getPredecessors` 方法在此处很有帮助。
这应在以下为你模板化的函数中完成：

* `void DivZeroAnalysis:flowIn(Instruction *I, Memory *In)`

给定一个 `Instruction` `I` 及其 **IN** 变量集 Memory `In`，你需要将 **IN** 与 `I` 的每个前驱的 **OUT** 进行并集。
为了取两个内存状态的并集，你需要实现以下模板化的 join 函数：

* `Memory* join (Memory *M1, Memory *M2)`

在此函数中，合并这些 `Memory` 对象时，你还需要考虑 `Domain` 值。
请参考抽象域，了解为什么这是必要的。
回想一下，`Domain` 类中定义了用于合并两个抽象值的 `join` 操作。

#### 步骤 2

调用你在第一部分中实现的 `transfer` 函数来填充当前指令的 **OUT** 集。

#### 步骤 3

在 `flowOut` 中，你将确定给定指令是否需要再次分析。
这应在以下为你模板化的函数中完成：

* `void DivZeroAnalysis::flowOut(Instruction *I, Memory *Pre, Memory *Post, SetVector<Instruction *> &WorkSet)`

给定一个 `Instruction` `I`，你将分析**转移前**的 Memory `Pre` 和**转移后**的 Memory `Post`。
如果在应用 `transfer` 后内存值存在变化，你将需要将指令 `I` 提交以进行额外分析。
为了确定在 `transfer` 函数期间内存是否发生了变化，你将实现函数 `equal`：

* `bool equal(Memory *M1, Memory * M2)`

在此函数中，确定两个 `Memory` 对象是否相等时，你同样需要考虑 `Domain` 值。
回想一下，`Domain` 类中定义了用于评估两个抽象值是否相等的 `equal` 操作。

最后，在 `flowOut` 中，确保更新指令 `I` 的 `OutMap`，使其包含 `Post` 中的值。

#### 步骤 4

回想一下，在第一部分中，可以使用参考的 `doAnalysis` 来验证你的 `check` 和 `transfer` 实现。
现在你正在编写自己的 `doAnalysis` 版本，你可能需要在不使用参考的情况下重新构建 pass。
按照以下步骤使用你的实现进行编译：

```sh
/lab6/build$ rm CMakeCache.txt
/lab6/build$ cmake ..
/lab6/build$ make
```

完成上述步骤后，你的分析应生成两个输出文件。
1. `test.out`，其中 test 是你正在测试的程序，是结果的精简版本，仅包含具有潜在除零操作的指令。
2. `test.err` 是一个完整报告，包括任何具有潜在除零操作的指令，以及每个被审查指令的 `InMap` 和 `OutMap` 的最终状态。

你的输出格式如下：

```
Dataflow Analysis Results:
Instruction:   %cmp = icmp ne i32 0, 0
In set: 

Out set: 
    [ %cmp     |-> Zero      ]

Instruction:   br i1 %cmp, label %if.then, label %if.end
In set: 
    [ %cmp     |-> Zero      ]
Out set: 
    [ %cmp     |-> Zero      ]

Instruction:   %div = sdiv i32 1, 0
In set: 
    [ %cmp     |-> Zero      ]
Out set: 
    [ %cmp     |-> Zero      ]
    [ %div     |-> Uninit    ]

Instruction:   br label %if.end
In set: 
    [ %cmp     |-> Zero      ]
    [ %div     |-> Uninit    ]
Out set: 
    [ %cmp     |-> Zero      ]
    [ %div     |-> Uninit    ]

Instruction:   ret i32 0
In set: 
    [ %cmp     |-> Zero      ]
    [ %div     |-> Uninit    ]
Out set: 
    [ %cmp     |-> Zero      ]
    [ %div     |-> Uninit    ]
```

## 提交

完成实验后，通过提交并推送 `lab6/` 下的更改来提交你的代码。具体来说，你需要提交对 `src/ChaoticIteration.cpp`、`src/DivZeroAnalysis.cpp` 和 `src/Transfer.cpp` 的更改。

```sh
lab6$ git add src/ChaoticIteration.cpp src/DivZeroAnalysis.cpp src/Transfer.cpp
lab6$ git commit -m "your commit message here"
lab6$ git push
```

[LLVM template functions]: http://releases.llvm.org/8.0.0/docs/ProgrammersManual.html#the-isa-cast-and-dyn-cast-templates
[LLVM CmpInst]: https://llvm.org/doxygen/classllvm_1_1CmpInst.html
[LLVM CastInst]: https://llvm.org/doxygen/classllvm_1_1CastInst.html
[LLVM BinOps]: https://llvm.org/doxygen/classllvm_1_1BinaryOperator.html
[LLVM Instruction class]: http://releases.llvm.org/8.0.0/docs/ProgrammersManual.html#the-instruction-class
[LLVM AllocaInst]: https://llvm.org/doxygen/classllvm_1_1AllocaInst.html
[LLVM SetVector]: https://llvm.org/doxygen/classllvm_1_1SetVector.html
[CMake Ref]: https://en.wikipedia.org/wiki/CMake
[Make Ref]: https://www.gnu.org/software/make/manual/html_node/Simple-Makefile.html#Simple-Makefile
[Menagerie Link]: https://drive.google.com/open?id=1uhCWzfBxsaBQQ6NyMTY64Y6x_qRR1YQwiTpWT0_N2Xc