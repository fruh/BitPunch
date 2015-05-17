#!/bin/bash
if [ "$1" = "" ]; then
        echo "First argument is result folder."
        exit 1
fi

TESTFILE_DIR=$(pwd)
echo "INFO: $TESTFILE_DIR"
APP="./dist/test/$2"

# now we are in current snapshot
cd "$1"

# control body
echo "INFO: fooarg1: $2"
echo "INFO: fooarg2: $3"

echo "WARNING: warning message"

echo "INFO: cleaning build..."
make clean
if [ $? != 0 ]; then
        echo "ERROR: make clean"
        exit 1
fi

echo "INFO: building static-lib..."
make static-lib
if [ $? != 0 ]; then
        echo "ERROR: building target static-lib"
        exit 2
fi
gcc -Isrc/ $TESTFILE_DIR/testFoo.c ./dist/libbpumecs.a -o $APP || exit 1

# run app
$APP

# return code
exit $?