/**
 This file is part of BitPunch
 Copyright (C) 2014 Frantisek Uhrecky <frantisek.uhrecky[at]gmail.com>
 Copyright (C) 2014 Andrej Gulyas <andrej.guly[at]gmail.com>
 Copyright (C) 2014 Marek Klein  <kleinmrk[at]gmail.com>
 Copyright (C) 2014 Filip Machovec  <filipmachovec[at]yahoo.com>
 Copyright (C) 2014 Jozef Kudlac Uhrecky <kudalc.jozef[at]gmail.com>

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
#ifndef INIT_H
#define INIT_H

#include "types.h"

#define GF2_POLY_DEG_5 0x3b
#define GF2_POLY_DEG_6 0x43
#define GF2_POLY_DEG_10 0x71d
#define GF2_POLY_DEG_11 0x805

/**
* Precalculate logaritmic and exponencial tables and initialize structure Aritmetic_Data
* @param g is a group generator
* @param mod modulus, ireducible polynomial
* @return number of elements or < 0 means error
*/
/// Precalculate logaritmic and exponencial tables and initialize structure Aritmetic_Data
int initLogExpTable(McEliece_Ctx *mce_ctx, GF2_16x g, GF2_16x mod);

/**
* Initialize all control H matricies (X, Y, Z). It uses McEl structure.
* @param[out] m poiter to Matrix_H structure
* @param[in] deg degree of goppa polynomial, number of errors
* @param[in] poly goppa polynomial over GF2x
* @param[in] a_data pointer to Aritmetic_Data structure, used by calculations
* @return 0 - on succes, else error
*/
/// Initialize all control H matricies (X, Y, Z)
int initMatH(Matrix_GF2_16x *m, Polynomial_GF2_16x *poly, Arithmetic_Data *a_data);

/**
* Initialize X matrix of t x t dimensions.
* @param[out] m pointer to matrix which should be initialized
* @param[in] t degree of goppa polynomial, number of errors
* @param[in] poly goppa polynomial over GF2x
*/
/// Initialize X matrix of t x t dimensions.
int initMatX(Matrix_GF2_16x *m, Polynomial_GF2_16x *poly);

/**
* Initialize Y matrix of t x n dimensions, n = ord of galois field.
* @param[out] m pointer to matrix which should be initialized
* @param[in] t degree of goppa polynomial, number of errors
* @param[in] a_data pointer to Aritmetic_Data structure, used by calculations
*/
/// Initialize Y matrix of t x n dimensions, where t = deg, n = ord of galois field.
int initMatY(Matrix_GF2_16x *m, uint8_t t, Arithmetic_Data *a_data);

/**
* Initialize Z matrix of n x n dimensions, where n = ord of galois field.
* @param[out] m pointer to Matrix_H structure
* @param[in] deg degree of goppa polynomial, number of errors
* @param[in] poly goppa polynomial over GF2x
* @param[in] a_data pointer to Aritmetic_Data structure, used by calculations
*/
/// Initialize Z matrix of n x n dimensions, where n is odr of galois field.
int initMatZ(Matrix_GF2_16x *m, Polynomial_GF2_16x *poly, Arithmetic_Data *a_data);

/**
* Hash input using SHA256 included in OpenSSL library. Output length is SHA256_DIGEST_LENGTH
* @retrun 0 if success, else -1
*/

//int gf2VecHashA(Vector_GF2 vec, unsigned char* hashed);
/**
* Initialize McEliece context. GF, Aritmetics, set degree of Goppa polynomial
*/
/// Initialize McEliece context.
int genKeyPair(McEliece_Ctx *mce_ctx, uint8_t deg);

/**
* Free allocated memory.
*/
/// Free allocated memory.
void freeMcElieceCtx(McEliece_Ctx *mce_ctx);

/**
 * Free dynamically or statically allocated matrix. 
 * @param[out] *m address of matrix object
 * @param[in] is_dyn boolean param, if 0 do not free mat object self (is statically allocated), just allocated matrix rows, cols, else free also m object
 */
/// Free dynamically or statically allocated matrix
void freeMat(Matrix_GF2_16x *m, int is_dyn);

/**
 * Free dynamiccaly or statically allocated Aritmetic_Data structure.
 * @param a      aaddress of Aritmetic_Data structure
 * @param is_dyn 0 - staticaly allocated Aritmetic_Data object or 1 when dynamically
 */
/// Free dynamiccaly or statically allocated Aritmetic_Data structure.
void freeArithmeticData(Arithmetic_Data *a, int is_dyn);

/**
 * Free dynamically or statically alocated permutation vector.
 * @param p      permutation to free
 * @param is_dyn 0 - staticaly allocated Permutation_Vector object or 1 when dynamically
 */
/// Free dynamically or statically alocated permutation vector.
void freePerm(Permutation_Vector *p, int is_dyn);

/**
 * Free dynamically or statically allocated matrix GF2. 
 * @param[out] *m address of matrix object
 * @param[in] is_dyn boolean param, if 0 do not free mat object self (is statically allocated), just allocated matrix rows, cols, else free also m object
 */
/// Free dynamically or statically allocated matrix
void freeMatGF2(Matrix_GF2 *m, int is_dyn);

/**
 * Free dynamically or statically allocated vector GF2. 
 * @param[out] *m address of vector object
 * @param[in] is_dyn boolean param, if 0 do not free mat object self (is statically allocated), just allocated vector cols, else free also m object
 */
/// Free dynamically or statically allocated vector
void freeVecGF2(Vector_GF2 *v, int is_dyn);

/**
 * Allocate permutation vector elements of size size using malloc.  After work you have to free memory using call freePerm(p)
 * @param  p    vector to be allocated	
 * @param  size size, permutation len
 * @return      on succes 0 else error
 */
/// Allocate permutation vector elements of size size.
int mallocPerm(Permutation_Vector *p, int size);

/**
 * Allocate memory for matrix. After work you have to free memory using call freeMat.
 * @param[out] m matrix to be allocated
 * @param[in] rows rows
 * @param[in] cols cols
 * @return on succes 0, else error
 */
 /// Allocate memory for matrix.
int mallocMatrix(Matrix_GF2_16x *m, int rows, int cols);

/**
 * Allocate memory for matrix GF2. It also nulls new matrix. After work you have to free memory using call freeMatGF2
 * @param rows rows
 * @param cols cols
 * @return on succes 0, else error
 */
int mallocMatrixGF2(Matrix_GF2 *m, int rows, int cols);

/**
 * Allocate memory for vector GF2. It also null vector. After work you have to free memory using call freeVecGF2.
 * @param len len of vector
 * @return on succes 0, else error
 */
int mallocVectorGF2(Vector_GF2 *v, int len) ;

/**
 * Create a binary vector of required length and weight. After work call freeVecGF2
 * @param  out GF2 vector representation (L)
 * @param  l length of vector
 * @param  w weight of vector, if 0 weight is random
 * @return     0 - succes, else error
 */
int initRandVector(Vector_GF2 *out, int l, int w);

/**
 * Malloc memory for polynomial and zero-initialize
 * @param  p representation of polynomial (every element is one coeficient)
 * @param  max_deg max degree of polynomial
 * @return
 */
int mallocPoly(Polynomial_GF2_16x *p, int16_t max_deg);

/**
 * dealloc memory
 * @param p
 * @param is_dyn
 */
void freePoly(Polynomial_GF2_16x *p, int is_dyn);

void mallocVectorGF2_16x(Vector_GF2_16x *vec, int size);

void freeVectorGF2_16x(Vector_GF2_16x *vec, int is_dyn);

#endif // INIT_H