MAKEFLAGS += --no-builtin-rules

SRC:=$(shell find ./src -type f -name "*.cpp")

all: submit

submit: ${SRC} clean
	chown -R --reference=CMakeLists.txt .
	zip -r /tmp/submission.zip * 2> /dev/null
	mv /tmp/submission.zip ./submission.zip
	chown --reference=CMakeLists.txt submission.zip
	echo "submission.zip created successfully."

clean:
	rm -rf build
	(cd test; make clean) > /dev/null
	rm -f submission.zip
