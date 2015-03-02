TMP_FILE=/tmp/memleak.$$
rc=0
trap "rm -f $TMP_FILE" EXIT
cd $CWD/..
make clean || exit 1
make $1 || exit 1

valgrind --leak-check=yes $APP &> $TMP_FILE || rc=$?
grep 'All heap blocks were freed -- no leaks are possible' $TMP_FILE

if [[ $? == 0 && $rc == 0 ]]; then 
	exit 0
else
	cat $TMP_FILE
	exit 2
fi
