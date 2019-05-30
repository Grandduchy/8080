# This Python file uses the following encoding: utf-8

# This is a quick and dirty way of allowing two binarys to compile
# Simply look in the header file and uncomment the neccesary defines

import sys
import os.path
import fileinput

headerFile = "../include/tester.h"

if not os.path.isfile(headerFile) :
    print("Cannot find header file, exiting...")
    sys.exit()


for line in fileinput.input(headerFile, inplace = True):
    if "// #define TESTENABLE" in line:
        line = line.replace("// #define TESTENABLE", "#define TESTENABLE")
    if "// #define TESTS" in line:
        line = line.replace("// #define TESTS", "#define TESTS")
    print(line)
