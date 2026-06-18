# 课程实验环境配置

本实验课程使用 VScode 和 Docker 容器来构建一个灵活、一致且开箱即用的开发环境。你可以查看下图来了解实验环境的架构（即 VS Code 的远程开发模式）。从概念上讲，你的本地操作系统运行 VS Code，而 VS Code 连接到运行在 Docker 容器中的远程操作系统。这样，你就可以通过本地操作系统上的 VS Code 编辑远程操作系统中的文件，并在远程操作系统上运行所有实验。在 Docker 容器中，我们已经为你配置好了一切（所有必要的工具和依赖项）。请注意，Docker 运行在你的本地操作系统上。

![](../images/principle_of_vscode_remote_development.png)


## Docker 容器

- Ubuntu 22.04, LLVM 19

:star: 你的本地机器需要至少有 15GB 的磁盘空间和 8GB 的内存才能运行 Docker 容器。

## 你需要安装什么？

- [Docker](https://www.docker.com/)
- [VS code](https://code.visualstudio.com/Download)

## 实验环境配置说明

### 第一步
对于本课程，我们将使用带有 VS Code 的 Docker 容器，以确保每个人在实验中使用一致的开发环境。因此，你需要在本地机器上安装 <a href="https://www.docker.com/">docker</a>[^1]。


![](../images/course-vm-d.jpg)

### 第二步
之后，安装 <a href="https://code.visualstudio.com/Download">VS Code</a>，并在 VS Code 中从 `Extensions` 安装 `Remote Development`。

![](../images/remote_development.png)

在 VS Code 中，我们还建议你从 `Extensions` 安装 `GitHub Repositories`。它让你可以直接在 Visual Studio Code 中快速浏览、搜索、编辑和提交到任何远程 GitHub 仓库。

![alt text](../images/github_repositories.png)

### 第三步
在你的本地机器上克隆我们的仓库（[ecnu-sa-labs](https://github.com/ecnu-sa-labs/ecnu-sa-labs)）。
在这个仓库中，我们在 `lab1`、`lab2` 等文件夹中提供了实验内容。
在每个实验中，我们都提供了一个配置文件 `.devcontainer/devcontainer.json`[^2]，该文件指示 VS Code 使用适当的 Docker 镜像和各种其他配置选项来设置开发环境。

以下是一个示例：`.devcontainer/devcontainer.json`，
```
"name": "ECNUA SA Lab Container",
"image": "ecnusa/ecnu-sa-labs:latest",
"runArgs": [
	"--cap-add=SYS_PTRACE",
	"--security-opt",
	"seccomp=unconfined",
	"--privileged"
],

// 为容器设置环境变量。
"remoteEnv": {
	"LD_LIBRARY_PATH": "${containerWorkspaceFolder}/build:${containerEnv:LD_LIBRARY_PATH}"
},
...
```

<!-- 在开始某个实验之前，请打开 Docker。 -->

<!-- ![](../images/course-vm-f.jpg) -->



### 第四步
要开始进行某个实验，请在一个**新的** VS Code 窗口中打开该实验。

:star: 注意：你需要在新的 VS Code 窗口中打开实验文件夹（例如 `lab1`）。
在 VS Code 中，转到 `File`，选择 `New Window`，找到并选择要打开的实验文件夹（例如 `lab1`）。你应该能够像下图一样打开实验。

![](../images/course-vm-lab1-folder.jpg)


打开 VS Code 中的命令面板[^3]，搜索并选择 `Dev Containers:Rebuild and Reopen in Container` 来设置实验环境（即构建并打开容器，并将你的本地机器连接到包含实验的容器）。这个过程可能需要几分钟，因为它需要下载我们的 Docker 镜像。

:star: 注意：如果在构建和打开容器时遇到错误，另一种方法是在终端中执行 `docker pull ecnusa/ecnu-sa-labs`，直接从 <a href="https://hub.docker.com/">docker hub</a> 拉取 <a href="https://hub.docker.com/r/ecnusa/ecnu-sa-labs">ecnusa/ecnu-sa-labs</a> 镜像。

:star: 如果你无法访问 Docker 页面，请参考 <a href="https://pan.baidu.com/s/1B7W2EeSUts_k2lzoTnJhDg?pwd=yebz">此链接</a> 下载 Docker 镜像。解压后，在容器中构建之前，使用 `docker load -i <镜像 tar 文件路径>` 加载镜像。

![](../images/course-vm-lab1-rebuild-and-reopen-container.jpg)

这将重新加载 VS Code 并设置开发环境。你现在可以在 VS Code 中编辑、运行和调试你的实验。你可以在 VS Code 中打开终端：

![](../images/course-vm-lab1.jpg) 

要检查你是否已成功在容器中打开实验，可以在终端中运行 `clang --version`，你应该能够看到 clang 版本：

![](../images/course-vm-lab1-clang.jpg)

如果你关闭了这个新窗口中的实验（即你的本地机器与容器断开了连接），并希望继续实验。
你可以再次在 VS Code 中打开实验文件夹，打开命令面板，搜索并选择 `Reopen in Container`。

:star: 注意：你在实验文件夹（容器内）所做的任何更改都将同步到你的 ecnu-sa-labs 文件夹（本地机器上）。


[^1]: 注意：对于 Windows 用户，请选择 `Download for Windows-AMD64`；对于 Mac 用户，请选择 `Download for Mac`。如果你无法直接访问 <a href="https://www.docker.com/">docker</a>，请参考 <a href="https://pan.quark.cn/s/6fc0c0d8ccf6">此链接</a> 进行下载。

[^2]: `devcontainer.json` 文件是一个 JSON 格式的配置文件，通常存放在项目根目录下的 `.devcontainer` 文件夹中。它定义了开发容器的配置信息，包括容器的基础镜像、需要安装的工具和扩展、环境变量等。通过这个文件，开发者可以确保项目在不同的开发环境中都能保持一致的运行状态。

[^3]: 在 VS Code 中，命令面板是一个功能强大的多功能工具，可以快速访问各种命令。在 Windows 和 Linux 上，你可以按 `Ctrl + Shift + P` 打开命令面板。在 macOS 上，使用 `Command + Shift + P`。