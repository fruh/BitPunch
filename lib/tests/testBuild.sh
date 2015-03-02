#!/bin/bash
if [ "$1" == "" ]; then
	target=default
else
	target=$1
fi
cd $CWD/..

if [ "$1" == "" ]; then
	# for each target
	for target in $(grep -E '^\w.*:' makefile | cut -d ':' -f 1); do
		make clean
		if [ $? != 0 ]; then
			echo "ERROR: make clean"
			exit 1
		fi

		echo "DEBUG: make $target"
		make $target
		if [ $? != 0 ]; then
			echo "ERROR: make $target"
			exit 2
		fi
	done
else
	make clean
	if [ $? != 0 ]; then
		echo "ERROR: make clean"
		exit 1
	fi

	echo "DEBUG: make $target"
		make $target
	if [ $? != 0 ]; then
		echo "ERROR: make $target"
		exit 2
	fi
fi
