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
#include "perm.h"
#include "int.h"

#include <stdlib.h>
#include <stdio.h>

#include <bitpunch/prng/prng.h>
#include <bitpunch/debugio.h>

#ifdef BPU_CONF_PRINT
void BPU_printPerm(const BPU_T_Perm_Vector * permutation) {
    int i;

    fprintf(stderr, "Perm (%4d): ", permutation->size);
    for (i = 0; i < permutation->size; i++) {
        fprintf(stderr, "%i ", permutation->elements[i]);
    }
    fprintf(stderr, "\n");
}
#endif // BPU_CONF_PRINT

void BPU_permFree(BPU_T_Perm_Vector ** p) {
    if (!*p) {
        return;
    }
    free((*p)->elements);
    free(*p);
}

int BPU_permMalloc(BPU_T_Perm_Vector ** p, int size) {
    // allocate memory
    int i;

    *p = (BPU_T_Perm_Vector *) calloc(sizeof(BPU_T_Perm_Vector), 1);

    if (!*p) {
        BPU_printError("allocation error");
        return -1;
    }

    (*p)->size = size;
    (*p)->elements =
        (BPU_T_Perm_Element *) malloc(sizeof(BPU_T_Perm_Element) * size);

    if (!(*p)->elements) {
        BPU_printError("BPU_mallocPerm: can not allocate permutation vector");

        return -1;
    }
    for (i = 0; i < (*p)->size; i++) {
        (*p)->elements[i] = i;
    }
    return 0;
}

int BPU_permRandomize(BPU_T_Perm_Vector * permutation) {
    int i;
    uint32_t rand_value;

    for (i = 0; i < permutation->size; i++) {
        permutation->elements[i] = i;
    }
    for (i = 0; i < permutation->size; i++) {
        rand_value = BPU_prngGetRand(i, permutation->size);
        BPU_permSwap(&permutation->elements[i],
                     &permutation->elements[rand_value]);
    }
    return 0;
}

void BPU_permSwap(BPU_T_Perm_Element * a, BPU_T_Perm_Element * b) {
    BPU_T_Perm_Element tmp;

    tmp = *a;
    *a = *b;
    *b = tmp;
}

int BPU_permGetInv(BPU_T_Perm_Vector * out, const BPU_T_Perm_Vector * in) {
    int i;

    if (out->size != in->size) {
        BPU_printError("permutation size error");

        return -1;
    }
    for (i = 0; i < in->size; i++) {
        out->elements[in->elements[i]] = i;
    }
    return 0;
}


int BPU_permPermute(BPU_T_Perm_Vector * to_permute,
                    const BPU_T_Perm_Vector * permutation) {
    int i;
    BPU_T_Perm_Vector *new_permutation;

    // check if the size is correct
    if (to_permute->size != permutation->size) {
        return -1;
    }
    // allocate new permutation
    if (BPU_permMalloc(&new_permutation, to_permute->size) != 0) {
        return -2;
    }
    // copy the permutation
    for (i = 0; i < to_permute->size; i++) {
        new_permutation->elements[i] = to_permute->elements[i];
    }
    // permute
    for (i = 0; i < permutation->size; i++) {   // row loop
        to_permute->elements[i] =
            new_permutation->elements[permutation->elements[i]];
    }
    BPU_permFree(&new_permutation);

    return 0;
}

int BPU_permIsValid(const BPU_T_Perm_Vector * p) {
    int i, j;

    for (i = 0; i < p->size; i++) {
        for (j = 0; j < p->size; j++) {
            if (i != j && p->elements[i] == p->elements[j]) {
                BPU_printError("permutation is not valid");
#ifdef BPU_CONF_PRINT
                BPU_printPerm(p);
#endif
                return 1;
            }
        }
    }
    return 0;
}
