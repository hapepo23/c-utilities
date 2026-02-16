#!/bin/bash
cd `dirname $0`
echo RUN ......................
time ./main
echo RC=$?
echo WAIT .....................
read X
