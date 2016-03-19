/*
This file is part of BitPunch
Copyright (C) 2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>

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
#ifndef GOPPATYPES_H
#define GOPPATYPES_H
#include <bitpunch/math/gf2.h>
#include <bitpunch/math/gf2x.h>
#include <bitpunch/math/perm.h>

/**
  Internal Goppa code structure.
  */
typedef struct _BPU_T_Goppa_Spec {
    BPU_T_GF2_Matrix *g_mat;    ///< Generator matrix
    BPU_T_GF2_16x_Matrix *h_mat;        ///< Control matrix H
    BPU_T_GF2_16x_Poly *g;      ///< gen. polynomial
    BPU_T_Perm_Vector *permutation;     ///< permutation "matrix"
    uint16_t support_len;       ///< number of elements in support
} BPU_T_Goppa_Spec;

/**
  Goppa code params.
  */
typedef struct _BPU_T_Goppa_Params {
    uint16_t m;                 ///< degree of mod polynomial
    uint16_t t;                 ///< error capability of code
    BPU_T_GF2_16x mod;          ///< Galois field polynomial
} BPU_T_Goppa_Params;

/**
 * @brief BPU_goppaFreeSpec Free Goppa code internal structure.
 * @param spec
 */
void BPU_goppaFreeSpec(BPU_T_Goppa_Spec * spec);

/**
 * @brief BPU_goppaInitParams Initi Goppa code params.
 * @param params
 * @param m
 * @param t
 * @param mod
 * @return
 */
int BPU_goppaInitParams(BPU_T_Goppa_Params ** params, const uint16_t m,
                        const uint16_t t, const BPU_T_GF2_16x mod);

/**
 * @brief BPU_goppaFreeParams Free goppa code params.
 * @param params
 */
void BPU_goppaFreeParams(BPU_T_Goppa_Params ** params);

#endif // GOPPATYPES_H
