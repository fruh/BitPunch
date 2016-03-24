/*
This file is part of BitPunch
Copyright (C) 2013-2016 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>
Copyright (C) 2013-2015 Andrej Gulyas <andrej.guly[what here]gmail.com>
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

#include <bitpunch/config.h>
#include "perm.h"
#include "gf2.h"
#include "int.h"
#include "uni.h"


/**
* Binary representation GF2
*/
typedef BPU_T_Element BPU_T_GF2;

/**
 * Row vector GF2 representation.
 * Every element is one bit.
 */
typedef BPU_T_Element_Array BPU_T_GF2_Vector;

/**
* Matrix representation over GF2.
* Every element is one bit.
*/
typedef struct _BPU_T_GF2_Matrix {
    BPU_T_GF2 **elements;       ///< all element of matrix
    uint8_t element_bit_size;   ///< element size, is sizeof(BPU_T_GF2) i.e. 64 bits
    uint16_t elements_in_row;   ///< number of elements in one row
    uint32_t k;                 ///< rows
    uint32_t n;                 ///< cols
} BPU_T_GF2_Matrix;

/**
 * Polynomial over GF2, represented as Vector.
 * Indexes are stored in elements, degree of polynomial is len - 1
 */
typedef BPU_T_GF2_Vector BPU_T_GF2_Poly;

/**
 * Sparse polynomial over GF2.
 * More effective than BPU_T_GF2_Poly for low weight polynomials.
 * Every set coefficient is stored in array of indexes.
 */
typedef struct _BPU_T_GF2_Sparse_Poly {
    uint32_t *index;            ///<indexes of set coefficients. 0 is coefficient for x^0
    uint32_t weight;            ///<weight of polynomial
} BPU_T_GF2_Sparse_Poly;

/**
 * Quasi-cyclic matrix over GF2.
 * Cyclic matrices are stored as polynomials.
 * Matrix can have multiple cyclic matrices (but just one in row and more in columns, or in reverse)
 * The orientation of is set in isVertical.
 * For appended identity matrix in left set is_I_appended to 1.
 */
typedef struct _BPU_T_GF2_QC_Matrix {
    BPU_T_GF2_Poly *matrices;   ///< all cyclic matrices of matrix
    uint16_t element_count;     ///< number of cyclic matrices
    uint32_t element_size;      ///< size of cyclic matrix
    uint8_t isVertical;         ///< if 1, elements are in vertical orientation, if 0 horizontal orientation
    uint32_t n;                 ///< cols of whole matrix
    uint32_t k;                 ///< rows of whole matrix
    uint8_t is_I_appended;      ///< if matrix has identity matrix on left
} BPU_T_GF2_QC_Matrix;

/**
 * Quasi-cyclic matrix over GF2 with sparse cyclic matrices.
 * More effective than BPU_T_QC_Matrix_GF2 for low weight cyclic matrices.
 * Matrix can have multiple cyclic matrices (but just one in row and more in columns, or in reverse)
 * The orientation of is set in isVertical.
 */
typedef struct _BPU_T_GF2_Sparse_Qc_Matrix {
    BPU_T_GF2_Sparse_Poly *matrices;    ///< all cyclic matrices of matrix
    uint16_t element_count;     ///< number of cyclic matrices
    uint32_t element_size;      ///< size of cyclic matrix
    uint8_t isVertical;         ///< if 1, elements are in vertical orientation, if 0 horizontal orientation
    uint32_t n;                 ///< cols of whole matrix
    uint32_t k;                 ///< rows of whole matrix
} BPU_T_GF2_Sparse_Qc_Matrix;

#ifdef BPU_CONF_PRINT
/* ==================================== Print functions ==================================== */
// TODO: add possibility of turning off print functions, do not compile
/**
* Print matrix GF2 with new lines.
* @param m matrix
*/
void BPU_printGf2Mat(const BPU_T_GF2_Matrix * m);

/**
* Print vector GF2 with new line.
* @param v vector
*/
#define BPU_printGf2Vec(v) BPU_printElementArray(v)

/**
 * @brief BPU_printGf2VecMsb Most significant bit is printed first.
 * @param v
 */
#define BPU_printGf2VecMsb(v) BPU_printElementArrayMsb(v)

/**
 * @brief BPU_printGf2VecOnes Print only ones.
 * @param vec
 */
#define BPU_printGf2VecOnes(v) BPU_printElementArrayOnes(v)


/**
 * Print sparse polynomial.
 * @param v sparse polynomial to print
 */
void BPU_printGf2SparsePoly(const BPU_T_GF2_Sparse_Poly * v);

/**
 * Print polynomial over GF2 for matrix.
 * Without header and new line.
 * @param v polynomial to print
 */
void BPU_printGf2PolyForMatrix(const BPU_T_GF2_Poly * v);

/**
 * Print polynomial.
 * @param v polynomial to print
 */
void BPU_printGf2Poly(const BPU_T_GF2_Poly * v);

/**
 * Print quasi-cyclic matrix.
 * @param v quasi-cyclic matrix to print
 */
void BPU_printGf2QcMatrix(const BPU_T_GF2_QC_Matrix * v);

/**
 * Print sparse quasi-cyclic matrix.
 * @param v sparse quasi-cyclic matrix to print
 */
void BPU_printGf2SparseQcMatrix(const BPU_T_GF2_Sparse_Qc_Matrix * v);

/* ------------------------------------ Print functions ------------------------------------ */
#endif // BPU_CONF_PRINT

/**
 * Null GF2 matrix row.
 * @param[out]  m_pointer pointer to GF2 matrix
 * @param[in]  row       row to null
 */
/// Null GF2 matrix row.
#define BPU_gf2MatNullRow(m_pointer, row) memset((void *) ((m_pointer)->elements[row]), 0, (m_pointer)->element_bit_size / 8 * (m_pointer)->elements_in_row)

/**
 * Null GF2 vector.
 * @param[out]  v_pointer pointer to GF2 vector
 */
/// Null GF2 vector.
#define BPU_gf2VecNull(v_pointer) BPU_elementArrayNull(v_pointer)

/**
 * Free dynamically or statically allocated matrix GF2.
 * @param[out] *m address of matrix object
 */
/// Free dynamically or statically allocated matrix
void BPU_gf2MatFree(BPU_T_GF2_Matrix ** m);

/**
 * Free dynamically or statically allocated vector GF2.
 * @param[out] *m address of vector object
 */
/// Free dynamically or statically allocated vector
#define BPU_gf2VecFree(v_double_pointer) BPU_elementArrayFree(v_double_pointer)

/**
 * Allocate memory for matrix GF2. It also nulls new matrix. After work you have to free memory using call BPU_freeMatGF2
 * @param rows rows
 * @param cols cols
 * @return on succes 0, else error
 */
int BPU_gf2MatMalloc(BPU_T_GF2_Matrix ** m, int rows, int cols);

/**
 * Allocate memory for vector GF2. It also null vector. After work you have to free memory using call BPU_freeVecGF2.
 * @param len len of vector
 * @return on succes 0, else error
 */
#define BPU_gf2VecMalloc(v_double_pointer, len) BPU_elementArrayMalloc(v_double_pointer, len)

/**
 * @brief BPU_gf2VecResize Resize vecor.
 * @param v
 * @param len
 * @return
 */
#define BPU_gf2VecResize(v_pointer, len) BPU_elementArrayResize(v_pointer, len)

/**********************************************************
gf2 POLY
**********************************************************/

/**
 * Allocate memory for polynomial over GF2. Allocate memory, so after work it has to be freed by using call BPU_gf2PolyFree.
 * @param p polynomial to allocate
 * @param len length of polynomial (length - 1 = degree of polynomial)
 * @return on succes 0, else error
 */
int BPU_gf2PolyMalloc(BPU_T_GF2_Poly * p, int len);

/**
 * Free dynamically or statically allocated polynomial over GF2.
 * @param p polynomial to free
 * @param is_dyn boolean param, if 0 do not free object self, else free also object
 */
void BPU_gf2PolyFree(BPU_T_GF2_Poly * p, int is_dyn);


/**********************************************************
gf2 SPARSE POLY
**********************************************************/

/**
 * Allocate memory for sparse polynomial over GF2. Allocate memory, so after work it has to be freed by using call BPU_gf2SparsePolyFree.
 * @param p sparse polynomial to allocate
 * @param weight weight of sparse polynomial
 */
void BPU_gf2SparsePolyMalloc(BPU_T_GF2_Sparse_Poly * p, int weight);

/**
 * Free dynamically or statically allocated sparse polynomial over GF2.
 * @param p sparse polynomial to free
 * @param is_dyn boolean param, if 0 do not free object self, else free also object
 */
void BPU_gf2SparsePolyFree(BPU_T_GF2_Sparse_Poly * p, int is_dyn);


/**********************************************************
gf2 QUASI-CYCLIC MATRIX
**********************************************************/

/**
 * Allocate memory for quasi-cyclic matrix over GF2. Allocate memory, so after work it has to be freed by using call BPU_gf2QcMatrixFree.
 * @param v quasi-cyclic matrix
 * @param element_count count of cyclic elements
 * @param element_size size of one cyclic element
 * @param isVertical boolean, if 1, elements are in vertical orientation, if 0 horizontal orientation
 * @param is_I_appended boolean, if 1, identity matrix is appended to the left, if 0, no identity matrix
 * @return 0 - succes, else error
 */
int BPU_gf2QcMatrixMalloc(BPU_T_GF2_QC_Matrix * v, int element_count,
                          int element_size, int isVertical, int is_I_appended);

/**
 * Free dynamically or statically allocated quasi-cyclic matrix over GF2.
 * @param v quasi-cyclic matrix
 * @param is_dyn boolean param, if 0 do not free object self, else free also object
 */
void BPU_gf2QcMatrixFree(BPU_T_GF2_QC_Matrix * v, int is_dyn);


/**********************************************************
gf2 SPARSE QUASI-CYCLIC MATRIX
**********************************************************/

/**
 * Allocate memory for sparse quasi-cyclic matrix over GF2. Allocate memory, so after work it has to be freed by using call BPU_gf2SparseQcMatrixFree.
 * @param v sparse quasi-cyclic matrix
 * @param element_count count of cyclic elements
 * @param element_size size of one cyclic element
 * @param isVertical boolean, if 1, elements are in vertical orientation, if 0 horizontal orientation
 */
void BPU_gf2SparseQcMatrixMalloc(BPU_T_GF2_Sparse_Qc_Matrix * v,
                                 int element_count, int element_size,
                                 int isVertical);


/**
 * Free dynamically or statically allocated sparse quasi-cyclic matrix over GF2.
 * @param v sparse quasi-cyclic matrix
 * @param is_dyn boolean param, if 0 do not free object self, else free also object
 */
void BPU_gf2SparseQcMatrixFree(BPU_T_GF2_Sparse_Qc_Matrix * v, int is_dyn);

/**
 * Check if is set bit at n-th index makro.
 * @param w bit word to check
 * @param n index of bit to check, starts from 0
 * @return 1 - is set, else zero
 */
 /// Check if is set bit at n-th index.
#define BPU_getBit(w, n) ((int)((((uint32_t)w) >> (n)) & 1u))

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
#define BPU_gf2VecGetBit(v_pointer, i) BPU_elementArrayGetBit(v_pointer, i)

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
													(m_pointer)->elements[s][(t) / (m_pointer)->element_bit_size] &= ((BPU_T_GF2) (0xFFFFFFFFu)) ^ (((BPU_T_GF2) 1) << ((t) % (m_pointer)->element_bit_size));\
												}

/**
 * Set bit in vector GF2 at index i
 * @param  v_pointer address to matrix GF2
 * @param  i         col index
 * @param  bit       bit value 0 or 1
 */
#define BPU_gf2VecSetBit(v_pointer, i, bit)  BPU_elementArraySetBit(v_pointer, i, bit)

/**
 * Copy Matrix GF2 row to Vector GF2.
 * @param v_pointer[out] pointer to GF2 vector  
 * @param m_pointer[in] pointer to GF2 matrix 
 * @param row[in] row index
 */
 /// Copy Matrix GF2 row to Vector GF2.
#define BPU_gf2MatCopyRowToVec(v_pointer, m_pointer, row) memcpy((void *) ((v_pointer)->elements), (void *) ((m_pointer)->elements[row]), (v_pointer)->element_bit_size / 8 * (v_pointer)->array_length)

/**
 * Check if is set coeff with index bit in poly.
 * @param  poly polynomial
 * @param  bit  index of coefficient
 * @return      1 if is set bit, else 0
 */
#define BPU_gf2PolyGetBit(poly, bit) ((poly->elements[bit/poly->element_bit_size] >> (bit%poly->element_bit_size)) & 1ul)

/**
 * @brief BPU_gf2MatCopyCreate copy of input matrix.
 * @param out
 * @param in
 * @return
 */
int BPU_gf2MatCopy(BPU_T_GF2_Matrix * out, const BPU_T_GF2_Matrix * in);

/**
 * Permute matrix GF2 with permutation.
 * @param m matrix GF2
 * @param permutation permutation vector
 * @return on succes 0, on size error -1, on allocation error -2
 */
int BPU_gf2MatPermute(BPU_T_GF2_Matrix * m, BPU_T_Perm_Vector * permutation);

/**
 * Permute vector GF2 with permutation.
 * @param vec vector GF2
 * @param permutation permutation vector
 * @return on succes 0, on size error -1, on allocation error -2
 */
int BPU_gf2VecPermute(BPU_T_GF2_Vector * vec,
                      const BPU_T_Perm_Vector * permutation);

/**
 * Get masked bit from exact row of Matrix GF2
 * @param m matrix GF2
 * @param row row
 * @param bit bit
 * @return masked bit
 */
BPU_T_GF2 BPU_gf2MatGetMaskedBit(const BPU_T_GF2_Matrix * m, uint32_t row,
                                 uint32_t bit);

/**
 * Get masked bit from Vector GF2
 * @param vec vector GF2
 * @param bit bit
 * @return masked bit
 */
BPU_T_GF2 BPU_gf2VecGetMaskedBit(const BPU_T_GF2_Vector * vec, uint32_t bit);

/**
 * Transpose GF2 Matrix.
 * @param  out transposed GF2 matrix
 * @param  in  matrix to transpose
 * @return     on success 0, else error
 */
int BPU_gf2MatTransp(BPU_T_GF2_Matrix * out, const BPU_T_GF2_Matrix * in);

/**
 * Swaps GF2 elements.
 * @param a pointer to GF2 element
 * @param b pointer to GF2 element 
 * @return
 */
void BPU_gf2Swap(BPU_T_GF2 * a, BPU_T_GF2 * b);

/**
 * It swaps i-th and j-th rows of Matrix GF2.
 * @param  mat Matrix GF2
 * @param  int i
 * @param  int j
 * @return permutation needed for equivalent codes
 */
void BPU_gf2MatSwapRows(BPU_T_GF2_Matrix * mat, int i, int j);

/**
 * Finds row in Matrix GF2 where 1 is at i-th position (from left). Looking for row starts at start_index.
 * @param  mat Matrix GF2
 * @param  i int
 * @param  start_index int
 * @return number of row on success, else -1 
 */
int BPU_gf2MatFindRow(const BPU_T_GF2_Matrix * mat, int i, int start_index);

/**
 * Finds column in Matrix GF2 where 1 is at i-th position (from top). Looking for column starts at start_index.
 * @param  mat Matrix GF2
 * @param  i int
 * @param  start_index int
 * @return number of row on success, else -1 
 */
int BPU_gf2MatFindCol(const BPU_T_GF2_Matrix * mat, int i, int start_index);

/**
 * It brings Matrix GF2 into the systematic form -> with I on the left side. After work, return value should be freed, it is allocated dynamically.
 * @param  inout Matrix GF2
 * @return       on success pointer to BPU_T_Perm_Vector
 */
/// It brings Matrix GF2 into the systematic form -> with I on the left side.
int BPU_gf2MatMakeSystematic(BPU_T_GF2_Matrix * inout);

/**
 * Concats two vectors without allocation ouput.
 * @param  vec1 first vector
 * @param  vec2 second vector
 * @param  out  output vector - concat of vec1 and vec2, it allocates vector inside function, should be freed after use BPU_freeVecGF2()
 * @return    0 - succes, else error
 */
/// Concats two vectors without allocation ouput.
int BPU_gf2VecConcat(BPU_T_GF2_Vector * out, const BPU_T_GF2_Vector * vec1,
                     const BPU_T_GF2_Vector * vec2);

/**
 * Crop the vector
 * @param  out    output vector
 * @param  in     input vector
 * @param  start  starting position in vector
 * @param  length count of bits of cropped vector
 * @return    0 - succes, else error
 */
int BPU_gf2VecCrop(BPU_T_GF2_Vector * out, const BPU_T_GF2_Vector * in,
                   const int start, const int length);

/**
 * Get matrix GF2 row as vector gf2. You can set alloc param, to allocate it dynamically inside function, or use allocated vector yet. When set alloc, after work call BPU_freeVecGF2
 * @param  out   vector
 * @param  in    matrix
 * @param  row   row index
 * @param  alloc if set to 0, jsut copy row to allocated vector yet, or also allocate output vector
 * @return       0 - success, else error
 */
int BPU_gf2MatGetRowAsGf2Vec(BPU_T_GF2_Vector * out,
                             const BPU_T_GF2_Matrix * in, int row);

/**
 * Copy VectorGF2.
 * @param dest[out] pointer to VectorGF2
 * @param src[in] pointer to VectorGF2  
 */
 /// Copy VectorGF2.
void BPU_gf2VecCopy(BPU_T_GF2_Vector * dest, const BPU_T_GF2_Vector * src);

/**
 * @brief BPU_gf2VecCmp Compare two vectors.
 * @param v1
 * @param v2
 * @return 0 - same, -1 - len differs, > 0 (index + 1) of first difference
 */
int BPU_gf2VecCmp(const BPU_T_GF2_Vector * v1, const BPU_T_GF2_Vector * v2);

/**
 * XOR operation on rows of Matrix GF2. row[i] = row[i] ^ row[j]
 * @param  mat Matrix GF2
 * @param  int i
 * @param  int j
 * @return
 */
void BPU_gf2MatXorRows(BPU_T_GF2_Matrix * mat, int i, int j);

/**
 * Xor two Vectors GF2 and store result in first vector.
 * @param out[out] vector, where will be stored result
 * @param in[in]  vector
 * @return 0 - successm else error
 */
/// Xor two Vectors GF2 and store result in first vector.
int BPU_gf2VecXor(BPU_T_GF2_Vector * out, const BPU_T_GF2_Vector * in);

/**
 * Create a binary vector of required length and weight. After work call BPU_freeVecGF2
 * @param  out GF2 vector representation (L)
 * @param  w weight of vector, if 0 weight is random
 * @return     0 - succes, else error
 */
int BPU_gf2VecRand(BPU_T_GF2_Vector * out, int w);

/**
* Vector and matrix multiplication over GF2.
* x = v * b, vector is in 'line' format like [010111010001]. It selects i-th row from matrix b and xor it together, when vector v has at i-th column 1.  After work you have to free memory using call BPU_freeMat
* @param[out] x vector GF2, it is matrix k = 1, n = length of vec, new vector (matrix)
* @param[in] v vector GF2, it is matrix k = 1, n = length of vec
* @param[in] b matrix GF2
* @return on succes 0, on size error -1, on allocation error -2
*/
int BPU_gf2VecMulMat(BPU_T_GF2_Vector * x, const BPU_T_GF2_Vector * v,
                     const BPU_T_GF2_Matrix * b);

/**
 * Crop matrix GF2 from left.
 * @param in input matrix GF2
 * @param out croped output matrix GF2
 * @param width width of cropped matrix
 * @return on succes 0, on higher or equal width as actual width of in matrix -1, on allocation of new matrix error -2
 */
/// Crop matrix GF2 from left.
int BPU_gf2MatCrop(BPU_T_GF2_Matrix * m, uint16_t width);

/**
 * @brief BPU_getParity Get parity of word.
 * @param dword
 * @return
 */
uint8_t BPU_getParity(BPU_T_GF2 dword);

/************************************************
POLYNOMIAL UTILS
************************************************/

/**
 * Is poly zero?
 * @param  a poly
 * @return   if 1, poly is zero, else 0
 */
int BPU_gf2PolyIsZero(const BPU_T_GF2_Poly * a);

/**
 * Find highest coefficient in polynomial and set degree.
 * Reallocating new size.
 * @param a   poly
 * @param deg if -1, set degree = max poly degree, else set given degree
 */
void BPU_gf2PolySetDeg(BPU_T_GF2_Poly * a, int deg);

/**
 * Returns highest set coefficient in polynomial.
 * @param  a poly
 * @return   Index of highest set coefficient in polynomial.
 */
int BPU_gf2PolyGetHighestBitPos(BPU_T_GF2_Poly * a);

/**
 * Shift polynomial left by given positions.
 * @param a           polynomial to be shifted
 * @param shift_count count of shiftes to do
 */
void BPU_gf2PolyShiftLeft(BPU_T_GF2_Poly * a, int shift_count);

/**
 * Shift polynomial right by one.
 * @param  a polynomial to be shifted
 */
void BPU_gf2PolyShiftRightOne(BPU_T_GF2_Poly * a);

/**
 * Transpose polynomial.
 * @param out transposed output polynomial
 * @param in  input polynomial
 */
void BPU_gf2PolyTransp(BPU_T_GF2_Poly * out, const BPU_T_GF2_Poly * in);


/************************************************
POLYNOMIAL MATH
************************************************/

/**
 * Multiplicate polynomial by x.
 * @param a input / output polynomial
 */
void BPU_gf2PolyMulX(BPU_T_GF2_Poly * a);

/**
 * Polynomial add.
 * out = out + in
 * If is set crop to 1, it will set new polynomial degree.
 * Else it will have degree of maximum degree of out and in poly.
 * @param out  input / output polynomial
 * @param in   intput polynomial
 * @param crop boolean, if set actual degree of output
 */
void BPU_gf2PolyAdd(BPU_T_GF2_Poly * out, const BPU_T_GF2_Poly * in, int crop);

/**
 * Polynomial multiplication modulo.
 * c = a * b mod m
 * If is set crop to 1, it will set new polynomial degree.
 * Else it will have degree of degree(modulo) - 1.
 * @param a    factor
 * @param b    factor
 * @param c    product
 * @param m    modulo
 * @param crop boolean, if set actual degree of output
 */
void BPU_gf2PolyMulMod(const BPU_T_GF2_Poly * a, const BPU_T_GF2_Poly * b,
                       BPU_T_GF2_Poly * c, const BPU_T_GF2_Poly * m, int crop);

/**
 * Polynomial division.
 * q = a / b
 * r = a % b
 * @param q quotient
 * @param r reminder
 * @param a dividend
 * @param b divisor
 */
void BPU_gf2PolyDiv(BPU_T_GF2_Poly * q, BPU_T_GF2_Poly * r,
                    const BPU_T_GF2_Poly * a, const BPU_T_GF2_Poly * b);

/**
 * Extended Euclidean algorithm (XGCD).
 * a*s + b*t = d
 * @param  d gcd(a,b)
 * @param  s Bézout coefficient s
 * @param  t Bézout coefficient t
 * @param  a poly a
 * @param  b poly b
 * @param  m modulo
 */
void BPU_gf2PolyExtEuclidA(BPU_T_GF2_Poly * d, BPU_T_GF2_Poly * s,
                           BPU_T_GF2_Poly * t, const BPU_T_GF2_Poly * a,
                           const BPU_T_GF2_Poly * b, const BPU_T_GF2_Poly * m);

/**
 * Calc inversion polynomial.
 * out = a^-1 mod m 
 * @param  out inversion to a
 * @param  a   polynomial
 * @param  m   modulo
 * @return     1 if found, else not found
 */
int BPU_gf2PolyInv(BPU_T_GF2_Poly * out, const BPU_T_GF2_Poly * a,
                   const BPU_T_GF2_Poly * mod);


/************************************************
SPARSE POLYNOMIAL UTILS
************************************************/

/**
 * Copy sparse polynomial. After work you have to free memory using call BPU_gf2SparsePolyFree.
 * @param out output sparse polynomial
 * @param in input sparse polynomial 
 */
void BPU_gf2SparsePolyCopy(BPU_T_GF2_Sparse_Poly * out,
                           const BPU_T_GF2_Sparse_Poly * in);

/**
 * Copy polynomial. After work you have to free memory using call BPU_gf2PolyFree.
 * @param out output polynomial
 * @param in input polynomial 
 */
void BPU_gf2PolyCopy(BPU_T_GF2_Poly * out, const BPU_T_GF2_Poly * in);

/**
 * Initialize random polynomial. After work you have to free memory using call BPU_gf2PolyFree.
 * @param out output polynomial
 * @param l length of polynomial (length - 1 = degree of polynomial)
 * @param w weight of polynomial
 * @param set_deg boolean param, if 1, it will crop polynomial to the actual degree and reallocate it
 * @return 0 - succes, else error
 */
int BPU_gf2PolyInitRand(BPU_T_GF2_Poly * out, int l, int w, int set_deg);


/************************************************
SPARSE POLYNOMIAL MATH
************************************************/

/**
 * Polynomial add sparse polynomial.
 * out = out + in
 * @param out  input / output polynomial
 * @param in   intput sparse polynomial
 */
void BPU_gf2SparsePolyAdd(BPU_T_GF2_Poly * out,
                          const BPU_T_GF2_Sparse_Poly * in);

/**
 * Polynomial and sparse polynomial, returns hamming weight of result.
 * hw(and(a,b))
 * @param  a polynomial
 * @param  b sparse polynomial
 * @return   hamming weight of result of and operation
 */
int BPU_gf2SparsePolyAndHW(const BPU_T_GF2_Poly * a,
                           const BPU_T_GF2_Sparse_Poly * b);


/************************************************
QUASI-CYCLIC MATRIX UTILS
************************************************/

/**
 * Transpose quasi-cyclic matrix.
 * @param out output transposed matrix
 * @param in  input matrix
 */
void BPU_gf2QcMatrixTransp(BPU_T_GF2_QC_Matrix * out,
                           const BPU_T_GF2_QC_Matrix * in);

/**
 * Convert quasi-cyclic matrix into sparse quasi-cyclic matrix. Allocate memory, so after work it has to be freed by using call BPU_gf2SparseQcMatrixFree.
 * @param out output sparse quasi-cyclic matrix
 * @param input quasi-cyclic matrix
 * @param wi weights of cyclic elements in input matrix
 * @return
 */
int BPU_gf2QcMatrixToSparse(BPU_T_GF2_Sparse_Qc_Matrix * out,
                            const BPU_T_GF2_QC_Matrix * in, const int wi[]);


/************************************************
SPARSE QUASI-CYCLIC MATRIX UTILS
************************************************/

/**
 * Get row from sparse quasi-cyclic matrix.
 * @param m       sparse QC matrix
 * @param p       output row
 * @param row_num index of row to get
 */
void BPU_gf2SparseQcMatrixGetRow(BPU_T_GF2_Sparse_Poly * p,
                                 const BPU_T_GF2_Sparse_Qc_Matrix * m,
                                 int row_num);

/**
 * Transpose sparse quasi-cyclic matrix.
 * @param  out output transposed matrix
 * @param  in  input matrix
 */
void BPU_gf2SparseQcMatrixTransp(BPU_T_GF2_Sparse_Qc_Matrix * out,
                                 const BPU_T_GF2_Sparse_Qc_Matrix * in);

#endif // BPU_GF2_H
