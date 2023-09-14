from delta_debugger import run_target

# store 1-minimal input that reproduces crash
X_STRING = b""


def delta_debug(target: str, input: bytes) -> bytes:
    """
    Delta-Debugging Algorithm.

    TODO: Implement your algorithm for delta-debugging.

    Hint: It may be helpful to use an auxilary function that
    takes as input a target, input string, n and
    returns the next input and n to try.

    :param target: target program
    :param input: crashing input to be minimized
    :return: 1-minimal crashing input
    """
    global X_STRING
    X_STRING = input
    return X_STRING
