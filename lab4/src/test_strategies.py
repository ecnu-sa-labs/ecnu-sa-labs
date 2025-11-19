# test_strategies.py
import hypothesis.strategies as st
from typing import List, Tuple, Any

def create_bst_strategies(bst_class):
    
    keys_strategy = st.one_of(
        st.integers(min_value=-25, max_value=25), 
        st.integers()
    )
    
    def build_bst_from_tuples(kv_list: List[Tuple[int, int]]) -> Any:
        bst = bst_class.nil()
        for k, v in kv_list:
            bst = bst.insert(k, v)
        return bst
    
    trees_strategy = st.lists(
        st.tuples(keys_strategy, st.integers()),
        min_size=0,
        max_size=50,
        unique_by=lambda kv: kv[0]
    ).map(build_bst_from_tuples)
    
    return {
        'keys_strategy': keys_strategy,
        'trees_strategy': trees_strategy,
        'build_bst_from_tuples': build_bst_from_tuples
    }