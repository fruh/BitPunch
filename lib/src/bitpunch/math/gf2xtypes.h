/**
This file is part of PROGRAM
Copyright (C) 2014 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>

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
#ifndef BPUT_GF2XTYPES_H
#define BPUT_GF2XTYPES_H

#include <stdint.h>

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
 * GF2_16x Vector representation
 */
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
 * Set Polynomial values to 0.
 * @param d_pointer[out] pointer to GF2_16x polynomial
 */
 /// Copy Polynomial.
#define BPU_gf2xPolyNull(d_pointer) memset((void *) ((d_pointer)->coef), 0, sizeof(BPU_T_GF2_16x)*((d_pointer)->max_deg + 1));\
  (d_pointer)->deg = -1

void BPU_gf2xMatNull(BPU_T_Matrix_GF2_16x *mat);

/**
 * Allocate memory for matrix. After work you have to free memory using call BPU_freeMat.
 * @param[out] m matrix to be allocated
 * @param[in] rows rows
 * @param[in] cols cols
 * @return on succes 0, else error
 */
 /// Allocate memory for matrix.
int BPU_gf2xMatMalloc(BPU_T_Matrix_GF2_16x *m, int rows, int cols);

void BPU_gf2xVecMalloc(BPU_T_Vector_GF2_16x *vec, int size);

void BPU_gf2xVecFree(BPU_T_Vector_GF2_16x *vec, int is_dyn);

/**
 * Free dynamically or statically allocated matrix.
 * @param[out] *m address of matrix object
 * @param[in] is_dyn boolean param, if 0 do not free mat object self (is statically allocated), just allocated matrix rows, cols, else free also m object
 */
/// Free dynamically or statically allocated matrix
void BPU_gf2xMatFree(BPU_T_Matrix_GF2_16x *m, int is_dyn);

/**
 * Malloc memory for polynomial and zero-initialize
 * @param  p representation of polynomial (every element is one coeficient)
 * @param  max_deg max degree of polynomial
 * @return
 */
int BPU_polyMalloc(BPU_T_Poly_GF2_16x *p, int16_t max_deg);

/**
 * dealloc memory
 * @param p
 * @param is_dyn
 */
void BPU_polyFree(BPU_T_Poly_GF2_16x *p, int is_dyn);

#endif // BPUT_GF2XTYPES_H
