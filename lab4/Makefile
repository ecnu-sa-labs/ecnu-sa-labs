MAKEFLAGS += --no-builtin-rules

SRC:=$(shell find . -name '*.py') requirements.txt Makefile

all: install

build: clean-package ${SRC}
	@echo "Building Deta-Debugger..."
	@python3 -m pip install --upgrade --editable . 1> /dev/null 2>&1

install: build
	@echo "Deta-Debugger installed."

submit:
	@rm -f submission.zip
	@echo "Creating submission..."
	@chown -R --reference=Makefile .
	zip -r /tmp/submission.zip ${SRC} 2> /dev/null
	@mv /tmp/submission.zip submission.zip
	@chown -R --reference=Makefile submission.zip
	@echo "submission zip created."

clean-package:
	@echo "Cleaning delta-debugger..."
	@python3 -m pip uninstall deta-debugger 2> /dev/null
	@rm -f /usr/local/bin/delta-debugger
	@rm -rf */__pycache__ *.egg-info

clean-test:
	@echo "Cleaning up test..."
	@(cd test; make clean)

clean: clean-package clean-test
	@echo "Removing submission.zip"
	rm -f ./submission.zip