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
#include "gf2types.h"
#include <bitpunch/debugio.h>

#include <stdlib.h>
#include <string.h>


void BPU_freeMatGF2(BPU_T_Matrix_GF2 *m, int is_dyn) {
	int i;
	
	// first free cols
	for (i = 0; i < m->k; i++) {
		free(m->elements[i]);
	}
	// then free rows
	free(m->elements);
	
	if (is_dyn) {
		free(m);
	}
}

void BPU_freeVecGF2(BPU_T_Vector_GF2 *v, int is_dyn) {  
	// then free rows
	free(v->elements);
	
	if (is_dyn) {
		free(v);
	}
}

int BPU_mallocMatrixGF2(BPU_T_Matrix_GF2 *m, int rows, int cols) {
	int i;

	// element size
	m->element_bit_size = sizeof(BPU_T_GF2)*8;

	// rows
	m->k = rows;
	// cols
	m->n = cols;
	
	// calc how many elements of set size will be in one row
	int modul = 0;
	if ( cols % m->element_bit_size > 0) {
		modul = 1;
	}
	m->elements_in_row = cols/m->element_bit_size + modul;

	// allocate rows
	m->elements = (BPU_T_GF2**) malloc(sizeof(BPU_T_GF2*) * m->k);

	if (!m->elements) {
		BPU_printError("BPU_mallocMatrixGF2: can not allocate memory for matrix rows");
		
		return 1;
	}

	// allocate cols
	for (i = 0; i < m->k; i++) {
		m->elements[i] = (BPU_T_GF2*) malloc(sizeof(BPU_T_GF2) * m->elements_in_row);
	}

	// fill matrix with zeros
	for (i = 0; i < m->k; i++) {
		BPU_gf2MatNullRow(m, i);
	}
	return 0;
}

int BPU_mallocVectorGF2(BPU_T_Vector_GF2 *v, int len) {
	// element size in bits
	v->element_bit_size = sizeof(BPU_T_GF2) * 8;

	// len
	v->len = len;
	
	// calc how many elements of set size will be in one row
	int modul = 0;

	if ( len % v->element_bit_size > 0) {
		modul = 1;
	}
	v->elements_in_row = len / v->element_bit_size + modul;

	// allocate elemtens
	v->elements = (BPU_T_GF2*) malloc(sizeof(BPU_T_GF2) * v->elements_in_row);

#ifdef BPU_DEBUG_MALLOC_VECTOR_GF2
	BPU_printDebug("vector len: %d, element_bit_size: %d, elements_in_row: %d", v->len, v->element_bit_size, v->elements_in_row);
#endif
	if (!v->elements) {
		BPU_printError("BPU_mallocVectorGF2: can not allocate memory for vector of len %d", len);
		
		return 1;
	}
	// null vector
	BPU_gf2VecNull(v);

	return 0;
}