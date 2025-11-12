from typing import List, Tuple
from hypothesis import strategies as st, given, assume, settings
import random

from src import is_valid, equivalent, insertions
from bugs import BSTBug3 as BST


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


# ---------------- Metamorphic Testing ----------------

# The relationship between the two insertion operations on (key1, value1) and (key2, value2) and the expected result
# If the key is identical, insert value2 (rather than value1); otherwise, insert both value1 and value2.
# the equivalent determining whether trees are equivalent while disregarding their structure
@given(keys_strategy, st.integers(), keys_strategy, st.integers(), trees_strategy)
def test_insert_metamorph_by_insert(key1: int, value1: int, key2: int, value2: int, bst: BST[int,int]) -> None:
    inserted = bst.insert(key1, value1).insert(key2, value2)
    expected = bst.insert(key2, value2) if key1 == key2 else bst.insert(key2, value2).insert(key1, value1)
    assert equivalent(inserted, expected)

# TODO: establish a relationship between `delete.insert` and `insert.delete` by adding an insert. That is, by inserting before and after the deletion, one can determine whether the tree structures are equivalent.
# NOTE: Take care to assess the scenario where key1 equals key2.
@given(keys_strategy, keys_strategy, st.integers(), trees_strategy)
def test_delete_metamorph_by_insert(key1: int, key2: int, value: int, bst: BST[int,int]) -> None:
    deleted = ...
    expected = ...
    assert equivalent(deleted, expected)

# TODO: To verify the outcome of the union operation, one may establish a relationship by adding an insert.
# NOTE: Please note that we have not introduced bugs into the union, but the metamorphic properties of this union are prone to errors in implementation. Correct properties will not detect bugs.
@given (keys_strategy, st.integers(), trees_strategy, trees_strategy)
def test_union_metamorph_by_insert(key: int, value: int, bst1: BST[int,int], bst2: BST[int,int]) -> None:
    unioned = ...
    expected = ...
    assert equivalent(unioned, expected)