from typing import TypeVar, Protocol, Any, Tuple, List
from abc import abstractmethod
from hypothesis import strategies as st
import random

from .BST import BST


# Declare generic type variables K denoting the type of keys (comparable) and V denoting the type of values.
class Comparable (Protocol):
    @abstractmethod
    def __eq__(self, other: Any) -> bool: ... # equal
    @abstractmethod
    def __ne__(self, other: Any) -> bool: ... # not equal
    @abstractmethod
    def __lt__(self, other: Any) -> bool: ... # less than
    @abstractmethod
    def __le__(self, other: Any) -> bool: ... # less than or equal
    @abstractmethod
    def __gt__(self, other: Any) -> bool: ... # greater than
    @abstractmethod
    def __ge__(self, other: Any) -> bool: ... # greater than or equal

K = TypeVar('K', bound = Comparable)
V = TypeVar('V')

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

def equivalent(bst1: BST[K,V], bst2: BST[K,V]) -> bool:
    return set(bst1.to_list()) == set(bst2.to_list())

def insertions(bst: BST[K,V]) -> List[Tuple[K,V]]:
    return bst.to_list()
