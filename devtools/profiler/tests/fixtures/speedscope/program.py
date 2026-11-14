"""Owned finite CPU workload for a real, opt-in py-spy capture."""

import time


def leaf(seed):
    for index in range(10000):
        seed = (seed * 1664525 + index + 1013904223) & 0xffffffff
    return seed


def recurse(depth, value):
    if depth:
        return recurse(depth - 1, value)
    return leaf(value)


if __name__ == "__main__":
    deadline = time.monotonic() + 3
    checksum = 1
    while time.monotonic() < deadline:
        checksum = recurse(4, checksum)
    print(checksum)
