/*
This file is part of BitPunch
Copyright (C) 2013-2016 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>
Copyright (C) 2013-2016 Marek Klein  <kleinmrk[what here]gmail.com>

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
#include "unitypes.h"
#include <bitpunch/debugio.h>

void BPU_elementArrayFree(BPU_T_Element_Array **a) {
    if (!*a) {
        return;
    }
    free((*a)->elements);
    free(*a);
    *a = NULL;
}

int BPU_elementArrayMalloc(BPU_T_Element_Array **a, int len) {
    *a = (BPU_T_Element_Array *) calloc(sizeof(BPU_T_Element_Array), 1);

    if (!*a) {
        BPU_printError("allocation error");
        return -1;
    }
    return BPU_elementArrayMallocElements(*a, len);
}

int BPU_elementArrayResize(BPU_T_Element_Array *a, int len) {
    if (a->elements) {
        free(a->elements);
    }
    return BPU_elementArrayMallocElements(a, len);
}

int BPU_elementArrayMallocElements(BPU_T_Element_Array *a, int len) {
    // element size in bits
    a->element_bit_size = sizeof(BPU_T_Element) * 8;

    // len
    a->len = len;

    // calc how many elements of set size will be in one row
    int modul = 0;

    if (len % a->element_bit_size > 0) {
        modul = 1;
    }
    a->array_length = len / a->element_bit_size + modul;

    // allocate elemtens
    a->elements = (BPU_T_Element*) calloc(1, sizeof(BPU_T_Element) * a->array_length);

    if (!a->elements) {
        BPU_printError("can not allocate memory for vector of len %d", len);

        return -1;
    }
    return 0;
}
