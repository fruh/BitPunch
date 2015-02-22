/**
This file is part of BitPunch
Copyright (C) 2013-2014 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>
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
#include "gf2.h"
#include "perm.h"

#include <stdlib.h>
#include <bitpunch/debugio.h>
#include <bitpunch/prng/prng.h>

/* ==================================== Print functions ==================================== */
void BPU_printBinary(uint64_t in, int len) {
	if (len > 0) {
		BPU_printBinary(in >> 1, len - 1);

		fprintf(stderr, "%d", (int) (in & (0x1ul)));
	}
}

void BPU_printBinaryLn(uint64_t in, int len) {
	BPU_printBinary(in, len);
	fprintf(stderr, "\n");
}

void BPU_printBinary64(uint64_t in) {
	BPU_printBinary(in, 64);
}

void BPU_printBinary64Ln(uint64_t in) {
	BPU_printBinaryLn(in, 64);
}

void BPU_printBinaryLe(uint64_t in, int len) {
	if (len > 0) {
		fprintf(stderr, "%d", (int) (in & (0x1ul)));

		BPU_printBinaryLe(in >> 1, len - 1);
	}
}

void BPU_printBinaryLnLe(uint64_t in, int len) {
	BPU_printBinaryLe(in, len);
	fprintf(stderr, "\n");
}

void BPU_printBinary64Le(uint64_t in) {
	BPU_printBinaryLe(in, 64);
}

void BPU_printBinary64LnLe(uint64_t in) {
	BPU_printBinaryLnLe(in, 64);
}

void BPU_printGf2Mat(const BPU_T_Matrix_GF2* m) {
	int i, j, bits_to_print;

	fprintf(stderr, "Matrix size: %dx%d\n", m->k, m->n);

	for (i = 0; i < m->k; i++) {
		fprintf(stderr, "%4d: ",i);

		for (j = 0; j <= m->elements_in_row - 1; j++) {
			if (j == m->elements_in_row-1) {
				if (m->n%(m->element_bit_size) != 0) {
					bits_to_print = m->n % m->element_bit_size;
				}
				else {
					bits_to_print = m->element_bit_size;
				}
			}
			else {
				bits_to_print = m->element_bit_size;
			}
			BPU_printBinaryLe(m->elements[i][j], bits_to_print);
			// fprintf(stderr, " "); // medzera medzi elementami
		}
		fprintf(stderr, "\n");
	}
}

void BPU_printGf2Vec(const BPU_T_Vector_GF2* v) {
	int j, bits_to_print;

	fprintf(stderr, "Vec (%4d): ", v->len);
	for (j = 0; j <= v->elements_in_row - 1; j++) {
		if (j == v->elements_in_row-1) {
			if (v->len % (v->element_bit_size) != 0) {
				bits_to_print = v->len % v->element_bit_size;
			}
			else {
				bits_to_print = v->element_bit_size;
			}
		}
		else {
			bits_to_print = v->element_bit_size;
		}
		BPU_printBinaryLe(v->elements[j], bits_to_print);
		fprintf(stderr, " "); // medzera medzi elementami
	}
	fprintf(stderr, "\n");
}

void BPU_printGf2VecBe(const BPU_T_Vector_GF2* v) {
	int j, bits_to_print;

	fprintf(stderr, "Vec (%4d): ", v->len);
	for (j = 0; j <= v->elements_in_row - 1; j++) {
		if (j == v->elements_in_row-1) {
			if (v->len % (v->element_bit_size) != 0) {
				bits_to_print = v->len % v->element_bit_size;
			}
			else {
				bits_to_print = v->element_bit_size;
			}
		}
		else {
			bits_to_print = v->element_bit_size;
		}
		BPU_printBinaryLn(v->elements[j], bits_to_print);
		fprintf(stderr, " "); // medzera medzi elementami
	}
	fprintf(stderr, "\n");
}

void BPU_printGf2VecOnes(const BPU_T_Vector_GF2 *vec) {
	int i;
	for (i = 0; i < vec->len; ++i)
	{
		if (BPU_gf2VecGetBit(vec, i)) {
			fprintf(stderr, "%d ", i);
		}
	}
	fprintf(stderr, "\n");
}
/* ------------------------------------ Print functions ------------------------------------ */

int BPU_initRandVector(BPU_T_Vector_GF2 *out, int l, int w) {
	int i, j;

	if (BPU_mallocVectorGF2(out, l)) {
		BPU_printError("initRandVector: allocation error");
		return -1;
	}

	if (w > l) {
		BPU_printError("initRandVector: weight error w > l");
		return -2;
	}

	//vector of random weight
	if(w == 0) {
		for(i = 0; i < l; i++) {
			BPU_gf2VecSetBit(out, i, BPU_prngGetRand(0, 2));
		}
	}
	//vector of required weight
	else {
		for(i = 0; i < w; i++) {
			j = BPU_prngGetRand(0, l);

			if(BPU_gf2VecGetBit(out, j) == 0) {
				BPU_gf2VecSetBit(out, j, 1);
			}
			else{
				i--;
			}
		}
	}
	return 0;
}

int BPU_gf2MatCopy(BPU_T_Matrix_GF2 *out, const BPU_T_Matrix_GF2 *in) {
	int i, j;

	if (out->k != in->k || out->n != in->n) {
		BPU_printError("BPU_gf2MatCopy: wrong matrix size");

		return -1;
	}

	// copy the matrix
	for (i = 0; i < in->k; i++) {
		for (j = 0; j < in->elements_in_row; j++) {
			out->elements[i][j] = in->elements[i][j];
		}
	}
	return 0;
}

int BPU_gf2MatPermute(BPU_T_Matrix_GF2 *m, const BPU_T_Perm_Vector *permutation) {
	int i, j, act_element, shift;
	BPU_T_Matrix_GF2 new_matrix;

	// check if the size is correct
	if (m->n != permutation->size)
		return -1;

	// allocate new matrix
	if (BPU_mallocMatrixGF2(&new_matrix, m->k ,m->n) != 0)
		return -2;

	// copy the matrix
	BPU_gf2MatCopy(&new_matrix, m);

	// erase the matrix
	for (i = 0; i < m->k; i++) {
		for (j = 0; j < m->elements_in_row; j++) {
			m->elements[i][j] = 0;
		}
	}

	// permute
	act_element = 0;
	for (j = 0; j < m->n; j++) { // column loop
		if (j >= m->element_bit_size + act_element * m->element_bit_size)
			act_element++;
		for (i = 0; i < m->k; i++) { // row loop
			if ((int)(permutation->elements[j]-j) >= 0) {
				shift = (permutation->elements[j]-j) % new_matrix.element_bit_size;
				m->elements[i][act_element] ^= BPU_gf2MatGetMaskedBit(&new_matrix, (uint64_t)i, (uint64_t)permutation->elements[j]) >> (shift); // permute the columns
			}
			else {
				shift = (j-permutation->elements[j]) % new_matrix.element_bit_size;
				m->elements[i][act_element] ^= BPU_gf2MatGetMaskedBit(&new_matrix, (uint64_t)i, (uint64_t)permutation->elements[j]) << (shift); // permute the columns
			}
		}
	}
	BPU_freeMatGF2(&new_matrix, 0);

	return 0;
}

int BPU_gf2VecPermute(BPU_T_Vector_GF2 *vec, const BPU_T_Perm_Vector *permutation) {
	int i;
	BPU_T_Vector_GF2 tmp;

	BPU_mallocVectorGF2(&tmp, vec->len);

	for (i = 0; i < permutation->size; i++) {
		BPU_gf2VecSetBit(&tmp, i, BPU_gf2VecGetBit(vec, permutation->elements[i]));
	}
	BPU_gf2VecCopy(vec, &tmp);

	BPU_freeVecGF2(&tmp, 0);

	return 0;
}

BPU_T_GF2 BPU_gf2MatGetMaskedBit(const BPU_T_Matrix_GF2 *m, uint32_t row, uint32_t bit) {
	int segment, bit_in_segment;

	segment = bit / (m->element_bit_size);
	bit_in_segment = bit % (m->element_bit_size);
	
	return m->elements[row][segment] & ((uint64_t)1 << bit_in_segment);
}

BPU_T_GF2 BPU_gf2VecGetMaskedBit(const BPU_T_Vector_GF2 *vec, uint32_t bit) {
	int segment, bit_in_segment;

	segment = bit / (vec->element_bit_size);
	bit_in_segment = bit % (vec->element_bit_size);
	
	return vec->elements[segment] & ((uint64_t)1 << bit_in_segment);
}

int BPU_gf2MatTranspA(BPU_T_Matrix_GF2 *out, const BPU_T_Matrix_GF2 *in) {
	int i, j;

	if (BPU_mallocMatrixGF2(out, in->n, in->k)) {
		BPU_printError("BPU_gf2MatTranspA allocation error");

		return -1;
	}
	for (i = 0; i < in->k; i++) {
		for (j = 0; j < in->n; j++) {
			BPU_gf2MatSetBit(out, j, i, BPU_gf2MatGetBit(in, i, j));
		} // col loop
	} // row loop

	return 0;
}

void BPU_gf2Swap(BPU_T_GF2 *a, BPU_T_GF2 *b) {
	BPU_T_GF2 tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}

void BPU_gf2MatSwapRows(BPU_T_Matrix_GF2 *mat, int i, int j) {
	int k;
	for (k = 0; k < mat->elements_in_row; k++) {
		BPU_gf2Swap(&(mat->elements[i][k]), &(mat->elements[j][k]));
	}
}



int BPU_gf2MatFindRow(const BPU_T_Matrix_GF2 *mat, int i, int start_index) {
	int k;
	for (k = start_index; k < mat->k; k++) {
		if ( BPU_gf2MatGetMaskedBit(mat, k, i) ){
			return k;
		}
	}
	return -1;
}

int BPU_gf2MatFindCol(const BPU_T_Matrix_GF2 *mat, int i, int start_index) {
	int k;
	for (k = start_index; k < mat->n; k++) {
		if ( BPU_gf2MatGetMaskedBit(mat, i, k) ){
			return k;
		}
	}
	return -1;
}

BPU_T_Perm_Vector* BPU_gf2MatMakeSystematicA(BPU_T_Matrix_GF2 *inout) {
	int act_position = 0;
	int i;
	int row, col;
	BPU_T_Perm_Vector perm;
	BPU_T_Perm_Vector *permOut;
	permOut = (BPU_T_Perm_Vector*) malloc (sizeof(BPU_T_Perm_Vector));

	BPU_mallocPerm(&perm, inout->n);
	BPU_mallocPerm(permOut, inout->n);

	for (i = 0; i < perm.size; i++) {
		perm.elements[i] = i;
		permOut->elements[i] = i;
	}
#ifdef BPU_TEST_DET_EQ_CODE
	__test_was_eq_code = 0;
#endif
	
	for (act_position = 0; act_position < inout->k; act_position++) {
		row = BPU_gf2MatFindRow(inout, act_position, act_position);
		if (row == -1){
			BPU_printWarning("Not systematic");
			// TODO: find bug in this if branch
			#ifdef BPU_TEST_DET_EQ_CODE
				__test_was_eq_code = 1;
			#endif
				BPU_freePerm(permOut, 1);
				BPU_freePerm(&perm, 0);

				return NULL;

			row = act_position;
			col = BPU_gf2MatFindCol(inout, act_position, act_position);
			if (col == -1) {
				BPU_printError("unable to create systematic form");
				
				exit(-1);
			}
			BPU_gf2Swap(&(perm.elements[act_position]), &(perm.elements[col])); 
			// printf("swap col: %d\t%d\n", act_position, col);
			BPU_permPermute(permOut, &perm);
			// BPU_printPerm(permOut);
			BPU_gf2MatPermute(inout, &perm);
			BPU_gf2Swap(&(perm.elements[act_position]), &(perm.elements[col])); // reset temporary permutation
		}
		BPU_gf2MatSwapRows(inout, act_position, row);

		// xor with the rest of rows if needed
		for (i = 0; i < inout->k; i++) {
			if ( BPU_gf2MatGetMaskedBit(inout, i, act_position) && act_position != i) {
				BPU_gf2MatXorRows(inout, i, act_position);
			}
		}
	}
	BPU_freePerm(&perm, 0);
	return permOut;
}
int BPU_gf2MatAppendIdenityA(BPU_T_Matrix_GF2 *out, const BPU_T_Matrix_GF2 *in) {
	int i, j;

	if (BPU_mallocMatrixGF2(out, in->k, in->n + in->k)) {
		BPU_printError("gf2MatAppendIdenityA: allocation error");

		return -1;
	}
	// first copy old rows
	for (i = 0; i < in->k; i++) {
		memcpy((void *) out->elements[i], (void *)in->elements[i], in->elements_in_row * (in->element_bit_size / 8));
	} // row loop
	// now append identity matrix
	for (i = 0; i < out->k; i++) {
			for (j = in->n; j < out->n; j++) {
				BPU_gf2MatSetBit(out, i, j, i == (j - in->n) ? 1 : 0);
		} // col loop
	} // row loop
	return 0;
}

int BPU_gf2VecConcat(BPU_T_Vector_GF2 *out, const BPU_T_Vector_GF2 *vec1, const BPU_T_Vector_GF2 *vec2) {
	int len = vec1->len + vec2->len;
	int i;

	if (out->len != len) {
		BPU_freeVecGF2(out, 0);
		BPU_mallocVectorGF2(out, vec1->len + vec2->len);
	}
	else {
		BPU_gf2VecNull(out);
	}
	// copy first vector
	BPU_gf2VecCopy(out, vec1);

	// copy second vector
	for (i = 0; i < vec2->len; i++) {
		BPU_gf2VecSetBit(out, i + vec1->len, BPU_gf2VecGetBit(vec2, i));
	}
	out->len = len;

	return 0;
}

int BPU_gf2VecConcatA(BPU_T_Vector_GF2 *out, const BPU_T_Vector_GF2 *vec1, const BPU_T_Vector_GF2 *vec2) {
	int len = vec1->len + vec2->len;

	if (BPU_mallocVectorGF2(out, len)) {
		BPU_printError("BPU_gf2VecConcat: allocation error");

		return -1;
	}
	return BPU_gf2VecConcat(out, vec1, vec2);
}

int BPU_gf2VecCropA(BPU_T_Vector_GF2 *out, const BPU_T_Vector_GF2 *in, const int start, const int length) {

	//input test
	if (start < 0 || (length+start) > in->len) {
		BPU_printError("cropVector: bad params");
		return -1;
	}

	//malloc output vector
	if (BPU_mallocVectorGF2(out, length)) {
		BPU_printError("cropVector: allocation error");
		return -1;
	}

	int i;

	//calc the shifts
	/*int crop_start = (start % in->element_bit_size);
	int crop_len = (length % in->element_bit_size);
	int shift1 = in->element_bit_size - crop_start - crop_len;
	int shift2 = in->element_bit_size - crop_len;
	int shift3 = in->element_bit_size - crop_start;
 
	//start elements   
	int start_element = start / in->element_bit_size;*/

	//element loop
	/*for(i = 0; i < out->elements_in_row; i++) {
		//crop from only one element
		if (((start / in->element_bit_size) == ((start + length) / in->element_bit_size)) || (i == out->elements_in_row-1 && crop_start == 0)) {
			printf("first way: \n");
			out->elements[i] = (in->elements[i+start_element] << shift1) >> shift2;
		}
		//concat of elements
		else { 
			printf("second way: \n");
			out->elements[i] = (in->elements[i-start_element] >> crop_start) ^ ((in->elements[i+start_element+1]) << shift1 >> (shift1 - shift3));
		}
	}*/
	int counter = 0;
	for (i = start; i < start+length; i++) {
		BPU_gf2VecSetBit(out, counter, BPU_gf2VecGetBit(in, i));
		counter++;
	}

	return 0;
}

int BPU_gf2MatCropA(BPU_T_Matrix_GF2 *out, const BPU_T_Matrix_GF2 *in, uint16_t width) {
	int i, j, startElement;
	uint8_t cropBits;

	// if is new width lower than old width of matrix
	if (in->n >= width) {
		// allocate memory for new matrix
		if (BPU_mallocMatrixGF2(out, in->k, width)!= 0) {
			return -2;
		}

		startElement = (in->n - width) / in->element_bit_size;
		cropBits = (in->n - width) % in->element_bit_size;

		// fill new matrix
		for (i = 0; i < out->k; i++) { // row loop
			for (j = 0; j < out->elements_in_row; j++) { // column loop
				
				// if there is no need to make shift through elements
				if (j+startElement >= in->elements_in_row-1)
					out->elements[i][j] = (in->elements[i][j+startElement] >> cropBits);
				// with shift through elements
				else 
					out->elements[i][j] = (in->elements[i][j+startElement] >> cropBits) ^ (in->elements[i][j+startElement+1] << (in->element_bit_size - cropBits));
			}
		}
		return 0;

	}
	// else error
	else {
		BPU_printError("BPU_gf2MatCropA in->n <= width, (%d <= %d)", in->n, width);

		return -1;
	}
}

int BPU_gf2MatGetRowAsGf2Vec(BPU_T_Vector_GF2 *out, const BPU_T_Matrix_GF2 *in, int row, int alloc) {
	if (!alloc && out->len != in->n) {
		BPU_printError("gf2MatGetRowAsGf2Vec: dimension is wrong out->len %d != in->n %d", out->len, in->n);

		return -1;
	}
	if (alloc) {
		if (BPU_mallocVectorGF2(out, in->n)) {
			BPU_printError("gf2MatGetRowAsGf2Vec: can not allocate vector");

			return -2;
		}
	}
	BPU_gf2MatCopyRowToVec(out, in, row);

	return 0;
}

void BPU_gf2VecCopy(BPU_T_Vector_GF2 *dest, const BPU_T_Vector_GF2 *src) {
	// if there is not enough space resize
	if (dest->elements_in_row < src->elements_in_row) {
		BPU_freeVecGF2(dest, 0);

		BPU_mallocVectorGF2(dest, src->elements_in_row * src->element_bit_size * sizeof(BPU_T_GF2));
	}
	else {
		BPU_gf2VecNull(dest);
	}
	memcpy((void *) (dest->elements), (void *) (src->elements), sizeof(BPU_T_GF2) * (src->elements_in_row));
	
	dest->len = src->len;
}



int BPU_gf2VecCmp(const BPU_T_Vector_GF2 *v1, const BPU_T_Vector_GF2 *v2) {
	int i;

	if (v1->len != v2->len) {
		return 1;
	}
	for (i = 0; i < v1->len; i++) {
		if (BPU_gf2VecGetBit(v1, i) != BPU_gf2VecGetBit(v2, i)) {
			return 2;
		}
	}
	return 0;
}

int BPU_gf2VecXor(BPU_T_Vector_GF2 *out, const BPU_T_Vector_GF2 *in) {
	int i;

	if (out->len != in->len) {
		BPU_printError("BPU_gf2VecXor: length error (el. in row) %d != %d, len %d != %d", out->elements_in_row, in->elements_in_row, out->len, in->len);

		return -1;
	}
	for (i = 0; i < out->elements_in_row; i++) {
		out->elements[i] ^= in->elements[i];
	}
	return 0;
}

int BPU_gf2VecMulMat(BPU_T_Vector_GF2 *out, const BPU_T_Vector_GF2 *v, const BPU_T_Matrix_GF2 *b) {
	int i, j;

	if ((v->len != b->k) || (out->len != b->n)) {
		BPU_printError("BPU_gf2VecMulMat: wrong vector and matrix dimension v->len = %d, b->k = %d", v->len, b->k);
		BPU_printError("BPU_gf2VecMulMat: wrong vector and matrix dimension out->len = %d, b->n = %d", out->len, b->n);

		return -1;
	}
	// null elements
	BPU_gf2VecNull(out);

	for (i = 0; i < v->len; i++) {
		if (BPU_gf2VecGetBit(v, i)) {
			// xor rows
			for (j = 0; j < out->elements_in_row; j++) {
				out->elements[j] ^= b->elements[i][j];
			}
		}
	}
	return 0;
}

void BPU_gf2MatXorRows(BPU_T_Matrix_GF2 *mat, int i, int j) {
	int k;
	
	for (k = 0; k < mat->elements_in_row; k++) {
		mat->elements[i][k] ^= mat->elements[j][k];
	}
}

// int BPU_gf2VecToPoly(BPU_T_Vector_GF2 *in, BPU_T_Poly_GF2_16x *out, uint8_t ord) {
//   if (BPU_mallocPoly(out, in->len / ord + 1) != 0) {
//     BPU_printError("gf2VecToPoly: BPU_mallocPoly");
//     return -1;
//   }

//   int i;
//   int actual_coef = -1;

//   for (i = 0; i<in->len; i++) {
//     //move to next coef
//     if (i % ord == 0) {
//       actual_coef++;
//       //fprintf(stderr, "next coef\n");
//     }

//     out->coef[actual_coef] ^= BPU_gf2VecGetBit(in,i) << (i%ord);
//     //fprintf(stderr, "i:%d  adding:%d coef", i,BPU_gf2VecGetBit(in,i));
//     //BPU_printBinaryLe(out->coef[actual_coef], ord);
//     //fprintf(stderr, "\n");
//   }

//   out->deg = BPU_gf2xPolyGetDeg(out);

//   return 0;
// }
