"""Owned producer fixture: duplicate names, recursion and sub-millisecond timing."""

import cProfile
import sys

from left import duplicate as left
from right import duplicate as right


def recurse(depth):
    if depth:
        return recurse(depth - 1) + 1
    return 0


def tiny(value):
    return (value + 1) * 3


def batch():
    result = 0
    for value in range(7):
        result += left(value) + right(value) + recurse(4)
    for value in range(11):
        result += tiny(value)
    return result


if __name__ == "__main__":
    profile = cProfile.Profile()
    print(profile.runcall(batch))
    profile.dump_stats(sys.argv[1])
