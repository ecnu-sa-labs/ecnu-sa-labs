TARGETS := test0 test1 test2 test3 test4 test5 test6 test7 test8 test9

.PHONY: all submit clean
.PRECIOUS: results/afl_logs/%/out.txt results/csa_logs/%_out.txt

all: $(TARGETS)

results/afl_logs/%/out.txt: c_programs/%.c
    @mkdir -p afl_output/
	@mkdir -p results/afl_logs/$*
	AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1 AFL_DONT_OPTIMIZE=1 afl-gcc $< -o $* 2>/dev/null
	-AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1 timeout 30s afl-fuzz -m none -i afl_input -o afl_output -- ./$* > $@ 2>/dev/null
	@cp -r afl_output results/afl_logs/$*/
	@cp -f $* results/afl_logs/$*/ 2>/dev/null || true
	@rm -rf afl_output $*

results/csa_logs/%_out.txt: c_programs/%.c
	@mkdir -p results/csa_logs
	clang -v --analyze $< > $@ 2>&1

%: results/afl_logs/%/out.txt results/csa_logs/%_out.txt
	@:

submit: answers.txt results/
	@mkdir -p submission
	@rm -rf submission/*
	@cp -r answers.txt results c_programs submission
	@chown -R --reference=Makefile submission
	@zip -r submission.zip submission/ >/dev/null 2>&1
	@chown -R --reference=Makefile submission.zip

clean:
	@rm -rf results $(TARGETS) output.db submission submission.zip afl_output
	
