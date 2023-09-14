#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const int STR_MAX_SIZE = 1024;

const char *getBinOpName(char symbol) {
  switch (symbol) {
  case '+':
    return "Addition";
  case '-':
    return "Subtraction";
  case '*':
    return "Multiplication";
  case '/':
    return "Division";
  case '%':
    return "Modulo";
  default:
    return "Unknown operation";
  }
}

void get_logfile(char *buf, const int buf_size, const char *ext) {
  char exe[STR_MAX_SIZE];
  int ret = readlink("/proc/self/exe", exe, sizeof(exe) - 1);
  if (ret == -1) {
    fprintf(stderr, "Error: Cannot find /proc/self/exe\n");
    exit(1);
  }
  exe[ret] = 0;
  int len = strlen(exe);
  strncpy(buf, exe, len);
  buf[len] = 0;
  for (int i = 0; i <= strlen(ext); ++i) {
    buf[buf_size - (i + 1)] = 0;
  }
  strncat(buf, ext, strlen(ext));
}

void __coverage__(int line, int col) {
  char logfile[STR_MAX_SIZE];
  get_logfile(logfile, sizeof(logfile), ".cov");
  FILE *f = fopen(logfile, "a");
  fprintf(f, "%d, %d\n", line, col);
  fclose(f);
}

void __binop_op__(char c, int line, int col, int op1, int op2) {
  char logfile[STR_MAX_SIZE];
  get_logfile(logfile, sizeof(logfile), ".binops");
  FILE *f = fopen(logfile, "a");
  fprintf(
      f,
      "%s on Line %d, Column %d with first operand=%d and second operand=%d\n",
      getBinOpName(c), line, col, op1, op2);
  fclose(f);
}
