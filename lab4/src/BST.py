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
    
    def delete(self, key:K) -> "BST[K,V]":
        """
        Delete the node with the given key from the Binary Search Tree.
        
        Args:
            key: The key to delete from the BST
            
        Returns:
            A new BST with the specified key removed (if the key exists)
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

    @staticmethod
    def union(bst1: "BST[K,V]", bst2: "BST[K,V]") -> "BST[K,V]":
        """
        Compute the union of two Binary Search Trees.
        
        Args:
            bst1: The first binary search tree
            bst2: The second binary search tree
            
        Returns:
            A new BST containing all key-value pairs from both trees
            When two keys are identical, the values from bst1 overwrite the values from bst2
        """

        # NOTE: When the key-value pair is identical, the value in bst1 shall overwrites the value in bst2.
        result = bst2
        for key, value in bst1.to_list():
            result = result.insert(key, value)
        return result

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

