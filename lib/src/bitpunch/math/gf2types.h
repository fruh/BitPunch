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
#ifndef BPUT_GF2TYPES_H
#define BPUT_GF2TYPES_H

#include <stdint.h>
#include "unitypes.h"

/**
* Binary representation GF2
*/
typedef BPU_T_Element BPU_T_GF2;

/**
 * Row vector GF2 representation.
 * Every element is one bit.
 */
typedef BPU_T_Element_Array BPU_T_GF2_Vector;

/**
* Matrix representation over GF2.
* Every element is one bit.
*/
typedef struct _BPU_T_GF2_Matrix {
	BPU_T_GF2 **elements; ///< all element of matrix
	uint8_t element_bit_size; ///< element size, is sizeof(BPU_T_GF2) i.e. 64 bits
	uint16_t elements_in_row; ///< number of elements in one row
	uint32_t k; ///< rows
	uint32_t n; ///< cols
}BPU_T_GF2_Matrix;

/**
 * Polynomial over GF2, represented as Vector.
 * Indexes are stored in elements, degree of polynomial is len - 1
 */
typedef BPU_T_GF2_Vector BPU_T_GF2_Poly;

/**
 * Sparse polynomial over GF2. 
 * More effective than BPU_T_GF2_Poly for low weight polynomials.
 * Every set coefficient is stored in array of indexes.
 */
typedef struct _BPU_T_GF2_Sparse_Poly {
  uint32_t *index; ///<indexes of set coefficients. 0 is coefficient for x^0
  uint32_t weight; ///<weight of polynomial
}BPU_T_GF2_Sparse_Poly;

/**
 * Quasi-cyclic matrix over GF2.
 * Cyclic matrices are stored as polynomials.
 * Matrix can have multiple cyclic matrices (but just one in row and more in columns, or in reverse)
 * The orientation of is set in isVertical.
 * For appended identity matrix in left set is_I_appended to 1.
 */
typedef struct _BPU_T_GF2_QC_Matrix {
  BPU_T_GF2_Poly *matrices; ///< all cyclic matrices of matrix
  uint16_t element_count; ///< number of cyclic matrices
  uint32_t element_size; ///< size of cyclic matrix
  uint8_t isVertical; ///< if 1, elements are in vertical orientation, if 0 horizontal orientation
  uint32_t n; ///< cols of whole matrix
  uint32_t k; ///< rows of whole matrix
  uint8_t is_I_appended; ///< if matrix has identity matrix on left
}BPU_T_GF2_QC_Matrix;

/**
 * Quasi-cyclic matrix over GF2 with sparse cyclic matrices.
 * More effective than BPU_T_QC_Matrix_GF2 for low weight cyclic matrices.
 * Matrix can have multiple cyclic matrices (but just one in row and more in columns, or in reverse)
 * The orientation of is set in isVertical.
 */
typedef struct _BPU_T_GF2_Sparse_Qc_Matrix {
  BPU_T_GF2_Sparse_Poly *matrices; ///< all cyclic matrices of matrix
  uint16_t element_count; ///< number of cyclic matrices
  uint32_t element_size; ///< size of cyclic matrix
  uint8_t isVertical; ///< if 1, elements are in vertical orientation, if 0 horizontal orientation
  uint32_t n; ///< cols of whole matrix
  uint32_t k; ///< rows of whole matrix
}BPU_T_GF2_Sparse_Qc_Matrix;

#endif // BPUT_GF2TYPES_H
