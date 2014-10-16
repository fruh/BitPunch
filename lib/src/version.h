/*
 This file is part of BitPunch
 Copyright (C) 2014 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>

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

\section Structure
  Structure of BitPunch will be changed in short period.

  \subsection Convention
  Current version: 0.1.0
    - function name format: BPU_thisFunctionMakesMeDead
    - data types name format: BPU_T_This_Data_Type_Rocks
    - variable name format: four_your_information
    - all data types has prefix BPU_T_*
    - all functions, macros has prefix BPU_* 
      - there is also specific prefix for functions, which are working with kind of data:
        - BPU_gf2Vec*
        - BPU_gf2xVec*
        - BPU_gf2xPoly*
        - BPU_gf2Matrix*
        - BPU_gf2xMatrix*
        - BPU_perm*
    - functions, which are allocating memory for output has suffix *A

  \subsection Files
    - lib/src/arithmetics.h
      - here are some arithmetic function over GF2, GF2x like polynomial multiplication mod f(x), matrix mul ... 
    - lib/src/debugio.h
      - print functions
    - lib/src/globals.h
      - global var used mainly for testing puprose
    - lib/src/init.h
      - init functions for structure allocation
    - lib/src/mceliece.h
      - mecs API, se example lib/src/main.c
    - lib/src/operations.h
      - here are some operations with matrices, vectors like conversion from GF2 to GF2x, concat, append or polynomial inversion, extended euclidean over GF2x polynomials...
    - lib/src/process.h
      - here is encoding, decoding, encryption, decryption ...
    - lib/src/types.h
      - datatypes used in our implementation, vectors, matrices, GF2 ...
    - lib/src/version.h
      - only version info file
  
\section Build
  For build details see 
  - lib/makefile help.

\section Changes
  File structure will be changed to:
  - lib/src/bitpunch
    - crypto
      - crypto functions, so MECS implementation
    - math
      - all of 'math magic' used in BitPunch
    - asn1
      - asn1 compatibilty for keys storage ...
*/
VERSION = "0.1.0"