MAKEFLAGS += --no-builtin-rules

TARGETS=test1 test2 test3 test4

all: $(TARGETS)

%.ll: %.c
	clang-19 -emit-llvm -S -O0 -fno-discard-value-names -Xclang -disable-O0-optnone -c $<

%: %.ll
	diff -y --suppress-common-lines --report-identical-files --ignore-all-space $@.ll ../ir_programs/$@.ll
	clang-19 -o $@ $@.ll

clean:
	rm -f *.ll $(TARGETS)
