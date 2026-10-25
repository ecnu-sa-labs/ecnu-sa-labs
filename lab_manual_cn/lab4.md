# 基于属性的测试

使用基于属性的测试来验证二叉搜索树的实现。

## 目标

在本实验中，你将应用基于属性的测试，基于 [Hypothesis](https://github.com/HypothesisWorks/hypothesis) 来验证二叉搜索树的实现。Hypothesis 是 Python 中一个流行的基于属性的测试工具。通过本实验，你将学习如何定义不同形式的属性以实现有效的测试。

## 环境配置

请在你的实验环境中安装 Python 包 `pytest` 和 `hypothesis`：

```bash
lab4$ pip install -r requirements.txt  # 安装所需的包
```

## 前置知识

### 二叉搜索树 (BST)

二叉搜索树是一种具有特定排序属性的有根二叉树，能够高效地执行插入、删除和搜索操作。

对于树中的每个节点：

1. 其左子树中所有节点的键都小于该节点自身的键。
2. 其右子树中所有节点的键都大于该节点自身的键。
3. 键通常是唯一的。

因此，中序遍历——按照左、根、右的顺序访问节点——总是会生成严格升序的键序列，如下例所示。

<div align="center">
<img src="../images/lab4-bst-example1.png"
  style="height: auto; width: 50%">
</div>

在本实验中，`lab4/src/BST.py` 给出了二叉搜索树的一个实现。该 BST 实现支持任何可比较类型的键和任何类型的值，以及四个核心操作 `insert`、`delete`、`find` 和 `union`。请仔细阅读代码以理解其实现。

### Hypothesis

Hypothesis 是一个实现了基于属性测试的 Python 库。基于属性测试根据某些给定的属性来验证函数（或模块甚至系统）的正确性。它使用大量自动生成的测试用例，而不是单个测试用例，来有效地对函数进行压力测试。如果发现某些属性违反，它会缩小并返回反例输入。

在下面的代码中，我们展示了如何使用经典的*基于示例的测试*来测试一个 `sort` 函数（参见 `test_sort_by_example`），以及如何使用*基于属性的测试*（基于 Hypothesis，参见 `test_sort_by_property`）来测试这个 `sort` 函数。这里，`sort` 函数按升序对数字进行排序。

```python
# 基于示例的测试
def test_sort_by_example():
    input_list = [3, 1, -1] # 提供单个输入
    output = sort(input_list)
    expected_output = [-1, 1, 3]
    assert output == expected_output # 检查单个输出
    
# 基于 Hypothesis 的基于属性的测试
# 自动生成包含 0 到 1000 个元素的随机整数列表。
@given(st.lists(st.integers(), min_size=0, max_size=1000))
def test_sort_by_property(input_list):
    # 前置条件
    assume(len(input_list) > 1)
    
    sorted_list = sort(input_list)
    # 后置条件：验证排序属性成立
    for i in range(len(sorted_list) - 1):
        assert sorted_list[i] <= sorted_list[i + 1]
```

#### 策略

Hypothesis 中的策略是数据生成器，它们自动为你的基于属性的测试创建多样化的测试输入（具有不同的类型和形式）。

```python
from hypothesis import strategies as st

# 基本类型
st.integers()           # 整数
st.floats()             # 浮点数  
st.text()               # 文本
st.booleans()           # 布尔值

# 复杂类型
st.lists(st.integers()) # 整数列表
st.dictionaries(st.text(), st.integers())  # 字典
st.tuples(st.integers(), st.text())        # 元组

# 自定义策略
st.one_of(st.integers(), st.text())        # 多种类型之一
```

在本实验中，`lab4/src/test_strategies.py` 已经实现了两个策略，即 `keys_strategy` 和 `trees_strategy`：

```python
keys_strategy = st.one_of(st.integers(min_value = -25, max_value = 25), st.integers())

def build_bst_from_tuples(kv_list: List[Tuple[int,int]]) -> BST[int,int]:
    bst: BST[int,int] = BST.nil()
    for k, v in kv_list:
        bst = bst.insert(k, v)
    return bst

trees_strategy = st.lists(
    st.tuples(keys_strategy, st.integers()),
    min_size = 0,
    max_size = 50,
    unique_by = lambda kv: kv[0]
).map(build_bst_from_tuples)
```

+ `keys_strategy`：它从受限范围 [-25, 25] 或完整的整数范围内随机选择一个键。

> 设计目的是：(1) 受限范围 (-25 到 25) 增加了键冲突（相同键多次出现）的概率，以模拟 BST 的真实使用场景；(2) 完整范围确保你也能用多样化、间隔较大的键进行测试。这种设计通过平衡冲突场景和一般情况，使测试更加有效。

+ `trees_strategy`：它生成最多包含 50 个（键，值）对的随机 BST 对象，且键是唯一的。

> 在内部，此策略使用 `insert` 操作添加带有（键，值）的节点来构建 BST。

#### 收缩

Hypothesis 的一个关键特性是收缩。如果测试用例失败，它不会仅仅报告原始的复杂输入。相反，它会智能地将该输入简化为仍然导致失败的最小形式，从而更容易识别根本问题。

### Pytest

Pytest 是一个强大的 Python 测试框架，简化了测试的创建和执行。它具有自动测试发现、全面的错误报告和丰富的插件生态系统等特点。

为了让你熟悉 Pytest，你可以在 `lab4/tests` 目录下运行 `lab4/tests/simple_test.py`，该测试测试了 BST 的一个有缺陷版本（对应于 `lab4/bugs/bug1.py`）：

```bash
lab4/tests$ pytest simple_test.py -q --tb=no # 简洁的输出结果
```

你将获得以下测试结果，表明 3 个测试失败，8 个测试通过：

```tex
$ pytest simple_test.py -q --tb=no
...F..F..F.                                                                        [100%]
================================ short test summary info =================================
FAILED simple_test.py::test_three_inserted_values_can_be_found - AssertionError: assert None == 'twenty'
FAILED simple_test.py::test_a_deleted_value_can_no_longer_be_found - AssertionError: assert None == 'twenty'
FAILED simple_test.py::test_union_of_two_bsts_contains_keys_of_both - AssertionError: assert {(1, 'one'), ..., (5, 'five')} == {(1, 'one'), ..., (5, 'five')}
3 failed, 8 passed in 0.09s
```

如果你想获得详细结果，可以运行：

```bash
lab4/tests$ pytest simple_test.py -v --tb=short # 详细的输出结果
```

## 实验指导

### 项目结构

```tex
.
|-- bugs
|   |-- __init__.py
|   |-- bug1.py     # 在 find 和 union 中植入了错误，旨在被 test1.py 发现
|   |-- bug2.py     # 在 delete 和 union 中植入了错误，旨在被 test2.py 发现
|   |-- bug3.py     # 在 delete 中植入了错误，旨在被 test3.py 发现
|   `-- bug4.py     # 在 delete 和 union 中植入了错误，旨在被 test4.py 发现
|-- requirements.txt
|-- src
|   |-- BST.py               # BST 数据结构的正确实现
|   |-- BSTUtils.py          # 工具函数
|   |-- __init__.py
|   `-- test_strategies.py   # Hypothesis 的测试策略
`-- tests
    |-- conftest.py      # 运行时环境配置和测试报告生成
    |-- hypothesis.ini   # Hypothesis 配置
    |-- makefile         # 运行脚本，包括 all、clean、test1、test2、test3、test4
    |-- simple_test.py   # BST 的简单测试
    |-- test1.py         # TODO1：为测试 find 和 delete 定义有效性属性。
    |-- test2.py         # TODO2：为测试 delete 和 union 定义后置条件属性。
    |-- test3.py         # TODO3：为测试 delete 和 union 定义蜕变属性。
    `-- test4.py         # TODO4：为测试 delete 和 union 定义基于模型的属性。
```

### BST 中的属性和植入的错误

在本实验中，你将在验证 BST 的上下文中学习并定义以下四种类型的属性。
你需要定义不同的属性来捕获我们植入 BST 中的错误。祝你好运，享受过程！

| 属性类型           | 目标方法       | 要识别的错误 | 错误描述                                                     |
| :----------------- | :------------- | :----------- | :----------------------------------------------------------- |
| 有效性属性         | `find`, `union` | bug1.py - BUG(1) | 在 `find(key)` 中：错误地将左子树赋值为右子树 |
|                    |                | bug1.py - BUG(2) | 在 `union(bst1, bst2)` 中：错误地将 bst1 作为 bst2 的左子树 |
| 后置条件属性       | `delete`, `union` | bug2.py - BUG(1) | 在 `delete(key)` 中：错误地选择了搜索方向。      |
|                    |                | bug2.py - BUG(2) | 在 `union` 中：当键相同时，错误地优先选择 bst2 而不是 bst1 |
| 蜕变属性           | `delete`, `union` | bug3.py - BUG(1) | 在 `delete(key)` 中：错误地选择了搜索方向。      |
| 基于模型的属性     | `delete`, `union` | bug4.py - BUG(1) | 在 `delete(key)` 中：错误地选择了要删除的子树 |
|                    |                | bug4.py - BUG(2) | 在 `union(bst1, bst2)` 中：当键相同时，错误地优先选择 bst2 而不是 bst1 |

### 有效性属性

无论执行了哪个操作（`insert`、`delete`、`find` 和 `union`），二叉搜索树都应始终满足一个有效性属性：*树中的键应该是有序的*——对于树中的每个节点，(1) 其左子树中所有节点的键都小于该节点自身的键，(2) 其右子树中所有节点的键都大于该节点自身的键。

例如，我们可以使用这个有效性属性来检查执行 `insert` 和 `delete` 操作后 BST 的有效性。

```python
# 空树是一棵有效的二叉搜索树。
def test_nil_valid() -> None:
    assert is_valid(BST.nil())

# 插入一个键值对后，结果仍然是一棵有效的 BST。
@given(keys_strategy, st.integers(), trees_strategy)
def test_insert_valid(key: int, value: int, bst: BST[int,int]) -> None:
    assert is_valid(bst.insert(key, value))

# 删除一个键后，结果仍然是一棵有效的 BST。
@given(keys_strategy, trees_strategy)
def test_delete_valid(key: int, bst: BST[int,int]) -> None:
    assert is_valid(bst.delete(key))
```

#### TODO1

(1) 在本节中，你需要在 `lab4/src/BSTUtils.py` 中定义上述有效性属性，该属性检查 BST 中的键是否始终有序。你可以在代码注释中找到一些提示。

(2) 基于你定义的有效性属性，你需要在 `lab4/tests/test1.py` 中编写基于有效性属性的相关代码，分别检查两个核心操作 `find` 和 `union` 是否遵守有效性属性。你可以在代码注释中找到一些提示。

之后，你可以运行以下命令来确认有效性属性是否能成功找到 `lab4/bugs/bug1.py` 中植入的两个错误。

```bash
lab4/tests$ make test1

# 如果你想要更详细的输出结果
lab4/tests$ pytest -v test1.py --tb=short
```

你应该获得以下测试结果，并可以在 `lab4/tests/report/test1` 中找到收缩后的测试：

```tex
Run Validity Testing (test1.py)...
....FF
FAILED test1.py::test_find_valid - assert False
FAILED test1.py::test_union_valid - assert False
2 failed, 4 passed in 1.38s
```

### 后置条件属性

后置条件是在函数/操作完成执行后必须为真的属性或条件。它描述了函数完成时的保证状态或结果。例如，在执行 `insert` 操作后，刚刚插入的键应该存在，并且任何原始键应该保持不变。

```python
# 插入操作不应影响其他键（包括自身）的搜索结果。
@given(keys_strategy, st.integers(), trees_strategy, keys_strategy)
def test_insert_post(key: int, value: int, bst: BST[int,int], search_key:int) -> None:
    found = bst.insert(key, value).find(search_key)
    expected = value if key == search_key else bst.find(search_key)
    assert found == expected
```

对于 `find` 操作，我们也可以提出一些后置条件属性。我们知道，如果我们刚刚插入了一个键，那么树中必须包含该键。同样，如果我们刚刚删除了一个键，那么树中不应该包含该键。因此，我们可以为 `find` 编写两个后置条件属性：

```python
# 插入后，查找应返回插入的值。
@given(st.integers(), st.integers(), trees_strategy)
def test_find_post_present(key: int, value: int, bst: BST[int,int]) -> None:
    assert bst.insert(key, value).find(key) == value

# 删除后，查找应返回 None。
@given(st.integers(), trees_strategy)
def test_find_post_absent(key: int, bst: BST[int,int]) -> None:
    assert bst.delete(key).find(key) is None
```

#### TODO2

你需要在 `lab4/tests/test2.py` 中分别为两个核心操作 `delete` 和 `union` 定义一些后置条件属性。你可以在代码注释中找到一些提示。

之后，你可以运行以下命令来确认你的属性是否能成功找到 `lab4/bugs/bug2.py` 中植入的两个错误。

```bash
lab4/tests$ make test2

# 如果你想要更详细的输出结果
lab4/tests$ pytest -v test2.py --tb=short
```

你应该获得以下测试结果，并可以在 `lab4/tests/report/test2` 中找到收缩后的测试：

```bash
Run Postconditions Testing (test2.py)...
.F.FF                                                                                       
FAILED test2.py::test_find_post_absent - assert 0 is None
FAILED test2.py::test_delete_post - assert 0 == None
FAILED test2.py::test_union_post - assert 1 == 0
3 failed, 2 passed in 4.10s
```

### 蜕变属性

蜕变测试是在许多上下文中解决 oracle 问题的一种成功方法。基本思想是：即使可能难以预测函数调用（如 `insert(key, value)`）的预期结果，我们仍然可以表达此结果与相关调用的结果之间的预期关系。例如，如果在调用 `insert(key, value)` 之前向 BST 中插入一个额外的键，我们期望该额外的键也出现在最终的 BST 中。我们将这种蜕变关系形式化为以下蜕变属性：

```python
# 在 (key1, value1) 和 (key2, value2) 上的两个插入操作与其预期结果之间的蜕变关系
# 如果键相同，则插入 value2（而不是 value1）；否则，同时插入 value1 和 value2。
# 用于确定两个 BST 在包含的（键，值）对方面是否等价的等价函数，忽略树结构之间的差异。
@given(keys_strategy, st.integers(), keys_strategy, st.integers(), trees_strategy)
def test_insert_metamorph_by_insert(key1: int, value1: int, key2: int, value2: int, bst: BST[int,int]) -> None:
    inserted = bst.insert(key1, value1).insert(key2, value2)
    expected = bst.insert(key2, value2) if key1 == key2 else bst.insert(key2, value2).insert(key1, value1)
    assert equivalent(inserted, expected)
```

你可能想知道为什么我们需要在前面的属性中检查 key1 和 key2 是否相同。原因是 `insert` 操作遵循*最后插入者获胜*的原则。因此，以下蜕变关系是有缺陷的，可能导致测试中的误报。

```python
@given(keys_strategy, st.integers(), keys_strategy, st.integers(), trees_strategy)
def test_insert_metamorph_by_insert(key1: int, value1: int, key2: int, value2: int, bst: BST[int,int]) -> None:
    inserted = bst.insert(key1, value1).insert(key2, value2)
    # 未考虑 value1 和 value2 的优先级。
    expected = bst.insert(key2, value2).insert(key1, value1)
    assert equivalent(inserted, expected)
```

#### TODO3

(1) 在本节中，你需要在 `lab4/src/BSTUtils.py` 中定义 `equivalent` 函数，该函数检查两个 BST 在包含的（键，值）对方面是否等价，忽略树结构之间的差异。你可以在代码注释中找到一些提示。

(2) 基于你实现的 `equivalent` 函数，你需要在 `lab4/tests/test3.py` 中分别为操作 `delete` 和 `union` 提出一些蜕变属性，以识别 `lab4/bugs/bug3.py` 中植入的一个错误。你可以在代码注释中找到一些提示。

之后，你可以运行以下命令来确认你的属性是否能成功找到 `lab4/bugs/bug3.py` 中植入的一个错误。请注意，我们只在 `delete` 中植入了一个错误，`union` 是正确的，没有错误。如果你的属性在 `union` 中发现了一些错误，你可能需要仔细检查你的属性定义是否正确。

```python
lab4/tests$ make test3

# 如果你想要更详细的输出结果
lab4/tests$ pytest -v test3.py --tb=short
```

你应该获得以下结果，并可以在 `lab4/tests/report/test3` 中找到收缩后的测试：

```tex
Run Metamorphic Testing (test3.py)...
.F.                                                                                         
FAILED test3.py::test_delete_metamorph_by_insert - assert False
1 failed, 2 passed in 2.62s
```

### 基于模型的属性

1972 年，Tony Hoare 提出了一种证明*数据表示*正确性的方法（参考 *C. A. Hoare. Proof of correctness of data representations. Acta Inf., 1(4):271–281, December 1972*），通过使用*抽象函数*将它们与*抽象数据*联系起来。

在本实验中：

+ 数据表示：BST
+ 抽象函数：`BST::to_list()`
+ 抽象数据：List[Tuple[K,V]]

基于模型的属性通过单次调用来测试单个函数，并将其结果与应用于相关抽象参数的相关*抽象操作*的结果进行比较。*抽象函数*将真实的、具体的参数和结果映射到抽象值，我们也称之为*模型*。

```python
def to_list(self) -> List[Tuple[K,V]]:
        if self.is_leaf():
            return []
        return [self._entry] + self.get_left().to_list()  + self.get_right().to_list()
```

以 `insert` 操作为例：

<div align="center">
<img src="../images/lab4-model-based-property-example.png"
  style="height: auto; width: 50%">
</div>

将数据插入 BST 应该产生一个与将数据插入抽象数据等价的集合。

```python
@given(keys_strategy, st.integers(), trees_strategy)
def test_insert_model(key: int, value: int, bst: BST[int,int]) -> None:
    abstract_data = bst.to_list()
    inserted_abstract_data = abstract_data.copy()
    flag = False
    for i, (k, v) in enumerate(inserted_abstract_data):
        if k == key:
            True
            inserted_abstract_data[i] = (key, value)
            break
    if flag is False:
        inserted_abstract_data += [(key, value)]
    inserted_bst = bst.insert(key, value)
    assert set(inserted_bst.to_list()) == set(inserted_abstract_data)
```

#### TODO4

按照上面关于 `insert` 的示例，你需要分别为操作 `delete` 和 `union` 定义一些基于模型的属性，以识别 `lab4/bugs/bug4.py` 中植入的两个错误：

+ 对于 `delete`，你可以在 BST 和抽象数据结构（例如 `list`）上执行 `delete` 操作，以确定最终的集合是否等价。

+ 对于 `union`，你可以在两个 BST 及其对应的抽象数据结构（例如两个 `list`）上执行 `union` 操作，以确定最终的集合是否等价。

之后，你可以运行以下命令来确认你的属性是否能成功找到 `lab4/bugs/bug4.py` 中植入的两个错误。

```python
lab4/tests$ make test4

# 如果你想要更详细的输出结果
lab4/tests$ pytest -v test4.py --tb=short
```

你应该获得以下结果，并可以在 `lab4/tests/report/test4` 中找到收缩后的测试：

```bash
Run Model-based Properties Testing (test4.py)...
..FF                                                                                      
FAILED test4.py::test_delete_model - assert {(0, 0), (1, 0)} == {(1, 0)}
FAILED test4.py::test_union_model - assert {(0, 1)} == {(0, 0)}
2 failed, 2 passed in 0.99s
```

## 提交

*注意：我们将验证你提交的代码是否针对错误。如果你的代码是针对错误的，你的分数可能会相应扣除。*

完成实验后，通过提交并推送 `lab4/` 下的更改来提交你的代码。具体来说，你需要提交对 `lab4/src/BSTUtils.py`、`lab4/tests/test1.py`、`lab4/tests/test2.py`、`lab4/tests/test3.py`、`lab4/tests/test4.py` 的更改。

```bash
   lab4$ git add src/BSTUtils.py tests/test1.py tests/test2.py tests/test3.py tests/test4.py
   lab4$ git commit -m "你的提交信息"
   lab4$ git push
```