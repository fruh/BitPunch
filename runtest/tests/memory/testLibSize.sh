#!/bin/bash
if [ "$1" = "" ]; then
	echo "First argument is result folder."
	exit 1
fi
cd "$1"

build () {
	echo "INFO: cleaning build..."
	make clean
	if [ $? != 0 ]; then
		echo "ERROR: make clean"
		exit 1
	fi

	echo "INFO: building target $1 $2 $3..."
	make -j4 $1 $2 "$3"
	if [ $? != 0 ]; then
		echo "ERROR: building target $target"
		exit 2
	fi
	echo "INFO: Library sizes (KiB):"
	echo "INFO: $(du -k dist/libbpumecs.a)"
	echo "INFO: $(du -k dist/libbpumecs.so)"
}
build static-lib shared-lib CFLAGS='-O2 -Wall -DBPU_CONF_FULL -DERROR_L'
build static-lib shared-lib CFLAGS='-O2 -Wall -DBPU_CONF_FULL -DBPU_CONF_NO_PRINT'
build static-lib shared-lib CFLAGS='-O2 -Wall -DBPU_CONF_ENCRYPTION -DBPU_CONF_NO_PRINT'
build static-lib shared-lib CFLAGS='-O2 -Wall -DBPU_CONF_DECRYPTION -DBPU_CONF_NO_PRINT'
build static-lib shared-lib CFLAGS='-O2 -Wall -DBPU_CONF_FULL_MECS -DBPU_CONF_NO_PRINT'
build static-lib shared-lib CFLAGS='-O2 -Wall -DBPU_CONF_FULL_MECS -DBPU_CONF_NO_PRINT -DBPU_CONF_MECS_CCA2_POINTCHEVAL_GOPPA'
build static-lib shared-lib CFLAGS='-O2 -Wall -DBPU_CONF_FULL_MECS -DBPU_CONF_NO_PRINT -DBPU_CONF_MECS_CCA2_POINTCHEVAL_QCMDPC'
exit 0
