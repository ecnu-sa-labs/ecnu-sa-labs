TARGETS:=$(shell find . -type f -name "*.c" -exec basename -s .c -a {} \;)

all: ${TARGETS}

%: %.c
	clang -emit-llvm -S -fno-discard-value-names -c -o $@.ll $< -g
	opt -load ../lib/InstrumentPass.so -Instrument -S $@.ll -o $@.instrumented.ll
	clang -o $@ -L../lib -lruntime -lm $@.instrumented.ll

fuzz-%: %
	@./test.sh $< 30s

clean:
	rm -rf *.ll *.cov ${TARGETS} core.* fuzz_output* out_*.txt
