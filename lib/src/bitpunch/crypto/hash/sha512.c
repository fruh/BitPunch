/*
This file is part of BitPunch
Copyright (C) 2015 Frantisek Uhrecky <frantisek.uhrecky[what here]gmail.com>

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
#include "sha512.h"

#ifdef BPU_CONF_SHA_512
#include "polarssl/polarssl/sha512.h"

#include <bitpunch/debugio.h>
#include <bitpunch/math/gf2.h>

int BPU_gf2VecHash(BPU_T_GF2_Vector * out, const BPU_T_GF2_Vector * in) {
    uint8_t md[BPU_HASH_LEN];

    if (out->len != BPU_HASH_LEN * 8) {
        BPU_printError("Wrong vector len %d", out->len);

        return -1;
    }
    // if input len is not divisible by 4, then it is not complete vector
    if (in->len % 8) {
//              BPU_printError("Wrong input vector len %d, should be divisible by 8", in->len);
        BPU_printWarning("input vector len %d, should be divisible by 8",
                         in->len);
    }
    // hash vector
    sha512((uint8_t *) in->elements, in->len / 8, md, 0);
    // copy digest to vector
    memcpy(out->elements, md, BPU_HASH_LEN);

    return 0;
}
#endif
