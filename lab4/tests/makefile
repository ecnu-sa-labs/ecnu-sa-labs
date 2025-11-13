# Configuration
PYTEST := pytest
PYTHON := python3

# Test file
TEST_FILES := test1.py test2.py test3.py test4.py
ALL_TESTS := $(TEST_FILES)

.PHONY: all test1 test2 test3 test4 clean

## Run all test files
all:
	@echo "Run all test files..."
	@for test in $(TEST_FILES); do \
		echo "=============================================="; \
		echo "Operational testing: $$test"; \
		$(PYTEST) $$test -q --tb=no || true; \
		echo "=============================================="; \
	done
	@echo "All tests have been completed!"

## Run test1.py (Validity Testing)
test1:
	@echo "Run Validity Testing (test1.py)..."
	@$(PYTEST) test1.py -q --tb=no || true

## Run test2.py (Postconditions Testing)
test2:
	@echo "Run Postconditions Testing (test2.py)..."
	@$(PYTEST) test2.py -q --tb=no || true

## Run test3.py (Metamorphic Testing)
test3:
	@echo "Run Metamorphic Testing (test3.py)..."
	@$(PYTEST) test3.py -q --tb=no || true

# Run test4.py (Model-based Properties Testing)
test4:
	@echo "Run Model-based Properties Testing (test4.py)..."
	@$(PYTEST) test4.py -q --tb=no || true

## Clear cache files
clean:
	@echo "Clear test cache..."
	@find . -name "*.pyc" -delete
	@find . -name "__pycache__" -type d -exec rm -rf {} + 2>/dev/null || true
	@find . -name ".pytest_cache" -type d -exec rm -rf {} + 2>/dev/null || true
	@find . -name ".hypothesis" -type d -exec rm -rf {} + 2>/dev/null || true
	@rm -rf report
	@find ../bugs -name "*.pyc" -delete 2>/dev/null || true
	@find ../bugs -name "__pycache__" -type d -exec rm -rf {} + 2>/dev/null || true
	@find ../src -name "*.pyc" -delete 2>/dev/null || true
	@find ../src -name "__pycache__" -type d -exec rm -rf {} + 2>/dev/null || true
	@echo "Clean-up completed"