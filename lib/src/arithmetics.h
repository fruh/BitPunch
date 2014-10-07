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
/**
* Here are defined some basic aritmetic operations and functions to initialize structures.
*/
#ifndef ARITHMETICS_H
#define ARITHMETICS_H

#include "types.h"

/**
* Multiplication over Galois field, modulus mod.
* @param a first operant
* @param b second operant
* @param mod modulus, ireducible polynomial
* @return a*b mod mod
*/
/// Multiplication over Galois field, modulus mod.
GF2_16x gf2xMulMod(GF2_16x a, GF2_16x b, GF2_16x mod);

/**
* Multiplication over Galois field, modulus mod.
* It precalculated tables initLogExpTable
* @param a_data pointer to Aritmetic_Data structure, used by calculations
* @param a first operant, GF2x element
* @param b second operant, GF2x element
* @return result
*/
/// Multiplication over Galois field, modulus mod.
GF2_16x gf2xMulModT(const GF2_16x a, const GF2_16x b, const Arithmetic_Data *a_data);

/**
* E-th power of a. It uses precomputed log and exp tables.
* @param a_data pointer to Aritmetic_Data structure, used by calculations
* @param a GF2x element
* @param e exponent
* @return result
*/
/// E-th power of a. It uses precomputed log and exp tables
GF2_16x gf2xPowerModT(GF2_16x a, int e, const Arithmetic_Data *a_data);

/**
 * Matrix multiplication over finite field.  After work you have to free memory using call freeMat.
 * x = a*b
 * @param[out] x matrix GF2^m, new matrix
 * @param[in] a matrix GF2^m
 * @param[in] b matrix GF2^m
 * @return on succes 0, on size error -1, on allocation error -2
 */
int gf2xMatrixMulA(Matrix_GF2_16x *x, const Matrix_GF2_16x *a, const Matrix_GF2_16x *b, const Arithmetic_Data *a_data);

/**
* Vector and matrix multiplication over GF2.
* x = v * b, vector is in 'line' format like [010111010001]. It selects i-th row from matrix b and xor it together, when vector v has at i-th column 1.  After work you have to free memory using call freeMat
* @param[out] x vector GF2, it is matrix k = 1, n = length of vec, new vector (matrix)
* @param[in] v vector GF2, it is matrix k = 1, n = length of vec
* @param[in] b matrix GF2
* @return on succes 0, on size error -1, on allocation error -2
*/
int gf2VecMulMat(Vector_GF2 *x, const Vector_GF2 *v, const Matrix_GF2 *b);

/**
* Vector and matrix multiplication over GF2.
* x = v * b, vector is in 'line' format like [010111010001]. It selects i-th row from matrix b and xor it together, when vector v has at i-th column 1.  After work you have to free memory using call freeMat
* @param[out] x vector GF2, it is matrix k = 1, n = length of vec, new vector (matrix)
* @param[in] v vector GF2, it is matrix k = 1, n = length of vec
* @param[in] b matrix GF2
* @return on succes 0, on size error -1, on allocation error -2
*/
int gf2VecMulMatA(Vector_GF2 *x, const Vector_GF2 *v, const Matrix_GF2 *b);

/**
 * out = x * M over finite field
 * @param x   [description]
 * @param mat [description]
 * @param out [description]
 */
void gf2xVecMulMat(Vector_GF2_16x *out, const Vector_GF2_16x *x, const Matrix_GF2_16x *mat, const Arithmetic_Data *a_data);

/**
 * XOR operation on rows of Matrix GF2. row[i] = row[i] ^ row[j]
 * @param  mat Matrix GF2
 * @param  int i
 * @param  int j
 * @return
 */
void gf2MatXorRows(Matrix_GF2 *mat, int i, int j);

/**
 * Xor two Vectors GF2 and store result in first vector.
 * @param out[out] vector, where will be stored result
 * @param in[in]  vector
 * @return 0 - successm else error
 */
/// Xor two Vectors GF2 and store result in first vector.
int gf2VecXor(Vector_GF2 *out, const Vector_GF2 *in);

/**
 * Plus operation on polynomials (out = a + b). All argumets must be allocated before using mallocPoly().
 * @param out [description]
 * @param a   [description]
 * @param b   [description]
 */
void gf2xPolyAdd(Polynomial_GF2_16x *out, const Polynomial_GF2_16x *a, const Polynomial_GF2_16x *b);

/**
 * Divide two polynomials. All argumets must be allocated before using mallocPoly().
 * @param q [out] output quocient
 * @param r [out] output reminder
 * @param a [in] input polynomial
 * @param b [in] input polynomial
 */
void gf2xPolyDiv(Polynomial_GF2_16x *q, Polynomial_GF2_16x *r, const Polynomial_GF2_16x *a, const Polynomial_GF2_16x *b, const Arithmetic_Data *a_data);

/**
 * Multiplicate two polynomials. All argumets must be allocated before using mallocPoly().
 * @param a   [in] input polynomial
 * @param b   [in] input polynomial
 * @param out [out] output polynomial
 * @param a_data log and exp table needed for multiplication of elements in GF2^m
 */
void gf2xPolyMul(Polynomial_GF2_16x *out, const Polynomial_GF2_16x *a, const Polynomial_GF2_16x *b, const Arithmetic_Data *a_data);

/**
 * Shift polynomial right, it is like a div x^n.
 * @param a[inout] input/output polynomial
 * @param n[in] shift
 */
/// Shift polynomial right, it is like a div x^n.
void gf2xPolyShr(Polynomial_GF2_16x *a, int n);

/**
 * Shift polynomial left, it is like a mul 1/x^n.
 * @param a[inout] input/output polynomial
 * @param n[in] shift
 */
/// Shift polynomial left, it is like a mul 1/x^n.
void gf2xPolyShl(Polynomial_GF2_16x *a, int n);

/**
 * Calculate power of polynomial.
 * @param a[inout] polynomial to be 'powered'
 * @param e exponent
 * @param a_data aitmetic data structure.
 */
/// Calculate power of polynomial.
void gf2xPolyPower(Polynomial_GF2_16x *a, int e, const Arithmetic_Data *a_data);

/**
 * Multiplication polynomial over GF2_16x and element from GF2_16x. It is like multiplication of polynomials
 * @param a      polynomial to be multiplicated
 * @param el     multiplicator
 * @param a_data arithmetic data structure
 */
/// Multiplication polynomial over GF2_16x and element from GF2_16x.
void gf2xPolyMulEl(Polynomial_GF2_16x *a, GF2_16x el, const Arithmetic_Data *a_data);

/**
 * Calculate reminder of a. Example a mod b = reminder. ALl arguments must be allocated before use.
 * @param a[in] input polynomial over GF2_16x
 * @param mod[in] input polynomial modul over GF2_16x
 * @param out[out] output polynomial reminder over GF2_16x, must be allocated first
 * @param a_data[in] aritmetics data structure
 */
/// Calculate reminder of a. Example a mod b = reminder.
void gf2xPolyMod(Polynomial_GF2_16x *out, const Polynomial_GF2_16x *a, const Polynomial_GF2_16x *mod, const Arithmetic_Data *a_data);

/**
 * Compute root of polynomial poly modulo polynomial mod and save result to polynomial out.
 * Matrix out is allocated inside of the function.
 * @param poly   [description]
 * @param mod    [description]
 * @param out    [description]
 * @param a_data [description]
 */
void gf2xMatRootA(Matrix_GF2_16x *out, const Polynomial_GF2_16x *mod, const Arithmetic_Data *a_data);

/**
 * Function returns sqrt(element)
 * @param  element [description]
 * @param  a_data  [description]
 * @return         [description]
 */
GF2_16x gf2xRoot(GF2_16x element, const Arithmetic_Data *a_data);

void gf2xPolyRoot(Polynomial_GF2_16x *out, const Polynomial_GF2_16x *poly, const Polynomial_GF2_16x *mod, const Arithmetic_Data *a_data);

#endif // ARITHMETICS_H
