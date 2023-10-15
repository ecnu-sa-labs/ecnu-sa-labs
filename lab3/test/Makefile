TARGETS:=$(shell find . -type f -name "*.c" -exec basename -s .c -a {} \;)

all: ${TARGETS}

%: %.c
	clang -emit-llvm -S -fno-discard-value-names -c -o $@.ll $< -g
	opt -load ../build/InstrumentPass.so -Instrument -S $@.ll -o $@.instrumented.ll
	clang -o $@ -L${PWD}/../build -lruntime -lm $@.instrumented.ll

fuzz-%: %
	@./test.sh $< 10s

clean:
	rm -rf *.ll *.cov ${TARGETS} core.* fuzz_output* out_*.txt
