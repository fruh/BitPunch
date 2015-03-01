CWD=$(dirname $0)
APP=$CWD/../dist/test/BitPunch
TMP_FILE=/tmp/memleak.$$

echo "Running: $0"

trap "rm -f $TMP_FILE; cd $CWD/tests" EXIT
cd $CWD/..
make clean || exit 1
make test-with-h-debug || exit 1
cd $CWD/tests

valgrind --leak-check=yes $APP &> $TMP_FILE
grep 'All heap blocks were freed -- no leaks are possible' $TMP_FILE

if [ $? == 0 ]; then 
	echo "PASSED"
	exit 0
else
	cat $TMP_FILE
	echo
	echo FAILED
	exit 2
fi
