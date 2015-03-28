/**
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
#include "gf2.h"
#include "perm.h"

#include <stdlib.h>
#include <bitpunch/debugio.h>
#include <bitpunch/prng/prng.h>

/* ==================================== Print functions ==================================== */
void BPU_printBinaryMsb(uint32_t in, int len) {
	if (len > 0) {
		BPU_printBinaryMsb(in >> 1, len - 1);

		fprintf(stderr, "%d", (int) (in & (0x1u)));
	}
}

void BPU_printBinaryMsbLn(uint32_t in, int len) {
	BPU_printBinaryMsb(in, len);
	fprintf(stderr, "\n");
}

void BPU_printBinaryMsb32(uint32_t in) {
	BPU_printBinaryMsb(in, 32);
}

void BPU_printBinaryMsb32Ln(uint32_t in) {
	BPU_printBinaryMsbLn(in, 32);
}

void BPU_printBinaryLsb(uint32_t in, int len) {
	if (len > 0) {
		fprintf(stderr, "%d", (int) (in & (0x1u)));

		BPU_printBinaryLsb(in >> 1, len - 1);
	}
}

void BPU_printBinaryLsbLn(uint32_t in, int len) {
	BPU_printBinaryLsb(in, len);
	fprintf(stderr, "\n");
}

void BPU_printBinaryLsb32(uint32_t in) {
	BPU_printBinaryLsb(in, 32);
}

void BPU_printBinary32LsbLn(uint32_t in) {
	BPU_printBinaryLsbLn(in, 32);
}

void BPU_printGf2Mat(const BPU_T_GF2_Matrix* m) {
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
			BPU_printBinaryLsb(m->elements[i][j], bits_to_print);
			// fprintf(stderr, " "); // medzera medzi elementami
		}
		fprintf(stderr, "\n");
	}
}

void BPU_printGf2Vec(const BPU_T_GF2_Vector* v) {
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
		BPU_printBinaryLsb(v->elements[j], bits_to_print);
		fprintf(stderr, " ");
	}
	fprintf(stderr, "\n");
}

void BPU_printGf2VecMsb(const BPU_T_GF2_Vector* v) {
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
		BPU_printBinaryMsbLn(v->elements[j], bits_to_print);
		fprintf(stderr, " ");
	}
	fprintf(stderr, "\n");
}

void BPU_printGf2VecOnes(const BPU_T_GF2_Vector *vec) {
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

int BPU_gf2VecRand(BPU_T_GF2_Vector *out, int w) {
	int i, j;

	if (w > out->len) {
		BPU_printError("weight error w > l");
		return -2;
	}
	//vector of random weight
	if(w == 0) {
		for(i = 0; i < out->len; i++) {
			BPU_gf2VecSetBit(out, i, BPU_prngGetRand(0, 2));
		}
	}
	//vector of required weight
	else {
		BPU_gf2VecNull(out);

		for(i = 0; i < w; i++) {
			j = BPU_prngGetRand(0, out->len);

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

int BPU_gf2MatCopy(BPU_T_GF2_Matrix *out, const BPU_T_GF2_Matrix *in) {
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

int BPU_gf2VecPermute(BPU_T_GF2_Vector *vec, const BPU_T_Perm_Vector *permutation) {
	int i;
	BPU_T_GF2_Vector tmp;

	BPU_gf2VecMalloc(&tmp, vec->len);

	for (i = 0; i < permutation->size; i++) {
		BPU_gf2VecSetBit(&tmp, i, BPU_gf2VecGetBit(vec, permutation->elements[i]));
	}
	BPU_gf2VecCopy(vec, &tmp);

	BPU_gf2VecFree(&tmp, 0);

	return 0;
}

BPU_T_GF2 BPU_gf2MatGetMaskedBit(const BPU_T_GF2_Matrix *m, uint32_t row, uint32_t bit) {
	int segment, bit_in_segment;

	segment = bit / (m->element_bit_size);
	bit_in_segment = bit % (m->element_bit_size);
	// TODO: consider repleacing di literal 1u
	return m->elements[row][segment] & ((uint32_t)1 << bit_in_segment);
}

BPU_T_GF2 BPU_gf2VecGetMaskedBit(const BPU_T_GF2_Vector *vec, uint32_t bit) {
	int segment, bit_in_segment;

	segment = bit / (vec->element_bit_size);
	bit_in_segment = bit % (vec->element_bit_size);
	// TODO: consider repleacing di literal 1u
	return vec->elements[segment] & ((uint32_t)1 << bit_in_segment);
}

int BPU_gf2MatTransp(BPU_T_GF2_Matrix *out, const BPU_T_GF2_Matrix *in) {
	int i, j;

	if (out->k != in->n || out->n != in->k) {
		BPU_printError("Wrong matrix dimenzion");

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

void BPU_gf2MatSwapRows(BPU_T_GF2_Matrix *mat, int i, int j) {
	int k;
	for (k = 0; k < mat->elements_in_row; k++) {
		BPU_gf2Swap(&(mat->elements[i][k]), &(mat->elements[j][k]));
	}
}



int BPU_gf2MatFindRow(const BPU_T_GF2_Matrix *mat, int i, int start_index) {
	int k;
	for (k = start_index; k < mat->k; k++) {
		if ( BPU_gf2MatGetMaskedBit(mat, k, i) ){
			return k;
		}
	}
	return -1;
}

int BPU_gf2MatFindCol(const BPU_T_GF2_Matrix *mat, int i, int start_index) {
	int k;
	for (k = start_index; k < mat->n; k++) {
		if ( BPU_gf2MatGetMaskedBit(mat, i, k) ){
			return k;
		}
	}
	return -1;
}

int  BPU_gf2MatMakeSystematic(BPU_T_GF2_Matrix *inout) {
	int act_position = 0;
	int i;
	int row;
	
	for (act_position = 0; act_position < inout->k; act_position++) {
		row = BPU_gf2MatFindRow(inout, act_position, act_position);
		if (row == -1){
			BPU_printWarning("Not systematic");
			return -1;
		}
		BPU_gf2MatSwapRows(inout, act_position, row);

		// xor with the rest of rows if needed
		for (i = 0; i < inout->k; i++) {
			if ( BPU_gf2MatGetMaskedBit(inout, i, act_position) && act_position != i) {
				BPU_gf2MatXorRows(inout, i, act_position);
			}
		}
	}
	return 0;
}

int BPU_gf2VecConcat(BPU_T_GF2_Vector *out, const BPU_T_GF2_Vector *vec1, const BPU_T_GF2_Vector *vec2) {
	int len = vec1->len + vec2->len;
	int i;

	if (out->len != len) {
		BPU_gf2VecFree(out, 0);
		BPU_gf2VecMalloc(out, vec1->len + vec2->len);
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

int BPU_gf2VecCrop(BPU_T_GF2_Vector *out, const BPU_T_GF2_Vector *in, const int start, const int length) {
	int i;
	int counter = 0;

	//input test
	if (start < 0 || (length+start) > in->len) {
		BPU_printError("cropVector: bad params");
		return -1;
	}
	if (out->len < length) {
		BPU_printError("cropVector: out vector is smaller then needed");
		return -1;
	}
	for (i = start; i < start+length; i++) {
		BPU_gf2VecSetBit(out, counter, BPU_gf2VecGetBit(in, i));
		counter++;
	}
	return 0;
}

int BPU_gf2MatCrop(BPU_T_GF2_Matrix *out, const BPU_T_GF2_Matrix *in, uint16_t width) {
	int i, j, startElement;
	uint8_t cropBits;

	if (out->k != in->k || out->n != width) {
		BPU_printError("wrong matrix dimenzion.");
		return -3;
	}
	// if is new width lower than old width of matrix
	if (in->n >= width) {
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

int BPU_gf2MatGetRowAsGf2Vec(BPU_T_GF2_Vector *out, const BPU_T_GF2_Matrix *in, int row, int alloc) {
	if (!alloc && out->len != in->n) {
		BPU_printError("dimension is wrong out->len %d != in->n %d", out->len, in->n);

		return -1;
	}
	if (alloc) {
		if (BPU_gf2VecMalloc(out, in->n)) {
			BPU_printError("can not allocate vector");

			return -2;
		}
	}
	BPU_gf2MatCopyRowToVec(out, in, row);

	return 0;
}

void BPU_gf2VecCopy(BPU_T_GF2_Vector *dest, const BPU_T_GF2_Vector *src) {
	// if there is not enough space resize
	if (dest->elements_in_row < src->elements_in_row) {
		BPU_gf2VecFree(dest, 0);
		BPU_gf2VecMalloc(dest, src->elements_in_row * src->element_bit_size * sizeof(BPU_T_GF2));
	}
	else {
		BPU_gf2VecNull(dest);
	}
	memcpy((void *) (dest->elements), (void *) (src->elements), sizeof(BPU_T_GF2) * (src->elements_in_row));
	
	dest->len = src->len;
}



int BPU_gf2VecCmp(const BPU_T_GF2_Vector *v1, const BPU_T_GF2_Vector *v2) {
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

int BPU_gf2VecXor(BPU_T_GF2_Vector *out, const BPU_T_GF2_Vector *in) {
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

int BPU_gf2VecMulMat(BPU_T_GF2_Vector *out, const BPU_T_GF2_Vector *v, const BPU_T_GF2_Matrix *b) {
	int i, j;

	if ((v->len != b->k) || (out->len != b->n)) {
		BPU_printError("wrong vector and matrix dimension v->len = %d, b->k = %d", v->len, b->k);
		BPU_printError("wrong vector and matrix dimension out->len = %d, b->n = %d", out->len, b->n);

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

void BPU_gf2MatXorRows(BPU_T_GF2_Matrix *mat, int i, int j) {
	int k;
	
	for (k = 0; k < mat->elements_in_row; k++) {
		mat->elements[i][k] ^= mat->elements[j][k];
	}
}

int BPU_gf2MatPermute(BPU_T_GF2_Matrix *inout, BPU_T_Perm_Vector *permutation) {
	int i, j;
	int bit = 0;
	BPU_T_GF2_Vector *vector;
	int length;
	vector = (BPU_T_GF2_Vector *) malloc(sizeof(BPU_T_GF2_Vector));
	if (inout->n != permutation->size) {
		BPU_printError("permutation size not correct m->n = %d, p->size = %d", inout->n, permutation->size);

		return -1;
	}
	length = inout->elements_in_row * (inout->element_bit_size / 8);

	BPU_gf2VecMalloc(vector, permutation->size);

	for (i = 0; i < inout->k; i++) {
		memcpy((vector->elements), inout->elements[i], length);
		for (j = 0; j < permutation->size; j++) {
			bit = BPU_gf2VecGetBit(vector, permutation->elements[j]);
			BPU_gf2MatSetBit(inout, i, j, bit);
		}
	}
	BPU_gf2VecFree(vector, 1);
	return 0;
}

int BPU_gf2MatCropMemory(BPU_T_GF2_Matrix *m, uint16_t width) {
	BPU_T_GF2_Vector *row, *cropped_row;
	int length, i, new_length;

	length = m->elements_in_row * (m->element_bit_size / 8);
	row = (BPU_T_GF2_Vector*) malloc(sizeof(BPU_T_GF2_Vector));
	cropped_row = (BPU_T_GF2_Vector*) malloc(sizeof(BPU_T_GF2_Vector));
	BPU_gf2VecMalloc(row, m->n);
	BPU_gf2VecMalloc(cropped_row, width);
	new_length = cropped_row->elements_in_row * (m->element_bit_size / 8);
	for (i = 0; i < m->k; i++) {
		memcpy(row->elements, m->elements[i], length);
		BPU_gf2VecCrop(cropped_row, row, m->k, width);
		free(m->elements[i]);
		m->elements[i] = (BPU_T_GF2*) malloc(new_length);
		memcpy(m->elements[i], cropped_row->elements, new_length);
	}
	m->n = cropped_row->len;
	m->elements_in_row = cropped_row->elements_in_row;

	BPU_gf2VecFree(row, 1);
	BPU_gf2VecFree(cropped_row, 1);

	return 0;
}

uint8_t BPU_getParity(BPU_T_GF2 dword) {
	uint32_t tmp = dword;
	tmp = (tmp >> 16) ^ tmp;
	tmp = (tmp >> 8 ) ^ tmp;
	tmp = (tmp >> 4 ) ^ tmp;
	tmp = (tmp >> 2 ) ^ tmp;
	tmp = (tmp >> 1 ) ^ tmp;
	return tmp & 1;
}
