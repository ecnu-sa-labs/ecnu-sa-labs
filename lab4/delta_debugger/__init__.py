from typing import Union
from subprocess import run, PIPE


def run_target(target: str, input: Union[str, bytes]) -> int:
    """
    Run target program with input on its stdin.

    :param target: target program to run
    :param input: input to pass to target program
    :return: return code of target program
    """
    if isinstance(input, str):
        input = input.encode()
    process = run( 
        [target],
        input=input,
        stdout=PIPE,
        stderr=PIPE,
    )

    # Debug information
    # print(
    #     f"ret={process.returncode}\n"
    #     f"out:\n{process.stdout}\n"
    #     f"err:\n{process.stderr}"
    # )
    
    return process.returncode
