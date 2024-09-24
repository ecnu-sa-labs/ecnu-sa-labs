# Labs for "Software Analysis, Testing and Verification"
This repository provides the lab sessions for the course "[Software Analysis, Testing and Verification (软件分析与验证前沿)](https://tingsu.github.io/files/courses/pa2024.html)" at East China Normal University.

We welcome any comments or contributions. Feel free to create issues and pull requests.

<br>

## Instructions （教程指引）

Before preceding the labs, you need to setup the lab environment. Checkout this [lab tutorial](lab_manual/course-vm.md). Later, you can access the lab tutorials for our labs
in the following table.


| Labs                         | Lab Title                         |
|------------------------------|-----------------------------------|
| [lab1](lab_manual/lab1.md)       | Introduction to Software Analysis |
| [lab2](lab_manual/lab2.md)       | The LLVM Framework                |
| [lab3](lab_manual/lab3.md)       | Random Testing / Fuzzing          |
| [lab4](lab_manual/lab4.md)       | Delta Debugging                   |
| [lab5](lab_manual/lab5.md)       | Dataflow Analysis                 |
| [lab6](lab_manual/lab6.md)       | Pointer Analysis                  |
| [lab7](lab_manual/lab7.md)       | Dynamic Symbolic Execution        |

<br>


## Troubleshooting (常见问题汇总)

- 第一次配置实验课环境([lab tutorial](lab_manual/course-vm.md))，由于需要下载docker image，请使用科学上网工具。Windows上的科学上网工具有：`Clash for windows`, `Clash Meta`, `V2rayN`（请打开tun模式实现接近于全局的代理）。
- 如果你的电脑系统是Mac，且使用的是Mac M芯片，可能需要安装[Rosetta](https://support.apple.com/en-us/102527) (Rosetta enables a Mac with Apple silicon to use apps built for a Mac with an Intel processor)。另外，确认Docker的`Settings` -> `General`里是否已经勾选上`Use Rosetta for x86_64/amd64 emulation on Apple Silicon`。

## Contributors （贡献者）

We thanks the TAs and students who have contributed to this course design:

<a href="https://ml-ming.dev/">明孟立</a>,
<a href="https://apochens.github.io/">黄杉</a>,
<a href="">麻恩泽</a>,
王祺昌,
方润渲,
<a href="https://xixianliang.github.io/resume/">梁锡贤</a>,
沈佳伟

林童奕凡、高雨宸


Acknowledgement: The lab sessions are currently developed based on the [cis547](https://software-analysis-class.org) course, and we are further designing and refining the materials based on our ideas.
