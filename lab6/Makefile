MAKEFLAGS += --no-builtin-rules

all: submit

submit:
	chown -R --reference=CMakeLists.txt .
	zip -r /tmp/submission.zip include src test/*.c CMakeLists.txt 2> /dev/null
	mv /tmp/submission.zip ./submission.zip
	chown --reference=CMakeLists.txt submission.zip
	echo "submission.zip created successfully."

clean:
	@(cd test; make clean) > /dev/null
	rm -rf build
	rm -f submission.zip