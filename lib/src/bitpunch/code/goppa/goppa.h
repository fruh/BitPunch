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

#ifndef BPU_GOPPA_H
#define BPU_GOPPA_H

#include <bitpunch/config.h>

#include <bitpunch/code/codectx.h>
#include <bitpunch/math/gf2.h>
#include <bitpunch/math/gf2x.h>

#ifdef BPU_CONF_ENCRYPTION
/**
 * @brief BPU_goppaEncode Encode message using goppa code.
 * @param out
 * @param in
 * @param ctx
 * @return
 */
int BPU_goppaEncode(BPU_T_GF2_Vector * out, const BPU_T_GF2_Vector * in,
                    const struct _BPU_T_Code_Ctx *ctx);
#endif

#ifdef BPU_CONF_DECRYPTION
/**
 * Decode message using Patterson alg.
 * @param encoded[in] encoded message
 * @param decoded[out] decoded message, allocated inside
 * @param ctx[out] McEliece context
 *
 * @return still 0
 */
int BPU_goppaDecode(BPU_T_GF2_Vector * out, const BPU_T_GF2_Vector * in,
                    const struct _BPU_T_Code_Ctx *ctx);

/**
 * Get error vector Patterson alg.
 * @param encoded[in] encoded message
 * @param decoded[out] decoded message, allocated inside
 * @param ctx[out] McEliece context
 *
 * @return still 0
 */
int BPU_goppaGetError(BPU_T_GF2_Vector * error,
                      const BPU_T_GF2_Vector * encoded,
                      const BPU_T_Code_Ctx * ctx);

/**
 * Find polynomials a, b of degree <= (t div 2). We are using GCD to find them, gcd(tau, mod) = a, so: a = tau * b + mod * (some s***s). Out a and b are allocated inside, after use must be freed using BPU_freePoly().
 * @param tau[in]    [description]
 * @param mod[in]    [description]
 * @param a[out]      polynomial is allocated inside, must be then freed using BPU_freePoly()
 * @param b[out]      polynomial is allocated inside, must be then freed using BPU_freePoly()
 * @param math_ctx[in] [description]
 */
/// Find polynomials a, b of degree <= (t div 2)
void BPU_goppaFindPolyAB(BPU_T_GF2_16x_Poly * a, BPU_T_GF2_16x_Poly * b,
                         const BPU_T_GF2_16x_Poly * tau,
                         const BPU_T_GF2_16x_Poly * mod,
                         const BPU_T_Math_Ctx * math_ctx);

/**
 * @brief BPU_goppaDetSyndrome
 * @param syndrome
 * @param z
 * @param ctx
 */
void BPU_goppaDetSyndrome(BPU_T_GF2_16x_Poly * syndrome,
                          const BPU_T_GF2_Vector * z,
                          const BPU_T_Code_Ctx * ctx);
#endif

#ifdef BPU_CONF_KEY_GEN
/**
 * @brief BPU_goppaGenCode Generate random code based on params.
 * @param ctx
 * @return
 */
int BPU_goppaGenCode(BPU_T_Code_Ctx * ctx);

/**
 * @brief BPU_goppaInitMatH2 Initialize control matrix.
 * @param h2
 * @param hx
 * @param ctx
 * @return
 */
int BPU_goppaInitMatH2(BPU_T_GF2_Matrix * h2, BPU_T_GF2_16x_Matrix * hx,
                       const BPU_T_Code_Ctx * ctx);
#endif

#endif // BPU_GOPPA_H
