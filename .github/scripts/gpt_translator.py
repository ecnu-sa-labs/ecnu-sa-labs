from openai import OpenAI
import logging
from string import Template

class OpenaiTranslator:
    def __init__(self, api_key:str):
        self.logger = logging.getLogger(name=self.__class__.__name__)
        self.client = OpenAI(
            api_key=api_key,
            base_url="https://api.deepseek.com" # 用deepseek API
        )

    def translate_text(self, reviewed, text):

        system_prompt = SYSTEM_PROMPT_TEMPLATE.substitute(
            GLOSSARY=GLOSSARY,
            REVIEWED=reviewed,
            ORIGINAL=text
        )

        return self.get_resp(system_prompt)

    def translate_text_without_review(self, text):
        system_prompt = SYSTEM_PROMPT_TEMPLATE_WITHOUT_REVIEW.substitute(
            GLOSSARY=GLOSSARY,
            ORIGINAL=text
        )

        return self.get_resp(system_prompt)
    
    def translate_text_without_review_and_glossary(self, text):
        system_prompt = SYSTEM_PROMPT_TEMPLATE_BASIC.substitute(
            ORIGINAL=text
        )

        return self.get_resp(system_prompt)
    
    def get_resp(self, prompt):
        response = self.client.chat.completions.create(
            model="deepseek-v4-flash",  # You can use gpt-4.1-mini, gpt-3.5-turbo, gpt-4, gpt-4o, etc.
            messages=[
                {"role": "user", "content": prompt},
            ],
            stream=False,
            temperature=0.2,
            extra_body={"thinking": {"type": "disabled"}} # 关闭思考模式
        )

        translated = response.choices[0].message.content

        return translated.strip()


GLOSSARY = """
software analysis -> 软件分析
property -> 性质
property-based testing -> 基于性质的测试
monkey event -> 随机事件
widget -> 控件
lark -> 飞书
delta debugger -> delta debugger

"""

SYSTEM_PROMPT_TEMPLATE = Template("""
你是一位专业的中英技术文档翻译助手。

我将提供一个英文的 Markdown 文件，请你将其翻译为中文。

为了保持术语一致性和语言风格，请严格参考以下 review 过的中文版本：
```
$REVIEWED
```

术语表如下：
```               
$GLOSSARY
```

请遵守以下要求：
1. 遇到相同或相似句子时，优先使用上方的参考中文翻译，尽量不要进行润色或句式改动；
2. 保留原始 Markdown 格式（如标题、列表、代码块等）；
3. 专有词汇（如 Activity、Fragment、Intent 等）、论文、人名等请保留英文；
4. 若原文中无匹配内容，才进行自由翻译，但请保持语言风格与参考一致；
5. 直接返回 Markdown 正文，不要用代码块包裹。


请开始翻译：
```                                  
$ORIGINAL
```
""")

SYSTEM_PROMPT_TEMPLATE_WITHOUT_REVIEW = Template("""
你是一位专业的中英技术文档翻译助手，对软件分析与测试的前沿领域有深刻的了解。

我将提供一份英文 Markdown 文档，请你将其翻译为中文，供学习软件分析与测试这门课程的学生参考使用。

术语表如下：
```               
$GLOSSARY
```

请遵守以下要求：
1. 保留原始 Markdown 格式（如标题、列表、代码块等）；
2. 专有词汇（如 LLVM、Hypothesis 等）、论文、人名、超链接标题等请保留英文；
3. 直接返回 Markdown 正文，不要用代码块包裹。
4. 翻译后的中文需尽量符合日常交流的表达习惯。


请开始翻译：
```                                  
$ORIGINAL
```
""")

SYSTEM_PROMPT_TEMPLATE_BASIC = Template("""
你是一位专业的中英技术文档翻译助手，对软件分析与测试的前沿领域有深刻的了解。

我将提供一份英文 Markdown 文档，请你将其翻译为中文，供学习软件分析与测试这门课程的学生参考使用。

请遵守以下要求：
1. 保留原始 Markdown 格式（如标题、列表、代码块等）；
2. 专有词汇（如 LLVM、Hypothesis、Delta Debugger 等）、论文、人名、超链接标题等请保留英文；
3. 直接返回 Markdown 正文，不要用代码块包裹。
4. 翻译后的中文需尽量符合日常交流的表达习惯。

请开始翻译：
```                                  
$ORIGINAL
```
""")