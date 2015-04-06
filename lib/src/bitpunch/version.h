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
\mainpage BitPunch McEliece

\section Introduction
	Goal of BitPunch project is to make a standalone McEliece implementation. Current version is only develompent version, it is not suitable to use in real-life implementation.
	Our implementation of MECS uses Goppa codes.

	I know, there is a lot of BUGS now.
	If you have any questions, ideas create new issue or contact me:
		- xuhreckyf [ you know what to put here ] stuba.sk

\section Changes
	v0.0.2
		- module organization: crypto, math, code, prng
		- convention unified see doc
		- new context managment: mecs, code and math context
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
	Current version: 0.0.2
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
			- BPU_perm*
		- print functions stating with prefix:
			-BPU_print*

	\subsection Modules
		BitPunch has more modules, are sources are lcated in lib/src/bitpunch/:
			- math
				- contains all aruthmetic operation over GF2,  GF2x, polynomials, Matrices, Vectors, Permutations, ...
			- code
				- implement code matters, here you can specify your own codes
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
		In lib/tests are some basic tests. You can run them:
			- go to lib/
			- run them using:
				- ./runAllTests
				- or go to lib/tests/ and use ./runTest TESTNAME ARGS.
		Now there is only build test and memory leak check tests.
  
\section Build
	For build details go to lib/ and see makefile help. Now you can build test aplication, realease or debug, you can specify wheter you want to use precomputed H matrix (more memory) or not:
		- make help - it is make help
		- make print-info - info where is your aplication
		- make test-wo-h
		- make test-wo-h-debug
		- make test-with
		- make test-with-debug
	Default optimization is -O2. To set up more verbosity define -DERRORL, -DWARNING_L or DDEBUG_L, this are using functions like BPU_printError, BPU_printWarning, BPU_printDebug.
*/
#define VERSION = "0.0.2"
