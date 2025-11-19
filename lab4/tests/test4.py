from typing import List, Tuple
from hypothesis import strategies as st, given, assume, settings
import random

from src import is_valid, equivalent, insertions
from bugs import BSTBug4 as BST
from src import create_bst_strategies

strategies = create_bst_strategies(BST)
keys_strategy = strategies['keys_strategy']
trees_strategy = strategies['trees_strategy']

# ---------------- Model-based Properties Testing ----------------


def nil_model() -> None:
    assert BST.nil().to_list() == []

@given(keys_strategy, trees_strategy)
def test_find_model(key: int, bst: BST[int,int]) -> None:
    abstract_data = bst.to_list()
    for k, v in abstract_data:
        if k == key:
            assert bst.find(key) == v
            break
    else:
        assert bst.find(key) is None

# Inserting data into a BST should yield a set equivalent to inserting data into an abstract data.
@given(keys_strategy, st.integers(), trees_strategy)
def test_insert_model(key: int, value: int, bst: BST[int,int]) -> None:
    abstract_data = bst.to_list()
    inserted_abstract_data = abstract_data.copy()
    flag = False
    for i, (k, v) in enumerate(inserted_abstract_data):
        if k == key:
            flag = True
            inserted_abstract_data[i] = (key, value)
            break
    if flag is False:
        inserted_abstract_data += [(key, value)]
    inserted_bst = bst.insert(key, value)
    assert set(inserted_bst.to_list()) == set(inserted_abstract_data)


# TODO:Perform a delete operation on the BST and abstract data structure, then determine whether the final sets are equivalent.
@given(keys_strategy, trees_strategy)
def test_delete_model(key: int, bst: BST[int,int]) -> None:
    abstract_data = bst.to_list()
    deleted_abstract_data = ...
    deleted_bst = bst.delete(key)
    assert set(deleted_bst.to_list()) == set(deleted_abstract_data)

# TODO:Perform a union operation on two BSTs and their corresponding abstract data structures, then determine whether the resulting union is equivalent.
@given(trees_strategy, trees_strategy)
def test_union_model(bst1: BST[int,int], bst2: BST[int,int]) -> None:
    abstract_data1 = bst1.to_list()
    abstract_data2 = bst2.to_list()
    unioned_abstract_data = ...
    unioned_bst = BST.union(bst1, bst2)
    assert set(unioned_bst.to_list()) == set(unioned_abstract_data)
