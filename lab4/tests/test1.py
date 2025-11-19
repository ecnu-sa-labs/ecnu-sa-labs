from typing import List, Tuple
from hypothesis import strategies as st, given, assume, settings
import random

from src import is_valid
from bugs import BSTBug1 as BST
from src import create_bst_strategies

strategies = create_bst_strategies(BST)
keys_strategy = strategies['keys_strategy']
trees_strategy = strategies['trees_strategy']


# ---------------- Validity Testing ----------------

# An empty tree is a valid binary search tree.
def test_nil_valid() -> None:
    assert is_valid(BST.nil())


# After inserting a key-value pair, the result remains a valid BST.
@given(keys_strategy, st.integers(), trees_strategy)
def test_insert_valid(key: int, value: int, bst: BST[int,int]) -> None:
    assert is_valid(bst.insert(key, value))

# After deleting a key, the result remains a valid BST.
@given(keys_strategy, trees_strategy)
def test_delete_valid(key: int, bst: BST[int,int]) -> None:
    assert is_valid(bst.delete(key))

# Ensure the generator is effective
@given(trees_strategy)
def test_strategy_valid(bst: BST[int,int]) ->None:
    assert(is_valid(bst))

# TODO: After locating a key, the result remains valid in a BST.
@given(keys_strategy, trees_strategy)
def test_find_valid(key: int, bst: BST[int,int]) -> None:
    ...

# TODO: After merging two binary search trees, the result remains a valid binary search tree.
@given(trees_strategy, trees_strategy)
def test_union_valid(bst1: BST[int,int], bst2: BST[int,int]) -> None:
    ...


