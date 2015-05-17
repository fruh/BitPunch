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


/**********************************************************
gf2 POLY
**********************************************************/

/**
 * Allocate memory for polynomial over GF2. Allocate memory, so after work it has to be freed by using call BPU_gf2PolyFree. 
 * @param p polynomial to allocate
 * @param len length of polynomial (length - 1 = degree of polynomial)
 * @return on succes 0, else error
 */
int BPU_gf2PolyMalloc(BPU_T_GF2_Poly *p, int len);

/**
 * Free dynamically or statically allocated polynomial over GF2.
 * @param p polynomial to free
 * @param is_dyn boolean param, if 0 do not free object self, else free also object
 */
void BPU_gf2PolyFree(BPU_T_GF2_Poly *p, int is_dyn);


/**********************************************************
gf2 SPARSE POLY
**********************************************************/

/**
 * Allocate memory for sparse polynomial over GF2. Allocate memory, so after work it has to be freed by using call BPU_gf2SparsePolyFree. 
 * @param p sparse polynomial to allocate
 * @param weight weight of sparse polynomial
 */
void BPU_gf2SparsePolyMalloc(BPU_T_GF2_Sparse_Poly *p, int weight);

/**
 * Free dynamically or statically allocated sparse polynomial over GF2.
 * @param p sparse polynomial to free
 * @param is_dyn boolean param, if 0 do not free object self, else free also object
 */
void BPU_gf2SparsePolyFree(BPU_T_GF2_Sparse_Poly *p, int is_dyn);


/**********************************************************
gf2 QUASI-CYCLIC MATRIX
**********************************************************/

/**
 * Allocate memory for quasi-cyclic matrix over GF2. Allocate memory, so after work it has to be freed by using call BPU_gf2QcMatrixFree. 
 * @param v quasi-cyclic matrix
 * @param element_count count of cyclic elements
 * @param element_size size of one cyclic element
 * @param isVertical boolean, if 1, elements are in vertical orientation, if 0 horizontal orientation
 * @param is_I_appended boolean, if 1, identity matrix is appended to the left, if 0, no identity matrix
 * @return 0 - succes, else error
 */
int BPU_gf2QcMatrixMalloc(BPU_T_GF2_QC_Matrix *v, int element_count, int element_size, int isVertical, int is_I_appended);

/**
 * Free dynamically or statically allocated quasi-cyclic matrix over GF2.
 * @param v quasi-cyclic matrix
 * @param is_dyn boolean param, if 0 do not free object self, else free also object
 */
void BPU_gf2QcMatrixFree(BPU_T_GF2_QC_Matrix *v, int is_dyn);


/**********************************************************
gf2 SPARSE QUASI-CYCLIC MATRIX
**********************************************************/

/**
 * Allocate memory for sparse quasi-cyclic matrix over GF2. Allocate memory, so after work it has to be freed by using call BPU_gf2SparseQcMatrixFree. 
 * @param v sparse quasi-cyclic matrix
 * @param element_count count of cyclic elements
 * @param element_size size of one cyclic element
 * @param isVertical boolean, if 1, elements are in vertical orientation, if 0 horizontal orientation
 */
void BPU_gf2SparseQcMatrixMalloc(BPU_T_GF2_Sparse_Qc_Matrix *v, int element_count, int element_size, int isVertical);


/**
 * Free dynamically or statically allocated sparse quasi-cyclic matrix over GF2.
 * @param v sparse quasi-cyclic matrix
 * @param is_dyn boolean param, if 0 do not free object self, else free also object
 */
void BPU_gf2SparseQcMatrixFree(BPU_T_GF2_Sparse_Qc_Matrix *v, int is_dyn);

#endif // BPUT_GF2TYPES_H
