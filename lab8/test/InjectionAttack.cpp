#include <cstdio>
#include <cstring>  // for strcat
#include <cstdlib>  // for system

// Command Injection Vulnerability Example
int main(int argc, char** argv) {
  char cmd[2048] = "/bin/cat ";
  char filename[1024];
  printf("Filename:");
  scanf (" %1023[^\n]", filename); // The attacker can inject a shell escape here
  strcat(cmd, filename);
  system(cmd); // Warning: Untrusted data is passed to a system call
}
