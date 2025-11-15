# Metamorphic Testing and Property-based Testing

Use property-based testing to validate the implementations of binary search trees.

## Objective

In this lab, you shall apply property-based testing to validate the implementations of binary search trees based on [Hypothesis](https://github.com/HypothesisWorks/hypothesis). Hypothesis is a popular property-based testing tool for Python. Through this lab, you will learn how to define different forms of properties for effective testing.

## Setup

Please install the Python packages `pytest` and `hypothesis`:

```bash
/lab4$ pip install -r requirements.txt  # Install the required packages
```

## Prerequisite knowledge

### Binary Search Tree (BST)

A binary search tree (BST), is a special type of binary tree used for both searching and sorting.

A binary search tree is either an empty tree or a binary tree satisfying the following properties:

+ If its left subtree is non-empty, then all nodes in the left subtree have keys less than the root's key;
+ If its right subtree is non-empty, then all nodes in the right subtree have keys greater than the root's key;
+ Both its left and right subtrees are  binary search trees.

The key characteristic of a BST can be summarized as: `left subtree < root < right subtree`. This ordering guarantees that an in-order traversal—which visits nodes in the order left, root, right—produces a sequence of keys in strictly ascending order, as shown in the following example.

<div align="center">
<img src="../images/lab4-bst-example1.png"
  style="height: auto; width: 50%">
</div>

The fundamental operations of a binary search tree include `insert`, `delete`, `find`, `union`. Its data structure is implemented in `/lab4/src/BST.py`. Should you require clarification on its implementation principles, please refer to [Binary Search Trees & Balanced Trees - OI Wiki](https://oi-wiki.org/ds/bst/).

The following section will cover operations related to binary search trees. Should you be thoroughly familiar with these concepts, you may skip ahead; however, please review the source code in `/lab4/src/BST.py` to ensure you can write the properties appropriately. **You should pay attention to the `NOTE` sections in the source code and the bolded parts of the following operations**, as these pertain to the structure of the binary search tree in this experiment.

#### find

Search for a node with the given `key` in the Binary Search Tree.

1. Search Phase:

   - If `current.key > target.key`: Search recursively in the left subtree (where smaller keys reside)

   - If `current.key < target.key`: Search recursively in the right subtree (where larger keys reside)

2. Termination Cases:

   - Case 1 leaf node reached: Key not found in the search path, return `None`

   - Case 2 key match found: Current node's key equals target key, return the corresponding value

```python
def find(self, key: K) -> Optional[V]:
    """
    Search for a node with the given key in the Binary Search Tree.

    Args:
        key: The key to search for in the BST

    Returns:
        The value associated with the key if found, None otherwise
    """
    if self.is_leaf():
        # Base case: Reached a leaf node without finding the key
        return None

    if self.key() > key:
        # Current node's key is greater than target key
        # Search recursively in the left subtree (where smaller keys reside)
        return self.get_left().find(key)
    elif self.key() < key:
        # Current node's key is less than target key
        # Search recursively in the right subtree (where larger keys reside)
        return self.get_right().find(key)

    # Current node's key matches the target key - search successful
    return self.value()
```

#### insert

Insert a new key-value pair into the Binary Search Tree.

Wrapper Method: `insert` creates a new branch and calls the internal recursive method

Recursive Insertion: `_insert_branch` handles the actual insertion logic while maintaining BST properties

- If `current.key > branch.key`: Insert into left subtree.
- If `current.key < branch.key`: Insert into right subtree.
- If `current.key == branch.key`: **Update the existing key with the new value**.

**The insertion principle in this experiment is "the last insertion wins"**

```python
def insert(self, key: K, value: V) -> "BST[K,V]":
    branch = BST(left = BST.nil(), entry = (key, value), right = BST.nil())
    return self._insert_branch(branch)

def _insert_branch(self, branch: "BST[K,V]") -> "BST[K,V]":
    """
    Internal recursive method to insert a branch into the BST.

    Args:
        branch: The branch (node) to be inserted

    Returns:
        A new BST with the branch inserted at the correct position
    """
    if self.is_leaf():
        # Base case: Reached an empty position, insert the branch here.
        return branch

    if self.key() > branch.key():
        # Current node's key is greater than the branch's key.
        # Insert recursively into the left subtree.
        return BST(
            left = self.get_left()._insert_branch(branch),
            entry = self._entry,
            right = self.get_right()
        )
    elif self.key() < branch.key():
        # Current node's key is less than the branch's key.
        # Insert recursively into the right subtree.
        return BST(
            left = self.get_left(),
            entry = self._entry,
            right = self.get_right()._insert_branch(branch)
        )

    # Current node's key matches the branch's key - update the value
    # If the key is identical, the newly inserted value replaces the original value
    return BST(
        left = self.get_left(),
        entry = branch._entry,
        right = self.get_right()
    )
```

#### delete

Delete the node with the given key from the Binary Search Tree.

1. Search Phase:

   + If `current.key > target.key`: Search and delete recursively in the left subtree

   - If `current.key < target.key`: Search and delete recursively in the right subtree

2. Deletion Cases:

   - Case 1: No left child: Replace node with its right child

   - Case 2: No right child: Replace node with its left child

   - Case 3: Two children: Use the right child as replacement and insert left child into right subtree

3. Two-Child Deletion Strategy:

   - The right child takes the position of the deleted node

   - The entire left subtree is inserted into the right subtree

   - This maintains BST properties since all left subtree values < right subtree values

```python
    def delete(self, key:K) -> "BST[K,V]":
        """
        Delete the node with the given key from the Binary Search Tree.
        
        Args:
            key: The key to delete from the BST
            
        Returns:
            A new BST with the specified key removed (if it existed)
        """
        if self.is_leaf():
            # Base case: Key not found in the tree, return unchanged
            return self
        
        if self.key() > key:
            # Current node's key is greater than target key
            # Delete recursively from the left subtree
            return BST(
                left = self.get_left().delete(key),
                entry = self._entry,
                right = self.get_right()
            )
        
        elif self.key() < key:
            # Current node's key is less than target key
            # Delete recursively from the right subtree
            return BST(
                left = self.get_left(),
                entry = self._entry,
                right = self.get_right().delete(key)
            )
        
        # Current node's key matches the target key - delete this node
        # Case 1: Node has no left child, replace with right child
        if self.get_left().is_leaf():
            return self.get_right()
        # Case 2: Node has no right child, replace with left child
        if self.get_right().is_leaf():
            return self.get_left()
        # Case 3: Node has two children
        # Replace the current node with the right child 
        # and insert the left child into the right child's subtree
        return self.get_right()._insert_branch(self.get_left())
```

#### union

Compute the union of two Binary Search Trees.

1. Initialization:
   - Start with `bst2` as the base result tree
   - This provides all elements from the second tree as a foundation
2. Merge Process:
   - Convert `bst1` to a list using `to_list()` method
   - Iterate through each key-value pair in `bst1`
   - Insert each pair into the result tree using the `insert` method
3. Conflict Resolution:
   - When the same key exists in both trees, the value from `bst1` **replaces** the value from `bst2`
   - **This follows the specified precedence rule: bst1 values take priority**

```python
@staticmethod
def union(bst1: "BST[K,V]", bst2: "BST[K,V]") -> "BST[K,V]":
    """
    Compute the union of two Binary Search Trees.

    Args:
        bst1: The first binary search tree
        bst2: The second binary search tree

    Returns:
        A new BST containing all key-value pairs from both trees
        When keys conflict, values from bst1 take precedence over bst2
    """

    # NOTE: When the key-value pair is identical, the value in BST1 shall replace the value in BST2.
    result = bst2
    for key, value in bst1.to_list():
        result = result.insert(key, value)
    return result
```

### Hypothesis

Hypothesis is a Python testing library based on property-based testing. It verifies general properties of code by automatically generating test data, rather than relying solely on specific single test cases.

```python
# Traditional testing
def test_sort_example():
    list_a = [3, 1, -1] # feed single input
    output = sort(list_a)
    expected_output = [-1, 1, 3]
    assert output == expected_output # check single output
    
# Hypothesis property-based test
# Input generator automatically creates various lists.
@given(st.lists(st.integers(), min_size=0, max_size=1000))
def test_sort_property(input_list):
    # Pre-condition
    assume(len(input_list) > 1)
    
	sorted_list = sort(input_list)
    # Post-condition: Verify the sorting property holds
    for i in range(len(sorted_list) - 1):
        assert sorted_list[i] <= sorted_list[i + 1]
```

#### Strategies

Strategies form the core of Hypothesis, serving to generate test data:

```python
# Basic Type
st.integers()           # integer
st.floats()             # floating-point number  
st.text()               # Text
st.booleans()           # Boolean value

# Complex type
st.lists(st.integers()) # List of integers
st.dictionaries(st.text(), st.integers())  # Dictionary
st.tuples(st.integers(), st.text())        # Tuple

# Custom policy
st.one_of(st.integers(), st.text())        # One of several types
```

In this experiment, the following strategies have been designed for you:

+ Keys: `keys_strategy`, predominantly selected from the range [-25, 25] to ensure test keys predominantly appear in the tree, primarily testing core functionality; with a minor probability of selecting random integers to account for edge cases.
+ Values: `st.integers()`, utilising random integers.
+ BST: `trees_strategy`, executes insert operations using the generated [Key, Value] pairs to construct the tree, with node count constrained between [0,50] and ensuring key uniqueness.

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

#### Assumptions

In property-based testing, assumptions may be employed to constrain the range or attributes of input data. If an assumption is unsatisfied, Hypothesis will automatically generate other input data that satisfies the assumption for testing. Assumptions are typically declared using the `assume()` function.

```python
from hypothesis import given, assume

@given(st.integers())
def test_positive_numbers(a):
    assume(a > 0)  # Pre-condition: Ensure that a must be greater than zero.
    assert is_positive(a)
```

#### Shrinking

Hypothesis also offers a feature called shrinking. When a property test fails, Hypothesis attempts to find a smaller input data set to make it easier to understand why the test failed.

### Pytest

pytest is a powerful testing framework for Python that makes writing and running tests simple and scalable. It automatically discovers test files and functions, provides detailed failure reports, and supports various plugins for extended functionality.

`simple_test.py` performs simple testing on the BST in `lab4\bugs\bug1.py`. You may utilise pytest as follows:

```bash
\lab4\tests$ pytest simple_test.py -v --tb=short # Detailed output results
\lab4\tests$ pytest simple_test.py -q --tb=no # Concise output results
```

The results of the operation are as follows:

```tex
$ pytest simple_test.py -q --tb=no
...F..F..F.                                                                        [100%]
================================ short test summary info =================================
FAILED simple_test.py::test_three_inserted_values_can_be_found - AssertionError: assert None == 'twenty'
FAILED simple_test.py::test_a_deleted_value_can_no_longer_be_found - AssertionError: assert None == 'twenty'
FAILED simple_test.py::test_union_of_two_bsts_contains_keys_of_both - AssertionError: assert {(1, 'one'), ..., (5, 'five')} == {(1, 'one'), ..., (5, 'five')}
3 failed, 8 passed in 0.09s
```

## Lab Instructions

### Project Structure

```tex
.
|-- bugs
|   |-- __init__.py
|   |-- bug1.py # Bugs has been inserted into both find and union for detection by test1.
|   |-- bug2.py # Bugs has been inserted into both the delete and union for detection by test2.
|   |-- bug3.py # Bug has been introduced in the delete function for detection by test3.
|   `-- bug4.py # Bugs has been inserted into both the delete and union operations for detection by test4.
|-- requirements.txt
|-- src
|   |-- BST.py # Correct implementation of the BST data structure
|   |-- BSTUtils.py # Related utility functions
|   `-- __init__.py
`-- tests
    |-- conftest.py # Runtime Environment Configuration and Test Report Generation
    |-- hypothesis.ini # Hypothesis configuration
    |-- makefile # Run the scripts, including all, clean, test1, test2, test3, test4
    |-- simple_test.py # simple test for BST
    |-- test1.py # TODO: Write Validity Properties tests for find and delete.
    |-- test2.py # TODO: Write Postcondition Properties tests for delete and union.
    |-- test3.py # TODO: Write Metamorphic Properties tests for delete and union.
    `-- test4.py # TODO: Write Model-based Properties tests for delete and union.
```

### Bugs

| TODO      | Property Type            | Target Methods    | Bugs to Identify     | Bug Description                                              |
| :-------- | :----------------------- | :---------------- | :------------------- | :----------------------------------------------------------- |
| **TODO1** | Validity Properties      | `find`, `union`   | **Bug1.py - BUG(1)** | In `find`: During search, right subtree mistakenly treated as left subtree |
|           |                          |                   | **Bug1.py - BUG(2)** | In `union`: Does not compare keys for merging, directly attaches as left subtree, violating BST property |
| **TODO2** | Postcondition Properties | `delete`, `union` | **Bug2.py - BUG(1)** | In `delete`: Path error when locating the key                |
|           |                          |                   | **Bug2.py - BUG(2)** | In `union`: Values in bst2 merged into bst1, but keys in bst2 take precedence (contrary to design) |
| **TODO3** | Metamorphic Properties   | `delete`, `union` | **Bug3.py - BUG(1)** | In `delete`: Path error when locating the key                |
| **TODO4** | Model-based Properties   | `delete`, `union` | **Bug4.py - BUG(1)** | In `delete`: Seeking the opposite path                       |
|           |                          |                   | **Bug4.py - BUG(2)** | In `union`: Values in bst2 merged into bst1, keys in bst2 take precedence over bst1 |

### Validity Testing

#### Knowledge of Validity Properties

Many data-structures need to satisfy **invariant properties**, above and beyond being well-typed, and binary search trees are no exception: the keys in the tree should be ordered. In this section, we shall see how to write properties that check that this invariant is preserved by each operation.

We find that a **invariant property: all the keys in a left subtree must be less than the key in the node, and all the keys in the right subtree must be greater**.

You can verify whether a binary search tree is satisfy the invariant properties using the `is_valid` function in `lab4\src\BSTUtils.py`. 

```python
def is_valid(bst: BST[K,V]) -> bool:
    if bst.is_leaf():
        return True
    root_key = bst.key()
    left = bst.get_left()
    right = bst.get_right()

    if not is_valid(left) or not is_valid(right):
        return False
    if any(k > root_key for k in left.keys()):
        return False
    if any(k < root_key for k in right.keys()):
        return False
    return True
```

Now it is straightforward to define properties that check that every operation that constructs a tree, constructs a valid one:

Clearly, we must verify that the tree remains valid after inserting a value node or removing a node.

```python
# An empty tree is a valid binary search tree.
def test_nil_valid() -> None:
    assert is_valid(BST.nil())


# After inserting a key-value pair, the result remains a valid BST.
@given(keys_strategy, st.integers(), trees_strategy)
def test_insert_valid(key: int, value: int, bst: BST[int,int]) -> None:
    # assume(is_valid(bst))
    assert is_valid(bst.insert(key, value))

# After deleting a key, the result remains a valid BST.
@given(keys_strategy, trees_strategy)
def test_delete_valid(key: int, bst: BST[int,int]) -> None:
    # assume(is_valid(bst))
    assert is_valid(bst.delete(key))
```

However, these properties, by themselves, do not provide good testing for validity. To see why, let us plant a bug in `insert`, so that it creates duplicate entries when inserting a key that is already present. Property `insert_valid` fails as it should, but so do `delete_valid`. But there is no bugs in `delete`, because the original tree was invalid, so these reported failures are **"false positives"**.

Therefore, before verifying the validity of an operation, it is necessary to ensure that the tree being operated upon is valid, i.e. `assume(is_valid(bst))`, which would discard invalid test cases (not satisfying the precondition) without running them, and thus make the properties pass. 

```python
@given(keys_strategy, trees_strategy)
def test_delete_valid(key: int, bst: BST[int,int]) -> None:
    assume(is_valid(bst))
    assert is_valid(bst.delete(key))
```

This is potentially inefficient (we might spend much of our testing time discarding test cases), but it is also really just applying a sticking plaster: what we want is that all generated trees should be valid! We can test this by defining an additional property:

```python
@given(trees_strategy)
def test_strategy_valid(bst: BST[int,int]) ->None:
    assert(is_valid(bst))
```

which at first sight seems to be testing that all trees are valid, but in fact tests that all trees generated by the Arbitrary instance are valid. If this property fails, then it is the generator that needs to be fixed — there is no point in looking at failures of other properties, as they are likely caused by the failing generator. Usually the generator for a type is intended to fulfill its invariant, but — as in this case — is defined independently. A property such as `test_strategy_valid` is essential to check that these invariant properties are mutually consistent.

**Summary: Validity testing consists of defining a function to check the invariants of your datatypes, writing properties to test that your generators and shrinkers only produce valid results, and writing a property for each function under test that performs a single random call, and checks that the return value is valid.**

#### Need To Do

Now, write **Validity Properties** tests for `find` and `delete` respectively to identify tow bugs in `\bugs\bug1.py`.

After that, Run the following command to detect whether the two bugs in `lab4\bugs\bug1` concerning `find` and `union` have been identified.

```bash
lab4\tests$ make test1
lab4\tests$ pytest -v test1.py --tb=short # More detailed output
```

You should obtain the following result and get the assert information and sharking test seed in `lab4\report\test1`:

```tex
Run Validity Testing (test1.py)...
...FF
FAILED test1.py::test_find_valid - assert False
FAILED test1.py::test_union_valid - assert False
2 failed, 3 passed in 1.12s
```

### Postconditions Testing

#### Knowledge of Postconditions Testing

A postcondition is a property that should be True after a call, or (equivalently, for a pure function) True of its result. Thus, we can define properties by asking ourselves “What should be True after calling f?”. For example, after calling `insert`, then we should be able to `find the key just inserted, and any previously inserted keys with unchanged values.

```python
# Insertions should not affect the search results of other keys (including themselves).
@given(keys_strategy, st.integers(), trees_strategy, keys_strategy)
def test_insert_post(key: int, value: int, bst: BST[int,int], search_key:int) -> None:
    found = bst.insert(key, value).find(search_key)
    expected = value if key == search_key else bst.find(search_key)
    assert found == expected
```

When considering how to design the postcondition for `find`, an obvious approach is to return the corresponding value if the key exists in the tree, or return None otherwise. However, this would be indistinguishable from replicating the `find` functionality itself, and replicating existing functionality is costly and counterproductive. Therefore, **avoid replicating your code in your tests.** We can finesse this problem using a very powerful and general idea, that of constructing a test case whose outcome is easy to predict. In this case, we know that a tree must contain a key `key`, if we have just inserted it. Likewise, we know that a tree cannot contain a key `key`, if we have just deleted it. Thus we can write two postconditions for `find`, covering the two cases:

```python
# After insertion, the lookup should return the inserted value.
@given(st.integers(), st.integers(), trees_strategy)
def test_find_post_present(key: int, value: int, bst: BST[int,int]) -> None:
    assert bst.insert(key, value).find(key) == value

# After deletion, the lookup should return None.
@given(st.integers(), trees_strategy)
def test_find_post_absent(key: int, bst: BST[int,int]) -> None:
    assert bst.delete(key).find(key) is None
```

**Summary: A postcondition tests a single function, calling it with random arguments, and checking an expected relationship between its arguments and its result.**

#### Need To Do

Following the approach of `insert`，Write **Postcondition Properties** tests for `delete` and `union` respectively to identify tow bugs in `\bugs\bug2.py`.

For `delete`, deletion should not affect the search results for other keys.

For `union`, after merging, the lookup should return the correct value, with keys from bst1 taking precedence over keys from bst2.

After that, Run the following command to detect whether the two bugs in `lab4\bugs\bug2` concerning `delete` and `union` have been identified.

```bash
lab4\tests$ make test2
lab4\tests$ pytest -v test2.py --tb=short # More detailed output
```

You should obtain the following result and get the assert information and sharking test seed in `lab4\report\test2`:

Note: no bug has been introduced in the find function; the incorrect find determination is due to an error in the delete operation.

```bash
Run Postconditions Testing (test2.py)...
.F.FF                                                                                       
FAILED test2.py::test_find_post_absent - assert 0 is None
FAILED test2.py::test_delete_post - assert 0 == None
FAILED test2.py::test_union_post - assert 1 == 0
3 failed, 2 passed in 4.10s
```

**Warning: We shall verify whether your code logic complies with the specifications outlined in TODO. Should your approach be bug-oriented or answer-oriented, points will be deducted accordingly.**

### Metamorphic Testing

#### Knowledge of Metamorphic Testing

Metamorphic testing is a successful approach to the oracle problem in many contexts. The basic idea is this: even if the expected result of a function call such as `tree.insert(key, value)` may be difficult to predict, we may still be able to express an expected relationship between this result, and the result of a related call. In this case, if we insert an additional key into `tree` before calling `insert(key, value)`, then we expect the additional key to appear in the result also. We formalize this as the following metamorphic property:

```python
# The relationship between the two insertion operations on (key1, value1) and (key2, value2) and the expected result
# If the key is identical, insert value2 (rather than value1); otherwise, insert both value1 and value2.
# the equivalent determining whether trees are equivalent while disregarding their structure
@given(keys_strategy, st.integers(), keys_strategy, st.integers(), trees_strategy)
def test_insert_metamorph_by_insert(key1: int, value1: int, key2: int, value2: int, bst: BST[int,int]) -> None:
    inserted = bst.insert(key1, value1).insert(key2, value2)
    expected = bst.insert(key2, value2) if key1 == key2 else bst.insert(key2, value2).insert(key1, value1)
    assert equivalent(inserted, expected)
```

`equivalent` is implemented in `lab4\src\BSTUtils`, serving to determine whether trees are equivalent while **disregarding their structural composition**.

```python
def equivalent(bst1: BST[K,V], bst2: BST[K,V]) -> bool:
    return set(bst1.to_list()) == set(bst2.to_list())
```

Note that the following logic is incorrect; please carefully compare it with the distinctions outlined above, The insertion principle in this experiment is **"the last insertion wins"**:

```python
@given(keys_strategy, st.integers(), keys_strategy, st.integers(), trees_strategy)
def test_insert_metamorph_by_insert(key1: int, value1: int, key2: int, value2: int, bst: BST[int,int]) -> None:
    inserted = bst.insert(key1, value1).insert(key2, value2)
    # The priority of value1 and value2 has not been considered.
    expected = bst.insert(key2, value2).insert(key1, value1)
    assert equivalent(inserted, expected)
```

**Summary: A metamorphic property tests a single function by making (usually) two related calls, and checking the expected relationship between the two results.**

#### Need To Do

Following the approach of `insert`, write **Metamorphic Properties** tests for `delete` and `union` respectively to identify one bugs in `\bugs\bug3.py`.

For `delete`: To verify the outcome of the delete operation, one may establish a relationship between `delete.insert` and `insert.delete` by **adding an insert**. That is, by inserting before and after the deletion, one can determine whether the tree structures are equivalent.

For `union`: To verify the outcome of the union operation, one may establish a relationship by **adding an insert**. **Please note that we have not introduced bugs into the union, but the metamorphic properties of this union are prone to errors in implementation. Correct properties will not detect bugs.**

After that, Run the following command to detect whether the two bugs in `lab4\bugs\bug3` concerning `delete` and `union` have been identified.

```python
lab4\tests$ make test3
lab4\tests$ pytest -v test3.py --tb=short # More detailed output
```

You should obtain the following result and get the assert information and sharking test seed in `lab4\report\test3`:

```tex
Run Metamorphic Testing (test3.py)...
.F.                                                                                         
FAILED test3.py::test_delete_metamorph_by_insert - assert False
1 failed, 2 passed in 2.62s
```

**Warning: We shall verify whether your code logic complies with the specifications outlined in TODO. Should your approach be bug-oriented or answer-oriented, points will be deducted accordingly.**

### Model-based Properties Testing

In 1972, Hoare published an approach to proving the correctness of **data representations**(C. A. Hoare. Proof of correctness of data representations. Acta Inf., 1(4):271–281, December 1972.), by relating them to **abstract data** using **an abstraction function**.

In this experiment:

+ date representation: BST
+ abstraction function: `BST::to_list()`
+ abstract data: List[Tuple[K,V]]

```python
def to_list(self) -> List[Tuple[K,V]]:
        if self.is_leaf():
            return []
        return [self._entry] + self.get_left().to_list()  + self.get_right().to_list()
```

Taking the `insert` operation as an example:

<div align="center">
<img src="../images/lab4-model-based-property-example.png"
  style="height: auto; width: 50%">
</div>

Inserting data into a BST should yield a set equivalent to inserting data into an abstract data.

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

**Summary: A model-based property tests a single function by making a single call, and comparing its result to the result of a related “abstract operation” applied to related abstract arguments. An abstraction functions maps the real, concrete arguments and results to abstract values, which we also call the “model”.**

#### Need To Do

Following the approach of `insert`，Write **Model-based Properties** tests for delete and union respectively to identify one bugs in `\bugs\bug4.py`:

+ For `delete`, Perform a delete operation on the BST and abstract data structure, then determine whether the final sets are equivalent.

+ For `union`, Perform a union operation on two BSTs and their corresponding abstract data structures, then determine whether the resulting union is equivalent.

After that, Run the following command to detect whether the two bugs in `lab4\bugs\bug2` concerning `delete` and `union` have been identified.

```python
lab4\tests$ make test4
lab4\tests$ pytest -v test4.py --tb=short # More detailed output
```

You should obtain the following result and get the assert information and sharking test seed in `lab4\report\test4`:

```bash
Run Model-based Properties Testing (test4.py)...
..FF                                                                                      
FAILED test4.py::test_delete_model - assert {(0, 0), (1, 0)} == {(1, 0)}
FAILED test4.py::test_union_model - assert {(0, 1)} == {(0, 0)}
2 failed, 2 passed in 0.99s
```

**Warning: We shall verify whether your code logic complies with the specifications outlined in TODO. Should your approach be bug-oriented or answer-oriented, points will be deducted accordingly.**

## Submission

Once you are done with the lab, submit your code by commiting and pushing the changes under `lab2/`. Specifically, you need to submit the changes to `lab4/tests/test1.py` , `lab4/tests/test2.py` , `lab4/tests/test4.py` ,  `lab4/tests/test4.py` .

```bash
   lab4$ git add tests/test1.py tests/test2.py tests/test3.py tests/test4.py
   lab4$ git commit -m "your commit message here"
   lab4$ git push
```

