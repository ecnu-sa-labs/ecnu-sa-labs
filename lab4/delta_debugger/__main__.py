#! /usr/bin/env python3

import sys

from delta_debugger import run_target
from delta_debugger.delta import delta_debug

from sys import argv
from pathlib import Path


def exist_check(file):
    if not Path(file).exists():
        print(f"{file} not found", sys.stderr)
        return 1
    return 0


def main() -> int:
    if len(argv) < 3:
        print(f"usage: {argv[0]} [target] [crashing input file]")
        return 1
    
    target, input_file = argv[1], argv[2]

    if not Path(target).exists():
        print(f"{target} not found", sys.stderr)
        return 1
    
    if not Path(input_file).exists():
        print(f"{input_file} not found", sys.stderr)
        return 1

    with open(input_file, "rb") as fp:
        input = fp.read()
        if not run_target(target=target, input=input):
            print(
                "program does not crash with initial input",
                file=sys.stderr
            )
            return 1

    delta_debugging_result = delta_debug(target=target, input=input)

    print(
        f"original inputs size: {len(input)}",
        f"minimized input size: {len(delta_debugging_result)}",
        sep="\n"
    )
    
    with open(f"{input_file}.delta", "wb") as fp:
        fp.write(delta_debugging_result)

    return 0


if __name__ == "__main__":
    """
    usage: delta-debug [target] [crashing input file]
    """
    sys.exit(main(*sys.argv[1:]))
