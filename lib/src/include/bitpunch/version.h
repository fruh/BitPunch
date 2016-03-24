/*
 This file is part of BitPunch
 Copyright (C) 2014-2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
\mainpage BitPunch McEliece v0.0.4

\section Introduction
	Goal of BitPunch project is to make a standalone McEliece implementation. Current version is only develompent version, it is not suitable to use in real-life implementation.
	Our implementation of MECS uses Goppa codes. For ASN.1 we are using libtasn1 library (tested with version 3.4-3).
	- in Debian based distro install package:
		- apt-get install libtasn1-6 libtasn1-6-dev

	I know, there is a lot of BUGS now.
	If you have any questions, ideas create new issue or contact me:
		- xuhreckyf [ you know what to put here ] stuba.sk

    Where are the most important processes done?
        - Initialization:
            - lib/src/bitpunch/crypto/mecsctx.h
                - BPU_mecsInitCtx()
                - BPU_codeInitCtx()
                - BPU_mathInitCtx()

        - Encryption/Decryption:
            - lib/src/bitpunch/crypto/mecsbasic/mecsbasic.h
                - BPU_mecsBasicEncrypt()
                - BPU_mecsBasicDecrypt()
            - lib/src/bitpunch/crypto/cca2/mecspointcheval.h
                - BPU_mecsPointchevalCCA2Encrypt()
                - BPU_mecsPointchevalCCA2Decrypt()

        - KeyGen:
            - lib/src/bitpunch/code/goppa/goppa.h
                - BPU_goppaGenCode()

        - Encode/Decode:
            - lib/src/bitpunch/code/goppa/goppa.h
                - BPU_goppaEncode()
                - BPU_goppaDecode()

\section Structure
	Here is the basic structure overview.

	\subsection Convention
    Current version: 0.0.3
		- function name format: BPU_thisFunctionMakesMeDead
		- data types name format: BPU_T_This_Data_Type_Rocks
		- variable name format: four_your_information
		- all data types has prefix BPU_T_*
		- data types has also prefix, which are working with some general types:
			- BPU_T_GF2_*
			- BPU_T_GF2_16x_*
		- enumeration data types BPU_T_EN_*
			-enumeration etries BPU_EN_*
		- unoin data types BPU_T_UN_*
        - error codes definitions BPU_EC_*
		- all functions, macros has prefix BPU_*
		  - there is also specific prefix for functions, which are working with kind of data:
			- BPU_gf2Vec*
			- BPU_gf2xVec*
			- BPU_gf2xPoly*
			- BPU_gf2Mat*
			- BPU_gf2xMat*
			- BPU_perm*
			- BPU_code*
			- BPU_goppa*
			- BPU_mecs*
			- BPU_math*
			- BPU_pad*
			- BPU_prng*
		- print functions stating with prefix:
			-BPU_print*

	\subsection Modules
        BitPunch has more modules, all sources are located in lib/src/bitpunch/:
            - asn1
				- is used for searializing key using libtasn1 (tested with version 3.4-3)
                - it uses DER encoding
			- math
                - contains all arithmetic operation over GF2,  GF2x, polynomials, Matrices, Vectors, Permutations, ...
			- code
                - implements code matters, here you can specify your own codes
				- goppa
            - crypto
				- implements cryptosystem matters
				- mecsbasic, padding
            - prng
                - interface for PRNG

	\subsection Contexts
		There are 3 contexts:
			- Mecs context
				- this context registers encrypt, decrypt and key generation functions
			- Code context
				- this context registers encode, decode and code generation functions
			- Math context
				- speed up math operations over GF2x
		When you want to use MECS you just need to initialize context, using BPU_mecsInitCtx, see doc for more details.
  
\section Build
	For build details go to lib/ and see makefile help. Now you can build test aplication, realease or debug, you can specify wheter you want to use precomputed H matrix (more memory) or not:
        - make help - it is make help
        - make print-info - info where is your aplication
        - make test - with H matrix
        - make test-debug - with H matrix
        - make test-wo-h - without H matrix
        - make test-with-debug - without H matrix
        - make static - to build static lib
        - make shared - to build shared lib

    Builded libraries are located in dist/ folder. For more preprocesor options see lib/src/bitpunch/config.h file.
	Default optimization is -O2. To set up more verbosity define -DERRORL, -DWARNING_L or DDEBUG_L, this are using functions like BPU_printError(), BPU_printWarning(), BPU_printDebug().

\page Changes
v0.0.4
	- implemented QC-MDPC codes (A. Gulyas)
	- updated test enviroment and doc for dummy test

v0.0.3
	- added Adapted Pointcheval's scheme
	- implemented SHA512 from (https://polarssl.org)
	- BPU_*Malloc - now using TYPE **pointer and also BPU_*Free
	- implemented ASN1 key serialization using libtasn1 (external dependency) (https://www.gnu.org/software/libtasn1/) for mecs basic, Pointcheval's scheme, both with H matrix
	- some error checks fixed
	- added zero to support
	- added code and mecs parameters initialization
	- test enviroment reconfigured, now it creates snapshot of current workspace
	- some BUGs fixed
	- now possible to compile static or shared lib
	- compilation w/o print functions, enc, dec or keygen, CCA2 Pointchval's scheme

v0.0.2
	- module organization: crypto, math, code, prng
	- convention unified see doc
	- new context management: mecs, code and math context
	- memory usage reduction (heap peak) from 8 MiB to ~170 KiB with params m=11 t=50 w/o H
	- removed memory leaks
	- removed functions which are allocating memory inside for output (suffix A)
	- posibility to build with or w/o using H precomputed matrix by decription
	- added basic tests
	- removed Adapted CCA2 Pointcheval's scheme (TEMPORARLY)
	- no external dependecies

v0.0.1
	- Initial version - of BitPunch project
	- contains implementation of MECS using Goppa code
	- Patterson's alg. for decoding
	- dependencies on OpenSSL (sha)
	- Adapted CCA2 Pointcheval's scheme
	- some math function over gf2, gf2x, gf2xpoly like ext. eclidean, gen. of irreducible goppa poly...

\page "Test enviroment"

\section Intro
	There is implemented simple test enviroment. There are test which requires 'valgrind'.
- Debian based distro: apt-get install valgrind

You can run it using ./runTest TEST_NAME, or use help: ./runTest -h to see all options.
\verbatim
fruh@htfuws:~/projects/BitPunch$ ./runTest -h
Usage: runTest [options]

Options:
  --version             show program's version number and exit
  -h, --help            show this help message and exit
  -a, --all             Run all registered tests
  -t RUN_TEST, --test=RUN_TEST
						Run named tests
  -r RUN_LEVEL, --runLevel=RUN_LEVEL
						Run all tests <= runLevel
  -v VERBOSE_LEVEL, --verbose=VERBOSE_LEVEL
						Verbose level, can be ERROR, WARNING, INFO, DEBUG
  -l, --list            List available tests
\endverbatim

This test enviroment creates on every run snapshot of current workspace and copies it into runtest/results/ISO_TIME_FORMAT/ folder.

Test is considered as PASSED when return code is 0 (zero) and 'ERROR' string not occured at the beggining of the line. Otherwise, test is considered as FAILED.

\section Example
	Here is the guide, how to create dummy test using C and bash sources. C source represents test body and bash source represents control section.

	We can create new suite called "foosuite" or use existing. "Create" means to make directory "foosuite" in our test root:
	- runtest/tests/foosuite/

	Then we have to create our source files:
	- runtest/tests/foosuite/testFoo.sh (with EXECUTABLE RIGHTS)
	- runtest/tests/foosuite/testFoo.c

	Now we have to register this test, it means we have to add following record into TESTS python structure defined in:
	- runtest/tests/register.py
\verbatim
TESTS = {
	"testFooName" : {
		"suite" : "foosuite",
		"file" : "testFoo.sh",
		"args" : "testFoo fooarg2",
		"runLevel" : "regular",
	},
	... heere are other tests ...
}
\endverbatim

Now we can set up body of the control bash source. The first lines are required (until cd $1 (included)), first argument is still set to the path of the current workspace snapshot (this is done automatically by test enviroment):
- runtest/tests/foosuite/testFoo.sh
\verbatim
#!/bin/bash
if [ "$1" = "" ]; then
	echo "First argument is result folder."
	exit 1
fi

TESTFILE_DIR=$(pwd)
echo "INFO: $TESTFILE_DIR"

# now we are in current snapshot
cd "$1"

APP="./dist/test/fooBuild"

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
\endverbatim

And now we set test body:
- runtest/tests/foosuite/testFoo.c
\verbatim
#include <bitpunch/bitpunch.h>

#include <stdio.h>

int main(int argc, char **argv) {
	int rc = 0;
	// MUST BE NULL
	BPU_T_Mecs_Ctx *ctx = NULL;
	BPU_T_UN_Mecs_Params params;

	/***************************************/
	// mce initialisation t = 50, m = 11
	if (BPU_mecsInitParamsGoppa(&params, 11, 50, 0)) {
		return 1;
	}
	fprintf(stderr, "Basic GOPPA Initialisation...\n");
	if (BPU_mecsInitCtx(&ctx, &params, BPU_EN_MECS_BASIC_GOPPA)) {
		return 1;
	}
	BPU_mecsFreeCtx(&ctx);
	BPU_mecsFreeParamsGoppa(&params);

	return rc;
}
\endverbatim

We can see our registered test in the list of the tests:
- ./runTest -l
\verbatim
fruh@htfuws:~/projects/BitPunch$ ./runTest -l
Test: testMemRun                     runLevel: regular
Test: testFooName                    runLevel: regular
Test: testMemLeakWithH               runLevel: regular
Test: testBuild                      runLevel: express
Test: testMemLeakWoH                 runLevel: regular
Test: testLibSize                    runLevel: express
\endverbatim

We can run our dummy test using:
	 - ./runTest testFooName
 \verbatim
 fruh@htfuws:~/projects/BitPunch$ ./runTest testFooName
 Tests to run (1): testFooName
 ======== BEGIN::testFooName ========
 INFO: /media/fruh/data/projects/BitPunch/runtest/tests/foosuite
 INFO: fooarg1: testFoo
 INFO: fooarg2: fooarg2
 WARNING: warning message
 INFO: cleaning build...
 INFO: building static-lib...
 PASSED
 --------   END::testFooName --------
 ======== RESULTS ========
 Result folder: /media/fruh/data/projects/BitPunch/runtest/results/2015-05-17T23:51:36.709632
 FAILED: 0
 SKIPPED: 0
 PASSED: 1
 -------------------------
 \endverbatim

*/
#define VERSION = "0.0.4"
