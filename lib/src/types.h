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
* Here are defined basic types used by our McEliece implementation.
*/
#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

/**
* Binary representation GF2
*/
typedef uint32_t BPU_T_GF2;

/**
* Polynomial representation over GF2, max deg f < 8
*/
typedef uint8_t BPU_T_GF2_8x;

/**
* Polynomial representation over GF2, max deg f < 16
*/
typedef uint16_t BPU_T_GF2_16x;

/**
* Polynomial representation over GF2, max deg f < 32
*/
typedef uint32_t BPU_T_GF2_32x;

/**
* Polynomial representation over GF2, max deg f < 64
*/
typedef uint64_t BPU_T_GF2_64x;

/**
* Representation of permutation.
* Elements are numbers from 0 to size-1.
* Size corresponds to count of columns in matrix.
*/
typedef struct _BPU_T_Perm_Vector {
  uint32_t *elements; ///< permutation vector
  uint32_t size; ///< permutation size
}BPU_T_Perm_Vector;


/**
* Matrix representation over GF2.
* Every element is one bit.
*/
typedef struct _BPU_T_Matrix_GF2 {
  BPU_T_GF2 **elements; ///< all element of matrix
  uint8_t element_bit_size; ///< element size, is sizeof(BPU_T_GF2) i.e. 64 bits
  uint8_t elements_in_row; ///< number of elements in one row
  uint32_t k; ///< rows
  uint32_t n; ///< cols
}BPU_T_Matrix_GF2;

/**
 * Row vector GF2 representation.
 * Every element is one bit.
 */
typedef struct _BPU_T_Vector_GF2 {
  BPU_T_GF2 *elements; ///< all element of matrix
  uint8_t element_bit_size; ///< element size, is sizeof(BPU_T_GF2) i.e. 64 bits
  uint8_t elements_in_row; ///< number of elements in one row
  uint32_t len; ///< cols
}BPU_T_Vector_GF2;

typedef struct _BPU_T_Vector_GF2_16x {
  BPU_T_GF2_16x *elements;
  uint8_t len; ///< number of elements
}BPU_T_Vector_GF2_16x;

/**
* Matrix representation over GF2_8x
*/
typedef struct _BPU_T_Matrix_GF2_8x {
  BPU_T_GF2_8x **elements; ///< elements of matrix
  uint8_t k; ///< rows
  uint8_t n; ///< cols
}BPU_T_Matrix_GF2_8x;

/**
* Matrix representation over GF2_16x.
*/
typedef struct _BPU_T_Matrix_GF2_16x {
  BPU_T_GF2_16x **elements; ///< all element of matrix
  uint16_t k; ///< rows
  uint16_t n; ///< cols
}BPU_T_Matrix_GF2_16x;

/**
* Representation of polynomial.
*/
typedef struct _BPU_T_Poly_GF2_16x{
  BPU_T_GF2_16x *coef; ///< Polynomial over GF2m
  int16_t deg; ///< degree
  int16_t max_deg; ///< degree
}BPU_T_Poly_GF2_16x;

/**
* McEliece public key representation.
*/
typedef struct _BPU_T_McEliece_Pub_Key{
  BPU_T_Matrix_GF2 g_mat; ///< public key matrix
  uint8_t t; ///< number of errors
  uint8_t m; ///< galois finite field GF(2^m)
}BPU_T_McEliece_Pub_Key;

/**
* McEliece private key representation.
*/
typedef struct _BPU_T_McEliece_Priv_Key{
  BPU_T_Perm_Vector permutation; ///< permutation "matrix"
  BPU_T_Poly_GF2_16x g;
  BPU_T_Matrix_GF2_16x h_mat; ///< Control matrix H
}BPU_T_McEliece_Priv_Key;

/**
* Representation of aritmetics data.
*/
typedef struct BPU_T_Arithmetic_Data {
  BPU_T_GF2_16x *exp_table; ///< there are all elements referenced by i, so at i-th index is g^i element, g - generator
  uint32_t *log_table; ///< there are all indexes referenced by element, so alpha elemnet (g^i) -> i
  BPU_T_GF2_16x mod; ///< polynomial modulus
  int mod_deg; ///< modulo degree, galois finite field GF(2^m)
  int ord; ///< group ord, number of elements
}BPU_T_Arithmetic_Data;

/**
* Representation of McEliece context. All necessary structures, data should be here obtained.
*/
typedef struct _BPU_T_McEliece_Ctx {
  BPU_T_Arithmetic_Data a_data; ///< aritmetic date
  BPU_T_McEliece_Pub_Key pub_key; ///< public key
  BPU_T_McEliece_Priv_Key priv_key; ///< private key
  uint16_t max_pt_len_bit; ///< max plaintext len in bits
  uint16_t max_ct_len_bit; ///< max ciphertext len in bits
}BPU_T_McEliece_Ctx;

#endif // TYPES_H
