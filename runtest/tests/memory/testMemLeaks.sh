TMP_FILE=/tmp/memleak.$$
trap "rm -f $TMP_FILE" EXIT

if [ "$1" = "" ]; then
	echo "First argument is result folder."
	exit 1
fi
rc=0
APP="./dist/test/BitPunch"
pwd
cd "$1"
pwd
echo "INFO: cleaning build..."
make clean || exit 1
echo "INFO: building target $2 ..."
make -j4 $2 || exit 1

echo "INFO: running valgrind 'valgrind --leak-check=yes --track-origins=yes $APP'"
valgrind --leak-check=yes $APP 2> $TMP_FILE || rc=$?
grep 'All heap blocks were freed -- no leaks are possible' $TMP_FILE
grc=$?
cat $TMP_FILE
echo "INFO: valgrind done"

if [ $grc -eq 0 -a $rc -eq 0 ]; then 
	echo "INFO: SUCCESS none memory leaks found"
	exit 0
else
	exit 2
fi
