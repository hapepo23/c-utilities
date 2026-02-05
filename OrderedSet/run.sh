#!/bin/bash
cd `dirname $0`
echo RUN ......................
time ./build/main
echo RC=$?
echo WAIT .....................
read X
