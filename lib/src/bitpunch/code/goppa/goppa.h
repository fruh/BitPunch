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

int BPU_goppaEncode(BPU_T_GF2_Vector *out, const BPU_T_GF2_Vector *in, const struct _BPU_T_Code_Ctx *ctx);

/**
 * Decode message. Decoded must be allocated before use, or use function decodeA
 * @param encoded[in] encoded message
 * @param decoded[out] decoded message, allocated inside
 * @param ctx[out] McEliece context
 *
 * @return still 0
 */
int BPU_goppaDecode(BPU_T_GF2_Vector *out, const BPU_T_GF2_Vector *in, const struct _BPU_T_Code_Ctx *ctx);

/**
 * Get error vector. Decoded must be allocated before use, or use function decodeA
 * @param encoded[in] encoded message
 * @param decoded[out] decoded message, allocated inside
 * @param ctx[out] McEliece context
 *
 * @return still 0
 */
int BPU_goppaGetError(BPU_T_GF2_Vector *error, const BPU_T_GF2_Vector *encoded, const BPU_T_Code_Ctx *ctx);

void BPU_goppaDetSyndromeA(BPU_T_GF2_16x_Poly *syndrome, const BPU_T_GF2_Vector *z, const BPU_T_GF2_16x_Matrix *H);

/**
 * Find polynomials a, b of degree <= (t div 2). We are using GCD to find them, gcd(tau, mod) = a, so: a = tau * b + mod * (some s***s). Out a and b are allocated inside, after use must be freed using BPU_freePoly().
 * @param tau[in]    [description]
 * @param mod[in]    [description]
 * @param a[out]      polynomial is allocated inside, must be then freed using BPU_freePoly()
 * @param b[out]      polynomial is allocated inside, must be then freed using BPU_freePoly()
 * @param math_ctx[in] [description]
 */
/// Find polynomials a, b of degree <= (t div 2)
void BPU_goppaFindPolyAB(BPU_T_GF2_16x_Poly *a, BPU_T_GF2_16x_Poly *b, const BPU_T_GF2_16x_Poly *tau, const BPU_T_GF2_16x_Poly *mod, const BPU_T_Math_Ctx *math_ctx);

/**
* Initialize all control matrix H. It uses McEl structure.
* @param[out] m poiter to Matrix_H structure
* @param[in] deg degree of goppa polynomial, number of errors
* @param[in] poly goppa polynomial over GF2x
* @param[in] math_ctx pointer to Aritmetic_Data structure, used by calculations
* @return 0 - on succes, else error
*/
/// Initialize control matrix H
int BPU_goppaInitMatH(BPU_T_GF2_16x_Matrix *m, BPU_T_GF2_16x_Poly *poly, BPU_T_Math_Ctx *math_ctx);

/**
* Initialize X matrix of t x t dimensions.
* @param[out] m pointer to matrix which should be initialized
* @param[in] t degree of goppa polynomial, number of errors
* @param[in] poly goppa polynomial over GF2x
*/
/// Initialize X matrix of t x t dimensions.
int BPU_goppaInitMatX(BPU_T_GF2_16x_Matrix *m, BPU_T_GF2_16x_Poly *poly);

/**
* Initialize Y matrix of t x n dimensions, n = ord of galois field.
* @param[out] m pointer to matrix which should be initialized
* @param[in] t degree of goppa polynomial, number of errors
* @param[in] math_ctx pointer to Aritmetic_Data structure, used by calculations
*/
/// Initialize Y matrix of t x n dimensions, where t = deg, n = ord of galois field.
int BPU_goppaInitMatY(BPU_T_GF2_16x_Matrix *m, uint8_t t, BPU_T_Math_Ctx *math_ctx);

/**
* Initialize Z matrix of n x n dimensions, where n = ord of galois field.
* @param[out] m pointer to Matrix_H structure
* @param[in] deg degree of goppa polynomial, number of errors
* @param[in] poly goppa polynomial over GF2x
* @param[in] math_ctx pointer to Aritmetic_Data structure, used by calculations
*/
/// Initialize Z matrix of n x n dimensions, where n is odr of galois field.
int BPU_goppaInitMatZ(BPU_T_GF2_16x_Matrix *m, BPU_T_GF2_16x_Poly *poly, BPU_T_Math_Ctx *math_ctx);

int BPU_goppaGenCode(BPU_T_Code_Ctx *ctx);

int BPU_goppaInitMatH2(BPU_T_GF2_Matrix *m, BPU_T_GF2_16x_Poly *poly, BPU_T_Math_Ctx *math_ctx);

int BPU_goppaEncodeM(BPU_T_GF2_Vector *out, const BPU_T_GF2_Vector *in, const struct _BPU_T_Code_Ctx *ctx);

void BPU_goppaDetSyndromeM(BPU_T_GF2_16x_Poly *syndrome, const BPU_T_GF2_Vector *z, const BPU_T_GF2_16x_Poly *poly, const BPU_T_Math_Ctx *math_ctx);
#endif // BPU_GOPPA_H
