from typing import List, Tuple
from hypothesis import strategies as st, given, assume, settings
import random

from src import is_valid
from bugs import BSTBug1 as BST


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

def build_eqivalent_bst_from_tuples(kv_list: List[Tuple[int,int]]) -> Tuple[BST[int,int], BST[int,int]]:
    bst1: BST[int,int] = BST.nil()
    for k, v in kv_list:
        bst1 = bst1.insert(k, v)
    random.shuffle(kv_list)
    bst2: BST[int,int] = BST.nil()
    for k, v in kv_list:
        bst2 = bst2.insert(k, v)
    return [bst1, bst2]

equivalent_trees_strategy = st.lists(
    st.tuples(keys_strategy, st.integers()),
    min_size = 0,
    max_size = 50,
    unique_by = lambda kv: kv[0]
).map(build_eqivalent_bst_from_tuples)


# ---------------- Validity Testing ----------------

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


