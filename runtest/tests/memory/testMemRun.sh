TMP_FILE=/tmp/runtime.$$
trap "rm -f $TMP_FILE" EXIT

if [ "$1" = "" ]; then
	echo "First argument is result folder."
	exit 1
fi

TESTFILE_DIR=$(pwd)
echo $TESTFILE_DIR
cd "$1"
pwd

measure() {
	rc=0
	APP="./dist/test/$2"
	
	echo "INFO: cleaning build..."
	make clean || exit 1
	echo "INFO: building target $1 $3..."
	if [ "$3" != "" ]; then
		make static-lib "$3" || exit 1
	else
		make static-lib || exit 1
	fi
	gcc -Isrc/ $TESTFILE_DIR/testMemRun.c ./dist/libbpumecs.a -o $APP -lm || exit 1

	echo "INFO: running valgrind 'valgrind --tool=massif --massif-out-file=$2.out $APP'"
	valgrind --tool=massif --massif-out-file=$2.out $APP && echo "" && echo -n "INFO: Mem usage (B): " && cat $2.out | grep mem_heap_B | sed -e 's/mem_heap_B=\(.*\)/\1/' | sort -g | tail -n 1 2> $TMP_FILE || rc=$?
	grc=$?
	cat $TMP_FILE
	echo "INFO: valgrind done"

	if [ $grc -eq 0 -a $rc -eq 0 ]; then 
		echo "INFO: Memory usage measure done"
	else
		exit 2
	fi
}

measure static-lib keygenencdecasn1
measure static-lib keygenencdecasn1woh CFLAGS='-O2 -Wall -DBPU_CONF_FULL -DERROR_L -DBPU_CONF_GOPPA_WO_H'
exit 0
