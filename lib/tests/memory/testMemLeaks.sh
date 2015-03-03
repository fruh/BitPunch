TMP_FILE=/tmp/memleak.$$
trap "rm -f $TMP_FILE" EXIT
rc=0
APP=./../../dist/test/BitPunch
pwd
cd ../../
pwd
echo "INFO: cleaning build..."
make clean || exit 1
echo "INFO: building target $1 ..."
make $1 || exit 1
cd tests/memory

echo "INFO: running valgrind 'valgrind --leak-check=yes $APP'"
valgrind --leak-check=yes $APP &> $TMP_FILE || rc=$?
grep 'All heap blocks were freed -- no leaks are possible' $TMP_FILE
grc=$?
echo "INFO: valgrind done"

cat $TMP_FILE

if [ $grc -ne 0 -a $rc -eq 0 ]; then 
	echo "INFO: SUCCESS none memory leaks found"
	exit 0
else
	exit 2
fi
