# Delta Debugging

构建一个 delta debugger，用于最小化导致程序崩溃的输入——帮助用户更容易理解 bug。

## 目标

在本实验中，你将构建一个 delta debugger，实现一种高效算法，在给定一个较大的崩溃输入时，找到 1-minimal 的崩溃输入。
你将把这个工具与一个模糊测试器（如你在 `lab3` 中构建的）结合起来，以最小化模糊测试器发现的崩溃输入。

## 环境设置

Lab 4 的代码位于 `/lab5/` 目录下。

本实验建立在之前实验的基础上。
我们为你提供了预编译的二进制文件，包括 `runtime` 库、
用于 `coverage` 和 `sanitize` 的 `InstrumentPass`、
以及一个 `fuzzer` 可执行文件；你可以在 `lab5/lib` 目录下找到它们。
它们的实现与 `lab3` 中的实现完全相同。

### 步骤 1

本实验使用 Python 实现 delta debugger。
我们通过构建一个名为 `delta_debugger` 的 Python 包来实现。

要构建并安装该包，请运行：

```sh
/lab5$ make install
```

与 `c++` 不同，修改代码后你不需要重新运行此命令。
此外，你将能够通过终端中的 `delta-debugger` 命令使用你的 delta debugger。

`delta-debugger` 工具执行 delta debugging，以缩小导致程序崩溃的输入。

### 步骤 2

要将 `delta-debugger` 与某个程序一起使用，你首先需要找到一些会导致该程序崩溃的输入。
为了找到这样的输入，我们将使用一个模糊测试器。

和 lab3 一样，要运行 `fuzzer`，你首先需要插桩程序，并设置合适的输出目录，模糊测试器会将结果存储在其中。

```sh
/lab5/test$ make sanity1               # 插桩并构建 sanity1。
/lab5/test$ mkdir fuzz_output_sanity1  # 创建输出目录。
# 在 sanity1 上运行模糊测试器，超时时间为 6 秒。
/lab5/test$ timeout 6s ../lib/fuzzer ./sanity1 fuzz_input fuzz_output_sanity1
```

你也可以使用 Makefile 来为你完成插桩、构建、设置输出目录和运行模糊测试器：

```sh
/lab5/test$ make sanity1               # 插桩并构建 sanity1。
/lab5/test$ make fuzz-sanity1          # 在 sanity1 上运行模糊测试器。
```

### 步骤 3

运行模糊测试器后，你会在 `test/fuzz_output_sanity1/failure` 目录下找到导致程序崩溃的输入。

```
fuzz_output_sanity1
├── success
├── randomSeed.txt
└── failure                            # 导致崩溃的输入。
    ├── input0
    ├── input1
    │    ...
    └── inputN
```

现在你可以使用 `delta-debugger` 来最小化模糊测试器发现的崩溃输入。

```
/lab5/test$ delta-debugger ./sanity1 fuzz_output_sanity1/failure/input1
```

最后一个参数是崩溃输入的路径，具体取决于你想要最小化哪个输入。
在这个例子中，精简后的输入存储在 `fuzz_output/failure/input1.delta` 中。
此外，在再次运行 `delta-debugger` 之前，请确保清理 `fuzz_output` 目录。

你可以通过运行以下命令来清理：

```sh
/lab5/test$ rm -rf fuzz_output_sanity1 && mkdir fuzz_output_sanity1
```

## 实验说明

你需要编辑 `lab5/delta_debugger/delta.py` 文件来构建一个 delta debugging 工具。
我们提供了一个模板函数——`delta_debug`——供你实现最小化逻辑。
`delta_debug` 函数接收一个 `target` 程序，以及一个导致 `target` 崩溃的 `input`，
并应返回一个仍然能使 `target` 程序崩溃的 1-minimal 输入。

为了执行 delta debugging，你需要反复使用不同的 `input` 字符串运行 `target`。
我们提供了一个 `run_target` 函数来帮助你使用 `input` 运行 `target` 程序。
如果目标程序没有崩溃，它会返回 0。

```py
def run_target(target: str, input: Union[str, bytes]) -> int:
    """
    使用输入作为 stdin 运行目标程序。
    :param target: 要运行的目标程序。
    :param input: 要传递给目标程序的输入。
    :return: 目标程序的返回码。
    """
    ...
```

在本实验中，你将修改 `delta_debug` 函数，以实现你在课堂上学到的算法，从而找到 1-minimal 的崩溃输入。

你可能需要添加一个辅助函数，例如命名为 `_delta_debug`，
它接收一个 `target`、一个 `input` 和一个对应于搜索粒度的参数 `n`，
并执行一次 delta debugging 算法的迭代，以返回下一个 `input` 和 `n`。

## 示例输入和输出

你的 delta debugger 应该能在任何接受来自 `stdin` 输入的可执行文件上运行。

你可以通过传入以下参数在测试程序上运行 delta debugger：

```sh
delta-debugger ./test crashing-input
```

然后 delta debugger 会将其结果存储在 `crashing-input.delta` 文件中。

举一个具体的例子，考虑字符串："This is theh) "，它会导致 `test2` 失败：

```sh
/lab5/test$ echo -n "This is theh) " > tmp
/lab5/test$ delta-debugger ./test2 tmp
/lab5/test$ cat tmp.delta
This is
```

## 需要提交的内容

完成实验后，通过提交并推送 `lab5/` 目录下的更改来提交你的代码。具体来说，你需要提交对 `delta_debugger/delta.py` 的更改。

```sh
lab5$ git add delta_debugger/delta.py
lab5$ git commit -m "在此处填写你的提交信息"
lab5$ git push
```