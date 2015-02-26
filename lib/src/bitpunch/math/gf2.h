/**
This file is part of BitPunch
Copyright (C) 2013-2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>
Copyright (C) 2013-2014 Andrej Gulyas <andrej.guly[what here]gmail.com>
Copyright (C) 2013-2014 Marek Klein  <kleinmrk[what here]gmail.com>
Copyright (C) 2013-2014 Filip Machovec  <filipmachovec[what here]yahoo.com>
Copyright (C) 2013-2014 Jozef Kudlac <jozef[what here]kudlac.sk>

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
#ifndef BPU_GF2_H
#define BPU_GF2_H

#include <stdio.h>
#include <string.h>

#include "permtypes.h"
#include "gf2types.h"
// #include "gf2xtypes.h"

/* ==================================== Print functions ==================================== */
// TODO: Be enable to turn off print function, to not compile in in code
/**
* Print number as binary string in big endian so msb is first printed. Do not print new line at the end.
* Example: number 21 -> 0001 0101
* @param in input of max 64 bits
* @param len print len
*/
/// Print number as binary string in big endian so msb is first printed.
void BPU_printBinaryMsb(uint64_t in, int len);

/**
* Print number as binary string in big endian so msb is first printed. Print also new line at the end.
* Example: number 21 -> 0001 0101
* @param in input of max 64 bits
* @param len print len
*/
/// Print number as binary string in big endian so msb is first printed.
void BPU_printBinaryMsbLn(uint64_t in, int len);

/**
* Print number as binary string in big endian so msb is first printed. Do not print new line at the end.
* Example: number 21 -> 0001 0101
* It will be padded to 64 bits.
* @param in input of max 64 bits
*/
/// Print number as binary string in big endian so msb is first printed.
void BPU_printBinaryMsb64(uint64_t in);

/**
* Print number as binary string in big endian so msb is first printed. Print also new line at the end.
* Example: number 21 -> 0001 0101
* It will be padded 64 bits.
* @param in input of max 64 bits
*/
/// Print number as binary string in big endian so msb is first printed.
void BPU_printBinaryMsb64Ln(uint64_t in);

/**
* Print number as binary string in little endian so lsb is first printed. Do not print new line at the end.
* Example: number 21 -> 1010 1000
* @param in input of max 64 bits
* @param len print len
*/
/// Print number as binary string in little endian so lsb is first printed.
void BPU_printBinaryLsb(uint64_t in, int len);

/**
* Print number as binary string in little endian so lsb is first printed. Print also new line at the end.
* Example: number 21 -> 1010 1000
* @param in input of max 64 bits
* @param len print len
*/
/// Print number as binary string in little endian so lsb is first printed.
void BPU_printBinaryLsbLn(uint64_t in, int len);

/**
* Print number as binary string in little endian so lsb is first printed. Do not print new line at the end.
* Example: number 21 -> 1010 1000
* It will be padded to 64 bits.
* @param in input of max 64 bits
*/
/// Print number as binary string in little endian so lsb is first printed.
void BPU_printBinaryLsb64(uint64_t in);

/**
* Print number as binary string in little endian so lsb is first printed. Print also new line at the end.
* Example: number 21 -> 1010 1000
* It will be padded 64 bits.
* @param in input of max 64 bits
*/
/// Print number as binary string in little endian so lsb is first printed.
void BPU_printBinary64LsbLn(uint64_t in);

/**
* Print matrix GF2 with new lines.
* @param m matrix
*/
void BPU_printGf2Mat(const BPU_T_GF2_Matrix *m);

/**
* Print vector GF2 with new line.
* @param v vector
*/
void BPU_printGf2Vec(const BPU_T_GF2_Vector *v);

void BPU_printGf2VecMsb(const BPU_T_GF2_Vector* v);

void BPU_printGf2VecOnes(const BPU_T_GF2_Vector *vec);
/* ------------------------------------ Print functions ------------------------------------ */


/**
 * Check if is set bit at n-th index makro.
 * @param w bit word to check
 * @param n index of bit to check, starts from 0
 * @return 1 - is set, else zero
 */
 /// Check if is set bit at n-th index.
#define BPU_getBit(w, n) ((int)((((uint64_t)w) >> (n)) & 1ul))

/**
 * Check if is set bit at s, t position in matrix.
 * @param m_pointer GF2 matrix address
 * @param s is row
 * @param t is col
 * @return 1 - is set, else zero
 */
 /// Check if is set bit at s, t position in matrix.
#define BPU_gf2MatGetBit(m_pointer, s, t) (BPU_getBit((m_pointer)->elements[s][(t) / (m_pointer)->element_bit_size], (t) % (m_pointer)->element_bit_size))

/**
* Check if is set bit at s, t position in vector.
* @param v_pointer GF2 vector address
* @param t is col
* @return 1 - is set, else zero
*/
/// Check if is set bit at i-th position in vector.
#define BPU_gf2VecGetBit(v_pointer, i) (BPU_getBit((v_pointer)->elements[(i) / (v_pointer)->element_bit_size], (i) % (v_pointer)->element_bit_size))

/**
 * Set bit in matrix GF2 at index s, t
 * @param  m_pointer address to matrix GF2
 * @param  s         row
 * @param  i         col index
 * @param  bit       bit value 0 or 1
 */
#define BPU_gf2MatSetBit(m_pointer, s, t, bit)  if (bit) { \
												(m_pointer)->elements[s][t / (m_pointer)->element_bit_size] |= ((BPU_T_GF2) 1) << ((t) % (m_pointer)->element_bit_size);\
												} \
												else { \
													/* this is like: 00101111 ^ 00000100 = 00101011 */\
													(m_pointer)->elements[s][(t) / (m_pointer)->element_bit_size] &= ((BPU_T_GF2) (0xFFFFFFFFFFFFFFFFul)) ^ (((BPU_T_GF2) 1) << ((t) % (m_pointer)->element_bit_size));\
												}

/**
 * Set bit in vector GF2 at index i
 * @param  v_pointer address to matrix GF2
 * @param  i         col index
 * @param  bit       bit value 0 or 1
 */
#define BPU_gf2VecSetBit(v_pointer, i, bit)  if (bit) { \
												(v_pointer)->elements[(i) / (v_pointer)->element_bit_size] |= ((BPU_T_GF2) 1) << ((i) % (v_pointer)->element_bit_size);\
												} \
												else { \
													/* this is like: 00101111 ^ 00000100 = 00101011 */\
													(v_pointer)->elements[(i) / (v_pointer)->element_bit_size] &= ((BPU_T_GF2) (0xFFFFFFFFFFFFFFFFul)) ^ (((BPU_T_GF2) 1) << ((i) % (v_pointer)->element_bit_size));\
												}

/**
 * Copy Matrix GF2 row to Vector GF2.
 * @param v_pointer[out] pointer to GF2 vector  
 * @param m_pointer[in] pointer to GF2 matrix 
 * @param row[in] row index
 */
 /// Copy Matrix GF2 row to Vector GF2.
#define BPU_gf2MatCopyRowToVec(v_pointer, m_pointer, row) memcpy((void *) ((v_pointer)->elements), (void *) ((m_pointer)->elements[row]), (v_pointer)->element_bit_size / 8 * (v_pointer)->elements_in_row)

int BPU_gf2MatCopy(BPU_T_GF2_Matrix *out, const BPU_T_GF2_Matrix *in);

/**
 * Permute matrix GF2 with permutation.
 * @param m matrix GF2
 * @param permutation permutation vector
 * @return on succes 0, on size error -1, on allocation error -2
 */
int BPU_gf2MatPermute(BPU_T_GF2_Matrix *m, BPU_T_Perm_Vector *permutation);

/**
 * Permute vector GF2 with permutation.
 * @param vec vector GF2
 * @param permutation permutation vector
 * @return on succes 0, on size error -1, on allocation error -2
 */
int BPU_gf2VecPermute(BPU_T_GF2_Vector *vec, const BPU_T_Perm_Vector *permutation);

/**
 * Get masked bit from exact row of Matrix GF2
 * @param m matrix GF2
 * @param row row
 * @param bit bit
 * @return masked bit
 */
BPU_T_GF2 BPU_gf2MatGetMaskedBit(const BPU_T_GF2_Matrix *m, uint32_t row, uint32_t bit);

/**
 * Get masked bit from Vector GF2
 * @param vec vector GF2
 * @param bit bit
 * @return masked bit
 */
BPU_T_GF2 BPU_gf2VecGetMaskedBit(const BPU_T_GF2_Vector *vec, uint32_t bit);

/**
 * Transpose GF2 Matrix.
 * @param  out transposed GF2 matrix
 * @param  in  matrix to transpose
 * @return     on success 0, else error
 */
int BPU_gf2MatTransp(BPU_T_GF2_Matrix *out, const BPU_T_GF2_Matrix *in);

/**
 * Swaps GF2 elements.
 * @param a pointer to GF2 element
 * @param b pointer to GF2 element 
 * @return
 */
void BPU_gf2Swap(BPU_T_GF2 *a, BPU_T_GF2 *b);

/**
 * It swaps i-th and j-th rows of Matrix GF2.
 * @param  mat Matrix GF2
 * @param  int i
 * @param  int j
 * @return permutation needed for equivalent codes
 */
void BPU_gf2MatSwapRows(BPU_T_GF2_Matrix *mat, int i, int j);

/**
 * Finds row in Matrix GF2 where 1 is at i-th position (from left). Looking for row starts at start_index.
 * @param  mat Matrix GF2
 * @param  i int
 * @param  start_index int
 * @return number of row on success, else -1 
 */
int BPU_gf2MatFindRow(const BPU_T_GF2_Matrix *mat, int i, int start_index);

/**
 * Finds column in Matrix GF2 where 1 is at i-th position (from top). Looking for column starts at start_index.
 * @param  mat Matrix GF2
 * @param  i int
 * @param  start_index int
 * @return number of row on success, else -1 
 */
int BPU_gf2MatFindCol(const BPU_T_GF2_Matrix *mat, int i, int start_index);

/**
 * It brings Matrix GF2 into the systematic form -> with I on the left side. After work, return value should be freed, it is allocated dynamically.
 * @param  inout Matrix GF2
 * @return       on success pointer to BPU_T_Perm_Vector
 */
/// It brings Matrix GF2 into the systematic form -> with I on the left side.
int BPU_gf2MatMakeSystematic(BPU_T_GF2_Matrix *inout);

/**
 * Concats two vectors without allocation ouput.
 * @param  vec1 first vector
 * @param  vec2 second vector
 * @param  out  output vector - concat of vec1 and vec2, it allocates vector inside function, should be freed after use BPU_freeVecGF2()
 * @return    0 - succes, else error
 */
/// Concats two vectors without allocation ouput.
int BPU_gf2VecConcat(BPU_T_GF2_Vector *out, const BPU_T_GF2_Vector *vec1, const BPU_T_GF2_Vector *vec2);

/**
 * Crop the vector
 * @param  out    output vector
 * @param  in     input vector
 * @param  start  starting position in vector
 * @param  length count of bits of cropped vector
 * @return    0 - succes, else error
 */
int BPU_gf2VecCrop(BPU_T_GF2_Vector *out, const BPU_T_GF2_Vector *in, const int start, const int length);

/**
 * Crop matrix GF2 from left.
 * @param in input matrix GF2
 * @param out croped output matrix GF2 
 * @param width width of cropped matrix
 * @return on succes 0, on higher or equal width as actual width of in matrix -1, on allocation of new matrix error -2
 */
/// Crop matrix GF2 from left.
int BPU_gf2MatCrop(BPU_T_GF2_Matrix *out, const BPU_T_GF2_Matrix *in, uint16_t width);

/**
 * Get matrix GF2 row as vector gf2. You can set alloc param, to allocate it dynamically inside function, or use allocated vector yet. When set alloc, after work call BPU_freeVecGF2
 * @param  out   vector
 * @param  in    matrix
 * @param  row   row index
 * @param  alloc if set to 0, jsut copy row to allocated vector yet, or also allocate output vector
 * @return       0 - success, else error
 */
int BPU_gf2MatGetRowAsGf2Vec(BPU_T_GF2_Vector *out, const BPU_T_GF2_Matrix *in, int row, int alloc);

/**
 * Copy VectorGF2.
 * @param dest[out] pointer to VectorGF2
 * @param src[in] pointer to VectorGF2  
 */
 /// Copy VectorGF2.
void BPU_gf2VecCopy(BPU_T_GF2_Vector *dest, const BPU_T_GF2_Vector *src);

int BPU_gf2VecCmp(const BPU_T_GF2_Vector *v1, const BPU_T_GF2_Vector *v2);

/**
 * XOR operation on rows of Matrix GF2. row[i] = row[i] ^ row[j]
 * @param  mat Matrix GF2
 * @param  int i
 * @param  int j
 * @return
 */
void BPU_gf2MatXorRows(BPU_T_GF2_Matrix *mat, int i, int j);

/**
 * Xor two Vectors GF2 and store result in first vector.
 * @param out[out] vector, where will be stored result
 * @param in[in]  vector
 * @return 0 - successm else error
 */
/// Xor two Vectors GF2 and store result in first vector.
int BPU_gf2VecXor(BPU_T_GF2_Vector *out, const BPU_T_GF2_Vector *in);

/**
 * Create a binary vector of required length and weight. After work call BPU_freeVecGF2
 * @param  out GF2 vector representation (L)
 * @param  l length of vector
 * @param  w weight of vector, if 0 weight is random
 * @return     0 - succes, else error
 */
int BPU_gf2VecRand(BPU_T_GF2_Vector *out, int l, int w);

/**
* Vector and matrix multiplication over GF2.
* x = v * b, vector is in 'line' format like [010111010001]. It selects i-th row from matrix b and xor it together, when vector v has at i-th column 1.  After work you have to free memory using call BPU_freeMat
* @param[out] x vector GF2, it is matrix k = 1, n = length of vec, new vector (matrix)
* @param[in] v vector GF2, it is matrix k = 1, n = length of vec
* @param[in] b matrix GF2
* @return on succes 0, on size error -1, on allocation error -2
*/
int BPU_gf2VecMulMat(BPU_T_GF2_Vector *x, const BPU_T_GF2_Vector *v, const BPU_T_GF2_Matrix *b);

int BPU_gf2MatCropMemory(BPU_T_GF2_Matrix *m, uint16_t width);
#endif // BPU_GF2_H
