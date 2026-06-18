from pathlib import Path

# ------------------------pdf元数据，不要的参数可以删除--------------------------
# toc                               是否生成目录
# toc-own-page                      目录是否单独一页
# titlepage                         是否生成封面
# ====以下2个必须和--top-level-division="chapter"同时删除，删除后，每个lab结束时将无法自动分页====
# book                              是否采用书籍排版（每个大标题分页）
# classoption: [oneside]            单栏排版
# =======================================================================================
YAML_HEADER = '''---
title: "软件分析与测试实验手册"
author: [ecnu-sa-labs]
date: "2026-06-01"
lang: "zh"
toc: true
toc-own-page: true
titlepage: true
book: true
classoption: [oneside]
...

'''

# 以下文件的根目录
BASE_FOLDER = "lab_manual_cn"

# 用于生成pdf的文件名，严格按顺序排列
MARKDOWN_FILE_ARRAY = [
    'course-vm.md',
    'lab1.md',
    'lab2.md',
    'lab3.md',
    'lab4.md',
    'lab5.md',
    'lab6.md',
    'lab7.md',
    'lab8.md',
    'lab9-MiniCREST.md',
    # 'lab9.md' # 格式有问题，暂时先不用来生成
]

OUTPUT_FILE = 'output/output.md'


def sanitize(text):
    """
    相对路径转绝对路径；去除emoji
    """
    final_text = text.replace("../images", "images").replace(":star:","")
    return final_text

if __name__ == "__main__":
    final_markdown = YAML_HEADER

    for file in MARKDOWN_FILE_ARRAY:
        input_path = Path(f"{BASE_FOLDER}/{file}")
        with input_path.open("r", encoding="utf-8") as f:
            original_content = f.read()

        final_markdown += original_content

        final_markdown += '\n'

        final_markdown += '\n'

    final_markdown = sanitize(final_markdown)

    output_path = Path(f"{OUTPUT_FILE}")
    with output_path.open("w", encoding="utf-8") as f:
        f.write(final_markdown)    