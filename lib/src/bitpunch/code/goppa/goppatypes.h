/**
This file is part of PROGRAM
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
#include <bitpunch/math/gf2types.h>
#include <bitpunch/math/gf2xtypes.h>
#include <bitpunch/math/permtypes.h>

typedef struct _BPU_T_Goppa_Spec {
	BPU_T_Matrix_GF2 *g_mat; ///< Generator matrix
	BPU_T_Matrix_GF2_16x *h_mat; ///< Control matrix H
	BPU_T_Poly_GF2_16x *g; ///< gen. polynomial
	BPU_T_Perm_Vector *permutation; ///< permutation "matrix"
}BPU_T_Goppa_Spec;

void BPU_goppaFreeSpec(BPU_T_Goppa_Spec *spec);

#endif // GOPPATYPES_H
