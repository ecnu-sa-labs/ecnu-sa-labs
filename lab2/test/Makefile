TARGETS=simple0 simple1 simple2 simple3 simple4 simple5 simple6 simple7 simple8 simple9


all: simple

simple: $(TARGETS)



%: %.c
	clang-19 -emit-llvm -S -O0 -fno-discard-value-names -Xclang -disable-O0-optnone -c -o $@.ll $<
	opt-19 -load-pass-plugin ../build/StaticAnalysisPass.so -passes="StaticAnalysisPass" -S $@.ll -o $@.static.ll
	opt-19 -load-pass-plugin ../build/DynamicAnalysisPass.so -passes="DynamicAnalysisPass" -S $@.ll -o $@.dynamic.ll
	clang-19 -o $@ -L${PWD}/../build -lruntime $@.dynamic.ll

clean:
	rm -f *.ll *.*cov *.binops ${TARGETS}
