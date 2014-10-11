/**
 This file is part of BitPunch
 Copyright (C) 2014 Frantisek Uhrecky <frantisek.uhrecky[at]gmail.com>
 Copyright (C) 2014 Andrej Gulyas <andrej.guly[at]gmail.com>
 Copyright (C) 2014 Marek Klein  <kleinmrk[at]gmail.com>
 Copyright (C) 2014 Filip Machovec  <filipmachovec[at]yahoo.com>
 Copyright (C) 2014 Jozef Kudlac <jozef[at]kudlac.sk>

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
* Here are some helpful print functions. i.e. print number as binary.
*/
#ifndef DEBUGIO_H
#define DEBUGIO_H

#include <stdio.h>


#ifndef _WIN32
  #include <inttypes.h>

  /// print macro for 64bit data types
  #define BPU_PRINT_U_64 PRIu64
#else
  #define BPU_PRINT_U_64 "I64u"
#endif

#include "types.h"
#include "operations.h"

#if defined(DEBUG_L) || defined(WARNING_L) || defined(ERROR_L)
  #include <stdlib.h>
  #include <errno.h>

  /// print error message with filename, line
  #define BPU_printError(fmt, ...) fprintf(stderr, "ERROR::%s::%d: "fmt"\n", __FILE__, __LINE__, ##__VA_ARGS__); if (errno) perror("ERROR TYPE:")
#else
  /// print error message with filename, line
  #define BPU_printError(fmt, ...)
#endif

#if defined(DEBUG_L) || defined(WARNING_L)
  /// print warning message with filename, line
  #define BPU_printWarning(fmt, ...) fprintf(stderr, "WARNING::%s::%d: "fmt"\n", __FILE__, __LINE__, ##__VA_ARGS__); if (errno) perror("ERROR TYPE:")
#else
  /// print warning message with filename, line
  #define BPU_printWarning(fmt, ...)
#endif

#if defined(DEBUG_L)
  /// print debug message with filename, line
  #define BPU_printDebug(fmt, ...) fprintf(stderr, "DEBUG::%s::%d: "fmt"\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
  /// print debug message with filename, line
  #define BPU_printDebug(fmt, ...)
#endif

/**
* Print number as binary string in big endian so msb is first printed. Do not print new line at the end.
* Example: number 21 -> 0001 0101
* @param in input of max 64 bits
* @param len print len
*/
/// Print number as binary string in big endian so msb is first printed.
void BPU_printBinary(uint64_t in, int len);

/**
* Print number as binary string in big endian so msb is first printed. Print also new line at the end.
* Example: number 21 -> 0001 0101
* @param in input of max 64 bits
* @param len print len
*/
/// Print number as binary string in big endian so msb is first printed.
void BPU_printBinaryLn(uint64_t in, int len);

/**
* Print number as binary string in big endian so msb is first printed. Do not print new line at the end.
* Example: number 21 -> 0001 0101
* It will be padded to 64 bits.
* @param in input of max 64 bits
*/
/// Print number as binary string in big endian so msb is first printed.
void BPU_printBinary64(uint64_t in);

/**
* Print number as binary string in big endian so msb is first printed. Print also new line at the end.
* Example: number 21 -> 0001 0101
* It will be padded 64 bits.
* @param in input of max 64 bits
*/
/// Print number as binary string in big endian so msb is first printed.
void BPU_printBinary64Ln(uint64_t in);

/**
* Print number as binary string in little endian so lsb is first printed. Do not print new line at the end.
* Example: number 21 -> 1010 1000
* @param in input of max 64 bits
* @param len print len
*/
/// Print number as binary string in little endian so lsb is first printed.
void BPU_printBinaryLe(uint64_t in, int len);

/**
* Print number as binary string in little endian so lsb is first printed. Print also new line at the end.
* Example: number 21 -> 1010 1000
* @param in input of max 64 bits
* @param len print len
*/
/// Print number as binary string in little endian so lsb is first printed.
void BPU_printBinaryLnLe(uint64_t in, int len);

/**
* Print number as binary string in little endian so lsb is first printed. Do not print new line at the end.
* Example: number 21 -> 1010 1000
* It will be padded to 64 bits.
* @param in input of max 64 bits
*/
/// Print number as binary string in little endian so lsb is first printed.
void BPU_printBinary64Le(uint64_t in);

/**
* Print number as binary string in little endian so lsb is first printed. Print also new line at the end.
* Example: number 21 -> 1010 1000
* It will be padded 64 bits.
* @param in input of max 64 bits
*/
/// Print number as binary string in little endian so lsb is first printed.
void BPU_printBinary64LnLe(uint64_t in);

/**
* Print number as binary string. Print also new line at the end.
* It will be padded 64 bits.
* @param in input of max 64 bits
*/
void BPU_printGf2xMat(const BPU_T_Matrix_GF2_16x *in);

/**
* Print matrix GF2 with new lines.
* @param m matrix
*/
void BPU_printGf2Mat(const BPU_T_Matrix_GF2 *m);

/**
* Print vector GF2 with new line.
* @param v vector
*/
void BPU_printGf2Vec(const BPU_T_Vector_GF2 *v);

/**
* Print permutation.
* @param permutation permutation
*/
void BPU_printPerm(const BPU_T_Perm_Vector *permutation);

void BPU_printGf2xPoly(const BPU_T_Poly_GF2_16x *p, const BPU_T_Arithmetic_Data *a_data);

void BPU_printGf2xVec(const BPU_T_Vector_GF2_16x *v);

/**
 * Print McEliece context, so print all parameters.
 * @param mce_ctx pointer to BPU_T_McEliece_Ctx structure
 */
void BPU_printMceCtx(const BPU_T_McEliece_Ctx *mce_ctx);

void BPU_printGf2VecBe(const BPU_T_Vector_GF2* v);

void BPU_printGf2VecOnes(const BPU_T_Vector_GF2 *vec);

#endif // DEBUGIO_H