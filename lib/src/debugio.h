/**
* Here are some helpful print functions. i.e. print number as binary.
*/
#ifndef DEBUGIO_H
#define DEBUGIO_H

#include <stdio.h>


#ifndef _WIN32
	#include <inttypes.h>

    /// print macro for 64bit data types
	#define PRINT_U_64 PRIu64
#else
	#define PRINT_U_64 "I64u"
#endif

#include "types.h"
#include "operations.h"

#if defined(DEBUG_L) || defined(WARNING_L) || defined(ERROR_L)
    #include <stdlib.h>
    #include <errno.h>

    /// print error message with filename, line
    #define printError(fmt, ...) fprintf(stderr, "ERROR::%s::%d: "fmt"\n", __FILE__, __LINE__, ##__VA_ARGS__); if (errno) perror("ERROR TYPE:")
#else
    /// print error message with filename, line
    #define printError(fmt, ...)
#endif

#if defined(DEBUG_L) || defined(WARNING_L)
    /// print warning message with filename, line
    #define printWarning(fmt, ...) fprintf(stderr, "WARNING::%s::%d: "fmt"\n", __FILE__, __LINE__, ##__VA_ARGS__); if (errno) perror("ERROR TYPE:")
#else
    /// print warning message with filename, line
    #define printWarning(fmt, ...)
#endif

#if defined(DEBUG_L)
    /// print debug message with filename, line
    #define printDebug(fmt, ...) fprintf(stderr, "DEBUG::%s::%d: "fmt"\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
    /// print debug message with filename, line
    #define printDebug(fmt, ...)
#endif

/**
* Print number as binary string in big endian so msb is first printed. Do not print new line at the end.
* Example: number 21 -> 0001 0101
* @param in input of max 64 bits
* @param len print len
*/
/// Print number as binary string in big endian so msb is first printed.
void printBinary(uint64_t in, int len);

/**
* Print number as binary string in big endian so msb is first printed. Print also new line at the end.
* Example: number 21 -> 0001 0101
* @param in input of max 64 bits
* @param len print len
*/
/// Print number as binary string in big endian so msb is first printed.
void printBinaryLn(uint64_t in, int len);

/**
* Print number as binary string in big endian so msb is first printed. Do not print new line at the end.
* Example: number 21 -> 0001 0101
* It will be padded to 64 bits.
* @param in input of max 64 bits
*/
/// Print number as binary string in big endian so msb is first printed.
void printBinary64(uint64_t in);

/**
* Print number as binary string in big endian so msb is first printed. Print also new line at the end.
* Example: number 21 -> 0001 0101
* It will be padded 64 bits.
* @param in input of max 64 bits
*/
/// Print number as binary string in big endian so msb is first printed.
void printBinary64Ln(uint64_t in);

/**
* Print number as binary string in little endian so lsb is first printed. Do not print new line at the end.
* Example: number 21 -> 1010 1000
* @param in input of max 64 bits
* @param len print len
*/
/// Print number as binary string in little endian so lsb is first printed.
void printBinaryLe(uint64_t in, int len);

/**
* Print number as binary string in little endian so lsb is first printed. Print also new line at the end.
* Example: number 21 -> 1010 1000
* @param in input of max 64 bits
* @param len print len
*/
/// Print number as binary string in little endian so lsb is first printed.
void printBinaryLnLe(uint64_t in, int len);

/**
* Print number as binary string in little endian so lsb is first printed. Do not print new line at the end.
* Example: number 21 -> 1010 1000
* It will be padded to 64 bits.
* @param in input of max 64 bits
*/
/// Print number as binary string in little endian so lsb is first printed.
void printBinary64Le(uint64_t in);

/**
* Print number as binary string in little endian so lsb is first printed. Print also new line at the end.
* Example: number 21 -> 1010 1000
* It will be padded 64 bits.
* @param in input of max 64 bits
*/
/// Print number as binary string in little endian so lsb is first printed.
void printBinary64LnLe(uint64_t in);

/**
* Print number as binary string. Print also new line at the end.
* It will be padded 64 bits.
* @param in input of max 64 bits
*/
void printGf2xMat(const Matrix_GF2_16x *in);

/**
* Print matrix GF2 with new lines.
* @param m matrix
*/
void printGf2Mat(const Matrix_GF2 *m);

/**
* Print vector GF2 with new line.
* @param v vector
*/
void printGf2Vec(const Vector_GF2 *v);

/**
* Print permutation.
* @param permutation permutation
*/
void printPerm(const Permutation_Vector *permutation);


void printGf2xPoly(const Polynomial_GF2_16x *p, const Arithmetic_Data *a_data);

void printGf2xVec(const Vector_GF2_16x *v);

/**
 * Print McEliece context, so print all parameters.
 * @param mce_ctx pointer to McEliece_Ctx structure
 */
void printMceCtx(const McEliece_Ctx *mce_ctx);

void printGf2VecBe(const Vector_GF2* v);

void printGf2VecOnes(const Vector_GF2 *vec);

#endif // DEBUGIO_H