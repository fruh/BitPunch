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
#include "bigint.h"
#include <bitpunch/debugio.h>

int BPU_bigintMultiply(BPU_T_Bigint *out, BPU_T_Bigint *a, BPU_T_Bigint *b) {
    uint32_t tmp, carry;
    uint32_t i, j;

    if ((a->len + b->len) > out->len) {
        BPU_printDebug("Resizing big int output to %d", a->len + b->len);

        BPU_bigintResize(out, a->len + b->len);
    }
    else {
        BPU_bigintNull(out);
    }

    for(i = 0; i < a->array_length; i++) {
        carry = 0;
        for(j = 0; j < b->array_length; j++) {
            tmp = (a->elements[j] * b->elements[i]) + out->elements[i + j] + carry;
            carry = tmp >> a->element_bit_size;
            out->elements[i + j] = tmp;
        }
    }
    return 0;
}
