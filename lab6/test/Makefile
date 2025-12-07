MAKEFLAGS += --no-builtin-rules

SRC:=$(wildcard *.c)
TARGETS:=$(patsubst %.c, %, $(SRC))

all: ${TARGETS}

%: %.c
	clang-19 -emit-llvm -S -fno-discard-value-names -Xclang -disable-O0-optnone -c -o $@.ll $<
	opt-19 -passes="mem2reg" -S $@.ll -o $@.opt.ll
	opt-19 -load-pass-plugin ../build/DivZeroPass.so -passes="DivZero" $@.opt.ll -disable-output 2>&1 > $@.out | tee $@.err
	@echo "\n"


clean:
	rm -f *.ll *.out *.err