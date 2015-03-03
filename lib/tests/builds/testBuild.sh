#!/bin/bash
if [ "$1" == "" ]; then
	target=default
else
	target=$1
fi
cd ../../

if [ "$1" == "" ]; then
	# for each target
	for target in $(grep -E '^\w.*:' makefile | cut -d ':' -f 1); do
		echo "INFO: cleaning build..."
		make clean
		if [ $? != 0 ]; then
			echo "ERROR: make clean"
			exit 1
		fi

		echo "INFO: building target $target ..."
		make $target
		if [ $? != 0 ]; then
			echo "ERROR: building target $target"
			exit 2
		fi
	done
else
	echo "INFO: cleaning build..."
	make clean
	if [ $? != 0 ]; then
		echo "ERROR: make clean"
		exit 1
	fi

	echo "INFO: building target $target ..."
		make $target
	if [ $? != 0 ]; then
		echo "ERROR: building target $target"
		exit 2
	fi
fi
