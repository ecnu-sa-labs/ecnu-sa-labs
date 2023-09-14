TARGETS=simple0 simple1 simple2 simple3 simple4 simple5 simple6 simple7 simple8 simple9


all: simple

simple: $(TARGETS)



%: %.c
	clang -emit-llvm -S -fno-discard-value-names -c -o $@.ll $< -g
	opt -load ../build/StaticAnalysisPass.so -StaticAnalysisPass -S $@.ll -o $@.static.ll
	opt -load ../build/DynamicAnalysisPass.so -DynamicAnalysisPass -S $@.ll -o $@.dynamic.ll
	clang -o $@ -L${PWD}/../build -lruntime $@.dynamic.ll

clean:
	rm -f *.ll *.*cov *.binops ${TARGETS}
