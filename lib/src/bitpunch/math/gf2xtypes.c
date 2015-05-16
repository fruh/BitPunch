/*
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
#include "gf2xtypes.h"
#include <bitpunch/debugio.h>

#include <stdlib.h>
#include <string.h>

int BPU_gf2xMatMalloc(BPU_T_GF2_16x_Matrix **m, int rows, int cols) {
	int i;
    *m = (BPU_T_GF2_16x_Matrix *) calloc(sizeof(BPU_T_GF2_16x_Matrix), 1);

    if (!*m) {
        BPU_printError("allocation error");
        return -1;
    }
	// rows
    (*m)->k = rows;
	// cols
    (*m)->n = cols;
	// allocate rows
    (*m)->elements = (BPU_T_GF2_16x **) malloc(sizeof(BPU_T_GF2_16x*) * (*m)->k);

    if (!(*m)->elements) {
		BPU_printError("BPU_mallocMatrix: can not allocate memory for matrix rows");

		return 1;
	}
	// allocate cols
    for (i = 0; i < (*m)->k; i++) {
        (*m)->elements[i] = (BPU_T_GF2_16x*) calloc(1, sizeof(BPU_T_GF2_16x) * (*m)->n);

        if (!(*m)->elements[i]) {
            BPU_printError("allocation error");
            return -2;
        }
	}
	return 0;
}

int BPU_gf2xVecMalloc(BPU_T_GF2_16x_Vector **vec, int size) {
    *vec = (BPU_T_GF2_16x_Vector *) calloc(sizeof(BPU_T_GF2_16x_Vector), 1);

    if (!*vec) {
        BPU_printError("allocation error");
        return -1;
    }
    (*vec)->len = size;
    (*vec)->elements = (BPU_T_GF2_16x*)calloc(size, sizeof(BPU_T_GF2_16x));

    if (!(*vec)->elements) {
        BPU_printError("allocation error");
        return -2;
    }
    return 0;
}

void BPU_gf2xVecFree(BPU_T_GF2_16x_Vector **vec) {
    if (!*vec) {
        return;
    }
    free((*vec)->elements);
    free(*vec);
    *vec = NULL;
}

void BPU_gf2xMatFree(BPU_T_GF2_16x_Matrix **m) {
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

void BPU_gf2xPolyFree(BPU_T_GF2_16x_Poly **p) {
    if (!*p) {
        return;
    }
    free((*p)->coef);
    free(*p);
    *p = NULL;
}

int BPU_gf2xPolyMalloc(BPU_T_GF2_16x_Poly **p, int16_t max_deg) {
    *p = (BPU_T_GF2_16x_Poly *) calloc(sizeof(BPU_T_GF2_16x_Poly), 1);

    if (!*p) {
        BPU_printError("allocation error");
        return -1;
    }
    return BPU_gf2xPolyMallocCoef(*p, max_deg);
}

int BPU_gf2xPolyResize(BPU_T_GF2_16x_Poly *p, int16_t max_deg) {
    if (p->coef) {
        free(p->coef);
    }
    return BPU_gf2xPolyMallocCoef(p, max_deg);
}

int BPU_gf2xPolyMallocCoef(BPU_T_GF2_16x_Poly *p, int16_t max_deg) {
    // allocate memory
    p->deg = -1;
    p->max_deg = max_deg;

    if (p->max_deg < 0) {
        BPU_printError("BPU_mallocPoly: max_deg must be at least 0");

        return -1;
    }
    p->coef = (BPU_T_GF2_16x*) calloc(max_deg + 1, sizeof(BPU_T_GF2_16x));

    if (!p->coef) {
        BPU_printError("BPU_mallocPoly: can not allocate polynomial");

        return -1;
    }
    return 0;
}

void BPU_gf2xMatNull(BPU_T_GF2_16x_Matrix *mat) {
	int i, j;
	for (i = 0; i < mat->k; i++) {
		for (j = 0; j < mat->n; j++) {
			mat->elements[i][j] = 0;
		}
	}
}
