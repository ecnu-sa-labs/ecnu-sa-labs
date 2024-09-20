## Setting up Course Lab Environment

Our labs use VScode and Docker containers to build a flexible, consistent and ready-to-use environment. You can view the picture below to understand the architecture of our lab environment (a.k.a VScode's remote development mode). Conceptually, your local machine (OS) runs VS Code, and the VS code connects to the remote machine (OS) running in a docker container. In this way, you can edits the files in the remote machine via VS code on your local machine, and run all the labs on this remote machine (OS). In the docker container, we have already setup everything (Ubuntu XX.XX, LLVM XX.XX) for you.

![](../images/principle_of_vscode_remote_development.png =200x300)

**Step1.** For this course, we will use Docker containers with VS Code to ensure everyone uses a consistent development environment for the labs . Before proceeding, you need to install <a href="https://www.docker.com/">docker</a> on your local machine(Note:For windows users, please select Download for Windows-AMD64; For Mac users, please select Download for Mac); we will refer to this machine as the host machine. 

![](../images/course-vm-d.jpg)

**Step2.** After this, install <a href="https://code.visualstudio.com/Download">VS Code</a> and within VS Code install the <a href="https://code.visualstudio.com/docs/remote/remote-overview">Remote Development Extension</a>. 

![](../images/remote_development.png)

**Step3.** For the course labs, we provide a configuration file(`.devcontainer/devcontainer.json`) in each lab that instructs VS Code how to set up a development environment using an appropriate docker image and various other configuration options. 

About `.devcontainer/devcontainer.json`,
```
"name": "ECNUA SA Lab Container",
"image": "ecnusa/ecnu-sa-labs:latest",
"runArgs": [
	"--cap-add=SYS_PTRACE",
	"--security-opt",`
	"seccomp=unconfined",
	"--privileged"
],

// Set the env-variables for the container.
"remoteEnv": {
	"LD_LIBRARY_PATH": "${containerWorkspaceFolder}/build:${containerEnv:LD_LIBRARY_PATH}"
},
...
```
Before you start with the labs, make sure that docker engine is running in the background. 
![](../images/course-vm-f.jpg)

Please clone the repository in your computer using `git clone "your repository https url"`.
**Note: You will need to open the lab folder such as lab1 or lab2 in VS Code rather than the ecnu-sa-labs folder. **
![](../images/course-vm-e.jpg)

**Step4.** To start working on a lab, open the lab inside VS Code. Press the F1 function key to open a search box (Note: Sometimes, it's not the F1 function key). Search and select Dev Containers:Rebuild and Reopen in Container. 
![](../images/course-vm-g.jpg)
This will reload VS Code and set up the development environment. You can now edit, run and debug your lab in VS Code.When the build is complete, select a file within the lab1 folder and open it in terminal.
![](../images/course-vm-h.jpg) 
To see if you have successfully open the labs in your container, you can type command `clang --version` in integred terminal, and you will get this:![](../images/course-vm-i.jpg)
To resume working on a lab, open the lab in VS Code and Reopen in Container. Any changes you make here will be available in your ecnu-sa-labs folder.

<!-- When the F1 key work, you will see..

![](../images/course-vm-c.png)

When the F1 key doesn't work, you can do the following:

![](../images/course-vm-a.png)
![](./images/course-vm-b.png) -->
