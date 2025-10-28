TARGETS:=$(shell find . -type f -name "*.c" -exec basename -s .c -a {} \;)

all: ${TARGETS}

%: %.c
	clang-19 -emit-llvm -S -fno-discard-value-names -O0 -Xclang -disable-O0-optnone -c -o $@.ll $< -g
	opt-19 -load-pass-plugin ../lib/InstrumentPass.so -passes="InstrumentPass" -S $@.ll -o $@.instrumented.ll
	clang-19 -o $@ -L../lib -lruntime -lm $@.instrumented.ll

fuzz-%: %
	@./test.sh $< 30s

clean:
	rm -rf *.ll *.cov ${TARGETS} core.* fuzz_output* out_*.txt
