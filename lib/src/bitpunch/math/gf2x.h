/*
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
#ifndef BPU_GF2X_H
#define BPU_GF2X_H

#include <stdint.h>

#include <bitpunch/config.h>

#include "gf2.h"
#include "gf2xtypes.h"
#include "mathctx.h"

#ifdef BPU_CONF_PRINT
/* ==================================== Print functions ==================================== */
/**
* Print number as binary string. Print also new line at the end.
* It will be padded 64 bits.
* @param in input of max 64 bits
*/
void BPU_printGf2xMat(const BPU_T_GF2_16x_Matrix *in);

/**
 * @brief BPU_printGf2xPoly print GF2x polynomial.
 * @param p
 * @param math_ctx
 */
void BPU_printGf2xPoly(const BPU_T_GF2_16x_Poly *p, const BPU_T_Math_Ctx *math_ctx);

/**
 * @brief BPU_printGf2xVec print GF2x vector.
 * @param v
 */
void BPU_printGf2xVec(const BPU_T_GF2_16x_Vector *v);
/* ------------------------------------ Print functions ------------------------------------ */
#endif // BPU_CONF_PRINT

/**
 * Get inverse element of galois field.
 * @param  gf2_16x_e find inverse element to this element
 * @param  math_ctx_p pointer to BPU_T_Math_Ctx structure
 * @return inverse element
 */
/// Get inverse element of galois field.
#define BPU_gf2xInvElement(gf2_16x_e, math_ctx_p) ((gf2_16x_e) == 1 ? 1 : (math_ctx_p)->exp_table[((math_ctx_p)->ord - (math_ctx_p)->log_table[gf2_16x_e])])

/**
 * Get leading coefitient of Polynomal over GF2x.
 * @param poly_gf2_16x_p pointer to BPU_T_Poly_GF2_16x structure
 * @return leading coef (element from GF2^16) or 0
 */
/// * Get leading coefitient of Polynomal over GF2x.
#define BPU_gf2xPolyLeadCoef(poly_gf2_16x_p) ((poly_gf2_16x_p)->deg > -1 ? (poly_gf2_16x_p)->coef[(poly_gf2_16x_p)->deg] : 0)

/**
* Multiplication over Galois field, modulus mod.
* @param a first operant
* @param b second operant
* @param mod modulus, ireducible polynomial
* @return a*b mod mod
*/
/// Multiplication over Galois field, modulus mod.
BPU_T_GF2_16x BPU_gf2xMulMod(BPU_T_GF2_16x a, BPU_T_GF2_16x b, BPU_T_GF2_16x mod);

/**
* Multiplication over Galois field, modulus mod.
* It precalculated tables BPU_initLogExpTable
* @param math_ctx pointer to Aritmetic_Data structure, used by calculations
* @param a first operant, GF2x element
* @param b second operant, GF2x element
* @return result
*/
/// Multiplication over Galois field, modulus mod.
//BPU_T_GF2_16x BPU_gf2xMulModT(const BPU_T_GF2_16x a, const BPU_T_GF2_16x b, const BPU_T_Math_Ctx *math_ctx);
#define BPU_gf2xMulModT(a, b, math_ctx) (((a) == 0 || (b) == 0) ? 0 : (math_ctx)->exp_table[((math_ctx)->log_table[(a)] + (math_ctx)->log_table[(b)]) % (math_ctx)->ord])

/**
* E-th power of a. It uses precomputed log and exp tables.
* @param math_ctx pointer to Aritmetic_Data structure, used by calculations
* @param a GF2x element
* @param e exponent
* @return result
*/
/// E-th power of a. It uses precomputed log and exp tables
BPU_T_GF2_16x BPU_gf2xPowerModT(BPU_T_GF2_16x a, int e, const BPU_T_Math_Ctx *math_ctx);

/**
 * Matrix multiplication over finite field..
 * x = a*b
 * @param[out] x matrix GF2^m, new matrix
 * @param[in] a matrix GF2^m
 * @param[in] b matrix GF2^m
 * @return on succes 0, on size error -1, on allocation error -2
 */
int BPU_gf2xMatMul(BPU_T_GF2_16x_Matrix *x, const BPU_T_GF2_16x_Matrix *a, const BPU_T_GF2_16x_Matrix *b, const BPU_T_Math_Ctx *math_ctx);

/**
 * out = x * M over finite field
 * @param x   [description]
 * @param mat [description]
 * @param out [description]
 */
void BPU_gf2xVecMulMat(BPU_T_GF2_16x_Vector *out, const BPU_T_GF2_16x_Vector *x, const BPU_T_GF2_16x_Matrix *mat, const BPU_T_Math_Ctx *math_ctx);

/**
 * Plus operation on polynomials (out = a + b). All argumets must be allocated before using BPU_mallocPoly().
 * @param out [description]
 * @param a   [description]
 * @param b   [description]
 */
void BPU_gf2xPolyAdd(BPU_T_GF2_16x_Poly *out, const BPU_T_GF2_16x_Poly *a, const BPU_T_GF2_16x_Poly *b);

/**
 * Divide two polynomials. All argumets must be allocated before using BPU_mallocPoly().
 * @param q [out] output quocient
 * @param r [out] output reminder
 * @param a [in] input polynomial
 * @param b [in] input polynomial
 */
void BPU_gf2xPolyDiv(BPU_T_GF2_16x_Poly *q, BPU_T_GF2_16x_Poly *r, const BPU_T_GF2_16x_Poly *a, const BPU_T_GF2_16x_Poly *b, const BPU_T_Math_Ctx *math_ctx);

/**
 * Multiplicate two polynomials. All argumets must be allocated before using BPU_mallocPoly().
 * @param a   [in] input polynomial
 * @param b   [in] input polynomial
 * @param out [out] output polynomial
 * @param math_ctx log and exp table needed for multiplication of elements in GF2^m
 */
void BPU_gf2xPolyMul(BPU_T_GF2_16x_Poly *out, const BPU_T_GF2_16x_Poly *a, const BPU_T_GF2_16x_Poly *b, const BPU_T_Math_Ctx *math_ctx);

/**
 * Shift polynomial right, it is like a div x^n.
 * @param a[inout] input/output polynomial
 * @param n[in] shift
 */
/// Shift polynomial right, it is like a div x^n.
void BPU_gf2xPolyShr(BPU_T_GF2_16x_Poly *a, int n);

/**
 * Shift polynomial left, it is like a mul 1/x^n.
 * @param a[inout] input/output polynomial
 * @param n[in] shift
 */
/// Shift polynomial left, it is like a mul 1/x^n.
void BPU_gf2xPolyShl(BPU_T_GF2_16x_Poly *a, int n);

/**
 * Calculate power of polynomial.
 * @param a[inout] polynomial to be 'powered'
 * @param e exponent
 * @param math_ctx aitmetic data structure.
 */
/// Calculate power of polynomial.
void BPU_gf2xPolyPower(BPU_T_GF2_16x_Poly *a, int e, const BPU_T_Math_Ctx *math_ctx);

/**
 * Multiplication polynomial over GF2_16x and element from GF2_16x. It is like multiplication of polynomials
 * @param a      polynomial to be multiplicated
 * @param el     multiplicator
 * @param math_ctx arithmetic data structure
 */
/// Multiplication polynomial over GF2_16x and element from GF2_16x.
void BPU_gf2xPolyMulEl(BPU_T_GF2_16x_Poly *a, BPU_T_GF2_16x el, const BPU_T_Math_Ctx *math_ctx);

/**
 * Calculate reminder of a. Example a mod b = reminder. ALl arguments must be allocated before use.
 * @param a[in] input polynomial over GF2_16x
 * @param mod[in] input polynomial modul over GF2_16x
 * @param out[out] output polynomial reminder over GF2_16x, must be allocated first
 * @param math_ctx[in] aritmetics data structure
 */
/// Calculate reminder of a. Example a mod b = reminder.
void BPU_gf2xPolyMod(BPU_T_GF2_16x_Poly *out, const BPU_T_GF2_16x_Poly *a, const BPU_T_GF2_16x_Poly *mod, const BPU_T_Math_Ctx *math_ctx);

/**
 * Compute root of polynomial poly modulo polynomial mod and save result to polynomial out.
 * Matrix out is allocated inside of the function.
 * @param poly   [description]
 * @param mod    [description]
 * @param out    [description]
 * @param math_ctx [description]
 */
void BPU_gf2xMatRoot(BPU_T_GF2_16x_Matrix *out, const BPU_T_GF2_16x_Poly *mod, const BPU_T_Math_Ctx *math_ctx);

/**
 * Function returns sqrt(element)
 * @param  element [description]
 * @param  math_ctx  [description]
 * @return         [description]
 */
BPU_T_GF2_16x BPU_gf2xRoot(BPU_T_GF2_16x element, const BPU_T_Math_Ctx *math_ctx);

/**
 * @brief BPU_gf2xPolyRoot calculate root of given polynomial over GF2x.
 * @param out
 * @param poly
 * @param mod
 * @param math_ctx
 */
void BPU_gf2xPolyRoot(BPU_T_GF2_16x_Poly *out, const BPU_T_GF2_16x_Poly *poly, const BPU_T_GF2_16x_Poly *mod, const BPU_T_Math_Ctx *math_ctx);

/**
 * Copy Polynomial.
 * @param dest[out] pointer to GF2_16x polynomial 
 * @param src[in] pointer to GF2_16x polynomial 
 */
 /// Copy Polynomial.
void BPU_gf2xPolyCopy(BPU_T_GF2_16x_Poly *dest, const BPU_T_GF2_16x_Poly *src);

/**
* Get degree of polynomial over GF2.
* @param pol input polynomial
* @return degree of pol or -1 if it is pol = 0
*/
/// Get degree of polynomial over GF2
int BPU_gf2xGetDeg(BPU_T_GF2_32x pol);

/**
* Get degree of polynomial over GF2x.
* @param pol input polynomial
* @return degree of pol or -1 if it is pol = 0
*/
/// Get degree of polynomial over GF2x
int BPU_gf2xPolyGetDeg(BPU_T_GF2_16x_Poly *poly);

/**
 * Permute matrix GF2^m with permutation.
 * @param m matrix GF2^m
 * @param permutation permutation vector
 * @return on succes 0, on size error -1, on allocation error -2
 */
int BPU_gf2xMatPermute(BPU_T_GF2_16x_Matrix *out, const BPU_T_GF2_16x_Matrix *m, const BPU_T_Perm_Vector *permutation);

/**
 * Converts matrix GF2m to matrix GF2.
 * @param m matrix GF2m
 * @param deg degree of elements from GF2m
 * @param out output matrix GF2
 * @return on succes 0, else error
 */
int BPU_gf2xMatConvertToGf2Mat(BPU_T_GF2_Matrix *out, const BPU_T_GF2_16x_Matrix *m, int deg);

/**
 * Evaluate polynomial over GF2^m with x.
 * @param math_ctx pointer to Aritmetic_Data structure, used by calculations
 * @param poly polynomial to evaluate over GF2x
 * @param x value to evaluate, GF2x element
 * @return evaluated polynomial, GF2x element
 */
/// Evaluate polynomial over GF2^m with x.
BPU_T_GF2_16x BPU_gf2xPolyEval(const BPU_T_GF2_16x_Poly *poly, const BPU_T_GF2_16x x, const BPU_T_Math_Ctx *math_ctx);

/**
 * Extended euclidian to find greatest common divisor and Bézout coefficients s, t, where gcd(a, b) = d = a*s + b*t.
 * You can specify degegree, when will it ends, deg(d) > end_deg. Polynomials d, s, t will be allocated inside function. After work should be freed using BPU_freePoly().
 * @param  d[out] greatest common divisor
 * @param  s[out] Bézout coefficients
 * @param  t[out] Bézout coefficients
 * @param  a[in] input polynomial
 * @param  b[in] input polynomial
 * @param  end_deg[in] it will ends, when deg(devider) <= end_deg
 * @param math_ctx[in] arithmetic data structure, initialized galois field
 * @return  0
 */
/// Extended Euclidean to find greatest common divisor and Bézout coefficients s, t, where gcd(a, b) = d = a*s + b*t.
int BPU_gf2xPolyExtEuclid(BPU_T_GF2_16x_Poly *d, BPU_T_GF2_16x_Poly *s, BPU_T_GF2_16x_Poly *t, const BPU_T_GF2_16x_Poly *a, const BPU_T_GF2_16x_Poly *b, int end_deg, const BPU_T_Math_Ctx *math_ctx);

/**
 * Function compares two polynomials
 * @param  p1 [description]
 * @param  p2 [description]
 * @return    0 if are same, -1 when deg differs, >0 index of first diff result is index + 1
 */
int BPU_gf2xPolyCmp(const BPU_T_GF2_16x_Poly *p1, const BPU_T_GF2_16x_Poly *p2);

/**
 * Function tests if polynomial is irreducible or not.
 * @param  p      [description]
 * @param  math_ctx [description]
 * @return        [description]
 */
int BPU_gf2xPolyIrredTest(const BPU_T_GF2_16x_Poly *p, const BPU_T_Math_Ctx *math_ctx);
/**
 * Get inverse polynomial over GF2_16x. It is using Extended Euclidean alg.
 * @param a[in]    input polynomial
 * @param mod[in]  modulus polynomial
 * @param out[out] output polynomial
 * @param math_ctx[in] arithmetic data structure
 */
/// Get inverse polynomial over GF2_16x.
void BPU_gf2xPolyInv(BPU_T_GF2_16x_Poly *out, const BPU_T_GF2_16x_Poly *a, const BPU_T_GF2_16x_Poly *mod, const BPU_T_Math_Ctx *math_ctx);

/**
 * Make from polynomial monic polynomial. It means that it will be multiplicated with (leading coef)^-1. It will change source polynomial.
 * @param  a      polynomial to be monic
 * @param  math_ctx arithmetic data structure
 * @return        inverse leading coefitient or 0
 */
/// Make from polynomial monic polynomial.
BPU_T_GF2_16x BPU_gf2xPolyMakeMonic(BPU_T_GF2_16x_Poly *a, const BPU_T_Math_Ctx *math_ctx);

/**
 * Insert coefitients from poly into matrix mat to i-th row
 * @param poly [description]
 * @param mat  [description]
 * @param i    [description]
 */
void BPU_gf2xMatInsertPoly(BPU_T_GF2_16x_Matrix *mat, const BPU_T_GF2_16x_Poly *poly, int i);

/**
 * save poly coefitients to vector, it allocates vec inside function. After use use BPU_freeVectorGF2_16x
 * @param poly [description]
 * @param vec  [description]
 */
void BPU_gf2xPolyToVec(BPU_T_GF2_16x_Vector *vec, const BPU_T_GF2_16x_Poly *poly, int len);

/**
 * GEM applied on matrix with coefitients from GF2^m
 * @param mat    [description]
 * @param math_ctx [description]
 */
void BPU_gf2xMatGEM(BPU_T_GF2_16x_Matrix *mat, const BPU_T_Math_Ctx *math_ctx);

/**
 * Swap
 * @param a [description]
 * @param b [description]
 */
void BPU_gf2xSwap(BPU_T_GF2_16x *a, BPU_T_GF2_16x *b);

/**
 * in matrix mat swap rows i and j
 * @param mat [description]
 * @param i   [description]
 * @param j   [description]
 */
void BPU_gf2xMatSwapRows(BPU_T_GF2_16x_Matrix *mat, int i, int j);

/**
 * Multiply whole row of matrix mat by element from finite field
 * @param mat     [description]
 * @param element [description]
 * @param i       [description]
 * @param math_ctx  [description]
 */
void BPU_gf2xMatMulElRow(BPU_T_GF2_16x_Matrix *mat, const BPU_T_GF2_16x element, int i, const BPU_T_Math_Ctx *math_ctx);

/**
 * find row where pivot is at index index
 * @param  mat   [description]
 * @param  index [description]
 * @return       [description]
 */
int BPU_gf2xMatFindPivot(const BPU_T_GF2_16x_Matrix *mat, int index);

/**
 * Get GF2_16x element which is a * return = res.
 * @param  a   input element
 * @param  res input element
 * @return     element a * return = res
 */
/// Get GF2_16x element which is a * return = res.
BPU_T_GF2_16x BPU_gf2xGetPseudoInv(const BPU_T_GF2_16x a, const BPU_T_GF2_16x res, const BPU_T_Math_Ctx *math_ctx);

/**
 * If at mat->elements[j][index] is not zero, then
 * mat->row[j] = mat->row[j] ^ (element * (mat->row[index]))
 * @param mat [description]
 * @param index   [description]
 * @param j   [description]
 */
void BPU_gf2xMatXorRows(BPU_T_GF2_16x_Matrix *mat, int index, int j, const BPU_T_Math_Ctx *math_ctx);

/**
 * vec = element*vec
 * @param vec     [description]
 * @param element [description]
 * @param math_ctx  [description]
 */
void BPU_gf2xVecMulEl(BPU_T_GF2_16x_Vector *vec, BPU_T_GF2_16x element, const BPU_T_Math_Ctx *math_ctx);

void BPU_gf2xVecToPoly(BPU_T_GF2_16x_Poly *poly, const BPU_T_GF2_16x_Vector *vec);

void BPU_gf2xMatClearCol(BPU_T_GF2_16x_Matrix *mat, int index, const BPU_T_Math_Ctx *math_ctx);

/**
 * Generate monic polynomial of degree t and with non-zero coefficient a_0
 * @param p      [description]
 * @param t      [description]
 * @param math_ctx [description]
 */
void BPU_gf2xPolyGenRandom(BPU_T_GF2_16x_Poly *p, int t, const BPU_T_Math_Ctx *math_ctx);

/**
 * Generate monic irreducible polynopmial of degree t.
 * @param p      [description]
 * @param t      [description]
 * @param math_ctx [description]
 */
void BPU_gf2xPolyGenGoppa(BPU_T_GF2_16x_Poly *p, int t, const BPU_T_Math_Ctx *math_ctx);

#endif // BPU_GF2X_H
