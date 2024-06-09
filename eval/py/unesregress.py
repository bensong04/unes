#!/usr/bin/env python3
"""
Runs the entire suite of Tom Harte's tests on the CPU driver.
Stops after encountering an error, and prints the offending test's
information.
"""
from typing import Optional
from pathlib import Path

import sys, subprocess as proc
import json

HOW_MANY = 10000
OK = 0
UNIMPLEMENTED = 3
FAIL_THRESH = 10

def hex2(h: int) -> str:
    return hex(h)[2:].zfill(2)

def gracefully_exit(tests_failed: int, tests_tried: int):
    if (tests_failed == 0):
        print("ALL TESTS PASSED!!!")
    print(f"TESTS ATTEMPTED: {tests_tried} TESTS FAILED: {tests_failed}")
    print(f"Success rate: {format(1 - tests_failed/tests_tried, ".2%")}")
    exit(0)

def run_test(test_no: int) -> Optional[tuple[int, int, str, str]]:
    for subtest_num in range(HOW_MANY):
        exec_name: str = proc.run(["eval/py/assembleunit.py",
                             f"eval/execs/tomharte/{hex2(test_no)}.json",
                             f"{subtest_num}"], capture_output=True,
                             text=True).stdout
        exec_name = exec_name.strip()
        result: proc.CompletedProcess = proc.run(["bin/cpu_unittest",
                                                  exec_name], capture_output=True,
                                                  text=True)
        if result.returncode == UNIMPLEMENTED:
            return test_no, -1, None, None
        if result.returncode != OK:
            return test_no, subtest_num, result.stdout, result.stderr

if __name__ == "__main__":
    failcount: int = 0
    testcount: int = 0
    bad_ops = []
    for no in range(0, 1):
        res = run_test(no)
        if res is not None:
            t_no, t_sub, so, se = res
            if (t_sub == -1):
                print(f"{t_no}: Unimplemented, continuing...")
                continue
            print("\n============\n")
            print(f"FAILED: {t_no}.{t_sub}")
            print("Offending test json:")
            with open(f"eval/execs/tomharte/{hex2(t_no)}.json", "r") as jfile:
                print(json.load(jfile)[t_sub])
            print("TEST STDOUT: ")
            print(so)
            print("TEST STDERR: ")
            print(se)
            failcount += 1
            bad_ops.append(no)
            print("============\n")
        else:
            print(f"{no}: Passed!")
        testcount += 1
        if failcount >= FAIL_THRESH:
            print("Too many errors, exiting...")
            break
    gracefully_exit(failcount, testcount)
