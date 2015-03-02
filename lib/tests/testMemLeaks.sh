TMP_FILE=/tmp/memleak.$$

trap "rm -f $TMP_FILE; cd $CWD/tests" EXIT
cd $CWD/..
make clean || exit 1
make $1 || exit 1
cd $CWD/tests

valgrind --leak-check=yes $APP &> $TMP_FILE
grep 'All heap blocks were freed -- no leaks are possible' $TMP_FILE

if [ $? == 0 ]; then 
	exit 0
else
	cat $TMP_FILE
	exit 2
fi
