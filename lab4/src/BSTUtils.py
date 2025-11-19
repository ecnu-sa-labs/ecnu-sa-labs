from typing import TypeVar, Protocol, Any, Tuple, List
from abc import abstractmethod
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

# TODO:  the keys in the tree should be ordered --- for every node in the tree
# (1) the key of all nodes in its left subtree is less than the node’s own key 
# (2) the key of all nodes in its right subtree is greater than the node’s own key
def is_valid(bst: BST[K,V]) -> bool:
    return True

# TODO: checks the two BSTs are equivalent in terms of the containing (key, value) pairs while disregarding the differences between tree structures.
# NOTE you can use the BST::to_list() to get all (key, value) pairs in a BST tree.
def equivalent(bst1: BST[K,V], bst2: BST[K,V]) -> bool:
    return True

def insertions(bst: BST[K,V]) -> List[Tuple[K,V]]:
    return bst.to_list()
