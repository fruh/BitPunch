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
\mainpage BitPunch McEliece v0.0.3

\section Introduction
	Goal of BitPunch project is to make a standalone McEliece implementation. Current version is only develompent version, it is not suitable to use in real-life implementation.
	Our implementation of MECS uses Goppa codes.

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


\section Changes
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
                - is used for searializing key using libtasn1
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

	\subsection Tests
        There is implemented simple test enviroment. You can run it using ./runTest TEST_NAME, or use help: ./runTest -h to see all options.
  
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
	Default optimization is -O2. To set up more verbosity define -DERRORL, -DWARNING_L or DDEBUG_L, this are using functions like BPU_printError, BPU_printWarning, BPU_printDebug.
*/
#define VERSION = "0.0.3"
