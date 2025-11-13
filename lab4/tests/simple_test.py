from src import BST, is_valid
import pytest

def test_new_bst_is_empty():
    bst = BST.nil()
    assert bst.is_empty()
    assert bst.size() == 0

def test_an_inserted_value_can_be_found():
    bst = BST.nil()
    updated = bst.insert(3, "three")
    assert not updated.is_empty()
    assert updated.find(3) == "three"
    assert updated.size() == 1
    assert bst.is_empty()

def test_a_replaced_value_can_be_found():
    bst = BST.nil()
    first = bst.insert(3, "three")
    updated = first.insert(3, "drei")
    assert updated.find(3) == "drei"
    assert first.find(3) == "three"

def test_three_inserted_values_can_be_found():
    bst = BST.nil()
    updated = bst.insert(10, "ten").insert(1, "one").insert(20, "twenty")
    assert not updated.is_empty()
    assert updated.size() == 3
    assert updated.find(1) == "one"
    assert updated.find(10) == "ten"
    assert updated.find(20) == "twenty"
    assert bst.is_empty()

def test_filled_with_same_values_in_same_order_are_equal():
    bst = BST.nil()
    first = bst.insert(10, "ten").insert(1, "one").insert(20, "twenty")
    second = bst.insert(10, "ten").insert(1, "one").insert(20, "twenty")
    assert first == second
    different_order = bst.insert(1, "one").insert(10, "ten").insert(20, "twenty")
    assert first != different_order

def test_to_string_contains_all_keys_with_values():
    bst = BST.nil()
    updated = bst.insert(10, "ten").insert(1, "one").insert(20, "twenty")
    s = repr(updated)
    assert "(10, 'ten')" in s
    assert "(1, 'one')" in s
    assert "(20, 'twenty')" in s

def test_a_deleted_value_can_no_longer_be_found():
    bst = BST.nil()
    ten = bst.insert(10, "ten")
    assert ten.delete(10).find(10) is None

    ten_one = ten.insert(1, "one")
    assert ten_one.delete(10).find(10) is None

    ten_one_twenty = ten_one.insert(20, "twenty")

    deleted1 = ten_one_twenty.delete(1)
    assert deleted1.find(1) is None
    assert deleted1.find(10) == "ten"
    assert deleted1.find(20) == "twenty"

    deleted20 = ten_one_twenty.delete(20)
    assert deleted20.find(20) is None
    assert deleted20.find(1) == "one"
    assert deleted20.find(10) == "ten"

    deleted10 = ten_one_twenty.delete(10)
    assert deleted10.find(10) is None
    assert deleted10.find(1) == "one"
    assert deleted10.find(20) == "twenty"

def test_keys_returns_set_of_inserted_keys():
    bst = BST.nil().insert(1, "one").insert(2, "two").insert(3, "three")
    keys = bst.keys()
    assert set(keys) == {1, 2, 3}

def test_to_list_returns_key_value_pairs():
    bst = BST.nil().insert(1, "one").insert(2, "two").insert(3, "three")
    items = set(bst.to_list())
    expected = {(1, "one"), (2, "two"), (3, "three")}
    assert items == expected

def test_union_of_two_bsts_contains_keys_of_both():
    bst = BST.nil()
    one = bst.insert(1, "one").insert(2, "two").insert(3, "three")
    two = bst.insert(4, "four").insert(5, "five").insert(3, "eerht")
    union = BST.union(one, two)
    # Java semantics: left (one) overrides right (two) for key conflicts
    expected_map = {k: v for k, v in two.to_list()}
    for k, v in one.to_list():
        expected_map[k] = v
    assert set(union.to_list()) == set(expected_map.items())

def test_insert_twice_still_valid():
    bst = BST.nil().insert(1, "one").insert(1, "two")
    assert is_valid(bst)