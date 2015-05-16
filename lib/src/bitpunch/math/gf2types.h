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

/**
* Binary representation GF2
*/
typedef uint32_t BPU_T_GF2;

/**
 * Row vector GF2 representation.
 * Every element is one bit.
 */
typedef struct _BPU_T_GF2_Vector {
	BPU_T_GF2 *elements; ///< all element of matrix
	uint8_t element_bit_size; ///< element size, is sizeof(BPU_T_GF2) i.e. 64 bits
	uint16_t elements_in_row; ///< number of elements in one row
	uint32_t len; ///< cols
}BPU_T_GF2_Vector;

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
 * Null GF2 matrix row.
 * @param[out]  m_pointer pointer to GF2 matrix
 * @param[in]  row       row to null
 */
/// Null GF2 matrix row.
#define BPU_gf2MatNullRow(m_pointer, row) memset((void *) ((m_pointer)->elements[row]), 0, (m_pointer)->element_bit_size / 8 * (m_pointer)->elements_in_row)

/**
 * Null GF2 vector.
 * @param[out]  v_pointer pointer to GF2 vector
 */
/// Null GF2 vector.
#define BPU_gf2VecNull(v_pointer) memset((void *) ((v_pointer)->elements), 0, (v_pointer)->element_bit_size / 8 * (v_pointer)->elements_in_row)

/**
 * Free dynamically or statically allocated matrix GF2. 
 * @param[out] *m address of matrix object
 */
/// Free dynamically or statically allocated matrix
void BPU_gf2MatFree(BPU_T_GF2_Matrix **m);

/**
 * Free dynamically or statically allocated vector GF2. 
 * @param[out] *m address of vector object
 */
/// Free dynamically or statically allocated vector
void BPU_gf2VecFree(BPU_T_GF2_Vector **v);

/**
 * Allocate memory for matrix GF2. It also nulls new matrix. After work you have to free memory using call BPU_freeMatGF2
 * @param rows rows
 * @param cols cols
 * @return on succes 0, else error
 */
int BPU_gf2MatMalloc(BPU_T_GF2_Matrix **m, int rows, int cols);

/**
 * Allocate memory for vector GF2. It also null vector. After work you have to free memory using call BPU_freeVecGF2.
 * @param len len of vector
 * @return on succes 0, else error
 */
int BPU_gf2VecMalloc(BPU_T_GF2_Vector **v, int len);

/**
 * @brief BPU_gf2VecResize Resize vecor.
 * @param v
 * @param len
 * @return
 */
int BPU_gf2VecResize(BPU_T_GF2_Vector *v, int len);

/**
 * @brief BPU_gf2VecMallocElements Malloc vector elements and set to zero.
 * @param v
 * @param len
 * @return
 */
int BPU_gf2VecMallocElements(BPU_T_GF2_Vector *v, int len);

#endif // BPUT_GF2TYPES_H
