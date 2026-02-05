#!/bin/bash
cd `dirname $0`
echo MAKE .....................
make clean
clang-format --style=Chromium -i src/*.c src/*.h
make
echo RUN ......................
valgrind --leak-check=full --show-error-list=yes ./build/main
echo RC=$?
echo WAIT .....................
read X
