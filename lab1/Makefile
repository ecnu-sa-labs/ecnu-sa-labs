GCC_VERSION := $(shell gcc --version | grep ^gcc | sed 's/^.* //g')
CLANG_VERSION := $(shell clang --version | head -n1 | cut -d' ' -f3)

check_versions:
ifeq ($(GCC_VERSION), 13)
else
	update-alternatives --set gcc /usr/bin/gcc-13
endif

ifeq ($(CLANG_VERSION), 18)
else
	update-alternatives --set clang /usr/bin/clang-18
endif

TARGETS=test0 test1 test2 test3 test4 test5 test6 test7 test8 test9

.PRECIOUS: results/afl_logs/%/out.txt results/csa_logs/%_out.txt

all: ${TARGETS}

results/afl_logs/%/out.txt: c_programs/%.c
	@mkdir -p afl_output/
	AFL_DONT_OPTIMIZE=1 afl-gcc $< $ -o $* 2> /dev/null
	@mkdir -p results/afl_logs/$*
	-timeout 30s afl-fuzz -i afl_input -o afl_output ./$*  > $@ 2> /dev/null
	@cp -r afl_output/ $* results/afl_logs/$*/ && rm -rf afl_output/* $*

results/csa_logs/%_out.txt: c_programs/%.c
	@mkdir -p results/csa_logs
	clang -v --analyze $< > $@ 2>&1

%: results/afl_logs/%/out.txt results/csa_logs/%_out.txt
	@:

submit:	answers.txt results/
	mkdir -p submission
	rm -rf submission/*
	cp -r answers.txt results c_programs submission
	@chown -R --reference=Makefile submission
	zip -r submission.zip submission/ 2>&1 >/dev/null
	@chown -R --reference=Makefile submission.zip

clean:
	rm -rf results ${TARGETS} output.db submission submission.zip afl_output
