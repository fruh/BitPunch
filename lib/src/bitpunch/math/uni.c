/*
This file is part of BitPunch
Copyright (C) 2016 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>
Copyright (C) 2016 Marek Klein  <kleinmrk[what here]gmail.com>

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
#include <bitpunch/bitpunch.h>
#include <bitpunch/math/uni.h>

#include <bitpunch/debugio.h>

#ifdef BPU_CONF_PRINT
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

void BPU_printElementArray(const BPU_T_Element_Array * a) {
    uint32_t j, bits_to_print;

    fprintf(stderr, "Vec (%4d): ", a->len);
    for (j = 0; j < a->array_length; j++) {
        if (j == a->array_length - 1) {
            if (a->len % (a->element_bit_size) != 0) {
                bits_to_print = a->len % a->element_bit_size;
            }
            else {
                bits_to_print = a->element_bit_size;
            }
        }
        else {
            bits_to_print = a->element_bit_size;
        }
        BPU_printBinaryLsb(a->elements[j], bits_to_print);
        fprintf(stderr, " ");
    }
    fprintf(stderr, "\n");
}

void BPU_printElementArrayMsb(const BPU_T_Element_Array * a) {
    uint32_t j, bits_to_print;

    fprintf(stderr, "Vec (%4d): ", a->len);
    for (j = 0; j < a->array_length; j++) {
        if (j == a->array_length - 1) {
            if (a->len % (a->element_bit_size) != 0) {
                bits_to_print = a->len % a->element_bit_size;
            }
            else {
                bits_to_print = a->element_bit_size;
            }
        }
        else {
            bits_to_print = a->element_bit_size;
        }
        BPU_printBinaryMsbLn(a->elements[j], bits_to_print);
        fprintf(stderr, " ");
    }
    fprintf(stderr, "\n");
}

void BPU_printElementArrayOnes(const BPU_T_Element_Array * a) {
    uint32_t i;

    for (i = 0; i < a->len; ++i) {
        if (BPU_elementArrayGetBit(a, i)) {
            fprintf(stderr, "%d ", i);
        }
    }
    fprintf(stderr, "\n");
}

/* ------------------------------------ Print functions ------------------------------------ */
#endif // BPU_CONF_PRINT

void BPU_elementArrayFree(BPU_T_Element_Array * a) {
    if (NULL == a) {
        return;
    }

    BPU_SAFE_FREE(free, a->elements);
    free(a);
}

BPU_T_Element_Array *BPU_elementArrayMalloc(uint32_t len) {
    BPU_T_Element_Array *a = NULL;
    BPU_T_Element_Array *a_local = NULL;
    uint32_t modul = 0;
    BPU_T_Element *elements_local;

    a_local = (BPU_T_Element_Array *) calloc(sizeof(BPU_T_Element_Array), 1);
    if (NULL == a_local) {
        BPU_printError("calloc failed");
        goto err;
    }
    // element size in bits
    a_local->element_bit_size = sizeof(BPU_T_Element) * 8;

    // len
    a_local->len = len;

    // calc how many elements of a set size will be in one row
    if (len % a_local->element_bit_size > 0) {
        modul = 1;
    }

    a_local->array_length = (len / a_local->element_bit_size) + modul;

    // allocate elemtens
    elements_local =
        (BPU_T_Element *) calloc(a_local->array_length, sizeof(BPU_T_Element));
    if (NULL == elements_local) {
        BPU_printError("calloc failed");
        goto err;
    }

    a_local->elements = elements_local;
    a = a_local;
    a_local = NULL;
    elements_local = NULL;
  err:
    BPU_SAFE_FREE(free, elements_local);
    BPU_SAFE_FREE(free, a_local);
    return a;
}

// TODO: I would like to get rid of this function
int BPU_elementArrayResize(BPU_T_Element_Array * a, uint32_t len) {
    if (a->elements) {
        free(a->elements);
    }
    return BPU_elementArrayMallocElements(a, len);
}

// TODO: I would like to get rid of this function
int BPU_elementArrayMallocElements(BPU_T_Element_Array * a, uint32_t len) {
    // element size in bits
    a->element_bit_size = sizeof(BPU_T_Element) * 8;

    // len
    a->len = len;

    // calc how many elements of set size will be in one row
    uint32_t modul = 0;

    if (len % a->element_bit_size > 0) {
        modul = 1;
    }
    a->array_length = len / a->element_bit_size + modul;

    // allocate elemtens
    a->elements =
        (BPU_T_Element *) calloc(1, sizeof(BPU_T_Element) * a->array_length);

    if (!a->elements) {
        BPU_printError("can not allocate memory for vector of len %d", len);

        return -1;
    }
    return 0;
}
