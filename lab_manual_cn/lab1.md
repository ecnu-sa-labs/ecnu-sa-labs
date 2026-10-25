# 理解静态分析与动态分析

理解软件分析的基本概念以及用于评估分析有效性的度量指标。

## 实验目标

本实验的目标是使用标准的静态分析与动态分析工具对 C 程序进行分析，发现除零错误，并解读分析结果，从而更好地理解不同技术之间的权衡取舍。具体来说，我们将使用 <a href="https://github.com/google/AFL">AFL</a>（American Fuzzy Lop，一种动态分析器/模糊测试器）和 <a href="https://clang-analyzer.llvm.org/">CSA</a>（Clang Static Analyzer，一种静态分析器）。

## 前置知识

课程讲座中介绍了本实验使用的各种术语，例如：静态分析与动态分析、正确性（soundness）、完备性（completeness）、精确率（precision）、召回率（recall）等。

## 环境配置

### 第一步
Lab 1 的骨架代码位于 `/lab1` 目录下。
在描述实验文件位置时，我们将 Lab 1 的这个顶层目录简称为 `lab1`。

### 第二步
在整个实验过程中，我们将使用 `CMake`，这是一个用于管理构建过程的现代工具。
如果你不熟悉 `CMake`，我们建议阅读 [CMake 教程][cmake-tutorial]（请特别注意教程中的 Step 1 和 Step 2）。
运行 `cmake` 会生成一个你可能更熟悉的 `Makefile`。
如果你不熟悉 `Make`，请先阅读 [Makefile 教程][makefile-tutorial] 或 [在 Y 分钟内学会 Make][learn-make-in-y-minutes]，然后仔细阅读 `lab1/Makefile` 文件。
请确保你能够熟练使用本实验中的 `Makefile`。

### 第三步

查看 Makefile，了解用于运行 <a href="https://github.com/google/AFL">AFL</a> 和 <a href="https://clang-analyzer.llvm.org/">CSA</a> 的命令。

```sh
# 使用 AFL 编译程序
AFL_DONT_OPTIMIZE=1 afl-gcc c_programs/test1.c -o test1
# 设置内核将崩溃信息转储到 AFL 的 "core" 文件中
echo core >/proc/sys/kernel/core_pattern
# 在 test1 上运行 AFL 30 秒
timeout 30s afl-fuzz -i afl_input -o afl_output -- ./test1

# 在 test1.c 上运行 CSA
clang -v --analyze c_programs/test1.c
```

## 实验说明

在本实验中，你将在一组 C 程序上运行 AFL 和 CSA，研究这两个工具的输出结果，并报告你的发现。

### 第一步

对 `lab1/c_programs` 目录下的所有 C 程序运行提供的分析工具 AFL 和 CSA。
为此，只需运行以下命令，该命令会先对每个程序运行 AFL（超时时间为 30 秒），然后运行 CSA。

```sh
/lab1$ make check_versions && make all
```

运行上述命令后，你将看到类似如下的输出：

```
AFL_DONT_OPTIMIZE=1 afl-gcc c_programs/test1.c -o ...
timeout 30s afl-fuzz -i afl_input -o ...
Makefile:8: recipe for target 'results/afl_logs/test1/out.txt' failed
make: [results/afl_logs/test1/out.txt] Error 124 (ignored)
clang -v --analyze c_programs/test1.c ...
...
```

请忽略上面 Make 报告的错误；这是正常的，因为 AFL 会一直运行，直到被 `timeout` 命令强制终止。
你可以随意尝试更改当前设置为 30 秒的超时时间。
我们不期望每个人都报告相同的结果，因为 AFL 本身是非确定性的。

运行 make 命令后，应该在 `lab1/results` 目录下生成以下文件和目录：

```
   ├── afl_logs/
   │   ├── test0/
   │   │   ├── out.txt
   │   │   ├── afl_output/
   │   │   └── test0
   │   ├── ... // test1 类似
   │   ...
   │
   └── csa_logs/
       ├── test0_out.txt
       ├── ... // test1 类似
       ...
```

### 第二步

确定这些 C 程序关于除零错误的真实情况（`正确` vs. `错误`）。
具体来说，对于每个程序中的每条除法指令，通过检查程序来判断是否存在某些程序输入会导致除零错误。
将你的答案填写在 `lab1/answers.txt` 文件中每个测试对应的“真实情况”列。

### 第三步

研究 AFL 和 CSA 的输出，判断它们对每个程序是接受还是拒绝。
将你的答案填写在 `lab1/answers.txt` 文件中每个测试程序对应的表格列中。

AFL 发现的导致崩溃的输入存储在 `lab1/results/afl_logs/<test-name>/afl_output/crashes/` 目录下的单独文件中。
这些文件具有独特的名称，形式如 `id:000000,sig:08,src:000000,op:arith8,pos:2,val:-8`[^1]。
这些文件的内容就是 AFL 在遇到崩溃时用作测试程序输入的数据。

检查 CSA 的输出时，如果出现 `core.DivideZero` 警告，则表示 CSA 检测到了除零错误。

例如：

```c
c_programs/test9.c:10:17: warning: Division by zero [core.DivideZero]
   10 |   int avg = sum / len;
      |             ~~~~^~~~~
```

### 第四步

使用你在第二步和第三步中填写的内容，计算每一列的精确率（Precision）、召回率（Recall）和 F1 分数（F1 Score）。
将它们填入 `lab1/answers.txt` 的相应行中。

### 第五步

借助你填写的表格，回答 `answers.txt` 中的问题。

## 提交

完成实验后，通过提交并推送 `lab1/` 目录下的更改来提交你的结果。
请注意，请在你的*本地机器*上提交结果，而不是在 Docker 中的远程机器上。
具体来说，你需要提交 CSA 和 AFL 在 `lab1/results` 中生成的结果以及你的答案表 `answers.txt`。

```
   lab1$ git add results/ answers.txt
   lab1$ git commit -m "你的提交信息"
   lab1$ git push
```

[^1]: 文件名编码了多种信息，例如崩溃输入的 ID、崩溃信号、产生此崩溃输入的非崩溃种子输入（在我们的例子中始终是文件 lab1/afl_input/seed.txt），以及将非崩溃种子输入转换为该崩溃输入所执行的操作。

[cmake-tutorial]: https://cmake.org/cmake/help/latest/guide/tutorial/index.html
[makefile-tutorial]: https://www.gnu.org/software/make/manual/html_node/Simple-Makefile.html
[learn-make-in-y-minutes]: https://learnxinyminutes.com/docs/make