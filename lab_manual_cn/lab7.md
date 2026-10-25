# 指针分析

编写一个作为 LLVM pass 的 C 语言程序"除零"静态分析，用于处理指针别名和动态分配的内存。

## 目标

本实验的目标是扩展实验 6 中的静态**除零**检查器，使其能够在存在指针的情况下进行分析。
你将把上一个实验中的数据流分析与流不敏感指针分析相结合，从而得到一个更全面的整体静态分析。

## 环境搭建

实验 6 的骨架代码位于 `/lab6` 目录下。
在描述文件位置时，我们会经常将 Lab 6 的顶层目录称为 `lab6`。
本实验基于你在实验 5 中的工作，因此你可以重用 `/lab5/src` 目录中的大部分内容。

### 步骤 1

以下命令使用之前见过的 CMake/Makefile 模式来搭建实验环境。

```sh
/lab6$ mkdir build && cd build
/lab6$ cmake ..
/lab6$ make
```

在生成的文件中，你应该会在 `build` 目录下看到 `DivZeroPass.so`，这与上一个实验类似。
在本实验中，你将修改 `src/ChaoticIteration.cpp`、`DivZeroAnalysis.cpp` 和 `Transfer.cpp`。
这些更改大部分可以从上一个实验复制过来，然后进行修改以适应新的需求。

现在，我们准备在一个示例输入 C 程序上运行我们的基础实验。

### 步骤 2

在运行 pass 之前，必须先生成 LLVM IR 代码。

```sh
/lab6/test$ clang -emit-llvm -S -fno-discard-value-names -Xclang -disable-O0-optnone -c test13.c -o test13.ll
/lab6/test$ opt -load ../build/DivZeroPass.so -DivZero test13.ll
```

第一行 (`clang`) 从输入 C 程序 `test13.c` 生成 LLVM IR 代码。
下一行 (`opt`) 在编译后的 LLVM IR 代码上运行你的 pass。

在之前的实验中，我们使用了一个带有参数 `-mem2reg` 的中间步骤，该参数将每个
[AllocaInst][LLVM AllocaInst] 提升为寄存器，从而让你的分析器在本实验中可以忽略指针的处理。
然而，在本实验中我们不再这样做，因此你将扩展之前的代码来处理指针。

成功完成本实验后，输出应如下所示：

```sh
/lab6/test$ opt -load ../build/DivZeroPass.so -DivZero test13.ll
Running DivZero on f
Potential Instructions by DivZero:
    %div = sdiv i32 1, %2
```

## 输入程序的格式

本实验的输入格式与实验 6 相同，只是现在你需要处理指针：

* 你*可以*忽略对整数以外值的精确处理，但你的 LLVM pass 在遇到其他类型的值时不能引发段错误。
* 你*必须*处理赋值、算术运算 (+, -, *, /)、比较运算 (<, <=, >, >=, ==, !=) 和分支。
* 你*不必*精确处理 XOR、OR、AND 和移位运算，但你的程序在这些情况下不能引发段错误。
* 输入程序*可以*包含 if 语句和循环。
* 用户输入*仅*通过提供的 `isInput` 函数返回 `True` 的那组函数引入。
* 你*可以忽略*对其他函数的其他调用指令。

## 实验说明

在本实验中，你将扩展在实验 6 中实现的**除零**分析，以在存在别名内存位置的情况下分析并捕获潜在的**除零**错误。

在课堂上，你了解到在语言中引入别名会使推理程序的行为变得更加困难，
并且需要某种形式的指针分析。
你将使用一种**流不敏感指针分析**
——我们抽象掉控制流并构建一个全局的**指向图**
——来帮助你的检查器分析更有意义的程序。

### 第一部分：函数参数/调用指令

#### 步骤 1

回想一下，在之前的实验中，所有测试程序都是不接受参数的基本函数。

例如：

```c
void f() {
    int x = 0;
    int y = 2;
    int z;
    if(x < 1) {
        z = y / x; // 分支内的除零
    }
}
```

函数 `f()` 的签名中没有参数。
实际上，函数可以接受任意数量的变量，
甚至可以是不同类型的（但本实验将所有参数视为 `int`）。

因此，在 `doAnalysis` 中，你需要处理带有参数的函数，并相应地设置它们的域。

#### 步骤 2

熟悉作为你的**除零** LLVM pass 入口点的 `doAnalysis()` 例程。
在上一个实验中，你在这里实现了混沌迭代算法。
对于实验 6，`doAnalysis()` 的函数签名现在略有变化，包含了一个 **PointerAnalysis** 对象。
我们将在第二部分中讨论这一点。

```cpp
/**
 * @brief 此函数使用 flowIn()、transfer() 和 flowOut() 实现混沌迭代算法。
 *
 * @param F 要分析的函数。
 */
void DivZeroAnalysis::doAnalysis(Function &F, PointerAnalysis *PA)
```

#### 步骤 3

给定一个传入 `doAnalysis()` 例程的任意函数 `F`，找到函数调用的参数并为每个参数实例化抽象域值。
请注意，这里的对象 `F` 是 `Function` 类型，可用于查找所有可用的参数。

此外，一旦你初始化了这些起始参数抽象值，
将这些值传递到你现有的**除零** pass 实现中，
以便这些变量在整个**到达定值分析**中得到传播。

#### 步骤 4

除了处理正在分析的函数 `F` 的参数外，
我们还希望覆盖程序中进行的其他函数调用。

我们之前见过这个函数：

```c
void main() {
    int x = getchar();
    int y = 5 / x;
    return 0;
}
```

在上面的例子中，`getchar()` 是一个没有参数的外部函数调用，返回一个 `int`。
更新你的分析以处理任意的 `CallInst` 指令，但仅限于返回类型为 `int` 的情况。

### 第二部分：存储/加载指令

#### 步骤 1

如上所述，之前的 `doAnalysis()` 函数有一个变化：

```cpp
void DivZeroAnalysis::doAnalysis(Function &F, PointerAnalysis *PA)
```

此外，我们还修改了实验 6 中使用的 `transfer` 函数的签名：

```cpp
void DivZeroAnalysis::transfer(Instruction *I, const Memory *In, Memory *NOut,
                               PointerAnalysis *PA, SetVector<Value *> PointerSet)
```

在重用上一个作业的代码时，请确保复制你的实现细节和函数内容，但**保持函数签名不变！**。

这些参数在我们探索指针别名时是必需的。

为了帮助理解代码与 lab6 有何不同以及它们是如何联系在一起的，
请考虑 `DivZeroAnalysis::runOnFunction()` 中的以下片段：

```cpp
bool DivZeroAnalysis::runOnFunction(Function &F) {
  outs() << "Running " << getAnalysisName() << " on " << F.getName() << "\n";

  // 此处有更多代码...
  PointerAnalysis *PA = new PointerAnalysis(F);
  doAnalysis(F, PA);
  // 此处有更多代码...
}
```

以及 `DivZeroAnalysis::doAnalysis()` 中的以下片段：

```cpp
void DivZeroAnalysis::doAnalysis(Function &F, PointerAnalysis *PA) {
    for(inst_iterator I = inst_begin(F), E = inst_end(F); I != E, ++I) {
        WorkSet.insert(&(*I));
        PointerSet.insert(&(*I));
    }
    // 此处有更多代码...
    transfer(I, In, NOut, PA, PointerSet);
    // 此处有更多代码...
}
```

并且，请注意 transfer 函数现在将 PointerAnalysis 和 PointerSet 作为输入。
在重用实验 6 的代码时请记住这一点。

#### 步骤 2

从高层次来看，你将修改 `Transfer.cpp` 中的 `transfer()` 函数，通过跟踪指针来执行更复杂的**除零**分析。

`PointerAnalysis` 的代码位于 `src/PointerAnalysis.cpp` 中，它包含了使用指针别名所需的各种方法的实现。
在对 `F` 运行指针分析后，`PointerAnalysis *PA` 对象将包含
对该函数运行的指针分析结果，
而 `PointerSet` 将包含该函数中的所有指针。

我们将在以下部分更详细地讨论这个 `PointerAnalysis` 类的作用，
但请通读文档字符串和代码，并理解所提供的每个方法中正在做什么。

##### 建模 LLVM alloca、store 和 load。

这里我们提供了一个用于在 LLVM 中处理指针的接口。

你可以按原样使用它作为后备方案，但也可以自由地以你希望的方式对 LLVM 中的引用进行建模。

对于本实验，我们禁用了实验 6 中使用的 `mem2reg` pass。
因此，LLVM 将为每个 C 变量创建一个内存单元。
结果，你将不会看到任何 **phi 节点**，并且不一定需要
你在实验 6 中为实现处理它们而编写的代码段。

考虑以下代码：

<table>
<tbody>
<tr valign="top">
<td>
<pre><code>
int f() {
  int a = 0;
  int *c = &a;
  int x = 1 / *c;
  return x;
}
</code></pre>
</td>
<td>
<pre><code>
I1: %a = alloca i32, align 4
I2: %c = alloca i32*, align 4
I3: %x = alloca i32, align 4
I4: store i32 0, i32* %a, align 4
I5: store i32* %a, i32** %c, align 8
I6: %0 = load i32*, i32** %c, align 8
I7: %1 = load i32, i32* %0, align 4
I8: %div = sdiv i32 1, %1
I9: store i32 %div, i32* %x, align 4
I10: %2 = load i32, i32* %x, align 4
I11: ret i32 %2
</code></pre>
</td>
<td>
<pre><code>
M[variable(I1)] = 0
M[variable(I2)] = variable(I1)
M[variable(I6)] = M[variable(I2)]
...
...
...
...
...
...
</code></pre>
</td>
</tr>
</tbody>
</table>

与实验 6 一样，`variable()` 方法仍然用于对指令的变量进行编码。

##### 构建指向图。

`PointerAnalysis` 类构建了一个指向图，你将在 `transfer` 函数中使用它。
`PointsToInfo` 表示从变量到 `PointsToSet` 的映射，
`PointsToSet` 表示一个变量可能指向的分配站点的集合。

为了帮助建模与变量 `%a`（即 `variable(I1)`）对应的内存位置，
我们提供了一个函数 `address`，
你可以在构建 `PointsToSet` 时使用它来对变量的内存地址（`address(I1)`）进行编码。

指令 `I2` 将被类似地分析。

在 `I5` 处，在 `I2` 处分配的内存位置（即 `address(I2)`）
将存储在 `I1` 处分配的内存位置（即 `address(I1)`）。

此外，字段 `PointsTo` 表示将要构建的完整指向图。

`PointerAnalysis` 构造函数的实现将遍历给定 `Function F` 的所有指令并填充 `PointsTo`，
该实现已作为本作业中骨架代码的一部分提供给你。

此外，我们还提供了一个 `alias()` 方法，如果两个指针可能是彼此的别名，则返回 true。

#### 步骤 3

使用 `PointerAnalysis` 对象，增强 `Transfer.cpp` 中的 `transfer()` 函数，使其在分析过程中考虑指针别名。
这应该通过添加代码来处理 `transfer` 函数中的 `StoreInst` 和 `LoadInst` 指令来完成。

##### LoadInst

我们可以依赖 `In` 内存中定义的现有变量来知道
应该为加载指令引入的新变量分配什么抽象域。

例如，给定如下加载指令：

```llvm
%2 = load i32, i32* %1, align 4
```

这是将 `%1` 处指针的值加载到类型为 `i32` 的新变量 `%2` 中。
因此 `%2` 的抽象域应该与 `%1` 的抽象域相同。

随着指针的加入，我们也可以有：

```llvm
%1 = load i32*, i32** %d, align 8
```

这是将 `%d` 处指针的值（其本身是一个指针）加载到类型为 `i32*` 的新变量 `%1` 中。

**注意** 与前面的示例相比，加载指令的类型（`load i32*`）中多了一个 `*` 字符。
你可以使用 `getType()` 检索此加载指令的类型，
并进一步使用 `isIntegerTy()` 或 `isPointerTy()` 等方法检查类型。

##### StoreInst

存储指令可以将新变量添加到我们的内存映射中，也可以覆盖现有变量。

例如，给定如下存储指令：

```llvm
store i32, 0, i32* %a, align 4
```

这是将值 `0` 存储到变量 `%a` 中。

你应该熟悉使用 `getOperand()` 检索这些操作数，但你也可以分别使用 `getValueOperand()` 和 `getPointerOperand()` 方法。
随着指针的加入，我们也可以有：

```llvm
store i32* %a, i32** %c, align 4
```

现在我们将 `%a` 处的指针存储到变量 `%c` 中，`%c` 是一个指向指针的指针。
我们可以再次使用每个操作数上 `getType()` 的类型信息来确定是否可能应用指针别名。

这显然使我们的抽象域分析变得复杂——如果某个后续指令更新了 `%a` 的值，我们不仅需要更新 `%c` 的抽象值，还需要考虑更新指向 `%a` 的其他指针的抽象值。
这也适用于对 `%c` 所做的更改，这正是 `test13.c` 示例中发生的情况。

```c
int f() {
    int a = 1;
    int *c = &a;
    int *d = &a;
    *c = 0;
}
```

为了解决这些情况，我们可以依赖在 `PointerAnalysis` 中构建的指向图。

我们需要遍历提供的 `PointerSet`：
如果我们遇到存在可能别名（`PA->isAlias()` 返回 `true`）的实例，
这基本上意味着存在一条连接两个变量之间指针值的边。
一旦我们知道存在哪些连接，
我们将需要获取每个抽象值，
通过 `Domain::join()` 将它们全部合并，
然后继续使用此抽象值更新当前赋值以及**所有**可能别名的赋值。
这确保了所有指针引用保持同步，并且将在我们的分析中收敛到一个精确的抽象值。

## 提交

完成实验后，通过提交并推送 `lab6/` 下的更改来提交你的代码。具体来说，你需要提交对 `src/ChaoticIteration.cpp`、`src/DivZeroAnalysis.cpp` 和 `src/Transfer.cpp` 的更改。

```sh
lab6$ git add src/ChaoticIteration.cpp src/DivZeroAnalysis.cpp src/Transfer.cpp
lab6$ git commit -m "你的提交信息"
lab6$ git push
```

[LLVM AllocaInst]: https://llvm.org/doxygen/classllvm_1_1AllocaInst.html