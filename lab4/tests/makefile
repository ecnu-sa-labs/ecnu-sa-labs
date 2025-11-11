# 配置
PYTEST := pytest
PYTHON := python3

# 测试文件
TEST_FILES := test1.py test2.py test3.py test4.py
ALL_TESTS := $(TEST_FILES)

.PHONY: all test1 test2 test3 test4 clean

## 运行所有测试文件
all:
	@echo "🚀 运行所有测试文件..."
	@for test in $(TEST_FILES); do \
		echo "=============================================="; \
		echo "运行测试: $$test"; \
		$(PYTEST) $$test -q --tb=no || true; \
		echo "=============================================="; \
	done
	@echo "✅ 所有测试执行完成!"

## 运行test1.py (属性测试)
test1:
	@echo "🔬 运行属性测试 (test1.py)..."
	@$(PYTEST) test1.py -q --tb=no || true

## 运行test2.py (变形测试)
test2:
	@echo "🔄 运行变形测试 (test2.py)..."
	@$(PYTEST) test2.py -q --tb=no || true

## 运行test3.py (复杂变形测试)
test3:
	@echo "🎯 运行复杂变形测试 (test3.py)..."
	@$(PYTEST) test3.py -q --tb=no || true

# 运行test4.py (基于模型测试)
test4:
	@echo "📊 运行基于模型测试 (test4.py)..."
	@$(PYTEST) test4.py -q --tb=no || true

## 清理缓存文件
clean:
	@echo "🧹 清理测试缓存..."
	@find . -name "*.pyc" -delete
	@find . -name "__pycache__" -type d -exec rm -rf {} + 2>/dev/null || true
	@find . -name ".pytest_cache" -type d -exec rm -rf {} + 2>/dev/null || true
	@find . -name ".hypothesis" -type d -exec rm -rf {} + 2>/dev/null || true
	@rm -rf report
	@find ../bugs -name "*.pyc" -delete 2>/dev/null || true
	@find ../bugs -name "__pycache__" -type d -exec rm -rf {} + 2>/dev/null || true
	@find ../src -name "*.pyc" -delete 2>/dev/null || true
	@find ../src -name "__pycache__" -type d -exec rm -rf {} + 2>/dev/null || true
	@echo "✅ 清理完成"