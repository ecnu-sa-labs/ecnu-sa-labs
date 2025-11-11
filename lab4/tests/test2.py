from typing import List, Tuple
from hypothesis import strategies as st, given, assume, settings
import random

from src import is_valid, equivalent, insertions
from bugs import BSTBug2 as BST



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


# ---------------- Postconditions Testing ----------------

# After insertion, the lookup should return the inserted value.
@given(st.integers(), st.integers(), trees_strategy)
def test_find_post_present(key: int, value: int, bst: BST[int,int]) -> None:
    assert bst.insert(key, value).find(key) == value

# After deletion, the lookup should return None.
@given(st.integers(), trees_strategy)
def test_find_post_absent(key: int, bst: BST[int,int]) -> None:
    assert bst.delete(key).find(key) is None

# Insertions should not affect the search results of other keys (including themselves).
@given(keys_strategy, st.integers(), trees_strategy, keys_strategy)
def test_insert_post(key: int, value: int, bst: BST[int,int], search_key:int) -> None:
    found = bst.insert(key, value).find(search_key)
    expected = value if key == search_key else bst.find(search_key)
    assert found == expected

# TODO: Deletion should not affect the search results for other keys.
@given(keys_strategy, trees_strategy, keys_strategy)
def test_delete_post(key: int, bst: BST[int,int], search_key: int) -> None:
    found = ...
    expected = ...
    assert found == expected

# TODO: After merging, the lookup should return the correct value, with keys from bst1 taking precedence over keys from bst2.
@given(trees_strategy, trees_strategy, keys_strategy)
def test_union_post(bst1: BST[int,int], bst2: BST[int,int], search_key:int) -> None:
    union_bst: BST[int,int] = BST.union(bst1, bst2)
    found = ...
    expected = ...
    assert found == expected