/*
This file is part of BitPunch
Copyright (C) 2014-2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>

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
#ifndef BPU_MATHCTX_H
#define BPU_MATHCTX_H

#include "gf2xtypes.h"

// polynomials in format BPU_GF2_POLY_DEG_m where 'm' is mceliece param m
#define BPU_GF2_POLY_DEG_5 0x3b
#define BPU_GF2_POLY_DEG_6 0x43
#define BPU_GF2_POLY_DEG_10 0x71d
#define BPU_GF2_POLY_DEG_11 0x805

/**
* Representation of aritmetics data.
*/
typedef struct _BPU_T_Math_Ctx {
	BPU_T_GF2_16x *exp_table; ///< there are all elements referenced by i, so at i-th index is g^i element, g - generator
  	BPU_T_GF2_16x *log_table; ///< there are all indexes referenced by element, so alpha elemnet (g^i) -> i
  	BPU_T_GF2_16x mod; ///< polynomial modulus
    uint8_t mod_deg; ///< modulo degree, galois finite field GF(2^m)
  	int ord; ///< group ord, number of elements
}BPU_T_Math_Ctx;

/**
* Precalculate logaritmic and exponencial tables and initialize structure Aritmetic_Data
* @param g is a group generator
* @param mod modulus, ireducible polynomial
* @return number of elements or < 0 means error
*/
/// Precalculate logaritmic and exponencial tables and initialize structure Aritmetic_Data
int BPU_mathInitCtx(BPU_T_Math_Ctx **ctx, const BPU_T_GF2_16x g, const BPU_T_GF2_16x mod);

/**
 * Free dynamiccaly or statically allocated Aritmetic_Data structure.
 * @param a      aaddress of Aritmetic_Data structure
 * @param is_dyn 0 - staticaly allocated Aritmetic_Data object or 1 when dynamically
 */
/// Free dynamiccaly or statically allocated Aritmetic_Data structure.
void BPU_mathFreeCtx(BPU_T_Math_Ctx **ctx);

#endif // BPU_MATHCTX_H
