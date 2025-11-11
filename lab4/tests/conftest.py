# conftest.py
import pytest
import os
from pathlib import Path
import random
import hypothesis
import sys

project_root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, project_root)

def pytest_configure(config):
    # 创建report目录
    Path("report").mkdir(exist_ok=True)

@pytest.hookimpl(hookwrapper=True)
def pytest_runtest_makereport(item, call):
    outcome = yield
    report = outcome.get_result()
    
    if report.failed:
        # 获取测试文件名称（不带扩展名）
        test_file = Path(item.fspath).stem
        # 获取测试函数名称
        test_func = item.name
        
        # 创建子目录
        report_dir = Path("report") / test_file
        report_dir.mkdir(exist_ok=True)
        
        # 创建报告文件
        report_file = report_dir / f"{test_func}_failure.txt"
        
        # 收集失败信息
        failure_info = f"""


File and Function: {report.nodeid}

Exception Info:
{call.excinfo}

Failure Message:
{report.longreprtext}

"""
        with open(report_file, 'w', encoding='utf-8') as f:
            f.write(failure_info)