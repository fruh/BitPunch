#!/bin/bash
if [ "$1" == "" ]; then
	target=default
else
	target=$1
fi
cd $CWD/..
trap "cd $CWD/tests" EXIT

make clean
if [ $? != 0 ]; then
	echo "ERROR: make clean"
	exit 1
fi
make $target
if [ $? != 0 ]; then
	echo "ERROR: make $target"
	exit 2
fi
