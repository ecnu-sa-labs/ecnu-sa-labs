MAKEFLAGS += --no-builtin-rules

PART1=c_programs/test1.c c_programs/test2.c c_programs/test3.c c_programs/test4.c
PART2=src/DynamicAnalysisPass.cpp src/StaticAnalysisPass.cpp include/Utils.h src/Utils.cpp

all: submit

submit:	${PART1} ${PART2} clean
	chown -R --reference=CMakeLists.txt .
	zip -r /tmp/submission.zip * 2> /dev/null
	mv /tmp/submission.zip ./submission.zip
	chown --reference=CMakeLists.txt submission.zip
	echo "submission.zip created successfully."

clean:
	(cd c_programs; make clean) > /dev/null
	rm -rf build
	(cd test; make clean) > /dev/null
	rm -f submission.zip
