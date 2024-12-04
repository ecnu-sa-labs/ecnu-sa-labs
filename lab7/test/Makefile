TARGETS=InjectionAttack ControlFlowHijack


all: simple

simple: $(TARGETS)



%: %.cpp
	clang -emit-llvm -S -fno-discard-value-names -c -o $@.ll $< -g
	opt -load ../build/DynTaintAnalysisPass.so -DynTaintAnalysisPass -S $@.ll -o $@.dynamic.ll
	clang -o $@ -L${PWD}/../build -lruntime $@.dynamic.ll

clean:
	rm -f *.ll *.*cov *.binops ${TARGETS}
