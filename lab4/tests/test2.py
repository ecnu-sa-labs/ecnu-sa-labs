from typing import List, Tuple
from hypothesis import strategies as st, given, assume, settings
import random

from src import is_valid, equivalent, insertions
from bugs import BSTBug2 as BST
from src import create_bst_strategies

strategies = create_bst_strategies(BST)
keys_strategy = strategies['keys_strategy']
trees_strategy = strategies['trees_strategy']




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