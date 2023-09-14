.PRECIOUS: %.ll %.instrumented.ll

SRC:=$(wildcard *.c)
TARGETS:=$(patsubst %.c, %, $(SRC))

build-all: ${TARGETS}

all: $(patsubst %, dse-%, $(TARGETS))


%: %.c
	clang -emit-llvm -S -fno-discard-value-names -c -o $@.ll $<
	opt -load ../build/InstrumentPass.so -DSEInstrument -S $*.ll -o $*.instrumented.ll
	clang -o $@ -L${PWD}/../build -lruntime $*.instrumented.ll

dse-%: %
	./test $<

clean:
	rm -f *.ll *.out *.err *.smt2 input.txt branch.txt ${TARGETS}
