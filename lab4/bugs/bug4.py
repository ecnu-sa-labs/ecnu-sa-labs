from typing import Generic, TypeVar, Any, Optional, Tuple, Protocol, List
from abc import abstractmethod


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


class BST(Generic[K, V]):

    _NIL: Optional["BST"] = None

    def __init__(self,
                 left: Optional["BST[K,V]"] = None,
                 entry: Optional[Tuple[K, V]] = None,
                 right: Optional["BST[K,V]"] = None):
        self._left = left
        self._entry = entry
        self._right = right

    @classmethod
    def nil(cls) -> "BST[K,V]":
        if cls._NIL is None:
            cls._NIL = BST()
        return cls._NIL
    
    def get_left(self) -> Optional["BST[K,V]"]:
        return self._left if self._left is not None else BST.nil()
    
    def get_right(self) -> Optional["BST[K,V]"]:
        return self._right if self._right is not None else BST.nil()
    
    def is_leaf(self) -> bool:
        return self._entry is None
    
    def is_empty(self) -> bool:
        return self.is_leaf()
    
    def size(self) -> int:
        if self.is_leaf():
            return 0
        return 1 + self.get_left().size() + self.get_right().size()
    
    def key(self) -> Optional[K]:
        return None if self.is_leaf() else self._entry[0]
    
    def value(self) -> Optional[V]:
        return None if self.is_leaf() else self._entry[1]
    
    def find(self, key: K) -> Optional[V]:
        if self.is_leaf():
            return None
        
        if self.key() > key:
            return self.get_left().find(key)
        elif self.key() < key:
            return self.get_right().find(key)
        
        return self.value()
    
    def insert(self, key: K, value: V) -> "BST[K,V]":
        branch = BST(left = BST.nil(), entry = (key, value), right = BST.nil())
        return self._insert_branch(branch)
    
    def _insert_branch(self, branch: "BST[K,V]") -> "BST[K,V]":
        if self.is_leaf():
            return branch
        
        if self.key() > branch.key():
            return BST(
                left = self.get_left()._insert_branch(branch),
                entry = self._entry,
                right = self.get_right()
            )
        elif self.key() < branch.key():
            return BST(
                left = self.get_left(),
                entry = self._entry,
                right = self.get_right()._insert_branch(branch)
            )
        
        return BST(
            left = self.get_left(),
            entry = branch._entry,
            right = self.get_right()
        )
    
    def delete(self, key:K) -> "BST[K,V]":
        if self.is_leaf():
            return self
        
        if self.key() > key:
            # BUG(1): Seeking the opposite path
            return BST(
                left = self.get_left(),
                entry = self._entry,
                right = self.get_right().delete(key)
            )
            # return BST(
            #     left = self.get_left().delete(key),
            #     entry = self._entry,
            #     right = self.get_right()
            # )
        
        elif self.key() < key:
            # BUG(1): Seeking the opposite path
            return BST(
                left = self.get_left().delete(key),
                entry = self._entry,
                right = self.get_right()
            )
            # return BST(
            #     left = self.get_left(),
            #     entry = self._entry,
            #     right = self.get_right().delete(key)
            # )
        
        if self.get_left().is_leaf():
            return self.get_right()
        if self.get_right().is_leaf():
            return self.get_left()
        
        return self.get_right()._insert_branch(self.get_left())

    @staticmethod
    def union(bst1: "BST[K,V]", bst2: "BST[K,V]") -> "BST[K,V]":

        # BUG(2): The values in bst2 are merged into bst1, keys in bst2 take precedence over keys in bst1 (contrary to design intent)
        result = bst1
        for key, value in bst2.to_list():
            result = result.insert(key, value)
        return result
    
        # result = bst2
        # for key, value in bst1.to_list():
        #     result = result.insert(key, value)
        # return result

    def keys(self) -> List[K]:
        if self.is_leaf():
            return []
        return [self.key()] + self.get_left().keys() + self.get_right().keys()
    
    def to_list(self) -> List[Tuple[K,V]]:
        if self.is_leaf():
            return []
        return [self._entry] + self.get_left().to_list()  + self.get_right().to_list()
    
    def __eq__(self, other: object) -> bool:
        if self is other:
            return True
        if not isinstance(other, BST):
            return False
        return (self.to_list() == other.to_list() and
                self._entry == other._entry and
                self.get_left() == other.get_left())
    
    def __hash__(self) -> int:
        if self.is_leaf():
            return 0
        hash_code = self.get_left().__hash__()
        hash_code = 31 * hash_code + hash(self._entry)
        hash_code = 31 * hash_code + self.get_right().__hash__()
        return hash_code
    
    def __repr__(self) -> str:
        if self.is_leaf():
            return "NIL"
        return f"BST({self._entry}, {self.get_left().__repr__()}, {self.get_right().__repr__()})"

