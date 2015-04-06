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
#include "gf2types.h"
#include <bitpunch/debugio.h>

#include <stdlib.h>
#include <string.h>


void BPU_gf2MatFree(BPU_T_GF2_Matrix **m) {
	int i;
	
    if (!*m) {
        return;
    }
	// first free cols
    for (i = 0; i < (*m)->k; i++) {
        free((*m)->elements[i]);
	}
	// then free rows
    free((*m)->elements);
    free((*m));
    *m = NULL;
}

void BPU_gf2VecFree(BPU_T_GF2_Vector **v) {
    if (!*v) {
        return;
    }
    free((*v)->elements);
    free(*v);
    *v = NULL;
}

int BPU_gf2MatMalloc(BPU_T_GF2_Matrix **m, int rows, int cols) {
	int i;

    *m = (BPU_T_GF2_Matrix *) calloc(sizeof(BPU_T_GF2_Matrix), 1);

    if (!*m) {
        BPU_printError("allocation error");
        return -1;
    }
	// element size
    (*m)->element_bit_size = sizeof(BPU_T_GF2)*8;

	// rows
    (*m)->k = rows;
	// cols
    (*m)->n = cols;
	
	// calc how many elements of set size will be in one row
	int modul = 0;
    if ( cols % (*m)->element_bit_size > 0) {
		modul = 1;
	}
    (*m)->elements_in_row = cols/(*m)->element_bit_size + modul;

	// allocate rows
    (*m)->elements = (BPU_T_GF2**) malloc(sizeof(BPU_T_GF2*) * (*m)->k);

    if (!(*m)->elements) {
        BPU_printError("can not allocate memory for matrix rows");
        return -1;
	}
	// allocate cols
    for (i = 0; i < (*m)->k; i++) {
        (*m)->elements[i] = (BPU_T_GF2*) calloc(1, sizeof(BPU_T_GF2) * (*m)->elements_in_row);

        if (!(*m)->elements[i]) {
            BPU_printError("can not allocate memory for matrix cols");
            return -2;
        }
	}
	return 0;
}

int BPU_gf2VecMalloc(BPU_T_GF2_Vector **v, int len) {
    *v = (BPU_T_GF2_Vector *) calloc(sizeof(BPU_T_GF2_Vector), 1);

    if (!*v) {
        BPU_printError("allocation error");
        return -1;
    }
    return BPU_gf2VecMallocElements(*v, len);
}

int BPU_gf2VecResize(BPU_T_GF2_Vector *v, int len) {
    if (v->elements) {
        free(v->elements);
    }
    return BPU_gf2VecMallocElements(v, len);
}

int BPU_gf2VecMallocElements(BPU_T_GF2_Vector *v, int len) {
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
    v->elements = (BPU_T_GF2*) calloc(1, sizeof(BPU_T_GF2) * v->elements_in_row);

    if (!v->elements) {
        BPU_printError("can not allocate memory for vector of len %d", len);

        return -1;
    }
    return 0;
}
