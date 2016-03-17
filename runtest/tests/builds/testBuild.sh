#!/bin/bash
if [ "$1" = "" ]; then
	echo "First argument is result folder."
	exit 1
fi
target=$2
cd "$1"

if [ "$2" = "" ]; then
	# for each target
	a=0
	for target in $(grep -E '^[A-Za-z\-]*:' Makefile | cut -d ':' -f 1 | sort | uniq); do
		let a=a+1
		echo "INFO: cleaning build..."
		make clean
		if [ $? != 0 ]; then
			echo "ERROR: make clean"
			exit 1
		fi

		echo "INFO: building target $target ..."
		make -j4 $target
		if [ $? != 0 ]; then
			echo "ERROR: building target $target"
			exit 2
		fi
	done
	if [ $a -eq 0 ]; then
		echo "ERROR: No make rule found"
		exit 2
	fi
else
	echo "INFO: cleaning build..."
	make clean
	if [ $? != 0 ]; then
		echo "ERROR: make clean"
		exit 1
	fi

	echo "INFO: building target $target ..."
		make -j4 $target
	if [ $? != 0 ]; then
		echo "ERROR: building target $target"
		exit 2
	fi
fi
