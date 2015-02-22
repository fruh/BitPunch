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

#ifndef BPU_GOPPA_H
#define BPU_GOPPA_H

#include <bitpunch/code/codectx.h>
#include <bitpunch/math/gf2.h>
#include <bitpunch/math/gf2x.h>

int BPU_goppaEncode(BPU_T_Vector_GF2 *out, const BPU_T_Vector_GF2 *in, const struct _BPU_T_Code_Ctx *ctx);

/**
 * Decode message. Decoded must be allocated before use, or use function decodeA
 * @param encoded[in] encoded message
 * @param decoded[out] decoded message, allocated inside
 * @param ctx[out] McEliece context
 *
 * @return still 0
 */
int BPU_goppaDecode(BPU_T_Vector_GF2 *out, const BPU_T_Vector_GF2 *in, const struct _BPU_T_Code_Ctx *ctx);

/**
 * Decode message. Decoded is allocated inside function, use freeVectorGF2() afer use.
 * @param encoded[in] encoded message
 * @param decoded[out] decoded message, allocated inside
 * @param ctx[out] McEliece context
 *
 * @return succes - 0, else non-zero
 */
int BPU_goppaDecodeA(BPU_T_Vector_GF2 *out, const BPU_T_Vector_GF2 *in, const struct _BPU_T_Code_Ctx *ctx);

/**
 * Get error vector. Decoded must be allocated before use, or use function decodeA
 * @param encoded[in] encoded message
 * @param decoded[out] decoded message, allocated inside
 * @param ctx[out] McEliece context
 *
 * @return still 0
 */
int BPU_goppaGetError(BPU_T_Vector_GF2 *encoded, BPU_T_Vector_GF2 *decoded, const BPU_T_Code_Ctx *ctx);

/**
 * Get error vector. Decoded is allocated inside function, use freeVectorGF2() afer use.
 * @param encoded[in] encoded message
 * @param decoded[out] decoded message, allocated inside
 * @param ctx[out] McEliece context
 *
 * @return succes - 0, else non-zero
 */
int BPU_goppaGetErrorA(BPU_T_Vector_GF2 *encoded, BPU_T_Vector_GF2 *decoded, const BPU_T_Code_Ctx *ctx);

void BPU_gf2xMatDetermineSyndromeA(BPU_T_Poly_GF2_16x *syndrome, const BPU_T_Vector_GF2 *z, const BPU_T_Matrix_GF2_16x *H, const BPU_T_Math_Ctx *math_ctx);

/**
 * Find polynomials a, b of degree <= (t div 2). We are using GCD to find them, gcd(tau, mod) = a, so: a = tau * b + mod * (some s***s). Out a and b are allocated inside, after use must be freed using BPU_freePoly().
 * @param tau[in]    [description]
 * @param mod[in]    [description]
 * @param a[out]      polynomial is allocated inside, must be then freed using BPU_freePoly()
 * @param b[out]      polynomial is allocated inside, must be then freed using BPU_freePoly()
 * @param math_ctx[in] [description]
 */
/// Find polynomials a, b of degree <= (t div 2)
void BPU_findPolynomialsAB(const BPU_T_Poly_GF2_16x *tau, const BPU_T_Poly_GF2_16x *mod, BPU_T_Poly_GF2_16x *a, BPU_T_Poly_GF2_16x *b, const BPU_T_Math_Ctx *math_ctx);

/**
* Initialize all control H matricies (X, Y, Z). It uses McEl structure.
* @param[out] m poiter to Matrix_H structure
* @param[in] deg degree of goppa polynomial, number of errors
* @param[in] poly goppa polynomial over GF2x
* @param[in] math_ctx pointer to Aritmetic_Data structure, used by calculations
* @return 0 - on succes, else error
*/
/// Initialize all control H matricies (X, Y, Z)
int BPU_goppaInitMatH(BPU_T_Matrix_GF2_16x *m, BPU_T_Poly_GF2_16x *poly, BPU_T_Math_Ctx *math_ctx);

/**
* Initialize X matrix of t x t dimensions.
* @param[out] m pointer to matrix which should be initialized
* @param[in] t degree of goppa polynomial, number of errors
* @param[in] poly goppa polynomial over GF2x
*/
/// Initialize X matrix of t x t dimensions.
int BPU_goppaInitMatX(BPU_T_Matrix_GF2_16x *m, BPU_T_Poly_GF2_16x *poly);

/**
* Initialize Y matrix of t x n dimensions, n = ord of galois field.
* @param[out] m pointer to matrix which should be initialized
* @param[in] t degree of goppa polynomial, number of errors
* @param[in] math_ctx pointer to Aritmetic_Data structure, used by calculations
*/
/// Initialize Y matrix of t x n dimensions, where t = deg, n = ord of galois field.
int BPU_goppaInitMatY(BPU_T_Matrix_GF2_16x *m, uint8_t t, BPU_T_Math_Ctx *math_ctx);

/**
* Initialize Z matrix of n x n dimensions, where n = ord of galois field.
* @param[out] m pointer to Matrix_H structure
* @param[in] deg degree of goppa polynomial, number of errors
* @param[in] poly goppa polynomial over GF2x
* @param[in] math_ctx pointer to Aritmetic_Data structure, used by calculations
*/
/// Initialize Z matrix of n x n dimensions, where n is odr of galois field.
int BPU_goppaInitMatZ(BPU_T_Matrix_GF2_16x *m, BPU_T_Poly_GF2_16x *poly, BPU_T_Math_Ctx *math_ctx);

int BPU_goppaGenCode(BPU_T_Code_Ctx *ctx);

#endif // BPU_GOPPA_H
